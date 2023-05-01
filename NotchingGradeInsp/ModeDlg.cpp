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
// 22.06.13 Ahn Add Start
#include "ImageProcessCtrl.h"
// 22.06.13 Ahn Add End
// CModeDlg 대화 상자
// 22.06.30 Ahn Add Start
#include "PioCtrl.h"
#include "StdIoCtrl.h"
#include "CLightControl.h"
#include "GrabberCtrl.h"
// 22.06.30 Ahn Add End
// 22.07.25 Ahn Add Start
#include "SigProc.h"
// 22.07.25 Ahn Add End
// 22.07.26 Ahn Add Start
#include "CIoMonitorDlg.h"
// 22.07.26 Ahn Add End
// 22.12.09 Ahn Add Start
#include "CTactTimeGraphDlg.h"
// 22.12.09 Ahn Add End

IMPLEMENT_DYNAMIC(CModeDlg, CDialogEx)

CModeDlg::CModeDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_MODE, pParent)
	, m_bDispSwitch(FALSE)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = pView->GetDocument();
	}
	m_fontTitle.CreatePointFont(200, _T("Arial"));
	m_nViewMode = enInspMode;

	// 22.07.04 Ahn Add Start
	m_FontDiskCapa.CreatePointFont(10, _T("Arial"));
	// 22.07.04 Ahn Add End

	// 22.07.01 Ahn Add Start
	m_bPlcLastFlag = FALSE ;
	m_bIoLastFlag = FALSE ;
	m_bLampLastFlag = FALSE ;
	m_bCamLastFlag = FALSE ;
	//m_bInspStatusLastFlag = FALSE;
	// 22.07.01 Ahn Add End
	// 22.07.07 Ahn Add Start
	m_bRunLastFlag = FALSE ;
	// 22.07.07 Ahn Add End

	// 22.08.03 Ahn Add Start
	m_pIoMonitDlg = NULL;
	// 22.08.03 Ahn Add End

	// 22.12.12 Ahn Add start
	m_pTactGraph = NULL ;
	// 22.12.12 Ahn Add End

}

CModeDlg::~CModeDlg()
{
	// 22.12.12 Ahn Add start
	if (m_pTactGraph != NULL) {
		delete m_pTactGraph ;
		m_pTactGraph = NULL;
	}
	// 22.12.12 Ahn Add End
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
	// 22.06.30 Ahn Add Start
	DDX_Control(pDX, IDC_ST_CAMERA_STATE, m_stCameraState);
	DDX_Control(pDX, IDC_ST_PLC_STATE, m_stPlcState);
	DDX_Control(pDX, IDC_ST_IO_STATE, m_stIoState);
	DDX_Control(pDX, IDC_ST_LIGHT_STATE, m_stLightState);
	DDX_Control(pDX, IDC_ST_DISK_SPACE, m_stDiskSpace);
	// 22.06.30 Ahn Add End


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
END_MESSAGE_MAP()


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

	// 22.05.25 Son Add Start
	cx -= 10;
	pButton = (CMFCButton*)GetDlgItem(IDC_BTN_INSP_SIM);
	if (pButton != nullptr) {
		pButton->GetWindowRect(btnRect);
		nBtnWidth = btnRect.Width() ;
		pButton->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}
	// 22.05.25 Son Add End

	// 22.06.30 Ahn Add Start
	CRect stRect;
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_ST_DISK_SPACE);
	cx -= 10;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}
	// 22.06.30 Ahn Add End

	// 22.07.21 Ahn Add Start
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
	// 22.07.21 Ahn Add End
	// 22.12.09 Ahn Add Start
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_TACT_TIME);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}
	// 22.12.09 Ahn Add End
	// 23.02.09 Ahn Add Start
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_SWITCH_DISP);
	cx -= 2;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(stRect);
		nBtnWidth = stRect.Width();
		pWnd->MoveWindow(cx = cx - nBtnWidth, 0, nBtnWidth, nBtnHeight);
	}
	// 23.02.09 Ahn Add End
}


BOOL CModeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_nViewMode = m_pDoc->GetViewMode();

	m_stTitle.SetFont(&m_fontTitle);
	m_stTitle.SetTextColor(RGB(128, 128, 255));
	m_stTitle.SetBackgroundColor(RGB(255, 255, 192));

	// 22.06.14 Ahn Add Start
	//m_stTitle.SetWindowText(_T("Notching Foil Exposure Vision")); // 22.05.30 Ahn Modify 
	CString strDispName;
	strDispName.Format(_T("%s Foil Exposure Vision"), AprData.m_System.m_strMachineID );
	m_stTitle.SetWindowText(strDispName); 
	// 22.06.14 Ahn Add End

	m_stState.SetFont(&m_fontTitle);
	m_stState.SetTextColor(RGB(64, 64, 64));
	m_stState.SetBackgroundColor(RGB(255, 192, 192));
	m_stState.SetWindowText(_T("STOP"));

	// 22.06.30 Ahn Add Start
	m_stCameraState.SetFont(&m_fontTitle);
	m_stCameraState.SetTextColor(RGB(64, 64, 64));
	m_stCameraState.SetBackgroundColor(RGB(255, 100, 100));
	m_stCameraState.SetWindowText(_T("CAM"));

	m_stPlcState.SetFont(&m_fontTitle);
	m_stPlcState.SetTextColor(RGB(64, 64, 64));
	m_stPlcState.SetBackgroundColor(RGB(255, 100, 100));
	m_stPlcState.SetWindowText(_T("PLC"));

	m_stIoState.SetFont(&m_fontTitle);
	m_stIoState.SetTextColor(RGB(64, 64, 64));
	m_stIoState.SetBackgroundColor(RGB(255, 100, 100));
	m_stIoState.SetWindowText(_T("I/O"));

	m_stLightState.SetFont(&m_fontTitle);
	m_stLightState.SetTextColor(RGB(64, 64, 64));
	m_stLightState.SetBackgroundColor(RGB(255, 100, 100));
	m_stLightState.SetWindowText(_T("Lamp"));
	// 22.06.30 Ahn Add End

	// 22.07.04 Ahn Add Start
	m_stDiskSpace.SetFont(&m_FontDiskCapa);
	m_stDiskSpace.SetTextColor(RGB(64, 64, 64));
	m_stDiskSpace.SetBackgroundColor(RGB(150, 255, 150));
	m_stDiskSpace.SetWindowText(_T("남은용량/총용량(MB)\n0.0/0.0(MB)"));
	// 22.07.04 Ahn Add End

	m_cBtnExit.PreSubclassWindow();
	m_cBtnExit.LoadBitmaps(IDB_APP_CLOSE, IDB_APP_CLOSE);

	m_cbtnLog.EnableWindowsTheming(FALSE);
	m_cbtnLog.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_cbtnLog.SetFaceColor(RGB(240, 255, 255), TRUE);

	// 23.02.27 Son Mod Start
	//// 23.01.12 Ahn Modify Start
	////m_cLogo.SetBitmap(IDB_LGES_LOGO);
	////m_cLogo.SetBitmap(IDB_ULTIUMCELLS_LOGO);
	//m_cLogo.SetBitmap(IDB_HLI_LOGO); // 23.02.24 Ahn Add
	//// 23.01.12 Ahn Modify End
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
	// 23.02.27 Son Mod End

	DisplayLanguage(); // 22.09.01 Ahn Add 

// 22.05.25 Son Add Start
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
// 22.05.25 Son Add End

	// 22.06.13 Ahn Add Start
	m_radInspRun.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_radInspRun.m_nAlignStyle = CMFCButton::ALIGN_LEFT;
	m_radInspRun.SetImage(IDB_INSP_RUN_S);
	//	m_rdInspRun.SetCheckedImage(IDB_INSP_RUN);
	m_radInspRun.m_bTransparent = FALSE;
	m_radInspRun.SetMouseCursorHand();
	m_radInspRun.m_bTopImage = FALSE;
	m_radInspRun.m_bRightImage = FALSE;
	m_radInspRun.SetTooltip(_T("개시"));

	m_radInspStop.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_radInspStop.m_nAlignStyle = CMFCButton::ALIGN_LEFT;
	m_radInspStop.SetImage(IDB_INSP_STOP_S);
	//	m_rdInspStop.SetCheckedImage(IDB_INSP_STOP);
	m_radInspStop.m_bTransparent = FALSE;
	m_radInspStop.SetMouseCursorHand();
	m_radInspStop.m_bTopImage = FALSE;
	m_radInspStop.m_bRightImage = FALSE;
	m_radInspStop.SetTooltip(_T("종료"));
	// 22.06.13 Ahn Add End

	// 22.06.15 Ahn Add Start
	ChangeState(enInspStop);
	// 22.06.15 Ahn Add End

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CModeDlg::OnBnClickedRadiInspMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//KANG 22.01.07 Modify Start
	//m_nViewMode = enInspMode;
	if (m_pDoc != NULL) {
		m_pDoc->SetViewMode(enInspMode);
	}
	m_nViewMode = m_pDoc->GetViewMode();
//KANG 22.01.07 Modify End
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	// 22.07.21 Ahn Modify Start
	//pFrame->ShowLeftPanel(TRUE);
	//// 22.06.06 Ahn Add Start
	//pFrame->ShowCropImagePanel(TRUE);
	//// 22.06.06 Ahn Add End
	//pFrame->ShowRightPanel(TRUE);
	//pFrame->ShowResultViewPanel(TRUE);
	pFrame->ShowResultPanel(TRUE);
	// 22.07.21 Ahn Modify End

	m_pView->ChangeViewMode(m_nViewMode);
}


void CModeDlg::OnBnClickedRadHistoryMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//KANG 22.01.07 Modify Start
	//m_nViewMode = enHistoryMode;
	if (m_pDoc != NULL) {
		m_pDoc->SetViewMode(enHistoryMode);
	}
	m_nViewMode = m_pDoc->GetViewMode();
//KANG 22.01.07 Modify End
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
// 22.07.21 Ahn Modify Start
//	pFrame->ShowLeftPanel(FALSE);
//	pFrame->ShowRightPanel(FALSE);
////KANG 22.05.24 Modify Start
////	pFrame->ShowResultViewPanel(TRUE);
//	BOOL bView = (m_pDoc->GetHistoryDispMode() != enHistoryDisp_List) ? TRUE : FALSE;
//	pFrame->ShowResultViewPanel(bView);
////KANG 22.05.24 Modify End
//// 22.06.06 Ahn Add Start
//	pFrame->ShowCropImagePanel(FALSE);
//// 22.06.06 Ahn Add End
	pFrame->ShowResultPanel(FALSE);
// 22.07.21 Ahn Modify End
	m_pView->ChangeViewMode(m_nViewMode);
}


void CModeDlg::OnBnClickedRadCondMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//KANG 22.01.07 Modify Start
	//m_nViewMode = enCondMode;
	if (m_pDoc != NULL) {
		m_pDoc->SetViewMode(enCondMode);
	}
	m_nViewMode = m_pDoc->GetViewMode();
//KANG 22.01.07 Modify End
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	// 22.07.21 Ahn Modify Start
	//pFrame->ShowLeftPanel(FALSE);
	//pFrame->ShowRightPanel(FALSE);
	//pFrame->ShowResultViewPanel(FALSE);
	//pFrame->ShowCropImagePanel(FALSE);
	pFrame->ShowResultPanel(FALSE);
	// 22.07.21 Ahn Modify End
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
	//pyjtest
// 	CSigProc* pSigProc = theApp.m_pSigProc;
// 
// 	if(pSigProc->GetConnectZone() == FALSE )
// 	{
// 		pSigProc->SetConnectZone(TRUE);
// 	}
// 	else
// 	{
// 		pSigProc->SetConnectZone(FALSE);
// 	}
// 
// 	return;




	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CSystemSettingDlg dlg;
//	CDIOTestDlg dlg;
//	dlg.DoModal();

// 	CIoMonitorDlg dlg(this);
// 	dlg.DoModal();



	// 22.08.03 Ahn Add Start
	if (m_pIoMonitDlg == nullptr) {
		m_pIoMonitDlg = new CIoMonitorDlg(this);
		m_pIoMonitDlg->Create(IDD_DLG_IO_MONITOR, this);
	}
	m_pIoMonitDlg->ShowWindow(SW_SHOW);

	// 22.08.03 Ahn Add End

}


void CModeDlg::OnBnClickedBtnExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( MessageBox( _T("Do you want to exit the application?"), _T("NotchingGradeInsp"), MB_ICONQUESTION | MB_YESNO) == IDYES ) {
		AfxGetMainWnd()->PostMessageA(WM_CLOSE);
	}
}
// 22.05.09 Ahn Add Start
void CModeDlg::OnCancel()
{
}
// 22.05.09 Ahn Add End

// 22.05.25 Son Add Start
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

	// 22.06.30 Ahn Add Start
	// 검사중/정지, 각 Device 연결상태 Display

	BOOL bPlc = FALSE ;
	BOOL bIo = FALSE;
	BOOL bLamp = FALSE;
	BOOL bGrabber = FALSE;
	BOOL bRunFlag = FALSE;

	if (theApp.m_pPioCtrl != NULL) {
		bPlc = theApp.m_pPioCtrl->IsOpened();
	}
	if (theApp.m_pIoCtrl != NULL) {
		bIo = theApp.m_pIoCtrl->IsOpened();
	}
	if (theApp.m_pLightCtrl != NULL) {
		bLamp = theApp.m_pLightCtrl->IsOpened();
	}
	if (theApp.m_pImgProcCtrl != NULL) {
		// 22.07.25 Ahn Modify Start
		if (m_pView->IsInspReady() == TRUE) {
			CSigProc* pSigProc = theApp.m_pSigProc;
			bRunFlag = pSigProc->SigInRun();
		}
		// 22.07.25 Ahn Modify End
	}

	// 22.07.01 Ahn Add Start
	COLORREF clrBk;
	//COLORREF clrText;
	//clrText = RGB(64, 64, 64);

	// 22.07.07 Ahn Add Start
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
	// 22.07.07 Ahn Add End
	if (m_bPlcLastFlag != bPlc) {
		if (bPlc == TRUE) {
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stPlcState.SetBackgroundColor(clrBk);
		m_stPlcState.SetWindowText(_T("PLC"));
	}
	if (m_bIoLastFlag != bIo) {
		if (bIo == TRUE) {
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stIoState.SetBackgroundColor(clrBk);
		m_stIoState.SetWindowText(_T("I/O"));
	}
	if (m_bLampLastFlag != bLamp) {
		if (bLamp == TRUE) {
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stLightState.SetBackgroundColor(clrBk);
		m_stLightState.SetWindowText(_T("Lamp"));
	}
	if ( m_bCamLastFlag != bGrabber) {
		if(bGrabber){
			clrBk = RGB(100, 255, 100);
		}
		else {
			clrBk = RGB(255, 100, 100);
		}
		m_stCameraState.SetBackgroundColor(clrBk);
		m_stCameraState.SetWindowText(_T("CAM"));
	}
	// 22.07.01 Ahn Add End

	// 22.07.04 Ahn Add Start
	CString strDiskSpace;
	double dDiskUse = AprData.m_dDiskTotal - AprData.m_dDiskFree ; 
	double dPercent = (dDiskUse / AprData.m_dDiskTotal) * 100.0;
	strDiskSpace.Format(_T("%.1lf/%.1lf(GB)[%.1lf%%]"), dDiskUse, AprData.m_dDiskTotal, dPercent );
	if (AprData.m_dDiskFree < 100.0) {
		clrBk = RGB(255, 100, 100);
	}
	else {
		clrBk = RGB(100, 255, 100);
	}
	m_stCameraState.SetBackgroundColor(clrBk);
	m_stDiskSpace.SetWindowText(strDiskSpace);
	// 22.07.04 Ahn Add End


	UpdateData(FALSE);
	// 22.06.30 Ahn Add End
}
// 22.05.25 Son Add End


//KANG 22.05.24 Add Start
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
//KANG 22.05.24 Add End

void CModeDlg::OnBnClickedBtnDirectory()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CResultDirectoryDlg dlg;
	dlg.DoModal();
}


void CModeDlg::OnBnClickedRadRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		
	m_pDoc->SetInspState(enInspRun);

	// 22.07.07 Ahn Delete Start
	//if (theApp.m_pImgProcCtrl->GrabStart() < 0) {
	//	MessageBox(_T("이미 Grabber가 동작중 입니다."));
	//}
	// 22.07.07 Ahn Delete End

	EnableControl(TRUE);

	m_pView->SetInspReady(TRUE);
}


void CModeDlg::OnBnClickedRadStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pDoc->SetInspState(enInspStop);

	// 22.07.07 Ahn Delete Start
	//theApp.m_pImgProcCtrl->GrabStop();
	// 22.07.07 Ahn Delete End

	EnableControl(FALSE);

	m_pView->SetInspReady(FALSE);

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
	
	pWnd = GetDlgItem(IDC_RADI_INSP_MODE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("검사모드"), _T("Inspect Mode")));
	}
	pWnd = GetDlgItem(IDC_RAD_COND_MODE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("설정모드"), _T("Recipe Setting")));
	}
	pWnd = GetDlgItem(IDC_BTN_DIRECTORY);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("저장경로"), _T("Directory")));
	}
	pWnd = GetDlgItem(IDC_RAD_RUN);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("시작"), _T("Run")));
	}
	pWnd = GetDlgItem(IDC_RAD_STOP);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("정지"), _T("Stop")));
	}
	pWnd = GetDlgItem(IDC_BTN_INSP_SIM);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("시뮬레이션"), _T("Simulation")));
	}

}


void CModeDlg::OnBnClickedBtnTactTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 22.12.12 Ahn Add start
	//CTactTimeGraphDlg* m_pTactGraph;

	if (m_pTactGraph != NULL) {
		delete m_pTactGraph ;
		m_pTactGraph = NULL;
	}

	m_pTactGraph = new CTactTimeGraphDlg;
	m_pTactGraph->Create(IDD_DLG_TACT_TIME_GRAPH, this);
	m_pTactGraph->ShowWindow(SW_SHOW);
	// 22.12.12 Ahn Add End

//	dlg
}


void CModeDlg::OnBnClickedChkSwitchDisp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_pView->SwitchDisplay(!m_bDispSwitch);
}

// 23.02.16 Ahn Add Start
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
// 23.02.16 Ahn Add End
