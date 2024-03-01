// CUserLoginDlg.cpp : implementation file
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "afxdialogex.h"
#include "CUserLoginDlg.h"

#include "GlobalData.h"

// CUserLoginDlg dialog

IMPLEMENT_DYNAMIC(CUserLoginDlg, CDialogEx)

CUserLoginDlg::CUserLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_USER_LOGIN, pParent)
{
	bLogin = FALSE;

	LoadPassword();
}

CUserLoginDlg::~CUserLoginDlg()
{
}

void CUserLoginDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_PASSWORD, m_EdPassword);
}


BEGIN_MESSAGE_MAP(CUserLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUserLoginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUserLoginDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CUserLoginDlg message handlers


void CUserLoginDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	GetDlgItem(IDC_ED_PASSWORD)->GetWindowTextA(m_EdPassword);

	if ((m_EdPassword != strPassword[0])) {
		AfxMessageBox("Invalid password.");
		return;
	}

	bLogin = TRUE;

	CDialogEx::OnOK();
}


void CUserLoginDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here

	bLogin = FALSE;

	CDialogEx::OnCancel();
}

void CUserLoginDlg::LoadPassword()
{
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
