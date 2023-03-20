// KExcel.h: interface for the KExcel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEXCEL_H__14CFB192_97AA_49DA_96E6_47D5D4204684__INCLUDED_)
#define AFX_KEXCEL_H__14CFB192_97AA_49DA_96E6_47D5D4204684__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class KExcel  
{
public:
	KExcel();
	virtual ~KExcel();

public:
	enum { Format_Text, Format_Numeric, Format_Comma, Format_Percent };

public:
	int		Init() ;
	void	Close() ;
	void	Show();

	int		Open( CString strFileName = _T("") ) ;
	int		Save() ;
	int		SaveAs( CString strFileName ) ;
	
	void	SetActiveSheet( int nSheetIndex ) ;
	int		SetSheetName( int nSheetIndex, CString strSheetName ) ;
	CString	GetSheetName() ;
	int		CopySheet( BOOL bAfter = TRUE ) ;
	int		DeleteSheet( int nSheetIndex ) ;

	void	SetFreezePanes( long lRow, long lCol, BOOL bFreeze ) ;

	void	SetRange( long lRow, long lCol ) ;	
	void	SetRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd ) ;
	LPDISPATCH	GetRange( long lRow, long lCol ) ;	
	LPDISPATCH	GetRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd ) ;
	void	SetRangeEntireColumn( long lColStart, long lColEnd ) ;
	void	SetRangeEntireColumn( long lCol ) ;
	void	SetRangeEntireRow( long lRowStart, long lRowEnd ) ;
	void	SetRangeEntireRow( long lCol ) ;
	void	SetRangeNumberFormat( long lRowStart, long lColStart, long lRowEnd, long lColEnd, int nStyle, int nDecimalPoint = 0 ) ;
	void	SetRangeNumberFormat( long lRow, long lCol, int nStyle, int nDecimalPoint = 0 ) ;
	void	SetRangeSelect( long lRow, long lCol ) ;	
	void	SetRangeSelect( long lRowStart, long lColStart, long lRowEnd, long lColEnd ) ;
	void	ClearRange( long lRow, long lCol ) ;
	void	ClearRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd ) ;
	int		SortRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd
					 , long lKeyRowStart, long lKeyColStart, long lKeyRowEnd, long lKeyColEnd
					 , long lSortOrder = xlAscending, long lSortOrientation = xlSortColumns ) ;
	int		CopyRange( long lSrcRowStart, long lSrcColStart, long lSrcRowEnd, long lSrcColEnd, long lDestSheetIndex
					 , long lDestRowStart, long lDestColStart, long lDestRowEnd, long lDestColEnd ) ;
	int		InsertCopyRange( long lInsertRowStart, long lInsertColStart, long lInsertRowEnd, long lInsertColEnd
					   , long lInsertShiftDirection, long lCopySheetIndex
					   , long lCopyRowStart, long lCopyColStart, long lCopyRowEnd, long lCopyColEnd ) ;
	int		InsertCopyRange( long lInsertRow, long lInsertCol, long lInsertShiftDirection, long lCopySheetIndex
					   , long lCopyRow, long lCopyCol ) ;
	int		InsertRange( long lInsertRowStart, long lInsertColStart, long lInsertRowEnd, long lInsertColEnd
					   , long lInsertShiftDirection ) ;
	int		InsertRange( long lInsertRow, long lInsertCol, LONG lInsertShiftDirection ) ;
	int		DeleteRange( long lDeleteRowStart, long lDeleteColStart, long lDeleteRowEnd, long lDeleteColEnd, long lDeleteShiftDirection ) ;
	int		DeleteRange( long lDeleteRow, long lDeleteCol, long lDeleteShiftDirection ) ;

	void	SetValue( CString strValue ) ;
	void	SetValue( long lRow, long lCol, CString strValue ) ;
	void	SetValue( long lValue ) ;
	void	SetValue( long lRow, long lCol, long lValue ) ;
	void	SetValue( double lValue, int nDecimalPoint ) ;
	void	SetValue( long lRow, long lCol, double lValue, int nDecimalPoint ) ;
	void	SetValue_Comma( long lValue ) ;
	void	SetValue_Comma( long lRow, long lCol, long lValue ) ;
	void	SetValue_Comma( double lValue, int nDecimalPoint ) ;
	void	SetValue_Comma( long lRow, long lCol, double lValue, int nDecimalPoint ) ;
	void	SetValue_Percent( long lValue ) ;
	void	SetValue_Percent( long lRow, long lCol, long lValue ) ;
	void	SetValue_Percent( double lValue, int nDecimalPoint ) ;
	void	SetValueEx( COleSafeArray sa ) ;
	void	SetValue_Percent( long lRow, long lCol, double lValue, int nDecimalPoint ) ;
	void	SetValueEx( int nRow, int nCol, int nRowDataNum, int nColDataNum, double *dValue[], int nDecimalPoint ) ;
	void	SetValueEx( int nRow, int nCol, int nRowDataNum, int nColDataNum, CString *strValue[] ) ;

	void	FillSafeArray( OLECHAR FAR* sz, int iRow, int iCol, COleSafeArray* sa ) ;
	void	FillSafeArray( double dValue, int iRow, int iCol, COleSafeArray* sa ) ;
	void	FillSafeArray( CString strValue, int iRow, int iCol, COleSafeArray* sa ) ;

	CString GetNumberFormat( int nStyle, int nDecimalPoint = 0 ) ;
	CString	GetValue( long lRow, long lCol ) ;

	void	SetColWidth( long lCol, long lWidth ) ;
	void	SetColWidth( long lColStart, long lColEnd, long lWidth ) ;
	void	SetRowHeight( long lRow, long lHeight ) ;
	void	SetRowHeight(long lRowStart, long lRowEnd, long lHeight ) ;

	void	SetFont( CString strFontName, long lSize, BOOL bBold = FALSE ) ;
	void	SetFont( long lRow, long lCol, CString strFontName, long lSize, BOOL bBold = FALSE ) ;
	void	SetFont( long lRowStart, long lColStart, long lRowEnd, long lColEnd, CString strFontName, long lSize, BOOL bBold = FALSE ) ;
	void	SetFontUnderLine( long lStyle = xlLineNone ) ;
	void	SetFontItalic( BOOL bItalic = FALSE ) ;
	void	SetFontBold( BOOL bBold = FALSE ) ;

	void	SetTextColor( long lColor ) ;
	void	SetTextColor( long lRow, long lCol, long lColor ) ;
	void	SetTextColor( long lRowStart, long lColStart, long lRowEnd, long lColEnd, long lColor ) ;
	void	SetBkColor( long lColor ) ;
	void	SetBkColor( long lRow, long lCol, long lColor ) ;
	void	SetBkColor( long lRowStart, long lColStart, long lRowEnd, long lColEnd, long lColor ) ;
	void	SetTextHAlign( long lHAligh = xlAlignHLeft ) ;
	void	SetTextVAlign( long lVAlign = xlAlignVCenter ) ;
	void	SetTextAlign( long lHAligh = xlAlignHLeft, long lVAlign = xlAlignVCenter ) ;

	// Edge -> Color -> Weight -> Style 순서로...
	void	SetLineEdge( long lEdge ) ;
	void	SetLineColor( long lColor ) ;
	void	SetLineWeight( long lWeight ) ;
	void	SetLineStyle( long lType);

	//먼저 병합 후 데이터 입력..
	void	SetMergeCell( BOOL bMerge = FALSE ) ;
 	void	SetMergeCell( long lRowStart, long lColStart, long lRowEnd, long lColEnd, BOOL bMerge = FALSE ) ;

	void	SetAutoFitColumn( long lCol ) ;				//열 간격 자동 맞추기
	void	SetAutoFitColumn( long lColStart, long lColEnd ) ;

	void	SetFormula( long lRow, long lCol, CString strFormula ) ;

	void	SetWrapText( BOOL bWrap = FALSE ) ;

	int		CreateChart( int lDrawSheetIndex
						, long lRowDrawStart, long lColDrawStart, long lRowDrawEnd, long lColDrawEnd
						, int sChartStyle, int sPlotBy, BOOL bHasLegend
						, CString sTitle, CString strCategoryTitle, CString strValueTitle
						, int lDataSheetIndex
						, long lRowDataStart, long lColDataStart, long lRowDataEnd, long lColDataEnd ) ;

	int		CreateChart( long lRowDrawStart, long lColDrawStart, long lRowDrawEnd, long lColDrawEnd
						, int sChartStyle, int sPlotBy, BOOL bHasLegend
						, CString sTitle, CString strCategoryTitle, CString strValueTitle ) ;

	int		SetActiveChart( int nSheetIndex, int nChartIndex ) ;

	LPDISPATCH	AddChart_Series( CString strName ) ;
	int		DeleteChart_Series( int nIndex ) ;
	void	SetChartHasTitle( BOOL bTitle ) ;	
	void	SetChart_Series_CharType( LPDISPATCH pSeries, long lChartType ) ;
	void	SetChart_Series_MarkerStyle( LPDISPATCH pSeries, long lMarkerStyle ) ;
	void	SetChart_Series_MarkerSize( LPDISPATCH pSeries, long lMarkerSize ) ;
	void	SetChart_Series_MarkerForeColor( LPDISPATCH pSeries, long lMarkerForeColor ) ;
	void	SetChart_Series_MarkerBkColor( LPDISPATCH pSeries, long lMarkerBkColor ) ;
	int		SetChart_Series_DataEx( LPDISPATCH pSeries, int nRowDataNum, int nColDataNum, double *dValue[] ) ;
	long	GetChart_SeriesNum() ;

	int		SetChart_BorderStyle( long lBorderStyle ) ;
	int		SetChart_Axis_X_MaxScale( double dMax ) ;
	int		SetChart_Axis_Y_MaxScale( double dMax ) ;
	int		SetChart_Axis_X_MinScale( double dMin ) ;
	int		SetChart_Axis_Y_MinScale( double dMin ) ;
	int		SetChart_Axis_X_MaxScaleAuto( BOOL bAuto ) ;
	int		SetChart_Axis_Y_MaxScaleAuto( BOOL bAuto ) ;
	int		SetChart_Axis_X_MinScaleAuto( BOOL bAuto ) ;
	int		SetChart_Axis_Y_MinScaleAuto( BOOL bAuto ) ;
	int		SetChart_Axis_X_DisplayUnit( long xlDisplayUnit ) ;
	int		SetChart_Axis_Y_DisplayUnit( long xlDisplayUnit ) ;
	int		SetChart_Axis_X_HasMajorGridlines( BOOL bGrid );
	int		SetChart_Axis_Y_HasMajorGridlines( BOOL bGrid );
	int		SetChart_Axis_X_Label_NumberFormat( int nFormatType );
	int		SetChart_Axis_Y_Label_NumberFormat( int nFormatType );
	int		SetChart_plotarea_BkColor( long nColor ) ;
	int		SetChart_plotarea_BorderStyle( long lWeight, long nColor = 0, long lBorderStyle = xlLineStyle_Continuous ) ;
	int		SetChart_plotarea_Pos( double lLeft = 0, double lTop = 0, double lWidth = 0, double lHeight = 0 ) ;
	int		SetChart_Legend_Position( long lPosition ) ;
	int		SetChart_Legend_BorderStyle( long lWeight, long nColor = 0, long lBorderStyle = xlLineStyle_Continuous ) ;
	int		SetChart_Legend_Pos( double lLeft = 0, double lTop = 0, double lWidth = 0, double lHeight = 0  ) ;
	int		GetChart_Legend_Pos( double &lLeft, double &lTop, double &lWidth, double &lHeight ) ;
	int		SetChart_Legend_Delete() ;

	int		InsertPicture( long lRow, long lCol, CString strFileName ) ;
	int		InsertPicture( long lRowStart, long lColStart, long lRowEnd, long lColEnd, CString strFileName ) ;
	int		InsertPicture2( long lRow, long lCol, CString strFileName ) ;

protected:
	CString GetColAlphabetName( long lCol ) ;

protected:
	_Application*		m_pApp ;
	Workbooks			m_books ;
	_Workbook			m_book ;
	Worksheets			m_sheets ;
	_Worksheet			m_sheet ;
	Range				m_range ;
	Borders				m_borders ;
	ChartObjects		m_chartobjects ;
	ChartObject			m_chartobject ;
	_Chart				m_chart ;
	Legend				m_legend ;
	SeriesCollection	m_seriescollection ;
	Series				m_series ;
	Axis				m_axis ;
	TickLabels			m_thickLabels ;
	PlotArea			m_plotarea ;
	Interior			m_interior ;

};

#endif // !defined(AFX_KEXCEL_H__14CFB192_97AA_49DA_96E6_47D5D4204684__INCLUDED_)
