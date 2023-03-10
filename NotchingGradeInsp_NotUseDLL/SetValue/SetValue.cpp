#include "pch.h"
#include "SetValue.h"
#include "SetValueDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CSetValue::CSetValue( CWnd *pParent /*= NULL*/ )
{

	m_pParent = pParent ;
	m_pfEventCheckfunc = NULL ;

}

CSetValue::~CSetValue()
{

	//

}


int	CSetValue::SetValue( LPCTSTR lpszMsg, double &value, double max, double min, BOOL mouseflag )
{

	CSetValueDlg	SetValue( m_pParent ) ;
	int	ret = -1 ;

	SetValue.m_BitSize = ( int)( CSetValueDlg::DOUBLE_SIZE ) ;

	SetValue.m_MinValue = ( double)min ;
	SetValue.m_MaxValue = ( double)max ;
	SetValue.d_Value = value ;

	SetValue.m_HeadStr = lpszMsg ;
	SetValue.m_MouseMoveFlag = mouseflag ;

	SetValue.SetEventCheckFunc( m_pfEventCheckfunc ) ;
	if ( SetValue.DoModal() == IDOK ) {
		value = SetValue.d_Value ;
		ret = 0 ;
	}

	return ( ret ) ;

}


int	CSetValue::SetValue( LPCTSTR lpszMsg, UINT &value, UINT max, UINT min, BOOL mouseflag )
{

	CSetValueDlg	SetValue( m_pParent ) ;
	int	ret = -1 ;

	SetValue.m_BitSize = ( int)( CSetValueDlg::UINT_SIZE ) ;

	SetValue.m_MinValue = ( double)min ;
	SetValue.m_MaxValue = ( double)max ;
	SetValue.u_Value = value ;

	SetValue.m_HeadStr = lpszMsg ;
	SetValue.m_MouseMoveFlag = mouseflag ;

	SetValue.SetEventCheckFunc( m_pfEventCheckfunc ) ;
	if ( SetValue.DoModal() == IDOK ) {
		value = SetValue.u_Value ;
		ret = 0 ;
	}

	return ( ret ) ;

}

int	CSetValue::SetValue( LPCTSTR lpszMsg, int &value, int max, int min, BOOL mouseflag )
{

	CSetValueDlg	SetValue( m_pParent ) ;
	int	ret = -1 ;

	SetValue.m_BitSize = ( int)( CSetValueDlg::INT_SIZE ) ;

	SetValue.m_MinValue = ( double)min ;
	SetValue.m_MaxValue = ( double)max ;
	SetValue.i_Value = value ;

	SetValue.m_HeadStr = lpszMsg ;
	SetValue.m_MouseMoveFlag = mouseflag ;

	SetValue.SetEventCheckFunc( m_pfEventCheckfunc ) ;
	if ( SetValue.DoModal() == IDOK ) {
		value = SetValue.i_Value ;
		ret = 0 ;
	}

	return ( ret ) ;

}


int	CSetValue::SetValue( LPCTSTR lpszMsg, short &value, short max, short min, BOOL mouseflag )
{

	CSetValueDlg	SetValue( m_pParent ) ;
	int	ret = -1 ;

	SetValue.m_BitSize = ( int)( CSetValueDlg::INT_SIZE ) ;

	SetValue.m_MinValue = ( double)min ;
	SetValue.m_MaxValue = ( double)max ;
	SetValue.i_Value = value ;

	SetValue.m_HeadStr = lpszMsg ;
	SetValue.m_MouseMoveFlag = mouseflag ;

	SetValue.SetEventCheckFunc( m_pfEventCheckfunc ) ;
	if ( SetValue.DoModal() == IDOK ) {
		value = SetValue.i_Value ;
		ret = 0 ;
	}

	return ( ret ) ;

}


int	CSetValue::SetValue( LPCTSTR lpszMsg, WORD &value, WORD max, WORD min, BOOL mouseflag )
{

	CSetValueDlg	SetValue( m_pParent ) ;
	int	ret = -1 ;

	SetValue.m_BitSize = ( int)( CSetValueDlg::INT_SIZE ) ;

	SetValue.m_MinValue = ( double)min ;
	SetValue.m_MaxValue = ( double)max ;
	SetValue.i_Value = value ;

	SetValue.m_HeadStr = lpszMsg ;
	SetValue.m_MouseMoveFlag = mouseflag ;

	SetValue.SetEventCheckFunc( m_pfEventCheckfunc ) ;
	if ( SetValue.DoModal() == IDOK ) {
		value = SetValue.i_Value ;
		ret = 0 ;
	}

	return ( ret ) ;

}


int	CSetValue::SetValue( LPCTSTR lpszMsg, unsigned long &value, unsigned long max, unsigned long min, BOOL mouseflag )
{

	CSetValueDlg	SetValue( m_pParent ) ;
	int	ret = -1 ;

	SetValue.m_BitSize = ( int)( CSetValueDlg::ULONG_SIZE ) ;

	SetValue.m_MinValue = ( double)min ;
	SetValue.m_MaxValue = ( double)max ;
	SetValue.ul_Value = value ;

	SetValue.m_HeadStr = lpszMsg ;
	SetValue.m_MouseMoveFlag = mouseflag ;

	SetValue.SetEventCheckFunc( m_pfEventCheckfunc ) ;
	if ( SetValue.DoModal() == IDOK ) {
		value = SetValue.ul_Value ;
		ret = 0 ;
	}

	return ( ret ) ;

}


int	CSetValue::SetValue( LPCTSTR lpszMsg, long &value, long max, long min, BOOL mouseflag )
{

	CSetValueDlg	SetValue( m_pParent ) ;
	int	ret = -1 ;

	SetValue.m_BitSize = ( int)( CSetValueDlg::LONG_SIZE ) ;

	SetValue.m_MinValue = ( double)min ;
	SetValue.m_MaxValue = ( double)max ;
	SetValue.l_Value = value ;

	SetValue.m_HeadStr = lpszMsg ;
	SetValue.m_MouseMoveFlag = mouseflag ;

	SetValue.SetEventCheckFunc( m_pfEventCheckfunc ) ;
	if ( SetValue.DoModal() == IDOK ) {
		value = SetValue.l_Value ;
		ret = 0 ;
	}

	return ( ret ) ;

}

// 22.04.13 Ahn Add Start
int	CSetValue::SetValue(LPCTSTR lpszMsg, CString& value, int max, int min, BOOL mouseflag /*= TRUE*/, BOOL bPassMode /*= FALSE*/)
{

	int nRet = 0; 

	CSetValueDlg	SetValue(m_pParent);
	int	ret = -1;

	SetValue.m_bModeString = TRUE;

	SetValue.m_BitSize = (int)(CSetValueDlg::STRING_SIZE);

	SetValue.m_MinValue = (double)min;
	SetValue.m_MaxValue = (double)max;
	SetValue.st_Value = value;

	SetValue.m_HeadStr = lpszMsg;
	SetValue.m_MouseMoveFlag = mouseflag;

	SetValue.SetEventCheckFunc(m_pfEventCheckfunc);
	if (SetValue.DoModal() == IDOK) {
		value = SetValue.st_Value;
		ret = 0;
	}

	return nRet ;
}
// 22.04.13 Ahn Add End

int	CSetValue::SwapValue( double *pData1, double *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	double	temp ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::SwapValue( long *pData1, long *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
		//SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	long	temp ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::SwapValue( int *pData1, int *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
		//SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	int	temp ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::SwapValue( WORD *pData1, WORD *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
		//SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	WORD	temp ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::SwapValue( short int *pData1, short int *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
		//SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	short int	temp ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::SwapValue( char *pData1, char *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
		//SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	char	temp ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::SwapValue( BYTE *pData1, BYTE *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
		//SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	BYTE	temp ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::SwapValue( CString *pData1, CString *pData2 )
{

	ASSERT( pData1 != NULL ) ;
	ASSERT( pData2 != NULL ) ;

	if ( ( pData1 == NULL ) || ( pData2 == NULL ) ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}

	CString	temp = _T( "" ) ;
	temp = *pData1 ;
	*pData1 = *pData2 ;
	*pData2 = temp ;

	return ( 0 ) ;

}


template < class T > int CSetValue::SwapValue( T &data1, T &data2 )
{

	T	temp ;
	temp = data1 ;
	data1 = data2 ;
	data2 = temp ;

	return ( 0 ) ;

}


int	CSetValue::CutValue( int *piValue, int iMax, int iMin, BOOL bEdge /*= TRUE*/ )
{

	ASSERT( piValue != NULL ) ;
	ASSERT( iMin < iMax ) ;

	if ( piValue == NULL ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	iRet = 0 ;

	if ( bEdge == FALSE ) {
		ASSERT( ( iMax - iMin ) >= 2 ) ;
		if ( *piValue <= iMin ) {
			*piValue = iMin + 1 ;
		} else if ( *piValue >= iMax ) {
			*piValue = iMax - 1 ;
		}
	} else {
		if ( *piValue < iMin ) {
			*piValue = iMin ;
		} else if ( *piValue > iMax ) {
			*piValue = iMax ;
		}
	}

	return ( iRet ) ;

}


int	CSetValue::CutValue( long *plValue, long lMax, long lMin, BOOL bEdge /*= TRUE*/ )
{

	ASSERT( plValue != NULL ) ;
	ASSERT( lMin < lMax ) ;

	if ( plValue == NULL ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	iRet = 0 ;

	if ( bEdge == FALSE ) {
		ASSERT( ( lMax - lMin ) >= 2 ) ;
		if ( *plValue <= lMin ) {
			*plValue = lMin + 1 ;
		} else if ( *plValue >= lMax ) {
			*plValue = lMax - 1 ;
		}
	} else {
		if ( *plValue < lMin ) {
			*plValue = lMin ;
		} else if ( *plValue > lMax ) {
			*plValue = lMax ;
		}
	}

	return ( iRet ) ;

}


int	CSetValue::CutValue( double *pdValue, double dMax, double dMin, BOOL bEdge /*= TRUE*/ )
{

	ASSERT( pdValue != NULL ) ;
	ASSERT( dMin < dMax ) ;

	if ( pdValue == NULL ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	iRet = 0 ;

	if ( bEdge == FALSE ) {
		ASSERT( ( dMax - dMin ) >= ( double)0.002 ) ;
		if ( *pdValue <= dMin ) {
			*pdValue = dMin + ( double)0.001 ;
		} else if ( *pdValue >= dMax ) {
			*pdValue = dMax - ( double)0.001 ;
		}
	} else {
		if ( *pdValue < dMin ) {
			*pdValue = dMin ;
		} else if ( *pdValue > dMax ) {
			*pdValue = dMax ;
		}
	}

	return ( iRet ) ;

}


int CSetValue::CutValue( DWORD *pValue, DWORD nMax, DWORD nMin, BOOL bEdge /*= TRUE*/ )
{

	ASSERT( pValue != NULL ) ;
	ASSERT( nMin < nMax ) ;

	if ( pValue == NULL ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	iRet = 0 ;

	if ( bEdge == FALSE ) {
		ASSERT( ( nMax - nMin ) >= 2 ) ;
		if ( *pValue <= nMin ) {
			*pValue = nMin + 1 ;
		} else if ( *pValue >= nMax ) {
			*pValue = nMax - 1 ;
		}
	} else {
		if ( *pValue < nMin ) {
			*pValue = nMin ;
		} else if ( *pValue > nMax ) {
			*pValue = nMax ;
		}
	}

	return ( iRet ) ;

}


template < class T > int CSetValue::CutValue( T &value, T max, T min, BOOL bEdge /*= TRUE*/ )
{
	ASSERT( min < max ) ;
	int	iRet = 0 ;
	if ( bEdge == FALSE ) {
		ASSERT( ( max - min ) >= 2 ) ;
		if ( value <= min ) {
			value = min + 1 ;
		} else if ( value >= max ) {
			value = max - 1 ;
		}
	} else {
		if ( value < min ) {
			value = min ;
		} else if ( value > max ) {
			value = max ;
		}
	}
	return ( iRet ) ;
}


int	CSetValue::SwapCheckFunc( WORD *val1, WORD *val2, LPCTSTR lpszMsg )
{

	ASSERT( val1 != NULL ) ;
	ASSERT( val2 != NULL ) ;

	if ( ( val1 == NULL ) || ( val2 == NULL ) ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	swapflag = 0 ;

	if ( _tcscmp( lpszMsg, _T( "<" ) ) == 0 ) {
	// -----"<"	: ( *val1 < *val2 )-----
		if ( *val1 >= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">" ) ) == 0 ) {
	// -----">"	: ( *val1 > *val2 )-----
		if ( *val1 <= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( "<=" ) ) == 0 ) {
	// -----"<="	: ( *val1 <= *val2 )-----
		if ( *val1 > *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">=" ) ) == 0 ) {
	// -----">="	: ( *val1 >= *val2 )-----
		if ( *val1 < *val2 ) {
			swapflag = 1 ;
		}
	}

	if ( swapflag == 1 ) {
		SwapValue( val1, val2 ) ;
	}

	return ( swapflag ) ;

}


int	CSetValue::SwapCheckFunc( int *val1, int *val2, LPCTSTR lpszMsg )
{

	ASSERT( val1 != NULL ) ;
	ASSERT( val2 != NULL ) ;

	if ( ( val1 == NULL ) || ( val2 == NULL ) ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	swapflag = 0 ;

	if ( _tcscmp( lpszMsg, _T( "<" ) ) == 0 ) {
	// -----"<"	: ( *val1 < *val2 )-----
		if ( *val1 >= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">" ) ) == 0 ) {
	// -----">"	: ( *val1 > *val2 )-----
		if ( *val1 <= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( "<=" ) ) == 0 ) {
	// -----"<="	: ( *val1 <= *val2 )-----
		if ( *val1 > *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">=" ) ) == 0 ) {
	// -----">="	: ( *val1 >= *val2 )-----
		if ( *val1 < *val2 ) {
			swapflag = 1 ;
		}
	}

	if ( swapflag == 1 ) {
		SwapValue( val1, val2 ) ;
	}

	return ( swapflag ) ;

}


int	CSetValue::SwapCheckFunc( long *val1, long *val2, LPCTSTR lpszMsg )
{

	ASSERT( val1 != NULL ) ;
	ASSERT( val2 != NULL ) ;

	if ( ( val1 == NULL ) || ( val2 == NULL ) ) {
		//SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	swapflag = 0 ;

	if ( _tcscmp( lpszMsg, _T( "<" ) ) == 0 ) {
	// -----"<"	: ( *val1 < *val2 )-----
		if ( *val1 >= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">" ) ) == 0 ) {
	// -----">"	: ( *val1 > *val2 )-----
		if ( *val1 <= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( "<=" ) ) == 0 ) {
	// -----"<="	: ( *val1 <= *val2 )-----
		if ( *val1 > *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">=" ) ) == 0 ) {
	// -----">="	: ( *val1 >= *val2 )-----
		if ( *val1 < *val2 ) {
			swapflag = 1 ;
		}
	}

	if ( swapflag == 1 ) {
		SwapValue( val1, val2 ) ;
	}

	return ( swapflag ) ;

}


int	CSetValue::SwapCheckFunc( double *val1, double *val2, LPCTSTR lpszMsg )
{

	ASSERT( val1 != NULL ) ;
	ASSERT( val2 != NULL ) ;

	if ( ( val1 == NULL ) || ( val2 == NULL ) ) {
	//	SAVE_ERROR_LINE() ;
		return ( -1 ) ;
	}
	int	swapflag = 0 ;

	if ( _tcscmp( lpszMsg, _T( "<" ) ) == 0 ) {
	// -----"<"	: ( *val1 < *val2 )-----
		if ( *val1 >= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">" ) ) == 0 ) {
	// -----">"	: ( *val1 > *val2 )-----
		if ( *val1 <= *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( "<=" ) ) == 0 ) {
	// -----"<="	: ( *val1 <= *val2 )-----
		if ( *val1 > *val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">=" ) ) == 0 ) {
	// -----">="	: ( *val1 >= *val2 )-----
		if ( *val1 < *val2 ) {
			swapflag = 1 ;
		}
	}

	if ( swapflag == 1 ) {
		SwapValue( val1, val2 ) ;
	}

	return ( swapflag ) ;

}


template < class T > int CSetValue::SwapCheckFunc( T &val1, T &val2, LPCTSTR lpszMsg )
{

	int	swapflag = 0 ;

	if ( _tcscmp( lpszMsg, _T( "<" ) ) == 0 ) {
	// -----"<"	: ( val1 < val2 )-----
		if ( val1 >= val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">" ) ) == 0 ) {
	// -----">"	: ( val1 > val2 )-----
		if ( val1 <= val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( "<=" ) ) == 0 ) {
	// -----"<="	: ( val1 <= val2 )-----
		if ( val1 > val2 ) {
			swapflag = 1 ;
		}
	} else if ( _tcscmp( lpszMsg, _T( ">=" ) ) == 0 ) {
	// -----">="	: ( val1 >= val2 )-----
		if ( val1 < val2 ) {
			swapflag = 1 ;
		}
	}

	if ( swapflag == 1 ) {
		SwapValue( val1, val2 ) ;
	}

	return ( swapflag ) ;

}


int CSetValue::SetEventCheckFunc( int ( *eventfunc)() )
{
	int	nRet = 0 ;
	m_pfEventCheckfunc = eventfunc ;
	return ( nRet ) ;
}
