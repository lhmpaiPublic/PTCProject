
#include "pch.h"
#include "NotchingGradeInsp.h"
#include "ColorControl.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
{
	m_rgbText = GetSysColor(COLOR_BTNTEXT);
	m_rgbBackground = GetSysColor(COLOR_BTNFACE);
	m_Brush.CreateSolidBrush(m_rgbBackground);
	m_bBold = FALSE;
}

CColorStatic::~CColorStatic()
{
}

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CColorStatic 메시지 처리기

HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  여기서 DC의 특성을 변경합니다.
	pDC->SetTextColor(m_rgbText);
	pDC->SetBkColor(m_rgbBackground);
	return (m_Brush);
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	//return NULL;
}


void CColorStatic::SetBackgroundColor(COLORREF color)
{
	if (m_Brush.m_hObject != NULL) {
		m_Brush.DeleteObject();
	}
	m_rgbBackground = color;
	if (m_Brush.m_hObject == NULL) {
		m_Brush.CreateSolidBrush(m_rgbBackground);
	}
}

int CColorStatic::SetTextColor(COLORREF color)
{
	int	nRet = 0;
	m_rgbText = color;
	return (nRet);
}



// CBmpStatic

IMPLEMENT_DYNAMIC(CBmpStatic, CStatic)

CBmpStatic::CBmpStatic()
{
	m_bBitmap = FALSE;
	m_nBmpID = 0; // 22.08.03 Ahn Add 
}

CBmpStatic::~CBmpStatic()
{
	DeleteObject(hBmp);
}

BEGIN_MESSAGE_MAP(CBmpStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CBmpStatic 메시지 처리기

void CBmpStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.
	if (m_bBitmap) {
		CRect rect;
		GetClientRect(&rect);
		HDC memDC = CreateCompatibleDC(dc.m_hDC);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBmp);
		BITMAP bmp;
		GetObject(hBmp, sizeof(BITMAP), &bmp);
		//StretchBlt(dc.m_hDC, 0, 0, rect.Width(), rect.Height(), memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		TransparentBlt(dc.m_hDC, 0, 0, rect.Width(), rect.Height(), memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 0, 255));
		dc.SetBkMode(TRANSPARENT);
		SelectObject(memDC, oldBitmap);
		DeleteDC(memDC);
	}

}

int CBmpStatic::SetBitmap(UINT nID)
{
	// TODO: 여기에 구현 코드 추가.
	m_bBitmap = TRUE;
	//	hBmp = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_LGES_LOGO));
	hBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(nID), IMAGE_BITMAP, 0, 0, 0);

	// 22.08.03 Ahn Add Start
	m_nBmpID = nID;
	// 22.08.03 Ahn Add End 
	return 0;
}

// 22.08.03 Ahn Add Start
UINT CBmpStatic::GetBitmapID()
{
	return m_nBmpID;
}
// 22.08.03 Ahn Add End


// CColorEdit

IMPLEMENT_DYNAMIC(CColorEdit, CEdit)

CColorEdit::CColorEdit()
{
	m_rgbText = GetSysColor(COLOR_BTNTEXT);
	m_rgbBackground = GetSysColor(COLOR_BTNFACE);
	m_Brush.CreateSolidBrush(m_rgbBackground);
}

CColorEdit::~CColorEdit()
{
}

BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CColorStatic 메시지 처리기

HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  여기서 DC의 특성을 변경합니다.
	pDC->SetTextColor(m_rgbText);
	pDC->SetBkColor(m_rgbBackground);
	return (m_Brush);
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	//return NULL;
}


void CColorEdit::SetBackgroundColor(COLORREF color)
{
	if (m_Brush.m_hObject != NULL) {
		m_Brush.DeleteObject();
	}
	m_rgbBackground = color;
	if (m_Brush.m_hObject == NULL) {
		m_Brush.CreateSolidBrush(m_rgbBackground);
	}
}

int CColorEdit::SetTextColor(COLORREF color)
{
	int	nRet = 0;
	m_rgbText = color;
	return (nRet);
}


// CBmpButton

IMPLEMENT_DYNAMIC(CBmpButton, CBitmapButton)

CBmpButton::CBmpButton()
{
	m_bRadioButton = 0; // 1:라디오 버튼
	m_bCheckButton = 0; // 1:체크 박스
	m_bPushed = FALSE;
}

CBmpButton::~CBmpButton()
{

}

BEGIN_MESSAGE_MAP(CBmpButton, CBitmapButton)

END_MESSAGE_MAP()


// CBmpButton 메시지 처리기

void CBmpButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{

	// TODO:  지정된 항목을 그리는 코드를 추가합니다.
	ASSERT(lpDIS != NULL);
	//	ASSERT(m_bitmap.m_hObject != NULL);

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	if (pDC == NULL) { // 21.08.03 Ahn Test
		return;
	}
	CBitmap* pBitmap = NULL;
	if (m_bitmap.m_hObject != NULL) {
		pBitmap = &m_bitmap;
	}

	CRect rectFocus, rectBtn, rectBmp;
	rectFocus.CopyRect(&lpDIS->rcItem);
	rectBtn.CopyRect(&lpDIS->rcItem);
	rectBmp.CopyRect(&lpDIS->rcItem);
	rectFocus.DeflateRect(4, 4);
	rectBmp.DeflateRect(2, 2);
	pDC->FillRect(&rectBtn, CBrush::FromHandle((HBRUSH)GetStockObject(COLOR_3DFACE)));
	pDC->DrawEdge(rectBtn, EDGE_RAISED, BF_RECT);

	UINT state = lpDIS->itemState;
	if ((state & ODS_SELECTED) != NULL) {
		if (m_bitmapSel.m_hObject != NULL) {
			pBitmap = &m_bitmapSel;
		}
		pDC->DrawEdge(rectBtn, EDGE_SUNKEN, BF_RECT);
		rectBmp.OffsetRect(CPoint(2, 2));
	}
	else if ((state & ODS_FOCUS) != NULL) {
		if (m_bitmapFocus.m_hObject != NULL) {
			pBitmap = &m_bitmapFocus;
		}
	}
	else if ((state & ODS_DISABLED) != NULL) {
		if (m_bitmapDisabled.m_hObject != NULL) {
			pBitmap = &m_bitmapDisabled;
		}
	}


	if (m_bitmap.m_hObject != NULL) {
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap* pOld = memDC.SelectObject(pBitmap);
		if (pOld == NULL) {
			return;
		}
		// pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
		// 	 &memDC, 0, 0, SRCCOPY);
		BITMAP bits;
		pBitmap->GetObject(sizeof(BITMAP), &bits);
		pDC->StretchBlt(rectBmp.left, rectBmp.top, rectBmp.Width(), rectBmp.Height(),
			&memDC, 0, 0, bits.bmWidth, bits.bmHeight, SRCCOPY);
		memDC.SelectObject(pOld);
	}
	else {
		CString strCaption;
		GetWindowText(strCaption);
		pDC->DrawText(strCaption, rectBtn, (DT_CENTER | DT_VCENTER | DT_SINGLELINE));
	}
}


void CBmpButton::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CBitmapButton::PreSubclassWindow();
	unsigned int  dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	switch (dwStyle & 0x0F) // 버튼속성.
	{
	case BS_CHECKBOX:
	case BS_AUTOCHECKBOX:
		m_bCheckButton = 1;
		break;
	case BS_RADIOBUTTON:
	case BS_AUTORADIOBUTTON:
		m_bRadioButton = 1;
		break;
	}
	if (m_bCheckButton == 1 || m_bRadioButton == 1) {
		unsigned long dwRemove = BS_AUTO3STATE | BS_3STATE;
		ModifyStyle(0, (dwStyle & ~dwRemove) | BS_OWNERDRAW);
	}
	else {
		ModifyStyle(0, BS_OWNERDRAW);
	}
}


int CBmpButton::SetPushed(BOOL bPush)
{
	// TODO: 여기에 구현 코드 추가.
	m_bPushed = bPush;
	InvalidateRect(NULL, TRUE);
	return 0;
}