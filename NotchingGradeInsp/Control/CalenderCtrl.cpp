// CalenderCtrl.cpp: 구현 파일
//

#include "pch.h"
#include "CalenderCtrl.h"
#include "locale.h"


// CCalenderCtrl
#define WEEKVIEWER_CLASSNAME    _T("CalenderCtrl")

IMPLEMENT_DYNAMIC(CCalenderCtrl, CWnd)


BEGIN_MESSAGE_MAP(CCustomListCtrl, CMFCListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CCustomListCtrl::OnNMDblclk)
END_MESSAGE_MAP()


void CCustomListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	 
	CMFCListCtrl::OnLButtonDblClk(nFlags, point);
}

void CCustomListCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_SelectedListIndex = pNMItemActivate->iItem;

	*pResult = 0;
}


COLORREF CCustomListCtrl::OnGetCellTextColor(int nRow, int nColum)
{
	if (!m_bColor)
	{
		return CMFCListCtrl::OnGetCellTextColor(nRow, nColum);
	}

	return(nRow % 2) == 0 ? RGB(128, 37, 0) : RGB(0, 0, 0);
}

COLORREF CCustomListCtrl::OnGetCellBkColor(int nRow, int nColum)
{
	if (!m_bColor)
	{
		return CMFCListCtrl::OnGetCellBkColor(nRow, nColum);
	}

	if (m_bMarkSortedColumn && nColum == m_iSortedColumn)
	{
		return(nRow % 2) == 0 ? RGB(233, 221, 229) : RGB(176, 218, 234);
	}

	return(nRow % 2) == 0 ? RGB(253, 241, 249) : RGB(196, 238, 254);
}

HFONT CCustomListCtrl::OnGetCellFont(int nRow, int nColum, DWORD /*dwData* = 0*/)
{
	if (!m_bModifyFont)
	{
		return NULL;
	}

	if (nColum == 2 && (nRow >= 4 && nRow <= 8))
	{
		return afxGlobalData.fontDefaultGUIBold;
	}

	return NULL;
}

int CCustomListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	CString strItem1 = GetItemText((int)(lParam1 < lParam2 ? lParam1 : lParam2), iColumn);
	CString strItem2 = GetItemText((int)(lParam1 < lParam2 ? lParam2 : lParam1), iColumn);

	if (iColumn == 0)
	{
		int nItem1 = _ttoi(strItem1);
		int nItem2 = _ttoi(strItem2);
		return(nItem1 < nItem2 ? -1 : 1);
	}
	else
	{
		int iSort = _tcsicmp(strItem1, strItem2);
		return(iSort);
	}
}

CCalenderCtrl::CCalenderCtrl()
{
	clrBackGrnd = RGB(255, 255, 213);
	clrOtherMonthBk = RGB(255, 244, 188);
	clrGradBot = RGB(249, 204, 95);	// Gradient color - bottom
	clrGradTop = RGB(236, 233, 216);
	clrGradLine = RGB(187, 85, 3);	// Gradient line color
	nFontSize = 100;
	strFontName = "TAHOMA";
	bMilTime = FALSE;	// NonMilitary Time
	bMonthView = FALSE;	// Do not draw Month view
	nUniqueItemID = 0;	// Unique Item ID Number
	dtMonth.SetDate(COleDateTime::GetCurrentTime().GetYear(),
		COleDateTime::GetCurrentTime().GetMonth(), 1);
	pImageList = NULL;
	m_pList = NULL;
	m_pListFont = NULL;

	// Create ALL Cell Objects for Week or Month Views
	CCalendarCell* pData = NULL;
	for (int x = 1; x <= WV_TOTALCELLS; x++)
	{
		pData = new CCalendarCell();
		pData->SetCellID(x);
		pData->SetMilitaryTime(bMilTime);
		pData->SetBkColor(clrBackGrnd);
		// Add node to Cell List
		m_CellList.AddTail(pData);
	}

	setlocale(LC_TIME, _T("") );

	RegisterWindowClass();
}

CCalenderCtrl::~CCalenderCtrl()
{
	POSITION pos = m_CellList.GetHeadPosition();
	CObject* pObj = NULL;
	while (pos != NULL)
	{
		pObj = m_CellList.GetNext(pos);
		if (pObj != NULL)
			delete pObj;
	}
	m_CellList.RemoveAll();
	
	if (m_pList != NULL) {
		delete m_pList;
		m_pList = NULL;
	}

	if (m_pListFont != NULL) {
		delete m_pListFont;
		m_pListFont = NULL;
	}
}


BEGIN_MESSAGE_MAP(CCalenderCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


BOOL CCalenderCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, WEEKVIEWER_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		//wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.hbrBackground = NULL;
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = WEEKVIEWER_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}
	return TRUE;
}


// CCalenderCtrl 메시지 처리기

BOOL CCalenderCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (wParam >= 25000 && wParam <= 25001) {
		const NMHDR& hdr = *(const NMHDR*)lParam;
		if(hdr.code == NM_DBLCLK ) {
			if (m_pList != NULL) {
				CString str;
				str.Format(_T("더블클릭 메세지"));
				AfxMessageBox(str);
				m_pList->ShowWindow(SW_HIDE);
			}
		}
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CCalenderCtrl::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWnd::PreSubclassWindow();
}


BOOL CCalenderCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rect;
	// Get client area to paint
	GetClientRect(&rect);
	// Fill with background color
	pDC->FillSolidRect(&rect, clrBackGrnd);
	
//	return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}


void CCalenderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.

	// Clear all cell rects
	CRect rArea;
	CCalendarCell* pData = NULL;
	for (int x = 1; x <= WV_TOTALCELLS; x++)
	{
		// Get current CellData
		pData = GetCellByID(x);
		if (pData)
		{
			rArea.SetRectEmpty();
			pData->SetCellRect(&rArea);	// reset cell area
		}
	}

	if (!bMonthView)
	{
		// Draw Week View
		PaintWeekView(&dc);
		// Paint Dates on Cell Headers
		DrawWeekCellHeaders(&dc);
	}
	else
	{
		// Draw Month View
		PaintMonthView(&dc);
		DrawMonthCellHeaders(&dc);
	}
	// Do not call CWnd::OnPaint() for painting messages

	CRect rectList;
	CPoint pos;
	GetCursorPos(&pos);
	rectList.left = pos.x;
	rectList.top = pos.y;
	rectList.right = rectList.left + 220;
	rectList.bottom = rectList.top + 500;
	ScreenToClient(rectList);
	rectList.OffsetRect(-70, -400);

	//if (m_pList == NULL) {
	//	m_pList = new CCustomListCtrl;
	//	m_pList->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect(10, 10, 250, 300), this, 25001);
	//	m_pList->SetOwner(this);
	//}
	////m_wndList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect(10, 10, 250, 300), this, 1);

	//if (m_pListFont == NULL) {
	//	m_pListFont = new CFont;
	//	m_pListFont->CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, 0, 1, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("MS PGothic"));
	//	//m_pListFont->CreatePointFont(85, _T("MS PGothic"));
	//	
	//	m_pList->SetFont(m_pListFont);
	//	m_pList->InsertColumn(0, _T("테스트"), LVCFMT_LEFT, 70);
	//	m_pList->InsertColumn(1, _T("테스트"), LVCFMT_LEFT, 70);
	//	m_pList->InsertColumn(2, _T("테스트"), LVCFMT_LEFT, 70);
	//	for (int i = 1; i < 50; i++)
	//	{
	//		CString str;
	//		str.Format(_T("%d"), i);
	//		m_pList->InsertItem(i, str);
	//		m_pList->SetItemData(i, i);

	//		for (int nColumn = 1; nColumn < 3; nColumn++)
	//		{
	//			str.Format(_T("Item(%d, %d)"), nColumn - 1, nColumn == 1 ? 99 - i : i + 50);
	//			m_pList->SetItemText(i, nColumn, str);
	//		}
	//		m_pList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//		//m_pList->EnableMarkSortedColumn();
	//	}
	//}
}


void CCalenderCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	POSITION pos = m_CellList.GetHeadPosition();
	CObject* pObj = NULL;
	while (pos != NULL)
	{
		pObj = m_CellList.GetNext(pos);
		if (pObj != NULL)
			delete pObj;
	}
	m_CellList.RemoveAll();
}

LRESULT CCalenderCtrl::SendMessageToParent(int nRow, int nCol, int nMessage) const
{
	if (!IsWindow(m_hWnd))
		return 0;

	NM_WVCELLDATA nmgv;

	// Fillin CellData Information
	nmgv.pCell = pCurrentCell;
	if (pCurrentCell != NULL)
	{
		if (pCurrentCell->pCellItem != NULL)
		{
			nmgv.nItem = pCurrentCell->pCellItem->GetItemID();
			nmgv.pItem = pCurrentCell->pCellItem;
		}
		else
		{
			nmgv.nItem = 0;
			nmgv.pItem = NULL;
		}
		nmgv.bDNFClicked = pCurrentCell->GetDNFClick();
		// Change message to reflect a double click - ONLY when DNF is TRUE
		if (pCurrentCell->GetDNFClick())
			nMessage = NM_DBLCLK;
	}
	else
	{
		// We do not have an item
		nmgv.pItem = NULL;
		nmgv.nItem = 0;
	}

	nmgv.iRow = nRow;
	nmgv.iColumn = nCol;
	nmgv.hdr.hwndFrom = m_hWnd;
	nmgv.hdr.idFrom = GetDlgCtrlID();
	nmgv.hdr.code = nMessage;

	CWnd* pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		return pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
	else
		return 0;
}

void CCalenderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	POSITION pos = m_CellList.GetHeadPosition();
	POSITION posItems = NULL;
	CCalendarCell* pCellData = NULL;
	CCalendarCellItem* pItem = NULL;
	CRect rect, rDNF;
	CString strBuf;
	CPoint ptPos = point;

#ifdef DEBUG
	strBuf.Format(_T("OnLButtonDown - Clicked point: %d, %d\n"), point.x, point.y);
	TRACE(strBuf);
#endif
	// Clear Highlight from any of the CellDataItems
	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		pCellData->SetHighlight(FALSE);
		posItems = pCellData->m_ItemList.GetHeadPosition();
		while (posItems != NULL)
		{
			pItem = (CCalendarCellItem*)pCellData->m_ItemList.GetNext(posItems);
			pItem->SetHightlight(FALSE);
		}
	}

	// Locate clicked cell
	pCurrentCell = NULL;	// No current cell
	pos = m_CellList.GetHeadPosition();
	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetAt(pos);
		pCellData->GetCellRect(&rect);

		// Mark which item is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Set Highlight Status
			pCurrentCell = pCellData;	// Make cell current
			pCellData->SetHighlight(TRUE);	// Highlight Cell Header

		//KANG 21.10.21 Add Start
		//	SetCurrentDate( pCurrentCell->GetCellDate());
			dtCurrentDate = pCurrentCell->GetCellDate();
		//KANG 21.10.21 Add End

			//Check if we are clicking on the Do Not Fit rectangle area
			pCellData->GetDNFRect(&rDNF);
			if (rDNF.PtInRect(point) == TRUE)
				pCellData->SetDNFClicked(TRUE);	// Set DNF Click status
			else
			{
				pCellData->SetDNFClicked(FALSE);	// Set DNF Click status
				// Check if we are clicking on a CellDataItem
				if (pCellData->HitItemTest(point) == TRUE)
				{
					pCellData->pCellItem->SetHightlight(TRUE);
				}
				// Send the click that we have clicked an item!!
				SendMessageToParent(point.x, point.y, NM_CLICK);
			}
			break;
		}
		else
		{
			// Remove highlight from current CellData. This is used when the
			// user is clicking inside a cell itself and not a CellDataItem. This will remove
			// the highlight
			pCellData->SetHighlight(FALSE);
		}
		m_CellList.GetNext(pos);
	}
	// Force repaint to update control
	InvalidateRect(NULL, TRUE);

	CWnd::OnLButtonDown(nFlags, point);
}


void CCalenderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonUp(nFlags, point);
}


void CCalenderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pCellData = NULL;
	CRect rect;
	CString strBuf;

#ifdef DEBUG
	strBuf.Format(_T("OnLButtonDblClk - Clicked point: %d, %d\n"), point.x, point.y);
	TRACE(strBuf);
#endif

	pos = m_CellList.GetHeadPosition();
	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		pCellData->GetCellRect(&rect);

		// Mark which cell is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Check if Item is clicked and make CellDataItem internally current
			pCellData->HitItemTest(point);
			break;
		}
		// Send Message to parent window
		SendMessageToParent(point.x, point.y, NM_DBLCLK);
	}
	// Force repaint to update control
	InvalidateRect(NULL, TRUE);

	CWnd::OnLButtonDblClk(nFlags, point);
}


void CCalenderCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	POSITION pos = m_CellList.GetHeadPosition();
	POSITION posItems = NULL;
	CCalendarCell* pCellData = NULL;
	CCalendarCellItem* pItem = NULL;
	CRect rect, rDNF;
	CString strBuf;
	CPoint ptPos = point;

#ifdef DEBUG
	strBuf.Format(_T("OnRButtonDown - Clicked point: %d, %d\n"), point.x, point.y);
	TRACE(strBuf);
#endif
	// Clear Highlight from any of the CellDataItems
	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		posItems = pCellData->m_ItemList.GetHeadPosition();
		while (posItems != NULL)
		{
			pItem = (CCalendarCellItem*)pCellData->m_ItemList.GetNext(posItems);
			pItem->SetHightlight(FALSE);
		}
	}

	// Locate clicked cell
	pCurrentCell = NULL;	// No current cell
	pos = m_CellList.GetHeadPosition();
	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		pCellData->GetCellRect(&rect);

		// Mark which item is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Set Highlight Status
			pCurrentCell = pCellData;	// Make cell current
			pCellData->SetHighlight(TRUE);	// Highlight Cell Header

			//Check if we are clicking on the Do Not Fit rectangle area
			pCellData->GetDNFRect(&rDNF);
			if (rDNF.PtInRect(point) == TRUE)
				pCellData->SetDNFClicked(TRUE);	// Set DNF Click status
			else
			{
				pCellData->SetDNFClicked(FALSE);	// Set DNF Click status
				// Check if we are clicking on a CellDataItem
				if (pCellData->HitItemTest(point) == TRUE)
					pCellData->pCellItem->SetHightlight(TRUE);
			}
		}
		else
		{
			// Remove highlight from current CellData. This is used when the
			// user is clicking inside a cell itself and not a CellDataItem. This will remove
			// the highlight
			pCellData->SetHighlight(FALSE);
		}
	}
	// Force repaint to update control
	InvalidateRect(NULL, TRUE);

	CWnd::OnRButtonDown(nFlags, point);
}


void CCalenderCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pCellData = NULL;
	CRect rect;
	CString strBuf;

#ifdef DEBUG
	strBuf.Format(_T("OnRButtonUp - Clicked point: %d, %d\n"), point.x, point.y);
	TRACE(strBuf);
#endif

	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		pCellData->GetCellRect(&rect);

		// Mark which item is to be highlighted
		if (rect.PtInRect(point) == TRUE)
		{
			// Send Message to parent window
			SendMessageToParent(point.x, point.y, NM_RCLICK);
			break;
		}
	}

	CWnd::OnRButtonUp(nFlags, point);
}


void CCalenderCtrl::DrawWeekCellHeaders(CPaintDC* pDC)
{
	CRect rect, rHdr, rArea;
	// Calculate height of Cell Header to be the width of the scroll bars
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);
	CBrush brHdrBrush, brOldBrush;
	CFont fFont, * pOldFont;
	CString strBuf;
	CString strToDay;
	CCalendarCell* pCellData = NULL;
	COLORREF clrHL = GetSysColor(COLOR_HIGHLIGHT);	// Highlight color
	COLORREF clrFace = RGB(250, 250, 180);//GetSysColor(COLOR_BTNFACE);	// Header color
	POSITION posItem = NULL;
	CCalendarCellItem* pCellItem = NULL;
	int nCellHeight = 0;
	COleDateTime dtDate = dtCurrentDate;

	// Find Monday
	while (dtDate.GetDayOfWeek() != 2)
	{
		dtDate -= 1;
	}

	// Initialize values
	fFont.CreatePointFont(nFontSize, strFontName, pDC);
	pOldFont = pDC->SelectObject(&fFont);
	// Remove time from date
//KANG 21.10.21 Modify Start
//	dtCurrentDate.SetDate(dtCurrentDate.GetYear(), dtCurrentDate.GetMonth(), dtCurrentDate.GetDay());
	COleDateTime dtToday = COleDateTime::GetCurrentTime();
	dtToday.SetDate(dtToday.GetYear(), dtToday.GetMonth(), dtToday.GetDay());
//KANG 21.10.21 Modify End
	// Make drawing text transparent
	pDC->SetBkMode(TRANSPARENT);

	// Get client area to paint
	GetClientRect(&rect);

	nCellHeight = rect.bottom / 3;
	// Build Week View Headers
	for (int x = 1; x <= 7; x++)
	{
		// Draw Header
		pCellData = GetCellByDate(dtDate);
		dtDate += 1;
		pCellData->GetCellRect(&rHdr);
		pDC->FillSolidRect(&rHdr, pCellData->GetBkColor());
		rHdr.left += 1;
		rHdr.top += 1;
		rHdr.bottom = rHdr.top + nHdrHeight;

	//KANG 21.20.21 Modify Start
	//	if (pCellData->GetCellDate() == dtCurrentDate)
		if (pCellData->GetCellDate() == dtToday)
	//KANG 21.20.21 Modify End
		{
			// Gradient Fill this header
			rHdr.left -= 1;
			rHdr.right -= 1;
			FillGradientRect(pDC, &rHdr);
			// Calculate Rectangle - for highlighted cell
			CRect rText;
			rText.CopyRect(&rHdr);
			strBuf = pCellData->GetCellDate().Format(_T("%B %d [%A]"));
			strToDay = _T("-Today-   ");
			strBuf = strToDay + strBuf;
			pDC->DrawText(strBuf, &rText, DT_CALCRECT);
			if (pCellData->GetHighlight())
			{
				// Highlight Header
				rText.left = rHdr.left + (rHdr.right - rText.right);
				rText.right = rHdr.right;
				rText.bottom = rHdr.bottom;
				pDC->FillSolidRect(&rText, clrHL);
				pDC->SetTextColor(RGB(255, 255, 255));
			}
		}
		else
		{
			strToDay = _T("");
			// Header not in Gradient fill
			if (pCellData->GetHighlight())
			{
				// Normal highlight of header
				pDC->FillSolidRect(&rHdr, clrHL);	// Highlight Header
				pDC->SetTextColor(RGB(255, 255, 255));
			}
			else
			{
				// Display as a regular non-modified header
				pDC->FillSolidRect(&rHdr, clrFace);
			}
		}
		// Draw Date within Header
		strBuf = pCellData->GetCellDate().Format(_T("%B %d [%A]"));
		strBuf = strToDay + strBuf;
		pDC->DrawText(strBuf, &rHdr, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		pDC->SetTextColor(RGB(0, 0, 0));
		// DrawItems for this cell
		pCellData->DrawItems(pDC, bMonthView);
	}

	pDC->SelectObject(pOldFont);
}

void CCalenderCtrl::SetBkColor(COLORREF clrBk)
{
	clrBackGrnd = clrBk;
	// Update all cells
	POSITION pos = m_CellList.GetHeadPosition();
	POSITION posItem = NULL;
	CCalendarCell* pData = NULL;
	CCalendarCellItem* pItem = NULL;
	while (pos != NULL)
	{
		pData = (CCalendarCell*)m_CellList.GetNext(pos);
		pData->SetBkColor(clrBk);
		// Update background color for all the CellData Items
		posItem = pData->m_ItemList.GetHeadPosition();
		while (posItem != NULL)
		{
			pItem = (CCalendarCellItem*)pData->m_ItemList.GetNext(posItem);
			pItem->SetBkColor(clrBk);
		}
	}
	InvalidateRect(NULL, TRUE);
}

void CCalenderCtrl::FillGradientRect(CDC* pDC, CRect* pRect)
{
	// Gradient Fill Header
	TRIVERTEX        vert[2];
	GRADIENT_RECT    gRect;

	vert[0].x = pRect->left;
	vert[0].y = pRect->top;
	vert[0].Alpha = 0x0000;
	vert[1].x = pRect->right;
	vert[1].y = pRect->bottom;
	vert[1].Alpha = 0x0000;

	if (!bMonthView)
	{
		// Week View Gradient Colors
		vert[0].Red = 0xEC00;
		vert[0].Green = 0xE900;
		vert[0].Blue = 0xD800;

		vert[1].Red = 0xF900;
		vert[1].Green = 0xCC00;
		vert[1].Blue = 0x5F00;
	}
	else
	{
		// Month View Gradient Colors
		vert[0].Red = 0xFF00;
		vert[0].Green = 0xFF00;
		vert[0].Blue = 0xD500;

		vert[1].Red = 0xF900;
		vert[1].Green = 0xCC00;
		vert[1].Blue = 0x5F00;
	}

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;
	GradientFill(pDC->m_hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

	// Draw Gradient deviding line
	CPen pLine(PS_SOLID, 1, clrGradLine);

	pDC->SelectObject(pLine);
	pDC->MoveTo(pRect->left, pRect->bottom);
	pDC->LineTo(pRect->right, pRect->bottom);
	pLine.DeleteObject();
	CPen pPen(PS_SOLID, 1, RGB(0, 0, 0));
	pDC->SelectObject(pPen);
}

// Get CellData node by ID
CCalendarCell* CCalenderCtrl::GetCellByID(int nID)
{
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pData = NULL;

	while (pos != NULL)
	{
		pData = (CCalendarCell*)m_CellList.GetNext(pos);
		if (pData->GetCellID() == nID)
			break;
	}
	return pData;
}

// Get CellData by Date value
CCalendarCell* CCalenderCtrl::GetCellByDate(COleDateTime dtDate)
{
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pData = NULL;
	COleDateTime dtCellDate;

	//Remove time from dtDate
	dtDate.SetDate(dtDate.GetYear(), dtDate.GetMonth(), dtDate.GetDay());

	while (pos != NULL)
	{
		pData = (CCalendarCell*)m_CellList.GetNext(pos);
		dtCellDate = pData->GetCellDate();
		if (dtCellDate == dtDate)
			break;
	}
	return pData;
}


long CCalenderCtrl::InsertItem(COleDateTime dtStart, COleDateTime dtEnd, CString strLine /*=""*/, BOOL bDispDate /*= TRUE*/, int nImage/*=-1*/)
{
	long nItemID = -1;

	CCalendarCell* pData = GetCellByDate(dtStart);	// Select Data Cell
	if (pData != NULL)
	{
		// We have a current cell to work with
		pData->SetImageList(pImageList);
		pData->InsertCellItem(dtStart, dtEnd, strLine, bDispDate, nImage);
		// Set unique ID Number
		nItemID = ++nUniqueItemID;
		pData->pCellItem->SetItemID(nItemID);
	}

	return nItemID;
}

void CCalenderCtrl::SetCurrentDate(COleDateTime dtDate, BOOL bMilitaryTime /*= FALSE*/,	int nView /*= WV_WEEKVIEW*/)
{
	bMilTime = bMilitaryTime;	// Set Military Time display
	bMonthView = nView == WV_WEEKVIEW ? FALSE : TRUE;	// Set drawing mode - Month or Week

	dtDate.SetDate(dtDate.GetYear(), dtDate.GetMonth(), dtDate.GetDay());	// Remove time from date
	dtCurrentDate = dtDate;

	// Initialize Cell Objects
	CCalendarCell* pData = NULL;
	CRect rArea;
	// Need to place correct date within each cell. Set up cells for Month View.
	// Calculate 1st day of month position
	dtMonth.SetDate(dtDate.GetYear(), dtDate.GetMonth(), 1);
	dtDate = dtMonth;
	// Find Sunday from first day of month
	while (dtDate.GetDayOfWeek() != 1)
		dtDate -= 1;

	for (int x = 1; x <= WV_TOTALCELLS; x++)
	{
		// Get current CellData
		pData = GetCellByID(x);
		if (pData)
		{
			// Set cell values
			pData->SetMilitaryTime(bMilTime);
			pData->SetCellDate(dtDate);

			// Place correct background color in cell
			if (dtDate.GetMonth() == dtMonth.GetMonth())
				pData->SetBkColor(clrBackGrnd);
			else
				pData->SetBkColor(clrOtherMonthBk);

			// Increment day by 1
			dtDate += 1;
		}
	} // for (x = 1; x <= WV_TOTALCELLS; x++)

	pCurrentCell = NULL;	// No current DataCellItem
	// Force a repaint of the control
	InvalidateRect(NULL, TRUE);
}

void CCalenderCtrl::SetFont(CString strFName, int nSize)
{
	nFontSize = nSize;
	strFontName = strFName;
}

// Returns the current date within the displayed week
COleDateTime CCalenderCtrl::GetCurrentDate()
{
	return dtCurrentDate;
}

void CCalenderCtrl::SetItemData(long nItem, DWORD dwData)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		// View all CelDataItems for this CellData
		pCellData->SetItemData(nItem, dwData);
	}
}

DWORD CCalenderCtrl::GetItemData(long nItem)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		// View all CelDataItems for this CellData
		return pCellData->GetItemData(nItem);
	}
	return NULL;
}

BOOL CCalenderCtrl::DeleteItem(long nItem)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		pCellData->DeleteItem(nItem);
	}
	return FALSE;
}

void CCalenderCtrl::DeleteAllItems()
{
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pData = NULL;

	while (pos != NULL)
	{
		pData = (CCalendarCell*)m_CellList.GetNext(pos);
		pData->DeleteAllItems();
	}
	InvalidateRect(NULL, TRUE);
}

// Retrieve the current date range for the control
void CCalenderCtrl::GetDateRange(COleDateTime* pStartDate, COleDateTime* pEndDate)
{
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pStartData = NULL;
	CCalendarCell* pEndData = NULL;

	if (bMonthView)
	{
		// Locate correct month date range
		pStartData = GetCellByID(1);
		pEndData = GetCellByID(WV_TOTALCELLS);
	}
	else
	{
		// Locate correct week date range
		COleDateTime dtDate = dtCurrentDate;

		// Get first date
		while (dtDate.GetDayOfWeek() != 1)
			dtDate -= 1;
		pStartData = GetCellByDate(dtDate);
		// Get last date
		dtDate = dtCurrentDate;
		while (dtDate.GetDayOfWeek() != 7)
			dtDate += 1;
		pEndData = GetCellByDate(dtDate);
	}

	// Return date range
	*pStartDate = pStartData->GetCellDate();
	*pEndDate = pEndData->GetCellDate();
}

// Paint Week Date View
void CCalenderCtrl::PaintWeekView(CPaintDC* pDC)
{
	CRect rect, rCell;
	COleDateTime dtDate = dtCurrentDate;
	COleDateTime dtTemp = dtDate;

	// Find Monday
	while (dtDate.GetDayOfWeek() != 2)
		dtDate -= 1;

	// Get client area to paint
	GetClientRect(&rect);
	rCell.SetRectEmpty();	// Initialize Cell Rect

	// Drawing the WeekView Grid
	// Draw vertical dividing lines
	pDC->MoveTo(rect.right / 2, rect.top);
	pDC->LineTo(rect.right / 2, rect.bottom);

	// Make the lines dividing the rows in both columns
	int nCellHeight = rect.bottom / 3;
	int nLinePos = 0;
	CCalendarCell* pCellData = NULL;

	// Line 1 across - make top two cells (2, 5)
	nLinePos = rect.top + nCellHeight;
	pDC->MoveTo(0, nLinePos);
	pDC->LineTo(rect.right, nLinePos);

	// Get Cell Node and store rectangle area
	// Monday
	//pCellData = GetCellByID(1);
	dtTemp = dtDate;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = 0;
	rCell.top = 0;
	rCell.right = rect.right / 2;
	rCell.bottom = rect.top + nCellHeight;
	pCellData->SetCellRect(&rCell);

	// Thursday
	dtTemp = dtDate;
	dtTemp += 3;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = 0;
	rCell.right = rect.right;
	rCell.bottom = rect.top + nCellHeight;
	pCellData->SetCellRect(&rCell);

	// Line 2 across - Makes middle and bottom cells (3, 6) - (4)
	nLinePos += nCellHeight;
	pDC->MoveTo(0, nLinePos);
	pDC->LineTo(rect.right, nLinePos);

	// Tuesday
	dtTemp = dtDate;
	dtTemp += 1;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = 0;
	rCell.top = nCellHeight + 1;
	rCell.right = rect.right / 2;
	rCell.bottom = (nCellHeight * 2);
	pCellData->SetCellRect(&rCell);

	// Friday
	dtTemp = dtDate;
	dtTemp += 4;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = nCellHeight + 1;
	rCell.right = rect.right;
	rCell.bottom = nCellHeight * 2;
	pCellData->SetCellRect(&rCell);

	// Wednesday
	dtTemp = dtDate;
	dtTemp += 2;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = 0;
	rCell.top = rCell.bottom + 1;
	rCell.right = rect.right / 2;
	rCell.bottom = rect.bottom;
	pCellData->SetCellRect(&rCell);

	// Line 3 for the right column - for Sat/Sun - (7, 1)
	nLinePos += (nCellHeight / 2);
	pDC->MoveTo(rect.right / 2, nLinePos);
	pDC->LineTo(rect.right, nLinePos);

	// Saturday
	dtTemp = dtDate;
	dtTemp += 5;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = (nCellHeight * 2) + 1;
	rCell.right = rect.right;
	rCell.bottom = nLinePos;
	pCellData->SetCellRect(&rCell);

	// Sunday
	dtTemp = dtDate;
	dtTemp += 6;
	pCellData = GetCellByDate(dtTemp);
	rCell.left = (rect.right / 2) + 1;
	rCell.top = rCell.bottom + 1;
	rCell.right = rect.right;
	rCell.bottom = rect.bottom;
	pCellData->SetCellRect(&rCell);
}

// Paint Month Date View
void CCalenderCtrl::PaintMonthView(CPaintDC* pDC)
{
	// Now draw the grid of cells representing the current month/year. We begin
	// at the bottom of the current Header
	CRect rArea(0, 0, 0, 0), rCArea(0, 0, 0, 0);
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);

	// Draw Header
	DrawMonthHeader(pDC);

	// Calculate Real Grid Area - without the day of week header
	GetClientRect(&rArea);
	GetClientRect(&rCArea);
	rCArea.top += nHdrHeight + 1;
	rArea.SetRect(0, nHdrHeight, rCArea.right, rCArea.bottom);

	int nCol = 0, nRow = 0;
	int nInterval = (int)(rCArea.right / 7);	// Width of each cell
	int nCellHeight = (int)(rCArea.bottom / 6) - 2; // Height of each cell
	CRect rCell(0, 0, 0, 0), rHdr(0, 0, 0, 0), rClear(0, 0, 0, 0);
	int nX = 0, nY = 0;
	CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush brBkGrnd(clrBackGrnd);
	CCalendarCell* pData = NULL;
	int nCellCount = 1;

	// Set Pen and Background Brush colors
	pDC->SelectObject(penBlack);
	pDC->SelectObject(brBkGrnd);

	// Draw Cell Rectangles for each Cell
	rCell.SetRectEmpty();
	// Cell 1 - upper left corner
	rCell.left = 0;
	rCell.top = nHdrHeight;
	rCell.right = nInterval + 1;
	rCell.bottom = rCell.top + nCellHeight;

	// For each row and column
	for (nRow = 1; nRow <= 6; nRow++)
	{
		// We go up to 6 because the sixth cell area will hold
		// Saturday and Sunday Date Cells. Otherwise calendar
		// will be off by one day for each week.
		for (nCol = 1; nCol <= 7; nCol++)
		{
			pData = GetCellByID(nCellCount);
			// Header Cell definition
			rHdr = rCell;
			rHdr.left += 1;
			rHdr.top += 1;
			rHdr.right -= 1;
			rHdr.bottom = rHdr.top + nHdrHeight;
			pData->SetHdrRect(&rHdr);

			brBkGrnd.DeleteObject(); // Delete existing brush
			brBkGrnd.CreateSolidBrush(pData->GetBkColor()); // Create new brush
			pDC->SelectObject(brBkGrnd); // Select the current brush

			pDC->FillSolidRect(&rCell, pData->GetBkColor());
			pData = GetCellByID(nCellCount);
			pData->SetCellRect(&rCell);

			nCellCount++;	// Increment Cell Counter to be in next cell
			// Change Cell Rectangle Area
			rCell.left = rCell.right + 1;
			rCell.right = rCell.left + nInterval;
		}

		// Move to next month calendar row
		rCell.left = 0;
		rCell.top = rCell.bottom;
		rCell.right = nInterval + 1;
		rCell.bottom = rCell.top + nCellHeight;
	}

	//-----------------------------------------------------
	// Draw Calendar Horizontal Grid
	GetClientRect(&rArea);
	rCell.left = 0;
	rCell.top = nCellHeight + nHdrHeight;
	rCell.right = rArea.right;
	rCell.bottom = rCell.top + nInterval;
	for (nRow = 1; nRow <= 6; nRow++)
	{
		pDC->MoveTo(rCell.left, rCell.top);
		pDC->LineTo(rCell.right, rCell.top);
		rCell.top += nCellHeight;
	}
	// Draw Calendar Vertical Lines
	GetClientRect(&rArea);
	rCell.left = nInterval;
	rCell.top = nHdrHeight;
	rCell.right = rArea.right;
	rCell.bottom = rArea.bottom;
	for (nCol = 1; nCol <= 6; nCol++)
	{
		pDC->MoveTo(rCell.left, rCell.top);
		pDC->LineTo(rCell.left, rCell.bottom);
		rCell.left += nInterval + 1;
	}

	penBlack.DeleteObject();
	brBkGrnd.DeleteObject();
}

// Draw Days of the Week for the Month View
void CCalenderCtrl::DrawMonthHeader(CPaintDC* pDC)
{
	// Area to paint
	CRect rArea(0, 0, 0, 0);
	this->GetClientRect(&rArea);

	CRect rHdrArea, rHdr;
	CRect rHdrText(0, 0, 0, 0);
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);
	int nInterval = (int)(rArea.right / 7);
	CPen penLLine(PS_SOLID, 1, RGB(172, 168, 153));
	CPen penRLine(PS_SOLID, 1, RGB(255, 255, 255));
	CFont fFont, * pOldFont;
//KANG 21.10.21 Modify Start
//	COleDateTime dtDate = dtCurrentDate;
	COleDateTime dtDate = COleDateTime::GetCurrentTime();
//KANG 21.10.21 Modify End
	CString strText = "";

	// Initialize variables
	rHdrArea.left = 0;
	rHdrArea.top = 0;
	rHdrArea.right = rArea.right;
	rHdrArea.bottom = nHdrHeight;

	// Fillin background color
	pDC->FillSolidRect(&rHdrArea, RGB(236, 233, 216));

	// Monday Header
	rHdr.left = rHdrArea.left;
	rHdr.top = rHdrArea.top;
	rHdr.right = rHdrArea.right;
	rHdr.bottom = nHdrHeight;
	pDC->Draw3dRect(rHdrArea, RGB(255, 255, 255), RGB(172, 168, 153));

	// Draw column separator bars
	rHdr.top = 1;
	rHdr.bottom = nHdrHeight - 1;

	// Make sure we start at Sunday
	while (dtDate.GetDayOfWeek() != 1)
		dtDate -= 1;

	// Select current font
	fFont.CreatePointFont(nFontSize, strFontName, pDC);
	pOldFont = pDC->SelectObject(&fFont);

	// Header Rect Area
	rHdrArea.left = 2;
	rHdrArea.top = 2;
	rHdrArea.right = nInterval - 1;
	rHdrArea.bottom = nHdrHeight - 1;

	// Make drawing text transparent
	pDC->SetBkMode(TRANSPARENT);

	for (int x = 1; x <= 7; x++)
	{
		// Draw Day of week text
		pDC->DrawText(dtDate.Format("%A"), &rHdrText, DT_SINGLELINE | DT_CALCRECT);
		rHdrText.left = rHdrArea.left;
		pDC->DrawText(dtDate.Format("%A"), &rHdrArea, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS);

		// Increment day of week
		dtDate += 1;
		// Recalculate Header rect area
		rHdrArea.left = rHdrArea.right + 1;
		rHdrArea.right = rHdrArea.left + nInterval;

		rHdr.left += nInterval;
		rHdr.right = rHdr.left + 2;

		// Select Gray Pen and draw line
		pDC->SelectObject(penLLine);
		pDC->MoveTo(rHdr.left, rHdr.top + 1);
		pDC->LineTo(rHdr.left, rHdr.bottom - 1);

		// Select White Pen and draw line
		pDC->SelectObject(penRLine);
		rHdr.left += 1;
		rHdr.right += 1;
		pDC->MoveTo(rHdr.left, rHdr.top + 1);
		pDC->LineTo(rHdr.left, rHdr.bottom - 1);
		// Move to next column separator
		rHdr.right += nInterval;
	}
	pDC->SelectObject(pOldFont);
}

void CCalenderCtrl::DrawMonthCellHeaders(CPaintDC* pDC)
{
	CRect rect, rHdr, rArea;
	// Calculate height of Cell Header to be the width of the scroll bars
	int nHdrHeight = GetSystemMetrics(SM_CXVSCROLL);
	CBrush brHdrBrush, brOldBrush;
	CFont fFont, * pOldFont;
	CString strBuf;
	COLORREF clrHL = GetSysColor(COLOR_HIGHLIGHT);	// Highlight color
	COLORREF clrFace = GetSysColor(COLOR_BTNFACE);	// Header color
	POSITION posItem = NULL;
	CCalendarCellItem* pCellItem = NULL;
	int nCellHeight = 0, nItem = 1, nCellCount = 1;
	CCalendarCell* pCell = NULL;

	// Initialize values
	fFont.CreatePointFont(nFontSize, strFontName, pDC);
	pOldFont = pDC->SelectObject(&fFont);
	// Remove time from date
//KANG 21.10.21 Modify Start
//	dtCurrentDate.SetDate(dtCurrentDate.GetYear(), dtCurrentDate.GetMonth(),dtCurrentDate.GetDay());
	COleDateTime dtToday = COleDateTime::GetCurrentTime();
	dtToday.SetDate(dtToday.GetYear(), dtToday.GetMonth(), dtToday.GetDay());
//KANG 21.10.21 Modify End
	// Make drawing text transparent
	pDC->SetBkMode(TRANSPARENT);

	// Get client area to paint
	GetClientRect(&rect);

	for (nItem = 1; nItem <= WV_TOTALCELLS; nItem++)
	{
		CString strText;
		CRect rHdr;

		// Get Cell Data
		pCell = GetCellByID(nItem);

		// Get Cell Header Rect Area
		pCell->GetHdrRect(&rHdr);
	//KANG 21.10.21 Modify Start
	//	if (pCell->GetCellDate() == dtCurrentDate)
		if (pCell->GetCellDate() == dtToday)
	//KANG 21.10.21 Modify End
		{
			// This is the current date within the Calendar
			// Gradient Fill this header
			rHdr.left -= 1;
			rHdr.right -= 1;
			FillGradientRect(pDC, &rHdr);

			// Calculate Rectangle - for highlighted cell
			CRect rText;
			rText.CopyRect(&rHdr);
			// Check if we highlight Day or Month and Day
			if (pCell->GetCellDate().GetDay() == 1)
				strBuf = pCell->GetCellDate().Format("%B, %d ");
			else
				strBuf = pCell->GetCellDate().Format("%d ");

			// Draw text on text area
			pDC->DrawText(strBuf, &rText, DT_CALCRECT);

			if (pCell->GetHighlight())
			{
				// Highlight Header
				rText.left = rHdr.left + (rHdr.right - rText.right);
				rText.right = rHdr.right;
				rText.bottom = rHdr.bottom;
				pDC->FillSolidRect(&rText, clrHL);
				pDC->SetTextColor(RGB(255, 255, 255));
			}
		}
		else
		{
			// Check if we need to highlight cell as well
			if (pCell->GetHighlight())
			{
				// Normal highlight of header
				rHdr.left -= 1;
				rHdr.top += 1;
				rHdr.right -= 1;
				rHdr.bottom += 1;
				pDC->FillSolidRect(&rHdr, clrHL);	// Highlight Header
				pDC->SetTextColor(RGB(255, 255, 255));
			}
		}

		// Draw date within header
		if (nCellCount == 1 || pCell->GetCellDate().GetDay() == 1)
		{
			// First cell or first of the month in first cell
			strText.Format(_T("%s %d "), pCell->GetCellDate().Format("%B"), pCell->GetCellDate().GetDay());
		}
		else
		{
			// First of the month within the calendar
			if (pCell->GetCellDate().GetDay() == 1)
				strText.Format(_T("%s %d "), pCell->GetCellDate().Format("%B"), pCell->GetCellDate().GetDay());
			else
				strText.Format(_T("%d "), pCell->GetCellDate().GetDay());	// Display normal day number
		}
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->DrawText(strText, &rHdr, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);

		// DrawItems for this cell
		pCell->DrawItems(pDC, bMonthView);

		// Increment to next cell
		nCellCount++;
	}
	// Replace current Font with original Font
	pDC->SelectObject(pOldFont);
	fFont.DeleteObject();
}

void CCalenderCtrl::SetView(int nV, BOOL bRedraw/*=TRUE*/)
{
	switch (nV)
	{
	case WV_WEEKVIEW:
		bMonthView = FALSE;
		break;

	case WV_MONTHVIEW:
		bMonthView = TRUE;
		break;
	}

	if (bRedraw)
		InvalidateRect(NULL, TRUE);
}

int CCalenderCtrl::GetMonth()
{
	return dtCurrentDate.GetMonth();
}

int CCalenderCtrl::GetYear()
{
	return dtCurrentDate.GetYear();
}

// Set Item Color
void CCalenderCtrl::SetItemColor(long nItem, COLORREF clrItem)
{
	// Locate the CellDataItem identified with nItem
	POSITION pos = m_CellList.GetHeadPosition();
	CCalendarCell* pCellData = NULL;

	while (pos != NULL)
	{
		pCellData = (CCalendarCell*)m_CellList.GetNext(pos);
		// View all CelDataItems for this CellData
		pCellData->SetItemColor(nItem, clrItem);
	}
}

void CCalenderCtrl::SetImageList(CImageList* pImgList)
{
	pImageList = pImgList;
}

int CCalenderCtrl::GetView()
{
	if (bMonthView == TRUE)
		return WV_MONTHVIEW;
	else
		return WV_WEEKVIEW;
}

int CCalenderCtrl::GetDay()
{
	return dtCurrentDate.GetDay();
}

void CCalenderCtrl::SetMilitaryTime(BOOL bMil)
{
	bMilTime = bMil;
}

BOOL CCalenderCtrl::IsMilitaryTime()
{
	return bMilTime;
}

void CCalenderCtrl::RedrawControl()
{
	InvalidateRect(NULL, TRUE);
}

void CCalenderCtrl::ClearCurrentCell()
{
	CCalendarCell* pData = NULL;

	for (int x = 1; x <= WV_TOTALCELLS; x++)
	{
		pData = GetCellByID(x);
		pData->SetHighlight(FALSE);
	}
	pCurrentCell = NULL ;

	InvalidateRect(NULL, TRUE);
}

void CCalenderCtrl::SetSelectCell( COleDateTime dtDate )
{
	CCalendarCell* pData = GetCellByDate(dtDate);
	
	ClearCurrentCell();
	if (pData != NULL)
	{
		pCurrentCell = pData;
		pData->SetHighlight(TRUE);
	}
}

int CCalenderCtrl::GetCellID( COleDateTime dtDate )
{
	CCalendarCell* pData = GetCellByDate(dtDate);
	return ( pData->GetCellID() );
}

COleDateTime CCalenderCtrl::GetDateByID(int nID)
{
	CCalendarCell* pData = NULL;
	pData = GetCellByID( nID );
	return ( pData->GetCellDate() );
}

int CCalenderCtrl::GetTotalCellNum()
{
	return (WV_TOTALCELLS);
}