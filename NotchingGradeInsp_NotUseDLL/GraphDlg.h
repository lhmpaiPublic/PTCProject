#pragma once

#include "GraphCtrl.h"

#define T_GRAPH_REFRESH 102 

// CGraphDlg 대화 상자
class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CGraphDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGraphDlg)

public:
	CGraphDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다..
	virtual ~CGraphDlg();

private:
	int m_nCount;  //TEST

protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	CGraphCtrl m_cGraphCam1 = { 1 };
	CGraphCtrl m_cGraphCam2 = { 1 };
	CGraphCtrl m_cGraphCam3 = { 1 };
	CGraphCtrl m_cGraphCam4 = { 1 };

	int		m_nMaxSize ;

public:

	void OnRefresh();
	void DataClearAll();
	
	// 22.06.20 Ahn Add Start
	enum{
		en_FoilExpGraph = 0,
		en_SurfaceGraph = 1,
		en_MaxGraphType = 2,
	};
	void SetMaxVerSize( int nMode, int nSize) ;
	void SetThreshold( int nMode, int nThreshold) ;
	// 22.06.20 Ahn Add Emd

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_GRAPH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
