// CalendarCell.cpp: 구현 파일
//

#include "pch.h"
#include "CalendarCell.h"


// CCalendarCell

CCalendarCell::CCalendarCell()
{
	rHdr.SetRectEmpty();
	rCell.SetRectEmpty();
	dtCellDate = COleDateTime::GetCurrentTime();
	nCellID = 0;
	nItemCount = 0;
	bDNFClicked = bMilTime = FALSE;
	bMonthView = bHighlight = FALSE;
	clrBackGrnd = RGB(255, 255, 213);	// Default color background

	pCellItem = NULL;	// Current cell item
}

CCalendarCell::~CCalendarCell()
{
	POSITION pos = m_ItemList.GetHeadPosition();
	CObject* pObj = NULL;
	while (pos != NULL)
	{
		pObj = m_ItemList.GetNext(pos);
		delete pObj;
	}
	m_ItemList.RemoveAll();
}


// CCalendarCell 멤버 함수

void CCalendarCell::SetCellRect(CRect* pRect)
{
	rCell.CopyRect(pRect);
}

void CCalendarCell::GetCellRect(CRect* pRect)
{
	pRect->CopyRect(&rCell);
}

void CCalendarCell::SetCellDate(COleDateTime dtD)
{
	dtCellDate = dtD;
}

COleDateTime CCalendarCell::GetCellDate()
{
	return dtCellDate;
}

void CCalendarCell::SetCellID(int nID)
{
	nCellID = nID;
}

int CCalendarCell::GetCellID()
{
	return nCellID;
}

void CCalendarCell::SetCellHdrRect(CRect* pRect)
{
	rHdr.CopyRect(pRect);
}

void CCalendarCell::GetCellHdrRect(CRect* pRect)
{
	pRect->CopyRect(&rHdr);
}

void CCalendarCell::SetHighlight(BOOL bOK)
{
	bHighlight = bOK;
}

BOOL CCalendarCell::GetHighlight()
{
	return bHighlight;
}

// Insert day item into the cell
long CCalendarCell::InsertCellItem(COleDateTime dtS, COleDateTime dtE, CString strLine, BOOL bDispDate, int nImage)
{
	CCalendarCellItem* pItem = new CCalendarCellItem();

	// Fill in item
	pCellItem = pItem;	// Current item
	pItem->SetItemID(nItemCount++);	// Assign Item ID Number
	pItem->SetItemText(strLine);
	pItem->SetStartTime(dtS);
	pItem->SetEndTime(dtE);
	pItem->SetMilitaryTime(bMilTime);
	pItem->SetBkColor(clrBackGrnd);
	pItem->SetItemData(0);
	pItem->SetImageList(pImageList);
	pItem->SetItemImage(nImage);
	pItem->SetHightlight(FALSE);
	pItem->SetDispDate(bDispDate);

	// Add item to list!! - May need to insert based on hour!!
	m_ItemList.AddTail(pItem);

	return pItem->GetItemID();
}

// Draw each individual Cell Data Items but only
// where the dates are the same!!
void CCalendarCell::DrawItems(CPaintDC* pDC, BOOL bMonView/*= FALSE*/)
{
	CRect rItem;
	COleDateTime dtS, dtE, dtItemDate;
	POSITION pos = m_ItemList.GetHeadPosition();
	int nItemHeight = GetSystemMetrics(SM_CXVSCROLL);
	CString strBuf;
	CPoint point;
	BOOL bDoesNotFit = FALSE;

	// Define Item rectangle dimentions for the Cell Area
	rItem.left = rCell.left + 1;
	rItem.top = rCell.top + nItemHeight + 3;
	rItem.right = rCell.right - 1;
	rItem.bottom = rItem.top + nItemHeight - 1;
	// Draw the items in the order they appear!!
	while (pos != NULL)
	{
		pCellItem = (CCalendarCellItem*)m_ItemList.GetNext(pos);
		pCellItem->SetRectArea(&rItem);
		point.x = rItem.left;
		point.y = rItem.bottom;

		// Draw Item text ONLY if the item is still
		// within the Cell rectangle area!!
		pCellItem->GetStartTime(&dtItemDate);	// Get Item date!!
		// Remove time info
		dtItemDate.SetDate(dtItemDate.GetYear(), dtItemDate.GetMonth(),
			dtItemDate.GetDay());

		// Only draw cell item IF within cell area and Item date 
		// corresponds with Cell Data Date
		if (dtCellDate == dtItemDate)
		{
			if (rCell.PtInRect(point))
				pCellItem->DrawItem(pDC, &rItem, bMonView);
			else
				bDoesNotFit = TRUE;
		}

		// Move item area down by nItemHeight
		rItem.top += nItemHeight;
		rItem.bottom = rItem.top + nItemHeight;
	}
	if (bDoesNotFit)
	{
		DrawDoNotFit(pDC, &rCell);
	}
}

BOOL CCalendarCell::HitItemTest(CPoint point)
{
	POSITION pos = m_ItemList.GetHeadPosition();
	CRect rArea;
	BOOL bHit = FALSE;
	CCalendarCellItem* pItem = NULL;

	rArea.SetRectEmpty();
	pCellItem = NULL;	// No current Cell Item!!
	while (pos != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetNext(pos);
		pItem->GetRectArea(&rArea);
		pItem->SetHightlight(FALSE);	// remove all highlight status

		if (rArea.PtInRect(point) == TRUE)
		{
			// We found the clicked item!!
			pCellItem = pItem;	//Current item
			bHit = TRUE;
		}
	}

	return bHit;
}
// Set time to display in Military time format
void CCalendarCell::SetMilitaryTime(BOOL bSet)
{
	bMilTime = bSet;
}

void CCalendarCell::DrawDoNotFit(CPaintDC* pDC, CRect* pRect)
{
	// We need to draw a cell indicator that there are more item
	// within the cell which are not visible.

	// Define rectangle area for the Do Not Fit object
	rDNF.left = pRect->right - 25;
	rDNF.top = pRect->bottom - 20;
	rDNF.right = pRect->right;
	rDNF.bottom = pRect->bottom;

	// Draw the Do Not Fit border
	CBrush brBk(clrBackGrnd);
	CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
	pDC->SelectObject(brBk);
	pDC->SelectObject(penBlack);
	pDC->Rectangle(&rDNF);
	penBlack.DeleteObject();
	brBk.DeleteObject();

	// Draw triangle
	//CPoint pt(rDNF.left + 2, rDNF.top + 2);
	//pDC->MoveTo(pt);
	//pt.x = rDNF.left + 9;
	//pDC->LineTo(pt);
	//// 2nd line
	//pt.x = rDNF.left + 3;
	//pt.y = rDNF.top + 3;
	//pDC->MoveTo(pt);
	//pt.x = rDNF.left + 8;
	//pDC->LineTo(pt);
	//// 3rd line
	//pt.x = rDNF.left + 4;
	//pt.y = rDNF.top + 4;
	//pDC->MoveTo(pt);
	//pt.x = rDNF.left + 7;
	//pDC->LineTo(pt);
	//// 4th line
	//pt.x = rDNF.left + 5;
	//pt.y = rDNF.top + 5;
	//pDC->MoveTo(pt);
	//pt.x = rDNF.left + 6;
	//pDC->LineTo(pt);

	CBrush brush, * pOldBrush;
	brush.CreateSolidBrush(RGB(0, 0, 0));
	pOldBrush = pDC->SelectObject(&brush);
	{
		POINT	poTriangle[3];
		poTriangle[0].x = rDNF.left + (rDNF.Width() / 2) - 5;
		poTriangle[0].y = rDNF.top + (rDNF.Height() / 2) - 2;
		poTriangle[1].x = rDNF.left + (rDNF.Width() / 2) + 5;
		poTriangle[1].y = rDNF.top + (rDNF.Height() / 2) - 2;
		poTriangle[2].x = rDNF.left + (rDNF.Width() / 2);
		poTriangle[2].y = rDNF.bottom - (rDNF.Height() / 2) + 2;
		pDC->Polygon(poTriangle, 3);
	}
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();

	//// Now draw the Ellipsis (. . .)
	//// Ellipsis #1
	//CRect rDot;
	//rDot.SetRect(rDNF.left + 10, rDNF.top + 2, rDNF.left + 12, rDNF.top + 4);
	//pDC->Rectangle(&rDot);
	//// Ellipsis #2
	//rDot.SetRect(rDNF.left + 13, rDNF.top + 2, rDNF.left + 15, rDNF.top + 4);
	//pDC->Rectangle(&rDot);
	//// Ellipsis #3
	//rDot.SetRect(rDNF.left + 16, rDNF.top + 2, rDNF.left + 18, rDNF.top + 4);
	//pDC->Rectangle(&rDot);
}

void CCalendarCell::SetBkColor(COLORREF clrBk)
{
	clrBackGrnd = clrBk;

	// Change the background color of all CellDataItems!!
	POSITION pos = m_ItemList.GetHeadPosition();
	CCalendarCellItem* pItem = NULL;

	while (pos != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetNext(pos);
		pItem->SetBkColor(clrBk);
	}
}

// Return Do Not Fit Rectange Area
void CCalendarCell::GetDNFRect(CRect* pRect)
{
	*pRect = rDNF;
}

void CCalendarCell::SetDNFClicked(BOOL bClick)
{
	bDNFClicked = bClick;
}

BOOL CCalendarCell::GetDNFClick()
{
	return bDNFClicked;
}

void CCalendarCell::SetItemData(long nItem, DWORD dwData)
{
	CCalendarCellItem* pItem = NULL;
	POSITION posItem = m_ItemList.GetHeadPosition();

	while (posItem != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetNext(posItem);
		if (pItem->GetItemID() == nItem)
		{
			// We have the item!!
			pItem->SetItemData(dwData);
			break;
		}
	}
}

DWORD CCalendarCell::GetItemData(long nItem)
{
	CCalendarCellItem* pItem = NULL;
	POSITION posItem = m_ItemList.GetHeadPosition();

	while (posItem != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetNext(posItem);
		if (pItem->GetItemID() == nItem)
		{
			// We have the item!!
			return pItem->GetItemData();
		}
	}
	return NULL;
}

CCalendarCellItem* CCalendarCell::GetItem(long nItem)
{
	POSITION pos = m_ItemList.GetHeadPosition();
	CCalendarCellItem* pItem = NULL;

	while (pos != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetNext(pos);
		if (pItem->GetItemID() == nItem)
			return pItem;
	}
	return NULL;
}

// Delete an item from the list!!
BOOL CCalendarCell::DeleteItem(long nItem)
{
	POSITION pos = m_ItemList.GetHeadPosition();
	CCalendarCellItem* pItem = NULL;

	while (pos != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetAt(pos);
		if (pItem->GetItemID() == nItem)
		{
			// Remove item from list!!
			m_ItemList.RemoveAt(pos);
			// Actually delete pItem
			delete pItem;
			return TRUE;
		}
		m_ItemList.GetNext(pos);
	}
	return FALSE;
}

void CCalendarCell::DeleteAllItems()
{
	POSITION pos = m_ItemList.GetHeadPosition();
	CCalendarCellItem* pItem = NULL;

	while (pos != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetNext(pos);
		delete pItem;
		pItem = NULL;
	}
	// Delete all items!!
	m_ItemList.RemoveAll();
}

COLORREF CCalendarCell::GetBkColor()
{
	return clrBackGrnd;
}

void CCalendarCell::GetHdrRect(CRect* pRect)
{
	*pRect = rHdr;
}

void CCalendarCell::SetHdrRect(CRect* pRect)
{
	rHdr.CopyRect(pRect);
}

void CCalendarCell::SetItemColor(long nItem, COLORREF clrItem)
{
	CCalendarCellItem* pItem = NULL;
	POSITION posItem = m_ItemList.GetHeadPosition();

	while (posItem != NULL)
	{
		pItem = (CCalendarCellItem*)m_ItemList.GetNext(posItem);
		if (pItem->GetItemID() == nItem)
		{
			// We have the item!!
			pItem->SetBkColor(clrItem);
			break;
		}
	}
}

void CCalendarCell::SetImageList(CImageList* pImgList)
{
	pImageList = pImgList;
}
