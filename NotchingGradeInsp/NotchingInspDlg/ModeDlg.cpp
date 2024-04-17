// ModeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "MainFrm.h"
#include "NotchingGradeInspView.h"
#include "ModeDlg.h"
#include "afxdialogex.h"
#include "CImageProcSimDlg.h"
#include "SystemSettingDlg.h"
#include "DIOTestDlg.h"
#include "GlobalData.h"					// 22.05.25 Son Add
#include "CResultDirectoryDlg.h"
#include "ImageProcessCtrl.h"
// CModeDlg 대화 상자
#include "PioCtrl.h"
#include "StdIoCtrl.h"
#include "CLightControl.h"
#include "GrabberCtrl.h"
#include "SigProc.h"
#include "CIoMonitorDlg.h"
#include "CTactTimeGraphDlg.h"


static CString UiText1[][3] =
{
	{_T("검사모드"), _T("Inspect Mode"), _T("检查模式")},
	{_T("설정모드"), _T("Recipe Setting"), _T("设定模式")},
	{_T("저장경로"), _T("Directory"), _T("阴极模式")},
	{_T("시작"), _T("Run"), _T("开始")},
	{_T("정지"), _T("Stop"), _T("停止")},
	{_T("시뮬레이션"), _T("Simulation"), _T("模拟")},
	{_T("HISTORY"), _T("HISTORY"), _T("历史履历")},
	{_T("LIVE"), _T("LIVE"), _T("实时")},//JJ
	{_T("Tact time"), _T("Tact time"), _T("Tact time")},
	{_T("CAM"), _T("CAM"), _T("相机")},
	{_T("PLC"), _T("PLC"), _T("PLC")},
	{_T("I/O"), _T("I/O"), _T("I/O")},
	{_T("LAMP"), _T("LAMP"), _T("光源")},
};

enum UiText1Name
{
	RADI_INSP_MODE,
	RAD_COND_MODE,
	BTN_DIRECTORY,
	RAD_RUN,
	RAD_STOP,
	BTN_INSP_SIM,
	RAD_HISTORY_MODE,
	CHK_SWITCH_DISP, //JJ
	BTN_TACT_TIME,
	ST_CAMERA_STATE,
	ST_PLC_STATE,
	ST_IO_STATE,
	ST_LIGHT_STATE,
	MAX_COUNT
};

static int UiText1NameText[] =
{
	IDC_RADI_INSP_MODE,
	IDC_RAD_COND_MODE,
	IDC_BTN_DIRECTORY,
	IDC_RAD_RUN,
	IDC_RAD_STOP,
	IDC_BTN_INSP_SIM,
	IDC_RAD_HISTORY_MODE,
	IDC_CHK_SWITCH_DISP, 
	IDC_BTN_TACT_TIME,
	IDC_ST_CAMERA_STATE,
	IDC_ST_PLC_STATE,
	IDC_ST_IO_STATE,
	IDC_ST_LIGHT_STATE,
};

CModeDlg* pTempLoad = NULL;

IMPLEMENT_DYNAMIC(CModeDlg, CDialogEx)

CModeDlg::CModeDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_MODE, pParent)
	, m_bDispSwitch(FALSE)
{
	//생성 시 인스턴스 객체 세팅
	UIMGR->setModeDlg(this);

	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = pView->GetDocument();
	}
	m_fontTitle.CreatePointFont(200, _T("Arial"));
	m_nViewMode = enInspMode;

	m_FontDiskCapa.CreatePointFont(10, _T("Arial"));

	m_bPlcLastFlag = FALSE ;
	m_bIoLastFlag = FALSE ;
	m_bLampLastFlag = FALSE ;
	m_bCamLastFlag = FALSE ;

	m_bRunLastFlag = FALSE ;

	m_pIoMonitDlg = NULL;

	m_pTactGraph = NULL ;

	m_nTimerInterval_DeviceCheck = 2000; // 240108
}

CModeDlg::~CModeDlg()
{
	//소멸 시 인스턴스 객체 삭제
	UIMGR->delModeDlg();

	if (m_pTactGraph != NULL) {
		delete m_pTactGraph ;
		m_pTactGraph = NULL;
	}
}

void CModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_STATE, m_stState);
	DDX_Control(pDX, IDC_ST_LOGO, m_cLogo);
	DDX_Radio(pDX, IDC_RADI_INSP_MODE, m_nViewMode);
	DDX_Control(pDX, IDC_ST_TITLE, m_stTitle);
	DDX_Control(pDX, IDC_BTN_EXIT, m_cBtnExit);
	DDX_Control(pDX, IDC_BTN_LOG, m_cbtnLog);
	DDX_Control(pDX, IDC_RAD_RUN, m_radInspRun);
	DDX_Control(pDX, IDC_RAD_STOP, m_radInspStop);
	DDX_Control(pDX, IDC_ST_CAMERA_STATE, m_stCameraState);
	DDX_Control(pDX, IDC_ST_PLC_STATE, m_stPlcState);
	DDX_Control(pDX, IDC_ST_IO_STATE, m_stIoState);
	DDX_Control(pDX, IDC_ST_LIGHT_STATE, m_stLightState);
	DDX_Control(pDX, IDC_ST_DISK_SPACE, m_stDiskSpace);
	DDX_Check(pDX, IDC_CHK_SWITCH_DISP, m_bDispSwitch);
}


BEGIN_MESSAGE_MAP(CModeDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADI_INSP_MODE, &CModeDlg::OnBnClickedRadiInspMode)
	ON_BN_CLICKED(IDC_RAD_HISTORY_MODE, &CModeDlg::OnBnClickedRadHistoryMode)
	ON_BN_CLICKED(IDC_RAD_COND_MODE, &CModeDlg::OnBnClickedRadCondMode)
	ON_BN_CLICKED(IDC_BTN_INSP_SIM, &CModeDlg::OnBnClickedBtnInspSim)
	ON_BN_CLICKED(IDC_BTN_LOG, &CModeDlg::OnBnClickedBtnLog)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CModeDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_DIRECTORY, &CModeDlg::OnBnClickedBtnDirectory)
	ON_BN_CLICKED(IDC_RAD_RUN, &CModeDlg::OnBnClickedRadRun)
	ON_BN_CLICKED(IDC_RAD_STOP, &CModeDlg::OnBnClickedRadStop)
	ON_BN_CLICKED(IDC_BTN_TACT_TIME, &CModeDlg::OnBnClickedBtnTactTime)
	ON_BN_CLICKED(IDC_CHK_SWITCH_DISP, &CModeDlg::OnBnClickedChkSwitchDisp)
	ON_MESSAGE(WM_COUNTBORDERROR, OnCountBordError)
	ON_MESSAGE(WM_TOGGLEIPCONFIG, OnToggleIPConfig)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//USER MESSAGE
//카운터 보드 네트워크 에러 처리
LRESULT CModeDlg::OnCountBordError(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedRadStop();
	ChangeState(enInspStop);

	return 0;
}
// CellTrack ToggleIPConfig Dlg
LRESULT CModeDlg::OnToggleIPConfig(WPARAM wParam, LPARAM lParam)
{
	ToggleIPConfig();

	return 0;
}
// CModeDlg 메시지 처리기

void CModeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect	btnRect;
	int		nBtnHeight = 0;
	int		nBtnWidth = 0;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_ST_LOGO);
	if (pStatic != nullptr) {
		pStatic->GetClientRect(&btnRect);
		pFrame->m_nTopPanelHeight = btnRect.Height();

	}

	//Exit 버튼 이동
	CMFCButton* pButton = (CMFCButton*)GetDlgItem(IDC_BTN_EXIT);
	if (pButton != nullptr) {

		pButton->GetClientRect(&btnRect);
		nBtnHeight = btnRect.Height();
		nBtnWidth = btnRect.Width();
		pButton->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}
	//LOG 버튼 이동
	cx -= 10;
	pButton = (CMFCButton*)GetDlgItem(IDC_BTN_LOG);
	if (pButton != nullptr) {
		pButton->GetClientRect(&btnRect);
		nBtnHeight = btnRect.Height();
		nBtnWidth = btnRect.Width();
		pButton->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	cx -= 10;
	pButton = (CMFCButton*)GetDlgItem(IDC_BTN_INSP_SIM);
	if (pButton != nullptr) {
		pButton->GetWindowRect(btnRect);
		nBtnWidth = btnRect.Width() ;
		pButton->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	CRect stRect;
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_ST_DISK_SPACE);
	cx -= 10;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_LIGHT_STATE);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_IO_STATE);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_PLC_STATE);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_CAMERA_STATE);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_STATE);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}	

	pWnd = (CWnd*)GetDlgItem(IDC_BTN_TACT_TIME);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_CHK_SWITCH_DISP);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}
}


BOOL CModeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_nViewMode = m_pDoc->GetViewMode();

	m_stTitle.SetFont(&m_fontTitle);
	m_stTitle.SetTextColor(RGB(128, 128, 255));
	m_stTitle.SetBackgroundColor(RGB(255, 255, 192));

	CString strDispName;
	strDispName.Format(_T("%s Foil Exposure Vision"), AprData.m_System.m_strMachineID );
	m_stTitle.SetWindowText(strDispName); 

	m_stState.SetFont(&m_fontTitle);
	m_stState.SetTextColor(RGB(64, 64, 64));
	m_stState.SetBackgroundColor(RGB(255, 192, 192));
	m_stState.SetWindowText(_T("STOP"));

	m_stCameraState.SetFont(&m_fontTitle);
	m_stCameraState.SetTextColor(RGB(64, 64, 64));
	m_stCameraState.SetBackgroundColor(RGB(255, 100, 100));

	m_stPlcState.SetFont(&m_fontTitle);
	m_stPlcState.SetTextColor(RGB(64, 64, 64));
	m_stPlcState.SetBackgroundColor(RGB(255, 100, 100));

	m_stIoState.SetFont(&m_fontTitle);
	m_stIoState.SetTextColor(RGB(64, 64, 64));
	m_stIoState.SetBackgroundColor(RGB(255, 100, 100));

	m_stLightState.SetFont(&m_fontTitle);
	m_stLightState.SetTextColor(RGB(64, 64, 64));
	m_stLightState.SetBackgroundColor(RGB(255, 100, 100));

	m_stDiskSpace.SetFont(&m_FontDiskCapa);
	m_stDiskSpace.SetTextColor(RGB(64, 64, 64));
	m_stDiskSpace.SetBackgroundColor(RGB(150, 255, 150));
	m_stDiskSpace.SetWindowText(_T("남은용량/총용량(MB)\n0.0/0.0(MB)"));

	m_cBtnExit.PreSubclassWindow();
	m_cBtnExit.LoadBitmaps(IDB_APP_CLOSE, IDB_APP_CLOSE);

	m_cbtnLog.EnableWindowsTheming(FALSE);
	m_cbtnLog.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_cbtnLog.SetFaceColor(RGB(240, 255, 255), TRUE);

	switch (AprData.m_System.m_nLogo)
	{
	case 0:
		m_cLogo.SetBitmap(IDB_LGES_LOGO);
		break;
	case 1:
		m_cLogo.SetBitmap(IDB_ULTIUMCELLS_LOGO);
		break;
	case 2:
		m_cLogo.SetBitmap(IDB_HLI_LOGO);
		break;
	default:
		m_cLogo.SetBitmap(IDB_LGES_LOGO);
		break;
	}

	DisplayLanguage(); 

	CWnd* pWndChild;
	pWndChild = GetTopWindow();
	while (pWndChild != NULL) {
		pWndChild->SetFont(&AprData.m_font);
		pWndChild = pWndChild->GetNextWindow();
	}

	CWnd* pWnd;
	if (AprData.UserMode == UserModeDefine::enOperator) {
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_INSP_SIM);
		pWnd->ShowWindow(SW_HIDE);
	}

	m_radInspRun.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_radInspRun.m_nAlignStyle = CMFCButton::ALIGN_LEFT;
	m_radInspRun.SetImage(IDB_INSP_RUN_S);
	m_radInspRun.m_bTransparent = FALSE;
	m_radInspRun.SetMouseCursorHand();
	m_radInspRun.m_bTopImage = FALSE;
	m_radInspRun.m_bRightImage = FALSE;
	m_radInspRun.SetTooltip(_T("개시"));

	m_radInspStop.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_radInspStop.m_nAlignStyle = CMFCButton::ALIGN_LEFT;
	m_radInspStop.SetImage(IDB_INSP_STOP_S);
	m_radInspStop.m_bTransparent = FALSE;
	m_radInspStop.SetMouseCursorHand();
	m_radInspStop.m_bTopImage = FALSE;
	m_radInspStop.m_bRightImage = FALSE;
	m_radInspStop.SetTooltip(_T("종료"));

	ChangeState(enInspStop);

	SetTimer(AUTO_START_TIMER, AUTO_START_DELAY, NULL);

	initCelltrack();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CModeDlg::OnBnClickedRadiInspMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDoc != NULL) {
		m_pDoc->SetViewMode(enInspMode);
	}
	m_nViewMode = m_pDoc->GetViewMode();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->ShowResultPanel(TRUE);

	m_pView->ChangeViewMode(m_nViewMode);
}


void CModeDlg::OnBnClickedRadHistoryMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDoc != NULL) {
		m_pDoc->SetViewMode(enHistoryMode);
	}
	m_nViewMode = m_pDoc->GetViewMode();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->ShowResultPanel(FALSE);
	m_pView->ChangeViewMode(m_nViewMode);
}


void CModeDlg::OnBnClickedRadCondMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

#if 1 // 240227 kjk 추가
	CUserLoginDlg dlg;
	dlg.DoModal();

	if (dlg.bLogin == FALSE) {
		OnBnClickedRadiInspMode();
		Refresh();
		return;
	}

#endif

	if (m_pDoc != NULL) {
		m_pDoc->SetViewMode(enCondMode);
	}
	m_nViewMode = m_pDoc->GetViewMode();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->ShowResultPanel(FALSE);
	m_pView->ChangeViewMode(m_nViewMode);
}

void CModeDlg::OnBnClickedBtnInspSim()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (theApp.m_pImageSimDlg == NULL) 
	{
		theApp.m_pImageSimDlg = new CImageProcSimDlg(this);
		theApp.m_pImageSimDlg->Create(IDD_DLG_INSP_SIM, this);

	}
	if (theApp.m_pImageSimDlg != NULL) 
	{
		theApp.m_pImageSimDlg->ShowWindow(SW_SHOW);
	}
}


void CModeDlg::OnBnClickedBtnLog()
{
	CIoMonitorDlg dlg(this);
 	dlg.DoModal();
}


void CModeDlg::OnBnClickedBtnExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( MessageBox( _T("Do you want to exit the application?"), _T("NotchingGradeInsp"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		AprData.SaveErrorLog(_T("============================= SHUTDOWN PROGRAM ============================="));
		AprData.SaveDebugLog_Format(_T("============================= SHUTDOWN PROGRAM ============================="));

		LOGDISPLAY_SPEC(8)(_T("## SHUTDOWN PROGRAM ============================="));

		AfxGetMainWnd()->PostMessageA(WM_CLOSE);
	}
}

void CModeDlg::OnCancel()
{
}

void CModeDlg::Refresh()
{
	// TODO: 여기에 구현 코드 추가.
	CWnd* pWnd;
	if (AprData.UserMode == UserModeDefine::enOperator) {
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_INSP_SIM);
		pWnd->ShowWindow(SW_HIDE);
	} else {
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_INSP_SIM);
		pWnd->ShowWindow(SW_SHOW);
	}

	// 검사중/정지, 각 Device 연결상태 Display

	BOOL bPlc = FALSE ;
	BOOL bIo = FALSE;
	BOOL bLamp = FALSE;
	BOOL bGrabber = TRUE;
	BOOL bRunFlag = FALSE;

#ifndef NEW_PLCTYPE
	if (theApp.m_pPioCtrl != NULL) 
	{
		bPlc = theApp.m_pPioCtrl->IsOpened();
	}
#endif //NEW_PLCTYPE

	if (theApp.m_pIoCtrl != NULL) {
		bIo = theApp.m_pIoCtrl->IsOpened();
	}
	if (theApp.m_pLightCtrl != NULL) {
	//	bLamp = theApp.m_pLightCtrl->IsOpened(); // 231102 연결방식 변경으로 주석.
		if (AprData.m_ErrStatus.m_nErrorType == AprData.m_ErrStatus.en_LampError) bLamp = FALSE;
		else bLamp = TRUE;
	}
	if (theApp.m_pImgProcCtrl != NULL) {
		if (m_pView->IsInspReady() == TRUE)
		{
			bRunFlag = theApp.m_pSigProc->SigInRun();
		}
	}
	if (theApp.m_pImgProcCtrl != NULL)
	{
		for( int i=0; i<MAX_CAMERA_NO; i++ )
		{
			bGrabber &= theApp.m_pImgProcCtrl->GetCameraLinkStatus(i);
		}
	}

	COLORREF clrBk;

	if (m_bRunLastFlag != bRunFlag) {
		CString strState;
		if (bRunFlag == TRUE) {
			clrBk = RGB(192, 255, 192);
			strState = _T("Run" );
		}
		else {
			clrBk = RGB(255, 192, 192);
			strState = _T("Stop");
		}
		m_stState.SetBackgroundColor(clrBk);
		m_stState.SetWindowText(strState);

	}
	if (m_bPlcLastFlag != bPlc) {
		if (bPlc == TRUE) {
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stPlcState.SetBackgroundColor(clrBk);
	}
	if (m_bIoLastFlag != bIo) {
		if (bIo == TRUE) {
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stIoState.SetBackgroundColor(clrBk);
	}

#if 0 //test 231102

	char aa[256] = "";
	sprintf_s(aa, "231102 m_bLampLastFlag =%d, bLamp =%d \n", m_bLampLastFlag, bLamp);
	OutputDebugString(aa);
#endif

//	if (m_bLampLastFlag != bLamp) { // 231103 연결방식 변경으로 주석함.
		if (bLamp == TRUE) {
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stLightState.SetBackgroundColor(clrBk);
		//m_stLightState.SetWindowText(_T("Lamp"));
//	}
	if ( m_bCamLastFlag != bGrabber) {
		if(bGrabber){
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stCameraState.SetBackgroundColor(clrBk);
		//m_stCameraState.SetWindowText(_T("CAM"));
	}

	CString strDiskSpace;
	double dDiskUse = AprData.m_dDiskTotal - AprData.m_dDiskFree ; 
	double dPercent = (dDiskUse / AprData.m_dDiskTotal) * 100.0;
	strDiskSpace.Format(_T("%.1lf/%.1lf(GB)[%.1lf%%]"), dDiskUse, AprData.m_dDiskTotal, dPercent );
	if (dPercent > DISK_SPACE_ALARM_HEAVY)
	{
		// 중알람
		clrBk = RGB(255, 100, 100);
	}
	else if (dPercent > DISK_SPACE_ALARM_LIGHT)
	{
		// 경알람
		clrBk = RGB(255, 127, 0);
	}
	else
	{
		//Normal
		clrBk = RGB(100, 255, 100);
	}
	
	m_stDiskSpace.SetBackgroundColor(clrBk);
	m_stDiskSpace.SetWindowText(strDiskSpace);

	UpdateData(FALSE);
}


BOOL CModeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CModeDlg::OnBnClickedBtnDirectory()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CResultDirectoryDlg dlg;
	dlg.DoModal();
}


void CModeDlg::OnBnClickedRadRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	AprData.SaveDebugLog_Format(_T("BUTTON RUN START"));

	//Button Click Start/Stop 시 Tab Id 초기화
	AprData.m_NowLotData.m_bInspStartStop = TRUE;

	m_pDoc->SetInspState(enInspRun);

	EnableControl(TRUE);

	m_pView->SetInspReady(TRUE);

#if 0 //231101
	theApp.m_pImgProcCtrl->LightON();
#else
	if (AprData.m_System.m_nRS232_Mode == 1) {
		theApp.m_pImgProcCtrl->LightON();
	}
	else {
		// TCP/IP
		int result = 0;

		theApp.m_pLightCtrl->Open();
		result = theApp.m_pImgProcCtrl->LightON();
//		theApp.m_pLightCtrl->Close();  // 원본 240108 
//		OutputDebugString("[OnBnClickedRadRun] SetTimer \n");
		if(result >= 0 && result != 2) SetTimer(T_CHECK_DEVICE, m_nTimerInterval_DeviceCheck, 0);// 240108
		else {
		//OnBnClickedRadStop()
		//	theApp.m_pLightCtrl->Close();
		}
	}

	Refresh();
#endif
}


void CModeDlg::OnBnClickedRadStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AprData.SaveDebugLog_Format(_T("BUTTON RUN STOP"));

	m_pDoc->SetInspState(enInspStop);

	EnableControl(FALSE);

	m_pView->SetInspReady(FALSE);

#if 0
#if 0 //231018 // GrabStop()으로 이동 kjk
	theApp.m_pImgProcCtrl->LightOFF();
#else
	if (theApp.m_pImgProcCtrl->m_bInspFlag == FALSE) theApp.m_pImgProcCtrl->LightOFF(); //시작했다가 바로 종료했을때 처리 231019 kjk
#endif

#else
	if (theApp.m_pImgProcCtrl->m_bInspFlag == FALSE) { //시작했다가 바로 종료했을때 처리 231019 kjk
		if (AprData.m_System.m_nRS232_Mode == 1) { 	//231101
			theApp.m_pImgProcCtrl->LightOFF(); 
		}
		else {
			// TCP/IP
#if 0
			theApp.m_pLightCtrl->Open();
			theApp.m_pImgProcCtrl->LightOFF();
			Sleep(150);
			theApp.m_pLightCtrl->Close();
#else 
			// 240108
			theApp.m_pImgProcCtrl->LightOFF();
			KillTimer(T_CHECK_DEVICE);
#endif
		}
	}

	Refresh();
#endif
}

void CModeDlg::ChangeState(int nStatus)
{
	switch (nStatus) {
	case	enInspRun :
		m_radInspRun.SetCheck(1);
		m_radInspStop.SetCheck(0);
		EnableChild(TRUE);
		EnableControl(TRUE);
		break;
	case	enInspStop :
		m_radInspRun.SetCheck(0);
		m_radInspStop.SetCheck(1);
		EnableChild(FALSE);
		EnableControl(FALSE);
		break;
	default :
		break;
	}
}

void CModeDlg::EnableChild(BOOL bMode)
{
	CWnd* pWnd;

	BOOL bEnableHistory;
	if (bMode == TRUE) {
		// 검사 개시
		bEnableHistory = FALSE;
	}
	else {
		// 검사 종료
		bEnableHistory = TRUE;
	}
	pWnd = (CWnd*)GetDlgItem(IDC_RAD_HISTORY_MODE);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->EnableWindow(bEnableHistory);
	}

}

void CModeDlg::DisplayLanguage()
{
	CWnd* pWnd;
	CString strDispName;
	for (int idx = 0; idx < MAX_COUNT; idx++)
	{
		pWnd = NULL;
		pWnd = GetDlgItem(UiText1NameText[idx]);
		if (pWnd != nullptr)
		{
			strDispName = UiText1[idx][__Lang];
			pWnd->SetWindowTextA(strDispName);
		}
	}
	
}


void CModeDlg::OnBnClickedBtnTactTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_pTactGraph != NULL) {
		delete m_pTactGraph ;
		m_pTactGraph = NULL;
	}

	m_pTactGraph = new CTactTimeGraphDlg;
	m_pTactGraph->Create(IDD_DLG_TACT_TIME_GRAPH, this);
	m_pTactGraph->ShowWindow(SW_SHOW);
}


void CModeDlg::OnBnClickedChkSwitchDisp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_pView->SwitchDisplay(!m_bDispSwitch);
}

void CModeDlg::EnableControl(BOOL bModeRun)
{
	BOOL bShow;
	if (bModeRun == TRUE) {
		bShow = SW_HIDE;
	}
	else {
		bShow = SW_SHOW;
	}
	GetDlgItem(IDC_RAD_HISTORY_MODE)->EnableWindow(bShow);
	GetDlgItem(IDC_BTN_INSP_SIM)->EnableWindow(bShow);
	GetDlgItem(IDC_RAD_COND_MODE)->EnableWindow(bShow);
	GetDlgItem(IDC_BTN_TACT_TIME)->EnableWindow(bShow);

}

void CModeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == AUTO_START_TIMER)
	{
		KillTimer(AUTO_START_TIMER);

#if defined(_DEBUG)
		return;
#endif

		m_bDispSwitch = TRUE;
		m_pView->SwitchDisplay(!m_bDispSwitch);

		OnBnClickedRadRun();
		ChangeState(enInspRun);

		UpdateData(FALSE);

	}
#if 1 //240108
	else if (nIDEvent == T_CHECK_DEVICE) {
		KillTimer(T_CHECK_DEVICE);

		if(CheckDevice() == TRUE) return;

		if(((CButton*)GetDlgItem(IDC_RAD_STOP))->GetCheck() == FALSE) SetTimer(T_CHECK_DEVICE, m_nTimerInterval_DeviceCheck, NULL);

	}
	else;
#endif

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CModeDlg::GetStopCheckFALSE()
{
	return (((CButton*)GetDlgItem(IDC_RAD_STOP))->GetCheck() == FALSE);
}

BOOL CModeDlg::CheckDevice()
{
	OutputDebugString("[CModeDlg] CheckDevice Start \n");
	// LED 장비가 응답하지 않으면 에러를 발생한다.
	if (theApp.m_pLightCtrl->CheckDevice() > 0  /*ping Check error*/) {
		OutputDebugString("[CModeDlg] CheckDevice Error!! \n");
		
		CString strMsg;
		strMsg.Format(_LANG(_T("조명 이상 발생."), _T("Check lamp.")));
		AprData.m_ErrStatus.SetError(CErrorStatus::en_LampError, strMsg); 
	
		return TRUE;
	}
	else {
		;
	}

	return FALSE;
}


BOOL CModeDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::DestroyWindow();
}

BOOL CModeDlg::initCelltrack()
{
	m_CriticalDLL.Lock();
	
	CString strDllPathName;
	::GetCurrentDirectory(MAX_PATH, strDllPathName.GetBuffer(MAX_PATH));
	strDllPathName.ReleaseBuffer();
	strDllPathName += _T('\\');
	// 현재경로
	//strDllPathName += "SPCTransUtilx64.dll";

	if (m_LoadVTECSDll.DllSocket_Initialize(DllSocket_ConnectStatus, DllSocket_ReceiveDataBuffer, false))
		//	if (m_LoadVTECSDll.DllSocket_Initialize(strDllPathName, DllSocket_ConnectStatus, DllSocket_ReceiveDataBuffer, false))
	{
		m_CurrentDllStatus.bInitialzed = true;
		//	AfxMessageBox(strDllPathName);
	}
	else;;//AfxMessageBox("error!!");

	m_CriticalDLL.Unlock();

	return 0;
}

void CModeDlg::ToggleIPConfig()
{
	m_CriticalDLL.Lock();

	m_LoadVTECSDll.DllSocket_IPConfigToggleShow(false, true);

	m_CriticalDLL.Unlock();
}

UINT __stdcall CModeDlg::DllSocket_ConnectStatus(char* strEventContents, int nStatus, int nErrorCode)
{
	
	if (NULL != pTempLoad)
	{
		pTempLoad->PostConnectStatus(strEventContents, nStatus, nErrorCode);
	}	
	
	return 0;
}

void CModeDlg::PostConnectStatus(char* strEventContents, int nStatus, int nErrorCode)
{
	const bool bConnected = enConnectEvent::OnConnected == nStatus;
	if (NULL != strEventContents && m_blastOldConnectStatus != bConnected)
	{
		m_strSocketEvent = strEventContents;
		m_blastOldConnectStatus = bConnected;
		if (::IsWindow(GetSafeHwnd()))
		{
	//		PostMessage(WM_CONNECT_STATUS_CHANGED, m_blastOldConnectStatus, (LPARAM)&m_strSocketEvent);
		}
	}
}

UINT __stdcall CModeDlg::DllSocket_ReceiveDataBuffer(unsigned char* pReceiveBuffer, int nReceiveLengthByByte)
{
	CString mTemp = "";
	mTemp.Format(_T("%s"), pReceiveBuffer);
	mTemp = mTemp.Mid(nReceiveLengthByByte);

	//theApp.m_pMainWnd->SetWindowTextA(mTemp);

	LOGDISPLAY_ALL("[DllSocket_ReceiveDataBuffer] pReceiveBuffer[ %s ]", mTemp);
	AprData.m_NowLotData.m_nCellID = mTemp;

	return 0;
}

void CModeDlg::SubDisplayUpdate(void)
{


	m_CurrentDllStatus.bInitialzed = m_LoadVTECSDll.DllSocket_IsIntialized();

	if (m_OldDllStatus != m_CurrentDllStatus)
	{
		if (m_CurrentDllStatus.bInitialzed != m_OldDllStatus.bInitialzed)
		{
			if (m_CurrentDllStatus.bInitialzed)
			{
		//		SetDlgItemText(IDC_STATIC_DLL_INITIALIZED, _T("Yes"));
				if (m_LoadVTECSDll.DllSocket_IsASCIITypeDll())
				{
		//			SetDlgItemText(IDC_STATIC_TYPE, _T("ASCII Type"));
				}
				else
				{
		//			SetDlgItemText(IDC_STATIC_TYPE, _T("Binary Type"));
				}
			}
			else
			{
	//			SetDlgItemText(IDC_STATIC_DLL_INITIALIZED, _T("No"));
		//		SetDlgItemText(IDC_STATIC_TYPE, _T("---"));
			}

			if (m_CurrentDllStatus.bInitialzed)
			{

			}
		}
		m_OldDllStatus = m_CurrentDllStatus;

	}
}
