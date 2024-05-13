// BottomPanel.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"
#include "NotchingGradeInspView.h"
#include "BottomPanel.h"


// CBottomPanel

IMPLEMENT_DYNAMIC(CBottomPanel, CDockablePane)

CBottomPanel::CBottomPanel(CWnd* pParent /*= NULL*/, CNotchingGradeInspView* pView /*= NULL*/)
{
	m_pParent = pParent;
	m_pView = pView;
}

CBottomPanel::~CBottomPanel()
{
}


BEGIN_MESSAGE_MAP(CBottomPanel, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CBottomPanel 메시지 처리기




int CBottomPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CBottomPanel::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CBottomPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CRect rect;
	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(pFrame->m_clrBkColor));
}
