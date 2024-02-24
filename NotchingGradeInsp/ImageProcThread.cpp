#include "pch.h"
#include "MainFrm.h"
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
#include "CCounterThread.h"

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

CString CImageProcThread::gKeyIdString = _T("");

CImageProcThread::CImageProcThread(CImageProcessCtrl *pParent)
{
	m_pParent = pParent ;
	m_pThread = NULL ;
	m_CreateMode = -1;
	pEvent_ImageProcThread_TabFind = NULL;
	pEvent_ImageProcThread_Result = NULL;
	pEvent_ImageProcThread_InspComplate = NULL;
}

CImageProcThread::~CImageProcThread(void)
{
	if (pEvent_ImageProcThread_TabFind)
	{
		::CloseHandle(pEvent_ImageProcThread_TabFind);
		pEvent_ImageProcThread_TabFind = NULL;
	}
	if (pEvent_ImageProcThread_Result)
	{
		::CloseHandle(pEvent_ImageProcThread_Result);
		pEvent_ImageProcThread_Result = NULL;
	}
	if (pEvent_ImageProcThread_InspComplate)
	{
		::CloseHandle(pEvent_ImageProcThread_InspComplate);
		pEvent_ImageProcThread_InspComplate = NULL;
	}
}

void CImageProcThread::Begin( int nMode ) // nMode  0 : Image Merge Mode , 1 : Image Proc Mode 
{
	m_bKill = FALSE ;
	m_CreateMode = nMode;
	//이벤트 객체 생성
	pEvent_ImageProcThread_TabFind = CreateEvent(NULL, TRUE, FALSE, NULL);
	pEvent_ImageProcThread_Result = CreateEvent(NULL, FALSE, FALSE, NULL);
	pEvent_ImageProcThread_InspComplate = CreateEvent(NULL, FALSE, FALSE, NULL);

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
		if (m_CreateMode == 1)
		{
			setEvent_ImageProcThread_Result();
		}
		CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
		m_pThread->m_hThread = NULL;
	}

}

double CImageProcThread::TabPitchCalculate(int bforeImageLengtch, int bforeTabLeft, int nownTabLeft, double dResolY)
{
	double realTabPitch = 0.0;
	realTabPitch = (double)((bforeImageLengtch - bforeTabLeft) + nownTabLeft) * (dResolY / 1000.0);
	return realTabPitch;
}

#define IMAGECUTTINGTAB_TIMEOUT 50
//#define TabPitch(a, b, c, d) ((a-b)+c)*(d)
#define TabPitch(a, b, c) ((a-b)+c)*(0.021)
//92.0 +- 20
#define MIN_TABPITCH 5.0
#define MAX_TABPITCH 5.0
static int bforeImageLengtch = 0;
static int bforeTabLeft = 0;

//44.79
#define MIN_TABWIDTH 40.0
#define MAX_TABWIDTH 50.0

static double RecipeInfoTabPitch = 0;
static double RecipeInfoTabWidth = 0;

//초기 실행 상태 확인 플래그
static bool bNowExecFlag = true;

// Queue에서 받아온 Frame Image를 Tab 으로 구분해서 처리용 Queue로 저장 하는 Thread
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
	CString strMsg;
	CString strTemp;
	CTabInfo RsvTabInfo;

	int nFindPos = 0;
	BOOL bHeadFlag = FALSE;
	BOOL bTailFlag = FALSE;
	BOOL bStopFlag = FALSE;

	// 22.04.06 Ahn Add Start - 첫 탭 버림
	BOOL bFirstTab = TRUE;
	// 22.04.06 Ahn Add End

	UINT ret = 0;
	//스래드 대기 여부
	BOOL bThreadWait = TRUE;

	//레시피 텝 피치 정보를 세팅한다.
	RecipeInfoTabPitch = (int)AprData.m_pRecipeInfo->TabCond.dTabPitch;
	RecipeInfoTabWidth = (int)AprData.m_pRecipeInfo->TabCond.dTabWidth;


	//다음 사용할 Tab ID (BCD ID)
	int nextBCDId = 64;

	//BCD ID 사용 아이디 차가 3이상이면 TRUE
	BOOL bBCDDiffBig = FALSE;

	//Trigger BCD 수신 카운터 변수가 MAX_INT를 5개 이상 들어온다면 초기화한다.
	//계속해서 BCD ID가 뒤에 들어오던가 아니면 안 들어오던가 ? 
	int TriggerBCDCountMAXINT = 0;

	//PET 설정 값
	int nPETCount = 0;
	BOOL bPETBCDIdSet = FALSE;

	//Last Backup
	int LastBCDIDBackup = 0;

	//사용한 BCD ID  백업
	//사용할 BCD ID가 들어오지 않았을 경우 사용
	int nUseBCDIDBackup = 0;


	//Image의 남은 픽셀 수 백업용
	//전에 남은 이미지 보다 작아 질 때
	UINT unNotUseCellLengthBackup = 0;
	//남은 Cell 크기(남은 이미지 픽셀수
	UINT unNotUseCellLength = 0;

//Encoder Counter 사용여부
#ifdef USE_BCDCOUNTER
	//Tab Counter log 변수들
	//총 Encoder Count 수
	UINT64 unTotalEncoderCount = 0;
	UINT64 unTotalEncoderCountBackup = 0;

	//총 Image Count 수
	UINT64 unTotalImageCount = 0;

	//총 Image Count 수
	UINT64 unTotalCellLength = 0;

	CString logStringEncoderCounter = 
		_T("TabNo	%d	")
		_T("LotID	%s	")
		_T("LastBCDID	%d	")
		_T("NowBCDID	%d	")
		_T("GrabCallBCDID	%d	")
		_T("GrabCallTime	%d	")
		_T("globalEncoderTotal	%d	")
		_T("globalImageTotal	%d	")
		_T("Diff_globalEnImg	%d	")
		_T("EncoderTotal	%d	")
		_T("ImageTotal	%d	")
		_T("Diff_EncoderImgCount	%d	")
		_T("NowEncoderCount	%d	")
		_T("NowCellLen	%d	")
		_T("TotalCellLen	%d	")
		_T("Diff_ImgUseTotal	%d	")
		_T("CellLen_notUse	%d	")
		_T("LastInputBCDID	%d")
		;
#endif //USE_BCDCOUNTER

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
			break;
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

			//TabNo 0으로 초기화 시 초기화한다.
			if (AprData.m_NowLotData.m_nTabCount == 0)
			{

//Encoder Counter 사용여부
#ifdef USE_BCDCOUNTER
				//총 Encoder Count 수
				unTotalEncoderCount = 0;
				//총 Image Count 수
				unTotalImageCount = 0;
				//총 Image Count 수
				unTotalCellLength = 0;
#endif //USE_BCDCOUNTER

				//Encoder Total Count
				AprData.m_NowLotData.m_unGTotalEncoderCount = 0;
				//Image Total Count
				AprData.m_NowLotData.m_unGTotalImageCount = 0;
			}

			//프레임 크기
			//Top Frame 크기
			int nSizeFrmL = pQueueFrame_Top->GetSize();
			//Bottom Frame 크기
			int nSizeFrmR = pQueueFrame_Bottom->GetSize();

			//프레임 정보가 Top1개 이상, Bottom 1개 이상
			bHeadFlag = !pQueueFrame_Top->IsEmpty();
			bTailFlag = !pQueueFrame_Bottom->IsEmpty();

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

//Encoder Counter 사용여부
#ifdef USE_BCDCOUNTER
				//총 Image Count 수 누적
				unTotalImageCount += pFrmInfo_Top->m_nHeight;
#endif //USE_BCDCOUNTER


				//Dalsa Camera Callback 함수에서 넣은 이미지 데이터가 저장된 Bottom 객체를 가져온다.
				CFrameInfo* pFrmInfo_Bottom = pQueueFrame_Bottom->Pop();

				//Top Frame 번호
				int nFrameCountL = pFrmInfo_Top->m_nFrameCount;
				//Bottom Frame 번호
				int nFrameCountR = pFrmInfo_Bottom->m_nFrameCount;

				AprData.m_NowLotData.m_nFrameCount = pFrmInfo_Top->m_nFrameCount;

				FrameImagePtr* pHeadPtr = pFrmInfo_Top->GetImagePtr();
				FrameImagePtr* pTailPtr = pFrmInfo_Bottom->GetImagePtr();


				//처리시간 체크 객체 생성 및 시간 진행
//				CTimeAnalyzer ctAna;
//				ctAna.Clear();
//				ctAna.StopWatchStart();



				DWORD dwTic = 0;

				// Tab으로 잘라 보냄
				// Projection 사용 
				{
					int nBndElectrode = 0;
					int nBneElectrodeBtm = 0;


					//양극일 경우 Top 프로젝션 데이터의 바운드리 위치 크기를 가져온다.
					//CImageProcess::en_FindFromLeft : 찾는 방향 왼쪽에서 오른쪽으로 찾음
					//return nBndElectrode : 실제 이미지에서 휘도 중 시작할 점을 찾음 - 휘도 샘플링을 이용하여 위치를 찾음
					nBndElectrode = CImageProcess::GetBoundaryOfElectorde(pHeadPtr->m_pImagePtr, nWidth, nHeight, AprData.m_pRecipeInfo, CImageProcess::en_FindFromLeft);

					// 22.05.09 Ahn Add End
					//Tab 정보를 저장할 vector 임시 객체
					CImageProcess::_VEC_TAB_INFO vecTabInfo;
					int nLevel = 0;
					int nBtmLevel = 0;

					//Tab 위치 : 양극일 경우 nBndElectrode 값에 레시피 Tab Condition 카메라 높이
					//휘도로 찾은 위치에 레시피 설정 높이를 더하여 Tab Find Pos 잡는다.
					int nTabFindPos = nBndElectrode + AprData.m_pRecipeInfo->TabCond.nCeramicHeight;

					//이미지 프로세싱을 위한 클래스 
					//이미지 Tab 정보에서 Tab을 그룹으로 나누기
					//PET Check TOP
					int nLocalRet = CImageProcess::DivisionTab_FromImageToTabInfo(pHeadPtr->m_pImagePtr, pTailPtr->m_pImagePtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo, nFrameCountL);

					//이미지를 합하여 Cell을 만들고 남은 픽셀 수
					unNotUseCellLength = RsvTabInfo.nImageLength;

					//Grab Call Time과 BCD ID를 백업한다.
					RsvTabInfo.m_GrabCallBCDId = pFrmInfo_Top->m_GrabCallBCDId;
					RsvTabInfo.m_GrabCallTime = pFrmInfo_Top->m_GrabCallTime;


					//Tab 정보 크기, Tab 정보가 없다면 에러처리
					int nVecSize = (int)vecTabInfo.size();


					BOOL bErrorAll = FALSE;
					if (nVecSize <= 0)
					{
						// 강제 분할 
						bErrorAll = TRUE;

						AprData.SaveDebugLog_Format(_T(">>>>>> Miss Tab Vec [T/B:%d][F/V:%d] : Size=%d, QueueCount=%d, FrameCountTop=%d, FrameCountBtm=%d "), 
							pFrmInfo_Top->m_nTabId_CntBoard, pFrmInfo_Top->nTabNo+1, nVecSize, pCntQueueInCtrl->GetSize(), pFrmInfo_Top->m_nFrameCount, pFrmInfo_Bottom->m_nFrameCount );
					}
					//nBneElectrodeBtm = CImageProcess::GetBoundaryOfElectordeBottom(pTailPtr->m_pImagePtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo); // Btm Edge 인식 함수 변경 : 해당 함수 사용 시 동일 조건임에도 이상 동작 발생. 원인 미상
					
					if (AprData.m_System.m_nMachineMode == CATHODE_MODE)
					{
						CImageProcess::FindTabLevel(pTailPtr->m_pImagePtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
						nBneElectrodeBtm = nBtmLevel;
					}
					else
					{
						nBneElectrodeBtm = CImageProcess::FindLevelBottom_Negative(pTailPtr->m_pImagePtr, nWidth, nHeight, AprData.m_pRecipeInfo, &nBtmLevel, CImageProcess::en_FindFromRight);
					}

					// PET Check BOTTOM
					CImageProcess::VEC_PET_INFO* pvstPetInfoBtm = new CImageProcess::VEC_PET_INFO;
					pvstPetInfoBtm->clear();

					BOOL bIsPET_Btm = CImageProcess::FindPetFilm(pTailPtr->m_pImagePtr, nWidth, nHeight, *AprData.m_pRecipeInfo, pvstPetInfoBtm, CAM_POS_BOTTOM);
					pFrmInfo_Bottom->m_bIsPET = bIsPET_Btm;

					//Tab Id Q Size 
					int TabQueueSize = pCntQueueInCtrl->GetSize();

					//Tab 정보 크기 만큼 루프 돌다.
					for (int idxi = 0; idxi < nVecSize; idxi++)
					{
//Encoder Counter 사용여부
#ifdef USE_BCDCOUNTER
						
						//실제 Tab Counter에 받은 마지막의 BCD ID
						UINT unRealLastBCDID = AprData.m_NowLotData.m_nLastBCDId;
#endif //USE_BCDCOUNTER

						//지금 Cell의 크기
						UINT unNowCellLength = 0;

						//Last Backup
						if ((AprData.m_NowLotData.m_nLastBCDId) >= 0 && (AprData.m_NowLotData.m_nLastBCDId < 64))
						{
							LastBCDIDBackup = AprData.m_NowLotData.m_nLastBCDId;
						}

						//다음에 사용될 BCD ID가 범위 밖을 경우 
						if ((AprData.m_NowLotData.m_nUseBCDID) < 0 || (AprData.m_NowLotData.m_nUseBCDID >= 64))
						{
							AprData.m_NowLotData.m_nUseBCDID = LastBCDIDBackup;
						}

						//Tab  정보 접근 임시 포인터 변수
						CTabInfo* pTabInfo = &vecTabInfo[idxi];

						//Tab Pitch 구하기
						double dTabPitch = 0.0;
						BOOL bErrorTabPitch = FALSE;
						//셀 크기 구하기
						double dCellLength = 0.0;

						//실제 텝의 넓이 구하기
						double dRrealTabWidth = 0.0;
						BOOL bErrorTabWidth = FALSE;

						//텝 피치 측정
						if((bforeImageLengtch > 0) && (bforeTabLeft > 0))
						{
							//min 72 max 112 범위의 피치를 벗어날 경우 

							//1.0 은 0으로 판단에 문제로 입력된 값
							if (AprData.m_System.m_dResolY1000P > 1.0)
							{
								dTabPitch = CImageProcThread::TabPitchCalculate(bforeImageLengtch, bforeTabLeft, pTabInfo->nTabLeft, AprData.m_System.m_dResolY1000P);
								dCellLength = pTabInfo->nImageLength * (AprData.m_System.m_dResolY1000P/1000.0);
							}
							else
							{
								dTabPitch = TabPitch(bforeImageLengtch, bforeTabLeft, pTabInfo->nTabLeft);
							}
							//Tab Id 정보 로그
							LOGDISPLAY_SPEC(7)("@@Cell Length<%f> Tab Pitch<%f> RecipeTabPitch<%f>@@@@ ", dCellLength, dTabPitch, RecipeInfoTabPitch);

							if (((RecipeInfoTabPitch - MIN_TABPITCH ) > dTabPitch) || (( RecipeInfoTabPitch + MAX_TABPITCH ) < dTabPitch))
							{
								//Trigger 에서 받아온 Tab Id 세팅하도록 한다.
								//Tab Id 정보 로그
								if (((RecipeInfoTabPitch - MIN_TABPITCH) > dTabPitch))
								{
									LOGDISPLAY_SPEC(7)("@@Tab Pitch가  작다@@@@ ");
								}
								else if (((RecipeInfoTabPitch + MAX_TABPITCH) < dTabPitch))
								{
									LOGDISPLAY_SPEC(7)("@@Tab Pitch가  크다@@@@ ");
								}
								bErrorTabPitch = TRUE;

								//메모리 로그 기록
								strMsg = "";
								strMsg.Format("Error == Tab Pitch Big or Small ========== === !!!  ");
								AprData.SaveMemoryLog(strMsg);
							}
						}

						//Cell의 Pitch를 구하기 위해서 cell 길아와 Tab의 길이를 백업한다.
						//지금 텝의 길이를 저장한다.
						bforeImageLengtch = pTabInfo->nImageLength;
						//지금 텝 왼쪽 길이를 저장한다.
						bforeTabLeft = pTabInfo->nTabLeft;

						//지금 셀의 크기
						unNowCellLength = pTabInfo->nImageLength;

						//지금 텝의 넓이를 구한다.
						//분해능 / 1000.0
						double dResolYLocal = (AprData.m_System.m_dResolY1000P / 1000);
						//지금 텝의 길이
						int nWidthLocal = abs(pTabInfo->nTabRight - pTabInfo->nTabLeft);
						//실제 텝의 길이 44.0 
						dRrealTabWidth = nWidthLocal * dResolYLocal;

						//Tab Id 정보 로그
						LOGDISPLAY_SPEC(7)("@@Tab Witch 픽셀<%d> - Recipe Width<%f>mm 실제 텝 넓이<%f>mm 분해능<%f>@@@@ ",  nWidthLocal, RecipeInfoTabWidth, dRrealTabWidth, AprData.m_System.m_dResolY1000P);


						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
					//SPC+ INSP===================================================================================================
					//SPC+ INSP 객체 생성
						CSpcInspManager* insp = new CSpcInspManager();

						//============================================================================================================
#endif //SPCPLUS_CREATE

						
						//컨테이너 정보 : 검사기 Tab 번호, Tab ID 받을 임시 객체
						CCounterInfo cntInfo;
						//전에 사용한 BCD ID 사용
						cntInfo.nTabID = nUseBCDIDBackup;
						//Tab Total Count MAX
						cntInfo.nTabIdTotalCount = MAX_INT;
						//TabNo Trigger 수신에서 가져온 값
						cntInfo.nTabNo = MAX_INT;

						//Encoder Count 값
						cntInfo.nEnCoderCount = 0;

						//BCD ID가 많이 남아 있을 경우 검사 진행 후 다음 BCD ID를 사용한다.
						//Grab 부여 BCD ID가 범위 안에 있고
						//이전 BCD ID 사용 여부
						bool bBforeUseBCDID = false;
						if ((pTabInfo->m_GrabCallBCDId >= 0) && (pTabInfo->m_GrabCallBCDId < 64))
						{
							//BCD Id Buffer 갯수가 얻은 Image Cell 갯수 보다 작고
							//BCD ID 이전 ID와 Grab Call BCD ID가 같으면 이전 BCD ID를 사용한다.
							if ((TabQueueSize <= (nVecSize - 1)) && (nUseBCDIDBackup == pTabInfo->m_GrabCallBCDId))
							{
								bBforeUseBCDID = true;
							}
						}

						//Tab Id 를 받은 것이 있다면
						if (TabQueueSize && (bBforeUseBCDID == false))
						{

							//Tab Id 정보 로그
							LOGDISPLAY_SPEC(7)("@@ Trigger에서 받은 BCD ID 사용 @@@@ ");

							//프로그램 실행 후 한번만 들어온다.
							if (bNowExecFlag)
							{
																
								//얻은 Tab 정보의 끝에서
								if (idxi == (nVecSize - 1))
								{
									//버퍼의 마지막 BCD ID 데이터를 가져온다.
									while (pCntQueueInCtrl->GetSize())
									{
										//정보를 하나 가지고 온다.
										cntInfo = pCntQueueInCtrl->Pop();
									}
								}
								else
								{
									//정보를 하나 가지고 온다.
									cntInfo = pCntQueueInCtrl->Pop();
								}
								
							}
							else
							{
								//정보를 하나 가지고 온다.
								cntInfo = pCntQueueInCtrl->Pop();
							}

							//BCD ID가 많이 남아 있을 경우 검사 진행 후 다음 BCD ID를 사용한다.
							if (TabQueueSize >= (nVecSize + 1))
							{
								//남은 이미지 셀이 0보다 크고
								if (unNotUseCellLength > 0)
								{
									if (unNotUseCellLengthBackup > unNotUseCellLength + 1000)
									{
										//Grab 부여 BCD ID가 범위 안에 있고
										if ((pTabInfo->m_GrabCallBCDId >= 0) && (pTabInfo->m_GrabCallBCDId < 64))
										{
											//Grab BCD ID와  쓰려고 하는 BCD ID가 다르다면 다음 BCD ID를 사용한다.
											if (cntInfo.nTabID != pTabInfo->m_GrabCallBCDId)
											{
												//Tab Id 정보 로그
												LOGDISPLAY_SPEC(11)("BCD ID delete	%d	===================", cntInfo.nTabID);
												//정보를 하나 가지고 온다.
												cntInfo = pCntQueueInCtrl->Pop();
											}
										}
									}
								}

								//BCD ID Buffer에 2개 이상의 ID가 있다면 지운다.
								if (pCntQueueInCtrl->GetSize() >= 2)
								{
									//Size 까지
									while (pCntQueueInCtrl->GetSize())
									{
										//다음 BCD ID 가져온다.
										cntInfo = pCntQueueInCtrl->Pop();
										//Grab 시 BCD ID 와 같으면 빠져 나가고 아니며 맨 마지막 것을 사용한다.
										if (cntInfo.nTabID == pTabInfo->m_GrabCallBCDId)
										{
											break;
										}
									}
								}

							}


							//사용한 BCD ID  백업
							nUseBCDIDBackup = cntInfo.nTabID;

							//Encoder Counter 사용여부
#ifdef USE_BCDCOUNTER
							//Encoder Counter 누적
							unTotalEncoderCount += cntInfo.nEnCoderCount;
#endif //USE_BCDCOUNTER

							//Tab Id 정보 로그
							LOGDISPLAY_SPEC(7)("@@ USE Tabid<%d>TabNo<%d> TotalCount<%d>@@@@ ",  cntInfo.nTabID, cntInfo.nTabNo + 1, cntInfo.nTabIdTotalCount);


						}
						else
						{
							//전에 사용한 BCD ID 사용
							cntInfo.nTabID = nUseBCDIDBackup;

							//Tab Id 정보 로그
							LOGDISPLAY_SPEC(7)("@@ 이전에  받은 BCD ID 사용 @@@@ ");

						}

						
						//Tab Id 정보 로그
						LOGDISPLAY_SPEC(7)("@@Tab Id Info@@@@  LotId<%s> Tab Id<%d> TabNo<%d><%d> TabTotalcnt<%d>",
							AprData.m_NowLotData.m_strLotNo, cntInfo.nTabID, cntInfo.nTabNo+1, AprData.m_NowLotData.m_nTabCount+1, cntInfo.nTabIdTotalCount);


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

							nErrorNo = 5;

							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> m_bErrorFlag = %d, nErrorNo = %d :: nLevel Error (nLevel:%d >= (nWidth:%d - 100 ))"),
								pTabInfo->m_bErrorFlag, nErrorNo, nLevel, nWidth);

						}
						// 22.09.30 Ahn Add End

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgCuttingTab> <CTabInfo> nVecSize:%d/%d, ImageLength:%d, FrameCount:%d, TabStartPosInFrame:%d, TabLeft:%d, TabRight:%d, nLevel:%d, nBtmLevel:%d"),
							idxi, nVecSize, pTabInfo->nImageLength, pTabInfo->nFrameCount, pTabInfo->nTabStartPosInFrame, pTabInfo->nTabLeft, pTabInfo->nTabRight, nLevel, nBtmLevel);


						//프레임 정보 임시 객체(Top 프레임 정보 처리)
						CFrameInfo* pInfo;
						pInfo = new CFrameInfo;

						//Tab Pitch 이상 여부
						pInfo->m_bErrorTabPitch = bErrorTabPitch;
						pInfo->m_dTabPitch = dTabPitch;

						//Tab Width 이상 여부
						pInfo->m_bErrorTabWitch = bErrorTabWidth;
						pInfo->m_dTabWidth = dRrealTabWidth;

						//Cell Pitch(mm)
						pInfo->m_unCellLength = pTabInfo->nImageLength;

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

						//메모리 로그 기록(TOP)
						strMsg = "";
						strMsg.Format(_T("TOP Insp Image Info FrameNum<%d>, TabNo<%d>, BCD ID<%d>"), pInfo->m_nFrameCount, pInfo->nTabNo+1, pInfo->m_nTabId_CntBoard);
						AprData.SaveMemoryLog(strMsg);


						//PET 가 인식 된 시점 부터 카운트 증가한다.
						if ((pTabInfo->m_bIsPET | pFrmInfo_Bottom->m_bIsPET))
						{
							//PET로 인식된 카운트를 올린다.
							nPETCount++;
							bPETBCDIdSet = FALSE;

							//메모리 로그 기록
							CString strMsg = "";
							strMsg.Format(_T("Error == PET Run Count<%d> ====== "), nPETCount);
							AprData.SaveMemoryLog(strMsg);
						}
						//PET RUN 이 끝나는 시점이거나 처음부터 PET가 아니거나
						else
						{
							//PET RUN이 1번 이상이고 PET BCD Id 초기화 변수가 FALSE 이면 초기화를 세팅한다.
							if ((nPETCount > 0) && (bPETBCDIdSet == FALSE))
							{
								//PET 시 Tab 카운트 출력
								LOGDISPLAY_SPEC(7)("@@ PET Run Count<%d> @@@@ ", nPETCount);

								//PET 초기화 카운트 초기화
								nPETCount = 0;
								//PET 초기화 변수 설정
								bPETBCDIdSet = TRUE;
							}
						}




						//프레임 정보 임시 객체(Bottom 프레임 정보 처리)
						CFrameInfo* pBtmInfo;
						pBtmInfo = new CFrameInfo;

						//Tab Pitch 이상 여부
						pBtmInfo->m_bErrorTabPitch = bErrorTabPitch;
						pBtmInfo->m_dTabPitch = dTabPitch;

						//Tab Width 이상 여부
						pBtmInfo->m_bErrorTabWitch = bErrorTabWidth;
						pBtmInfo->m_dTabWidth = dRrealTabWidth;

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

						//메모리 로그 기록(TOP)
						strMsg = "";
						strMsg.Format(_T("BOTTOM Insp Image Info FrameNum<%d>, TabNo<%d>, BCD ID<%d>"), pBtmInfo->m_nFrameCount, pBtmInfo->nTabNo+1, pBtmInfo->m_nTabId_CntBoard);
						AprData.SaveMemoryLog(strMsg);


						// 22.12.09 Ahn Add Start
						//프레임 처리 시간 세팅
						LARGE_INTEGER tmp;
						LARGE_INTEGER start;
						QueryPerformanceFrequency(&tmp);
						double dFrequency = (double)tmp.LowPart + ((double)tmp.HighPart * (double)0xffffffff);
						QueryPerformanceCounter(&start);

						double TabFind_TacTime = CGlobalFunc::GetDiffTime(start_TabFind, dFrequency_TabFind);
						//Image Cutting Tab 정보 출력 로그
						LOGDISPLAY_SPEC(4)("*TacTime - TabFind : TabID-[%d], TabNo-[%d], TacTime[%f]",
							pInfo->m_nTabId_CntBoard,  pInfo->nTabNo+1, TabFind_TacTime
							);

//Encoder Counter 사용여부
#ifdef USE_BCDCOUNTER
						//Tab Counter에서 받은 마지막 BCD ID
						int nLastInputBCDId = CCounterThread::GetInputReadId();

						//Tab Counter Log
						//Total Cell 길이 (누적 Cell 크기)
						unTotalCellLength += unNowCellLength;
						LOGDISPLAY_SPEC(11)(
							logStringEncoderCounter
							,pInfo->nTabNo + 1
							,unRealLastBCDID
							,cntInfo.nTabID
							,pTabInfo->m_GrabCallBCDId
							,pTabInfo->m_GrabCallTime
							,AprData.m_NowLotData.m_unGTotalEncoderCount
							,AprData.m_NowLotData.m_unGTotalImageCount
							,(AprData.m_NowLotData.m_unGTotalEncoderCount - AprData.m_NowLotData.m_unGTotalImageCount)
							,unTotalEncoderCount
							,unTotalImageCount
							,(unTotalEncoderCount - unTotalImageCount)
							,(unTotalEncoderCount - unTotalEncoderCountBackup)
							,unNowCellLength
							,unTotalCellLength
							,(unTotalImageCount - unTotalCellLength)
							,unNotUseCellLength
							,nLastInputBCDId
						);

						unTotalEncoderCountBackup = unTotalEncoderCount;

						CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
						if (pFrame)
						{
							pFrame->UpdateBCDIDData(cntInfo.nTabID, unRealLastBCDID, pTabInfo->m_GrabCallBCDId);
						}
#endif //USE_BCDCOUNTER

						//스래드에 처리할 정보를 저장 TOP, BOTTOM
						CImageProcessCtrl::GetThreadQueuePtr(CAM_POS_TOP)->push(pInfo);
						CImageProcessCtrl::GetThreadQueuePtr(CAM_POS_BOTTOM)->push(pBtmInfo);

						//Lot Data Tab 번호를 증가 시킨다.
						AprData.m_NowLotData.m_nTabCount++;

					}
					//처리한 Tab 정보를 삭제한다.
					vecTabInfo.clear();

					//이전 Cell 크기 백업한다.
					unNotUseCellLengthBackup = unNotUseCellLength;

				}

				//초기 실행 상태 플래그를 변경한다.
				//실행 후 한번만 들어오도록 설정
				bNowExecFlag = false;

				delete pFrmInfo_Top;
				pFrmInfo_Top = NULL;
				delete pFrmInfo_Bottom;
				pFrmInfo_Bottom = NULL;

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
		delete RsvTabInfo.pImgPtr;
	}
	if (RsvTabInfo.pImgBtmPtr != NULL) {
		delete RsvTabInfo.pImgBtmPtr;
	}

	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	//Tab Id 정보 로그
	LOGDISPLAY_SPEC(7)("@@ Thread **** CtrlThreadImgCuttingTab 종료됩니다 @@@@ ");

	return 0;
}



#define IMAGEPROCTHREAD_RESULT_TIMEOUT 10

#define IMAGEPROCTHREAD_INSPCOMPLATE_TIMEOUT 3

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
	//Key Id를 받았는지 확인
	BOOL bReciveKeyId = FALSE;

	UINT ret = 0;

	while (1)
	{
		
		//타임 주기 이벤트
		ret = WaitForSingleObject(pThis->getEvent_ImageProcThread_Result(), IMAGEPROCTHREAD_RESULT_TIMEOUT);

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> Invalid Handle") );

			break;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			//Key Id String 로컬 객체 초기 세팅
			CString strKeyIdString = _T("");
			//지멘스 Key Id 읽은 데이터
			int KeyIdSize = sizeof(AprData.m_NowLotData.m_ReadDataSms.wCell_KeyID);
			strKeyIdString = CStrSuport::byteToHexbyteValue((byte*)&AprData.m_NowLotData.m_ReadDataSms.wCell_KeyID[0], KeyIdSize);

			//초기값 세팅 후 변화가 일어나는지 확인
			if (CImageProcThread::gKeyIdString != strKeyIdString)
			{
				LOGDISPLAY_SPEC(2)("$$ PLC String KeyId Hex Data<%s>", strKeyIdString);
				//변화된 값을 저장한다.
				CImageProcThread::gKeyIdString = strKeyIdString;
				//키 초기값 세팅
				if ((bReciveKeyId == FALSE) && CImageProcThread::gKeyIdString != _T(""))
				{
					//Key Id를 받았는지 확인
					bReciveKeyId = TRUE;
				}

				//지멘스 Dummy 읽은 데이터
				int DummySize = sizeof(AprData.m_NowLotData.m_ReadDataSms.wCell_KeyID_Dummy);
				CString strDummyString = CStrSuport::byteToHexbyteValue((byte*)&AprData.m_NowLotData.m_ReadDataSms.wCell_KeyID_Dummy[0], DummySize);
				LOGDISPLAY_SPEC(2)("$$ PLC String Dummy Hex Data<%s>", strDummyString);
			}


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
					if (pUnitTop)
					{
						LOGDISPLAY_SPEC(8)("## TopQueueCtrl TopPtr Get pUnitTop");
					}
				}
				else
				{
					LOGDISPLAY_SPEC(8)("## TopQueueCtrl TopPtr Get NULL");
				}
			}
			if (pUnitBtm == NULL)
			{
				CThreadQueueCtrl* btmPtr = CImageProcessCtrl::GetThreadQueuePtr(CAM_POS_BOTTOM);
				if (btmPtr)
				{
					pUnitBtm = btmPtr->deQueue();
					if (pUnitBtm)
					{
						LOGDISPLAY_SPEC(8)("## BtmQueueCtrl BtmPtr Get pUnitBtm");
					}
				}
				else
				{
					LOGDISPLAY_SPEC(8)("## BtmQueueCtrl BtmPtr Get NULL");
				}
			}

			//Top, Bottom 처리 조건 : Defect 검사 프로세스가 처리 되었을 때
			//일정한 시간이 지나도 처리하지 못했을 때
			if (pUnitTop && pUnitBtm)
			{

				int topWaitVal = 0;
				int btmWaitVal = 0;

				int ProcEnd_WaitCountTop = 0;
				int ProcEnd_WaitCountBottom = 0;

				//로그 출력 -  Unit Loop count 10개
				while (1)
				{

					if (pThis->m_bKill == TRUE)
					{
						LOGDISPLAY_SPEC(8)(_T("## <CImageProcThread> TabNo<%d> CtrlThreadImgProc pThis->m_bKill == TRUE"), pUnitTop->m_pFrmInfo->nTabNo+1);

						break;
					}

					if (topWaitVal != 1)
					{
						topWaitVal = pUnitTop->eventProcEnd_WaitTime("TOP", ProcEnd_WaitCountTop);
					}
					if (btmWaitVal != 1)
					{
						btmWaitVal = pUnitBtm->eventProcEnd_WaitTime("BOTTOM", ProcEnd_WaitCountBottom);
					}

					if ((topWaitVal == 1) && (btmWaitVal == 1))
					{
						//CImageProcThreadUnit에서 결과 저장 객체를 가져온다.
						//CFrameRsltInfo의 생성을 CImageProcThreadUnit 생성자에서 생성하도록 수정됨
						CFrameRsltInfo* pTopInfo = pUnitTop->GetResultPtr();
						CFrameRsltInfo* pBtmInfo = pUnitBtm->GetResultPtr();

						LOGDISPLAY_SPEC(8)("## <<%s>>>CtrlThreadImgProc TabNo<%d>-TabId<%d> - ResultProcWait-Enter LoopCount<%d>",
							"Top", pTopInfo->nTabNo+1, pTopInfo->m_nTabId_CntBoard, ProcEnd_WaitCountTop);
						LOGDISPLAY_SPEC(8)("## <<%s>>>CtrlThreadImgProc TabNo<%d>-TabId<%d> - ResultProcWait-Enter",
							"Btm", pBtmInfo->nTabNo+1, pBtmInfo->m_nTabId_CntBoard, ProcEnd_WaitCountBottom);

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> ResultProc Enter"), pTopInfo->nTabNo+1);


						CString strMsg;
						strMsg.Format(_T("LotId<%s> TabNo<%d> TabId<%d> JUDGE : Top-<%s> Bottom-<%s>")
							, AprData.m_NowLotData.m_strLotNo
							,pTopInfo->nTabNo + 1
							,pTopInfo->m_nTabId_CntBoard
							,(pTopInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) ? "NG" : "OK"
							,(pBtmInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) ? "NG" : "OK");
						AprData.SaveMemoryLog(strMsg);

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

						//외관 판정 시 미응답에 의한 NG 발생시 처리 부분
						InspInData->setDetectFallFlact(pTopInfo->m_bErrorFlag == TRUE ? "TRUE" : "FALSE");
						InspInData->setDetectFallReasoAll(pTopInfo->m_bErrorFlag == TRUE ? "Foil-Inner Exposure, Foil-Outer Exposure, Foil-exposure" : "Null");

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

#endif //SPCPLUS_CREATE

						int nBtmJudge = pBtmInfo->m_pTabRsltInfo->m_nJudge;
						int nTopJudge = pTopInfo->m_pTabRsltInfo->m_nJudge;

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

										//if( (nId >= 0) && (nId < 64) )
										//{
										//	AprData.m_NowLotData.m_SeqDataOutSms.wDuplicateNG_Cell_ID[nId] = (nTopJudge == JUDGE_NG) ? 1 : 0;

										//	AprData.FileCtrl_DuplicateNG(MODE_WRITE, nId, AprData.m_NowLotData.m_SeqDataOutSms.wDuplicateNG_Cell_ID[nId]);
										//}
									}
									else
									{
										pSigProc->SigOutAlarmExist(TRUE);
										pSigProc->WriteAlarmCode(wAlarmCode);
									}

								}
							}
						}
						else
						{

							AprData.m_NowLotData.m_nTabCountOK++;
							AprData.m_NowLotData.m_nContinueCount = 0; // 22.08.09 Ahn Add
							AprData.m_NowLotData.m_secNgJudge.AddOkTab(pTopInfo->nTabNo, AprData.m_nSectorBaseCount/*AprData.m_pRecipeInfo->nSectorCount*/);

						}
						// 결과 Queue에 보냄

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Write NG Code To PLC"), pTopInfo->nTabNo+1);



						// Counter 신호 출력
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

							//마킹 정보를 세팅한다.
							//input id 스래드에서 보낸다.
							CCounterThread::MarkSendInfo_Push_back(pTopInfo->m_nTabId_CntBoard, wOutPut);

							CString strMsg;
							strMsg.Format(_T("Output Make BCD ID[%d]_OutPutValue[0x%x]_TabNo[%d]"),
								pTopInfo->m_nTabId_CntBoard, wOutPut, pTopInfo->nTabNo + 1);
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

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Write Judge To PLC"), pTopInfo->nTabNo+1);



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

							//통합비전 BCD Id에 대한 Key 번호
							//PLC에서 받은 데이터가 있는가 확인 후 세팅한다.
							WORD nKeyId = -1;
							if (bReciveKeyId)
							{
								//BCD ID 범위 확인
								if (pTopInfo->m_nTabId_CntBoard >= 0 && pTopInfo->m_nTabId_CntBoard <= 63)
								{
									nKeyId = AprData.m_NowLotData.m_ReadDataSms.wCell_KeyID[pTopInfo->m_nTabId_CntBoard];
									LOGDISPLAY_SPEC(2)("$$ PLC Key Id<%d> TabNo<%d> BCD Id<%d>", nKeyId, pTopInfo->nTabNo + 1, pTopInfo->m_nTabId_CntBoard);
								}
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
							strResult.Format(_T("%s,%d,%d,%d,%s,%s,%s,%s,%d,%d,%.2lf,%.2lf,%s,%s,%s,%s,%.2lf,%s,%.2lf,%d\r\n")
								// 23.01.06 Ahn Modify End
								, AprData.m_NowLotData.m_strLotNo
								, pTopInfo->nTabNo + 1
								, pTopInfo->m_nTabId_CntBoard
								, nKeyId
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
								, (pTopInfo->m_pTabRsltInfo->m_bIsPET == TRUE) ? _T("PET") : _T("Foil")
								, (pTopInfo->m_bErrorTabPitch == TRUE) ? _T("X") : _T("OK")
								, pTopInfo->m_dTabPitch
								, (pTopInfo->m_bErrorTabWitch == TRUE) ? _T("X") : _T("OK")
								, pTopInfo->m_dTabWidth
								, pTopInfo->m_unCellLength
							);
							int nRet = CWin32File::TextSave1Line(strFilePath, strCsvFileName, strResult, _T("at"), FALSE);


							strResult.TrimRight();
							AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Write CSV(%d) [%s%s][%s]"),
								pTopInfo->nTabNo+1, nRet, strFilePath, strCsvFileName, strResult );
						}



						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
						//SPC+ IMAGE Save===================================================================================================
						CString strSPCFilePath = "";
						InspInData->ImageFilePath(strSPCFilePath);
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

							if (nImgSize > 0)
							{
								//SPC+ 전송용 이미지 파일 저장 정보
								if (CSpcInfo::Inst()->getSPCStartFlag())
								{
									CImgSaveInfo* pSaveInfo = new CImgSaveInfo;
									BYTE* pImgSavePtr;
									pImgSavePtr = new BYTE[nImgSize + 1];
									memset(pImgSavePtr, 0x00, sizeof(BYTE) * nImgSize + 1);
									memcpy(pImgSavePtr, pFrmRsltInfo->GetImagePtr()->m_pImagePtr, sizeof(BYTE) * nImgSize);
									//퀄리티 정보를  세팅한다.
									pSaveInfo->SetImgPtr(pImgSavePtr, pFrmRsltInfo->m_nWidth, pFrmRsltInfo->m_nHeight, SPCImageQuality);
									pSaveInfo->m_strSavePath.Format(_T("%s\\%s"), strSPCFilePath, SPCImageFileName);
									pImgSaveQueueCtrl->PushBack(pSaveInfo);
								}

								//기존 이미지 저장정보
								if ((pFrmRsltInfo->m_bSaveFlag) && (pFrmRsltInfo->m_pTabRsltInfo->m_bImageFlag == TRUE))
								{
									CImgSaveInfo* pSaveInfo = new CImgSaveInfo;
									BYTE* pImgSavePtr;
									pImgSavePtr = new BYTE[nImgSize + 1];
									memset(pImgSavePtr, 0x00, sizeof(BYTE) * nImgSize + 1);
									memcpy(pImgSavePtr, pFrmRsltInfo->GetImagePtr()->m_pImagePtr, sizeof(BYTE) * nImgSize);
									pSaveInfo->SetImgPtr(pImgSavePtr, pFrmRsltInfo->m_nWidth, pFrmRsltInfo->m_nHeight);
									pSaveInfo->m_strSavePath.Format(_T("%s\\%s"), pFrmRsltInfo->m_pTabRsltInfo->m_chImagePath, pFrmRsltInfo->m_pTabRsltInfo->m_chImageFile);
									pImgSaveQueueCtrl->PushBack(pSaveInfo);

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
								if (pFrmRsltInfo->GetImagePtr())
								{
									pFrmRsltInfo->m_nWidth;
									CImgSaveInfo* pSaveInfo = new CImgSaveInfo;
									BYTE* pImgSavePtr;
									pImgSavePtr = new BYTE[nImgSize + 1];
									memset(pImgSavePtr, 0x00, sizeof(BYTE) * nImgSize + 1);
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

							}
						}
#endif //SPCPLUS_CREATE
						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- Save Image PushBack"), pTopInfo->nTabNo);


						int tempTabNo = pTopInfo->nTabNo;

						CImageProcessCtrl::GetResultPtr(CAM_POS_TOP)->PushBack((CFrameInfo*)pTopInfo);
						CImageProcessCtrl::GetResultPtr(CAM_POS_BOTTOM)->PushBack((CFrameInfo*)pBtmInfo);

						AprData.SaveDebugLog_Format(_T("<CtrlThreadImgProc> TabNo<%d> --- pRsltQueueCtrl->PushBack()"), tempTabNo+1);


						AprData.m_NowLotData.m_ctLastAcqTime = CTime::GetCurrentTime();

						if (bClearFlag == TRUE)
						{

							AprData.SaveDebugLog_Format(_T("------ NG_STOP Signal OFF ------"));
							bJudgeNG = FALSE;
							bClearFlag = FALSE;// 22.03.28 Ahn Add
						}

						break;
					}
					else if ((topWaitVal == 2) || (btmWaitVal == 2))
					{
						LOGDISPLAY_SPEC(8)("## ==== UNIT LOOPPING ERROR  ==== Top<%d> Bottom<%d>", topWaitVal, btmWaitVal);

						CString ErrorLog;
						ErrorLog.Format(_T("============================= UNIT LOOPPING ERROR  ============================="));
						AprData.SaveErrorLog(ErrorLog);

						//CImageProcThreadUnit에서 결과 저장 객체를 가져온다.
						//CFrameRsltInfo의 생성을 CImageProcThreadUnit 생성자에서 생성하도록 수정됨
						CFrameRsltInfo* pTopInfo = NULL;
						if(pUnitTop)
							pTopInfo = pUnitTop->GetResultPtr();
						CFrameRsltInfo* pBtmInfo = NULL;
						if(pUnitBtm)
							pBtmInfo = pUnitBtm->GetResultPtr();

						if (pTopInfo && pBtmInfo)
						{
							LOGDISPLAY_SPEC(8)("## === UNIT LOOPPING ERROR === Info Print Start ==  ");
							CString strCsvFileName;
							CString strFilePath;
							strFilePath.Format(_T("%s\\"), AprData.m_strNowCsvPath);

							strCsvFileName.Format(_T("%s.csv"), AprData.m_NowLotData.m_strLotNo);

							int nMarkSel1 = 0;
							int nMarkSel2 = 0;

							WORD wOutPut = CImageProcThread::GetCounterSignal(pTopInfo->m_nTabId_CntBoard, JUDGE_NG, JUDGE_NG, nMarkSel1, nMarkSel2);

							CString strMarking = _T("△");
							CString strMarkReason = _T("");

							CString strTime;
							CString strJudge = _T("OK");
							CString strBtmJudge = _T("OK");
							CString strTopJudge = _T("OK");

							int nSurfaceNgCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][RANK_NG] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][JUDGE_NG];
							int nFoilExpNgCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][RANK_NG] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][JUDGE_NG]
								+ pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][RANK_NG] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][JUDGE_NG];

							double 	dTopMaxSize = pTopInfo->m_pTabRsltInfo->m_dMaxSizeDef;
							double 	dBtmMaxSize = pBtmInfo->m_pTabRsltInfo->m_dMaxSizeDef;

							SYSTEMTIME* pSysTime;
							pSysTime = &(pTopInfo->m_pTabRsltInfo->sysTime);

							strTime.Format(_T("%02d:%02d:%02d(%03d)"), pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond, pSysTime->wMilliseconds);

							//통합비전 BCD Id에 대한 Key 번호
							//PLC에서 받은 데이터가 있는가 확인 후 세팅한다.
							WORD nKeyId = -1;
							if (bReciveKeyId)
							{
								//BCD ID 범위 확인
								if (pTopInfo->m_nTabId_CntBoard >= 0 && pTopInfo->m_nTabId_CntBoard <= 63)
								{
									nKeyId = AprData.m_NowLotData.m_ReadDataSms.wCell_KeyID[pTopInfo->m_nTabId_CntBoard];
									LOGDISPLAY_SPEC(2)("$$ PLC Key Id<%d> TabNo<%d> BCD Id<%d>", nKeyId, pTopInfo->nTabNo + 1, pTopInfo->m_nTabId_CntBoard);
								}
							}

							//피더파일에 기록하기
							CString strResult = _T("");
							strResult.Format(_T("%s,%d,%d,%d,%s,%s,%s,%s,%d,%d,%.2lf,%.2lf,%s,%s,%s\r\n")
								, AprData.m_NowLotData.m_strLotNo
								, pTopInfo->nTabNo + 1
								, pTopInfo->m_nTabId_CntBoard
								, nKeyId
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
								, _T("OVER")
							);
							int nRet = CWin32File::TextSave1Line(strFilePath, strCsvFileName, strResult, _T("at"), FALSE);


							CString strTimeImage;
							strTimeImage.Format(_T("%02d_%02d_%02d_%03d"), pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond, pSysTime->wMilliseconds);

							//이미지 저장 포맷
							CString strImageFormat = AprData.getGSt()->GetOutImageFormat();

							//Top Image 저장
							if (pTopInfo->GetImagePtr())
							{
								int nImgSizeTop = pTopInfo->m_nWidth * pTopInfo->m_nHeight;
								//ImageProc: 이미지저장명 결정 생성
								CString strFileNameTop = _T("");
								strFileNameTop.Format(_T("%s_%s_%s_%s_%d_%s_%s%s")
									, INSPECTION_TYPE
									, strTimeImage
									, AprData.m_System.m_strMachineID
									, AprData.m_NowLotData.m_strLotNo
									, pTopInfo->nTabNo + 1
									, (pTopInfo->m_nHeadNo == CAM_POS_TOP) ? _T("TAB") : _T("BTM")
									, _T("OVER")
									, strImageFormat
								);

								CImgSaveInfo* pSaveInfoTop = new CImgSaveInfo;
								BYTE* pImgSavePtrTop;
								pImgSavePtrTop = new BYTE[nImgSizeTop + 1];
								memset(pImgSavePtrTop, 0x00, sizeof(BYTE) * nImgSizeTop + 1);
								memcpy(pImgSavePtrTop, pTopInfo->GetImagePtr()->m_pImagePtr, sizeof(BYTE) * nImgSizeTop);
								pSaveInfoTop->SetImgPtr(pImgSavePtrTop, pTopInfo->m_nWidth, pTopInfo->m_nHeight);
								pSaveInfoTop->m_strSavePath.Format(_T("%s\\%s"), AprData.m_strNowOkPath, strFileNameTop);
								pImgSaveQueueCtrl->PushBack(pSaveInfoTop);
							}

							//Bottom Image 저장
							if (pBtmInfo->GetImagePtr())
							{
								int nImgSizeBottom = pBtmInfo->m_nWidth * pBtmInfo->m_nHeight;
								//ImageProc: 이미지저장명 결정 생성
								CString strFileNameBottom = _T("");
								strFileNameBottom.Format(_T("%s_%s_%s_%s_%d_%s_%s%s")
									, INSPECTION_TYPE
									, strTimeImage
									, AprData.m_System.m_strMachineID
									, AprData.m_NowLotData.m_strLotNo
									, pBtmInfo->nTabNo + 1
									, (pBtmInfo->m_nHeadNo == CAM_POS_TOP) ? _T("TAB") : _T("BTM")
									, _T("OVER")
									, strImageFormat
								);

								CImgSaveInfo* pSaveInfoBottom = new CImgSaveInfo;
								BYTE* pImgSavePtrBottom;
								pImgSavePtrBottom = new BYTE[nImgSizeBottom + 1];
								memset(pImgSavePtrBottom, 0x00, sizeof(BYTE) * nImgSizeBottom + 1);
								memcpy(pImgSavePtrBottom, pBtmInfo->GetImagePtr()->m_pImagePtr, sizeof(BYTE) * nImgSizeBottom);
								pSaveInfoBottom->SetImgPtr(pImgSavePtrBottom, pBtmInfo->m_nWidth, pBtmInfo->m_nHeight);
								pSaveInfoBottom->m_strSavePath.Format(_T("%s\\%s"), AprData.m_strNowOkPath, strFileNameBottom);
								pImgSaveQueueCtrl->PushBack(pSaveInfoBottom);
							}

							LOGDISPLAY_SPEC(8)("## === UNIT LOOPPING ERROR === Info Print End ==  ");
						}

						break;
					}
					else
					{
						WaitForSingleObject(pThis->getEvent_ImageProcThread_InspComplate(), IMAGEPROCTHREAD_INSPCOMPLATE_TIMEOUT);
					}

				}

				//객체 확인
				if (pUnitTop)
				{
					//Unit Thread 에서 빠져나오지 못했을 때 체크
					if (topWaitVal == 1)
					{
						delete pUnitTop;
					}
					pUnitTop = NULL;
				}

				//객체 확인
				if (pUnitBtm)
				{
					//Unit Thread 에서 빠져나오지 못했을 때 체크
					if (btmWaitVal == 1)
					{
						delete pUnitBtm;
					}
					pUnitBtm = NULL;
				}

			}

		}
		else
		{
			break;
		}
	}

	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	//Tab Id 정보 로그
	LOGDISPLAY_SPEC(7)("@@ Thread **** CtrlThreadImgProc 종료됩니다 @@@@ ");

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
