#include "pch.h"
#include "CHistoryLotView.h"
#include "CHistoryLotDlg.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"

// CResultView
IMPLEMENT_DYNAMIC(CHistoryLotView, CDockablePane)

CHistoryLotView::CHistoryLotView(CWnd* pParent /*= NULL*/, CNotchingGradeInspView* pView /*= NULL*/)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = m_pView->GetDocument();
	}
	m_pHistoryLotDlg = NULL;
}

CHistoryLotView::~CHistoryLotView()
{
	if (m_pHistoryLotDlg != nullptr) {
		if (m_pHistoryLotDlg->m_hWnd != nullptr) {
			m_pHistoryLotDlg->DestroyWindow();
		}
		delete m_pHistoryLotDlg;
		m_pHistoryLotDlg = NULL;
	}
}


BEGIN_MESSAGE_MAP(CHistoryLotView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CResultView 메시지 처리기

int CHistoryLotView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (m_pHistoryLotDlg == nullptr) {
		m_pHistoryLotDlg = new CHistoryLotDlg(this, m_pView);
		if (m_pHistoryLotDlg != nullptr) {
			if (m_pHistoryLotDlg->Create(IDD_DLG_RESULT, this) == 0) {
				delete m_pHistoryLotDlg;
				m_pHistoryLotDlg = NULL;
				return -1;
			}
			else {
				m_pHistoryLotDlg->ShowWindow(SW_SHOW);
			}
		}
	}

	return 0;
}


void CHistoryLotView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	RecalcLayout();
	if (m_pHistoryLotDlg->GetSafeHwnd()) {
		m_pHistoryLotDlg->SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, SWP_NOZORDER);
		//KANG 22.01.07 Add Start
		CRect rc;
		m_pHistoryLotDlg->GetClientRect(&rc);
		m_pHistoryLotDlg->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
		//KANG 22.01.07 Add End
	}
}

void CHistoryLotView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CRect rect;
	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(pFrame->m_clrBkColor));
}

int CHistoryLotView::OnRefresh()
{
	// TODO: 여기에 구현 코드 추가.
	int nRet = 0;
//	nRet = m_pHistoryLotDlg->UpdateGrid();
	return nRet;
}


void CHistoryLotView::RefreshAll()
{
	m_pHistoryLotDlg->RefreshAll();
}
