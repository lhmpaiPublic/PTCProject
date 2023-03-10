#pragma once


class CSetValue
{

public:
	CSetValue( CWnd *pParent = NULL ) ;
	virtual ~CSetValue() ;

public:
	int	SetValue( LPCTSTR lpszMsg, double &value, double max, double min, BOOL mouseflag = TRUE ) ;
	int	SetValue( LPCTSTR lpszMsg, UINT &value, UINT max, UINT min, BOOL mouseflag = TRUE ) ;
	int	SetValue( LPCTSTR lpszMsg, int &value, int max, int min, BOOL mouseflag = TRUE ) ;
	int	SetValue( LPCTSTR lpszMsg, short &value, short max, short min, BOOL mouseflag = TRUE ) ;
	int	SetValue( LPCTSTR lpszMsg, WORD &value, WORD max, WORD min, BOOL mouseflag = TRUE ) ;
	int	SetValue( LPCTSTR lpszMsg, unsigned long &value, unsigned long max, unsigned long min, BOOL mouseflag = TRUE ) ;
	int	SetValue( LPCTSTR lpszMsg, long &value, long max, long min, BOOL mouseflag = TRUE ) ;

	int	SetValue( LPCTSTR lpszMsg, CString &value, int max, int min, BOOL mouseflag = TRUE, BOOL bPassMode = FALSE ) ;

	int	SetValue( LPCTSTR lpszMsg, double &valuex, double maxx, double minx, double &valuey, double maxy, double miny, LPCTSTR headerx = _T( "X" ), LPCTSTR headery = _T( "Y" ), BOOL mouseflag = TRUE, BOOL bSetFocusY = FALSE ) ;
	int	SetValue( LPCTSTR lpszMsg, UINT &valuex, UINT maxx, UINT minx, UINT &valuey, UINT maxy, UINT miny, LPCTSTR headerx = _T( "X" ), LPCTSTR headery = _T( "Y" ), BOOL mouseflag = TRUE, BOOL bSetFocusY = FALSE ) ;
	int	SetValue( LPCTSTR lpszMsg, int &valuex, int maxx, int minx, int &valuey, int maxy, int miny, LPCTSTR headerx = _T( "X" ), LPCTSTR headery = _T( "Y" ), BOOL mouseflag = TRUE, BOOL bSetFocusY = FALSE ) ;
	int	SetValue( LPCTSTR lpszMsg, unsigned long &valuex, unsigned long maxx, unsigned long minx, unsigned long &valuey, unsigned long maxy, unsigned long miny, LPCTSTR headerx = _T( "X" ), LPCTSTR headery = _T( "Y" ), BOOL mouseflag = TRUE, BOOL bSetFocusY = FALSE ) ;
	int	SetValue( LPCTSTR lpszMsg, long &valuex, long maxx, long minx, long &valuey, long maxy, long miny, LPCTSTR headerx = _T( "X" ), LPCTSTR headery = _T( "Y" ), BOOL mouseflag = TRUE, BOOL bSetFocusY = FALSE ) ;

	int SwapValue( double *pData1, double *pData2 ) ;
	int SwapValue( long *pData1, long *pData2 ) ;
	int SwapValue( int *pData1, int *pData2 ) ;
	int SwapValue( WORD *pData1, WORD *pData2 ) ;
	int SwapValue( short int *pData1, short int *pData2 ) ;
	int SwapValue( char *pData1, char *pData2 ) ;
	int SwapValue( BYTE *pData1, BYTE *pData2 ) ;
	int SwapValue( CString *pData1, CString *pData2 ) ;
	template < class T > int SwapValue( T &data1, T &data2 ) ;

	int CutValue( WORD *pwValue, WORD wMax, WORD wMin, BOOL bEdge = TRUE ) ;
	int CutValue( int *piValue, int iMax, int iMin, BOOL bEdge = TRUE ) ;
	int CutValue( long *plValue, long lMax, long lMin, BOOL bEdge = TRUE ) ;
	int CutValue( double *pdValue, double dMax, double dMin, BOOL bEdge = TRUE ) ;
	int CutValue( DWORD *pValue, DWORD nMax, DWORD nMin, BOOL bEdge = TRUE ) ;
	template < class T > int CutValue( T &value, T max, T min, BOOL bEdge = TRUE ) ;

	int SwapCheckFunc( WORD *val1, WORD *val2, LPCTSTR lpszMsg ) ;
	int SwapCheckFunc( int *val1, int *val2, LPCTSTR lpszMsg ) ;
	int SwapCheckFunc( long *val1, long *val2, LPCTSTR lpszMsg ) ;
	int SwapCheckFunc( double *val1, double *val2, LPCTSTR lpszMsg ) ;
	template < class T > int SwapCheckFunc( T &val1, T &val2, LPCTSTR lpszMsg ) ;

	int SetEventCheckFunc( int ( *eventfunc)() ) ;

protected:
	CWnd *m_pParent ;
	int	(*m_pfEventCheckfunc)() ;

} ;
