#pragma once


// 22.06.24 Ahn Modify Start
//#include "GraphDlg.h"
//class CHistoryLotDlg;
class CResultViewDlg;
// 22.06.24 Ahn Modify End

// CRightPanel
class CNotchingGradeInspView;
class CRightPanel : public CDockablePane
{
	DECLARE_DYNAMIC(CRightPanel)

// �����Դϴ�.
public:
	CRightPanel(CWnd* pParent = NULL, CNotchingGradeInspView* pView = NULL);

// Ư���Դϴ�.
protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	// 22.06.24 Ahn Modify Start
	//CGraphDlg* m_pGraphDlg;
	//CHistoryLotDlg* m_pHistoryLotDlg;
	CResultViewDlg* m_pResultViewDlg;
	// 22.06.24 Ahn Modify End

public:
	int ResetAll();
	int	RefreshAll();

// �����Դϴ�.
public:
	virtual ~CRightPanel();
	virtual BOOL CanAutoHide() const { return FALSE; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


