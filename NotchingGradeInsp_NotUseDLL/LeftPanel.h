#pragma once

#include "InspInfoDlg.h"

// CLeftPanel
class CNotchingGradeInspView;
class CLeftPanel : public CDockablePane
{
	DECLARE_DYNAMIC(CLeftPanel)

//생성입니다.
public:
	CLeftPanel(CWnd* pParent = NULL, CNotchingGradeInspView* pView = NULL);

// 특성입니다.
protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	CInspInfoDlg* m_pInspInfoDlg ;

// 구현입니다.
public:
	virtual ~CLeftPanel();
	virtual BOOL CanAutoHide() const { return FALSE; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	void RefreshAll();
};


