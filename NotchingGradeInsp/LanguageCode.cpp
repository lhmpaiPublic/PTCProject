//	$Id: $
//!	@file	LanguageCode.cpp
//!	@brief	GetLanguageCode 듫릶궻렳몧
//!	@author	
//!	@date	
//!	@note	


//#include "stdafx.h"
#include "pch.h"
#include "LanguageCode.h"
//#include "PrivateProfile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define		STR_PRIVATE_PROFILE_NAME	_T( "\\Data\\SystemSetting.ini" )


LPCTSTR GetPrivateProfilePath(void)
{
	static TCHAR szProfilePath[_MAX_PATH] = { _T('\0') };
	if (szProfilePath[0] == _T('\0'))
	{
		memset(szProfilePath, 0x00, sizeof(szProfilePath));
		TCHAR szCurrentPath[_MAX_PATH];
		memset(szCurrentPath, 0x00, sizeof(szCurrentPath));
		::GetCurrentDirectory(_MAX_PATH, szCurrentPath);
		_tcsncat_s(szProfilePath, sizeof(szProfilePath), szCurrentPath, _MAX_PATH);
		_tcsncat_s(szProfilePath, sizeof(szProfilePath), STR_PRIVATE_PROFILE_NAME, _MAX_PATH);
	}
	return (szProfilePath);
}


LPCTSTR GetLanguageCode( void )
{
	static TCHAR szLanguageCode[ _MAX_PATH ] = { _T( '\0' ) } ;
	if ( szLanguageCode[ 0 ] == _T( '\0' ) )
	{
		memset( szLanguageCode, 0x00, sizeof( szLanguageCode ) ) ;
		::GetPrivateProfileString( _T( "EQP_SETTING" ), _T( "DISPLAY_LANGUAGE" ), LANGUAGE_CODE_KOR
			, szLanguageCode, _MAX_PATH, GetPrivateProfilePath()
			) ;
	}
	return ( szLanguageCode ) ;
}
