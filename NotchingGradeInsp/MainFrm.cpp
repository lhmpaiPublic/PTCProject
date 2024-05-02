
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "MainFrm.h"
#include "GlobalData.h"				// 22.05.25 Son Add
#include "CHistoryLotView.h"		// 22.06.24 Ahn Add
#include "NotchingGradeInspView.h"	// 22.11.09 Ahn Add
#include "LogDisplayDlg.h" //Log 출력창

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum {
	en_MENU_FILE = 0,
	en_MENU_VIEW,
	en_MENU_HELP,
	en_MENU_SETTING,
	en_MENU_DEBUG
};

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.

	//생성 시 인스턴스 객체 세팅
	UIMGR->setMainFrame(this);

	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	m_clrBkColor = RGB(235,238,252);

	m_nTopPanelHeight = 100;

	m_pWndTopPanel = NULL;
	m_pWndBottomPanel = NULL;

	m_pCropDlg = NULL ;
	m_pHistoryDlg = NULL ;
	m_pInspInfoDlg = NULL ;
	m_pResultDlg = NULL ;

	m_pView = NULL;
}

CMainFrame::~CMainFrame()
{
	//소멸 시 인스턴스 객체 삭제
	UIMGR->delMainFrame();

	if (m_pWndTopPanel != nullptr) {
		delete m_pWndTopPanel;
		m_pWndTopPanel = NULL;
	}

	if (m_pCropDlg != nullptr) {
		delete m_pCropDlg;
		m_pCropDlg = NULL;
	}
	if (m_pHistoryDlg != nullptr) {
		delete m_pHistoryDlg;
		m_pHistoryDlg = NULL;
	}
	if (m_pInspInfoDlg != nullptr) {
		delete m_pInspInfoDlg;
		m_pInspInfoDlg = NULL;
	}
	if (m_pResultDlg != nullptr) {
		delete m_pResultDlg;
		m_pResultDlg = NULL;
	}

	if (m_pWndBottomPanel != nullptr) {
		delete m_pWndBottomPanel;
		m_pWndBottomPanel = NULL;
	}

	//Log Dlg  메모리해제
	CLogDisplayDlg::ExitLogDisplayDlg();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ((nID & 0x0FFF0) != SC_CLOSE)
	{
		CFrameWnd::OnSysCommand(nID, lParam);
	}
	//	CFrameWndEx::OnSysCommand(nID, lParam);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

//	BOOL bNameValid;

	SetMenu(NULL); // 메뉴바 없애기

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY); // 22.07.21 Ahn Delete 

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	DWORD dwStyle;

//	m_pWndTopPanel->EnableDocking(CBRS_ALIGN_ANY);
	dwStyle = m_pWndTopPanel->GetControlBarStyle();
	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | AFX_CBRS_RESIZE);
	m_pWndTopPanel->SetControlBarStyle(dwStyle);
	DockPane(m_pWndTopPanel);

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);


	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	ShowWindow(SW_MAXIMIZE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	//cs.style &= ~WS_SYSMENU;
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	
	CNotchingGradeInspView* pView;
	{	
		CDocTemplate* pDocTemplate;
		CDocument* pDoc;
		POSITION pos1, pos2;

		pos1 = theApp.GetFirstDocTemplatePosition();
		if (pos1) {
			pDocTemplate = theApp.GetNextDocTemplate(pos1);
			pos2 = pDocTemplate->GetFirstDocPosition();
			pDoc = pDocTemplate->GetNextDoc(pos2);
			pos2 = pDoc->GetFirstViewPosition();
			m_pDoc = (CNotchingGradeInspDoc*)pDoc;
			pView = (CNotchingGradeInspView*)pDoc->GetNextView(pos2);
			m_pView = pView ; // 22.11.09 Ahn Add

		}
		else {
			return FALSE;
		}
		if (pView == NULL) {
			return FALSE;
		}
	}
	//BOOL bNameValid;

	if (m_pWndTopPanel == NULL) {

		m_pWndTopPanel = new CTopPanel(this, pView);
		CString strCaption;
		strCaption = _T("");
		if (!m_pWndTopPanel->Create(strCaption, this, CRect(0, 0, 0, 0), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
		{
			TRACE0("출력 창을 만들지 못했습니다.\n");
			return FALSE; // 만들지 못했습니다.
		}
	}

	if (m_pCropDlg == NULL) {
		m_pCropDlg = new CCropImageViewDlg(this, pView);
		m_pCropDlg->Create(IDD_DLG_CROP_IMAGE_VIEW, this);
	}
	if (m_pHistoryDlg  == NULL) {
		m_pHistoryDlg = new CHistoryLotDlg(this, pView);
		m_pHistoryDlg->Create(IDD_DLG_HISTORY_LOT, this);
	}
	if (m_pInspInfoDlg == NULL) {
		m_pInspInfoDlg = new CInspInfoDlg(this, pView);
		m_pInspInfoDlg->Create(IDD_DLG_INSP_INFO, this);
	}
	if (m_pResultDlg == NULL) {
		m_pResultDlg = new CResultViewDlg(this, pView);
		m_pResultDlg->Create(IDD_DLG_RESULT, this);
	}
	ShowResultPanel(TRUE);

	SetDockingWindowIcons(theApp.m_bHiColorIcons);

	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{

}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_clrBkColor = RGB(240, 240, 240);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_clrBkColor = RGB(247, 247, 247);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_clrBkColor = RGB(255, 255, 255);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		m_clrBkColor = RGB(196, 229, 255);
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_clrBkColor = RGB(218, 218, 218);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_clrBkColor = RGB(235, 237, 252);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_clrBkColor = RGB(247, 247, 247);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			m_clrBkColor = RGB(191, 219, 255);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			m_clrBkColor = RGB(83, 83, 83);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			m_clrBkColor = RGB(208, 212, 221);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			m_clrBkColor = RGB(196, 202, 217);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 기본 클래스가 실제 작업을 수행합니다.

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
}

int CMainFrame::OnRefresh_HistoryLotView()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_pHistoryDlg != nullptr) {
		m_pHistoryDlg->RefreshAll();
	}
	return 0;
}

void CMainFrame::ReflashAll()
{

	OnRefresh_HistoryLotView();

	m_pWndTopPanel->RefreshAll();

	m_pInspInfoDlg->ReflashAll();
}

//마킹 BCD ID를 갱신한다.
void CMainFrame::UpdateBCDID()
{
	if (CHistoryLotDlg::gsHistoryLotDlg && CHistoryLotDlg::gsHistoryLotDlg->m_hWnd)
	{
		CHistoryLotDlg::gsHistoryLotDlg->UpdateBCDID();
	}
}

//마킹 BCD ID를 출럭한다.
void CMainFrame::UpdateBCDIDData(int nowBCDID, int lastBCDID, int grabBCDID)
{
	if (CHistoryLotDlg::gsHistoryLotDlg && CHistoryLotDlg::gsHistoryLotDlg->m_hWnd)
	{
		CHistoryLotDlg::gsHistoryLotDlg->UpdateBCDIDData(nowBCDID, lastBCDID, grabBCDID);
	}
}

void CMainFrame::ResetAndRefreshAll()
{
	m_pHistoryDlg->RefreshAll();
	m_pCropDlg->ClearAllCropImage();
}

void CMainFrame::ResetResultViewDlg()
{
	m_pResultDlg->CounterReset();
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int nBottomOfTop = 0;
	if (m_pWndTopPanel) {
		int nScreenResolutionH = GetSystemMetrics(SM_CYSCREEN);
		int nOffset = 0;
		if (nScreenResolutionH == 1080) {
			nOffset = 5;
		}
		else if (nScreenResolutionH > 1080) {
			nOffset = 15;
		}
		else {
			nOffset = 5;
		}
		int nHeight = m_nTopPanelHeight + m_pWndTopPanel->m_nTitleHeight + nOffset;
		m_pWndTopPanel->SetWindowPos(this, 0, 0, cx, nHeight, SWP_NOZORDER);
		nBottomOfTop = nHeight;
	}

	int nTop = nBottomOfTop;

	int nLeftHeight = 380;

	CRect rcStatusBar;
	if (m_wndStatusBar.m_hWnd != nullptr) {
		m_wndStatusBar.GetWindowRect(rcStatusBar);
	}

	int nOffset = 3 ; 
	int nTopPos = nBottomOfTop + nOffset;

	// 좌 
	int nMapHeight = cy - nBottomOfTop - 20;
	int nMapRight = 800 ;

	int nCropHeight = (int)(nMapHeight * 0.37);
	int nCropBottom = nCropHeight + nBottomOfTop ;

	int nDefListHeight = (int)(nMapHeight * 0.4);
	int nDefListBottom = nCropBottom + nDefListHeight ;

	int nCropWidth = (int)( ( cx - nMapRight ) * 0.67 ) ;
	int nInfoWidth = cx - nMapRight - nCropWidth ;

	int nHistHeight = (int)(nMapHeight * 0.23);
	int nDefListWidth = cx - nMapRight ;
	int nInfoStartX = nMapRight + nCropWidth ;

	m_cpInspDispPos.x = 0 ;
	m_cpInspDispPos.y = 0 ;
	m_cpMapSize.y = nMapHeight ;
	m_cpMapSize.x = nMapRight - 20 ; // 30은 Scroll 폭

	if (m_pCropDlg != NULL) {
		m_pCropDlg->SetWindowPos(this, nMapRight, nTopPos, nCropWidth, nCropHeight, SWP_NOZORDER);
	}
	if (m_pInspInfoDlg != NULL) {
		m_pInspInfoDlg->SetWindowPos(this, nInfoStartX, nTopPos, nInfoWidth, nCropHeight, SWP_NOZORDER);
	}
	if (m_pResultDlg != NULL) {
		m_pResultDlg->SetWindowPos(this, nMapRight, nCropBottom, nDefListWidth, nDefListHeight, SWP_NOZORDER);
	}
	if (m_pHistoryDlg != NULL) {
		m_pHistoryDlg->SetWindowPos(this, nMapRight, nDefListBottom, nDefListWidth, nHistHeight, SWP_NOZORDER);
	}

	if (m_pView != NULL) {
		CRect rcMapSize;
		rcMapSize.left = 0;
		rcMapSize.right = nMapRight;
		rcMapSize.top = 0;
		rcMapSize.bottom = m_cpMapSize.y;
		m_pView->SetMapDlgSize(rcMapSize);
	}

}

int CMainFrame::ShowResultPanel(BOOL bShow)
{
	if (m_pInspInfoDlg != nullptr) {
		m_pInspInfoDlg->ShowWindow(bShow);
	}
	;
	if (m_pCropDlg != nullptr) {
		m_pCropDlg->ShowWindow(bShow);
	}

	if (m_pResultDlg != nullptr) {
		m_pResultDlg->ShowWindow(bShow);
	}
	if (m_pHistoryDlg != nullptr) {
		m_pHistoryDlg->ShowWindow(bShow);
	}

	return 0;
}

int CMainFrame::OnRefresh_CropImageView()
{
	// TODO: 여기에 구현 코드 추가.
	return 0;
}

void CMainFrame::LoadHistoryData()
{
	m_pDoc->LoadHistory();
}

