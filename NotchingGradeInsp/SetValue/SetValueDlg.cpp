
#include "pch.h"
#include "SetValueDlg.h"
#include "SetValueFunc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#if !defined( IS_LANG_JAPANESE )
	#define IS_LANG_JAPANESE() FALSE
#endif
#if !defined( _LANG )
	#define _LANG( eng, jpn ) _T( eng )
#endif


#define		T_ID_EVENTCHECK_SETVALUE			211


CSetValueDlg::CSetValueDlg( CWnd *pParent /*= NULL*/ )
	: CDialog(CSetValueDlg::IDD, pParent)
{
	m_HeadStr = _T("") ;
	m_MaxMinStr = _T("") ;
	m_ChkInsert = FALSE ;

	select_start = 0 ;
	select_end = 0 ;
	m_MouseMoveFlag = FALSE ;
	TID_EventCheck = 0 ;
	m_pfEventCheckfunc = NULL ;

	m_bModeString = FALSE; // 22.04.13 Ahn Add
}


void CSetValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX) ;
	DDX_Control(pDX, IDC_ED_INPUTTEXT, m_InputValueText) ;
	DDX_Text(pDX, IDC_ST_CAPTION, m_HeadStr) ;
	DDX_Text(pDX, IDC_CAP_TEXT, m_MaxMinStr) ;
	DDX_Check(pDX, IDC_CHK_INSERT, m_ChkInsert) ;
}


BEGIN_MESSAGE_MAP(CSetValueDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_1, OnBtn1)
	ON_BN_CLICKED(IDC_BTN_2, OnBtn2)
	ON_BN_CLICKED(IDC_BTN_3, OnBtn3)
	ON_BN_CLICKED(IDC_BTN_4, OnBtn4)
	ON_BN_CLICKED(IDC_BTN_5, OnBtn5)
	ON_BN_CLICKED(IDC_BTN_6, OnBtn6)
	ON_BN_CLICKED(IDC_BTN_7, OnBtn7)
	ON_BN_CLICKED(IDC_BTN_8, OnBtn8)
	ON_BN_CLICKED(IDC_BTN_9, OnBtn9)
	ON_BN_CLICKED(IDC_BTN_0, OnBtn0)
	ON_BN_CLICKED(IDC_BTN_PLUS, OnBtnPlus)
	ON_BN_CLICKED(IDC_BTN_MINUS, OnBtnMinus)
	ON_BN_CLICKED(IDC_BTN_BS, OnBtnBs)
	ON_BN_CLICKED(IDC_BTN_DOT, OnBtnDot)
	ON_EN_KILLFOCUS(IDC_ED_INPUTTEXT, OnKillfocusEdInputtext)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_EN_SETFOCUS(IDC_ED_INPUTTEXT, OnSetfocusEdInputtext)
	ON_BN_CLICKED(IDC_BTN_GOLEFT, OnBtnGoleft)
	ON_BN_CLICKED(IDC_BTN_GORIGHT, OnBtnGoright)
	ON_COMMAND( 100, OnMouseSet)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHK_INSERT, OnChkInsert)
END_MESSAGE_MAP()


BOOL CSetValueDlg::OnInitDialog()
{

	CDialog::OnInitDialog() ;

	if ( IS_LANG_JAPANESE() == FALSE ) {
		CWnd	*pWnd = NULL ;
		pWnd = ( CWnd *)this ;
		pWnd->SetWindowText( _T( "Parameter setting" ) ) ;
		pWnd = ( CWnd *)GetDlgItem( IDCANCEL ) ;
		pWnd->SetWindowText( _T( "CANCEL" ) ) ;
	}

	CString	strtext = _T( "" ) ;

	switch ( m_BitSize ) {
	case	( int)INT_SIZE :
		m_MaxMinStr.Format( _T( "%10.0lf ? %10.0lf" ), m_MinValue, m_MaxValue ) ;
		strtext.Format( _T( "%d" ), i_Value ) ;
		m_InputValueText.SetWindowText( strtext ) ;
		break ;
	case	( int)UINT_SIZE :
		m_MaxMinStr.Format( _T( "%10.0lf ? %10.0lf" ), m_MinValue, m_MaxValue ) ;
		strtext.Format( _T( "%u" ), u_Value ) ;
		m_InputValueText.SetWindowText( strtext ) ;
		break ;
	case	( int)LONG_SIZE :
		m_MaxMinStr.Format( _T( "%10.0lf ? %10.0lf" ), m_MinValue, m_MaxValue ) ;
		strtext.Format( _T( "%ld" ), l_Value ) ;
		m_InputValueText.SetWindowText( strtext ) ;
		break ;
	case	( int)ULONG_SIZE :
		m_MaxMinStr.Format( _T( "%10.0lf ? %10.0lf" ), m_MinValue, m_MaxValue ) ;
		strtext.Format( _T( "%lu" ), ul_Value ) ;
		m_InputValueText.SetWindowText( strtext ) ;
		break ;
	case	( int)DOUBLE_SIZE :
		m_MaxMinStr.Format( _T( "%10.3lf ? %10.3lf" ), m_MinValue, m_MaxValue ) ;
		strtext.Format( _T( "%lf" ), d_Value ) ;
		m_InputValueText.SetWindowText( strtext ) ;
		break ;
	// 22.04.13 Ahn Add Start
	case	STRING_SIZE :
		m_InputValueText.SetWindowText(st_Value);
		break;
	// 22.04.13 Ahn Add End
	default :
		ASSERT( FALSE ) ;
		return ( FALSE ) ;
	}

	m_ChkInsert = IsInsertMode() ;

	UpdateData( FALSE ) ;

	if ( m_MouseMoveFlag == TRUE ) {
		PostMessage( WM_COMMAND, 100, 0L ) ;
	}

	return ( TRUE ) ;

}


void CSetValueDlg::OnDestroy()
{

	CDialog::OnDestroy() ;

	// TODO: 
	if ( m_MouseMoveFlag == TRUE ) {
		::SetCursorPos( PrevMousePoint.x , PrevMousePoint.y ) ;
	}

}


void CSetValueDlg::OnOK()
{

	// TODO:
	TCHAR	buff[ 128 ], temp[ 32 ] ;
	int		i ;
	int	nowlength ;
	BOOL	cancelflag = FALSE ;

// DLG->Class
	UpdateData( TRUE ) ;

	nowlength = m_InputValueText.GetWindowTextLength() ;
	if ( ( nowlength <= 0 ) || ( nowlength >= 64 ) ) {
		AfxMessageBox( _T("입력값을 확인해 주세요..")
			, ( MB_OK | MB_ICONEXCLAMATION ) ) ;
		m_InputValueText.SetFocus() ;
		return ;
	}

	if (m_bModeString == TRUE) {
		CString strValue;
		m_InputValueText.GetWindowTextA(st_Value);
		CDialog::OnOK();
		return;
	}

	CString	str ;
	m_InputValueText.GetWindowText( str ) ;
	_stprintf_s( buff, _T( "%s" ), ( LPCTSTR)str ) ;
	for ( i = 0 ; i < ( int)_tcslen( buff ) ; i++ ) {
		switch ( buff[ i ] ) {
		case	_T( ' ' ) :
		case	_T( '0' ) :
		case	_T( '1' ) :
		case	_T( '2' ) :
		case	_T( '3' ) :
		case	_T( '4' ) :
		case	_T( '5' ) :
		case	_T( '6' ) :
		case	_T( '7' ) :
		case	_T( '8' ) :
		case	_T( '9' ) :
		case	_T( '.' ) :
		case	_T( '+' ) :
		case	_T( '-' ) :
		case	_T( '*' ) :
		case	_T( '/' ) :
		case	_T( '(' ) :
		case	_T( ')' ) :
			continue ;
		default :
			AfxMessageBox( _T( "입력문자를 확인해 주세요..") 
				, ( MB_OK | MB_ICONEXCLAMATION ) ) ;
			m_InputValueText.SetFocus() ;
			return ;
		}
	}

	if ( CalclateExpressionString( str ) < 0 ) {
		AfxMessageBox( _T( "입력 수식을 확인해 주세요.." ) 
			, ( MB_OK | MB_ICONEXCLAMATION ) ) ;
		m_InputValueText.SetFocus() ;
		return ;
	}
	_stprintf_s( buff, _T( "%s" ), str.GetBuffer(0) ) ;

	switch ( m_BitSize ) {

	case	( int)INT_SIZE :
		if ( _stscanf_s( buff, _T( "%d" ), ( int *)temp ) == 1 ) {
			if ( ( *( ( int *)temp ) <= ( int)m_MaxValue ) &&
			     ( *( ( int *)temp ) >= ( int)m_MinValue ) ) {
				i_Value = ( int)( *( ( int *)temp ) ) ;
			} else {
				cancelflag = TRUE ;
			}
		} else {
			cancelflag = TRUE ;
		}
		break ;

	case	( int)UINT_SIZE :
		if ( _stscanf_s( buff, _T( "%u" ), ( unsigned int *)temp ) == 1 ) {
			if ( ( *( ( unsigned int *)temp ) <= ( unsigned int)m_MaxValue ) &&
			     ( *( ( unsigned int *)temp ) >= ( unsigned int)m_MinValue ) ) {
				u_Value = ( unsigned int)( *( ( unsigned int *)temp ) ) ;
			} else {
				cancelflag = TRUE ;
			}
		} else {
			cancelflag = TRUE ;
		}
		break ;

	case	( int)LONG_SIZE :
		if ( _stscanf_s( buff, _T( "%ld" ), ( long *)temp ) == 1 ) {
			if ( ( *( (long *)temp ) <= ( long)m_MaxValue ) &&
			     ( *( (long *)temp ) >= ( long)m_MinValue ) ) {
				l_Value = ( long)( *( ( long *)temp ) ) ;
			} else {
				cancelflag = TRUE ;
			}
		} else {
			cancelflag = TRUE ;
		}
		break ;

	case	( int)ULONG_SIZE :
		if ( _stscanf_s( buff, _T( "%lu" ), ( unsigned long *)temp ) == 1 ) {
			if ( ( *( ( unsigned long *)temp ) <= ( unsigned long)m_MaxValue ) &&
			     ( *( ( unsigned long *)temp ) >= ( unsigned long)m_MinValue ) ) {
				ul_Value = ( unsigned long)( *( ( unsigned long *)temp ) ) ;
			} else {
				cancelflag = TRUE ;
			}
		} else {
			cancelflag = TRUE ;
		}
		break ;

	case	( int)DOUBLE_SIZE :
		if ( _stscanf_s( buff, _T( "%lf" ), ( double *)temp ) == 1 ) {
			if ( ( *( ( double *)temp ) <= ( double)m_MaxValue ) &&
			     ( *( ( double *)temp ) >= ( double)m_MinValue ) ) {
				d_Value = ( double)( *( ( double *)temp ) ) ;
			} else {
				cancelflag = TRUE ;
			}
		} else {
			cancelflag = TRUE ;
		}
		break ;

	default :
		cancelflag = TRUE ;
		m_InputValueText.SetFocus() ;
		return ;

	}
	if ( cancelflag == FALSE ) {
		CDialog::OnOK() ;
	} else {
		CString	warnmsg = _T( "" ) ;
		warnmsg.Format( _LANG( "Please confirm the input.(%lf~%lf)", "(%lf~%lf)" )
			, ( double)m_MinValue, ( double)m_MaxValue ) ;
		AfxMessageBox( warnmsg, ( MB_OK | MB_ICONEXCLAMATION ) ) ;
		m_InputValueText.SetFocus() ;
	}

}


void CSetValueDlg::OnBtn1()
{
	InsertText( _T( "1" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn2()
{
	InsertText( _T( "2" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn3()
{
	InsertText( _T( "3" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn4()
{
	InsertText( _T( "4" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn5()
{
	InsertText( _T( "5" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn6()
{
	InsertText( _T( "6" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn7()
{
	InsertText( _T( "7" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn8()
{
	InsertText( _T( "8" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn9()
{
	InsertText( _T( "9" ) ) ;

	return ;
}


void CSetValueDlg::OnBtn0()
{
	InsertText( _T( "0" ) ) ;

	return ;
}


void CSetValueDlg::OnBtnPlus()
{
	InsertText( _T( "+" ) ) ;

	return ;
}


void CSetValueDlg::OnBtnMinus()
{
	InsertText( _T( "-" ) ) ;

	return ;
}


void CSetValueDlg::OnBtnDot()
{
	InsertText( _T( "." ) ) ;

	return ;
}


void CSetValueDlg::OnBtnBs()
{
	InsertText( _T( "BS" ) ) ;

	return ;
}


void CSetValueDlg::OnBtnDel()
{
	InsertText( _T( "DEL" ) ) ;

	return ;
}


void CSetValueDlg::OnBtnGoleft()
{
	GotoSelectPosition( _T( "←" ) ) ;

	return ;
}


void CSetValueDlg::OnBtnGoright()
{
	GotoSelectPosition( _T( "→" ) ) ;

	return ;
}


void CSetValueDlg::OnMouseSet()
{
	int	xpos, ypos ;

	::GetCursorPos( &PrevMousePoint ) ;
	RECT	brect ;
	CButton	*pButton ;
	pButton = ( CButton *)GetDlgItem( IDOK ) ;
	pButton->GetWindowRect( &brect ) ;
	xpos = ( brect.left + brect.right ) / 2 ;
	ypos = ( brect.top + brect.bottom ) / 2 ;
	::SetCursorPos( xpos, ypos ) ;

	return ;
}


int	CSetValueDlg::InsertText( CString addmsg )
{
	CString	temptext ;

	UpdateData( TRUE ) ;

	m_InputValueText.GetWindowText( temptext ) ;

	if ( ( addmsg == _T( "BS" ) )
	  || ( addmsg == _T( "DEL" ) ) ) {
		if ( select_end != select_start ) {
			temptext.Delete( select_start, abs( select_end - select_start ) ) ;
			select_end = select_start ;
		} else {
			if ( temptext.GetLength() >= 1 ) {
				if ( addmsg == _T( "BS" ) ) {
					if ( select_start >= 1 ) {
						int	nDeleteLength = 1 ;
						if ( ( select_start - 1 ) >= 1 ) {
							if ( IsMyMbcLegal( temptext, ( select_start - 2 ) ) == TRUE ) {
								nDeleteLength++ ;
								select_start -= 1 ;
							}
						}
						select_start -= 1 ;
						temptext.Delete( select_start, nDeleteLength ) ;
						select_end = select_start ;
					}
				} else if ( addmsg == _T( "DEL" ) ) {
					if ( select_start < temptext.GetLength() ) {
						int	nDeleteLength = 1 ;
						if ( ( select_start + 1 ) < temptext.GetLength() ) {
							if ( IsMyMbcLegal( temptext, select_start ) == TRUE ) {
								nDeleteLength++ ;
							}
						}
						temptext.Delete( select_start, nDeleteLength ) ;
					}
				}
			}
		}
	} else {
		if ( select_end > select_start ) {
			temptext.Delete( select_start, ( select_end - select_start ) ) ;
		} else {
			if ( IsInsertMode() != TRUE ) {
				int	nAddLength = addmsg.GetLength() ;
				int	nLength = temptext.GetLength() ;
				if ( ( select_start + nAddLength ) < nLength ) {
					if ( IsMyMbcLegal( temptext, ( select_start + nAddLength - 1 ) ) == TRUE ) {
						nAddLength++ ;
					}
					temptext.Delete( select_start, nAddLength ) ;
				} else {
					temptext = temptext.Left( select_start ) ;
				}
			}
		}
		temptext.Insert( select_start, addmsg ) ;
		select_start += addmsg.GetLength() ;
		select_end = select_start ;

	}

	m_InputValueText.SetWindowText( temptext ) ;

	UpdateData( FALSE ) ;

	m_InputValueText.SetFocus() ;

	return ( 0 ) ;

}


int CSetValueDlg::GotoSelectPosition( CString movemsg )
{

	if ( select_start == select_end ) {
		CString	temptext ;

		UpdateData( TRUE ) ;

		m_InputValueText.GetWindowText( temptext ) ;

		if ( movemsg == _T( "←" ) ) {
			if ( select_start >= 1 ) {
				if ( select_start >= 2 ) {
					if ( IsMyMbcLegal( temptext, ( select_start - 2 ) ) == TRUE ) {
						select_start -= 1 ;
					}
				}
				select_start -= 1 ;
				select_end = select_start ;
			}
		} else if ( movemsg == _T( "→" ) ) {
			if ( select_start < temptext.GetLength() ) {
				if ( ( select_start + 1 ) < temptext.GetLength() ) {
					if ( IsMyMbcLegal( temptext, ( select_start ) ) == TRUE ) {
						select_start += 1 ;
					}
				}
				select_start += 1 ;
				select_end = select_start ;
			}
		}
		m_InputValueText.SetFocus() ;
		m_InputValueText.SetSel( select_start, select_end ) ;

	} else {
		m_InputValueText.SetFocus() ;
		m_InputValueText.SetSel( select_start, select_end ) ;
	}

	return ( 0 ) ;

}


void CSetValueDlg::OnSetfocusEdInputtext()
{

	if ( ( select_start == 0 ) && ( select_end == 0 ) ) {
		if ( IsInsertMode() != TRUE ) {
			m_InputValueText.SetSel( select_start, select_end ) ;
		}
	} else {
		m_InputValueText.SetSel( select_start, select_end ) ;
	}

	{
		HDC hDC = ::GetDC( m_InputValueText.GetSafeHwnd() ) ;
		if ( hDC != NULL ) {
			CDC mdc ;
			mdc.CreateCompatibleDC( CDC::FromHandle( hDC ) ) ;
			CFont* pOldFont = mdc.SelectObject( m_InputValueText.GetFont() ) ;
			CSize text_size = mdc.GetTextExtent( CString( _T( "0" ) ) ) ;
			if ( IsInsertMode() ) {
				::CreateCaret( m_InputValueText.GetSafeHwnd(), NULL, 1, text_size.cy ) ;
			} else {
				::CreateCaret( m_InputValueText.GetSafeHwnd(), NULL, text_size.cx, text_size.cy ) ;
			}
			mdc.SelectObject( pOldFont ) ;
			::ReleaseDC( m_InputValueText.GetSafeHwnd(), hDC ) ;
		}
	}

	::ShowCaret( m_InputValueText.GetSafeHwnd() ) ;
}


void CSetValueDlg::OnKillfocusEdInputtext()
{
	int	itemp ;

	m_InputValueText.GetSel( select_start, select_end ) ;

	if ( select_start > select_end ) {
		itemp = select_start ;
		select_start = select_end ;
		select_end = itemp ;
	}

	::DestroyCaret() ;
}


int CSetValueDlg::SetEventCheckFunc( int ( *eventfunc)( void ) )
{
	int	nRet = 0 ;
	m_pfEventCheckfunc = eventfunc ;
	return ( nRet ) ;
}


void CSetValueDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:
	if ( nIDEvent == TID_EventCheck ) {
		TimerCtrl( FALSE ) ;
		if ( *m_pfEventCheckfunc != NULL ) {
			if ( ( *m_pfEventCheckfunc )() != 0 ) {
				PostMessage( WM_COMMAND, IDCANCEL, 0L ) ;
			}
		}
		TimerCtrl( TRUE ) ;
	}

	CDialog::OnTimer(nIDEvent) ;
}


void CSetValueDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus) ;

	// TODO:
	if ( ( bShow == TRUE ) || ( nStatus == SW_PARENTOPENING ) ) {
		TimerCtrl( TRUE ) ;
	} else {
		TimerCtrl( FALSE ) ;
	}
}


int CSetValueDlg::TimerCtrl( BOOL startmode )
{
	int	nRet = 0 ;
	if ( startmode == TRUE ) {
		if ( TID_EventCheck == 0 ) {
			TID_EventCheck = SetTimer( T_ID_EVENTCHECK_SETVALUE, 200, NULL ) ;
			if ( TID_EventCheck == 0 ) {
				nRet = -1 ;
			}
		} else {
			nRet = -1 ;
		}
	} else {
		if ( TID_EventCheck == 0 ) {
			nRet = -1 ;
		} else {
			if ( KillTimer( TID_EventCheck ) == 0 ) {
				nRet = -1 ;
			}
			TID_EventCheck = 0 ;
		}
	}
	return ( nRet ) ;
}


BOOL CSetValueDlg::IsInsertMode( void )
{
	return ( GetInsertMode() ) ;
}


void CSetValueDlg::OnChkInsert()
{
	UpdateData( TRUE ) ;
	SetInsertMode( m_ChkInsert ) ;
	m_InputValueText.SetFocus() ;
}


BOOL CSetValueDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:
	if ( pMsg->message == WM_KEYUP ) {
		switch ( pMsg->wParam ) {
		case	VK_INSERT :
			{
				CWnd* pWnd = ( CWnd *)GetDlgItem( IDC_CHK_INSERT ) ;
				if ( ( pWnd != NULL ) && ( pWnd->m_hWnd != NULL ) ) {
					pWnd->SetFocus() ;
					m_ChkInsert = !m_ChkInsert ;
					UpdateData( FALSE ) ;
					OnChkInsert() ;
				}
			}
			return ( TRUE ) ;
		default :
			break ;
		}
	}

	return CDialog::PreTranslateMessage(pMsg) ;
}
