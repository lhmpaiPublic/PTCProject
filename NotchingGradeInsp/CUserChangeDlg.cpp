// CUserChangeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CUserChangeDlg.h"
#include "afxdialogex.h"
#include "GlobalData.h"


// CUserChangeDlg 대화 상자

IMPLEMENT_DYNAMIC(CUserChangeDlg, CDialogEx)

CUserChangeDlg::CUserChangeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_USER_CHANGE, pParent)
	, m_EdChangePassword(_T(""))
	, m_ChkChangePassword(FALSE)
{
	m_font.CreateFont(20, 9,            // 세로,가로 크기
		0, 0,
		FW_HEAVY,    //Font 굵기
		FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH,
		"Arial");
}

CUserChangeDlg::~CUserChangeDlg()
{
}

void CUserChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_USERLIST, m_CbUserList);
	DDX_Text(pDX, IDC_ED_PASSWORD, m_EdPassword);
	DDX_Text(pDX, IDC_ED_PASSWORD2, m_EdChangePassword);
	DDX_Control(pDX, IDC_ST_PASSWORD, m_StaticPassword);
	DDX_Control(pDX, IDC_ST_PASSWORD2, m_StaticChangePassword);
	DDX_Check(pDX, IDC_CHK_PASSWORD_CHANGE, m_ChkChangePassword);
}


BEGIN_MESSAGE_MAP(CUserChangeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUserChangeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_PASSWORD_CHANGE, &CUserChangeDlg::OnBnClickedChkPasswordChange)
	ON_CBN_SELCHANGE(IDC_CB_USERLIST, &CUserChangeDlg::OnCbnSelchangeCbUserlist)
	ON_BN_CLICKED(IDCANCEL, &CUserChangeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CUserChangeDlg 메시지 처리기
BOOL CUserChangeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	int nCount = UserModeDefine::enMax;

	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_CB_USERLIST);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDC_ED_PASSWORD);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDC_ED_PASSWORD2);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDC_ST_USERMODE);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDC_ST_PASSWORD);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDC_ST_PASSWORD2);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDOK);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->SetFont(&m_font);

	pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
	pWnd->SetFont(&m_font);

	for (int i = 0; i < nCount; i++) {
		m_CbUserList.AddString(UserModeDefine::szName[i]);
	}

	// 22.09.15 Ahn Modify Start
	//if (AprData.UserMode == UserModeDefine::enOperator) {
	//	m_CbUserList.SetCurSel(1);
	//}
	//else {
	//	m_CbUserList.SetCurSel(0);
	//}
	switch (AprData.UserMode) {
	case	UserModeDefine::enOperator :
	case	UserModeDefine::enEngineer :
	case	UserModeDefine::enMaker :
		m_CbUserList.SetCurSel(AprData.UserMode);
		break;
	default :
		m_CbUserList.SetCurSel(0);
		break;
	}
	// 22.09.15 Ahn Modify End

	LoadPassword();

	m_ChkChangePassword = FALSE;
	if (m_CbUserList.GetCurSel() == 0)
	{
		pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
		pWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
		pWnd->ShowWindow(SW_SHOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CUserChangeDlg::LoadPassword()
{
	// TODO: 여기에 구현 코드 추가.
	CString strFileName = _T("");
	strFileName.Format(_T("%s\\PASSWORD.ini"), AprData.m_strDataPath);

	int nCount = UserModeDefine::enMax;
	CString strSection;
	CString strKey;
	CString strData;
	if (PathFileExists(strFileName) == FALSE) {
		strSection = _T("PASSWORD");
		for (int i = 0; i < nCount; i++) {
			strKey.Format(_T("%s"), UserModeDefine::szName[i]);
			strData = _T("0000");
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
		}
	}
	else {
		char buff[256];
		CString strTmp;
		strSection = _T("PASSWORD");
		for (int i = 0; i < nCount; i++) {
			strKey.Format(_T("%s"), UserModeDefine::szName[i]);
			::GetPrivateProfileString(strSection, strKey, "0000", buff, 256, strFileName);
			strPassword[i].Format(_T("%s"), buff);
		}
	}
}


void CUserChangeDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_CbUserList.GetCurSel();
	if (m_ChkChangePassword == TRUE)
	{
		CWnd* pWnd;
		UpdateData(TRUE);
		if ((m_EdChangePassword.GetLength() > 0) && (m_EdPassword.Compare(strPassword[nSel]) == 0)) {
			if (AfxMessageBox(_T("패스워드를 변경하시겠습니까?"), MB_YESNO) != IDYES) {
				return;
			}
			else {
				ChangePassword(nSel);
			}
			pWnd = GetDlgItem(IDOK);
			pWnd->ShowWindow(SW_SHOW);
			pWnd->SetWindowTextA(_T("확인"));
			pWnd = GetDlgItem(IDC_ST_PASSWORD2);
			pWnd->ShowWindow(SW_HIDE);
			pWnd = GetDlgItem(IDC_ED_PASSWORD2);
			pWnd->ShowWindow(SW_HIDE);
			m_StaticPassword.SetWindowText(_T("비밀번호"));
			pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
			pWnd->ShowWindow(SW_SHOW);
			pWnd->SetWindowTextA(_T("비밀번호 변경"));
		}
		else {
			AfxMessageBox(_T("현재 비밀번호와 변경하고자하는 비밀번호를 확인해주세요"));
			pWnd = GetDlgItem(IDOK);
			pWnd->ShowWindow(SW_SHOW);
			pWnd->SetWindowTextA(_T("확인"));
			pWnd = GetDlgItem(IDC_ST_PASSWORD2);
			pWnd->ShowWindow(SW_HIDE);
			pWnd = GetDlgItem(IDC_ED_PASSWORD2);
			pWnd->ShowWindow(SW_HIDE);
			m_StaticPassword.SetWindowText(_T("비밀번호"));
			pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
			pWnd->SetWindowTextA(_T("비밀번호 변경"));
			pWnd->ShowWindow(SW_SHOW);
		}
		m_ChkChangePassword = FALSE;
	}
	else
	{
		UpdateData(TRUE);
		// 22.09.20 Ahn Modify Start
		if (nSel == UserModeDefine::enMaker) {
			if ((m_EdPassword == strPassword[nSel])) {
				AprData.UserMode = UserModeDefine::enMaker;
				CDialogEx::OnOK();
			}
			else {
				AfxMessageBox(_LANG(_T("패스워드가 잘못 되었습니다."), _T("Entered Password is worng")));
			}
		}
		else
		{
			if ((m_EdPassword == strPassword[nSel]) || (nSel == 0)) {
				AprData.UserMode = (UserModeDefine::eMode)nSel;
				CDialogEx::OnOK();
			}
			else {
				AfxMessageBox(_LANG(_T("패스워드가 잘못 되었습니다."), _T("Entered Password is worng")));
			}
		}
	}
}


void CUserChangeDlg::OnBnClickedChkPasswordChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_CbUserList.GetCurSel();
	CWnd* pWnd;

	if (m_ChkChangePassword == FALSE && (nSel != 0)) {
		m_StaticPassword.SetWindowText(_T("현재 비밀번호"));
		pWnd = GetDlgItem(IDOK);
		pWnd->ShowWindow(SW_SHOW);
		pWnd->SetWindowTextA(_T("변경"));
		pWnd = GetDlgItem(IDC_ST_PASSWORD2);
		pWnd->ShowWindow(SW_SHOW);
		pWnd = GetDlgItem(IDC_ED_PASSWORD2);
		pWnd->ShowWindow(SW_SHOW);
		pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
		pWnd->ShowWindow(SW_HIDE);
		m_ChkChangePassword = TRUE;

	}
}


void CUserChangeDlg::ChangePassword(int nMode)
{
	// TODO: 여기에 구현 코드 추가.
	CString strFileName = _T("");
	strFileName.Format(_T("%s\\PASSWORD.ini"), AprData.m_strDataPath);

	CString strSection;
	CString strKey;
	CString strData;
	strSection = _T("PASSWORD");
	strKey.Format(_T("%s"), UserModeDefine::szName[nMode]);
	strData.Format(_T("%s"), m_EdChangePassword);
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);

	strPassword[nMode] = strData;
	m_EdPassword = strData;
	UpdateData(FALSE);
}

void CUserChangeDlg::OnCbnSelchangeCbUserlist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd = nullptr;
	pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
	pWnd->ShowWindow(SW_SHOW);
	pWnd->SetWindowTextA(_T("비밀번호 변경"));
	pWnd = GetDlgItem(IDC_ST_PASSWORD2);
	pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_ED_PASSWORD2);
	pWnd->ShowWindow(SW_HIDE);
	m_StaticPassword.SetWindowText(_T("비밀번호"));

	m_ChkChangePassword = FALSE;

	if (m_CbUserList.GetCurSel() == 0)
	{
		pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
		pWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		pWnd = GetDlgItem(IDC_CHK_PASSWORD_CHANGE);
		pWnd->ShowWindow(SW_SHOW);
	}
	UpdateData(FALSE);
}


void CUserChangeDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
	m_ChkChangePassword = FALSE;
}
