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
		}
		else {
			m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadImgProc,
				(LPVOID)this,
				THREAD_PRIORITY_HIGHEST,
				0,
				CREATE_SUSPENDED,
				NULL);
		}
		if ( m_pThread != NULL ) {
			m_pThread->m_bAutoDelete = FALSE ;
			m_pThread->ResumeThread() ;
		}
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


// Queue에서 받아온 Frame Image를 Tab 으로 구분해서 처리용 Queue로 저장 하는 Thread
#define IMAGEPROCTHREAD_TABFIND_TIMEOUT 60
UINT CImageProcThread::CtrlThreadImgCuttingTab(LPVOID Param)
{
	CImageProcThread* pThis = (CImageProcThread*)Param;
	CQueueCtrl* pQueueFrame_Top = pThis->m_pParent->GetQueueFrmPtr(0);
	CQueueCtrl* pQueueFrame_Bottom = pThis->m_pParent->GetQueueFrmPtr(1);
	CCounterQueueCtrl* pCntQueueInCtrl = pThis->m_pParent->GetCounterQueInPtr(); // 탭 카운터 용 큐
	CThreadQueueCtrl* pThreadQue[MAX_CAMERA_NO];
	pThreadQue[CAM_POS_TOP] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_TOP);
	pThreadQue[CAM_POS_BOTTOM] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_BOTTOM);

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

	static int TempLogCount = 0;

	UINT ret = 0;
	//스래드 대기 여부
	BOOL bThreadWait = TRUE;
	while (1)
	{
		//타임 주기 이벤트
		//대기 상태일 때
		if (bThreadWait)
			ret = WaitForSingleObject(pThis->getEvent_ImageProcThread_TabFind(), INFINITE);
		//대기하지 않고 바로 처리한다.
		else
			ret = WAIT_OBJECT_0;

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return 0;
		}
		else if (ret == WAIT_OBJECT_0) //TIMEOUT시 명령
		{

			//로그카운트 증가
			TempLogCount++;
			if (pThis == NULL) {
				break;
			}
			if (pThis->m_bKill == TRUE) {
#if !defined( BOTTOM_CAMERA_DEBUG )
				if ((pQueueFrame_Top != NULL) && (pQueueFrame_Bottom != NULL)) {
					if ((pQueueFrame_Top->GetSize() <= 0) || (pQueueFrame_Bottom->GetSize() <= 0)) {
						break;
					}
				}
#else
				if ((pQueueFrame_Top != NULL)) {
					if ((pQueueFrame_Top->GetSize() <= 0)) {
						break;
					}
				}
#endif
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

				//Image Cutting Tab 정보 출력 로그
				LOGDISPLAY_SPEC(5)("Logcount<%d> Top-Bottom Frame 사이즈 차가 5 이상이면 엔코더 신호 에러", TempLogCount);

				// 에러 처리 
			//	pThis->SetFameSizeError(); // 
				CString strErrMsg;
				strErrMsg.Format(_T("프레임 사이즈 이상 : Top [%d], Bottom[%d], 검사 상태 [%d], 종료 처리!!!!"), nSizeFrmL, nSizeFrmR, pThis->m_pParent->IsInspection());
				//	AprData.SaveErrorLog(strErrMsg);

					//카메라의 에러를 세팅한다.
				AprData.m_ErrStatus.SetError(CErrorStatus::en_CameraError, strErrMsg);
			}
#if defined( BOTTOM_CAMERA_DEBUG )
			bTailFlag = TRUE;
#endif
			// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
			BOOL bMakeDummyBtm;
			//if ((bHeadFlag == TRUE) && (bTailFlag == FALSE)) {
			if (((nSizeFrmL - nSizeFrmR) > 0) && (btLastBtmImg != NULL)) {
				bMakeDummyBtm = TRUE;
				AprData.SaveDebugLog(_T("!!!! MAKE Dummy Data -- 00 !!!!"));
			}
			else {
				bMakeDummyBtm = FALSE;
			}
#endif
			// 22.02.22 Ahn Add End

					// 22.02.22 Ahn Modify Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
			if ((bHeadFlag && bTailFlag) || (bMakeDummyBtm == TRUE))
			{
#else
		//Top과 Bottom  정보가 1개씩 이상이 되어야 한다.
			if (bHeadFlag && bTailFlag)
			{
#endif

				//Image Cutting Tab 정보 출력 로그
				LOGDISPLAY_SPEC(5)("Logcount<%d> =====================================================================", TempLogCount);
				LOGDISPLAY_SPEC(5)("Logcount<%d> 카메라에서 읽은 Top Frame 크기<%d>, Bottom Frame 크기<%d>"
					, TempLogCount, nSizeFrmL, nSizeFrmR);
				//Image Cutting Tab 정보 출력 로그
				LOGDISPLAY_SPEC(5)("Logcount<%d> Top-Bottom Frame 정보 처리 구간", TempLogCount);

				// 22.02.22 Ahn Modify End
				//Dalsa Camera Callback 함수에서 넣은 이미지 데이터가 저장된 Top 객체를 가져온다.
				CFrameInfo* pFrmInfo_Top = pQueueFrame_Top->Pop();

				//Top 이미지의 크기 값
				int nHeight = pFrmInfo_Top->m_nHeight;
				int nFrmWIdth = pFrmInfo_Top->m_nWidth;
				int nWidth = nFrmWIdth;

				//Image Cutting Tab 정보 출력 로그
				LOGDISPLAY_SPEC(5)("Logcount<%d> Top 이미지 크기 값 =>  높이 : nHeight<%d>, 넓이 : nWidth<%d> ",
					TempLogCount, nHeight, nWidth);

				// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
				CFrameInfo* pFrmInfo_Bottom;
				if (bMakeDummyBtm == TRUE)
				{
					pFrmInfo_Bottom = new CFrameInfo;
					pFrmInfo_Bottom->m_nFrameCount = pFrmInfo_Top->m_nFrameCount;
					pFrmInfo_Bottom->m_nWidth = pFrmInfo_Top->m_nWidth;
					pFrmInfo_Bottom->m_nHeight = pFrmInfo_Top->m_nHeight;
					pFrmInfo_Bottom->m_nHeadNo = CAM_POS_BOTTOM;
					pFrmInfo_Bottom->m_nBand = 1;
					BYTE* pDummyImgPtr = new BYTE[nWidth * nHeight];
					//memset(pDummyImgPtr, 0x00, sizeof(BYTE)nWidth * nHeight);
					//if (btLastBtmImg != NULL) {
					AprData.SaveDebugLog(_T("!!!! Last bottom frame copy start !!!!"));
					memcpy(pDummyImgPtr, btLastBtmImg, sizeof(BYTE) * nWidth * nHeight);
					AprData.SaveDebugLog(_T("!!!! Last bottom frame copy end !!!!"));
					//}
					//else {
					//	memset(pDummyImgPtr, 0x00, sizeof(BYTE)* nWidth * nHeight);
					//}
					pFrmInfo_Bottom->SetImgPtr(pDummyImgPtr);
					AprData.SaveDebugLog(_T("!!!! MAKE Dummy Data !!!!"));
					pFrmInfo_Bottom->m_bErrorFlag = TRUE;
				}
				else
				{
					pFrmInfo_Bottom = pQueueFrame_Bottom->Pop();
				}

#else
// 22.02.22 Ahn Add End
// 22.01.04 Ahn Modify Start
#if !defined( BOTTOM_CAMERA_DEBUG )

			//Dalsa Camera Callback 함수에서 넣은 이미지 데이터가 저장된 Bottom 객체를 가져온다.
				CFrameInfo* pFrmInfo_Bottom = pQueueFrame_Bottom->Pop();

				//Image Cutting Tab 정보 출력 로그
				LOGDISPLAY_SPEC(5)("Logcount<%d> Queue에 저장된 Frame_Bottom  FrameInfo를 Pop한다.===남은 크기<%d> ", TempLogCount, pQueueFrame_Bottom->GetSize());
#else
				CFrameInfo* pFrmInfo_Bottom = new CFrameInfo();
				BYTE* pImg = new BYTE[nWidth * nHeight];
				memset(pImg, 0x00, sizeof(BYTE) * nWidth * nHeight);

				BOOL bSend = FALSE;
				pFrmInfo_Bottom = new CFrameInfo;
				pFrmInfo_Bottom->SetImgPtr(pImg);
				pFrmInfo_Bottom->m_nHeight = nHeight;
				pFrmInfo_Bottom->m_nWidth = nWidth;
				pFrmInfo_Bottom->m_nFrameCount = pFrmInfo_Top->m_nFrameCount;
				pFrmInfo_Bottom->m_nBand = 1;
				pFrmInfo_Bottom->m_nHeadNo = CAM_POS_BOTTOM;
#endif 
				// 22.01.04 Ahn Modify End
				// 22.02.22 Ahn Add Start
#endif
// 22.02.22 Ahn Add End
			//Top Frame 번호
				int nFrameCountL = pFrmInfo_Top->m_nFrameCount;
				//Bottom Frame 번호
				int nFrameCountR = pFrmInfo_Bottom->m_nFrameCount;

				//Image Cutting Tab 정보 출력 로그
				LOGDISPLAY_SPEC(5)("Logcount<%d> Top Frame 번호<%d>, Bottom Frame 번호<%d> ",
					TempLogCount, nFrameCountL, nFrameCountR);

				// 22.11.30 Ahn Modify Start
				AprData.m_NowLotData.m_nFrameCount = pFrmInfo_Top->m_nFrameCount;
				// 22.11.30 Ahn Modify End

				BYTE* pHeadPtr = pFrmInfo_Top->GetImagePtr();
				BYTE* pTailPtr = pFrmInfo_Bottom->GetImagePtr();



				// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
				if (btLastBtmImg == NULL)
				{
					btLastBtmImg = new BYTE[nWidth * nHeight];
				}
				memcpy(btLastBtmImg, pTailPtr, sizeof(BYTE) * nWidth * nHeight);
#endif
				// 22.02.22 Ahn Add End

				//처리시간 체크 객체 생성 및 시간 진행
				CTimeAnalyzer ctAna;
				ctAna.Clear();
				ctAna.StopWatchStart();

				// Tab으로 잘라 보냄
				// Projection 사용 
				{
					// 22.05.09 Ahn Add Start
					int nBndElectrode = 0;
					int nBneElectrodeBtm = 0;
#if defined( ANODE_MODE )
					//양극일 경우 Top 프로젝션 데이터의 바운드리 위치 크기를 가져온다.
					nBndElectrode = CImageProcess::GetBoundaryOfElectorde(pHeadPtr, nWidth, nHeight, AprData.m_pRecipeInfo, CImageProcess::en_FindFromLeft);

					//Image Cutting Tab 정보 출력 로그
					LOGDISPLAY_SPEC(5)("Logcount<%d> TabFindPos 값 => 양극경우 CImageProcess::GetBoundaryOfElectorde 처리 값을  nBndElectrode 저장 ", TempLogCount);
#endif
					// 22.05.09 Ahn Add End
					//Tab 정보를 저장할 vector 임시 객체
					CImageProcess::_VEC_TAB_INFO vecTabInfo;
					int nLevel = 0;
					int nBtmLevel = 0;

					//Tab 위치 : 양극일 경우 nBndElectrode 값에 레시피 Tab Condition 카메라 높이
					int nTabFindPos = nBndElectrode + AprData.m_pRecipeInfo->TabCond.nCeramicHeight;

					//Image Cutting Tab 정보 출력 로그
					LOGDISPLAY_SPEC(5)("Logcount<%d> TabFindPos 값 =>  nBndElectrode<%d> + Recipe에 설정한 CeramicHeight 값<%d>으로 구한다.",
						TempLogCount, nBndElectrode, AprData.m_pRecipeInfo->TabCond.nCeramicHeight);

					//이미지 프로세싱을 위한 클래스 
					//이미지 Tab 정보에서 Tab을 그룹으로 나누기
					int nLocalRet = CImageProcess::DivisionTab_FromImageToTabInfo(pHeadPtr, pTailPtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo, nFrameCountL, TempLogCount);


					//Tab 정보 크기, Tab 정보가 없다면 에러처리
					int nVecSize = (int)vecTabInfo.size();

					//Image Cutting Tab 정보 출력 로그
					LOGDISPLAY_SPEC(5)(_T("Logcount<%d> Top TabInfo 갯수<%d>"),
						TempLogCount, nVecSize);

					BOOL bErrorAll = FALSE;
					if (nVecSize <= 0)
					{
						// 강제 분할 
						bErrorAll = TRUE;
						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(5)(_T("Logcount<%d> CtrlThreadImgCuttingTab - 찾은 Tab정보 없음 processing NG 마킹 처리"),
							TempLogCount);
					}
#if defined( ANODE_MODE )
					//양극일 경우 Bottom 프로젝션 데이터의 바운드리 위치 크기를 가져온다.
					nBneElectrodeBtm = CImageProcess::GetBoundaryOfElectordeBottom(pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo);
#else
					int nLocalRet2 = CImageProcess::FindTabLevel(pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
#endif
					// 21.12.28 Ahn Modify End
					//Top Bottom 프로젝션 바운드리 처리 시간 체크 및 로그 출력
					double dTime = ctAna.WhatTimeIsIt_Double();
					CString strLog;
					strLog.Format(_T("TabCutting Time [%.2lf]msec, 전극경계Top[%d], Bottom[%d] BtmLevel[%d]"), dTime, nBndElectrode, nBneElectrodeBtm, nBtmLevel);
					AprData.SaveTactLog(strLog);

					//Image Cutting Tab 정보 출력 로그
					LOGDISPLAY_SPEC(5)(_T("Logcount<%d> TabCutting Time [%.2lf]msec, 전극경계Top[%d], Bottom[%d] BtmLevel[%d]"),
						TempLogCount, dTime, nBndElectrode, nBneElectrodeBtm, nBtmLevel);

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
							AprData.m_NowLotData.m_nTabIDEmptyTotalCnt++;
							//2개 이상 TabID가 안들어왔을 때는 ID를 64를 준다.
							if (quUserTabID.size() > 2)
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

							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPEC(1)(_T("Tab ID Empty Totalcount<%d> - Set TabID<%d>"),
								AprData.m_NowLotData.m_nTabIDEmptyTotalCnt, cntInfo.nTabID);
						}

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(1)("Tab ID Empty Totalcount<%d>, TabID Overflow TotalCount<%d>, Diff<%d>-<%s>",
							AprData.m_NowLotData.m_nTabIDEmptyTotalCnt, AprData.m_NowLotData.m_nTabIDOverflowTotalCnt
							, (AprData.m_NowLotData.m_nTabIDEmptyTotalCnt - AprData.m_NowLotData.m_nTabIDOverflowTotalCnt)
							, (AprData.m_NowLotData.m_nTabIDEmptyTotalCnt > AprData.m_NowLotData.m_nTabIDOverflowTotalCnt) ? "Big-TabEmpty" :
							(AprData.m_NowLotData.m_nTabIDEmptyTotalCnt < AprData.m_NowLotData.m_nTabIDOverflowTotalCnt) ? "Big-TabOverflow" : "TabEmpty == TabOverflow");

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(5)(_T("Logcount<%d> 사용할 Trigger Tab Id<%d>"),
							TempLogCount, cntInfo.nTabID);

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
							nErrorNo = 1;
							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPECTXT(1)("^^TabFind-NGSet Kind : Left가 레시피 설정 반지름 보다 작거나 Right 가 레시피 설정 반지름 보다 작다");

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: Tab 반지름 Error(nLeft:%d<%d or nRight:%d<%d)"),
								pTabInfo->m_bErrorFlag, nErrorNo, nLeft, AprData.m_pRecipeInfo->TabCond.nRadiusW, nRight, AprData.m_pRecipeInfo->TabCond.nRadiusW);

						}

						//에레 체크 : 
						if ((AprData.m_NowLotData.m_bProcError == TRUE) && (AprData.m_System.m_bFirstTabDoNotProc == TRUE))
						{
							pTabInfo->m_bErrorFlag = TRUE;
							AprData.m_NowLotData.m_bProcError = FALSE;
							nErrorNo = 2;
							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPECTXT(1)("^^TabFind-NGSet Kind : Left가 레시피 설정 반지름 보다 작거나 Right 가 레시피 설정 반지름 보다 작다으면서 시스템 설정 m_bFirstTabDoNotProc가 TRUE");

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: Inital 상태 or 첫 Tab"),
								pTabInfo->m_bErrorFlag, nErrorNo);

						}

						// 21.12.28 Ahn Add Start
						//Tab 정보 크기, Tab 정보가 없을 때
						if (bErrorAll == TRUE)
						{
							pTabInfo->m_bErrorFlag = TRUE;
							nErrorNo = 3;
							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPECTXT(1)("^^TabFind-NGSet Kind : Tab 정보 구하기에서 못 구했을 때");

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: Tab 정보 없음"),
								pTabInfo->m_bErrorFlag, nErrorNo);

						}

						// 21.12.28 Ahn Add End
						// 22.06.22 Ahn Add Start

						//Tab부의 흑연 코팅높이 에러
						if (nLevel <= 0)
						{
							pTabInfo->m_bErrorFlag = TRUE;
							nErrorNo = 4;

							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPEC(1)("^^TabFind-NGSet Kind: nLevel 이미지 바운드리 값이 잘못되었을 때<%d>", nLevel);

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: nLevel Error (nLevel:%d <= 0)"),
								pTabInfo->m_bErrorFlag, nErrorNo, nLevel);

						}
						// 22.06.22 Ahn Add End

						// 22.09.30 Ahn Add Start

						//Tab부의 흑연 코팅높이 에러
						if (nLevel >= (nWidth - 100))
						{
							pTabInfo->m_bErrorFlag = TRUE;
							nErrorNo = 5;

							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPEC(1)("^^TabFind-NGSet Kind : nLevel 이미지 바운드리 값<%d>이 (이미지 넓이<%d>-100) 보다 클 경우",
								nLevel, nWidth);

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: nLevel Error (nLevel:%d >= (nWidth:%d - 100 ))"),
								pTabInfo->m_bErrorFlag, nErrorNo, nLevel, nWidth);

						}
						// 22.09.30 Ahn Add End

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(5)("Logcount<%d> 구한 Tab 정보 => ImageLength<%d>, FrameCount<%d>, TabStartPosInFrame<%d>, TabLeft<%d>, TabRight<%d>, ErrorFlag<%d>",
							TempLogCount, pTabInfo->nImageLength, pTabInfo->nFrameCount, pTabInfo->nTabStartPosInFrame, pTabInfo->nTabLeft, pTabInfo->nTabRight, pTabInfo->m_bErrorFlag);

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> nVecSize:%d/%d, ImageLength:%d, FrameCount:%d, TabStartPosInFrame:%d, TabLeft:%d, TabRight:%d, nLevel:%d"),
							i, nVecSize, pTabInfo->nImageLength, pTabInfo->nFrameCount, pTabInfo->nTabStartPosInFrame, pTabInfo->nTabLeft, pTabInfo->nTabRight, nLevel);


						//프레임 정보 임시 객체(Top 프레임 정보 처리)
						CFrameInfo* pInfo;
						pInfo = new CFrameInfo;

						//임시 로그 카운터
						pInfo->TempLogCount = TempLogCount;

						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
					//SPc+ 객체를 Top에 만 추가한다.
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

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(5)("Logcount<%d> TopFrame Trigger Input ID Set <%d>", TempLogCount, pInfo->m_nTabId_CntBoard);

						pInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;
						pInfo->m_nBndElectrode = nBndElectrode;

						//프레임 정보 임시 객체(Bottom 프레임 정보 처리)
						CFrameInfo* pBtmInfo;
						pBtmInfo = new CFrameInfo;

						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
					//SPc+ 객체를 Top에 만 추가한다.
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

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(5)("Logcount<%d> BottomFrame Trigger Input ID Set <%d>", TempLogCount, pInfo->m_nTabId_CntBoard);

						pBtmInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;

						//Bottom 프로젝션 데이터의 바운드리 위치 크기를 가져온다.
						pBtmInfo->m_nBndElectrode = nBneElectrodeBtm;// 22.05.11 Ahn Add 
						// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
						if (bMakeDummyBtm == TRUE) {
							pBtmInfo->m_bErrorFlag = pFrmInfo_Bottom->m_bErrorFlag;
						}
#endif
						// 22.02.22 Ahn Add End

#if defined( BOTTOM_CAMERA_DEBUG )
						pBtmInfo->m_bErrorFlag = TRUE;
#endif

						// 22.05.18 Ahn Add Start

						//처리할  Top 프레임 정보 갯수
						int nTopQueCnt = pThreadQue[CAM_POS_TOP]->GetSize();
						//처리할 Bottom  프레임 정보 갯수
						int nBtmQueCnt = pThreadQue[CAM_POS_BOTTOM]->GetSize();

						//처리할 정보가 스킵 숫자보다 크다면
						if ((nTopQueCnt > IMAGE_PROC_SKIP_COUNT) && (nBtmQueCnt > IMAGE_PROC_SKIP_COUNT)
							//Top, Bottom 이미지 처리 프레임 정보가 스킵 숫자보다 크다면
							|| ((nSizeFrmL > IMAGE_PROC_SKIP_COUNT) && (nSizeFrmR > IMAGE_PROC_SKIP_COUNT))) {
							pInfo->m_bErrorFlag = TRUE;
							pBtmInfo->m_bErrorFlag = TRUE;

							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPEC(5)("Logcount<%d> Top/Bottom Overflow TopFrame<%d>, BottonFrame<%d>, TopQueueCnt<%d>, BottonQueueCnt<%d>"
								, TempLogCount, nSizeFrmL, nSizeFrmR, nTopQueCnt, nBtmQueCnt);

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> Top/Bottom Overflow TopFrameCnt:%d, BottonFrameCnt:%d, TopQueueCnt:%d, BottonQueueCnt:%d > IMAGE_PROC_SKIP_COUNT:%d"),
								nSizeFrmL, nSizeFrmR, nTopQueCnt, nBtmQueCnt, IMAGE_PROC_SKIP_COUNT);

						}
						// 22.05.18 Ahn Add Start

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


						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(1)("*2*2*Use-ID : TabID-Top[%d]-Bottom[%d], TabNo-Top[%d]-Bottom[%d]",
							pInfo->m_nTabId_CntBoard, pBtmInfo->m_nTabId_CntBoard, pInfo->nTabNo, pBtmInfo->nTabNo
							);


						//스래드에 처리할 정보를 저장 TOP, BOTTOM
						pThreadQue[CAM_POS_TOP]->push(pInfo);
						pThreadQue[CAM_POS_BOTTOM]->push(pBtmInfo);

						//Lot Data Tab 번호를 증가 시킨다.
						AprData.m_NowLotData.m_nTabCount++;

						//메모리 로그 기록
						CString strMsg;
						strMsg.Format(_T("Find Image Tab TotalCount<%d>"), AprData.m_NowLotData.m_nTabCount);
						AprData.SaveMemoryLog(strMsg);




						//메모리 로그 기록
						strMsg = "";
						strMsg.Format(_T("Recive TabID TotalCount<%d>, Find Image Tab TotalCount<%d>, CountDiff<%d>-<%s>"),
							AprData.m_NowLotData.m_nInputTabIDTotalCnt, AprData.m_NowLotData.m_nTabCount,
							abs(AprData.m_NowLotData.m_nInputTabIDTotalCnt - AprData.m_NowLotData.m_nTabCount)
							, (AprData.m_NowLotData.m_nInputTabIDTotalCnt > AprData.m_NowLotData.m_nTabCount) ? "Big TabID" :
							(AprData.m_NowLotData.m_nInputTabIDTotalCnt < AprData.m_NowLotData.m_nTabCount) ? "Big FindTab" : "TabID == FindTab");
						AprData.SaveMemoryLog(strMsg);

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(1)(_T("*2*3*Recive TabID TotalCount<%d>, Find Image Tab TotalCount<%d>, CountDiff<%d>-<%s>"),
							AprData.m_NowLotData.m_nInputTabIDTotalCnt, AprData.m_NowLotData.m_nTabCount,
							abs(AprData.m_NowLotData.m_nInputTabIDTotalCnt - AprData.m_NowLotData.m_nTabCount)
							, (AprData.m_NowLotData.m_nInputTabIDTotalCnt > AprData.m_NowLotData.m_nTabCount) ? "Big TabID" :
							(AprData.m_NowLotData.m_nInputTabIDTotalCnt < AprData.m_NowLotData.m_nTabCount) ? "Big FindTab" : "TabID == FindTab");

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(5)("Logcount<%d> Next TabNo<%d>", TempLogCount, AprData.m_NowLotData.m_nTabCount);
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

	// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
	if (btLastBtmImg != NULL) {
		delete[] btLastBtmImg;
		btLastBtmImg = NULL;
	}
#endif

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

#define IMAGEPROCTHREAD_RESULT_TIMEOUT 40
#define IMAGEPROCTHREAD_RESULTWAITE_TIMEOUT 5
UINT CImageProcThread::CtrlThreadImgProc(LPVOID Param)
{
	//스래드생성 시 넘긴 객체 포인터
	CImageProcThread* pThis = (CImageProcThread*)Param;

	//CImageSaveQueueCtrl에서 이미지 저장 객체<CImageSaveQueueCtrl> 포인터를 가져온다.
	CImageSaveQueueCtrl* pImgSaveQueueCtrl = pThis->m_pParent->GetImageSaveQueuePtr();

	//CImageSaveQueueCtrl에서 이미지 저장 객체<CTacTimeDataCtrl> 포인터를 가져온다.
	CTacTimeDataCtrl* pTactCtrl = pThis->m_pParent->GetTactDataCtrlPtr();

	//결과 저장객체를 가져온다.
	//CImageProcessCtrl의 객체 멤버인 결과저장 객체<CQueueCtrl> 포인터를 가져온다.
	CQueueCtrl* pRsltQueueCtrl[GRABBER_COUNT];
	for (int i = 0; i < GRABBER_COUNT; i++) {
		pRsltQueueCtrl[i] = pThis->m_pParent->GetResultPtr(i);
	}

	//CFrameRsltInfo* pFrmRsltInfo;
	CThreadQueueCtrl* pThdQue[MAX_CAMERA_NO];
	for (int i = 0; i < MAX_CAMERA_NO; i++) {
		pThdQue[i] = pThis->m_pParent->GetThreadQueuePtr(i);
	}

	CImageProcThreadUnit* pUnitTop = NULL;
	CImageProcThreadUnit* pUnitBtm = NULL;
	BOOL bBitmapSave = FALSE;
	BOOL bJudgeNG = FALSE;
	char szJudge[2][4] = { "OK", "NG" };
	char szPos[2][8] = { "TOP","BTM" };

	BOOL bMarkingActive = FALSE;
	BOOL bClearFlag = FALSE;

	UINT ret = 0;
	//이벤트 객체 생성(결과 기다리기 이벤트
	HANDLE pEvent_ResultWaitepEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	while (1)
	{
		//타임 주기 이벤트
		ret = WaitForSingleObject(pThis->getEvent_ImageProcThread_Result(), IMAGEPROCTHREAD_RESULT_TIMEOUT);

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return 0;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{

			if (pThis == NULL) {
				break;
			}
			if (pThis->m_bKill == TRUE) {
				break;
			}

			//Top/Botton에 Tab 정보가 저장된 정보가 있을 경우
			//위 함수 CtrlThreadImgCuttingTab Tab 정보를 찾는 스래드에서
			//GetThreadQueuePtr(i) Queue 에 push 되면 처리를 탄다.
			//처리를 하여 결과 정보 저장 Queue pRsltQueueCtrl 에 저장된다.
			if (!pThdQue[CAM_POS_TOP]->IsEmpty() && !pThdQue[CAM_POS_BOTTOM]->IsEmpty()) {
				pUnitTop = pThdQue[CAM_POS_TOP]->pop();
				pUnitBtm = pThdQue[CAM_POS_BOTTOM]->pop();

				while (1)
				{

					if (pThis->m_bKill == TRUE) {
						break;
					}

					// 22.12.09 Ahn Add Start
					LARGE_INTEGER stTime;
					// 22.12.09 Ahn Add End
					if ((pUnitTop->IsProcEnd() == TRUE) && (pUnitBtm->IsProcEnd() == TRUE))
					{


						CFrameRsltInfo* pTopInfo = pUnitTop->GetResultPtr();
						CFrameRsltInfo* pBtmInfo = pUnitBtm->GetResultPtr();

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> Process Start :: Board Tab ID <Top:%d/Btm:%d>, Find Tab ID <%d>"),
							pTopInfo->m_nTabId_CntBoard, pBtmInfo->m_nTabId_CntBoard, pTopInfo->nTabNo + 1);

						//로그 카운ㅌ 임시변수
						int TempLogCount = pTopInfo->TempLogCount;

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
						IqInfoTop->setIqScreenImageSizeX(CGlobalFunc::intToString(pTopInfo->m_nHeight));
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
						IqInfoBottom->setIqScreenImageSizeX(CGlobalFunc::intToString(pBtmInfo->m_nHeight));
						//판정결과
						CString IqBottomJudge = (pBtmInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) ? "NG" : "OK";
						IqInfoBottom->setImageJudge(IqBottomJudge);
						//SPC+ 저장할 이미지 명을 입력(Bottom 이미지 명)
						CString IqImageFileNameBottom = IqInfoBottom->ImagIqFileName();
						IqInfoBottom->setImageFileName(IqImageFileNameBottom);

#endif //SPCPLUS_CREATE

						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(5)("Top Logcount<%d> Bottom Logcount<%d> ========", pTopInfo->TempLogCount, pBtmInfo->TempLogCount);
						LOGDISPLAY_SPEC(1)("*4*1*Image Result Output : Top-TabID<%d>Bottom-TabID<%d>, Find TabNo<%d>"
							, pTopInfo->m_nTabId_CntBoard, pBtmInfo->m_nTabId_CntBoard, pTopInfo->nTabNo + 1);

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
								strMessage.Format(_T("연속 NG Alarm 발생. %d Tab연속 NG 발생"), AprData.m_NowLotData.m_nContinueCount);
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
									strMessage.Format(_T("구간 NG Alarm 발생. 발생 개수:%d, 설정 개수:%d, 설정 거리:%d"), nSecterNgCount, AprData.m_nSectorNgCount, AprData.m_nSectorBaseCount);
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

																	// Image Cutting Tab 정보 출력 로그
									LOGDISPLAY_SPEC(5)("Logcount<%d> NG Stop 신호처리 Trigger Input ID Set <%d>", TempLogCount, pTopInfo->m_nTabId_CntBoard);

									//								nJudge = tab.nJudge ;
									//								nCode = wAlarmCode ;
									//								pSigProc->ReportJudge(nId, nJudge, nCode);
																	// 22.12.12 Ahn Add End
								}
							}
						}
						else
						{
							//Image Cutting Tab 정보 출력 로그
							LOGDISPLAY_SPEC(5)("Logcount<%d> Image Result OK 처리 Tab ID<%d>", TempLogCount, pTopInfo->m_nTabId_CntBoard);

							AprData.m_NowLotData.m_nTabCountOK++;
							AprData.m_NowLotData.m_nContinueCount = 0; // 22.08.09 Ahn Add
							AprData.m_NowLotData.m_secNgJudge.AddOkTab(pTopInfo->nTabNo, AprData.m_nSectorBaseCount/*AprData.m_pRecipeInfo->nSectorCount*/);

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> Result OK :: Find Tab Count <%d>"), AprData.m_NowLotData.m_nTabCountOK);

						}
						// 결과 Queue에 보냄

						// Counter 신호 출력
						WORD wOutPut;
						CString strMarking = _T("OFF");
						{
							CAppDIO dio;
							int nMarkSel1 = 0;
							int nMarkSel2 = 0;

							// 22.07.19 Ahn Modify Start
							GetMarkingFlag(AprData.m_pRecipeInfo, nTopJudge, nBtmJudge, pTopInfo->m_pTabRsltInfo->m_wNgReason, pBtmInfo->m_pTabRsltInfo->m_wNgReason, nMarkSel1, nMarkSel2);
							// 22.07.19 Ahn Modify End

							CSigProc* pSigProc = theApp.m_pSigProc;
							bMarkingActive = pSigProc->GetInkMarkActive();
							//						bMarkingActive = TRUE; // PLC 강제 마킹 처리

							if ((AprData.m_System.m_bChkEnableMarker == FALSE) || (bMarkingActive == FALSE))
							{
								//체크박스 로그 출력
								LOGDISPLAY_SPEC(5)("Logcount<%d> 불량도 마킹 안함", TempLogCount);
								nMarkSel1 = 0;
								nMarkSel2 = 0;
							}

							wOutPut = CImageProcThread::GetCounterSignal(pTopInfo->m_nTabId_CntBoard, nTopJudge, nBtmJudge, nMarkSel1, nMarkSel2, TempLogCount);
							dio.OutputWord(wOutPut);

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <Ink Marking> :: Output ID<%d>, Value<%d>, Tab Cnt<%d>"),
								pTopInfo->m_nTabId_CntBoard, wOutPut, pTopInfo->nTabNo + 1);


							Sleep(20);
							dio.OutputBit(CAppDIO::eOut_PULSE, TRUE);

							CString strMsg;
							strMsg.Format(_T("Output ID[%d]_OutPutValue[0x%x]_TabNo[%d] : VISION Marking[%s], PLC Marking[%s]"),
								pTopInfo->m_nTabId_CntBoard, wOutPut, pTopInfo->nTabNo + 1,
								(AprData.m_System.m_bChkEnableMarker == FALSE) ? _T("SKIP") : _T("USE"),
								(bMarkingActive == FALSE) ? _T("SKIP") : _T("USE"));
							AprData.SaveMemoryLog(strMsg);

							// GEN 체크박스 Log 출력
							LOGDISPLAY_SPEC(1)("*4*2*Output : TabID[%d]-TabNo[%d]-Value[%d], Top-%s, Bottom-%s",
								pTopInfo->m_nTabId_CntBoard, pTopInfo->nTabNo + 1, wOutPut
								, (nTopJudge == JUDGE_NG) ? "NG" : "OK"
								, (nBtmJudge == JUDGE_NG) ? "NG" : "OK");


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

								CString strMsg;
								strMsg.Format(_T("Cell ID:%d, Judge:%d, Code:%d"), AprData.m_NowLotData.m_stCellJudge.dwCellTriggerID, AprData.m_NowLotData.m_stCellJudge.dwCellJudge, AprData.m_NowLotData.m_stCellJudge.dwCellNgCode);
								AprData.SaveDebugLog(strMsg); //pyjtest

							}


						}

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
								pImgSavePtr = new BYTE[nImgSize];
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
										pImgSavePtr = new BYTE[nImgSize];
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
									pFrmRsltInfo->m_nWidth;
									CImgSaveInfo* pSaveInfo = new CImgSaveInfo;
									BYTE* pImgSavePtr;
									pImgSavePtr = new BYTE[nImgSize];
									memcpy(pImgSavePtr, pFrmRsltInfo->GetImagePtr(), sizeof(BYTE) * nImgSize);
									pSaveInfo->SetImgPtr(pImgSavePtr, pFrmRsltInfo->m_nWidth, pFrmRsltInfo->m_nHeight);
									pSaveInfo->m_strSavePath.Format(_T("%s\\%s"), pFrmRsltInfo->m_pTabRsltInfo->m_chImagePath, pFrmRsltInfo->m_pTabRsltInfo->m_chImageFile);
									pImgSaveQueueCtrl->PushBack(pSaveInfo);

									AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <ImageSave> %s"), pSaveInfo->m_strSavePath);


									//								CString strMsg;
									//								strMsg.Format(_T("Save Image Path = %s"), pSaveInfo->m_strSavePath);
									//								AprData.SaveDebugLog(strMsg); //pyjtest


								}
								else
								{
									AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> <ImageSave> Overflow Q-Size(%d>=%d)"), pImgSaveQueueCtrl->GetSize(), MAX_SAVE_IMAGE_QUEUE);
								}
							}
						}
#endif //SPCPLUS_CREATE

						double dTactTime = GetDiffTime(pTopInfo->m_stTime, pTopInfo->m_dFrecuency);
						CTactTimeData data;
						data.nCellNo = pTopInfo->m_pTabRsltInfo->m_nTabNo;
						data.dTactTime = dTactTime;
						pTactCtrl->AddNewTactData(data);

						//체크박스 로그 출력
						LOGDISPLAY_SPEC(1)("*4*3*TabID[%d]-TabNo[%d] - TacTime[%f]",
							pTopInfo->m_nTabId_CntBoard, pTopInfo->m_pTabRsltInfo->m_nTabNo + 1, dTactTime);

						pRsltQueueCtrl[CAM_POS_TOP]->PushBack((CFrameInfo*)pTopInfo);
						pRsltQueueCtrl[CAM_POS_BOTTOM]->PushBack((CFrameInfo*)pBtmInfo);

						// GEN 체크박스 Log 출력
						LOGDISPLAY_SPEC(5)("Logcount<%d> Top/Bottom 마킹정보를 pRsltQueueCtrl 저장", TempLogCount);



						delete pUnitTop;
						delete pUnitBtm;

						AprData.m_NowLotData.m_ctLastAcqTime = CTime::GetCurrentTime();

						// 22.04.06 Ahn Modify Start
						CAppDIO dio;
						dio.OutputBit(CAppDIO::eOut_PULSE, FALSE);


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
					//Sleep(AprData.m_nSleep);
					WaitForSingleObject(pEvent_ResultWaitepEvent, IMAGEPROCTHREAD_RESULTWAITE_TIMEOUT);
				}
			}
			//Sleep(AprData.m_nSleep);
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

// 22.12.09 Ahn Add Start
double CImageProcThread::GetDiffTime(LARGE_INTEGER stTime, double dFrequency)
{
	LARGE_INTEGER edTime;
	QueryPerformanceCounter(&edTime);

	double	dv0, dv1;
	dv0 = (double)stTime.LowPart + ((double)stTime.HighPart * (double)0xffffffff);
	dv1 = (double)edTime.LowPart + ((double)edTime.HighPart * (double)0xffffffff);
	double	dtimev;
	dtimev = (dv1 - dv0) / dFrequency * (double)1000.0;
	return (dtimev);
}
// 22.12.09 Ahn Add End


WORD CImageProcThread::GetCounterSignal(int nTabId, int nJudge1, int nJudge2, int nMarkSel1, int nMarkSel2, int LogCount)
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
		//체크박스 출력 로그
		LOGDISPLAY_SPEC(5)("Logcount<%d> NG - Marking 설정 OUTPUT<%d>", LogCount, wOutput);
	}

	//wOutput |= CAppDIO::eOut_PULSE; // 22.04.07 Ahn Delete 
	wOutput |= (nTabId << 2) & CAppDIO::eInOut_ID_Mask; // 3F = 1111 1100

	//체크박스 출력 로그
	LOGDISPLAY_SPEC(5)("Logcount<%d> Trigger OUTPUT<%d> TabId<%d>", LogCount, wOutput, nTabId);

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
