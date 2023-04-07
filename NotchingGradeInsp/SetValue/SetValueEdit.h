
#pragma once

class CSetValueEdit : public CEdit
{

public:
	CSetValueEdit() ;
	virtual ~CSetValueEdit() ;

protected:
	BOOL IsInsertMode( void ) ;

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	DECLARE_MESSAGE_MAP()

} ;
