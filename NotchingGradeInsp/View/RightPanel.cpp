// RightPanel.cpp: 구현 파일
//

#include "pch.h"
#include "MainFrm.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspView.h"
#include "RightPanel.h"
#include "GlobalData.h" // 22.06.20 Ahn Add
#include "CHistoryLotDlg.h" // 22.06.24 Ahn Add

// CRightPanel

IMPLEMENT_DYNAMIC(CRightPanel, CDockablePane)

CRightPanel::CRightPanel(CWnd* pParent /*= NULL*/, CNotchingGradeInspView* pView /*= NULL*/)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = m_pView->GetDocument();
	}

	//m_pHistoryLotDlg = NULL;
	m_pResultViewDlg = NULL;
}

CRightPanel::~CRightPanel()
{
	if (m_pResultViewDlg != nullptr) {
		if (m_pResultViewDlg->m_hWnd != nullptr) {
			m_pResultViewDlg->DestroyWindow();
		}
		delete m_pResultViewDlg;
		m_pResultViewDlg = NULL;
	}
}


BEGIN_MESSAGE_MAP(CRightPanel, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CRightPanel 메시지 처리기

int CRightPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
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


void CRightPanel::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	RecalcLayout();
	if (m_pResultViewDlg->GetSafeHwnd()) {
		m_pResultViewDlg->SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, SWP_NOZORDER);
	}
}


void CRightPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CRect rect;
	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(pFrame->m_clrBkColor));
}

int CRightPanel::RefreshAll()
{
	// TODO: 여기에 구현 코드 추가.
	int nRet = 0;
//	m_pResultViewDlg->OnRefresh();
	return nRet;
}

int CRightPanel::ResetAll()
{
//	m_pResultViewDlg->DataClearAll();
//	m_pResultViewDlg->OnRefresh();
	return 0;
}
