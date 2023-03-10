//	$Id: $
//!	@file	ImgClipedBmp.cpp
//!	@brief	
//!	@author	
//!	@date	2005.08.05
//!	@note	2005.08.05	doxygen用のコ?ドを追加。自動生成。


// ImgClipedBmp.cpp: CImgClipedBmp クラスのインプリメンテ?ション
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
// ?築/消滅
//////////////////////////////////////////////////////////////////////

//!
//!			関数名:	CImgClipedBmp::CImgClipedBmp
//!	@brief	??:	コンストラク?
//!	@param	引数:	なし
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
CImgClipedBmp::CImgClipedBmp()
{
	Init() ;

}


//!
//!			関数名:	CImgClipedBmp::~CImgClipedBmp
//!	@brief	??:	デストラク?
//!	@param	引数:	なし
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
CImgClipedBmp::~CImgClipedBmp()
{
	if ( m_pBitmap ) {
		delete	m_pBitmap ;
		m_pBitmap = NULL ;
	}
}


//!
//!			関数名:	CImgClipedBmp::operator=
//!	@brief	??:	代入演算子
//!	@param	引数:	const	CImgClipedBmp&	src	: 代入元への参照
//!	@return	関数値:	CImgClipedBmp	&	: 代入先への参照
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
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
//!			関数名:	CImgClipedBmp::CImgClipedBmp
//!	@brief	??:	コピ?コンストラク?
//!	@param	引数:	const	CImgClipedBmp&	src	: コピ?元への参照
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
CImgClipedBmp::CImgClipedBmp( const CImgClipedBmp& src )
{
	Init() ;
	Copy( src ) ;
}


//!
//!			関数名:	CImgClipedBmp::Init
//!	@brief	??:	初期処理
//!	@param	引数:	なし
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
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
//!			関数名:	CImgClipedBmp::Copy
//!	@brief	??:	コピ?
//!	@param	引数:	const	CImgClipedBmp&	src	: コピ?元への参照
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
void CImgClipedBmp::Copy( const CImgClipedBmp& src )
{
	*m_pBitmap = *src.m_pBitmap ;
	m_Clipinfo = src.m_Clipinfo ;
}


//!
//!			関数名:	CImgClipedBmp::SetClipInfo
//!	@brief	??:	クリップ情報をセットする
//!	@param	引数:	_CLIPINFO&	info	: クリップ情報への参照
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
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
//!			関数名:	CImgClipedBmp::GetClipInfo
//!	@brief	??:	クリップ情報を取得する
//!	@param	引数:	なし
//!	@return	関数値:	CImgClipedBmp::_CLIPINFO	: クリップ情報
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
CImgClipedBmp::_CLIPINFO CImgClipedBmp::GetClipInfo( void )
{
	return ( m_Clipinfo ) ;
}


//!
//!			関数名:	CImgClipedBmp::GetBitmap
//!	@brief	??:	ビット?ップへの?イン?を取得する
//!	@param	引数:	なし
//!	@return	関数値:	CBitmapStd	*	: ビット?ップへの?イン?
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
CBitmapStd* CImgClipedBmp::GetBitmap( void )
{
	return ( m_pBitmap ) ;
}


//!
//!			関数名:	int	CImgClipedBmp::GetValidXsize
//!	@brief	??:	画像Ｘ有効サイズ[PIX]を取得する
//!	@param	引数:	なし
//!	@return	関数値:	int	: 画像Ｘ有効サイズ[PIX]
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int	CImgClipedBmp::GetValidXsize( void )
{
	return ( m_Clipinfo.xpixel ) ;
}


//!
//!			関数名:	int	CImgClipedBmp::GetValidYsize
//!	@brief	??:	画像Ｙ有効サイズ[PIX]を取得する
//!	@param	引数:	なし
//!	@return	関数値:	int	:画像Ｙ有効サイズ[PIX]
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int	CImgClipedBmp::GetValidYsize( void )
{
	return ( m_Clipinfo.ypixel ) ;
}


//!
//!			関数名:	int	CImgClipedBmp::GetPitchX
//!	@brief	??:	切り出しピッ?Ｘ[PIX]を取得する
//!	@param	引数:	なし
//!	@return	関数値:	int	: 切り出しピッ?Ｘ[PIX]
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int	CImgClipedBmp::GetPitchX( void )
{
	return ( m_Clipinfo.xpitch ) ;
}


//!
//!			関数名:	int	CImgClipedBmp::GetPitchY
//!	@brief	??:	切り出しピッ?Ｙ[PIX]を取得する
//!	@param	引数:	なし
//!	@return	関数値:	int	: 切り出しピッ?Ｙ[PIX]
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int	CImgClipedBmp::GetPitchY( void )
{
	return ( m_Clipinfo.ypitch ) ;
}
