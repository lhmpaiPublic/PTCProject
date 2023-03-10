#include "pch.h"
#include "CExcelResultFileCtrl.h"
#include "Win32File.h"
#include "CExcelFileSaveDlg.h"

CExcelResultFileCtrl::CExcelResultFileCtrl()
{
	m_pThread = NULL;
	m_bComplet = FALSE;
}
CExcelResultFileCtrl::~CExcelResultFileCtrl()
{
//	m_Excel.Close();
	if (m_pThread != NULL) {
		delete m_pThread;
		m_pThread = NULL;
	}
}

int CExcelResultFileCtrl::InitExcelFile(KExcel* pExcel)
{
	if (pExcel->Init() < 0) {
		return -1;
	}
	if (pExcel->Open() < 0) {
		return -2;
	}

	return 0;
}

int CExcelResultFileCtrl::SaveTitleLine( KExcel *pExcel )
{
	if (pExcel == NULL) return -1;

	CString strDisc;
	int nWidth;
	for (long i = 0; i < en_MaxCols; i++ ){
		switch (i) {
		case	en_colLotID :
			strDisc = _T("Lot ID");
			nWidth = 15;
			break;
		case	en_colTotalCount:
			strDisc = _T("Total Count");
			nWidth = 10;
			break;
		case	en_colCellNo:
			strDisc = _T("Cell No");
			nWidth = 10;
			break;
		case	en_colInspTime:
			strDisc = _T("검사시간");
			nWidth = 25;
			break;
		case	en_colPosition:
			strDisc = _T("Position");
			nWidth = 15;
			break;
		case	en_colJudge:
			strDisc = _T("Judge");
			nWidth = 15;
			break;
		case	en_colSurface: 
			strDisc = _T("Surface");
			nWidth = 15;
			break;
		case	en_colExposure:
			strDisc = _T("FoilExp");
			nWidth = 15;
			break;
		case	en_colDefR :
#if defined(_DEBUG)
			strDisc = _T("Pos X(Pix)");
#else
			strDisc = _T("Defect R");
#endif
			nWidth = 15;
			break;
		case	en_colDefTheta:
#if defined(_DEBUG)
			strDisc = _T("Pos Y(Pix)");
#else
			strDisc = _T("Defect θ");
#endif
			nWidth = 15;
			break;
		case	en_colDefSizeX:
			strDisc = _T("Defect Size X(㎛)");
			nWidth = 15;
			break;
		case	en_colDefSizeY:
			strDisc = _T("Defect Size Y(㎛)");
			nWidth = 15;
			break;
		case	en_colDefImage:
			strDisc = _T("Defect Image");
			nWidth = 15;
			break;
		case	en_colInkMarking:
			strDisc = _T("Ink Marking");
			nWidth = 10;
			break;
		case	en_colReason:
			strDisc = _T("Ink Marking Reason");
			nWidth = 20;
			break;
		default :
			strDisc = _T("");
			break;
		}
		pExcel->SetValue(1, i + 1, strDisc );
		pExcel->SetColWidth(i + 1, nWidth);
	}
	pExcel->SetBkColor(1, 1, 1, en_MaxCols, RGB(50, 250, 250));
	pExcel->SetLineWeight(xlThin);
	pExcel->SetLineStyle(xlLineStyle_Continuous);
	pExcel->SetFreezePanes(2, 1, TRUE); // 틀 고정
	pExcel->SetRange(1, 1, 2, en_MaxCols);
	pExcel->SetFont(_T("Gulim"), 11);
	pExcel->SetTextAlign(xlAlignHCenter, xlAlignVCenter);

	return 0;
}

int CExcelResultFileCtrl::MakeDummyData()
{
	return 0;
}

BOOL CExcelResultFileCtrl::IsImageFileAvailable(CString strImgFile)
{
	if (strImgFile.GetLength() <= 0) {
		return FALSE ; // 22.06.29 Ahn Modify
	}

	if (CWin32File::GetFileSize(strImgFile) <= 7) {
		return FALSE;
	}

	if (CWin32File::ExistFile(strImgFile) == FALSE) {
		return FALSE ; // 22.06.29 Ahn Modify
	}


	//CString strExist = strImgFile.Right(4);
	//if( strExist.Compare( _T(".BMP") ) == 0 ){
	//}
	//else if (strExist.Compare(_T(".JPG")) == 0 ){
	//}

	return TRUE;
}

int CExcelResultFileCtrl::SaveExcelOneLine(KExcel* pExcel, int nRow, int nTotalCell, CDefectSearchList defInfo )
{
	if (pExcel == NULL) return -1;

	pExcel->SetValue(nRow, en_colLotID + 1, defInfo.strLotID);
	pExcel->SetValue(nRow, en_colTotalCount + 1, nTotalCell);
	pExcel->SetValue(nRow, en_colCellNo + 1, defInfo.nCell + 1 );
	CString strTime;
	strTime.Format(_T("%04d-%02d-%02d_%02d:%02d:%02d.%03d")
		, defInfo.tLotStartTime.wYear, defInfo.tLotStartTime.wMonth, defInfo.tLotStartTime.wDay
		, defInfo.tLotStartTime.wHour, defInfo.tLotStartTime.wMinute, defInfo.tLotStartTime.wSecond, defInfo.tLotStartTime.wMilliseconds);
	pExcel->SetValue(nRow, en_colInspTime + 1, strTime);

	pExcel->SetValue(nRow, en_colPosition + 1, ((defInfo.nTopBottom == 0) ? _T("TOP") : _T("BOTTOM")));
	// 22.11.21 Ahn Modify Start - JUDGE_GRAY
	//pExcel->SetValue(nRow, en_colJudge + 1, (defInfo.nRank == 0 ? _T("OK") : _T("NG")));
	pExcel->SetValue(nRow, en_colJudge + 1, (defInfo.nRank == JUDGE_NG ? _T("NG") : _T("OK")));
	// 22.11.21 Ahn Modify End
	pExcel->SetValue(nRow, en_colSurface + 1, (defInfo.nType == TYPE_SURFACE ? _T("O") : _T("X")));
	pExcel->SetValue(nRow, en_colExposure + 1, (defInfo.nType == TYPE_FOILEXP ? _T("O") : _T("X")));
	pExcel->SetValue(nRow, en_colDefR + 1		, defInfo.dDefRadius, 0 );
	pExcel->SetValue(nRow, en_colDefTheta + 1	, defInfo.dDefTheta, 0 );
	pExcel->SetValue(nRow, en_colDefSizeX + 1, defInfo.dXSize, 2);
	pExcel->SetValue(nRow, en_colDefSizeY + 1, defInfo.dYSize, 2);

	//strImgFileName.Format(_T("%s\\%s"), strImgPath. strFileName );
	if (IsImageFileAvailable(defInfo.strImageFileName) == TRUE) {
		pExcel->InsertPicture(nRow, en_colDefImage + 1, defInfo.strImageFileName);
	}
	else {
		pExcel->SetValue(nRow, en_colDefImage + 1, _T("No Image"));
	}

	pExcel->SetValue(nRow, en_colInkMarking + 1, (defInfo.bMarking == TRUE) ? _T("○") : _T("Χ"));
	//	m_Excel.SetValue(nRow, en_colReason + 1	, iter->strMarkReason );
	//if (defInfo.strImageFileName.GetLength() > 3) {
	//	pExcel->InsertPicture(nRow, en_colDefImage + 1, (defInfo.strImageFileName));
	//}

	return 0;
}


void CExcelResultFileCtrl::Begin() // nMode  0 : Image Merge Mode , 1 : Image Proc Mode 
{
	m_bKill = FALSE;
	m_bComplet = FALSE;

	//	m_DisphWnd = NULL ;
	if (m_pThread == NULL) {
		m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlExcelFileSaveThread,
			(LPVOID)this,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);
		if (m_pThread != NULL) {
			m_pThread->m_bAutoDelete = FALSE;
			m_pThread->ResumeThread();
		}
	}
	else {
		AfxMessageBox( _T("Excel thread is not null !!") );
	}
}

void CExcelResultFileCtrl::Kill(void)
{
	DWORD	dwCode;
	LONG	ret;

	if (m_pThread != NULL) {
		ret = ::GetExitCodeThread(m_pThread->m_hThread, &dwCode);
		if (ret && dwCode == STILL_ACTIVE) {
			m_bKill = TRUE;
			WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		}
		delete m_pThread;
		m_pThread = NULL;
	}

}

int CExcelResultFileCtrl::MakeDefectListSheet(KExcel* pExcel, VEC_DEF_SEARCH_LIST* pList, CExcelResultFileCtrl* pThis)
{
	int nRet = 0; 

	VEC_DEF_SEARCH_LIST::iterator iter;

	pExcel->SetSheetName(1, _T("검사결과"));

	if (SaveTitleLine(pExcel) < 0) {
		AfxMessageBox(_T("Excel 파일 저장 실패"));
		return -1 ;
	}

	int nCount = (int)pList->size();
	pExcel->SetRowHeight(1, 30);
	pExcel->SetRowHeight(2, nCount + 1, 60);

	int i = 0;

	CString strData;
	pExcel->SetRange(2, 1, nCount + 1, en_MaxCols);
	pExcel->SetFont(_T("Gulim"), 12);
	pExcel->SetLineWeight(xlThin);
	pExcel->SetTextAlign(xlAlignHCenter, xlAlignVCenter);
	CString strImgFileName;

	int nRow;
	for (nRow = 2, iter = pList->begin(); iter != pList->end(); nRow++, iter++) {
		if (pThis->m_bKill == TRUE) {
			break;
		}

		pThis->SaveExcelOneLine(pExcel, nRow, 0, *(*iter));
		pThis->m_nSaveCount++;
	}

	return nRet;
}
int CExcelResultFileCtrl::MakeChartSheet(KExcel* pExcel, VEC_DEF_SEARCH_LIST* pList, CExcelResultFileCtrl* pThis)
{
	int nRet = 0;

	ASSERT(pExcel);
	ASSERT(pList);
	ASSERT(pThis);
	pExcel->SetActiveSheet(1);
	pExcel->SetSheetName(1, _T("차트"));

	pExcel->SetColWidth(1, 50, 1);
	pExcel->SetRowHeight(1, 7);
	pExcel->SetFont(_T("Gulim"), 8);
	//pExcel->SetRowHeight( 2, )

	pExcel->CreateChart(34, 1, 44, 27, xl3DColumn, xlColumns, TRUE, _T(""), _T(""), _T("") );

	VEC_DEF_SEARCH_LIST::iterator iter;

	int nRow;
	for (nRow = 2, iter = pList->begin(); iter != pList->end(); nRow++, iter++) {
		if (pThis->m_bKill == TRUE) {
			break;
		}

		int nCellNo = (*iter)->nCell;
		int nDefMode = (*iter)->nType;
		double dSize = (*iter)->dXSize;

		
	}

	return nRet;
}


UINT CExcelResultFileCtrl::CtrlExcelFileSaveThread(LPVOID Param)
{
	CExcelResultFileCtrl* pThis = (CExcelResultFileCtrl*)Param ;

	//while (1) 
	if (pThis != NULL) {
		VEC_DEF_SEARCH_LIST* pList = pThis->pVecDefSearchList;

		ASSERT(pList);
		
		CString strSaveFile;
		strSaveFile = pThis->GetFileName();

		CWin32File::CreateDirectory(strSaveFile, TRUE ); // 22.06.13 Ahn Add Start

		KExcel Excel;
		pThis->m_nSaveCount = 0 ;

		CoInitialize(NULL);

		if (InitExcelFile(&Excel) < 0) {
			AfxMessageBox(_T("초기화 실패"));
			pThis->m_bComplet = TRUE;
			AfxEndThread(0);
			pThis->m_bKill = TRUE;
		}

		if (MakeDefectListSheet(&Excel, pList, pThis) < 0) {
			AfxMessageBox(_T("검사이력 저장 실패"));
			pThis->m_bComplet = TRUE;
			AfxEndThread(0);
			pThis->m_bKill = TRUE;
		}

		Excel.SaveAs(strSaveFile);

		Excel.Close();

		CoUninitialize();

	}

	pThis->m_bComplet = TRUE;
	pThis->m_bKill = TRUE ;

	AfxEndThread(0);

	return 0;
}
