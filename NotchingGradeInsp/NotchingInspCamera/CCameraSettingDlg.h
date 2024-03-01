#pragma once


// CCameraSettingDlg 대화 상자

class CCameraSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraSettingDlg)

public:
	CCameraSettingDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCameraSettingDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CAMERA_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSetSpeed();
	afx_msg void OnBnClickedBtnSetExptime();
	afx_msg void OnBnClickedBtnSetGain();
	afx_msg void OnBnClickedBtnSetAll();
	afx_msg void OnBnClickedOk();
	double m_dEdSpeed;
	double m_dLineRate;
	double m_dEdExposureTime;
	double m_dEdGian;
	afx_msg void OnEnSetfocusEdSpeed();
	afx_msg void OnEnSetfocusEdExposureTime();
	afx_msg void OnEnSetfocusEdGain();
	virtual BOOL OnInitDialog();

	CEdit m_EdDummy;
};
