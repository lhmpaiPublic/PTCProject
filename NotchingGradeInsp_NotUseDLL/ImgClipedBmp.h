//	$Id: $
//!	@file	ImgClipedBmp.h
//!	@brief	
//!	@author	
//!	@date	2005.08.05
//!	@note	2005.08.05	doxygen�p�̃R?�h��ǉ��B���������B


// ImgClipedBmp.h: CImgClipedBmp �N���X�̃C��??�t�F�C�X
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
		int	xs ;		//!< �n?�w[PIX](int)x1
		int	ys ;		//!< �n?�x[PIX](int)x1
		int	xpitch ;	//!< �N���b�v�s�b?�w[PIX](int)x1
		int ypitch ;	//!< �N���b�v�s�b?�x[PIX](int)x1
		int xpixel ;	//!< �N���b�v������f���w[PIX](int)x1
		int ypixel ;	//!< �N���b�v������f���x[PIX](int)x1
		int	bitcount ;	//!< �r�b�g��(int)x1
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
	CBitmapStd* m_pBitmap ;		//!< �r�b�g?�b�v�ւ�?�C��?(CBitmapStd *)x1
	_CLIPINFO m_Clipinfo ;		//!< �N���b�v���(_CLIPINFO)x1

};


#endif // !defined(AFX_IMGCLIPEDBMP_H__B041CB53_EEAC_4E1B_8490_0B6863E9FCA2__INCLUDED_)
