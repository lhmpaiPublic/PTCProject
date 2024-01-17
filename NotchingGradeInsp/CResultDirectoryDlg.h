#pragma once


// CResultDirectoryDlg 대화 상자

class CResultDirectoryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResultDirectoryDlg)

public:
	CResultDirectoryDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CResultDirectoryDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DIRECTORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCsvOpen();
	afx_msg void OnBnClickedBtnOpenNgImg();
	afx_msg void OnBnClickedBtnOpenOkImg();
	virtual BOOL OnInitDialog();
	CString m_strCsvPath;
	CString m_strNgImagePath;
	CString m_strOkImagePath;
	void DisplayLanguage();
};
