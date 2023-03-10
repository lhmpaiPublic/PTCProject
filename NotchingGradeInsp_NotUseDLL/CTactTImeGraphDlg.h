#pragma once

#include "GraphCtrl.h"
//class CGraphCtrl;

// CTactTImeGraphDlg 대화 상자

class CTactTimeGraphDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTactTimeGraphDlg)

public:
	CTactTimeGraphDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTactTimeGraphDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_TACT_TIME_GRAPH };
#endif

public :
	void AddData();
	void DataClearAll();
private :
	// 22.12.13 Ahn Modify Start
	//CGraphCtrl *m_pGraphTact ;
	// 23.02.08 Son Mod Start
	//CGraphCtrl m_graphTact = { 1 };
	CGraphCtrl m_pGraphTact;
	// 23.02.08 Son Mod End
	// 22.12.13 Ahn Modify End


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
