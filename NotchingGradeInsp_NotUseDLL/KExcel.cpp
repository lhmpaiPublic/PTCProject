// KExcel.cpp: implementation of the KExcel class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "excel.h"
#include "ExcelConst.h"
#include "KExcel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COleVariant covTrue((short)TRUE),  covFalse((short)FALSE), covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

KExcel::KExcel()
{
	m_pApp = NULL ;
}

KExcel::~KExcel()
{
	Close() ;
}

int KExcel::Init()
{
	if(m_pApp != NULL)
	{
		Close();
	}

	m_pApp=new _Application;
	if(!m_pApp->CreateDispatch(_T("Excel.Application"))) {
		AfxMessageBox( _T("엑셀을 시작할 수 없습니다!!\n\r엑셀 프로그램이 설치 되지 않았거나\n\r엑셀 프로그램에 문제가 있을 수 있습니다." ) ) ;
		delete m_pApp;
		return -1;
	}

	m_pApp->SetDisplayAlerts( FALSE ) ;
	m_books=m_pApp->GetWorkbooks();

	return 0;
}

void KExcel::Close()
{
	if(m_pApp != NULL)
	{
		m_pApp->Quit() ;

		m_borders.ReleaseDispatch() ;
		m_thickLabels.ReleaseDispatch() ;
		m_axis.ReleaseDispatch() ;
		m_series.ReleaseDispatch() ;
		m_seriescollection.ReleaseDispatch() ;
		m_plotarea.ReleaseDispatch() ;
		m_interior.ReleaseDispatch() ;
		m_legend.ReleaseDispatch() ;
		m_chart.ReleaseDispatch() ;
		m_chartobject.ReleaseDispatch() ;
		m_chartobjects.ReleaseDispatch() ;
		m_range.ReleaseDispatch() ;
		m_sheet.ReleaseDispatch() ;
		m_sheets.ReleaseDispatch() ;
		m_book.ReleaseDispatch() ;
		m_books.ReleaseDispatch() ;
		m_pApp->ReleaseDispatch() ;
	
		delete m_pApp;
		m_pApp=NULL;
	}
}

void KExcel::Show()
{
	m_pApp->SetVisible( TRUE ) ;
}

void KExcel::SetActiveSheet( int nSheetIndex )
{
	if( m_sheets == NULL ) {
		return ;
	}

	int nTemp = ( int)m_sheets.GetCount() ;

	if( ( nSheetIndex - nTemp ) > 0 ) {
		COleVariant After ;
		After.vt = VT_DISPATCH ;
		for( int i = 0 ; i < ( nSheetIndex - nTemp ) ; i++ ) {
			m_sheet = m_sheets.GetItem( COleVariant( ( short)( nTemp+ i ) ) ) ;
			After.pdispVal = m_sheet.m_lpDispatch ;
			After.pdispVal->AddRef();
			m_sheet = m_sheets.Add( covOptional, After, covOptional, covOptional ) ;
		}
	}else {
		m_sheet = m_sheets.GetItem( COleVariant( ( short)nSheetIndex ) ) ;
	}
	m_sheet.Activate() ;
}

int KExcel::CopySheet( BOOL bAfter /*= TRUE*/ )
{
	if( m_sheets == NULL ) {
		return -1 ;
	}

	COleVariant val ;
	val.vt = VT_DISPATCH ;
	val.pdispVal = m_sheet.m_lpDispatch ;
	val.pdispVal->AddRef() ;
	if( bAfter == TRUE ) {			
		m_sheet.Copy( covOptional, val ) ;		//Copy된 Sheet를 뒤로 삽입
	}else {
		m_sheet.Copy( val, covOptional ) ;		//Copy된 Sheet를 앞으로 삽입
	}

	m_sheet.Activate() ;

	return 0 ;
}

int KExcel::DeleteSheet( int nSheetIndex )
{
	if( m_sheets == NULL ) {
		return -1 ;
	}

	int nTemp = ( int)m_sheets.GetCount() ;
	if( nSheetIndex > nTemp ) {
		return -2 ;
	}

	m_sheet = m_sheets.GetItem( COleVariant( ( short)nSheetIndex ) ) ;
	m_sheet.Delete() ;

	return 0 ;
}


int KExcel::Open( CString strFileName /* = _T( "" )*/ )
{
	try {
		if( m_books != NULL ) {
			if( _tcscmp( strFileName, _T("") ) != 0  ) {
				m_book = m_books.Open( strFileName,
										covOptional,covOptional,covOptional,covOptional,
										covOptional,covOptional,covOptional,covOptional,
										covOptional,covOptional,covOptional,covOptional,
										covOptional,covOptional ) ;
			}else {
				m_book=m_books.Add(covOptional);
			}
			m_sheets=m_book.GetWorksheets();
			SetActiveSheet(1);
			return 0 ;
		}else {
			AfxMessageBox( _T( "엑셀이 초기화 되어 있지 않습니다." ) ) ;
			return -1 ;
		}

	}catch(COleException *e) {
		TCHAR buf[1024];
		::wsprintf(buf, _T("COleException. SCODE: %08lx."), (long)e->m_sc);
		::MessageBox(NULL, buf, _T( "COleException" ), MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}catch(COleDispatchException *e) {
		TCHAR buf[1024];
		::wsprintf(buf, _T("COleDispatchException. SCODE: %08lx, Description: \"%s\"."),
				(long)e->m_wCode, (LPSTR)e->m_strDescription.GetBuffer(1024));
		::MessageBox(NULL, buf, _T( "COleDispatchException" ),
					MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}catch(...) {
		::MessageBox(NULL, _T( "General Exception caught.") , _T( "Catch-All" ),
                    MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}
}

int KExcel::Save()
{
	try {
		if( m_book != NULL ) {
			m_book.Save();
			return 0 ;
		}else {
			AfxMessageBox( _T( "Excel파일의 저장을 실패하였습니다." ) ) ;
			return -1 ;
		}
	}catch(COleException *e) {
		TCHAR buf[1024];
		::wsprintf( buf, _T( "COleException. SCODE: %08lx." ), (long)e->m_sc );
		::MessageBox(NULL, buf, _T( "COleException" ), MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}catch(COleDispatchException *e) {
		TCHAR buf[1024];
		::wsprintf(buf, _T( "COleDispatchException. SCODE: %08lx, Description: \"%s\"." ),
			   (long)e->m_wCode, (LPSTR)e->m_strDescription.GetBuffer(1024));
		::MessageBox(NULL, buf, _T( "COleDispatchException" ),
					MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}catch(...) {
		::MessageBox(NULL, _T( "General Exception caught.") , _T( "Catch-All" ),
                    MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}
}

int KExcel::SaveAs( CString strFileName )
{
	try {
		if( m_book != NULL ) {
			m_book.SaveAs(COleVariant(strFileName), COleVariant((long)-4143), //xlNoraml(-4143)=> xls, xlCurrentPlatformText => txt
							covOptional,
							covOptional,
							covFalse,
							covFalse,
							(long)0 , //xlNoChange(0)
							COleVariant((long)1), //xlUserResolution(1) 
							covFalse,
							covFalse,
							covFalse,
							covFalse);
			return 0 ;
		}else {
			AfxMessageBox( _T( "Failed Save Excel" ) ) ;
			return -1 ;
		}
	}catch(COleException *e) {
		TCHAR buf[1024];
		::wsprintf( buf, _T( "COleException. SCODE: %08lx." ), (long)e->m_sc );
		::MessageBox(NULL, buf, _T( "COleException" ), MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}catch(COleDispatchException *e) {
		TCHAR buf[1024];
		::wsprintf(buf, _T( "COleDispatchException. SCODE: %08lx, Description: \"%s\"." ),
			   (long)e->m_wCode, (LPSTR)e->m_strDescription.GetBuffer(1024));
		::MessageBox(NULL, buf, _T( "COleDispatchException" ),
					MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}catch(...) {
		::MessageBox(NULL, _T( "General Exception caught.") , _T( "Catch-All" ),
                    MB_SETFOREGROUND | MB_OK);
		Close() ;
		return -1 ;
	}
}

int KExcel::SetSheetName( int nSheetIndex, CString strSheetName ) 
{
	if( m_sheets == NULL ) {
		return -1 ;
	}

	int nTemp = ( int)m_sheets.GetCount() ;
	if( nSheetIndex > nTemp ) {
		AfxMessageBox( _T( "지정된 Sheet가 존재하지 않습니다." ) ) ;
		return -1 ;
	}

	for( int i = 0 ; i < nTemp  ; i++ ) {
		m_sheet = m_sheets.GetItem( COleVariant( ( short)( i + 1 ) ) ) ;
		if( _tcscmp( m_sheet.GetName(), strSheetName ) == 0  ) {
			AfxMessageBox( _T( "같은 이름의 Sheet가 이미 존재합니다." ) ) ;
			return -1 ;
		}
	}

	m_sheet = m_sheets.GetItem( COleVariant( ( short)nSheetIndex ) ) ;
	m_sheet.SetName( strSheetName ) ;
	return 0 ;
}

CString	KExcel::GetSheetName()
{
	return m_sheet.GetName() ;
}

void KExcel::SetFreezePanes( long lRow, long lCol, BOOL bFreeze )
{
	Windows windows = m_pApp->GetWindows() ;
	Window window = windows.GetItem( COleVariant( (short)1 ) ) ;
	LPDISPATCH lpDisp = GetRange( lRow, lCol ) ;
	ASSERT(lpDisp);
	m_range.AttachDispatch(lpDisp);
	m_range.Select() ;
	window.SetFreezePanes( bFreeze ) ;	
}

CString KExcel::GetColAlphabetName( long lCol )
{
	CString strColName = _T( "" ) ;

	if( lCol > 256 || lCol < 0 ) 
	{
		ASSERT(0);
		return _T( "" );
	}
	
	int i = ( lCol - 1 ) / 26 ;
	int j = lCol - ( i * 26 ) ;
	int k = 'A' - 1 ;
	
	
	if( i == 0 ) {
		strColName.Format( _T("%c"), k + j ) ;
	}else {
		strColName.Format( _T("%c%c"), k + i, k + j ) ;
	}
	
	return strColName;
}

void KExcel::SetRange( long lRow, long lCol )
{
	if( m_sheets == NULL ) {
		return ;
	}

//	CString strCell = _T( "" );
//	strCell.Format( _T("%s%d"), GetColAlphabetName( lCol ), lRow ) ;
//	m_range = m_sheet.GetRange( COleVariant( strCell ), COleVariant( strCell ) ) ;
//	m_borders = m_range.GetBorders() ;
	SetRange( lRow, lCol, lRow, lCol ) ;

}

void KExcel::SetRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd )
{
	if( m_sheets == NULL ) {
		return ;
	}

	if( ( ( lRowStart == 0 ) && ( lRowEnd == 0 ) ) && ( ( lColStart > 0 ) && ( lColEnd > 0 ) ) ) {
		SetRangeEntireColumn( lColStart, lColEnd ) ;
	}else if( ( ( lRowStart > 0 ) && ( lRowEnd > 0 ) ) && ( ( lColStart == 0 ) && ( lColEnd == 0 ) ) ) {
		SetRangeEntireRow( lRowStart, lRowEnd ) ;
	}else if( ( lRowStart == 0 ) || ( lRowEnd == 0 ) || ( lRowStart == 0 ) || ( lRowEnd == 0 ) ) {
		return ;
	}else {
		CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
		strCellStart.Format( _T("%s%d"), GetColAlphabetName( lColStart ), lRowStart ) ;
		strCellEnd.Format( _T("%s%d"), GetColAlphabetName( lColEnd ), lRowEnd ) ;
		m_range = m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;
		m_borders = m_range.GetBorders() ;
	}
}

LPDISPATCH KExcel::GetRange( long lRow, long lCol )
{
	if( m_sheets == NULL ) {
		return NULL ;
	}
//	CString strCell = _T( "" );
//	strCell.Format( _T("%s%d"), GetColAlphabetName( lCol ), lRow ) ;
//	return ( m_sheet.GetRange( COleVariant( strCell ), COleVariant( strCell ) ) ) ;
	return ( GetRange( lRow, lCol, lRow, lCol ) ) ;

}
LPDISPATCH KExcel::GetRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd )
{
	if( m_sheets == NULL ) {
		return NULL ;
	}
	if( lRowStart == 0 || lColStart == 0 || lRowEnd == 0 || lColEnd == 0  ) {
		return NULL ;
	}
	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
	strCellStart.Format( _T("%s%d"), GetColAlphabetName( lColStart ), lRowStart ) ;
	strCellEnd.Format( _T("%s%d"), GetColAlphabetName( lColEnd ), lRowEnd ) ;
	return ( m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ) ;
}

void KExcel::SetRangeEntireColumn( long lColStart, long lColEnd )
{
	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
	strCellStart.Format( _T("%s1"), GetColAlphabetName( lColStart ) ) ;
	strCellEnd.Format( _T("%s1"), GetColAlphabetName( lColEnd ) ) ;
	Range Range = m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;

	m_range = Range.GetEntireColumn() ;

	Range.ReleaseDispatch() ;
}

void KExcel::SetRangeEntireColumn( long lCol )
{
	SetRangeEntireColumn( lCol, lCol ) ;
}

void KExcel::SetRangeEntireRow( long lRowStart, long lRowEnd )
{
	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
	strCellStart.Format( _T("A%d"), lRowStart ) ;
	strCellEnd.Format( _T("A%d"), lRowEnd ) ;
	Range Range = m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;

	m_range = Range.GetEntireRow() ;

	Range.ReleaseDispatch() ;
}

void KExcel::SetRangeEntireRow( long lCol )
{
	SetRangeEntireRow( lCol, lCol ) ;
}

void KExcel::SetRangeNumberFormat( long lRowStart, long lColStart, long lRowEnd, long lColEnd, int nStyle, int nDecimalPoint/* = 0*/ )
{
	CString strNumberFormat = GetNumberFormat( nStyle, nDecimalPoint ) ;
	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	m_range.SetNumberFormatLocal( COleVariant( strNumberFormat ) ) ;
}

void KExcel::SetRangeNumberFormat( long lRow, long lCol, int nStyle, int nDecimalPoint/* = 0*/ )
{
	SetRangeNumberFormat( lRow, lCol, lRow, lCol, nStyle, nDecimalPoint ) ; 
}

void KExcel::SetRangeSelect( long lRow, long lCol )
{
	SetRange( lRow, lCol ) ;
	m_range.Select() ;
}

void KExcel::SetRangeSelect( long lRowStart, long lColStart, long lRowEnd, long lColEnd )
{
	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	m_range.Select() ;
}

void KExcel::ClearRange( long lRow, long lCol )
{
	if( m_range == NULL ) {
		return ;
	}	

	SetRange( lRow, lCol ) ;
	m_range.Clear() ;
}

void KExcel::ClearRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd )
{
	if( m_range == NULL ) {
		return ;
	}

	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	m_range.Clear() ;
}

int KExcel::SortRange( long lRowStart, long lColStart, long lRowEnd, long lColEnd
					 , long lKeyRowStart, long lKeyColStart, long lKeyRowEnd, long lKeyColEnd
					 , long lSortOrder /*= xlAscending*/, long lSortOrientation /*= xlSortColumns*/ )
{
	if( m_range == NULL ) {
		return -1 ;
	}

	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
	strCellStart.Format( _T("%s%d"), GetColAlphabetName( lKeyColStart ), lKeyRowStart ) ;
	strCellEnd.Format( _T("%s%d"), GetColAlphabetName( lKeyColEnd ), lKeyRowEnd ) ;

	Range KeyRange ;
	LPDISPATCH lpDisp = m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;
	ASSERT(lpDisp);
	KeyRange.AttachDispatch( lpDisp ) ;

	VARIANT var;
	var.vt = VT_DISPATCH;
	var.pdispVal = lpDisp;

	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	m_range.SortSpecial( xlPinYin, var, lSortOrder, covOptional, covOptional, lSortOrder,
						 covOptional, lSortOrder, xlGuess, COleVariant( ( short)( 1) ),
						 covFalse, lSortOrientation, 0, 0, 0 ) ;

	KeyRange.ReleaseDispatch() ;

	return 0 ;
}

int KExcel::CopyRange( long lSrcRowStart, long lSrcColStart, long lSrcRowEnd, long lSrcColEnd, long lDestSheetIndex
					 , long lDestRowStart, long lDestColStart, long lDestRowEnd, long lDestColEnd )
{
	if( m_range == NULL ) {
		return -1 ;
	}
	if( lDestSheetIndex > ( int)m_sheets.GetCount() ) {
		return -2 ;
	}

	SetRange( lSrcRowStart, lSrcColStart, lSrcRowEnd, lSrcColEnd ) ;
 	m_range.Copy( covOptional ) ;

	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
	strCellStart.Format( _T("%s%d"), GetColAlphabetName( lDestColStart ), lDestRowStart ) ;
	strCellEnd.Format( _T("%s%d"), GetColAlphabetName( lDestColEnd ), lDestRowEnd ) ;

	_Worksheet pasteSheet = m_sheets.GetItem( COleVariant( ( short)( lDestSheetIndex ) ) ) ;
	pasteSheet.Activate() ;
	Range pasteRange = pasteSheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;
	BOOL bPasteSpecial = FALSE ;
	if( bPasteSpecial == TRUE ) {
		pasteRange.Select() ;
		pasteRange.PasteSpecial( xlPasteAll, xlPasteSpecialOperationNone, covOptional, covOptional ) ;
	}else {
		pasteRange.Select() ;
		pasteSheet.Paste( covOptional, covOptional ) ;
	}
	pasteRange = pasteSheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellStart ) ) ;
	pasteRange.Select() ;

	pasteRange.ReleaseDispatch() ;
	pasteSheet.ReleaseDispatch() ;
	m_sheet.Activate() ;

	return 0 ;
}

int KExcel::InsertCopyRange( long lInsertRowStart, long lInsertColStart, long lInsertRowEnd, long lInsertColEnd
					   , long lInsertShiftDirection, long lCopySheetIndex
					   , long lCopyRowStart, long lCopyColStart, long lCopyRowEnd, long lCopyColEnd )
{
	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
	strCellStart.Format( _T("%s%d"), GetColAlphabetName( lCopyColStart ), lCopyRowStart ) ;
	strCellEnd.Format( _T("%s%d"), GetColAlphabetName( lCopyColEnd ), lCopyRowEnd ) ;
	Range CopyRange = m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;
	CopyRange.Copy( covOptional ) ;

	SetRange( lInsertRowStart, lInsertColStart, lInsertRowEnd, lInsertColEnd ) ;
	m_range.Select() ;
	m_range.Insert( COleVariant( lInsertShiftDirection ), COleVariant( (long)xlFormatFromRightOrBelow ) ) ;

	CopyRange.ReleaseDispatch() ;

	return 0 ;
}

int KExcel::InsertCopyRange( long lInsertRow, long lInsertCol, long lInsertShiftDirection, long lCopySheetIndex
					   , long lCopyRow, long lCopyCol )
{
	return InsertCopyRange( lInsertRow, lInsertCol, lInsertRow, lInsertCol, lInsertShiftDirection, lCopySheetIndex
					  , lCopyRow, lCopyCol, lCopyRow, lCopyCol ) ;
}

int KExcel::InsertRange( long lInsertRowStart, long lInsertColStart, long lInsertRowEnd, long lInsertColEnd
					   , long lInsertShiftDirection )
{
	SetRange( lInsertRowStart, lInsertColStart, lInsertRowEnd, lInsertColEnd ) ;
	m_range.Select() ;
	m_range.Insert( COleVariant( lInsertShiftDirection ), COleVariant( (long)xlFormatFromRightOrBelow ) ) ;

	return 0 ;
}

int KExcel::InsertRange( long lInsertRow, long lInsertCol, LONG lInsertShiftDirection )
{
	return InsertRange( lInsertRow, lInsertCol, lInsertRow, lInsertCol, lInsertShiftDirection ) ;
}

int KExcel::DeleteRange( long lDeleteRowStart, long lDeleteColStart, long lDeleteRowEnd, long lDeleteColEnd, long lDeleteShiftDirection )
{
	SetRange( lDeleteRowStart, lDeleteColStart, lDeleteRowEnd, lDeleteColEnd ) ;
	m_range.Delete( COleVariant( lDeleteShiftDirection ) ) ;
	return 0 ;
}

int KExcel::DeleteRange( long lDeleteRow, long lDeleteCol, long lDeleteShiftDirection )
{
	return DeleteRange( lDeleteRow, lDeleteCol, lDeleteRow, lDeleteCol, lDeleteShiftDirection ) ;
}

void KExcel::SetValue( CString strValue )
{
	if( m_range == NULL ) {
		return ;
	}
	
	m_range.SetNumberFormatLocal(COleVariant( _T( "@" ) ) ) ;
	m_range.SetValue2( COleVariant( strValue ) ) ;
}

void KExcel::SetValue( long lRow, long lCol, CString strValue )
{
	SetRange( lRow, lCol ) ;
	SetValue( strValue ) ;
}

void KExcel::SetValue( long lValue )
{
	if( m_range == NULL ) {
		return ;
	}

	m_range.SetNumberFormatLocal( COleVariant( _T( "0_ " ) ) ) ;
	m_range.SetValue2( COleVariant( lValue ) ) ;
}

void KExcel::SetValue( long lRow, long lCol, long lValue )
{
	SetRange( lRow,lCol ) ;
	SetValue( lValue ) ;
}

void KExcel::SetValue( double lValue, int nDecimalPoint )
{
	if( m_range == NULL ) {
		return ;
	}

	CString strNumberFormat = GetNumberFormat( Format_Numeric, nDecimalPoint ) ;

	m_range.SetNumberFormatLocal( COleVariant( strNumberFormat ) ) ;
	m_range.SetValue2( COleVariant( lValue ) ) ;
}

void KExcel::SetValue( long lRow, long lCol, double lValue, int nDecimalPoint )
{
	SetRange( lRow,lCol  ) ;
	SetValue( lValue, nDecimalPoint ) ;
}

void KExcel::SetValue_Comma( long lValue )
{
	if( m_range == NULL ) {
		return ;
	}

	m_range.SetNumberFormatLocal(COleVariant( _T("#,##0_ ") ) ) ;
	m_range.SetValue2( COleVariant( lValue ) ) ;
}

void KExcel::SetValue_Comma( long lRow, long lCol, long lValue )
{
	SetRange( lRow,lCol  ) ;
	SetValue_Comma( lValue ) ;
}

void KExcel::SetValue_Comma( double lValue, int nDecimalPoint )
{
	if( m_range == NULL ) {
		return ;
	}
	
	CString strNumberFormat = GetNumberFormat( Format_Comma, nDecimalPoint ) ;

	m_range.SetNumberFormatLocal( COleVariant( strNumberFormat ) ) ;
	m_range.SetValue2( COleVariant( lValue ) ) ;
}

void KExcel::SetValue_Comma( long lRow, long lCol, double lValue, int nDecimalPoint )
{
	SetRange( lRow,lCol  ) ;
	SetValue_Comma( lValue, nDecimalPoint ) ;
}

void KExcel::SetValue_Percent( long lValue )
{
	if( m_range == NULL ) {
		return ;
	}

	m_range.SetNumberFormatLocal( COleVariant( _T("0%" ) ) ) ;
	m_range.SetValue2( COleVariant( lValue ) ) ;
}

void KExcel::SetValue_Percent( long lRow, long lCol, long lValue )
{
	SetRange( lRow,lCol  ) ;
	SetValue_Percent( lValue ) ;
}

void KExcel::SetValue_Percent( double lValue, int nDecimalPoint )
{
	if( m_range == NULL ) {
		return ;
	}
	
	CString strNumberFormat = GetNumberFormat( Format_Percent, nDecimalPoint ) ;

	m_range.SetNumberFormatLocal( COleVariant( strNumberFormat ) ) ;
	m_range.SetValue2( COleVariant( lValue ) ) ;
}

void KExcel::SetValue_Percent( long lRow, long lCol, double lValue, int nDecimalPoint )
{
	SetRange( lRow,lCol  ) ;
	SetValue_Percent( lValue, nDecimalPoint ) ;
}

void KExcel::SetValueEx( COleSafeArray sa )
{
	m_range.SetValue2( COleVariant( sa ) ) ;
}

void KExcel::SetValueEx( int nRow, int nCol, int nRowDataNum, int nColDataNum, double *dValue[], int nDecimalPoint )
{
	COleSafeArray saRet ;
	DWORD numElements[ 2 ] ; //={5,2};   //5(Row)x2(Col) element array
	numElements[ 0 ] = nRowDataNum ;
	numElements[ 1 ] = nColDataNum ;
	saRet.Create( VT_VARIANT, 2, numElements );

	for( int i = 0 ; i < nRowDataNum ; i++ ) {
		for( int j = 0 ; j < nColDataNum ; j++ ) {
			FillSafeArray( dValue[ i ][ j ], i, j, &saRet ) ;
		}
	}

	SetRange( nRow, nCol, ( nRow + ( nRowDataNum - 1 ) ), ( nCol + ( nColDataNum - 1 ) ) ) ;

	CString strNumberFormat = GetNumberFormat( Format_Numeric, nDecimalPoint ) ;
	m_range.SetNumberFormatLocal( COleVariant( strNumberFormat ) ) ;
	m_range.SetValue2( COleVariant( saRet ) ) ;
	saRet.Detach() ;
}

void KExcel::SetValueEx( int nRow, int nCol, int nRowDataNum, int nColDataNum, CString *strValue[] )
{
	COleSafeArray saRet ;
	DWORD numElements[ 2 ] ; //={5,2};   //5(Row)x2(Col) element array
	numElements[ 0 ] = nRowDataNum ;
	numElements[ 1 ] = nColDataNum ;
	saRet.Create( VT_VARIANT, 2, numElements );

	CString strTemp ;

	for( int i = 0 ; i < nRowDataNum ; i++ ) {
		for( int j = 0 ; j < nColDataNum ; j++ ) {
			FillSafeArray( strValue[ i ][ j ], i, j, &saRet ) ;
		}
	}

	SetRange( nRow, nCol, ( nRow + ( nRowDataNum - 1 ) ), ( nCol + ( nColDataNum - 1 ) ) ) ;
	m_range.SetValue2( COleVariant( saRet ) ) ;
	saRet.Detach() ;
}

void KExcel::FillSafeArray( OLECHAR FAR* sz, int iRow, int iCol, COleSafeArray* sa )
{
	VARIANT v ;
	long index[ 2 ] ;
	index[ 0 ] = iRow ;
	index[ 1 ] = iCol ;
	VariantInit( &v ) ;
	v.vt = VT_BSTR ;
	v.bstrVal = SysAllocString( sz ) ;
	sa->PutElement( index, &v ) ;
	SysFreeString( v.bstrVal ) ;
	VariantClear( &v ) ; 
}

void KExcel::FillSafeArray( double dValue, int iRow, int iCol, COleSafeArray* sa )
{
	VARIANT v ;
	long index[ 2 ] ;
	index[ 0 ] = iRow ;
	index[ 1 ] = iCol ;
	VariantInit( &v ) ;
	v.vt = VT_R8 ;
	v.dblVal = dValue ;
	sa->PutElement( index, &v ) ;
	VariantClear( &v ) ; 
}

void KExcel::FillSafeArray( CString strValue, int iRow, int iCol, COleSafeArray* sa )
{
	VARIANT v ;
	long index[ 2 ] ;
	index[ 0 ] = iRow ;
	index[ 1 ] = iCol ;
	VariantInit( &v ) ;
	v.vt = VT_BSTR ;
#ifdef  _UNICODE
	v.bstrVal = SysAllocString( strValue ) ;
#else
	v.bstrVal = strValue.AllocSysString() ;
#endif
	sa->PutElement( index, &v ) ;
	SysFreeString( v.bstrVal ) ;
	VariantClear( &v ) ; 
}

CString KExcel::GetNumberFormat( int nStyle, int nDecimalPoint /*= 0*/ )
{
	CString strTemp = _T( "" ) ;
	if( ( nDecimalPoint > 0 ) && ( nStyle != Format_Text ) ) {
		strTemp = _T( "." ) ;
	}

	for(int i = 0; i < nDecimalPoint; i++ ) {
		strTemp = strTemp + _T( "0" ) ;
	}
	
	CString strNumberFormat ;
	switch( nStyle )
	{
		case Format_Text :
			strNumberFormat = _T( "@" ) ;
			break ;
		case Format_Comma :
			strNumberFormat = _T("#,##0") ;
			strNumberFormat = strNumberFormat + strTemp + _T( "_ " ) ;
			break ;
		case Format_Percent :
			strNumberFormat = _T( "0" ) ;
			strNumberFormat = strNumberFormat + strTemp + _T( "%" ) ;
			break ;
		case Format_Numeric :
		default:
			strNumberFormat = _T( "0" ) ;
			strNumberFormat = strNumberFormat + strTemp + _T( "_ " ) ;
			break ;
	}

	return strNumberFormat ;
}

CString	KExcel::GetValue( long lRow, long lCol )
{
	CString strRet = _T( "" ) ;

	SetRange( lRow, lCol ) ;
	COleVariant value = m_range.GetValue2() ;
	if( value.vt != VT_BSTR ) {
		value.ChangeType( VT_BSTR ) ;
	}
	strRet = value.bstrVal ;
	
	return strRet ;
}

void KExcel::SetColWidth( long lCol, long lWidth )
{
	SetColWidth( lCol, lCol, lWidth ) ;
}

void KExcel::SetColWidth( long lColStart, long lColEnd, long lWidth )
{
	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;
	CString strColStart_Name = GetColAlphabetName( lColStart ) ;
	CString strColEnd_Name = GetColAlphabetName( lColEnd ) ;

	strCellStart.Format( _T( "%s:%s" ), strColStart_Name, strColStart_Name ) ;
	strCellEnd.Format( _T( "%s:%s" ), strColEnd_Name, strColEnd_Name ) ;

	m_range=m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;
	m_range.SetColumnWidth( COleVariant( lWidth ) ) ;
}

void KExcel::SetRowHeight( long lRow, long lHeight )
{
	SetRowHeight( lRow, lRow, lHeight ) ;
}

void KExcel::SetRowHeight(long lRowStart, long lRowEnd, long lHeight )
{
	CString strCellStart = _T( "" ), strCellEnd = _T( "" ) ;

	strCellStart.Format( _T( "%d:%d" ), lRowStart, lRowStart ) ;
	strCellEnd.Format( _T( "%d:%d" ), lRowEnd, lRowEnd ) ;

	m_range=m_sheet.GetRange( COleVariant( strCellStart ), COleVariant( strCellEnd ) ) ;
	m_range.SetRowHeight( COleVariant( lHeight ) ) ;	
}

void KExcel::SetFont( CString strFontName, long lSize, BOOL bBold /*= FALSE*/ )
{
	ExcelFont font = m_range.GetFont() ;
	font.SetName( COleVariant( strFontName ) ) ;
	font.SetSize( COleVariant( lSize ) ) ;
	SetFontBold( bBold ) ;
	SetFontUnderLine() ;
	SetFontItalic() ;	
}

void KExcel::SetFont( long lRow, long lCol, CString strFontName, long lSize, BOOL bBold /*= FALSE*/ )
{
	SetRange( lRow, lCol ) ;
	SetFont( strFontName, lSize, bBold ) ;
}

void KExcel::SetFont( long lRowStart, long lColStart, long lRowEnd, long lColEnd, CString strFontName, long lSize, BOOL bBold /*= FALSE*/ )
{
	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	SetFont( strFontName, lSize, bBold ) ;	
}

void KExcel::SetFontUnderLine( long lStyle /*= xlLineNone*/ )
{
	switch( lStyle ) {
		case xlTextUnderLineNone :
		case xlTextUnderLineSingle :
		case xlTextUnderLineDouble :
			break ;
		default :
			return ;
			break ;
	}
	ExcelFont font = m_range.GetFont() ;
	font.SetUnderline( COleVariant( lStyle ) ) ;
}

void KExcel::SetFontItalic( BOOL bItalic /*= FALSE*/ )
{
	ExcelFont font = m_range.GetFont() ;

	if( bItalic == TRUE ) {
		font.SetItalic( COleVariant( ( long)1 ) ) ;
	}else {
		font.SetItalic( COleVariant( ( long)0 ) ) ;
	}
}

void KExcel::SetFontBold( BOOL bBold /*= FALSE*/ )
{
	ExcelFont font = m_range.GetFont() ;
	
	if( bBold == TRUE ) {
		font.SetBold( COleVariant( ( long)1 ) ) ;
	}else {
		font.SetBold( COleVariant( ( long)0 ) ) ;
	}
}

void KExcel::SetTextHAlign( long lHAligh /*= xlAlignHLeft*/ )
{
	switch( lHAligh ) {
		case xlAlignHLeft : 
		case xlAlignHCenter :
		case xlAlignHRight :
			break ;
		default :
			return ;
			break ;
	}
	m_range.SetHorizontalAlignment( COleVariant( lHAligh ) ) ;
}

void KExcel::SetTextVAlign( long lVAlign /*= xlAlignVCenter*/ )
{
	switch( lVAlign ) {
		case xlAlignVTop :
		case xlAlignVCenter :
		case xlAlignVBottom :
			break ;
		default :
			return ;
			break ;
	}
	m_range.SetVerticalAlignment( COleVariant( lVAlign ) ) ;
}

void KExcel::SetTextAlign( long lHAligh /*= xlAlignHLeft*/, long lVAlign /*= xlAlignVCenter*/ )
{
	SetTextHAlign( lHAligh ) ;
	SetTextVAlign( lVAlign ) ;
}


void KExcel::SetTextColor( long lColor )
{
	ExcelFont font = m_range.GetFont() ;
	font.SetColor( COleVariant( lColor ) ) ;
}

void KExcel::SetTextColor( long lRow, long lCol, long lColor )
{
	SetRange( lRow, lCol ) ;
	SetTextColor( lColor ) ;
}

void KExcel::SetTextColor( long lRowStart, long lColStart, long lRowEnd, long lColEnd, long lColor )
{
	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	SetTextColor( lColor ) ;
}

void KExcel::SetBkColor( long lColor )
{
	Interior interior = m_range.GetInterior() ;
	interior.SetColor( COleVariant( lColor ) ) ;
}

void KExcel::SetBkColor( long lRow, long lCol, long lColor )
{
	SetRange( lRow, lCol ) ;
	SetBkColor( lColor ) ;
}

void KExcel::SetBkColor( long lRowStart, long lColStart, long lRowEnd, long lColEnd, long lColor )
{
	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	SetBkColor( lColor ) ;
}

void KExcel::SetLineEdge( long lEdge )
{
	switch( lEdge ) {
		case xlLineDiagonalDown : 
		case xlLineDiagonalUp :
		case xlLineLeft : 
		case xlLineTop : 
		case xlLineBottom : 
		case xlLineRight :
		case xlLineInsideVertical :
		case xlLineInsideHorizontal :
		case xlLineNone :
			break ;
		default :
			ASSERT(0) ;
			break ;
	}
	m_borders=m_borders.GetItem( lEdge ) ;
}

void KExcel::SetLineColor( long lColor )
{
	m_borders.SetColor( COleVariant( lColor ) ) ;
}

void KExcel::SetLineWeight( long lWeight )
{
	switch( lWeight ) {
		case xlThin :
		case xlMedium :
		case xlThick :
			break ;
		default :
			ASSERT(0) ;
			break ;
	}
	m_borders.SetWeight( COleVariant( lWeight ) ) ;
}

void KExcel::SetLineStyle( long lType)
{
	switch( lType ) {
		case xlLineStyle_Continuous : 
		case xlLineStyle_Dot : 
		case xlLineStyle_Dash :
		case xlLineStyle_Double :
		case xlLineStyle_None :
			break ;
		default :
			ASSERT(0) ;
			break ;
	}
	m_borders.SetLineStyle( COleVariant( lType ) ) ;
}

void KExcel::SetMergeCell( BOOL bMerge /*= FALSE*/ )
{
	if( bMerge == TRUE ) {
		m_range.SetMergeCells( COleVariant( ( long)1 ) ) ;
	}else {
		m_range.SetMergeCells( COleVariant( ( long)0 ) ) ;
	}
}

void KExcel::SetMergeCell( long lRowStart, long lColStart, long lRowEnd, long lColEnd, BOOL bMerge /*= FALSE*/ )
{
	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;
	SetMergeCell(bMerge);
}

void KExcel::SetAutoFitColumn( long lCol )
{
	SetRange( 1, lCol ) ;
	Range cols = m_range.GetEntireColumn() ;
	cols.AutoFit() ;
	
}

void KExcel::SetAutoFitColumn( long lColStart, long lColEnd )
{
	SetRange( 1, lColStart, 1, lColEnd ) ;
	Range cols = m_range.GetEntireColumn() ;
	cols.AutoFit() ;
}

void KExcel::SetFormula( long lRow, long lCol, CString strFormula )
{
	SetRange( lRow, lCol ) ;
	
	m_range.SetFormula( COleVariant(strFormula) ) ;
}

void KExcel::SetWrapText( BOOL bWrap /*= FALSE*/ )
{
	if( bWrap == TRUE) {
		m_range.SetWrapText( COleVariant( ( long)1 ) ) ;
	}else {
		m_range.SetWrapText( COleVariant( ( long)0 ) ) ;
	}
}

int	KExcel::CreateChart( int lDrawSheetIndex
						, long lRowDrawStart, long lColDrawStart, long lRowDrawEnd, long lColDrawEnd
						, int sChartStyle, int sPlotBy, BOOL bHasLegend
						, CString sTitle, CString strCategoryTitle, CString strValueTitle
						, int lDataSheetIndex
						, long lRowDataStart, long lColDataStart, long lRowDataEnd, long lColDataEnd )
{
	SetActiveSheet( lDrawSheetIndex ) ;
	SetRange( lRowDrawStart, lColDrawStart, lRowDrawEnd, lColDrawEnd ) ;

	VARIANT left, top, width, height ;
	left = m_range.GetLeft();
	top = m_range.GetTop();
	width = m_range.GetWidth();
	height = m_range.GetHeight();

	m_chartobjects = m_sheet.ChartObjects(covOptional);
	m_chartobject = m_chartobjects.Add( (double)left.dblVal+1, (double)top.dblVal+1, (double)width.dblVal-2, (double)height.dblVal-2 ) ; // Adds 1 to item count.

	m_chart = m_chartobject.GetChart() ;

	SetActiveSheet( lDataSheetIndex ) ;
	LPDISPATCH lpDisp = GetRange( lRowDataStart, lColDataStart, lRowDataEnd, lColDataEnd ) ;
	ASSERT(lpDisp);
	m_range.AttachDispatch(lpDisp);

	VARIANT var;			// ChartWizard needs a Variant for the Source range.
	var.vt = VT_DISPATCH;	// .vt is the usable member of the tagVARIANT
	var.pdispVal = lpDisp;	// Contains IDispatch pointer
							// to the Source range.

	COleVariant	vTitle ;
	COleVariant vCategoryTitle ;
	COleVariant vValueTitle ;

	if( sTitle.GetLength() == 0  ) {
		vTitle = COleVariant( (long)DISP_E_PARAMNOTFOUND, VT_ERROR ) ;
	}else {
		vTitle = COleVariant( sTitle ) ;
	}

	if( strCategoryTitle.GetLength() == 0  ) {
		vCategoryTitle = COleVariant( (long)DISP_E_PARAMNOTFOUND, VT_ERROR ) ;
	}else {
		vCategoryTitle = COleVariant( strCategoryTitle ) ;
	}

	if( strValueTitle.GetLength() == 0  ) {
		vValueTitle = COleVariant( (long)DISP_E_PARAMNOTFOUND, VT_ERROR ) ;
	}else {
		vValueTitle = COleVariant( strValueTitle ) ;
	}

	m_chart.ChartWizard(var,							// Source.
					COleVariant( ( short)sChartStyle ),	// Scatter (-4169)
					covOptional,						// Format, use default.
					COleVariant( ( short)sPlotBy ),		// PlotBy xlColumns.
					COleVariant( ( short)1 ),			// CategoryLabels.
					COleVariant( ( short)1 ),			// SeriesLabels.
					COleVariant( ( short)bHasLegend ),	// HasLegend.
					vTitle				,				// Title.
					vCategoryTitle,						// CategoryTitle.
					vValueTitle,						// ValueTitles.
					covOptional							// ExtraTitle.
					);
	::Sleep(100);

	return 0 ;
}

int	KExcel::CreateChart( long lRowDrawStart, long lColDrawStart, long lRowDrawEnd, long lColDrawEnd
						, int sChartStyle, int sPlotBy, BOOL bHasLegend
						, CString sTitle, CString strCategoryTitle, CString strValueTitle )
{
	int nRet = CreateChart( m_sheet.GetIndex(), lRowDrawStart, lColDrawStart, lRowDrawEnd, lColDrawEnd
							, sChartStyle, sPlotBy, bHasLegend, sTitle, strCategoryTitle, strValueTitle
							, m_sheet.GetIndex(), 1, 1, 1, 1 ) ;

	if( nRet >= 0 ) {
		m_seriescollection = m_chart.SeriesCollection( covOptional ) ;
		while( m_seriescollection.GetCount() > 0 ) {
			m_series = m_seriescollection.Item( COleVariant((short)1) ) ;
			m_series.Delete() ;
		}
	}

	return nRet ;
}

int KExcel::SetActiveChart( int nSheetIndex, int nChartIndex )
{
	if( m_sheets == NULL ) {
		return -1 ;
	}

	SetActiveSheet( nSheetIndex ) ;
	m_chartobjects = m_sheet.ChartObjects(covOptional);

	int nCount = ( int)m_chartobjects.GetCount() ;
	if( nChartIndex > nCount ) {
		CString strMsg ;
		strMsg.Format( _T( "해당 인덱스의 Chart가 없습니다.\n시트명:%s, 차트인덱스:%d" ), GetSheetName(), nChartIndex ) ;  
		AfxMessageBox( strMsg ) ;
		m_chartobject.DetachDispatch() ;
		return -1 ;
	}
	
	m_chartobject = m_chartobjects.Item( COleVariant( ( short)nChartIndex ) ) ;
	m_chart = m_chartobject.GetChart() ;

	return 0 ;
}

LPDISPATCH KExcel::AddChart_Series( CString strName )
{
	if( m_chart == NULL ) {
		return NULL ;
	}

	m_seriescollection = m_chart.SeriesCollection( covOptional ) ;
	m_series = m_seriescollection.NewSeries() ;
	m_series.SetName( strName ) ;

	return m_series ;
}

int KExcel::DeleteChart_Series( int nIndex )
{
	if( m_chart == NULL ) {
		return -1 ;
	}

	m_seriescollection = m_chart.SeriesCollection( covOptional ) ;
	if( nIndex <= m_seriescollection.GetCount() ) {
		m_series = m_seriescollection.Item( COleVariant( ( short )nIndex ) ) ;
		m_series.Delete() ;	
	}

	return 0 ;
}

void KExcel::SetChartHasTitle( BOOL bTitle )
{
	if( m_chart != NULL ) {
		m_chart.SetHasTitle( bTitle ) ;
	}
}

void KExcel::SetChart_Series_CharType( LPDISPATCH pSeries, long lChartType )
{
	Series m_series ;
	m_series.AttachDispatch( pSeries ) ;
	m_series.SetChartType( lChartType ) ;
	m_series.DetachDispatch() ;
}

void KExcel::SetChart_Series_MarkerStyle( LPDISPATCH pSeries, long lMarkerStyle )
{
	Series m_series ;
	m_series.AttachDispatch( pSeries ) ;
	m_series.SetMarkerStyle( lMarkerStyle ) ;
	m_series.DetachDispatch() ;
}

void KExcel::SetChart_Series_MarkerSize( LPDISPATCH pSeries, long lMarkerSize )
{
	Series m_series ;
	m_series.AttachDispatch( pSeries ) ;
	m_series.SetMarkerSize( lMarkerSize ) ;
	m_series.DetachDispatch() ;
}

void KExcel::SetChart_Series_MarkerForeColor( LPDISPATCH pSeries, long lMarkerForeColor )
{
	Series m_series ;
	m_series.AttachDispatch( pSeries ) ;
	m_series.SetMarkerForegroundColorIndex( (long)xlColorIndexNone ) ;
	m_series.SetMarkerForegroundColor( lMarkerForeColor ) ;
	m_series.DetachDispatch() ;
}

void KExcel::SetChart_Series_MarkerBkColor( LPDISPATCH pSeries, long lMarkerBkColor )
{
	Series m_series ;
	m_series.AttachDispatch( pSeries ) ;
	m_series.SetMarkerBackgroundColorIndex( (long)xlColorIndexNone ) ;
	m_series.SetMarkerBackgroundColor( lMarkerBkColor ) ;
	m_series.DetachDispatch() ;
}

int KExcel::SetChart_Series_DataEx( LPDISPATCH pSeries, int nRowDataNum, int nColDataNum, double *dValue[] )
{
	if( nRowDataNum <= 0 || nColDataNum <= 0 ) {
		return -1 ;
	}
	if( nColDataNum > 2 ) {
		nColDataNum = 2 ;
	}
	
	Series m_series ;
	m_series.AttachDispatch( pSeries ) ;

	COleSafeArray saRet_X ;
	COleSafeArray saRet ;

	DWORD numElements[ 2 ] ;
	numElements[ 0 ] = nRowDataNum ;
	numElements[ 1 ] = nColDataNum ;

	saRet_X.Create( VT_VARIANT, 2, numElements ) ;
	saRet.Create( VT_VARIANT, 2, numElements );

	for( int i = 0 ; i < nRowDataNum ; i++ ) {
		if( nColDataNum == 2 ) {
			FillSafeArray( dValue[ i ][ 0 ], i, 0, &saRet_X ) ;
			FillSafeArray( dValue[ i ][ 1 ], i, 0, &saRet ) ;
		}else {
			FillSafeArray( dValue[ i ][ 0 ], i, 0, &saRet ) ;
		}
	}

	if( nColDataNum == 2 ) {
		m_series.SetXValues( COleVariant( saRet_X ) ) ;
	}
	m_series.SetValues( COleVariant( saRet ) ) ;

	saRet_X.Detach() ;
	saRet.Detach() ;

	m_series.DetachDispatch() ;

	return 0 ;
}

long KExcel::GetChart_SeriesNum()
{
	if( m_chart == NULL ) {
		return -1 ;
	}

	m_seriescollection = m_chart.SeriesCollection( covOptional ) ;

	return m_seriescollection.GetCount() ;
}

int KExcel::SetChart_BorderStyle( long lBorderStyle )
{
	if( m_chartobject == NULL ) {
		CString strMsg ;
		AfxMessageBox( _T( "Style을 적용할 Chart가 없습니다." ) ) ;
		return -1 ;
	}

	m_borders = m_chartobject.GetBorder() ;
	m_borders.SetLineStyle(COleVariant( lBorderStyle ) ) ;

	return 0 ;
}

int KExcel::SetChart_Axis_X_MaxScale( double dMax )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)1 ), (long) 1 ) ;
	m_axis.SetMaximumScale( dMax ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_Y_MaxScale( double dMax )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)2 ), (long) 1 ) ;
	m_axis.SetMaximumScale( dMax ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_X_MinScale( double dMin )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)1 ), (long) 1 ) ;
	m_axis.SetMinimumScale( dMin ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_Y_MinScale( double dMin )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)2 ), (long) 1 ) ;
	m_axis.SetMinimumScale( dMin ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_X_MaxScaleAuto( BOOL bAuto )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)1 ), (long) 1 ) ;
	m_axis.SetMaximumScaleIsAuto( bAuto ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_Y_MaxScaleAuto( BOOL bAuto )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)2 ), (long) 1 ) ;
	m_axis.SetMaximumScaleIsAuto( bAuto ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_X_MinScaleAuto( BOOL bAuto )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)1 ), (long) 1 ) ;
	m_axis.SetMinimumScaleIsAuto( bAuto ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_Y_MinScaleAuto( BOOL bAuto )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)2 ), (long) 1 ) ;
	m_axis.SetMinimumScaleIsAuto( bAuto ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_X_DisplayUnit( long xlDisplayUnit )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)1 ), (long) 1 ) ;
	m_axis.SetDisplayUnit( xlDisplayUnit ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_Y_DisplayUnit( long xlDisplayUnit )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)2 ), (long) 1 ) ;
	m_axis.SetDisplayUnit( xlDisplayUnit ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_X_HasMajorGridlines( BOOL bGrid )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)1 ), (long) 1 ) ;
	m_axis.SetHasMajorGridlines( bGrid ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_Y_HasMajorGridlines( BOOL bGrid )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)2 ), (long) 1 ) ;
	m_axis.SetHasMajorGridlines( bGrid ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_X_Label_NumberFormat( int nFormatType )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)1 ), (long) 1 ) ;
	m_thickLabels = m_axis.GetTickLabels() ;
	CString strNumberFomat = GetNumberFormat( nFormatType ) ;
	m_thickLabels.SetNumberFormatLocal( COleVariant( strNumberFomat ) ) ;
	return 0 ;
}

int KExcel::SetChart_Axis_Y_Label_NumberFormat( int nFormatType )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_axis = m_chart.Axes( COleVariant( (short)2 ), (long) 1 ) ;	
	m_thickLabels = m_axis.GetTickLabels() ;
	CString strNumberFomat = GetNumberFormat( nFormatType ) ;
	m_thickLabels.SetNumberFormatLocal( COleVariant( strNumberFomat ) ) ;
	return 0 ;
}

int KExcel::SetChart_plotarea_BkColor( long nColor )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_plotarea = m_chart.GetPlotArea() ;
	m_interior = m_plotarea.GetInterior() ;
	m_interior.SetColor( COleVariant( nColor ) ) ;
	return 0 ;
}

int KExcel::SetChart_plotarea_BorderStyle( long lWeight, long nColor /*= 0*/, long lBorderStyle /*= xlLineStyle_Continuous*/ )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_plotarea = m_chart.GetPlotArea() ;
	m_borders = m_plotarea.GetBorder() ;
	m_borders.SetColor( COleVariant( nColor ) ) ;
	m_borders.SetWeight( COleVariant( lWeight ) ) ;
	m_borders.SetLineStyle( COleVariant( lBorderStyle ) ) ;
	return 0 ;
}

int KExcel::SetChart_plotarea_Pos( double lLeft /*= 0*/, double lTop /*= 0*/, double lWidth /*= 0*/, double lHeight /*= 0*/ )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	m_plotarea = m_chart.GetPlotArea() ;
	double left = m_plotarea.GetLeft() ;
	double top = m_plotarea.GetTop() ;
	double width = m_plotarea.GetWidth() ;
	double height = m_plotarea.GetHeight() ;

	m_plotarea.SetTop( lTop ) ;
	m_plotarea.SetLeft( lLeft ) ;

	if( lHeight == 0 ) {
		m_plotarea.SetHeight( ( height + ( top * 3 ) ) - lTop ) ;
	}else {
		m_plotarea.SetHeight( lHeight ) ;
	}
	if( lWidth == 0 ) {
		m_plotarea.SetWidth( ( width + ( left * 3 ) ) - lLeft ) ;
	}else {
		m_plotarea.SetWidth( lWidth ) ;
	}
	return 0 ;
}

int KExcel::SetChart_Legend_Position( long lPosition )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	Legend m_legend = m_chart.GetLegend() ;
	m_legend.SetPosition( lPosition ) ;

	return 0 ;
}

int KExcel::SetChart_Legend_BorderStyle( long lWeight, long nColor /*= 0*/, long lBorderStyle /*= xlLineStyle_Continuous*/ )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	Legend m_legend = m_chart.GetLegend() ;
	m_borders = m_legend.GetBorder() ;
	m_borders.SetColor( COleVariant( nColor ) ) ;
	m_borders.SetWeight( COleVariant( lWeight ) ) ;
	m_borders.SetLineStyle( COleVariant( lBorderStyle ) ) ;

	return 0 ;
}

int KExcel::SetChart_Legend_Pos( double lLeft, double lTop, double lWidth, double lHeight  )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	Legend m_legend = m_chart.GetLegend() ;
	m_legend.SetLeft( lLeft ) ;
	m_legend.SetTop( lTop ) ;
	m_legend.SetWidth( lWidth ) ;
	m_legend.SetHeight( lHeight ) ;

	return 0 ;
}

int KExcel::GetChart_Legend_Pos( double &lLeft, double &lTop, double &lWidth, double &lHeight )
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	Legend m_legend = m_chart.GetLegend() ;
	lLeft = m_legend.GetLeft() ;
	lTop = m_legend.GetTop() ;
	lWidth = m_legend.GetWidth() ;
	lHeight = m_legend.GetHeight() ;

	return 0 ;
}

int KExcel::SetChart_Legend_Delete()
{
	if( m_chart == NULL ) {
		return -1 ;
	}
	Legend m_legend = m_chart.GetLegend() ;
	m_legend.Delete() ;
	return 0 ;
}

int KExcel::InsertPicture( long lRow, long lCol, CString strFileName )
{
	VARIANT vLeft, vTop, vWidth, vHeight ;
	
	Shapes	m_shapes = m_sheet.GetShapes() ;
	SetRange( lRow, lCol ) ;

	vLeft = m_range.GetLeft() ;
	vTop = m_range.GetTop() ;
	vWidth = m_range.GetWidth() ;
	vHeight = m_range.GetHeight() ;

	LPDISPATCH pDisp = m_shapes.AddPicture( strFileName, TRUE, TRUE,
											(float)vLeft.dblVal+1, (float)vTop.dblVal+1, 
											(float)vWidth.dblVal-2, (float)vHeight.dblVal-2 ) ;

	m_shapes.ReleaseDispatch() ;
	pDisp->Release() ;

	return 0 ;

}

int KExcel::InsertPicture( long lRowStart, long lColStart, long lRowEnd, long lColEnd, CString strFileName )
{
	VARIANT vLeft, vTop, vWidth, vHeight ;
	
	Shapes	m_shapes = m_sheet.GetShapes() ;
	SetRange( lRowStart, lColStart, lRowEnd, lColEnd ) ;

	vLeft = m_range.GetLeft() ;
	vTop = m_range.GetTop() ;
	vWidth = m_range.GetWidth() ;
	vHeight = m_range.GetHeight() ;

	LPDISPATCH pDisp = m_shapes.AddPicture( strFileName, TRUE, TRUE,
											(float)vLeft.dblVal+1, (float)vTop.dblVal+1, 
											(float)vWidth.dblVal-2, (float)vHeight.dblVal-2 ) ;

	m_shapes.ReleaseDispatch() ;
	pDisp->Release() ;

	return 0 ;

}

int KExcel::InsertPicture2( long lRow, long lCol, CString strFileName )
{
	ExcelPicture pic;
	Pictures pics;

	SetRange( lRow, lCol ) ;

	pics = m_sheet.Pictures( covOptional ) ;
	pic = pics.Insert( strFileName, covOptional ) ;
	pic.SetTop( m_range.GetTop().dblVal ) ;
	pic.SetLeft( m_range.GetLeft().dblVal ) ;

	pic.ReleaseDispatch() ;
	pics.ReleaseDispatch() ;

	return 0 ;

}
