#pragma once


// CBottomPanel
class CNotchingGradeInspView;
class CBottomPanel : public CDockablePane
{
	DECLARE_DYNAMIC(CBottomPanel)

// 생성입니다.
public:
	CBottomPanel(CWnd* pParent = NULL, CNotchingGradeInspView* pView = NULL);

// 특성입니다.
protected:
	CWnd* m_pParent;
	CNotchingGradeInspView* m_pView;

// 구현입니다.
public:
	virtual ~CBottomPanel();
	virtual BOOL CanAutoHide() const { return FALSE; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


