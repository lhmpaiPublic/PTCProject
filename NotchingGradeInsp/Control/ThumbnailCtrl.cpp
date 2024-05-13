// ThumbnailCtrl.cpp: 구현 파일
//

#include "pch.h"
#include "ThumbnailCtrl.h"


// CExpandViewImage

IMPLEMENT_DYNAMIC(CExpandViewImage, CStatic)

CExpandViewImage::CExpandViewImage(LPCTSTR lpszImageFileName /*= _T("")*/)
{
	m_strImageFileName = lpszImageFileName;
//KANG 22.07.08 Add Start
	m_bLClickClose = TRUE;
	m_bRClickClose = TRUE;
//KANG 22.07.08 Add End
}

CExpandViewImage::~CExpandViewImage()
{

}

BEGIN_MESSAGE_MAP(CExpandViewImage, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()	//KANG 22.07.08 Add
END_MESSAGE_MAP()

// CExpandViewImage 메시지 처리기

void CExpandViewImage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.

	CRect imgRect;
	GetClientRect(&imgRect);

	Graphics	graphics(dc.m_hDC);
	WCHAR		wchar[512];
	float contrast = 0.0;
	float scale = (100.0f + contrast) / 100.0f;
	scale *= scale;
	float append = 0.5f * (1.0f - scale);
	ColorMatrix cm =
	{
	scale,0.0f,0.0f,0.0f,0.0f,
	0.0f,scale,0.0f,0.0f,0.0f,
	0.0f,0.0f,scale,0.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f,0.0f,
	append,append,append,0.0f,1.0f
	};
	ImageAttributes ia;
	ia.SetColorMatrix(&cm);

	if (m_strImageFileName.GetLength() > 0) {
		memset(wchar, 0, sizeof(WCHAR) * 512);
		size_t ReturnValue;
		mbstowcs_s(&ReturnValue, wchar, sizeof(wchar) / sizeof(WCHAR), m_strImageFileName.GetBuffer(0), _TRUNCATE);

		Image img(wchar);

		LONG imgWidth = img.GetWidth();
		LONG imgHeight = img.GetHeight();
		LONG rectWidth = imgRect.Width();
		LONG rectHeight = imgRect.Height();
		LONG drawWidth;
		LONG drawHeight;
		double rateW = 1.0;
		double rateH = 1.0;

		rateW = (double)rectWidth / (double)imgWidth;
		rateH = (double)rectHeight / (double)imgHeight;

		if (rateW < rateH) {
			drawWidth = (LONG)(imgWidth * rateW);
			drawHeight = (LONG)(imgHeight * rateW);
		}
		else if (rateW > rateH) {
			drawWidth = (LONG)(imgWidth * rateH);
			drawHeight = (LONG)(imgHeight * rateH);
		}
		else {
			drawWidth = (LONG)(imgWidth * rateW) - 20;
			drawHeight = (LONG)(imgHeight * rateH) - 20;
		}

		DWORD color = RGB(0, 0, 0);
		SolidBrush brush(Color((BYTE)color, (BYTE)(color >> 8), (BYTE)(color >> 16)));
		graphics.FillRectangle(&brush, imgRect.left + imgRect.Width() / 2 - rectWidth / 2 - 1, imgRect.top + imgRect.Height() / 2 - rectHeight / 2 - 1, rectWidth + 1, rectHeight + 1);
		graphics.DrawImage(&img, Rect(imgRect.left + imgRect.Width() / 2 - drawWidth / 2, imgRect.top + imgRect.Height() / 2 - drawHeight / 2, drawWidth, drawHeight), 0, 0, img.GetWidth(), img.GetHeight(), UnitPixel, &ia);

		Pen fpen(Color(0, 0, 0));
		DWORD clrFrm = RGB(0, 0, 0);
		fpen.SetColor(Color((BYTE)clrFrm, (BYTE)(clrFrm >> 8), (BYTE)(clrFrm >> 16)));
		fpen.SetWidth(REAL(2.0));
		graphics.DrawRectangle(&fpen, imgRect.left + imgRect.Width() / 2 - rectWidth / 2 - 1, imgRect.top + imgRect.Height() / 2 - rectHeight / 2 - 1, rectWidth + 1, rectHeight + 1);
	}
}

void CExpandViewImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//KANG 22.07.08 Modify Start
//	this->DestroyWindow();
	if (m_bLClickClose == TRUE) {
		this->DestroyWindow();
	}
//KANG 22.07.08 Modify End

	CStatic::OnLButtonDown(nFlags, point);
}

void CExpandViewImage::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//KANG 22.07.08 Modify Start
//	this->DestroyWindow();
	if (m_bRClickClose == TRUE) {
		this->DestroyWindow();
	}
//KANG 22.07.08 Modify End

	CStatic::OnRButtonDown(nFlags, point);
}

int CExpandViewImage::SetImageFile(LPCTSTR lpszImageFileName)
{
	// TODO: 여기에 구현 코드 추가.
	m_strImageFileName = lpszImageFileName;
	Invalidate();
	return 0;
}

//KANG 22.07.08 Add Start
void CExpandViewImage::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	NMHDR nmhdr;
	nmhdr.code = NM_DBLCLK;
	nmhdr.idFrom = GetDlgCtrlID();
	nmhdr.hwndFrom = m_hWnd;

	CWnd* pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd)) {
		pOwner->SendMessage(WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);
	}

	CStatic::OnLButtonDblClk(nFlags, point);
}

void CExpandViewImage::SetLClickClose(BOOL bSet)
{
	m_bLClickClose = bSet;
}

void CExpandViewImage::SetRClickClose(BOOL bSet)
{
	m_bRClickClose = bSet;
}
//KANG 22.07.08 Add End


// CThumbnailCtrl

#define THUMBNAIL_CLASSNAME		_T("ThumbnailCtrl")
#define	IMAGE_SPACE_LEN_W		10
#define	IMAGE_SPACE_LEN_H		20
#define	BTN_H					10 
#define	INTR_W					30

IMPLEMENT_DYNAMIC(CThumbnailCtrl, CWnd)

CThumbnailCtrl::CThumbnailCtrl()
{
	RegisterWindowClass();
	m_clrBackGrnd = RGB(255, 255, 213);
	m_clrImgFrame = RGB(128, 128, 255);
	m_nImgFrameWidth = 2;
	m_nThumCols = 5;
	m_nThumRows = 3;
	m_DispSize.cx = 0;
	m_DispSize.cy = 0;
	m_pExpandView = NULL;
	m_bReDraw = TRUE;
	m_nSelectIndex = -1;
	m_strContextMenu[0] = _T("메뉴 1");
	m_strContextMenu[1] = _T("메뉴 2");
	m_strContextMenu[2] = _T("메뉴 3");
	m_strContextMenu[3] = _T("메뉴 4");
	m_strContextMenu[4] = _T("메뉴 5");
	m_nContextMenuNum = 0;
	m_bDispPopupImage = FALSE; // 22.06.15 Ahn Add
	m_vecThumbData.clear(); // 22.06.15 Ahn Add
}

CThumbnailCtrl::~CThumbnailCtrl()
{
	m_vecThumbData.clear(); // 22.06.15 Ahn Add

}

BEGIN_MESSAGE_MAP(CThumbnailCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDR_MENU_USER1, &CThumbnailCtrl::OnMenuUser1)
	ON_COMMAND(IDR_MENU_USER2, &CThumbnailCtrl::OnMenuUser2)
	ON_COMMAND(IDR_MENU_USER3, &CThumbnailCtrl::OnMenuUser3)
	ON_COMMAND(IDR_MENU_USER4, &CThumbnailCtrl::OnMenuUser4)
	ON_COMMAND(IDR_MENU_USER5, &CThumbnailCtrl::OnMenuUser5)
END_MESSAGE_MAP()


BOOL CThumbnailCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, THUMBNAIL_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		//wndcls.hbrBackground  = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.hbrBackground = NULL;
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = THUMBNAIL_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}
	return TRUE;
}


// CThumbnailCtrl 메시지 처리기

void CThumbnailCtrl::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWnd::PreSubclassWindow();
}


void CThumbnailCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bmpBuffer.DeleteObject();
	DestroyExpandView();
}


BOOL CThumbnailCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//return FALSE;
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, m_clrBackGrnd);

	return CWnd::OnEraseBkgnd(pDC);
}


void CThumbnailCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
	CRect rect;
	GetClientRect(&rect);
	CDC memDC;
	CBitmap bmp, * pOldBmp;

	memDC.CreateCompatibleDC(&dc);

	if (m_bReDraw == TRUE) {
		bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		pOldBmp = memDC.SelectObject(&bmp);
		DrawImage(&memDC, rect);
	}
	else {
		pOldBmp = memDC.SelectObject(&m_bmpBuffer);
	}
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	bmp.DeleteObject();
	memDC.DeleteDC();
}


void CThumbnailCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int i = 0;
	int DspImageSt = GetScrollPos(SB_VERT) * m_nThumCols;
	CString strFileName = _T("");
	BOOL bDestroy = TRUE;

	if (m_bDispPopupImage == TRUE) {
		for (i = 0; i < m_nMaxImageDispNum; i++) {
			if (i >= MAX_DISP_IMAGE_NUM) {
				ASSERT(0);
				break;
			}

			if (m_rectImage[i].PtInRect(point) != 0) {
				// 22.06.15 Ahn Modify Start
				//POSITION pos = m_strFileList.GetHeadPosition();
				//pos = m_strFileList.FindIndex(DspImageSt + i);
				//if (pos != NULL) {
				//	strFileName = m_strFileList.GetAt(pos);
				//}

				VEC_THUMB_DATA::iterator iter;
				iter = m_vecThumbData.begin() + (DspImageSt + i);
				strFileName = iter->strFileName;
				// 22.06.15 Ahn Modify End

				if (strFileName.GetLength() > 0) {
					m_nSelectIndex = DspImageSt + i;
					if (m_pExpandView != NULL) {
						if (m_pExpandView->m_hWnd == NULL) {
							DestroyExpandView();
							CreateExpandView(strFileName);
						}
						else {
							m_pExpandView->SetImageFile(strFileName);
						}
					}
					else {
						DestroyExpandView();
						CreateExpandView(strFileName);
					}
					bDestroy = FALSE;
				}
				break;
			}
		}
		if (bDestroy == TRUE) {
			DestroyExpandView();
		}
	}

	if (1) {
		for (i = 0; i < m_nMaxImageDispNum; i++) {
			if (i >= MAX_DISP_IMAGE_NUM) {
				ASSERT(0);
				break;
			}

			if (m_rectImage[i].PtInRect(point) != 0) {
				SendMessageToParent(i, NM_CLICK);
				m_nSelectIndex = DspImageSt + i;
				Invalidate(FALSE);
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}


void CThumbnailCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int DspImageSt = GetScrollPos(SB_VERT) * m_nThumCols;

	for (int DispIdx = 0; DispIdx < m_nMaxImageDispNum; DispIdx++) {
		if (DispIdx >= MAX_DISP_IMAGE_NUM) {
			ASSERT(0);
			break;
		}

		if (m_rectImage[DispIdx].PtInRect(point) == TRUE) {
		//KANG 22.07.08 Add Start
			SendMessageToParent( DspImageSt + DispIdx, NM_DBLCLK);
		//KANG 22.07.08 Add End
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}


void CThumbnailCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int i = 0;
	int DspImageSt = GetScrollPos(SB_VERT) * m_nThumCols;
	BOOL bContext = FALSE;

	for (i = 0; i < m_nMaxImageDispNum; i++) {
		if (i >= MAX_DISP_IMAGE_NUM) {
			ASSERT(0);
			break;
		}

		if (m_rectImage[i].PtInRect(point) != 0) {
			m_nSelectIndex = DspImageSt + i;
			// 22.06.15 Ahn Modify Start
			//if (m_nSelectIndex < m_strFileList.GetSize()) {
			if( m_nSelectIndex < (int)m_vecThumbData.size() ){ 
			// 22.06.15 Ahn Modify End

				if (m_pExpandView != NULL) {
					if (m_pExpandView->m_hWnd == NULL) {
						bContext = TRUE;
					}
				}
				else {
					bContext = TRUE;
				}
			}
		}
	}

	if (bContext == TRUE && m_nContextMenuNum > 0) {
		CMenu menu;
		menu.CreatePopupMenu();
		if (m_nContextMenuNum > 0) {
			menu.AppendMenu(MF_STRING, IDR_MENU_USER1, m_strContextMenu[0]);
		}
		if (m_nContextMenuNum > 1) {
			menu.AppendMenu(MF_STRING, IDR_MENU_USER2, m_strContextMenu[1]);
		}
		if (m_nContextMenuNum > 2) {
			menu.AppendMenu(MF_STRING, IDR_MENU_USER3, m_strContextMenu[2]);
		}
		if (m_nContextMenuNum > 3) {
			menu.AppendMenu(MF_STRING, IDR_MENU_USER4, m_strContextMenu[3]);
		}
		if (m_nContextMenuNum > 4) {
			menu.AppendMenu(MF_STRING, IDR_MENU_USER5, m_strContextMenu[4]);
		}

		CPoint	pt = point;
		ClientToScreen(&pt);
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		menu.DestroyMenu();
	}

	CWnd::OnRButtonDown(nFlags, point);

}


BOOL CThumbnailCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CWnd::OnNotify(wParam, lParam, pResult);
}


void CThumbnailCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	DestroyExpandView();
	BOOL bScrollBarDIsplay = CheckScrollDIsplay();
	ShowScrollBar(SB_VERT, bScrollBarDIsplay);
	AdjustDispSize(cx, cy);
	Invalidate(FALSE);
}


void CThumbnailCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DestroyExpandView();

	SCROLLINFO scrollInfo;
	GetScrollInfo(SB_VERT, &scrollInfo);
	int scrlpos = scrollInfo.nPos;

	switch (nSBCode) {
	case SB_LINEDOWN:
		scrlpos++;
		break;
	case SB_LINEUP:
		scrlpos--;
		break;
	case SB_PAGEUP:
		scrlpos -= m_nThumRows;
		break;
	case SB_PAGEDOWN:
		scrlpos += m_nThumRows;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		scrlpos = nPos;
		break;
	default:
		break;
	}

	if (scrlpos < scrollInfo.nMin) {
		scrlpos = scrollInfo.nMin;
	}
	else if (scrlpos > scrollInfo.nMax) {
		scrlpos = scrollInfo.nMax;
	}
	SetScrollPos(SB_VERT, scrlpos);
	Invalidate(FALSE);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CThumbnailCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta > 0) {
		PagePrev();
	}
	else {
		PageNext();
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


int CThumbnailCtrl::PagePrev()
{
	// TODO: 여기에 구현 코드 추가.
	DestroyExpandView();

	int scrlpos = GetScrollPos(SB_VERT);

	if (scrlpos > m_nThumRows) {
		scrlpos -= m_nThumRows;
		SetScrollPos(SB_VERT, scrlpos, TRUE);
		Invalidate(FALSE);
	}
	else if (scrlpos > 0) {
		scrlpos = 0;
		SetScrollPos(SB_VERT, scrlpos, TRUE);
		Invalidate(FALSE);
	}

	return 0;
}

int CThumbnailCtrl::PageNext()
{
	// TODO: 여기에 구현 코드 추가.
	DestroyExpandView();

	// 22.06.15 Ahn Modify Start
	//int	MaxDispImage = (int)m_strFileList.GetSize();
	int MaxDispImage = (int)m_vecThumbData.size();
	// 22.06.15 Ahn Modify End
	int scrlpos = GetScrollPos(SB_VERT);

	if (scrlpos + m_nThumRows < (MaxDispImage / m_nThumCols)) {
		scrlpos += m_nThumRows;
		SetScrollPos(SB_VERT, scrlpos, TRUE);
		Invalidate(FALSE);
	}
	else if (scrlpos <= (MaxDispImage / m_nThumCols) - m_nThumRows) {
		scrlpos = (MaxDispImage / m_nThumCols) - m_nThumRows + 1;
		SetScrollPos(SB_VERT, scrlpos, TRUE);
		Invalidate(FALSE);
	}

	return 0;
}

void CThumbnailCtrl::OnMenuUser1()
{
	SendMessageToParent(m_nSelectIndex, WM_USER_DEF1);
}

void CThumbnailCtrl::OnMenuUser2()
{
	SendMessageToParent(m_nSelectIndex, WM_USER_DEF2);
}

void CThumbnailCtrl::OnMenuUser3()
{
	SendMessageToParent(m_nSelectIndex, WM_USER_DEF3);
}

void CThumbnailCtrl::OnMenuUser4()
{
	SendMessageToParent(m_nSelectIndex, WM_USER_DEF4);
}

void CThumbnailCtrl::OnMenuUser5()
{
	SendMessageToParent(m_nSelectIndex, WM_USER_DEF5);
}

int CThumbnailCtrl::AdjustDispSize(int cx, int cy)
{
	if (cx == 0 && cy == 0) {
		return(0);
	}
	CRect	rectImageArea;
	{
		int	btnW = (cx - INTR_W) / 9;
		int top1 = cy - BTN_H * 2;
		int top2 = cy - BTN_H;
		int	lft = 0 ;

		rectImageArea.left = 10;
		rectImageArea.right = cx - 10;
		rectImageArea.top = 0;
		rectImageArea.bottom = cy - BTN_H * 2 - 10;
	}

	{
		int imgW, imgH, imgh, imgw;

		m_nMaxImageDispNum = m_nThumCols * m_nThumRows ;

		imgH = imgh = (int)(rectImageArea.Height() - IMAGE_SPACE_LEN_H * ( m_nThumRows - 1 )) / (m_nThumRows) ;
		imgW = imgw = (int)(rectImageArea.Width() - IMAGE_SPACE_LEN_W * ( m_nThumCols - 1 )) / (m_nThumCols) ;

		int ptcW = imgW + IMAGE_SPACE_LEN_W ;
		int ptcH = imgH + IMAGE_SPACE_LEN_H ;

		if (imgw > imgh) {
			imgw = imgh;
		} else {
			imgh = imgw;
		}

		for (int i = 0; i < MAX_DISP_IMAGE_NUM; i++) {
			m_rectImage[i].SetRect(0, 0, 0, 0);
		}

		for (int i = 0; i < m_nMaxImageDispNum; i++) {
			int x = i % m_nThumCols;
			int y = i / m_nThumCols;

			int xs = (int)(rectImageArea.left + x * ptcW);
			int xe = xs + imgw;
			int ys = (int)(rectImageArea.top + (IMAGE_SPACE_LEN_H / 4) + ptcH * y + (imgH - imgh) / 2);
			int ye = ys + imgh;

			if (i >= MAX_DISP_IMAGE_NUM) {
				ASSERT(0);
				break;
			}

			m_rectImage[i].left = xs;
			m_rectImage[i].right = xe;
			m_rectImage[i].top = ys;
			m_rectImage[i].bottom = ye;
		}

		// 22.06.15 Ahn Modify Start
		//int	MaxDispImage = (int)m_strFileList.GetSize();
		int	MaxDispImage = (int)m_vecThumbData.size();
		// 22.06.15 Ahn Modify End
		int DispIdx = 0;

		SCROLLINFO sri;
		memset(&sri, 0x00, sizeof(sri));
		sri.cbSize = sizeof(sri);
		sri.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		sri.nPage = m_nThumRows;
		sri.nMax = ((MaxDispImage - 1) / m_nThumCols);
		sri.nMin = 0;
		sri.nPos = (DispIdx / m_nThumCols) - m_nThumRows + 1;
		SetScrollInfo(SB_VERT, &sri);
	}

	m_DispSize.cx = cx;
	m_DispSize.cy = cy;

	return(0);
}
void CThumbnailCtrl::DrawImage(CDC* pDC, CRect& WinRect)
{
	Graphics		graphics(pDC->m_hDC);
	FontFamily		fontFamily(L"MS PGothic");
	Gdiplus::Font	font(&fontFamily, 15, FontStyleRegular, UnitPixel);
	Gdiplus::Font	fontScale(&fontFamily, 10, FontStyleRegular, UnitPixel);
	FontFamily		fontFamily2(L"MS PGothic");
	Gdiplus::Font   fontmark(&fontFamily2, 12, FontStyleRegular, UnitPixel);
	StringFormat format;
	WCHAR wchr[256], wbuf[256];
	format.SetAlignment(StringAlignmentCenter);
	SolidBrush  solidBrush(Color(255, 0, 0, 0));
	CString strTmp;
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	CString	fname;
	CRect	rectImg;

	{
		DWORD c = m_clrBackGrnd;//::GetSysColor(COLOR_3DFACE);
		SolidBrush brs(Color((BYTE)c, (BYTE)(c >> 8), (BYTE)(c >> 16)));
		graphics.FillRectangle(&brs, WinRect.left, WinRect.top, WinRect.Width(), WinRect.Height());
	}

	{
		WCHAR	wchar[512];

		int DspImageSt = GetScrollPos(SB_VERT) * m_nThumCols;
		int ErrVectorNum = 0;
		float contrast = 0.0;
		float scale = (100.0f + contrast) / 100.0f;
		scale *= scale;
		float append = 0.5f * (1.0f - scale);
		ColorMatrix cm =
		{
		scale,0.0f,0.0f,0.0f,0.0f,
		0.0f,scale,0.0f,0.0f,0.0f,
		0.0f,0.0f,scale,0.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,0.0f,
		append,append,append,0.0f,1.0f
		};
		ImageAttributes ia;
		ia.SetColorMatrix(&cm);


		// 22.06.15 Ahn Modify Start
		//POSITION pos = m_strFileList.GetHeadPosition();
		//pos = m_strFileList.FindIndex(DspImageSt);
		VEC_THUMB_DATA::iterator iter;
		// 22.12.15 Ahn Add Start
		if (m_vecThumbData.size() < DspImageSt) 
			return ;
		// 22.12.15 Ahn Add End
		iter = m_vecThumbData.begin() + DspImageSt;
		// 22.06.15 Ahn Modify End

		for (int DispIdx = 0; DispIdx < m_nMaxImageDispNum; DispIdx++) {
			// 22.06.15 Ahn Modify Start
			//if (pos == NULL) {
			//	break;
			//}
			if ((iter == m_vecThumbData.end())) {
				break ;
			}
			// 22.06.15 Ahn Modify End


			// 22.06.15 Ahn Modify Start
			//fname = m_strFileList.GetNext(pos);
			fname = iter->strFileName;
			// 22.06.15 Ahn Modify End


			rectImg = m_rectImage[DispIdx];
			rectImg.InflateRect(1, 1, 1, 1);

			memset(wchar, 0, sizeof(WCHAR) * 512);
			size_t ReturnValue;
			mbstowcs_s(&ReturnValue, wchar, sizeof(wchar) / sizeof(WCHAR), fname.GetBuffer(0), _TRUNCATE);

			Image img(wchar);

			LONG imageBottom;

			{
				Pen fpen(Color(0, 0, 0));
				DWORD clrFrm = m_clrImgFrame;
				// 23.01.16 Ahn Add Start
				if (m_nSelectIndex == (DispIdx + DspImageSt)) {
					clrFrm = RGB(200, 50, 50);
				}
				// 23.01.16 Ahn Add End
				fpen.SetColor(Color((BYTE)clrFrm, (BYTE)(clrFrm >> 8), (BYTE)(clrFrm >> 16)));
				fpen.SetWidth(REAL(m_nImgFrameWidth));

				LONG imgWidth = img.GetWidth();
				LONG imgHeight = img.GetHeight();
				LONG rectWidth = rectImg.Width();
				LONG rectHeight = rectImg.Height();
				LONG drawWidth;
				LONG drawHeight;
				double rateW = 1.0;
				double rateH = 1.0;

				rateW = (double)rectWidth / (double)imgWidth;
				rateH = (double)rectHeight / (double)imgHeight;

				double rateMax = 3.0;
				if (rateW < rateH) {
					if (rateW > rateMax) {
						rateW = rateMax;
					}
					drawWidth = (LONG)(imgWidth * rateW);
					drawHeight = (LONG)(imgHeight * rateW);
				}
				else if (rateW > rateH) {
					if (rateH > rateMax) {
						rateH = rateMax;
					}
					drawWidth = (LONG)(imgWidth * rateH);
					drawHeight = (LONG)(imgHeight * rateH);
				}
				else {
					if (rateW > rateMax) {
						rateW = rateMax;
					}
					if (rateH > rateMax) {
						rateH = rateMax;
					}
					drawWidth = (LONG)(imgWidth * rateW);
					drawHeight = (LONG)(imgHeight * rateH);
				}

				if (drawWidth < 16) {
					drawWidth = 16;
				}
				if (drawHeight < 16) {
					drawHeight = 16;
				}
				DWORD color = RGB(0, 0, 0);
				SolidBrush brush(Color((BYTE)color, (BYTE)(color >> 8), (BYTE)(color >> 16)));

				graphics.FillRectangle(&brush, rectImg.left + rectImg.Width() / 2 - rectWidth / 2 - 1, rectImg.top + rectImg.Height() / 2 - rectHeight / 2 - 1, rectWidth + 1, rectHeight + 1);

				graphics.DrawImage(&img, Rect(rectImg.left + rectImg.Width() / 2 - drawWidth / 2, rectImg.top + rectImg.Height() / 2 - drawHeight / 2, drawWidth, drawHeight), 0, 0, img.GetWidth(), img.GetHeight(), UnitPixel, &ia);
				drawWidth = drawHeight;
				graphics.DrawRectangle(&fpen, rectImg.left + rectImg.Width() / 2 - rectWidth / 2 - 1, rectImg.top + rectImg.Height() / 2 - rectHeight / 2 - 1, rectWidth + 1, rectHeight + 1);
				imageBottom = rectImg.CenterPoint().y + rectHeight / 2;
			}

			memset(wchr, 0, 100);

			//CString tmp = fname.Mid(fname.ReverseFind('\\') + 1);
			CString tmp = iter->strDispName ;
			::mbstowcs_s(NULL, wbuf, sizeof(wbuf) / sizeof(WCHAR), tmp.GetBuffer(0), _TRUNCATE);
			wcscat_s(wchr, wbuf);

			{
				PointF	point;
				point.X = (float)(rectImg.left);
				point.Y = (float)(imageBottom + 2);
				format.SetAlignment(StringAlignmentNear);
				graphics.DrawString(wchr, -1, &font, point, &format, &solidBrush);
				format.SetAlignment(StringAlignmentCenter);
			}

			iter++;
		}
	}

	return;
}


int CThumbnailCtrl::ReadImageFileInFolder(LPCTSTR lpszFilePath, LPCTSTR lpszExt /*= _T("JPG|BMP")*/, BOOL bSubFolder /*= FALSE*/, BOOL bReDraw /*= TRUE*/)
{
	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];										// 경로저장 버퍼 

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);

	CString strFilePath = lpszFilePath;
	CString strExt = lpszExt;
	if (strExt.GetLength() == 0) {
		strExt = _T("JPG|BMP");
	}

	CString strPath = lpszFilePath;
	if (strPath.GetLength() == 0) {
		BrInfo.hwndOwner = GetSafeHwnd();
		BrInfo.lpszTitle = _T("파일이 저장될 폴더를 선택하세요");
		BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
		LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
		::SHGetPathFromIDList(pItemIdList, szBuffer);               // 파일경로 읽어오기
		strPath.Format(_T("%s"), szBuffer);
	}

	if (strPath.GetLength() > 0) {
		// 22.06.15 Ahn Modify Start
		//GetFileList(strPath, m_strFileList, strExt, 0, bSubFolder, TRUE);
		GetFileList(strPath, m_vecThumbData, strExt, 0, bSubFolder, TRUE);
		// 22.06.15 Ahn Modify End

	}

	if (bReDraw == TRUE) {
		OnRefresh();
	}

	return 0;
}

// 22.06.15 Ahn Add Start
int CThumbnailCtrl::GetFileList(LPCTSTR lpszPath, VEC_THUMB_DATA& vecThumbList, LPCTSTR lpszExt /*= _T( "*" )*/, int nMaxFiles /*= 0*/, BOOL bSubFolder /*= FALSE*/, BOOL bListClear /*= TRUE*/)
{
	if (bListClear) {
		vecThumbList.clear();
	}

	CString strPath = lpszPath;
	CString strExt = lpszExt;
	CStringArray strExtArr;
	CString strFileName = _T("");
	CStringList rstrFileNameList;
	rstrFileNameList.RemoveAll();

	if (strPath.Right(1).Compare("\\") != 0) {
		strPath += _T("\\");
	}
	strPath += _T("*.*");

	int nSt = 0;
	int nEd = 0;
	CString strTemp;
	while (1) {
		nEd = strExt.Find('|', nSt);
		if (nEd < 0) {
			strTemp = strExt.Mid(nSt);
			strExtArr.Add(strExt.Mid(nSt));
			break;
		}
		strTemp = strExt.Mid(nSt, (nEd - nSt));
		strExtArr.Add(strExt.Mid(nSt, (nEd - nSt)));
		nSt = nEd + 1;
	}

	CFileFind	finder;
	BOOL bWorking = finder.FindFile(strPath);
	if (bWorking == 0) {
		return (0);
	}
	strPath += strExt;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) {
			continue;
		}
		if (finder.IsDirectory()) {
			if (bSubFolder) {
				GetFileList(finder.GetFilePath(), vecThumbList, lpszExt, nMaxFiles, bSubFolder, FALSE);
			}
			continue;
		}

		if (strExtArr.GetSize() > 0) {
			strFileName = finder.GetFilePath();
			int nFind = strFileName.ReverseFind('.');
			CString strTmp = strFileName.Mid(nFind + 1);
			if (strFileName.GetLength() == 0) {
				continue;
			}
			BOOL bFind = FALSE;
			for (int i = 0; i < strExtArr.GetSize(); i++) {
				if (strFileName.Mid(nFind + 1).CompareNoCase(strExtArr.GetAt(i)) == 0) {
					bFind = TRUE;
					break;
				}
			}
			if (bFind != TRUE) {
				continue;
			}
		}

		//rstrFileNameList.AddTail(finder.GetFilePath());
		CThumbData data;
		data.strFileName = finder.GetFilePath();
		data.strDispName = _T("");
		vecThumbList.push_back(data);
		if (nMaxFiles > 0) {
			if (rstrFileNameList.GetCount() >= nMaxFiles) {
				break;
			}
		}
	}
	finder.Close();

	//OnRefresh();

	return (0);
}
// 22.06.16 Ahn Add End
int CThumbnailCtrl::GetFileList(LPCTSTR lpszPath, CStringList& rstrFileNameList, LPCTSTR lpszExt /*= _T( "*" )*/, int nMaxFiles /*= 0*/, BOOL bSubFolder /*= FALSE*/, BOOL bListClear /*= TRUE*/)
{
	if (bListClear) {
		rstrFileNameList.RemoveAll();
	}

	CString strPath = lpszPath;
	CString strExt = lpszExt;
	CStringArray strExtArr;
	CString strFileName = _T("");

	if (strPath.Right(1).Compare("\\") != 0) {
		strPath += _T("\\");
	}
	strPath += _T("*.*");

	int nSt = 0;
	int nEd = 0;
	CString strTemp;
	while (1) {
		nEd = strExt.Find('|', nSt);
		if (nEd < 0) {
			strTemp = strExt.Mid(nSt);
			strExtArr.Add(strExt.Mid(nSt));
			break;
		}
		strTemp = strExt.Mid(nSt, (nEd - nSt));
		strExtArr.Add(strExt.Mid(nSt, (nEd - nSt)));
		nSt = nEd + 1;
	}

	CFileFind	finder;
	BOOL bWorking = finder.FindFile(strPath);
	if (bWorking == 0) {
		return (0);
	}
	strPath += strExt;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) {
			continue;
		}
		if (finder.IsDirectory()) {
			if (bSubFolder) {
				GetFileList(finder.GetFilePath(), rstrFileNameList, lpszExt, nMaxFiles, bSubFolder, FALSE);
			}
			continue;
		}

		if (strExtArr.GetSize() > 0) {
			strFileName = finder.GetFilePath();
			int nFind = strFileName.ReverseFind('.');
			CString strTmp = strFileName.Mid(nFind + 1);
			if (strFileName.GetLength() == 0) {
				continue;
			}
			BOOL bFind = FALSE;
			for (int i = 0; i < strExtArr.GetSize(); i++) {
				if (strFileName.Mid(nFind + 1).CompareNoCase(strExtArr.GetAt(i)) == 0) {
					bFind = TRUE;
					break;
				}
			}
			if (bFind != TRUE) {
				continue;
			}
		}

		rstrFileNameList.AddTail(finder.GetFilePath());
		if (nMaxFiles > 0) {
			if (rstrFileNameList.GetCount() >= nMaxFiles) {
				break;
			}
		}
	}
	finder.Close();

	//OnRefresh();

	return (0);
}

void CThumbnailCtrl::OnRefresh()
{
	CRect rect;
	this->GetClientRect(rect);
	//SetScrollPos(SB_VERT, 0, TRUE); // 22.12.06 Ahn Delete 
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.Width(), rect.Height()));
	// 22.12.06 Ahn Add Start
	int MaxDispImage = (int)m_vecThumbData.size();

	int scrlpos = GetScrollPos(SB_VERT);
	int scrlMax = (MaxDispImage / m_nThumCols);

	if (scrlpos < scrlMax) {
		SetScrollPos(SB_VERT, scrlMax, TRUE);
		Invalidate(FALSE);
	}
	// 22.12.06 Ahn Add End
	m_nSelectIndex = -1;
}

LRESULT CThumbnailCtrl::SendMessageToParent(int nIndex, int nMessage) const
{
	if (!IsWindow(m_hWnd))
		return 0;

	NM_THUMBDATA nmgv;

	nmgv.nIndex = nIndex;
	nmgv.nItem = nIndex;
	nmgv.hdr.hwndFrom = m_hWnd;
	nmgv.hdr.idFrom = GetDlgCtrlID();
	nmgv.hdr.code = nMessage;

	CWnd* pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		return pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
	else
		return 0;
}

void CThumbnailCtrl::CreateExpandView(LPCTSTR lpszImageFileName)
{
	CRect rectClient;
	this->GetClientRect(&rectClient);

	if (m_bReDraw == TRUE) {
		CBitmap* pOldBmp;
		CDC dcMem;
		CDC* pDC = GetDC();
		m_bmpBuffer.DeleteObject();
		dcMem.CreateCompatibleDC(pDC);
		m_bmpBuffer.CreateCompatibleBitmap(pDC, rectClient.Width(), rectClient.Height());
		pOldBmp = (CBitmap*)dcMem.SelectObject(&m_bmpBuffer);
		dcMem.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), pDC, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOldBmp);
		dcMem.DeleteDC();
	}


	int iDispSize = rectClient.Width() > rectClient.Height() ? (int)((double)rectClient.Height() * 3.0 / 4.0) : (int)((double)rectClient.Width() * 3.0 / 4.0);
	int nXpos = (int)((double)rectClient.Width() / 2.0) - (int)((double)iDispSize / 2.0);
	int nYpos = (int)((double)rectClient.Height() / 2.0) - (int)((double)iDispSize / 2.0);
	m_pExpandView = new CExpandViewImage(lpszImageFileName);
	if (m_pExpandView->Create(_T("확대이미지"), WS_CHILD | WS_DLGFRAME | WS_BORDER | SS_NOTIFY | SS_LEFT, CRect(nXpos, nYpos, iDispSize + nXpos, iDispSize + nYpos), this)) {
		//m_pExpandView->SetWindowPos(NULL, nXpos, nYpos, iDispSize, iDispSize, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW );
		m_pExpandView->ShowWindow(SW_SHOW);
		m_bReDraw = FALSE;
	}
}

void CThumbnailCtrl::DestroyExpandView()
{
	if (m_pExpandView != NULL) {
		if (m_pExpandView->m_hWnd != NULL) {
			m_pExpandView->DestroyWindow();
		}
		delete m_pExpandView;
		m_pExpandView = NULL;
	}
	UpdateWindow();
	m_bReDraw = TRUE;
}

void CThumbnailCtrl::SetBackGroundColor(COLORREF bkColor)
{
	m_clrBackGrnd = bkColor;
}

void CThumbnailCtrl::SetImageFrameColor(COLORREF frmColor)
{
	m_clrImgFrame = frmColor;
}

void CThumbnailCtrl::SetImageFrameWidth(int width)
{
	m_nImgFrameWidth = width;
}

void CThumbnailCtrl::SetThumbCols(int cols)
{
	m_nThumCols = cols;
}

void CThumbnailCtrl::SetThumbRows(int rows)
{
	m_nThumRows = rows;
}

int CThumbnailCtrl::GetImageCount()
{
	// 22.06.15 Ahn Modify Start
	//return (int)m_strFileList.GetCount();
	return (int)m_vecThumbData.size();
	// 22.06.15 Ahn Modify End
}

CString CThumbnailCtrl::GetImageFileNameFromList( int nIndex )
{
	CString strFileName = _T("");

	// 22.06.15 Ahn Modify End
	//POSITION pos = m_strFileList.GetHeadPosition();
	//pos = m_strFileList.FindIndex(nIndex);
	//if (pos != NULL) {
	//	strFileName = m_strFileList.GetAt(pos);
	//}

	// 22.07.19 Ahn Modify Start
	int nSize = (int)m_vecThumbData.size() ;

	if (nSize > nIndex) {

		VEC_THUMB_DATA::iterator iter;
		iter = m_vecThumbData.begin() + nIndex;

		if (iter != m_vecThumbData.end()) {
			// 22.07.19 Ahn Modify End
			strFileName = iter->strFileName;
		}
	}
	// 22.06.15 Ahn Modify End


	return strFileName;
}

void CThumbnailCtrl::SetContextMenuString(LPCTSTR lpszMenu, int nIndex)
{
	if (nIndex < 5 && nIndex >= 0) {
		m_strContextMenu[nIndex] = lpszMenu;
	}
}

void CThumbnailCtrl::SetContextMenuNum(int nNum)
{
	if (nNum < 5 && nNum >= 0) {
		m_nContextMenuNum = nNum;
	}
}


// 22.02.11 Ahn Add Start
int	CThumbnailCtrl::AddImageList(LPCTSTR lpszFilePath)
{
	int nRet = 0;

	// 22.06.15 Ahn Modify Start
	//m_strFileList.AddTail(lpszFilePath);
	CThumbData data;
	data.strFileName = lpszFilePath;
	data.strDispName = _T("");
	m_vecThumbData.push_back(data);
	// 22.06.15 Ahn Modify End

	return nRet;
}

int	CThumbnailCtrl::SetImageList(CStringList &strList)
{
	int nRet = 0; 

	POSITION pos;
	pos = strList.GetHeadPosition();
	CString strTemp;

	// 22.06.15 Ahn Modify Start
	//m_strFileList.RemoveAll();
	//for (; pos != nullptr; ) {
	//	strTemp = strList.GetNext(pos);
	//	m_strFileList.AddTail(strTemp);
	//}
	m_vecThumbData.clear();
	for (; pos != nullptr; ) {
		strTemp = strList.GetNext(pos);
		CThumbData data;
		data.strFileName = strTemp;
		data.strDispName = _T("");
		m_vecThumbData.push_back(data);
	}
	// 22.06.15 Ahn Modify End


	OnRefresh();

	return nRet;
}
// 22.02.11 Ahn Add End

// 22.06.10 Ahn Add Start
int CThumbnailCtrl::AddImageBufferMode(CString strFileName, CString strDispName)
{
	int nRet = 0;

	int nSize = (int) m_vecThumbData.size();

	VEC_THUMB_DATA::iterator iter;
	if (nSize >= MAX_CROP_IMAGE_QUEUE_SIZE) {
		iter = m_vecThumbData.erase( m_vecThumbData.begin() ) ;		
	}

	CThumbData data;
	data.strFileName = strFileName;
	data.strDispName = strDispName;
	m_vecThumbData.push_back(data);

	//int nSize = (int)m_strFileList.GetCount();
	//if ( 0 > (nSize - MAX_CROP_IMAGE_QUEUE_SIZE) ){
	//	POSITION  pos = m_strFileList.GetHeadPosition();
	//	int nDelCnt = (nSize - MAX_CROP_IMAGE_QUEUE_SIZE); 
	//	for  ( int i = 0 ; i < nDelCnt ; i++) {
	//		m_strFileList.RemoveHead();
	//	}
	//}

	//nSize = (int)m_strNameList.GetCount();
	//if (0 > (nSize - MAX_CROP_IMAGE_QUEUE_SIZE)) {
	//	POSITION  pos = m_strFileList.GetHeadPosition();
	//	int nDelCnt = (nSize - MAX_CROP_IMAGE_QUEUE_SIZE);
	//	for (int i = 0; i < nDelCnt; i++) {
	//		m_strNameList.RemoveHead();
	//	}
	//}

	//nSize = (int)m_strFileList.GetCount();
	//if (nSize == MAX_CROP_IMAGE_QUEUE_SIZE) {
	//	m_strFileList.RemoveHead();
	//}
	//m_strFileList.AddTail(strFileName);
	//nSize = (int)m_strNameList.GetCount();
	//if (nSize == MAX_CROP_IMAGE_QUEUE_SIZE) {
	//	m_strNameList.RemoveHead();
	//}
	//m_strNameList.AddTail(strFileName);

	return nRet;
}
// 22.06.10 Ahn Add End


// 22.06.14 Ahn Add Start
BOOL CThumbnailCtrl::CheckScrollDIsplay()
{
	// 22.06.15 Ahn Modify Start
	//int nSize = (int)m_strFileList.GetSize();
	int nSize = (int)m_vecThumbData.size();
	// 22.06.15 Ahn Modify End
	if (nSize > m_nMaxImageDispNum) {
		return SW_SHOW ;
	}
	return SW_HIDE;
}
// 22.06.14 Ahn Add End

// 22.06.15 Ahn Add Start
void CThumbnailCtrl::SetPopupImageMode(BOOL bMode)
{
	m_bDispPopupImage = bMode ;
}
// 22.06.15 Ahn Add End

// 22.07.06 Ahn Add Start
void CThumbnailCtrl::ClearAll()
{
	m_vecThumbData.clear();
}
// 22.07.06 Ahn Add End
