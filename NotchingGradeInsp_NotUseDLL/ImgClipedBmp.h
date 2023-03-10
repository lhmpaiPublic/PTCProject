//	$Id: $
//!	@file	ImgClipedBmp.h
//!	@brief	
//!	@author	
//!	@date	2005.08.05
//!	@note	2005.08.05	doxygen用のコ?ドを追加。自動生成。


// ImgClipedBmp.h: CImgClipedBmp クラスのイン??フェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGCLIPEDBMP_H__B041CB53_EEAC_4E1B_8490_0B6863E9FCA2__INCLUDED_)
#define AFX_IMGCLIPEDBMP_H__B041CB53_EEAC_4E1B_8490_0B6863E9FCA2__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class	CBitmapStd ;


class CImgClipedBmp
{
public:
	typedef struct tagClipInfo {
		int	xs ;		//!< 始?Ｘ[PIX](int)x1
		int	ys ;		//!< 始?Ｙ[PIX](int)x1
		int	xpitch ;	//!< クリップピッ?Ｘ[PIX](int)x1
		int ypitch ;	//!< クリップピッ?Ｙ[PIX](int)x1
		int xpixel ;	//!< クリップした画素数Ｘ[PIX](int)x1
		int ypixel ;	//!< クリップした画素数Ｙ[PIX](int)x1
		int	bitcount ;	//!< ビット数(int)x1
	} _CLIPINFO ;

	int	GetValidXsize( void ) ;
	int	GetValidYsize( void ) ;
	int	GetPitchX( void ) ;
	int	GetPitchY( void ) ;

	void SetClipInfo( _CLIPINFO& info ) ;
	_CLIPINFO GetClipInfo( void ) ;

	CBitmapStd* GetBitmap( void ) ;

	CImgClipedBmp() ;
	virtual ~CImgClipedBmp() ;
	CImgClipedBmp& operator= ( const CImgClipedBmp& src ) ;
	CImgClipedBmp( const CImgClipedBmp& src ) ;

protected :
	void Init( void ) ;
	void Copy( const CImgClipedBmp& src ) ;

protected :
	CBitmapStd* m_pBitmap ;		//!< ビット?ップへの?イン?(CBitmapStd *)x1
	_CLIPINFO m_Clipinfo ;		//!< クリップ情報(_CLIPINFO)x1

};


#endif // !defined(AFX_IMGCLIPEDBMP_H__B041CB53_EEAC_4E1B_8490_0B6863E9FCA2__INCLUDED_)
