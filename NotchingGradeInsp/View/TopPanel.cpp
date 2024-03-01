// TopPanel.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"
#include "NotchingGradeInspView.h"
#include "TopPanel.h"


// CTopPanel

IMPLEMENT_DYNAMIC(CTopPanel, CDockablePane)

CTopPanel::CTopPanel(CWnd* pParent /*= NULL*/, CNotchingGradeInspView* pView /*= NULL*/)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pModeDlg = NULL;
	m_nTitleHeight = 0;
}

CTopPanel::~CTopPanel()
{
	if ( m_pModeDlg != nullptr ) {
		if ( m_pModeDlg->m_hWnd != nullptr ) {
			m_pModeDlg->DestroyWindow();
		}
		delete m_pModeDlg ;
		m_pModeDlg = NULL ;
	}
}


BEGIN_MESSAGE_MAP(CTopPanel, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CTopPanel 메시지 처리기

int CTopPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	if (m_pModeDlg == nullptr) {
		m_pModeDlg = new CModeDlg(this, m_pView);
		if (m_pModeDlg != nullptr) {
			if (m_pModeDlg->Create(IDD_DLG_MODE, this) == 0) {
				delete m_pModeDlg;
				m_pModeDlg = NULL;
				return -1;
			}
			else {
				m_pModeDlg->ShowWindow(SW_SHOW);
			}
		}
	}

	int iEdge;
	iEdge = GetSystemMetrics(SM_CYEDGE);
	m_nTitleHeight = GetSystemMetrics(SM_CYCAPTION) + iEdge;
	
	return 0;
}


void CTopPanel::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	RecalcLayout();
	if (m_pModeDlg->GetSafeHwnd()) {
		m_pModeDlg->SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, SWP_NOZORDER);
	}

}


void CTopPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CRect rect;
	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(pFrame->m_clrBkColor));
}

// 22.05.25 Son Add Start
void CTopPanel::RefreshAll()
{
	// TODO: 여기에 구현 코드 추가.
	m_pModeDlg->Refresh();
}
// 22.05.25 Son Add End
