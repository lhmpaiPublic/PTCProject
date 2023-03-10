#pragma once

// CCalenderCtrl

#include "CalendarCell.h"

class CCustomListCtrl : public CMFCListCtrl
{
	virtual COLORREF OnGetCellTextColor(int nRow, int nColum);
	virtual COLORREF OnGetCellBkColor(int nRow, int nColum);
	virtual HFONT OnGetCellFont(int nRow, int nColum, DWORD dwData = 0);

	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);

public:
	BOOL m_bColor;
	BOOL m_bModifyFont;
	int m_SelectedListIndex;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
};

typedef struct tagNM_WVCELLDATA {
	NMHDR hdr;
	CCalendarCell* pCell;
	CCalendarCellItem* pItem;
	int   iRow;
	int   iColumn;
	BOOL bDNFClicked;
	long nItem;
} NM_WVCELLDATA;

#define WV_TOTALCELLS 42
#define WV_WEEKVIEW WM_USER + 1
#define WV_MONTHVIEW WM_USER + 2

class CCalenderCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCalenderCtrl)

public:
	CCalenderCtrl();
	virtual ~CCalenderCtrl();

private:
	CImageList* pImageList;
	void DrawMonthCellHeaders(CPaintDC* pDC);
	COleDateTime dtMonth;
	void DrawMonthHeader(CPaintDC* pDC);
	BOOL bMonthView;
	CCalendarCell* GetCellByDate(COleDateTime dtDate);
	void PaintMonthView(CPaintDC* pDC);
	void PaintWeekView(CPaintDC* pDC);
	void DrawWeekCellHeaders(CPaintDC* pDC);
	BOOL bMilTime;
	CString strFontName;
	int nFontSize;
	long nUniqueItemID;
	CCalendarCell* pCurrentCell;
	CCalendarCell* GetCellByID(int nID);
	CObList m_CellList;
	void FillGradientRect(CDC* pDC, CRect* pRect);
	COleDateTime dtCurrentDate;
	COLORREF clrBackGrnd, clrOtherMonthBk;
	COLORREF clrGradBot, clrGradTop, clrGradLine;
	BOOL RegisterWindowClass();
	CCustomListCtrl* m_pList;
	CFont* m_pListFont;

protected:
	LRESULT SendMessageToParent(int nRow, int nCol, int nMessage) const;

public:
	void RedrawControl();
	BOOL IsMilitaryTime();
	void SetMilitaryTime(BOOL bMil = TRUE);
	int GetDay();
	int GetView();
	void SetImageList(CImageList* pImgList);
	void SetItemColor(long nItem, COLORREF clrItem);
	int GetYear();
	int GetMonth();
	void SetView(int nV, BOOL bRedraw = TRUE);
	void GetDateRange(COleDateTime* pStartDate, COleDateTime* pEndDate);
	void DeleteAllItems();
	BOOL DeleteItem(long nItem);
	DWORD GetItemData(long nItem);
	void SetItemData(long nItem, DWORD dwData);
	COleDateTime GetCurrentDate();
	void SetFont(CString strFName, int nSize);
	void SetCurrentDate(COleDateTime dtDate, BOOL bMilitaryTime = FALSE, int nView = WV_MONTHVIEW);
	long InsertItem(COleDateTime dtStart, COleDateTime dtEnd, CString strLine = _T(""), BOOL bDispDate = TRUE, int nImage = -1);
	void SetBkColor(COLORREF clrBk);
//KANG 21.10.21 Add Start
	void ClearCurrentCell();
	void SetSelectCell(COleDateTime dtDate);
	int GetCellID(COleDateTime dtDate);
	COleDateTime GetDateByID( int nID );
	int GetTotalCellNum();
//KANG 21.10.21 Add End

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


