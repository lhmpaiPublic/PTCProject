
#include "pch.h"
#include "SetValueFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CTerm
{
public:
	TCHAR cOperator ;
	double dValue ;
protected:
	void Init( void )
	{
		cOperator = 0 ;
		dValue = 0.0 ;
	}
	void Copy( const CTerm& src )
	{
		cOperator = src.cOperator ;
		dValue = src.dValue ;
	}
public:
	CTerm()
	{
		Init() ;
	}
	virtual ~CTerm()
	{
		//
	}
	CTerm& operator=( const CTerm& src )
	{
		if ( this == &src ) {
			return ( *this ) ;
		}
		Copy( src ) ;
		return ( *this ) ;
	}
	CTerm( const CTerm& src )
	{
		Init() ;
		Copy( src ) ;
	}
} ;


typedef	CArray< CTerm, CTerm& >	CTermArray ;

int CalclateExpression( CTermArray& terms, int nStartIndex = 0 ) ;
BOOL IsNumericalCharacter( TCHAR c ) ;
int BasicArithmeticOperations( CTermArray& terms, int nIndex, int nStartIndex, TCHAR cOperator ) ;
static BOOL g_bInsertMode = TRUE ;


void SetInsertMode( BOOL bInsert )
{
	g_bInsertMode = bInsert ;
}


BOOL GetInsertMode( void )
{
	return ( g_bInsertMode ) ;
}


int SetKeyLock( BYTE byteTargetKey, BOOL bState, BOOL *pbBackState /*= NULL*/ )
{
	int	nRet = 0 ;
	BOOL	bBackState = GetKeyLock( byteTargetKey ) ;
	if ( pbBackState != NULL ) {
		*pbBackState = bBackState ;
	}
	if ( bState != bBackState ) {
		keybd_event( byteTargetKey, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 ) ;
		keybd_event( byteTargetKey, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 ) ;
	}
	return ( nRet ) ;
}


BOOL GetKeyLock( BYTE byteTargetKey )
{
	BYTE	keyState[ 256 ] ;
	memset( keyState, 0x00, sizeof( keyState ) ) ;
	GetKeyboardState( keyState ) ;
	BOOL	bState = ( keyState[ byteTargetKey ] & 0x01 ) ;
	return ( bState ) ;
}


int IsMyMbcLegal( LPCTSTR lpszStr, int nStart /*= 0*/ )
{
	if ( lpszStr == NULL ) {
		ASSERT( FALSE ) ;
		return ( FALSE ) ;
	}
	const	BYTE	*pByte = ( BYTE *)&lpszStr[ nStart ] ;
	if ( ( ( *pByte >= 0x81 ) && ( *pByte <= 0x9f ) )
	  || ( ( *pByte >= 0xe0 ) && ( *pByte <= 0xfc ) ) ) {
		//
	} else {
		return ( FALSE ) ;
	}
	pByte = ( BYTE *)&lpszStr[ nStart + 1 ] ;
	if ( ( ( *pByte >= 0x40 ) && ( *pByte <= 0x7e ) )
	  || ( ( *pByte >= 0x80 ) && ( *pByte <= 0xfc ) ) ) {
		  //
	} else {
		return ( FALSE ) ;
	}
	return ( TRUE ) ;
}


int CalclateExpressionString( CString& rstrExpression )
{
	int nRet = 0 ;

	CString str = rstrExpression ;

	int i ;
	for ( i = 0 ; i < str.GetLength() ; i++ ) {
		TCHAR c = str.GetAt( i ) ;
		if ( isdigit( c ) == 0 ) {
			switch ( c ) {
			case	_T( '(' ) :
			case	_T( ')' ) :
			case	_T( '*' ) :
			case	_T( '/' ) :
			case	_T( '+' ) :
			case	_T( '-' ) :
			case	_T( '.' ) :
				break ;
			default	:
				return ( -1 ) ;
			}
		}
	}

	CTermArray terms ;

	for ( i = 0 ; i < str.GetLength() ; i++ ) {
		if ( IsNumericalCharacter( str.GetAt( i ) ) == FALSE ) {
			CTerm term ;
			term.cOperator = str.GetAt( i ) ;
			terms.Add( term ) ;
		} else {
			CTerm term ;
			if ( _stscanf_s( str.Mid( i ), _T( "%lf" ), &term.dValue ) == 1 ) {
				terms.Add( term ) ;
				i++ ;
				for ( ; i < str.GetLength() ; i++ ) {
					if ( IsNumericalCharacter( str.GetAt( i ) ) == FALSE ) {
						i-- ;
						break ;
					}
				}
			}
		}
	}

	nRet = CalclateExpression( terms ) ;

	if ( nRet == 0 ) {
		rstrExpression.Format( _T( "%lf" ), terms[ 0 ].dValue ) ;
	}

	return ( nRet ) ;
}


int CalclateExpression( CTermArray& terms, int nStartIndex /*= 0*/ )
{
	int nRet = 0 ;
	int i ;

	for ( i = nStartIndex ; i < terms.GetSize() ; i++ ) {
		if ( terms[ i ].cOperator == _T( '(' ) ) {
			if ( CalclateExpression( terms, ( i + 1 ) ) < 0 ) {
				nRet = -1 ;
			}
			terms.RemoveAt( i ) ;
			i = nStartIndex ;
			continue ;
		}
	}

	for ( i = nStartIndex ; i < terms.GetSize() ; i++ ) {
		if ( terms[ i ].cOperator == _T( ')' ) ) {
			break ;
		}
		if ( ( terms[ i ].cOperator == _T( '*' ) ) || ( terms[ i ].cOperator == _T( '/' ) ) ) {
			if ( BasicArithmeticOperations( terms, i, nStartIndex, terms[ i ].cOperator ) < 0 ) {
				nRet = -1 ;
			}
			i = nStartIndex ;
			continue ;
		}
	}

	for ( i = nStartIndex ; i < terms.GetSize() ; i++ ) {
		if ( terms[ i ].cOperator == _T( ')' ) ) {
			terms.RemoveAt( i ) ;
			break ;
		}
		if ( ( terms[ i ].cOperator == _T( '+' ) ) || ( terms[ i ].cOperator == _T( '-' ) ) ) {
			if ( BasicArithmeticOperations( terms, i, nStartIndex, terms[ i ].cOperator ) < 0 ) {
				nRet = -1 ;
			}
			i = nStartIndex ;
			continue ;
		}
	}

	return ( nRet ) ;
}


BOOL IsNumericalCharacter( TCHAR c )
{
	if ( isdigit( c ) || c == _T( '.' ) ) {
		return ( TRUE ) ;
	}
	return ( FALSE ) ;
}


int BasicArithmeticOperations( CTermArray& terms, int nIndex, int nStartIndex, TCHAR cOperator )
{
	if ( ( nIndex + 1 ) >= terms.GetSize() ) {
		terms.RemoveAt( nIndex ) ;
		return ( -1 ) ;
	}
	if ( terms[ nIndex + 1 ].cOperator != _T( '\0' ) ) {
		terms.RemoveAt( nIndex ) ;
		return ( -1 ) ;
	}
	CTerm term ;
	if ( nIndex > nStartIndex ) {
		switch ( cOperator ) {
		case	_T( '*' ) :
			term.dValue = terms[ nIndex - 1 ].dValue * terms[ nIndex + 1 ].dValue ;
			break ;
		case	_T( '/' ) :
			term.dValue = terms[ nIndex - 1 ].dValue / terms[ nIndex + 1 ].dValue ;
			break ;
		case	_T( '+' ) :
			term.dValue = terms[ nIndex - 1 ].dValue + terms[ nIndex + 1 ].dValue ;
			break ;
		case	_T( '-' ) :
			term.dValue = terms[ nIndex - 1 ].dValue - terms[ nIndex + 1 ].dValue ;
			break ;
		default	:
			return ( -1 ) ;
		}
		terms[ nIndex - 1 ] = term ;
		terms.RemoveAt( nIndex ) ;
		terms.RemoveAt( nIndex ) ;
	} else {
		switch ( cOperator ) {
		case	_T( '*' ) :
			term.dValue = 1 * terms[ nIndex + 1 ].dValue ;
			break ;
		case	_T( '/' ) :
			term.dValue = 1 / terms[ nIndex + 1 ].dValue ;
			break ;
		case	_T( '+' ) :
			term.dValue = 0 + terms[ nIndex + 1 ].dValue ;
			break ;
		case	_T( '-' ) :
			term.dValue = 0 - terms[ nIndex + 1 ].dValue ;
			break ;
		default	:
			return ( -1 ) ;
		}
		terms[ nIndex ] = term ;
		terms.RemoveAt( nIndex + 1 ) ;
	}
	return ( 0 ) ;
}
