//	$Id: $
//!	@file	LanguageCode.h
//!	@brief	GetLanguageCode 関数の宣言
//!	@author	
//!	@date	
//!	@note	


#pragma once

LPCTSTR GetPrivateProfilePath(void);
LPCTSTR GetLanguageCode( void ) ;
//// 17.11.08 Ahn Add Start
//CString	GetDispLanguage( CString strEng, CString strJpn, CString strKor, CString strChn ) ;
//// 17.11.08 Ahn Add End

#define		LANGUAGE_CODE_ENG		_T( "eng" )
#define		LANGUAGE_CODE_KOR		_T( "kor" )
#define		LANGUAGE_CODE_CHN		_T( "chn" )
//#define		LANGUAGE_CODE_JPN		_T( "jpn" )

#define		IS_LANG_ENGLISH()		!_tcscmp( GetLanguageCode(), LANGUAGE_CODE_ENG )
#define		IS_LANG_KOREAN()		!_tcscmp( GetLanguageCode(), LANGUAGE_CODE_KOR )
//#define		IS_LANG_JAPANESE()		!_tcscmp( GetLanguageCode(), LANGUAGE_CODE_JPN )
#define		IS_LANG_CHINESE()		!_tcscmp( GetLanguageCode(), LANGUAGE_CODE_CHN )

#define		_LANG( kor, eng )		( IS_LANG_KOREAN() ? _T( kor ) : _T( eng ) )
enum languagekind
{
	kor,
	eng,
	chn
};
#define __Lang ( IS_LANG_KOREAN() ? kor : IS_LANG_ENGLISH() ? eng : chn)

//// 17.11.08 Ahn Add Start
//#define		_DISP_LANG( eng, kor, jpn, chn )		GetDispLanguage( eng, kor, jpn, chn ) 				
//// 17.11.08 Ahn Add End
