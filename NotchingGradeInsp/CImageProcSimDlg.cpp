// CImageProcSimDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"
// #include "NotchingGradeInspDoc.h"

#include "CImageProcSimDlg.h"
#include "afxdialogex.h"
#include "CImageDispDlg.h"
#include "BitmapStd.h"
#include "GlobalDef.h"
#include "CImageProcess.h"
#include "TimeAnalyzer.h"
#include "Win32File.h"
#include "GlobalData.h"
#include "CTabRsltInfo.h"
#include "CDefectDataCtrl.h"
#include "CRecipeSettingDlg.h"
#include "CRecipeCtrl.h"
#include "CHistogramDlg.h"
#include "SigProc.h"
#include "GlobalData.h" // 22.05.04
#include "CExcelFileSaveDlg.h" //
#include "CExcelResultFileCtrl.h"// 22.06.04
#include "CJoinImageDlg.h"
#include "CDebugImageAcqDlg.h"	// 22.08.09 Ahn Add
#include "SetValue.h" //  22.09.16 Ahn Add

#include "CResultThread.h" // 23.02.06 Ahn Test 
#include "LogDisplayDlg.h"



// CImageProcSimDlg 대화 상자

IMPLEMENT_DYNAMIC(CImageProcSimDlg, CDialogEx)

CImageProcSimDlg::CImageProcSimDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_INSP_SIM, pParent)
	, m_bUsePreFilter(FALSE)
	, m_strEdFileName(_T(""))
	, m_chkMeasure(FALSE)
{
	m_strProcTime = _T("");
	m_bChkDIspBoundary = FALSE;
	m_bChkDefLink = FALSE;

	for (int i = 0; i < en_MaxProcImage; i++) {
		m_pBmpStd[i] = NULL;
	}
	m_pImageDispDlg = NULL;

	m_pBmpStd[en_OrgImage] = new CBitmapStd();

	m_pVecBlockAll = NULL;

	m_bModeTop = TRUE ;
	m_pRecipeInfo = NULL ;
	m_pRecipeSetDlg = NULL;
	m_nSelectDefNo = 0;
	m_pHistoDlg = NULL;

	m_strListFiles.RemoveAll() ;
	m_nNowPos = NULL;
	m_nSelImageNo = 0; // 22.01.17 Ahn Add 

}

CImageProcSimDlg::~CImageProcSimDlg()
{

	if (m_pImageDispDlg != NULL) {
		delete m_pImageDispDlg;
		m_pImageDispDlg = NULL;
	}

	for (int i = 0; i < en_MaxProcImage; i++) {
		if (m_pBmpStd[i] != NULL) {
			delete m_pBmpStd[i];
			m_pBmpStd[i] = NULL;
		}
	}

	if (m_pVecBlockAll != NULL) {
		m_pVecBlockAll->clear();
		delete m_pVecBlockAll;
		m_pVecBlockAll = NULL;
	}

	if (m_pFontGrid != NULL) {
		delete m_pFontGrid;
		m_pFontGrid = NULL;
	}

	if (m_pRecipeInfo != NULL) {
		delete m_pRecipeInfo;
		m_pRecipeInfo = NULL;
	}

	if (m_pRecipeSetDlg != NULL) {
		delete m_pRecipeSetDlg;
		m_pRecipeSetDlg = NULL;
	}

	if (m_pHistoDlg != NULL) {
		delete m_pHistoDlg;
		m_pHistoDlg = NULL;
	}
}

void CImageProcSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_TAC_INFO, m_strProcTime);
	DDX_Check(pDX, IDC_CHK_DISP_LINE, m_bChkDIspBoundary);
	DDX_Control(pDX, IDC_GRID_CTRL_DEF, m_GridCtrlDef);
	DDX_Check(pDX, IDC_CHK_DEF_LINK, m_bChkDefLink);
	DDX_Check(pDX, IDC_CHK_PRE_FILTER, m_bUsePreFilter);
	DDX_Control(pDX, IDC_GRID_RECIPE_SET, m_GridCtrlRecipe);
	DDX_Text(pDX, IDC_ED_IMAGE_FILE_NAME, m_strEdFileName);
	DDX_Check(pDX, IDC_CHK_DISP_DEFECT, m_bDispDefect);
	DDX_Check(pDX, IDC_CHK_MEASURE, m_chkMeasure);
	DDX_Control(pDX, IDC_CMB_RECIPE_SELECT, m_cmbRecipeSelect);
}


BEGIN_MESSAGE_MAP(CImageProcSimDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILE_OPEN, &CImageProcSimDlg::OnBnClickedBtnFileOpen)
	ON_BN_CLICKED(IDC_BTN_PROC_ALL, &CImageProcSimDlg::OnBnClickedBtnProcAll)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_IMG_ORG, &CImageProcSimDlg::OnBnClickedBtnImgOrg)
	ON_BN_CLICKED(IDC_BTN_IMG_PROC1, &CImageProcSimDlg::OnBnClickedBtnImgProc1)
	ON_BN_CLICKED(IDC_BTN_IMG_PROC2, &CImageProcSimDlg::OnBnClickedBtnImgProc2)
	ON_BN_CLICKED(IDC_BTN_IMG_PROC3, &CImageProcSimDlg::OnBnClickedBtnImgProc3)
	ON_BN_CLICKED(IDC_BTN_IMG_PROC4, &CImageProcSimDlg::OnBnClickedBtnImgProc4)
	ON_BN_CLICKED(IDC_BTN_IMG_PROC5, &CImageProcSimDlg::OnBnClickedBtnImgProc5)
	ON_BN_CLICKED(IDC_BTN_IMG_PROC6, &CImageProcSimDlg::OnBnClickedBtnImgProc6)
	ON_BN_CLICKED(IDC_BTN_IMG_PROC7, &CImageProcSimDlg::OnBnClickedBtnImgProc7)
	ON_BN_CLICKED(IDC_BTN_DETECT_LINE, &CImageProcSimDlg::OnBnClickedBtnDetectLine)
	ON_BN_CLICKED(IDC_CHK_DISP_LINE, &CImageProcSimDlg::OnBnClickedChkDispLine)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_CLICK, IDC_GRID_CTRL_DEF, OnClickGridCtrlDef)
	//ON_NOTIFY(NM_CLICK, IDD_DLG_INSP_SIM, OnClickGridCtrlRecipeSet) // 22.09.16 Ahn Add
	ON_BN_CLICKED(IDC_BTN_DIVISION_TAB, &CImageProcSimDlg::OnBnClickedBtnDivisionTab)
	ON_BN_CLICKED(IDC_BTN_RECIPE_SET, &CImageProcSimDlg::OnBnClickedBtnRecipeSet)
	ON_BN_CLICKED(IDC_BTN_TEST, &CImageProcSimDlg::OnBnClickedBtnTest)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_RECIPE_SET, OnClickGridCtrlRecipeSet)  // 22.09.16 Ahn Add

	ON_BN_CLICKED(IDC_CHK_MEASURE, &CImageProcSimDlg::OnBnClickedChkMeasure)
	ON_BN_CLICKED(IDC_CHK_DISP_DEFECT, &CImageProcSimDlg::OnBnClickedChkDispDefect)
	ON_BN_CLICKED(IDC_BTN_HISTOGRAM, &CImageProcSimDlg::OnBnClickedBtnHistogram)
	ON_BN_CLICKED(IDC_BTN_INSP_SPETER, &CImageProcSimDlg::OnBnClickedBtnInspSpeter)
	ON_BN_CLICKED(IDC_BTN_CROP_SAVE, &CImageProcSimDlg::OnBnClickedBtnCropSave)
	ON_BN_CLICKED(IDC_BTN_ATTACH_IMG, &CImageProcSimDlg::OnBnClickedBtnAttachImg)
	ON_CBN_SELCHANGE(IDC_CMB_RECIPE_SELECT, &CImageProcSimDlg::OnCbnSelchangeCmbRecipeSelect)
	ON_WM_SHOWWINDOW()
	ON_CBN_DROPDOWN(IDC_CMB_RECIPE_SELECT, &CImageProcSimDlg::OnCbnDropdownCmbRecipeSelect)
	ON_BN_CLICKED(IDC_BTN_RESET_COUNT, &CImageProcSimDlg::OnBnClickedBtnResetCount)
END_MESSAGE_MAP()


// CImageProcSimDlg 메시지 처리기


void CImageProcSimDlg::OnBnClickedBtnFileOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//KANG 22.01.07 Modify Start
	TCHAR szFilter[256] = _T("Image Files (*.bmp;*.JPG;*.JPEG)|*.bmp;*.JPG;*.JPEG|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("bmp"), m_strCropImagePath, OFN_READONLY, szFilter, this);
	if (dlg.DoModal() == IDOK) {
		m_strSrcFileName = dlg.GetPathName();
		LoadImageFile(m_strSrcFileName);
	}
//KANG 22.01.07 Modify End
}

//KANG 22.01.07 Add Start
void CImageProcSimDlg::LoadImageFile(LPCTSTR lpszFileName)
{
	CString strFileName = lpszFileName;
	if (strFileName.GetLength() > 0) {
		if (m_strCropImagePath.GetLength() <= 0) {
			m_strCropImagePath = _T("");
		}

		m_bLoadImage = FALSE;
		m_strSrcFileName = strFileName;
		m_strCropImagePath = m_strSrcFileName.Left(m_strSrcFileName.ReverseFind('\\') + 1);
		SetCurrentDirectory(m_strCropImagePath);
		OpenImage();

		CWin32File::GetFileList(m_strCropImagePath, m_strListFiles, _T("*"));
		POSITION pos = m_strListFiles.GetHeadPosition();
		CString strOrgFile = m_strSrcFileName;
		strOrgFile.Replace(m_strCropImagePath, _T(""));
		do {
			CString strFile = m_strListFiles.GetNext(pos);
			if (strFile.Compare(strOrgFile) == 0) {
				m_nNowPos = pos;
				break;
			}
		} while (pos != NULL);
	}
}
//KANG 22.01.07 Add End

void CImageProcSimDlg::OpenImage() 
{

	if (m_strSrcFileName.GetLength() <= 0)
	{
		MessageBox(_T("Please select the Image file"));
		return;
	}

	if( (m_strSrcFileName.Find(_T("HeadNo[0]"), 0) > 0) || (m_strSrcFileName.Find(_T("TOP"), 0) > 0) ){ // 22.05.31 Ahn Modify
		m_bModeTop = TRUE;
	}
	else {
		m_bModeTop = FALSE;
	}

	if (m_pBmpStd[en_OrgImage] == NULL) {
		m_pBmpStd[en_OrgImage] = new CBitmapStd(DEFAULT_IMG_SIZE_W, DEFAULT_IMG_SIZE_H, 8);
	}

	int nLocalRet = m_pBmpStd[en_OrgImage]->ReadBitmap(m_strSrcFileName);
	m_pBmpStd[en_OrgImage]->ImageReverse();
	if (nLocalRet < 0) {
		return;
	}
	m_strEdFileName = m_strSrcFileName ;
;
	CSize size = m_pBmpStd[en_OrgImage]->GetImgSize();
	int nBitCnt = m_pBmpStd[en_OrgImage]->GetBitCount();


	m_pImageDispDlg->SetBitmapStd(m_pBmpStd[en_OrgImage], 1);

	for (int i = en_ProcImage1; i < en_MaxProcImage; i++) {
		if (m_pBmpStd[i] != NULL) {
			delete m_pBmpStd[i];
			m_pBmpStd[i] = NULL;
		}
		m_pBmpStd[i] = new CBitmapStd(size.cx, size.cy, nBitCnt);
	}

	m_bLoadImage = TRUE;

	UpdateData(FALSE);
}
void CImageProcSimDlg::SetBitmapStd(CBitmapStd bmpStd)
{
	m_pBmpStd[en_OrgImage]->SetBitmap( bmpStd );
	Invalidate();
}

void CImageProcSimDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	ResizeDlg(cx, cy);
}

void CImageProcSimDlg::ResizeDlg(int cx, int cy)
{
	int nImageLeft = 5;
	int nImageTop = 5;
	int nImageHeigh = cy - 60 ;
	int nImageWidth = cx - (cx / 2);
	if (m_pImageDispDlg != NULL) {
		m_pImageDispDlg->SetWindowPos(NULL, nImageLeft, nImageTop, nImageWidth, nImageHeigh, (SWP_NOZORDER));
	}
	CWnd* pWnd;
	pWnd = (CWnd*)GetDlgItem(IDC_ST_TAC_INFO);
	CRect rect;

	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
		pWnd->SetWindowPos(NULL, nImageLeft, nImageHeigh + 5, rect.Width(), rect.Height(), SWP_NOZORDER);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_IMAGE_FILE_NAME);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
		pWnd->SetWindowPos(NULL, nImageLeft, nImageHeigh + 22, rect.Width(), rect.Height(), SWP_NOZORDER);
	}
	

	pWnd = (CWnd*)GetDlgItem(IDC_CHK_DISP_LINE);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
		pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, 10, rect.Width(), rect.Height(), SWP_NOZORDER);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_CHK_DEF_LINK);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
		pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, 30, rect.Width(), rect.Height(), SWP_NOZORDER);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_CHK_PRE_FILTER);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
		pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, 50, rect.Width(), rect.Height(), SWP_NOZORDER);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_CHK_DISP_DEFECT);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
		pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, 70, rect.Width(), rect.Height(), SWP_NOZORDER);
	}	

	int nGridLeft = nImageWidth + 10;
	int nGridTop = cy / 2;
	int nGridWidth = nImageWidth - 50;
	int nGridHeight = nGridTop - 80 ;

	if ( m_GridCtrlDef.m_hWnd !=NULL ){
		m_GridCtrlDef.SetWindowPos(NULL, nGridLeft, nGridTop, nGridWidth, nGridHeight, SWP_NOZORDER);
	}

	pWnd = (CWnd*)GetDlgItem(IDOK);
	int nOkTop = nGridTop + nGridHeight + 5;
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
		int nOkLeft = nGridLeft + nGridWidth - rect.Width() - 5;
		pWnd->SetWindowPos(NULL, nOkLeft, nOkTop, rect.Width(), rect.Height(), SWP_NOZORDER);
	}

	// Image Disp Button
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_ORG);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rect);
	}

	int nBtnBaseTop = 60;
	int nBtnLeft = cx - 300;
	int nBtnW = 120;
	int nBtnH = 40;
	int nBtnTop;
	int nBtnPitchH = 45;

	int nGroupTop = nBtnBaseTop - 20;
	int nGroupLeft = nBtnLeft - 20;
	int nGroupWidth = nBtnW + 40;
	int nGroupHeight = (nBtnPitchH * 8) + 30;
	pWnd = (CWnd*)GetDlgItem(IDC_ST_IMG_SEL_GROUP);
	if (pWnd != nullptr) {
		//	pWnd->GetWindowRect(rect);
		pWnd->SetWindowPos(NULL, nGroupLeft, nGroupTop, nGroupWidth, nGroupHeight, SWP_NOZORDER);
	}

	for (int i = 0; i < 8; i++) {
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_ORG + i);
		nBtnTop = nBtnBaseTop + (nBtnPitchH * i);
		if (pWnd != nullptr) {
			pWnd->SetWindowPos(NULL, nBtnLeft, nBtnTop, nBtnW, nBtnH, SWP_NOZORDER);
		}
	}

	int nID;
	nBtnLeft = cx - 150;
	for (int i = 0; i < 9; i++) {
		switch (i) {
		case	0:
			nID = IDC_BTN_FILE_OPEN;
			break;
		case	 1:
			nID = IDC_BTN_PROC_ALL;
			break;
		case	2:
			nID = IDC_BTN_DIVISION_TAB;
			break;
		case	3:
			nID = IDC_BTN_DETECT_LINE;
			break;
		case	4:
			nID = IDC_BTN_RECIPE_SET;
			break;
		case	5 :
			nID = IDC_BTN_TEST;
			break;
		case	6:
			nID = IDC_BTN_INSP_SPETER;
			break;
		case	7 :
			nID = IDC_BTN_CROP_SAVE;
			break;
		case	8 :
			nID = IDC_BTN_ATTACH_IMG;
			break;
		default:
			continue;
		}
		pWnd = (CWnd*)GetDlgItem(nID);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			nBtnTop = nBtnBaseTop + (nBtnPitchH * i);

			pWnd->SetWindowPos(NULL, nBtnLeft, nBtnTop, nBtnW, nBtnH, SWP_NOZORDER);
		}
	}

	int nRecipeGrpuHeight = 0;
	{
		pWnd = (CWnd*)GetDlgItem(IDC_ST_RECIPE_GROUP);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, 90, rect.Width(), rect.Height(), SWP_NOZORDER);
		}
		nRecipeGrpuHeight = rect.Height() ;

		// 22.07.26 Ahn Add Start
		pWnd = (CWnd*)GetDlgItem(IDC_ST_RCP_NAME);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 15, 105, rect.Width(), rect.Height(), SWP_NOZORDER);
		}

		pWnd = (CWnd*)GetDlgItem(IDC_CMB_RECIPE_SELECT);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 150, 105, rect.Width(), rect.Height(), SWP_NOZORDER);
		}
		// 22.07.26 Ahn Add Start

		pWnd = (CWnd*)GetDlgItem(IDC_GRID_RECIPE_SET);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 15, 135, rect.Width(), rect.Height(), SWP_NOZORDER);
		}
	}

	{
		pWnd = (CWnd*)GetDlgItem(IDC_CHK_MEASURE);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, nRecipeGrpuHeight + 110 , rect.Width(), rect.Height(), SWP_NOZORDER);
		}

		pWnd = (CWnd*)GetDlgItem(IDC_BTN_HISTOGRAM);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, nRecipeGrpuHeight + 140 , rect.Width(), rect.Height(), SWP_NOZORDER);
		}

		pWnd = (CWnd*)GetDlgItem(IDC_BTN_RESET_COUNT);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rect);
			pWnd->SetWindowPos(NULL, nImageLeft + nImageWidth + 10, nRecipeGrpuHeight + 170, rect.Width(), rect.Height(), SWP_NOZORDER);
		}
	}

	Invalidate(FALSE);

}

BOOL CImageProcSimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if (m_pImageDispDlg == NULL) {
		m_pImageDispDlg = new CImageDispDlg(this);
		m_pImageDispDlg->Create(IDD_DISP_IMAGE_DLG, this);
		m_pImageDispDlg->ShowWindow(SW_SHOW);
	}

	if (m_pVecBlockAll == NULL) {
		m_pVecBlockAll = new CImageProcess::_VEC_BLOCK;
		m_pVecBlockAll->clear();
	}

	if (m_pRecipeInfo == NULL) {
		m_pRecipeInfo = new CRecipeInfo;
		*m_pRecipeInfo = *AprData.m_pRecipeInfo;
	}

	int iEdge = GetSystemMetrics(SM_CYEDGE);
	int nTitleHeight = GetSystemMetrics(SM_CYCAPTION) + iEdge;

	MakeGridData();
	UpdateRecipeGrid(); // 22.09.16 Ahn Add Start

	CRect rcDlg;
	GetWindowRect(rcDlg);
	rcDlg.bottom -= nTitleHeight;
	ResizeDlg(rcDlg.Width(), rcDlg.Height());

	RecipeListLoad();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CImageProcSimDlg::SelectImage(int nImgNo)
{
	if (m_pBmpStd[nImgNo] == NULL) {
		return;
	}
	m_pImageDispDlg->SetBitmapStd(m_pBmpStd[nImgNo], 1);

	m_nSelImageNo = nImgNo ; // 22.01.17 Ahn Add 
}

void CImageProcSimDlg::OnBnClickedBtnImgOrg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_OrgImage);
}


void CImageProcSimDlg::OnBnClickedBtnImgProc1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_ProcImage1);
}


void CImageProcSimDlg::OnBnClickedBtnImgProc2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_ProcImage2);
}


void CImageProcSimDlg::OnBnClickedBtnImgProc3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_ProcImage3);
}


void CImageProcSimDlg::OnBnClickedBtnImgProc4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_ProcImage4);
}


void CImageProcSimDlg::OnBnClickedBtnImgProc5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_ProcImage5);
}


void CImageProcSimDlg::OnBnClickedBtnImgProc6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_ProcImage6);
}

void CImageProcSimDlg::OnBnClickedBtnImgProc7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectImage(en_ProcImage7);
}

void CImageProcSimDlg::OnBnClickedBtnProcAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if ((m_pRecipeInfo == NULL) || (m_pRecipeInfo->m_strRecipeName.GetLength() <= 0))
	{
		MessageBox(_T("레시피를 선택해 주세요."));
		return ;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사 전 선택된 레시피 다시 로드 함
	int nSelNo = m_cmbRecipeSelect.GetCurSel();
	m_cmbRecipeSelect.SetCurSel(nSelNo);

	CString strRcpName;
	m_cmbRecipeSelect.GetWindowText(strRcpName);

	CRecipeCtrl rcpCtrl;
	if (m_pRecipeInfo != nullptr) {
		rcpCtrl.LoadRecipe(m_pRecipeInfo, strRcpName);
	}
	//////////////////////////////////////////////////////////////////////////

	UpdateRecipeGrid(); // 22.09.16 Ahn Add 


	if (m_bModeTop == TRUE)
	{
		// 22.01.17 Ahn Modify Start
		// 22.09.15 Ahn Modify End
		//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
		if (AprData.m_System.m_nMachineMode == CATHODE_MODE)
		{
		// 22.09.15 Ahn Modify End
			ProceTopAll_AreaDiff();
		}
		else
		{ // Negative Mode
			// 23.02.16 Ahn Modify Start
			if (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1)
			{
				ProcTopAll_BrightRoll();
			}
			else
			{
				ProceTopAll_Negative();
			}
			// 23.02.16 Ahn Modify End
		}
		// 22.01.17 Ahn Modify End
	}
	else
	{
		// 22.01.17 Ahn Modify Start
		// 22.09.15 Ahn Modify End
		//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
		if (AprData.m_System.m_nMachineMode == CATHODE_MODE)
		{
		// 22.09.15 Ahn Modify End
			ProceBottomAll_AreaDiff();
		}
		else
		{// Negative Mode
			// 23.02.16 Ahn Modify Start
			if (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_BOTTOM] == 1)
			{
				ProcBottomAll_BrightRoll();
			}
			else
			{
				ProceBottomAll_Negative();
			}
			// 23.02.16 Ahn Modify End
		}
		// 22.01.17 Ahn Modify End
	}

	UpdateData(FALSE);
}

int  CImageProcSimDlg::AddDefectInfo(CImageProcess::_VEC_BLOCK* vecBlockFoilExp, CImageProcess::_VEC_BLOCK* vecBlockDross, int nCamPos, BOOL bModeClear/*=TRUE*/ )
{
	CImageProcess::_VEC_BLOCK* pVecBlockInfo;

	ASSERT(m_pVecBlockAll);
	if (m_pVecBlockAll == NULL) return -1;
	if ( bModeClear == TRUE) {
		m_pVecBlockAll->clear();
	}
	int nMinSize[MAX_INSP_TYPE];
	nMinSize[TYPE_FOILEXP_OUT] = m_pRecipeInfo->nFoilExpOutMinSize[nCamPos];
	nMinSize[TYPE_FOILEXP] = m_pRecipeInfo->nFoilExpInMinSize[nCamPos];

	for (int nType = 0; nType < MAX_INSP_TYPE; nType++)
	{
		switch (nType)
		{
		case TYPE_FOILEXP:
			pVecBlockInfo = vecBlockFoilExp;
			break;
		case TYPE_FOILEXP_OUT:
			pVecBlockInfo = vecBlockDross;
			break;
		default:
			return -1 ;
		}
		if (pVecBlockInfo == NULL) continue; // 22.01.17 Ahn Add 

		int nSize = (int)pVecBlockInfo->size();

 		for (int idx = 0; idx < nSize; idx++)
		{
			CBlockData* pBlockData = &(*pVecBlockInfo)[idx];
			if (pBlockData->bDeleteFlag == TRUE) continue;
			if (pBlockData->nPixelCnt < nMinSize[pBlockData->nType]) continue;

			pBlockData->nDefJudge = CTabRsltInfo::GetDefJudge(m_pRecipeInfo->dFoilExpInNgSize[nCamPos], m_pRecipeInfo->dDefJudgeHeight, pBlockData->dJudgeSize, pBlockData->dHeight );
			
			if (pBlockData->nDefJudge == JUDGE_NG)
			{
				if (pBlockData->nType == en_ModeFoilExp) {
					if ((m_pRecipeInfo->dIgnoreDistance < pBlockData->dDistance) && (m_pRecipeInfo->dIgnoreDistance > 0 )) {
						pBlockData->dJudgeSize = pBlockData->dWidth;
						pBlockData->nDefJudge = JUDGE_OK;
						pBlockData->bDeleteFlag = TRUE;
					}
				}
			}

			if (pBlockData->dJudgeSize == 0.0)
			{
				pBlockData->dJudgeSize = pBlockData->dWidth;
			}
			
			if (pBlockData->bDeleteFlag == FALSE)
			{
				m_pVecBlockAll->push_back(*pBlockData);
			}
		}
	}

	return 0;
}

int CImageProcSimDlg::GetLineLevel(int* nLevel) 
{
	ASSERT(nLevel);
	if (m_bLoadImage == FALSE) return -1;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	BYTE* pRsltPtr2 = m_pBmpStd[en_ProcImage2]->GetImgPtr();

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();

	CImageProcess::FindTabLevel(pImgPtr, nWidth, nHeight, nLevel, m_pRecipeInfo->TabCond, m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);

	return 0;
}


int CImageProcSimDlg::GetTabHeadPos(CSize* pSize, int* pnLevel)
{
	ASSERT(pSize);
	if (pSize == nullptr) return -1;
	if (m_bLoadImage == FALSE) return -1;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	BYTE* pRsltPtr2 = m_pBmpStd[en_ProcImage2]->GetImgPtr();

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();

	int nLevel = -1;

	CImageProcess::VEC_SECTOR vecSec;
	int nLocalRet = 0;
	int nTabLeft;
	int nTabRight;

	// 22.01.06 Ahn Modify Start
	//if (bNegativeMode == TRUE){
	// 22.09.15 Ahn Modify Start
	//if(AprData.m_System.m_nMachineMode == CATHODE_MODE ){
	if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	// 22.09.15 Ahn Modify End
	// 22.01.06 Ahn Modify End
		int nTabFindPos = ( nWidth - 220 ) ;
		vecSec.clear();
		nLocalRet = CImageProcess::FindTab_Negative(pImgPtr, nWidth, nHeight, nTabFindPos, m_pRecipeInfo, &vecSec, &nLevel);
		if (nLocalRet < 0) {
			MessageBox(_T("Tab 찾기 실패"));
			return nLocalRet;
		}

		nTabLeft = vecSec[0].nStartPos;
		nTabRight = vecSec[0].nEndPos;

		// 22.01.12 Ahn Add Start
		// Left & Right Sholder Level find
		
		// 22.01.12 Ahn Add End

	}
	else {
//		CImageProcess::FindTabLevel(pImgPtr, nWidth, nHeight, &nLevel, m_pRecipeInfo->TabCond, m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP], CImageProcess::en_FindLeft);
		CRect rect;
		int nPrjWidth = 2000;
		// 22.09.20 Ahn Add Start
		if (nWidth < 2000) {
			nPrjWidth = nWidth;
		}
		// 22.09.20 Ahn Add End
		rect.left = nWidth - nPrjWidth;
		rect.right = nWidth;
		rect.top = 0;
		rect.bottom = nHeight - 1;
		int nSamplingSize = nHeight / 100;
		int* pnPrj = new int[nPrjWidth];

		// 22.05.30 Ahn Delete Start
		//CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
		//int nBndryElectrode = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP], CImageProcess::en_FindFromLeft) + rect.left ;

		//int nTabFindStPos = nBndryElectrode + m_pRecipeInfo->TabCond.nCeramicHeight - 40;
		//int nTabFindEdPos = nBndryElectrode + m_pRecipeInfo->TabCond.nCeramicHeight;

		//int thMin = (int)(m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2;
		//int thMax = m_pRecipeInfo->TabCond.nCeramicBrightHigh[CAM_POS_TOP];

		//if ((nTabFindStPos < 0) || (nTabFindEdPos > nWidth)) {
		//	MessageBox(_T("Tab Level 이상"));
		//	delete[] pnPrj;
		//	return -2;
		//}
		//CImageProcess::FindTabPos(pImgPtr, nWidth, nHeight, nTabFindStPos, nTabFindEdPos, thMin, thMax, &vecSec);
		// 22.05.30 Ahn Delete End
		// 22.05.30 Ahn Add Start
		int nTabFindStPos = (nWidth - 420);
		int nTabFindEdPos = nWidth - 1 ;
		int thMin = m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] ;
		int thMax = 255;// m_pRecipeInfo->TabCond.nCeramicBrightHigh[CAM_POS_TOP];
		CImageProcess::FindTabPos(pImgPtr, nWidth, nHeight, nTabFindStPos, nTabFindEdPos, thMin, thMax, &vecSec);
		// 22.05.30 Ahn Add End

		// 22.08.09 Ahn Add Start
		//CImageProcess::FindTabLevel(pImgPtr, size.cx, size.cy, &nLevel, pThreadPrm->pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP], CImageProcess::en_FindLeft);
		//CImageProcess::GetTabHeadPos(pImgPtr, size.cx, size.cy, pThreadPrm->pRecipeInfo, nLeft, nRight, nLevel);
		// 22.08.09 Ahn Add End

		CImageProcess::_VEC_TAB_INFO vecTabInfo;
		CImageProcess::ConvertSectToTabInfo(vecSec, &vecTabInfo, 0, m_pRecipeInfo->TabCond);

		if (vecTabInfo.empty()) {
			MessageBox(_T("Tab 찾기 실패"));
			delete[] pnPrj;
			return -1;
		}
		nTabLeft = vecTabInfo[0].nTabLeft;
		nTabRight = vecTabInfo[0].nTabRight;

		// 22.05.09 Ahn Add Start
		rect.top = 100;
		rect.bottom = nTabLeft - m_pRecipeInfo->TabCond.nRadiusH;
		if (rect.bottom < 0) {
			delete[] pnPrj;
			return -3;
		}

		BOOL bUseDarkRoll = (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;
		CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
		nLevel = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP], CImageProcess::en_FindFromRight, bUseDarkRoll);
		nLevel += rect.left;





		// Level2 - Tab Right
		rect.top = nTabRight + m_pRecipeInfo->TabCond.nRadiusH;
		rect.bottom = nHeight - 100;
		if (rect.top < 0 || rect.bottom < rect.top )
		{
			delete[] pnPrj;
			return -4;
		}

		CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
		int nLevel2 = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP], CImageProcess::en_FindFromRight, bUseDarkRoll);
		nLevel2 += rect.left;


		nLevel = (nLevel + nLevel2) / 2;



		/*
		int nCenterPos = (int)((nTabLeft + nTabRight) / 2);


		CRect rcPrj;
		int* pnPrjData;
		pnPrjData = new int[nWidth];
		memset(pnPrjData, 0x00, sizeof(int) * nWidth);
		rcPrj.top = nCenterPos - 20;
		rcPrj.bottom = nCenterPos + 20;
		rcPrj.left = 0;
		rcPrj.right = nWidth;



		int nCount = CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, 10, TRUE);

		BOOL bUseDarkRoll = (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;

		int nUpperBright = nCount * ((m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2);//pyjtest : 주기적인 NG 발생건, 양극에서 이 값 계산으로 인해 기준 Edge 인식 못하는 경우가 발생하는 듯


		nLevel = CImageProcess::FindBoundary_FromPrjData(pnPrjData, nWidth, nUpperBright, CImageProcess::en_FindFromRight, bUseDarkRoll);
		*/



		delete[] pnPrj;
	}

	*pnLevel = nLevel;

	pSize->cx = nTabLeft;
	pSize->cy = nTabRight;

	long lTime = cta.WhatTimeIsIt();
	strTime.Format("MeanProc[%d], ", lTime);
	//int nLevel, nTabLeft, nTabRight;
	//CTabInfo TabInfo;
	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strTime;

	return 0;
}

// 22.04.20 Ahn Add Start
int CImageProcSimDlg::FileNameFromTabNo(CString strFileName)
{
	int nRet = -1;
	CString strFind = _T("TabNo[");
	int nFindStrSize = strFind.GetLength();
	int nStartPos = strFileName.Find(_T("TabNo[")) + nFindStrSize;
	int nStrLen = strFileName.GetLength();

	int nEndPos;
	int nCount = 0;
	for (int i = nStartPos; i < nStrLen; i++, nCount++) {
		if (strFileName.GetAt(i) == ']') {
			nEndPos = i;
			break;
		}
	}

	CString strNo = strFileName.Mid(nStartPos, nCount);
	nRet = atoi(strNo.GetBuffer());

	return nRet;
}

int CImageProcSimDlg::FileNameFromHeadNo(CString strFileName)
{
	int nRet = -1;
	CString strFind = _T("HeadNo[");
	int nFindStrSize = strFind.GetLength();
	int nStartPos = strFileName.Find(_T("HeadNo[")) + nFindStrSize;
	int nStrLen = strFileName.GetLength();

	int nEndPos;
	int nCount = 0;
	for (int i = nStartPos; i < nStrLen; i++, nCount++) {
		if (strFileName.GetAt(i) == ']') {
			nEndPos = i;
			break;
		}
	}

	CString strNo = strFileName.Mid(nStartPos, nCount);
	nRet = atoi(strNo.GetBuffer());

	return nRet;
}
// 22.04.20 Ahn Add End

// 초기 용도와 다르게 현재 Directiory 내의 모든 Image를 검사하는 버튼
void CImageProcSimDlg::OnBnClickedBtnDetectLine()
{
	// 22.08.09 Ahn Add Start
	CDebugImageAcqDlg dlg;
	dlg.DoModal();
	// 22.08.09 Ahn Add End

	//if (m_bLoadImage == FALSE) return ;


// 22.07.14 Ahn Add Start
//	ProcessHalfSlittingInsp() ;
// 22.07.14 Ahn Add End

#if defined( TEST_RESULT_PRINT )
	// 결과 저장
	CWin32File wFile;
	CStringList strList;
	CString strExtName ;
	CString strFileName;
	CString strImgFileNmae;
	CString strRsltFile;

	//strFileName = _T("Result.csv");
	wFile.GetFileList(m_strCropImagePath, strList, _T("*.jpg"));
	POSITION pos;
	pos = strList.GetHeadPosition();

	typedef std::vector <CTabRsltBase*> VEC_SIM_TEST ;
	VEC_SIM_TEST vec_rst;
	vec_rst.clear();

	for (; pos != NULL; ) {
		strFileName = strList.GetNext(pos);
		int nTabNo = FileNameFromTabNo(strFileName);
		int m_nHeadNo = FileNameFromHeadNo(strFileName);
		//vec_rslt
		CTabRsltBase* pData = new CTabRsltBase;
		pData->m_nTabNo = nTabNo ;
		pData->m_nHeadNo = m_nHeadNo;
		strcpy_s(pData->m_chImagePath, strFileName.GetBuffer(0));		
		vec_rst.push_back(pData);
	}

	// sorting ;
	CTabRsltBase* pTemp;
	int nSize = (int)vec_rst.size();
	for (int i = 0; i < nSize - 1; i++) {
		for (int j = i + 1; j < nSize; j++) {
			if (vec_rst[i]->m_nTabNo > vec_rst[j]->m_nTabNo) {
				pTemp = vec_rst[i];
				vec_rst[i] = vec_rst[j];
				vec_rst[j] = pTemp;
			}
		}
	}

	// ImageProcess ;
	//CString strRsltFile = _T("Result.csv");
	CString strTemp;
	strTemp = m_strCropImagePath;
	strTemp.Replace(_T("\\IMAGE\\NG\\"), _T(""));
	int nFindPos = strTemp.ReverseFind('\\');
	int nLength = strTemp.GetLength()- (nFindPos+1);
	CString strLotID;
	strLotID = strTemp.Right(nLength);

	strRsltFile.Format( _T("%s_Result.csv"), strLotID ) ;

	CString strSavePath = strTemp;
	//CString strTemp2;
	//strTemp2.Format(_T("%s\\"), strLotID);
	strSavePath.Replace(strLotID, _T(""));

	CString strDelFile;
	strDelFile.Format(_T("%s%s"), strSavePath, strRsltFile);
	CWin32File::DeleteFile(strDelFile);

	CString strLine;
	strLine.Format(_T("TabNo,POS,JUDGE,DROSS_OK,DROSS_NG,FOILEXP_OK,FOILEXP_NG,MAX_SIZE\r\n"));
	CWin32File::TextSave1Line(strSavePath, strRsltFile, strLine, _T("at"), FALSE);

	//strSavePath.Replace( )

	for (int i = 0; i < nSize ; i++) {
	//	strLine.Format( _T("%d,%s,%s,%d,%d,%d,%d,"))
		CString strImageFile;
		strImageFile.Format(_T("%s%s"), m_strCropImagePath, vec_rst[i]->m_chImagePath) ;
		LoadImageFile(strImageFile);
//		m_pBmpStd[en_OrgImage]->ReadBitmap(strImageFile);
		CSize size = m_pBmpStd[en_OrgImage]->GetImgSize();
		BYTE* pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
		int nBitCnt = m_pBmpStd[en_OrgImage]->GetBitCount();

		CString strJudge = _T("");
		CString strPos;
		if (vec_rst[i]->m_nHeadNo == CAM_POS_TOP) {
			strPos = _T("TOP");
			ProceTopAll_AreaDiff();
		}
		else {
			ProceBottomAll_AreaDiff();
			strPos = _T("BOTTOM");
		}

		char szJudge[2][4] = { "OK","NG" };
		int nSize = (int)m_pVecBlockAll->size();
		int nDefCount[2][2];
		int nJudge = 0 ;
		int nTabNo = vec_rst[i]->m_nTabNo;
		double dMaxSize = 0.0;

		memset(nDefCount, 0, sizeof(int) * 4);
		int nJudgeNG = JUDGE_OK ;
		for (int j = 0; j < nSize; j++) {
			if ((*m_pVecBlockAll)[j].bDeleteFlag == TRUE) continue;
			int nType = (*m_pVecBlockAll)[j].nType;
			nJudge = (*m_pVecBlockAll)[j].nDefJudge;
			double dJudgeSize = (*m_pVecBlockAll)[j].dJudgeSize;
			if (dMaxSize < dJudgeSize) {
				dMaxSize = dJudgeSize;
			}
			nDefCount[nType][nJudge]++;
			if( nJudge == JUDGE_NG ) {
				nJudgeNG = JUDGE_NG;
			}
		}
		strLine.Format( _T("%d,%s,%s,%d,%d,%d,%d,%.1lf\r\n")
			, nTabNo
			, strPos
			, szJudge[nJudgeNG]
			, nDefCount[0][0], nDefCount[0][1], nDefCount[1][0], nDefCount[1][1]
			, dMaxSize );

		CWin32File::TextSave1Line(strSavePath, strRsltFile, strLine, _T("at"), FALSE);

	}

	// delete 
	VEC_SIM_TEST::iterator iter;
	for (iter = vec_rst.begin(); iter != vec_rst.end(); ) {
		iter = vec_rst.erase(iter);
	}
#endif

	//CSize size = m_pBmpStd[en_OrgImage]->GetImgSize();
	//BYTE* pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
	//int nBitCnt = m_pBmpStd[en_OrgImage]->GetBitCount();

	//int nMagnif = 8;
	//int xSize = size.cx ;
	//int ySize = size.cy ;

	//BYTE* pResizePtr;
	//int nReWidth = (xSize / nMagnif);
	//int nReHeight = (ySize / nMagnif);
	//int nReSize = nReWidth * nReHeight;
	//CBitmapStd bmp(nReWidth, nReHeight, nBitCnt);
	//bmp.MakeImageArea(nReWidth, nReHeight, nBitCnt);
	////pResizePtr = bmp.GetImgPtr();
	//pResizePtr = new BYTE[nReSize];
	//CImageProcess::ResizeImage(pImgPtr, pResizePtr, xSize, ySize, nMagnif);

	//bmp.SetImage(nReWidth, nReHeight, pResizePtr);
	//bmp.SetJpegQuality(90);

	//int nDefCnt = (int) m_pVecBlockAll->size();

	//CString strFilePath;
	//strFilePath.Format(_T("%sCrop\\Test.jpg"), m_strCropImagePath);
	//bmp.SaveBitmap(strFilePath);

// 22.01.17 Ahn Add Start 
//	CString strImgName;
//	strImgName.Format(_T("D:\\Test\\TestImage.bmp"));
//	m_pBmpStd[m_nSelImageNo]->SaveBitmap(strImgName);
// 22.01.17 Ahn Add End 
	//SaveResultFile(m_pVecBlockAll);

	//CBitmapStd* pBmpStd;
	//pBmpStd = m_pBmpStd[en_OrgImage];

	//CString strReversSavePath;
	//strReversSavePath = m_strSrcFileName;
	//CString strNewPath;
	//strNewPath.Format(_T("%sReversImg\\"), m_strCropImagePath);
	//strReversSavePath.Replace(m_strCropImagePath, strNewPath);
	////pBmpStd->ImageRevers();
	//pBmpStd->SetJpegQuality(100);
	//CWin32File::CreateDirectory(strNewPath);
	//pBmpStd->SaveBitmap( strReversSavePath );

//
//	CBitmapStd* pBmpStd;
//	pBmpStd = m_pBmpStd[en_OrgImage];
//	BYTE* pImgPtr = pBmpStd->GetImgPtr();
//	BYTE* pBasePtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
//	BYTE* pProctPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
//	BYTE* pResultPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();
//
//	int nWidth, nHeight;
//
//	CSize size;
//	size = pBmpStd->GetImgSize();
//	nWidth = size.cx;
//	nHeight = size.cy;
//	CRect rcProc(0, 0, nWidth, nHeight);
//	CTimeAnalyzer ta;
//	ta.Clear();
//	ta.StopWatchStart();
//
////	rcProc = m_pImageDispDlg->GetMaskRect();
////	CImageProcess::PreFilterCross(pImgPtr, pSobelPtr, nWidth, nHeight, rcProc);
//	int nMeanX = 15 ;
//	int nMeanY = 15 ;
//	int nProcX = 3;
//	int nProcY = 3;
//	double dMagnif = 3.0 ;
//
//	CImageProcess::ImageMean_Part(pImgPtr, pBasePtr, nWidth, nHeight, rcProc, nMeanX, nMeanY, CImageProcess::en_NoFill);
//	CImageProcess::ImageMean_Part(pImgPtr, pProctPtr, nWidth, nHeight, rcProc, nProcX, nProcY, CImageProcess::en_NoFill);
//
//	CImageProcess::DiffProcImage(pProctPtr, pBasePtr, pResultPtr, nWidth, nHeight, rcProc, dMagnif, 200, TRUE);
//
//
//	double dTime = ta.WhatTimeIsIt_Double();
//
//	m_strProcTime.Format(_T("ProcTime : PreFilterCross [%.2lf]"), dTime);
//	UpdateData(FALSE);

	//// 결과 저장
	//CWin32File wFile;
	//CStringList strList;
	//CString strExtName ;
	//CString strFileName;
	//CString strImgFileNmae;
	//strFileName = m_strSrcFileName;
	//strExtName = m_strSrcFileName.Right(4);
	//strImgFileNmae = m_strSrcFileName;
	//strImgFileNmae.Replace(m_strCropImagePath, _T(""));
	//strFileName = _T("Result.csv");
	//int nSize = (int)m_pVecBlockAll->size();
	////CSize size = m_pBmpStd[en_OrgImage]->GetImgSize() ;
	//int nCenterY = size.cy / 2;

	//CString strLine;
	//strLine.Format(_T("FileName : %s\r\n"), strImgFileNmae);
	//wFile.TextSave1Line(m_strCropImagePath, strFileName, strLine, _T("at"), FALSE);

	//int nStart = 0;
	//for (int y = 0; y < 2; y++) {
	//	double dMaxSize = 0.0;
	//	double dMinSize = 99999999.0;
	//	double dSumSize = 0.0;
	//	double dAveSize = 0.0;
	//	double dSize = 0.0;
	//	int nMaxSizePos = 0;
	//	int nCount = 0;
	//	CString strPos;
	//	if (y == 0) {
	//		strPos = _T("Left");
	//	}
	//	else {
	//		strPos = _T("Right");
	//	}
	//	strLine.Format(_T("%s\r\n"), strPos);
	//	wFile.TextSave1Line(m_strCropImagePath, strFileName, strLine, _T("at"), FALSE);

	//	for (int i = nStart; i < nSize; i++) {
	//		if (nSize <= i) break;

	//		CBlockData* pData = &(*m_pVecBlockAll)[i] ;
	//		dSize = (*m_pVecBlockAll)[i].dJudgeSize;

	//		if( (y == 0) && (nCenterY < pData->nTop ) ){
	//			nStart = i ;
	//			break;
	//		}

	//		dSumSize += dSize;
	//		nCount++;
	//		if (dMinSize > dSize) {
	//			dMinSize = dSize;
	//		}
	//		if (dMaxSize < dSize) {
	//			dMaxSize = dSize;
	//			nMaxSizePos = i;
	//		}
	//	}
	//	dAveSize = dSumSize / nCount ;

	//	strLine.Format(_T("Average : %.1lf \r\n"), dAveSize);
	//	wFile.TextSave1Line(m_strCropImagePath, strFileName, strLine, _T("at"), FALSE);
	//	strLine.Format(_T("MinSize : %.1lf \r\n"), dMinSize);
	//	wFile.TextSave1Line(m_strCropImagePath, strFileName, strLine, _T("at"), FALSE);
	//	strLine.Format(_T("MaxSize : %.1lf \r\n"), dMaxSize);
	//	wFile.TextSave1Line(m_strCropImagePath, strFileName, strLine, _T("at"), FALSE);

	//	//nCenterY = ((*m_pVecBlockAll)[nMaxSizePos].nBottom + (*m_pVecBlockAll)[nMaxSizePos].nTop) / 2;
	//	//strLine.Format(_T("MaxSize Position : %d \r\n\r\n"), nCenterY);
	//	// 22.02.16 Ahn Add Start
	//	if (m_pVecBlockAll->size() > 0) {
	//	// 22.02.16 Ahn Add End
	//		strLine.Format(_T("MaxSize Position : %d \r\n\r\n"), (*m_pVecBlockAll)[nMaxSizePos].nMaxPosY);
	//		wFile.TextSave1Line(m_strCropImagePath, strFileName, strLine, _T("at"), FALSE);
	//	}
	//}
		
}



void CImageProcSimDlg::OnBnClickedChkDispLine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);
}

int CImageProcSimDlg::MakeGridData()
{
	{
		CRect rect;
		GetClientRect(rect);
		// 22.08.08 Ahn Modify Start
		//if (m_pFontGrid == NULL) {
		//	CFont* pFont = NULL;
		//	LOGFONT lf;
		//	pFont = m_GridCtrlDef.GetFont();
		//	pFont->GetLogFont(&lf);
		//	CString strFontFace = _T("Arial");
		//	lf.lfHeight = (long)(AprData.m_System.m_nGridFontSize);
		//	lf.lfWeight = FW_NORMAL;
		//	::_tcsnccpy_s(lf.lfFaceName, _countof(lf.lfFaceName), strFontFace.GetBuffer(), _TRUNCATE);
		//	m_pFontGrid = new CFont();
		//	m_pFontGrid->CreateFontIndirect(&lf);
		//	m_GridCtrlDef.SetFont(m_pFontGrid, TRUE);
		//}
		if (m_pFontGrid == NULL) {
			m_pFontGrid = new CFont();
			m_pFontGrid->CreateFont(AprData.m_System.m_nGridFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
				, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
				, FIXED_PITCH, "Arial");
		}
		// 22.08.08 Ahn Add End

		{

			int m_nFixCols = 1;
			int m_nFixRows = 1;
			int m_nCols = en_CimDef_Max;
			int m_nRows = 1;
			int nRow = 0;
			int nCol = 0;

			m_GridCtrlDef.SetAutoSizeStyle();
			m_GridCtrlDef.SetFont(m_pFontGrid, TRUE );

			TRY{
				m_GridCtrlDef.SetRowCount(m_nRows);
				m_GridCtrlDef.SetColumnCount(m_nCols);
				m_GridCtrlDef.SetFixedRowCount(m_nFixRows);
				m_GridCtrlDef.SetFixedColumnCount(m_nFixCols);
			}
				CATCH(CMemoryException, e) {
				e->ReportError();
				return -1;
			}
			END_CATCH

				CString strTitle[en_CimDef_Max] = { _T(" Defect No "),_T(" Type "),_T("Judge"), _T(" Pixel Count "), _T("PosX"), _T("PosY"),_T("Judge Size[㎛]"), _T("Interval[㎛]"), _T("Length[㎛]"), _T("Width[㎛]"), _T("Ave Bright"), _T("MAX Brigit"), _T("Min Bright"), _T("Org Img Ave"), _T("Org Img Max"), _T("Org Img Min")};
			for (nCol = 0; nCol < m_GridCtrlDef.GetColumnCount(); nCol++) {
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = 0;
				Item.col = nCol;
				//strText.Format(_T("%s"), strTitle[nCol]);
				//Item.strText = strText;
				Item.strText = strTitle[nCol];

				//폰트 픽셀 넓이 저정
				CDC* dc = GetDC();
				m_GridCtrlDef.setGrideFontWidth(nCol, dc, strTitle[nCol], 2);

				m_GridCtrlDef.SetItem(&Item);

			}

			// fill rows/cols with text
			for (int nRow = 1; nRow < m_GridCtrlDef.GetRowCount(); nRow++)
			{
				for (int nCol = 0; nCol < m_GridCtrlDef.GetColumnCount(); nCol++)
				{
					CString strText;
					GV_ITEM Item;
					Item.mask = GVIF_TEXT;
					Item.row = nRow;
					Item.col = nCol;

					if (nRow < m_GridCtrlDef.GetFixedRowCount())
						strText.Format(_T("Column %d"), nCol);
					else if (nCol < m_GridCtrlDef.GetFixedColumnCount())
						strText.Format(_T("Row %d"), nRow);
					else
						strText.Format(_T("%d"), nRow * nCol);
					Item.strText = strText;

					//폰트 픽셀 넓이 저정
					CDC* dc = GetDC();
					m_GridCtrlDef.setGrideFontWidth(nCol, dc, strText, 2);

					if (rand() % 10 == 1)
					{
						COLORREF clr = RGB(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);
						Item.crBkClr = clr;             // or - m_Grid.SetItemBkColour(row, col, clr);
						Item.crFgClr = RGB(255, 0, 0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
						Item.mask |= (GVIF_BKCLR | GVIF_FGCLR);
					}
					m_GridCtrlDef.SetItem(&Item);
				}
			}
			// 크기 조정
			m_GridCtrlDef.setGridFullColumnWidthVec();

			//	m_GridCtrlDef.AutoSize();
			m_GridCtrlDef.ShowWindow(SW_SHOW);
		}
	}

	{
		CRect rect;
		GetClientRect(rect);

		int m_nFixCols = 1;
		int m_nFixRows = 1;
		int m_nCols = MAX_INSP_TYPE + 1 ;
		int m_nRows = en_RcpPrm_Max ;
		int nRow = 0;
		int nCol = 0;

		CGridCtrl* pGridCtrl = &m_GridCtrlRecipe;
		pGridCtrl->SetAutoSizeStyle();
		pGridCtrl->SetFont(m_pFontGrid, TRUE);

		TRY{
			pGridCtrl->SetRowCount(m_nRows);
			pGridCtrl->SetColumnCount(m_nCols);
			pGridCtrl->SetFixedRowCount(m_nFixRows);
			pGridCtrl->SetFixedColumnCount(m_nFixCols);
		}
			CATCH(CMemoryException, e) {
			e->ReportError();
			return -1;
		}
		END_CATCH

		CString strTitle[MAX_INSP_TYPE + 1] = { _T(" Sort "),_T("Foil_Exp_in"), _T("Foil_Exp_Out"),_T("Surface") };
		for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;
			Item.col = nCol;

			Item.strText = strTitle[nCol];

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			m_GridCtrlDef.setGrideFontWidth(nCol, dc, strTitle[nCol], 2);

			pGridCtrl->SetItem(&Item);

		}
		int nHeight = 20;
		pGridCtrl->SetRowHeight(0, nHeight);

		// fill rows/cols with text
		for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
		{
			pGridCtrl->SetRowHeight(nRow, nHeight);
			for (int nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++)
			{
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = nRow;
				Item.col = nCol;

				if (nRow < pGridCtrl->GetFixedRowCount())
					strText.Format(_T("Column %d"), nCol);
				else if (nCol < pGridCtrl->GetFixedColumnCount()) {
					switch (nRow) {
					case	en_RcpPrm_ThresOrg :
						strText.Format(_T("Thres Origin"));
						break;
					case	en_RcpPrm_ThresMin :
						strText.Format(_T("Thres Min"));
						break;
					case	en_RcpPrm_MinSize :
						strText.Format(_T("Min Size"));
						break;
					case	en_RcpPrm_InspRange :
						strText.Format(_T("Insp Range"));
						break;
					case	en_RcpPrm_JudgeSize :
						strText.Format(_T("NG Size"));
						break;
					case	en_RcpPrm_Magnification :
						strText.Format(_T("Magnif"));
						break;
					case	en_RcpPrm_UseDefLink :
						strText.Format(_T("Use Link"));
						break;
					case	en_RcpPrm_LinkRangeX :
						strText.Format(_T("Range X"));
						break;
					case	en_RcpPrm_LinkRangeY :
						strText.Format(_T("Range Y"));
						break;
					}
				} else {
					strText.Format(_T(""));
				}
				Item.strText = strText;

				//폰트 픽셀 넓이 저정
				CDC* dc = GetDC();
				pGridCtrl->setGrideFontWidth(nCol, dc, strText, 2);

				pGridCtrl->SetItem(&Item);
			}
		}

		//크기 조정
		pGridCtrl->setGridFullColumnWidthVec();

		//	pGridCtrl->AutoSize();
		pGridCtrl->ShowWindow(SW_SHOW);
	}

	return 0;
}


int CImageProcSimDlg::UpdateGrid()
{
	// TODO: 여기에 구현 코드 추가.
	int m_nRows;
	ASSERT(m_pVecBlockAll);
	if (m_pVecBlockAll == NULL) {
		return -1;
	}
	m_nRows = (int)m_pVecBlockAll->size() + 1 ;


	if (!::IsWindow( m_GridCtrlDef.m_hWnd)) {
		return -1;
	}

	UpdateData();

	int nOldNumRows = m_GridCtrlDef.GetRowCount();

	TRY{
		m_GridCtrlDef.SetRowCount(m_nRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return -1;
	}
	END_CATCH

	UpdateData(FALSE);
	CBlockData* pData;
	//char chMode[CImageProcess::en_ModeMax][12]; // = { "Foil","Dross" };
	CString strMode[en_ModeMax];
	char chJudge[3][6] = { "OK","GRAY","NG"};

	for (int i = 0; i < en_ModeMax; i++) {
		switch (i) {
		case	en_ModeFoilExp :
			strMode[en_ModeFoilExp] = _T("Foil");
			break;
		case	en_ModeFoilExpOut:
			strMode[en_ModeFoilExpOut] = _T("FoilOut");
			break;
		case	en_ModeSurface:
			strMode[en_ModeSurface] = _T("Surface");
			break;
		case	en_ModeFoilBoth :
		// 23.01.13 Ahn Modify Start
		//	strMode[en_ModeSurface] = _T("FoilBoth");
			strMode[en_ModeFoilBoth] = _T("FoilBoth");
		// 23.01.13 Ahn Modify End
			break;
		}
	}


	COLORREF clrBkFoil = RGB( 170,200,170);
	COLORREF clrBkDross = RGB( 200,200,200);
	COLORREF clrBkSurface = RGB(200, 200, 170);
	COLORREF clrBkFoilBoth = RGB(170, 170, 200);
	COLORREF clrBkOK = RGB(60, 200, 60);
	COLORREF clrBkNG = RGB(200, 10, 10);

	for (int nRow = 1; nRow < m_nRows; nRow++) {
		pData = &(*m_pVecBlockAll)[nRow - 1];
		for (int nCol = 0; nCol < m_GridCtrlDef.GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			switch (nCol) {
			case	en_CimDef_Index: 
				strText.Format(_T("%d"), nRow );
				break;
			case	en_CimDef_Type:
				strText.Format(_T("%s"), strMode[ pData->nType ] );
				{
					// 22.05.09 Ahn Modify Start
					//if (pData->nType == TYPE_FOILEXP) {
					//	Item.crBkClr = clrBkFoil;
					//}
					//else {
					//	Item.crBkClr = clrBkDross;
					//}
					switch (pData->nType) {
					case	en_ModeFoilExp:
						Item.crBkClr = clrBkFoil;
						break;
					case	en_ModeFoilExpOut:
						Item.crBkClr = clrBkDross;
						break;
					case	en_ModeSurface:
						Item.crBkClr = clrBkSurface;
						break;
					case	en_ModeFoilBoth :
						Item.crBkClr = clrBkFoilBoth;
						break;
					}
					// 22.05.09 Ahn Modify End
					Item.crFgClr = RGB(0, 0, 0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
					Item.mask |= (GVIF_BKCLR | GVIF_FGCLR);
				}
				break;
			case	en_CimDef_Judge :
				strText.Format(_T("%s"), chJudge[pData->nDefJudge]);
				{
					switch( pData->nDefJudge ) {
					case	JUDGE_OK :
						Item.crBkClr = clrBkOK;
						break;
					case	JUDGE_NG :
						Item.crBkClr = clrBkNG;
						break;
					default :
						Item.crBkClr = clrBkOK;
						break;
					}
					Item.crFgClr = RGB(0, 0, 0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
					Item.mask |= (GVIF_BKCLR | GVIF_FGCLR);
				}
				break;
			case	en_CimDef_Count :
				strText.Format(_T("%d"), pData->nPixelCnt );
				break;
			case en_CimDef_PosX:
				strText.Format(_T("%d"), pData->rcRect.left);
				break;
			case en_CimDef_PosY:
				strText.Format(_T("%d"), pData->rcRect.top);
				break;
			case en_CimDef_JuegeSize:
				strText.Format(_T("%.1lf"), pData->dJudgeSize);
				break;
			// 22.04.15 Ahn Add Start
			case en_CimDef_Distance:	
				strText.Format(_T("%.1lf"), pData->dDistance);
				break;
			// 22.04.15 Ahn Add End
			case	en_CimDef_Width:
				strText.Format(_T("%.1lf"), pData->dWidth );
				break;
			case	en_CimDef_Height:
				strText.Format(_T("%.1lf"), pData->dHeight);
				break;
			case	en_CimDef_BriAve:
				strText.Format(_T("%d"), pData->nBriAve);
				break;
			case	en_CimDef_BriMax:
				strText.Format(_T("%d"), pData->nBriMax);
				break;
			case	en_CimDef_BriMin:
				strText.Format(_T("%d"), pData->nBriMin);
				break;
			case en_CimDef_OrgAve:
				strText.Format(_T("%d"), pData->nOrgBriAve);
				break;
			case en_CimDef_OrgMax:
				strText.Format(_T("%d"), pData->nOrgBriMax);
				break;
			case en_CimDef_OrgMin:
				strText.Format(_T("%d"), pData->nOrgBriMin);
				break;
			default :
				break;
			}

			Item.strText = strText;
			m_GridCtrlDef.SetItem(&Item);
		}
	}
	m_GridCtrlDef.EnsureVisible(m_nRows - 1, 0);
	m_GridCtrlDef.Refresh();	

	return 0;
}

void CImageProcSimDlg::OnClickGridCtrlDef(NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(m_pVecBlockAll);
	if (m_pVecBlockAll == nullptr) return;
	CGridCtrl* pGridCtrl = &(m_GridCtrlDef);
	if (pGridCtrl == NULL) {
		return;
	}
	int	iMouseRow = pGridCtrl->GetMouseRow();
	int	iMouseCol = pGridCtrl->GetMouseCol();
	if (iMouseRow < 1) return;

	int nSize = (int) m_pVecBlockAll->size();
	if (iMouseRow > nSize) {
		MessageBox("범위 밖의 값이 입력되었습니다.");
		return;
	}
	int nIndex = iMouseRow - 1;
	CRect rcDef;
	CBlockData* pData;
	pData = &(*m_pVecBlockAll)[nIndex];
	//rcDef.top = pData->nTop ;
	//rcDef.left = pData->nLeft;
	//rcDef.bottom = pData->nBottom + 1;
	//rcDef.right = pData->nRight + 1;
	//m_pImageDispDlg->SetSelectDefRect(rcDef);
	m_pImageDispDlg->SetSelDefectNo(nIndex);
	int nPosX;
	int nPosY;
	nPosX = pData->rcRect.left + (pData->rcRect.right - pData->rcRect.left);
	nPosY = pData->rcRect.top + (pData->rcRect.bottom - pData->rcRect.top);
	m_pImageDispDlg->PointMove(nPosX, nPosY);

}

void CImageProcSimDlg::OnBnClickedBtnDivisionTab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_nNowPos == NULL) return;

	CString strFileName = m_strListFiles.GetNext(m_nNowPos);
	CString strFilePath;
	strFilePath.Format(_T("%s%s"), m_strCropImagePath, strFileName);

	m_strSrcFileName = strFilePath;
	OpenImage();

	OnBnClickedBtnProcAll();
	//UpdateData(TRUE);
	//if (m_bLoadImage == FALSE) return;
	//CBitmapStd* pStd;
	//pStd = m_pBmpStd[en_OrgImage];
	//BYTE *pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
	//BYTE* pMeanPtr = m_pBmpStd[en_ProcImage7]->GetImgPtr();
	//CRect rcMask = m_pImageDispDlg->GetMaskRect();
	//CSize size = pStd->GetImgSize();

	//CImageProcess::ImageMean_Part(pImgPtr, pMeanPtr, size.cx, size.cy, rcMask, 5, 5);
	//this->OnBnClickedBtnDetectLine(); // 22.01.25 발견 즉시 삭제요망.
}


void CImageProcSimDlg::OnBnClickedBtnRecipeSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bLoadImage == FALSE) return ;

	CTabRsltInfo tabRsltInfo;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pBndLinePtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pRsltPixel = m_pBmpStd[en_ProcImage3]->GetImgPtr();
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	BYTE* pProcPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();

	BYTE* pImgArr[6];
	pImgArr[0] = pMeanPtr;
	pImgArr[1] = pBndLinePtr;
	pImgArr[2] = pDiffPtr;
	pImgArr[3] = pRsltPixel;
	pImgArr[4] = pStdPtr;
	pImgArr[5] = pProcPtr;


	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;
	int nLevel;




	//////////////////////////////////////////////////////////////////////////
	/// pyjtest
	{
		CTabInfo RsvTabInfo;

		int nBndElectrode = 0;
		int nBneElectrodeBtm = 0;

		//양극일 경우 Top 프로젝션 데이터의 바운드리 위치 크기를 가져온다.
		nBndElectrode = CImageProcess::GetBoundaryOfElectorde(pImgPtr, nWidth, nHeight, AprData.m_pRecipeInfo, /*CImageProcess::en_FindFromRight*/CImageProcess::en_FindFromLeft);


		//Tab 정보를 저장할 vector 임시 객체
		CImageProcess::_VEC_TAB_INFO vecTabInfo;
		int nLevel = 0;
		int nBtmLevel = 0;

		//Tab 위치 : 양극일 경우 nBndElectrode 값에 레시피 Tab Condition 카메라 높이
		int nTabFindPos = nBndElectrode + AprData.m_pRecipeInfo->TabCond.nCeramicHeight;


		CImageProcess::DivisionTab_FromImageToTabInfo(pImgPtr, pImgPtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo, 0, 0);

		return;
	}

	//////////////////////////////////////////////////////////////////////////






	if (m_bModeTop == TRUE) {
	//	ProcTopAll_BrightRoll();
		CSize tabPos;
		if (GetTabHeadPos(&tabPos, &nLevel) < 0) return;

		BOOL bReserved = FALSE;
		int nFrameCount = 0;

		CString strTime = _T("Division Time ");
		CString strTact;
		CTimeAnalyzer ctAna;

		CImageProcess::ImageProcessTopSide_BrightRoll(pImgPtr, nWidth, nHeight, m_pRecipeInfo, nLevel, tabPos.cx, tabPos.cy, &tabRsltInfo, TRUE, pImgArr, 4);
	}
	else {
		ProcBottomAll_BrightRoll();
	}

}


BOOL CImageProcSimDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::DestroyWindow();
}


void CImageProcSimDlg::InspectionAuto()
{
	CTabRsltInfo tabRsltInfo;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pBndLinePtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pRsltPixel = m_pBmpStd[en_ProcImage3]->GetImgPtr();
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	BYTE* pProcPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();

	BYTE* pImgArr[6];
	pImgArr[0] = pMeanPtr;
	pImgArr[1] = pBndLinePtr;
	pImgArr[2] = pDiffPtr;
	pImgArr[3] = pRsltPixel;
	pImgArr[4] = pStdPtr;
	pImgArr[5] = pProcPtr;


	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;
	int nLevel;

	if (m_bModeTop == TRUE)
	{
		CSize tabPos;
		if (GetTabHeadPos(&tabPos, &nLevel) < 0) return;

		BOOL bReserved = FALSE;
		int nFrameCount = 0;
		//	BYTE* pBackImage;

		CString strTime = _T("Division Time ");
		CString strTact;
		CTimeAnalyzer ctAna;
		if (AprData.m_System.m_nMachineMode == ANODE_MODE)
		{
			if (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1)
			{
				CImageProcess::ImageProcessTopSide_BrightRoll(pImgPtr, nWidth, nHeight, m_pRecipeInfo, nLevel, tabPos.cx, tabPos.cy, &tabRsltInfo, TRUE, pImgArr, 4);
			}
			else 
			{
				CImageProcess::ImageProcessTopSide_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, nLevel, tabPos.cx, tabPos.cy, &tabRsltInfo, TRUE, pImgArr, 4);
			}
		}
		else
		{
			CImageProcess::ImageProcessTopSide_AreaDiff(pImgPtr, nWidth, nHeight, m_pRecipeInfo, nLevel, tabPos.cx, tabPos.cy, &tabRsltInfo, TRUE, pImgArr, 4);
		}
	}
	else
	{
		if (AprData.m_System.m_nMachineMode == ANODE_MODE)
		{
			if (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_BOTTOM] == 1) {
				CImageProcess::FindLevelBottom_BrightRoll(pImgPtr, nWidth, nHeight, m_pRecipeInfo, &nLevel, CImageProcess::en_FindFromLeft);
				CImageProcess::ImageProcessBottomSide_BrightRoll(pImgPtr, nWidth, nHeight, m_pRecipeInfo, nLevel, &tabRsltInfo, TRUE, pImgArr, 4);
			}
			else {
				CImageProcess::FindLevelBottom_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, &nLevel, CImageProcess::en_FindFromRight);
				CImageProcess::ImageProcessBottomSide_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, nLevel, &tabRsltInfo, TRUE, pImgArr, 4);
			}
		}
		else
		{
			GetLineLevel(&nLevel);
			CImageProcess::ImageProcessBottomSide_AreaDiff(pImgPtr, nWidth, nHeight, m_pRecipeInfo, nLevel, &tabRsltInfo, TRUE, pImgArr, 4);
		}

	}

	m_pVecBlockAll->clear();
	int nSize = (int)tabRsltInfo.m_vecDefInfo.size();

	for (int i = 0; i < nSize; i++)
	{
		CBlockData data;
		data.nPixelCnt = tabRsltInfo.m_vecDefInfo[i]->nSize;
		data.rcRect = tabRsltInfo.m_vecDefInfo[i]->rcPos ;
		data.nType = tabRsltInfo.m_vecDefInfo[i]->nType;
		data.nBriAve = tabRsltInfo.m_vecDefInfo[i]->nAvgBright;
		data.nBriMax = tabRsltInfo.m_vecDefInfo[i]->nMaxBright;
		data.nBriMin = tabRsltInfo.m_vecDefInfo[i]->nMinBright;
		data.nOrgBriAve = tabRsltInfo.m_vecDefInfo[i]->nAveOrgBir;
		data.nOrgBriMax = tabRsltInfo.m_vecDefInfo[i]->nMaxOrgBir;
		data.nOrgBriMin = tabRsltInfo.m_vecDefInfo[i]->nMinOrgBir;
		data.dWidth = tabRsltInfo.m_vecDefInfo[i]->dSizeX;
		data.dHeight = tabRsltInfo.m_vecDefInfo[i]->dSizeY;
		data.dJudgeSize = tabRsltInfo.m_vecDefInfo[i]->dJudgeSize;
		data.dDistance = tabRsltInfo.m_vecDefInfo[i]->dDistance; // 22.04.15 Ahn Add

		int nHeadNo = (m_bModeTop == TRUE) ? CAM_POS_TOP : CAM_POS_BOTTOM;
		data.nDefJudge = CTabRsltInfo::GetDefJudge(m_pRecipeInfo->dFoilExpInNgSize[nHeadNo], m_pRecipeInfo->dDefJudgeHeight, data.dJudgeSize, data.dHeight);

		m_pVecBlockAll->push_back(data);

	}

	UpdateGrid();
}

void CImageProcSimDlg::OnBnClickedBtnTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bLoadImage == FALSE) return;
	

	//////////////////////////////////////////////////////////////////////////
	// 검사 전 선택된 레시피 다시 로드 함
	int nSelNo = m_cmbRecipeSelect.GetCurSel();
	m_cmbRecipeSelect.SetCurSel(nSelNo);

	CString strRcpName;
	m_cmbRecipeSelect.GetWindowText(strRcpName);

	CRecipeCtrl rcpCtrl;
	if (m_pRecipeInfo != nullptr) {
		rcpCtrl.LoadRecipe(m_pRecipeInfo, strRcpName);
	}
	//////////////////////////////////////////////////////////////////////////

	UpdateRecipeGrid();


	// 23.02.03 Ahn Modify Start
	InspectionAuto(); // Foil 노출 검사.	



//	int nLocalRet = ProcessFoilExpInRect();
//	// Image Crop 및 결과 저장.
//	if (nLocalRet >= 0) {
//		SaveCropAndResultSave() ;
//	}
//	// 23.02.03 Ahn Modify End
	
	Invalidate(FALSE);

}

// 23.02.06 Ahn Test start

void CImageProcSimDlg::DrawImage(CDC *pDC, int nWidth, int nHeight, int nMagnif)
{
//	ASSERT(HWnd);
//	ASSERT(pImgPtr);

//	HDC hdc = ::GetDC(HWnd);

//	if (hdc == nullptr) {
//		return;
//	}
//	CDC* pDC = CDC::FromHandle(hdc);
//	CWnd* pWnd = CWnd::FromHandle(HWnd);
	CRect rect;
	//CRect rcImg;
	CRect rcClient;
	//if (pWnd == NULL) {
	//	::ReleaseDC(HWnd, hdc);
	//	return;

	//}
	//if (pDC == NULL) {
	//	::ReleaseDC(HWnd, hdc);
	//	return;
	//}
	//pWnd->GetWindowRect(&rect);
	rcClient.left = 0;
	rcClient.right = nWidth;// rect.Width();
	rcClient.top = 0;
	rcClient.bottom = nHeight; //rect.Height();

//	CBitmapStd* pbmpstd;
	//rcImg.left = 0;
	//rcImg.right = nWidth;
	//rcImg.top = 0;
	//rcImg.bottom = nHeight;
//	pbmpstd = new CBitmapStd(nWidth, nHeight, 8);
//	pbmpstd->SetImage(nWidth, nHeight, pImgPtr);
//	pbmpstd->DrawImageFunction(pDC, &rcClient, &rcImg, SRCCOPY, FALSE);

	double dRateX = (double)rcClient.Width() / (double)(nWidth * nMagnif);
	double dRateY = (double)rcClient.Height() / (double)(nHeight * nMagnif);

	//pbmpstd->drawText()
	int x, y;
	CString strLine;
	COLORREF color;
	if(0) {
		color = RGB(255, 0, 0);
		strLine = _T("JUDGE : NG");
	}
	else {
		color = RGB(0, 255, 0);
		strLine = _T("JUDGE : OK");
	}

	//if (pRsltInfo->m_nHeadNo == CAM_POS_TOP) 
	if( this->m_bModeTop == CAM_POS_TOP ) {
		x = 20;
	}
	else {
		x = 150;
	}
	y = 20;
	int yPitch = 30;

	CResultThread::DrawString(pDC, x, y, color, strLine);

	int nDefCount = (int)m_pVecBlockAll->size();

	// 22.05.11 Ahn Delete Start
	//if (nDefCount > MAX_DISP_DEF_COUNT) nDefCount = MAX_DISP_DEF_COUNT;
	// 22.05.11 Ahn Delete End
	// 22.05.11 Ahn Add Start
	int nDispCnt = 0;
	// 22.05.11 Ahn Add End
	int nLastPosY = y + yPitch;

	VEC_BLOCK_ALL::iterator iter;

	for (iter = m_pVecBlockAll->begin(); iter != m_pVecBlockAll->end(); iter++) {

	//for (int i = 0; i < nDefCount; i++) {
		COLORREF col;

		if (iter->nType == TYPE_SURFACE) continue;

		if (iter->nDefJudge == JUDGE_NG) {
			col = RGB(230, 50, 50);
		}
		else {
			col = RGB(50, 230, 50);
		}
		strLine.Format(_T("Size_F %d:%.1lf um"), nDispCnt + 1, iter->dJudgeSize);
		CResultThread::DrawString(pDC, x, y + ((nDispCnt + 1) * yPitch), col, strLine);

		CRect defRect;
		defRect.top = (int)(iter->rcRect.top * dRateY);
		defRect.bottom = (int)(iter->rcRect.bottom * dRateY);
		defRect.left = (int)(iter->rcRect.left * dRateX);
		defRect.right = (int)(iter->rcRect.right * dRateX);

		int nInfate = 5;
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		CResultThread::DrawDefectRect(pDC, defRect, col);
		nDispCnt++;

	}

	y = 300;
	// 22.06.24 Ahn Modify Start
	int nIdx = 0;
	for (iter = m_pVecBlockAll->begin(); iter != m_pVecBlockAll->end(); iter++, nIdx++ ) {
		// 22.06.24 Ahn Modify End
		COLORREF col;
		
		if (iter->nType != TYPE_SURFACE) continue;

		if (iter->nDefJudge == JUDGE_NG) {
			col = RGB(230, 50, 50);
		}
		else {
			col = RGB(50, 230, 50);
		}

		// Spetter 결함 사이즈 표시
		if (nIdx < MAX_DISP_DEF_COUNT) {
			strLine.Format(_T("Size_S %d:%.1lf um"), (nIdx + nDispCnt), iter->dJudgeSize);
			CResultThread::DrawString(pDC, x, nLastPosY + ((nIdx + nDispCnt) * yPitch), col, strLine);
		}
		nIdx++;

		CRect defRect;
		defRect.top = (int)(iter->rcRect.top * dRateY);
		defRect.bottom = (int)(iter->rcRect.bottom * dRateY);
		defRect.left = (int)(iter->rcRect.left * dRateX);
		defRect.right = (int)(iter->rcRect.right * dRateX);

		int nInfate = 5;
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		CResultThread::DrawDefectRect(pDC, defRect, col);
	}

	//::ReleaseDC(HWnd, hdc);

	//delete pbmpstd;
}
// 23.02.06 Ahn Test End

// 22.06.04 Ahn Add Start
int CImageProcSimDlg::SaveCropAndResultSave()
{
	ASSERT(m_pVecBlockAll);

	int nSize = (int)m_pVecBlockAll->size();
	
	if (nSize <= 0) {
		return 1;
	}
	
	CString strFileName;
	strFileName = m_strSrcFileName;
	strFileName.Replace(m_strCropImagePath, _T(""));
	CString strPosName = _T("");
	CString strCropPath;

	CString strCellNo;
	CString strPos;
	CString strLotID;

	strLotID = GetTokenFromString(strFileName, '_', 4);
	strCellNo = GetTokenFromString(strFileName, '_', 5);
	strPos = GetTokenFromString(strFileName, '_', 6);

	strCropPath.Format(_T("%s%s_%s"), m_strCropImagePath, strCellNo, strPos);
	CWin32File::CreateDirectory(strCropPath);

	CString strRsltFile ;
	CString strCropImgName;

	VEC_BLOCK_ALL::iterator iter;

	CExcelResultFileCtrl::VEC_DEF_SEARCH_LIST vecDefSearchList;
	vecDefSearchList.clear();

	int nCelNo = atoi(strCellNo);
	int nPos = 0;
	if (strPos.Compare(_T("TOP")) == 0) {
		nPos = CAM_POS_TOP;
	}

	int nDefIdx = 0;

	for(iter = m_pVecBlockAll->begin(); iter != m_pVecBlockAll->end(); iter++) {
	//	if (iter->nDefJudge == JUDGE_OK ) continue;

		CDefectSearchList* pData;
		pData = new CDefectSearchList();

		if (iter->nType == en_ModeSurface) {
			pData->dXSize = iter->dWidth;
		}
		else {
			pData->dXSize = iter->dJudgeSize;
		}
		pData->dYSize = iter->dHeight;
		pData->nCell = 0;
		pData->nNo = nDefIdx++ ;
		pData->nTopBottom = nPos;
		pData->nRank = iter->nDefJudge;
		pData->bMarking = FALSE;
		pData->strLotID = strLotID;

		int nCenterX, nCenterY;
		nCenterX = (iter->rcRect.right + iter->rcRect.left) / 2 ;
		nCenterY = (iter->rcRect.top + iter->rcRect.bottom) / 2;
		pData->dDefRadius = (double)nCenterX;
		pData->dDefTheta = (double)nCenterY;


		CString strCromImgName;
		strCromImgName.Format( _T("%s\\DefNo_%d.jpg"), strCropPath,pData->nNo+1 );
		SaveCropImage(nCenterX, nCenterY, strCromImgName);
		pData->strImageFileName = strCromImgName;

		vecDefSearchList.push_back(pData);
	}

	CExcelFileSaveDlg ExcelDlg;
	ExcelDlg.CopyData(vecDefSearchList);

	CString strRsltFilePath;
	strRsltFilePath.Format(_T("%sResultFile_%s_%s.xls"), m_strCropImagePath, strCellNo, strPos );
	ExcelDlg.SetSaveFilePath(strRsltFilePath);

	ExcelDlg.DoModal();

	CExcelResultFileCtrl::VEC_DEF_SEARCH_LIST::iterator iterDef;
	for (iterDef = vecDefSearchList.begin(); iterDef != vecDefSearchList.end(); ) {
		delete *iterDef;
		iterDef = vecDefSearchList.erase(vecDefSearchList.begin());
	}

	vecDefSearchList.clear();

	return 0;
}

int CImageProcSimDlg::SaveCropImage(int nPosX, int nPosY, CString strFileName)
{
	int nRet = 0;

	BYTE* pImgPtr;
	pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
	CSize size;
	size = m_pBmpStd[en_OrgImage]->GetImgSize();
	CRect rcCrop;

	rcCrop.left = nPosX - 128;
	rcCrop.right = nPosX + 128;
	rcCrop.top = nPosY - 128;
	rcCrop.bottom = nPosY + 128;

	CImageProcess::SaveCropImage(pImgPtr, size.cx, size.cy, rcCrop, _T(""), strFileName);

	return nRet;
}

// 22.06.04 Ahn Add End


// 22.06.04 Ahn Add Start
CString CImageProcSimDlg::GetTokenFromString(CString strLine, char chTokinizer, int nPos)
{
	CString strRet = _T("") ;
	std::vector< CSize > vecToken;

	int nLength = strLine.GetLength();
	int nStart = 0;
	for (int i = 0; i < nLength; i++) {
		if (strLine.GetAt(i) == chTokinizer) {
			CSize size;
			size.cx = nStart ;
			size.cy = i ;
			vecToken.push_back(size);
			nStart = i + 1;
		}
	}

	int nTkSize = (int) vecToken.size();
	if (nTkSize <= 0) return _T("");

	int nStartPos;
	int nCount ;
	if (nPos < nTkSize) {
		nStartPos = vecToken[nPos].cx;
		nCount = vecToken[nPos].cy - vecToken[nPos].cx;
		strRet = strLine.Mid(nStartPos, nCount);
	}else if( nPos == nTkSize ){
		nStartPos = vecToken[nPos-1].cx;
		nCount = strLine.GetLength() - ( vecToken[nPos-1].cx ) ;
		strRet = strLine.Right(nCount);

	}
	return strRet ;
}
// 22.06.04 Ahn Add Start


void CImageProcSimDlg::Initial()
{
	m_pVecBlockAll->clear();
}


void CImageProcSimDlg::OnBnClickedChkMeasure()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_pImageDispDlg->set
	UpdateData(TRUE);
	m_pImageDispDlg->SetMeasureMode(m_chkMeasure);
}


void CImageProcSimDlg::OnBnClickedChkAreaSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CImageProcSimDlg::OnBnClickedChkDispDefect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_pImageDispDlg->SetDispDefect(m_bDispDefect);

}


void CImageProcSimDlg::OnBnClickedBtnHistogram()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_bLoadImage == FALSE) return;


	if (m_pHistoDlg == NULL) {
		m_pHistoDlg = new CHistogramDlg(this);
		m_pHistoDlg->Create(IDD_DLG_HISTOGRAM, this);
	}
	m_pHistoDlg->ShowWindow(SW_SHOW);

	//CHistogramDlg dlg;
	BYTE* pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
	CSize szImg = m_pBmpStd[en_OrgImage]->GetImgSize();
	int nWidth = szImg.cx;
	int nHeight = szImg.cy;
	int nHistoArr[256];
	CRect rcRange;
	rcRange = m_pImageDispDlg->GetMaskRect();



	CImageProcess::GetHistoList(pImgPtr, nWidth, nHeight, rcRange, nHistoArr );
	m_pHistoDlg->SetHistoData(nHistoArr, 256);

	//dlg.SetHistoData(nHistoArr);
	//dlg.DoModal();
}



int CImageProcSimDlg::ProceTopAll_AreaDiff()
{
	if (m_bLoadImage == FALSE) return -1;

	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();

	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	BYTE* pRndMeanPtr = m_pBmpStd[en_ProcImage7]->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	CSize tabPos;
	int nLevel;

	if (GetTabHeadPos(&tabPos, &nLevel) < 0) return -1;

	int nOffset = 50;

	// 22.01.05 Ahn Add Start
	int nLeftOffset;	
	int nRightOffset;
	int nMeanLeftOffest;
	int nMeanRightOffset;
	int nTabRoundOffsetR;
	//if (m_pRecipeInfo->bNegativeMode == TRUE) {
	// 22.09.15 Ahn Modify Start
	//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	// 22.09.15 Ahn Modify End
		nLeftOffset = (int)( (double)m_pRecipeInfo->TabCond.nNegCoatHeight  );
		nRightOffset = (int)((m_pRecipeInfo->dFoilOutInspWidth[CAM_POS_TOP] * 1000.0) / AprData.m_System.m_dResolX[CAM_POS_TOP]);
		nMeanLeftOffest = (int)((double)m_pRecipeInfo->TabCond.nNegCoatHeight );
		nMeanRightOffset = 50 ;
		nTabRoundOffsetR = m_pRecipeInfo->TabCond.nNegCoatHeight;
	}
	else {
		// 22.05.09 Ahn Modify Start
		//nLeftOffset = m_pRecipeInfo->TabCond.nCeramicHeight ;
		nLeftOffset = (int)( ( m_pRecipeInfo->dFoilExpInspWidth[CAM_POS_TOP] * 1000.0 ) / AprData.m_System.m_dResolX[CAM_POS_TOP]);
		nRightOffset = (int)( (m_pRecipeInfo->dFoilOutInspWidth[CAM_POS_TOP] * 1000.0 ) / AprData.m_System.m_dResolX[CAM_POS_TOP] );
		// 22.05.09 Ahn Modify End
		nMeanLeftOffest = 70;
		nMeanRightOffset = 70;
		nTabRoundOffsetR = m_pRecipeInfo->TabCond.nRadiusW;
	}
	// 22.01.05 Ahn Add End

	CRect rcAll;
	CRect rcLeft;
	CRect rcRight;
	// 22.01.05 Ahn Modify Start
	//rcAll.left = rcRight.left = rcLeft.left = nLevel - m_pRecipeInfo->TabCond.nCeramicHeight;
	rcAll.left = rcRight.left = rcLeft.left = nLevel - nLeftOffset ;
	// 22.01.05 Ahn Modify End


	rcAll.right = nLevel + (int)(m_pRecipeInfo->TabCond.nRadiusW );
	// 22.05.09 Ahn Modify Start
	//rcRight.right = rcLeft.right = nLevel + 100;
	rcRight.right = rcLeft.right = nLevel + nRightOffset;
	// 22.05.09 Ahn Modify ENd

	CRect rcEdgeMean ;
	rcEdgeMean.bottom = nHeight;
	rcEdgeMean.top = 0;
	// 22.01.05 Ahn Add Start
	rcEdgeMean.left = nLevel - nMeanLeftOffest;
	rcEdgeMean.right = nLevel + nMeanRightOffset;
	// 22.01.05 Ahn Add End
	// 22.06.03 Ahn Modify Start
	if (rcAll.right > nWidth) {
		rcAll.right = nWidth - 1;
	}

	if (rcAll.left < 0) {
		return -1;
	}
	// 22.06.03 Ahn Modify End

	rcAll.top = rcLeft.top = 0;
	rcAll.bottom = rcRight.bottom = nHeight;
	rcLeft.bottom = tabPos.cx + 30;//m_pRecipeInfo->nFoilExpInspWidth[ CAM_POS_TOP ] ; 
	rcRight.top = tabPos.cy - 30;//m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CRect rcLeftRnd;
	CRect rcRightRnd;
	rcLeftRnd = rcLeft;
	rcRightRnd = rcRight;
	rcLeftRnd.right = rcRightRnd.right = nLevel + nTabRoundOffsetR ; // Tab Level 필요
	rcLeftRnd.top = rcLeft.bottom - m_pRecipeInfo->TabCond.nRadiusH;
	rcRightRnd.bottom = rcRightRnd.top + m_pRecipeInfo->TabCond.nRadiusH;

	// Test
	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight);
	CImageProcess::CheckRect(&rcLeftRnd, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRight, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRightRnd, nWidth, nHeight);

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();
	// 22.04.26 Ahn Delete Start
	//int* pnResultArr;
	//pnResultArr = new int[nHeight];
	//memset(pnResultArr, 0, sizeof(int) * nHeight);
	// 22.04.26 Ahn Delete End

	CImageProcess::VEC_ROUND_INFO vecLeftRndInfo;
	CImageProcess::VEC_ROUND_INFO vecRightRndInfo;
	vecLeftRndInfo.clear();
	vecRightRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nMeanSize = MEAN_SIZE_HOR;
	// 22.01.06 Ahn Add Start
	int nHeadBndry;
	// 22.09.15 Ahn Modify Start
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) { // 22.01.06 Ahn Add Start
	if (AprData.m_System.m_nMachineMode == CATHODE_MODE) { 
	// 22.09.15 Ahn Modify End
	// 22.03.30 Ahn Delete Start
	//	CImageProcess::MeanImageDirection(pImgPtr, pMeanPtr, nWidth, nHeight, rcEdgeMean, nMeanSize, DIR_VER);
	// 22.03.30 Ahn Delete End
		nHeadBndry = nLevel + m_pRecipeInfo->TabCond.nTabCeramicHeight;
	}
	else {
		nHeadBndry = nLevel + nTabRoundOffsetR;
	}
	// 22.01.06 Ahn Add End

	rcRightRnd.right = rcLeftRnd.right = rcAll.right = nHeadBndry;


	int nMaskRight = nHeadBndry - 30;


	double dTime = cta.WhatTimeIsIt_Double();
	CString strMeanTime;
	strMeanTime.Format("MeanProc[%.1lf], ", dTime);
	cta.Clear();
	cta.StopWatchStart();

	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeftRnd, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRightRnd, 15, 15, CImageProcess::en_FillBottom);

	BYTE* pProcPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeftRnd, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRightRnd, 3, 3, CImageProcess::en_FillBottom);

	// 22.04.26 Ahn Add Start
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeftRnd);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRightRnd);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);
	// 22.04.26 Ahn Add End

	// 비교 처리용 이미지 
	double dMeanProc = cta.WhatTimeIsIt_Double();
	CString strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	cta.Clear();
	cta.StopWatchStart();

	int nRoundOffset = 50;
	int nEdgeWidth = 10;	// 

	CRect rcTemp;
	rcTemp = rcLeft;
	rcTemp.bottom = rcLeftRnd.top;
	rcTemp.right = rcLeftRnd.right;
	int nCropWidth = rcLeftRnd.Width();
	int nCropHeight = rcLeftRnd.Height();
	int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];

	int  bLocalRet = 0;

	// 22.01.06 Ahn Add Start
	BYTE* pEdgePtr;

	// 22.04.26 Ahn Add Start
	// 22.04.26 Ahn Add End
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	//	pEdgePtr = pMeanPtr;
	//}
	//else 
	{
		pEdgePtr = pMeanPtr; // pProcPtr;
	}
	// 22.01.06 Ahn Add End


	// 23.01.20 Ahn Delete Start
	//if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
	//	bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nEdgeWidth, 2, DIR_VER);
	//}
	//else {
	//bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindLeft);
	//}
	bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindLeft);
	// 23.01.20 Ahn Delete End

	BYTE* pThresPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	BYTE* pPreFltPtr = pImgPtr;

	if (bLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_AreaDiff>>에러 - vecLeftRndInfo Edge found faile, ret = %d"), bLocalRet);
		return -1;
	}
	rcLeftRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, pRsltPtr, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft);
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize <= 0) {
		
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_AreaDiff>>에러 - vecLeftRndInfo Left EdgeLine Vector Size is 0"));

		return -1;
	}
	rcLeft.top = vecLeftRndInfo[0].y;
	rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	if (rcLeft.bottom < nLeftSize - 1) {
		
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_AreaDiff>>에러 - vecLeftRndInfo Left 엣지라인 데이터 사이즈 이상."));

		CString strMsg;
		strMsg.Format(_T("Left 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -1;
	}

	double dEdge2 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	nCropWidth = rcRightRnd.Width();
	nCropHeight = rcRightRnd.Height();
	rcRightRnd.right = nMaskRight;

	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, pRsltPtr, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight);

	rcTemp = rcRight;
	rcTemp.top = rcRightRnd.bottom;
	rcTemp.right = rcRightRnd.right;

	if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecRightRndInfo, nWidth, nHeight, rcTemp, nEdgeWidth, 2, DIR_VER);
	}
	else {
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, pRsltPtr, &vecRightRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindRight);
	}
	if (bLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_AreaDiff>>에러 - vecRightRndInfo Edge found faile, ret = %d."), bLocalRet);
		return -2;
	}

	CImageProcess::SmoothVecRoundData(&vecRightRndInfo, CImageProcess::en_FindRight);

	int nRightSize = (int)vecRightRndInfo.size();
	if (nRightSize <= 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_AreaDiff>>에러 - vecRightRndInfo Right EdgeLine Vector Size is 0"));

		CString strMsg;
		strMsg.Format(_T("Right EdgeLine Vector Size is 0"));
		MessageBox(strMsg);
		return -2;
	}

	rcRight.top = vecRightRndInfo[0].y;
	rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	if (rcRight.bottom < nRightSize - 1) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_AreaDiff>>에러 - vecRightRndInfo Right 엣지라인 데이터 사이즈 이상."));

		CString strMsg;
		strMsg.Format(_T("Right 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -1;
	}
	// 22.04.26 Ahn Delete Start
	//delete[]pnResultArr;
	// 22.04.26 Ahn Delete End

	double dEdge3 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
	roiDross.SetRoiPtr(pThresPtr);
	roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	// 22.03.30 Ahn Delete Start
	//rcLeft.right = nHeadBndry;
	//rcRight.right = nHeadBndry;
	// 22.03.30 Ahn Delete End

	int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	int nFoilOutMaskOffset = m_pRecipeInfo->nFoilOutMaskOffset[CAM_POS_TOP];
	int nFileExpMaskOffset = m_pRecipeInfo->nFoilExpMaskOffset[CAM_POS_TOP];
	int nFoilOutInspWidth = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	int nFoilExpInspWidth = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];

	CRect rcLeftDiff = rcLeft;
	CRect rcRightDiff = rcRight;

	rcLeftDiff.bottom = rcLeftRnd.top;
	rcRightDiff.top = rcRightRnd.bottom;
	rcLeftDiff = rcLeft;
	rcRightDiff = rcRight;

	pPreFltPtr = pProcPtr;

	// 21.10.22 Ahn Add End

	BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];
	// 22.03.30 Ahn Modify Start
	//CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftDiff, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	//CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightDiff, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeft, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRight, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftRnd, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightRnd, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);

	// 22.03.30 Ahn Modify End

	// 22.03.30 Ahn Add Start
	CString strProcDiff;
	double dDiffTime = cta.WhatTimeIsIt_Double();
	strProcDiff.Format("ProcDiff[%.1lf]", dDiffTime);
	cta.Clear();
	cta.StopWatchStart();
	// 22.03.30 Ahn Add End

	// 22.04.20 Ahn Add Start
	CRect rcLeftAll;
	CRect rcRightAll;
	rcLeftAll = rcLeft;
	rcRightAll = rcRight;
	rcLeftAll.right = rcLeftRnd.right;
	rcRightAll.right = rcRightRnd.right;
	// 22.04.20 Ahn Add End

	// 22.04.20 Ahn Modify Start
	roiFoilExp.SetRect(rcLeftAll);
	roiDross.SetRect(rcLeftAll);
	// 22.04.20 Ahn Modify End

	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecLeftRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecLeftRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, nMaskRight, en_ModeFoilExpOut, FALSE);

	// 22.04.20 Ahn Modify Start
	roiFoilExp.SetRect(rcRightAll);
	roiDross.SetRect(rcRightAll);
	// 22.04.20 Ahn Modify End
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecRightRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecRightRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, nMaskRight, en_ModeFoilExpOut, FALSE);


	// 22.03.30 Ahn Modify Start
	double dThTime = cta.WhatTimeIsIt_Double();
	strThres.Format("Threshold[%.1lf]", dThTime);

	cta.Clear();
	cta.StopWatchStart();
	// 22.03.30 Ahn Modify End

	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);

	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());
	}

	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, m_pRecipeInfo->nFoilExpOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

	m_pImageDispDlg->SetBoundary(&vecLeftRndInfo, &vecRightRndInfo);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	dTime = cta.WhatTimeIsIt_Double();
	// 22.03.30 Ahn Modify Start
	CString strLabeling;
	strLabeling.Format("Labeling[%.1lf]", dTime);
	// 22.03.30 Ahn Modify End

	// 22.07.20 Ahn Delete Start
	//if (m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
	//	CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
	//	CImageProcess::BlockLink(&vecBlockDross, m_pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);
	//}
	// 22.07.20 Ahn Delete End

	// 라운드 사이즈 판정.
	//

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) {
		if (nMode == TYPE_FOILEXP_OUT) {
			pVecBlockPtr = &vecBlockDross;
		}
		else {
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			//if (pData->nType == CImageProcess::en_ModeFoilExpOut) {
			//	pData->dJudgeSize = pData->dWidth;
			//	continue;
			//}
			CPoint ptCenter;
			ptCenter.x = (pData->rcRect.right + pData->rcRect.left) / 2;
			ptCenter.y = (pData->rcRect.bottom + pData->rcRect.top) / 2;

			if (rcLeftRnd.PtInRect(ptCenter) == TRUE) {
			// 22.09.20 Ahn Test Start
				CImageProcess::CalcRoundDef_Size_LeftFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcLeftRnd, pData, &vecLeftRndInfo, CImageProcess::en_FindLeft, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
			//	CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcLeftRnd, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
			// 22.09.20 Ahn Test End
			}
			else if (rcRightRnd.PtInRect(ptCenter) == TRUE) {
				CImageProcess::CalcRoundDef_Size_RightFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcRightRnd, pData, &vecRightRndInfo, CImageProcess::en_FindRight, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
			} else {
				// Round 결함이 아닌 경우, Edge에서 거리를 계산, 3 점의 위치를 측정하여 가장 큰 값을 취득함.
				if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
					if (pData->rcRect.top >= vecRightRndInfo[0].y) {
						CImageProcess::CalcSizeToEdgeLine(pData, &vecRightRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
					}
					else {
						CImageProcess::CalcSizeToEdgeLine(pData, &vecLeftRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
					}
				}
			}
		}
	}
	// 22.07.20 Ahn Add Start
	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, m_pRecipeInfo, CAM_POS_TOP);
	//CImageProcess::SortingBlockInfo(&vecBlockMerge);	
	// 22.07.20 Ahn Add End

	// 22.07.20 Ahn Modify Start
	//AddDefectInfo(&vecBlockFoilExp, &vecBlockDross, CAM_POS_TOP);
	AddDefectInfo(&vecBlockMerge, NULL, CAM_POS_TOP);
	// 22.07.20 Ahn Modify End

	//SaveResultFile(m_pVecBlockAll);

	UpdateGrid();

	CString strEdge;
	strEdge.Format(_T(", Edge2 [%.2lf], Edge3 [%.2lf]"), dEdge2, dEdge3);
	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strMeanTime;
	m_strProcTime += strMeanTime2;
	m_strProcTime += strEdge;
	m_strProcTime += strTime;
	// 22.03.30 Ahn Modify Start
	m_strProcTime += strProcDiff;
	m_strProcTime += strThres;
	m_strProcTime += strLabeling;
	// 22.03.30 Ahn Modify End

	return 0;
}

int CImageProcSimDlg::ProceBottomAll_AreaDiff()
{

	if (m_bLoadImage == FALSE) return -1;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();

	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	BYTE* pRsltPtr5 = m_pBmpStd[en_ProcImage7]->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	CSize tabPos;
	int nLevel;
	// 22.01.05 Ahn Modify Start
	//if (m_pRecipeInfo->bNegativeMode == FALSE) {
	// 22.09.15 Ahn Modify End
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	// 22.09.15 Ahn Modify End
		CImageProcess::FindTabLevel(pImgPtr, nWidth, nHeight, &nLevel, m_pRecipeInfo->TabCond, m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
	}
	else {
		//CImageProcess::FindTab_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, NULL, &nLevel);
		CImageProcess::FindLevelBottom_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, &nLevel, CImageProcess::en_FindFromRight);
	}
	// 22.01.05 Ahn Modify Start

	int nOffset = 10;

	if (nLevel < 0) {
		AfxMessageBox(_T("Tab Level 이상 !!"));
		return -1;
	}

	CRect rcAll;
	rcAll.left = nLevel - m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM] ;
	// 22.01.06 Ahn Modify Start
	rcAll.right = nLevel + m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM] + m_pRecipeInfo->TabCond.nNegVGrooveHeight ;
	// 22.01.06 Ahn Modify End
	rcAll.top = 0;
	rcAll.bottom = nHeight;

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();
	// 22.04.26 Ahn Delete End
	//int* pnResultArr;
	//pnResultArr = new int[nHeight];
	//memset(pnResultArr, 0, sizeof(int) * nHeight);
	// 22.04.26 Ahn Delete End

	CImageProcess::VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nMeanSize = MEAN_SIZE_HOR;
	BYTE* pPreFltPtr = pImgPtr;

	cta.Clear();
	cta.StopWatchStart();

	BYTE* pProcPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	// 22.04.26 Ahn Add Start
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);
	// 22.04.26 Ahn Add End

	// 비교 처리용 이미지 
	double dMeanProc = cta.WhatTimeIsIt_Double();
	CString strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresLower[0];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, m_pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	}
	else {
		pDiffPtr = pImgPtr;
	}

	double dTime = cta.WhatTimeIsIt_Double();
	CString strMeanTime;
	strMeanTime.Format("MeanProc[%.1lf], ", dTime);

	cta.Clear();
	cta.StopWatchStart();

	int nRoundOffset = 50;
	int nEdgeWidth = 10;	// 

	// 22.05.30 Ahn Modify Start
	//int nThresBnd = m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM];
	int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
	// 22.05.30 Ahn Modify End
		BYTE* pEdgePtr ;

	// 22.03.30 Ahn Modify Start
	// 22.01.06 Ahn Add Start
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) { 
	//	pEdgePtr = pMeanPtr;
	//}
	//else 
	// 22.03.30 Ahn Modify End
	{
		// 22.04.26 Ahn Modify Start
		//pEdgePtr = pStdPtr; // 22.04.26 
		pEdgePtr = pMeanPtr ; 
		// 22.04.26 Ahn Modify Start
	}
	// 22.01.06 Ahn Add End

	if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM] == CImageProcess::en_FineMaxDiffMode) {
		CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecAllRndInfo, nWidth, nHeight, rcAll, nEdgeWidth, 2, DIR_VER);
	}
	else {
		CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, pRsltPtr, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, CImageProcess::en_BottomSide, nLevel, CImageProcess::en_FindLeft);
	}
	// 22.04.26 Ahn Delete Start
	//delete[]pnResultArr;
	// 22.04.26 Ahn Delete End

	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft );

	dTime = cta.WhatTimeIsIt();
	strTime.Format(_T("Edge Detect[%.1lf], "), dTime);

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	cta.Clear();
	cta.StopWatchStart();

	BYTE* pSmoothPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	{
		pSmoothPtr = pDiffPtr;
	}
	BYTE* pThresPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
	roiDross.SetRoiPtr(pThresPtr);
	roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	roiFoilExp.SetRect(rcAll);
	roiDross.SetRect(rcAll);
	////	roiFoilExp.SetRoiPtr(pThresPtr);
	int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_BOTTOM];
	int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_BOTTOM];
	int nFoilOutMaskOffset = m_pRecipeInfo->nFoilOutMaskOffset[CAM_POS_BOTTOM];
	int nFileExpMaskOffset = m_pRecipeInfo->nFoilExpMaskOffset[CAM_POS_BOTTOM];
	int nFoilOutInspWidth = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	int nFoilExpInspWidth = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];

	CImageProcess::Threshold_RoundMask(pSmoothPtr, &roiFoilExp, &vecAllRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, 0, en_ModeFoilExp, FALSE, CImageProcess::en_TailSide);
	CImageProcess::Threshold_RoundMask(pSmoothPtr, &roiDross, &vecAllRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, 0, en_ModeFoilExpOut, FALSE, CImageProcess::en_TailSide);

	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	//CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());
	}

	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, m_pRecipeInfo->nFoilExpOutMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);

	m_pImageDispDlg->SetBoundary(&vecAllRndInfo, NULL);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	dTime = cta.WhatTimeIsIt_Double();
	strThres.Format(_T("Thres & Labeling[%.1lf]"), dTime);

	// 22.07.20 Ahn Delete Start
	//if (m_pRecipeInfo->bEnableDefectLink[CAM_POS_BOTTOM] == TRUE) {
	//	CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, CAM_POS_BOTTOM);
	//	CImageProcess::BlockLink(&vecBlockDross, m_pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_BOTTOM);
	//}
	// 22.07.20 Ahn Delete End

	// Round 결함이 아닌 경우, Edge에서 거리를 계산, 3 점의 위치를 측정하여 가장 큰 값을 취득함.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) {
		if (nMode == TYPE_FOILEXP_OUT) {
			pVecBlockPtr = &vecBlockDross;
		}
		else {
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			// 22.09.15 Ahn Modify End
			//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
			if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
			// 22.09.15 Ahn Modify End
				CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
			}
		}
	}

	//// 22.08.29 Ahn Add Start
	//switch (pData->nDefPos) {
	//case	CImageProcess::en_LeftRound :
	//	CImageProcess::CalcRoundDef_SizeLeftOnlyBright(pImgPtr, nWidth, nHeight, &vecAllRndInfo, );
	//	break;
	//case	CImageProcess::en_RightRound :
	//	break;
	//}
	//// 22.08.29 Ahn Add End

	// 22.07.20 Ahn Add End
	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, m_pRecipeInfo, CAM_POS_BOTTOM);
	CImageProcess::SortingBlockInfo(&vecBlockMerge);
	// 22.07.20 Ahn Add End
	// 22.07.20 Ahn Modify Start
	//AddDefectInfo(&vecBlockFoilExp, &vecBlockDross, CAM_POS_BOTTOM);
	AddDefectInfo(&vecBlockFoilExp, NULL, CAM_POS_BOTTOM);
	// 22.07.20 Ahn Modify End

	UpdateGrid();

	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strMeanTime;
	m_strProcTime += strTime;
	m_strProcTime += strThres;

	return 0;
}

// 22.01.17 Ahn Add Start
int CImageProcSimDlg::ProceTopAll_Negative()
{
	if (m_bLoadImage == FALSE) return -1;

	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pMaxnPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	BYTE* pErosPtr = m_pBmpStd[en_ProcImage7]->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	CSize tabPos;
	int nLevel;
	if (GetTabHeadPos(&tabPos, &nLevel) < 0) return -1;

	int nOffset = 50;

	// 22.01.05 Ahn Add Start
	int nLeftOffset;
	int nMeanLeftOffest;
	int nMeanRightOffset;
	int nTabRoundOffsetR;

	// 22.05.30 Ahn Modify Start
	//nLeftOffset = (int)((double)m_pRecipeInfo->TabCond.nNegVGrooveHeight );
	nLeftOffset = m_pRecipeInfo->TabCond.nNegVGrooveHeight + m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP] ;
	// 22.05.30 Ahn Modify End
	nMeanLeftOffest = m_pRecipeInfo->TabCond.nNegCoatHeight ;
	nMeanRightOffset = 50;
	nTabRoundOffsetR = m_pRecipeInfo->TabCond.nNegCoatHeight;


	CRect rcAll;
	CRect rcLeft;
	CRect rcRight;
	// 22.01.05 Ahn Modify Start
	//rcAll.left = rcRight.left = rcLeft.left = nLevel - m_pRecipeInfo->TabCond.nCeramicHeight;
	rcAll.left = rcRight.left = rcLeft.left = nLevel - nLeftOffset;
	// 22.01.05 Ahn Modify End


	rcAll.right = nLevel + (int)(m_pRecipeInfo->TabCond.nRadiusW);

	// 23.01.20 Ahn Add Start
	//rcRight.right = rcLeft.right = nLevel + 100;
	int nOutRange = m_pRecipeInfo->TabCond.nNegCoatHeight ;
	rcRight.right = rcLeft.right = nLevel + nOutRange;
	// 23.01.20 Ahn Add ENd

	CRect rcEdgeMean;
	rcEdgeMean.bottom = nHeight;
	rcEdgeMean.top = 0;
	// 22.01.05 Ahn Add Start
	rcEdgeMean.left = nLevel - nMeanLeftOffest;
	rcEdgeMean.right = nLevel + nMeanRightOffset;
	// 22.01.05 Ahn Add End

	// 22.11.07 Ahn Modify Start
	if ((rcAll.right > nWidth) || (rcAll.left < 0)) {
		return -1;
	}
	// 22.11.07 Ahn Modify End

	rcAll.top = rcLeft.top = 0;
	rcAll.bottom = rcRight.bottom = nHeight;
	// 22.05.30 Ahn Modify Start
	//rcLeft.bottom = tabPos.cx - 50; //m_pRecipeInfo->nFoilExpInspWidth[ CAM_POS_TOP ] ; 
	//rcRight.top = tabPos.cy + 50;//m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	rcLeft.bottom = tabPos.cx - 200; //m_pRecipeInfo->nFoilExpInspWidth[ CAM_POS_TOP ] ; 
	rcRight.top = tabPos.cy + 200;//m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	// 22.05.30 Ahn Modify End

	CRect rcLeftRnd;
	CRect rcRightRnd;
	rcLeftRnd = rcLeft;
	rcRightRnd = rcRight;
	rcLeftRnd.right = rcRightRnd.right = nLevel + nTabRoundOffsetR; // Tab Level 필요
	rcLeftRnd.top = rcLeft.bottom - m_pRecipeInfo->TabCond.nRadiusH;
	rcRightRnd.bottom = rcRightRnd.top + m_pRecipeInfo->TabCond.nRadiusH;

	// Test
	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight);
	CImageProcess::CheckRect(&rcLeftRnd, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRight, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRightRnd, nWidth, nHeight);

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();
	int* pnResultArr;
	pnResultArr = new int[nHeight];
	memset(pnResultArr, 0, sizeof(int) * nHeight);

	CImageProcess::VEC_ROUND_INFO vecLeftRndInfo;
	CImageProcess::VEC_ROUND_INFO vecRightRndInfo;
	vecLeftRndInfo.clear();
	vecRightRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nMeanSize = MEAN_SIZE_HOR;
	// 22.01.06 Ahn Add Start
	int nHeadBndry;
	nHeadBndry = nLevel + nTabRoundOffsetR;
	// 22.01.06 Ahn Add End
	// 22.05.30 Ahn Modify Start
	//rcRightRnd.right = rcLeftRnd.right = rcAll.right = nHeadBndry;
	int nInspRangeRight = nLevel + m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	rcRightRnd.right = rcLeftRnd.right = rcAll.right = nInspRangeRight;
	// 22.05.30 Ahn Modify End

	int nMaskRight = nHeadBndry - 30;


	double dTime = cta.WhatTimeIsIt_Double();
	CString strMeanTime;
	strMeanTime.Format(_T("MeanProc[%.1lf], "), dTime);
	cta.Clear();
	cta.StopWatchStart();

	memset(pStdPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);
	//CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeftRnd, 15, 15, CImageProcess::en_FillTop);
	//CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRightRnd, 15, 15, CImageProcess::en_FillBottom);

	BYTE* pProcPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	memset(pProcPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);
	//CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeftRnd, 3, 3, CImageProcess::en_FillTop);
	//CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRightRnd, 3, 3, CImageProcess::en_FillBottom);

	// 22.05.30 Ahn Add Start 
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);

	//CImageProcess::GetMaxImage(pMaxnPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	//CImageProcess::GetMaxImage(pMaxnPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);
	// 22.05.30 Ahn Add End
	// 비교 처리용 이미지 
	double dMeanProc = cta.WhatTimeIsIt_Double();
	CString strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	cta.Clear();
	cta.StopWatchStart();


	int nRoundOffset = 50;
	int nEdgeWidth = 10;	// 

	CRect rcTemp;
	rcTemp = rcLeft;
	rcTemp.bottom = rcLeftRnd.top;
	rcTemp.right = rcLeftRnd.right;
	int nCropWidth = rcLeftRnd.Width();
	int nCropHeight = rcLeftRnd.Height();
	int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];
	int nThresMax = m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; // 22.05.30 Ahn Add

	int  bLocalRet = 0;

	BYTE* pEdgePtr;
	// 22.05.30 Ahn Modify Start
	//pEdgePtr = pProcPtr;
	pEdgePtr = pMeanPtr;
	// 22.05.30 Ahn Modify End

	// 23.01.20 Ahn Modify Start
	BOOL bRollMode ;
	//if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
	//	bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nEdgeWidth, 2, DIR_VER);
	// 23.02.15 Ahn Modify Start
	if( m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1 ){
		bRollMode = TRUE ;
	// 23.02.15 Ahn Modify End
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindLeft);
	}
	else {
		bRollMode = FALSE; // 22.02.14 Ahn Add 
		bLocalRet = CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindLeft);
	}
	// 23.01.20 Ahn Modify End

	BYTE* pThresPtr = m_pBmpStd[en_ProcImage7]->GetImgPtr();
	BYTE* pPreFltPtr = pImgPtr;

	if (bLocalRet < 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_Negative>>에러 - vecLeftRndInfo Edge found faile, ret = %d"), bLocalRet);

		return -1;
	}
	rcLeftRnd.right = nMaskRight;
	// 23.01.20 Ahn Modify Start
	//CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, pRsltPtr, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft, CImageProcess::en_FindFromLeft);
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, pRsltPtr, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft, CImageProcess::en_FindFromLeft, bRollMode );
	// 23.01.20 Ahn Modify End
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize <= 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<ProceTopAll_Negative>>에러 - vecLeftRndInfo Left EdgeLine Vector Size is 0"));
		return -1;
	}
	rcLeft.top = vecLeftRndInfo[0].y;
	rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	if (rcLeft.bottom < nLeftSize - 1) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<ProceTopAll_Negative>>에러 - vecLeftRndInfo Left 엣지라인 데이터 사이즈 이상."));

		CString strMsg;
		strMsg.Format(_T("Left 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -1;
	}

	double dEdge2 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	nCropWidth = rcRightRnd.Width();
	nCropHeight = rcRightRnd.Height();
	rcRightRnd.right = nMaskRight;

	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, pRsltPtr, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight, CImageProcess::en_FindFromLeft );

	rcTemp = rcRight;
	rcTemp.top = rcRightRnd.bottom;
	rcTemp.right = rcRightRnd.right;

	if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecRightRndInfo, nWidth, nHeight, rcTemp, nEdgeWidth, 2, DIR_VER);
	}
	else {
		// 22.05.30 Ahn Modify Start
		bLocalRet = CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, pRsltPtr, &vecRightRndInfo, nWidth, nHeight, rcTemp, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindRight);
		// 22.05.30 Ahn Modify End
	}
	if (bLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProceTopAll_Negative>>에러 - vecRightRndInfo Edge found faile, ret = %d"), bLocalRet);

		return -2;
	}

	CImageProcess::SmoothVecRoundData(&vecRightRndInfo, CImageProcess::en_FindRight);

	int nRightSize = (int)vecRightRndInfo.size();
	if (nRightSize <= 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<ProceTopAll_Negative>>에러 - vecRightRndInfo Right EdgeLine Vector Size is 0"));

		CString strMsg;
		strMsg.Format(_T("Right EdgeLine Vector Size is 0"));
		MessageBox(strMsg);
		return -2;
	}

	rcRight.top = vecRightRndInfo[0].y;
	rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	if (rcRight.bottom < nRightSize - 1) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<ProceTopAll_Negative>>에러 - vecRightRndInfo Right 엣지라인 데이터 사이즈 이상."));

		CString strMsg;
		strMsg.Format(_T("Right 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -1;
	}
	delete[]pnResultArr;

	double dEdge3 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	CRegionInfo roiInfo;
	CString strThres;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiInfo.SetRoiPtr(pThresPtr);
	roiInfo.SetProcBit(CImageProcess::en_FoilExp_Bit);

	rcLeft.right = nHeadBndry;
	rcRight.right = nHeadBndry;

	int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	int nStartX = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	int nEndX = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CRect rcLeftDiff = rcLeft;
	CRect rcRightDiff = rcRight;

	rcLeftDiff.bottom = rcLeftRnd.top;
	rcRightDiff.top = rcRightRnd.bottom;
	rcLeftDiff = rcLeft;
	rcRightDiff = rcRight;
	pPreFltPtr = pProcPtr;

	// 21.10.22 Ahn Add End

	BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftDiff, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightDiff, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);


	// 22.01.19 Ahn Add Start
//	BOOL bUseErosionTest = TRUE;
////	BYTE* pErsPtr = new BYTE[nWidth * nHeight];
//	if (bUseErosionTest == TRUE) {
//
//		CImageProcess::Threshold_Default(pDiffPtr, pThresPtr, &vecLeftRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, rcLeft, TRUE);
//		CImageProcess::Threshold_Default(pDiffPtr, pThresPtr, &vecRightRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, rcRight, FALSE);
//
//		CImageProcess::Erosion(pThresPtr, pErosPtr, nWidth, nHeight, 0, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
//		CImageProcess::RemoveNoise(pErosPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
//		CImageProcess::Exptention( pErosPtr, pThresPtr, nWidth, nHeight, 0, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
//	//	SWAP(BYTE*, pThresPtr, pErosPtr);
//
//		roiInfo.SetRoiPtr(pErosPtr);
//		roiInfo.SetRect(rcLeft);
//		CImageProcess::CollectFifo(pThresPtr, &roiInfo, &vecLeftRndInfo, nWidth, nHeight, nStartX, nEndX , TRUE, CAM_POS_TOP);
//		roiInfo.SetRect(rcRight);
//		CImageProcess::CollectFifo(pThresPtr, &roiInfo, &vecRightRndInfo, nWidth, nHeight, nStartX, nEndX, FALSE, CAM_POS_TOP);
//	}else 
	{
		roiInfo.SetRect(rcLeft);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecLeftRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
		roiInfo.SetRect(rcRight);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecRightRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
	}
	// 22.01.19 Ahn Add End
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	vecBlockFoilExp.clear();


	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiInfo.GetFifoPtr());
	}

	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
//	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, m_pRecipeInfo->nFoilOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

	m_pImageDispDlg->SetBoundary(&vecLeftRndInfo, &vecRightRndInfo);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	dTime = cta.WhatTimeIsIt_Double();
	strThres.Format(_T("Thres & Labeling[%.1lf]"), dTime);

	if (m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
//		CImageProcess::BlockLink(&vecBlockDross, m_pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);
	}

	// 라운드 사이즈 판정.
	//

	cta.Clear();
	cta.StopWatchStart();

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	//for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) 
	{
		pVecBlockPtr = &vecBlockFoilExp;
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;

		//	if (rcLeftRnd.PtInRect(ptCenter) == TRUE) {
		// Tab 왼쪽 의 결함은 결함의 왼쪽이 Round 영역에 포함되는지,
		// Tab 오른쪽 결함은 결함의 왼쪽이 Round 영역에 포함 되는지에 따라, 
			CPoint cpLT;
			CPoint cpLB;
			cpLB.x = cpLT.x = pData->rcRect.left ;
			cpLT.y = pData->rcRect.top;
			cpLB.y = pData->rcRect.bottom;

			CRect rcDef;
			CRect rcTemp;
			// 22.07.20 Ahn Modify Start
			//rcDef.SetRect(pData->rcRect.left, pData->rcRect.top, pData->rcRect.right, pData->rcRect.bottom);
			rcDef = pData->rcRect;
			// 22.07.20 Ahn Modify End

			// 22.02.07 Ahn Modify Start
			if (rcLeft.PtInRect(cpLT) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecLeftRndInfo, m_pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				//if (rcTemp.IntersectRect(rcLeftRnd, rcDef) == TRUE) 
				if(rcLeftRnd.PtInRect( cpLT ) == TRUE )
				{
					CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
				}
				else {
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
			else if (rcRight.PtInRect(cpLB) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecRightRndInfo, m_pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				if (rcLeftRnd.PtInRect(cpLB) == TRUE)
				{
					CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindRight);
				}
				else {
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
			// 22.02.07 Ahn Modify End
		}
	}

	double dCalcSize = cta.WhatTimeIsIt_Double();
	CString strCalcSize;
	strCalcSize.Format(_T("CalcSize[%.2lf]"), dCalcSize);
	
	AddDefectInfo(&vecBlockFoilExp, NULL, CAM_POS_TOP);

	//SaveResultFile(m_pVecBlockAll);

	UpdateGrid();

	CString strEdge;
	strEdge.Format(_T(", Edge2 [%.2lf], Edge3 [%.2lf]"), dEdge2, dEdge3);
	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strMeanTime;
	m_strProcTime += strMeanTime2;
	m_strProcTime += strEdge;
	m_strProcTime += strTime;
	m_strProcTime += strThres;
	m_strProcTime += strCalcSize;

	return 0;
}

int CImageProcSimDlg::ProceBottomAll_Negative()
{

	if (m_bLoadImage == FALSE) return -1;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();

	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	BYTE* pRsltPtr5 = m_pBmpStd[en_ProcImage7]->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	CSize tabPos;
	int nLevel;
	CImageProcess::FindLevelBottom_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, &nLevel, CImageProcess::en_FindFromRight);
	// 22.01.05 Ahn Modify Start

	int nOffset = 10;

	if (nLevel < 0) {
		AfxMessageBox(_T("Tab Level 이상 !!"));
		return -1;
	}

	CRect rcAll;
	rcAll.left = nLevel - m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	// 22.01.06 Ahn Modify Start
	rcAll.right = nLevel + m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM] + m_pRecipeInfo->TabCond.nNegVGrooveHeight;
	// 22.01.06 Ahn Modify End
	rcAll.top = 0;
	rcAll.bottom = nHeight;

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();
	int* pnResultArr;
	pnResultArr = new int[nHeight];
	memset(pnResultArr, 0, sizeof(int) * nHeight);

	CImageProcess::VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	BYTE* pPreFltPtr = pImgPtr;

	cta.Clear();
	cta.StopWatchStart();

	BYTE* pProcPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();

	// 22.01.21 Ahn Add Start
	memset(pStdPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	// 22.01.21 Ahn Add End

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	// 22.05.30 Ahn Modify Start
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);
	// 22.05.30 Ahn Modify End

	// 비교 처리용 이미지 
	double dMeanProc = cta.WhatTimeIsIt_Double();
	CString strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresLower[0];
	CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, m_pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, m_pRecipeInfo->bDarkEmpMode) ;

	double dTime = cta.WhatTimeIsIt_Double();
	CString strMeanTime;
	strMeanTime.Format(_T("MeanProc[%.1lf], "), dTime);

	cta.Clear();
	cta.StopWatchStart();

	int nRoundOffset = 50;
	int nEdgeWidth = 10;	// 

	int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];

	BYTE* pEdgePtr;

	pEdgePtr = pMeanPtr;
	//// 22.01.06 Ahn Add Start
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	//	pEdgePtr = pMeanPtr;
	//}
	//else {
	//	pEdgePtr = pProcPtr;
	//}
	//// 22.01.06 Ahn Add End

	if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM] == CImageProcess::en_FineMaxDiffMode) {
		CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecAllRndInfo, nWidth, nHeight, rcAll, nEdgeWidth, 2, DIR_VER);
	}
	else {
		int nThresMax = m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; // 22.05.30 Ahn Add
		CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, pRsltPtr, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, nThresMax, CImageProcess::en_BottomSide, nLevel, CImageProcess::en_FindLeft);
	}
	delete[]pnResultArr;

	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft);

	dTime = cta.WhatTimeIsIt();
	strTime.Format(_T("Edge Detect[%.1lf], "), dTime);

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	cta.Clear();
	cta.StopWatchStart();

	BYTE* pSmoothPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	{
		pSmoothPtr = pDiffPtr;
	}
	BYTE* pThresPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
	//roiDross.SetRoiPtr(pThresPtr);
	//roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	roiFoilExp.SetRect(rcAll);
	//roiDross.SetRect(rcAll);
	////	roiFoilExp.SetRoiPtr(pThresPtr);
	int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_BOTTOM];
	int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_BOTTOM];
	int nFoilOutMaskOffset = m_pRecipeInfo->nFoilOutMaskOffset[CAM_POS_BOTTOM];
	int nFileExpMaskOffset = m_pRecipeInfo->nFoilExpMaskOffset[CAM_POS_BOTTOM];
	int nFoilOutInspWidth = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	int nFoilExpInspWidth = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];

	//CImageProcess::Threshold_RoundMask(pSmoothPtr, &roiFoilExp, &vecAllRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, 0, CImageProcess::en_ModeFoilExp, FALSE, CImageProcess::en_TailSide);
	//CImageProcess::Threshold_RoundMask(pSmoothPtr, &roiDross, &vecAllRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, 0, CImageProcess::en_ModeFoilExpOut, FALSE, CImageProcess::en_TailSide);
	{
		roiFoilExp.SetRect(rcAll);
		CImageProcess::Threshold_RoundMask_Negative(pSmoothPtr, &roiFoilExp, &vecAllRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilOutInspWidth, nFoilExpThLower, rcAll.right, TRUE, CImageProcess::en_BottomSide);
	}

	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	//CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	//vecBlockDross.clear();

	//CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	//	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());
	}

	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
	//CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, m_pRecipeInfo->nFoilOutMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);

	m_pImageDispDlg->SetBoundary(&vecAllRndInfo, NULL);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	dTime = cta.WhatTimeIsIt_Double();
	strThres.Format(_T("Thres & Labeling[%.1lf]"), dTime);

	if (m_pRecipeInfo->bEnableDefectLink[CAM_POS_BOTTOM] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, CAM_POS_BOTTOM);
	//	CImageProcess::BlockLink(&vecBlockDross, m_pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_BOTTOM);
	}

	// Round 결함이 아닌 경우, Edge에서 거리를 계산, 3 점의 위치를 측정하여 가장 큰 값을 취득함.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	//for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) 
	{
		//if (nMode == TYPE_FOILEXP_OUT) {
		//	pVecBlockPtr = &vecBlockDross;
		//}
		//else {
			pVecBlockPtr = &vecBlockFoilExp;
		//}
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];

			CImageProcess::CheckDistanceToEdgeLine(pData, &vecAllRndInfo, m_pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);

			if (pData->bDeleteFlag == TRUE) continue;
			//if (AprData.m_System.m_nMachineMode == ANODE_MODE) 
			{
			//	CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
			}
		}
	}

	//AddDefectInfo(&vecBlockFoilExp, &vecBlockDross, CAM_POS_BOTTOM);
	AddDefectInfo(&vecBlockFoilExp, NULL, CAM_POS_BOTTOM);

	UpdateGrid();

	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strMeanTime;
	m_strProcTime += strTime;
	m_strProcTime += strThres;

	return 0;
}
// 22.01.17 Ahn Add End

int CImageProcSimDlg::SaveResultFile(CImageProcess::_VEC_BLOCK* pVecDef)
{
	int nRet = 0;

	CWin32File wFile;
	CStringList strList;
	CString strExtName;
	CString strFileName;
	CString strImgFileNmae;
	strFileName = m_strSrcFileName;
	strExtName = m_strSrcFileName.Right(4);
	strImgFileNmae = m_strSrcFileName;
	strImgFileNmae.Replace(m_strCropImagePath, _T(""));
	strImgFileNmae.Replace(strExtName, _T(".csv"));
	strFileName.Replace(strExtName, _T(".csv"));

	CImageProcess::_VEC_BLOCK* pVecData = pVecDef;
	CString strLine;
	wFile.DeleteFileA(strFileName);

	CSize size = m_pBmpStd[en_OrgImage]->GetImgSize();
	int nCenterY = size.cy / 2;
	
	int nSize = (int)pVecData->size();
	int nRightStNo = nSize;
	for (int i = 0; i < nSize; i++) {
		CBlockData* pData;
		pData = &(*pVecData)[i];
		if (pData->rcRect.top > nCenterY) {
			nRightStNo = i;
			break;
		}
	}

	for( int nPos = 0 ; nPos < 2 ; nPos++ )
	{
		double dMaxSize = 0.0;
		double dMinSize = 99999999.0;
		double dSumSize = 0.0;
		double dAveSize = 0.0;
		double dSize =0.0 ;
		int nMaxSizePos = -1;
		int nMinSizePos = -1;
		int nCount;

		int nStartIdx = 0;
		int nEndIdx = nSize;
		if (nPos == 0) {
			nStartIdx = 0;
			nEndIdx = nRightStNo;
			strLine.Format(_T("Left Side Result \r\n"));
		}
		else {
			nStartIdx = nRightStNo;
			nEndIdx = nSize;
			strLine.Format(_T("Right Side Result \r\nf"));
		}
		wFile.TextSave1Line(m_strCropImagePath, strImgFileNmae, strLine, _T("at"), FALSE);

		nCount = 0;
		for (int i = nStartIdx; i < nEndIdx; i++) {
			CBlockData* pData;
			pData = &(*pVecData)[i];
			if (pData->dJudgeSize > dMaxSize) {
				dMaxSize = pData->dJudgeSize;
				nMaxSizePos = i;
			}
			if (pData->dJudgeSize < dMinSize) {
				dMinSize = pData->dJudgeSize;
				nMinSizePos = i;
			}
			nCount++;
			dSumSize += pData->dJudgeSize;
		}
		dAveSize = dSumSize / nCount ;
		strLine.Format(_T("Min Size:,%.1lf\r\nf"), dMinSize);
		wFile.TextSave1Line(m_strCropImagePath, strImgFileNmae, strLine, _T("at"), FALSE);
		strLine.Format(_T("Max Size:,%.1lf\r\n"), dMaxSize);
		wFile.TextSave1Line(m_strCropImagePath, strImgFileNmae, strLine, _T("at"), FALSE);
		strLine.Format(_T("Average Size:,%.1lf\r\n"), dAveSize );
		wFile.TextSave1Line(m_strCropImagePath, strImgFileNmae, strLine, _T("at"), FALSE);

		strLine.Format(_T("Index, PosX, PosY, 판정Size, 세로Size, Pixel Cnt\r\n"));
		wFile.TextSave1Line(m_strCropImagePath, strImgFileNmae, strLine, _T("at"), FALSE);
		for (int i = nStartIdx; i < nEndIdx; i++) {
			CBlockData* pData;
			pData = &(*pVecData)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			int nPosX = (pData->rcRect.left + pData->rcRect.right) / 2;
			int nPosY = (pData->rcRect.top + pData->rcRect.bottom) / 2;
			strLine.Format(_T("%d,%d,%d,%.1lf,%.1lf,%d\r\n"), i+1, nPosX, nPosY, pData->dJudgeSize, pData->dHeight, pData->nPixelCnt );
			wFile.TextSave1Line(m_strCropImagePath, strImgFileNmae, strLine, _T("at"), FALSE);
		}

		strLine = _T("\r\n");
		wFile.TextSave1Line(m_strCropImagePath, strImgFileNmae, strLine, _T("at"), FALSE);
	}

	return nRet;
}

void CImageProcSimDlg::OnBnClickedBtnInspSpeter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CTabRsltInfo tabRsltInfo;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pBndLinePtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pRsltPixel = m_pBmpStd[en_ProcImage3]->GetImgPtr();
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	BYTE* pProcPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();

	BYTE* pImgArr[6];
	pImgArr[0] = pMeanPtr;
	pImgArr[1] = pBndLinePtr;
	pImgArr[2] = pDiffPtr;
	pImgArr[3] = pRsltPixel;
	pImgArr[4] = pStdPtr;
	pImgArr[5] = pProcPtr;

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;
	int nLevel;
	int nPrjWidth = 2000;
	int* pnPrj = new int[ nPrjWidth ];
	CRect rect;
	CRect rcProc;
	//CImageProcess::_VEC_BLOCK vecBlock;
	//vecBlock.clear();
	CRegionInfo roiInfo;

	CString strThresTime;
	CString strLabelTime;

	if (m_bModeTop == TRUE) {
		rect.left = nWidth - nPrjWidth;
		rect.right = nWidth;
		rect.top = 0;
		rect.bottom = nHeight - 1 ;
		int nSamplingSize = nHeight / 100;

		BOOL bUseDarkRoll = (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;

		CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
		nLevel = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP], CImageProcess::en_FindFromLeft, bUseDarkRoll);
		nLevel += rect.left;

		rcProc.left = 0;
		rcProc.right = nLevel - m_pRecipeInfo->nSurfaceMaskOffset[CAM_POS_TOP];
		rcProc.top = 0; 
		rcProc.bottom = nHeight;

		roiInfo.SetRect(rcProc);
		roiInfo.SetRoiPtr(pMeanPtr);
		roiInfo.m_bProcPit = 0xff;

		CTimeAnalyzer timeAna;
		timeAna.Clear();
		timeAna.StopWatchStart();
		double dTime;

		CImageProcess::ImageProcessDetectSurface(pImgPtr, nWidth, nHeight, m_pRecipeInfo, rcProc, &tabRsltInfo, CAM_POS_TOP, TRUE, pImgArr, 5);

		dTime = timeAna.WhatTimeIsIt_Double();
		strLabelTime.Format(_T("Labeling [%.1lf]"), dTime);
		timeAna.Clear();
	}
	else {
		rect.left = 0 ;
		rect.right = nPrjWidth ;
		rect.top = 0;
		rect.bottom = nHeight - 1;
		int nSamplingSize = nHeight / 100;

		BOOL bUseDarkRoll = (m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_BOTTOM] == 1) ? FALSE : TRUE;

		CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
		nLevel = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM], CImageProcess::en_FindFromRight, bUseDarkRoll);
		nLevel += rect.left;


		rcProc.left = nLevel + m_pRecipeInfo->nSurfaceMaskOffset[CAM_POS_BOTTOM] ;
		rcProc.right = nWidth;
		rcProc.top = 0;
		rcProc.bottom = nHeight;

		roiInfo.SetRect(rcProc);
		roiInfo.SetRoiPtr(pMeanPtr);
		roiInfo.m_bProcPit = 0xff;

		CTimeAnalyzer timeAna;
		timeAna.Clear();
		timeAna.StopWatchStart();

		double dTime ;		
		//CImageProcess::Threshold(pImgPtr, &roiInfo, nWidth, nHeight, m_pRecipeInfo->nThresSurface[CAM_POS_BOTTOM], TRUE);
		//CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlock, roiInfo.m_bProcPit, 4, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

		CImageProcess::ImageProcessDetectSurface(pImgPtr, nWidth, nHeight, m_pRecipeInfo, rcProc, &tabRsltInfo, CAM_POS_BOTTOM, TRUE, pImgArr, 5);
		dTime = timeAna.WhatTimeIsIt_Double();
		strLabelTime.Format(_T("ImageProcess [%.1lf]"), dTime);
		timeAna.Clear();
	}

	CString strMsg;
	strMsg.Format(_T("nLevel = %d"), nLevel);
	AfxMessageBox(strMsg);

	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strThresTime;
	m_strProcTime += strLabelTime;

	UpdateData(FALSE);

	// 22.02.25 Ahn Add Start
	tabRsltInfo.SortingDefect(0);
	// 22.02.25 Ahn Add End

	// 22.05.09 Ahn Add Start
	delete[] pnPrj;
	pnPrj = NULL;
	// 22.05.09 Ahn Add End

	m_pVecBlockAll->clear();
	int nSize = (int)tabRsltInfo.m_vecDefInfo.size();

	for (int i = 0; i < nSize; i++) {
		CBlockData data;
		data.nPixelCnt = tabRsltInfo.m_vecDefInfo[i]->nSize;// vecBlock[i].nPixelCnt;
		// 22.07.20 Ahn Modify Start
		//data.nLeft = tabRsltInfo.m_vecDefInfo[i]->rcPos.left;  // vecBlock[i].nLeft;
		//data.nRight = tabRsltInfo.m_vecDefInfo[i]->rcPos.right;  // vecBlock[i].nRight;
		//data.nTop = tabRsltInfo.m_vecDefInfo[i]->rcPos.top;  // vecBlock[i].nTop;
		//data.nBottom = tabRsltInfo.m_vecDefInfo[i]->rcPos.bottom;  // vecBlock[i].nBottom;
		data.rcRect = tabRsltInfo.m_vecDefInfo[i]->rcPos; 
		// 22.07.20 Ahn Modify ENd
		data.nType = en_ModeSurface; // vecBlock[i].nType;
		data.nBriAve = tabRsltInfo.m_vecDefInfo[i]->nAvgBright;  // vecBlock[i].nBriAve;
		data.nBriMax = tabRsltInfo.m_vecDefInfo[i]->nMaxBright;  // vecBlock[i].nBriMax;
		data.nBriMin = tabRsltInfo.m_vecDefInfo[i]->nMinBright;  // vecBlock[i].nBriMin;
		data.nOrgBriAve = tabRsltInfo.m_vecDefInfo[i]->nAveOrgBir;  // vecBlock[i].nOrgBriAve;
		data.nOrgBriMax = tabRsltInfo.m_vecDefInfo[i]->nMaxOrgBir;  // vecBlock[i].nOrgBriMax;
		data.nOrgBriMin = tabRsltInfo.m_vecDefInfo[i]->nMinOrgBir;  // vecBlock[i].nOrgBriMin;
		data.dWidth = tabRsltInfo.m_vecDefInfo[i]->dSizeX;  // vecBlock[i].dWidth;
		data.dHeight = tabRsltInfo.m_vecDefInfo[i]->dSizeY;  // vecBlock[i].dHeight;
		data.dDistance = tabRsltInfo.m_vecDefInfo[i]->dDistance;  // vecBlock[i].dDistance; // 22.04.15 Ahn Add
		//data = tabRsltInfo.m_vecDefInfo[i];
		//data.nDefJudge = CTabRsltInfo::GetDefJudge(m_pRecipeInfo->dDefJudgeWidth, m_pRecipeInfo->dDefJudgeHeight, data.dJudgeSize, data.dHeight);
		if (data.dWidth > data.dHeight) {
			data.dJudgeSize = data.dWidth;
		}
		else {
			data.dJudgeSize = data.dHeight;
		}

		m_pVecBlockAll->push_back(data);
	}

	UpdateGrid();
}

void CImageProcSimDlg::LoopLabelingTest()
{
	BYTE* pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
	BYTE* pPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	CSize size = m_pBmpStd[en_OrgImage]->GetImgSize();
	int nWidth = size.cx ;
	int nHeight = size.cy ;

	CRect rcRange;
	rcRange = m_pImageDispDlg->GetMaskRect();

	if ((rcRange.Width() <= 0) || (rcRange.Height() <= 0)) {
		return; 
	}
	
	CRegionInfo roiInfo;
	int nCamPos = CAM_POS_TOP ;
	if (m_bModeTop == FALSE) {
		nCamPos = CAM_POS_BOTTOM ;
	}
	roiInfo.SetProcBit(CImageProcess::en_Black_Bit);
	roiInfo.SetRect(rcRange);
	roiInfo.SetRoiPtr(pPtr);
	CImageProcess::Threshold(pImgPtr, &roiInfo, nWidth, nHeight, m_pRecipeInfo->nThresSurface[nCamPos], TRUE);

	CImageProcess::_VEC_BLOCK vecBlock;
	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlock, CImageProcess::en_Black_Bit, 5, AprData.m_System.m_dResolX[nCamPos], AprData.m_System.m_dResolY);

}

// 22.06.05 Ahn Add Start
void CImageProcSimDlg::OnBnClickedBtnCropSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_bLoadImage == FALSE) return ;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr;
	pImgPtr = pBmpStd->GetImgPtr();
	CSize size; 
	size = pBmpStd->GetImgSize();

	int nWidth = size.cx;
	int nHeight = size.cy;

	CRect rcRange;
	rcRange = m_pImageDispDlg->GetMaskRect();

	if ((rcRange.Width() <= 0) || (rcRange.Height() <= 0)) {
		return;
	}

	CString strPath;
	CString strFileName;

	strPath = m_strCropImagePath;
	strFileName = m_strSrcFileName;
	strFileName.Replace(strPath, _T(""));
	strFileName.Replace(_T("."), _T("_Crop."));

	CImageProcess::SaveCropImage(pImgPtr, nWidth, nHeight, rcRange, strPath, strFileName);
}


void CImageProcSimDlg::OnBnClickedBtnAttachImg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//TCHAR szFilter[256] = _T("Image Files (*.bmp;*.JPG;*.JPEG)|*.bmp;*.JPG;*.JPEG|All Files (*.*)|*.*||");
	//CFileDialog dlg(TRUE, _T("bmp"), m_strCropImagePath, OFN_READONLY, szFilter, this);
	//if (dlg.DoModal() == IDOK) {
	//	m_strSrcFileName = dlg.GetPathName();
	//}

	CJoinImageDlg dlg(this, m_strCropImagePath);

	dlg.DoModal();
}
// 22.06.05 Ahn Add End


// 22.07.14 Ahn Add Start
int CImageProcSimDlg::ProcessHalfSlittingInsp()
{
	int nRet = 0;

	if (m_bLoadImage == FALSE) return -1;

	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	ProcHalfSlittingInsp(pImgPtr, nWidth, nHeight);

	return nRet;
}

int CImageProcSimDlg::ProcHalfSlittingInsp(BYTE* pImgPtr, int nWidth, int nHeight)
{
	int nRet = 0 ;

	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();
	BYTE* pRndMeanPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();

	// 조건
	int nEdgeThreshold = 30;

	int nDivision = 10;

	// Edge 검출
	CRect rcRect;
	int* pnPrjData = new int[nWidth];
	// 세로 방향 Mean 처리
	rcRect.left = 0; 
	rcRect.right = nWidth;
	rcRect.top = 0;
	rcRect.bottom = nHeight / nDivision ;

	CImageProcess::MeanImageDirection(pImgPtr, pMeanPtr, nWidth, nHeight, 100, DIR_VER );
	CImageProcess::GetProjection(pMeanPtr, pnPrjData, nWidth, nHeight, rcRect, DIR_VER, nDivision, FALSE );

	int nThreshold = 30;
	int nEdgePos = CImageProcess::FindEdgePosFromProjection(pnPrjData, nWidth, nThreshold);

	delete pnPrjData;
	pnPrjData = NULL;

	// 결함 검출

	CImageProcess::VEC_ROUND_INFO vecEdgeInfo;
	vecEdgeInfo.clear();

	int nAveLine = 0;
	int nLocalRet = CImageProcess::SearchEdgeLine(pMeanPtr, nWidth, nHeight, rcRect, &vecEdgeInfo, nAveLine, nEdgeThreshold, CImageProcess::en_FindFromRight );

	CRect rcFoilArea;
	int nStartX = nAveLine - 400;
	int nEndX = nAveLine + 400;
	if (nStartX < 0) nStartX = 0;
	if (nEndX > nWidth) nEndX = nWidth;
	rcFoilArea.SetRect(nStartX, 0, nEdgePos + 2, nHeight - 1 );
	CRect rcBurArea;
	rcBurArea.SetRect(nEdgePos - 2, 0, nEndX, nHeight - 1 );

	CRegionInfo roiInfo;
	roiInfo.SetRect(rcFoilArea);
	roiInfo.SetRoiPtr(pRsltPtr);
	roiInfo.SetProcBit(0x80);

	int nInspWidth = 200;
	int nMinTh = 0;
	int nMaxTh = 150;

	CImageProcess::Threshold_ByEdgeLine(pImgPtr, &roiInfo, &vecEdgeInfo, nWidth, nHeight, nInspWidth, nMinTh, nMaxTh, 1, CImageProcess::en_ModeWave, TRUE, CImageProcess::en_LeftSide);
	CImageProcess::_VEC_BLOCK vecBlockWave;
	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockWave, 0x80, 20, 10, 10);


	CRegionInfo roiBurInfo;
	roiBurInfo.SetRect(rcBurArea);
	roiBurInfo.SetRoiPtr(pRsltPtr);
	roiBurInfo.SetProcBit(0x40);
	nMinTh = 60;
	nMaxTh = 255;

	CImageProcess::Threshold_ByEdgeLine(pImgPtr, &roiBurInfo, &vecEdgeInfo, nWidth, nHeight, nInspWidth, nMinTh, nMaxTh, 1, CImageProcess::en_ModeCrak, FALSE, CImageProcess::en_LeftSide);
	CImageProcess::_VEC_BLOCK vecBlockBur;
	CImageProcess::LoopLabeling(&roiBurInfo, nWidth, nHeight, &vecBlockBur, 0x40, 20, 10, 10);

	AddDefectInfo(&vecBlockWave, &vecBlockBur, CAM_POS_TOP);

	UpdateGrid();

	return nRet ;
}
// 22.07.14 Ahn Add End

// 22.07.26 Ahn Add Start
int CImageProcSimDlg::RecipeListLoad()
{
	CRecipeCtrl rcpCtrl;
	m_cmbRecipeSelect.Clear();
	CStringList strRcpList;
	rcpCtrl.SearchRecipeFile(strRcpList);

	int nSize = m_cmbRecipeSelect.GetCount();
	while (m_cmbRecipeSelect.GetCount() > 0) {
		m_cmbRecipeSelect.DeleteString(0);
	}

	POSITION pos;
	for (pos = strRcpList.GetHeadPosition(); pos != nullptr; ) {
		CString strRcpName = strRcpList.GetNext(pos);
		strRcpName.Replace(_T(".ini"), _T(""));
		m_cmbRecipeSelect.AddString(strRcpName);

	}
	m_cmbRecipeSelect.SetCurSel(0);

	return 0;
}

void CImageProcSimDlg::OnCbnSelchangeCmbRecipeSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelNo = m_cmbRecipeSelect.GetCurSel() ;
	m_cmbRecipeSelect.SetCurSel(nSelNo);
	UpdateData(TRUE);

	CString strRcpName;
	m_cmbRecipeSelect.GetWindowText(strRcpName);

	CRecipeCtrl rcpCtrl;
	if (m_pRecipeInfo != nullptr) {
		rcpCtrl.LoadRecipe(m_pRecipeInfo, strRcpName);
	}
	//m_cmbRecipeSelect.

	UpdateRecipeGrid(); // 22.09.16 Ahn Add 

}
// 22.07.26 Ahn Add End

// 22.09.15 Ahn Add Start
int CImageProcSimDlg::UserChange()
{
	CWnd* pWnd;
	BOOL bMakerShow = SW_HIDE;
	BOOL bEngineerShow = SW_HIDE;

	switch (AprData.UserMode) {
	case	UserModeDefine::enOperator :
		bMakerShow = SW_HIDE;
		bEngineerShow = SW_HIDE;
		break ;
	case	UserModeDefine::enEngineer :
		bMakerShow = SW_HIDE;
		bEngineerShow = SW_SHOW;
		break ;
	case	UserModeDefine::enMaker :
		bMakerShow = SW_SHOW;
		bEngineerShow = SW_SHOW;
		break ;
	default :
		bMakerShow = SW_HIDE;
		bEngineerShow = SW_HIDE;
		break ;
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_IMG_SEL_GROUP);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_ORG);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_PROC1);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_PROC2);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_PROC3);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_PROC4);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_PROC5);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_PROC6);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_IMG_PROC7);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_BTN_ATTACH_IMG);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_CROP_SAVE);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_INSP_SPETER);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_TEST);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_DETECT_LINE);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(bMakerShow);
	}

	return 0 ;
}
// 22.09.15 Ahn Add End

// 22.09.15 Ahn Add Start
void CImageProcSimDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UserChange();
}
// 22.09.15 Ahn Add End


// 22.09.16 Ahn Add Start
int CImageProcSimDlg::UpdateRecipeGrid()
{
	int nRet = 0 ;

	int nRows = en_RcpPrm_Max;

	if (!::IsWindow( m_GridCtrlRecipe.m_hWnd)) {
		return -1;
	}
	UpdateData();

	//this->m_pRecipeInfo.T
	int nPos = CAM_POS_TOP ;
	if (m_bModeTop != TRUE) {
		nPos = CAM_POS_BOTTOM ;
	}
	for (int nCol = 0; nCol < MAX_INSP_TYPE; nCol++) {
		for (int nRow = 1; nRow < en_RcpPrm_Max; nRow++) {
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol + 1 ;

			switch (nCol)
			{
				case	TYPE_FOILEXP:
				{
					switch (nRow) {
					case	en_RcpPrm_ThresOrg:
						strText.Format(_T("%d"), m_pRecipeInfo->nFoilExpThresOrigin[nPos]);
						break;
					case	en_RcpPrm_ThresMin:
						strText.Format(_T("%d"), m_pRecipeInfo->nFoilExpThresLower[nPos]);
						break;
					case	en_RcpPrm_MinSize:
						strText.Format(_T("%d"), m_pRecipeInfo->nFoilExpInMinSize[nPos]);
						break;
					case	en_RcpPrm_InspRange:
						strText.Format(_T("%.2lf"), m_pRecipeInfo->dFoilExpInspWidth[nPos]);
						break;
					case	en_RcpPrm_JudgeSize:
						strText.Format(_T("%.2lf"), m_pRecipeInfo->dFoilExpInNgSize[nPos]);
						break;
					case	en_RcpPrm_Magnification:
						strText.Format(_T("%.2lf"), m_pRecipeInfo->dMagnification[nPos]);
						break;
					case	en_RcpPrm_UseDefLink:
						strText.Format(_T("%d"), m_pRecipeInfo->bEnableDefectLink[nPos]);
						break;
					case	en_RcpPrm_LinkRangeX:
						strText.Format(_T("%d"), m_pRecipeInfo->pntLinkRange[nPos][TYPE_FOILEXP_OUT].x);
						break;
					case	en_RcpPrm_LinkRangeY:
						strText.Format(_T("%d"), m_pRecipeInfo->pntLinkRange[nPos][TYPE_FOILEXP_OUT].y);
						break;
					}
					break;
				}
				case	TYPE_FOILEXP_OUT:
				{
					switch (nRow) {
					case	en_RcpPrm_ThresOrg:
						strText.Format(_T("%d"), m_pRecipeInfo->nDrossThresOrigin[nPos]);
						break;
					case	en_RcpPrm_ThresMin:
						strText.Format(_T("%d"), m_pRecipeInfo->nDrossThresLower[nPos]);
						break;
					case	en_RcpPrm_MinSize:
						strText.Format(_T("%d"), m_pRecipeInfo->nFoilExpOutMinSize[nPos]);
						break;
					case	en_RcpPrm_InspRange:
						strText.Format(_T("%.2lf"), m_pRecipeInfo->dFoilOutInspWidth[nPos]);
						break;
					case	en_RcpPrm_JudgeSize:
						strText.Format(_T("%.2lf"), m_pRecipeInfo->dFoilExpOutNgSize[nPos]);
						break;
					case	en_RcpPrm_Magnification:
						strText.Format(_T("%.2lf"), m_pRecipeInfo->dMagnification[nPos]);
						break;
					case	en_RcpPrm_UseDefLink:
						strText.Format(_T("%d"), m_pRecipeInfo->bEnableDefectLink[nPos]);
						break;
					case	en_RcpPrm_LinkRangeX:
						strText.Format(_T("%d"), m_pRecipeInfo->pntLinkRange[nPos][TYPE_FOILEXP_OUT].x);
						break;
					case	en_RcpPrm_LinkRangeY:
						strText.Format(_T("%d"), m_pRecipeInfo->pntLinkRange[nPos][TYPE_FOILEXP_OUT].y);
						break;
					}
					break;
				}
				case	TYPE_SURFACE:
				{
					switch (nRow) {
					case	en_RcpPrm_ThresOrg:
						strText.Format(_T("---"));
						break;
					case	en_RcpPrm_ThresMin:
						strText.Format(_T("%d"), m_pRecipeInfo->nThresSurface[nPos]);
						break;
					case	en_RcpPrm_MinSize:
						strText.Format(_T("%d"), m_pRecipeInfo->m_nSurfaceMinSize[nPos]);
						break;
					case	en_RcpPrm_InspRange:
						strText.Format(_T("---"));
						break;
					case	en_RcpPrm_JudgeSize:
						strText.Format(_T("%.2lf"), m_pRecipeInfo->dSurfaceNgSize[nPos]);
						break;
					case	en_RcpPrm_Magnification:
						strText.Format(_T("---"));
						break;
					case	en_RcpPrm_UseDefLink:
						strText.Format(_T("%d"), m_pRecipeInfo->bEnableDefectLink[nPos]);
						break;
					case	en_RcpPrm_LinkRangeX:
						strText.Format(_T("%d"), m_pRecipeInfo->pntLinkRange[nPos][TYPE_FOILEXP_OUT].x);
						break;
					case	en_RcpPrm_LinkRangeY:
						strText.Format(_T("%d"), m_pRecipeInfo->pntLinkRange[nPos][TYPE_FOILEXP_OUT].y);
						break;
					}
					break;
				}
			}
			Item.crFgClr = RGB(0, 0, 0);
			Item.crBkClr = RGB(150, 150, 150);
			Item.mask |= (GVIF_BKCLR | GVIF_FGCLR);
			Item.strText = strText;
			m_GridCtrlRecipe.SetItem(&Item);
		}
		//pGridCtrl->EnsureVisible(m_nRows - 1, 0);
		m_GridCtrlRecipe.Refresh();
	}

	return nRet;
}

void CImageProcSimDlg::OnClickGridCtrlRecipeSet(NMHDR* pNMHDR, LRESULT* pResult)
{

	CGridCtrl* pGridCtrl = &(m_GridCtrlRecipe);
	if (pGridCtrl == NULL) {
		return;
	}
	int	iMouseRow = pGridCtrl->GetMouseRow();
	int	iMouseCol = pGridCtrl->GetMouseCol();
	if (iMouseRow < 1) return;

	int nRow = iMouseRow ;
	int nCol = iMouseCol - 1 ;

	int nPos = CAM_POS_TOP;
	if (m_bModeTop != TRUE) {
		nPos = CAM_POS_BOTTOM;
	}

	CSetValue setValue;
	
	switch (nRow) {
	case	en_RcpPrm_ThresOrg:
		switch (nCol) {
		case	TYPE_FOILEXP :
			setValue.SetValue(_T("Foil exposure threshold(Original image bright)"), m_pRecipeInfo->nFoilExpThresOrigin[nPos], 255, 128);			;
			break; 
		case	TYPE_FOILEXP_OUT :
			setValue.SetValue(_T("Foil exposure out threshold(Original image bright)"), m_pRecipeInfo->nFoilExpThresOrigin[nPos], 255, 128); ;
			break;
		case	TYPE_SURFACE :
			//setValue.SetValue(_T("Surface threshold(Original image bright)"), m_pRecipeInfo->nFoilExpThresOrigin[nPos], 255, 128); ;
			break;
		}
		break;
	case	en_RcpPrm_ThresMin:
		switch (nCol) {
		case	TYPE_FOILEXP:
			setValue.SetValue(_T("Foil exposure threshold Lower"), m_pRecipeInfo->nFoilExpThresLower[nPos], 255, 128); ;
			break;
		case	TYPE_FOILEXP_OUT:
			setValue.SetValue(_T("Foil exposure out threshold Lower"), m_pRecipeInfo->nDrossThresLower[nPos], 255, 128); ;
			break;
		case	TYPE_SURFACE:
			setValue.SetValue(_T("Surface threshold Lower"), m_pRecipeInfo->nThresSurface[nPos], 255, 128); ;
			break;
		}		break;
	case	en_RcpPrm_MinSize:
		switch (nCol) {
		case	TYPE_FOILEXP:
			setValue.SetValue(_T("Foil exposure Min size"), m_pRecipeInfo->nFoilExpInMinSize[nPos], 999, 0); ;
			break;
		case	TYPE_FOILEXP_OUT:
			setValue.SetValue(_T("Foil exposure Min size"), m_pRecipeInfo->nFoilExpOutMinSize[nPos], 999, 0); ;
			break;
		case	TYPE_SURFACE:
			setValue.SetValue(_T("Surface Min size"), m_pRecipeInfo->m_nSurfaceMinSize[nPos], 999, 0); ;
			break;
		}		break;
	case	en_RcpPrm_InspRange:
		switch (nCol) {
		case	TYPE_FOILEXP:
			setValue.SetValue(_T("Foil exposure inspection width"), m_pRecipeInfo->dFoilExpInspWidth[nPos], 999.9, 0.0); ;
			m_pRecipeInfo->nFoilExpInspWidth[nPos] = (int)((double)m_pRecipeInfo->dFoilExpInspWidth[nPos] * AprData.m_System.m_dResolX[nPos]);
			break;
		case	TYPE_FOILEXP_OUT:
			setValue.SetValue(_T("Foil exposure inspection width"), m_pRecipeInfo->dFoilOutInspWidth[nPos], 999.9, 0.0); ;
			m_pRecipeInfo->nFoilOutInspWidth[nPos] = (int)((double)m_pRecipeInfo->dFoilOutInspWidth[nPos] * AprData.m_System.m_dResolX[nPos]);
			break;
		case	TYPE_SURFACE:
			break;
		}		break;
	case	en_RcpPrm_JudgeSize:
		switch (nCol) {
		case	TYPE_FOILEXP:
			setValue.SetValue(_T("Foil exposure NG size"), m_pRecipeInfo->dFoilExpInNgSize[nPos], 999.9, 0.0); ;
			break;
		case	TYPE_FOILEXP_OUT:
			setValue.SetValue(_T("Foil exposure out NG size"), m_pRecipeInfo->dFoilExpOutNgSize[nPos], 999.9, 0.0); ;
			break;
		case	TYPE_SURFACE:
			setValue.SetValue(_T("Surface NG size"), m_pRecipeInfo->dSurfaceNgSize[nPos], 999.9, 0.0); ;
			break;
		}		break;
	case	en_RcpPrm_Magnification:
		switch (nCol) {
		case	TYPE_FOILEXP:
		case	TYPE_FOILEXP_OUT:
			setValue.SetValue(_T("Foil exposure Magnification."), m_pRecipeInfo->dMagnification[nPos], 999.9, 0.0); ;
			break;
		case	TYPE_SURFACE:
			break;
		}		break;
	case	en_RcpPrm_UseDefLink:
		switch (nCol) {
		case	TYPE_FOILEXP:
			break;
		case	TYPE_FOILEXP_OUT:
			break;
		case	TYPE_SURFACE:
			break;
		}		break;
	case	en_RcpPrm_LinkRangeX:
		switch (nCol) {
		case	TYPE_FOILEXP:
			break;
		case	TYPE_FOILEXP_OUT:
			break;
		case	TYPE_SURFACE:
			break;
		}		break;
	case	en_RcpPrm_LinkRangeY:
		switch (nCol) {
		case	TYPE_FOILEXP:
			break;
		case	TYPE_FOILEXP_OUT:
			break;
		case	TYPE_SURFACE:
			break;
		}		break;
	}

	UpdateRecipeGrid();
}

// 22.09.16 Ahn Add End


// 23.01.06 Ahn Add Start
int CImageProcSimDlg::ProcessFoilExpInRect()
{
	int nRet = 0;


	if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
		ProcFoilExpInRect_Anode() ;
	}
	else {
		ProcFoilExpInRect_Cathode();
	}

	return nRet ;
}
// 23.01.06 Ahn Add End

// 23.01.12 Ahn Add Start
int CImageProcSimDlg::CheckProcRange(CRect& rcRect, int nWidth, int nHeight)
{

	if ((rcRect.Width() <= 0) || (rcRect.Height() <= 0))
	{
		return -1;
	}

	if (rcRect.left <= 0) rcRect.left = 0;
	if (rcRect.right > (nWidth-1) ) rcRect.right = nWidth - 1 ;
	if (rcRect.top <= 0) rcRect.top = 0;
	if (rcRect.bottom > ( nHeight - 1 ) ) rcRect.bottom = nHeight - 1 ;

	return 0; 
}

int CImageProcSimDlg::ProcFoilExpInRect_Anode()// 음극
{
	int nRet = 0;

	if (m_bLoadImage == FALSE) return -1;

	if (m_pImageDispDlg == NULL) {
		return -2;
	}

	CRect rcRange;
	rcRange = m_pImageDispDlg->GetMaskRect();

	if ((rcRange.Width() <= 0) || (rcRange.Height() <= 0))
	{
		MessageBox(_T("Please set the area to be inspected."));
		return -4;
	}

	int nCamPos = CAM_POS_TOP;
	int nHeight;
	int nWidth;
	CSize size;
	size = this->m_pBmpStd[en_OrgImage]->GetImgSize();
	nHeight = (int)size.cy;
	nWidth = (int)size.cx;

	CheckProcRange(rcRange, nWidth, nHeight);

	BYTE* pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr(); //new BYTE[nHeight * nWidth];
	BYTE* pProcPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr(); //new BYTE[nHeight * nWidth];
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	BYTE* pThresPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();

	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);


	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRange, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRange, 15, 15, CImageProcess::en_FillTop);

	BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresOrigin[nCamPos];

	CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRange, m_pRecipeInfo->dMagnification[nCamPos], btThreshold, 0);

	int nThMin = m_pRecipeInfo->nFoilExpThresLower[nCamPos];

	CRegionInfo roiInfo;
	roiInfo.SetRoiPtr(pThresPtr);
	roiInfo.SetProcBit(CImageProcess::en_FoilExp_Bit);
	roiInfo.SetRect(rcRange);

	CImageProcess::Threshold(pDiffPtr, &roiInfo, nWidth, nHeight, nThMin, TRUE, 0);

	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	vecBlockFoilExp.clear();

	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpOutMinSize[nCamPos], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

	CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, nCamPos);

	{
		CImageProcess::_VEC_BLOCK* pVecBlockPtr = &vecBlockFoilExp;
		CBlockData* pData;
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;

			CPoint cpLT;
			CPoint cpLB;
			cpLB.x = cpLT.x = pData->rcRect.left;
			cpLT.y = pData->rcRect.top;
			cpLB.y = pData->rcRect.bottom;

			CRect rcDef;
			CRect rcTemp;
			rcDef = pData->rcRect;
			CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[nCamPos], AprData.m_System.m_dResolY);
		}
	}

	AddDefectInfo(&vecBlockFoilExp, NULL, nCamPos);
	// 22.07.20 Ahn Modify End

	UpdateGrid();

	UpdateData(FALSE);
	Invalidate();
	return nRet;
}

int CImageProcSimDlg::ProcFoilExpInRect_Cathode()// 양극
{
	int nRet = 0;


	if (m_bLoadImage == FALSE) return -1;

	if (m_pImageDispDlg == NULL) {
		return -2;
	}

	CRect rcRange;
	rcRange = m_pImageDispDlg->GetMaskRect();

	if ((rcRange.Width() <= 0) || (rcRange.Height() <= 0))
	{
		MessageBox(_T("Please set the area to be inspected."));
		return -4;
	}

	int nCamPos = CAM_POS_TOP;
	int nHeight;
	int nWidth;
	CSize size;
	size = this->m_pBmpStd[en_OrgImage]->GetImgSize();
	nHeight = (int)size.cy;
	nWidth = (int)size.cx;

	BYTE* pImgPtr = m_pBmpStd[en_OrgImage]->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();

	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	BYTE* pRsltPtr5 = m_pBmpStd[en_ProcImage7]->GetImgPtr();

	CheckProcRange(rcRange, nWidth, nHeight);

	// Tab 유무 Check
	// Tab 찾기 위치 확인
	
	CImageProcess::VEC_SECTOR vecSector;
	int nLevel = FindTabFromRect(rcRange, vecSector);

	int nSectCount = (int)vecSector.size();
	if( (nLevel < 0) && ( nSectCount != 1 ) ){
		// Tab 찾기 실패.
		// Tab 을 찾을 수 없을 경우 단순 처리 할 것인가??
		MessageBox(_T("Tab find fail."));
		return -1;
	}
	CImageProcess::VEC_SECTOR::iterator iter;
	iter = vecSector.begin();
	int nTabLeft = iter->nStartPos;
	int nTabRight = iter->nEndPos;

	// Tab 을 찾은경우 Round 처리 추가
	CRect rcAll;
	rcAll = rcRange ;

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();

	CImageProcess::VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nMeanSize = MEAN_SIZE_HOR;
	BYTE* pPreFltPtr = pImgPtr;

	cta.Clear();
	cta.StopWatchStart();

	BYTE* pProcPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);

	// 비교 처리용 이미지 
	double dMeanProc = cta.WhatTimeIsIt_Double();
	CString strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresLower[0];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, m_pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	}
	else {
		pDiffPtr = pImgPtr;
	}

	double dTime = cta.WhatTimeIsIt_Double();
	CString strMeanTime;
	strMeanTime.Format(_T("MeanProc[%.1lf], "), dTime);

	cta.Clear();
	cta.StopWatchStart();

	int nRoundOffset = 50;
	int nEdgeWidth = 10;	// 

	BYTE* pEdgePtr;

	pEdgePtr = pMeanPtr;


	CRect rcLeft;
	CRect rcRight;

	rcLeft = rcRight = rcAll;

	if (rcAll.right > nWidth) {
		rcAll.right = nWidth - 1;
	}

	if (rcAll.left < 0) {
		return -1;
	}

	//rcAll.top = rcLeft.top = 0;
	//rcAll.bottom = rcRight.bottom = nHeight;
	rcLeft.bottom = nTabLeft +30;//m_pRecipeInfo->nFoilExpInspWidth[ CAM_POS_TOP ] ; 
	rcRight.top = nTabRight - 30;//m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CRect rcLeftRnd;
	CRect rcRightRnd;
	rcLeftRnd = rcLeft;
	rcRightRnd = rcRight;
	rcLeftRnd.top = rcLeft.bottom - m_pRecipeInfo->TabCond.nRadiusH;
	rcRightRnd.bottom = rcRightRnd.top + m_pRecipeInfo->TabCond.nRadiusH;

	// Test
	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight);
	CImageProcess::CheckRect(&rcLeftRnd, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRight, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRightRnd, nWidth, nHeight);

	cta.Clear();
	cta.StopWatchStart();

	CImageProcess::VEC_ROUND_INFO vecLeftRndInfo;
	CImageProcess::VEC_ROUND_INFO vecRightRndInfo;
	vecLeftRndInfo.clear();
	vecRightRndInfo.clear();

	int nHeadBndry;
	nHeadBndry = nLevel + m_pRecipeInfo->TabCond.nTabCeramicHeight;

	//rcRightRnd.right = rcLeftRnd.right = rcAll.right = nHeadBndry;


	int nMaskRight = nHeadBndry - 30;

	dTime = cta.WhatTimeIsIt_Double();
	strMeanTime.Format(_T("MeanProc[%.1lf], "), dTime);
	cta.Clear();
	cta.StopWatchStart();

	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);

	pProcPtr = m_pBmpStd[en_ProcImage4]->GetImgPtr();

	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	// 22.04.26 Ahn Add Start
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);
	// 22.04.26 Ahn Add End

	// 비교 처리용 이미지 
	dMeanProc = cta.WhatTimeIsIt_Double();
	strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	cta.Clear();
	cta.StopWatchStart();

	CRect rcTemp;
	rcTemp = rcLeft;
	rcTemp.bottom = rcLeftRnd.top;
	rcTemp.right = rcLeftRnd.right;
	int nCropWidth = rcLeftRnd.Width();
	int nCropHeight = rcLeftRnd.Height();
	int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];

	int  bLocalRet = 0;

	pEdgePtr = pMeanPtr; // pProcPtr;


	if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nEdgeWidth, 2, DIR_VER);
	}
	else {
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindLeft);
	}

	BYTE* pThresPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();

	if (bLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcFoilExpInRect_Cathode>>에러 - vecLeftRndInfo Edge found faile, ret = %d"), bLocalRet);
		return -1;
	}
	//rcLeftRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, pRsltPtr, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft);
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize <= 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcFoilExpInRect_Cathode>>에러 - vecLeftRndInfo Left EdgeLine Vector Size is 0"));
		return -1;
	}
	rcLeft.top = vecLeftRndInfo[0].y;
	rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	if (rcLeft.bottom < nLeftSize - 1) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcFoilExpInRect_Cathode>>에러 - vecLeftRndInfo Left 엣지라인 데이터 사이즈 이상."));

		CString strMsg;
		strMsg.Format(_T("Left 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -1;
	}

	double dEdge2 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	nCropWidth = rcRightRnd.Width();
	nCropHeight = rcRightRnd.Height();
	rcRightRnd.right = nMaskRight;

	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, pRsltPtr, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight);

	rcTemp = rcRight;
	rcTemp.top = rcRightRnd.bottom;
	rcTemp.right = rcRightRnd.right;

	if (m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr, &vecRightRndInfo, nWidth, nHeight, rcTemp, nEdgeWidth, 2, DIR_VER);
	}
	else {
		bLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, pRsltPtr, &vecRightRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindRight);
	}
	if (bLocalRet < 0) {
		CString strMsg;

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcFoilExpInRect_Cathode>>에러 - vecRightRndInfo 엣지라인 데이터 사이즈 이상."), bLocalRet);
		return -2;
	}

	CImageProcess::SmoothVecRoundData(&vecRightRndInfo, CImageProcess::en_FindRight);

	int nRightSize = (int)vecRightRndInfo.size();
	if (nRightSize <= 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcFoilExpInRect_Cathode>>에러 - vecRightRndInfo Right EdgeLine Vector Size is 0."), bLocalRet);

		CString strMsg;
		strMsg.Format(_T("Right EdgeLine Vector Size is 0"));
		MessageBox(strMsg);
		return -2;
	}

	rcRight.top = vecRightRndInfo[0].y;
	rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	if (rcRight.bottom < nRightSize - 1) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcFoilExpInRect_Cathode>>에러 - vecRightRndInfo Right EdgeLine Vector Size is 0."), bLocalRet);

		CString strMsg;
		strMsg.Format(_T("Right 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -1;
	}
	// 22.04.26 Ahn Delete Start
	//delete[]pnResultArr;
	// 22.04.26 Ahn Delete End

	double dEdge3 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
	roiDross.SetRoiPtr(pThresPtr);
	roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	// 22.03.30 Ahn Delete Start
	//rcLeft.right = nHeadBndry;
	//rcRight.right = nHeadBndry;
	// 22.03.30 Ahn Delete End

	int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	int nFoilOutMaskOffset = m_pRecipeInfo->nFoilOutMaskOffset[CAM_POS_TOP];
	int nFileExpMaskOffset = m_pRecipeInfo->nFoilExpMaskOffset[CAM_POS_TOP];
	int nFoilOutInspWidth = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	int nFoilExpInspWidth = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];

	CRect rcLeftDiff = rcLeft;
	CRect rcRightDiff = rcRight;

	rcLeftDiff.bottom = rcLeftRnd.top;
	rcRightDiff.top = rcRightRnd.bottom;
	rcLeftDiff = rcLeft;
	rcRightDiff = rcRight;

	pPreFltPtr = pProcPtr;

	BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeft, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRight, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftRnd, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightRnd, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);

	CString strProcDiff;
	double dDiffTime = cta.WhatTimeIsIt_Double();
	strProcDiff.Format(_T("ProcDiff[%.1lf]"), dDiffTime);
	cta.Clear();
	cta.StopWatchStart();

	CRect rcLeftAll;
	CRect rcRightAll;
	rcLeftAll = rcLeft;
	rcRightAll = rcRight;
	rcLeftAll.right = rcLeftRnd.right;
	rcRightAll.right = rcRightRnd.right;

	roiFoilExp.SetRect(rcLeftAll);
	roiDross.SetRect(rcLeftAll);

	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecLeftRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecLeftRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, nMaskRight, en_ModeFoilExpOut, FALSE);

	roiFoilExp.SetRect(rcRightAll);
	roiDross.SetRect(rcRightAll);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecRightRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecRightRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, nMaskRight, en_ModeFoilExpOut, FALSE);


	double dThTime = cta.WhatTimeIsIt_Double();
	strThres.Format(_T("Threshold[%.1lf]"), dThTime);

	cta.Clear();
	cta.StopWatchStart();

	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);

	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());
	}

	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, m_pRecipeInfo->nFoilExpOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

	m_pImageDispDlg->SetBoundary(&vecLeftRndInfo, &vecRightRndInfo);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	dTime = cta.WhatTimeIsIt_Double();

	CString strLabeling;
	strLabeling.Format(_T("Labeling[%.1lf]"), dTime);
	// 라운드 사이즈 판정.
	//

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) {
		if (nMode == TYPE_FOILEXP_OUT) {
			pVecBlockPtr = &vecBlockDross;
		}
		else {
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;

			CPoint ptCenter;
			ptCenter.x = (pData->rcRect.right + pData->rcRect.left) / 2;
			ptCenter.y = (pData->rcRect.bottom + pData->rcRect.top) / 2;

			if (rcLeftRnd.PtInRect(ptCenter) == TRUE) {
				CImageProcess::CalcRoundDef_Size_LeftFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcLeftRnd, pData, &vecLeftRndInfo, CImageProcess::en_FindLeft, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
			}
			else if (rcRightRnd.PtInRect(ptCenter) == TRUE) {
				CImageProcess::CalcRoundDef_Size_RightFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcRightRnd, pData, &vecRightRndInfo, CImageProcess::en_FindRight, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
			}
			else {
				// Round 결함이 아닌 경우, Edge에서 거리를 계산, 3 점의 위치를 측정하여 가장 큰 값을 취득함.
				if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
					if (pData->rcRect.top >= vecRightRndInfo[0].y) {
						CImageProcess::CalcSizeToEdgeLine(pData, &vecRightRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
					}
					else {
						CImageProcess::CalcSizeToEdgeLine(pData, &vecLeftRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
					}
				}
			}
		}
	}
	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, m_pRecipeInfo, CAM_POS_TOP);

	AddDefectInfo(&vecBlockMerge, NULL, CAM_POS_TOP);

	UpdateGrid();

	CString strEdge;
	strEdge.Format(_T(", Edge2 [%.2lf], Edge3 [%.2lf]"), dEdge2, dEdge3);
	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strMeanTime;
	m_strProcTime += strMeanTime2;
	m_strProcTime += strEdge;
	m_strProcTime += strTime;
	// 22.03.30 Ahn Modify Start
	m_strProcTime += strProcDiff;
	m_strProcTime += strThres;
	m_strProcTime += strLabeling;
	// 22.03.30 Ahn Modify End


	return nRet;
}


int CImageProcSimDlg::FindTabFromRect(CRect rcRect, CImageProcess::VEC_SECTOR& vecSector)
{
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;
	int nLevel;
	int nFindPos = rcRect.right - 200;

	vecSector.clear();

	BYTE* pImgPartPtr = pImgPtr + (nWidth * rcRect.top);
	int nPartHeight = rcRect.Height();

	CImageProcess::FindTabLevel_Simple(pImgPartPtr, nWidth, nPartHeight, nFindPos, m_pRecipeInfo, &vecSector, &nLevel);

	if (vecSector.size() > 0) {
		CImageProcess::VEC_SECTOR::iterator iter;
		for (iter = vecSector.begin(); iter != vecSector.end(); iter++) {
			iter->nStartPos += rcRect.top;
			iter->nEndPos += rcRect.top;
		}

	}

	return nLevel;
}
// 23.01.12 Ahn Add End


// 23.02.10 Ahn Add Start
void CImageProcSimDlg::OnCbnDropdownCmbRecipeSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	RecipeListLoad();
}
// 23.02.10 Ahn Add End


// 23.02.15 Ahn Add Start
int CImageProcSimDlg::ProcTopAll_BrightRoll()
{
	int nRet = 0;

	if (m_bLoadImage == FALSE) return 1 ;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	BYTE* pRsltPtr5 = m_pBmpStd[en_ProcImage7]->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	CSize tabPos;
	int nLevel;

	//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	//	CImageProcess::FindTabLevel(pImgPtr, nWidth, nHeight, &nLevel, m_pRecipeInfo->TabCond, m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
	//}
	//else {
	//	CImageProcess::FindLevelBottom_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, &nLevel, CImageProcess::en_FindFromRight);
	//}
	// Tab Find 
	if (GetTabHeadPos(&tabPos, &nLevel) < 0) {
		return -1 ;
	}

	// Edge Find 
	if (nLevel < 0) {
		AfxMessageBox(_T("Level 이상 !!"));
		return -1;
	}


	int nOffset = 50;

	// 22.01.05 Ahn Add Start
	int nLeftOffset;
	int nMeanLeftOffest;
	int nMeanRightOffset;
	int nTabRoundOffsetR;

	// 22.05.30 Ahn Modify Start
	//nLeftOffset = (int)((double)m_pRecipeInfo->TabCond.nNegVGrooveHeight );
	nLeftOffset = m_pRecipeInfo->TabCond.nNegVGrooveHeight + m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	// 22.05.30 Ahn Modify End
	nMeanLeftOffest = m_pRecipeInfo->TabCond.nNegCoatHeight;
	nMeanRightOffset = 50;
	nTabRoundOffsetR = m_pRecipeInfo->TabCond.nNegCoatHeight;


	CRect rcAll;
	CRect rcLeft;
	CRect rcRight;
	// 22.01.05 Ahn Modify Start
	//rcAll.left = rcRight.left = rcLeft.left = nLevel - m_pRecipeInfo->TabCond.nCeramicHeight;
	rcAll.left = rcRight.left = rcLeft.left = nLevel - nLeftOffset;
	// 22.01.05 Ahn Modify End


	rcAll.right = nLevel + (int)(m_pRecipeInfo->TabCond.nRadiusW);

	// 23.01.20 Ahn Add Start
	//rcRight.right = rcLeft.right = nLevel + 100;
	int nOutRange = m_pRecipeInfo->TabCond.nNegCoatHeight;
	rcRight.right = rcLeft.right = nLevel + nOutRange;
	// 23.01.20 Ahn Add ENd

	CRect rcEdgeMean;
	rcEdgeMean.bottom = nHeight;
	rcEdgeMean.top = 0;
	// 22.01.05 Ahn Add Start
	rcEdgeMean.left = nLevel - nMeanLeftOffest;
	rcEdgeMean.right = nLevel + nMeanRightOffset;
	// 22.01.05 Ahn Add End

	// 22.11.07 Ahn Modify Start
	if ((rcAll.right > nWidth) || (rcAll.left < 0)) {
		return-2;
	}
	// 22.11.07 Ahn Modify End

	rcAll.top = rcLeft.top = 0;
	rcAll.bottom = rcRight.bottom = nHeight;
	// 22.05.30 Ahn Modify Start
	//rcLeft.bottom = tabPos.cx - 50; //m_pRecipeInfo->nFoilExpInspWidth[ CAM_POS_TOP ] ; 
	//rcRight.top = tabPos.cy + 50;//m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	rcLeft.bottom = tabPos.cx - m_pRecipeInfo->TabCond.nRadiusH; //m_pRecipeInfo->nFoilExpInspWidth[ CAM_POS_TOP ] ; 
	rcRight.top = tabPos.cy + m_pRecipeInfo->TabCond.nRadiusH;//m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	// 22.05.30 Ahn Modify End

	//CRect rcLeftRnd;
	//CRect rcRightRnd;
	//rcLeftRnd = rcLeft;
	//rcRightRnd = rcRight;
	//rcLeftRnd.right = rcRightRnd.right = nLevel + nTabRoundOffsetR; // Tab Level 필요
	//rcLeftRnd.top = rcLeft.bottom - m_pRecipeInfo->TabCond.nRadiusH;
	//rcRightRnd.bottom = rcRightRnd.top + m_pRecipeInfo->TabCond.nRadiusH;

	// Test
	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRight, nWidth, nHeight);
	//CImageProcess::CheckRect(&rcLeftRnd, nWidth, nHeight);
	//CImageProcess::CheckRect(&rcRightRnd, nWidth, nHeight);

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();
	int* pnResultArr;
	pnResultArr = new int[nHeight];
	memset(pnResultArr, 0, sizeof(int) * nHeight);

	CImageProcess::VEC_ROUND_INFO vecLeftRndInfo;
	CImageProcess::VEC_ROUND_INFO vecRightRndInfo;
	vecLeftRndInfo.clear();
	vecRightRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nMeanSize = MEAN_SIZE_HOR;
	// 22.01.06 Ahn Add Start
	int nHeadBndry;
	nHeadBndry = nLevel + nTabRoundOffsetR;
	// 22.01.06 Ahn Add End
	// 22.05.30 Ahn Modify Start
	//rcRightRnd.right = rcLeftRnd.right = rcAll.right = nHeadBndry;
	int nInspRangeRight = nLevel + m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	//rcRightRnd.right = rcLeftRnd.right = rcAll.right = nInspRangeRight;
	// 22.05.30 Ahn Modify End

	int nMaskRight = nHeadBndry - 30;


	double dTime = cta.WhatTimeIsIt_Double();
	CString strMeanTime;
	strMeanTime.Format(_T("MeanProc[%.1lf], "), dTime);
	cta.Clear();
	cta.StopWatchStart();

	memset(pStdPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);
	//CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeftRnd, 15, 15, CImageProcess::en_FillTop);
	//CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRightRnd, 15, 15, CImageProcess::en_FillBottom);

	BYTE* pProcPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	memset(pProcPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);
	//CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeftRnd, 3, 3, CImageProcess::en_FillTop);
	//CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRightRnd, 3, 3, CImageProcess::en_FillBottom);

	// 22.05.30 Ahn Add Start 
	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);

	double dMeanProc = cta.WhatTimeIsIt_Double();
	CString strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	cta.Clear();
	cta.StopWatchStart();

	int nRoundOffset = 50;
	int nEdgeWidth = 10;	// 

	CRect rcTemp;
	rcTemp = rcLeft;
	//rcTemp.bottom = rcLeftRnd.top;
	//rcTemp.right = rcLeftRnd.right;
	//int nCropWidth = rcLeftRnd.Width();
	//int nCropHeight = rcLeftRnd.Height();
	int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];
	int nThresMax = m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; // 22.05.30 Ahn Add

	int  bLocalRet = 0;

	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr;

	BOOL bRollMode = TRUE;

	bLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, pRsltPtr, &vecLeftRndInfo, nWidth, nHeight, rcLeft, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindLeft);

	BYTE* pThresPtr = m_pBmpStd[en_ProcImage7]->GetImgPtr();
	BYTE* pPreFltPtr = pImgPtr;

	if (bLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcTopAll_BrightRoll>>에러 - vecLeftRndInfo Edge found faile, ret = %d."), bLocalRet);

		return -3;
	}
	//rcLeftRnd.right = nMaskRight;
	// 23.01.20 Ahn Modify Start
	//CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, pRsltPtr, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft, CImageProcess::en_FindFromLeft);
//	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, pRsltPtr, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft, CImageProcess::en_FindFromLeft, bRollMode);
	// 23.01.20 Ahn Modify End
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize <= 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcTopAll_BrightRoll>>에러 - vecLeftRndInfo Left EdgeLine Vector Size is 0."));
		return -4;
	}
	rcLeft.top = vecLeftRndInfo[0].y;
	rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	if (rcLeft.bottom < nLeftSize - 1) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcTopAll_BrightRoll>>에러 - vecLeftRndInfo Left 엣지라인 데이터 사이즈 이상."));

		CString strMsg;
		strMsg.Format(_T("Left 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -5;
	}

	double dEdge2 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	//nCropWidth = rcRightRnd.Width();
	//nCropHeight = rcRightRnd.Height();
	//rcRightRnd.right = nMaskRight;

	rcTemp = rcRight;
	//rcTemp.top = rcRightRnd.bottom;
	//rcTemp.right = rcRightRnd.right;

	bLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, pRsltPtr, &vecRightRndInfo, nWidth, nHeight, rcRight, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLevel, CImageProcess::en_FindRight);

	if (bLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcTopAll_BrightRoll>>에러 - vecRightRndInfo Edge found faile, ret = %d"), bLocalRet);

		return -6;
	}

	CImageProcess::SmoothVecRoundData(&vecRightRndInfo, CImageProcess::en_FindRight);

	int nRightSize = (int)vecRightRndInfo.size();
	if (nRightSize <= 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcTopAll_BrightRoll>>에러 - vecRightRndInfo Right EdgeLine Vector Size is 0"));

		CString strMsg;
		strMsg.Format(_T("Right EdgeLine Vector Size is 0"));
		MessageBox(strMsg);
		return -7;
	}

	rcRight.top = vecRightRndInfo[0].y;
	rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	if (rcRight.bottom < nRightSize - 1) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcTopAll_BrightRoll>>에러 - vecRightRndInfo Right 엣지라인 데이터 사이즈 이상."));

		CString strMsg;
		strMsg.Format(_T("Right 엣지라인 데이터 사이즈 이상."));
		MessageBox(strMsg);
		return -8;
	}
	delete[]pnResultArr;

	double dEdge3 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	CRegionInfo roiInfo;
	CString strThres;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiInfo.SetRoiPtr(pThresPtr);
	roiInfo.SetProcBit(CImageProcess::en_FoilExp_Bit);

	rcLeft.right = nHeadBndry;
	rcRight.right = nHeadBndry;

	int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	int nStartX = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	int nEndX = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CRect rcLeftDiff = rcLeft;
	CRect rcRightDiff = rcRight;

	//rcLeftDiff.bottom = rcLeftRnd.top;
	//rcRightDiff.top = rcRightRnd.bottom;
	rcLeftDiff = rcLeft;
	rcRightDiff = rcRight;
	pPreFltPtr = pProcPtr;

	// 21.10.22 Ahn Add End

	BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftDiff, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightDiff, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);

	m_pImageDispDlg->SetBoundary(&vecLeftRndInfo, &vecRightRndInfo);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	// Inspect
	{
		roiInfo.SetRect(rcLeft);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecLeftRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
		roiInfo.SetRect(rcRight);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecRightRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
	}
	// 22.01.19 Ahn Add End
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	vecBlockFoilExp.clear();


	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiInfo.GetFifoPtr());
	}

	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	//	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, m_pRecipeInfo->nFoilOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

	m_pImageDispDlg->SetBoundary(&vecLeftRndInfo, &vecRightRndInfo);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	dTime = cta.WhatTimeIsIt_Double();
	strThres.Format(_T("Thres & Labeling[%.1lf]"), dTime);

	if (m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
		//		CImageProcess::BlockLink(&vecBlockDross, m_pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);
	}

	// 라운드 사이즈 판정.
	//

	cta.Clear();
	cta.StopWatchStart();

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	//for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) 
	{
		pVecBlockPtr = &vecBlockFoilExp;
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;

			//	if (rcLeftRnd.PtInRect(ptCenter) == TRUE) {
			// Tab 왼쪽 의 결함은 결함의 왼쪽이 Round 영역에 포함되는지,
			// Tab 오른쪽 결함은 결함의 왼쪽이 Round 영역에 포함 되는지에 따라, 
			CPoint cpLT;
			CPoint cpLB;
			cpLB.x = cpLT.x = pData->rcRect.left;
			cpLT.y = pData->rcRect.top;
			cpLB.y = pData->rcRect.bottom;

			CRect rcDef;
			CRect rcTemp;
			// 22.07.20 Ahn Modify Start
			//rcDef.SetRect(pData->rcRect.left, pData->rcRect.top, pData->rcRect.right, pData->rcRect.bottom);
			rcDef = pData->rcRect;
			// 22.07.20 Ahn Modify End

			// 22.02.07 Ahn Modify Start
			if (rcLeft.PtInRect(cpLT) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecLeftRndInfo, m_pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				//if (rcTemp.IntersectRect(rcLeftRnd, rcDef) == TRUE) 
				//if (rcLeftRnd.PtInRect(cpLT) == TRUE)
				//{
				//	CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
				//}
				//else 
				{
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
			else if (rcRight.PtInRect(cpLB) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecRightRndInfo, m_pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				//if (rcLeftRnd.PtInRect(cpLB) == TRUE)
				//{
				//	CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindRight);
				//}
				//else 
				{
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
			// 22.02.07 Ahn Modify End
		}
	}

	double dCalcSize = cta.WhatTimeIsIt_Double();
	CString strCalcSize;
	strCalcSize.Format(_T("CalcSize[%.2lf]"), dCalcSize);

	AddDefectInfo(&vecBlockFoilExp, NULL, CAM_POS_TOP);

	//SaveResultFile(m_pVecBlockAll);

	UpdateGrid();

	CString strEdge;
	strEdge.Format(_T(", Edge2 [%.2lf], Edge3 [%.2lf]"), dEdge2, dEdge3);
	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strMeanTime;
	m_strProcTime += strMeanTime2;
	m_strProcTime += strEdge;
	m_strProcTime += strTime;
	m_strProcTime += strThres;
	m_strProcTime += strCalcSize;
	// 
	return nRet;

}

int CImageProcSimDlg::ProcBottomAll_BrightRoll()
{
	int nRet = 0;

	if (m_bLoadImage == FALSE) return 1;
	CBitmapStd* pBmpStd;
	pBmpStd = m_pBmpStd[en_OrgImage];
	BYTE* pImgPtr = pBmpStd->GetImgPtr();
	BYTE* pMeanPtr = m_pBmpStd[en_ProcImage1]->GetImgPtr();
	BYTE* pRsltPtr = m_pBmpStd[en_ProcImage2]->GetImgPtr();
	BYTE* pStdPtr = m_pBmpStd[en_ProcImage3]->GetImgPtr();
	BYTE* pDiffPtr = m_pBmpStd[en_ProcImage6]->GetImgPtr();
	BYTE* pRsltPtr5 = m_pBmpStd[en_ProcImage7]->GetImgPtr();

	int nWidth;
	int nHeight;
	CSize size = pBmpStd->GetImgSize();
	nWidth = size.cx;
	nHeight = size.cy;

	CSize tabPos;
	int nLevel;

	// 23.02.24 Ahn Mod Start
	//CImageProcess::FindLevelBottom_Negative(pImgPtr, nWidth, nHeight, m_pRecipeInfo, &nLevel, CImageProcess::en_FindFromLeft);
	CImageProcess::FindLevelBottom_BrightRoll(pImgPtr, nWidth, nHeight, m_pRecipeInfo, &nLevel, CImageProcess::en_FindFromLeft);
	// 23.02.24 Ahn Mod End

	// Edge Find 
	if (nLevel < 0) {
		AfxMessageBox(_T("Level 이상 !!"));
		return -1;
	}


	int nOffset = 50;

	// 22.01.05 Ahn Add Start
	int nLeftOffset;
	int nMeanLeftOffest;
	int nMeanRightOffset;
	int nTabRoundOffsetR;

	// 22.05.30 Ahn Modify Start
	//nLeftOffset = (int)((double)m_pRecipeInfo->TabCond.nNegVGrooveHeight );
	// 23.02.24 Ahn Modify Start
	//nLeftOffset = m_pRecipeInfo->TabCond.nNegVGrooveHeight + m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	nLeftOffset = m_pRecipeInfo->TabCond.nNegVGrooveHeight + m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	// 23.02.24 Ahn Modify End
	// 22.05.30 Ahn Modify End
	nMeanLeftOffest = m_pRecipeInfo->TabCond.nNegCoatHeight;
	nMeanRightOffset = 50;
	nTabRoundOffsetR = m_pRecipeInfo->TabCond.nNegCoatHeight;


	CRect rcAll;
	// 22.01.05 Ahn Modify Start
	//rcAll.left = rcRight.left = rcLeft.left = nLevel - m_pRecipeInfo->TabCond.nCeramicHeight;
	rcAll.left = nLevel - nLeftOffset;
	// 22.01.05 Ahn Modify End
	rcAll.right = nLevel + (int)(m_pRecipeInfo->TabCond.nRadiusW);
	rcAll.top = 0;
	rcAll.bottom = nHeight;

	// 23.01.20 Ahn Add Start
	//rcRight.right = rcLeft.right = nLevel + 100;
	int nOutRange = m_pRecipeInfo->TabCond.nNegCoatHeight;
	// 23.01.20 Ahn Add ENd

	// Test

	CString strTime;
	CTimeAnalyzer cta;
	cta.Clear();
	cta.StopWatchStart();
	int* pnResultArr;
	pnResultArr = new int[nHeight];
	memset(pnResultArr, 0, sizeof(int) * nHeight);

	CImageProcess::VEC_ROUND_INFO vecRndInfo;
	vecRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nMeanSize = MEAN_SIZE_HOR;
	// 22.01.06 Ahn Add Start
	int nHeadBndry;
	nHeadBndry = nLevel + nTabRoundOffsetR;
	// 22.01.06 Ahn Add End
	// 22.05.30 Ahn Modify Start
	//rcRightRnd.right = rcLeftRnd.right = rcAll.right = nHeadBndry;
	// 23.02.24 Ahn Modify Start
	//int nInspRangeRight = nLevel + m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	int nInspRangeRight = nLevel + m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	// 23.02.24 Ahn Modify End
	// 22.05.30 Ahn Modify End

	int nMaskRight = nHeadBndry - 30;


	double dTime = cta.WhatTimeIsIt_Double();
	CString strMeanTime;
	strMeanTime.Format(_T("MeanProc[%.1lf], "), dTime);
	cta.Clear();
	cta.StopWatchStart();

	memset(pStdPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);

	BYTE* pProcPtr = m_pBmpStd[en_ProcImage5]->GetImgPtr();
	memset(pProcPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	// 22.05.30 Ahn Add Start 
	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);

	double dMeanProc = cta.WhatTimeIsIt_Double();
	CString strMeanTime2;
	strMeanTime2.Format(_T("MeanTime:%.1lf"), dMeanProc);

	cta.Clear();
	cta.StopWatchStart();

	int nRoundOffset = 50;
	int nEdgeWidth = 10;	 

	CRect rcTemp;
	rcTemp = rcAll;
	// 23.02.24 Ahn Modify Start
	//int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];
	//int nThresMax = m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; // 22.05.30 Ahn Add
	int nThresBnd = m_pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
	int nThresMax = m_pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM];
	// 23.02.24 Ahn Modify End

	int  bLocalRet = 0;

	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr;

	BOOL bRollMode = TRUE;

	bLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, pRsltPtr, &vecRndInfo, nWidth, nHeight, rcAll, nThresBnd, nThresMax, CImageProcess::en_BottomSide, nLevel, CImageProcess::en_FindLeft);

	BYTE* pThresPtr = m_pBmpStd[en_ProcImage7]->GetImgPtr();
	BYTE* pPreFltPtr = pImgPtr;

	if (bLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcBottomAll_BrightRoll>>에러 - vecRndInfo Edge found faile, ret = %d"), bLocalRet);

		return -3;
	}

	CImageProcess::SmoothVecRoundData(&vecRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecRndInfo.size();
	if (nLeftSize <= 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ProcBottomAll_BrightRoll>>에러 - vecRndInfo Left EdgeLine Vector Size is 0"));
		return -4;
	}

	double dEdge2 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	delete[]pnResultArr;

	double dEdge3 = cta.WhatTimeIsIt_Double();
	cta.Clear();
	cta.StopWatchStart();

	CRegionInfo roiInfo;
	CString strThres;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiInfo.SetRoiPtr(pThresPtr);
	roiInfo.SetProcBit(CImageProcess::en_FoilExp_Bit);


	// 23.02.24 Ahn Modify Start
	//int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	//int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	//int nStartX = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	//int nEndX = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	int nDrossThLower = m_pRecipeInfo->nDrossThresLower[CAM_POS_BOTTOM];
	int nFoilExpThLower = m_pRecipeInfo->nFoilExpThresLower[CAM_POS_BOTTOM];
	int nStartX = m_pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	int nEndX = m_pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	// 23.02.24 Ahn Modify End

	pPreFltPtr = pProcPtr;

	// 21.10.22 Ahn Add End

	// 23.02.24 Ahn Modify Start
	//BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];
	//CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, m_pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	BYTE btThreshold = (BYTE)m_pRecipeInfo->nFoilExpThresOrigin[CAM_POS_BOTTOM];
	CImageProcess::DiffProcImage(pPreFltPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, m_pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, m_pRecipeInfo->bDarkEmpMode);
	// 23.02.24 Ahn Modify End

	m_pImageDispDlg->SetBoundary(&vecRndInfo, NULL);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	// Inspect
	{
		roiInfo.SetRect(rcAll);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
	}
	// 22.01.19 Ahn Add End
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	vecBlockFoilExp.clear();


	if (m_pRecipeInfo->bDisableProcDiff == FALSE) {
		CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiInfo.GetFifoPtr());
	}

	// 23.02.24 Ahn Modify Start
	//CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
	// 23.02.24 Ahn Modify End
	//	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, m_pRecipeInfo->nFoilOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

	m_pImageDispDlg->SetBoundary(&vecRndInfo, NULL);
	m_pImageDispDlg->SetDrawBoundaryFlag(m_bChkDIspBoundary);

	dTime = cta.WhatTimeIsIt_Double();
	strThres.Format("Thres & Labeling[%.1lf]", dTime);

	// 23.02.24 Ahn Modify Start
	//if (m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
	//	CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
	//	//		CImageProcess::BlockLink(&vecBlockDross, m_pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);
	//}
	if (m_pRecipeInfo->bEnableDefectLink[CAM_POS_BOTTOM] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, m_pRecipeInfo, TYPE_FOILEXP, CAM_POS_BOTTOM);
	}
	// 23.02.24 Ahn Modify End

	// 라운드 사이즈 판정.
	//

	cta.Clear();
	cta.StopWatchStart();

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	pVecBlockPtr = &vecBlockFoilExp;
	//}
	int nDefSize = (int)pVecBlockPtr->size();
	for (int i = 0; i < nDefSize; i++) {
		pData = &(*pVecBlockPtr)[i];

		CImageProcess::CheckDistanceToEdgeLine(pData, &vecRndInfo, m_pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);

		if (pData->bDeleteFlag == TRUE) continue;
		//if (AprData.m_System.m_nMachineMode == ANODE_MODE) 
		{
			//	CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
			CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
		}
	}

	double dCalcSize = cta.WhatTimeIsIt_Double();
	CString strCalcSize;
	strCalcSize.Format(_T("CalcSize[%.2lf]"), dCalcSize);

	// 23.02.24 Ahn Modify Start
	//AddDefectInfo(&vecBlockFoilExp, NULL, CAM_POS_TOP);
	AddDefectInfo(&vecBlockFoilExp, NULL, CAM_POS_BOTTOM);
	// 23.02.24 Ahn Modify End

	//SaveResultFile(m_pVecBlockAll);

	UpdateGrid();

	CString strEdge;
	strEdge.Format(_T(", Edge2 [%.2lf], Edge3 [%.2lf]"), dEdge2, dEdge3);
	m_strProcTime = _T("Proc Time[msec], ");
	m_strProcTime += strMeanTime;
	m_strProcTime += strMeanTime2;
	m_strProcTime += strEdge;
	m_strProcTime += strTime;
	m_strProcTime += strThres;
	m_strProcTime += strCalcSize;
	// 
	return nRet;
}
// 23.02.15 Ahn Add End

void CImageProcSimDlg::OnBnClickedBtnResetCount()
{
	CSigProc* pSigProc = theApp.m_pSigProc;

	AprData.LotEndProcess();

// 	theApp.m_pImgProcCtrl->TabCountReset();

	AprData.m_NowLotData.ClearAllCount();
	AprData.FileCtrl_LotInfo(CGlobalData::en_mode_LotEnd);

// 	CNotchingGradeInspDoc* pDoc = (CNotchingGradeInspDoc*)pView->GetDocument;
// 	pDoc->SetReqCounterReset(TRUE);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->ResetAndRefreshAll();
	pFrame->ReflashAll();
	pFrame->ResetResultViewDlg();

}
