#pragma once

// CDebugSetDlg 대화 상자
#include "CDebugSet.h"


class CDebugSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDebugSetDlg)

public:
	CDebugSetDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDebugSetDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DEBUG_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();


	CDebugSet m_DebugSet;
	virtual BOOL OnInitDialog();

private :
	CCheckListBox m_chkListDebug ;
public:
	afx_msg void OnLbnSelchangeListCheckDebug();
};
