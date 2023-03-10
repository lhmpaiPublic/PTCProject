#pragma once
//#include <afxdockablepane.h>

class CHistoryLotDlg;
class CNotchingGradeInspDoc;
class CNotchingGradeInspView;

class CHistoryLotView : public CDockablePane
{
	DECLARE_DYNAMIC(CHistoryLotView)


	//�����Դϴ�.
public:
	CHistoryLotView(CWnd* pParent = NULL, CNotchingGradeInspView* pView = NULL);

	// Ư���Դϴ�.
protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	CHistoryLotDlg* m_pHistoryLotDlg;
public:
	int	OnRefresh();
	CHistoryLotDlg* GetResultViewDlgPtr() { return m_pHistoryLotDlg; }

	void RefreshAll();
	// �����Դϴ�.
public:
	virtual ~CHistoryLotView();
	virtual BOOL CanAutoHide() const { return FALSE; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();


};

