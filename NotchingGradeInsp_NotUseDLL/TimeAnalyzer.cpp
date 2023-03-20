//	$Id: $
//!	@file	TimeAnalyzer.cpp
//!	@brief	CTimeAnalyzer 긏깋긚궻귽깛긵깏긽깛긡?긘깈깛
//!	@author	
//!	@date	2005.08.10
//!	@note	2005.08.10	doxygen뾭궻긓?긤귩믁돿갃렔벍맯맟갃


// TimeAnalyzer.cpp: CTimeAnalyzer 긏깋긚궻귽깛긵깏긽깛긡?긘깈깛
//
//////////////////////////////////////////////////////////////////////


#include "pch.h"
//#include "StdRes.h"

#include "TimeAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__ ;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CTimeAnalyzer ?귽귺깓긐


//////////////////////////////////////////////////////////////////////
// ?뭱/뤑뽅
//////////////////////////////////////////////////////////////////////


//!
//!			듫릶뼹:	CTimeAnalyzer::CTimeAnalyzer
//!	@brief	??:	긓깛긚긣깋긏?
//!	@param	덙릶:	int		nNumOfAllocData = enTimeAna_DefaultAllocNum	: 긢??긽긾깏귩둴뺎궥귡뭁댧
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CTimeAnalyzer::CTimeAnalyzer( int nNumOfAllocData /*= enTimeAna_DefaultAllocNum*/ )
{
	LARGE_INTEGER	lv ;
	QueryPerformanceFrequency( &lv ) ;
	m_Frequency = ( double)lv.LowPart + ( ( double )lv.HighPart * ( double)0xffffffff ) ;

	m_pTimeAnaData = NULL ;
	m_nDataNumber = 0 ;
	m_nNumOfMaxData = 0 ;
	m_nNumOfAllocData = nNumOfAllocData ;
	if ( m_nNumOfAllocData <= 0 ) {
		ASSERT( FALSE ) ;
		m_nNumOfAllocData = enTimeAna_DefaultAllocNum ;
	}
	Clear() ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::~CTimeAnalyzer
//!	@brief	??:	긢긚긣깋긏?
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CTimeAnalyzer::~CTimeAnalyzer()
{
	Clear() ;
	{
		if ( m_pTimeAnaData != NULL ) {
			delete [] m_pTimeAnaData ;
			m_pTimeAnaData = NULL ;
		}
		m_nNumOfMaxData = 0 ;
	}
}


// --------------------------------------------------
//
// --------------------------------------------------


//!
//!			듫릶뼹:	CTimeAnalyzer::AddData
//!	@brief	??:	렄뜌귩똶뫇궢륷궢궋긢??귩딯?깏긚긣궸돿궑귏궥갆
//!	@param	덙릶:	LPCTSTR		description	: 똶뫇딯?먣뼻빁귩둰?궢궫CString긆긳긙긃긏긣귉궻랷뤖귏궫궼긫긞긲?귉궻?귽깛?
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:	directorylevel	: 긢??궕뫌궥귡둏몏귩렑궥릶뭠걁괥궔귞럑귏귡걂
//!	@note			궞궻릶뭠궕몵궑귡뤾뜃괦궱궰몵돿궥귡뷠뾴궇귟갆뙵룺궼궞궻맕뙽궼궶궋갆
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CTimeAnalyzer::AddData( LPCTSTR description )
{
	if ( m_nDataNumber >= m_nNumOfMaxData ) {
	// -----긽긾깏둴뺎궕뷠뾴-----
		ASSERT( m_nNumOfAllocData > 0 ) ;
		TIME_ANA_DATA	*pNewData = new TIME_ANA_DATA[ m_nNumOfMaxData + m_nNumOfAllocData ] ;
		memset( pNewData, 0x00, ( sizeof( TIME_ANA_DATA ) * ( m_nNumOfMaxData + m_nNumOfAllocData ) ) ) ;
#if	0
		{
			int	nIndex ;
			for ( nIndex = 0 ; nIndex < m_nDataNumber ; nIndex++ ) {
				pNewData[ nIndex ] = m_pTimeAnaData[ nIndex ] ;
			}
		}
#else
		memcpy( pNewData, m_pTimeAnaData, ( sizeof( *m_pTimeAnaData ) * m_nNumOfMaxData ) ) ;
#endif
		if ( m_pTimeAnaData != NULL ) {
			delete [] m_pTimeAnaData ;
			m_pTimeAnaData = NULL ;
		}
		m_pTimeAnaData = pNewData ;
		m_nNumOfMaxData += m_nNumOfAllocData ;
	}
	TIME_ANA_DATA	*pTd = &( m_pTimeAnaData[ m_nDataNumber ] ) ;
	memset( pTd, 0x00, sizeof( *pTd ) ) ;
// @ x64 Compatible @ st
//	int	nSrcLength = strlen( description ) ;
	int	nSrcLength = ( int)( strlen( description ) ) ;
// @ x64 Compatible @ ed
	int	nTarSize = sizeof( pTd->description ) ;
	if ( nSrcLength > ( nTarSize - 1 ) ) {
		ASSERT( FALSE ) ;
		memcpy( pTd->description, description, ( nTarSize - 1 ) ) ;
	} else {
		strcpy_s( pTd->description, description ) ;
	}
	pTd->directorylevel = LevelMemory ;

	LARGE_INTEGER	ltime ;
	QueryPerformanceCounter( &ltime ) ;
	pTd->time = ltime ;
	m_nDataNumber++ ;
	return ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::Clear
//!	@brief	??:	렄듩똶뫇딯?귩멣궲긏깏귺궢귏궥
//!	@param	덙릶:	int		nMode = 0	: 긾?긤(0:긢??긽긾깏귖둎뺳궥귡/1:긢??긽긾깏궼둎뺳궢궶궋)
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CTimeAnalyzer::Clear( int nMode /*= 0*/ )
{
	switch ( nMode ) {
	case	0 :		// 긢??긽긾깏귖둎뺳궥귡
		if ( m_pTimeAnaData != NULL ) {
			delete [] m_pTimeAnaData ;
			m_pTimeAnaData = NULL ;
		}
		m_nNumOfMaxData = 0 ;
		break ;
	case	1 :		// 긢??긽긾깏궼둎뺳궢궶궋
	default :
		break ;
	}
	m_nDataNumber = 0 ;
	LevelMemory = 0 ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::GetData
//!	@brief	??:	똶뫇딯?궻뭷궔귞긢??귩롦벦궢귏궥
//!	@param	덙릶:	int		number			: 벶귒뢯궢궫궋긢??궕먩벆궔귞돺붥뽞궔귩럚믦궥귡릶뭠
//!	@param			CString	&description	: 똶뫇먣뼻빒귩롷궚롦귡궫귕궻CString긆긳긙긃긏긣귉궻랷뤖
//!	@param			double	*dtimev			: 똶뫇렄듩걁똶뫇둎럑궔귞궻똮됡렄듩걂귩렑궥?깏뷳뭁댧궻릶뭠귩롷궚롦귡긫긞긲?귉궻?귽깛?(NULL:둰?궢궶궋)
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int	CTimeAnalyzer::GetData( int number, CString &description, double *dtimev )
{
	int	directorylevel ;
	LARGE_INTEGER	v0, v1 ;

	if ( ( m_pTimeAnaData == NULL )
	  || ( number < 0 )
	  || ( number >= m_nDataNumber )
	  ) {
		description = "None" ;
		if ( dtimev != NULL ) {
			*dtimev = ( double)0.0 ;
		}
		return ( 0 ) ;
	}

	TIME_ANA_DATA	*tm = m_pTimeAnaData ;
	description = tm[ number ].description ;
	v0 = tm[ 0 ].time ;
	v1 = tm[ number ].time ;
	directorylevel = tm[ number ].directorylevel ;

	if ( dtimev != NULL ) {
		double	dv0, dv1 ;
		dv0 = ( double)v0.LowPart + ( ( double)v0.HighPart * ( double)0xffffffff ) ;
		dv1 = ( double)v1.LowPart + ( ( double)v1.HighPart * ( double)0xffffffff ) ;
		*dtimev = ( dv1 - dv0 ) / m_Frequency * ( double)1000.0 ;
	}
	return ( directorylevel ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::WhatTimeIsIt
//!	@brief	??:	궇귡렄?궔귞궻똮됡렄듩귩롨몓궘뭢귟궫궋궴궖갅StopWatchStart궳렄똶귩긚??긣궠궧갅
//!	@brief			궞궻듫릶궳갅똮됡렄듩귩롦벦궥귡갆
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	long	: 똮됡렄듩[msec]
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
long CTimeAnalyzer::WhatTimeIsIt()
{
	LARGE_INTEGER	v1 ;
	QueryPerformanceCounter( &v1 ) ;
	double	dv0, dv1 ;
	dv0 = ( double)mStopWatchStartTime.LowPart + ( ( double)mStopWatchStartTime.HighPart * ( double)0xffffffff ) ;
	dv1 = ( double)v1.LowPart + ( ( double)v1.HighPart * ( double)0xffffffff ) ;
	double	dtimev ;
	dtimev = ( dv1 - dv0 ) / m_Frequency * ( double)1000.0 ;
	return ( ( long )dtimev ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::WhatTimeIsIt_Double
//!	@brief	??:	궇귡렄?궔귞궻똮됡렄듩귩롨몓궘뭢귟궫궋궴궖갅StopWatchStart궳렄똶귩긚??긣궠궧갅
//!	@brief			궞궻듫릶궳갅똮됡렄듩귩롦벦궥귡갆
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	double	: 똮됡렄듩[msec]
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
double CTimeAnalyzer::WhatTimeIsIt_Double()
{
	LARGE_INTEGER	v1 ;
	QueryPerformanceCounter( &v1 ) ;
	double	dv0, dv1 ;
	dv0 = ( double)mStopWatchStartTime.LowPart + ( ( double)mStopWatchStartTime.HighPart * ( double)0xffffffff ) ;
	dv1 = ( double)v1.LowPart + ( ( double )v1.HighPart * ( double)0xffffffff ) ;
	double	dtimev ;
	dtimev = ( dv1 - dv0 ) / m_Frequency * ( double)1000.0 ;
	return ( dtimev ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::StopWatchStart
//!	@brief	??:	궇귡렄?궔귞궻똮됡렄듩귩롨몓궘뭢귟궫궋궴궖갅궞궻듫릶궳렄똶귩긚??긣궠궧귡갆
//!	@brief			똮됡렄듩귩롦벦궥귡궸궼갅궞궻듫릶궻뚣갅 WhatTimeIsIt 듫릶귩뾭궋귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CTimeAnalyzer::StopWatchStart()
{
	QueryPerformanceCounter( &mStopWatchStartTime ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::LevelInc
//!	@brief	??:	긢??깒긹깑귩?괦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int CTimeAnalyzer::LevelInc()
{
	LevelMemory++ ;
	return ( LevelMemory ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::LevelDec
//!	@brief	??:	긢??깒긹깑귩?괦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int CTimeAnalyzer::LevelDec()
{
	LevelMemory-- ;
	if ( LevelMemory < 0 ) {
		LevelMemory = 0 ;
	}
	return ( LevelMemory ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::LevelCancel
//!	@brief	??:	긢??깒긹깑귩룊딖돸궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int CTimeAnalyzer::LevelCancel()
{
	LevelMemory = 0 ;
	return ( LevelMemory ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::DoDialogBox
//!	@brief	??:	똶뫇딯?긢??둴봃긂귻깛긤긂귩둎궘
//!	@param	덙릶:	CWnd	*pParent	: 릂긂귻깛긤긂귉궻?귽깛?
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int CTimeAnalyzer::DoDialogBox( CWnd *pParent )
{
//#if	defined( RESOURCE_STDRES ) || ( defined( RESOURCE_COMRES ) & !defined( NOMFCOMPAPP ) )
//	CTimeAnaDlg	dlg( pParent ) ;
//	dlg.mpTimeAnalyzer = this ;
//	return ( dlg.DoModal() ) ;
//#else
//	AfxMessageBox( "This is not Supported. Make yourself!" ) ;
//	return ( IDOK ) ;
//#endif
	return ( IDOK ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::delay
//!	@brief	??:	럚믦렄듩뫲궰듫릶
//!	@param	덙릶:	DWORD	msec	: 뫲궰렄듩[msec]
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CTimeAnalyzer::delay( DWORD msec )
{
	if ( msec == 0 ) {
		return ;
	}
	LARGE_INTEGER	m1 ;
	LARGE_INTEGER	v1 ;
	double	dtimev ;
	double	dv0, dv1 ;

	QueryPerformanceCounter( &m1 ) ;
	while ( 1 ) {
		QueryPerformanceCounter( &v1 ) ;
		dv0 = ( double)m1.LowPart + ( ( double)m1.HighPart * ( double)0xffffffff ) ;
		dv1 = ( double)v1.LowPart + ( ( double)v1.HighPart * ( double)0xffffffff ) ;
		dtimev = ( dv1 - dv0 ) / m_Frequency * ( double)1000.0 ;
		if ( dtimev > msec ) {
			break ;
		}
	}
	return ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::delayMicroSec
//!	@brief	??:	럚믦렄듩뫲궰듫릶
//!	@param	덙릶:	DWORD	microsec	: 뫲궰렄듩[꺤sec]
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CTimeAnalyzer::delayMicroSec( DWORD microsec )
{
	if ( microsec == 0 ) {
		return ;
	}
	LARGE_INTEGER	m1 ;
	LARGE_INTEGER	v1 ;
	double	dtimev ;
	double	dv0, dv1 ;

	QueryPerformanceCounter( &m1 ) ;
	while ( 1 ) {
		QueryPerformanceCounter( &v1 ) ;
		dv0 = ( double)m1.LowPart + ( ( double)m1.HighPart * ( double)0xffffffff ) ;
		dv1 = ( double)v1.LowPart + ( ( double)v1.HighPart * ( double)0xffffffff ) ;
		dtimev = ( dv1 - dv0 ) / m_Frequency * ( double)1000 * ( double)1000 ;
		if ( dtimev > microsec ) {
			break ;
		}
	}
	return ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::GetNumOfDatas
//!	@brief	??:	뾎뚼궶긢??릶귩롦벦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	int	: 뾎뚼궶긢??릶
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int CTimeAnalyzer::GetNumOfDatas( void )
{
	ASSERT( m_nDataNumber >= 0 ) ;
	return ( m_nDataNumber ) ;
}


//!
//!			듫릶뼹:	CTimeAnalyzer::SetNumOfDatas
//!	@brief	??:	뾎뚼궶긢??릶귩긜긞긣궥귡
//!	@param	덙릶:	int		nNumOfData	:
//!	@return	듫릶뭠:	int	0	: 맫륂
//!	@return				-1	: 댶륂
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int CTimeAnalyzer::SetNumOfDatas( int nNumOfData )
{
	ASSERT( nNumOfData >= 0 ) ;
	if ( nNumOfData > m_nDataNumber ) {
	// 긽긾깏둴뺎갋긢??볙뾢궻뽦묋궕궇귡궻궳갂뙸띪궻뾎뚼긢??릶댥돷궢궔떀궠궶궋갏
		return ( -1 ) ;
	}
	m_nDataNumber = nNumOfData ;
	return ( 0 ) ;
}