// CImgDisplayDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CImgDisplayDlg.h"
#include "afxdialogex.h"
#include "Win32File.h"
#include "BitmapStd.h"
#include "CDefectMapDlg.h"
#include "GlobalData.h"
#include "CDefectInfo.h"
#include "CImageProcess.h"
// CImgDisplayDlg 대화 상자

IMPLEMENT_DYNAMIC(CImgDisplayDlg, CDialogEx)

CImgDisplayDlg::CImgDisplayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DISP_IMAGE, pParent)
{
	m_ThumbCtrl.ClearAll();
	m_ThumbCtrl.SetThumbRows(2);
	m_ThumbCtrl.SetThumbCols(5);

	m_pImgPtr = NULL;
	m_ImgSize.cx = 0 ;
	m_ImgSize.cy = 0 ;

	m_pParent = (CDefectMapDlg*)pParent;
	m_pFontGrid = NULL;
	m_nSelDefNo = -1;
}

CImgDisplayDlg::~CImgDisplayDlg()
{
	if (m_pImgPtr != NULL) {
		delete m_pImgPtr;
		m_pImgPtr = NULL;
	}
	
	if (m_pFontGrid != NULL) {
		delete m_pFontGrid;
		m_pFontGrid = NULL;
	}
}

void CImgDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CROP_THUMB, m_ThumbCtrl);
	DDX_Control(pDX, IDC_GRID_DEFECT_INFO, m_GridDefInfo);

}


BEGIN_MESSAGE_MAP(CImgDisplayDlg, CDialogEx)
	ON_WM_PAINT()
	ON_NOTIFY(NM_CLICK, IDC_CROP_THUMB, OnSelectImage)
	ON_BN_CLICKED(IDOK, &CImgDisplayDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CImgDisplayDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CImgDisplayDlg 메시지 처리기


BOOL CImgDisplayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_DefList.clear();
	// m_DefList

	m_BmpStd.Clear();

	MakeDefInfoGrid();
	UpdateDefInfoGrid(NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CImgDisplayDlg::SetDefectList(CDefectDataCtrl::_VEC_DEFECT_LIST* pList)
{
	if (pList == NULL) {
		return;
	}

	//pList->size();
	CDefectDataCtrl::_VEC_DEFECT_LIST::iterator iter;
	m_ThumbCtrl.ClearAll();
	m_DefList.clear();

	for (iter = pList->begin(); iter != pList->end(); iter++) {
		m_DefList.push_back(*iter);

		CString strFileName;
		strFileName.Format(_T("%s"), iter->szImgFileName);

		int a = 0;
		if (CWin32File::ExistFile(strFileName) == TRUE) {
			m_ThumbCtrl.AddImageBufferMode(strFileName, _T(""));
			m_ThumbCtrl.Invalidate(FALSE);
			m_ThumbCtrl.OnRefresh();

		}
	}

	// 22.12.20 Ahn Add Start
	CString strImgPath;
	iter = pList->begin();
	if (iter != pList->end()) {
		CString strFileName;
		strFileName.Format(_T("%s"), iter->szImgFileName);
		//strImgPath = GetTabPathFormDefPath(strFileName);

		ImageLoad(strFileName);
	}

}

// 22.12.16 Ahn Add Start
CString CImgDisplayDlg::GetTabPathFormDefPath(CString strDefImagePath)
{

	CString strFullPath = strDefImagePath;
	CString strFileName;
	CString strFilePath;

	CString strTabImgPath;
	CString strTabImgFileName;

	//FOIL-EXP_20221215104539389_GMLV_ANODE_FBILT1NC44_7_TOP_NG
	CString strRet = _T("");

	int nPos = strFullPath.ReverseFind('\\');
	int nLength = strFullPath.GetLength();
	int nFileNameLength = nLength - nPos - 1 ;

	strFileName = strFullPath.Right(nFileNameLength);
	strFilePath = strFullPath.Left(nPos);

	CString strTabJudge = _T("");
	CString strDefJudge = _T("");
	CString strDefNo = _T("");
	CString strCamPos = _T("");
	int nFindCnt = 0;
	int nFindPos = 0;
	int nTokenLen = 0;
	int nTokLen = 0;
	int nDefJudgePos = 0;
	BOOL bFind = FALSE;
	//char chTemp;

	CString strTemp;
	for (int i = nFileNameLength - 1 ; i > 0; i--) {
		if (strFileName.GetAt(i) == '_') {
			{
				nFindPos = i ;
				int nCopyLen = nTokLen;
				if (nCopyLen > 0) {
					strTemp = strFileName.Mid(nFindPos + 1, nTokLen);
					switch (nFindCnt) {
					case	1:
						strDefNo = strTemp ;
						break;
					case	2:
						strDefJudge = strTemp;
						nDefJudgePos = i;
						break;
					case	3:
						strTabJudge = strTemp;
						break;
					case	4:
						strCamPos = strTemp ;
						break;
					default:
						break;
					}
				}
				nTokLen = 0 ;
				nFindCnt++ ;
			}
		}
		else {
			nTokLen++;
		}
	}

	strTabImgFileName = strFileName.Left(nDefJudgePos);
	strTabImgFileName += _T(".jpg");
	strTabImgPath = strFilePath;
	strTabImgPath.Replace(_T("\\CROP"), _T(""));

	if( ( strTabJudge.Compare(_T("NG") ) == 0 ) && ( strDefJudge.Compare( _T("OK") ) == 0 ) ) {
		strTabImgPath.Replace(_T("\\OK\\"), _T("\\NG\\")) ;
	}

	strRet = strTabImgPath + _T("\\") + strTabImgFileName;
	return strRet;

}
// 22.12.16 Ahn Add End

void CImgDisplayDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.


	//CRect rcWnd;
	//CRect rcCtrl;
	CWnd* pWnd = NULL;
	pWnd = (CWnd*)GetDlgItem(IDC_ST_ORG_IMAGE);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		//pWnd->GetWindowRect(&rcCtrl);
		//ScreenToClient(&rcCtrl);
		BYTE* pImgPtr;
		CSize imgSize;
		imgSize = m_BmpStd.GetImgSize();
		pImgPtr = m_BmpStd.GetImgPtr();
		DrawImage(pWnd->m_hWnd, pImgPtr, imgSize.cx, imgSize.cy, 8, m_pDefInfo);

	}

	// 23.01.05 Ahn Add Start
	pWnd = (CWnd*)GetDlgItem(IDC_ST_SELECT_IMG);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		BYTE* pImgPtr;
		CSize imgSize;
		imgSize = m_CropBmpStd.GetImgSize();
		pImgPtr = m_CropBmpStd.GetImgPtr();
		DrawImageCrop(pWnd->m_hWnd, pImgPtr, imgSize);

	}
	// 23.01.05 Ahn Add End


}


void CImgDisplayDlg::OnSelectImage(NMHDR* pNMHDR, LRESULT* pResult)
{
	//	int nIndex = (int*)pNMHDR;
	NM_THUMBDATA* pNmgv = (NM_THUMBDATA*)pNMHDR;

	//int nTotalNo = m_CropThumb.GetImageCount();
	//int nSelectNo = pNmgv->nIndex;

	int nTotalNo = m_ThumbCtrl.GetImageCount();
	int nSelectNo = pNmgv->nIndex ;

	int nSize = (int)m_DefList.size();

	if (nSize <= nSelectNo) {
		m_nSelDefNo = -1;
		return ;
	}

	CDefectInfo *pDefInfo;
	pDefInfo = &m_DefList[nSelectNo];

	m_nSelDefNo = nSelectNo;

	// 23.01.10 Ahn Modify Start
	//CString strFilePath; 
	//CString strOrgImgPath;
	//strFilePath.Format(_T("%s"), pDefInfo->szImgFileName);
	//ImageLoad(strFilePath);
	//UpdateDefInfoGrid(pDefInfo);

	//m_pDefInfo = pDefInfo;
	SelectDefectInfo(pDefInfo);
	// 23.01.10 Ahn Modify End

}

// 23.01.10 Ahn Add Start
int CImgDisplayDlg::SelectDefectInfo(CDefectInfo* pDefInfo)
{
	int nRet = 0;
	if (pDefInfo != NULL) {
		CString strFilePath; 
		CString strOrgImgPath;
		strFilePath.Format(_T("%s"), pDefInfo->szImgFileName);

		ImageLoad(strFilePath);
		UpdateDefInfoGrid(pDefInfo);
		m_pDefInfo = pDefInfo;
	}

	return nRet ;
}
// 23.01.10 Ahn Add End

int CImgDisplayDlg::ImageLoad(CString strImgFile)
{
	int nRet = 1; 
	CString strOrgImgPath;

	strOrgImgPath = GetTabPathFormDefPath(strImgFile);

	if (CWin32File::ExistFile(strOrgImgPath) == TRUE) {	
		if (m_BmpStd.ReadBitmap(strOrgImgPath) >= 0) {
			m_BmpStd.ImageReverse();
			nRet = 0 ;
		}
	}
	else {
		nRet = -1;
	}

	if (CWin32File::ExistFile(strImgFile) == TRUE) {
		if (m_CropBmpStd.ReadBitmap(strImgFile) >= 0) {
		//	m_CropBmpStd.ImageRevers();
			nRet = 0;
		}
	}
	else {
		nRet = -1;
	}

	if (nRet == 0) {
		Invalidate(FALSE);
	}

	return 0 ;
}


void CImgDisplayDlg::DrawImageCrop(HWND HWnd, BYTE* pImgPtr, CSize imgSize)
{
	ASSERT(HWnd);
	ASSERT(pImgPtr);

	HDC hdc = ::GetDC(HWnd);

	if (hdc == nullptr) {
		return;
	}
	CDC* pDC = CDC::FromHandle(hdc);
	CWnd* pWnd = CWnd::FromHandle(HWnd);
	CRect rect;
	CRect rcImg;
	CRect rcClient;
	if (pWnd == NULL) {
		::ReleaseDC(HWnd, hdc);
		return;

	}
	if (pDC == NULL) {
		::ReleaseDC(HWnd, hdc);
		return;
	}

	if (pImgPtr == NULL) {
		return;
	}

	pWnd->GetWindowRect(&rect);
	rcClient.left = 0;
	rcClient.right = rect.Width();
	rcClient.top = 0;
	rcClient.bottom = rect.Height();
	CRect rcDisp;

	int nWidth = rcImg.Width();
	int nHeight = rcImg.Height();
	rcImg.top = 0;
	rcImg.bottom = imgSize.cy;
	rcImg.left = 0;
	rcImg.right = imgSize.cx;
	m_CropBmpStd.DrawImageFunction(pDC, &rcClient, &rcImg, SRCCOPY, FALSE);

	::ReleaseDC(HWnd, hdc);
}

void CImgDisplayDlg::DrawImage(HWND HWnd, BYTE* pImgPtr, int nWidth, int nHeight, int nMagnif, CDefectInfo *pDefInfo )
{
	ASSERT(HWnd);
	ASSERT(pImgPtr);

	HDC hdc = ::GetDC(HWnd);

	if (hdc == nullptr) {
		return;
	}
	CDC* pDC = CDC::FromHandle(hdc);
	CWnd* pWnd = CWnd::FromHandle(HWnd);
	CRect rect;
	CRect rcImg;
	CRect rcClient;
	if (pWnd == NULL) {
		::ReleaseDC(HWnd, hdc);
		return;

	}
	if (pDC == NULL) {
		::ReleaseDC(HWnd, hdc);
		return;
	}
	pWnd->GetWindowRect(&rect);
	rcClient.left = 0;
	rcClient.right = rect.Width();
	rcClient.top = 0;
	rcClient.bottom = rect.Height();

	CBitmapStd* pbmpstd;
	BYTE* pResizePtr;

	int nReWidth = (nWidth / nMagnif);
	int nReHeight = (nHeight / nMagnif);
	int nReSize = nReWidth * nReHeight;
	pResizePtr = new BYTE[nReSize + 1];
	memset(pResizePtr, 0x00, sizeof(BYTE) * nReSize + 1);

	rcImg.left = 0;
	rcImg.right = nReWidth;
	rcImg.top = 0;
	rcImg.bottom = nReHeight;

	CImageProcess::ResizeImage(pImgPtr, pResizePtr, nWidth, nHeight, nMagnif);
	pbmpstd = new CBitmapStd(nReWidth, nReHeight, 8);
	pbmpstd->SetImage(nReWidth, nReHeight, pResizePtr);

	pbmpstd->DrawImageFunction(pDC, &rcClient, &rcImg, SRCCOPY, FALSE);

	double dRateX = (double)rect.Width() / (double)(nReWidth * nMagnif);
	double dRateY = (double)rect.Height() / (double)(nReHeight * nMagnif);


	if (m_pDefInfo != NULL) {
		CPoint cpCenter;
		cpCenter = m_pDefInfo->rcPos.CenterPoint();
		COLORREF clrJudge;
		switch (m_pDefInfo->nRank) {
		case	JUDGE_NG :
			clrJudge = RGB(255, 0, 0);
			break;
		default :
			clrJudge = RGB(0, 255, 0);
			break;
		}

		CRect defRect;
		defRect.top = (int)(pDefInfo->rcPos.top * dRateY);
		defRect.bottom = (int)(pDefInfo->rcPos.bottom * dRateY);
		defRect.left = (int)(pDefInfo->rcPos.left * dRateX);
		defRect.right = (int)(pDefInfo->rcPos.right * dRateX);

		int nInfate = 5;
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		DrawDefectRect(pDC, defRect, clrJudge);

	}

	::ReleaseDC(HWnd, hdc);

	delete pbmpstd;
	delete pResizePtr;
}

// 22.12.27 Ahn Add Start
void CImgDisplayDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pParent->CloseImgDispDlg();

	delete this;
	CDialogEx::PostNcDestroy();
}
// 22.12.27 Ahn Add End 

void CImgDisplayDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DestroyWindow();
	//CDialogEx::OnOK();
}


void CImgDisplayDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DestroyWindow();
	//CDialogEx::OnCancel();
}

void CImgDisplayDlg::MakeDefInfoGrid()
{
	CRect rect;
	GetClientRect(rect);
	CGridCtrl* pGridCtrl;
	pGridCtrl = &m_GridDefInfo;

	if (m_pFontGrid == NULL) {
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFont(AprData.m_System.m_nGridFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
			, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
			, FIXED_PITCH, "Arial");
	}

	pGridCtrl->SetFont(m_pFontGrid, TRUE);

	int m_nFixCols = 1;
	int m_nFixRows = 1;
	int m_nCols = 2 ;
	int m_nRows = en_Def_MaxRows ;
	int nRow = 0;
	int nCol = 0;

	TRY{
		pGridCtrl->SetRowCount(m_nRows);
		pGridCtrl->SetColumnCount(m_nCols);
		pGridCtrl->SetFixedRowCount(m_nFixRows);
		pGridCtrl->SetFixedColumnCount(m_nFixCols);
	}
		CATCH(CMemoryException, e) {
		e->ReportError();
		return ;
	}
	END_CATCH

	CString strTitle[en_Def_MaxCols]; ;

	for (int i = 0; i < en_Def_MaxCols; i++) {
		switch (i) {
		case	0:
			strTitle[i] = _LANG(_T("항목"), _T("Item"));
			break;
		case	1:
			strTitle[i] = _LANG(_T("값"), _T("Value"));
			break;
		}
	}

	for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
		CString strText;
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = 0;
		Item.col = nCol;
		Item.strText = strTitle[nCol];

		//폰트 픽셀 넓이 저정
		CDC* dc = GetDC();
		pGridCtrl->setGrideFontWidth(nCol, dc, strTitle[nCol], 2);
		//cd 메모리 해제
		ReleaseDC(dc);

		pGridCtrl->SetItem(&Item);

	}


	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {

		CDefectInfo defInfo;
		CString strText;
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = nRow;
		Item.col = 0;

		CString strTemp;

		switch (nRow) {
		case	en_Def_TabNo:
			strText.Format(_T("TabNo"));
			break;
		case	en_Def_No:
			strText.Format(_T("DefNo"));
			break;
		case	en_Def_Judge :
			strText = _T("Judge");
			break;
		case	en_Def_Size:
			strText.Format(_T("Size"));
			break;
		case	en_Def_Bright_Max:
			strText.Format(_T("Max"));
			break;
		case	en_Def_Bright_Min:
			strText.Format(_T("Min"));
			break;
		case	en_Def_Bright_Ave:
			strText.Format(_T("Ave"));
			break;
		}

		Item.strText = strText;

		pGridCtrl->SetItem(&Item);
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}

	//크기 조정
	pGridCtrl->setGridFullColumnWidthVec();

	pGridCtrl->SetRowHeight(0, 22);
	pGridCtrl->ShowWindow(SW_SHOW);

	return ;
}
void CImgDisplayDlg::UpdateDefInfoGrid(CDefectInfo* pInfo)
{

	int nRet = 0;
	// TODO: 여기에 구현 코드 추가.
	int nRows;

	CGridCtrl* pGrid = &m_GridDefInfo;

	nRows = en_Def_MaxRows;

	if (!::IsWindow(pGrid->m_hWnd)) {
		return ;
	}

	UpdateData();

	int nOldNumRows = pGrid->GetRowCount();

	TRY{
		pGrid->SetRowCount(nRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return ;
	}
	END_CATCH

	UpdateData(FALSE);

	COLORREF clrBkFoil = RGB(170, 200, 170);
	COLORREF clrBkDross = RGB(200, 200, 200);

	if (pInfo == NULL) {
		return;
	}

	CString strJudge;
	switch ( pInfo->nRank ) {
	case	JUDGE_OK :
	case	JUDGE_GRAY:
		strJudge = _T("OK");
		break;
	case	JUDGE_NG :
		strJudge = _T("NG");
		break;

	}

	for (int nRow = 1; nRow < nRows; nRow++) {
		CString strText;
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = nRow;
		Item.col = 1;

		// 23.02.08 Ahn Add Start
		int nBrightMax;
		int nBrightMin;
		int nBrightAve;
		if (pInfo->nType == en_defPosSurface) {
			nBrightMax = pInfo->nMaxBright;
			nBrightMin = pInfo->nMinBright;
			nBrightAve = pInfo->nAvgBright;
		}
		else {
			nBrightMax = pInfo->nMaxOrgBir;
			nBrightMin = pInfo->nMinOrgBir;
			nBrightAve = pInfo->nAveOrgBir;
		}
		// 23.02.08 Ahn Add End

		switch (nRow) {
		case	en_Def_TabNo:
			strText.Format(_T("%d"), pInfo->nTabNo+1 );
			break;
		case	en_Def_No:
			strText.Format(_T("%d"), pInfo->nDefNo );
			break;
		case	en_Def_Judge :
			strText.Format(_T("%s"), strJudge);
			break;
		case	en_Def_Size:
			strText.Format(_T("%.1lf"), pInfo->dJudgeSize );
			break;
		case	en_Def_Bright_Max:
			// 23.02.08 Ahn Add Start
			//strText.Format(_T("%d"), pInfo->nMaxOrgBir );
			strText.Format(_T("%d"), nBrightMax);
			// 23.02.08 Ahn Add End
			break;
		case	en_Def_Bright_Min:
			// 23.02.08 Ahn Add Start
			//strText.Format(_T("%d"), pInfo->nMinOrgBir );
			strText.Format(_T("%d"), nBrightMin);
			// 23.02.08 Ahn Add End
			break;
		case	en_Def_Bright_Ave:
			// 23.02.08 Ahn Add Start
			//strText.Format(_T("%d"), pInfo->nAveOrgBir );
			strText.Format(_T("%d"), nBrightAve);
			// 23.02.08 Ahn Add End
			break;
		}

		Item.strText = strText;
		pGrid->SetItem(&Item);
	}
	pGrid->Refresh();

	return ;
}

// 23.01.05 Ahn Add Start
void CImgDisplayDlg::DrawDefectRect(CDC* pDC, CRect rcDef, COLORREF color)
{
	if (pDC == NULL) {
		return;
	}

	CRect	rc;
	CBrush	myBrush, * poldBrush;
	myBrush.CreateSolidBrush(color);
	poldBrush = pDC->SelectObject(&myBrush);

	pDC->FrameRect(rcDef, &myBrush);

	pDC->SelectObject(poldBrush);
	myBrush.DeleteObject();
}
// 23.01.05 Ahn Add End