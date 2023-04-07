// CImageDispDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CImageDispDlg.h"
#include "afxdialogex.h"
#include "ImgClipedBmp.h"
#include "BitmapStd.h"
#include "CImageProcess.h"
#include "CImageProcSimDlg.h"
#include "GlobalData.h"

// CImageDispDlg 대화 상자

IMPLEMENT_DYNAMIC(CImageDispDlg, CDialogEx)

CImageDispDlg::CImageDispDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DISP_IMAGE_DLG, pParent)
{

	m_pParent = (CImageProcSimDlg*)pParent;
	m_EdRate = _T("");
	m_nZoomOutH = 1;
	m_nZoomOutV = 1;
	m_bDispDefectFlag = FALSE;
	m_bDispMaskFlag = FALSE;
	m_bMaskDrawFlag = FALSE;
//	m_bDispBaundary = FALSE;
	m_bDispRound = FALSE;
	m_bDispSelectedRect = FALSE;

	m_pBitmap = NULL;
	m_pBmpMeas = NULL;
	m_nScopeRate = 1;
	m_nDrawRate = 1;

	int	ImageSizeX = DEFAULT_IMG_SIZE_W;
	int	ImageSizeY = DEFAULT_IMG_SIZE_H;
	int	nBmpSizeX = (((ImageSizeX / m_nScopeRate) + 8 - 1) / 8) * 8;
	int	nBmpSizeY = (((ImageSizeY / m_nScopeRate) + 8 - 1) / 8) * 8;
	m_sizeImage.cx = nBmpSizeX;
	m_sizeImage.cy = nBmpSizeY;

	m_rcWnd.SetRectEmpty();
	m_rcCur.SetRectEmpty();
	m_rcDisp.SetRectEmpty();
	m_ptPrev.x = 0;
	m_ptPrev.y = 0;
	m_nThreshold = 128;
	m_pBmpDraw = new CImgClipedBmp();

	m_rcMask.left = 0;
	m_rcMask.right = 0;
	m_rcMask.top = 0;
	m_rcMask.bottom = 0;
	m_rcDragPos = m_rcMask;
	m_nDrawColorMode = 3;
	m_bDispBoundary = FALSE;

	m_bMeasureFlag = FALSE;
	m_nMeasureState = 0;

	m_cpLinePos[0].SetPoint(0, 0);
	m_cpLinePos[1].SetPoint(0, 0);
	m_dMeasureLength = 0.0;
	m_nMeasurePixLen = 0;

	{
		int i;
		for (i = 0; i < 256; i++) {
			m_btColorTable[i] = (BYTE)i;
		}
	}

//	m_pVecLeftRound = NULL;
//	m_pVecRightRound = NULL;
	m_bDispMeasureLine = TRUE;

}

CImageDispDlg::~CImageDispDlg()
{
	if (m_pBitmap != NULL) {
		delete	m_pBitmap;
		m_pBitmap = NULL;
	}
	if (m_pBmpDraw != NULL) {
		delete m_pBmpDraw;
		m_pBmpDraw = NULL;
	}
	if (m_pBmpMeas != NULL) {
		delete m_pBmpMeas;
		m_pBmpMeas = NULL;
	}
//	if (m_pVecLeftRound != NULL) {
//		delete m_pVecLeftRound;
//		m_pVecLeftRound = NULL;
//	}
//	if (m_pVecRightRound != NULL) {
//		delete m_pVecRightRound;
//		m_pVecRightRound = NULL;
//	}
}

void CImageDispDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_DISP_POS, m_stDispPos);
	DDX_Text(pDX, IDC_ED_RATE, m_EdRate);

}


BEGIN_MESSAGE_MAP(CImageDispDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CImageDispDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CImageDispDlg::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_ZOOM_IN, &CImageDispDlg::OnBnClickedBtnZoomIn)
	ON_BN_CLICKED(IDC_BTN_ZOOM_OUT, &CImageDispDlg::OnBnClickedBtnZoomOut)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CImageDispDlg 메시지 처리기


BOOL CImageDispDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	memset(&m_ScrollInfoH, 0x00, sizeof(m_ScrollInfoH));
	m_ScrollInfoH.cbSize = sizeof(SCROLLINFO);
	m_ScrollInfoH.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	m_ScrollInfoH.nMin = 0;
	m_ScrollInfoH.nMax = 0;
	m_ScrollInfoH.nPage = 0;
	m_ScrollInfoH.nPos = 0;
	SetScrollInfo(SB_HORZ, &m_ScrollInfoH, TRUE);

	memset(&m_ScrollInfoV, 0x00, sizeof(m_ScrollInfoV));
	m_ScrollInfoV.cbSize = sizeof(SCROLLINFO);
	m_ScrollInfoV.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	m_ScrollInfoV.nMin = 0;
	m_ScrollInfoV.nMax = 0;
	m_ScrollInfoV.nPage = 0;
	m_ScrollInfoV.nPos = 0;
	SetScrollInfo(SB_VERT, &m_ScrollInfoV, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CImageDispDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	// 긏깋귽귺깛긣쀌덃
	GetClientRect(&m_rcWnd);

	CRect rcWnd;
	CRect rcCtrl;
	CWnd* pWnd = NULL;

	pWnd = (CWnd*)GetDlgItem(IDC_ST_DISP_POS);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		pWnd->GetWindowRect(&rcCtrl);
		m_rcWnd.bottom -= rcCtrl.Height();
	}

	// ==========
	// 뿞됪뽋귩?뷈
	// ==========
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);
	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, m_rcWnd.Width(), m_rcWnd.Height());
	CBitmap* pOldBm = mdc.SelectObject(&bm);

	// 봶똧긪긞?깛긐
	{
		CBrush* poldBrush;
		CBrush myBrush;
		myBrush.CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 0));
		poldBrush = mdc.SelectObject(&myBrush);
		mdc.SetBkColor(RGB(64, 64, 64));
		mdc.FillRect(m_rcWnd, &myBrush);
		mdc.SelectObject(poldBrush);
		myBrush.DeleteObject();
	}

	rcWnd = m_rcWnd;

	// ==========
	// 뿞됪뽋귉?됪
	// ==========
	if (m_pBitmap != NULL) {
		if (m_rcWnd.Width() != 0) {
			m_ScrollInfoH.nPage = m_rcWnd.Width() / m_nDrawRate * m_nScopeRate;
			m_ScrollInfoH.nMin = m_ScrollInfoH.nPage / 2;
			m_ScrollInfoH.nMax = m_sizeImage.cx + m_ScrollInfoH.nMin;
			if (m_ScrollInfoH.nPos < m_ScrollInfoH.nMin) {
				m_ScrollInfoH.nPos = m_ScrollInfoH.nMin;
			}
			if (m_ScrollInfoH.nPos > (int)(m_ScrollInfoH.nMax - m_ScrollInfoH.nPage)) {
				m_ScrollInfoH.nPos = m_ScrollInfoH.nMax - m_ScrollInfoH.nPage;
				if (m_ScrollInfoH.nPos < m_ScrollInfoH.nMin) {
					m_ScrollInfoH.nPos = (int)(((double)m_ScrollInfoH.nMin + (double)m_ScrollInfoH.nMax - (double)m_ScrollInfoH.nPage) / (double)2.0);
				}
			}
			SetScrollInfo(SB_HORZ, &m_ScrollInfoH, TRUE);
		}
		if (m_rcWnd.Height() != 0) {
			m_ScrollInfoV.nPage = m_rcWnd.Height() / m_nDrawRate * m_nScopeRate;
			m_ScrollInfoV.nMin = m_ScrollInfoV.nPage / 2;
			m_ScrollInfoV.nMax = m_sizeImage.cy + m_ScrollInfoV.nMin;
			if (m_ScrollInfoV.nPos < m_ScrollInfoV.nMin) {
				m_ScrollInfoV.nPos = m_ScrollInfoV.nMin;
			}
			if (m_ScrollInfoV.nPos > (int)(m_ScrollInfoV.nMax - m_ScrollInfoV.nPage)) {
				m_ScrollInfoV.nPos = m_ScrollInfoV.nMax - m_ScrollInfoV.nPage;
				if (m_ScrollInfoV.nPos < m_ScrollInfoV.nMin) {
					m_ScrollInfoV.nPos = (int)(((double)m_ScrollInfoV.nMin + (double)m_ScrollInfoV.nMax - (double)m_ScrollInfoV.nPage) / (double)2.0);
				}
			}
			SetScrollInfo(SB_VERT, &m_ScrollInfoV, TRUE);
		}
		MakeDrawBitmap();
		if (m_pBmpDraw != NULL) {
			CBitmapStd* pBmpDest = m_pBmpDraw->GetBitmap();
			if (pBmpDest != NULL) {
				m_rcDisp.SetRectEmpty();
				m_rcDisp.right = m_pBmpDraw->GetValidXsize() * m_nDrawRate;
				m_rcDisp.bottom = m_pBmpDraw->GetValidYsize() * m_nDrawRate;
				// furuya 2010.07.07 01 st
				CRect rcImage;
				rcImage.SetRectEmpty();
				rcImage.right = m_pBmpDraw->GetValidXsize();
				rcImage.bottom = m_pBmpDraw->GetValidYsize();
				// furuya 2010.07.07 01 en
				CRect rcReal;	// 렳띧궸?됪궠귢귡됪뽋띆뷭
				rcReal.SetRectEmpty();
				rcReal.right = m_rcDisp.Width() / m_nScopeRate;
				rcReal.bottom = m_rcDisp.Height() / m_nScopeRate;
				if (m_rcCur.Width() > m_sizeImage.cx) {
					int dx = m_rcDisp.Width() / m_nScopeRate;
					int cx = rcWnd.CenterPoint().x;
					rcReal.left = cx - dx / 2;
					rcReal.right = cx + dx / 2;
				}
				if (m_rcCur.Height() > m_sizeImage.cy) {
					int dy = m_rcDisp.Height() / m_nScopeRate;
					int cy = rcWnd.CenterPoint().y;
					rcReal.top = cy - dy / 2;
					rcReal.bottom = cy + dy / 2;
				}
				// furuya 2010.07.07 01 st
					//pBmpDest->DrawImageFunction( &mdc, &rcReal, &m_rcDisp, SRCCOPY, FALSE ) ;
				pBmpDest->DrawImageFunction(&mdc, &rcReal, &rcImage, SRCCOPY, FALSE);
				// furuya 2010.07.07 01 en
			}
		}
	}

	DrawBoundaryLine(&mdc);
	DrawDefect(&mdc);

	DrawMeasureLine(&mdc);
	DrawArea(&mdc);

	DrawSelectRect(&mdc);
	DrawMask(&mdc);

	dc.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), &mdc, 0, 0, SRCCOPY);
	mdc.SelectObject(pOldBm);
	bm.DeleteObject();
	mdc.DeleteDC();

	CDialog::OnPaint();

}

void CImageDispDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcWnd;
	CRect rcCtrl;
	CRect rcCtrl2;
	CRect rcCtrl3;
	CRect rcCtrl4;
	GetClientRect(&rcWnd);
	int xpos = rcWnd.right;

	CWnd* pWnd = NULL;
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_ZOOM_OUT);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		pWnd->GetWindowRect(&rcCtrl);
		xpos -= rcCtrl.Width();
		pWnd->SetWindowPos(NULL
			, xpos
			, (rcWnd.bottom - rcCtrl.Height())
			, 0
			, 0
			, (SWP_NOSIZE | SWP_NOZORDER)
		);
		// furuya 2009.04.22 01 st
		pWnd->Invalidate();
		// furuya 2009.04.22 01 en
	}
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_ZOOM_IN);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		pWnd->GetWindowRect(&rcCtrl);
		xpos -= rcCtrl.Width();
		pWnd->SetWindowPos(NULL
			, xpos
			, (rcWnd.bottom - rcCtrl.Height())
			, 0
			, 0
			, (SWP_NOSIZE | SWP_NOZORDER)
		);
		// furuya 2009.04.22 01 st
		pWnd->Invalidate();
		// furuya 2009.04.22 01 en
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_RATE);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		pWnd->GetWindowRect(&rcCtrl);
		xpos -= rcCtrl.Width();
		pWnd->SetWindowPos(NULL
			, xpos
			, (rcWnd.bottom - rcCtrl.Height())
			, 0
			, 0
			, (SWP_NOSIZE | SWP_NOZORDER)
		);
		// furuya 2009.04.22 01 st
		pWnd->Invalidate();
		// furuya 2009.04.22 01 en
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_DISP_POS);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		pWnd->GetWindowRect(&rcCtrl2);
		CRect rc;
		rc.left = rcWnd.left;
		rc.top = rcWnd.bottom - rcCtrl2.Height();
		rc.right = xpos;
		rc.bottom = rcWnd.bottom;
		pWnd->MoveWindow(rc);
		// furuya 2009.04.22 01 st
		pWnd->Invalidate();
		// furuya 2009.04.22 01 en
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_DISPMEASRSLT);
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		pWnd->GetWindowRect(&rcCtrl2);
		CRect rc;
		rc.left = rcWnd.left;
		rc.top = rcWnd.bottom - rcCtrl2.Height();
		rc.right = xpos;
		rc.bottom = rcWnd.bottom;
		pWnd->MoveWindow(rc);
		// furuya 2009.04.22 01 st
		pWnd->Invalidate();
		// furuya 2009.04.22 01 en
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}




void CImageDispDlg::OnBnClickedBtnZoomIn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_nScopeRate == 1) {
		m_nDrawRate++;
	}
	else {
		m_nScopeRate--;
	}
	m_EdRate.Format(_T("%4.0lf%% "), (double)((double)m_nDrawRate / (double)m_nScopeRate) * 100);
	UpdateData(FALSE);

	CPoint center;
	center.x = m_ScrollInfoH.nPos;
	center.y = m_ScrollInfoV.nPos;
	OnPaint();

	Invalidate(FALSE);
}


void CImageDispDlg::OnBnClickedBtnZoomOut()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_nDrawRate == 1) {
		m_nScopeRate++;
	}
	else {
		m_nDrawRate--;
	}
	m_EdRate.Format(_T("%4.0lf%% "), (double)((double)m_nDrawRate / (double)m_nScopeRate) * 100);
	UpdateData(FALSE);

	CPoint center;
	center.x = m_ScrollInfoH.nPos;
	center.y = m_ScrollInfoV.nPos;
	OnPaint();
	m_ScrollInfoH.nPos = center.x;
	m_ScrollInfoV.nPos = center.y;
	Invalidate(FALSE);
}


void CImageDispDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nSBCode) {
	case	SB_BOTTOM:
		m_ScrollInfoV.nPos = m_ScrollInfoV.nMax;
		break;
	case	SB_ENDSCROLL:
		break;
	case	SB_LINEDOWN:
		m_ScrollInfoV.nPos += m_ScrollInfoV.nPage / 2;
		break;
	case	SB_LINEUP:
		m_ScrollInfoV.nPos -= m_ScrollInfoV.nPage / 2;
		break;
	case	SB_PAGEDOWN:
		m_ScrollInfoV.nPos += m_ScrollInfoV.nPage;
		break;
	case	SB_PAGEUP:
		m_ScrollInfoV.nPos -= m_ScrollInfoV.nPage;
		break;
	case	SB_THUMBPOSITION:
		m_ScrollInfoV.nPos = nPos;
		break;
	case	SB_THUMBTRACK:
		m_ScrollInfoV.nPos = nPos;
		break;
	case	SB_TOP:
		m_ScrollInfoV.nPos = m_ScrollInfoV.nMin;
		break;
	default:
		break;
	}

	if (m_ScrollInfoV.nPos < m_ScrollInfoV.nMin) {
		m_ScrollInfoV.nPos = m_ScrollInfoV.nMin;
	}
	else if (m_ScrollInfoV.nPos > (m_ScrollInfoV.nMax - (int)m_ScrollInfoV.nPage)) {
		m_ScrollInfoV.nPos = m_ScrollInfoV.nMax - m_ScrollInfoV.nPage;
	}
	SetScrollInfo(SB_VERT, &m_ScrollInfoV, TRUE);
	Invalidate(FALSE);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CImageDispDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nSBCode) {
	case	SB_RIGHT:
		m_ScrollInfoH.nPos = m_ScrollInfoH.nMax;
		break;
	case	SB_ENDSCROLL:
		break;
	case	SB_LINERIGHT:
		m_ScrollInfoH.nPos += m_ScrollInfoH.nPage / 2;
		break;
	case	SB_LINELEFT:
		m_ScrollInfoH.nPos -= m_ScrollInfoH.nPage / 2;
		break;
	case	SB_PAGERIGHT:
		m_ScrollInfoH.nPos += m_ScrollInfoH.nPage;
		break;
	case	SB_PAGELEFT:
		m_ScrollInfoH.nPos -= m_ScrollInfoH.nPage;
		break;
	case	SB_THUMBPOSITION:
		m_ScrollInfoH.nPos = nPos;
		break;
	case	SB_THUMBTRACK:
		m_ScrollInfoH.nPos = nPos;
		break;
	case	SB_LEFT:
		m_ScrollInfoH.nPos = m_ScrollInfoH.nMin;
		break;
	default:
		break;
	}

	if (m_ScrollInfoH.nPos < m_ScrollInfoH.nMin) {
		m_ScrollInfoH.nPos = m_ScrollInfoH.nMin;
	}
	else if (m_ScrollInfoH.nPos > (m_ScrollInfoH.nMax - (int)m_ScrollInfoH.nPage)) {
		m_ScrollInfoH.nPos = m_ScrollInfoH.nMax - m_ScrollInfoH.nPage;
	}
	SetScrollInfo(SB_HORZ, &m_ScrollInfoH, TRUE);
	Invalidate(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


int CImageDispDlg::MakeDrawBitmap(void)
{
	if (m_pBitmap == NULL) {
		return (-1);
	}
	int	nSize[2];
	memset(nSize, 0x00, sizeof(nSize));
	m_pBitmap->GetImgSize(nSize);
	int	maxsizex = nSize[0];
	int	maxsizey = nSize[1];

	m_rcCur.left = m_ScrollInfoH.nPos - (m_rcWnd.Width() / m_nDrawRate * m_nScopeRate) / 2;
	m_rcCur.top = m_ScrollInfoV.nPos - (m_rcWnd.Height() / m_nDrawRate * m_nScopeRate) / 2;
	m_rcCur.right = m_rcCur.left + (m_rcWnd.Width() / m_nDrawRate * m_nScopeRate);
	m_rcCur.bottom = m_rcCur.top + (m_rcWnd.Height() / m_nDrawRate * m_nScopeRate);

	CRect rcTmp = m_rcCur;

	if (rcTmp.left < 0) {
		rcTmp.left = 0;
	}
	if (rcTmp.top < 0) {
		rcTmp.top = 0;
	}
	if (rcTmp.right >= maxsizex) {
		rcTmp.right = maxsizex - 1;
	}
	if (rcTmp.bottom >= maxsizey) {
		rcTmp.bottom = maxsizey - 1;
	}

	int	xpitch, ypitch, xpixel, ypixel;
	xpitch = 1;
	ypitch = 1;
	xpixel = m_rcWnd.Width() / m_nDrawRate * m_nScopeRate;
	ypixel = m_rcWnd.Height() / m_nDrawRate * m_nScopeRate;
	if (xpixel > rcTmp.Width()) {
		xpixel = rcTmp.Width();
	}
	if (ypixel > rcTmp.Height()) {
		ypixel = rcTmp.Height();
	}

	if (xpixel <= 0) {
		xpixel = 1;
	}
	if (ypixel <= 0) {
		ypixel = 1;
	}

	CImgClipedBmp::_CLIPINFO	info;
	memset(&info, 0x00, sizeof(info));
	info.xs = rcTmp.left;
	info.ys = rcTmp.top;
	info.xpixel = xpixel;
	info.ypixel = ypixel;
	info.xpitch = xpitch;
	info.ypitch = ypitch;
	info.bitcount = m_pBitmap->GetBitCount();
	m_pBmpDraw->SetClipInfo(info);

	GetPickBitmap(*m_pBmpDraw);

	return (0);
}

void CImageDispDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	if (m_rcWnd.PtInRect(point) == 0) {
		return;
	}

	BOOL bImageDrag = FALSE;
	CPoint	pointImage(point);
	int nOffsetX = 0;
	int nOffsetY = 0;
	int	nBright = 0;
	double	dPosX = 0.0;
	double	dPosY = 0.0;

	// 럨뽰띆뷭귩롦벦
	pointImage.x = pointImage.x / m_nDrawRate * m_nScopeRate;
	pointImage.y = pointImage.y / m_nDrawRate * m_nScopeRate;
	nOffsetX = m_rcCur.left;
	nOffsetY = m_rcCur.top;
	pointImage.Offset(nOffsetX, nOffsetY);

	// 딶뱗귩롦벦
	if ((pointImage.x >= 0) && (pointImage.y >= 0)) {
		if (m_pBitmap != NULL) {
			nBright = m_pBitmap->GetColor(pointImage);
		}
	}

	// 딈붋띆뷭귩롦벦
	int xpos = pointImage.x;
	int ypos = pointImage.y;

	xpos *= m_nZoomOutH;
	ypos *= m_nZoomOutV;



	if ((nFlags & MK_CONTROL) && (m_bMaskDrawFlag == TRUE)) {
		m_rcDragPos.right = point.x / m_nDrawRate * m_nScopeRate;
		m_rcDragPos.bottom = point.y / m_nDrawRate * m_nScopeRate;

		int nOffsetX = m_rcCur.left;
		int nOffsetY = m_rcCur.top;
		m_rcDragPos.right += nOffsetX;
		m_rcDragPos.bottom += nOffsetY;
		m_bDispMaskFlag = TRUE;
		Invalidate(FALSE);
	}
	else if( (m_nMeasureState == en_Second_Point) && ( m_bMeasureFlag == TRUE ) ) {
		m_cpLinePos[en_Second_Point].x = point.x / m_nDrawRate * m_nScopeRate;
		m_cpLinePos[en_Second_Point].y = point.y / m_nDrawRate * m_nScopeRate;
		int nOffsetX = m_rcCur.left;
		int nOffsetY = m_rcCur.top;
		m_cpLinePos[en_Second_Point].Offset(nOffsetX, nOffsetY);
		int nDistX = m_cpLinePos[0].x - m_cpLinePos[1].x;
		int nDistY = m_cpLinePos[0].y - m_cpLinePos[1].y;
		double dDistX = (double)(nDistX) * AprData.m_System.m_dResolX[0] / 1000.0;
		double dDistY = (double)(nDistY) * AprData.m_System.m_dResolY / 1000.0;
		double dLength = sqrt(pow(dDistX, 2) + pow(dDistY, 2));
		m_dMeasureLength = dLength;
		m_nMeasurePixLen = (int)sqrt(pow(nDistX, 2) + pow(nDistY, 2));
		Invalidate(FALSE);

	}


	if (m_bMeasureFlag == TRUE) {
		m_stDispPos.Format(_T(" PosX[%d], PosY[%d], Birght[%d], \t Measure Length[%.3lfmm][%d Pix]"), xpos, ypos, nBright, m_dMeasureLength, m_nMeasurePixLen );
	}
	else {
		m_stDispPos.Format(_T(" PosX[%d], PosY[%d], Birght[%d]"), xpos, ypos, nBright);
	}
	UpdateData(FALSE);

	CDialog::OnMouseMove(nFlags, point);
}


int CImageDispDlg::GetPickBitmap(CImgClipedBmp& clipbmp)
{
	int smooth = 1;

	CImgClipedBmp::_CLIPINFO	info = clipbmp.GetClipInfo();
	int	xs, xe;
	int	ys, ye;
	int	bitcnt;
	int	xpitch, ypitch;
	int	v[2];
	int	v2[2];
	CBitmapStd* pBmpSrc = m_pBitmap;
	CBitmapStd* pBmpDest = clipbmp.GetBitmap();

	pBmpSrc->GetImgSize(v);
	pBmpDest->Clear();
	pBmpDest->GetImgSize(v2);

	xs = info.xs;
	ys = info.ys;
	xe = xs + (info.xpixel * info.xpitch) - 1;
	ye = ys + (info.ypixel * info.ypitch) - 1;
	xpitch = info.xpitch;
	ASSERT(xpitch > 0);
	ypitch = info.ypitch;
	ASSERT(ypitch > 0);
	bitcnt = info.bitcount;

	// check
	int	nOffsetX = 0;
	int	nOffsetY = 0;
	if (xs < 0) {
		nOffsetX = (0 - xs) / xpitch;
		xs = 0;
	}
	if (ys < 0) {
		nOffsetY = (0 - ys) / ypitch;
		ys = 0;
	}
	if (xe >= v[0]) {
		xe = v[0] - 1;
	}
	if (ye >= v[1]) {
		ye = v[1] - 1;
	}
	if (bitcnt != pBmpSrc->GetBitCount()) {
		ASSERT(FALSE);
		return (-1);
	}
	//// furuya 2014.07.08 01 st
	//if ((xpitch == 1) && (ypitch == 1)) {
	//	CAprImageProc img_proc;
	//	if (img_proc.ImageSmooth(pBmpSrc, pBmpDest, ys, ye, xs, xe, smooth) < 0) {
	//		SAVE_ERROR_TEXT("%d,%d,%d,%d,%d", ys, ye, xs, xe, smooth);
	//		return (-1);
	//	}
	//	return (0);
	//}
	//// furuya 2014.07.08 01 en
	int	nNumOfByteCount = bitcnt / 8;
	BYTE* ps = NULL;
	BYTE* pd = NULL;
	for (int y = ys, line = 0; y <= ye; y += ypitch, line++) {
		ps = pBmpSrc->GetLinePtr(y) + (xs * nNumOfByteCount);
		pd = pBmpDest->GetLinePtr(line + nOffsetY);
		pd += nOffsetX * nNumOfByteCount;
		if (xpitch == 1) {
			ASSERT((xe - xs + 1) >= 0);
			memcpy(pd, ps, (sizeof(BYTE) * (xe - xs + 1) * nNumOfByteCount));
		}
		else {
			if (bitcnt == 24) {
				for (int x = xs; x <= xe; x += xpitch) {
					*pd++ = *ps;
					*pd++ = *(ps + 1);
					*pd++ = *(ps + 2);
					ps += 3 * xpitch;
				}
			}
			else {
				for (int x = xs; x <= xe; x += xpitch) {
					*pd++ = *ps;
					ps += xpitch;
				}
			}
		}
		pBmpSrc->UnlockBitmapinfoPtr();
		pBmpDest->UnlockBitmapinfoPtr();
	}
	return (0);
}

int CImageDispDlg::SetBitmapStd(CBitmapStd* pBmp, BOOL bReverse)
{

	if (pBmp == NULL) {
		return (-1);
	}

	if (m_pBitmap != NULL) {
		delete	m_pBitmap;
		m_pBitmap = NULL;
	}

	int	nSize[2];
	memset(nSize, 0x00, sizeof(nSize));
	pBmp->GetImgSize(nSize);
	if (m_pBitmap == NULL) {
		m_pBitmap = new CBitmapStd(nSize[0], nSize[1]);
	}

	if (m_pBitmap != NULL) {
		m_pBitmap->SetBitmap(*pBmp);
		if (bReverse) {
		//	m_pBitmap->ImageCopy(1);
			m_pBitmap->SetImage(nSize[0], nSize[1], pBmp->GetImgPtr());
		}

	}
	else {
		return (-1);
	}

	CSize size = pBmp->GetImgSize();
	int nWidth = (int)size.cx;
	int nHeight = (int)size.cy;

	// +++kato 2009.11.18 st
	if (m_pBitmap != NULL) {
		int	nSize[2];
		memset(nSize, 0x00, sizeof(nSize));
		m_pBitmap->GetImgSize(nSize);
		m_sizeImage.cx = nSize[0];
		m_sizeImage.cy = nSize[1];
		m_nZoomOutH = (int)((double)nWidth / m_sizeImage.cx + 0.5);
		m_nZoomOutV = (int)((double)nHeight / m_sizeImage.cy + 0.5);

	}

	Invalidate(FALSE);

	return (0);
}

void CImageDispDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CDialog::OnOK();
}


void CImageDispDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CDialog::OnCancel();
}


void CImageDispDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if ((nFlags & MK_CONTROL) && (m_bMaskDrawFlag == FALSE)) {
		m_bMaskDrawFlag = TRUE;
		m_rcDragPos.left = point.x / m_nDrawRate * m_nScopeRate;
		m_rcDragPos.top = point.y / m_nDrawRate * m_nScopeRate;

		int nOffsetX = m_rcCur.left;
		int nOffsetY = m_rcCur.top;
		m_rcDragPos.OffsetRect(nOffsetX, nOffsetY);

	}else if (m_bMeasureFlag == TRUE) {
		switch (m_nMeasureState) {
		case	en_First_Point :
		case	en_Second_Point:
			{
				m_cpLinePos[m_nMeasureState].x = point.x / m_nDrawRate * m_nScopeRate;
				m_cpLinePos[m_nMeasureState].y = point.y / m_nDrawRate * m_nScopeRate;

				int nOffsetX = m_rcCur.left;
				int nOffsetY = m_rcCur.top;
				m_cpLinePos[m_nMeasureState].Offset(nOffsetX, nOffsetY);
				if (m_nMeasureState == en_First_Point) {
					m_cpLinePos[1].x = m_cpLinePos[0].x;
					m_cpLinePos[1].y = m_cpLinePos[0].y;
					m_dMeasureLength = 0.0;
					m_nMeasurePixLen = 0 ;
				}
				else {
					double dDistX = (double)(m_cpLinePos[0].x - m_cpLinePos[1].x ) * AprData.m_System.m_dResolX[ 0 ] / 1000.0 ;
					double dDistY = (double)(m_cpLinePos[0].y - m_cpLinePos[1].y) * AprData.m_System.m_dResolY / 1000.0 ;
					double dLength = sqrt(pow(dDistX, 2) + pow(dDistY, 2));
				//	m_dMeasureLength = ( dLength / m_nDrawRate * m_nScopeRate ) * m_nZoomOutH ;
					m_dMeasureLength = dLength;
				}
				m_bDispMeasureLine = TRUE;
				m_nMeasureState++;
				Invalidate(FALSE);
			}
			break;
		case	en_Measure_Point:
		case	en_Reset_Point :
			m_nMeasureState = en_First_Point;
			m_bDispMeasureLine = FALSE;
			Invalidate(FALSE);
			break;

		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void CImageDispDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if ((nFlags & MK_CONTROL) && (m_bMaskDrawFlag == TRUE)) {
		m_bMaskDrawFlag = FALSE;
		m_rcDragPos.right = point.x / m_nDrawRate * m_nScopeRate;
		m_rcDragPos.bottom = point.y / m_nDrawRate * m_nScopeRate;

		int nOffsetX = m_rcCur.left;
		int nOffsetY = m_rcCur.top;
		m_rcDragPos.right += nOffsetX;
		m_rcDragPos.bottom += nOffsetY;
		m_bDispMaskFlag = TRUE;
		Invalidate(FALSE);
	}


	CDialog::OnLButtonUp(nFlags, point);
}


BOOL CImageDispDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	UINT	msg;
	WPARAM	wpara;

	//if (PtInRect(m_rcWnd, pt) == 0) {
	//	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	//}

	if (nFlags == MK_CONTROL) {
		if (zDelta < 0) {
			OnBnClickedBtnZoomOut();
//			OnBnClickedBtnZoomIn();
		}
		else
		{
//			OnBnClickedBtnZoomOut();
			OnBnClickedBtnZoomIn();
		}
		return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}

	if (nFlags == MK_RBUTTON || nFlags == MK_SHIFT) {
		// -----덭??깛 or 긓깛긣깓?깑긌? or 긘긲긣긌?-----
			// 릣빟긚긏깓?깑
		msg = WM_HSCROLL;
	}
	else {
		// 릠뮳긚긏깓?깑
		msg = WM_VSCROLL;
	}

	if (zDelta < 0) {
		wpara = SB_LINEDOWN;
	}
	else {
		wpara = SB_LINEUP;
	}
	PostMessage(msg, wpara, NULL);


	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CImageDispDlg::PointMove(int nPosX, int nPosY)
{

	int nX, nY;
	nX = nPosX / m_nZoomOutH;
	nY = nPosY / m_nZoomOutV;
	m_ScrollInfoH.nPos = nX;
	m_ScrollInfoV.nPos = nY;
	InvalidateRect(FALSE);

}

void CImageDispDlg::DrawBoundaryLine(CDC* pDC)
{
	if (m_bDispBoundary == FALSE) return ;

	if (pDC == NULL) {
		return ;
	}

	if (m_pBmpDraw == NULL) {
		return ;
	}
	CBitmapStd* pBmpDest = m_pBmpDraw->GetBitmap();
	if (pBmpDest == NULL) {
		return ;
	}
	int	nRet = 0;
	CRect	rc;
	CPen	hpen, * hpenold = NULL;
	int	nBitCount = pBmpDest->GetBitCount();
	if (nBitCount == 24) {
		pDC->SetROP2(R2_COPYPEN);
		hpen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	}
	else {
		//	pDC->SetROP2(R2_XORPEN);
		hpen.CreatePen(PS_SOLID, 3
			, RGB(255, 50, 50));
	}
	hpenold = pDC->SelectObject(&hpen);
	GetClientRect(&rc);

	CSize size = pBmpDest->GetImgSize();

	CImageProcess::VEC_ROUND_INFO* pRoundInfo;
	for (int i = 0; i < 2; i++) {
		if (i == 0) {
			pRoundInfo = &m_VecLeftRound;
		}
		else {
			pRoundInfo = &m_VecRightRound;
		}
		int size = (int)pRoundInfo->size();

		if (pRoundInfo == NULL) continue;
		if (size <= 0) continue;

		CPoint pnt0 = (*pRoundInfo)[0];
		CPoint dispPnt;

		dispPnt.x = pnt0.x / m_nZoomOutH;
		dispPnt.y = pnt0.y / m_nZoomOutV;

		int nOffsetX = 0;
		int nOffsetY = 0;
		dispPnt.x -= m_rcCur.left;
		dispPnt.y -= m_rcCur.top;

		double rate = (double)m_nDrawRate / (double)m_nScopeRate;

		pnt0.x = (long)(dispPnt.x * rate);
		pnt0.y = (long)(dispPnt.y * rate);
		pDC->MoveTo(pnt0.x, pnt0.y);

		for (int j = 1; j < size; j++) {
			CPoint pnt = (*pRoundInfo)[j];

			dispPnt.x = pnt.x / m_nZoomOutH;
			dispPnt.y = pnt.y / m_nZoomOutV;

			int nOffsetX = 0;
			int nOffsetY = 0;
			dispPnt.x -= m_rcCur.left;
			dispPnt.y -= m_rcCur.top;

			double rate = (double)m_nDrawRate / (double)m_nScopeRate;

			pnt.x = (long)(dispPnt.x * rate);
			pnt.y = (long)(dispPnt.y * rate);
			pDC->LineTo(pnt.x, pnt.y);
		}
	}
	pDC->SelectObject(hpenold);
	hpen.DeleteObject();
}


void CImageDispDlg::SetBoundary(CImageProcess::VEC_ROUND_INFO* pVecLeft, CImageProcess::VEC_ROUND_INFO* pVecRight)
{
	ASSERT(pVecLeft);
//	ASSERT(pVecRight);

	m_VecLeftRound.clear();
	m_VecRightRound.clear();
	if(pVecRight != NULL){
		m_VecRightRound = *pVecRight;
	}

	m_VecLeftRound = *pVecLeft;

	Invalidate(FALSE);
}



void CImageDispDlg::SetDrawBoundaryFlag(BOOL bFlag)
{
	m_bDispBoundary = bFlag;
	Invalidate(FALSE);
}



int CImageDispDlg::DrawSelectRect(CDC* pDC) // 선택 결함 영역 표시
{
	if (m_bDispSelectedRect == FALSE) {
		return 0;
	}
	if (m_pBmpDraw == NULL) {
		return (-1);
	}
	CBitmapStd* pBmpDest = m_pBmpDraw->GetBitmap();
	if (pBmpDest == NULL) {
		return (-1);
	}
	int	nRet = 0;
	CRect	rc;
	CPen	hpen, * hpenold = NULL;
	int	nBitCount = pBmpDest->GetBitCount();
	if (nBitCount == 24) {
		pDC->SetROP2(R2_COPYPEN);
		hpen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	}
	else {
		//	pDC->SetROP2(R2_XORPEN);
		hpen.CreatePen(PS_SOLID, 3, RGB(255, 64, 64));
	}
	hpenold = pDC->SelectObject(&hpen);
	GetClientRect(&rc);

	{
		// 마스크 위치 계산 
		CRect rcDefect;
		if ((m_rectSelected.Width() > 0) && (m_rectSelected.Height())) {
			rcDefect.left = m_rectSelected.left / m_nZoomOutH;
			rcDefect.right = m_rectSelected.right / m_nZoomOutH;
			rcDefect.top = m_rectSelected.top / m_nZoomOutV;
			rcDefect.bottom = m_rectSelected.bottom / m_nZoomOutV;
			rcDefect.NormalizeRect();
		}

		if (rcDefect.IntersectRect(&rcDefect, &m_rcCur) == 0) {
			// -----뫮뤭궻?긚긏궼뙸띪?됪뭷궻붝댪궸궼궶궋-----
			return 0;
		}

		{
			// ?긚긏뗩?귩긏깋귽귺깛긣띆뷭궸빾듂궥귡
			int nOffsetX = 0;
			int nOffsetY = 0;
			nOffsetX -= m_rcCur.left;
			nOffsetY -= m_rcCur.top;
			rcDefect.OffsetRect(nOffsetX, nOffsetY);
			double rate = (double)m_nDrawRate / (double)m_nScopeRate;
			rcDefect.left = (long)(rcDefect.left * rate);
			rcDefect.right = (long)(rcDefect.right * rate);
			rcDefect.top = (long)(rcDefect.top * rate);
			rcDefect.bottom = (long)(rcDefect.bottom * rate);
		}

		pDC->MoveTo(rcDefect.left, rcDefect.top);
		pDC->LineTo(rcDefect.right, rcDefect.top);
		pDC->LineTo(rcDefect.right, rcDefect.bottom);
		pDC->LineTo(rcDefect.left, rcDefect.bottom);
		pDC->LineTo(rcDefect.left, rcDefect.top);
	}
	pDC->SelectObject(hpenold);
	hpen.DeleteObject();

	return 0;
}

void CImageDispDlg::DrawMask(CDC *pDC)
{
	if (m_bDispMaskFlag == FALSE) return ;

	if ((m_rcDragPos.Width() <= 0) || (m_rcDragPos.Height() <= 0))return ;

	if (pDC == NULL) {
		return ;
	}

	if (m_pBmpDraw == NULL) {
		return ;
	}
	CBitmapStd* pBmpDest = m_pBmpDraw->GetBitmap();
	if (pBmpDest == NULL) {
		return ;
	}
	int	nRet = 0;
	CRect	rc;
	CPen	hpen, * hpenold = NULL;
	int	nBitCount = pBmpDest->GetBitCount();
	if (nBitCount == 24) {
		pDC->SetROP2(R2_COPYPEN);
		hpen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	}
	else {
		pDC->SetROP2(R2_XORPEN);
		hpen.CreatePen(PS_SOLID, 1, RGB(64, 255, 64));
	}
	hpenold = pDC->SelectObject(&hpen);
	GetClientRect(&rc);


	{
		// 마스크 위치 계산 
		CRect rcMask;
		if ((m_rcDragPos.Width() > 0) && (m_rcDragPos.Height())) {
			rcMask.left = m_rcDragPos.left / m_nZoomOutH;
			rcMask.right = m_rcDragPos.right / m_nZoomOutH;
			rcMask.top = m_rcDragPos.top / m_nZoomOutV;
			rcMask.bottom = m_rcDragPos.bottom / m_nZoomOutV;
			rcMask.NormalizeRect();
		}

		if (rcMask.IntersectRect(&rcMask, &m_rcCur) == 0) {
			return ;
		}

		{
			int nOffsetX = 0;
			int nOffsetY = 0;
			nOffsetX -= m_rcCur.left;
			nOffsetY -= m_rcCur.top;
			rcMask.OffsetRect(nOffsetX, nOffsetY);
			double rate = (double)m_nDrawRate / (double)m_nScopeRate;
			rcMask.left = (long)(rcMask.left * rate);
			rcMask.right = (long)(rcMask.right * rate);
			rcMask.top = (long)(rcMask.top * rate);
			rcMask.bottom = (long)(rcMask.bottom * rate);
		}

		pDC->MoveTo(rcMask.left, rcMask.top);
		pDC->LineTo(rcMask.right, rcMask.top);
		pDC->LineTo(rcMask.right, rcMask.bottom);
		pDC->LineTo(rcMask.left, rcMask.bottom);
		pDC->LineTo(rcMask.left, rcMask.top);
	}
	pDC->SelectObject(hpenold);
	hpen.DeleteObject();
}


void CImageDispDlg::SetSelectDefRect(CRect rcDefect)
{
//	m_rectSelected = rcDefect;
//	m_bDispSelectedRect = TRUE;
	Invalidate(FALSE);
}


void CImageDispDlg::DrawDefect(CDC* pDC)
{
	CImageProcess::_VEC_BLOCK* pVecDefect = NULL ;
	pVecDefect = m_pParent->GetDefectData();

	if (pVecDefect == NULL) {
		return;
	}
	if (m_bDispDefectFlag == FALSE) return;

	int nSize = (int)pVecDefect->size();

	CBitmapStd* pBmpDest = m_pBmpDraw->GetBitmap();
	if (pBmpDest == NULL) {
		return ;
	}
	int	nRet = 0;
	CRect	rc;
//	CPen	hSelPen, *hselold = NULL ;
	int	nBitCount = pBmpDest->GetBitCount();


	GetClientRect(&rc) ;
	
	for (int i = 0; i < nSize; i++){
		if (pVecDefect->empty() == TRUE) break;
		CBlockData DefData;
		DefData = (*pVecDefect)[i];

		CPen	hpen, * hpenold = NULL;

		{
			// 마스크 위치 계산 
			CRect rcDefect;
			rcDefect.left = DefData.rcRect.left / m_nZoomOutH;
			rcDefect.right = ( DefData.rcRect.right + 1 )/ m_nZoomOutH;
			rcDefect.top = DefData.rcRect.top / m_nZoomOutV;
			rcDefect.bottom = ( DefData.rcRect.bottom + 1 )/ m_nZoomOutV;
			rcDefect.NormalizeRect();

			if (rcDefect.IntersectRect(&rcDefect, &m_rcCur) == 0) {
			//	return ;
			}

			if (m_nSelectDefNo == i) {
				hpen.CreatePen(PS_SOLID, 3, RGB(64, 64, 255));
			}
			else {
				if (DefData.nDefJudge == JUDGE_NG) {
					hpen.CreatePen(PS_SOLID, 3, RGB(255, 64, 64));
				}
				else {
					hpen.CreatePen(PS_SOLID, 3, RGB(64, 255, 64));
				}

			}
			hpenold = pDC->SelectObject(&hpen);

			int nExtSize = 5;
			int nOffsetX = 0;
			int nOffsetY = 0;
			nOffsetX -= m_rcCur.left;
			nOffsetY -= m_rcCur.top;
			rcDefect.OffsetRect(nOffsetX, nOffsetY);
			double rate = (double)m_nDrawRate / (double)m_nScopeRate;
			rcDefect.left = (long)(rcDefect.left * rate) - nExtSize;
			rcDefect.right = (long)(rcDefect.right * rate) + nExtSize;
			rcDefect.top = (long)(rcDefect.top * rate) - nExtSize ;
			rcDefect.bottom = (long)(rcDefect.bottom * rate) + nExtSize;

			pDC->MoveTo(rcDefect.left, rcDefect.top);
			pDC->LineTo(rcDefect.right, rcDefect.top);
			pDC->LineTo(rcDefect.right, rcDefect.bottom);
			pDC->LineTo(rcDefect.left, rcDefect.bottom);
			pDC->LineTo(rcDefect.left, rcDefect.top);

			hpenold = pDC->SelectObject(&hpen);
			hpen.DeleteObject();

		}
	}
}

void CImageDispDlg::DrawArea(CDC* pDC)
{
}
void CImageDispDlg::DrawMeasureLine(CDC* pDC)
{
	if (m_bMeasureFlag == FALSE) return;
	if (m_nMeasureState == en_Reset_Point) return;
	if (m_bDispMeasureLine == FALSE) return;
	

	if (pDC == NULL) {
		return ;
	}

	if (m_pBmpDraw == NULL) {
		return ;
	}
	CBitmapStd* pBmpDest = m_pBmpDraw->GetBitmap();
	if (pBmpDest == NULL) {
		return ;
	}
	int	nRet = 0;
	CRect	rc;
	CPen	hpen, * hpenold = NULL;
	int	nBitCount = pBmpDest->GetBitCount();
	if (nBitCount == 24) {
		pDC->SetROP2(R2_COPYPEN);
		hpen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	}
	else {
		pDC->SetROP2(R2_XORPEN);
		hpen.CreatePen(PS_SOLID, 1, RGB(64, 255, 64));
	}
	hpenold = pDC->SelectObject(&hpen);
	GetClientRect(&rc);

	CPoint cpLine[en_Reset_Point];

	{
		// 마스크 위치 계산 
		cpLine[en_First_Point].x = m_cpLinePos[en_First_Point].x / m_nZoomOutH ;
		cpLine[en_First_Point].y = m_cpLinePos[en_First_Point].y / m_nZoomOutV;
		cpLine[en_Second_Point].x = m_cpLinePos[en_Second_Point].x / m_nZoomOutH;
		cpLine[en_Second_Point].y = m_cpLinePos[en_Second_Point].y / m_nZoomOutV;

		int nOffsetX = 0;
		int nOffsetY = 0;
		nOffsetX -= m_rcCur.left;
		nOffsetY -= m_rcCur.top;
		cpLine[en_First_Point].Offset(nOffsetX, nOffsetY);
		cpLine[en_Second_Point].Offset(nOffsetX, nOffsetY);

		double rate = (double)m_nDrawRate / (double)m_nScopeRate;
		cpLine[en_First_Point].x = (long)(cpLine[en_First_Point].x * rate);
		cpLine[en_First_Point].y = (long)(cpLine[en_First_Point].y * rate);
		cpLine[en_Second_Point].x = (long)(cpLine[en_Second_Point].x * rate);
		cpLine[en_Second_Point].y = (long)(cpLine[en_Second_Point].y * rate);

		pDC->MoveTo(cpLine[en_First_Point].x, cpLine[en_First_Point].y);
		pDC->LineTo(cpLine[en_Second_Point].x, cpLine[en_Second_Point].y);

		//// 21.10.21 Ahn Add Start
		//double m;
		//double dy = (double)(m_cpLinePos[en_Second_Point].y - m_cpLinePos[en_First_Point].y);
		//double dx = (double)(m_cpLinePos[en_Second_Point].x - m_cpLinePos[en_First_Point].x);
		//CPoint cpCenter; 
		//int nCtx;
		//int nCty;
		//nCtx = cpCenter.x = (m_cpLinePos[en_Second_Point].x + m_cpLinePos[en_First_Point].x) / 2;
		//nCty = cpCenter.y = (m_cpLinePos[en_Second_Point].y + m_cpLinePos[en_First_Point].y) / 2;
		//m = dy / dx;
		//	

		//// y - y1 = m * ( x - x1 ) ; 
		//// y = ( m * ( x - x1 ) ) + y1
		//// 
		//double dX1 = m_cpLinePos[en_First_Point].x;
		//double dY1 = m_cpLinePos[en_First_Point].y;
		//double dInterceptY =  ( m * ( 0.0 - dX1 ) ) + dY1 ;

		//// y = ( m * x ) + n 
		//cpCenter.x = (long)(cpCenter.x / m_nZoomOutH) ;
		//cpCenter.y = (long)(cpCenter.y / m_nZoomOutV) ;
		//cpCenter.Offset(nOffsetX, nOffsetY);
		//cpCenter.x = (long)(cpCenter.x * rate);
		//cpCenter.y = (long)(cpCenter.y * rate);

		//double mPrime = (-1.0 / m);
		//double x = nCtx - 10;

		//double dItcptY2 = -nCtx * mPrime + nCty;
		//double y = mPrime * x + dItcptY2;

		//CPoint cpCross;
		//int nX = (int)(((x / m_nZoomOutH) - m_rcCur.left) * rate);
		//int nY = (int)(((y / m_nZoomOutV) - m_rcCur.top) * rate);

		//pDC->MoveTo(cpCenter.x, cpCenter.y);
		//pDC->LineTo(nX, nY);
		//// 21.10.21 Ahn Add End
	}
	pDC->SelectObject(hpenold);
	hpen.DeleteObject();
}

void CImageDispDlg::SetDispDefect(BOOL bDispFlag) 
{ 
	m_bDispDefectFlag = bDispFlag; 
	m_nSelectDefNo = -1;
	Invalidate(FALSE); 
};

void CImageDispDlg::SetSelDefectNo(int nSelDefNo)
{
	m_nSelectDefNo = nSelDefNo;
	Invalidate(FALSE);
}

void CImageDispDlg::SetMeasureMode(BOOL bFlag)
{
	m_bMeasureFlag = bFlag;
	if (m_bMeasureFlag == TRUE) {
		m_nMeasureState = en_First_Point;
	}
	else {
		m_nMeasureState = en_Reset_Point;
		Invalidate(FALSE);
	}
}


CRect CImageDispDlg::GetMaskRect() 
{
	CRect rcMaskRect;

	rcMaskRect = m_rcDragPos;

	return rcMaskRect;
}

