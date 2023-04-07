#pragma once


// CHistogramDlg 대화 상자
#include "GraphCtrl.h"

class CHistogramDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHistogramDlg)

public:
	CHistogramDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CHistogramDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_HISTOGRAM };
#endif


	CGraphCtrl m_cGrphHist ;
	int m_nMax; 
	int m_nMin;
	int m_nSum;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void SetHistoData(int* nHistoArray, int nSize );
	afx_msg void OnBnClickedOk();
};
