// CRecipeSelectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CRecipeSelectDlg.h"
#include "afxdialogex.h"
#include "Win32File.h"
#include "GlobalData.h"
#include "CRecipeSettingDlg.h"
#include "CRecipeTableCtrl.h"

// CRecipeSelectDlg 대화 상자

IMPLEMENT_DYNAMIC(CRecipeSelectDlg, CDialogEx)

CRecipeSelectDlg::CRecipeSelectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RECIPE_SELECT, pParent)
	, m_strRecipeName(_T(""))
{
	m_strSelectedRcpName = _T("");
	m_pFontGrid = NULL;
	m_bAvailableRecipe = FALSE;

}

CRecipeSelectDlg::~CRecipeSelectDlg()
{
	if (m_pFontGrid != NULL) {
		delete m_pFontGrid;
		m_pFontGrid = NULL;
	}
}

void CRecipeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, m_ListRecipeName);
	DDX_Control(pDX, IDC_GRID_RECIPE_TABLE, m_GridRecipeTable);
	DDX_Text(pDX, IDC_ED_RECIPE_NAME, m_strRecipeName);
}


BEGIN_MESSAGE_MAP(CRecipeSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRecipeSelectDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_GRID_RECIPE_TABLE, &CRecipeSelectDlg::OnClickGridCtrlRecipeTable)
END_MESSAGE_MAP()


// CRecipeSelectDlg 메시지 처리기


void CRecipeSelectDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bAvailableRecipe == TRUE) {
		
	}
	else {
		MessageBox(_T("레시피가 선택되지 않았거나 파일이 존재하지 않습니다."));
	}

	CDialogEx::OnOK();
}


BOOL CRecipeSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//CWin32File winFile;
	//CStringList strList;
	//CString strRecipePath;
	//strRecipePath.Format(_T("%s\\Recipe"), AprData.m_strDataPath);
	//winFile.GetFileList(strRecipePath, strList, _T("ini"));

	//int nSize = (int)strList.GetSize();
	//POSITION pos;
	//;
	//for (pos = strList.GetHeadPosition(); pos != NULL; ) {
	//	CString strRecipeName = strList.GetNext(pos);
	//	strRecipeName.Replace(_T(".ini"), _T(""));
	//	m_ListRecipeName.AddString(strRecipeName);
	//}

	m_RcpTableCtrl.FileCtrl(MODE_READ);

	m_GridRecipeTable.SetRowHeight(0, 20);
	MakeGridCtrl_RecipeTable();
	UpdateGrid_RecipeTable();	


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

CString CRecipeSelectDlg::GetSeletedName() 
{
	CString strRet = _T("");
	if (m_bAvailableRecipe == TRUE) {
		strRet = m_strRecipeName ;
	}
	return strRet;
}


int CRecipeSelectDlg::MakeGridCtrl_RecipeTable()
{
	CRect rect;
	GetClientRect(rect);
	CGridCtrl* pGridCtrl;
	pGridCtrl = &m_GridRecipeTable;

	if (m_pFontGrid == NULL) {
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFont(AprData.m_System.m_nGridFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
			, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
			, FIXED_PITCH, "Gulim");
	}


	pGridCtrl->SetFont(m_pFontGrid, TRUE);

	int m_nFixCols = 1;
	int m_nFixRows = 1;
	int m_nCols = CRecipeSettingDlg::en_RcpName_MaxCols;
	int m_nRows = MAX_RECIPE_TABLE + 1;
	int nRow = 0;
	int nCol = 0;

	//	pGridCtrl->SetAutoSizeStyle();

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

	// 22.09.19 Ahn Modify Start
	//CString strTitle[CRecipeSettingDlg::en_RcpName_MaxCols] = { _T("번호"), _T("레시피 명"),_T("메모") };
	CString strTitle[CRecipeSettingDlg::en_RcpName_MaxCols]; //  = { _T("번호"), _T("레시피 명"),_T("메모") };

	for (int i = 0; i < CRecipeSettingDlg::en_RcpName_MaxCols; i++) {
		switch (i) {
		case	CRecipeSettingDlg::en_RcpTable_No :
			strTitle[i] = _LANG(_T("번호"), _T("No.") );
			break;
		case	CRecipeSettingDlg::en_RcpTable_Name :
			strTitle[i] = _LANG(_T("레시피 명"), _T("Recipe Name"));
			break;
		case	CRecipeSettingDlg::en_RcpName_Memo :
			strTitle[i] = _LANG(_T("메모"), _T("Memo"));
			break;
		}
	}
	// 22.09.19 Ahn Modify End

	for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
		CString strText;
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = 0;
		Item.col = nCol;
		Item.strText = strTitle[nCol];
		pGridCtrl->SetItem(&Item);
		int nWidth = 50;
		switch (nCol) {
		case	 0:
			nWidth = 40;
			break;
		case	1:
			nWidth = 200;
			break;
		case	2:
			nWidth = 100;
			break;
		default:
			nWidth = 50;
			break;
		}
		pGridCtrl->SetColumnWidth(nCol, nWidth);
	}


	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;

			//if (nRow < pGridCtrl->GetFixedRowCount())
			//	strText.Format(_T("%d"), nCol);
			//else 
			CString strTemp;
			CRecipeTableInfo* pInfo = m_RcpTableCtrl.GetRecipeTableInfo(nRow - 1);
			if (pInfo == NULL) {
				return -1;
			}

			switch (nCol) {
			case	0:
				strText.Format(_T("%d"), nRow);
				break;
			case	1:
				strText = pInfo->strRecipeName;
				break;
			case	2:
				strText = pInfo->strMemo;
				break;
			}
			Item.strText = strText;

			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
	pGridCtrl->ShowWindow(SW_SHOW);
	return 0;
}
int CRecipeSelectDlg::UpdateGrid_RecipeTable()
{

	int nRet = 0;
	// TODO: 여기에 구현 코드 추가.
	int nRows;

	CGridCtrl* pGrid = &m_GridRecipeTable;

	//CRecipeInfo* pRecipeInfo;
	//pRecipeInfo = m_pRecipeInfo;
	//if (pRecipeInfo == nullptr) {
	//	MessageBox("Pleases Load Recipe.");
	//	return -1;
	//}
	nRows = MAX_RECIPE_TABLE + 1;

	if (!::IsWindow(pGrid->m_hWnd)) {
		return -1;
	}

	UpdateData();

	int nOldNumRows = pGrid->GetRowCount();

	TRY{
		pGrid->SetRowCount(nRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return -1;
	}
	END_CATCH

		UpdateData(FALSE);

	COLORREF clrBkFoil = RGB(170, 200, 170);
	COLORREF clrBkDross = RGB(200, 200, 200);

	for (int nRow = 1; nRow < nRows; nRow++) {
		for (int nCol = 1; nCol < pGrid->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;
			int nCondNo = nCol - 1;

			CRecipeTableInfo* pInfo = m_RcpTableCtrl.GetRecipeTableInfo(nRow - 1);
			if (pInfo == NULL) {
				return -1;
			}

			switch (nCol) {
			case	1:
				strText = pInfo->strRecipeName;
				break;
			case	2:
				strText = pInfo->strMemo;
				break;
			}

			Item.strText = strText;
			pGrid->SetItem(&Item);
		}
	}
	pGrid->Refresh();

	return 0;
}


void CRecipeSelectDlg::OnClickGridCtrlRecipeTable(NMHDR* pNMHDR, LRESULT* pResult)
{
	CGridCtrl* pGridCtrl = &m_GridRecipeTable;

	if (pGridCtrl != NULL) {
		int	iMaxColNum = pGridCtrl->GetCols();
		int	iMaxRowNum = pGridCtrl->GetRows();
		int iRow = pGridCtrl->GetMouseRow();
		int iCol = pGridCtrl->GetMouseCol();

		int	iMouseRow = pGridCtrl->GetMouseRow();
		int	iMouseCol = pGridCtrl->GetMouseCol();
		if ((iMouseRow < 0) || (iMouseRow > iMaxRowNum)
			|| (iMouseCol < 1) || (iMouseCol > iMaxColNum)) {
			// -----붝댪둖-----
			return;
		}

		int	ret = 0;
		int	iSetValue = 0;
		double	dSetValue = (double)0.0;
		double	dMaxValue = (double)0.0;
		double	dTmpValue = (double)0.0;
		CString	strRecipeName = _T("");
		strRecipeName = pGridCtrl->GetTextMatrix(iRow, 1);
		int nTemp = 0;

		int nColNo = iCol - 1;


		m_bAvailableRecipe = FALSE;
		if (strRecipeName.GetLength() > 0) {
			CString strRecipeFilePath;
			strRecipeFilePath.Format( _T("%s\\Recipe\\%s.ini"), AprData.m_strDataPath, strRecipeName );
			if (CWin32File::ExistFile(strRecipeFilePath) == TRUE) {
				m_strRecipeName = strRecipeName;
				m_bAvailableRecipe = TRUE;
			
			}
		}

		if (m_bAvailableRecipe == FALSE) {
			m_strRecipeName = _T("");
		}

		UpdateData(FALSE);
	}

}
