// HistoryDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "HistoryDlg.h"
#include "afxdialogex.h"
#include "CImageProcSimDlg.h"
#include "GlobalData.h"
//#include "CDefectDataCtrl.h"
//KANG 22.01.07 Add Start
#include "Win32File.h"
//KANG 22.01.27 Add End
//KANG 22.05.24 Add Start
#include "MainFrm.h"
//KANG 22.05.24 Add End
// 22.05.27 Ahn Add Start
//#include "CExcelResultFileCtrl.h"
#include "CExcelFileSaveDlg.h"
// 22.05.27 Ahn Add End

// CHistoryDlg 대화 상자

IMPLEMENT_DYNAMIC(CHistoryDlg, CDialogEx)

CHistoryDlg::CHistoryDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_HISTORY, pParent)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = pView->GetDocument();
	}
	m_pImageSimDlg = NULL;
	// 22.02.11 Ahn Add Start
	//m_pDefData = m_pDoc->GetErrorDataCtrl() ;
	m_pDefData = NULL;
	// 22.02.11 Ahn Add End

//KANG 22.01.07 Add Start
	m_nCalcViewType = 0;
	m_nCurMonth = 0;
	for (int i = 0; i < 4; i++)
	{
		m_strImageFilePath[i] = _T("");
		m_nImageFileCount[i] = 0;
	}
	m_fontHeader.CreatePointFont(200, _T("Gulim"));
	m_nImageType = 0;
//KANG 22.05.24 Modify Start
//	m_nDispMode = 0;
	m_nDispMode = enHistoryDisp_List;
//KANG 22.05.24 Modify End
//KANG 22.01.07 Add End
//KANG 22.05.24 Add Start
	m_pSearchList = NULL;
	m_pFontSearchList = NULL;
	m_nSearchMethod = 0;
	m_pVtList = NULL;
	SearchListClear();
	m_nSelectRow = -1;
	m_pImageViewerDlg = NULL;
//KANG 22.05.24 Add End

	// 22.05.27 Ahn Add start
	m_pExcelDlg = NULL;
	// 22.05.27 Ahn Add End

//KANG 22.05.27 Add Start
	m_strSearchLotID = _T("");
//KANG 22.05.27 Add End
//KANG 22.06.17 Add Start
	m_pGraphHist = NULL;
	m_bShowGraphHist = FALSE;
	m_nGraphHistHorSize = 5;
	m_nGraphHistVerSize = 10;
//KANG 22.06.17 Add End
}

CHistoryDlg::~CHistoryDlg()
{
	if (m_pImageSimDlg != NULL) {
		delete m_pImageSimDlg;
		m_pImageSimDlg = NULL;
	}

	// 22.02.11 Ahn Add Start
	if (m_pDefData != NULL) {
		delete m_pDefData; 
		m_pDefData = NULL;
	}
	// 22.02.11 Ahn Add End
//KANG 22.05.24 Add Start
	if (m_pSearchList != nullptr) {
		delete m_pSearchList;
		m_pSearchList = NULL;
	}
	if (m_pFontSearchList != nullptr) {
		m_pFontSearchList->DeleteObject();
		delete m_pFontSearchList;
		m_pFontSearchList = NULL;
	}
	FreeSearchList(m_pVtList);
	if (m_pImageViewerDlg != NULL) {
		delete m_pImageViewerDlg;
		m_pImageViewerDlg = NULL;
	}
//KANG 22.05.24 Add End
	// 22.05.27 Ahn Add start
	if (m_pExcelDlg != NULL) {
		delete m_pExcelDlg;
		m_pExcelDlg = NULL;
	}
	// 22.05.27 Ahn Add End

//KANG 22.06.17 Add Start
	if (m_pGraphHist != NULL) {
		delete m_pGraphHist;
		m_pGraphHist = NULL;
	}
//KANG 22.06.17 Add End
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//KANG 22.01.07 Add Start
	DDX_Control(pDX, IDC_CALCTRL, m_CalCtrl);
	DDX_Radio(pDX, IDC_RD_MONTH, m_nCalcViewType);
	DDX_Control(pDX, IDC_THUMBCTRL, m_ThumbnailCtrl);
	//KANG 22.01.07 Add End
	DDX_Control(pDX, IDC_ST_HEADER, m_stHeader);
	DDX_Radio(pDX, IDC_RD_NG_CROP, m_nImageType);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_dtPickStart);	//KANG 22.05.24 Add
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dtPickEnd);		//KANG 22.05.24 Add
	DDX_Control(pDX, IDC_CMB_TIME_START, m_cmbTimeStart);		//KANG 22.05.24 Add
	DDX_Control(pDX, IDC_CMB_TIME_END, m_cmbTimeEnd);			//KANG 22.05.24 Add
	DDX_Radio(pDX, IDC_RD_LOTID, m_nSearchMethod);				//KANG 22.05.24 Add
	DDX_Text(pDX, IDC_ED_SERCH_LOTID, m_strSearchLotID);		//KANG 22.05.27 Add
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_PREV, &CHistoryDlg::OnBnClickedBtnPrev)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_NEXT, &CHistoryDlg::OnBnClickedBtnNext)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_TODAY, &CHistoryDlg::OnBnClickedBtnToday)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_MONTH, &CHistoryDlg::OnBnClickedRdMonth)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_WEEK, &CHistoryDlg::OnBnClickedRdWeek)			//KANG 22.01.07 Add
	ON_NOTIFY(NM_DBLCLK, IDC_CALCTRL, OnCellDblClick)					//KANG 22.01.07 Add
	ON_NOTIFY(NM_CLICK, IDC_CALCTRL, OnCellClick)						//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CHistoryDlg::OnBnClickedBtnRefresh)	//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_NG_CROP, &CHistoryDlg::OnBnClickedRdNgCrop)	//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_NG, &CHistoryDlg::OnBnClickedRdNg)				//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_OK_CROP, &CHistoryDlg::OnBnClickedRdOkCrop)	//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_OK, &CHistoryDlg::OnBnClickedRdOk)				//KANG 22.01.07 Add
	ON_NOTIFY(WM_USER_DEF1, IDC_THUMBCTRL, OnNotifySimulation)			//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_PREV, &CHistoryDlg::OnBnClickedBtnPrev)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_NEXT, &CHistoryDlg::OnBnClickedBtnNext)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_TODAY, &CHistoryDlg::OnBnClickedBtnToday)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_MONTH, &CHistoryDlg::OnBnClickedRdMonth)		//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_WEEK, &CHistoryDlg::OnBnClickedRdWeek)			//KANG 22.01.07 Add
	ON_NOTIFY(NM_DBLCLK, IDC_CALCTRL, OnCellDblClick)					//KANG 22.01.07 Add
	ON_NOTIFY(NM_CLICK, IDC_CALCTRL, OnCellClick)						//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CHistoryDlg::OnBnClickedBtnRefresh)	//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_NG_CROP, &CHistoryDlg::OnBnClickedRdNgCrop)	//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_NG, &CHistoryDlg::OnBnClickedRdNg)				//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_OK_CROP, &CHistoryDlg::OnBnClickedRdOkCrop)	//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_RD_OK, &CHistoryDlg::OnBnClickedRdOk)				//KANG 22.01.07 Add
	ON_NOTIFY(WM_USER_DEF1, IDC_THUMBCTRL, OnNotifySimulation)			//KANG 22.01.07 Add
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CHistoryDlg::OnBnClickedBtnSearch)	//KANG 22.05.24 Add
	ON_BN_CLICKED(IDC_RD_LOTID, &CHistoryDlg::OnBnClickedRdLotid)		//KANG 22.05.24 Add
	ON_BN_CLICKED(IDC_RD_TIME, &CHistoryDlg::OnBnClickedRdTime)			//KANG 22.05.24 Add
	ON_BN_CLICKED(IDC_BTN_EXPORT, &CHistoryDlg::OnBnClickedBtnExport)	//KANG 22.05.24 Add
	ON_NOTIFY(NM_CLICK, IDC_SEARCH_LIST, OnMouseLDownSearchList)		//KANG 22.05.24 Add
	ON_NOTIFY(NM_DBLCLK, IDC_SEARCH_LIST, OnMouseDblClickSearchList)	//KANG 22.05.24 Add
	ON_NOTIFY(GVN_SELCHANGING, IDC_SEARCH_LIST, OnCellChangingSearchList)	//KANG 22.06.17 Add
	ON_CBN_SELCHANGE(IDC_CMB_TIME_START, &CHistoryDlg::OnCbnSelchangeCmbTimeStart)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CHistoryDlg::OnBnClickedBtnClear)
END_MESSAGE_MAP()


// CHistoryDlg 메시지 처리기


void CHistoryDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//KANG 22.01.07 Add Start
	CRect rectControl, rectParent;
	CButton* pButton = NULL;
	int nWidth = 0, nHeight = 0;
	int nXPos = 0, nYPos = 0;
	int nLeftMargin = 10, nRightMargin = 10, nTopMargin = 9, nBottomMargin = 10;
	this->GetClientRect(rectParent);

	pButton = (CButton*)GetDlgItem(IDC_ST_HEADER);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = ((cx - nLeftMargin - nRightMargin) / 2) - (nWidth / 2);
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 7), nWidth, nHeight);
	}

	nXPos = nLeftMargin;
	nYPos = 0;
	pButton = (CButton*)GetDlgItem(IDC_ST_TITLE);
	if (pButton != nullptr) {
	//KANG 22.05.24 Modify Start
	//	pButton->GetWindowRect(rectControl);
	//	nWidth = cx - nLeftMargin - nRightMargin;
	//	nHeight = rectControl.Height();
	//	pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		if (m_nDispMode != enHistoryDisp_List) {
			pButton->GetWindowRect(rectControl);
			nWidth = cx - nLeftMargin - nRightMargin;
			nHeight = (int)((cy * 5.0) / 100.0);
			pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		}
	//KANG 22.05.24 Modify End
	}

	pButton = (CButton*)GetDlgItem(IDC_BTN_PREV);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos += 3;
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
		nXPos += nWidth;
	}

	pButton = (CButton*)GetDlgItem(IDC_BTN_TODAY);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos += 3;
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
		nXPos += nWidth;
	}

	pButton = (CButton*)GetDlgItem(IDC_BTN_NEXT);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos += 3;
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
	}

	nXPos = cx - nRightMargin;
	pButton = (CButton*)GetDlgItem(IDC_BTN_REFRESH);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos -= (nWidth + 3);
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
	}

	pButton = (CButton*)GetDlgItem(IDC_RD_WEEK);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos -= (nWidth + 3);
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
	}

	pButton = (CButton*)GetDlgItem(IDC_RD_MONTH);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos -= (nWidth + 3);
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
	}

	if (m_CalCtrl.GetSafeHwnd())
	{
		nYPos += (nHeight + 5);
		m_CalCtrl.SetWindowPos(NULL, nLeftMargin, nYPos, (cx - nLeftMargin - nRightMargin), (cy - nYPos - nBottomMargin), SWP_NOZORDER);
	}

	if (m_ThumbnailCtrl.GetSafeHwnd())
	{
		nYPos += 0;
		m_ThumbnailCtrl.SetWindowPos(NULL, nLeftMargin, nYPos, (cx - nLeftMargin - nRightMargin), (cy - nYPos - nBottomMargin), SWP_NOZORDER);
	}

	nXPos = nLeftMargin;
	nYPos = 0;
	pButton = (CButton*)GetDlgItem(IDC_RD_NG_CROP);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos += 3;
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
		nXPos += nWidth;
	}

	pButton = (CButton*)GetDlgItem(IDC_RD_NG);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos += 3;
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
		nXPos += nWidth;
	}

	pButton = (CButton*)GetDlgItem(IDC_RD_OK_CROP);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos += 3;
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
		nXPos += nWidth;
	}

	pButton = (CButton*)GetDlgItem(IDC_RD_OK);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nXPos += 3;
		pButton->MoveWindow(nXPos, nYPos + (nTopMargin + 3), nWidth, nHeight - (nTopMargin + (3 * 2)));
	}
//KNAG 22.01.07 Add End
//KANG 22.05.24 Add Start
	nXPos = 0;
	nYPos = 0;
	pButton = (CButton*)GetDlgItem(IDC_ST_SERCH);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(20, 0, nWidth, nHeight);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_LOTID);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nXPos = (int)((cx * 1.0) / 100.0);
		nYPos = (int)((cy * 3.5) / 100.0);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_TIME);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nXPos = (int)((cx * 1.0) / 100.0);
		nYPos = (int)((cy * 7.2) / 100.0);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_START);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nXPos = (int)((cx * 10.0) / 100.0);
		nYPos = (int)((cy * 7.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		nXPos += nWidth + 5;
	}
	pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_START);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nYPos = (int)((cy * 7.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		nXPos += nWidth + 5;
	}
	pButton = (CButton*)GetDlgItem(IDC_ST_TILDE);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nYPos = (int)((cy * 8.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		nXPos += nWidth + 5;
	}
	pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_END);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nYPos = (int)((cy * 7.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		nXPos += nWidth + 5;
	}
	int nXPos2 = 0;
	int nYPos2 = 0;
	pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_END);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nYPos = (int)((cy * 7.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		nXPos2 = nXPos + nWidth;
		nYPos2 = nYPos + nHeight;
	}
	pButton = (CButton*)GetDlgItem(IDC_ED_SERCH_LOTID);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nXPos = (int)((cx * 10.0) / 100.0);
		nYPos = (int)((cy * 3.0) / 100.0);
		nWidth = nXPos2 - nXPos;
		nHeight = rectControl.Height();
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		nXPos += nWidth + 10;
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_SEARCH);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nYPos = (int)((cy * 3.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = nYPos2 - nYPos;
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		nXPos += nWidth + 5;
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_EXPORT);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nYPos = (int)((cy * 3.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = nYPos2 - (int)((cy * 3.0) / 100.0);
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		// 22.09.15 Ahn Add Start
		nXPos += nWidth + 5;
		// 22.09.15 Ahn Add End
	}
	// 22.09.15 Ahn Add Start
	pButton = (CButton*)GetDlgItem(IDC_BTN_CLEAR);
	if (pButton != nullptr) {
		pButton->GetWindowRect(rectControl);
		nYPos = (int)((cy * 3.0) / 100.0);
		nWidth = rectControl.Width();
		nHeight = nYPos2 - (int)((cy * 3.0) / 100.0);
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	// 22.09.15 Ahn Add End

	pButton = (CButton*)GetDlgItem(IDC_ST_TITLE);
	if (pButton != nullptr) {
		if (m_nDispMode == enHistoryDisp_List) {
			pButton->GetWindowRect(rectControl);
			nXPos = nLeftMargin;
			nYPos = 0;
			nWidth = cx - nLeftMargin - nRightMargin;
			nHeight = nYPos2 + 20;
			pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
			nYPos += nHeight + 20;
		}
	}
	if (m_pSearchList->GetSafeHwnd()) {
	//KANG 22.06.17 Modify Start
	//	m_pSearchList->SetWindowPos(NULL, nXPos, nYPos, cx - nLeftMargin - nRightMargin, cy - nYPos - 10, SWP_NOZORDER);
		nWidth = cx - nLeftMargin - nRightMargin;
		if (m_bShowGraphHist == TRUE) {
			nHeight = cy - nYPos - (int)((cy * 25.0) / 100.0);
		}
		else {
			nHeight = cy - nYPos - 10;
		}
		m_pSearchList->SetWindowPos(NULL, nXPos, nYPos, nWidth, nHeight, SWP_NOZORDER);
		nYPos += nHeight + 10;

		if (m_pGraphHist != nullptr) {
			m_pGraphHist->MoveWindow(nXPos, nYPos, nWidth, cy - nYPos - 10);
		}
	//KANG 22.06.17 Modify End
	}
//KANG 22.05.24 Add End
}


BOOL CHistoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
//KANG 22.01.07 Add Start
	CBitmap bmpImgSm;
	m_ImgList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 1);
	bmpImgSm.LoadBitmap(IDB_BMPREMINDER);
	m_ImgList.Add(&bmpImgSm, RGB(0, 255, 0));
	m_CalCtrl.SetImageList(&m_ImgList);
	m_CalCtrl.SetCurrentDate(COleDateTime::GetCurrentTime());

	COleDateTime OleDataTime;
	OleDataTime = m_CalCtrl.GetCurrentDate();
	m_nCurMonth = OleDataTime.GetMonth();

	SetTextHeader(OleDataTime);
	SetupItems();

	m_stHeader.SetFont(&m_fontHeader);
	m_stHeader.SetTextColor(RGB(64, 64, 64));
	m_stHeader.SetBackgroundColor(RGB(220, 220, 255));

	m_ThumbnailCtrl.SetContextMenuNum(1);
	m_ThumbnailCtrl.SetContextMenuString(_T("시뮬레이션"), 0);

//KANG 22.05.24 Add Start
	if (m_pSearchList == NULL) {
		m_pSearchList = new CGridCtrl();
	}
	MakeGridSearchList();
	int nHour = 0;
	CString strHour;
	for (int nHour = 0; nHour < 24; nHour++) {
		strHour.Format(_T("%02d"), nHour);
		m_cmbTimeStart.AddString(strHour);
		m_cmbTimeEnd.AddString(strHour);
	}
	m_cmbTimeStart.SetCurSel(0);
	m_cmbTimeEnd.SetCurSel(0);
//KANG 22.05.24 Add End

//KANG 22.06.17 Add Start
	if (m_pGraphHist == NULL) {
		m_pGraphHist = new CGraphCtrl(MAX_DEFECT_IN_TAB_NUM * 2);
		CRect rect = CRect(0, 0, 0, 0);
		if (m_pGraphHist->Create(rect, this, 103) == FALSE) {
			delete m_pGraphHist;
			m_pGraphHist = NULL;
			return -1;
		}
	}
	m_pGraphHist->SetImageSize(m_nGraphHistHorSize);
	m_pGraphHist->SetGraphSize(m_nGraphHistHorSize);
	m_pGraphHist->SetVisibleHScollbar(FALSE);
	m_pGraphHist->SetBkColor(RGB(255, 255, 255));
	m_pGraphHist->DrawDataLine(FALSE);
	m_pGraphHist->DrawDataPoint(TRUE);
	m_pGraphHist->DrawPointZeroData(FALSE);
	for (int i = 0; i < MAX_DEFECT_IN_TAB_NUM; i++) {
		m_pGraphHist->SetDataColor(i, RGB(128, 128, 255 - (i * 5)));
		m_pGraphHist->SetDataColor(i + MAX_DEFECT_IN_TAB_NUM, RGB(255 - (i * 5), 128, 128));
		m_pGraphHist->SetDataPointShape(i, CGraphCtrl::en_Shape_Rectangle);
		m_pGraphHist->SetDataPointShape(i + MAX_DEFECT_IN_TAB_NUM, CGraphCtrl::en_Shape_Ellipse);
	}
	m_pGraphHist->SetMaxVerScaleSize(m_nGraphHistVerSize);
	m_pGraphHist->SetVerScaleLableSpace(40);
	m_pGraphHist->UseVerScaleLabel(TRUE);
//KANG 22.06.17 Add End

//KANG 22.05.24 Modify Start
//	OnRefresh();
	OnRefresh(m_nDispMode);
	if (m_pDoc != NULL) {
		m_pDoc->SetHistoryDispMode(m_nDispMode);
	}
//KANG 22.05.24 Modify End
//KANG 22.01.07 Add End

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CHistoryDlg::SetDefectCtrlPtr(CDefectDataCtrl* pCtrl)
{
	m_pDefData = pCtrl;
}

BOOL CHistoryDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//KANG 22.05.24 Add Start
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
	//KANG 22.06.17 Modify Start
	//	case VK_ESCAPE:
	//	case VK_RETURN:
	//		return TRUE;
		case VK_ESCAPE:
			return TRUE;
		case VK_RETURN:
		{
			if (GetFocus() == m_pSearchList) {
				CDefectSearchList* pList = NULL;
				if (m_pImageViewerDlg == NULL) {
					m_pImageViewerDlg = new CImageViewerDlg();
					m_pImageViewerDlg->Create(IDD_DLG_IMAGE_VIEWER, this);

					CRect rcTmp, rcPos;
					int nWidth, nHeight;
					m_pSearchList->GetWindowRect(rcTmp);
					if (rcTmp.Width() > rcTmp.Height()) {
						nWidth = (int)(rcTmp.Height() * 4 / 5);
						nHeight = nWidth;
					}
					else {
						nHeight = (int)(rcTmp.Width() * 4 / 5);
						nWidth = nHeight;
					}
					rcPos.top = (int)((rcTmp.top + rcTmp.bottom) / 2) - (int)(nHeight / 2);
					rcPos.left = (int)((rcTmp.left + rcTmp.right) / 2) - (int)(nWidth / 2);
					rcPos.bottom = rcPos.top + nHeight;
					rcPos.right = rcPos.left + nWidth;

					m_pImageViewerDlg->MoveWindow(rcPos);
				}
				if (m_pImageViewerDlg != NULL) {
					pList = GetSearchListData(m_nSelectRow - 1);
					m_pImageViewerDlg->ShowWindow(SW_SHOW);
					m_pImageViewerDlg->SetImageFile(pList->strImageFileName);
				}
				m_pSearchList->SetFocus();
			}
			return TRUE;
		}
	//KANG 22.06.17 Modify End
		default:
			break;
		}
	}
//KANG 22.05.24 Add End
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CHistoryDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	CRect rect;
//KANG 22.01.07 Modify Start
	//	GetClientRect(&rect);
	if (m_CalCtrl.GetSafeHwnd()) {
		m_CalCtrl.GetClientRect(&rect);
	}
	else {
		this->GetClientRect(&rect);
	}
//KANg 22.01.07 Modify End
}

void CHistoryDlg::OpenSimDlg()
{
	if (m_pImageSimDlg == NULL) {
		m_pImageSimDlg = new CImageProcSimDlg(this);
		m_pImageSimDlg->Create(IDD_DLG_INSP_SIM, this);
	}

	m_pImageSimDlg->ShowWindow(SW_SHOW);

}

//KANG 22.01.07 Add Start
void CHistoryDlg::OnNotifySimulation(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_THUMBDATA* pData = (NM_THUMBDATA*)pNMHDR;
	int nImageIndex = pData->nIndex;
	CString strImageFileName = m_ThumbnailCtrl.GetImageFileNameFromList(nImageIndex);

	if (m_nImageType == 0 || m_nImageType == 2) {
		CString strFilePath = _T("");
		CString strFileName = _T("");
		CString strFindFileName = _T("");
		TCHAR szTemp[4096] = { 0, };
		strcpy_s(szTemp, sizeof(szTemp), strImageFileName);

		strFileName = PathFindFileName(szTemp);
		PathRemoveFileSpec(szTemp);
		strFilePath = szTemp;
		ZeroMemory(szTemp, sizeof(szTemp));
		strcpy_s(szTemp, sizeof(szTemp), strFileName);
		PathRemoveExtension(szTemp);
		strFileName = szTemp;
		strImageFileName = _T("");

		CFileFind finder;
		strFilePath.Replace(_T("Crop"), _T("*.*"));
		BOOL bWorking = finder.FindFile( strFilePath );
		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			if (finder.IsDots()) {
				continue;
			}
			if (finder.IsDirectory()) {
				continue;
			}
			strFindFileName = finder.GetFileName();
			if (strFindFileName.Find(strFileName) > 0) {
				strImageFileName = finder.GetFilePath();
				bWorking = FALSE;
			}
		}
	}

	if (theApp.m_pImageSimDlg == NULL) {
		theApp.m_pImageSimDlg = new CImageProcSimDlg(this);
		theApp.m_pImageSimDlg->Create(IDD_DLG_INSP_SIM, this);
	}
	if (theApp.m_pImageSimDlg != NULL) {
		theApp.m_pImageSimDlg->ShowWindow(SW_SHOW);
		theApp.m_pImageSimDlg->LoadImageFile(strImageFileName);
	}
}

void CHistoryDlg::SetTextHeader(COleDateTime dtDate)
{
	CString strHeader = _T("");
	strHeader = dtDate.Format(_T("%Y %B"));
	m_stHeader.SetWindowTextA(strHeader);
}

void CHistoryDlg::SetupItems()
{
	// Setup some appointments for the day!
	COleDateTime dtS, dtE;
	int nItem = 0;

	m_CalCtrl.DeleteAllItems();	// Clear calendar control

	CWin32File file;
	COleDateTime OleDateTime;
	int nMonth = 0, nDay = 0;
	int nYear = 0;
	CString strFolder = _T("");
	for (int nCell = 1; nCell <= m_CalCtrl.GetTotalCellNum(); nCell++)
	{
		OleDateTime = m_CalCtrl.GetDateByID(nCell);
		nMonth = OleDateTime.GetMonth();
		nDay = OleDateTime.GetDay();
		// 22.03.02 Ahn Modify Start
		nYear = OleDateTime.GetYear();
		//strFolder.Format("%s\\%02d\\%02d", AprData.m_strFeederPath, nMonth, nDay);
		strFolder.Format(_T("%s\\%d\\%02d\\%02d"), AprData.m_strFeederPath, nYear, nMonth, nDay);
		// 22.03.02 Ahn Modify End
		if (file.ExistFile(strFolder))
		{
			CString strText = _T("");
			int nFolderCnt = file.GetFolderCount(strFolder);
			strText.Format(_T("진행롤 : %02d"), nFolderCnt);
			dtS.SetDateTime(OleDateTime.GetYear(), OleDateTime.GetMonth(), OleDateTime.GetDay(), 0, 0, 0);
			dtE.SetDateTime(OleDateTime.GetYear(), OleDateTime.GetMonth(), OleDateTime.GetDay(), 0, 0, 0);
			nItem = m_CalCtrl.InsertItem(dtS, dtE, strText, FALSE, 0);
			m_CalCtrl.SetItemColor(nItem, RGB(255, 148, 132));
			m_CalCtrl.SetItemData(nItem, 1);
		}
	}
}

void CHistoryDlg::OnBnClickedBtnPrev()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COleDateTime OleDateTime;
	OleDateTime = m_CalCtrl.GetCurrentDate();
	int nID = m_CalCtrl.GetCellID(OleDateTime);

	int nViewType = WV_MONTHVIEW;
	switch (m_nCalcViewType)
	{
	case 1:
	{
		OleDateTime -= 7;
		OleDateTime.SetDateTime(OleDateTime.GetYear(), OleDateTime.GetMonth(), OleDateTime.GetDay(), 0, 0, 0);

		nViewType = WV_WEEKVIEW;
		break;
	}
	case 0:
	default:
	{
		if ((m_nCurMonth - 1) == 0) {
			OleDateTime.SetDateTime((OleDateTime.GetYear() - 1), 12, 1, 0, 0, 0);
			m_nCurMonth = 12;
		}
		else {
			OleDateTime.SetDateTime(OleDateTime.GetYear(), (--m_nCurMonth), 1, 0, 0, 0);
			//SetTextHeader(OleDateTime);

		}
		//m_CalCtrl.SetCurrentDate(OleDateTime, FALSE, nViewType);
		//OleDateTime = m_CalCtrl.GetDateByID(nID); // 22.03.03 Ahn Delete
		nViewType = WV_MONTHVIEW;
		break;
	}
	}

	m_CalCtrl.SetCurrentDate(OleDateTime, FALSE, nViewType);
	SetSelectCell(OleDateTime);
	SetupItems();
	SetTextHeader(OleDateTime);
	InvalidateRect(NULL, TRUE);
}


void CHistoryDlg::OnBnClickedBtnNext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COleDateTime OleDateTime;
	OleDateTime = m_CalCtrl.GetCurrentDate();
	int nID = m_CalCtrl.GetCellID(OleDateTime);

	int nViewType = WV_MONTHVIEW;
	switch (m_nCalcViewType)
	{
	case 1:
	{
		OleDateTime += 7;
		OleDateTime.SetDateTime(OleDateTime.GetYear(), OleDateTime.GetMonth(), OleDateTime.GetDay(), 0, 0, 0);

		nViewType = WV_WEEKVIEW;
		break;
	}
	case 0:
	default:
	{
		if ((m_nCurMonth + 1) == 13) {
			OleDateTime.SetDateTime((OleDateTime.GetYear() + 1), 1, 1, 0, 0, 0);
			m_nCurMonth = 1;
		}
		else {
			OleDateTime.SetDateTime(OleDateTime.GetYear(), (++m_nCurMonth), 1, 0, 0, 0);
		}
		//m_CalCtrl.SetCurrentDate(OleDateTime, FALSE, nViewType);
		//OleDateTime = m_CalCtrl.GetDateByID(nID); // 22.03.03 Ahn Delete
		nViewType = WV_MONTHVIEW;
		break;
	}
	}

	m_CalCtrl.SetCurrentDate(OleDateTime, FALSE, nViewType);
	SetSelectCell(OleDateTime);
	SetupItems();
	SetTextHeader(OleDateTime);
	InvalidateRect(NULL, TRUE);
}


void CHistoryDlg::OnBnClickedBtnToday()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nViewType = WV_MONTHVIEW;
	switch (m_nCalcViewType)
	{
	case 1:
	{
		nViewType = WV_WEEKVIEW;
		break;
	}
	case 0:
	default:
	{
		nViewType = WV_MONTHVIEW;
		break;
	}
	}

	m_CalCtrl.SetCurrentDate(COleDateTime::GetCurrentTime(), FALSE, nViewType);
	SetSelectCell(COleDateTime::GetCurrentTime());
	SetupItems();
	SetTextHeader(COleDateTime::GetCurrentTime());
	// 22.03.03 Ahn Add Start
	m_nCurMonth = COleDateTime::GetCurrentTime().GetMonth();
	InvalidateRect(NULL, TRUE);
}


void CHistoryDlg::OnBnClickedRdMonth()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COleDateTime OleDataTime;
	OleDataTime = m_CalCtrl.GetCurrentDate();
	m_CalCtrl.SetCurrentDate(OleDataTime, FALSE, WV_MONTHVIEW);
	m_nCurMonth = OleDataTime.GetMonth();

	m_nCalcViewType = 0;
	m_CalCtrl.SetView(WV_MONTHVIEW);
	SetupItems();
	UpdateData(FALSE);
}


void CHistoryDlg::OnBnClickedRdWeek()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COleDateTime OleDataTime;
	OleDataTime = m_CalCtrl.GetCurrentDate();
	if (OleDataTime.GetMonth() != m_nCurMonth) {

		m_CalCtrl.SetCurrentDate(OleDataTime, FALSE, WV_WEEKVIEW);
		m_nCurMonth = OleDataTime.GetMonth();
	}

	SetSelectCell(OleDataTime);
	m_nCalcViewType = 1;
	m_CalCtrl.SetView(WV_WEEKVIEW);
	SetupItems();
	UpdateData(FALSE);
}

void CHistoryDlg::OnBnClickedBtnRefresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_nDispMode == 0) {
		SetupItems();
		InvalidateRect(NULL, TRUE);
	}else if (m_nDispMode == 1) {
		OnRefresh(0);
	}
}

void CHistoryDlg::OnBnClickedRdNgCrop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 22.02.11 Ahn Modify Start
	//m_nImageType = 0;
	//m_ThumbnailCtrl.ReadImageFileInFolder(m_strImageFilePath[m_nImageType]);
	m_nImageType = en_NgCrop;
	m_ThumbnailCtrl.SetImageList(m_strImageList[en_NgCrop]);
	// 22.02.11 Ahn Modify End

}


void CHistoryDlg::OnBnClickedRdNg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 22.02.11 Ahn Modify Start
	//m_nImageType = 1;
	//m_ThumbnailCtrl.ReadImageFileInFolder(m_strImageFilePath[m_nImageType]);
	m_nImageType = en_NgImage;
	m_ThumbnailCtrl.SetImageList(m_strImageList[m_nImageType]);
	// 22.02.11 Ahn Modify End
}


void CHistoryDlg::OnBnClickedRdOkCrop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 22.02.11 Ahn Modify Start
	//m_nImageType = 2;
	//m_ThumbnailCtrl.ReadImageFileInFolder(m_strImageFilePath[m_nImageType]);
	m_nImageType = en_OkCrop;
	m_ThumbnailCtrl.SetImageList(m_strImageList[m_nImageType]);
	// 22.02.11 Ahn Modify End
}


void CHistoryDlg::OnBnClickedRdOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 22.02.11 Ahn Modify Start
	//m_nImageType = 3;
	//m_ThumbnailCtrl.ReadImageFileInFolder(m_strImageFilePath[m_nImageType]);
	m_nImageType = en_OkImage;
	m_ThumbnailCtrl.SetImageList(m_strImageList[m_nImageType]);
	// 22.02.11 Ahn Modify End
}

void CHistoryDlg::OnCellDblClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{

}


void CHistoryDlg::OnCellClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_WVCELLDATA* pData = (NM_WVCELLDATA*)pNMHDR;

	if (pData->pItem != NULL)
	{
		// Save current Date ItemID
		nDataItemID = pData->nItem;
	}
	else
	{
		nDataItemID = -1;
	}

	COleDateTime OleDateTime;
	OleDateTime = m_CalCtrl.GetCurrentDate();
	SetSelectCell(OleDateTime);
}

void CHistoryDlg::SetSelectCell(COleDateTime dtDate)
{
	m_CalCtrl.SetSelectCell(dtDate);
	int nMonth = dtDate.GetMonth();
	int nDay = dtDate.GetDay();
	CWin32File file;
	CString strFolder = _T("");
	// 22.03.02 Ahn Modify Start
	int nYear = dtDate.GetYear();
	//strFolder.Format(_T("%s\\%02d\\%02d"), AprData.m_strFeederPath, nMonth, nDay);
	strFolder.Format(_T("%s\\%d\\%02d\\%02d"), AprData.m_strFeederPath, nYear, nMonth, nDay);
	// 22.03.02 Ahn Modify End

	if (file.ExistFile(strFolder) == FALSE)
	{
		nMonth = 0;
		nDay = 0;
	}

//	CRect rect;
//	theApp.GetResultViewPrt()->SetWindowTextA(_T("검사이력"));
//// 22.06.09 Ahn Delete Start
////	theApp.GetResultViewPrt()->GetResultViewDlgPtr()->MakeGridHist(strFolder);
//// 22.06.09 Ahn Delete End
//	theApp.GetResultViewPrt()->GetResultViewDlgPtr()->GetClientRect(rect);
//	theApp.GetResultViewPrt()->GetResultViewDlgPtr()->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.Width(), rect.Height()));
}

int CHistoryDlg::OnRefresh(int nKind /*= 0*/)
{
//KANG 22.05.24 Modify Start
//	if (nKind == 0) {
	if (nKind == enHistoryDisp_Calender) {
//KANG 22.05.24 Modify End		
		m_CalCtrl.ShowWindow(SW_SHOW);
		m_ThumbnailCtrl.ShowWindow(SW_HIDE);
		m_stHeader.ShowWindow(SW_SHOW);
		CButton* pButton = NULL;
		pButton = (CButton*)GetDlgItem(IDC_BTN_PREV);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_TODAY);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_NEXT);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_REFRESH);
		if (pButton != nullptr) {
		//KANG 22.05.24 Add Start
			pButton->ShowWindow(SW_SHOW);
		//KANG 22.05.24 Add End
			pButton->SetWindowTextA(_T("갱신"));
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_WEEK);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_MONTH);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_NG_CROP);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_NG);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_OK_CROP);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_OK);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
	//KANG 22.05.24 Modify Start
	//	m_nDispMode = 0;
		pButton = (CButton*)GetDlgItem(IDC_ST_SERCH);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_LOTID);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_TIME);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_ED_SERCH_LOTID);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_START);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_START);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_END);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_END);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_ST_TILDE);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_SEARCH);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_EXPORT);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		if (m_pSearchList->GetSafeHwnd()) {
			m_pSearchList->ShowWindow(SW_HIDE);
		}
	//KANG 22.06.17 Add Start
		if (m_pGraphHist->GetSafeHwnd()) {
			m_pGraphHist->ShowWindow(SW_HIDE);
		}
	//KANG 22.06.17 Add End
		m_nDispMode = enHistoryDisp_Calender;
	//KANG 22.05.24 Modify End
//KANG 22.05.24 Modify Start
//	}else {
	}
	else if (nKind == enHistoryDisp_Thumbnail) {
//KNAG 22.05.24 Modify End
		m_CalCtrl.ShowWindow(SW_HIDE);
		m_ThumbnailCtrl.ShowWindow(SW_SHOW);
		m_stHeader.ShowWindow(SW_HIDE);
		CButton* pButton = NULL;
		pButton = (CButton*)GetDlgItem(IDC_BTN_PREV);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_TODAY);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_NEXT);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_REFRESH);
		if (pButton != nullptr) {
		//KANG 22.05.24 Add Start
			pButton->ShowWindow(SW_SHOW);
		//KANG 22.05.24 Add End
			pButton->SetWindowTextA(_T("나가기"));
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_WEEK);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_MONTH);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_NG_CROP);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_NG);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_OK_CROP);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_OK);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_SHOW);
		}
	//KANG 22.05.24 Modify Start
	//	m_nDispMode = 1;
		pButton = (CButton*)GetDlgItem(IDC_ST_SERCH);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_LOTID);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_RD_TIME);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_ED_SERCH_LOTID);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_START);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_START);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_END);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_END);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_ST_TILDE);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_SEARCH);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		pButton = (CButton*)GetDlgItem(IDC_BTN_EXPORT);
		if (pButton != nullptr) {
			pButton->ShowWindow(SW_HIDE);
		}
		if (m_pSearchList->GetSafeHwnd()) {
			m_pSearchList->ShowWindow(SW_HIDE);
		}
	//KANG 22.06.17 Add Start
		if (m_pGraphHist->GetSafeHwnd()) {
			m_pGraphHist->ShowWindow(SW_HIDE);
		}
	//KANG 22.06.17 Add End
		m_nDispMode = enHistoryDisp_Thumbnail;
	//KANG 22.05.24 Modify End
	}
//KANG 22.05.24 Add Start
	else if (nKind == enHistoryDisp_List) {
	m_CalCtrl.ShowWindow(SW_HIDE);
	m_ThumbnailCtrl.ShowWindow(SW_HIDE);
	m_stHeader.ShowWindow(SW_HIDE);
	CButton* pButton = NULL;
	pButton = (CButton*)GetDlgItem(IDC_BTN_PREV);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_TODAY);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_NEXT);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_REFRESH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_WEEK);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_MONTH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_NG_CROP);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_NG);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_OK_CROP);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_OK);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_ST_SERCH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_LOTID);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_TIME);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
	}
	pButton = (CButton*)GetDlgItem(IDC_ED_SERCH_LOTID);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
		if (m_nSearchMethod == 0) {
			pButton->EnableWindow(TRUE);
		}
		else {
			pButton->EnableWindow(FALSE);
		}
	}
	pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_START);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
		if (m_nSearchMethod == 0) {
			pButton->EnableWindow(FALSE);
		}
		else {
			pButton->EnableWindow(TRUE);
		}
	}
	pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_START);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
		if (m_nSearchMethod == 0) {
			pButton->EnableWindow(FALSE);
		}
		else {
			pButton->EnableWindow(TRUE);
		}
	}
	pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_END);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
		if (m_nSearchMethod == 0) {
			pButton->EnableWindow(FALSE);
		}
		else {
			pButton->EnableWindow(TRUE);
		}
	}
	pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_END);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
		if (m_nSearchMethod == 0) {
			pButton->EnableWindow(FALSE);
		}
		else {
			pButton->EnableWindow(TRUE);
		}
	}
	pButton = (CButton*)GetDlgItem(IDC_ST_TILDE);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_SEARCH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_EXPORT);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_SHOW);
	}
	if (m_pSearchList->GetSafeHwnd()) {
		m_pSearchList->ShowWindow(SW_SHOW);
	}
//KANG 22.06.17 Add Start
	if (m_pGraphHist->GetSafeHwnd()) {
		m_pGraphHist->ShowWindow(SW_SHOW);
	}
//KANG 22.06.17 Add End
	m_nDispMode = enHistoryDisp_List;
	}
	else {
	m_CalCtrl.ShowWindow(SW_HIDE);
	m_ThumbnailCtrl.ShowWindow(SW_HIDE);
	m_stHeader.ShowWindow(SW_HIDE);
	CButton* pButton = NULL;
	pButton = (CButton*)GetDlgItem(IDC_BTN_PREV);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_TODAY);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_NEXT);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_REFRESH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_WEEK);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_MONTH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_NG_CROP);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_NG);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_OK_CROP);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_OK);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_ST_TITLE);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_ST_SERCH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_LOTID);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_RD_TIME);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_ED_SERCH_LOTID);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_START);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_START);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_DATETIMEPICKER_END);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_CMB_TIME_END);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_ST_TILDE);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_SEARCH);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	pButton = (CButton*)GetDlgItem(IDC_BTN_EXPORT);
	if (pButton != nullptr) {
		pButton->ShowWindow(SW_HIDE);
	}
	if (m_pSearchList->GetSafeHwnd()) {
		m_pSearchList->ShowWindow(SW_HIDE);
	}
//KANG 22.06.17 Add Start
	if (m_pGraphHist->GetSafeHwnd()) {
		m_pGraphHist->ShowWindow(SW_HIDE);
	}
//KANG 22.06.17 Add End
	}
//KANG 22.05.24 Add End

	CRect rect;
	this->GetClientRect(rect);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.Width(), rect.Height()));
	
	return 0;
}

int CHistoryDlg::ReadThumbnailfileInFolder(LPCTSTR lpszPath)
{
	int nRet = 0;
	int nIndex = 0;
	CString strPath = lpszPath;
	int nID[4];

	strPath += _T("\\IMAGE");
	m_nImageType = 0;

	m_strImageFilePath[0] = strPath + _T("\\NG\\Crop");
	m_strImageFilePath[1] = strPath + _T("\\NG");
	m_strImageFilePath[2] = strPath + _T("\\OK\\Crop");
	m_strImageFilePath[3] = strPath + _T("\\OK");
	nID[0] = GetDlgItem(IDC_RD_NG_CROP)->GetDlgCtrlID();
	nID[1] = GetDlgItem(IDC_RD_NG)->GetDlgCtrlID();
	nID[2] = GetDlgItem(IDC_RD_OK_CROP)->GetDlgCtrlID();
	nID[3] = GetDlgItem(IDC_RD_OK)->GetDlgCtrlID();

	for (nIndex = 3; nIndex >= 0; nIndex--) {
		nRet = m_ThumbnailCtrl.ReadImageFileInFolder(m_strImageFilePath[nIndex], _T("JPG|BMP"), FALSE, FALSE);
		m_nImageFileCount[nIndex] = m_ThumbnailCtrl.GetImageCount();
		if (m_nImageFileCount[nIndex] > 0) {
			m_nImageType = nIndex;
			GetDlgItem(nID[nIndex])->EnableWindow(TRUE);
		}else {
			GetDlgItem(nID[nIndex])->EnableWindow(FALSE);
		}
	}
	m_ThumbnailCtrl.OnRefresh();
	UpdateData(FALSE);

	return nRet;
}
//KANG 22.01.07 Add End

// 22.02.11 Ahn Add Start
//int CHistoryDlg::ReadThumbnailFromErrorData( CString strPath, CString strMonth, CString strDay, CString strLotID )
int CHistoryDlg::ReadThumbnailFromErrorData(CString strPath, CString strYear, CString strMonth, CString strDay, CString strLotID)
{
	// 22.06.23 Ahn Delete Start
	//int nRet = 0;
	//int nIndex = 0;
	//int nID[4];

	//CString strImgPath;
	//strImgPath.Format( _T("%s\\IMAGE"), strPath );
	//m_nImageType = 0;

	//if (m_pDefData == nullptr) {
	//	m_pDefData = new CDefectDataCtrl();
	//}

	//CString strDate;
	//// 22.03.02 Ahn Modify Start
	////strDate.Format(_T("%s\\%s"), strMonth, strDay);
	//strDate.Format(_T("%s\\%s\\%s"), strYear, strMonth, strDay);
	//// 22.03.02 Ahn Modify End
	//m_pDefData->RemoveAll();
	//m_pDefData->LoadErrorData(strDate, strLotID);

	//m_strImageFilePath[en_NgCrop] = strPath + _T("\\NG\\Crop");
	//m_strImageFilePath[en_NgImage] = strPath + _T("\\NG");
	//m_strImageFilePath[en_OkCrop] = strPath + _T("\\OK\\Crop");
	//m_strImageFilePath[en_OkImage] = strPath + _T("\\OK");
	//nID[en_NgCrop] = GetDlgItem(IDC_RD_NG_CROP)->GetDlgCtrlID();
	//nID[en_NgImage] = GetDlgItem(IDC_RD_NG)->GetDlgCtrlID();
	//nID[en_OkCrop] = GetDlgItem(IDC_RD_OK_CROP)->GetDlgCtrlID();
	//nID[en_OkImage] = GetDlgItem(IDC_RD_OK)->GetDlgCtrlID();

	//m_nImageFileCount[en_NgCrop] = m_pDefData->GetNgCropImgCnt();
	//m_nImageFileCount[en_NgImage] = m_pDefData->GetNgImgCnt();
	//m_nImageFileCount[en_OkCrop] = m_pDefData->GetOkCropImgCnt();
	//m_nImageFileCount[en_OkImage] = m_pDefData->GetOkImgCnt();

	//for (int i = 0; i < en_ImageAllType; i++) {
	//	m_strImageList[i].RemoveAll() ;
	//	if (m_nImageFileCount[nIndex] > 0) {
	//		m_nImageType = nIndex;
	//		GetDlgItem(nID[nIndex])->EnableWindow(TRUE);
	//	}	
	//	else {
	//		GetDlgItem(nID[nIndex])->EnableWindow(FALSE);
	//	}
	//}

	////nRet = m_ThumbnailCtrl.ReadImageFileInFolder(m_strImageFilePath[en_NgCrop], _T("JPG|BMP"), FALSE, FALSE ) ;
	//int nTotalTabCnt = m_pDefData->GetSize();
	//for (int i = 0; i < nTotalTabCnt ; i++) {
	//	//CTabRsltInfo *pInfo = m_pDefData->GetTabRsltInfo(CAM_POS_TOP, i);
	//	CTabRsltInfo* pInfo = m_pDefData->GetTabRsltInfoPtr(i);
	//	if ( (pInfo->m_bImageFlag == FALSE ) && ( pInfo->m_bCropImgFlag == FALSE ) ) {
	//		continue;
	//	}

	//	pInfo->m_nHeadNo;
	//	pInfo->m_nTabNo;

	//	//if (pInfo->m_nJudge == JUDGE_NG) {
	//	//	m_strImageList[en_NgCrop].AddTail(pInfo->m_chCropPath);
	//	//	m_strImageList[en_NgImage].AddTail(pInfo->m_chImagePath);
	//	//}
	//	//else {
	//	//	if (pInfo->m_bImageFlag == TRUE) {
	//	//		m_strImageList[en_OkImage].AddTail(pInfo->m_chImagePath);
	//	//	}
	//	//	if(pInfo->m_bCropImgFlag == TRUE) {
	//	//		m_strImageList[en_OkCrop].AddTail(pInfo->m_chCropPath);
	//	//	}
	//	//}
	//}

	//m_ThumbnailCtrl.SetImageList( m_strImageList[en_NgCrop] );

	//m_ThumbnailCtrl.OnRefresh();
	//UpdateData(FALSE);

	//return nRet;
	// 22.06.23 Ahn Delete End
	return 0;
}
// 22.02.11 Ahn Add End

//KANG 22.05.24 Add Start
int CHistoryDlg::MakeGridSearchList()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_pSearchList != nullptr) {
		if (m_pSearchList->m_hWnd == NULL) {
			CRect rect;
			GetClientRect(rect);
			if (m_pSearchList->Create(rect, this, IDC_SEARCH_LIST) == FALSE) {
				delete m_pSearchList;
				m_pSearchList = NULL;
				return -1;
			}
		}

		if (m_pSearchList != NULL && m_pSearchList->m_hWnd != NULL) {
			if (m_pFontSearchList == NULL) {
				m_pFontSearchList = new CFont();
				m_pFontSearchList->CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
					, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
					, FIXED_PITCH, "Gulim");
				m_pSearchList->SetFont(m_pFontSearchList, FALSE);
			}

			int nFixCols = 1;
			int nFixRows = 1;
			int nCols = en_MaxListCol;
			int nRows = 1;
			int nRow = 0;
			int nCol = 0;

			nRows += GetSearchListCount();

			//m_pSearchList->SetAutoSizeStyle();
			m_pSearchList->SetFixedColumnSelection(TRUE);
			m_pSearchList->SetFixedRowSelection(TRUE);
			m_pSearchList->SetRows(nRows);
			m_pSearchList->SetCols(nCols);

			// 22.06.02 Ahn Modify Start
			CString strTitle[en_MaxListCol] = { _T(""), _T("Time"), _T("LOT"), _T("Model"), _T("Lane"), _T("CELL"), _T("Top/Bottom"), _T("Type"), _T("Judge"), _T("X(um)"), _T("Y(um)"), _T("Marking"), _T("IMAGE") };
			// 22.06.02 Ahn Modify End
			m_pSearchList->SetColWidth(en_ListCol_No, 1000);
			m_pSearchList->SetColWidth(en_ListCol_Time, 3000);
			m_pSearchList->SetColWidth(en_ListCol_LotID, 2500);
			m_pSearchList->SetColWidth(en_ListCol_Model, 2500);
			m_pSearchList->SetColWidth(en_ListCol_Lane, 2000);
			m_pSearchList->SetColWidth(en_ListCol_Cell, 1000);
			m_pSearchList->SetColWidth(en_ListCol_TopBottom, 1500);
			m_pSearchList->SetColWidth(en_ListCol_Type, 2000);
			m_pSearchList->SetColWidth(en_ListCol_Rank, 1000);
			m_pSearchList->SetColWidth(en_ListCol_XSize, 2000);
			m_pSearchList->SetColWidth(en_ListCol_YSize, 2000);
			m_pSearchList->SetColWidth(en_ListCol_Marking, 1000);
			m_pSearchList->SetColWidth(en_ListCol_Link, 1500);

			for (nCol = 0; nCol < m_pSearchList->GetColumnCount(); nCol++) {
				m_pSearchList->SetColAlignment(nCol, 5);
				m_pSearchList->SetTextMatrix(0, nCol, strTitle[nCol]);
			}

			m_pSearchList->SetRedraw(FALSE);
			for (int nRow = 1; nRow < nRows; nRow++)
			{
				CDefectSearchList* pList = NULL;
				pList = GetSearchListData(nRow - 1);
				for (int nCol = 0; nCol < m_pSearchList->GetColumnCount(); nCol++)
				{
					CString strText;
					switch (nCol) {
					case	en_ListCol_No:			//No.
						strText.Format(_T("%d"), nRow);
						break;
					case	en_ListCol_Time:		//Time
						strText.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), pList->tLotStartTime.wYear, pList->tLotStartTime.wMonth, pList->tLotStartTime.wDay,
							pList->tLotStartTime.wHour, pList->tLotStartTime.wMinute, pList->tLotStartTime.wSecond);
						break;
					case	en_ListCol_LotID:		//LotID
						strText = pList->strLotID;
						break;
					case	en_ListCol_Model:		//Model
						strText = pList->strModel;
						break;
					case	en_ListCol_Lane:		//Lane
						strText = pList->strLane;
						break;
					case	en_ListCol_Cell:		//CELL
						// 22.06.03 Ahn Modify Start
						//strText.Format(_T("%d"), pList->nCell );
						strText.Format(_T("%d"), pList->nCell + 1 );
						// 22.06.03 Ahn Modify Start
						break;
					case	en_ListCol_TopBottom:	//Top/Bottom
					{
						if (pList->nTopBottom == 0) {
							strText = _T("Top");
						}
						else {
							strText = _T("Bottom");
						}
					}
						break;
					case	en_ListCol_Type:		//Type
					{
						switch (pList->nType)
						{
						case en_ModeFoilExp:
						case en_ModeFoilBoth: // 22.07.20 Ahn Add 
							strText = _T("Foil Exposure");
							break;
						case en_ModeFoilExpOut:
							strText = _T("Foil Exposure");
							break;
						case en_ModeSurface:
							strText = _T("Surface");
							break;
						default:
							strText = _T("None");
							break;
						}
					}
						break;
					case	en_ListCol_Rank:		//Rank
					{
						switch (pList->nRank)
						{
						case JUDGE_OK:
							strText = _T("OK");
							break;
						case JUDGE_NG:
							strText = _T("NG");
							break;
						// 22.11.21 Ahn Modify Start - JUDGE_GRAY
						case JUDGE_GRAY :
							strText = _T("GRAY");
							break;
						// 22.11.21 Ahn Modify End
						default:
							strText = _T("None");
							break;
						}
					}
					break;
					case	en_ListCol_XSize:		//XSize
						strText.Format(_T("%03lf"), pList->dXSize);
						break;
					case	en_ListCol_YSize:		//YSize
						strText.Format(_T("%03lf"), pList->dYSize);
						break;
					case	en_ListCol_Marking	:	//Marking
					{
						if (pList->bMarking == TRUE) {
							strText = _T("O");
						}
						else {
							strText = _T("X");
						}
					}
					break;
					case	en_ListCol_Link:		//ImageFileName
						strText = _T("LINK");
						break;
					default:
						break;
					}
					m_pSearchList->SetTextMatrix(nRow, nCol, strText);
				}
			}
			m_pSearchList->SetRedraw(TRUE);
			m_pSearchList->AutoSizeRows();
			m_pSearchList->ShowWindow(SW_SHOW);
		//KANG 22.06.17 Add Start
			m_pSearchList->SetEnsureVisibleType(1);
		//KANG 22.06.17 Add End
		}
	}

	return 0;
}

void CHistoryDlg::SearchListClear()
{
	FreeSearchList(m_pVtList);
	m_pVtList = new std::vector<CDefectSearchList*>;
}

void CHistoryDlg::AddSearchListData(CDefectSearchList* pList)
{
	if (pList != NULL) {
		m_pVtList->push_back(pList);
	}
}

int CHistoryDlg::GetSearchListCount()
{
	return ((int)m_pVtList->size());
}

CDefectSearchList* CHistoryDlg::GetSearchListData(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= (int)m_pVtList->size())) {
		return (NULL);
	}

	CDefectSearchList* ptr = (*m_pVtList)[nIndex];
	return (ptr);
}

void CHistoryDlg::FreeSearchList(std::vector<CDefectSearchList*>* pVector)
{
	AfxBeginThread((AFX_THREADPROC)FreeAsyncThread, pVector, THREAD_PRIORITY_IDLE);
}

void CHistoryDlg::OnBnClickedRdLotid()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSearchMethod = 0;
	OnRefresh(enHistoryDisp_List);
	UpdateData(FALSE);
}

UINT CHistoryDlg::FreeAsyncThread(LPVOID param)
{
	std::vector<CDefectSearchList*>* pTmp = (std::vector<CDefectSearchList*> *)param;
	if (pTmp != NULL) {
		int size = (int)pTmp->size();
		for (int i = 0; i < size; i++) {
			delete (*pTmp)[i];
		}
		delete pTmp;
	}
	return (0);
}

void CHistoryDlg::OnBnClickedRdTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSearchMethod = 1;
	OnRefresh(enHistoryDisp_List);
	UpdateData(FALSE);
}

void CHistoryDlg::OnBnClickedBtnSearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//KANG 22.05.27 Add Start
	UpdateData(TRUE);
	CWin32File file;
	CStringList strSearchFileList;
	CString strReadFileName;
	strSearchFileList.RemoveAll();

	if (m_nSearchMethod == 0) {
		if (m_strSearchLotID.GetLength() == 0) {
			MessageBox(_T("Please input the LOT ID to search.") );
			return;
		}
		else {
			CString strIndexFileName;
			strIndexFileName = AprData.GetIndexFileName(m_strSearchLotID);
			if (file.ExistFile(strIndexFileName) == FALSE) {
				MessageBox(_T("No search results found."));
				return;
			}
			else {
				CStdioFile cf;
				CString strMsg;
				if (!cf.Open(strIndexFileName, (CFile::typeText | CFile::modeRead))) {
					strMsg.Format(_T("File open error[%s]"), strIndexFileName);
					MessageBox(strMsg);
					return;
				}
				while (1) {
					try {
						if (cf.ReadString(strReadFileName) == NULL) {
							break;
						}
						strSearchFileList.AddTail(strReadFileName);
					}
					catch (CFileException& e) {
						CHAR	szChar[256];
						e.GetErrorMessage(szChar, 255);
						strMsg.Format(_T("Error - %s"), szChar);
						MessageBox(strMsg);
						cf.Close();
						return;
					}
				}
				cf.Close();
			}
		//KANG 22.06.17 Add Start
			m_pGraphHist->ShowWindow(SW_SHOW);
			m_bShowGraphHist = TRUE;
			OnRefresh(m_nDispMode);
			m_pGraphHist->DataClear();
			m_nGraphHistHorSize = 5;
			m_nGraphHistVerSize = 10;
		//KANG 22.06.17 Add End
		}
	}
	else {
		COleDateTime sTime, eTime, spanTime;
		int sHour, eHour;
		VERIFY(m_dtPickStart.GetTime(sTime));
		VERIFY(m_dtPickEnd.GetTime(eTime));
		sHour = m_cmbTimeStart.GetCurSel();
		eHour = m_cmbTimeEnd.GetCurSel();

		spanTime = eTime - sTime;
		if (spanTime.m_dt <= 0) {
			if ((spanTime.m_dt == 0) && ((eHour - sHour) < 0)) {
				MessageBox(_T("The search date setting is invalid."));
				return;
			}
		}

		unsigned int nStartTime = (sTime.GetYear() * 1000000) + (sTime.GetMonth() * 10000) + (sTime.GetDay() * 100) + sHour;
		unsigned int nEndTime = (eTime.GetYear() * 1000000) + (eTime.GetMonth() * 10000) + (eTime.GetDay() * 100) + eHour;
		CString strSearchFolder = _T("");
		CFileFind	finder;
		for (int nYear = sTime.GetYear(); nYear <= eTime.GetYear(); nYear++) {
			for (int nMonth = 1; nMonth <= 12; nMonth++) {
				for (int nDay = 1; nDay <= 31; nDay++) {
					for (int nHour = 0; nHour < 24; nHour++) {
						unsigned int nSearch = (nYear * 1000000) + (nMonth * 10000) + (nDay * 100) + nHour;
						if ((nStartTime <= nSearch) && (nEndTime >= nSearch)) {
							strSearchFolder.Format(_T("%s\\BINARY\\%04d%02d\\%02d\\%02d"), AprData.m_strResultPath, nYear, nMonth, nDay, nHour);
							GetFileList(strSearchFolder, strSearchFileList, _T("bin"), 0, TRUE, TRUE, FALSE);
						}
					}
				}
			}
		}
		if (strSearchFileList.GetCount() == 0) {
			MessageBox(_T("No search results found."));
		}
	//KANG 22.06.17 Add Start
		else {
			m_pGraphHist->ShowWindow(SW_HIDE);
			m_bShowGraphHist = FALSE;
			OnRefresh(m_nDispMode);
			m_pGraphHist->DataClear();
			m_nGraphHistHorSize = 5;
			m_nGraphHistVerSize = 10;
		}
	//KANG 22.06.17 Add End 
	}

	SearchListClear();
	POSITION pos = strSearchFileList.GetHeadPosition();
	while (pos != NULL) {
		AddDefectSearchList(strSearchFileList.GetNext(pos));
	}
	MakeGridSearchList();
//KANG 22.05.27 Add End
//KANG 22.06.17 Add Start
	if (m_nSearchMethod == 0) {
		int nTemp = (int)(m_nGraphHistHorSize / 10.0) + 1;
		m_nGraphHistHorSize = nTemp * 10;
		m_pGraphHist->SetImageSize(m_nGraphHistHorSize);
		m_pGraphHist->SetGraphSize(m_nGraphHistHorSize);

		nTemp = GetGraphIntervalSize((int)(m_nGraphHistHorSize / 15));
		m_pGraphHist->SetHorScaleInterval(nTemp);

		nTemp = GetGraphIntervalSize((int)(m_nGraphHistVerSize / 4));
		m_pGraphHist->SetVerMajorScaleInterval(nTemp);
		m_pGraphHist->SetVerMinorScaleInterval((nTemp / 10));

		nTemp = (int)(m_nGraphHistVerSize / 10.0) + 1;
		m_nGraphHistVerSize = nTemp * 10;
		m_pGraphHist->SetMaxVerScaleSize(m_nGraphHistVerSize);
	}
//KANG 22.06.17 Add End
}

//KANG 22.05.27 Add Start
int CHistoryDlg::GetFileList(LPCTSTR lpszPath, CStringList& rstrFileNameList, LPCTSTR lpszExt /*= _T( "*" )*/, int nMaxFiles /*= 0*/, BOOL bSubFolder /*= FALSE*/, BOOL bIncludePath  /*= FALSE*/, BOOL bListClear /*= TRUE*/)
{
	if (bListClear) {
		rstrFileNameList.RemoveAll();
	}

	CString strPath = lpszPath;
	CString strExt = lpszExt;
	if (strPath.Right(1).Compare("\\") != 0) {
		strPath += _T("\\");
	}
	if (strExt.Left(1).Compare(".") != 0) {
		strExt = _T(".") + strExt;
	}
	strPath += _T("*.*");

	CFileFind	finder;
	BOOL bWorking = finder.FindFile(strPath);
	if (bWorking == 0) {
		return (0);
	}

	CString strPathName;
	CString strFileName;
	CString strTemp;
	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) {
			continue;
		}
		if (finder.IsDirectory()) {
			if (bSubFolder) {
				GetFileList(finder.GetFilePath(), rstrFileNameList, lpszExt, nMaxFiles, bSubFolder, bIncludePath, FALSE);
			}
			continue;
		}

		strPathName.Format(_T("%s"), lpszPath);
		strFileName = PathFindFileName(finder.GetFileName());
		TCHAR szTmp[4096];
		ZeroMemory(szTmp, 4096);
		StrCpy(szTmp, strFileName);
		PathRemoveExtension(szTmp);
		strFileName.Format(_T("%s"), szTmp);

		if (strPathName.Find(strFileName) < 0) {
			continue;
		}

		if (bIncludePath == TRUE) {
			strFileName.Format(_T("%s\\%s"), strPathName, finder.GetFileName());
		}
		else {
			strFileName = finder.GetFileName();
		}

		if (_tcscmp(strExt, _T(".*")) != 0) {
			if (_tcscmp(strExt, PathFindExtension(strFileName)) == 0) {
				rstrFileNameList.AddTail(strFileName);
			}
		}
		else {
			rstrFileNameList.AddTail(strFileName);
		}

		if (nMaxFiles > 0) {
			if (rstrFileNameList.GetCount() >= nMaxFiles) {
				break;
			}
		}
	}
	finder.Close();

	return (0);
}
//KANG 22.05.27 Add End

void CHistoryDlg::OnBnClickedBtnExport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 22.05.27 Ahn Add Start
	//CString strSavePath;
	//CString strExcelSavePath;
	//TCHAR szFilter[256] = _T("Excel (*.xls)|*.xls;");
	//CFileDialog dlg(TRUE, _T("xls"), strExcelSavePath, OFN_HIDEREADONLY, szFilter, this);
	//if (dlg.DoModal() == IDOK) {
	//	CExcelResultFileCtrl excel;
	//	strSavePath = dlg.GetPathName();
	//	excel.SaveExeltFile(strSavePath, *m_pVtList);
	//}

	//if( m_pExcelDlg == NULL ) {
	//	m_pExcelDlg = new CExcelFileSaveDlg(this);
	//	m_pExcelDlg->Create(IDD_DLG_EXCEL_SAVE, this);
	//}
	//
	//{
	//	m_pExcelDlg->ShowWindow(SW_SHOW);
	//	m_pExcelDlg->CopyData(*m_pVtList);
	//}
	// 22.05.27 Ahn Add End

	// 22.06.13 Ahn Add Start
	CExcelFileSaveDlg dlg(this);
	dlg.CopyData(* m_pVtList);

	if (dlg.DoModal() == IDOK) {

	}
	// 22.06.13 Ahn Add End
}

void CHistoryDlg::OnMouseLDownSearchList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	NM_GRIDVIEW* pNmgv = (NM_GRIDVIEW*)pNMHDR;
	CGridCtrl* pGrid = m_pSearchList;
	CDefectSearchList* pList = NULL;

	int row = pNmgv->iRow;
	int col = pNmgv->iColumn;
	int nMaxCol = pGrid->GetCols() - 1;

//KANG 22.06.17 Delete Start
//	for (int nCol = pGrid->GetFixedColumnCount(); nCol < nMaxCol; nCol++) {
//		pGrid->SetCellBackColor(row, nCol, ::GetSysColor(COLOR_HIGHLIGHT));
//		pGrid->SetCellTextColor(row, nCol, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
//		if ((m_nSelectRow > 0) && (row != m_nSelectRow)) {
//			pGrid->SetCellBackColor(m_nSelectRow, nCol, ::GetSysColor(COLOR_WINDOW));
//			pGrid->SetCellTextColor(m_nSelectRow, nCol, ::GetSysColor(COLOR_WINDOWTEXT));
//		}
//	}
//	m_nSelectRow = row;
//	Invalidate(FALSE);
//KANG 22.06.17 Delete End

	if (col == en_ListCol_Link) {
		if (m_pImageViewerDlg == NULL) {
			m_pImageViewerDlg = new CImageViewerDlg();
			m_pImageViewerDlg->Create(IDD_DLG_IMAGE_VIEWER, this);

			CRect rcTmp, rcPos;
			int nWidth, nHeight;
			m_pSearchList->GetWindowRect(rcTmp);
			if (rcTmp.Width() > rcTmp.Height()) {
				nWidth = (int)(rcTmp.Height() * 4 / 5);
				nHeight = nWidth;
			}
			else {
				nHeight = (int)(rcTmp.Width() * 4 / 5);
				nWidth = nHeight;
			}
			rcPos.top = (int)((rcTmp.top + rcTmp.bottom) / 2) - (int)(nHeight / 2);
			rcPos.left = (int)((rcTmp.left + rcTmp.right) / 2) - (int)(nWidth / 2);
			rcPos.bottom = rcPos.top + nHeight;
			rcPos.right = rcPos.left + nWidth;

			m_pImageViewerDlg->MoveWindow(rcPos);
		}
		if (m_pImageViewerDlg != NULL) {
			pList = GetSearchListData(row - 1);
		//KANG 22.06.17 Modify Start
		//	m_pImageViewerDlg->ShowWindow(SW_SHOW);
		//	m_pImageViewerDlg->SetImageFile(pList->strImageFileName);
			if (pList != NULL) {
				m_pImageViewerDlg->ShowWindow(SW_SHOW);
				m_pImageViewerDlg->SetImageFile(pList->strImageFileName);
			}
		//KANG 22.06.17 Modify End
		}
	}
//KANG 22.06.17 Delete Start
//	else {
//		if (m_pImageViewerDlg != NULL) {
//			m_pImageViewerDlg->ShowWindow(SW_HIDE);
//		}
//	}
//KANG 22.06.17 Delete End
//KANG 22.06.17 Add Start	
	pGrid->SetFocusCell(row, 0);
//KANG 22.06.17 Add End
}

void CHistoryDlg::OnMouseDblClickSearchList(NMHDR* pNMHDR, LRESULT* pResult)
{

}

//KANG 22.06.17 Add Start
void CHistoryDlg::OnCellChangingSearchList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	NM_GRIDVIEW* pNmgv = (NM_GRIDVIEW*)pNMHDR;
	CGridCtrl* pGrid = m_pSearchList;
	CDefectSearchList* pList = NULL;

	int row = pNmgv->iRow;
	int col = pNmgv->iColumn;
	int nMaxCol = pGrid->GetCols() - 1;
	
	pGrid->SetFocusCell(row, 0);

	if ((row != m_nSelectRow) && (row > pGrid->GetFixedRowCount() - 1)) {
		pGrid->SetRedraw(FALSE);
		for (int nCol = pGrid->GetFixedColumnCount(); nCol < nMaxCol; nCol++) {
			pGrid->SetCellBackColor(row, nCol, ::GetSysColor(COLOR_HIGHLIGHT));
			pGrid->SetCellTextColor(row, nCol, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
			// 22.06.20 Ahn Modify Start
			//if ( (m_nSelectRow > 0) && (row != m_nSelectRow)) {
			if ((m_nSelectRow > 0) && (row != m_nSelectRow) && (m_nSelectRow < pGrid->GetRows())) {
			// 22.06.20 Ahn Modify Start
				pGrid->SetCellBackColor(m_nSelectRow, nCol, ::GetSysColor(COLOR_WINDOW));
				pGrid->SetCellTextColor(m_nSelectRow, nCol, ::GetSysColor(COLOR_WINDOWTEXT));
			}
		}
		pGrid->SetRedraw(TRUE);
		m_nSelectRow = row;
	}

	if (m_pImageViewerDlg != NULL) {
		m_pImageViewerDlg->ShowWindow(SW_HIDE);
	}
}
//KANG 22.06.17 Add End

int CHistoryDlg::AddDefectSearchList(LPCTSTR lpszSerchFileName)
{
	int nRet = 0;

	CString strFileName = lpszSerchFileName;
	CString strTemp;
	int nIndex = 0;
	CFile binFile;

	if (binFile.Open(strFileName, CFile::modeRead | CFile::shareDenyNone) != 0) {
		int bytes;
		CDefectInfo DefInfo;

		while (1) {
			bytes = binFile.Read(&DefInfo, sizeof(CDefectInfo));
			if (bytes == sizeof(CDefectInfo)) {

				//if (DefInfo.nRank != JUDGE_NG) continue; // 22.06.29 Ahn Add Start
				CDefectSearchList* pList = new CDefectSearchList;
				pList->nNo = ++nIndex;
				pList->tLotStartTime = DefInfo.sysTime;
				strTemp.Format(_T("%s"), DefInfo.szLotID);
				pList->strLotID = strTemp;
				strTemp.Format(_T("%s"), DefInfo.szRecipeName);
				pList->strModel = strTemp;
				pList->strLane = AprData.m_System.m_strMachineID ;//_T("----");
				pList->nCell = DefInfo.nTabNo;
				pList->nTopBottom = DefInfo.nHeadNo;
				pList->nType = DefInfo.nType;
				pList->nRank = DefInfo.nRank;
				// 22.06.05 Ahn Add Start
				pList->dDefRadius = 0.0;
				pList->dDefTheta = 0.0;
				// 22.06.05 Ahn Add End
				// 22.06.02 Ahn Modify Start
				if (pList->nType == en_ModeSurface) {
					pList->dXSize = DefInfo.dSizeX;
					pList->dYSize = DefInfo.dSizeY;
				}
				else {
					pList->dXSize = DefInfo.dJudgeSize;
					pList->dYSize = DefInfo.dSizeY;
				}
				// 22.06.02 Ahn Modify End
				// 22.06.03 Ahn Add Start
				pList->bMarking = DefInfo.bMarking;
				// 22.06.03 Ahn Add End
				strTemp.Format(_T("%s"), DefInfo.szImgFileName);
				pList->strImageFileName = strTemp;
				AddSearchListData(pList);
			//KANG 22.06.17 Add Start
				if (m_nSearchMethod == 0) {
					AddGraphHistData(DefInfo.nTabNo, DefInfo.nType, DefInfo.dJudgeSize);
				}
			//KANG 22.06.17 Add End
			}
			else {
				break;
			}
		}
	}
	binFile.Close();

	return nRet;
}
//KANG 22.05.24 Add End

// 22.06.02 Ahn Modify Start
void CHistoryDlg::OnCbnSelchangeCmbTimeStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 시간 정보가 음수가 되지 않도록 체크
	UpdateData(TRUE);
	CString strStartTime;
	CString strEndTime;
	m_cmbTimeStart.GetWindowText(strStartTime) ;
	m_cmbTimeEnd.GetWindowText(strEndTime);

	//strStartTime.
	
}
// 22.06.02 Ahn Modify End

//KANG 22.06.17 Add Start
void CHistoryDlg::AddGraphHistData(int nTab, int nType, double dData)
{
	int i = 0;
	int nDataIndex = 0;
	for (i = 0; i < MAX_DEFECT_IN_TAB_NUM; i++) {
		if (nType == en_ModeSurface) {
			nDataIndex = i + MAX_DEFECT_IN_TAB_NUM;
		}
		else {
			nDataIndex = i;
		}
		if (m_pGraphHist->GetData(nDataIndex, nTab) == 0) {
			m_pGraphHist->SetData(nDataIndex, nTab, (int)dData);
			if (m_nGraphHistHorSize < (nTab + 1)) {
				m_nGraphHistHorSize = nTab + 1;
			}
			if (m_nGraphHistVerSize < (int)dData + 1) {
				m_nGraphHistVerSize = (int)dData + 1;
			}
			break;
		}
	}
}

int CHistoryDlg::GetGraphIntervalSize(int nNum)
{
	int i = nNum;
	int nCount = 0;
	while (i) {
		i /= 10;
		nCount++;
	}
	double nTemp = (double)nNum;
	for (i = 1; i < nCount; i++) {
		nTemp /= 10.0;
	}
	nNum = (int)(nTemp + 0.5);
	for (i = 1; i < nCount; i++) {
		nNum *= 10;
	}

	return nNum;
}
//KANG 22.06.17 Add End


void CHistoryDlg::OnBnClickedBtnClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (MessageBox(_LANG(_T("검색 내용을 모두 제거하시겠습니까?"), _T("Do you want clear searched list.")), _T("Clear searched list"), MB_OKCANCEL) == IDOK) {
		SearchListClear();
		MakeGridSearchList();
	}
}
