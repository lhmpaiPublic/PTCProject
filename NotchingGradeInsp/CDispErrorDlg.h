#pragma once


// CDispErrorDlg 대화 상자

class CDispErrorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDispErrorDlg)

public:
	CDispErrorDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDispErrorDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DISPERROR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CColorStatic m_ErrTextStatic;
	CListBox m_ListBox;
	bool AddMessage(CString msg);
	bool IsError();
	bool m_bReset;
private:
	CCriticalSection m_cs;
	void Data2Dlg();
	CFont m_Font, m_FontListBox;
	CStringList m_ListMsg;
	int m_Cnt;

	int IsAlarmReset();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};
