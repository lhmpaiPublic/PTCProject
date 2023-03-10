// GraphCtrl.cpp: 구현 파일
//

#include "pch.h"
#include "GraphCtrl.h"

// CGraphCtrl

IMPLEMENT_DYNAMIC(CGraphCtrl, CWnd)

CGraphCtrl::CGraphCtrl(int nDataNum)
{
	RegisterWindowClass();

	if (nDataNum < 1) {
		nDataNum = 1;
	}
	if (nDataNum > MAX_DATA_NUM) {
		nDataNum = MAX_DATA_NUM;
	}

	m_nDataNum = nDataNum;

	m_bDrawData = NULL;
	if (m_bDrawData == NULL) {
		m_bDrawData = new BOOL[m_nDataNum];
		if (m_bDrawData != NULL) {
			memset(m_bDrawData, 0x00, (sizeof(BOOL) * m_nDataNum));
		}
	}

	m_pMemo = NULL;
	if( m_pMemo == NULL ) {
	//KANG 22.06.17 Modify Start
	//	m_pMemo = new BYTE * [m_nDataNum];
		m_pMemo = new WORD * [m_nDataNum];
	//KANG 22.06.17 Modify End	
	}
	for (int i = 0; i < m_nDataNum; i++) {
		m_pMemo[i] = NULL;
		if (m_pMemo[i] == NULL) {
		//KANG 22.06.17 Modify Start
		//	m_pMemo[i] = new BYTE[MAX_GRAPH_DATA_NUM];
			m_pMemo[i] = new WORD[MAX_GRAPH_DATA_NUM];
		//KANG 22.06.17 Modify End	
			if (m_pMemo[i] != NULL) {
			//KANG 22.06.17 Modify Start
			//	memset(m_pMemo[i], 0x00, (sizeof(BYTE) * MAX_GRAPH_DATA_NUM));
				memset(m_pMemo[i], 0x00, (sizeof(WORD) * MAX_GRAPH_DATA_NUM));
			//KANG 22.06.17 Modify End
				m_bDrawData[i] = TRUE;
			}
		}
	}

	m_colorDataLine = NULL;
	if (m_colorDataLine == NULL) {
		m_colorDataLine = new COLORREF[m_nDataNum];
		if (m_colorDataLine != NULL) {
			for (int i = 0; i < m_nDataNum; i++) {
				m_colorDataLine[i] = RGB(255, 255, 255);
			}
		}
	}

	m_bDrawDataTotal = FALSE;

	m_rcDrawRect.left = 0;
	m_rcDrawRect.top = 0;
	m_rcDrawRect.right = 0;
	m_rcDrawRect.bottom = 0;
	AdjustSize();

	m_bIndividualDraw = FALSE;

	m_nImageSIze = 200;
	grp_size = 100;
	grp_start = 0;

	m_nMaxVerSIze = 100;
	m_nVerMajorScaleInterval = 20;
	m_nVerMinorScaleInterval = 5;
	m_nHorStartNum = 0;
	m_nHorScaleInterval = 10;

	m_nThreshold1 = 0;
	m_nThreshold2 = 0;

	m_bVerAxis = TRUE;
	m_bHorAxis = TRUE;

	m_bVerSubLine = FALSE;
	m_bHorSubLine = FALSE;
	m_bDrawFrameRect = FALSE;

	m_bFixHScroll = FALSE;

	m_strVerScaleTitle = _T("");
	m_strHorScaleTitle = _T("");

//KANG 22.06.17 Add Start
	m_bShowHScrollbar = TRUE;
	m_ColorBk = RGB(16, 16, 80);
	m_bDrawDataLine = TRUE;
	m_bDrawDataPoint = FALSE;
	m_bDrawPointZeroData = TRUE;
	m_bVerScaleLabel = FALSE;
	// 22.12.12 Ahn Modify Start
	//m_nVerScaleLabelSpace = 0;
	m_nVerScaleLabelSpace = 20;
	// 22.12.12 Ahn Modify End

	m_colorDataPoint = NULL;
	if (m_colorDataPoint == NULL) {
		m_colorDataPoint = new COLORREF[m_nDataNum];
		if (m_colorDataPoint != NULL) {
			for (int i = 0; i < m_nDataNum; i++) {
				m_colorDataPoint[i] = RGB(0, 0, 0);
			}
		}
	}

	m_nDataPointShape = NULL;
	if (m_nDataPointShape == NULL) {
		m_nDataPointShape = new int[m_nDataNum];
		if (m_nDataPointShape != NULL) {
			for (int i = 0; i < m_nDataNum; i++) {
				m_nDataPointShape[i] = 0;
			}
		}
	}
//KANG 22.06.17 Add End
}

CGraphCtrl::~CGraphCtrl()
{
	for (int i = 0; i < m_nDataNum; i++) {
		if (m_pMemo[ i ]) {
			delete[] m_pMemo[i];
			m_pMemo[i] = NULL;
		}
	}
	if (m_pMemo != NULL) {
		delete[] m_pMemo;
		m_pMemo = NULL;
	}

	if (m_bDrawData) {
		delete m_bDrawData;
	}

	if (m_colorDataLine) {
		delete m_colorDataLine;
	}
//KANG 22.06.17 Add Start
	if (m_colorDataPoint) {
		delete m_colorDataPoint;
	}

	if (m_nDataPointShape) {
		delete m_nDataPointShape;
	}
//KANG 22.06.17 Add End
}


BEGIN_MESSAGE_MAP(CGraphCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
END_MESSAGE_MAP()


BOOL CGraphCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetResourceHandle();

	if (!(::GetClassInfo(hInst, GRAPHCTRL_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = GRAPHCTRL_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CGraphCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	ASSERT(pParentWnd->GetSafeHwnd());

	if (!CWnd::Create(_T("GraphCtrl"), NULL, dwStyle, rect, pParentWnd, nID)) {
		return FALSE;
	}
	
	return TRUE;
}

void CGraphCtrl::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWnd::PreSubclassWindow();
}


// CGraphCtrl 메시지 처리기

int CGraphCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CGraphCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
	CRect memrect;
	GetClientRect(memrect);
	CBitmap bmp, * pOldBmp;
	CDC dcm;
	dcm.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, memrect.Width(), memrect.Height());
	pOldBmp = dcm.SelectObject(&bmp);
	DWORD bkColor = GetSysColor(COLOR_3DFACE);
	dcm.FillSolidRect(0, 0, memrect.Width(), memrect.Height(), bkColor);
	if (m_bDrawFrameRect) {
		CRect tmpRect;
		tmpRect.CopyRect(memrect);
		CBrush brush;
		brush.CreateSolidBrush(RGB(0, 0, 0));
		CBrush* pOldBrush = dcm.SelectObject(&brush);
		dcm.FrameRect(&tmpRect, &brush);
		dcm.SelectObject(pOldBrush);
		brush.DeleteObject();
	}

	DrawFtcGraphBack(&dcm, m_bHorAxis, m_bVerAxis);

	dc.BitBlt(0, 0, memrect.Width(), memrect.Height(), &dcm, 0, 0, SRCCOPY);
	dcm.SelectObject(pOldBmp);
	bmp.DeleteObject();
	dcm.DeleteDC();
}


void CGraphCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//KANG 22.06.17 Modify Start
//	CWnd::EnableScrollBarCtrl(SB_HORZ, TRUE);
	if (m_bShowHScrollbar == TRUE) {
		CWnd::EnableScrollBarCtrl(SB_HORZ, TRUE);
	}
//KANG 22.06.17 Modify End
	// 22.12.12 Ahn Delete Start
	SCROLLINFO si;
	si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	si.nMin = 0;
	si.nMax = m_nImageSIze - 1;
	si.nPage = m_nImageSIze;
	si.nPos = grp_start;
	SetScrollInfo(SB_HORZ, &si, FALSE);
	// 22.12.12 Ahn Delete End
	m_rcDrawRect.left = 0;
	m_rcDrawRect.top = 0;
	m_rcDrawRect.right = m_rcDrawRect.left + cx;
	m_rcDrawRect.bottom = m_rcDrawRect.top + cy - 20;

	Invalidate();
	AdjustSize();
}
void CGraphCtrl::AdjustSize(void)
{
	int nVerSpace = m_strVerScaleTitle.GetLength() > 0 ? ( FONT_HEIGHT * 2 ) : 0;
	int nHorSpace = FONT_HEIGHT * m_strHorScaleTitle.GetLength();
	int nVerAxis = m_bVerAxis ? 20 : 0;
	int nHorAxis = m_bHorAxis ? 20 : 0;

	m_rcGraphRect.top = m_rcDrawRect.top + nVerSpace + 20;
//KANG 22.06.17 Modify Start
//	m_rcGraphRect.left = m_rcDrawRect.left + nVerAxis + 10;
	m_rcGraphRect.left = m_rcDrawRect.left + nVerAxis + m_nVerScaleLabelSpace + 10;
//KANG 22.06.17 Modify End
	// 22.12.12 Ahn Modify Start
	m_rcGraphRect.bottom = m_rcDrawRect.bottom - nHorAxis - 10;
	// 22.12.12 Ahn Modify End
	m_rcGraphRect.right = m_rcDrawRect.right - nHorSpace - 10;

	int nRectWidth = 0;
	int nRectStart = 0;
	if (m_nDataNum >= 1) {
		nRectWidth = (((m_rcDrawRect.right - 10) - (m_rcDrawRect.left + (nVerAxis * m_nDataNum) + 10)) / m_nDataNum) - 10;
		nRectStart = m_rcGraphRect.left;
	}
	for (int i = 0; i < m_nDataNum; i++) {
		m_rcSubGraphRect[i].top = m_rcDrawRect.top + nVerSpace + 20;
		m_rcSubGraphRect[i].left = nRectStart;
		m_rcSubGraphRect[i].bottom = m_rcDrawRect.bottom - nHorAxis;
		m_rcSubGraphRect[i].right = m_rcSubGraphRect[i].left + nRectWidth;
		nRectStart = m_rcSubGraphRect[i].right + nVerAxis + 10;
	}
}

void CGraphCtrl::DrawFtcGraphBack(CDC* cdc, BOOL bHorizAxis/* =TRUE */, BOOL bVerticAxis/* =TRUE */)// FILMONLY
{
	CPen	hpen, * oldpen = NULL;
	short	j, GW, GH, nIndex = 0, GW_Sub[MAX_DATA_NUM] = { 0, }, GH_Sub[MAX_DATA_NUM] = { 0, };
	CPoint	cp;
	TCHAR	msg[64];
	RECT	rc;
	CFont	MyFont, * poldfont;

	MyFont.CreatePointFont(80, _T("MS PGothic"));
	poldfont = cdc->SelectObject(&MyFont);

	GH = (short)(m_rcGraphRect.bottom - m_rcGraphRect.top);
	GW = (short)(m_rcGraphRect.right - m_rcGraphRect.left);
	for (nIndex = 0; nIndex < m_nDataNum; nIndex++) {
		GH_Sub[nIndex] = (short)(m_rcSubGraphRect[nIndex].bottom - m_rcSubGraphRect[nIndex].top);
		GW_Sub[nIndex] = (short)(m_rcSubGraphRect[nIndex].right - m_rcSubGraphRect[nIndex].left);
	}

	cdc->SetBkMode(TRANSPARENT);

	/* 전체표시 */
	{
		if (!m_bIndividualDraw) {
			::_tcsnccpy_s(msg, _countof(msg), m_strVerScaleTitle.GetBuffer(0), _TRUNCATE);
			rc.left = m_rcGraphRect.left - 20;
			rc.top = m_rcGraphRect.top - (FONT_HEIGHT * 2) - 1;
			rc.right = rc.left + 100;
			rc.bottom = rc.top + 16;
			cdc->DrawText(msg, -1, &rc, DT_LEFT);
		}
	}

	/* 외곽선 */
	{
	//KANG 22.06.17 Add Start
		CBrush brush;
		brush.CreateSolidBrush(RGB(0, 0, 0));
		CBrush* pOldBrush = cdc->SelectObject(&brush);
	//KANG 22.06.17 Add End
		if (!m_bIndividualDraw) {
			rc.left = m_rcGraphRect.left - 1;
			rc.top = m_rcGraphRect.top - 10;
			rc.right = m_rcGraphRect.right + 1;
			rc.bottom = m_rcGraphRect.bottom + 10;
			//cdc->FillRect(&rc, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
		//KANG 22.06.17 Modify Start	 
		//	cdc->FillSolidRect(&rc, RGB(16, 16, 80));
			cdc->FrameRect(&rc, &brush);
			rc.left += 1;
			rc.right -= 1;
			rc.top += 1;
			rc.bottom -= 1;
			cdc->FillSolidRect(&rc, m_ColorBk);
		//KANG 22.06.17 Modify End
		}
		else {
			for (int i = 0; i < m_nDataNum; i++) {
				rc.left = m_rcSubGraphRect[i].left - 1;
				rc.top = m_rcSubGraphRect[i].top - 10;
				rc.right = m_rcSubGraphRect[i].right + 1;
				rc.bottom = m_rcSubGraphRect[i].bottom + 10;
			//KANG 22.06.17 Modify Start
			//	cdc->FillSolidRect(&rc, RGB(16, 16, 80));
				cdc->FrameRect(&rc, &brush);
				rc.left += 1;
				rc.right -= 1;
				rc.top += 1;
				rc.bottom -= 1;
				cdc->FillSolidRect(&rc, m_ColorBk);
			//KANG 22.06.17 Modify End
			}
		}
	//KANG 22.06.17 Add Start
		cdc->SelectObject(pOldBrush);
		brush.DeleteObject();
	//KANG 22.06.17 Add End
	}

	/* 세로축 눈금 */
//KANG 22.06.17 Modify Start	
//	{
	if (m_nVerMajorScaleInterval > 0) {
//KANG 22.06.17 Modify End
		int	i;
		CPen	pen, * oldpen = NULL;
		pen.CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
		oldpen = cdc->SelectObject(&pen);
		if (!m_bIndividualDraw) {
			for (i = 0; i < (m_nMaxVerSIze + 1); i += m_nVerMajorScaleInterval) {
				if (i > m_nMaxVerSIze) {
					i = m_nMaxVerSIze;
				}
				if (bVerticAxis) {
					cdc->MoveTo(m_rcGraphRect.left - 14, m_rcGraphRect.bottom - ((GH * i) / (m_nMaxVerSIze + 1)));
					cdc->LineTo(m_rcGraphRect.left - 4, m_rcGraphRect.bottom - ((GH * i) / (m_nMaxVerSIze + 1)));
				}
				if (m_bVerSubLine) {
					cdc->MoveTo(m_rcGraphRect.left + 3, m_rcGraphRect.bottom - ((GH * i) / (m_nMaxVerSIze + 1)));
					cdc->LineTo(m_rcGraphRect.right - 3, m_rcGraphRect.bottom - ((GH * i) / (m_nMaxVerSIze + 1)));
				}
			//KANG 22.06.17 Add Start
				if (m_bVerScaleLabel == TRUE) {
					::wsprintf(msg, _T("%5u"), i);
					// 22.12.12 Ahn Modify Start
					//rc.left = (LONG)((m_rcGraphRect.left - 14) - (_tcslen(msg) * 8)) - 3;
					rc.left = (LONG)((m_rcGraphRect.left) - (_tcslen(msg) * 8)) - 3;
					// 22.12.12 Ahn Modify End
					rc.top = m_rcGraphRect.bottom - ((GH * i) / (m_nMaxVerSIze + 1)) - 8;
					rc.right = m_rcGraphRect.left - 14 - 3;
					rc.bottom = rc.top + 16;
					cdc->DrawText(msg, -1, &rc, DT_LEFT | DT_NOCLIP);
				}
			//KANG 22.06.17 Add End
			}
		}
		else {
			for (nIndex = 0; nIndex < m_nDataNum; nIndex++) {
				for (i = 0; i < (m_nMaxVerSIze + 1); i += m_nVerMajorScaleInterval) {
					if (i > m_nMaxVerSIze) {
						i = m_nMaxVerSIze;
					}
					if (bVerticAxis) {
						cdc->MoveTo(m_rcSubGraphRect[nIndex].left - 14, m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * i) / (m_nMaxVerSIze + 1)));
						cdc->LineTo(m_rcSubGraphRect[nIndex].left - 4, m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * i) / (m_nMaxVerSIze + 1)));
					}
					if (m_bVerSubLine) {
						cdc->MoveTo(m_rcSubGraphRect[nIndex].left + 3, m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * i) / (m_nMaxVerSIze + 1)));
						cdc->LineTo(m_rcSubGraphRect[nIndex].right - 3, m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * i) / (m_nMaxVerSIze + 1)));
					}
				//KANG 22.06.17 Add Start
					if (m_bVerScaleLabel == TRUE) {
						::wsprintf(msg, _T("%5u"), i);
						rc.left = (LONG)((m_rcGraphRect.left - 14) - (_tcslen(msg) * 8)) - 3;
						rc.top = m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * i) / (m_nMaxVerSIze + 1)) - 8;
						rc.right = m_rcGraphRect.left - 14 - 3;
						rc.bottom = rc.top + 16;
						cdc->DrawText(msg, -1, &rc, DT_LEFT | DT_NOCLIP);
					}
				//KANG 22.06.17 Add End
				}
			}
		}
		cdc->SelectObject(oldpen);
		pen.DeleteObject();
	}

//KANG 22.06.14 Modify Start
//	if (bVerticAxis) {
	if (bVerticAxis == TRUE && m_nVerMinorScaleInterval > 0) {
//KANG 22.06.14 Modify End
		int	i;
		CPen	pen, * oldpen = NULL;
		pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		oldpen = cdc->SelectObject(&pen);
		if (!m_bIndividualDraw) {
			for (i = 0; i < (m_nMaxVerSIze + 1); i += m_nVerMinorScaleInterval) {
				cdc->MoveTo(m_rcGraphRect.left - 8, m_rcGraphRect.bottom - ((GH * i) / (m_nMaxVerSIze + 1)));
				cdc->LineTo(m_rcGraphRect.left - 4, m_rcGraphRect.bottom - ((GH * i) / (m_nMaxVerSIze + 1)));
			}
		}
		else {
			for (nIndex = 0; nIndex < m_nDataNum; nIndex++) {
				for (i = 0; i < (m_nMaxVerSIze + 1); i += m_nVerMinorScaleInterval) {
					cdc->MoveTo(m_rcSubGraphRect[nIndex].left - 8, m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * i) / (m_nMaxVerSIze + 1)));
					cdc->LineTo(m_rcSubGraphRect[nIndex].left - 4, m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * i) / (m_nMaxVerSIze + 1)));
				}
			}
		}
		cdc->SelectObject(oldpen);
		pen.DeleteObject();
	}

	/* 가로축 눈금 */
	{
		int	i;
		CPen	pen, * oldpen = NULL;
		::_tcsnccpy_s(msg, _countof(msg), m_strHorScaleTitle.GetBuffer(0), _TRUNCATE);
		rc.left = m_rcGraphRect.right + 24;
		rc.top = m_rcGraphRect.bottom + 16;
		rc.right = rc.left + 100;
		rc.bottom = rc.top + 24;
		cdc->DrawText(msg, -1, &rc, DT_LEFT);
		pen.CreatePen(PS_SOLID, 1, RGB(80, 80, 80));

		if (!m_bIndividualDraw) {
			for (i = 0; i <= (int)(grp_size / m_nHorScaleInterval); i++) {
				{
					int nInterval = i * m_nHorScaleInterval;
					// 22.12.13 Ahn Delete Start
					//if (nInterval > (int)(grp_size - 1)) {
					//	nInterval = grp_size - 1;
					//}
					// 22.12.13 Ahn Delete Start
					if (bHorizAxis) {
						cdc->MoveTo(m_rcGraphRect.left + ((GW * nInterval) / (grp_size - 1)), m_rcGraphRect.bottom + 12);
						cdc->LineTo(m_rcGraphRect.left + ((GW * nInterval) / (grp_size - 1)), m_rcGraphRect.bottom + 17);
						::wsprintf(msg, _T("%4u "), grp_start + nInterval + m_nHorStartNum);
						rc.left = (LONG)(m_rcGraphRect.left + ((GW * nInterval) / (grp_size - 1)) - (_tcslen(msg) * 4));
						rc.top = m_rcGraphRect.bottom + 18;
						rc.right = rc.left + 100;
						rc.bottom = rc.top + 16;
						cdc->DrawText(msg, -1, &rc, DT_LEFT | DT_NOCLIP);
					}
					if (m_bHorSubLine) {
						oldpen = cdc->SelectObject(&pen);
						cdc->MoveTo(m_rcGraphRect.left + ((GW * nInterval) / (grp_size - 1)), m_rcGraphRect.top + 3);
						cdc->LineTo(m_rcGraphRect.left + ((GW * nInterval) / (grp_size - 1)), m_rcGraphRect.bottom - 3);
						cdc->SelectObject(oldpen);
					}
				}
			}
		}
		else {
			for (nIndex = 0; nIndex < m_nDataNum; nIndex++) {
				for (i = 1; i <= (int)(grp_size / m_nHorScaleInterval) - 1; i++) {
					int nInterval = i * m_nHorScaleInterval;
					if (nInterval > (int)(grp_size - 1)) {
						nInterval = grp_size - 1;
					}
					if (bHorizAxis) {
						cdc->MoveTo(m_rcSubGraphRect[nIndex].left + ((GW_Sub[nIndex] * nInterval) / (grp_size - 1)), m_rcSubGraphRect[nIndex].bottom + 12);
						cdc->LineTo(m_rcSubGraphRect[nIndex].left + ((GW_Sub[nIndex] * nInterval) / (grp_size - 1)), m_rcSubGraphRect[nIndex].bottom + 17);

						::wsprintf(msg, _T("%4u "), grp_start + nInterval + m_nHorStartNum);
						rc.left = (LONG)(m_rcSubGraphRect[nIndex].left + ((GW_Sub[nIndex] * nInterval) / (grp_size - 1)) - (_tcslen(msg) * 4));
						rc.top = m_rcSubGraphRect[nIndex].bottom + 18;
						rc.right = rc.left + 100;
						rc.bottom = rc.top + 16;
						cdc->DrawText(msg, -1, &rc, DT_LEFT | DT_NOCLIP);
					}
					if (m_bHorSubLine) {
						oldpen = cdc->SelectObject(&pen);
						cdc->MoveTo(m_rcSubGraphRect[nIndex].left + ((GW_Sub[nIndex] * nInterval) / (grp_size - 1)), m_rcSubGraphRect[nIndex].top + 3);
						cdc->LineTo(m_rcSubGraphRect[nIndex].left + ((GW_Sub[nIndex] * nInterval) / (grp_size - 1)), m_rcSubGraphRect[nIndex].bottom - 3);
						cdc->SelectObject(oldpen);
					}
				}
			}

		}
		pen.DeleteObject();
	}

	/* 리미트 라인 표시 */
	{
		hpen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		oldpen = cdc->SelectObject(&hpen);
		if (!m_bIndividualDraw) {
			if (m_nThreshold1 > 0) {
				j = (short)(m_rcGraphRect.bottom - ((GH * m_nThreshold1) / (m_nMaxVerSIze + 1)));
				cdc->MoveTo(m_rcGraphRect.left, j);
				cdc->LineTo(m_rcGraphRect.right - 1, j);
			}
		}
		else {
			for (nIndex = 0; nIndex < m_nDataNum; nIndex++) {
				if (m_nThreshold1 > 0) {
					j = (short)(m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * m_nThreshold1) / (m_nMaxVerSIze + 1)));
					cdc->MoveTo(m_rcSubGraphRect[nIndex].left, j);
					cdc->LineTo(m_rcSubGraphRect[nIndex].right - 1, j);
				}
			}
		}
		cdc->SelectObject(oldpen);
		hpen.DeleteObject();
		hpen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
		oldpen = cdc->SelectObject(&hpen);
		if (!m_bIndividualDraw) {
			if (m_nThreshold2 > 0) {
				j = (short)(m_rcGraphRect.bottom - ((GH * m_nThreshold2) / (m_nMaxVerSIze + 1)));
				cdc->MoveTo(m_rcGraphRect.left, j);
				cdc->LineTo(m_rcGraphRect.right - 1, j);
			}
		}
		else {
			for (nIndex = 0; nIndex < m_nDataNum; nIndex++) {
				if (m_nThreshold2 > 0) {
					j = (short)(m_rcSubGraphRect[nIndex].bottom - ((GH_Sub[nIndex] * m_nThreshold2) / (m_nMaxVerSIze + 1)));
					cdc->MoveTo(m_rcSubGraphRect[nIndex].left, j);
					cdc->LineTo(m_rcSubGraphRect[nIndex].right - 1, j);
				}
			}
		}
		cdc->SelectObject(oldpen);
		hpen.DeleteObject();
	}

	DrawFtcGraphData(cdc);

	cdc->SelectObject(poldfont);
	MyFont.DeleteObject();

	return;
}

void CGraphCtrl::DrawFtcGraphData(CDC* cdc)
{
	CPen	hpen, *oldpen = NULL, hpenImage;
	CBrush	hbrshImage, *oldbrs = NULL;
//KANG 22.06.17 Modify Start
	//	BYTE*  pData = NULL;
	WORD* pData = NULL;
//KANG 22.06.17 Modify End

	UINT	i = 0;
	short	GW, GH, GW_Sub[MAX_DATA_NUM] = { 0, }, GH_Sub[MAX_DATA_NUM] = { 0, };
	POINT* pBuf = NULL;
	CPoint	cp;
//KANG 22.06.17 Add Start
	BOOL	bDrawShape = TRUE;
//KANG 22.06.17 Add End

	pBuf = new POINT[grp_size];
	GW = (short)(m_rcGraphRect.right - m_rcGraphRect.left);
	GH = (short)(m_rcGraphRect.bottom - m_rcGraphRect.top);
	for (int i = 0; i < m_nDataNum; i++) {
		GW_Sub[i] = (short)(m_rcSubGraphRect[i].right - m_rcSubGraphRect[i].left);
		GH_Sub[i] = (short)(m_rcSubGraphRect[i].bottom - m_rcSubGraphRect[i].top);
	}

	int	startbuf = grp_start;
	if (startbuf >= 0)
	{
		int* ptr = NULL;
		if (!m_bIndividualDraw) {
			if (m_bDrawDataTotal) {
				hpen.CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
				for (i = 0; i < grp_size; i++) {
					if ((i + startbuf) < (MAX_GRAPH_DATA_NUM)) {
						pBuf[i].x = m_rcGraphRect.left + ((GW * i) / (grp_size - 1));
						unsigned char tmpData = 0;
						for (int nNum = 0; nNum < m_nDataNum; nNum++) {
							pData = m_pMemo[nNum];
							if (m_bDrawData[nNum] && (pData != NULL)) {
							//KANG 22.06.17 Modify Start
							//	tmpData += (unsigned char)pData[(i + startbuf)];
								tmpData += (unsigned short)pData[(i + startbuf)];
							//KANG 22.06.17 Modify End
							}
						}
						pBuf[i].y = m_rcGraphRect.bottom - (GH * tmpData) / (m_nMaxVerSIze + 1);
					//KANG 22.06.17 Add Start
						if (m_bDrawPointZeroData == FALSE && tmpData == 0) {
							pBuf[i].y = 0;
						}
					//KANG 22.06.17 Add End
					}
				}
			//KANG 22.06.17 Modify Start
			//	cdc->Polyline(pBuf, i);
				if (m_bDrawDataLine == TRUE) {
					cdc->Polyline(pBuf, i);
				}
				if (m_bDrawDataPoint == TRUE) {
					DrawShape(cdc, pBuf, i, 10, RGB(180, 180, 180), en_Shape_Ellipse);
				}
			//KANG 22.06.17 Modify End
				cdc->Polyline(pBuf, i);
				cdc->SelectObject(oldpen);
				hpen.DeleteObject();
			}
			else {
				for( int nNum = 0 ; nNum < m_nDataNum ; nNum++ ) {
					pData = m_pMemo[nNum];
					hpen.CreatePen(PS_SOLID, 1, m_colorDataLine[nNum]);
					if (m_bDrawData[nNum] && (pData != NULL)) {
						for (i = 0; i < grp_size; i++) {
							if ((i + startbuf) < (MAX_GRAPH_DATA_NUM)) {
								pBuf[i].x = m_rcGraphRect.left + ((GW * i) / (grp_size - 1));
							//KANG 22.06.17 Modify Start
							//	pBuf[i].y = m_rcGraphRect.bottom - (GH * (unsigned char)pData[(i + startbuf)]) / (m_nMaxVerSIze + 1);
								pBuf[i].y = m_rcGraphRect.bottom - (GH * (unsigned short)pData[(i + startbuf)]) / (m_nMaxVerSIze + 1);
								if (m_bDrawPointZeroData == FALSE && (int)pData[(i + startbuf)] == 0) {
									pBuf[i].y = 0;
								}
							//KANG 22.06.17 Modify End
							}
						}
						oldpen = cdc->SelectObject(&hpen);
					//KANG 22.06.17 Modify Start
					//	cdc->Polyline(pBuf, i);
						if (m_bDrawDataLine == TRUE) {
							cdc->Polyline(pBuf, i);
						}
						if (m_bDrawDataPoint == TRUE) {
							DrawShape(cdc, pBuf, i, 5, m_colorDataPoint[nNum], m_nDataPointShape[nNum]);
						}
					//KANG 22.06.17 Modify End
					}
					cdc->SelectObject(oldpen);
					hpen.DeleteObject();
				}
			}
		}
		else {
			for (int nNum = 0; nNum < m_nDataNum; nNum++) {
				pData = m_pMemo[nNum];
				hpen.CreatePen(PS_SOLID, 1, m_colorDataLine[nNum]);
				if (m_bDrawData[nNum] && pData != NULL) {
					for (i = 0; i < grp_size; i++) {
						if ((i + startbuf) < (MAX_GRAPH_DATA_NUM)) {
							pBuf[i].x = m_rcSubGraphRect[nNum].left + ((GW_Sub[nNum] * i) / (grp_size - 1));
						//KANG 22.06.17 Modify Start
						//	pBuf[i].y = m_rcSubGraphRect[nNum].bottom - (GH_Sub[nNum] * pData[(i + startbuf)]) / (m_nMaxVerSIze + 1);
							pBuf[i].y = m_rcSubGraphRect[nNum].bottom - (GH_Sub[nNum] * (unsigned short)pData[(i + startbuf)]) / (m_nMaxVerSIze + 1);
							if (m_bDrawPointZeroData == FALSE && (int)pData[(i + startbuf)] == 0) {
								pBuf[i].y = 0;
							}
						//KANG 22.06.17 Modify End
						}
					}
					cdc->SelectObject(&hpen);
				//KANG 22.06.17 Modify Start
				//	cdc->Polyline(pBuf, i);
					if (m_bDrawDataLine == TRUE) {
						cdc->Polyline(pBuf, i);
					}
					if (m_bDrawDataPoint == TRUE) {
						DrawShape(cdc, pBuf, i, 10, m_colorDataPoint[nNum], m_nDataPointShape[nNum]);
					}
				//KANG 22.06.17 Modify End
				}
				cdc->SelectObject(oldpen);
				hpen.DeleteObject();
			}
		}
	}
	hpen.DeleteObject();

	delete[] pBuf;

	return;
}


void CGraphCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int	startbuf;
	startbuf = grp_start;
	SCROLLINFO	si;
	GetScrollInfo(SB_HORZ, &si);
	int	nMaxHor = si.nMax + 1;
	int	nStep = 1;

	switch (nSBCode) {
	case SB_LINEDOWN:
	case SB_PAGEDOWN:
		switch (nSBCode) {
		case SB_LINEDOWN:
			nStep = 1;
			break;
		case SB_PAGEDOWN:
			nStep = 10;
			break;
		default:
			return;
		}
		if (startbuf < (nMaxHor - (int)grp_size - nStep)) {
			startbuf += nStep;
		}
		else {
			startbuf = nMaxHor - (int)grp_size;
		}
		break;
	case SB_THUMBTRACK:
		if ((int)nPos > (nMaxHor - (int)grp_size)) {
			nPos = nMaxHor - (int)grp_size;
		}
		startbuf = nPos;
		break;
	case SB_LINEUP:
	case SB_PAGEUP:
		switch (nSBCode) {
		case SB_LINEUP:
			nStep = 1;
			break;
		case SB_PAGEUP:
			nStep = 10;
			break;
		default:
			return;
		}
		if (startbuf >= nStep) {
			startbuf -= nStep;
			break;
		}
		else {
			startbuf = 0;
		}
		break;
	default:
		return;
	}
	if (startbuf < 0) {
		startbuf = 0;
	}
	if (startbuf > nMaxHor) {
		startbuf = nMaxHor;
	}
//	grp_start = startbuf;
//	SetScrollPos(SB_HORZ, startbuf);
//	InvalidateRect(&m_rcDrawRect);
	OnRefresh(startbuf);
 
//	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CGraphCtrl::ScaleChange(int nScale)
{
//KANG 22.06.17 Modify Start
	//	CWnd::EnableScrollBarCtrl(SB_HORZ, TRUE);
	if (m_bShowHScrollbar == TRUE) {
		CWnd::EnableScrollBarCtrl(SB_HORZ, TRUE);
	}
//KANG 22.06.17 Modify End
	if (nScale <= 0) {
		nScale = 1;
	}

//	m_nHorScaleInterval = grp_size / 10;
	if (m_nHorScaleInterval <= 0) {
		m_nHorScaleInterval = 1;
	}

	grp_start = m_nImageSIze - grp_size;

	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	si.fMask = SIF_POS | SIF_PAGE;
	si.nPage = grp_size;
	si.nPos = 0;
	si.nMax = m_nImageSIze ;
	SetScrollInfo(SB_HORZ, &si, TRUE);
	grp_start = GetScrollPos(SB_HORZ);

	InvalidateRect(&m_rcDrawRect);

	return;
}

int CGraphCtrl::OnRefresh( int nScrollPos )
{
	// TODO: 여기에 구현 코드 추가.
	int nRet = 0;

	if (nScrollPos >= MAX_GRAPH_DATA_NUM) {
		return -1;
	}

	int nEndPos = m_nImageSIze - grp_size;
	if (nScrollPos > nEndPos) {
		nScrollPos = nEndPos;
	}

	int nPos = nScrollPos;
	if (m_bFixHScroll) {
		nPos = nScrollPos - grp_size;
	}
	if (nPos < 0) {
		nPos = 0;
	}
	grp_start = nPos;
	SetScrollPos(SB_HORZ, nPos);

	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	si.nPage = grp_size;
	si.nPos = nPos;
	si.nMax = m_nImageSIze;
	if (si.nMax < nScrollPos) {
		si.nMax = nScrollPos;
	}
	SetScrollInfo(SB_HORZ, &si, TRUE);

	CRect rcInvalidate;
	rcInvalidate.top = m_rcDrawRect.top;
	rcInvalidate.left = m_rcDrawRect.left;
	rcInvalidate.right = m_rcDrawRect.right;
	rcInvalidate.bottom = m_rcDrawRect.bottom + 35;
	InvalidateRect(&rcInvalidate);

	return nRet;
}
// 22.12.14 Ahn Add Start
void CGraphCtrl::SetScrollV()
{
	SCROLLINFO si;
	memset(&si, 0x00, sizeof(si));
	si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	si.nPage = grp_size ;
	si.nPos = grp_start ;
	si.nMin = 0;
	si.nMax = grp_size;

	SetScrollInfo(SB_HORZ, &si, TRUE);
}
// 22.12.14 Ahn Add End


void CGraphCtrl::DrawFrameRect(BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	m_bDrawFrameRect = bDraw;
}


int CGraphCtrl::SetData(int nDataIndex, int nCount, int nData)
{
	// TODO: 여기에 구현 코드 추가.
	int nRet = 0;
	if ((nDataIndex < 0) || (nDataIndex > m_nDataNum)
     || ( nCount < 0 ) || ( nCount > MAX_GRAPH_DATA_NUM ) ) {
		nRet = -1;
	}else {
		m_pMemo[nDataIndex][nCount] = nData;
	}
	//// 22.12.12 Ahn Add Start
	//{
	//	SCROLLINFO si;
	//	si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	//	si.nMin = 0;
	//	si.nMax = nCount - grp_size - 1 ;
	//	si.nPage = nCount - grp_size  ;
	//	si.nPos = nCount - grp_size - 1;
	//	SetScrollInfo(SB_HORZ, &si, FALSE);
	//}
	//// 22.12.12 Ahn Add Start
	return nRet;
}


int CGraphCtrl::DataClear()
{
	// TODO: 여기에 구현 코드 추가.
	for (int i = 0; i < m_nDataNum; i++) {
		if (m_pMemo[i] != NULL) {
		//KANG 22.06.17 Modify Start
		//	memset(m_pMemo[i], 0x00, (sizeof(int) * MAX_GRAPH_DATA_NUM));
			memset(m_pMemo[i], 0x00, (sizeof(WORD) * MAX_GRAPH_DATA_NUM));
		//KANG 22.06.17 Modify End	
		}
	}
	return 0;
}


void CGraphCtrl::SetGraphSize(int nSize)
{
	// TODO: 여기에 구현 코드 추가.
	grp_size = nSize;
}


void CGraphCtrl::SetThresHold1(int nThreshold)
{
	// TODO: 여기에 구현 코드 추가.
	m_nThreshold1 = nThreshold;
}

void CGraphCtrl::SetThresHold2(int nThreshold)
{
	// TODO: 여기에 구현 코드 추가.
	m_nThreshold2 = nThreshold;
}

void CGraphCtrl::SetImageSize(int nSize)
{
	// TODO: 여기에 구현 코드 추가.
	m_nImageSIze = nSize;
}


void CGraphCtrl::SetMaxVerScaleSize(int nSize)
{
	// TODO: 여기에 구현 코드 추가.
	m_nMaxVerSIze = nSize;
}


void CGraphCtrl::SetVerMajorScaleInterval(int nInterval)
{
	// TODO: 여기에 구현 코드 추가.
	if (nInterval <= 0) {
		nInterval = 1;
	}
	m_nVerMajorScaleInterval = nInterval;
}

void CGraphCtrl::SetVerMinorScaleInterval(int nInterval)
{
	// TODO: 여기에 구현 코드 추가.
	if (nInterval <= 0) {
		nInterval = 1;
	}
	m_nVerMinorScaleInterval = nInterval;
}


void CGraphCtrl::SetHorScaleInterval(int nInterval)
{
	// TODO: 여기에 구현 코드 추가.
	if (nInterval <= 0) {
		nInterval = 1;
	}
	m_nHorScaleInterval = nInterval;
}


void CGraphCtrl::SetHorScaleStartNum(int nNum)
{
	// TODO: 여기에 구현 코드 추가.
	m_nHorStartNum = nNum;
}


void CGraphCtrl::DrawVerAxis(BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	m_bVerAxis = bDraw;
}

void CGraphCtrl::DrawHorAxis(BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	m_bHorAxis = bDraw;
}


void CGraphCtrl::DrawVerSubLine(BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	m_bVerSubLine = bDraw;
}

void CGraphCtrl::DrawHorSubLine(BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	m_bHorSubLine = bDraw;
}


void CGraphCtrl::SetVerScaleTitle(LPCTSTR strTitle)
{
	// TODO: 여기에 구현 코드 추가.
	m_strVerScaleTitle = strTitle;
}

void CGraphCtrl::SetHorScaleTitle(LPCTSTR strTitle)
{
	// TODO: 여기에 구현 코드 추가.
	m_strHorScaleTitle = strTitle;
}

void CGraphCtrl::SetDrawData(int nDataIndex, BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	if (nDataIndex > m_nDataNum) {
		return;
	}
	m_bDrawData[nDataIndex] = bDraw;
}

void CGraphCtrl::SetDrawDataTotal(BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	m_bDrawDataTotal = bDraw;
}

BOOL CGraphCtrl::IsDrawData(int nDataIndex)
{
	return m_bDrawData[nDataIndex];
}

BOOL CGraphCtrl::IsDrawDataTotal()
{
	return m_bDrawDataTotal;
}

BOOL CGraphCtrl::IsDrawDataIndividual()
{
	return m_bIndividualDraw;
}

void CGraphCtrl::SetFixHScroll(BOOL bFix)
{
	// TODO: 여기에 구현 코드 추가.
	m_bFixHScroll = bFix;
}

void CGraphCtrl::SetDataColor(int nDataIndex, COLORREF color)
{
	// TODO: 여기에 구현 코드 추가.
	if (nDataIndex > m_nDataNum) {
		return;
	}
	m_colorDataLine[nDataIndex] = color;
//KANG 22.06.17 Add Start
	m_colorDataPoint[nDataIndex] = color;
//KANG 22.06.17 Add End
}

void CGraphCtrl::SetDrawIndividual(BOOL bDraw)
{
	// TODO: 여기에 구현 코드 추가.
	m_bIndividualDraw = bDraw;
}

//KANG 22.06.14 Add Start
void CGraphCtrl::SetVisibleHScollbar(BOOL bShow)
{
	// TODO: 여기에 구현 코드 추가.
	m_bShowHScrollbar = bShow;
}

void CGraphCtrl::SetBkColor(COLORREF bkColor)
{
	// TODO: 여기에 구현 코드 추가.
	m_ColorBk = bkColor;
}

void CGraphCtrl::DrawShape(CDC* cdc, const POINT* lpPoints, int nCount, int nSize, COLORREF color/* = RGB(0, 0, 0)*/, int nShape/* = en_Shape_Rectangle*/)
{
	// TODO: 여기에 구현 코드 추가.
	if (cdc == NULL || lpPoints == NULL || nCount <= 0) {
		return;
	}

	CPen	hpen, * oldpen = NULL;
	CBrush	hbrsh, * oldbrs = NULL;

	int	i = 0;

	hpen.CreatePen(PS_SOLID, 1, color);
	hbrsh.CreateSolidBrush(color);
	oldpen = cdc->SelectObject(&hpen);
	oldbrs = cdc->SelectObject(&hbrsh);
	for (i = 0; i < nCount; i++) {
		if (m_bDrawPointZeroData == FALSE && lpPoints[i].y == 0) {
			continue;
		}
		else {
			switch (nShape) {
			case en_Shape_Ellipse:
				cdc->Ellipse(lpPoints[i].x, lpPoints[i].y - nSize, lpPoints[i].x + nSize, lpPoints[i].y);
				break;
			case en_Shape_Triangle:
			{
				POINT pt[3];
				pt[0].x = lpPoints[i].x + (LONG)(nSize / 2);
				pt[0].y = lpPoints[i].y - nSize;
				pt[1].x = lpPoints[i].x;
				pt[1].y = lpPoints[i].y;
				pt[2].x = lpPoints[i].x + nSize;
				pt[2].y = lpPoints[i].y;
				cdc->Polygon(pt, 3);
			}
			break;
			case en_Shape_Rectangle:
			default:
				cdc->Rectangle(lpPoints[i].x, lpPoints[i].y - nSize, lpPoints[i].x + nSize, lpPoints[i].y);
				break;
			}
		}
	}
	cdc->SelectObject(oldpen);
	hpen.DeleteObject();
	cdc->SelectObject(oldbrs);
	hbrsh.DeleteObject();

	return;
}

void  CGraphCtrl::DrawDataLine(BOOL bDraw)
{
	m_bDrawDataLine = bDraw;
}

void  CGraphCtrl::DrawDataPoint(BOOL bDraw)
{
	m_bDrawDataPoint = bDraw;
}

int CGraphCtrl::GetData(int nDataIndex, int nCount)
{
	// TODO: 여기에 구현 코드 추가.
	int nRet = 0;
	if ((nDataIndex < 0) || (nDataIndex > m_nDataNum)
		|| (nCount < 0) || (nCount > MAX_GRAPH_DATA_NUM)) {
		nRet = -1;
	}
	else {
		nRet = m_pMemo[nDataIndex][nCount];
	}
	return nRet;
}

void  CGraphCtrl::DrawPointZeroData(BOOL bDraw)
{
	m_bDrawPointZeroData = bDraw;
}

void  CGraphCtrl::UseVerScaleLabel(BOOL bUse)
{
	m_bVerScaleLabel = bUse;
}

void  CGraphCtrl::SetVerScaleLableSpace(int nSpace)
{
	m_nVerScaleLabelSpace = nSpace;
}

void CGraphCtrl::SetDataLineColor(int nDataIndex, COLORREF color)
{
	// TODO: 여기에 구현 코드 추가.
	if (nDataIndex > m_nDataNum) {
		return;
	}
	m_colorDataLine[nDataIndex] = color;
}

void CGraphCtrl::SetDataPointColor(int nDataIndex, COLORREF color)
{
	// TODO: 여기에 구현 코드 추가.
	if (nDataIndex > m_nDataNum) {
		return;
	}
	m_colorDataPoint[nDataIndex] = color;
}

void CGraphCtrl::SetDataPointShape(int nDataIndex, int nShape)
{
	if (nDataIndex > m_nDataNum) {
		return;
	}
	m_nDataPointShape[nDataIndex] = nShape;
}
//KANG 22.06.14 Add End
