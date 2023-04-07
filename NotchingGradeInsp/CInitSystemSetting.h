#pragma once


// CInitSystemSetting 대화 상자

class CInitSystemSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CInitSystemSetting)

public:
	CInitSystemSetting(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CInitSystemSetting();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INITSYSTEMSETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButEqpSetting();
	afx_msg void OnBnClickedButCameraSetting();
	afx_msg void OnBnClickedButDisplaySetting();
	afx_msg void OnBnClickedButResultDelete();
	afx_msg void OnBnClickedButOtherSetting();
	afx_msg void OnBnClickedButAddressSetting();
};
