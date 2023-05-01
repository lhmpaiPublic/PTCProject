// CImageViewerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CImageViewerDlg.h"
#include "afxdialogex.h"
#include "Win32File.h"


// CImageViewerDlg 대화 상자

IMPLEMENT_DYNAMIC(CImageViewerDlg, CDialog)

CImageViewerDlg::CImageViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_IMAGE_VIEWER, pParent)
{
//KANG 22.05.24 Add Start
	m_rcImageRect.SetRectEmpty();
//KANG 22.05.24 Add End
}

CImageViewerDlg::~CImageViewerDlg()
{
}

void CImageViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImageViewerDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CImageViewerDlg 메시지 처리기


void CImageViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//KANG 22.05.24 Add Start
	CRect rcTmp;
	CStatic* pStatic = NULL;
	CButton* pButton = NULL;
	int nXMargin = 10;
	int nYMargin = 10;
	int nXPos = 0;
	int nYPos = 0;
	int nWidth = 0;
	int nHeight = 0;

	pButton = (CButton*)this->GetDlgItem(IDOK);
	if (pButton != nullptr) {

		pButton->GetClientRect(&rcTmp);
		nWidth = rcTmp.Width();
		nHeight = rcTmp.Height();
		nXPos = cx - rcTmp.Width() - nXMargin;
		nYPos = cy - rcTmp.Height() - nYMargin;
		pButton->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}

	pStatic = (CStatic*)this->GetDlgItem(IDC_ST_IMAGE);
	if (pStatic != nullptr) {
		pStatic->GetClientRect(&rcTmp);
		nWidth = cx - (nXMargin * 2);
		nHeight = nYPos - (nYMargin * 2);
		nXPos = nXMargin;
		nYPos = nYMargin;
		pStatic->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		m_rcImageRect.SetRect(nXPos, nYPos, nXPos + nWidth, nYPos + nHeight);
	}
//KANG 22.05.24 Add End
}


void CImageViewerDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

//KANG 22.05.24 Add Start
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

	if (m_strImageFileName.GetLength() > 0 && CWin32File::ExistFile(m_strImageFileName) == TRUE) {
		memset(wchar, 0, sizeof(WCHAR) * 512);
		size_t ReturnValue;
		mbstowcs_s(&ReturnValue, wchar, sizeof(wchar) / sizeof(WCHAR), m_strImageFileName.GetBuffer(0), _TRUNCATE);

		Image img(wchar);

		LONG imgWidth = img.GetWidth();
		LONG imgHeight = img.GetHeight();
		LONG rectWidth = m_rcImageRect.Width();
		LONG rectHeight = m_rcImageRect.Height();
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
		graphics.FillRectangle(&brush, m_rcImageRect.left, m_rcImageRect.top, m_rcImageRect.Width(), m_rcImageRect.Height());
		graphics.DrawImage(&img, Rect(m_rcImageRect.left + m_rcImageRect.Width() / 2 - drawWidth / 2, m_rcImageRect.top + m_rcImageRect.Height() / 2 - drawHeight / 2, drawWidth, drawHeight), 0, 0, img.GetWidth(), img.GetHeight(), UnitPixel, &ia);

		Pen fpen(Color(0, 0, 0));
		DWORD clrFrm = RGB(0, 0, 0);
		fpen.SetColor(Color((BYTE)clrFrm, (BYTE)(clrFrm >> 8), (BYTE)(clrFrm >> 16)));
		fpen.SetWidth(REAL(2.0));
		graphics.DrawRectangle(&fpen, m_rcImageRect.left, m_rcImageRect.top, m_rcImageRect.Width(), m_rcImageRect.Height());
	}
	else {
		DWORD color = RGB(0, 0, 0);
		SolidBrush brush(Color((BYTE)color, (BYTE)(color >> 8), (BYTE)(color >> 16)));
		graphics.FillRectangle(&brush, m_rcImageRect.left, m_rcImageRect.top, m_rcImageRect.Width(), m_rcImageRect.Height());

		StringFormat sFormat;
		graphics.SetTextContrast(Gdiplus::TextRenderingHintAntiAlias);
		Gdiplus::Font font(L"Arial", 100, FontStyleBold, UnitPixel);
		sFormat.SetAlignment(StringAlignmentCenter);
		sFormat.SetLineAlignment(StringAlignmentCenter);
		DWORD txtColor = RGB(255, 255, 255);
		SolidBrush txtBrush(Color((BYTE)txtColor, (BYTE)(txtColor >> 8), (BYTE)(txtColor >> 16)));
		RectF rectF((REAL)m_rcImageRect.left, (REAL)m_rcImageRect.top, (REAL)m_rcImageRect.Width(), (REAL)m_rcImageRect.Height());
		graphics.DrawString(L"No Image", -1, &font, rectF, &sFormat, &txtBrush);
	}
//KANG 22.05.24 Add End
}

//KANG 22.05.24 Add Start
int CImageViewerDlg::SetImageFile(LPCTSTR lpszImageFileName)
{
	// TODO: 여기에 구현 코드 추가.
	m_strImageFileName = lpszImageFileName;
	Invalidate();
	return 0;
}
//KANG 22.05.24 Add End