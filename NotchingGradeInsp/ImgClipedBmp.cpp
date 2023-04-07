//	$Id: $
//!	@file	ImgClipedBmp.cpp
//!	@brief	
//!	@author	
//!	@date	2005.08.05
//!	@note	2005.08.05	doxygen뾭궻긓?긤귩믁돿갃렔벍맯맟갃


// ImgClipedBmp.cpp: CImgClipedBmp 긏깋긚궻귽깛긵깏긽깛긡?긘깈깛
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ImgClipedBmp.h"
#include "BitmapStd.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// ?뭱/뤑뽅
//////////////////////////////////////////////////////////////////////

//!
//!			듫릶뼹:	CImgClipedBmp::CImgClipedBmp
//!	@brief	??:	긓깛긚긣깋긏?
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CImgClipedBmp::CImgClipedBmp()
{
	Init() ;

}


//!
//!			듫릶뼹:	CImgClipedBmp::~CImgClipedBmp
//!	@brief	??:	긢긚긣깋긏?
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CImgClipedBmp::~CImgClipedBmp()
{
	if ( m_pBitmap ) {
		delete	m_pBitmap ;
		m_pBitmap = NULL ;
	}
}


//!
//!			듫릶뼹:	CImgClipedBmp::operator=
//!	@brief	??:	묆볺뎶럁럔
//!	@param	덙릶:	const	CImgClipedBmp&	src	: 묆볺뙰귉궻랷뤖
//!	@return	듫릶뭠:	CImgClipedBmp	&	: 묆볺먩귉궻랷뤖
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CImgClipedBmp& CImgClipedBmp::operator= ( const CImgClipedBmp& src )
{
	if ( this == &src ) {
		return ( *this ) ;
	}

	Copy( src ) ;

	return ( *this ) ;
}


//!
//!			듫릶뼹:	CImgClipedBmp::CImgClipedBmp
//!	@brief	??:	긓긯?긓깛긚긣깋긏?
//!	@param	덙릶:	const	CImgClipedBmp&	src	: 긓긯?뙰귉궻랷뤖
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CImgClipedBmp::CImgClipedBmp( const CImgClipedBmp& src )
{
	Init() ;
	Copy( src ) ;
}


//!
//!			듫릶뼹:	CImgClipedBmp::Init
//!	@brief	??:	룊딖룉뿚
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CImgClipedBmp::Init( void )
{
	m_pBitmap = new CBitmapStd ;

	int	v[ 2 ] ;
	memset( v, 0x00, sizeof( v ) ) ;
	m_pBitmap->GetImgSize( v ) ;

	memset( &m_Clipinfo, 0x00, sizeof( m_Clipinfo ) ) ;
	m_Clipinfo.xs = m_Clipinfo.ys = 0 ;
	m_Clipinfo.xpitch = m_Clipinfo.ypitch = 1 ;
	m_Clipinfo.xpixel = v[ 0 ] ;
	m_Clipinfo.ypixel = v[ 1 ] ;
	m_Clipinfo.bitcount = m_pBitmap->GetBitCount() ;
}


//!
//!			듫릶뼹:	CImgClipedBmp::Copy
//!	@brief	??:	긓긯?
//!	@param	덙릶:	const	CImgClipedBmp&	src	: 긓긯?뙰귉궻랷뤖
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CImgClipedBmp::Copy( const CImgClipedBmp& src )
{
	*m_pBitmap = *src.m_pBitmap ;
	m_Clipinfo = src.m_Clipinfo ;
}


//!
//!			듫릶뼹:	CImgClipedBmp::SetClipInfo
//!	@brief	??:	긏깏긞긵륃뺪귩긜긞긣궥귡
//!	@param	덙릶:	_CLIPINFO&	info	: 긏깏긞긵륃뺪귉궻랷뤖
//!	@return	듫릶뭠:	궶궢
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
void CImgClipedBmp::SetClipInfo( _CLIPINFO& info )
{
	m_Clipinfo = info ;
	int	xsize = ( ( info.xpixel + info.xpitch - 1 ) / info.xpitch ) * info.xpitch ;
	xsize = ( xsize + 7 ) / 8 * 8 ;

	int	ysize = ( ( info.ypixel + info.ypitch - 1 ) / info.ypitch ) * info.ypitch ;

	m_pBitmap->MakeImageArea( xsize, ysize, info.bitcount, 256 ) ;
}


//!
//!			듫릶뼹:	CImgClipedBmp::GetClipInfo
//!	@brief	??:	긏깏긞긵륃뺪귩롦벦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	CImgClipedBmp::_CLIPINFO	: 긏깏긞긵륃뺪
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CImgClipedBmp::_CLIPINFO CImgClipedBmp::GetClipInfo( void )
{
	return ( m_Clipinfo ) ;
}


//!
//!			듫릶뼹:	CImgClipedBmp::GetBitmap
//!	@brief	??:	긮긞긣?긞긵귉궻?귽깛?귩롦벦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	CBitmapStd	*	: 긮긞긣?긞긵귉궻?귽깛?
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
CBitmapStd* CImgClipedBmp::GetBitmap( void )
{
	return ( m_pBitmap ) ;
}


//!
//!			듫릶뼹:	int	CImgClipedBmp::GetValidXsize
//!	@brief	??:	됪몴굓뾎뚼긖귽긛[PIX]귩롦벦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	int	: 됪몴굓뾎뚼긖귽긛[PIX]
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int	CImgClipedBmp::GetValidXsize( void )
{
	return ( m_Clipinfo.xpixel ) ;
}


//!
//!			듫릶뼹:	int	CImgClipedBmp::GetValidYsize
//!	@brief	??:	됪몴굕뾎뚼긖귽긛[PIX]귩롦벦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	int	:됪몴굕뾎뚼긖귽긛[PIX]
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int	CImgClipedBmp::GetValidYsize( void )
{
	return ( m_Clipinfo.ypixel ) ;
}


//!
//!			듫릶뼹:	int	CImgClipedBmp::GetPitchX
//!	@brief	??:	먛귟뢯궢긯긞?굓[PIX]귩롦벦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	int	: 먛귟뢯궢긯긞?굓[PIX]
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int	CImgClipedBmp::GetPitchX( void )
{
	return ( m_Clipinfo.xpitch ) ;
}


//!
//!			듫릶뼹:	int	CImgClipedBmp::GetPitchY
//!	@brief	??:	먛귟뢯궢긯긞?굕[PIX]귩롦벦궥귡
//!	@param	덙릶:	궶궢
//!	@return	듫릶뭠:	int	: 먛귟뢯궢긯긞?굕[PIX]
//!	@date	뛛륷:
//!	@note	됶먣:
//!	@note	럊뾭쀡:
//!	@note	뷈뛩:
//!
int	CImgClipedBmp::GetPitchY( void )
{
	return ( m_Clipinfo.ypitch ) ;
}
