#pragma once


// CUserChangeDlg 대화 상자

class CUserChangeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserChangeDlg)

public:
	CUserChangeDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUserChangeDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_USER_CHANGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CbUserList;
	CString m_EdPassword;
	virtual BOOL OnInitDialog();
	void LoadPassword();

	CString strPassword[UserModeDefine::enMax];
	afx_msg void OnBnClickedOk();
	CString m_EdChangePassword;
	CStatic m_StaticPassword;
	CStatic m_StaticChangePassword;
	afx_msg void OnBnClickedChkPasswordChange();
	BOOL m_ChkChangePassword;
	void ChangePassword(int nMode);
	CFont	m_font;
	afx_msg void OnCbnSelchangeCbUserlist();
	afx_msg void OnBnClickedCancel();
};
