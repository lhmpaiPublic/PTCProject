//	$Id: $
//!	@file	ImgClipedBmp.cpp
//!	@brief	
//!	@author	
//!	@date	2005.08.05
//!	@note	2005.08.05	doxygen�p�̃R?�h��ǉ��B���������B


// ImgClipedBmp.cpp: CImgClipedBmp �N���X�̃C���v�������e?�V����
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
// ?�z/����
//////////////////////////////////////////////////////////////////////

//!
//!			�֐���:	CImgClipedBmp::CImgClipedBmp
//!	@brief	??:	�R���X�g���N?
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	�Ȃ�
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
CImgClipedBmp::CImgClipedBmp()
{
	Init() ;

}


//!
//!			�֐���:	CImgClipedBmp::~CImgClipedBmp
//!	@brief	??:	�f�X�g���N?
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	�Ȃ�
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
CImgClipedBmp::~CImgClipedBmp()
{
	if ( m_pBitmap ) {
		delete	m_pBitmap ;
		m_pBitmap = NULL ;
	}
}


//!
//!			�֐���:	CImgClipedBmp::operator=
//!	@brief	??:	������Z�q
//!	@param	����:	const	CImgClipedBmp&	src	: ������ւ̎Q��
//!	@return	�֐��l:	CImgClipedBmp	&	: �����ւ̎Q��
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
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
//!			�֐���:	CImgClipedBmp::CImgClipedBmp
//!	@brief	??:	�R�s?�R���X�g���N?
//!	@param	����:	const	CImgClipedBmp&	src	: �R�s?���ւ̎Q��
//!	@return	�֐��l:	�Ȃ�
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
CImgClipedBmp::CImgClipedBmp( const CImgClipedBmp& src )
{
	Init() ;
	Copy( src ) ;
}


//!
//!			�֐���:	CImgClipedBmp::Init
//!	@brief	??:	��������
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	�Ȃ�
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
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
//!			�֐���:	CImgClipedBmp::Copy
//!	@brief	??:	�R�s?
//!	@param	����:	const	CImgClipedBmp&	src	: �R�s?���ւ̎Q��
//!	@return	�֐��l:	�Ȃ�
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
void CImgClipedBmp::Copy( const CImgClipedBmp& src )
{
	*m_pBitmap = *src.m_pBitmap ;
	m_Clipinfo = src.m_Clipinfo ;
}


//!
//!			�֐���:	CImgClipedBmp::SetClipInfo
//!	@brief	??:	�N���b�v�����Z�b�g����
//!	@param	����:	_CLIPINFO&	info	: �N���b�v���ւ̎Q��
//!	@return	�֐��l:	�Ȃ�
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
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
//!			�֐���:	CImgClipedBmp::GetClipInfo
//!	@brief	??:	�N���b�v�����擾����
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	CImgClipedBmp::_CLIPINFO	: �N���b�v���
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
CImgClipedBmp::_CLIPINFO CImgClipedBmp::GetClipInfo( void )
{
	return ( m_Clipinfo ) ;
}


//!
//!			�֐���:	CImgClipedBmp::GetBitmap
//!	@brief	??:	�r�b�g?�b�v�ւ�?�C��?���擾����
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	CBitmapStd	*	: �r�b�g?�b�v�ւ�?�C��?
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
CBitmapStd* CImgClipedBmp::GetBitmap( void )
{
	return ( m_pBitmap ) ;
}


//!
//!			�֐���:	int	CImgClipedBmp::GetValidXsize
//!	@brief	??:	�摜�w�L���T�C�Y[PIX]���擾����
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	int	: �摜�w�L���T�C�Y[PIX]
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
int	CImgClipedBmp::GetValidXsize( void )
{
	return ( m_Clipinfo.xpixel ) ;
}


//!
//!			�֐���:	int	CImgClipedBmp::GetValidYsize
//!	@brief	??:	�摜�x�L���T�C�Y[PIX]���擾����
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	int	:�摜�x�L���T�C�Y[PIX]
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
int	CImgClipedBmp::GetValidYsize( void )
{
	return ( m_Clipinfo.ypixel ) ;
}


//!
//!			�֐���:	int	CImgClipedBmp::GetPitchX
//!	@brief	??:	�؂�o���s�b?�w[PIX]���擾����
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	int	: �؂�o���s�b?�w[PIX]
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
int	CImgClipedBmp::GetPitchX( void )
{
	return ( m_Clipinfo.xpitch ) ;
}


//!
//!			�֐���:	int	CImgClipedBmp::GetPitchY
//!	@brief	??:	�؂�o���s�b?�x[PIX]���擾����
//!	@param	����:	�Ȃ�
//!	@return	�֐��l:	int	: �؂�o���s�b?�x[PIX]
//!	@date	�X�V:
//!	@note	���:
//!	@note	�g�p��:
//!	@note	���l:
//!
int	CImgClipedBmp::GetPitchY( void )
{
	return ( m_Clipinfo.ypitch ) ;
}
