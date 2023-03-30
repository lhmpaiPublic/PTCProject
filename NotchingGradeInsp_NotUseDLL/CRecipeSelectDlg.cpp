// CRecipeSelectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "afxdialogex.h"
#include "Win32File.h"
#include "CRecipeSelectDlg.h"
#include "LogDisplayDlg.h"
#include "GlobalData.h"

// CRecipeSelectDlg 대화 상자

IMPLEMENT_DYNAMIC(CRecipeSelectDlg, CDialogEx)

CRecipeSelectDlg::CRecipeSelectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RECIPE_SELECT, pParent)
	, m_strRecipeName(_T(""))
	, m_strRecipeMemo(_T(""))
{

}

CRecipeSelectDlg::~CRecipeSelectDlg()
{
}

void CRecipeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, m_ListRecipeName);
	DDX_CBString(pDX, IDC_COMBO_RECIPE_NAME, m_strRecipeName);
	DDX_Text(pDX, IDC_EDIT_RECIPE_MEMO, m_strRecipeMemo);
	DDX_Control(pDX, IDC_COMBO_RECIPE_NAME, m_RecipeNameCtrl);
}


BEGIN_MESSAGE_MAP(CRecipeSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRecipeSelectDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUT_RECIPEDELETE, &CRecipeSelectDlg::OnBnClickedButRecipedelete)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_NAME, &CRecipeSelectDlg::OnCbnSelchangeComboRecipeName)
END_MESSAGE_MAP()


// CRecipeSelectDlg 메시지 처리기


void CRecipeSelectDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	//메모 기본 텍스트 쓰기
	m_strRecipeMemo = m_strRecipeName;
	CDialogEx::OnOK();
}

void CRecipeSelectDlg::LoadRecipeFileList()
{
	//로드된 레시피명 정보
	CStringList strRecipeList;
	strRecipeList.RemoveAll();

	//레시피 파일이 들어있는 경로 설정
	CString strRecipeDir;
	strRecipeDir.Format(_T("%s\\Recipe\\"), AprData.m_strDataPath);

	CLogDisplayDlg::gInst()->AddLogDisplayMessage("RecipeListLoad 경로 : " + strRecipeDir);

	//경로에 있는 레시피 파일 목록을 가져온다.
	CWin32File winFile;
	winFile.GetFileList(strRecipeDir, strRecipeList);

	//가져온 레시피 파일의 목록을 콤보박스에 추가한다.
	POSITION pos;
	CString strTemp;
	for (pos = strRecipeList.GetHeadPosition(); pos != NULL; ) {
		strTemp = strRecipeList.GetNext(pos);
		strTemp.Replace(_T(".ini"), _T(""));
		m_RecipeNameCtrl.AddString(strTemp);
	}
	m_RecipeNameCtrl.SetCurSel(0);

	CString str;
	m_RecipeNameCtrl.GetLBText(0, str);
	//메모 기본 텍스트 쓰기
	m_strRecipeName = m_strRecipeMemo = str;
	UpdateData(FALSE);
}

BOOL CRecipeSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	LoadRecipeFileList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CRecipeSelectDlg::OnBnClickedButRecipedelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	CString deleteFileName;
	deleteFileName.Format(_T("%s\\Recipe\\%s.ini"), AprData.m_strDataPath, m_strRecipeName);
	if (DeleteFile(deleteFileName) == FALSE)
	{
		MessageBox(_T("파일 삭제 실패"));
	}
	else
	{
		m_RecipeNameCtrl.ResetContent();
		LoadRecipeFileList();
	}
}


void CRecipeSelectDlg::OnCbnSelchangeComboRecipeName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	int index = m_RecipeNameCtrl.GetCurSel();
	m_RecipeNameCtrl.GetLBText(m_RecipeNameCtrl.GetCurSel(), str);
	//메모 기본 텍스트 쓰기
	m_strRecipeName = m_strRecipeMemo = str;
	UpdateData(FALSE);
}
