#if !defined(AFX_BITMAPSTD_H__E2E432F3_F072_11D1_80AE_00000E7CA01F__INCLUDED_)
#define AFX_BITMAPSTD_H__E2E432F3_F072_11D1_80AE_00000E7CA01F__INCLUDED_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
#include <afxmt.h>
#endif


class CBitmapStd
{
public :
	COLORREF GetColorTable( int v ) ;

	int GetClrUsedCnt( void ) ;
	LPRGBQUAD GetRgbQuadPtr( void ) ;

	void GetBitmap( CBitmapStd &rBmp ) ;
	void SetBitmap( CBitmapStd &rBmp ) ;
	LPBYTE GetLinePtr( int line ) ;
	LPBYTE GetScanLinePtr( int verpos ) ;
	void ChangeTable( BYTE *pR, BYTE *pG, BYTE *pB ) ;
	void ChangeTable( int r, int g, int b, int thre ) ;
	void ChangeTable( int RGB ) ;
	void ChangeTable( int tabletype, int thre ) ;

	void UnlockBitmapinfoPtr() ;
	BITMAPINFO * GetBitmapinfoPtr() ;
	BYTE *GetImgPtr() ;
	BYTE *GetImgEndPtr() ;
	BYTE *GetColorTablePtr() ;
	void GetImgSize( int *v ) ;
	CSize GetImgSize( void ) ;

	virtual int ReadBitmap( CWnd *pwnd = NULL, CString *pszSelectName = NULL ) ;
	virtual int ReadBitmap( CString &filename ) ;
	virtual int SaveBitmap( CWnd *pwnd, LPCTSTR pszInitDir ) ;
	virtual int SaveBitmap( CWnd *pwnd = NULL, CString *pszSelectName = NULL ) ;
	virtual int SaveBitmap( CString &filepath ) ;

	//GdiPlus JPG저장 함수
	int SaveJPG(CString& filepath);

	//Gdiplus 인코딩 Guiid 가져오기
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	static void grayscale(HDC hdc, HBITMAP& hbitmap);

	virtual int Clear() ;

	CBitmapStd( int nSizeX = 256, int nSizeY = 256, int nBitCount = 8 ) ;
	virtual ~CBitmapStd() ;
	CBitmapStd& operator = ( const CBitmapStd& ) ;
	CBitmapStd( const CBitmapStd &src ) ;

	CBitmapStd& operator = ( const CBitmap& ) ;

	virtual void DrawImageFunction( CDC *cdc, RECT *rcDraw, RECT *rcImg, DWORD dwRop = SRCCOPY, int mReverse = FALSE ) ;	
	virtual void DrawImageFunction( CDC *cdc, RECT *rc, DWORD dwRop, int mReverse = FALSE, int nBltMode = -1 ) ;	
	virtual void DrawImage( CDC *cdc, RECT *rc, DWORD dwRop, int mReverse = FALSE ) ;
	void DrawAreaFunction( CDC *cdc, RECT *rc, RECT *img_rc, DWORD dwRop, int mReverse ) ;

	virtual int MakeImageArea( int xsize = 256, int ysize = 256, int bitcount = 8, int colorused = 256, SIZE_T *allocSize = NULL ) ;
	virtual void SetPixel( POINT p, int color ) ;
	virtual int SetPixel( int x, int y, int color ) ;
	virtual int GetColor( POINT p ) ;

	virtual int GetColor( int x, int y ) ;
	virtual CPoint MoveTo( int x, int y ) ;
	virtual CPoint MoveTo( POINT point ) ;
	virtual BOOL LineTo( int x, int y, int iColor ) ;
	virtual BOOL LineTo( POINT point, int iColor ) ;
	virtual BOOL LineTo( int x, int y, int iColor, int iBaseColor ) ;
	virtual BOOL LineTo( POINT point, int iColor, int iBaseColor ) ;
	virtual BOOL Rectangle( int x1, int y1, int x2, int y2, int iColor ) ;
	virtual BOOL Rectangle( LPCRECT lpRect, int iColor ) ;
	virtual int GetLineByteSize( void ) ;

	virtual int ImageCopy( int iReverse ) ;
	virtual int GetColorUsed( void ) ;
	virtual int GetColorType( void ) { return ( m_colortype ) ; }
	virtual int GetThreshold( void ) { return ( m_threshold ) ; }

	int ImageReverse();
	static int ImageReverse(BYTE* pImgPtr, int nWidth, int nHeight);

	int	getc( void ) ;
	void putc( int v ) ;
	int	GetBitCount() ;
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CCriticalSection *GetBmpCriticalSection( void ) ;
#endif
	int GetJpegQuality( void ) ;
	int SetJpegQuality( int nJpegQuality ) ;
	int	m_ReverseFlag ;			//반전플래그(0:통상/1:상하반전/2:좌우반전/3:상하좌우반전)
	void SetImage( int xsize, int ysize, const BYTE* pImage, BOOL bRB_Revers = FALSE ) ;

	// 23.02.03 Ahn Add Start
	int CopyImgBit8toBit24(CBitmapStd* pOrgImg);
	int CopyImgBit8toBit24(BYTE* pImgPtr, int nWidth, int nHeight);
	// 23.02.03 Ahn Add End
protected :
	BYTE*	m_ptr ;
	BYTE*	m_ptraccess ;
	int		m_colortype ;			//테이블타입(0:농담/ 1:의사컬러/ 2:2치/ 3:그레이/ 4:16계조 /5:R /6:G /7:B /8:홀수휘도만의2치 /9:Bit0 2치 /10:Bit1 2치 /11:Bit2 2치 /12:Bit3 2치 /13:Bit4 2치 /14:Bit5 2치 /15:Bit6 2치 /16:Bit7 2치)
	int		m_threshold ;			//2치화 레벨

	CPoint	m_poNow ;				//현재 위치 좌표
	int m_nJpegQuality ;			//JPEG의 화질(0~100)
#if	!defined( NOT_USE_BMPSTD_CRITICALSECTION )
	CCriticalSection m_csBmpLock ;
#endif

private :
	void Init( int nSizeX = 256, int nSizeY = 256, int nBitCount = 8 ) ;

} ;


#endif // !defined(AFX_BITMAPSTD_H__E2E432F3_F072_11D1_80AE_00000E7CA01F__INCLUDED_)
