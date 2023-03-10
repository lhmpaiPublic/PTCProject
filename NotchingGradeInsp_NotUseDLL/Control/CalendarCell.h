#pragma once

// CCalendarCell 명령 대상

#include "CalendarCellItem.h"	

class CCalendarCell : public CObject
{
public:
	CCalendarCell();
	virtual ~CCalendarCell();

private:
	CImageList* pImageList;
	COLORREF clrBackGrnd;
	void DrawDoNotFit(CPaintDC* pDC, CRect* pRect);
	long nItemCount;
	BOOL bHighlight, bMilTime, bDNFClicked, bMonthView;
	int nCellID;
	CRect rCell, rHdr, rDNF; // rDNF = Do Not Fit Area Rectangle
	COleDateTime dtCellDate;

public:
	void SetImageList(CImageList* pImgList);
	void SetItemColor(long nItem, COLORREF clrItem);
	void SetHdrRect(CRect* pRect);
	void GetHdrRect(CRect* pRect);
	COLORREF GetBkColor();
	void DeleteAllItems();
	BOOL DeleteItem(long nItem);
	CCalendarCellItem* GetItem(long nItem);
	DWORD GetItemData(long nItem);
	void SetItemData(long nItem, DWORD dwData);
	BOOL GetDNFClick();
	void SetDNFClicked(BOOL bClick);
	void GetDNFRect(CRect* pRect);
	void SetBkColor(COLORREF clrBk);
	void SetMilitaryTime(BOOL bSet);
	BOOL HitItemTest(CPoint point);
	void DrawItems(CPaintDC* pDC, BOOL bMonView = FALSE);
	long InsertCellItem(COleDateTime dtS, COleDateTime dtE, CString strLine, BOOL bDispDate, int nImage);
	CCalendarCellItem* pCellItem;
	CObList m_ItemList;
	BOOL GetHighlight();
	void SetHighlight(BOOL bOK);
	void GetCellHdrRect(CRect* pRect);
	void SetCellHdrRect(CRect* pRect);
	int GetCellID();
	void SetCellID(int nID);
	COleDateTime GetCellDate();
	void SetCellDate(COleDateTime dtD);
	void GetCellRect(CRect* pRect);
	void SetCellRect(CRect* pRect);

};


