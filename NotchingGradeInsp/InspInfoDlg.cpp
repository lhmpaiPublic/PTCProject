// InspInfoDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "InspInfoDlg.h"
#include "afxdialogex.h"
#include "GlobalData.h"
#include "CRecipeCtrl.h"
#include "CLightManualCtrlDlg.h"
#include "CLightControl.h"
#include "CRecipeSettingDlg.h"
#include "CUserChangeDlg.h"			// 22.05.25 Son Add
#include "CRecipeSelectDlg.h"		// 22.08.04 Ahn Add 
// CInspInfoDlg 대화 상자
static CString UiText1[][3] =
{
	{_T("검사 정보"), _T("Information"), _T("检查信息")},
	{_T("사용자 변경"), _T("User Change"), _T("切换用户权限")},
	{_T("작업수량"), _T("Count"), _T("数量")},
	{_T("총 수량"), _T("Total"), _T("总数量")},
	{_T("OK 수량"), _T("OK Count"), _T("OK数量")},
	{_T("NG 수량"), _T("NG Count"), _T("NG数量")},
	{_T("수    율"), _T("YIELD Count"), _T("良率")},
	{_T("Select"), _T("Select"), _T("选择")},
};

enum UiText1Name
{
	ST_INSP_INFO,
	BTN_USER_CHANGE,
	ST_INSP_COUNT,
	ST_TOTAL_COUNT,
	ST_OK_COUNT,
	ST_NG_COUNT,
	ST_YIELD,
	BTN_RECIPE_SELECT,
	MAX_COUNT
};

static int UiText1NameText[] =
{
	IDC_ST_INSP_INFO,
	IDC_BTN_USER_CHANGE,
	IDC_ST_INSP_COUNT,
	IDC_ST_TOTAL_COUNT,
	IDC_ST_OK_COUNT,
	IDC_ST_NG_COUNT,
	IDC_ST_YIELD,
	IDC_BTN_RECIPE_SELECT,

};


IMPLEMENT_DYNAMIC(CInspInfoDlg, CDialogEx)

CInspInfoDlg::CInspInfoDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_INSP_INFO, pParent)
	, m_strEdRecipeName(_T(""))
	, m_strNextRecipeName(_T(""))
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
	m_pDoc = pView->GetDocument();
	}
}

CInspInfoDlg::~CInspInfoDlg()
{
}

void CInspInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_LOT_ID, m_stLotID);
	DDX_Control(pDX, IDC_ST_NUM_TOTAL, m_stNumTotal);
	DDX_Control(pDX, IDC_ST_NUM_OK, m_stNumOK);
	DDX_Control(pDX, IDC_ST_NUM_NG, m_stNumNG);
	DDX_Control(pDX, IDC_ST_NUM_YIELD, m_stNumYield );
	// 22.12.13 Ahn Modify End
	DDX_Text(pDX, IDC_ED_RECIPE_NAME, m_strEdRecipeName);
	DDX_Control(pDX, IDC_ST_USER_MODE, m_strUserMode);				// 22.05.25 Son Add
	DDX_Text(pDX, IDC_ED_NEXT_RECIPE_NAME, m_strNextRecipeName);
}


BEGIN_MESSAGE_MAP(CInspInfoDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_USER_CHANGE, &CInspInfoDlg::OnBnClickedBtnUserChange)				// 22.05.25 Son Add
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_RECIPE_SELECT, &CInspInfoDlg::OnBnClickedBtnRecipeSelect)
END_MESSAGE_MAP()


// CInspInfoDlg 메시지 처리기

void CInspInfoDlg::DisplayLanguage()

/* {
	CWnd* pWnd;
	pWnd = (CWnd*)GetDlgItem(IDC_ST_INSP_INFO);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("검사 정보"), _T("Information")));
	}

	pWnd = (CWnd*)GetDlgItem(IDC_BTN_USER_CHANGE);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("사용자 변경"), _T("User Change")));
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_INSP_COUNT);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("작업수량"), _T("Count")));
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_TOTAL_COUNT);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("총 수량"), _T("Total")));
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_OK_COUNT);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("OK 수량"), _T("OK Count")));
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_NG_COUNT);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("NG 수량"), _T("NG Count")));
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_YIELD);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("수    율"), _T("YIELD Count")));
	}
	
}
*/
{
	CWnd* pWnd;
	CString strDispName;
	for (int idx = 0; idx < MAX_COUNT; idx++)
	{
		pWnd = NULL;
		pWnd = GetDlgItem(UiText1NameText[idx]);
		if (pWnd != nullptr)
		{
			strDispName = UiText1[idx][__Lang];
			pWnd->SetWindowTextA(strDispName);
		}
	}
}

BOOL CInspInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// 22.12.13 Ahn Modify Start
	//m_stMemo.SetTextColor(RGB(64, 64, 64));
	//m_stMemo.SetBackgroundColor(RGB(255, 255, 255));
	//m_stMemo.SetWindowText(_T("Memo\n1\n2"));
	m_stNumYield.SetTextColor(RGB(64, 64, 64));
	m_stNumYield.SetBackgroundColor(RGB(192, 192, 192));
	m_stNumYield.SetWindowText(_T("0.0 %%"));
	// 22.12.13 Ahn Modify End

	m_stLotID.SetTextColor(RGB(64, 64, 64));
	m_stLotID.SetBackgroundColor(RGB(192, 192, 192));
	m_stLotID.SetWindowText(_T("LOT ID"));

	m_stNumTotal.SetTextColor(RGB(64, 64, 64));
	m_stNumTotal.SetBackgroundColor(RGB(255, 255, 192));
	m_stNumTotal.SetWindowText(_T("0"));

	m_stNumOK.SetTextColor(RGB(64, 64, 64));
	m_stNumOK.SetBackgroundColor(RGB(192, 255, 192));
	m_stNumOK.SetWindowText(_T("0"));

	m_stNumNG.SetTextColor(RGB(64, 64, 64));
	m_stNumNG.SetBackgroundColor(RGB(255, 192, 192));
	m_stNumNG.SetWindowText(_T("0"));

	m_strEdRecipeName = AprData.m_NowLotData.m_strRecipeName;
	m_strNextRecipeName = AprData.m_NowLotData.m_strNextRecipeName; // 22.08.05 Ahn Add
	UpdateData(FALSE);

	DisplayLanguage(); // 22.09.01 Ahn Add 

// 22.05.25 Son Add Start
	m_strUserMode.SetFont(&AprData.m_font);
	// 22.09.15 Ahn Modify Start
	//if (AprData.UserMode == UserModeDefine::enMaker) {
	//	m_strUserMode.SetTextColor(RGB(255, 64, 64));
	//}
	//else {
	//	m_strUserMode.SetTextColor(RGB(64, 64, 64));
	//}
	switch (AprData.UserMode) {
	case	UserModeDefine::enOperator :
		m_strUserMode.SetTextColor(RGB(64, 64, 64));
		break;
	case	UserModeDefine::enEngineer :
		m_strUserMode.SetTextColor(RGB(255, 64, 128));
		break;
	case	UserModeDefine::enMaker :
		m_strUserMode.SetTextColor(RGB(255, 64, 64));
		break;
	}

	// 22.09.15 Ahn Modify End
	m_strUserMode.SetBackgroundColor(RGB(192, 255, 192));
	m_strUserMode.SetWindowText(UserModeDefine::szName[AprData.UserMode]);

	CWnd* pWndChild;
	pWndChild = GetTopWindow();
	while (pWndChild != NULL) {
		pWndChild->SetFont(&AprData.m_font);
		pWndChild = pWndChild->GetNextWindow();
	}
// 22.05.25 Son Add End

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInspInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

}

void CInspInfoDlg::ReflashAll()
{
//	m_cmbRecipeSelect.SetWindowText(AprData.m_NowLotData.m_strRecipeName);

	CString strTotalCnt;
	CString strNgCnt;
	CString strOkCnt;

	CDefectDataCtrl *pCtrl = m_pDoc->GetErrorDataCtrl(); 
	int nTabCount = AprData.m_NowLotData.m_nTabCount ;
	int nNgCount = AprData.m_NowLotData.m_nTabCountNG ;
	int nOKCount = AprData.m_NowLotData.m_nTabCount - AprData.m_NowLotData.m_nTabCountNG; //OK 수량 //AprData.m_NowLotData.m_nTabCountOK;

	strTotalCnt.Format(_T("%d"), nTabCount);
	strNgCnt.Format(_T("%d"), nNgCount);
	strOkCnt.Format(_T("%d"), nOKCount);
	m_stNumTotal.SetWindowText(strTotalCnt);
	m_stNumOK.SetWindowText(strOkCnt);
	m_stNumNG.SetWindowText(strNgCnt);

	// 22.12.13 Ahn Add Start
	double dYield = 0.0;
	if (nTabCount > 0) {
		dYield = ((double)nOKCount / (double)nTabCount) * 100.0;
		if (dYield > 100.0) {
			dYield = 100.0;
		}
	}
	CString strYield ;
	// 23.02.08 Ahn Modify Start
	//strYield.Format(_T("%.1lf %%"), dYield);
	strYield.Format(_T("%.2lf %%"), dYield);
	// 23.02.08 Ahn Modify End
	m_stNumYield.SetWindowTextA(strYield);
	// 22.12.13 Ahn Modify End

	m_stLotID.SetWindowText(AprData.m_NowLotData.m_strLotNo);
	//m_strEdRecipeName = AprData.m_NowLotData.m_strRecipeName;
	// 22.08.05 Ahn Modify Start
	//GetDlgItem(IDC_ED_RECIPE_NAME)->SetWindowTextA(AprData.m_NowLotData.m_strRecipeName);
	//GetDlgItem(IDC_ED_NEXT_RECIPE_NAME)->SetWindowTextA(AprData.m_NowLotData.m_strNextRecipeName);
	m_strEdRecipeName.GetBufferSetLength(AprData.m_NowLotData.m_strRecipeName.GetLength() + 1);
	strcpy_s((LPSTR)(LPCTSTR)m_strEdRecipeName, m_strEdRecipeName.GetLength(), (LPCTSTR)AprData.m_NowLotData.m_strRecipeName);
	m_strNextRecipeName.GetBufferSetLength(AprData.m_NowLotData.m_strNextRecipeName.GetLength() + 1);
	strcpy_s((LPSTR)(LPCTSTR)m_strNextRecipeName, m_strNextRecipeName.GetLength(), (LPCTSTR)AprData.m_NowLotData.m_strNextRecipeName);
	// 22.08.05 Ahn Modify End
	UpdateData(FALSE);
}

// 22.05.25 Son Add Start
void CInspInfoDlg::OnBnClickedBtnUserChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CUserChangeDlg dlg;
	dlg.DoModal();
	// 22.09.15 Ahn Modify Start
	//if (AprData.UserMode == UserModeDefine::enMaker) {
	//	m_strUserMode.SetTextColor(RGB(255, 64, 64));
	//}
	//else {
	//	m_strUserMode.SetTextColor(RGB(64, 64, 64));
	//}
	switch (AprData.UserMode) {
	case	UserModeDefine::enOperator:
		m_strUserMode.SetTextColor(RGB(64, 64, 64));
		break;
	case	UserModeDefine::enEngineer:
		m_strUserMode.SetTextColor(RGB(255, 64, 128));
		break;
	case	UserModeDefine::enMaker:
		m_strUserMode.SetTextColor(RGB(255, 64, 64));
		break;
	}
	// 22.09.15 Ahn Modify End

	m_strUserMode.SetWindowText(UserModeDefine::szName[AprData.UserMode]);

	m_pView->OnRefresh();
}
// 22.05.25 Son Add End

void CInspInfoDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	CDialogEx::OnClose();
}


void CInspInfoDlg::OnBnClickedBtnRecipeSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CRecipeSelectDlg dlg;
	dlg.DoModal();

	// AprData.m_NowLotData.m_strNextRecipeName = m_strEdRecipeName ;

	CString strRecipeName;
	strRecipeName = dlg.m_strRecipeName;
	if ( strRecipeName.GetLength() > 0 ) {
		m_strNextRecipeName = strRecipeName;
		UpdateData(FALSE); // 22.12.08 Ahn Add
	}
	//AprData.m_NowLotData.m_strRecipeName = m_strEdRecipeName ;
	AprData.m_NowLotData.m_strNextRecipeName = m_strNextRecipeName;
	// Recipe Name Save 
	AprData.FileCtrl_LotInfo(CGlobalData::en_mode_RecipeSelect);
}
