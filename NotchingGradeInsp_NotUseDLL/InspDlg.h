#pragma once

#include "ColorControl.h"

// CInspDlg 대화 상자

class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CImageDispDlg;
class CInspDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInspDlg)

public:
	CInspDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다.
	virtual ~CInspDlg();

protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	int m_nIspState;
	// 22.06.13 Ahn Delete End
	//CMFCButton m_rdInspRun;
	//CMFCButton m_rdInspStop;
	//CMFCButton m_rdInspPause;
	// 22.06.13 Ahn Delete End

	CImageDispDlg* m_pImgTabDlg;
	CImageDispDlg* m_pImgBtmDlg;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INSP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	UINT_PTR m_TID_Renewal;
	BOOL SetRenewalTimer();
	BOOL KillRenewalTimer();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// 22.06.13 Ahn Delete Start
	//afx_msg void OnBnClickedRdRun();
	//afx_msg void OnBnClickedRdStop();
	//afx_msg void OnBnClickedRdPause();
	//afx_msg void OnBnClickedBtnLotChange();
	// 22.06.13 Ahn Delete End

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// 22.06.13 Ahn Delete Start
	//void ChangeStatus(int nState);
	// 22.06.13 Ahn Delete End

	CString m_strDebugStatus;
	afx_msg void OnDestroy();

	// 22.07.06 Ahn Delete Start - 미사용
	//// 22.06.13 Ahn Add Start
	//void InspRun();
	//void InspStop();
	//// 22.06.13 Ahn Add End
	// 22.07.06 Ahn Delete End
};
