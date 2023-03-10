#pragma once


// CStartDlg 대화 상자

#define T_ID_START 107

class CStartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStartDlg)

public:
	CStartDlg(CWnd* pParent = nullptr, int nStartMode = en_AppStart );   // 표준 생성자입니다.
	virtual ~CStartDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_START };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMessage;
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	virtual BOOL OnInitDialog();
	CProgressCtrl m_ProgressTimer;

private :
	CTime TimeEnd;
	CTime TimeStart;
	LONG  lWait_Time;
	int nPos;

	int m_nStartMode; 
public :
	enum {
		en_AppStart  = 0,
		en_InspStart ,
		en_MaxStartMode ,
	};
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnDebugSetting();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnSystemSet();
};
