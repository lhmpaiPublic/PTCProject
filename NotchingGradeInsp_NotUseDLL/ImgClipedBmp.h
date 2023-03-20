//	$Id: $
//!	@file	ImgClipedBmp.h
//!	@brief	
//!	@author	
//!	@date	2005.08.05
//!	@note	2005.08.05	doxygen뾭궻긓?긤귩믁돿갃렔벍맯맟갃


// ImgClipedBmp.h: CImgClipedBmp 긏깋긚궻귽깛??긲긃귽긚
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
		int	xs ;		//!< 럑?굓[PIX](int)x1
		int	ys ;		//!< 럑?굕[PIX](int)x1
		int	xpitch ;	//!< 긏깏긞긵긯긞?굓[PIX](int)x1
		int ypitch ;	//!< 긏깏긞긵긯긞?굕[PIX](int)x1
		int xpixel ;	//!< 긏깏긞긵궢궫됪멹릶굓[PIX](int)x1
		int ypixel ;	//!< 긏깏긞긵궢궫됪멹릶굕[PIX](int)x1
		int	bitcount ;	//!< 긮긞긣릶(int)x1
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
	CBitmapStd* m_pBitmap ;		//!< 긮긞긣?긞긵귉궻?귽깛?(CBitmapStd *)x1
	_CLIPINFO m_Clipinfo ;		//!< 긏깏긞긵륃뺪(_CLIPINFO)x1

};


#endif // !defined(AFX_IMGCLIPEDBMP_H__B041CB53_EEAC_4E1B_8490_0B6863E9FCA2__INCLUDED_)
