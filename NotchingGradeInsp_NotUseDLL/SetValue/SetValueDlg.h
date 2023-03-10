
#pragma once


#include "SetValueRes.h"
#include "SetValueEdit.h"


class CSetValueDlg : public CDialog
{

public:
	CSetValueDlg( CWnd *pParent = NULL ) ;

	int SetEventCheckFunc( int ( *eventfunc)( void ) ) ;

	int i_Value ;
	long l_Value ;
	unsigned int u_Value ;
	unsigned long ul_Value ;
	double d_Value ;
	// 22.04.13 Ahn Add Start
	CString st_Value; 
	BOOL m_bModeString; 
	// 22.04.13 Ahn Add End

	int m_BitSize ;

	double m_MinValue ;
	double m_MaxValue ;

	BOOL	m_MouseMoveFlag ;

	enum BITSIZE {
		 INT_SIZE= 15,
		 UINT_SIZE = 16,
		 LONG_SIZE = 31,
		 ULONG_SIZE = 32,
		 DOUBLE_SIZE = 64,
		 STRING_SIZE = 128, // 22.04.13 Ahn Add 
	} ;


public:
	enum { IDD = IDD_SETVALUE } ;
	CSetValueEdit	m_InputValueText ;
	CString	m_HeadStr ;
	CString	m_MaxMinStr ;
	BOOL	m_ChkInsert ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog() ;
	virtual void OnOK() ;
	afx_msg void OnBtn1() ;
	afx_msg void OnBtn2() ;
	afx_msg void OnBtn3() ;
	afx_msg void OnBtn4() ;
	afx_msg void OnBtn5() ;
	afx_msg void OnBtn6() ;
	afx_msg void OnBtn7() ;
	afx_msg void OnBtn8() ;
	afx_msg void OnBtn9() ;
	afx_msg void OnBtn0() ;
	afx_msg void OnBtnPlus() ;
	afx_msg void OnBtnMinus() ;
	afx_msg void OnBtnBs() ;
	afx_msg void OnBtnDot() ;
	afx_msg void OnKillfocusEdInputtext() ;
	afx_msg void OnBtnDel() ;
	afx_msg void OnSetfocusEdInputtext() ;
	afx_msg void OnBtnGoleft() ;
	afx_msg void OnBtnGoright() ;
	afx_msg void OnMouseSet() ;
	afx_msg void OnDestroy() ;
	afx_msg void OnTimer(UINT_PTR nIDEvent) ;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus) ;
	afx_msg void OnChkInsert() ;
	DECLARE_MESSAGE_MAP()

	int	InsertText( CString addmsg ) ;
	int	GotoSelectPosition( CString movemsg ) ;
	int TimerCtrl( BOOL startmode ) ;
	int IsInsertMode( void ) ;

protected:
	int	select_start ;
	int	select_end ;
	UINT_PTR TID_EventCheck ;
	int	(*m_pfEventCheckfunc)( void ) ;

private:
	POINT	PrevMousePoint ;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) ;

} ;
