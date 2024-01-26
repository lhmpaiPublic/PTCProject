
#include "pch.h"
#include "BitmapStd.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if !defined( STOP_BOUNDS_CHECKER_EVT_REPORTING )
	#define	STOP_BOUNDS_CHECKER_EVT_REPORTING
#endif	// end of !defined( SAVE_ERROR_TEXT )

namespace
{
	TCHAR g_szLibrary[] = _T( "ImageCoder.dll" ) ;
	HMODULE	g_hDll = NULL ;

	typedef	int ( __cdecl *DLL_EncodeJpg	)( LPCTSTR szFile, BITMAPINFO* pInfo, BYTE* pImage, int nQuality ) ;
	typedef	int ( __cdecl *DLL_GetInfoJpg	)( LPCTSTR szFile, int* pnSizeH, int* pnSizeV, int* pnBitCount ) ;
	typedef	int ( __cdecl *DLL_DecodeJpg	)( LPCTSTR szFile, BITMAPINFO* pInfo, BYTE* pImage ) ;

	typedef	int ( __cdecl *DLL_EncodePng	)( LPCTSTR szFile, BITMAPINFO* pInfo, BYTE* pImage ) ;
	typedef	int ( __cdecl *DLL_GetInfoPng	)( LPCTSTR szFile, int* pnSizeH, int* pnSizeV, int* pnBitCount ) ;
	typedef	int ( __cdecl *DLL_DecodePng	)( LPCTSTR szFile, BITMAPINFO* pInfo, BYTE* pImage ) ;

	DLL_EncodeJpg	EncodeJpg	= NULL ;
	DLL_GetInfoJpg	GetInfoJpg	= NULL ;	
	DLL_DecodeJpg	DecodeJpg	= NULL ;

	DLL_EncodePng	EncodePng	= NULL ;
	DLL_GetInfoPng	GetInfoPng	= NULL ;
	DLL_DecodePng	DecodePng	= NULL ;

	int LoadImageCoderDll( void )
	{
		int nRet = 0 ;
		if ( g_hDll == NULL ) {
			g_hDll = ::LoadLibrary( g_szLibrary ) ;
			if ( g_hDll == NULL ) {
				TRACE( _T( "LoadLibrary Failure.(%s)\n" ), g_szLibrary ) ;
				nRet = -1 ;
			} else {
				EncodeJpg	= ( DLL_EncodeJpg)	GetProcAddress( g_hDll, "EncodeJpg" ) ;
				GetInfoJpg	= ( DLL_GetInfoJpg)	GetProcAddress( g_hDll, "GetInfoJpg" ) ;
				DecodeJpg	= ( DLL_DecodeJpg)	GetProcAddress( g_hDll, "DecodeJpg" ) ;
				EncodePng	= ( DLL_EncodePng)	GetProcAddress( g_hDll, "EncodePng" ) ;
				GetInfoPng	= ( DLL_GetInfoPng)	GetProcAddress( g_hDll, "GetInfoPng" ) ;
				DecodePng	= ( DLL_DecodePng)	GetProcAddress( g_hDll, "DecodePng" ) ;
				if ( EncodeJpg == NULL ) {
					TRACE( _T( "GetProcAddress Failure.(%s)\n" ), _T( "EncodeJpg" ) ) ;
					nRet = -1 ;
				}
				if ( GetInfoJpg == NULL ) {
					TRACE( _T( "GetProcAddress Failure.(%s)\n" ), _T( "GetInfoJpg" ) ) ;
					nRet = -1 ;
				}
				if ( DecodeJpg == NULL ) {
					TRACE( _T( "GetProcAddress Failure.(%s)\n" ), _T( "DecodeJpg" ) ) ;
					nRet = -1 ;
				}
				if ( EncodePng == NULL ) {
					TRACE( _T( "GetProcAddress Failure.(%s)\n" ), _T( "EncodePng" ) ) ;
					nRet = -1 ;
				}
				if ( GetInfoPng == NULL ) {
					TRACE( _T( "GetProcAddress Failure.(%s)\n" ), _T( "GetInfoPng" ) ) ;
					nRet = -1 ;
				}
				if ( DecodePng == NULL ) {
					TRACE( _T( "GetProcAddress Failure.(%s)\n" ), _T( "DecodePng" ) ) ;
					nRet = -1 ;
				}
			}
		}
		return ( nRet ) ;
	}

	int FreeImageCoderDll( void )
	{
		int nRet = 0 ;
		if ( g_hDll != NULL ) {
			STOP_BOUNDS_CHECKER_EVT_REPORTING ;
			if ( ::FreeLibrary( g_hDll ) == FALSE ) {
				TRACE( _T( "FreeLibrary Failure.\n" ) ) ;
				nRet = -1 ;
			} else {
				g_hDll		= NULL ;
				EncodeJpg	= NULL ;
				GetInfoJpg	= NULL ;
				DecodeJpg	= NULL ;
				EncodePng	= NULL ;
				GetInfoPng	= NULL ;
				DecodePng	= NULL ;
			}
		}
		return ( nRet ) ;
	}

	class CImageCoderDllLoader
	{
	public:
		CImageCoderDllLoader()
		{
			LoadImageCoderDll() ;
		}
		~CImageCoderDllLoader()
		{
			FreeImageCoderDll() ;
		}
	} ;

	CImageCoderDllLoader g_DllLoader ;
} ;


CBitmapStd::CBitmapStd( int nSizeX /*= 256*/, int nSizeY /*= 256*/, int nBitCount /*= 8*/ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	m_ptr = NULL ;
	Init( nSizeX, nSizeY, nBitCount ) ;
}


CBitmapStd::~CBitmapStd()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	if ( m_ptr != NULL ) {
		HeapFree( GetProcessHeap(), 0, m_ptr ) ;
		m_ptr = NULL ;
	}
}


CBitmapStd::CBitmapStd( const CBitmapStd &src )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	Init() ;

	*this = src ;
}


CBitmapStd& CBitmapStd::operator = ( const CBitmapStd& src )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
	CBitmapStd	*pSrc = &( ( CBitmapStd &)src ) ;
	ASSERT( pSrc != NULL ) ;
	CSingleLock	csSrc( pSrc->GetBmpCriticalSection(), TRUE ) ;
#endif
	if ( this == &src ) {
		return ( *this ) ;
	}

	if ( m_ptr != NULL ) {
		HeapFree( GetProcessHeap(), 0, m_ptr ) ;
		m_ptr = NULL ;
	}

	if ( src.m_ptr != NULL ) {
		void	*p = NULL ;
		BITMAPINFO	*pbmp = NULL ;
		int	colorused, xsize, ysize, bitcount ;

		pbmp = ( BITMAPINFO *)src.m_ptr ;
		colorused = pbmp->bmiHeader.biClrUsed ;
		xsize = pbmp->bmiHeader.biWidth ;
		ysize = pbmp->bmiHeader.biHeight ;
		bitcount = pbmp->bmiHeader.biBitCount ;
		SIZE_T	allocSize = 0 ;
		MakeImageArea( xsize, ysize, bitcount, colorused, &allocSize ) ;

		p = ( BITMAPINFO *)m_ptr ;
		if ( allocSize > 0 ) {
			memcpy( p, pbmp, allocSize ) ;
		}
	}

	m_colortype = src.m_colortype ;
	m_threshold = src.m_threshold ;
	m_nJpegQuality = src.m_nJpegQuality ;
	return ( *this ) ;
}


CBitmapStd& CBitmapStd::operator = ( const CBitmap& src )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	if ( m_ptr != NULL ) {
		HeapFree( GetProcessHeap(), 0, m_ptr ) ;
		m_ptr = NULL ;
	}

	CBitmap* pBmp = ( CBitmap *)&src ;
	BITMAP bmp ;
	memset( &bmp, 0, sizeof( BITMAP ) ) ;
	pBmp->GetBitmap( &bmp ) ;
	int	nBuffSize = bmp.bmWidthBytes * bmp.bmHeight ;
	BYTE* pBuff = new BYTE[ nBuffSize +1] ;
	memset(pBuff, 0x00, sizeof(BYTE) * nBuffSize +1);
	src.GetBitmapBits( nBuffSize, pBuff ) ;

	MakeImageArea( bmp.bmWidth, bmp.bmHeight, ( bmp.bmBitsPixel * bmp.bmPlanes ) ) ;

	CSize size = GetImgSize() ;
	int line_size = GetLineByteSize() ;
	BYTE* psrc = pBuff ;
	BYTE* pdst = NULL ;
	int y ;
	for ( y = 0 ; y < size.cy ; y++ ) {
		pdst = GetLinePtr( y ) ;
		memcpy( pdst, psrc, line_size ) ;
		UnlockBitmapinfoPtr() ;
		psrc += bmp.bmWidthBytes ;
	}

	if ( pBuff != NULL ) {
		delete [] pBuff ;
		pBuff = NULL ;
	}

	return ( *this ) ;

}


void CBitmapStd::Init( int nSizeX /*= 256*/, int nSizeY /*= 256*/, int nBitCount /*= 8*/ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	m_ptr = NULL ;
	MakeImageArea( nSizeX, nSizeY, nBitCount, 0 ) ;

//	Clear() ;
	m_ptraccess = NULL ;
	m_ReverseFlag = 0 ;

	m_poNow.x = 0 ;
	m_poNow.y = 0 ;
	m_colortype = 0 ;
	m_threshold = 128 ;
	m_nJpegQuality = 75 ;

}


int CBitmapStd::GetColor( POINT p )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	unsigned char	*hp = NULL ;
	BITMAPINFOHEADER	*bih = NULL ;
	int	color ;
	hp = ( unsigned	char *)m_ptr ;
	bih = ( BITMAPINFOHEADER *)hp ;
	int	nLineWidth = ( ( ( bih->biWidth * bih->biBitCount / 8 ) + 4 - 1 ) / 4 ) * 4 ;
	if ( ( p.x >= bih->biWidth ) || ( p.y >= bih->biHeight ) ) {
		return ( 0 ) ;
	} else {
		hp += sizeof( BITMAPINFOHEADER ) + bih->biClrUsed * sizeof( RGBQUAD ) ;
		if ( m_ReverseFlag & 0x01 ) {
			hp += ( p.y * nLineWidth ) + ( p.x * bih->biBitCount / 8 ) ;
		} else {
			hp += ( ( bih->biHeight - 1 - p.y ) * nLineWidth ) + ( p.x * bih->biBitCount / 8 ) ;
		}
		if ( bih->biBitCount == 24 ) {
			color = *( ( DWORD *)hp ) ;
		} else {
			color = *hp ;
		}
		return ( color ) ;
	}

	return ( 0 ) ;

}


int CBitmapStd::GetColor( int x, int y )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	POINT	p ;
	p.x = x ;
	p.y = y ;
	return ( GetColor( p ) ) ;
}


void CBitmapStd::SetPixel( POINT p, int color )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	unsigned char	*hp = NULL ;
	BITMAPINFOHEADER	*bih = NULL ;
	hp = ( unsigned	char *)m_ptr ;
	bih = ( BITMAPINFOHEADER *)hp ;
	hp += sizeof( BITMAPINFOHEADER ) + bih->biClrUsed * sizeof( RGBQUAD ) ;
	int	nLineWidth = ( ( ( bih->biWidth * bih->biBitCount / 8 ) + 4 - 1 ) / 4 ) * 4 ;
	hp += ( ( bih->biHeight - p.y - 1 ) * nLineWidth ) + ( p.x * bih->biBitCount / 8 ) ;
	if ( bih->biBitCount == 24 ) {
		*hp++ = color & 0x0000ff ;
		*hp++ = ( color >> 8 ) & 0x000000ff ;
		*hp++ = ( color >> 16 ) & 0x000000ff ;
	} else {
		*hp++ = color ;
	}

	return ;
}


int CBitmapStd::SetPixel( int x, int y, int color )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	POINT	p ;
	p.x = x ;
	p.y = y ;
	SetPixel( p, color ) ;
	return ( 0 ) ;
}


int CBitmapStd::MakeImageArea( int xsize, int ysize, int bitcount, int colorused , SIZE_T *allocSize /*= NULL*/ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif

	BITMAPINFO	*pbmi = NULL ;
	RGBQUAD	*bmiTable = NULL ;
	if ( m_ptr != NULL ) {
		HeapFree( GetProcessHeap(), 0, m_ptr ) ;
		m_ptr = NULL ;
	}
	int	xmemsize ;
	xmemsize = xsize + 3 & 0xffffffc ;

	if ( ( colorused == 0 ) && ( bitcount == 1 ) ) {
		colorused = 2 ;
	}
	if ( ( colorused == 0 ) && ( bitcount == 4 ) ) {
		colorused = 16 ;
	}
	if ( ( colorused == 0 ) && ( bitcount == 8 ) ) {
		colorused = 256 ;
	}
	if ( bitcount >= 24 ) {
		colorused = 0 ;
	}

	SIZE_T	bmpSize = sizeof( BITMAPINFOHEADER ) + colorused * sizeof( RGBQUAD ) + xmemsize * ysize * bitcount / 8 ;
	ASSERT( bmpSize > 0 ) ;
	if ( allocSize != NULL ) {
		*allocSize = bmpSize ;
	}
	ASSERT( m_ptr == NULL ) ;
	m_ptr = ( BYTE*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, bmpSize ) ;
	ASSERT( m_ptr != NULL ) ;
	if ( m_ptr == NULL ) {
		return ( -1 ) ;
	}

	pbmi = ( BITMAPINFO *)m_ptr ;

	pbmi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER ) ;
	pbmi->bmiHeader.biWidth = xsize ;
	pbmi->bmiHeader.biHeight = ysize ;
	pbmi->bmiHeader.biPlanes = 1 ;
	pbmi->bmiHeader.biBitCount = ( WORD )bitcount ;
	pbmi->bmiHeader.biCompression = BI_RGB ;
	pbmi->bmiHeader.biSizeImage = 0 ;
	bmiTable = pbmi->bmiColors ;

	int	n, cln ;

	cln = pbmi->bmiHeader.biClrUsed = colorused ;

	bmiTable = pbmi->bmiColors ;
	if ( ( colorused == 2 ) && ( bitcount == 1 ) ) {
	// -----MOMO-----
		for ( n = 0 ; n < cln ; n++ ) {
			bmiTable[ n ].rgbBlue = ( BYTE)n * 255 ;
			bmiTable[ n ].rgbRed= ( BYTE)n * 255 ;
			bmiTable[ n ].rgbGreen= ( BYTE)n * 255 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
	} else {
		for ( n = 0 ; n < cln ; n++ ) {
			bmiTable[ n ].rgbBlue = ( BYTE)n ;
			bmiTable[ n ].rgbRed= ( BYTE)n ;
			bmiTable[ n ].rgbGreen= ( BYTE)n ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
	}
	return ( 0 ) ;
}


int CBitmapStd::Clear()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif

	char	*ptr = NULL ;
	if ( m_ptr != NULL ) {
		BITMAPINFOHEADER	*bih = NULL ;
		ptr = ( char *)m_ptr ;
		bih = ( BITMAPINFOHEADER *)ptr ;
		ptr += sizeof( BITMAPINFOHEADER ) + bih->biClrUsed * sizeof( RGBQUAD ) ;
	//	int	nLineWidth = ( ( ( bih->biWidth * bih->biBitCount / 8 ) + 4 - 1 ) / 4 ) * 4 ;
		memset( ptr, 0, sizeof(BYTE)*( bih->biWidth * bih->biHeight ) ) ;
	}
	return ( 0 ) ;

}


void CBitmapStd::DrawImageFunction( CDC *cdc, RECT *rc, DWORD dwRop, int mReverse /*= FALSE*/, int nBltMode /*= -1*/ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif

	char	*ptr = NULL ;
	BITMAPINFO	*pbmi = NULL ;
	HBITMAP	hbm, old ;
	CDC	hdcMem ;
	BITMAP	bm ;

	if ( m_ptr == NULL ) {
		MakeImageArea() ;
	}

	ptr = ( char *)m_ptr ;
	pbmi = ( BITMAPINFO *)ptr ;
	ptr += sizeof( BITMAPINFOHEADER ) + pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
	hbm = ::CreateDIBitmap( cdc->m_hDC, ( BITMAPINFOHEADER *)pbmi, CBM_INIT, ptr, pbmi, DIB_RGB_COLORS ) ;

	if ( hbm == NULL ) {
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}
	if ( ::GetObject( hbm, sizeof( BITMAP ), ( LPSTR )&bm ) == 0 ) {
		DeleteObject( hbm ) ;
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}

	hdcMem.CreateCompatibleDC( cdc ) ;
	old = ( HBITMAP )hdcMem.SelectObject( hbm ) ;

	int	nBackBltMode = 0 ;
	POINT	poNewOrg ;
	poNewOrg.x = 0 ;
	poNewOrg.y = 0 ;
	POINT	poOrg ;
	poOrg.x = 0 ;
	poOrg.y = 0 ;
	if ( nBltMode != -1 ) {
		if ( nBltMode == HALFTONE ) {
			::GetBrushOrgEx( cdc->m_hDC, &poOrg ) ;
			poNewOrg.x = poOrg.x ;
			poNewOrg.y = poOrg.y ;
		}
		nBackBltMode = cdc->SetStretchBltMode( nBltMode ) ;
		if ( nBltMode == HALFTONE ) {
			::SetBrushOrgEx( cdc->m_hDC, poNewOrg.x, poNewOrg.y, &poOrg ) ;
		}
	}
	if ( mReverse == 0 ) {
		cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, dwRop ) ;
	} else {
		if ( mReverse == 1 ) {
			cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, 0, bm.bmHeight - 1, bm.bmWidth, -bm.bmHeight, dwRop ) ;
		} else if ( mReverse == 2 ) {
			cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, bm.bmWidth - 1, 0, -bm.bmWidth, bm.bmHeight, dwRop ) ;
		} else {
			cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, bm.bmWidth - 1, bm.bmHeight - 1, -bm.bmWidth, -bm.bmHeight, dwRop ) ;
		}
	}
	if ( nBltMode != -1 ) {
		cdc->SetStretchBltMode( nBackBltMode ) ;
		if ( nBltMode == HALFTONE ) {
			::SetBrushOrgEx( cdc->m_hDC, poNewOrg.x, poNewOrg.y, &poOrg ) ;
		}
	}

	m_ReverseFlag = mReverse ;

	hdcMem.SelectObject( old ) ;
	hdcMem.DeleteDC() ;

	DeleteObject( hbm ) ;

}


void CBitmapStd::DrawImage( CDC *cdc, RECT *rc, DWORD dwRop, int mReverse /* = FALSE */ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif

	char	*ptr = NULL ;
	BITMAPINFO	*pbmi = NULL ;
	HBITMAP	hbm, old ;
	CDC	hdcMem ;
	BITMAP	bm ;

	if ( m_ptr == NULL ) {
		MakeImageArea() ;
	}

	ptr = ( char *)m_ptr ;
	pbmi = ( BITMAPINFO *)ptr ;
	ptr += sizeof( BITMAPINFOHEADER ) + pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
	hbm = ::CreateDIBitmap( cdc->m_hDC, ( BITMAPINFOHEADER *)pbmi, CBM_INIT, ptr, pbmi, DIB_RGB_COLORS ) ;

	if ( hbm == NULL ) {
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}
	if ( ::GetObject( hbm, sizeof( BITMAP ), ( LPSTR )&bm ) == 0 ) {
		DeleteObject( hbm ) ;
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}

	hdcMem.CreateCompatibleDC( cdc ) ;
	old = ( HBITMAP )hdcMem.SelectObject( hbm ) ;

	if ( mReverse == 0 ) {
		cdc->BitBlt(rc->left, rc->top, rc->right-rc->left, (rc->bottom - rc->top), &hdcMem, 0, 0/*rc->bottom*/, dwRop ) ;
	} else {
		if ( mReverse == 1 ) {
			cdc->BitBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, 0, 0, dwRop ) ;
		} else if ( mReverse == 2 ) {
			cdc->BitBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, bm.bmWidth - 1, 0, dwRop ) ;
		} else {
			cdc->BitBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, bm.bmWidth - 1, bm.bmHeight - 1, dwRop ) ;
		}
	}

	m_ReverseFlag = mReverse ;

	hdcMem.SelectObject( old ) ;
	hdcMem.DeleteDC() ;

	DeleteObject( hbm ) ;
}


void CBitmapStd::DrawImageFunction( CDC *cdc, RECT *rcDraw, RECT *rcImg, DWORD dwRop /*= SRCCOPY*/, int mReverse /*= FALSE*/ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif

	char	*ptr = NULL ;
	BITMAPINFO	*pbmi = NULL ;
	HBITMAP	hbm, old ;
	CDC	hdcMem ;
	BITMAP	bm ;

	if ( m_ptr == NULL ) {
		MakeImageArea() ;
	}

	ptr = ( char *)m_ptr ;
	pbmi = ( BITMAPINFO *)ptr ;
	ptr += sizeof( BITMAPINFOHEADER ) + pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
	hbm = ::CreateDIBitmap( cdc->m_hDC, ( BITMAPINFOHEADER *)pbmi, CBM_INIT, ptr, pbmi, DIB_RGB_COLORS ) ;

	if ( hbm == NULL ) {
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}
	if ( ::GetObject( hbm, sizeof( BITMAP ), ( LPSTR )&bm ) == 0 ) {
		DeleteObject( hbm ) ;
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}

	hdcMem.CreateCompatibleDC( cdc ) ;
	old = ( HBITMAP )hdcMem.SelectObject( hbm ) ;

	int	nSrcWidth, nSrcHeight ;
	RECT	rcSrc ;

	if ( rcImg == NULL ) {
		rcSrc.left = rcSrc.top = 0 ;
		rcSrc.right = bm.bmWidth ;
		rcSrc.bottom = bm.bmHeight ;
	} else {
		rcSrc.left = rcImg->left ;
		rcSrc.right = rcImg->right ;
		rcSrc.top = rcImg->top ;
		rcSrc.bottom = rcImg->bottom ;
	}


	nSrcWidth = rcSrc.right - rcSrc.left ;
	if ( nSrcWidth > bm.bmWidth ) {
		nSrcWidth = bm.bmWidth ;
	}

	nSrcHeight = rcSrc.bottom - rcSrc.top ;
	if ( nSrcHeight > bm.bmHeight ) {
		nSrcHeight = bm.bmHeight ;
	}
	BOOL bConvert ;

	if ( ( abs( rcDraw->left - rcDraw->right ) < abs( rcSrc.left - rcSrc.right ) )
		|| ( abs( rcDraw->top - rcDraw->bottom ) < abs( rcSrc.top - rcSrc.bottom ) ) ) {
		bConvert = TRUE ;
	} else {
		bConvert = FALSE ;
	}


	int	iBltMode ;

	if ( bConvert ) {
		iBltMode = cdc->SetStretchBltMode( HALFTONE ) ;
	}

	if ( mReverse == 0 ) {
		cdc->StretchBlt( rcDraw->left, rcDraw->top, rcDraw->right- rcDraw->left, rcDraw->bottom - rcDraw->top, &hdcMem, rcSrc.left, rcSrc.top, nSrcWidth, nSrcHeight, dwRop ) ;
	} else {
		if ( mReverse == 1 ) {
			cdc->StretchBlt( rcDraw->left, rcDraw->top, rcDraw->right - rcDraw->left, rcDraw->bottom - rcDraw->top,
							 &hdcMem, rcSrc.left, nSrcHeight - 1, nSrcWidth, -nSrcHeight, dwRop ) ;
		} else if ( mReverse == 2 ) {
			cdc->StretchBlt( rcDraw->left, rcDraw->top, rcDraw->right - rcDraw->left, rcDraw->bottom - rcDraw->top,
				&hdcMem, nSrcWidth - 1, rcSrc.top, - nSrcWidth, nSrcHeight, dwRop ) ;
		} else {
			cdc->StretchBlt( rcDraw->left, rcDraw->top, rcDraw->right - rcDraw->left, rcDraw->bottom - rcDraw->top,
				&hdcMem, nSrcWidth - 1, nSrcHeight - 1, -nSrcWidth, -nSrcHeight, dwRop ) ;
		}
	}
	if ( bConvert ) {
		cdc->SetStretchBltMode( iBltMode ) ;
	}

	m_ReverseFlag = mReverse ;

	hdcMem.SelectObject( old ) ;
	hdcMem.DeleteDC() ;

	DeleteObject( hbm ) ;
}


int CBitmapStd::ReadBitmap( CWnd *pwnd, CString *pszSelectName /*= NULL*/ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	CString	szFilters = _T("BitMapFiles(*.bmp)|*.bmp|AllFiles(*.*)|*.*||") ;

	if ( pwnd == NULL ) {
		pwnd = AfxGetMainWnd() ;
	}
	CString	szDefaultName = _T("*.bmp") ;
	if ( pszSelectName != NULL ) {
		int	iPosition = pszSelectName->ReverseFind( '\\' ) ;
		if ( iPosition >= 0 ) {
			if ( pszSelectName->GetLength() > ( iPosition + 1 ) ) {
				szDefaultName = pszSelectName->Mid( iPosition + 1 ) ;
			}
		}
	}

	CFileDialog dlg( TRUE, _T("bmp"), szDefaultName, OFN_HIDEREADONLY, szFilters, pwnd ) ;

	if ( dlg.DoModal() == IDOK ) {
		CString filename ;
		filename = dlg.GetPathName() ;
		if ( ReadBitmap( filename ) == 0 ) {
			if ( pszSelectName != NULL ) {
				*pszSelectName = filename ;
			}
		} else {
			return ( -1 ) ;
		}
	} else {
		return ( -1 ) ;
	}

	return ( 0 ) ;
}


int CBitmapStd::ReadBitmap( CString &filepath )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	CWaitCursor	wc ;
	int	nRet = 0 ;

	try {
		int	j ;
		CString	expname ;

		j = filepath.ReverseFind( _T( '.' ) ) ;
		if ( j <= 0 ) {
			filepath += _T( ".BMP" ) ;
			expname = _T( "BMP" ) ;
		} else {
			expname = filepath.Right( filepath.GetLength() - j - 1 ) ;
		}
		expname.MakeUpper() ;

		if ( expname.Compare( _T( "BMP" ) ) == 0 ) {
			BITMAPFILEHEADER	bmfh ;
			DWORD	xsize, ysize ;
			DWORD	xsize4, imgmemsize ;

			CFile file ;
			if ( file.Open( filepath, CFile::modeRead ) == 0 ) {
				return ( -1 ) ;
			}

			if ( m_ptr != NULL ) {
				HeapFree( GetProcessHeap(), 0, m_ptr ) ;
				m_ptr = NULL ;
			}

			file.Read( &bmfh, sizeof( BITMAPFILEHEADER ) ) ;

			{
				char *buffer = NULL ;
				BITMAPINFO *pbmi, *pbmj = NULL ;
				BYTE	*BufPtr = NULL ;
				BYTE	*IP = NULL ;

				buffer = ( char *)malloc( sizeof( BITMAPINFOHEADER ) ) ;
				file.Read( buffer, sizeof( BITMAPINFOHEADER ) ) ;

				pbmj = ( BITMAPINFO *)buffer ;
				if ( ( pbmj->bmiHeader.biClrUsed == 0 )
				  && ( bmfh.bfOffBits != ( sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER ) ) ) ) {
					DWORD	dwBits ;
					{
						switch ( pbmj->bmiHeader.biBitCount ) {
						case	1 :
						case	4 :
						case	8 :
							dwBits = ( DWORD)pow( 2.0, pbmj->bmiHeader.biBitCount ) ;
							pbmj->bmiHeader.biClrUsed = dwBits ;
							break ;
						default:
							pbmj->bmiHeader.biClrUsed = 0 ;
							break ;
						}
					}
				}

				xsize = pbmj->bmiHeader.biWidth ;
				ysize = pbmj->bmiHeader.biHeight ;
				xsize4 = ( ( ( xsize * pbmj->bmiHeader.biBitCount / 8 ) + 3 ) / 4 ) * 4 ;
				imgmemsize = ( xsize4 * ysize ) ;
				m_ptr = ( BYTE*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY,
									sizeof( BITMAPINFOHEADER ) + imgmemsize
									+ pbmj->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ) ;

				IP = ( BYTE *)m_ptr ;
				pbmi = ( BITMAPINFO *)IP ;
				if ( pbmi == NULL ) {
					file.Close();
					AfxMessageBox( _T( "Memory Lock False Error." ) ) ;
					return ( -1 ) ;
				}

				memcpy( pbmi, pbmj, sizeof( BITMAPINFOHEADER ) ) ;
				free ( buffer ) ;
				buffer = NULL ;

				file.Read( IP + sizeof( BITMAPINFOHEADER ), ( sizeof( RGBQUAD ) * pbmi->bmiHeader.biClrUsed ) ) ;

				DWORD line_mem, line_file ;
				DWORD y ;

				line_mem = xsize4 ;
				if ( ( imgmemsize > pbmi->bmiHeader.biSizeImage ) && ( pbmi->bmiHeader.biSizeImage != 0 ) ) {
					line_file = xsize * pbmi->bmiHeader.biBitCount / 8 ;
				} else {
					line_file = line_mem ;
				}

				BufPtr = IP + sizeof( BITMAPINFOHEADER )
							+ pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
				if ( m_ReverseFlag & 0x01 ) {
					BufPtr += imgmemsize ;
					BufPtr -= line_mem ;
					for ( y = 0 ; y < ysize ; y++ )
					{
						file.Read( BufPtr, line_file ) ;
						BufPtr -= line_mem ;
					}
				} else {
					for ( y = 0 ; y < ysize ; y++ )
					{
						file.Read( BufPtr, line_file ) ;
						BufPtr += line_mem ;
					}
				}

				file.Close() ;

			}
		} else if ( ( expname.Compare( _T( "JPG" ) ) == 0 ) || ( expname.Compare( _T( "JPEG" ) ) == 0 ) ) {
			if ( ( GetInfoJpg != NULL ) && ( DecodeJpg != NULL ) ) {
				int nSizeH = 0 ;
				int nSizeV = 0 ;
				int nBitCount = 0 ;
				if ( GetInfoJpg( filepath, &nSizeH, &nSizeV, &nBitCount ) < 0 ) {
					nRet = -1 ;
				} else {
					MakeImageArea( nSizeH, nSizeV, nBitCount ) ;
					BITMAPINFO* pbmi = ( BITMAPINFO *)m_ptr ;
					BYTE* pimg = ( BYTE *)pbmi ;
					pimg += sizeof( BITMAPINFOHEADER ) ;
					pimg += pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
					if ( DecodeJpg( filepath, pbmi, pimg ) < 0 ) {
						nRet = -1 ;
					}
				}
			} else {
				nRet = -1 ;
				CString strMsg ;
				strMsg.Format( _T( "Please check the library file \"%s\"." ), g_szLibrary ) ;
				AfxMessageBox( strMsg ) ;
			}
		} else if ( expname.Compare( _T( "PNG" ) ) == 0 ) {
			if ( ( GetInfoPng != NULL ) && ( DecodePng != NULL ) ) {
				int nSizeH = 0 ;
				int nSizeV = 0 ;
				int nBitCount = 0 ;
				if ( GetInfoPng( filepath, &nSizeH, &nSizeV, &nBitCount ) < 0 ) {
					nRet = -1 ;
				} else {
					MakeImageArea( nSizeH, nSizeV, nBitCount ) ;
					BITMAPINFO* pbmi = ( BITMAPINFO *)m_ptr ;
					BYTE* pimg = ( BYTE *)pbmi ;
					pimg += sizeof( BITMAPINFOHEADER ) ;
					pimg += pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
					if ( DecodePng( filepath, pbmi, pimg ) < 0 ) {
						nRet = -1 ;
					}
				}
			} else {
				nRet = -1 ;
				CString strMsg ;
				strMsg.Format( _T( "Please check the library file \"%s\"." ), g_szLibrary ) ;
				AfxMessageBox( strMsg ) ;
			}
		}
	} catch ( ... ) {
		nRet = -1 ;
	}
	return ( nRet ) ;
}


int CBitmapStd::SaveBitmap( CWnd *pwnd, CString *pszSelectName /*= NULL*/ )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	nRet = 1 ;
	// char	szFilters[] = "ImageFiles(*.bmp)|*.bmp|AllFiles(*.*)|*.*||" ;
	static char BASED_CODE szFilter[] = "Image Files (*.bmp;*.JPG;*.JPEG)|*.bmp;*.JPG;*.JPEG|All Files (*.*)|*.*||" ;

	if ( pwnd == NULL ) {
		pwnd = AfxGetMainWnd() ;
	}
	CString	szDefaultName = _T( "*.bmp" ) ;
	if ( pszSelectName != NULL ) {
		int	iPosition = pszSelectName->ReverseFind( '\\' ) ;
		if ( iPosition >= 0 ) {
			if ( pszSelectName->GetLength() > ( iPosition + 1 ) ) {
				szDefaultName = pszSelectName->Mid( iPosition + 1 ) ;
			}
		}
	}
	CFileDialog dlg( FALSE, _T( "bmp" ), szDefaultName, OFN_HIDEREADONLY, szFilter, pwnd ) ;
	CString fname ;

	if ( dlg.DoModal() == IDOK ) {
		nRet = 0 ;
		fname = dlg.GetPathName() ;
		if ( SaveBitmap( fname ) != 0 ) {
			nRet = -1 ;
		}
		if ( pszSelectName != NULL ) {
			*pszSelectName = fname ;
		}
	}
	return ( nRet ) ;
}


int CBitmapStd::SaveBitmap( CWnd *pwnd, LPCTSTR pszInitDir )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	nRet = 1 ;
	// char	szFilters[] = "ImageFiles(*.bmp)|*.bmp|AllFiles(*.*)|*.*||" ;
	static char BASED_CODE szFilter[] = "Image Files (*.bmp;*.JPG;*.JPEG)|*.bmp;*.JPG;*.JPEG|All Files (*.*)|*.*||" ;

	if ( pwnd == NULL ) {
		pwnd = AfxGetMainWnd() ;
	}
	CFileDialog dlg( FALSE, _T( "bmp" ), _T("*.bmp"), OFN_HIDEREADONLY, szFilter, pwnd ) ;

	dlg.m_ofn.lpstrInitialDir = pszInitDir ;
	if ( dlg.DoModal() == IDOK ) {
		CString fname ;
		nRet = 0 ;
		fname = dlg.GetPathName() ;
		if ( SaveBitmap( fname ) != 0 ) {
			nRet = -1 ;
		}
	}
	return ( nRet ) ;
}

int CBitmapStd::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders  
	UINT  size = 0;         // size of the image encoder array in bytes  

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure  

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success  
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure  
}

void CBitmapStd::grayscale(HDC hdc, HBITMAP& hbitmap)
{
	BITMAP bm;
	GetObject(hbitmap, sizeof(bm), &bm);
	if (bm.bmBitsPixel < 24)
	{
		DebugBreak();
		return;
	}

	DWORD size = ((bm.bmWidth * bm.bmBitsPixel + 31) / 32) * 4 * bm.bmHeight;
	BITMAPINFO bmi
	{ sizeof(BITMAPINFOHEADER),bm.bmWidth,bm.bmHeight,1,bm.bmBitsPixel,BI_RGB,size };
	int stride = bm.bmWidth + (bm.bmWidth * bm.bmBitsPixel / 8) % 4;
	BYTE* bits = new BYTE[size];
	GetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
	for (int y = 0; y < bm.bmHeight; y++) {
		for (int x = 0; x < stride; x++) {
			int i = (x + y * stride) * bm.bmBitsPixel / 8;
			BYTE gray = BYTE(0.1 * bits[i + 0] + 0.6 * bits[i + 1] + 0.3 * bits[i + 2]);
			bits[i + 0] = bits[i + 1] = bits[i + 2] = gray;
		}
	}
	SetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
	ReleaseDC(HWND_DESKTOP, hdc);
	delete[]bits;
}

int CBitmapStd::SaveBitmap( CString &filepath )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	CWaitCursor	wc ;
	int	nRet = 0 ;
	try {
		unsigned char	*BufPtr = NULL, *IP = NULL ;
		BITMAPFILEHEADER	bmfh ;
		BITMAPINFO	*pbmi = NULL ;
		CString filename, bmpfname, expname ;
		int	j ;
		j = filepath.ReverseFind( _T( '.' ) ) ;
		if ( j <= 0 ) {
			filename = filepath ;
			filepath += _T( ".BMP" ) ;
			expname = _T( "BMP" ) ;
		} else {
			expname = filepath.Right( filepath.GetLength() - j - 1 ) ;
			filename = filepath.Left( j ) ;
		}
		expname.MakeUpper() ;

		if ( expname.Compare( _T( "BMP" ) ) == 0 ) {

			//CFile file( filepath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite ) ;
			CFile file( filepath, CFile::modeCreate | CFile::modeWrite ) ;

			IP = ( unsigned char *)m_ptr ;
			pbmi = ( BITMAPINFO *)IP ;

			if ( IP == NULL ) {
				AfxMessageBox( _T( "Memory Lock False Error." ) ) ;
				return ( -1 ) ;
			}

			if ( file.m_hFile == 0 ) {
				AfxMessageBox( _T( "File Make Error" ) ) ;
				return ( -1 ) ;
			}

			bmfh.bfType = *( WORD * )( _T( "BM" ) ) ;

			DWORD	imgmemsize ;
			DWORD	xsize, xsize4, ysize ;
			DWORD	line_mem ;
			int	bitcnt = pbmi->bmiHeader.biBitCount ;
			DWORD	y ;

			xsize = pbmi->bmiHeader.biWidth ;
			ysize = pbmi->bmiHeader.biHeight ;
			xsize4 = ( ( ( xsize * bitcnt / 8 ) + 3 ) / 4 ) * 4 ;
			line_mem = xsize4 ;
			imgmemsize = line_mem * ysize ;

			bmfh.bfSize = sizeof( BITMAPFILEHEADER )
						+ sizeof( BITMAPINFOHEADER )
						+ sizeof( RGBQUAD ) * pbmi->bmiHeader.biClrUsed
						+ imgmemsize ;
			bmfh.bfReserved1 = 0 ;
			bmfh.bfReserved2 = 0 ;
			bmfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * pbmi->bmiHeader.biClrUsed ;

			file.Write( ( LPCSTR )&bmfh, sizeof( BITMAPFILEHEADER ) ) ;
			file.Write( ( LPCSTR )pbmi, sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * pbmi->bmiHeader.biClrUsed ) ;

			if ( m_ReverseFlag & 0x01 )
			{
				BufPtr = IP + sizeof( BITMAPINFOHEADER ) + pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) + imgmemsize ;
				BufPtr -= line_mem ;
				for ( y = 0 ; y < ysize ; y++ ) {
					file.Write( BufPtr, line_mem ) ;
					BufPtr -= line_mem ;
				}
			} else {
				BufPtr = IP + sizeof( BITMAPINFOHEADER ) + pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
				for ( y = 0 ; y < ysize ; y++ ) {
					file.Write( BufPtr, line_mem ) ;
					BufPtr += line_mem ;
				}
			}

			file.Close() ;
		} else if ( ( expname.Compare( _T( "JPG" ) ) == 0 ) || ( expname.Compare( _T( "JPEG" ) ) == 0 ) ) {
			if ( EncodeJpg != NULL ) {
				BITMAPINFO* pbmi = ( BITMAPINFO *)m_ptr ;
				BYTE* pimg = ( BYTE *)pbmi ;
				pimg += sizeof( BITMAPINFOHEADER ) ;
				pimg += pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
				if ( EncodeJpg( filepath, pbmi, pimg, m_nJpegQuality ) < 0 ) {
					nRet = -1 ;
				}
			} else {
				nRet = -1 ;
				CString strMsg ;
				strMsg.Format( _T( "Please check the library file \"%s\"." ), g_szLibrary ) ;
				AfxMessageBox( strMsg ) ;
			}
			//int nQuality = m_nJpegQuality;
			//ULONG colorDeph = 8;

			//Gdiplus::EncoderParameters* pEncoderParameters = (Gdiplus::EncoderParameters*)malloc(sizeof(Gdiplus::EncoderParameters)
			//	+ 1 * sizeof(Gdiplus::EncoderParameter));

			//pEncoderParameters->Count = 2;
			//pEncoderParameters->Parameter[0].NumberOfValues = 1;
			//pEncoderParameters->Parameter[0].Guid = Gdiplus::EncoderQuality;
			//pEncoderParameters->Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			//pEncoderParameters->Parameter[0].Value = &nQuality;
			//pEncoderParameters->Parameter[1].NumberOfValues = 1;
			//pEncoderParameters->Parameter[1].Guid = Gdiplus::EncoderColorDepth;
			//pEncoderParameters->Parameter[1].Type = Gdiplus::EncoderParameterValueTypeLong;
			//pEncoderParameters->Parameter[1].Value = &colorDeph;

			//HDC hdc = GetDC(HWND_DESKTOP);

			//BITMAPINFO* pbmi = (BITMAPINFO*)m_ptr;
			//BYTE* pimg = (BYTE*)pbmi;
			//pimg += sizeof(BITMAPINFOHEADER);
			//pimg += pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD);
			//HBITMAP hbm = ::CreateDIBitmap(hdc, (BITMAPINFOHEADER*)pbmi, CBM_INIT, pimg, pbmi, DIB_RGB_COLORS);
			//grayscale(hdc, hbm);

			//HPALETTE hpal;
			//Gdiplus::Bitmap* image;
			//image = Gdiplus::Bitmap::FromHBITMAP(hbm, NULL);
			//CLSID   encoderClsid;
			//// Get the CLSID of the JPEG encoder.
			//GetEncoderClsid(L"image/jpeg", &encoderClsid);
			//CStringW wStr = (CStringW)filepath.GetBuffer(0);
			//image->Save(wStr, &encoderClsid, pEncoderParameters);
			//Gdiplus::ImageFormatBMP;

			//BITMAPINFO* pbmi = (BITMAPINFO*)m_ptr;
			//BYTE* pimg = (BYTE*)pbmi;
			//pimg += sizeof(BITMAPINFOHEADER);
			//pimg += pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD);

			//Gdiplus::Bitmap* image;
			//image = Gdiplus::Bitmap::FromBITMAPINFO(pbmi, pimg);

			//Gdiplus::REAL w = (Gdiplus::REAL)image->GetWidth();
			//Gdiplus::REAL h = (Gdiplus::REAL)image->GetHeight();
			//Gdiplus::RectF rect(0, 0, w, h);
			//Gdiplus::Bitmap copy((INT)w, (INT)h, image->GetPixelFormat());
			//Gdiplus::ColorMatrix matrix =
			//{
			//	.3f, .3f, .3f,   0,   0,
			//	.6f, .6f, .6f,   0,   0,
			//	.1f, .1f, .1f,   0,   0,
			//	0,   0,   0,   1,   0,
			//	0,   0,   0,   0,   1
			//};
			//Gdiplus::ImageAttributes attr;
			//attr.SetColorMatrix(&matrix,
			//	Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
			//Gdiplus::Graphics gr(image);
			//gr.DrawImage(image, rect, 0, 0, w, h, Gdiplus::UnitPixel, &attr);

			//int nQuality = m_nJpegQuality;
			//Gdiplus::EncoderParameters EncoderParameters;
			//EncoderParameters.Count = 1;
			//EncoderParameters.Parameter[0].NumberOfValues = 1;
			//EncoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
			//EncoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			//EncoderParameters.Parameter[0].Value = &nQuality;

			//CLSID   encoderClsid;
			//// Get the CLSID of the JPEG encoder.
			//GetEncoderClsid(L"image/jpeg", &encoderClsid);
			//CStringW wStr = (CStringW)filepath.GetBuffer(0);
			//image->Save(wStr, &encoderClsid, &EncoderParameters);
		} else if ( expname.Compare( _T( "PNG" ) ) == 0 ) {
			if ( EncodePng != NULL ) {
				BITMAPINFO* pbmi = ( BITMAPINFO *)m_ptr ;
				BYTE* pimg = ( BYTE *)pbmi ;
				pimg += sizeof( BITMAPINFOHEADER ) ;
				pimg += pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
				if ( EncodePng( filepath, pbmi, pimg ) < 0 ) {
					nRet = -1 ;
				}
			} else {
				nRet = -1 ;
				CString strMsg ;
				strMsg.Format( _T( "Please check the library file \"%s\"." ), g_szLibrary ) ;
				AfxMessageBox( strMsg ) ;
			}
		}
	} catch ( CException* e ) {
		TCHAR szError[ 512 ] ;
		memset( szError, 0x00, sizeof( szError ) ) ;
		e->GetErrorMessage( szError, 512 ) ;
		e->Delete() ;
		nRet = -1 ;
	} catch ( ... ) {
		nRet = -1 ;
	}
	return ( nRet ) ;
}

//GdiPlus JPG저장 함수
int CBitmapStd::SaveJPG(CString& filepath)
{
	int	nRet = 0;
	try
	{
		BITMAPINFO* pbmi = NULL;
		CString filename, expname;
		int	j;
		j = filepath.ReverseFind(_T('.'));
		if (j <= 0)
		{
			filename = filepath;
			filepath += _T(".JPG");
			expname = _T("JPG");
		}
		else
		{
			expname = filepath.Right(filepath.GetLength() - j - 1);
			filename = filepath.Left(j);
		}
		expname.MakeUpper();

		if ((expname.Compare(_T("JPG")) == 0) || (expname.Compare(_T("JPEG")) == 0))
		{

			BITMAPINFO* pbmi = (BITMAPINFO*)m_ptr;
			BYTE* pimg = (BYTE*)pbmi;
			pimg += sizeof(BITMAPINFOHEADER);
			pimg += pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD);

			Gdiplus::Bitmap* image;
			image = Gdiplus::Bitmap::FromBITMAPINFO(pbmi, pimg);

			int nQuality = m_nJpegQuality;
			Gdiplus::EncoderParameters EncoderParameters;
			EncoderParameters.Count = 1;
			EncoderParameters.Parameter[0].NumberOfValues = 1;
			EncoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
			EncoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			EncoderParameters.Parameter[0].Value = &nQuality;

			CLSID   encoderClsid;
			// Get the CLSID of the JPEG encoder.
			GetEncoderClsid(L"image/jpeg", &encoderClsid);
			CStringW wStr = (CStringW)filepath.GetBuffer(0);
			image->Save(wStr, &encoderClsid, &EncoderParameters);

			delete image;
		}
	}
	catch (CException* e)
	{
		TCHAR szError[512];
		memset(szError, 0x00, sizeof(szError));
		e->GetErrorMessage(szError, 512);
		e->Delete();
		nRet = -1;
	}
	catch (...)
	{
		nRet = -1;
	}
	return (nRet);
}


void CBitmapStd::GetImgSize( int *v )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFOHEADER	*pbmi = NULL ;
	pbmi = ( BITMAPINFOHEADER *)m_ptr ;

	v[ 0 ] = pbmi->biWidth ;
	v[ 1 ] = pbmi->biHeight ;

	return ;
}


CSize CBitmapStd::GetImgSize( void )
{
	int v[ 2 ] ;
	memset( v, 0x00, sizeof( v ) ) ;
	GetImgSize( v ) ;
	CSize szRet( v[ 0 ], v[ 1 ] ) ;
	return ( szRet ) ;
}


int	CBitmapStd::GetBitCount()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	r ;
	BITMAPINFOHEADER	*pbmi = NULL ;
	pbmi = ( BITMAPINFOHEADER *)m_ptr ;
	r = pbmi->biBitCount ;
	return ( r ) ;
}


BITMAPINFO * CBitmapStd::GetBitmapinfoPtr()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFO	*bi = NULL ;
	bi = ( BITMAPINFO *)m_ptr ;
	return ( bi ) ;
}


int	CBitmapStd::getc()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	j ;
	if ( m_ptraccess ) {
		j = *m_ptraccess++ ;
		return ( j ) ;
	} else {
		return ( 0 ) ;
	}
}


void CBitmapStd::putc( int v )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	if ( m_ptraccess ) {
		*m_ptraccess++ = ( BYTE )v ;
	}
	return ;
}


BYTE *CBitmapStd::GetImgPtr()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFO	*bi = NULL ;
	BYTE	*ptr = NULL ;
	bi = ( BITMAPINFO *)m_ptr ;
	ptr = ( BYTE *)bi ;
	ptr += sizeof( BITMAPINFOHEADER ) ;
	ptr += bi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
	m_ptraccess = ptr ;
	return ( ptr ) ;
}


BYTE *CBitmapStd::GetImgEndPtr()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFO	*bi = NULL ;
	BYTE	*ptr = NULL ;
	bi = ( BITMAPINFO *)m_ptr ;
	ptr = ( BYTE *)bi ;
	ptr += sizeof( BITMAPINFOHEADER ) ;
	ptr += bi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
	int	nLineWidth = ( ( ( bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount / 8 ) + 4 - 1 ) / 4 ) * 4 ;
	ptr += ( nLineWidth * bi->bmiHeader.biHeight ) - 1 ;
	m_ptraccess = ptr ;
	return ( ptr ) ;
}


BYTE *CBitmapStd::GetColorTablePtr()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFO	*bi = NULL ;
	BYTE	*ptr = NULL ;
	bi = ( BITMAPINFO *)m_ptr ;
	ptr = ( BYTE *)bi ;
	ptr += sizeof( BITMAPINFOHEADER ) ;
	m_ptraccess = ptr ;
	return ( ptr ) ;
}


void CBitmapStd::UnlockBitmapinfoPtr()
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	m_ptraccess = NULL ;
}


int CBitmapStd::GetClrUsedCnt( void )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFO	*pbmi = NULL ;
	int	cln ;
	pbmi = ( BITMAPINFO *)m_ptr ;
	cln = pbmi->bmiHeader.biClrUsed ;

	return ( cln ) ;
}


LPRGBQUAD CBitmapStd::GetRgbQuadPtr( void )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFO	*pbmi = NULL ;
	RGBQUAD	*bmiTable = NULL ;
	pbmi = ( BITMAPINFO *)m_ptr ;
	int cln = pbmi->bmiHeader.biClrUsed ;

	if ( cln == 0 ) {
		return ( NULL ) ;
	}

	return ( pbmi->bmiColors ) ;
}


void CBitmapStd::ChangeTable( int tabletype, int thre )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	n, cln ;

	BITMAPINFO	*pbmi = NULL ;
	RGBQUAD	*bmiTable = NULL ;
	pbmi = ( BITMAPINFO *)m_ptr ;
	cln = pbmi->bmiHeader.biClrUsed ;

	if ( cln != 256 ) {
		return ;
	}

	bmiTable = pbmi->bmiColors ;

	switch ( tabletype ) {
	case	0 :
	// -----Gray-----
		for ( n = 0 ; ( n < 128 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed= ( BYTE)n ;
			bmiTable[ n ].rgbGreen= ( BYTE)n ;
			bmiTable[ n ].rgbBlue = ( BYTE)( n * 2 ) ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		for ( ; ( n < 256 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed = ( BYTE)n ;
			bmiTable[ n ].rgbGreen= ( BYTE)n ;
			bmiTable[ n ].rgbBlue = 255 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		break ;
	case	1 :
	// -----Fake Color-----
		for ( n = 0 ; ( n < 64 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed= ( BYTE)( n * 4 ) ;
			bmiTable[ n ].rgbGreen= 0 ;
			bmiTable[ n ].rgbBlue = 0 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		for ( ; ( n < 128 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed = ( BYTE)( 255 -( n - 64 ) * 4 ) ;
			bmiTable[ n ].rgbGreen= ( BYTE)( ( n - 64 ) * 4 ) ;
			bmiTable[ n ].rgbBlue = 0 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		for ( ; ( n < 192 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed = 0 ;
			bmiTable[ n ].rgbGreen= ( BYTE)( 255 - ( n - 128 ) * 4 ) ;
			bmiTable[ n ].rgbBlue = ( BYTE)( ( n - 128 ) * 4 ) ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		for ( ; ( n < 256 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed = ( BYTE)( ( n - 192 ) * 4 ) ;
			bmiTable[ n ].rgbGreen= ( BYTE)( ( n - 192 ) * 4 ) ;
			bmiTable[ n ].rgbBlue = 255 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		break ;
	case	2 :
	// -----2치-----
		for ( n = 0 ; ( n <= thre ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbBlue = 0 ;
			bmiTable[ n ].rgbRed= 0 ;
			bmiTable[ n ].rgbGreen= 0 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		for ( /* */ ; ( n < 256 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbBlue = 0xff ;
			bmiTable[ n ].rgbRed= 0xff ;
			bmiTable[ n ].rgbGreen= 0xff ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		break ;
	case	3 :
	// -----백/흑만의 Gray-----
		for ( n = 0 ; ( n < 128 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed= ( BYTE)n ;
			bmiTable[ n ].rgbGreen= ( BYTE)n ;
			bmiTable[ n ].rgbBlue = ( BYTE)n ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		for ( ; ( n < 256 ) && ( n < cln ) ; n++ ) {
			bmiTable[ n ].rgbRed = ( BYTE)n ;
			bmiTable[ n ].rgbGreen= ( BYTE)n ;
			bmiTable[ n ].rgbBlue = ( BYTE)n ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		break ;
	case	4 :
	// -----16계조-----
		int	n_p_16 ;
		for ( n = 0 ; ( n < 128 ) && ( n < cln ) ; n++ ) {
			n_p_16 = ( n / 16 ) * 16 ;
			bmiTable[ n ].rgbRed= ( BYTE)n_p_16 ;
			bmiTable[ n ].rgbGreen= ( BYTE)n_p_16 ;
			bmiTable[ n ].rgbBlue = ( BYTE)n_p_16 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		for ( ; ( n < 256 ) && ( n < cln ) ; n++ ) {
			n_p_16 = ( n / 16 ) * 16 ;
			bmiTable[ n ].rgbRed = ( BYTE)n_p_16 ;
			bmiTable[ n ].rgbGreen= ( BYTE)n_p_16 ;
			bmiTable[ n ].rgbBlue = ( BYTE)n_p_16 ;
			bmiTable[ n ].rgbReserved = 0 ;
		}
		break ;
	case	5 :
	case	6 :
	case	7 :
	// -----Only R or Only G or Only B-----
		for ( n = 0 ; n < 256 ; n++ ) {
			bmiTable[ n ].rgbRed= ( BYTE)0 ;
			bmiTable[ n ].rgbGreen= ( BYTE)0 ;
			bmiTable[ n ].rgbBlue = ( BYTE)0 ;
			bmiTable[ n ].rgbReserved = 0 ;
			if ( tabletype == 5 ) {		//R
				bmiTable[ n ].rgbRed= ( BYTE)n ;
			} else if ( tabletype == 6 ) {	//G
				bmiTable[ n ].rgbGreen= ( BYTE)n ;
			} else if ( tabletype == 7 ) {	//B
				bmiTable[ n ].rgbBlue= ( BYTE)n ;
			}
		}
		break ;
	case	8 :
	// -----홀수휘도만의 2치-----
		{
			BYTE	byteTemp = 0x00 ;
			for ( n = 0 ; ( n <= 256 ) && ( n < cln ) ; n++ ) {
				if ( ( n % 2 ) == 1 ) {
					byteTemp = 0xff ;
				} else {
					byteTemp = 0 ;
				}
				bmiTable[ n ].rgbBlue = byteTemp ;
				bmiTable[ n ].rgbRed= byteTemp ;
				bmiTable[ n ].rgbGreen= byteTemp ;
				bmiTable[ n ].rgbReserved = 0 ;
			}
		}
		break ;
	case	9 :
	case	10 :
	case	11 :
	case	12 :
	case	13 :
	case	14 :
	case	15 :
	case	16 :
	// -----지정비트만의 2치-----
		{
			BYTE	byteTemp = 0x00 ;
			BYTE	checkBit = 0x01 << ( tabletype - 9 ) ;
			for ( n = 0 ; ( n <= 256 ) && ( n < cln ) ; n++ ) {
				if ( ( n & checkBit ) != 0x00 ) {
					byteTemp = 0xff ;
				} else {
					byteTemp = 0 ;
				}
				bmiTable[ n ].rgbBlue = byteTemp ;
				bmiTable[ n ].rgbRed= byteTemp ;
				bmiTable[ n ].rgbGreen= byteTemp ;
				bmiTable[ n ].rgbReserved = 0 ;
			}
		}
		break ;
	case	17 :
	case	18 :
	case	19 :
	case	20 :
	case	21 :
	case	22 :
	case	23 :
	case	24 :
	// -----지정비트 이상 유효(미만 무시)의 농담-----
		{
			BYTE	byteTemp = 0x00 ;
			BYTE	activeBit = ~( ( BYTE)( ( ( WORD)0x01 << ( tabletype - 17 ) ) - 1 ) ) ;
			for ( n = 0 ; ( n < 128 ) && ( n < cln ) ; n++ ) {
				byteTemp = ( BYTE)n & activeBit ;
				bmiTable[ n ].rgbRed= byteTemp ;
				bmiTable[ n ].rgbGreen= byteTemp ;
				bmiTable[ n ].rgbBlue = ( BYTE)( byteTemp * 2 ) ;
				bmiTable[ n ].rgbReserved = 0 ;
			}
			for ( ; ( n < 256 ) && ( n < cln ) ; n++ ) {
				byteTemp = ( BYTE)n & activeBit ;
				bmiTable[ n ].rgbRed = ( BYTE)byteTemp ;
				bmiTable[ n ].rgbGreen= ( BYTE)byteTemp ;
				bmiTable[ n ].rgbBlue = ( ( byteTemp * 2 ) >= 255 ) ? 255 : ( byteTemp * 2 ) ;
				bmiTable[ n ].rgbReserved = 0 ;
			}
		}
		break ;
	case	25 :
	case	26 :
	case	27 :
	case	28 :
	case	29 :
	case	30 :
	case	31 :
	case	32 :
	// -----지정비트 이하 유효(초과 무시)의 농담-----
		{
			BYTE	byteTemp = 0x00 ;
			BYTE	activeBit = ( ( BYTE)( ( ( WORD)0x01 << ( 7 - ( tabletype - 25 ) + 1 ) ) - 1 ) ) ;
			int		nShift = ( tabletype - 25 ) ;
			DWORD	dwValue ;
			for ( n = 0 ; ( n < 256 ) && ( n < cln ) ; n++ ) {
				if ( n > activeBit ) {
					bmiTable[ n ].rgbRed= 255 ;
					bmiTable[ n ].rgbGreen= 255 ;
					bmiTable[ n ].rgbBlue = 255 ;
					bmiTable[ n ].rgbReserved = 0 ;
					continue ;
				}
				byteTemp = ( BYTE)n & activeBit ;
				dwValue = ( DWORD)byteTemp << nShift ;
				byteTemp = ( dwValue > 255 ) ? 255 : ( BYTE)dwValue ;
				bmiTable[ n ].rgbRed= byteTemp ;
				bmiTable[ n ].rgbGreen= byteTemp ;
				bmiTable[ n ].rgbBlue = ( ( byteTemp * 2 ) >= 255 ) ? 255 : ( byteTemp * 2 ) ;
				bmiTable[ n ].rgbReserved = 0 ;
			}
		}
		break ;
	}
	m_colortype = tabletype ;
	m_threshold = thre ;
	return ;
}


void CBitmapStd::DrawAreaFunction( CDC *cdc, RECT *rc, RECT *img_rc, DWORD dwRop, int mReverse)
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	char	*ptr = NULL ;
	BITMAPINFO	*pbmi = NULL ;
	HBITMAP	hbm, old ;
	CDC	hdcMem	 ;
	BITMAP	bm ;

	if ( m_ptr == NULL ) {
		MakeImageArea() ;
	}

	ptr = ( char *)m_ptr ;
	pbmi = ( BITMAPINFO *)ptr ;
	ptr += sizeof( BITMAPINFOHEADER ) + pbmi->bmiHeader.biClrUsed * sizeof( RGBQUAD ) ;
	hbm = ::CreateDIBitmap( cdc->m_hDC, ( BITMAPINFOHEADER *)pbmi, CBM_INIT, ptr, pbmi, DIB_RGB_COLORS ) ;

	if ( hbm == NULL ) {
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}
	if ( ::GetObject( hbm, sizeof( BITMAP ), ( LPSTR )&bm ) == 0 ) {
		DeleteObject( hbm ) ;
		LPVOID lpMsgBuf ;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR) &lpMsgBuf, 0, NULL ) ;
		LocalFree( lpMsgBuf ) ;
		return ;
	}

	hdcMem.CreateCompatibleDC( cdc ) ;
	old = ( HBITMAP )hdcMem.SelectObject( hbm ) ;

	if ( mReverse == 0 ) {
		cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top,
						&hdcMem,
						img_rc->left, img_rc->top, img_rc->right - img_rc->left, img_rc->bottom - img_rc->top , dwRop ) ;
	} else {
		if ( mReverse == 1 ) {
			cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, 0, bm.bmHeight - 1, bm.bmWidth, -bm.bmHeight, dwRop ) ;
		} else if ( mReverse == 2 ) {
			cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, bm.bmWidth - 1, 0, -bm.bmWidth, bm.bmHeight, dwRop ) ;
		} else {
			cdc->StretchBlt( rc->left, rc->top, rc->right- rc->left, rc->bottom-rc->top, &hdcMem, bm.bmWidth - 1, bm.bmHeight - 1, -bm.bmWidth, -bm.bmHeight, dwRop ) ;
		}
	}

	m_ReverseFlag = mReverse ;

	hdcMem.SelectObject( old ) ;
	hdcMem.DeleteDC() ;

	DeleteObject( hbm ) ;

}

void CBitmapStd::ChangeTable( int RGB )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	n, cln ;

	BITMAPINFO	*pbmi = NULL ;
	RGBQUAD	*bmiTable = NULL ;
	pbmi = ( BITMAPINFO *)m_ptr ;
	cln = pbmi->bmiHeader.biClrUsed ;
	bmiTable = pbmi->bmiColors ;

	for ( n = 0 ; n < 256 ; n++ ) {
		bmiTable[ n ].rgbRed= ( BYTE)0 ;
		bmiTable[ n ].rgbGreen= ( BYTE)0 ;
		bmiTable[ n ].rgbBlue = ( BYTE)0 ;
		bmiTable[ n ].rgbReserved = 0 ;
		if ( RGB == 0 ) {			// R
			bmiTable[ n ].rgbRed= ( BYTE)n ;
		} else if ( RGB == 1 ) {	// G
			bmiTable[ n ].rgbGreen= ( BYTE)n ;
		} else if ( RGB == 2 ) {	// B
			bmiTable[ n ].rgbBlue= ( BYTE)n ;
		}
	}

	return ;
}


void CBitmapStd::ChangeTable( int r, int g, int b, int thre )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	n, cln ;

	BITMAPINFO	*pbmi = NULL ;
	RGBQUAD	*bmiTable = NULL ;
	pbmi = ( BITMAPINFO *)m_ptr ;
	cln = pbmi->bmiHeader.biClrUsed ;

	if ( cln != 256 ) {
		return ;
	}

	bmiTable = pbmi->bmiColors ;
	for ( n = 0 ; ( n < thre ) && ( n < cln ) ; n++ ) {
		bmiTable[ n ].rgbRed= 0 ;
		bmiTable[ n ].rgbGreen= 0 ;
		bmiTable[ n ].rgbBlue = 0 ;
		bmiTable[ n ].rgbReserved = 0 ;
	}
	for ( ; ( n < 256 ) && ( n < cln ) ; n++ ) {
		bmiTable[ n ].rgbRed = r ;
		bmiTable[ n ].rgbGreen= g ;
		bmiTable[ n ].rgbBlue = b ;
		bmiTable[ n ].rgbReserved = 0 ;
	}

	return ;
}


void CBitmapStd::ChangeTable( BYTE *pR, BYTE *pG, BYTE *pB )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	n, cln ;
	BITMAPINFO	*pbmi = NULL ;
	RGBQUAD	*bmiTable = NULL ;
	pbmi = ( BITMAPINFO *)m_ptr ;
	cln = pbmi->bmiHeader.biClrUsed ;

	if ( cln != 256 ) {
		return ;
	}

	bmiTable = pbmi->bmiColors ;
	for ( n = 0 ; ( n < 256 ) && ( n < cln ) ; n++ ) {
		bmiTable[ n ].rgbRed= pR[ n ] ;
		bmiTable[ n ].rgbGreen= pG[ n ] ;
		bmiTable[ n ].rgbBlue = pB[ n ] ;
		bmiTable[ n ].rgbReserved = 0 ;
	}

	return ;
}


LPBYTE CBitmapStd::GetLinePtr( int line )
{
	return ( GetScanLinePtr( line ) ) ;
}


LPBYTE CBitmapStd::GetScanLinePtr( int verpos )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	DWORD	xsize = 0 ;
	int	v[ 2 ] ;
	memset( v, 0x00, sizeof( v ) ) ;
	GetImgSize( v ) ;

	int	nBitCount = GetBitCount() ;
	switch ( nBitCount ) {
	case	24 :
	case	8 :
	case	1 :
		xsize = ( nBitCount * ( ( ( v[ 0 ] + 4 - 1 ) / 4 ) * 4 ) ) / 8 ;
		break ;
	default :
		// Not Support !
		ASSERT( FALSE ) ;
		xsize = ( nBitCount * ( ( ( v[ 0 ] + 4 - 1 ) / 4 ) * 4 ) ) / 8 ;
		break ;
	}

	LPBYTE	ptr = GetImgPtr() ;
	ptr += sizeof( BYTE ) * ( v[ 1 ] - 1 - verpos ) * xsize ;
	return ( ptr ) ;

}


CPoint CBitmapStd::MoveTo( int x, int y )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	CPoint	poOld = m_poNow ;
	m_poNow.x = x ;
	m_poNow.y = y ;

	return ( poOld ) ;

}


CPoint CBitmapStd::MoveTo( POINT point )
{
	return ( MoveTo( point.x, point.y ) ) ;
}


BOOL CBitmapStd::LineTo( int x, int y, int iColor )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BOOL	bRet = FALSE ;
	CPoint	poOld = m_poNow ;
	m_poNow.x = x ;
	m_poNow.y = y ;
	if ( poOld != m_poNow ) {
		double	dVectorX = ( double)( m_poNow.x - poOld.x ) ;
		double	dVectorY = ( double)( m_poNow.y - poOld.y ) ;
		double	dLength = _hypot( dVectorX, dVectorY ) ;
		if ( dLength >= ( double)1.0 ) {
			bRet = TRUE ;
			dVectorX /= dLength ;
			dVectorY /= dLength ;
			double	dPitch = ( double)1.0 ;
			CPoint	poReal ;
			BOOL	bOut = FALSE ;
			int	nSize[ 2 ] ;
			memset( nSize, 0x00, sizeof( nSize ) ) ;
			GetImgSize( nSize ) ;
			{
				if ( poOld.x < 0 ) {
					bOut = TRUE ;
				} else if ( poOld.x >= nSize[ 0 ] ) {
					bOut = TRUE ;
				} else if ( poOld.y < 0 ) {
					bOut = TRUE ;
				} else if ( poOld.y >= nSize[ 1 ] ) {
					bOut = TRUE ;
				} else if ( m_poNow.x < 0 ) {
					bOut = TRUE ;
				} else if ( m_poNow.x >= nSize[ 0 ] ) {
					bOut = TRUE ;
				} else if ( m_poNow.y < 0 ) {
					bOut = TRUE ;
				} else if ( m_poNow.y >= nSize[ 1 ] ) {
					bOut = TRUE ;
				}
			}
			if ( bOut != FALSE ) {
				for ( double i = ( double)0.0 ; i <= dLength ; i += dPitch ) {
					poReal.x = poOld.x + ( long)( dVectorX * i ) ;
					poReal.y = poOld.y + ( long)( dVectorY * i ) ;
					if ( poReal.x < 0 ) {
						continue ;
					} else if ( poReal.x >= nSize[ 0 ] ) {
						continue ;
					}
					if ( poReal.y < 0 ) {
						continue ;
					} else if ( poReal.y >= nSize[ 1 ] ) {
						continue ;
					}
					SetPixel( poReal, iColor ) ;
				}
			} else {
				for ( double i = ( double)0.0 ; i <= dLength ; i += dPitch ) {
					poReal.x = poOld.x + ( long)( dVectorX * i ) ;
					poReal.y = poOld.y + ( long)( dVectorY * i ) ;
					SetPixel( poReal, iColor ) ;
				}
			}
		}
	}
	return ( bRet ) ;

}


BOOL CBitmapStd::LineTo( POINT point, int iColor )
{
	return ( LineTo( point.x, point.y, iColor ) ) ;
}


BOOL CBitmapStd::LineTo( int x, int y, int iColor, int iBaseColor )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BOOL	bRet = FALSE ;
	CPoint	poOld = m_poNow ;
	m_poNow.x = x ;
	m_poNow.y = y ;
	if ( poOld != m_poNow ) {
		double	dVectorX = ( double)( m_poNow.x - poOld.x ) ;
		double	dVectorY = ( double)( m_poNow.y - poOld.y ) ;
		double	dLength = _hypot( dVectorX, dVectorY ) ;
		if ( dLength >= ( double)1.0 ) {
			bRet = TRUE ;
			dVectorX /= dLength ;
			dVectorY /= dLength ;
			double	dPitch = ( double)1.0 ;
			CPoint	poReal ;
			BOOL	bOut = FALSE ;
			int	nSize[ 2 ] ;
			memset( nSize, 0x00, sizeof( nSize ) ) ;
			GetImgSize( nSize ) ;
			{
				if ( poOld.x < 0 ) {
					bOut = TRUE ;
				} else if ( poOld.x >= nSize[ 0 ] ) {
					bOut = TRUE ;
				} else if ( poOld.y < 0 ) {
					bOut = TRUE ;
				} else if ( poOld.y >= nSize[ 1 ] ) {
					bOut = TRUE ;
				} else if ( m_poNow.x < 0 ) {
					bOut = TRUE ;
				} else if ( m_poNow.x >= nSize[ 0 ] ) {
					bOut = TRUE ;
				} else if ( m_poNow.y < 0 ) {
					bOut = TRUE ;
				} else if ( m_poNow.y >= nSize[ 1 ] ) {
					bOut = TRUE ;
				}
			}
			if ( bOut != FALSE ) {
				for ( double i = ( double)0.0 ; i <= dLength ; i += dPitch ) {
					poReal.x = poOld.x + ( long)( dVectorX * i ) ;
					poReal.y = poOld.y + ( long)( dVectorY * i ) ;
					if ( poReal.x < 0 ) {
						continue ;
					} else if ( poReal.x >= nSize[ 0 ] ) {
						continue ;
					}
					if ( poReal.y < 0 ) {
						continue ;
					} else if ( poReal.y >= nSize[ 1 ] ) {
						continue ;
					}
					if ( GetColor( poReal ) == iBaseColor ) {
						SetPixel( poReal, iColor ) ;
					}
				}
			} else {
				for ( double i = ( double)0.0 ; i <= dLength ; i += dPitch ) {
					poReal.x = poOld.x + ( long)( dVectorX * i ) ;
					poReal.y = poOld.y + ( long)( dVectorY * i ) ;
					if ( GetColor( poReal ) == iBaseColor ) {
						SetPixel( poReal, iColor ) ;
					}
				}
			}
		}
	}
	return ( bRet ) ;

}


BOOL CBitmapStd::LineTo( POINT point, int iColor, int iBaseColor )
{

	return ( LineTo( point.x, point.y, iColor, iBaseColor ) ) ;

}


BOOL CBitmapStd::Rectangle( int x1, int y1, int x2, int y2, int iColor )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BOOL	bRet = TRUE ;
	CPoint	p[ 2 ] ;	// ([럑?/뢎?])
	{
		int	nSize[ 2 ] ;
		memset( nSize, 0x00, sizeof( nSize ) ) ;
		GetImgSize( nSize ) ;
		if ( x1 < 0 ) {
			x1 = 0 ;
		} else if ( x1 >= nSize[ 0 ] ) {
			x1 = nSize[ 0 ] - 1 ;
		}
		if ( x2 < 0 ) {
			x2 = 0 ;
		} else if ( x2 >= nSize[ 0 ] ) {
			x2 = nSize[ 0 ] - 1 ;
		}
		if ( y1 < 0 ) {
			y1 = 0 ;
		} else if ( y1 >= nSize[ 1 ] ) {
			y1 = nSize[ 1 ] - 1 ;
		}
		if ( y2 < 0 ) {
			y2 = 0 ;
		} else if ( y2 >= nSize[ 1 ] ) {
			y2 = nSize[ 1 ] - 1 ;
		}
	}

	p[ 0 ].y = y1 ;
	p[ 1 ].y = y2 ;
	for ( int x = x1 ; x <= x2 ; x++ ) {
		p[ 0 ].x = x ;
		SetPixel( p[ 0 ], iColor ) ;
		p[ 1 ].x = x ;
		SetPixel( p[ 1 ], iColor ) ;
	}

	p[ 0 ].x = x1 ;
	p[ 1 ].x = x2 ;
	for ( int y = y1 ; y <= y2 ; y++ ) {
		p[ 0 ].y = y ;
		SetPixel( p[ 0 ], iColor ) ;
		p[ 1 ].y = y ;
		SetPixel( p[ 1 ], iColor ) ;
	}

	return ( bRet ) ;

}


BOOL CBitmapStd::Rectangle( LPCRECT lpRect, int iColor )
{
	return ( Rectangle( lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, iColor ) ) ;
}


void CBitmapStd::GetBitmap( CBitmapStd &rBmp )
{
	rBmp = *this ;
}


void CBitmapStd::SetBitmap( CBitmapStd &rBmp )
{
	*this = rBmp ;
}


COLORREF CBitmapStd::GetColorTable( int v )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	COLORREF r ;
	if ( GetBitCount() == 24 ) {
		COLORREF	rgb = RGB( 0, 0, 0 ) ;
		return ( rgb ) ;
	}

	RGBQUAD *ptr = NULL, rd ;
	ptr = ( RGBQUAD *)GetColorTablePtr() ;
	rd = *( ptr + v ) ;
	r = RGB( rd.rgbRed, rd.rgbGreen, rd.rgbBlue ) ;
	UnlockBitmapinfoPtr() ;

	return ( r ) ;
}


int CBitmapStd::GetLineByteSize( void )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	nSize[ 2 ] ;
	memset( nSize, 0x00, sizeof( nSize ) ) ;
	GetImgSize( nSize ) ;
	int	nBitCount = GetBitCount() ;
	int	nLineSize = ( nBitCount * ( ( ( nSize[ 0 ] + 4 - 1 ) / 4 ) * 4 ) ) / 8 ;
	return ( nLineSize ) ;
}


int CBitmapStd::ImageCopy( int iReverse )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	int	nRet = 0 ;
	try {
		CString	strLocal ;
		CBitmapStd	*pBmp = this ;
		ASSERT( pBmp != NULL ) ;
		int	iSize[ 2 ] ;
		memset( iSize, 0x00, sizeof( iSize ) ) ;
		pBmp->GetImgSize( iSize ) ;
		if ( ( iSize[ 0 ] <= 0 ) || ( iSize[ 1 ] <= 0 ) ) {
			return ( -1 ) ;
		}
		int	iBitCount = pBmp->GetBitCount() ;
		int	iBitLoop = iBitCount / 8 ;
		if ( ( iBitCount % 8 ) != 0 ) {
			return ( -1 ) ;
		}
		if ( iBitLoop < 1 ) {
			return ( -1 ) ;
		}

		BYTE	*pByteAll = pBmp->GetImgPtr() ;
		BYTE	*pByte = pByteAll ;
		BYTE	*pTargetByteAll = pByteAll ;
		BYTE	*pTargetByte = pTargetByteAll ;
		BYTE	byteTemp ;
		int	iX, iY, iColor ;
		int	iMaxX = iSize[ 0 ] / 2 ;
		int	iMaxY = iSize[ 1 ] / 2 ;
		switch ( iReverse ) {
		case	0 :
			break ;
		case	1 :
			for ( iY = 0 ; iY < iMaxY ; iY++ ) {
				pTargetByte = pTargetByteAll + ( iSize[ 0 ] * iBitLoop * ( iSize[ 1 ] - iY - 1 ) ) ;
				for ( iX = 0 ; iX < iSize[ 0 ] ; iX++ ) {
					for ( iColor = 0 ; iColor < iBitLoop ; iColor++ ) {
						byteTemp = *pByte ;
						*pByte = *pTargetByte ;
						*pTargetByte = byteTemp ;
						pByte++ ;
						pTargetByte++ ;
					}
				}
			}
			break ;
		case	2 :
			for ( iY = 0 ; iY < iSize[ 1 ] ; iY++ ) {
				for ( iX = 0 ; iX < iMaxX ; iX++ ) {
					pTargetByte = pTargetByteAll + ( ( iSize[ 0 ] * iY ) + ( iSize[ 0 ] - iX - 1 ) ) * iBitLoop ;
					for ( iColor = 0 ; iColor < iBitLoop ; iColor++ ) {
						byteTemp = *pByte ;
						*pByte = *pTargetByte ;
						*pTargetByte = byteTemp ;
						pByte++ ;
						pTargetByte++ ;
					}
				}
			}
			break ;
		case	3 :
			for ( iY = 0 ; iY < iMaxY ; iY++ ) {
				for ( iX = 0 ; iX < iSize[ 0 ] ; iX++ ) {
					pTargetByte = pTargetByteAll + ( ( iSize[ 0 ] * ( iSize[ 1 ] - iY - 1 ) ) + ( iSize[ 0 ] - iX - 1 ) ) * iBitLoop ;
					for ( iColor = 0 ; iColor < iBitLoop ; iColor++ ) {
						byteTemp = *pByte ;
						*pByte = *pTargetByte ;
						*pTargetByte = byteTemp ;
						pByte++ ;
						pTargetByte++ ;
					}
				}
			}
			break ;
		default :
			nRet = -1 ;
			break ;
		}
		pBmp->UnlockBitmapinfoPtr() ;
	} catch ( ... ) {
		nRet = -1 ;
	}
	return ( nRet ) ;
}


int CBitmapStd::GetColorUsed( void )
{
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CSingleLock	cs( GetBmpCriticalSection(), TRUE ) ;
#endif
	BITMAPINFO	*pbmi = NULL ;
	int	cln ;
	pbmi = ( BITMAPINFO *)m_ptr ;
	cln = pbmi->bmiHeader.biClrUsed ;

	return ( cln ) ;

}


#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
CCriticalSection *CBitmapStd::GetBmpCriticalSection( void )
{
	return ( &m_csBmpLock ) ;
}
#endif


int CBitmapStd::GetJpegQuality( void )
{
	return ( m_nJpegQuality ) ;
}


int CBitmapStd::SetJpegQuality( int nJpegQuality )
{
	int	nRet = 0 ;
	if ( nJpegQuality < 0 ) {
		nJpegQuality = 0 ;
		nRet = -1 ;
	} else if ( nJpegQuality > 100 ) {
		nJpegQuality = 100 ;
		nRet = -1 ;
	}
	m_nJpegQuality = nJpegQuality ;
	return ( nRet ) ;
}


void CBitmapStd::SetImage( int xsize, int ysize, const BYTE* pImage, BOOL bRB_Revers ) 
{
	BYTE* ps = (BYTE*)pImage ;
	BYTE* pd ;
	BITMAPINFO	*pbmi = NULL ;

	pbmi = ( BITMAPINFO *)m_ptr ;
	int bitCnt = pbmi->bmiHeader.biBitCount ;

	for ( int y = 0 ; y < ysize ;y++ ) {
		pd = GetLinePtr( y ) ; 
		memcpy( pd, ps, sizeof( BYTE ) * xsize ) ; 
		if( bRB_Revers == TRUE ){
			for( int x = 1 ; x < ( xsize - 1 ) ; x+=3 ){
				BYTE temp ;
				temp = *(pd + x -1) ;
				*(pd + x -1) = *(pd + x +1) ;
				*(pd + x +1) = temp ;
			}
		}
		ps += xsize; 
	}
}

int CBitmapStd::ImageReverse() {

	BITMAPINFO* pbmi = NULL;
	pbmi = (BITMAPINFO*)m_ptr;
	int bitCnt = pbmi->bmiHeader.biBitCount;
	int nWidth = pbmi->bmiHeader.biWidth;
	int nHeight = pbmi->bmiHeader.biHeight;
	
	BYTE* pImgPtr = GetImgPtr();
	BYTE* pTempPtr = new BYTE[ nWidth * nHeight +1];
	memset(pTempPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memcpy(pTempPtr, pImgPtr , sizeof(BYTE) * nWidth * nHeight);

	BYTE* pSrc = NULL ;
	BYTE* pTar = NULL ;



	for (int y = 0; y < nHeight;y++) {
		pSrc = (pTempPtr + (y * nWidth)) ;
		pTar = (pImgPtr + (((nHeight-1)-y) * nWidth));

		memcpy(pTar, pSrc, sizeof(BYTE) * nWidth);
	}

	delete[] pTempPtr;

	return 0;
}
// 23.02.08 Ahn Add Start
int CBitmapStd::ImageReverse(BYTE* pImgPtr, int nWidth, int nHeight)
{
	ASSERT(pImgPtr);

	if ((nWidth <= 0) || (nHeight <= 0)) return -1;

	BYTE* pTempPtr = new BYTE[nWidth * nHeight+1];
	memset(pTempPtr, 0x00, sizeof(BYTE) * nWidth * nHeight+1);
	memcpy(pTempPtr, pImgPtr, sizeof(BYTE) * nWidth * nHeight);

	BYTE* pSrc = NULL;
	BYTE* pTar = NULL;

	for (int y = 0; y < nHeight;y++) {
		pSrc = (pTempPtr + (y * nWidth));
		pTar = (pImgPtr + (((nHeight - 1) - y) * nWidth));

		memcpy(pTar, pSrc, sizeof(BYTE) * nWidth);
	}

	delete[] pTempPtr;

	return 0;
}
// 23.02.08 Ahn Add End


// 23.02.03 Ahn Add Start
int CBitmapStd::CopyImgBit8toBit24(CBitmapStd* pOrgImg)
{
	if (pOrgImg == NULL) return -1;

	CSize size = pOrgImg->GetImgSize();
	int nWidth = size.cx;
	int nHeight = size.cy;
	MakeImageArea(size.cx, size.cy, 24);

	BYTE* pImgPtr = pOrgImg->GetImgPtr();
	BYTE* pTarPtr = GetImgPtr();
	
	for (int y = 0; y < nHeight ; y++) {
		BYTE* pLinePtr = (pImgPtr + (nWidth * y) ) ;
		BYTE* pTarLinePtr = ( pTarPtr + (nWidth * 3 * y) );
		for (int x = 0; x < nWidth; x++) {
			*(pTarLinePtr + (x * 3)) = *( pLinePtr + x );
			*(pTarLinePtr + (x * 3) + 1 ) = *(pLinePtr + x);
			*(pTarLinePtr + (x * 3) + 2 ) = *(pLinePtr + x);
		}
	}

	return 0;
}

int CBitmapStd::CopyImgBit8toBit24(BYTE* pImgPtr, int nWidth, int nHeight)
{
	ASSERT(pImgPtr);
	
	if ((nWidth <= 0) || (nHeight <= 0)) return -1;

	BYTE* pTarPtr = GetImgPtr();

	for (int y = 0; y < nHeight; y++) {
		BYTE* pLinePtr = (pImgPtr + (nWidth * y));
		BYTE* pTarLinePtr = (pTarPtr + (nWidth * 3 * y));
		for (int x = 0; x < nWidth; x++) {
			*(pTarLinePtr + (x * 3)) = *(pLinePtr + x);
			*(pTarLinePtr + (x * 3) + 1) = *(pLinePtr + x);
			*(pTarLinePtr + (x * 3) + 2) = *(pLinePtr + x);
		}
	}

	return 0;
}

// 23.02.03 Ahn Add End
