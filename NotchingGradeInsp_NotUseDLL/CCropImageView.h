#pragma once
//#include <afxdockablepane.h>
#include "CCropImageViewDlg.h"

class CNotchingGradeInspDoc;
class CNotchingGradeInspView;

class CCropImageView : public CDockablePane
{
	DECLARE_DYNAMIC(CCropImageView)

public :
	CCropImageView(CWnd* pParent = NULL, CNotchingGradeInspView* pView = NULL);

protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc ;
	CNotchingGradeInspView* m_pView ;
	CCropImageViewDlg* m_pCropImgViewDlg ;
public:
	int	OnRefresh();
	void ClearAllData();
	CCropImageViewDlg* GetCropImageViewDlgPtr() { return m_pCropImgViewDlg; }

	// 구현입니다.
public:
	virtual ~CCropImageView();
	virtual BOOL CanAutoHide() const { return FALSE; }

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};

