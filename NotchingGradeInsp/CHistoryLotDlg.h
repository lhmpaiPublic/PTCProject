#pragma once

#include "GridCtrl.h"
#include "ColorControl.h"

// CHistoryLotDlg 대화 상자

class CNotchingGradeInspDoc;
class CNotchingGradeInspView;

class CHistoryLotDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHistoryLotDlg)

public:
	CHistoryLotDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다.

	virtual ~CHistoryLotDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_HISTORY_LOT };
#endif

protected :
	CWnd*					m_pParent;
	CNotchingGradeInspDoc*	m_pDoc;
	CNotchingGradeInspView* m_pView;
	CGridCtrl*				m_pHistoryList;
	CFont*					m_pFontGrid;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public :
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseLDownHistoryList(NMHDR* pNMHDR, LRESULT* pResult);

public :
	enum {
		en_col_no				= 0,
		en_col_Date				= 1,
		en_col_LotId			= 2,
		en_col_StartTime		= 3,
		en_col_EndTime			= 4,
		en_col_TotalCount		= 5,
		en_col_OK_Count			= 6,
		en_col_OK_Rate			= 7,
		en_col_NG_Count			= 8,
		en_col_NG_Rate			= 9,
		en_col_Top_NG_Count		= 10,
		en_col_Btm_NG_Count		= 11,
		en_col_Marking_Count	= 12 ,
		en_col_Marking_Rage		= 13,
		en_Max_GridCols			= 14,
	};


private :

	int MakeHitoryGrid();
	int UpdateHistoryGrid();
public:

	void RefreshAll();
	afx_msg void OnClose();
	virtual void OnOK();

	//마킹 BCD ID를 갱신한다.
	void UpdateBCDID();
	afx_msg void OnDeltaposSpinBcdidDiff(NMHDR* pNMHDR, LRESULT* pResult);
};
