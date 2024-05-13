#pragma once

#include "GridCtrl.h"
#include "ColorControl.h"

#define T_GRID_REFRESH 101 
//KANG 22.01.07 Add Start
#define IDC_HIST_LIST 1002
//KANG 22.01.07 Add End

// CResultViewDlg 대화 상자
class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CDefectDataCtrl;
class CImageViewerDlg;	// 22.06.23 Ahn Add
class CDefectInfo;		// 22.07.07 Ahn Add

class CResultViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResultViewDlg)

public:
	CResultViewDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다..
	virtual ~CResultViewDlg();

protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	CGridCtrl*	m_pResultList;
	CFont*		m_pFontGrid;
	BOOL		m_bAutoRefresh;

	int			m_nSelectRow;	// 22.07.01 Ahn Add

public:
	int MakeGrid();
	int UpdateGrid();
	void SelectResultRow(int nRow);

// 22.06.09 Ahn Add Starrt
	int CounterReset();
// 22.06.09 Ahn Add End

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RESULT };
#endif

private :
	// 22.06.23 Ahn Add Start
	CImageViewerDlg* m_pImageViewerDlg;
	void DisplayLanguage();
	enum {
		en_col_No	= 0,
		en_col_InspTime = 1,
		en_col_LOT_ID = 2,
		en_col_CELL_NO = 3,
		en_col_CELL_ID = 4,
		en_col_POSITION = 5,
		en_col_DEF_TYPE	= 6,
		en_col_JUDGE = 7,
		en_col_DEF_SIZE	= 8,
		en_col_InkMarking = 9,
		en_col_DefPosY	= 10,
		en_col_ImagePath = 11,
		en_Max_GridCols = 12,
	};
	// 22.06.23 Ahn Add End

	void LoadLastLotData();
	int  AddGridOneLine(CDefectInfo* pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnBnClickedChkRefresh();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// 22.06.23 AHn Modify Start
	//afx_msg void OnMouseLDownHistList(NMHDR* pNMHDR, LRESULT* pResult);		//KANG 22.01.07 Add
	//afx_msg void OnMouseDblClickHistList(NMHDR* pNMHDR, LRESULT* pResult);	//KANG 22.01.07 Add
	// 22.06.23 AHn Modify End
	virtual BOOL PreTranslateMessage(MSG* pMsg);							//KANG 22.05.24 Add
	afx_msg void OnMouseDblClickReltList(NMHDR* pNMHDR, LRESULT* pResult);	 // 22.06.23 Ahn Add 
	afx_msg void OnRBottonDownReltList(NMHDR* pNMHDR, LRESULT* pResult);	 // 22.07.01 Ahn Add 
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonIpconfig();
};
