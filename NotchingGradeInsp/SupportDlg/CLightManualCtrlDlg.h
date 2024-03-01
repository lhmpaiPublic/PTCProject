#pragma once


// CLightManualCtrlDlg 대화 상자

class CLightManualCtrlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLightManualCtrlDlg)

public:
	CLightManualCtrlDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLightManualCtrlDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LIGHT_CTRL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSetAll();
	int m_nLightRef_1;
	int m_nLightRef_2;
	int m_nLightDiffRef_1;
	int m_nLightDiffRef_2;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
