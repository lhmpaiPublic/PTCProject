#pragma once


// CDIOTestDlg 대화 상자

class CDIOTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDIOTestDlg)

public:
	CDIOTestDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDIOTestDlg();

	BOOL CloseDevice();
	BOOL OnModuleInfo(long	lModuleNo, short* InputCount);
	BOOL ThreadData();
	BOOL AXTInit();
	DWORD GetBitPat(short nIndex);

public:
	CButton		m_radioMessage;
	CButton		m_chkRising;
	CListBox	m_listMessage;
	CButton		m_chkFalling;
	CButton		m_chkIntrrupt;
	CButton		m_stcLow;
	CButton		m_stcHigh;
	CComboBox	m_cboModuleInfo;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DIO_TEST };
#endif

// 구현입니다.
private:
	HANDLE		m_hHandle;
	HANDLE		m_hInterrupt;
	BOOL		m_bThread;

	WORD		m_wLastInfo;

protected:
	HICON		m_hIcon;
	CButton* m_pchkHigh[16];
	CButton* m_pchkLow[16];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeCombo();
	afx_msg void OnCheckIntrrupt();
	afx_msg void OnCheckRising();
	afx_msg void OnCheckFalling();
	afx_msg void OnListClear();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRadioMessage();
	afx_msg void OnRadioCallback();
	afx_msg void OnRadioEvent();
	afx_msg void OnButtonExit();
	afx_msg void OnClickHigh(UINT nID);
	afx_msg void OnClickLow(UINT nID);
	afx_msg LRESULT OnInterruptMessage(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnDIOInterruptCallback(long nModuleNo, DWORD uFlag);
	DECLARE_MESSAGE_MAP()
	
public:
	BOOL m_bChkTestRun;
	afx_msg void OnBnClickedChkFreerun();
};
