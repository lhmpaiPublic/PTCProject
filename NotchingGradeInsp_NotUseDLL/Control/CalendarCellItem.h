#pragma once

// CCalendarCellItem 명령 대상

class CCalendarCellItem : public CObject
{
public:
	CCalendarCellItem();
	virtual ~CCalendarCellItem();

private:
	int nImage;
	CImageList* pImgList;
	DWORD dwData;
	COLORREF clrBackGrnd;
	BOOL bMilTime;
	long nItemID;
	COleDateTime dtStart, dtEnd;
	CString strItem;
	COLORREF clrItem;
	BOOL bAlarm, bHighlight;
	COleDateTime dtAlarm;
	CRect rArea;
	int nFontSize;
	CString strFontName;
	BOOL bDispDate;

public:
	void SetItemImage(int nImg);
	void SetImageList(CImageList* pIList);
	DWORD GetItemData();
	void SetItemData(DWORD dwD);
	void SetBkColor(COLORREF clrBk);
	void SetMilitaryTime(BOOL bSet);
	void DrawItem(CPaintDC* pDC, CRect* pCellArea, BOOL bMView = FALSE);
	void SetHightlight(BOOL bSet);
	void SetRectArea(CRect* pRect);
	void GetRectArea(CRect* pRect);
	void SetItemFont(int nSize, CString strFName);
	COLORREF GetItemColor();
	void SetItemColor(COLORREF clrColor);
	void SetAlarmTime(COleDateTime dtDate);
	void GetAlarmTime(COleDateTime* dtDate);
	BOOL IsAlarmSet();
	void SetItemAlarm(BOOL bSet, COleDateTime dtDate);
	CString GetItemText();
	void SetItemText(CString strLine);
	void GetEndTime(COleDateTime* dtDate);
	void SetEndTime(COleDateTime dtDate);
	void GetStartTime(COleDateTime* dtDate);
	void SetStartTime(COleDateTime dtDate);
	long GetItemID();
	void SetItemID(long nID);
	void SetDispDate(BOOL bDisp);
	BOOL GetDispDate();
};


