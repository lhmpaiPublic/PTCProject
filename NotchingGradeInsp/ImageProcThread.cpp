#include "pch.h"
#include "ImageProcThread.h"
#include "ImageProcessCtrl.h"
#include "Bitmapstd.h"
#include "QueueCtrl.h"
#include "FrameInfo.h"
#include "FrameRsltInfo.h"
#include "Win32File.h"
#include "TimeAnalyzer.h"
#include "CImageProcess.h"
#include "GlobalData.h"
#include "GlobalDef.h"
#include "CRecipeCtrl.h"
#include "CCounterQueueCtrl.h" 
#include "SigProc.h"
#include "NotchingGradeInsp.h" // 22.04.01 Ahn Add 

#include "CThreadQueueCtrl.h"
#include "CImageProcThreadUnit.h"
#include "AppDIO.h"

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
//SPC+ INSP 객체 생성을 위한 클래스
#include "SpcPlusManager.h"
#include "SpcInspManager.h"
#include "SpcAlarmManager.h"
#include "SpcCreateJSONFileThread.h"
#include "SpcPlus.h"
#endif //SPCPLUS_CREATE

// 22.05.31 Ahn Add Start
#include "CImageSaveQueueCtrl.h"
// 22.05.31 Ahn Add End
// 22.12.09 Ahn Add Start
#include "CTacTimeDataCtrl.h"
// 22.12.09 Ahn Add End

//using namespace HalconCpp;

CImageProcThread::CImageProcThread(CImageProcessCtrl *pParent)
{
	m_pParent = pParent ;
	m_pThread = NULL ;
	m_CreateMode = 0;
}

CImageProcThread::~CImageProcThread(void)
{
}

void CImageProcThread::Begin( int nMode ) // nMode  0 : Image Merge Mode , 1 : Image Proc Mode 
{
	m_bKill = FALSE ;
	m_CreateMode = nMode;
	//이벤트 객체 생성
	pEvent_ImageProcThread_TabFind = CreateEvent(NULL, TRUE, FALSE, NULL);
	pEvent_ImageProcThread_Result = CreateEvent(NULL, FALSE, FALSE, NULL);

//	m_DisphWnd = NULL ;
	if ( m_pThread == NULL ) {
		if (nMode == 0) {
			m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadImgCuttingTab,
				(LPVOID)this,
				THREAD_PRIORITY_HIGHEST,
				0,
				CREATE_SUSPENDED,
				NULL);
			if (m_pThread != NULL) {
				m_pThread->m_bAutoDelete = FALSE;
				m_pThread->ResumeThread();
			}
		}
		else {
			m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadImgProc,
				(LPVOID)this);
		}
		
	}
}

void CImageProcThread::CreateDioMarkingThread(WORD* nOutPutData)
{
	CWinThread* pDioMarkingThread = AfxBeginThread((AFX_THREADPROC)DioMarkingThreadProc,
		(LPVOID)nOutPutData,
		THREAD_PRIORITY_HIGHEST,
		0,
		CREATE_SUSPENDED,
		NULL);
	if (pDioMarkingThread != NULL) {
		pDioMarkingThread->m_bAutoDelete = TRUE;
		pDioMarkingThread->ResumeThread();
	}
}


void CImageProcThread::Kill( void ) 
{
	// source file
	if (m_pThread)
	{
		m_bKill = TRUE;
		if (m_CreateMode == 0)
		{
			setEvent_ImageProcThread_TabFind();
		}
		else
		{
			setEvent_ImageProcThread_Result();
		}
		CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
		m_pThread->m_hThread = NULL;
	}

}

//Dio Marking Thread Proc 함수
UINT CImageProcThread::DioMarkingThreadProc(LPVOID pParam)
{
	WORD nOutPutData = *(WORD*)pParam;
	delete pParam;

	CAppDIO dio;

	AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <Ink Marking> Call Enter"));

	dio.OutputWord(nOutPutData);

	AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <Ink Marking> :: Output ID<%d>, Value<%d>"),
		(nOutPutData>>2), nOutPutData);

	Sleep(20);
	dio.OutputBit(CAppDIO::eOut_PULSE, TRUE);

	return 0;
}


// Queue에서 받아온 Frame Image를 Tab 으로 구분해서 처리용 Queue로 저장 하는 Thread
#define IMAGEPROCTHREAD_TABFIND_TIMEOUT 40
UINT CImageProcThread::CtrlThreadImgCuttingTab(LPVOID Param)
{
	CImageProcThread* pThis = (CImageProcThread*)Param;
	CQueueCtrl* pQueueFrame_Top = pThis->m_pParent->GetQueueFrmPtr(0);
	CQueueCtrl* pQueueFrame_Bottom = pThis->m_pParent->GetQueueFrmPtr(1);
	CCounterQueueCtrl* pCntQueueInCtrl = pThis->m_pParent->GetCounterQueInPtr(); // 탭 카운터 용 큐
	//CThreadQueueCtrl* pThreadQue[MAX_CAMERA_NO];
	//pThreadQue[CAM_POS_TOP] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_TOP);
	//pThreadQue[CAM_POS_BOTTOM] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_BOTTOM);

	BOOL bReserved = FALSE; // 크기가 작아서 보내지 못한 부분이 있음 다음 이미지 받아서 처리 할 것인지에 대한 Flag.
	BOOL bReservFrmNo = -1;
	int nLastLengh = 0;
	int nLastWidth = 0;
	CTabInfo reservTabInfo;
	CString strMsg;
	CString strTemp;
	CTabInfo RsvTabInfo;

	int nFindPos = 0;
	BOOL bHeadFlag = FALSE;
	BOOL bTailFlag = FALSE;
	BOOL bStopFlag = FALSE;

	// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
	BYTE* btLastBtmImg = NULL;
#endif
	// 22.02.22 Ahn Add End

	// 22.04.06 Ahn Add Start - 첫 탭 버림
	BOOL bFirstTab = TRUE;
	// 22.04.06 Ahn Add End

	//CCounterInfo 에 값이 없을때 사용할 값
	int useTabID = 64;
	//CCounterInfo 에 값이 없을때 미리 사용한 값 저장
	std::queue<int> quUserTabID;

	UINT ret = 0;
	//스래드 대기 여부
	BOOL bThreadWait = TRUE;
	while (1)
	{
		//타임 주기 이벤트
		//대기 상태일 때
		if (bThreadWait)
		{
			ret = WaitForSingleObject(pThis->getEvent_ImageProcThread_TabFind(), INFINITE);
		}
		//대기하지 않고 바로 처리한다.
		else
		{
			ret = WAIT_OBJECT_0;
		}

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return 0;
		}
		else if (ret == WAIT_OBJECT_0) //TIMEOUT시 명령
		{
			if (pThis == NULL)
			{
				break;
			}
			if (pThis->m_bKill == TRUE)
			{
				if ((pQueueFrame_Top != NULL) && (pQueueFrame_Bottom != NULL))
				{
					if ((pQueueFrame_Top->GetSize() <= 0) || (pQueueFrame_Bottom->GetSize() <= 0))
					{
						break;
					}
				}
			}

			//프레임 크기
			//Top Frame 크기
			int nSizeFrmL = pQueueFrame_Top->GetSize();
			//Bottom Frame 크기
			int nSizeFrmR = pQueueFrame_Bottom->GetSize();

			//프레임 정보가 Top1개 이상, Bottom 1개 이상
			bHeadFlag = !pQueueFrame_Top->IsEmpty();
			bTailFlag = !pQueueFrame_Bottom->IsEmpty();

			//Top과 Bottom Frame 크기의 차가 FRAME_ACQ_ERROR_CHK_CNT 이상이면 에러 출력한다.
			if (abs(nSizeFrmL - nSizeFrmR) > FRAME_ACQ_ERROR_CHK_CNT)
			{

				// 에러 처리 
				CString strErrMsg;
				strErrMsg.Format(_T("Frame Size Error : Top [%d], Bottom[%d], Inspect Status [%d], Process End!!!!"), nSizeFrmL, nSizeFrmR, pThis->m_pParent->IsInspection());
				AprData.SaveErrorLog(strErrMsg);

				//카메라의 에러를 세팅한다.
				AprData.m_ErrStatus.SetError(CErrorStatus::en_CameraError, strErrMsg);
			}

		//Top과 Bottom  정보가 1개씩 이상이 되어야 한다.
			if (bHeadFlag && bTailFlag)
			{
				//TabFind TacTime
				LARGE_INTEGER tmp_TabFind;
				LARGE_INTEGER start_TabFind;
				QueryPerformanceFrequency(&tmp_TabFind);
				double dFrequency_TabFind = (double)tmp_TabFind.LowPart + ((double)tmp_TabFind.HighPart * (double)0xffffffff);
				QueryPerformanceCounter(&start_TabFind);

				//Dalsa Camera Callback 함수에서 넣은 이미지 데이터가 저장된 Top 객체를 가져온다.
				CFrameInfo* pFrmInfo_Top = pQueueFrame_Top->Pop();

				//Top 이미지의 크기 값
				int nHeight = pFrmInfo_Top->m_nHeight;
				int nFrmWIdth = pFrmInfo_Top->m_nWidth;
				int nWidth = nFrmWIdth;


				//Dalsa Camera Callback 함수에서 넣은 이미지 데이터가 저장된 Bottom 객체를 가져온다.
				CFrameInfo* pFrmInfo_Bottom = pQueueFrame_Bottom->Pop();

				//Top Frame 번호
				int nFrameCountL = pFrmInfo_Top->m_nFrameCount;
				//Bottom Frame 번호
				int nFrameCountR = pFrmInfo_Bottom->m_nFrameCount;

				AprData.m_NowLotData.m_nFrameCount = pFrmInfo_Top->m_nFrameCount;

				BYTE* pHeadPtr = pFrmInfo_Top->GetImagePtr();
				BYTE* pTailPtr = pFrmInfo_Bottom->GetImagePtr();


				//처리시간 체크 객체 생성 및 시간 진행
				CTimeAnalyzer ctAna;
				ctAna.Clear();
				ctAna.StopWatchStart();



				DWORD dwTic = 0;

				// Tab으로 잘라 보냄
				// Projection 사용 
				{
					int nBndElectrode = 0;
					int nBneElectrodeBtm = 0;


					//양극일 경우 Top 프로젝션 데이터의 바운드리 위치 크기를 가져온다.
					nBndElectrode = CImageProcess::GetBoundaryOfElectorde(pHeadPtr, nWidth, nHeight, AprData.m_pRecipeInfo, CImageProcess::en_FindFromLeft);

					// 22.05.09 Ahn Add End
					//Tab 정보를 저장할 vector 임시 객체
					CImageProcess::_VEC_TAB_INFO vecTabInfo;
					int nLevel = 0;
					int nBtmLevel = 0;

					//Tab 위치 : 양극일 경우 nBndElectrode 값에 레시피 Tab Condition 카메라 높이
					int nTabFindPos = nBndElectrode + AprData.m_pRecipeInfo->TabCond.nCeramicHeight;

					LOGDISPLAY_SPEC(5)("<<Proc>> Tab_Find_Pos<%d>-Head_Image_Boundary_Electorde_Level<%d>+Recipe_CameraHeight<%d> ",
						nTabFindPos
						, nBndElectrode
						, AprData.m_pRecipeInfo->TabCond.nCeramicHeight);

					//이미지 프로세싱을 위한 클래스 
					//이미지 Tab 정보에서 Tab을 그룹으로 나누기
					//PET Check TOP
					int nLocalRet = CImageProcess::DivisionTab_FromImageToTabInfo(pHeadPtr, pTailPtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo, nFrameCountL);


					//Tab 정보 크기, Tab 정보가 없다면 에러처리
					int nVecSize = (int)vecTabInfo.size();

					BOOL bErrorAll = FALSE;
					if (nVecSize <= 0)
					{
						// 강제 분할 
						bErrorAll = TRUE;
					}
					nBneElectrodeBtm = CImageProcess::GetBoundaryOfElectordeBottom(pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo);


					// PET Check BOTTOM
					CImageProcess::VEC_PET_INFO* pvstPetInfoBtm = new CImageProcess::VEC_PET_INFO;
					pvstPetInfoBtm->clear();

					BOOL bIsPET_Btm = CImageProcess::FindPetFilm(pTailPtr, nWidth, nHeight, *AprData.m_pRecipeInfo, pvstPetInfoBtm, CAM_POS_BOTTOM);
					pFrmInfo_Bottom->m_bIsPET = bIsPET_Btm;





					//Top Bottom 프로젝션 바운드리 처리 시간 체크 및 로그 출력
					double dTime = ctAna.WhatTimeIsIt_Double();
					CString strLog;
					strLog.Format(_T("TabCutting Time [%.2lf]msec, 전극경계Top[%d], Bottom[%d] BtmLevel[%d]"), dTime, nBndElectrode, nBneElectrodeBtm, nBtmLevel);
					AprData.SaveTactLog(strLog);

					//Image Cutting Tab 정보 출력 로그
					LOGDISPLAY_SPEC(1)("*2*1*Now Tab Find Count<%d>, TabID QueueCount<%d>",
						nVecSize, pCntQueueInCtrl->GetSize());

					//Tab 정보 크기 만큼 루프 돌다.
					for (int i = 0; i < nVecSize; i++)
					{

						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
					//SPC+ INSP===================================================================================================
					//SPC+ INSP 객체 생성
						CSpcInspManager* insp = new CSpcInspManager();

						//============================================================================================================
#endif //SPCPLUS_CREATE

					//컨테이너 정보 : 검사기 Tab 번호, Tab ID 받을 임시 객체
						CCounterInfo cntInfo;

						//Trigger input id 사용 예외처리
						//queue에서 정보가 없을 때 들어올 값을 미리 사용한다.
						//Tab Id 정보가 있을 경우
						bool bNextTabId = false;
						while (pCntQueueInCtrl->GetSize())
						{
							//정보를 하나 가지고 온다.
							cntInfo = pCntQueueInCtrl->Pop();
							//미리 땡겨 쓴 Tab Id가 있다면
							if (quUserTabID.size())
							{
								//미리 땡겨 쓴 id를 가져온다.
								int beforeTabId = quUserTabID.front();
								//정보 삭제
								quUserTabID.pop();
								//비교해서 다르다면 빠져나간다.
								if (beforeTabId != cntInfo.nTabID)
								{
									//사용할 Tab Id를 찾았다
									bNextTabId = true;

									//다음에 사용할 id : 1 증가 시켜 저장
									useTabID = cntInfo.nTabID + 1;
									//Tab id는 0 ~ 63 까지 사용한다.
									if (useTabID >= 64)
									{
										useTabID = 0;
									}
									break;
								}
								else
								{
									//다음에 사용할 id를 못찾았다.
									bNextTabId = false;
								}

								//미리 사용한 Tab ID 정보 삭제
								while (quUserTabID.size())
									quUserTabID.pop();
							}
							//땡겨 쓴 id가 없다.
							else
							{
								//사용할 Tab Id를 찾았다
								bNextTabId = true;

								//다음에 사용할 id : 1 증가 시켜 저장
								useTabID = cntInfo.nTabID + 1;
								//Tab id는 0 ~ 63 까지 사용한다.
								if (useTabID >= 64)
								{
									useTabID = 0;
								}
								break;
							}
						}

						//Tab id 정보가 없을 경우
						//다음에 들어올 id를 할당한다.
						if (bNextTabId == false)
						{
							//2개 이상 TabID가 안들어왔을 때는 ID를 64를 준다.
							if (quUserTabID.size() > 5)
							{
								//다음 아이디를 할당한다.
								cntInfo.nTabID = 64;
							}
							else
							{
								//다음 아이디를 할당한다.
								cntInfo.nTabID = useTabID;
								//다음에 사용할 id : 1 증가 시켜 저장
								useTabID++;
								//Tab id는 0 ~ 63 까지 사용한다.
								if (useTabID >= 64)
								{
									useTabID = 0;
								}
							}
							//사용한 아이디를 backup 한다. 확인용
							quUserTabID.push(cntInfo.nTabID);

						}

						//Tab  정보 접근 임시 포인터 변수
						CTabInfo* pTabInfo = &vecTabInfo[i];

						//Tab 정보에서 Left 크기, Right 크기
						int nLeft = pTabInfo->nTabLeft - pTabInfo->nLeft;
						int nRight = pTabInfo->nRight - pTabInfo->nTabRight;

						// 22.05.03 Ahn Modify Start
						int nErrorNo = 0;
						//if ((nLeft < (AprData.m_pRecipeInfo->TabCond.nRadiusW * 2)) || (nRight < (AprData.m_pRecipeInfo->TabCond.nRadiusW * 2))) {
						//레시피 텝컨디션 영역(Tab 라운드 가로 방향 Pixel수 ) 검사
						if ((nLeft < AprData.m_pRecipeInfo->TabCond.nRadiusW) || (nRight < AprData.m_pRecipeInfo->TabCond.nRadiusW))
						{
							// 22.05.03 Ahn Modify End
							pTabInfo->m_bErrorFlag = TRUE;

							CString strError = "NONE";
							if (nLeft < AprData.m_pRecipeInfo->TabCond.nRadiusW)
							{
								strError = "LEFT-RecipeInfo의 Tab-Radius 보다 작은 Error";
							}
							else if (nRight < AprData.m_pRecipeInfo->TabCond.nRadiusW)
							{
								strError = "RIGHT-RecipeInfo의 Tab-Radius 보다 작은 Error";
							}
							LOGDISPLAY_SPEC(5)("<<Proc>> CtrlThreadImgCuttingTab-Case-RadiusW-<%s> : ERRORFLAG 1",
								strError);

							nErrorNo = 1;

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: Tab 반지름 Error(nLeft:%d<%d or nRight:%d<%d)"),
								pTabInfo->m_bErrorFlag, nErrorNo, nLeft, AprData.m_pRecipeInfo->TabCond.nRadiusW, nRight, AprData.m_pRecipeInfo->TabCond.nRadiusW);

						}

						//에레 체크 : 
						if ((AprData.m_NowLotData.m_bProcError == TRUE) && (AprData.m_System.m_bFirstTabDoNotProc == TRUE))
						{
							pTabInfo->m_bErrorFlag = TRUE;

							CString strError = "NONE";
							if (AprData.m_NowLotData.m_bProcError == TRUE)
							{
								strError = "PROCERROR";
							}
							else if (AprData.m_System.m_bFirstTabDoNotProc == TRUE)
							{
								strError = "FIRST_TabDonotProc Error";
							}
							LOGDISPLAY_SPEC(5)("<<Proc>> CtrlThreadImgCuttingTab-Case<%s> : ERRORFLAG 2",
								strError);

							AprData.m_NowLotData.m_bProcError = FALSE;
							nErrorNo = 2;

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: Inital 상태 or 첫 Tab"),
								pTabInfo->m_bErrorFlag, nErrorNo);

						}

						// 21.12.28 Ahn Add Start
						//Tab 정보 크기, Tab 정보가 없을 때
						if (bErrorAll == TRUE)
						{
							pTabInfo->m_bErrorFlag = TRUE;

							LOGDISPLAY_SPEC(5)("<<Proc>> CtrlThreadImgCuttingTab-Case<ErrorAll> : ERRORFLAG 3");

							nErrorNo = 3;

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: Tab 정보 없음"),
								pTabInfo->m_bErrorFlag, nErrorNo);

						}

						// 21.12.28 Ahn Add End
						// 22.06.22 Ahn Add Start

						//Tab부의 흑연 코팅높이 에러
						if (nLevel <= 0)
						{
							pTabInfo->m_bErrorFlag = TRUE;

							LOGDISPLAY_SPEC(5)("<<Proc>> CtrlThreadImgCuttingTab-Case<Level:%d> : ERRORFLAG 4", nLevel);

							nErrorNo = 4;

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: nLevel Error (nLevel:%d <= 0)"),
								pTabInfo->m_bErrorFlag, nErrorNo, nLevel);

						}
						// 22.06.22 Ahn Add End

						// 22.09.30 Ahn Add Start

						//Tab부의 흑연 코팅높이 에러
						if (nLevel >= (nWidth-1) )
						{
							pTabInfo->m_bErrorFlag = TRUE;
							LOGDISPLAY_SPEC(5)("<<Proc>> CtrlThreadImgCuttingTab-Case<Level:%d >= nWidth - 100 : %d> : ERRORFLAG 5", nLevel, nWidth - 100);

							nErrorNo = 5;


							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: nLevel Error (nLevel:%d >= (nWidth:%d - 100 ))"),
								pTabInfo->m_bErrorFlag, nErrorNo, nLevel, nWidth);

						}
						// 22.09.30 Ahn Add End

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> nVecSize:%d/%d, ImageLength:%d, FrameCount:%d, TabStartPosInFrame:%d, TabLeft:%d, TabRight:%d, nLevel:%d"),
							i, nVecSize, pTabInfo->nImageLength, pTabInfo->nFrameCount, pTabInfo->nTabStartPosInFrame, pTabInfo->nTabLeft, pTabInfo->nTabRight, nLevel);


						//프레임 정보 임시 객체(Top 프레임 정보 처리)
						CFrameInfo* pInfo;
						pInfo = new CFrameInfo;

						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
					//SPc+ 객체를 Top에 추가한다.
					//Tab 있는 이미지 Frame  정보
						pInfo->m_SpcInspMgr = insp;
#endif //SPCPLUS_CREATE

						//Tab정보에서 Top 이미지 데이터 세팅
						pInfo->SetImgPtr(pTabInfo->pImgPtr);

						//이미지 데이터 높이
						pInfo->m_nHeight = pTabInfo->nImageLength;
						//Top 번호
						pInfo->m_nHeadNo = pFrmInfo_Top->m_nHeadNo;
						//데이터 넓이
						pInfo->m_nWidth = nWidth;

						// 22.11.18 Ahn Modify Start
						//pBtmInfo->m_nFrameCount = nFrameCountL;
						//프레임 번호
						pInfo->m_nFrameCount = pTabInfo->nFrameCount;
						// 22.11.18 Ahn Modify End
						// Tab  번호
						pInfo->nTabNo = AprData.m_NowLotData.m_nTabCount;
						//Tab 시작위치 프레임
						pInfo->nTabStartPosInFrame = pTabInfo->nTabStartPosInFrame;
						pInfo->m_nTabLevel = nLevel;
						//검사 모드 : TopFrame
						pInfo->m_nInspMode = CFrameInfo::en_TopFrame;
						pInfo->m_nTabLeft = pTabInfo->nTabLeft;
						pInfo->m_nTabRight = pTabInfo->nTabRight;
						pInfo->m_nTabId_CntBoard = cntInfo.nTabID;

						pInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;
						pInfo->m_nBndElectrode = nBndElectrode;

						pInfo->m_bIsPET = (pTabInfo->m_bIsPET | pFrmInfo_Bottom->m_bIsPET);







						//프레임 정보 임시 객체(Bottom 프레임 정보 처리)
						CFrameInfo* pBtmInfo;
						pBtmInfo = new CFrameInfo;

						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
					//SPc+ 객체를 Bottom에 추가한다.
					//Tab 없는 이미지  Frame 정보
						pBtmInfo->m_SpcInspMgr = insp;
#endif //SPCPLUS_CREATE

						//Tab정보에서 Bottom 이미지 데이터 세팅
						pBtmInfo->SetImgPtr(pTabInfo->pImgBtmPtr);
						pBtmInfo->m_nHeight = pTabInfo->nImageLength;
						pBtmInfo->m_nHeadNo = pFrmInfo_Bottom->m_nHeadNo;
						pBtmInfo->m_nWidth = nWidth;

						// 22.11.18 Ahn Modify Start
						//pBtmInfo->m_nFrameCount = nFrameCountL;
						//Frame 번호
						pBtmInfo->m_nFrameCount = pTabInfo->nFrameCount;

						// 22.11.18 Ahn Modify End
						//Lot Data에서 Tab 번호
						pBtmInfo->nTabNo = AprData.m_NowLotData.m_nTabCount;
						//Tab  시작위치 프레임
						pBtmInfo->nTabStartPosInFrame = pTabInfo->nTabStartPosInFrame;
						pBtmInfo->m_nTabLevel = nBtmLevel;

						//검사모드  BottomFrame
						pBtmInfo->m_nInspMode = CFrameInfo::en_BottomFrame;
						pBtmInfo->m_nTabId_CntBoard = cntInfo.nTabID;

						pBtmInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;

						//Bottom 프로젝션 데이터의 바운드리 위치 크기를 가져온다.
						pBtmInfo->m_nBndElectrode = nBneElectrodeBtm;// 22.05.11 Ahn Add 

						pBtmInfo->m_bIsPET = (pTabInfo->m_bIsPET | pFrmInfo_Bottom->m_bIsPET);


						// 22.12.09 Ahn Add Start
						//프레임 처리 시간 세팅
						LARGE_INTEGER tmp;
						LARGE_INTEGER start;
						QueryPerformanceFrequency(&tmp);
						double dFrequency = (double)tmp.LowPart + ((double)tmp.HighPart * (double)0xffffffff);
						QueryPerformanceCounter(&start);

						//프레임 처리시간 객체를 세팅한다.
						pInfo->m_stTime = start;
						pInfo->m_dFrecuency = dFrequency;
						pBtmInfo->m_stTime = start;
						pBtmInfo->m_dFrecuency = dFrequency;
						// 22.12.09 Ahn Add End


						double TabFind_TacTime = CGlobalFunc::GetDiffTime(start_TabFind, dFrequency_TabFind);
						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(4)("*TacTime - TabFind : TabID-[%d], TabNo-[%d], TacTime[%f]",
							pInfo->m_nTabId_CntBoard,  pInfo->nTabNo, TabFind_TacTime
							);


						//스래드에 처리할 정보를 저장 TOP, BOTTOM
						CImageProcessCtrl::GetThreadQueuePtr(CAM_POS_TOP)->push(pInfo);
						CImageProcessCtrl::GetThreadQueuePtr(CAM_POS_BOTTOM)->push(pBtmInfo);

						//Lot Data Tab 번호를 증가 시킨다.
						AprData.m_NowLotData.m_nTabCount++;

						//메모리 로그 기록
						CString strMsg;
						strMsg.Format(_T("Find Image Tab TotalCount<%d>"), AprData.m_NowLotData.m_nTabCount);
						AprData.SaveMemoryLog(strMsg);

					}
					//처리한 Tab 정보를 삭제한다.
					vecTabInfo.clear();
				}

				delete pFrmInfo_Top;
				pFrmInfo_Top = NULL;
				delete pFrmInfo_Bottom;
				pFrmInfo_Bottom = NULL;

				double dSecond = ctAna.WhatTimeIsIt_Double();
				AprData.SetTactTime_1(dSecond);
			}
			//Top Frame 크기
			nSizeFrmL = pQueueFrame_Top->GetSize();
			//Bottom Frame 크기
			nSizeFrmR = pQueueFrame_Bottom->GetSize();

			//큐에 데이터가 있으면 기다리지 않고 실행하도록 설정
			if (nSizeFrmL && nSizeFrmR)
			{
				bThreadWait = FALSE;
			}
			//없으면 대기
			else
			{
				//Reset 호출
				pThis->resetEvent_ImageProcThread_TabFind();
				bThreadWait = TRUE;
			}
		}
		else
		{
			break;
		}
	}


	if (RsvTabInfo.pImgPtr != NULL) {
		delete[] RsvTabInfo.pImgPtr;
	}
	if (RsvTabInfo.pImgBtmPtr != NULL) {
		delete[] RsvTabInfo.pImgBtmPtr;
	}

	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	return 0;
}



#define IMAGEPROCTHREAD_RESULT_TIMEOUT 10
#define IMAGEPROCTHREAD_RESULTWAITE_TIMEOUT 5

#define DIOMARKINGEVENT_TIMEOUT 1000

UINT CImageProcThread::CtrlThreadImgProc(LPVOID Param)
{
	//스래드생성 시 넘긴 객체 포인터
	CImageProcThread* pThis = (CImageProcThread*)Param;

	//CImageSaveQueueCtrl에서 이미지 저장 객체<CImageSaveQueueCtrl> 포인터를 가져온다.
	CImageSaveQueueCtrl* pImgSaveQueueCtrl = pThis->m_pParent->GetImageSaveQueuePtr();

	//CImageSaveQueueCtrl에서 이미지 저장 객체<CTacTimeDataCtrl> 포인터를 가져온다.
	//CTacTimeDataCtrl* pTactCtrl = pThis->m_pParent->GetTactDataCtrlPtr();

	//결과 저장객체를 가져온다.
	//CImageProcessCtrl의 객체 멤버인 결과저장 객체<CQueueCtrl> 포인터를 가져온다.
	//CQueueCtrl* pRsltQueueCtrl[GRABBER_COUNT];
	//for (int i = 0; i < GRABBER_COUNT; i++) {
	//	pRsltQueueCtrl[i] = pThis->m_pParent->GetResultPtr(i);
	//}

	////CFrameRsltInfo* pFrmRsltInfo;
	//CThreadQueueCtrl* pThdQue[MAX_CAMERA_NO];
	//for (int i = 0; i < MAX_CAMERA_NO; i++) {
	//	pThdQue[i] = pThis->m_pParent->GetThreadQueuePtr(i);
	//}
	
	CImageProcThreadUnit* pUnitTop = NULL;
	CImageProcThreadUnit* pUnitBtm = NULL;
	BOOL bBitmapSave = FALSE;
	BOOL bJudgeNG = FALSE;
	char szJudge[2][4] = { "OK", "NG" };
	char szPos[2][8] = { "TAB","BTM" };

	BOOL bMarkingActive = FALSE;
	BOOL bClearFlag = FALSE;

	UINT ret = 0;

	while (1)
	{
		
		//타임 주기 이벤트
		ret = WaitForSingleObject(pThis->getEvent_ImageProcThread_Result(), IMAGEPROCTHREAD_RESULT_TIMEOUT);
		theApp.m_nImageProcThreadTimeEnter = GetTickCount();

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> Invalid Handle") );

			return 0;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{

			if (pThis == NULL) {
				AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> pThis == NULL"));
				break;
			}
			if (pThis->m_bKill == TRUE) {
				AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> pThis->m_bKill == TRUE"));
				break;
			}

			//Top/Botton에 Tab 정보가 저장된 정보가 있을 경우
			//위 함수 CtrlThreadImgCuttingTab Tab 정보를 찾는 스래드에서
			//GetThreadQueuePtr(i) Queue 에 push 되면 처리를 탄다.
			//처리를 하여 결과 정보 저장 Queue pRsltQueueCtrl 에 저장된다.
			if (pUnitTop == NULL)
			{
				CThreadQueueCtrl*  topPtr = CImageProcessCtrl::GetThreadQueuePtr(CAM_POS_TOP);
				if (topPtr)
				{
					pUnitTop = topPtr->deQueue();
				}
				else
				{
					LOGDISPLAY_SPEC(6)("TopQueueCtrl NULL");
				}
			}
			if (pUnitBtm == NULL)
			{
				CThreadQueueCtrl* btmPtr = CImageProcessCtrl::GetThreadQueuePtr(CAM_POS_BOTTOM);
				if (btmPtr)
				{
					pUnitBtm = btmPtr->deQueue();
				}
				else
				{
					LOGDISPLAY_SPEC(6)("BtmQueueCtrl NULL");
				}
			}

			//Top, Bottom 처리 조건 : Defect 검사 프로세스가 처리 되었을 때
			//일정한 시간이 지나도 처리하지 못했을 때
			if (pUnitTop && pUnitBtm)
			{
				theApp.m_nImageProcThreadTimeBefore = GetTickCount();

				////이미지 처리 스래드 (대기 스래드)
				////출력 대기 이벤트 객체 push
				//HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				////순서대로 push 저장한다.
				//pThis->m_pParent->ImgProcWaitThread_Event_push(hEvent);

				while (1)
				{

					if (pThis->m_bKill == TRUE) {
						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> ResultProc pThis->m_bKill == TRUE"), pUnitTop->m_pFrmInfo->nTabNo);
						break;
					}

					// 22.12.09 Ahn Add Start
					LARGE_INTEGER stTime;
					// 22.12.09 Ahn Add End

					int topWaitVal = pUnitTop->eventProcEnd_WaitTime();
					int btmWaitVal = pUnitBtm->eventProcEnd_WaitTime();

					if ((topWaitVal == 1) && (btmWaitVal == 1))
					{
						CFrameRsltInfo* pTopInfo = pUnitTop->GetResultPtr();
						CFrameRsltInfo* pBtmInfo = pUnitBtm->GetResultPtr();

						LOGDISPLAY_SPEC(6)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - ResultProcWait-Enter",
							"Top", pTopInfo->nTabNo, pTopInfo->m_nTabId_CntBoard
							);
						LOGDISPLAY_SPEC(6)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - ResultProcWait-Enter",
							"Btm", pBtmInfo->nTabNo, pBtmInfo->m_nTabId_CntBoard
							);

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> ResultProc Enter"), pTopInfo->nTabNo);


						//======TacTime 출력 ========================================================================
						pTopInfo->m_tacTimeList[2] = CGlobalFunc::GetDiffTime(pTopInfo->m_stTime, pTopInfo->m_dFrecuency);
						pBtmInfo->m_tacTimeList[2] = CGlobalFunc::GetDiffTime(pBtmInfo->m_stTime, pBtmInfo->m_dFrecuency);
						//=================================================================================================


						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
					//SPC+ INSP===================================================================================================
					//SPC+ 객체 포인터 받는다.(정보를 추가하기 위해)
					//Tab  있는 포인터 만 값을 세팅 하면 같이 변한다.
						CSpcInspManager* insp = dynamic_cast<CSpcInspManager*>(pTopInfo->m_SpcInspMgr);
						//InData ===
						//Insp InData 객체 포인터
						CSpcInspInData* InspInData = insp->getSpcInspInData();
						//Cell 추적 PLC에서 받은 ID (VIRTUAL_CELL_ID와 동일값 작성)	
						CString	CellId = "";
						CellId.Format(_T("%02d"), pTopInfo->m_nTabId_CntBoard);
						InspInData->setCellId(CellId);
						//셀 카운트 번호
						InspInData->setCellCountNo(CGlobalFunc::intToString(pTopInfo->nTabNo));
						//Cell 추적 PLC에서 받은 ID (CELL_ID와 동일값 작성)	
						CString	 VirtualCellId = CellId;
						InspInData->setVirtualCellId(VirtualCellId);
						//Cell 판정결과
						CString CellFinalJudge = ((pTopInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) || (pBtmInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG)) ? "NG" : "OK";
						InspInData->setCellFinalJudge(CellFinalJudge);
						//외관 판정 결과
						InspInData->setAppearanceJudgeResult(CellFinalJudge);
						//외관 NG 개수
						int TopJudge = (pTopInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) ? 1 : 0;
						int BottomJudge = (pBtmInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) ? 1 : 0;
						InspInData->setTotalAppearanceNgCount(CGlobalFunc::intToString(TopJudge + BottomJudge));

						//IqInfo ===Tab 보이는 카메라 1
						//Top 객체
						CSpcInDataIqInfo* IqInfoTop = insp->getSpcInDataIqInfo(CSpcInspManager::IQINFO_TOP);
						//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고
						//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
						IqInfoTop->setIqCameraNumber("1");
						//이미지 X Size [pxl]
						IqInfoTop->setIqScreenImageSizeX(CGlobalFunc::intToString(pTopInfo->m_nWidth));
						//이미지 Y Size [pxl]
						IqInfoTop->setIqScreenImageSizeY(CGlobalFunc::intToString(pTopInfo->m_nHeight));
						//판정결과
						CString IqTopJudge = (pTopInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) ? "NG" : "OK";
						IqInfoTop->setImageJudge(IqTopJudge);
						//SPC+ 저장할 이미지 명을 입력(Top 이미지 명)
						CString IqImageFileNameTop = IqInfoTop->ImagIqFileName();
						IqInfoTop->setImageFileName(IqImageFileNameTop);

						//IqInfo ===Tab 없는 카메라
						//Bottom 객체
						CSpcInDataIqInfo* IqInfoBottom = insp->getSpcInDataIqInfo(CSpcInspManager::IQINFO_BOTTOM);
						//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
						IqInfoBottom->setIqCameraNumber("2");
						//이미지 X Size [pxl]
						IqInfoBottom->setIqScreenImageSizeX(CGlobalFunc::intToString(pBtmInfo->m_nWidth));
						//이미지 Y Size [pxl]
						IqInfoBottom->setIqScreenImageSizeY(CGlobalFunc::intToString(pBtmInfo->m_nHeight));
						//판정결과
						CString IqBottomJudge = (pBtmInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) ? "NG" : "OK";
						IqInfoBottom->setImageJudge(IqBottomJudge);
						//SPC+ 저장할 이미지 명을 입력(Bottom 이미지 명)
						CString IqImageFileNameBottom = IqInfoBottom->ImagIqFileName();
						IqInfoBottom->setImageFileName(IqImageFileNameBottom);

#endif //SPCPLUS_CREATE

						int nBtmJudge = pBtmInfo->m_pTabRsltInfo->m_nJudge;
						int nTopJudge = pTopInfo->m_pTabRsltInfo->m_nJudge;

						// 22.12.09 Ahn Add Start 
						stTime = pTopInfo->m_stTime;
						// 22.12.09 Ahn Add End

						// NG Tab 보고
						if ((nTopJudge == JUDGE_NG) || (nBtmJudge == JUDGE_NG))
						{

							WORD wAlarmCode = 0x0000;
							bJudgeNG = TRUE;
							if (nTopJudge == JUDGE_NG)
							{
								wAlarmCode = pTopInfo->m_pTabRsltInfo->m_wNgReason;
								AprData.m_NowLotData.m_nTopNG++;
							}
							if (nBtmJudge == JUDGE_NG)
							{
								wAlarmCode |= pBtmInfo->m_pTabRsltInfo->m_wNgReason;
								AprData.m_NowLotData.m_nBottomNG++;
							}
							AprData.m_NowLotData.m_nTabCountNG++;

							// 22.08.09 Ahn Add Start
							AprData.m_NowLotData.m_nContinueCount++;

							CString strDbg;
							strDbg.Format(_T("연속 알람 Count: %d"), AprData.m_NowLotData.m_nContinueCount);
							AprData.SaveDebugLog(strDbg); //pyjtest



							CTabJudge tab;
							tab.nJudge = JUDGE_NG;
							tab.nReason = wAlarmCode;
							tab.nTabNo = pTopInfo->nTabNo;
							// 22.08.10 Ahn Modify Start
							//int nSecterNgCount = AprData.m_NowLotData.m_secNgJudge.AddNgTab(tab, AprData.m_pRecipeInfo->nSectorCount) ;
							//if ( (AprData.m_NowLotData.m_nContinueCount >= AprData.m_pRecipeInfo->nContinousNgCount) && (AprData.m_pRecipeInfo->nContinousNgCount > 2) ) {
							if ((AprData.m_NowLotData.m_nContinueCount >= AprData.m_nCoutinuouCount) && (AprData.m_nCoutinuouCount >= 2))
							{
								wAlarmCode |= CSigProc::en_Alarm_ContinueNg;

								CString strMessage;
								strMessage.Format(_T("Alarm : Continuous NG = Tab Count %d NG"), AprData.m_NowLotData.m_nContinueCount);
								AprData.m_ErrStatus.SetError(CErrorStatus::en_ContinuousNg, strMessage);
								AprData.SaveDebugLog(strMessage); //pyjtest

								//알람이 설정되었으면 초기화
								AprData.m_NowLotData.m_nContinueCount = 0;

								AprData.m_NowLotData.m_SeqDataOut.dwContinueAlarmCount++;
							}

							// 22.08.10 Ahn Modify End
							//if ((AprData.m_pRecipeInfo->nSectorCount > 0) && (AprData.m_pRecipeInfo->nAlarmCount > 0)) {
							//	if (nSecterNgCount >= AprData.m_pRecipeInfo->nAlarmCount) {
							int nSecterNgCount = AprData.m_NowLotData.m_secNgJudge.AddNgTab(tab, AprData.m_nSectorBaseCount);

							strDbg.Format(_T("구간 알람 Count: %d"), nSecterNgCount);
							AprData.SaveDebugLog(strDbg); //pyjtest


							if ((AprData.m_nSectorNgCount > 0) && (AprData.m_nSectorBaseCount > 0))
							{
								// 22.09.22 Ahn Modify Start
								if (nSecterNgCount >= AprData.m_nSectorNgCount)
								{
									wAlarmCode |= CSigProc::en_Alarm_SectorNg;

									CString strMessage;
									strMessage.Format(_T("Alarm : Section NG = NG Count :%d , Setting Count : %d, Setting Distance : %d"), nSecterNgCount, AprData.m_nSectorNgCount, AprData.m_nSectorBaseCount);
									AprData.m_ErrStatus.SetError(CErrorStatus::en_ContinuousNg, strMessage);
									AprData.SaveDebugLog(strMessage); //pyjtest

									//알람이 설정되었으면 초기화
									AprData.m_NowLotData.m_secNgJudge.ResetAll();

									AprData.m_NowLotData.m_SeqDataOut.dwSectorAlarmCount++;
								}
							}
							// 22.08.09 Ahn Add End

							if (AprData.m_System.m_bEnableNgStop == TRUE)
							{
								if (AprData.m_pRecipeInfo->bNgStop == TRUE)
								{
									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_FoilExpIn_Top)
									{
										wAlarmCode |= CSigProc::en_Alarm_FoilExpIn_Top;
									}
									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_FoilExpOut_Top)
									{
										wAlarmCode |= CSigProc::en_Alarm_FoilExpOut_Top;
									}
									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_FoilExpBoth_Top)
									{
										wAlarmCode |= CSigProc::en_Alarm_FoilExpBoth_Top;
									}
									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_Surface_Top)
									{
										wAlarmCode |= CSigProc::en_Alarm_Spatter_Top;
									}

									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_FoilExpIn_Btm)
									{
										wAlarmCode |= CSigProc::en_Alarm_FoilExpIn_Btm;
									}
									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_FoilExpOut_Btm)
									{
										wAlarmCode |= CSigProc::en_Alarm_FoilExpOut_Btm;
									}
									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_FoilExpBoth_Btm)
									{
										wAlarmCode |= CSigProc::en_Alarm_FoilExpBoth_Btm;
									}
									if (pTopInfo->m_pTabRsltInfo->m_wNgReason & CTabRsltBase::en_Reason_Surface_Btm)
									{
										wAlarmCode |= CSigProc::en_Alarm_Spatter_Btm;
									}

									//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
								//SPC+ ALARM===================================================================================================
								//SPC+ ALARM 객체 생성
									CSpcAlarmManager* alarm = new CSpcAlarmManager();
									CSpcAlarmInData* alarmInData = alarm->getSpcAlarmInData();
									//알람 발생 or 해제 MSG Flag : TRUE -> 알람발생
									alarmInData->setAlarmFlag("TRUE");
									alarmInData->setAlarmCode(CGlobalFunc::intToString(wAlarmCode));

									//SPC+ 파일 생성을 위한 스래드에 추가한다.
									CSpcCreateJSONFileThread::AddSpcPlusManager(alarm);
									//============================================================================================================
#endif //SPCPLUS_CREATE

									bClearFlag = TRUE;; // 22.03.03 Ahn Add 
									CSigProc* pSigProc = theApp.m_pSigProc;

									if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
									{
										int nId = pTopInfo->m_nTabId_CntBoard;
										int nJudge = (nTopJudge == JUDGE_NG || nBtmJudge == JUDGE_NG) ? 2 : 1; // 2:NG / 1:OK
										int nNgCode = 1; // 임시, 정의되지 않음

										pSigProc->WriteAlarmCodeAndJudge(wAlarmCode, nId, nJudge, nNgCode);
									}
									else
									{
										pSigProc->SigOutAlarmExist(TRUE);
										pSigProc->WriteAlarmCode(wAlarmCode);
									}

									CString strLog;
									strLog.Format(_T("!!!! NG_STOP Signal Output [0x%x]!!!!"), wAlarmCode);
									AprData.SaveMemoryLog(strLog);

									//								int nId, nJudge, nCode ;
									//								nId = pTopInfo->m_nTabId_CntBoard ;


									//								nJudge = tab.nJudge ;
									//								nCode = wAlarmCode ;
									//								pSigProc->ReportJudge(nId, nJudge, nCode);
																	// 22.12.12 Ahn Add End
								}
							}
						}
						else
						{

							AprData.m_NowLotData.m_nTabCountOK++;
							AprData.m_NowLotData.m_nContinueCount = 0; // 22.08.09 Ahn Add
							AprData.m_NowLotData.m_secNgJudge.AddOkTab(pTopInfo->nTabNo, AprData.m_nSectorBaseCount/*AprData.m_pRecipeInfo->nSectorCount*/);

//							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> Result OK :: OK Tab Count <%d>"), AprData.m_NowLotData.m_nTabCountOK);

						}
						// 결과 Queue에 보냄

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Write NG Code To PLC"), pTopInfo->nTabNo);



						// Counter 신호 출력
						//WaitForSingleObject(hEvent, DIOMARKINGEVENT_TIMEOUT);
						WORD wOutPut;
						CString strMarking = _T("OFF");
						{
							
							int nMarkSel1 = 0;
							int nMarkSel2 = 0;
//							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <Marking Event> Enter"));

							// 22.07.19 Ahn Modify Start
							GetMarkingFlag(AprData.m_pRecipeInfo, nTopJudge, nBtmJudge, pTopInfo->m_pTabRsltInfo->m_wNgReason, pBtmInfo->m_pTabRsltInfo->m_wNgReason, nMarkSel1, nMarkSel2);
							// 22.07.19 Ahn Modify End

							CSigProc* pSigProc = theApp.m_pSigProc;
							bMarkingActive = pSigProc->GetInkMarkActive();
							//						bMarkingActive = TRUE; // PLC 강제 마킹 처리

							if ((AprData.m_System.m_bChkEnableMarker == FALSE) || (bMarkingActive == FALSE))
							{
								nMarkSel1 = 0;
								nMarkSel2 = 0;
							}

							wOutPut = CImageProcThread::GetCounterSignal(pTopInfo->m_nTabId_CntBoard, nTopJudge, nBtmJudge, nMarkSel1, nMarkSel2);

							CAppDIO dio;

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <Ink Marking> Call Enter"));

							dio.OutputWord(wOutPut);
							Sleep(5);
							dio.OutputBit(CAppDIO::eOut_PULSE, TRUE);

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <Ink Marking> :: Output ID<%d>, Value<%d>"),
								(wOutPut >> 2), wOutPut);

							Sleep(10);

							CString strMsg;
							strMsg.Format(_T("Output ID[%d]_OutPutValue[0x%x]_TabNo[%d] : VISION Marking[%s], PLC Marking[%s]"),
								pTopInfo->m_nTabId_CntBoard, wOutPut, pTopInfo->nTabNo + 1,
								(AprData.m_System.m_bChkEnableMarker == FALSE) ? _T("SKIP") : _T("USE"),
								(bMarkingActive == FALSE) ? _T("SKIP") : _T("USE"));
							AprData.SaveMemoryLog(strMsg);


							if ((wOutPut & CAppDIO::eOut_MARK_SEL_01) || (wOutPut & CAppDIO::eOut_MARK_SEL_02))
							{
								strMarking = _T("ON");
								pTopInfo->m_pTabRsltInfo->m_bMarkingFlag = TRUE;
								pBtmInfo->m_pTabRsltInfo->m_bMarkingFlag = TRUE;
								AprData.m_NowLotData.m_nMarkingCount++; // 22.06.29 Ahn Add 
							}
							else
							{
								strMarking = _T("OFF");
								pTopInfo->m_pTabRsltInfo->m_bMarkingFlag = FALSE;
								pBtmInfo->m_pTabRsltInfo->m_bMarkingFlag = FALSE;
							}


							if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
							{
								//							AprData.m_NowLotData.m_stCellJudgeSms.wCellTriggerID = pTopInfo->m_nTabId_CntBoard;
								//							AprData.m_NowLotData.m_stCellJudgeSms.wCellJudge = (nTopJudge == JUDGE_NG || nBtmJudge == JUDGE_NG) ? 2 : 1; // 2:NG / 1:OK
								//							AprData.m_NowLotData.m_stCellJudgeSms.wCellNgCode = 1; // 임시, 정의되지 않음
								//
								//							int nAddress = CSigProc::GetWordAddress(CSigProc::en_WordWrite_Cell_Trigger_ID, MODE_WRITE);
								//							short* pData = (short*)(&AprData.m_NowLotData.m_stCellJudgeSms);
								//							int nSize = sizeof(_CELL_JUDGE_SMS) / sizeof(WORD);
								//							pSigProc->WritePLC_Block_device(nAddress, pData, nSize);
								//
								//							CString strMsg;
								//							strMsg.Format(_T("Cell ID:%d, Judge:%d, Code:%d"), AprData.m_NowLotData.m_stCellJudgeSms.wCellTriggerID, AprData.m_NowLotData.m_stCellJudgeSms.wCellJudge, AprData.m_NowLotData.m_stCellJudgeSms.wCellNgCode);
								//							AprData.SaveDebugLog(strMsg); //pyjtest

							}
							else
							{
								AprData.m_NowLotData.m_stCellJudge.dwCellTriggerID = pTopInfo->m_nTabId_CntBoard;
								AprData.m_NowLotData.m_stCellJudge.dwCellJudge = (nTopJudge == JUDGE_NG || nBtmJudge == JUDGE_NG) ? 2 : 1; // 2:NG / 1:OK
								AprData.m_NowLotData.m_stCellJudge.dwCellNgCode = 1; // 임시, 정의되지 않음

								int nAddress = CSigProc::GetWordAddress(CSigProc::en_WordWrite_Cell_Trigger_ID, MODE_WRITE);
								int* pData = (int*)(&AprData.m_NowLotData.m_stCellJudge);
								int nSize = sizeof(_CELL_JUDGE) / sizeof(int);
								pSigProc->WritePLC_Block_device(nAddress, pData, nSize);

//								CString strMsg;
//								strMsg.Format(_T("Cell ID:%d, Judge:%d, Code:%d"), AprData.m_NowLotData.m_stCellJudge.dwCellTriggerID, AprData.m_NowLotData.m_stCellJudge.dwCellJudge, AprData.m_NowLotData.m_stCellJudge.dwCellNgCode);
//								AprData.SaveDebugLog(strMsg); //pyjtest

							}


						}

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Write Judge To PLC"), pTopInfo->nTabNo);



						{ // CSV 파일 작성
							CString strCsvFileName;
							CString strFilePath;
							strFilePath.Format(_T("%s\\"), AprData.m_strNowCsvPath);

							strCsvFileName.Format(_T("%s.csv"), AprData.m_NowLotData.m_strLotNo);
							CString strResult;
							SYSTEMTIME* pSysTime;
							pSysTime = &(pTopInfo->m_pTabRsltInfo->sysTime);

							int nSurfaceNgCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][RANK_NG] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][JUDGE_NG];
							int nFoilExpNgCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][RANK_NG] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][JUDGE_NG]
								+ pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][RANK_NG] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][JUDGE_NG];
							double 	dTopMaxSize = pTopInfo->m_pTabRsltInfo->m_dMaxSizeDef;
							double 	dBtmMaxSize = pBtmInfo->m_pTabRsltInfo->m_dMaxSizeDef;

							int nSurfaceGrayCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][JUDGE_GRAY] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][JUDGE_GRAY];
							int nFoilExpGrayCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][JUDGE_GRAY] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][JUDGE_GRAY]
								+ pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][JUDGE_GRAY] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][JUDGE_GRAY];

							CString strMarking;
							CString strMarkReason = _T("");
							if (pBtmInfo->m_pTabRsltInfo->m_bMarkingFlag || pTopInfo->m_pTabRsltInfo->m_bMarkingFlag) {
								strMarking.Format(_T("○"));
							}
							else {
								strMarking.Format(_T("Χ"));
							}
							CString strTime;
							CString strJudge = _T("OK");
							CString strBtmJudge = _T("OK");
							CString strTopJudge = _T("OK");
							strTime.Format(_T("%02d:%02d:%02d(%03d)"), pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond, pSysTime->wMilliseconds);
							if (pTopInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) {
								strJudge = _T("NG");
								strTopJudge = _T("NG");
							}
							if (pBtmInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) {
								strJudge = _T("NG");
								strBtmJudge = _T("NG");
							}
							// LotID, /*Total Count*/, Cell_No, Time, Pos(Top,Bottom),Top Judge,Bottom Judge,Surface Exposure, DefR, DefTheta, Top DefectSize, BTM Def Size
							//,  Top Surface max Size, Btm Surface Max Size, InkMarking, InkMarkingReason
							// 23.01.06 Ahn Modify Start
							//strResult.Format(_T("%s,%d,%s,%s,%s,%s,%d,%d,%.2lf,%.2lf,%s,%s\r\n")
							strResult.Format(_T("%s,%d,%s,%s,%s,%s,%d,%d,%.2lf,%.2lf,%s,%s,%d,%d\r\n")
								// 23.01.06 Ahn Modify End
								, AprData.m_NowLotData.m_strLotNo
								, pTopInfo->nTabNo + 1
								, strTime
								, strJudge
								, strTopJudge
								, strBtmJudge
								, nSurfaceNgCnt
								, nFoilExpNgCnt
								, dTopMaxSize
								, dBtmMaxSize
								, strMarking
								, strMarkReason
								// 23.01.06 Ahn Add Start
								, nSurfaceGrayCnt
								, nFoilExpGrayCnt
								// 23.01.06 Ahn Add End
							);
							CWin32File::TextSave1Line(strFilePath, strCsvFileName, strResult, _T("at"), FALSE);
						}

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Write CSV"), pTopInfo->nTabNo);


						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
						//SPC+ IMAGE Save===================================================================================================
						CString strSPCFilePath = InspInData->ImageFilePath();
						int SPCImageQuality = CGlobalFunc::StringToint(SPCINFO->getIqJpgQuality());
						for (int i = 0; i < MAX_CAMERA_NO; i++)
						{
							CFrameRsltInfo* pFrmRsltInfo;

							CString SPCImageFileName = "";
							if (i == CAM_POS_TOP)
							{
								pFrmRsltInfo = pTopInfo;
								SPCImageFileName = IqImageFileNameTop;

							}
							else
							{
								pFrmRsltInfo = pBtmInfo;
								SPCImageFileName = IqImageFileNameBottom;
							}

							int nImgSize = pFrmRsltInfo->m_nWidth * pFrmRsltInfo->m_nHeight;

							//SPC+ 전송용 이미지 파일 저장 정보
							if (nImgSize)
							{
								pFrmRsltInfo->m_nWidth;
								CImgSaveInfo* pSaveInfo = new CImgSaveInfo;
								BYTE* pImgSavePtr;
								pImgSavePtr = new BYTE[nImgSize + 1];
								memset(pImgSavePtr, 0x00, sizeof(BYTE) * nImgSize + 1);
								memcpy(pImgSavePtr, pFrmRsltInfo->GetImagePtr(), sizeof(BYTE) * nImgSize);
								//퀄리티 정보를  세팅한다.
								pSaveInfo->SetImgPtr(pImgSavePtr, pFrmRsltInfo->m_nWidth, pFrmRsltInfo->m_nHeight, SPCImageQuality);
								pSaveInfo->m_strSavePath.Format(_T("%s\\%s"), strSPCFilePath, SPCImageFileName);
								pImgSaveQueueCtrl->PushBack(pSaveInfo);
							}

							//기존 이미지 저장정보
							if (nImgSize && pFrmRsltInfo->m_bSaveFlag)
							{
								if (pFrmRsltInfo->m_pTabRsltInfo->m_bImageFlag == TRUE)
								{
									if (pImgSaveQueueCtrl->GetSize() < MAX_SAVE_IMAGE_QUEUE)
									{
										pFrmRsltInfo->m_nWidth;
										CImgSaveInfo* pSaveInfo = new CImgSaveInfo;
										BYTE* pImgSavePtr;
										pImgSavePtr = new BYTE[nImgSize + 1];
										memset(pImgSavePtr, 0x00, sizeof(BYTE)* nImgSize + 1);
										memcpy(pImgSavePtr, pFrmRsltInfo->GetImagePtr(), sizeof(BYTE) * nImgSize);
										pSaveInfo->SetImgPtr(pImgSavePtr, pFrmRsltInfo->m_nWidth, pFrmRsltInfo->m_nHeight);
										pSaveInfo->m_strSavePath.Format(_T("%s\\%s"), pFrmRsltInfo->m_pTabRsltInfo->m_chImagePath, pFrmRsltInfo->m_pTabRsltInfo->m_chImageFile);
										pImgSaveQueueCtrl->PushBack(pSaveInfo);

									}
								}
							}
						}
#else
						// 22.05.31 Ahn Add Start - Image Save Thread 
						for (int i = 0; i < MAX_CAMERA_NO; i++)
						{
							CFrameRsltInfo* pFrmRsltInfo;
							if (i == CAM_POS_TOP)
							{
								pFrmRsltInfo = pTopInfo;
							}
							else
							{
								pFrmRsltInfo = pBtmInfo;
							}
							// 22.08.09 Ahn Move Start
							int nImgSize = pFrmRsltInfo->m_nWidth * pFrmRsltInfo->m_nHeight;
							// 22.08.09 Ahn Move End
							if (pFrmRsltInfo->m_pTabRsltInfo->m_bImageFlag == TRUE)
							{
								if (pImgSaveQueueCtrl->GetSize() < MAX_SAVE_IMAGE_QUEUE)
								{
									if (pFrmRsltInfo->GetImagePtr())
									{
										pFrmRsltInfo->m_nWidth;
										CImgSaveInfo* pSaveInfo = new CImgSaveInfo;
										BYTE* pImgSavePtr;
										pImgSavePtr = new BYTE[nImgSize + 1];
										memset(pImgSavePtr, 0x00, sizeof(BYTE)* nImgSize + 1);
										CopyMemory(pImgSavePtr, pFrmRsltInfo->GetImagePtr(), sizeof(BYTE) * nImgSize);
										pSaveInfo->SetImgPtr(pImgSavePtr, pFrmRsltInfo->m_nWidth, pFrmRsltInfo->m_nHeight);
										pSaveInfo->m_strSavePath.Format(_T("%s\\%s"), pFrmRsltInfo->m_pTabRsltInfo->m_chImagePath, pFrmRsltInfo->m_pTabRsltInfo->m_chImageFile);
										pImgSaveQueueCtrl->PushBack(pSaveInfo);

//										AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <ImageSave> %s"), pSaveInfo->m_strSavePath);
									}
									else
									{
										AprData.SaveDebugLog_Format(_T("@@@@@@@@@@@@@@@@@<CtrlThreadImgProc> Image Save Info NULL "));
									}


									//								CString strMsg;
									//								strMsg.Format(_T("Save Image Path = %s"), pSaveInfo->m_strSavePath);
									//								AprData.SaveDebugLog(strMsg); //pyjtest


								}
								else
								{
									AprData.SaveDebugLog_Format(_T("@@@@@@@@@@@@@<CtrlThreadImgProc> <ImageSave> Overflow Q-Size(%d>=%d)"), pImgSaveQueueCtrl->GetSize(), MAX_SAVE_IMAGE_QUEUE);
								}
							}
						}
#endif //SPCPLUS_CREATE
						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Save Image PushBack"), pTopInfo->nTabNo);



						//double dTactTime = CGlobalFunc::GetDiffTime(pTopInfo->m_stTime, pTopInfo->m_dFrecuency);
						//CTactTimeData data;
						//data.nCellNo = pTopInfo->m_pTabRsltInfo->m_nTabNo;
						//data.dTactTime = dTactTime;
						//pTactCtrl->AddNewTactData(data);


						//======TacTime 출력 ========================================================================
						pTopInfo->m_tacTimeList[3] = CGlobalFunc::GetDiffTime(pTopInfo->m_stTime, pTopInfo->m_dFrecuency);
						pBtmInfo->m_tacTimeList[3] = CGlobalFunc::GetDiffTime(pBtmInfo->m_stTime, pBtmInfo->m_dFrecuency);


						LOGDISPLAY_SPEC(6)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - ResultProcWait-Exit",
							"Top", pTopInfo->nTabNo, pTopInfo->m_nTabId_CntBoard
							);

						int tempTabNo = pTopInfo->nTabNo;

						CImageProcessCtrl::GetResultPtr(CAM_POS_TOP)->PushBack((CFrameInfo*)pTopInfo);
						CImageProcessCtrl::GetResultPtr(CAM_POS_BOTTOM)->PushBack((CFrameInfo*)pBtmInfo);

						LOGDISPLAY_SPEC(6)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - ResultProcWait-Exit1",
							"Top", pTopInfo->nTabNo, pTopInfo->m_nTabId_CntBoard
							);

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- pRsltQueueCtrl->PushBack()"), tempTabNo);


						AprData.m_NowLotData.m_ctLastAcqTime = CTime::GetCurrentTime();

						// 22.04.06 Ahn Modify Start
						CAppDIO dio;

						dio.OutputBit(CAppDIO::eOut_PULSE, FALSE);

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- OutputBit"), tempTabNo);


						// 22.02.17 Ahn Modify End

						// 22.02.17 Ahn Modify Start
						if (bClearFlag == TRUE)
						{
							//						CSigProc *pSigProc = theApp.m_pSigProc;
							//						WORD wResetCode = 0x00;
							//						pSigProc->WriteAlarmCode(wResetCode);

							AprData.SaveMemoryLog(_T("------ NG_STOP Signal OFF ------"));
							bJudgeNG = FALSE;
							bClearFlag = FALSE;// 22.03.28 Ahn Add
						}

						break;
					}
					else if ((topWaitVal == 2) || (btmWaitVal == 2))
					{
						LOGDISPLAY_SPEC(6)("UnitThread - ResultProcWait-Timeout"	);


						//출력 대기 이벤트 객체 pop, 이벤트 닫기

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> UnitThread - ResultProcWait-Timeout"));


						break;
					}
				}

				AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> ResultProc End") );


				if (pUnitTop)
				{
					delete pUnitTop;
					pUnitTop = NULL;
				}
				if (pUnitBtm)
				{
					delete pUnitBtm;
					pUnitBtm = NULL;
				}

				theApp.m_nImageProcThreadTimeAfter = GetTickCount();

				////출력 대기 이벤트 객체 pop, 이벤트 닫기
				//pThis->m_pParent->ImgProcWaitThread_Event_pop();
				//CloseHandle(hEvent);
			}

		}
		else
		{
			break;
		}
	}

	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	return 0;
}

WORD CImageProcThread::GetCounterSignal(int nTabId, int nJudge1, int nJudge2, int nMarkSel1, int nMarkSel2)
{
	WORD wOutput = 0x00;

	// 22.01.11 Ahn Add Start
	// 마킹 테스트용 모든 탭 마킹 신호 출력.
	if (AprData.m_System.m_bMarkingAllTab == TRUE)
	{
		nJudge1 = JUDGE_NG;
		nJudge2 = JUDGE_NG;
		// 22.08.11 Ahn Add Start
		nMarkSel1 = 1;
		nMarkSel2 = 0;
		// 22.08.11 Ahn Add Start
	}
	// 22.01.11 Ahn Add End

	if ( ( nJudge1 == JUDGE_NG )  || ( nJudge2 == JUDGE_NG ) )
	{
		wOutput |= CAppDIO::eOut_TAB_JUDGE_SURFACE;	//기존 버전 0번 bit 사용
		//wOutput |= CAppDIO::eOut_TAB_JUDGE_SIZE;
		//wOutput |= CAppDIO::eOut_TAB_JUDGE_SIZE ; // 22.08.11 Ahn Test
		//22.07.26 Ahn Modify Start
		//int nMarkSel = (nMarkSel1 || nMarkSel2 );
		int nMarkSel = ( nMarkSel1 | (nMarkSel2 << 1));
		//22.07.26 Ahn Modify End
		switch (nMarkSel) {
		case	0:
			break;
		case	1:
			wOutput |= CAppDIO::eOut_MARK_SEL_01;
			break;
		case	2 :
		case	3 :
			wOutput |= CAppDIO::eOut_MARK_SEL_01;
			wOutput |= CAppDIO::eOut_MARK_SEL_02;
			break;
		}
	}

	//wOutput |= CAppDIO::eOut_PULSE; // 22.04.07 Ahn Delete 
	wOutput |= (nTabId << 2) & CAppDIO::eInOut_ID_Mask; // 3F = 1111 1100

	return wOutput;
}

// 22.07.19 Ahn Modify Start
int CImageProcThread::GetMarkingFlag(CRecipeInfo* pRecipeInfo, int nTopJudge, int nBtmJudge, WORD wTopReson, WORD wBtmReson, int& nMarkSel1, int& nMarkSel2 )
{
	int nRet = 0 ; 
	ASSERT(pRecipeInfo);

	// 판정결과 적용
	pRecipeInfo = AprData.m_pRecipeInfo;
	if ((nTopJudge == JUDGE_NG) || (nBtmJudge == JUDGE_NG))
	{
		WORD wReason = wTopReson | wBtmReson ;
		if ( ( wReason & CTabRsltBase::en_Reason_Surface_Top ) || ( wReason & CTabRsltBase::en_Reason_Surface_Btm ) )
		{
			if (pRecipeInfo->nMarkingUse[en_ModeSurface] == TRUE)
			{
				if (pRecipeInfo->nMarkingType[en_ModeSurface] == 0)
				{
					nMarkSel1 = 1;
				}
				else
				{
					nMarkSel1 = 1;
					nMarkSel2 = 1;
				}
			}
		}

		if ((wReason & CTabRsltBase::en_Reason_FoilExpIn_Top) || (wReason & CTabRsltBase::en_Reason_FoilExpIn_Btm))
		{
			if (pRecipeInfo->nMarkingUse[en_ModeFoilExp] == TRUE)
			{
				if (pRecipeInfo->nMarkingType[en_ModeFoilExp] == 0)
				{
					nMarkSel1 = 1;
				}
				else
				{
					nMarkSel1 = 1;
					nMarkSel2 = 1;
				}
			}
		}

		if ((wReason & CTabRsltBase::en_Reason_FoilExpOut_Top) || (wReason & CTabRsltBase::en_Reason_FoilExpOut_Btm))
		{
			if (pRecipeInfo->nMarkingUse[en_ModeFoilExpOut] == TRUE)
			{
				if (pRecipeInfo->nMarkingType[en_ModeFoilExpOut] == 0)
				{
					nMarkSel1 = 1;
				}
				else
				{
					nMarkSel1 = 1;
					nMarkSel2 = 1;
				}
			}
		}

		if ((wReason & CTabRsltBase::en_Reason_FoilExpBoth_Top) || (wReason & CTabRsltBase::en_Reason_FoilExpBoth_Btm))
		{
			if (pRecipeInfo->nMarkingUse[en_ModeFoilExpOut/*en_ModeFoilBoth*/] == TRUE)
			{
				if (pRecipeInfo->nMarkingType[en_ModeFoilExpOut/*en_ModeFoilBoth*/] == 0)
				{
					nMarkSel1 = 1;
				}
				else
				{
					nMarkSel1 = 1;
					nMarkSel2 = 1;
				}
			}
		}


	}

	nRet = ( nMarkSel1 | ( nMarkSel2 << 1 ) );
	return nRet;
}
// 22.07.19 Ahn Modify End
