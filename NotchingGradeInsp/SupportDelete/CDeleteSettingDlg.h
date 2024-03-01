#pragma once


// CDeleteSettingDlg 대화 상자

class CDeleteSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDeleteSettingDlg)

public:
	CDeleteSettingDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDeleteSettingDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DELETE_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedChkNotuseBackup();
	BOOL m_bChkBackupNotUse;
	BOOL m_bChkStopDuringRun;
	afx_msg void OnBnClickedChkStopDuringRun();
	CEdit m_EdDummy;

	int DataControl(int nMode);
	int OnReflesh();

	afx_msg void OnEnSetfocusEdBackupDirectory();
	afx_msg void OnEnSetfocusEdBackupDay();
	afx_msg void OnEnSetfocusEdDeleteDay();
	afx_msg void OnEnSetfocusEdStartTime();
	afx_msg void OnEnSetfocusEdEndTime();
	CString m_strEdBackupPath;
	int m_nEdBackupDate;
	int m_nEdDeleteDate;
	int m_nEdStartTime;
	int m_nEdEndTime;
};
