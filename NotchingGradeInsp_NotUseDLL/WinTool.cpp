#include "pch.h"
#include "WinTool.h"

BOOL PumpMessages( LPMSG lpMsg /*= NULL*/, HWND hWnd /*= NULL*/, UINT wMsgFilterMin /*= WM_PAINT*/, UINT wMsgFilterMax /*= WM_PAINT*/, UINT wRemoveMsg /*= PM_REMOVE*/ )
{

	MSG	msg ;
	if ( lpMsg == NULL ) {
		lpMsg = &msg ;
	}
	BOOL	bMsgExist = ::PeekMessage( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg ) ;
	if ( bMsgExist != FALSE ) {
		::TranslateMessage( lpMsg ) ;
		::DispatchMessage( lpMsg ) ;
	}

	return ( bMsgExist ) ;

}


CString	FormatErrorMsg( DWORD dwErrNo ) 
{
	LPVOID lpMsgBuf ;
	CString	str ;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		( LPTSTR)&lpMsgBuf,
		0,
		NULL
	) ;

	str.Format( _T("%s"), lpMsgBuf ) ;
	LocalFree( lpMsgBuf ) ;

	return ( str ) ;
}


BOOL SetPriorityClassMacro( DWORD dwPriority, DWORD *pdwBackPriority /*= NULL*/, int nMode /*= 0*/ )
{ 
	HANDLE op = OpenProcess( PROCESS_ALL_ACCESS
					, TRUE
					, GetCurrentProcessId() ) ;
	if ( pdwBackPriority != NULL ) {
		*pdwBackPriority = GetPriorityClass( op ) ;
	}
	if ( nMode == 1 ) {
		return ( TRUE ) ;
	}
	return ( SetPriorityClass( op, dwPriority ) ) ;
}


BOOL PickMessages( LPMSG lpMsg /*= NULL*/, HWND hWnd /*= NULL*/, UINT wMsgFilterMin /*= WM_PAINT*/, UINT wMsgFilterMax /*= WM_PAINT*/, UINT wRemoveMsg /*= PM_REMOVE*/ )
{

	MSG	msg ;
	if ( lpMsg == NULL ) {
		lpMsg = &msg ;
	}
	BOOL	bMsgExist = ::PeekMessage( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg ) ;
	if ( bMsgExist != FALSE ) {
		//::TranslateMessage( lpMsg ) ;
		//::DispatchMessage( lpMsg ) ;
	}

	return ( bMsgExist ) ;

}
