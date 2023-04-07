
#include "pch.h"
#include "SetValueEdit.h"
#include "SetValueFunc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CSetValueEdit::CSetValueEdit()
{
}


CSetValueEdit::~CSetValueEdit()
{
}


BEGIN_MESSAGE_MAP(CSetValueEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


void CSetValueEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:
	if ( isprint( nChar ) != 0 ) {
		if ( IsInsertMode() != TRUE ) {
			int	nStartChar = 0 ;
			int	nEndChar = 0 ;
			CEdit::GetSel( nStartChar, nEndChar ) ;
			if ( nStartChar > nEndChar ) {
				int	nTemp = nStartChar ;
				nStartChar = nEndChar ;
				nEndChar = nTemp ;
			}
			int	nLength = CEdit::LineLength( -1 ) ;
			if ( ( nStartChar + 1 ) < nEndChar ) {
				TCHAR	szString[ 256 ] ;
				memset( szString, 0x00, sizeof( szString ) ) ;
				CEdit::GetLine( 0, szString, sizeof( szString ) ) ;
				int	nMaxIndex = nLength - nEndChar ;
				int nIndex = 0 ;
				for ( nIndex = 0 ; nIndex < nMaxIndex ; nIndex++ ) {
					szString[ nStartChar + 1 + nIndex ] = szString[ nEndChar + nIndex ] ;
				}
				szString[ nStartChar + 1 + nIndex ] = 0x00 ;
				szString[ nStartChar ] = ' ' ;
				szString[ nStartChar ] = nChar ;
				CEdit::SetSel( 0, -1, FALSE ) ;
				CEdit::ReplaceSel( szString, FALSE ) ;
				CEdit::SetSel( ( nStartChar + 1 ), ( nStartChar + 1 ), FALSE ) ;
				return ;
			}
			if ( nStartChar < nLength ) {
				TCHAR	szString[ 256 ] ;
				memset( szString, 0x00, sizeof( szString ) ) ;
				CEdit::GetLine( 0, szString, sizeof( szString ) ) ;

				if ( nLength >= 256 ) {
					nLength = 255;
				}
				szString[ nLength ] = _T( '\0' ) ;

				if ( ( nStartChar + 1 ) < nLength ) {
					if ( IsMyMbcLegal( &szString[ nStartChar ] ) == TRUE ) {
						for ( int nIndex = ( nStartChar + 2 ) ; nIndex < nLength ; nIndex++ ) {
							szString[ nIndex - 1 ] = szString[ nIndex ] ;
						}
						szString[ nLength - 1 ] = 0x00 ;
					}
				}
				szString[ nStartChar ] = nChar ;
				CEdit::SetSel( 0, -1, FALSE ) ;
				CEdit::ReplaceSel( szString, FALSE ) ;
				CEdit::SetSel( ( nStartChar + 1 ), ( nStartChar + 1 ), FALSE ) ;
				return ;
			}
		}
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags) ;
}


void CSetValueEdit::OnSetFocus( CWnd *pOldWnd )
{
	CEdit::OnSetFocus(pOldWnd) ;

	// TODO:

}


void CSetValueEdit::OnKillFocus( CWnd *pNewWnd )
{
	CEdit::OnKillFocus(pNewWnd) ;

	// TODO:
}


BOOL CSetValueEdit::IsInsertMode( void )
{
	return ( GetInsertMode() ) ;
}
