#pragma once

#include "ColorControl.h"

// CInspDlg 대화 상자

class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CImageDispDlg;
class CInspDlg : public CSubDialogEx
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CString m_strDebugStatus;
	afx_msg void OnDestroy();
};
