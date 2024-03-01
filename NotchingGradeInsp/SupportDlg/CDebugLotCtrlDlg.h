#pragma once


// CDebugLotCtrlDlg 대화 상자

class CNotchingGradeInspView;

class CDebugLotCtrlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDebugLotCtrlDlg)

public:
	CDebugLotCtrlDlg(CNotchingGradeInspView* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDebugLotCtrlDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DEBUG_CTRL };
#endif

	CNotchingGradeInspView* m_pView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLotChange();
	afx_msg void OnBnClickedBtnLotEnd();
};
