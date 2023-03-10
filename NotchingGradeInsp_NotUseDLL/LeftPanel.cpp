// LeftPanel.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "LeftPanel.h"


// CLeftPanel

IMPLEMENT_DYNAMIC(CLeftPanel, CDockablePane)

CLeftPanel::CLeftPanel(CWnd* pParent /*= NULL*/, CNotchingGradeInspView* pView /*= NULL*/)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = m_pView->GetDocument();
	}
	m_pInspInfoDlg = NULL;
}

CLeftPanel::~CLeftPanel()
{
	if (m_pInspInfoDlg != nullptr) {
		if (m_pInspInfoDlg->m_hWnd != nullptr) {
			m_pInspInfoDlg->DestroyWindow();
		}
		delete m_pInspInfoDlg;
		m_pInspInfoDlg = NULL;
	}
}


BEGIN_MESSAGE_MAP(CLeftPanel, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CLeftPanel 메시지 처리기




int CLeftPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_pInspInfoDlg = new CInspInfoDlg(this, m_pView);
	if (m_pInspInfoDlg != nullptr) {
		if (m_pInspInfoDlg->Create(IDD_DLG_INSP_INFO, this) == 0) {
			delete m_pInspInfoDlg;
			m_pInspInfoDlg = NULL;
			return -1;
		}
		else {
			m_pInspInfoDlg->ShowWindow(SW_SHOW);
		}
	}

	return 0;
}


void CLeftPanel::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pInspInfoDlg->SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, SWP_NOZORDER);
}


void CLeftPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CRect rect;
	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(pFrame->m_clrBkColor));
}

void CLeftPanel::RefreshAll()
{
	m_pInspInfoDlg->ReflashAll();
}
