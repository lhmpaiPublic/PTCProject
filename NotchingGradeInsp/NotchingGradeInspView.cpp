
// NotchingGradeInspView.cpp: CNotchingGradeInspView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "NotchingGradeInsp.h"
#endif

#include "MainFrm.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "SigProc.h"
#include "CCameraSettingDlg.h"
//#include "CCameraViewDlg.h"
#include "CCameraWaveDlg.h"
#include "ImageProcessCtrl.h"
#include "GlobalData.h"
#include "Win32File.h"
#include "CDebugImageAcqDlg.h"
#include "CRecipeSettingDlg.h"
#include "CRecipeCtrl.h"
#include "CLightControl.h"
#include "SystemSettingDlg.h"
#include "CStartDlg.h"
#include "CDeleteSettingDlg.h"
#include "CCounterQueueCtrl.h" // 22.03.28 Ahn Add
#include "CResultFileManager.h" // 22.04.21 Ahn Add 
#include "ModeDlg.h" // 22.06.13 Ahn Add
#include "CDebugLotCtrlDlg.h" // 22.06.27 Ahn Add

#include "CDeleteResultFileThread.h" // 22.07.04 Ahn Add

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define T_ID_IO_CHECK	101
#define T_ID_IO_ALIVE	102
#define T_ID_LONG_TERM	103 
#define T_ID_IO			104
// CNotchingGradeInspView

IMPLEMENT_DYNCREATE(CNotchingGradeInspView, CView)

BEGIN_MESSAGE_MAP(CNotchingGradeInspView, CView)
//	ON_WM_CONTEXTMENU()	//KANG 22.01.07 Delete
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
//	ON_COMMAND(ID_CAMERA_SETTING, &CNotchingGradeInspView::OnCameraSetting)
	ON_COMMAND(ID_CAMERA_VIEW, &CNotchingGradeInspView::OnCameraView)
	ON_COMMAND(ID_CAMERA_WAVE, &CNotchingGradeInspView::OnCameraWave)
	ON_COMMAND(ID_DEBUG_START, &CNotchingGradeInspView::OnDebugImageAcqStart)
	ON_COMMAND(ID_RECIPE_SETTING, &CNotchingGradeInspView::OnRecipeSetting)
	ON_COMMAND(ID_SYSTEM_SETTING, &CNotchingGradeInspView::OnSystemSetting)
	ON_COMMAND(ID_BACKUP_DELETE_SETTING, &CNotchingGradeInspView::OnBackupDeleteSetting)
	ON_COMMAND(IDD_DEBUG_CONTROL, &CNotchingGradeInspView::OnDebugControl)
END_MESSAGE_MAP()

// CNotchingGradeInspView 생성/소멸

CNotchingGradeInspView::CNotchingGradeInspView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pInspDlg = NULL;
	m_pDefMapDlg = NULL; // 22.11.09 Ahn Add
	m_pHistoryDlg = NULL;
	m_pCondDlg = NULL;
//	m_pCamViewDlg = NULL;
	m_pCamWaveDlg = NULL;
	m_nStatus = en_InspStop ;
	m_bReady = FALSE;
	pImgAcqDlg = NULL;
	m_bLotChangeReq = FALSE;
	m_nRunCheckCounter = 0;
	// 22.03.24 Ahn Add Start
	m_bLoadGlsInfoReq = TRUE; 
	m_bLastAlarmCode = 0xFFFF; 
	// 22.03.24 Ahn Add End

	m_bLotStartFlag = FALSE;
	m_bLotEndFlag = FALSE;
	m_bTabCountResetFlag = FALSE;
	m_bLotStartInitFlag = TRUE;
	m_bAlarmResetFlag = FALSE;


	//// 22.04.21 Ahn Add Start
	//m_pFileManager = NULL ;
	//// 22.04.21 Ahn Add End

	m_nCamErrorResetCnt = 0;

	// 22.06.27 Ahn Add Start
	m_bDebugLotStartReq = FALSE ;
	m_bDebugLotEndReq = FALSE ;
	m_bDebug_counterResetReq = FALSE;
	// 22.06.27 Ahn Add End

	// 22.07.04 Ahn Add Start
	m_pDeleteThread = NULL;
	// 22.07.04 Ahn Add End

	// 22.11.09 Ahn Add Start
	m_bDispMap = TRUE ;
	// 22.11.09 Ahn Add End
	// 22.12.07 Ahn Add Start
	m_nRedrawCnt = 0;
	// 22.12.07 Ahn Add End

	//로그 출력창 활성화 키(Ctrl 눌림 확인용)
	logControlKeyDown = false;
}

CNotchingGradeInspView::~CNotchingGradeInspView()
{
	if (m_pInspDlg != nullptr) {
		if (m_pInspDlg->m_hWnd != nullptr) {
			m_pInspDlg->DestroyWindow();
		}
		delete m_pInspDlg;
		m_pInspDlg = NULL;
	}
	// 22.11.09 Ahn Add Start
	if (m_pDefMapDlg == nullptr) {
		if (m_pDefMapDlg->m_hWnd != nullptr) {
			m_pDefMapDlg->DestroyWindow();
		}
		delete m_pDefMapDlg;
		m_pDefMapDlg = nullptr;
	}
	// 22.11.09 Ahn Add End

	if (m_pHistoryDlg != nullptr) {
		if (m_pHistoryDlg->m_hWnd != nullptr) {
			m_pHistoryDlg->DestroyWindow();
		}
		delete m_pHistoryDlg;
		m_pHistoryDlg = NULL;
	}
	if (m_pCondDlg != nullptr) {
		if (m_pCondDlg->m_hWnd != nullptr) {
			m_pCondDlg->DestroyWindow();
		}
		delete m_pCondDlg;
		m_pCondDlg = NULL;
	}
	//if (m_pCamViewDlg != nullptr) {
	//	if (m_pCamViewDlg->m_hWnd != nullptr) {
	//		m_pCamViewDlg->DestroyWindow();
	//	}
	//	delete m_pCamViewDlg;
	//	m_pCamViewDlg = NULL;
	//}

	if (m_pCamWaveDlg != nullptr) {
		if (m_pCamWaveDlg->m_hWnd != nullptr) {
			m_pCamWaveDlg->DestroyWindow();
		}
		delete m_pCamWaveDlg;
		m_pCamWaveDlg = NULL;
	}

	//// 22.04.21 Ahn Add Start
	//if (m_pFileManager != NULL) {
	//	delete m_pFileManager;
	//	m_pFileManager = NULL;
	//}
	//// 22.04.21 Ahn Add End

	// 22.07.04 Ahn Add Start
	if (m_pDeleteThread != NULL) {
		delete m_pDeleteThread ;
		m_pDeleteThread = NULL ;
	}
	// 22.07.04 Ahn Add End

	// 22.11.24 Ahn Add Start
	if (m_pDefMapDlg != NULL) {
		delete m_pDefMapDlg ;
		m_pDefMapDlg = NULL ;
	}
	// 22.11.24 Ahn Add End

	CloseDebugImgAcqDlg();
}

BOOL CNotchingGradeInspView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CNotchingGradeInspView 그리기

void CNotchingGradeInspView::OnDraw(CDC* /*pDC*/)
{
	CNotchingGradeInspDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CNotchingGradeInspView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
//KNAG 22.01.07 Delete Start
//	ClientToScreen(&point);
//	OnContextMenu(this, point);
//KANG 22.01.07 Delete End
}

//KANG 22.01.07 Delete Start
//void CNotchingGradeInspView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
//{
//#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
//#endif
//}
//KANG 22.01.07 Delete End


// CNotchingGradeInspView 진단

#ifdef _DEBUG
void CNotchingGradeInspView::AssertValid() const
{
	CView::AssertValid();
}

void CNotchingGradeInspView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNotchingGradeInspDoc* CNotchingGradeInspView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNotchingGradeInspDoc)));
	return (CNotchingGradeInspDoc*)m_pDocument;
}
#endif //_DEBUG


// CNotchingGradeInspView 메시지 처리기


int CNotchingGradeInspView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_pInspDlg = new CInspDlg(this, this);
	if (m_pInspDlg != nullptr) {
		if (m_pInspDlg->Create(IDD_DLG_INSP, this) == 0) {
			delete m_pInspDlg;
			m_pInspDlg = NULL;
			return -1;
		}
		else {
			m_pInspDlg->ShowWindow(SW_HIDE);
		}
	}

	// 22.11.09 Ahn Add Start
	m_pDefMapDlg = new CDefectMapDlg(this, this);
	if (m_pDefMapDlg != nullptr) {
		if (m_pDefMapDlg->Create(IDD_DEFECT_MAP_DLG, this) == 0) {
			delete m_pDefMapDlg;
			m_pDefMapDlg = nullptr;
			return -1 ;
		} else {
			m_pDefMapDlg->ShowWindow(SW_HIDE);
		}
	}
	// 22.11.09 Ahn Add End


	m_pHistoryDlg = new CHistoryDlg(this, this);
	if (m_pHistoryDlg != nullptr) {
		if (m_pHistoryDlg->Create(IDD_DLG_HISTORY, this) == 0) {
			delete m_pHistoryDlg;
			m_pHistoryDlg = NULL;
			return -1;
		}
		else {
			m_pInspDlg->ShowWindow(SW_HIDE);
		}
	}

	// 22.07.21 Ahn Modify Start
	//m_pCondDlg = new CCondDlg(this, this);
	//if (m_pCondDlg != nullptr) {
	//	if (m_pCondDlg->Create(IDD_DLG_COND, this) == 0) {
	m_pCondDlg = new CRecipeSettingDlg(FALSE, AprData.m_pRecipeInfo, this);
	if (m_pCondDlg != nullptr) {
		if (m_pCondDlg->Create(IDD_DLG_RECIPE_SETTING, this) == 0) {

	// 22.07.21 Ahn Modify Start
			delete m_pCondDlg;
			m_pCondDlg = NULL;
			return -1;
		}
		else {
			m_pInspDlg->ShowWindow(SW_HIDE);
		}
	}

	return 0;
}


void CNotchingGradeInspView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pInspDlg->SetWindowPos(NULL, 5, 0, cx - 10, cy , SWP_NOZORDER);
	//m_pDefMapDlg->SetWindowPos(NULL, 5, 0, cx - 10, cy, SWP_NOZORDER);// 22.11.09 Ahn Add 
	m_pHistoryDlg->SetWindowPos(NULL, 5, 0, cx - 10, cy, SWP_NOZORDER);
	m_pCondDlg->SetWindowPos(NULL, 5, 0, cx - 10, cy, SWP_NOZORDER);
}

// 22.11.15 Ahn Add Start
void CNotchingGradeInspView::SetMapDlgSize(CRect rcMapSize)
{
	m_pDefMapDlg->SetWindowPos(NULL, rcMapSize.left, 0, rcMapSize.Width(), rcMapSize.Height(), SWP_NOZORDER);// 22.11.09 Ahn Add 
}
// 22.11.15 Ahn Add End

int CNotchingGradeInspView::ChangeViewMode(int nMode)
{
	// TODO: 여기에 구현 코드 추가.
	switch (nMode) {
	case enInspMode:
					// 23.02.09 Ahn Modify Start
					//// 22.11.09 Ahn Add Start
					//if (m_bDispMap == TRUE) {
					//	if (m_pDefMapDlg != nullptr) {
					//		m_pDefMapDlg->ShowWindow(SW_SHOW);
					//	}
					//}
					//else {
					//	if (m_pInspDlg != nullptr) {
					//		m_pInspDlg->ShowWindow(SW_SHOW);
					//	}
					//}
					//// 22.11.09 Ahn Add End
					SwitchDisplay(m_bDispMap) ;
					// 23.02.09 Ahn Modify End 

					if (m_pHistoryDlg != nullptr) {
						m_pHistoryDlg->ShowWindow(SW_HIDE);
					}
					if (m_pCondDlg != nullptr) {
						m_pCondDlg->ShowWindow(SW_HIDE);
					}
					break;
	case enHistoryMode:
					if (m_pInspDlg != nullptr) {
						m_pInspDlg->ShowWindow(SW_HIDE);
					}
					// 22.11.09 Ahn Add Start
					if (m_pDefMapDlg != nullptr) {
						m_pDefMapDlg->ShowWindow(SW_HIDE);
						// 23.01.03 Ahh Add Start
						m_pDefMapDlg->RequestCloseImgDispDlg();
						// 23.01.03 Ahh Add End
					}
					// 22.11.09 Ahn Add End
					if (m_pHistoryDlg != nullptr) {
						m_pHistoryDlg->ShowWindow(SW_SHOW);
					}
					if (m_pCondDlg != nullptr) {
						m_pCondDlg->ShowWindow(SW_HIDE);
					}
					break;
	case enCondMode:
					if (m_pInspDlg != nullptr) {
						m_pInspDlg->ShowWindow(SW_HIDE);
					}
					// 22.11.09 Ahn Add Start
					if (m_pDefMapDlg != nullptr) {
						m_pDefMapDlg->ShowWindow(SW_HIDE);
						// 23.01.03 Ahh Add Start
						m_pDefMapDlg->RequestCloseImgDispDlg();
						// 23.01.03 Ahh Add End
					}
					// 22.11.09 Ahn Add End
					if (m_pHistoryDlg != nullptr) {
						m_pHistoryDlg->ShowWindow(SW_HIDE);
					}
					if (m_pCondDlg != nullptr) {
						m_pCondDlg->ShowWindow(SW_SHOW);
					}
					break;
	default:
					break;
	}
	return 0;
}


void CNotchingGradeInspView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	SetSignalCheckTimer();
	SetAlivePulseTimer();
	SetLogTermTimer();
	Set_I0Timer();	// 22.03.24 Ahn Add 

	SigInitialize();

	ChangeViewMode(GetDocument()->GetViewMode());

	OnRefresh();

}


int CNotchingGradeInspView::OnRefresh()
{
	// TODO: 여기에 구현 코드 추가.
	int nRet = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->ReflashAll();

	return nRet;
}

void CNotchingGradeInspView::SigInitialize()
{
	CSigProc *pSigProc = theApp.m_pSigProc ;

	pSigProc->SigOutEncoderZeroSet(FALSE);
	pSigProc->SigOutRecipeChangeAck(FALSE);
	pSigProc->sigOutLotEndAck(FALSE);
	pSigProc->SigOutLotStartAck(FALSE);
	pSigProc->SigOutRecipeChangeAck(FALSE);
	pSigProc->WriteAlarmCode(0x0000);
}

void CNotchingGradeInspView::ResetSignal()
{
	CSigProc* pSigProc = theApp.m_pSigProc;

	KillSignalCheckTimer();
	KillAlivePulseTimer();
	KillLongTermTimer();

	pSigProc->SigOutReady(FALSE);
	pSigProc->SigOutEncoderZeroSet(FALSE);
	pSigProc->SigOutRecipeChangeAck(FALSE);
	pSigProc->sigOutLotEndAck(FALSE);
	pSigProc->SigOutLotStartAck(FALSE);
	pSigProc->SigOutRecipeChangeAck(FALSE);
	pSigProc->SigOutAlivePulse(FALSE);
}

void CNotchingGradeInspView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == m_TID_IO_Check)
	{
		CSigProc* pSigProc = theApp.m_pSigProc;

		KillSignalCheckTimer();

		//////////////////////////////////////////////////////////////////////////
		//  Lot Start, Lot End, Tab Count Reset 감시 ]
		CheckLotEndProcess2();
		CheckTabZeroReset();
		CheckLotStartProcess();
		CheckAlarmReset();





		//////////////////////////////////////////////////////////////////////////
		// [ Main Loof ]

		switch (m_nStatus)
		{
		case	en_CameraReset:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => CameraReset");
			m_nCamErrorResetCnt++;
			if (GrabberResetReqest() == 0)
			{
				m_nCamErrorResetCnt = 0;
				// m_pInspDlg->ChangeStatus(enInspRun);
				ChangeStatus(enInspRun);
			}
			else
			{
				if (m_nCamErrorResetCnt > 3)
				{
					// m_pInspDlg->ChangeStatus(enInspStop);
					ChangeStatus(enInspStop);
					m_nStatus = en_Initialize;
				}
				else
				{ // 3회 까지 Retry
					m_nStatus = en_CameraReset;
					break;
				}
				break;
			}

			m_nStatus = en_Initialize;
			break;





			// 22.05.19 Ahn Add End
		case	en_Initialize:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => Initialize");
			pSigProc->SigOutReady(FALSE);
			pSigProc->SigOutEncoderZeroSet(FALSE);
			pSigProc->sigOutLotEndAck(FALSE);
			pSigProc->SigOutLotStartAck(FALSE);
			pSigProc->SigOutRecipeChangeAck(FALSE);
			pSigProc->WriteAlarmCode(0x0000);
			m_nStatus = en_InspStop;
			AprData.m_NowLotData.m_bProcError = TRUE;

			break;





		case	en_InspStop:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => InspStop");
			if (IsInspReady() == TRUE)
			{
				m_nStatus = en_WaitReady;
				pSigProc->SigOutReady(TRUE);
			}
			else
			{
				if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == TRUE)
				{
					if (m_bDebugLotStartReq == TRUE)
					{
						AprData.LotStartProcess(TRUE, 0x0001);
						m_bDebugLotStartReq = FALSE;
					}
					else if (m_bDebugLotEndReq == TRUE)
					{
						AprData.LotEndProcess();
						m_bDebugLotEndReq = FALSE;
						CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
						pFrame->ResetAndRefreshAll();
					}
				}
				CameraGrabStop(); // 22.07.07 Ahn Add - Stop Button이 Click 되어 있는 상태.
			}
			break;





		case	en_WaitReady:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => WaitReady");
			if (IsInspReady() == FALSE)
			{
				// Stop 버튼을 누른경우.
				m_nStatus = en_Initialize;
				InspectionEnd();
			}

			if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == TRUE)
			{
				m_nStatus = en_Ready;
				if (AprData.LotStartProcess(FALSE) < 0)
				{
					m_nStatus = en_Initialize;
					MessageBox(_T("LotStartProcess Error !! Please Check Recipe Parameter"));
					break;
				}
			}

			if (pSigProc->SigInReady() == TRUE)
			{
				m_nStatus = en_Ready;
				if (m_pDeleteThread != nullptr)
				{
					m_pDeleteThread->Kill();
					delete m_pDeleteThread;
					m_pDeleteThread = nullptr;
				}
			}
			else
			{
				SYSTEMTIME sysTime;
				SYSTEMTIME* pLastTIme = &AprData.m_NowLotData.m_LastDeleteCompletTime;
				::GetSystemTime(&sysTime);
				long lNowTime;
				lNowTime = (sysTime.wYear * 100000000) + (sysTime.wMonth * 1000000) + (sysTime.wDay * 10000) + (sysTime.wHour + 100) + sysTime.wMinute;
				long lLastDelCompletTime = (pLastTIme->wYear * 100000000) + (pLastTIme->wMonth * 1000000) + (pLastTIme->wDay * 10000) + (pLastTIme->wHour * 100) + pLastTIme->wMinute;

				double dDiskUse = AprData.m_dDiskTotal - AprData.m_dDiskFree;
				double dPercent = (dDiskUse / AprData.m_dDiskTotal) * 100.0;
				BOOL bDeleteStart = FALSE;
				BOOL bDeleteCapa = FALSE;

				if ((lNowTime - lLastDelCompletTime) > 60)
				{
					bDeleteStart = TRUE;
				}

				if (dPercent > 80.0)
				{
					bDeleteCapa = TRUE;
				}

				if ((bDeleteStart == TRUE) || (bDeleteCapa == TRUE))
				{
					if (m_pDeleteThread == nullptr)
					{
						m_pDeleteThread = new CDeleteResultFileThread();
						m_pDeleteThread->Begin();
					}
					else
					{
						if (m_pDeleteThread->IsComplet() == TRUE)
						{
							// 마지막 저장 완료 일시를 저장함.
							::GetLocalTime(&AprData.m_NowLotData.m_LotEndTime);
							AprData.FileCtrl_LotInfo(CGlobalData::en_mode_Write_LastDelTime);
							delete m_pDeleteThread;
							m_pDeleteThread = NULL;
						}
					}
				}
			}

			//			CheckLotEndProcess();
			break;






		case	en_Ready:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => Ready");
			if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == TRUE)
			{
				m_nStatus = en_PrepareRun;
			}
			else if (pSigProc->SigInRun() == TRUE)
			{
				m_nStatus = en_PrepareRun;
			}

			break;





		case	en_PrepareRun:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => PrepareRun");

			if (m_pDeleteThread != nullptr)
			{
				m_pDeleteThread->Kill();
				delete m_pDeleteThread;
				m_pDeleteThread = nullptr;
			}

			if (m_bLotStartInitFlag == TRUE)
			{
				if (AprData.LotStartProcess(FALSE) < 0)
				{
					m_nStatus = en_Initialize;
				}
				CameraGrabStart();

				m_bLotStartInitFlag = FALSE;

				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->ReflashAll();
			}
			else
			{
				if (theApp.m_pImgProcCtrl->IsGrabberRun() == FALSE)
				{
					CameraGrabStart();
				}

				CString strOldLotID;
				CString strOldRecipeName;
				strOldLotID.Format(_T("%s"), AprData.m_SeqDataIN.strCell_ID);
				strOldRecipeName.Format(_T("%s"), AprData.m_SeqDataIN.strRecipeName);
				pSigProc->ReadBlockAllData(&AprData.m_SeqDataIN);
				CString strNewLotID;
				strNewLotID = AprData.m_SeqDataIN.strCell_ID;
				strNewLotID.TrimRight();
				strOldLotID.TrimRight();

			}

			{
				int nRecipeNo = 0;
				AprData.m_NowLotData.m_strRecipeName = AprData.m_NowLotData.m_strNextRecipeName;
				CNotchingGradeInspDoc* pDoc = (CNotchingGradeInspDoc*)m_pDocument;
				pDoc->RecipeChange(nRecipeNo, AprData.m_NowLotData.m_strRecipeName);
			}


			if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == TRUE) {
				CString strNextRcp;
				strNextRcp = AprData.m_NowLotData.m_strNextRecipeName;
				int nRecipeNo = 0;
				CNotchingGradeInspDoc* pDoc = (CNotchingGradeInspDoc*)m_pDocument;
				pDoc->RecipeChange(nRecipeNo, strNextRcp);
				CString strLog;
				strLog.Format(_T("5.Recipe Change : RcipeNo[%d], RecipeName[%s]"), nRecipeNo, AprData.m_SeqDataIN.strRecipeName);
				AprData.SaveLotLog(strLog);
			}


			// 			pSigProc->SigOutLotStartAck(FALSE);
			// 			pSigProc->sigOutLotEndAck(FALSE);
			// 			pSigProc->SigOutTabZeroReset(FALSE);

			m_bEncoderReset = FALSE;
			m_nStatus = en_Run;
			m_nRedrawCnt = 0;

			//검사개시 설정함수
			InspectionStart();
			break;





		case	en_Run:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => Run");
			if (IsInspReady() == FALSE)
			{
				// Stop 버튼을 누른경우.
				m_nStatus = en_Initialize;
				InspectionEnd();
			}

			if (AprData.m_ErrStatus.IsHeavyError() == TRUE)
			{
				InspectionEnd();
				CameraGrabStop(); // 22.07.07 Ahn Add

				m_nStatus = en_CameraReset;
				ChangeStatus(enInspStop);
				AprData.m_ErrStatus.ErrorClear();
				break;
			}

			if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == FALSE)
			{
				if (pSigProc->SigInRun() == FALSE)
				{
					// POC에서 Run 신호가 OFF된 경우.
					// 검사 정지 처리
					InspectionEnd();
					m_nStatus = en_Initialize;
				}
			}

			if (AprData.m_NowLotData.m_SeqDataOut.dwDataReportV1 != (DWORD)AprData.m_NowLotData.m_nTabCount)
			{
				AprData.m_NowLotData.m_SeqDataOut.dwDataReportV1 = (DWORD)AprData.m_NowLotData.m_nTabCount;
				AprData.m_NowLotData.m_SeqDataOut.dwDataReportV2 = (DWORD)(AprData.m_NowLotData.m_nTabCount - AprData.m_NowLotData.m_nTabCountNG);
				AprData.m_NowLotData.m_SeqDataOut.dwDataReportV3 = (DWORD)AprData.m_NowLotData.m_nTabCountNG;

				if (AprData.m_NowLotData.m_nTabCount > 0)
				{
					AprData.m_NowLotData.m_SeqDataOut.dwDataReportV4 = (DWORD)(((double)AprData.m_NowLotData.m_nTabCountOK / (double)AprData.m_NowLotData.m_nTabCount) * 1000.0);
					//AprData.m_NowLotData.m_SeqDataOut.dwDataReportV5 = 100 - AprData.m_NowLotData.m_SeqDataOut.dwDataReportV4;
				}
				AprData.m_NowLotData.m_SeqDataOut.dwTopNgRealTimeCount = (DWORD)AprData.m_NowLotData.m_nTopNG; // 22.07.13 Ahn Modify  TabNG -> TopNG
				AprData.m_NowLotData.m_SeqDataOut.dwBottomNgRealTimeCount = (DWORD)AprData.m_NowLotData.m_nBottomNG;

				//////////////////////////////////////////////////////////////////////////
				// [ 레시피 설정값 전송 ]
				// [ 목표값 ]
				AprData.m_NowLotData.m_SeqDataOut.dwDrossTopTarget = (DWORD)(AprData.m_pRecipeInfo->dFoilExpOutNgSize + AprData.m_pRecipeInfo->dFoilExpBothNgSize);
				AprData.m_NowLotData.m_SeqDataOut.dwDrossBottomTarget = (DWORD)(AprData.m_pRecipeInfo->dFoilExpOutNgSize + AprData.m_pRecipeInfo->dFoilExpBothNgSize);
				AprData.m_NowLotData.m_SeqDataOut.dwFoilExpTopTarget = (DWORD)AprData.m_pRecipeInfo->dFoilExpInNgSize;
				AprData.m_NowLotData.m_SeqDataOut.dwFoilExpBottomTarget = (DWORD)AprData.m_pRecipeInfo->dFoilExpInNgSize;
				AprData.m_NowLotData.m_SeqDataOut.dwSpeterTopTarget = (DWORD)AprData.m_pRecipeInfo->dSurfaceNgSize;
				AprData.m_NowLotData.m_SeqDataOut.dwSpeterBottomTarget = (DWORD)AprData.m_pRecipeInfo->dSurfaceNgSize;


				// [ 연속/구간 불량 ]
				AprData.m_NowLotData.m_SeqDataOut.dwPrmContinuousCnt = AprData.m_nCoutinuouCount; //AprData.m_pRecipeInfo->nContinousNgCount;
				AprData.m_NowLotData.m_SeqDataOut.dwPrmSectorNgTabCnt= AprData.m_nSectorNgCount; //AprData.m_pRecipeInfo->nAlarmCount;
				AprData.m_NowLotData.m_SeqDataOut.dwPrmSectorBaseCnt = AprData.m_nSectorBaseCount; //AprData.m_pRecipeInfo->nSectorCount;


				//int nAddress = CSigProc::enWordWrite_DataReportV1_Ea;
				int nAddress;
				nAddress = CSigProc::GetWordAddress(CSigProc::enWordWrite_DataReportV1_Ea, MODE_WRITE);

				// 				int* pData = (int*)(&AprData.m_NowLotData.m_SeqDataOut);
				// 				int nSize = sizeof(_SEQ_OUT_DATA) / sizeof(int) ;
				// 				pSigProc->WritePLC_Block_device(nAddress, pData, nSize);

				if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
				{
					AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV1 = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDataReportV1;
					AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV2 = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDataReportV2;
					AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV3 = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDataReportV3;
					AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV4 = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDataReportV4;
					AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV5 = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDataReportV5;
					AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV6 = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDataReportV6;
					AprData.m_NowLotData.m_SeqDataOutSms.wContinueAlarmCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwContinueAlarmCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wHeavyAlarmCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwHeavyAlarmCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wDrossTopCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDrossTopCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wDrossBottomCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDrossBottomCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wFoilExpTopCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwFoilExpTopCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wFoilExpBottomCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwFoilExpBottomCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wSpeterTopCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwSpeterTopCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wSpeterBottomCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwSpeterBottomCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wTopNgRealTimeCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwTopNgRealTimeCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wBottomNgRealTimeCount = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwBottomNgRealTimeCount;
					AprData.m_NowLotData.m_SeqDataOutSms.wDrossTopTarget = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDrossTopTarget;

					AprData.m_NowLotData.m_SeqDataOutSms.wDrossBottomTarget = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwDrossBottomTarget;
					AprData.m_NowLotData.m_SeqDataOutSms.wFoilExpTopTarget = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwFoilExpTopTarget;
					AprData.m_NowLotData.m_SeqDataOutSms.wFoilExpBottomTarget = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwFoilExpBottomTarget;
					AprData.m_NowLotData.m_SeqDataOutSms.wSpeterTopTarget = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwSpeterTopTarget;
					AprData.m_NowLotData.m_SeqDataOutSms.wSpeterBottomTarget = (WORD)AprData.m_NowLotData.m_SeqDataOut.dwSpeterBottomTarget;




					short* pData = (short*)(&AprData.m_NowLotData.m_SeqDataOutSms);
					int nSize = sizeof(_SEQ_OUT_DATA_SMS) / sizeof(WORD);
					pSigProc->WritePLC_Block_device(nAddress, pData, nSize);

				}
				else
				{
					int* pData = (int*)(&AprData.m_NowLotData.m_SeqDataOut);
					int nSize = sizeof(_SEQ_OUT_DATA) / sizeof(int);
					pSigProc->WritePLC_Block_device(nAddress, pData, nSize);
				}

			}

			if (m_nRedrawCnt >= 50)
			{
				CRect rcRect;
				ReDrawMap(FALSE, rcRect);
				m_nRedrawCnt = 0;
			}
			else
			{
				m_nRedrawCnt++;
			}

			{
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->ReflashAll();
			}
			break;







		case	en_LotChange:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => LotChange");
			m_nStatus = en_Initialize;
			break;





		case	en_ErrorStop:
			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CNotchingGradeInspView::OnTimer => ErrorStop");
			break;

		}

		SetSignalCheckTimer();
	}




	//////////////////////////////////////////////////////////////////////////
	// [ Alive ]
	if (nIDEvent == m_TID_Alive_Pulse)
	{
		KillAlivePulseTimer();
		CSigProc* pSigProc = theApp.m_pSigProc;
		pSigProc->SigOutAlivePulse(TRUE);
		SetAlivePulseTimer(); //pyjtest

	}






	if (m_TID_IO == nIDEvent)
	{
		Kill_IoTimer();

		// 22.12.06 Ahn Delete Start
		//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		//pFrame->ReflashAll();
		// 22.12.06 Ahn Delete End


		//pyjtest
// 		CSigProc* pSigProc = theApp.m_pSigProc;
// 		if (pSigProc->SigInAlarmReset() == TRUE)
// 		{
// 			pSigProc->SigOutAlarmResetAck(TRUE);
// 
// 			Sleep(200);
// 
// 			pSigProc->SigOutAlarmResetAck(FALSE);
// 		}

		Set_I0Timer();
	}






	if (m_TID_Long_Term == nIDEvent)
	{
		KillLongTermTimer();

#if !defined( _DEBUG )
		CheckDiskSpace();
#endif 
		SetLogTermTimer();
	}

	CView::OnTimer(nIDEvent);
}

// 22.12.01 Ahn Add Start
void CNotchingGradeInspView::ReDrawMap( BOOL bModeRect, CRect rcRange )
{
	if(m_pDefMapDlg != nullptr){
		if (bModeRect == FALSE) {
			m_pDefMapDlg->Invalidate(FALSE);
		}
		else {
			m_pDefMapDlg->InvalidateRect(rcRange);
		}
	}
}
// 22.12.01 Ahn Add End

// 22.07.01 Ahn Add Start
void CNotchingGradeInspView::CheckDiskSpace()
{
	CWin32File file;

	double dFreeSpace, dTotalSize, dPercent;

	double ret = file.GetDiskSpace(AprData.m_strResultPath, &dTotalSize, &dFreeSpace, &dPercent);
	// 22.07.04 Ahn Add Start
	AprData.m_dDiskTotal = dTotalSize ;
	AprData.m_dDiskFree = dFreeSpace ;
	AprData.m_dDiskPercent = dPercent ;
	// 22.07.04 Ahn Add End

	// 22.08.05 Ahn Add Start
	double dAlarmSize = 90.0;
	double dWarningSize = 80.0;
	if (dAlarmSize < dPercent) {
		CString strMsg;
		strMsg.Format(_T("디스크 사용량이 %1.lf%% 입니다. 디스크 공간을 확보해 주세요."), dPercent);
		AprData.m_ErrStatus.SetError(CErrorStatus::en_DiskCapacityAlarm, strMsg);
	}
	else if (dWarningSize < dPercent) {
		CString strMsg;
		strMsg.Format(_T("디스크 사용량이 %1.lf %%입니다. 디스크 공간을 확보해 주세요."), dPercent);
		AprData.m_ErrStatus.SetError(CErrorStatus::en_DiskCapacityWarning, strMsg);
	}
	// 22.08.05 Ahn Add End
}
// 22.07.01 Ahn Add End


BOOL CNotchingGradeInspView::SetSignalCheckTimer()
{
	m_TID_IO_Check = SetTimer(T_ID_IO_CHECK, 50/*10*/, NULL);
//	m_TID_IO_Check = SetTimer(T_ID_IO_CHECK, 500, NULL); //pyjtest
	return FALSE;
}
BOOL CNotchingGradeInspView::KillSignalCheckTimer()
{
	if (m_TID_IO_Check != 0) {
		KillTimer(m_TID_IO_Check);
		m_TID_IO_Check = 0;
		return TRUE;
	}

	return FALSE;
}

BOOL CNotchingGradeInspView::Set_I0Timer()
{
	m_TID_IO = SetTimer(T_ID_IO, 1000, NULL);
	return FALSE;

}
BOOL CNotchingGradeInspView::Kill_IoTimer()
{
	if (m_TID_IO != 0) {
		KillTimer(m_TID_IO);
		m_TID_IO = 0;
		return TRUE;
	}

	return FALSE;
}

BOOL CNotchingGradeInspView::SetAlivePulseTimer()
{
	m_TID_Alive_Pulse = SetTimer(T_ID_IO_ALIVE, 500, NULL);

	return FALSE;
}

BOOL CNotchingGradeInspView::KillAlivePulseTimer()
{
	if (m_TID_Alive_Pulse != 0) {
		KillTimer(m_TID_Alive_Pulse);
		m_TID_Alive_Pulse = 0;
		return TRUE;
	}

	return FALSE;
}

BOOL CNotchingGradeInspView::SetLogTermTimer()
{
	m_TID_Long_Term = SetTimer( T_ID_LONG_TERM, 6000, NULL ); // 1분에 한번

	return FALSE;
}
BOOL CNotchingGradeInspView::KillLongTermTimer()
{
	if (m_TID_Long_Term != 0) {
		KillTimer(m_TID_Long_Term);
		m_TID_Long_Term = 0;
		return TRUE;
	}
	return FALSE;
}

void CNotchingGradeInspView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ResetSignal();
}

void CNotchingGradeInspView::OnCameraView()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	//if (m_pCamViewDlg == NULL) {
	//	m_pCamViewDlg = new CCameraViewDlg(this);
	//	m_pCamViewDlg->Create(IDD_DLG_CAM_VIEW, this);
	//}
	//m_pCamViewDlg->ShowWindow(SW_SHOW);	
}

void CNotchingGradeInspView::OnCameraWave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	if (m_pCamWaveDlg == NULL) {
		m_pCamWaveDlg = new CCameraWaveDlg(this);
		m_pCamWaveDlg->Create(IDD_DLG_CAM_WAVE);
	}
	m_pCamWaveDlg->ShowWindow(SW_SHOW);

}

// 22.05.19 Ahn Add Start
int CNotchingGradeInspView::GrabberResetReqest()
{
	int nRet = 0;
	// 혹시나 
	theApp.m_pImgProcCtrl->InspectionEnd(); 
		
	// 
	theApp.m_pImgProcCtrl->ResetCamera();

	for (int i = 0; i < GRABBER_COUNT; i++) {
		if (theApp.m_pImgProcCtrl->GetCameraLinkStatus(i) == FALSE) {
			nRet =  -1 ; // 연결 실패
			break;
		}
	}

	return nRet ;
}
// 22.05.19 Ahn Add End

// 22.07.06 Ahn Add Start
// Lot Start 시점에 Call
int CNotchingGradeInspView::CameraGrabStart()
{
	int nRet = 0;
	//카메라 Image Processing Start
	if (theApp.m_pImgProcCtrl != nullptr) {
		nRet = theApp.m_pImgProcCtrl->GrabStart();
	}
	// 22.07.07 Ahn Add Test Start
	CString strLog;
	strLog.Format(_T("Camera Grab Start"));
	AprData.SaveErrorLog(strLog);
	// 22.07.07 Ahn Add Test End
	return nRet;
}
// Lot End
int CNotchingGradeInspView::CameraGrabStop()
{
	int nRet = 0;
	if (theApp.m_pImgProcCtrl != nullptr) {
		if (theApp.m_pImgProcCtrl->IsGrabberRun() == TRUE) {
			nRet = theApp.m_pImgProcCtrl->GrabStop();

			// 22.07.07 Ahn Add Test Start
			CString strLog;
			strLog.Format(_T("Camera Grab Stop"));
			AprData.SaveErrorLog(strLog);
			// 22.07.07 Ahn Add Test End
		}
	}

	return nRet;
}
// 22.07.06 Ahn Add End


int CNotchingGradeInspView::InspectionStart()
{
	int nRet = 0;
	CNotchingGradeInspDoc *pDoc = (CNotchingGradeInspDoc*)m_pDocument;
	AprData.SaveDebugLog(_T("!!!! 검사 개시 ( Inspection Start ) !!!!"));

	theApp.m_pImgProcCtrl->InspectionStart();

	return nRet;
}
int CNotchingGradeInspView::InspectionEnd()
{
	int nRet = 0;
	CNotchingGradeInspDoc* pDoc = (CNotchingGradeInspDoc*)m_pDocument;

	// 22.02.23 Ahn Delete End
	theApp.m_pImgProcCtrl->InspectionEnd();
	// 21.12.23 Ahn Modify End
	AprData.SaveDebugLog(_T("----검사 종료 ( Inspection End )----"));

	return nRet;

}
// 기동 준비 완료 상태.
BOOL CNotchingGradeInspView::IsInspReady() 
{
	return m_bReady;
}

BOOL CNotchingGradeInspView::SetInspReady(BOOL bStatus)
{
	m_bReady = bStatus;

	CSigProc *pSigProc = theApp.m_pSigProc ;
	pSigProc->WriteBlockAllData(bStatus);	

	// 23.01.03 Ahn Add Start
	m_pDefMapDlg->RequestCloseImgDispDlg();
	// 23.01.03 Ahn Add End

	// 23.01.10 Ahn Add Start
	m_pDefMapDlg->EnableWindow(!bStatus);
	// 23.01.10 Ahn Add End

	return m_bReady;
}

BOOL CNotchingGradeInspView::IsRun() 
{
	if (m_nStatus == en_Run) {
		return TRUE;
	}
	return FALSE;
}


void CNotchingGradeInspView::OnDebugImageAcqStart()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (pImgAcqDlg == NULL) {
		pImgAcqDlg = new CDebugImageAcqDlg(this);
		pImgAcqDlg->Create(IDD_DEBUG_IMG_ACQ_DLG, this);
	}
	pImgAcqDlg->ShowWindow(SW_SHOW);
}

void CNotchingGradeInspView::CloseDebugImgAcqDlg()
{
	if (pImgAcqDlg != NULL) {
		pImgAcqDlg->Destroy();
		delete pImgAcqDlg;
		pImgAcqDlg = NULL;
	}
}


void CNotchingGradeInspView::OnRecipeSetting()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	//CRecipeInfo recipeInfo = *AprData.m_pRecipeInfo;
	//CRecipeSettingDlg dlg(FALSE, &recipeInfo, this ) ;
	//if (dlg.DoModal() == IDOK) {

	//}	 
}


void CNotchingGradeInspView::OnSystemSetting()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CSystemSettingDlg dlg( this ) ;
	if (dlg.DoModal() == IDOK) {
	}
	int a = 0; 
}

BOOL CNotchingGradeInspView::GetLotChangeReq()
{
	return m_bLotChangeReq;
}
void CNotchingGradeInspView::SetLotChangeReq(BOOL bMode)
{
	m_bLotChangeReq = bMode;
}


void CNotchingGradeInspView::OnBackupDeleteSetting()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDeleteSettingDlg dlg;
	dlg.DoModal();

}

void CNotchingGradeInspView::ChangeStatus(int nStatus)
{
	// 버튼 상태 변경 
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->ChangeStatus(nStatus);
}


void CNotchingGradeInspView::OnDebugControl()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDebugLotCtrlDlg dlg(this);
	if (dlg.DoModal() == IDOK) {
		
	}
}



// 22.07.26 Ahn Add Start
int CNotchingGradeInspView::CheckLotEndProcess()
{
	int nRet = 0;
	CSigProc* pSigProc = theApp.m_pSigProc;

	if ((pSigProc->SigInLotEnd() == TRUE) && (pSigProc->sigOutLotEndAck(-1) == FALSE))
	{
		pSigProc->sigOutLotEndAck(TRUE);

		CameraGrabStop();

		AprData.LotEndProcess();


		{
			AprData.m_NowLotData.m_SeqDataLotEnd.dwTopNgLotEndCount = AprData.m_NowLotData.m_nTopNG;
			AprData.m_NowLotData.m_SeqDataLotEnd.dwBottomNgLotEndCount = AprData.m_NowLotData.m_nBottomNG;

			//int nAddress = CSigProc::enWordWrite_Top_Defect_Count_LotEnd;
			int nAddress = CSigProc::GetWordAddress(CSigProc::enWordWrite_Top_Defect_Count_LotEnd, MODE_WRITE);

			int* pData = (int*)(&AprData.m_NowLotData.m_SeqDataLotEnd); // 22.07.13 Ahn Modify m_SeqDataLot -> LotEnd
			int nSize = sizeof(_SEQ_OUT_DATA_LOT_END) / sizeof(int);
			pSigProc->WritePLC_Block_device(nAddress, pData, nSize);
		}

		Sleep(100); // 22.03.29 Ahn Modify 10 -> 100 

		if (pSigProc->SigInTabZeroReset() == TRUE)
		{
			AprData.SaveLotLog(_T("Tab Zero Reset 신호 ON"));
			pSigProc->SigOutTabZeroReset(TRUE);

			theApp.m_pImgProcCtrl->TabCountReset();
			AprData.SaveLotLog(_T("Tab No And Queue Reset"));

			AprData.m_NowLotData.ClearAllCount();
			AprData.FileCtrl_LotInfo(CGlobalData::en_mode_LotEnd);

			CNotchingGradeInspDoc* pDoc = (CNotchingGradeInspDoc*)m_pDocument;
			pDoc->SetReqCounterReset(TRUE);
			pSigProc->SigOutEncoderZeroSet(FALSE);
		}
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->ResetAndRefreshAll();

		pSigProc->sigOutLotEndAck(FALSE);

	}
	return nRet;
}
// 22.07.26 Ahn Add End


int CNotchingGradeInspView::CheckLotEndProcess2() //조건 없이 Lot End Check
{
	int nRet = 0;
	CSigProc* pSigProc = theApp.m_pSigProc;

	BOOL bSigIn = (pSigProc->SigInLotEnd() == 1) ? TRUE : FALSE;

	if ( (bSigIn == TRUE) && (m_bLotEndFlag == FALSE) )
	{
		m_bLotEndFlag = TRUE;

		AprData.SaveDebugLog(_T("CheckLotEndProcess2 ON"));

		pSigProc->sigOutLotEndAck(TRUE);
		CameraGrabStop();
		AprData.LotEndProcess();

		AprData.m_NowLotData.m_SeqDataLotEnd.dwTopNgLotEndCount = AprData.m_NowLotData.m_nTopNG;
		AprData.m_NowLotData.m_SeqDataLotEnd.dwBottomNgLotEndCount = AprData.m_NowLotData.m_nBottomNG;

		int nAddress = CSigProc::GetWordAddress(CSigProc::enWordWrite_Top_Defect_Count_LotEnd, MODE_WRITE);

		int* pData = (int*)(&AprData.m_NowLotData.m_SeqDataLotEnd); // 22.07.13 Ahn Modify m_SeqDataLot -> LotEnd
		int nSize = sizeof(_SEQ_OUT_DATA_LOT_END) / sizeof(int);
		pSigProc->WritePLC_Block_device(nAddress, pData, nSize);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->ResetAndRefreshAll();
	}
	if ((bSigIn == FALSE) && (m_bLotEndFlag == TRUE))
	{
		m_bLotEndFlag = FALSE;
		AprData.SaveDebugLog(_T("CheckLotEndProcess2 OFF"));

		pSigProc->sigOutLotEndAck(FALSE);
	}


	return nRet;
}



int CNotchingGradeInspView::CheckTabZeroReset()
{
	int nRet = 0;
	CSigProc* pSigProc = theApp.m_pSigProc;

	BOOL bSigIn = (pSigProc->SigInTabZeroReset() == 1) ? TRUE : FALSE;
	if ((bSigIn == TRUE) && (m_bTabCountResetFlag == FALSE) )
	{
		m_bTabCountResetFlag = TRUE;
		AprData.SaveDebugLog(_T("CheckTabZeroReset ON"));

		AprData.SaveLotLog(_T("Tab Zero Reset 신호 ON"));
		pSigProc->SigOutTabZeroReset(TRUE);

		theApp.m_pImgProcCtrl->TabCountReset();
		AprData.SaveLotLog(_T("Tab No And Queue Reset"));

		AprData.m_NowLotData.ClearAllCount();
		AprData.FileCtrl_LotInfo(CGlobalData::en_mode_LotEnd);

		CNotchingGradeInspDoc* pDoc = (CNotchingGradeInspDoc*)m_pDocument;
		pDoc->SetReqCounterReset(TRUE);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->ResetAndRefreshAll();
		pFrame->ReflashAll();
		pFrame->ResetResultViewDlg();


	}
	if ((bSigIn == FALSE) && (m_bTabCountResetFlag == TRUE))
	{
		m_bTabCountResetFlag = FALSE;
		AprData.SaveDebugLog(_T("CheckTabZeroReset OFF"));

		pSigProc->SigOutTabZeroReset(FALSE);

	}

	return nRet;
}


int CNotchingGradeInspView::CheckLotStartProcess()
{
	int nRet = 0;
	CSigProc* pSigProc = theApp.m_pSigProc;

	BOOL bLotStartSigIn = (pSigProc->SigInLotStart()==1) ? TRUE : FALSE;
	if ((bLotStartSigIn == TRUE) && (m_bLotStartFlag == FALSE))
	{
		m_bLotStartFlag = TRUE;
		AprData.SaveDebugLog(_T("CheckLotStartProcess ON"));
		AprData.SaveLotLog(_T("1.Lot Start Signal ON "));

		if (AprData.LotStartProcess(bLotStartSigIn) < 0)
		{
			m_nStatus = en_Initialize;
		}

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->ReflashAll();
	}
	if ((bLotStartSigIn == FALSE) && (m_bLotStartFlag == TRUE))
	{
		m_bLotStartFlag = FALSE;
		AprData.SaveDebugLog(_T("CheckLotStartProcess OFF"));

		pSigProc->SigOutLotStartAck(FALSE);
	}

	return nRet;
}


int CNotchingGradeInspView::CheckAlarmReset()
{
	int nRet = 0;
	CSigProc* pSigProc = theApp.m_pSigProc;

	BOOL bSigIn = (pSigProc->SigInAlarmReset() == 1) ? TRUE : FALSE;
	if ((bSigIn == TRUE) && (m_bAlarmResetFlag == FALSE))
	{
		m_bAlarmResetFlag = TRUE;
		AprData.SaveDebugLog(_T("CheckAlarmReset ON"));

		pSigProc->SigOutAlarmResetAck(TRUE);
		pSigProc->WriteAlarmCode(0x0000);
		pSigProc->SigOutAlarmExist(FALSE);

	}
	if ((bSigIn == FALSE) && (m_bAlarmResetFlag == TRUE))
	{
		m_bAlarmResetFlag = FALSE;
		AprData.SaveDebugLog(_T("CheckAlarmReset OFF"));

		pSigProc->SigOutAlarmResetAck(FALSE);

	}

	return nRet;
}

// 23.02.09 Ahn Add Start
void CNotchingGradeInspView::SwitchDisplay(BOOL bModeMap)
{
	m_bDispMap = bModeMap;

	int nMode = GetDocument()->GetViewMode();

	if (enInspMode == nMode) {
		if (m_bDispMap == TRUE) {
			m_pInspDlg->ShowWindow(SW_HIDE);
			m_pDefMapDlg->ShowWindow(SW_SHOW);
		}
		else {
			m_pInspDlg->ShowWindow(SW_SHOW);
			m_pDefMapDlg->ShowWindow(SW_HIDE);
		}
	}
}
// 23.02.09 Ahn Add End


BOOL CNotchingGradeInspView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_CONTROL)
		{
			logControlKeyDown = true;
		}
		else if (pMsg->wParam == VK_F5)
		{
			if (logControlKeyDown && CLogDisplayDlg::gInst())
			{
				CLogDisplayDlg::gInst()->ShowWindow(SW_SHOW);
				logControlKeyDown = false;
			}
		}
	}

	return CView::PreTranslateMessage(pMsg);
}
