#pragma once

#include "ModeDlg.h"

// CTopPanel
class CNotchingGradeInspView;
class CTopPanel : public CDockablePane
{
	DECLARE_DYNAMIC(CTopPanel)

// 생성입니다.
public:
	CTopPanel(CWnd* pParent = NULL, CNotchingGradeInspView* pView = NULL);

// 특성입니다.
protected:
	CWnd*		m_pParent;
	CNotchingGradeInspView* m_pView;
	CModeDlg*	m_pModeDlg ;

public:
	int			m_nTitleHeight;

// 구현입니다.
public:
	virtual ~CTopPanel();
	virtual BOOL CanAutoHide() const { return FALSE; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	void RefreshAll();

public :
	// 22.06.13 Ahn Add Start
	void ChangeState(int nStatus) {
		m_pModeDlg->ChangeState(nStatus);
	};
	// 22.06.13 Ahn Add End
};


