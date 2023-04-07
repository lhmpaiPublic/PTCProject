// CalendarCellItem.cpp: 구현 파일
//

#include "pch.h"
#include "CalendarCellItem.h"


// CCalendarCellItem

CCalendarCellItem::CCalendarCellItem()
{
	bMilTime = bHighlight = bAlarm = FALSE;
	dtAlarm = COleDateTime::GetCurrentTime();
	dtEnd = dtStart = dtAlarm;
	nItemID = -1;
	strItem = "Unknown Item";
	clrItem = RGB(0, 0, 0);
	clrBackGrnd = RGB(255, 255, 213);
	nFontSize = 85;
	strFontName = "TAHOMA";
	nImage = -1;
	pImgList = NULL;
	bDispDate = TRUE;
}

CCalendarCellItem::~CCalendarCellItem()
{
}


// CCalendarCellItem 멤버 함수

void CCalendarCellItem::SetItemID(long nID)
{
	nItemID = nID;
}

long CCalendarCellItem::GetItemID()
{
	return nItemID;
}

void CCalendarCellItem::SetStartTime(COleDateTime dtDate)
{
	dtStart = dtDate;
}

void CCalendarCellItem::GetStartTime(COleDateTime* dtDate)
{
	*dtDate = dtStart;
}

void CCalendarCellItem::SetEndTime(COleDateTime dtDate)
{
	dtEnd = dtDate;
}

void CCalendarCellItem::GetEndTime(COleDateTime* dtDate)
{
	*dtDate = dtEnd;
}

void CCalendarCellItem::SetItemText(CString strLine)
{
	strItem = strLine;
}

CString CCalendarCellItem::GetItemText()
{
	return strItem;
}

void CCalendarCellItem::SetItemAlarm(BOOL bSet, COleDateTime dtDate)
{
	bAlarm = bSet;
	dtAlarm = dtDate;
}

BOOL CCalendarCellItem::IsAlarmSet()
{
	return bAlarm;
}

void CCalendarCellItem::GetAlarmTime(COleDateTime* dtDate)
{
	*dtDate = dtAlarm;
}

void CCalendarCellItem::SetAlarmTime(COleDateTime dtDate)
{
	dtAlarm = dtDate;
}

void CCalendarCellItem::SetItemColor(COLORREF clrColor)
{
	clrItem = clrColor;
}

COLORREF CCalendarCellItem::GetItemColor()
{
	return clrItem;
}

void CCalendarCellItem::SetItemFont(int nSize, CString strFName)
{
	nFontSize = nSize;
	strFontName = strFName;
}

void CCalendarCellItem::GetRectArea(CRect* pRect)
{
	*pRect = rArea;
}

void CCalendarCellItem::SetRectArea(CRect* pRect)
{
	rArea.CopyRect(pRect);
}

void CCalendarCellItem::SetHightlight(BOOL bSet)
{
	bHighlight = bSet;
}

void CCalendarCellItem::DrawItem(CPaintDC* pDC, CRect* pCellArea, BOOL bMView/*= FALSE*/)
{
	COleDateTime dtS, dtE;
	int nItemHeight = GetSystemMetrics(SM_CXVSCROLL);
	CString strBuf;
	COLORREF clrHL = GetSysColor(COLOR_HIGHLIGHT);	// Highlight color

	// Build string line to display
	GetStartTime(&dtS);
	GetEndTime(&dtE);
	if (bMilTime)
	{
		if (GetDispDate() == TRUE)
		{
			if (!bMView)
				strBuf.Format(_T("%s - %s  %s"), dtS.Format("%H%M"), dtE.Format("%H%M"), GetItemText());
			else
				strBuf.Format(_T("%s  %s"), dtS.Format("%H%M"), GetItemText());
		}
		else
		{
			strBuf.Format(_T("%s"), GetItemText());
		}
	}
	else
	{
		if (GetDispDate() == TRUE)
		{
			if (!bMView)
				strBuf.Format(_T("%s  %s  %s"), dtS.Format("%I:%M%p"), dtE.Format("%I:%M%p"), GetItemText());
			else
				strBuf.Format(_T("%s  %s"), dtS.Format("%I:%M%p"), GetItemText());
		}
		else
		{
			strBuf.Format(_T("%s"), GetItemText());
		}
	}

	// Calculate Rectangle - for highlighted cell
	CRect rText;
	rText.CopyRect(&rArea);
	pDC->DrawText(strBuf, &rText, DT_CALCRECT);
	if (bHighlight)
	{
		pDC->FillSolidRect(&rArea, clrHL);	// Highlight Header
		pDC->SetTextColor(RGB(255, 255, 255));
	}
	else
		pDC->FillSolidRect(&rArea, clrBackGrnd); // Display normal background

	// Draw the items in the order they appear!!
	if (nImage > -1 && pImgList != NULL)
	{
		// Draw specified image from ImageList make space for 
		// icon when displaying the text
		if (pImgList->Draw(pDC, nImage, rArea.TopLeft(), ILD_TRANSPARENT) == TRUE)
			rText.left += 20;	// Space for icon
	}
	rText.right = rArea.right;
	pDC->DrawText(strBuf, &rText, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(clrItem);
}

void CCalendarCellItem::SetMilitaryTime(BOOL bSet)
{
	bMilTime = bSet;
}

void CCalendarCellItem::SetBkColor(COLORREF clrBk)
{
	clrBackGrnd = clrBk;
}

void CCalendarCellItem::SetItemData(DWORD dwD)
{
	dwData = dwD;
}

DWORD CCalendarCellItem::GetItemData()
{
	return dwData;
}

void CCalendarCellItem::SetImageList(CImageList* pIList)
{
	pImgList = pIList;
}

void CCalendarCellItem::SetItemImage(int nImg)
{
	nImage = nImg;
}

void CCalendarCellItem::SetDispDate(BOOL bDisp)
{
	bDispDate = bDisp;
}

BOOL CCalendarCellItem::GetDispDate()
{
	return bDispDate;
}

