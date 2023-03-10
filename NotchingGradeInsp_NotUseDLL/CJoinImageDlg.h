#pragma once


// CJoinImageDlg 대화 상자

class CJoinImageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJoinImageDlg)

public:
	CJoinImageDlg(CWnd* pParent = nullptr, CString strFilePath = _T("") );   // 표준 생성자입니다.
	virtual ~CJoinImageDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ATTACH_IMAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoadImg1();
	afx_msg void OnBnClickedBtnLoadImg2();
	afx_msg void OnBnClickedOk();


	CString m_strImageName1;
	CString m_strImageName2;

private :
	CString m_strImagePath;

	CString GetSelectFilePath();
};
