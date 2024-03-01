#pragma once
#include "afxdialogex.h"


// CUserLoginDlg dialog

class CUserLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserLoginDlg)

public:
	CUserLoginDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CUserLoginDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_USER_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	BOOL bLogin;
	CString m_EdPassword;

	afx_msg void OnBnClickedCancel();

	void LoadPassword();

	CString strPassword[UserModeDefine::enMax];
};
