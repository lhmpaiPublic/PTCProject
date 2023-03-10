#pragma once

//#include "GridCtrl.h"
#include "ResultViewDlg.h"
// CResultView
class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CResultView : public CDockablePane
{
	DECLARE_DYNAMIC(CResultView)

//�����Դϴ�.
public:
	CResultView(CWnd* pParent = NULL, CNotchingGradeInspView* pView = NULL);

// Ư���Դϴ�.
protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	CResultViewDlg* m_pResultViewDlg;
public:
	int	OnRefresh();
//KANG 22.01.07 Add Start
	CResultViewDlg* GetResultViewDlgPtr() { return m_pResultViewDlg; }
//KANG 22.01.07 Add End

// �����Դϴ�.
public:
	virtual ~CResultView();
	virtual BOOL CanAutoHide() const { return FALSE; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

};


