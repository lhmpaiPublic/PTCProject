#include "pch.h"
#include "ImageProcessCtrl.h"
#include "FrameInfo.h"
#include "QueueCtrl.h"
#include "ImageProcThread.h"
#include "GrabDalsaCameraLink.h"
#include "CResultThread.h"
#include "BitmapStd.h"
#include "GlobalData.h"
#include "Win32File.h"
#include "CCounterThread.h"
#include "CCounterQueueCtrl.h"

#include "NotchingGradeInsp.h"
#include "CLightControl.h"
#include "CRecipeCtrl.h"

#include "CThreadQueueCtrl.h"
// 22.05.26 Ahn Add Start
#include "NotchingGradeInsp.h"
#include "SigProc.h"
// 22.05.26 Ahn Add End
// 22.05.31 Ahn Add Start
#include "CImageSaveQueueCtrl.h"
#include "CImageSaveThread.h"
// 22.05.31 Ahn Add ENd
// 22.06.10 Ahn Add Start
#include "CCropImgQueueCtrl.h"
// 22.06.10 Ahn Add End
// 22.12.09 Ahn Add Start
#include "CTacTimeDataCtrl.h"
// 22.12.09 Ahn Add End

#define MAX_PROC_THREAD 4
#define THREAD_MODE_CUT_TAB 0 
#define THREAD_MODE_PROC 1

CThreadQueueCtrl* CImageProcessCtrl::m_pThreadQueueCtrl[GRABBER_COUNT];

CQueueCtrl* CImageProcessCtrl::m_pQueueResult[GRABBER_COUNT];

CImageProcessCtrl::CImageProcessCtrl(void)
{
	int i;
	m_bInspFlag = FALSE;
	for ( i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i] = NULL;

		m_pQueueFrmCtrl[i] = new CQueueCtrl();
		m_pQueueFrmCtrl[i]->ResetQueue();
	}

	m_pQueueCtrl = new CQueueCtrl();
	m_pQueueCtrl->ResetQueue();

	for ( i = 0; i < GRABBER_COUNT; i++) {
		m_pQueueResult[i] = new CQueueCtrl();
		m_pQueueResult[i]->ResetQueue();
	}

	//22.05.31 Ahn Add Start
	m_pImgSaveQueueCtrl = new CImageSaveQueueCtrl();
	m_pImgSaveQueueCtrl->ResetQueue();
	//22.05.31 Ahn Add Start


	// 21.11.11 Ahn Add Start
	//grabber 갯수 만큼 스래드큐를 제어할 객체를 생성한다.
	for (i = 0; i < GRABBER_COUNT; i++) {
		m_pThreadQueueCtrl[i] = new  CThreadQueueCtrl(this);
	}
	// 21.11.11 Ahn Add End

	// 22.06.10 Ahn Add Start
	m_pCropImgQueueCtrl = new CCropImgQueueCtrl();
	m_pCropImgQueueCtrl->ResetQueue();
	// 22.06.10 Ahn Add End

	// 22.06.23 Ahn Add Start
	m_pDefectQueueCtrl = new CDefectQueueCtrl();
	m_pDefectQueueCtrl->ResetQueue();
	// 22.06.23 Ahn Add End

	// 22.12.09 Ahn Add Start
	m_pTacTimeDataCtrl = new CTacTimeDataCtrl();
	m_pTacTimeDataCtrl->ResetQueue();
	// 22.12.09 Ahn Add End

	// 21.11.11 Ahn Delete Start
	//for (i = 0; i < GRABBER_COUNT; i++) {
	//	m_pQueueCounter[i] = new CCounterQueueCtrl();
	//	m_pQueueCounter[i]->ResetQueue();
	//}
	// 21.11.11 Ahn Delete End
	m_pQueueCounterIn = new CCounterQueueCtrl();
	m_pQueueCounterIn->ResetQueue();

	//이미지 Cut 스래드 객체 생성
	m_pImgCutTabThread = new CImageProcThread(this);
	if (m_pImgCutTabThread != NULL) {
		m_pImgCutTabThread->Begin(THREAD_MODE_CUT_TAB);
	}	

	// 21.11.11 Ahn Delete Start
	//CImageProcThread* pThread;
	//for (i = 0; i < MAX_PROC_THREAD; i++) {
	//	pThread = new CImageProcThread(this);
	//	m_vecThread.push_back(pThread);
	//	pThread->Begin(THREAD_MODE_PROC);
	//}
	
	//이미지 처리 스래드 (대기 스래드)
	//출력 대기 이벤트 객체 동기화 객체 초기화
	::InitializeCriticalSection(&m_csImgProcWaitThread_Event);
	//스래드 활성화 갯수 초기화 0
	m_nImgProcWaitThread_ActiveCount = 0;

	//이미지 처리 대기 스래드 객체 생성
	for (int i = 0; i < MAX_WAITETHREAD_MAKE; i++)
	{
		m_pImgProcWaitThread[i] = new CImageProcThread(this);
		m_pImgProcWaitThread[i]->Begin(THREAD_MODE_PROC);
	}
	// 21.11.11 Ahn Delete End

	m_DefDataCtrl.RemoveAll();

	for (i = 0; i < GRABBER_COUNT; i++) {
		m_pResultThread[i] = new CResultThread(this, i);
		m_pResultThread[i]->Begin();
		m_pResultThread[i]->SetDefectDataCtrl(&m_DefDataCtrl);
	}

	m_pCounterThread = new CCounterThread(this);
	m_pCounterThread->Begin();

	// 22.05.31 Ahn Add Start - Image Save Thread
	// 22.07.08 Son Mod Start
	//for (int i = 0; i < GRABBER_COUNT; i++) {
	for (int i = 0; i < IMAGE_SAVE_THREAD; i++) {
	// 22.07.08 Son Mod End
		m_pImgSaveThread[i] = new CImageSaveThread( i, this);
		if (m_pImgSaveThread[i] != NULL) {
			m_pImgSaveThread[i]->Begin();
		}
	}
	// 22.05.31 Ahn Add Start

	Top_FrameCtn = -1;
	Bottom_FrameCtn = -1;

#if defined( IMAGE_DRAW_NOTIFY_VERSION )

	for (i = 0; i < GRABBER_COUNT; i++) {
		m_pLastBmpStd[i] = new CBitmapStd(DEFAULT_IMG_SIZE_W, DEFAULT_IMG_SIZE_H, 8);
		::InitializeCriticalSection(&m_csLastImg[i]);
	}
#endif
}

//이미지 처리 스래드 (대기 스래드)
//출력 대기 이벤트 객체 push
void CImageProcessCtrl::ImgProcWaitThread_Event_push(HANDLE hEvent)
{
	//출력 대기 이벤트 객체 동기화 객체 진입
	::EnterCriticalSection(&m_csImgProcWaitThread_Event);
	if (m_nImgProcWaitThread_ActiveCount != 0)
	{
		//출력 대기 이벤트 객체
		m_pImgProcWaitThread_Event.push(hEvent);
	}
	else
	{
		SetEvent(hEvent);
	}
	//갯수 증가
	m_nImgProcWaitThread_ActiveCount++;
	if (m_nImgProcWaitThread_ActiveCount > MAX_WAITETHREAD_MAKE)
		m_nImgProcWaitThread_ActiveCount = MAX_WAITETHREAD_MAKE;
	//출력 대기 이벤트 객체 동기화 객체 통과
	::LeaveCriticalSection(&m_csImgProcWaitThread_Event);
}
//출력 대기 이벤트 객체 pop
void CImageProcessCtrl::ImgProcWaitThread_Event_pop()
{
	//출력 대기 이벤트 객체 동기화 객체 진입
	::EnterCriticalSection(&m_csImgProcWaitThread_Event);
	//출력 대기 이벤트 객체
	if (m_pImgProcWaitThread_Event.size())
	{
		HANDLE hEvent = m_pImgProcWaitThread_Event.front();
		m_pImgProcWaitThread_Event.pop();
		if (hEvent)
		{
			SetEvent(hEvent);
		}
	}
	//갯수 감소
	m_nImgProcWaitThread_ActiveCount--;
	if (m_nImgProcWaitThread_ActiveCount < 0)
		m_nImgProcWaitThread_ActiveCount = 0;
	//출력 대기 이벤트 객체 동기화 객체 통과
	::LeaveCriticalSection(&m_csImgProcWaitThread_Event);
}

// 22.05.09 Ahn Add Start
int CImageProcessCtrl::ResetCamera()
{
	int nRet = 0; 
	//Grabber 갯수 만큼 돌면서객체를 삭제
	//Grabber 객체 삭제
	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pGrabCtrl[i] != NULL) {
			m_pGrabCtrl[i]->Close();
			delete m_pGrabCtrl[i];
			m_pGrabCtrl[i] = NULL;
		}
	}

	// 이미지 Cut 스래드 객체 종료 후 다시 생성 카메라가 Reset 될 경우
	if (m_pImgCutTabThread != NULL) {
		//이미지 Cut 스래드 Kill
		m_pImgCutTabThread->Kill();
		delete m_pImgCutTabThread;

		//이미지 Cut 객체 재 생성
		m_pImgCutTabThread = new CImageProcThread(this);
		if (m_pImgCutTabThread != NULL) {
			m_pImgCutTabThread->Begin(THREAD_MODE_CUT_TAB);
		}
	}

	//Grabber 숫자 만큼 Frame 큐를 Reset
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pQueueFrmCtrl[i]->ResetQueue();	// 22.05.19 Ahn Add 

		//Grabber 객체를 생성 하고  Dalsa 카메라 연결
		nRet |= Initialize(NULL, i);
	}
	return nRet;
}
// 22.05.09 Ahn Add End

// 22.12.01 Ahn Modify Start
void CImageProcessCtrl::SetFrameCount(long lFrameCount)
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pGrabCtrl[i] != NULL) {
			m_pGrabCtrl[i]->SetFrameCount(lFrameCount);
		}
	}
}
// 22.12.01 Ahn Modify End


//// 22.05.25 Ahn Add Start
//int CImageProcessCtrl::CameraResetForMonitorDlg()
//{
//	int nRet = 0; 
//	for (int i = 0; i < GRABBER_COUNT; i++) {
//		if (m_pGrabCtrl[i] != NULL) {
//			m_pGrabCtrl[i]->Close();
//			delete m_pGrabCtrl[i];
//			m_pGrabCtrl[i] = NULL;
//		}
//	}
//
//	for (int i = 0; i < GRABBER_COUNT; i++) {
//		m_pQueueFrmCtrl[i]->ResetQueue();	// 22.05.19 Ahn Add 
//		nRet |= Initialize(NULL, i);
//	}
//	return nRet ;
//}
//// 22.05.25 Ahn Add End


int CImageProcessCtrl::Initialize(HWND hWnd, int nIndex )
{
	//초기화 시 Dalsa Grabber  연결 클래스 생성
	// 카메라 장비 이벤트 연결하여 이미지를 받기
	//nIndex 0 : Top , 1 : Bottom
	if (m_pGrabCtrl[ nIndex ] == NULL) {
		m_pGrabCtrl[nIndex] = new CGrabDalsaCameraLink(this);
	}
	
	//이미지 인텍스
	m_nImgProcIdx[nIndex] = nIndex;

	//Grabber index 0 Top 열기, 1 Bottom  열기
	//메모리 할당된 CQueueCtrl 포인터 객체을 변수로 넘긴다.
	m_pGrabCtrl[nIndex]->Open( hWnd, m_pQueueFrmCtrl[ nIndex ], nIndex );

	// Check SerialNo And Swap 
	//if( AprData.m_System.m_nCamViewHeight)

	return 0;
}

int CImageProcessCtrl::Destroy()
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->Close();
		delete m_pGrabCtrl[i];
		m_pGrabCtrl[i] = NULL;
	}

	//이미지 Cut 스래드 살아 있다면 Kill 후 개게 삭제
	if (m_pImgCutTabThread != NULL) {
		m_pImgCutTabThread->Kill();
		delete m_pImgCutTabThread;
		m_pImgCutTabThread = NULL;
	}

	int i = 0;
	// 21.11.11 Ahn Delete Start
	//CImageProcThread* pThread;
	//while (m_vecThread.size() > 0) {
	//	pThread = m_vecThread[0];
	//	pThread->Kill();
	//	delete pThread;
	//	pThread = NULL;
	//	m_vecThread.erase(m_vecThread.begin());
	//}

	//이미지 처리 스래드 (대기 스래드)
	//출력 대기 이벤트 객체 동기화 객체 종료
	::DeleteCriticalSection(&m_csImgProcWaitThread_Event);
	//스래드 활성화 갯수 초기화 0
	m_nImgProcWaitThread_ActiveCount = 0;
	//이미지 처리 대기 스래드
	for (int i = 0; i < MAX_WAITETHREAD_MAKE; i++)
	{
		if (m_pImgProcWaitThread[i] != NULL) {
			m_pImgProcWaitThread[i]->Kill();
			delete m_pImgProcWaitThread[i];
			m_pImgProcWaitThread[i] = NULL;
		}
	}
	// 21.11.11 Ahn Delete End

	if (m_pCounterThread != NULL) {
		m_pCounterThread->Kill();
		delete m_pCounterThread;
		m_pCounterThread = NULL;
	}

	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pResultThread[i] != NULL) {
			m_pResultThread[i]->Kill();
			delete m_pResultThread[i];
			m_pResultThread[i] = NULL;
		}
	}

	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pQueueFrmCtrl[i] != NULL) {
			delete m_pQueueFrmCtrl[i];
			m_pQueueFrmCtrl[i] = NULL;
		}
	}

	// 21.11.11 Ahn Add Start
	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pThreadQueueCtrl[i] != NULL) {
			delete m_pThreadQueueCtrl[i];
			m_pThreadQueueCtrl[i] = NULL;
		}
	}
	// 21.11.11 Ahn Add End

	// 22.05.31 Ahn Add Start
	// 22.07.08 Son Mod Start
	//for (int i = 0; i < GRABBER_COUNT; i++) {
	for (int i = 0; i < IMAGE_SAVE_THREAD; i++) {
	// 22.07.08 Son Mod End
		if (m_pImgSaveThread[i] != NULL) {
			m_pImgSaveThread[i]->Kill();
			delete m_pImgSaveThread[i];
			m_pImgSaveThread[i] = NULL;
		}
	}
	// 22.05.31 Ahn Add End


	if (m_pQueueCtrl != NULL) {
		delete m_pQueueCtrl;
		m_pQueueCtrl = NULL;
	}

	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pQueueResult[i] != NULL) {
			delete m_pQueueResult[i];
			m_pQueueResult[i] = NULL;
		}
	}

	if (m_pQueueCounterIn != NULL) {
		delete m_pQueueCounterIn;
		m_pQueueCounterIn = NULL;
	}

	// 22.05.31 Ahn Add Start
	if (m_pImgSaveQueueCtrl != NULL) {
		delete m_pImgSaveQueueCtrl;
		m_pImgSaveQueueCtrl = NULL;
	}
	// 22.05.31 Ahn Add End

	// 22.06.10 Ahn Add Start
	if (m_pCropImgQueueCtrl != NULL) {
		delete m_pCropImgQueueCtrl;
		m_pCropImgQueueCtrl = NULL;
	}
	// 22.06.10 Ahn Add End
	// 22.06.23 Ahn Add Start
	if (m_pDefectQueueCtrl != NULL) {
		delete m_pDefectQueueCtrl;
		m_pDefectQueueCtrl = NULL;
	}
	// 22.06.23 Ahn Add End

	m_DefDataCtrl.RemoveAll();

	// 22.12.09 Ahn Add Start
	if (m_pTacTimeDataCtrl != NULL) {
		delete m_pTacTimeDataCtrl ;
		m_pTacTimeDataCtrl = NULL ;
	}
	// 22.12.09 Ahn Add End

#if defined( IMAGE_DRAW_NOTIFY_VERSION )

	for (i = 0; i < GRABBER_COUNT; i++) {
		if(m_pLastBmpStd[i] != NULL){\
			delete m_pLastBmpStd[i] ;
		m_pLastBmpStd[i] = NULL;

		}
		::DeleteCriticalSection(&m_csLastImg[i]);
	}
#endif
	return 0;
}


CImageProcessCtrl::~CImageProcessCtrl(void)
{
	Destroy();
}

//CQueueCtrl* CImageProcessCtrl::GetQueuePtr()
//{
//	return m_pQueueCtrl;
//}


CQueueCtrl* CImageProcessCtrl::GetResultPtr(int nHeadNo)
{
	if (nHeadNo >= GRABBER_COUNT) {
		ASSERT(FALSE);
		return NULL;
	}
	return m_pQueueResult[ nHeadNo ];
}

CQueueCtrl* CImageProcessCtrl::GetQueueFrmPtr( int nIndex )
{
	if (nIndex >= GRABBER_COUNT) {
		ASSERT(FALSE);
		return NULL;
	}
	return m_pQueueFrmCtrl[ nIndex ];
}

CCounterQueueCtrl* CImageProcessCtrl::GetCounterQueInPtr()
{
	return m_pQueueCounterIn;
}

// 21.11.11 Ahn Add Start
CThreadQueueCtrl* CImageProcessCtrl::GetThreadQueuePtr( int nHeadNo )
{
	return m_pThreadQueueCtrl[nHeadNo];
}
// 21.11.11 Ahn Add End


BOOL CImageProcessCtrl::IsInspection()
{
	return m_bInspFlag;
}

void CImageProcessCtrl::TabCountReset()
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->FrameCounterReset();
		m_pQueueFrmCtrl[i]->ResetQueue();
		m_pQueueResult[i]->ResetQueue();
		m_pQueueCounterIn->ResetQueue(); // 22.03.28 Ahn Add
	}
	m_pQueueCtrl->ResetQueue();
	m_pQueueCounterIn->ResetQueue();

	m_DefDataCtrl.RemoveAll();

}

int CImageProcessCtrl::FrameQueueReset()
{
	BOOL bFrmQueueReset = FALSE;
	for (int nCamNo = 0; nCamNo < MAX_CAMERA_NO; nCamNo++) {
		if (m_pQueueFrmCtrl[nCamNo]->GetSize() > 1) {
			bFrmQueueReset = TRUE;
		}
	}

	if (bFrmQueueReset == TRUE) {
		for (int nCamNo = 0; nCamNo < MAX_CAMERA_NO; nCamNo++) {
			m_pQueueFrmCtrl[nCamNo]->ResetQueue();
		}
	}

	return 0;
}
int CImageProcessCtrl::InspectionStart()
{
	FrameQueueReset();

//	LightON();
	
	m_DefDataCtrl.m_strLotNo = AprData.m_NowLotData.m_strLotNo;
	m_DefDataCtrl.m_strDate = AprData.m_NowLotData.m_strDate;

	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pResultThread[i]->SetDefectDataCtrl(&m_DefDataCtrl);
	}

	return 0;

}

int CImageProcessCtrl::InspectionEnd()
{
//	LightOFF();

	return 0;

}


int CImageProcessCtrl::GrabStart(BOOL bMode)
{
	// Camera 연결상태 확인.
	int i = 0;
	BOOL bConnected[MAX_CAMERA_NO] = {FALSE, FALSE};

	//Log Camera Setting
	LOGDISPLAY_SPECTXT(0)(_T("CImageProcessCtrl Grab Start"));

	// 22.05.26 Ahn Add Start
	theApp.m_pSigProc->EnableWorkSet(FALSE);
	// 22.05.26 Ahn Add End

	for (i = 0; i < MAX_CAMERA_NO; i++) {
		bConnected[i] = m_pGrabCtrl[i]->IsConnected();
	}

	if (bConnected[0] == TRUE) 
	{
		//Log Camera Setting
		LOGDISPLAY_SPECTXT(0)(_T("1번 카메라가 이미 연결중 입니다."));
	}
	if (bConnected[1] == TRUE) 
	{
		//Log Camera Setting
		LOGDISPLAY_SPECTXT(0)(_T("2번 카메라가 이미 연결중 입니다."));
	}
	// 22.01.04 Ahn Add Start
	int nLocalRet = 0;
	for (i = 0; i < GRABBER_COUNT; i++) {
		nLocalRet |= m_pGrabCtrl[i]->GrabPrepare(FALSE);
	}
	// 22.01.04 Ahn Add End

	if(nLocalRet != 0 ) return  - 1;

	for ( i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->GrabStart(FALSE);
	}

	// 22.04.06 Ahn Add Start
	GetCounterQueInPtr()->ResetQueue();
	CounterThreadRun(TRUE);
	// 22.04.06 Ahn Add End

	m_bInspFlag = TRUE;

	return 0;
}

int CImageProcessCtrl::GrabStart_Monitoring()
{
	if (m_bInspFlag == TRUE) {
		AfxMessageBox(_T("검사중 입니다."));
		return 1;
	}

	int nLevel = 0;
	int nChanel = 0;
	int nUnit = 0;
	// 22.07.07 Ahn Modify Start
	int nMaxUnit = theApp.m_pLightCtrl->GetMaxUnit();
	for (nUnit = 0; nUnit < nMaxUnit; nUnit++) {
	// 22.07.07 Ahn Modify End
		for (nChanel = 0; nChanel < MAX_LIGHT_CHANEL; nChanel++) {
			nLevel = AprData.m_pRecipeInfo->nLightLevel[nUnit][nChanel];
			theApp.m_pLightCtrl->SetLevel(nUnit, nChanel, nLevel);
		}
	}

	// 22.01.10 Ahn Add Start
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->GrabPrepare(TRUE);
	}
	// 22.01.10 Ahn Add End
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->GrabStart(TRUE);
	}

	return 0;
}


int CImageProcessCtrl::GrabStop()
{
	m_bInspFlag = FALSE;
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->GrabStop();
	}

	CounterThreadRun(FALSE); // 22.04.06 Ahn Add 

	m_DefDataCtrl.SaveErrorData();

	// 22.05.26 Ahn Add Start
	theApp.m_pSigProc->EnableWorkSet(TRUE);
	// 22.05.26 Ahn Add End

#if 1 //231018
	theApp.m_pImgProcCtrl->LightOFF();
#endif

	return 0;
}

int CImageProcessCtrl::LightON()
{
	int nLevel = 0;
	int nChanel = 0;
	int nUnit = 0;

	int nRet = 0;
	// 22.07.07 Ahn Modify Start
	int nMaxUnit = theApp.m_pLightCtrl->GetMaxUnit();
	for (nUnit = 0; nUnit < nMaxUnit; nUnit++) {
	// 22.07.07 Ahn Modify End
		for (nChanel = 0; nChanel < MAX_LIGHT_CHANEL; nChanel++) {
			nLevel = AprData.m_pRecipeInfo->nLightLevel[nUnit][nChanel];
			nRet = theApp.m_pLightCtrl->SetLevel(nUnit, nChanel, nLevel);
			AprData.SaveDebugLog_Format(_T("[ Light%d ON ]... %s"), nChanel, (nRet < 0) ? _T("Error") : _T("OK"));
		}
	}


	return 0;
}

int CImageProcessCtrl::LightOFF()
{
	int nLevel = 0;
	int nChanel = 0;
	int nUnit = 0;
	// 22.07.07 Ahn Modify Start
	int nMaxUnit = theApp.m_pLightCtrl->GetMaxUnit();

	int nRet = 0;

	for (nUnit = 0; nUnit < nMaxUnit; nUnit++)
	{
	// 22.07.07 Ahn Modify End
		for (nChanel = 0; nChanel < MAX_LIGHT_CHANEL; nChanel++)
		{
			nRet = theApp.m_pLightCtrl->SetLevel(nUnit, nChanel, 0);
			AprData.SaveDebugLog_Format(_T("[ Light%d OFF ]... %s"), nChanel, (nRet < 0) ? _T("Error") : _T("OK"));
		}
	}



	return 0;
}


int CImageProcessCtrl::GetImagePtr(BYTE** pImagePtr, int* pnWidth, int* pnHeight, int nIndex )
{
	int nRet = m_pGrabCtrl[nIndex]->GetImagePtr(pImagePtr, pnWidth, pnHeight);
	return nRet;
}

int CImageProcessCtrl::SetFrameHeight(int nLength)
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->SetFrameHeight(nLength);
	}
	return 0;
}

// 22.05.09 Ahn Add Start
int CImageProcessCtrl::SetFrameWidth(int nWidth)
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->SetFrameHeight(nWidth);
	}
	return 0;
}
// 22.05.09 Ahn Add End

int CImageProcessCtrl::SetFrameOffset(int nCamNo, int nOffset)
{
	int nRet = 0;
	if ((nCamNo < 0) || (nCamNo >= GRABBER_COUNT)) {
		return -1;
	}
	nRet = m_pGrabCtrl[nCamNo]->SetFrameOffset(nOffset);

	return 0;
}


#if defined( IMAGE_DRAW_NOTIFY_VERSION )
void CImageProcessCtrl::SetLastBmpStd(BYTE* pImgPtr, int nWidth, int nHeight, int nHeadNo)
{
	ASSERT(pImgPtr);
	if ((nHeadNo < 0) || (nHeadNo >= GRABBER_COUNT)) {
		return ;
	}
	::EnterCriticalSection(&m_csLastImg[nHeadNo]);
	
	m_pLastBmpStd[nHeadNo]->SetImage( nWidth, nHeight, pImgPtr ) ;

	::LeaveCriticalSection(&m_csLastImg[nHeadNo]);
}
int CImageProcessCtrl::GetLastBmpStd(CBitmapStd *pBmpStd, int nHeadNo)
{
	ASSERT(pBmpStd);
	if ((nHeadNo < 0) || (nHeadNo >= GRABBER_COUNT)) {
		return -1;
	}
	::EnterCriticalSection(&m_csLastImg[nHeadNo]);

	*pBmpStd = *m_pLastBmpStd[nHeadNo];
	
	::LeaveCriticalSection(&m_csLastImg[nHeadNo]);

	return 0;
}
#endif

#if defined (IMAGE_DRAW_DIRECT_VERSION )
void CImageProcessCtrl::SetImgDispHWnd(HWND HWnd, int nHeadNo)
{
	ASSERT(HWnd);
	if ((nHeadNo < 0) || (nHeadNo > GRABBER_COUNT)) {
		return;
	}
	m_pDispHWnd[nHeadNo] = HWnd;
	m_pResultThread[nHeadNo]->SetDispWnd( HWnd );
}
HWND CImageProcessCtrl::GetImgDispHWnd(int nHeadNo)
{
	if ((nHeadNo < 0) || (nHeadNo > GRABBER_COUNT)) {
		return NULL ;
	}
	return m_pDispHWnd[nHeadNo];
}
#endif

//// 21.08.21 Ahn Add Start
//void CImageProcessCtrl::SetMonitorDispHwnd(HWND hWnd, int nHeadNo)
//{
//	m_pGrabCtrl[nHeadNo]->SetDispWindow(hWnd);
//}
//// 21.08.21 Ahn Add End



void CImageProcessCtrl::SaveErrorData()
{
	m_DefDataCtrl.SaveErrorData();
}


int CImageProcessCtrl::SetLineRate(double dLineRate)
{
	int nRet = 0;
	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pGrabCtrl[i] != nullptr) {
			nRet = m_pGrabCtrl[i]->SetLineRate(dLineRate);
		}
	}
	return nRet;
}
int CImageProcessCtrl::SetGain(int nCamNo, double dGain)
{
	int nRet = 0;
//	for (int i = 0; i < GRABBER_COUNT; i++) {
//		if (m_pGrabCtrl[i] != nullptr) {
//			nRet = m_pGrabCtrl[i]->SetGain(dGain);
//		}
//	}
	if ((nCamNo >= MAX_CAMERA_NO) || (nCamNo < 0)) {
		return -1;
	}
	nRet = m_pGrabCtrl[nCamNo]->SetGain(dGain);
	return nRet;
}
int CImageProcessCtrl::SetExposureTime(double dTime)
{
	int nRet = 0;
	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pGrabCtrl[i] != nullptr) {
			nRet = m_pGrabCtrl[i]->SetExposureTime(dTime);
		}
	}
	return nRet;
}

double CImageProcessCtrl::GetLineRate()
{
	if (m_pGrabCtrl[0] == nullptr) return 0.0;
	return m_pGrabCtrl[0]->dGetLineRate();
}
double CImageProcessCtrl::GetGain()
{
	if (m_pGrabCtrl[0] == nullptr) return 0.0;
	return m_pGrabCtrl[0]->dGetGain();
}
double CImageProcessCtrl::GetExposureTime()
{
	if (m_pGrabCtrl[0] == nullptr) return 0.0;
	return m_pGrabCtrl[0]->dGetExposureTime();
}

// 22.07.06 Ahn Add Start
BOOL CImageProcessCtrl::IsGrabberRun()
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (m_pGrabCtrl[i]->IsRun() == FALSE) {
			return FALSE;
		}
	}

	return TRUE;
}
// 22.07.06 Ahn Add End


BOOL CImageProcessCtrl::IsConnected(int nCamNo) 
{
	//if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Image)) {
	//	return TRUE;
	//}

	if (m_pGrabCtrl[nCamNo] == nullptr) {
		return FALSE;
	}

	return m_pGrabCtrl[nCamNo]->IsConnected();
}

// 22.07.01 Ahn Add Start
BOOL CImageProcessCtrl::IsConnectedAll()
{	
	BOOL bConnected = TRUE;

	for (int nCamNo = 0; nCamNo < MAX_CAMERA_NO; nCamNo++) {
		if (m_pGrabCtrl[nCamNo]->IsConnected() == FALSE) {
			bConnected = FALSE;
		}
	}

	return bConnected ;
}
// 22.07.01 Ahn Add End


// 22.05.20 Ahn Add Start
BOOL CImageProcessCtrl::GetCameraLinkStatus(int nCamNo)
{
	if (m_pGrabCtrl[nCamNo] == nullptr) {
		return FALSE;
	}

	BOOL bGrabber;
	BOOL bCamera;
	BOOL bRet = TRUE;

	for (int i = 0; i < GRABBER_COUNT; i++) {
		bCamera = m_pGrabCtrl[nCamNo]->GetCameraLinkStatus();
		bGrabber = m_pGrabCtrl[nCamNo]->GetGrabberInitStatus();
		if ((bCamera == FALSE) || (bGrabber == FALSE)) {
			bRet = FALSE;
			break;
		}
	}

	return bRet;

}
// 22.05.20 Ahn Add End

void CImageProcessCtrl::SetTriggerMode(int nMode)
{
	int nRet = 0;
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->SetTriggerMode(nMode);
	}

}
int CImageProcessCtrl::GetTriggerMode()
{
	int nRet = m_pGrabCtrl[0]->GetTriggerMode();

	return nRet;
}

BOOL CImageProcessCtrl::GetCameraWaveData(int nCamNo, BYTE* btWaveData)
{
	if ((nCamNo < 0) || (GRABBER_COUNT <= nCamNo)) {
		return FALSE;
	}

	m_pGrabCtrl[nCamNo]->GetCamWaveData(btWaveData);

	return TRUE;
}

void CImageProcessCtrl::SetWaveMode(BOOL bMode)
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_pGrabCtrl[i]->SetWaveMode(bMode);
	}
}


// 22.04.06 Ahn Add Start
void CImageProcessCtrl::CounterThreadRun(BOOL bRunFlag)
{
	m_pCounterThread->ThreadRun(bRunFlag);
}
// 22.04.06 Ahn Add End

// 22.05.31 Ahn Add Start
CImageSaveQueueCtrl* CImageProcessCtrl::GetImageSaveQueuePtr() 
{ 
	return m_pImgSaveQueueCtrl; 
};
// 22.05.31 Ahn Add End


// 22.06.10 Ahn Add Start
CCropImgQueueCtrl* CImageProcessCtrl::GetCropImageQueuePtr()
{
	return m_pCropImgQueueCtrl;
}
// 22.06.10 Ahn Add End

	// 22.06.23 Ahn Add Start
CDefectQueueCtrl* CImageProcessCtrl::GetDefectQueuePtr()
{
	return m_pDefectQueueCtrl;
}
// 22.06.23 Ahn Add End

	// 22.12.09 Ahn Add Start
CTacTimeDataCtrl* CImageProcessCtrl::GetTactDataCtrlPtr()
{
	return m_pTacTimeDataCtrl ;
}
// 22.12.09 Ahn Add End


//CamType : Top 0, Bottom 1 카메라 이미지를 받았을 때
//FrameCtn : Camera에서 이벤트로 받은 이미지 번호 : Top, Bottom 각각
void CImageProcessCtrl::GrabDalsaCameraLink(int CamType, int FrameCnt)
{
	if (CamType == 0)
	{
		Top_FrameCtn = FrameCnt;
	}
	else
	{
		Bottom_FrameCtn = FrameCnt;
	}

	if ((Top_FrameCtn != -1) && (Bottom_FrameCtn != -1))
	{
		Top_FrameCtn = -1;
		Bottom_FrameCtn = -1;
		m_pImgCutTabThread->setEvent_ImageProcThread_TabFind();
		m_pImgCutTabThread->resume_ImageProcThread_TabFind();
	}
}
