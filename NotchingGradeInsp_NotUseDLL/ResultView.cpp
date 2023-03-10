// ResultView.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "ResultView.h"

// CResultView
IMPLEMENT_DYNAMIC(CResultView, CDockablePane)

CResultView::CResultView(CWnd* pParent /*= NULL*/, CNotchingGradeInspView* pView /*= NULL*/)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if(m_pView != NULL) {
		m_pDoc = m_pView->GetDocument();
	}
	m_pResultViewDlg = NULL;
}

CResultView::~CResultView()
{
	if (m_pResultViewDlg != nullptr) {
		if (m_pResultViewDlg->m_hWnd != nullptr) {
			m_pResultViewDlg->DestroyWindow();
		}
		delete m_pResultViewDlg;
		m_pResultViewDlg = NULL;
	}
}


BEGIN_MESSAGE_MAP(CResultView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CResultView 메시지 처리기

int CResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (m_pResultViewDlg == nullptr) {
		m_pResultViewDlg = new CResultViewDlg(this, m_pView);
		if (m_pResultViewDlg != nullptr) {
			if (m_pResultViewDlg->Create(IDD_DLG_RESULT, this) == 0) {
				delete m_pResultViewDlg;
				m_pResultViewDlg = NULL;
				return -1;
			}
			else {
				m_pResultViewDlg->ShowWindow(SW_SHOW);
			}
		}
	}

	return 0;
}


void CResultView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	RecalcLayout();
	if (m_pResultViewDlg->GetSafeHwnd()) {
		m_pResultViewDlg->SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, SWP_NOZORDER);
	//KANG 22.01.07 Add Start
		CRect rc;
		m_pResultViewDlg->GetClientRect(&rc);
		m_pResultViewDlg->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
	//KANG 22.01.07 Add End
	}
}


void CResultView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CRect rect;
	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(pFrame->m_clrBkColor));
}

int CResultView::OnRefresh()
{
	// TODO: 여기에 구현 코드 추가.
	int nRet = 0;
	nRet = m_pResultViewDlg->UpdateGrid();
	return nRet;
}

