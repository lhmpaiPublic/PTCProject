// CDefectMapDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CDefectMapDlg.h"
#include "afxdialogex.h"
// 22.11.09 Ahn Add Start
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h" 
#include "MainFrm.h"	
#include "ImageProcessCtrl.h"
#include "GlobalData.h"
#include "SystemSetting.h"
//#include "QueueCtrl.h
// 22.11.09 Ahn Add End
// 22.12.15 Ahn Add Start
#include "CImgDisplayDlg.h"
// 22.12.15 Ahn Add End

// CDefectMapDlg 대화 상자

IMPLEMENT_DYNAMIC(CDefectMapDlg, CDialogEx)

CDefectMapDlg::CDefectMapDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*= nullptr*/)
	: CDialogEx(IDD_DEFECT_MAP_DLG, pParent)
{
	m_dMapLength = 2000.0 ;
	m_dMapWidth = 4000 * 0.006 ;

	//m_nDispStartY = 0 ; 
	//m_nDispLength = m_dMapLength;
	m_nDispStartY = 0;
	m_nDispLength = (int)m_dMapLength;
	m_nMapScrollMax = 2000 ;
	// 22.12.15 Ahn Add Start
	m_pImgDispDlg = NULL ;
	// 22.12.15 Ahn Add End
	// 22.12.27 Ahn Add Start
	m_pView = pView;
	// 22.12.27 Ahn Add End

}

CDefectMapDlg::~CDefectMapDlg()
{

	// 22.12.15 Ahn Add Start
	if (m_pImgDispDlg != NULL) {
		if (m_pImgDispDlg->m_hWnd != NULL) {
			m_pImgDispDlg->DestroyWindow();
		}

		delete m_pImgDispDlg ;
		m_pImgDispDlg = NULL ;
	}
	// 22.12.15 Ahn Add End

}

void CDefectMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_MAP_DISP_LENGTH, m_cmbMapDispLen);
}


BEGIN_MESSAGE_MAP(CDefectMapDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDefectMapDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDefectMapDlg::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_CBN_SELCHANGE(IDC_CMB_MAP_DISP_LENGTH, &CDefectMapDlg::OnCbnSelchangeCmbMapDispLength)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BTN_DISP_IMAGE, &CDefectMapDlg::OnBnClickedBtnDispImage)
END_MESSAGE_MAP()


// CDefectMapDlg 메시지 처리기


void CDefectMapDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDefectMapDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDefectMapDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	CBitmap bmp;
	CBitmap* pOldBmp;
	CDC dcm;

	bmp.CreateCompatibleBitmap(&dc, m_rcDraw.Width(), m_rcDraw.Height());
	dcm.CreateCompatibleDC(&dc);
	pOldBmp = dcm.SelectObject(&bmp);

	//
	DrawMap(&dcm, m_rcDraw);

	dc.BitBlt(m_rcDraw.left, m_rcDraw.top, m_rcDraw.Width(), m_rcDraw.Height(), &dcm, 0, 0, SRCCOPY);

	dcm.SelectObject(pOldBmp);
	bmp.DeleteObject();
	dcm.DeleteDC();
}


BOOL CDefectMapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_cmbMapDispLen.InsertString(0, _T("10"));
	m_cmbMapDispLen.InsertString(1, _T("20"));
	m_cmbMapDispLen.InsertString(2, _T("50"));
	m_cmbMapDispLen.InsertString(3, _T("100"));
	m_cmbMapDispLen.InsertString(4, _T("500"));
	m_cmbMapDispLen.InsertString(5, _T("1000"));
	m_cmbMapDispLen.InsertString(6, _T("All"));
	m_cmbMapDispLen.SetCurSel(6);

	{
		SCROLLINFO sriv;
		memset(&sriv, 0x00, sizeof(sriv));
		sriv.cbSize = sizeof(sriv);
		sriv.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
		sriv.nMin = 0;
		sriv.nMax = 2000;
		sriv.nPage = 2000;
		SetScrollInfo(SB_VERT, &sriv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDefectMapDlg::CalcMapRangeAndZoomRate()
{
	if (m_dZoomRate < 0.0) {
		m_dZoomRate = 1.0 ;
	}

	if (m_nDispStartY < 0) {
		m_nDispStartY = 0 ;
	}
}

void CDefectMapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rectControl, rectParent;
	CButton* pButton = NULL;

	int nWidth = 0, nHeight = 0;
	int nXPos = 0, nYPos = 0;
	GetWindowRect(rectParent);

	CWnd* pWnd;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr) {
		CPoint cpStartPos = pFrame->GetInspDIspPosition();
		CPoint cpMapSize = pFrame->GetInspMapSize() ;
		// 22.12.07 Ahn Add Start
		cpMapSize.Offset(0, -1*en_MapOffsetY);
		// 22.12.07 Ahn Add End

		pWnd = (CWnd*)GetDlgItem(IDC_ST_MAP_DISP);
		if (pWnd != nullptr) {
			nXPos = 0 ;
			// 22.12.07 Ahn Modify Start
			//nYPos = 0;
			nYPos = en_MapOffsetY ;
			// 22.12.07 Ahn Modify End
			nHeight = rectParent.Height();
			nWidth = rectParent.Width();
			pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
			m_rcDraw.SetRect(nXPos, nYPos, nXPos+ cpMapSize.x, nYPos + cpMapSize.y );
			m_nMargin = 15 ; 
			m_nPosSizeX = 40 ;
			m_nMapSizeX = ( m_rcDraw.Width() - ( m_nMargin * 3 ) - m_nPosSizeX ) / 2 ;
		}

		pWnd = (CWnd*)GetDlgItem(IDC_BTN_DISP_IMAGE);
		CRect rcBtn ;
		CRect rcMove;
		// GetWindowRect( )
		// GetClientRect(rcBtn)
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rcBtn);
			rcMove.top = 1;
			rcMove.bottom = rcMove.top + rcBtn.Height();
			rcMove.right = rectParent.right - 20;
			rcMove.left = rectParent.right - rcBtn.Width() - 20 ;
			pWnd->MoveWindow(rcMove);

			//pWnd->MoveWindow(rcBtn);
		}
	}
}


void CDefectMapDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_cpMouse = point;

	CDialogEx::OnMouseMove(nFlags, point) ;
}


CPoint CDefectMapDlg::CalcMapPosByRealPos(double dPosX, double dPosY, int nCamPos)
{
	CPoint cpRet;
	cpRet.SetPoint(0, 0);

	if ((nCamPos < 0) || (nCamPos >= MAX_CAMERA_NO)) {
		return cpRet ;
	}

	double dResoX ;
	double dResoY ;

	dResoX = (double)m_nMapSizeX / m_dMapWidth;
	dResoY = (double)m_nMapSizeY / (double)m_nDispLength ;

	cpRet.x = (int)( dPosX * dResoX ) + m_nMapStartPosX[nCamPos] ;
	// 22.12.07 Ahn Add Start
	//cpRet.y = (int)( ( dPosY - (double)m_nDispStartY) * dResoY ) + m_nMapStartPosY;
	cpRet.y = (int)((dPosY - (double)m_nDispStartY) * dResoY) + (m_nMapStartPosY - en_MapOffsetY);
	// 22.12.07 Ahn Add End
	return cpRet;
}

int CDefectMapDlg::CalcRealPosByMapPos(double dMapPosX, double dMapPosY, double& dPosX, double& dPosY, int& nCamPos)
{

	double dResoX;
	double dResoY;

	dResoX = m_dMapWidth / (double)m_nMapSizeX ;
	dResoY = (double)m_nDispLength / (double)m_nMapSizeY ;

	if ((dMapPosX < 0.0) || (dMapPosX < 0.0) || (dMapPosY > enMaxMapLen) ){
		return -1 ;
	}

	int nClickPosX;
	if (dMapPosX > m_nMapStartPosX[CAM_POS_TOP]) {
	// Top 
		nClickPosX = (int)dMapPosX - m_nMapStartPosX[CAM_POS_TOP];
		nCamPos = CAM_POS_TOP ;
	}
	else {
	// Top 
		nClickPosX = (int)dMapPosX - m_nMapStartPosX[CAM_POS_BOTTOM];
		nCamPos = CAM_POS_BOTTOM ;
	}
	dPosX = (double)nClickPosX * dResoX;
	// 23.02.02 Ahn Modify Start
	dPosY = (double)(dMapPosY - m_nMapStartPosY ) * dResoY + (double)m_nDispStartY ;
	//dPosY = (double)(dMapPosY - (m_nMapStartPosY + en_MapOffsetY)) * dResoY + (double)m_nDispStartY;
	// 23.02.02 Ahn Modify End
	return 0 ;
}

BOOL CDefectMapDlg::IsInMapRange(double dPosX, double dPosY)
{
	BOOL bRet = FALSE;

	if ((dPosX <= m_dMapWidth) && (dPosX >= 0.0) && (dPosY >= 0) && (dPosY <= m_dMapLength)) {
		bRet = TRUE ;
	}

	return bRet ;
}


void CDefectMapDlg::ChangeVScroll(UINT nSBCode, BOOL bSetScrollBar)
{
	switch (nSBCode) {
	case SB_LINEUP:
		m_nDispStartY -= (m_nDispLength / 10);
		break;
	case SB_LINEDOWN:
		m_nDispStartY += (m_nDispLength / 10);
		break;
	case SB_PAGEUP:
		m_nDispStartY -= m_nDispLength;
		break;
	case SB_PAGEDOWN:
		m_nDispStartY += m_nDispLength;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:	// スクロールボックスを指定位置へドラッグします（現在位置は引数 nPos で指定されます）
	{	//	CAprRollData::_ROLLINFO_ALL_PC* pRollInfoAll = theApp.GetRollData()->GetRollCurrent();

		SCROLLINFO scrollV;
		GetScrollInfo(SB_VERT, &scrollV);
		int pos = scrollV.nTrackPos;
		m_nDispStartY = pos;
	}

	break;
	default:
		return;
	}
	SetScrollV();
	Invalidate(FALSE);

}

void CDefectMapDlg::SetScrollV()
{
	//if (pView->m_ViewModeMain != VIEW_MODE_MAIN_NEW && m_iMapLength >= 10000 ||		// 表示長　10m以上 (過去)
	//	m_iMapLength >= 100000) {													// 表示長　100m以上 (最新)　滑らかに流れるようにする。
	//	m_iMapLength = ((int)((m_iMapLength + 99) / 1000)) * 1000;
	//	m_iStartLength = (m_iStartLength / 1000) * 1000;
	//	m_iMapScrollMax = ((int)((m_iMapScrollMax + 99) / 1000)) * 1000;
	//}
	//else if (m_iMapLength >= 1000) {	// 表示長　1m～10m
	//	m_iMapLength = ((int)((m_iMapLength + 99) / 100)) * 100;
	//	m_iStartLength = (m_iStartLength / 100) * 100;
	//	m_iMapScrollMax = ((int)((m_iMapScrollMax + 99) / 100)) * 100;
	//}
	//else {							// 表示長　1m未満
	//	m_iMapLength = ((int)((m_iMapLength + 9) / 10)) * 10;
	//	m_iStartLength = (m_iStartLength / 10) * 10;
	//	m_iMapScrollMax = ((int)((m_iMapScrollMax + 9) / 10)) * 10;
	//}

	if ( m_nDispLength < enMinMapLen) {
		m_nDispLength = enMinMapLen;
	}

	if (m_nMapScrollMax < m_nDispLength) {
		m_nMapScrollMax = m_nDispLength;
	}

	if (m_nDispStartY > m_nMapScrollMax - m_nDispLength) {
		m_nDispStartY = m_nMapScrollMax - m_nDispLength;
	}

	if (m_nDispStartY < 0) {
		m_nDispStartY = 0;
	}

	SCROLLINFO scrollV;

	memset(&scrollV, 0x00, sizeof(scrollV));
	scrollV.cbSize = sizeof(scrollV);
	scrollV.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	scrollV.nMin = 0;
	scrollV.nMax = m_nMapScrollMax;
	scrollV.nPage = m_nDispLength;
	scrollV.nPos = m_nDispStartY;

	SetScrollInfo(SB_VERT, &scrollV);
}

void CDefectMapDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	ChangeVScroll(nSBCode, FALSE );

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDefectMapDlg::OnCbnSelchangeCmbMapDispLength()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString tmp;
	int iMapLengthOld = m_nDispLength;
	int	curSel = m_cmbMapDispLen.GetCurSel();
	m_cmbMapDispLen.GetLBText(curSel, tmp);

	if (tmp.Find(_T("All")) != -1) {
	//	CAprRollData::_ROLLINFO_ALL_PC* pRollInfoAll = theApp.GetRollData()->GetRollCurrent();
		m_nDispLength = enMaxMapLen;// (int)(pRollInfoAll->dLength * m_dDispSheetFullRate);
	}
	else {
		double dTmp;
		dTmp = atof(tmp);
		m_nDispLength = (int)(dTmp);
		if (m_nDispLength < enMinMapLen) {
			m_nDispLength = enMinMapLen;
		}

		m_nDispStartY = (iMapLengthOld / 2) - (m_nDispLength / 2) + m_nDispStartY;
	}

	SetScrollV();
	Invalidate();
}

void CDefectMapDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	double dPosX = -1.0  ;
	double dPosY = -1.0 ;
	int nCamPos = -1 ;

	double dMapPosX = -1.0 , dMapPosY = -1.0 ;
	dMapPosX = point.x;
	// 23.02.02 Ahn Modify Start
	dMapPosY = point.y - en_MapOffsetY ;
	// 23.02.02 Ahn Modify End

	// 23.02.02 Ahn Modify Start
	if (CalcRealPosByMapPos(dMapPosX, dMapPosY, dPosX, dPosY, nCamPos) < 0) {
		return;
	}
	if (IsInMapRange(dPosX, dPosY) == FALSE) {
		return;
	}
	// 23.02.02 Ahn Modify End

	OnBnClickedBtnDispImage();

	// 22.12.27 Ahn Add Start
	if ( (m_pImgDispDlg != NULL ) && (m_pImgDispDlg->m_hWnd != NULL ) ) {
	// 22.12.27 Ahn Add End
	// 23.02.02 Ahn Delete End
	//	if (CalcRealPosByMapPos(dMapPosX, dMapPosY, dPosX, dPosY, nCamPos) >= 0) 
	// 23.02.02 Ahn Delete End
		{
			CDefectDataCtrl* pDefCtrl;
			pDefCtrl = theApp.m_pImgProcCtrl->GetDefectDataCtrlPtr();

			if (pDefCtrl == NULL) return;

			CDefectDataCtrl::_VEC_DEFECT_LIST::iterator iter;
			CDefectDataCtrl::_VEC_DEFECT_LIST* pDefList = pDefCtrl->GetDrawDefectList();

			CDefectDataCtrl::_VEC_DEFECT_LIST vecSelect;
			vecSelect.clear();

			double dMinDistY = 999999.9;
			BOOL bFind = FALSE;

			// 23.02.08 Ahn Modify Start
			//SetDefectList(dPosY);
			SetDefectList(dPosY, nCamPos);
			// 23.02.08 Ahn Modify End
		}
	}


	CDialogEx::OnLButtonDown(nFlags, point);
}

// 22.12.16 Ahn Add Start
void CDefectMapDlg::SetDefectList(double dPosY, int nCamPos )
{
	CDefectDataCtrl* pDefCtrl;
	pDefCtrl = theApp.m_pImgProcCtrl->GetDefectDataCtrlPtr();

	if (pDefCtrl == NULL) return;

	CDefectDataCtrl::_VEC_DEFECT_LIST::iterator iter;
	CDefectDataCtrl::_VEC_DEFECT_LIST* pDefList = pDefCtrl->GetDrawDefectList();

	CDefectDataCtrl::_VEC_DEFECT_LIST vecSelect;
	vecSelect.clear();

	double dMinDistY = 999999.9;
	BOOL bFind = FALSE;
	CDefectInfo* pDefInfo = NULL ;

	// 가장 가까운 결함 검색 후 
	double dNearPosY = dPosY;

	for (iter = pDefList->begin(); iter != pDefList->end(); iter++) {
		// 23.02.08 Ahn Add Start
		if (iter->nHeadNo != nCamPos) continue ;
		// 23.02.08 Ahn Add End
		double dDistY = abs(dPosY - (iter->dDefectPosY / 1000.0));
		if (dMinDistY > dDistY) {
			dNearPosY = (iter->dDefectPosY / 1000.0) ;
			dMinDistY = dDistY;
		}
	}

	for (iter = pDefList->begin(); iter != pDefList->end(); iter++) {
		double dDistY = abs(dNearPosY - ( iter->dDefectPosY / 1000.0 )) ;
		if ( dDistY <= 0.5) 
		{
			if (dMinDistY > dDistY) {
				pDefInfo = &(*iter) ;
			}
			vecSelect.push_back(*iter);
			bFind = TRUE;
		}
	}

	if (bFind == FALSE) {
		dMinDistY = 999999.9;
		for (iter = pDefList->begin(); iter != pDefList->end(); iter++) {
			double dDistY = abs(dPosY - (iter->dDefectPosY / 1000.0));
			if (dMinDistY > dDistY) {
				pDefInfo = &(*iter);
				bFind = TRUE;
			}
		}
	}

	int nSize = 0;
	if (bFind == TRUE) {
		nSize = (int)vecSelect.size();
		if (m_pImgDispDlg != NULL) {
			m_pImgDispDlg->SetDefectList(&vecSelect);
		}
	}

	if (pDefInfo != NULL) {
		m_pImgDispDlg->SelectDefectInfo(pDefInfo);
	}
}
// 22.12.16 Ahn Add End


BOOL CDefectMapDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (zDelta < 0) {
		ChangeVScroll(SB_LINEDOWN, TRUE /*bSetScrollBar*/);
	}
	else {
		ChangeVScroll(SB_LINEUP, TRUE /*bSetScrollBar*/);
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CDefectMapDlg::DrawInspLine(Graphics& graphics, double dBgnLength, double dLinePos, DWORD color, REAL fWidth /*= 2.0*/)
{
}


int	CDefectMapDlg::GetXGraphPos(double dPosX, double dBasePos)
{
	return 0;
}

int	CDefectMapDlg::GetYGraphPos(double dPosY, double dBasePos)
{
	return 0;
}

// 23.01.16 Ahn Add Start
void CDefectMapDlg::DrawTabAndRemark(CDC* pDC, CRect& WinRect)
{
	static WCHAR	wchr[100];
	int plotsize = 12;

	swprintf_s(wchr, L"Arial");

	Gdiplus::FontFamily		fontFamily(wchr);
	Gdiplus::Font			font(&fontFamily, 12, FontStyleRegular, UnitPixel);

	StringFormat format;
	format.SetAlignment(StringAlignmentFar);
	format.SetLineAlignment(StringAlignmentCenter);

	SolidBrush  solidBrushA(Color(255, 200, 200, 200));
	SolidBrush  solidBrushRed(Color(255, 255, 0, 0));
	SolidBrush	solidBrushGray(Color(80, 80, 80, 80));
	SolidBrush	solidBrushWhite(Color(200, 200, 200, 200));

	Graphics	graphics(pDC->m_hDC);

	int nWidth = WinRect.Width() ;
	int nHeight = WinRect.Height();

	int nCenter = WinRect.Width() / 2;
	m_nMapStartPosY = 200;

	CRect rcElectrode( 100, 20, nWidth - 80, 160 ) ;

	// Bottom Map Darw
	int nMapStartY = m_nMapStartPosY;
	graphics.FillRectangle(&solidBrushGray, rcElectrode.left, rcElectrode.top, rcElectrode.Width(), rcElectrode.Height() );

	CRect rcTab( rcElectrode.right , 60, rcElectrode.right + 40 , 120 ) ;
	graphics.FillRectangle(&solidBrushWhite, rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height());
	//// Top Map Draw 
	//graphics.FillRectangle(&brs, m_nMapStartPosX[CAM_POS_TOP], nMapStartY, m_nMapSizeX, m_nMapSizeY);

	// Focus Line 
	Pen penLine(Color(200, 100, 100), 2);
	penLine.SetDashStyle(DashStyleDash);
	Gdiplus::Point ptStart;
	Gdiplus::Point ptEnd;

	for (int i = 0; i < 2; i++) {
		CRect rcRect ;
		if (i == 0) {
			rcRect.left = rcElectrode.left - 20;
			rcRect.right = rcElectrode.left + 80;
			rcRect.top = rcElectrode.top - 5;
			rcRect.bottom = rcElectrode.bottom + 5;
		}
		else {
			rcRect.left = rcElectrode.right - 80;
			rcRect.right = rcElectrode.right + 20;
			rcRect.top = rcElectrode.top - 5;
			rcRect.bottom = rcElectrode.bottom + 5;
		}
		
		DrawShapeRectangle(graphics, &penLine, rcRect);

	}

	// Draw Remark 
	{
		Pen penRemark(Color(200, 200, 200), 1);
		penLine.SetDashStyle(DashStyleSolid);

		CPoint cpCenter = rcElectrode.CenterPoint();
		CRect rcRect;
		rcRect.left = cpCenter.x - 80;
		rcRect.right = cpCenter.x + 50;
		rcRect.top = rcElectrode.top + 10 ;
		rcRect.bottom = rcElectrode.bottom - 10 ;

		DrawShapeRectangle(graphics, &penRemark, rcRect);

		int nScalPosX = cpCenter.x + 20 ;

		Gdiplus::Color clrJudge;
		Gdiplus::Color clrRed(255, 255, 0, 0);
		Gdiplus::Color clrGreen(255, 0, 255, 0);
		Gdiplus::Color clrBlue(255, 0, 0, 255);
		Gdiplus::Color clrYellow(255, 255, 255, 0);

		WCHAR wchDefType[64];
		CString strDefType;

		{
			ptStart.Y = rcRect.top + 20;
			LPWSTR lpszW = wchDefType;

			strDefType = _T("Remark");
			LPTSTR lpStr = strDefType.GetBuffer(strDefType.GetLength());
			int nLen = MultiByteToWideChar(CP_ACP, 0, lpStr, -1, NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, lpStr, -1, lpszW, nLen);

			//WCHAR* wchDefType = T2W(strDefType.GetBuffer());
			size_t tSize = wcslen(wchDefType);
			PointF point((Gdiplus::REAL)nScalPosX, (Gdiplus::REAL)ptStart.Y);
			graphics.DrawString(wchDefType, (int)tSize, &font, point, &format, &solidBrushA);
		}

		for( int i = 0 ; i < en_defPosMax ; i++ ){
			switch (i) {
			case	en_defPosEdge:
				clrJudge = clrRed;
				strDefType.Format(_T("Electrod Edge"));
				break;
			case	en_defPosRound:
				clrJudge = clrBlue;
				strDefType.Format(_T("Round Cut"));
				break;
			case	en_defPosSurface:
				clrJudge = clrYellow;
				strDefType.Format(_T("On Surface"));
				break;
			default:
				strDefType.Format(_T(""));
				break;
			}

			ptStart.Y = rcRect.top + (i * 25) + 50 ;

			Pen pen(clrJudge);
			SolidBrush  brush(clrJudge);

			LPWSTR lpszW = wchDefType;

			LPTSTR lpStr = strDefType.GetBuffer(strDefType.GetLength());
			int nLen = MultiByteToWideChar(CP_ACP, 0, lpStr, -1, NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, lpStr, -1, lpszW, nLen);

			//WCHAR* wchDefType = T2W(strDefType.GetBuffer());
			size_t tSize = wcslen(wchDefType);
			PointF point((Gdiplus::REAL)nScalPosX, (Gdiplus::REAL)ptStart.Y);
			graphics.DrawString(wchDefType, (int)tSize, &font, point, &format, &solidBrushA);

			int width, height;
			CPoint cpTemp;
			cpTemp.x = (int)(point.X + 10 );
			cpTemp.y = (int)(point.Y - 2 );

			width = 10;
			height = 10;
			cpTemp.Offset(-5, -5);
			Gdiplus::RectF rcRect((REAL)cpTemp.x, (REAL)cpTemp.y, (REAL)width, (REAL)height);
			DrawShapeCircle(graphics, &pen, &brush, rcRect);

		}

	}
}
// 23.01.16 Ahn Add End


void CDefectMapDlg::DrawAxis(CDC* pDC, CRect& WinRect)
{
	static WCHAR	wchr[100];
	int plotsize = 12;

	swprintf_s(wchr, L"Arial");

	Gdiplus::FontFamily		fontFamily(wchr);
	Gdiplus::Font			font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	//GdiPuls::Font   fontmark(&fontFamily, (REAL)plotsize, FontStyleRegular, UnitPixel);

	StringFormat format;
	format.SetAlignment(StringAlignmentFar);
	format.SetLineAlignment(StringAlignmentCenter);

	SolidBrush  solidBrushA(Color(255, 200, 200, 200));
	SolidBrush  solidBrushRed(Color(255, 255, 0, 0));

	Graphics	graphics(pDC->m_hDC);

	int nWidth = (WinRect.Width() / 2) - 15;
	int nHeight = WinRect.Height();

	int nCenter = WinRect.Width() / 2;
	m_nMapStartPosY = 200 ;
	m_nMapStartPosX[CAM_POS_BOTTOM] = m_nMargin + m_nPosSizeX;
	m_nMapStartPosX[CAM_POS_TOP] = m_nMapStartPosX[CAM_POS_BOTTOM] + m_nMapSizeX + m_nMargin;
	// 22.12.07 Ahn Modify Start
	//m_nMapSizeY = nHeight - m_nMapStartPosY - 15 ;
	m_nMapSizeY = nHeight - m_nMapStartPosY - 15 ;
	// 22.12.07 Ahn Modify End

	// Bottom Map Darw
	int nMapStartY = m_nMapStartPosY;
	SolidBrush brs(Color(20, 20, 20));
	graphics.FillRectangle(&brs, m_nMapStartPosX[CAM_POS_BOTTOM], nMapStartY, m_nMapSizeX, m_nMapSizeY);

	// Top Map Draw 
	graphics.FillRectangle(&brs, m_nMapStartPosX[CAM_POS_TOP], nMapStartY, m_nMapSizeX, m_nMapSizeY);

	int nDivideY = 10;
	int nDivideX = 3;
	Pen penLine(Color(100, 100, 100), 1);
	penLine.SetDashStyle(DashStyleDash);
	Gdiplus::Point ptStart;
	Gdiplus::Point ptEnd;
	double dPosY;
	double dPosX;

	// Draw Line - Bottom 
	double		dInterval = (double)m_nMapSizeY / (double)nDivideY;

	WCHAR wchScale[32];

	int i;
	Gdiplus::REAL nScalPosX = (Gdiplus::REAL)m_nPosSizeX;

	CPoint ptLinePos;
	int nCamPos;
	for (i = 0; i <= nDivideY; i++) {
		ptStart.X = m_nMapStartPosX[CAM_POS_BOTTOM];
		ptEnd.X = ptStart.X + m_nMapSizeX;
		//dPosY = (double)i * nInterval ;

		double dMapPosX = (double)ptStart.X;
		double dMapPosY = (double)( m_nMapStartPosY ) + (dInterval * i);

		ptLinePos.x = m_nMapStartPosX[CAM_POS_BOTTOM];
		ptLinePos.y = (int)(dMapPosY);

		if (CalcRealPosByMapPos(dMapPosX, dMapPosY, dPosX, dPosY, nCamPos) < 0) {
			continue;
		}
		ptStart.Y = ptEnd.Y = ptLinePos.y;

		// BOTTOM
		graphics.DrawLine(&penLine, ptStart, ptEnd);

		swprintf_s(wchScale, L"%.1lf", dPosY);
		size_t tSize = wcslen(wchScale);
		PointF point(nScalPosX, (Gdiplus::REAL)ptStart.Y);
		graphics.DrawString(wchScale, (int)tSize, &font, point, &format, &solidBrushA);

		// TOP 
		ptStart.X = m_nMapStartPosX[CAM_POS_TOP];
		ptEnd.X = ptStart.X + m_nMapSizeX;
		graphics.DrawLine(&penLine, ptStart, ptEnd);
	}

	// 세로선
	double dIntervalY = m_dMapWidth / nDivideX;
	int nPos = 0;
	int nScalePosY = m_nMapStartPosY - 20;


	ptStart.Y = m_nMapStartPosY;
	ptEnd.Y = ptStart.Y + m_nMapSizeY;

	for (nPos = 0; nPos < MAX_CAMERA_NO; nPos++) {
		for (i = 0; i <= nDivideX; i++) {
			dPosX = (dIntervalY * i);
			CPoint cpTemp = CalcMapPosByRealPos(dPosX, 0, nPos);
			ptEnd.X = ptStart.X = cpTemp.x;

			graphics.DrawLine(&penLine, ptStart, ptEnd);

			swprintf_s(wchScale, L"%d", (int)dPosX);
			size_t tSize = wcslen(wchScale);
			PointF point((Gdiplus::REAL)ptStart.X, (Gdiplus::REAL)nScalePosY);
			graphics.DrawString(wchScale, (INT)tSize, &font, point, &format, &solidBrushA);
		}
	}
}

void CDefectMapDlg::DrawMap(CDC* pDC, CRect& WinRect)
{

	DrawAxis(pDC, WinRect);

	DrawDefect(pDC, WinRect);

	DrawTabAndRemark(pDC, WinRect);

}

void CDefectMapDlg::DrawDefect(CDC* pDC, CRect& WinRect)
{

	static WCHAR	wchr[100];
	int plotsize = 12;

	swprintf_s(wchr, L"Arial");

	Gdiplus::FontFamily		fontFamily(wchr);
	Gdiplus::Font			font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	//GdiPuls::Font   fontmark(&fontFamily, (REAL)plotsize, FontStyleRegular, UnitPixel);

	StringFormat format;
	format.SetAlignment(StringAlignmentFar);
	format.SetLineAlignment(StringAlignmentCenter);

	SolidBrush  solidBrushA(Color(255, 200, 200, 200));
	SolidBrush  solidBrushRed(Color(255, 255, 0, 0));

	Graphics	graphics(pDC->m_hDC);

	int nWidth = (WinRect.Width() / 2) - 15;
	int nHeight = WinRect.Height();

	int nCenter = WinRect.Width() / 2;
	// 22.12.07 Ahn Modify Start
	//m_nMapStartPosY = 200 ;
	m_nMapStartPosY = 200 + en_MapOffsetY ;
	// 22.12.07 Ahn Modify Start
	m_nMapStartPosX[CAM_POS_BOTTOM] = m_nMargin + m_nPosSizeX;
	m_nMapStartPosX[CAM_POS_TOP] = m_nMapStartPosX[CAM_POS_BOTTOM] + m_nMapSizeX + m_nMargin;
	m_nMapSizeY = nHeight - m_nMapStartPosY ;
	// 22.12.07 Ahn Modify End

	CDefectDataCtrl* pDefCtrl ;
	pDefCtrl = theApp.m_pImgProcCtrl->GetDefectDataCtrlPtr();

	if (pDefCtrl == NULL) return;

	CDefectDataCtrl::_VEC_DEFECT_LIST::iterator iter ;
	CDefectDataCtrl::_VEC_DEFECT_LIST* pDefList = pDefCtrl->GetDrawDefectList() ;

	int nSize = (int)pDefList->size() ;

	Gdiplus::Color clrRed(255, 255, 0, 0 );
	Gdiplus::Color clrGreen(255, 0, 255, 0);
	Gdiplus::Color clrBlue(255, 0, 0, 255);
	Gdiplus::Color clrYellow(255, 255, 255, 0);

	CPoint cpCamPosR = CalcMapPosByRealPos(24.0, 85.6, 0);

		//for (int i = 0; i < nSize; i++) {
	for(iter = pDefList->begin() ; iter != pDefList->end() ; iter++ ){
		// 위치 확인
		double dDefPosY = iter->dDefectPosY / 1000.0 ;
		if( (dDefPosY < (double)m_nDispStartY) || (dDefPosY > (double)(m_nDispStartY + m_nDispLength) ) ) {
			continue ;
		}

		int nCamPos = iter->nHeadNo;
		double dPosY = dDefPosY;
		CPoint cpPoint = iter->rcPos.CenterPoint() ;
		double dPosX = cpPoint.x * 6.0 / 1000.0;
		CPoint cpTemp = CalcMapPosByRealPos(dPosX, dPosY, nCamPos);

		Gdiplus::Color clrJudge;
		switch (iter->nDispPos) {
	//	switch( iter->nTabNo % 3 ){
		case	en_defPosEdge:
			clrJudge = clrRed;
			break;
		case	en_defPosRound :
			clrJudge = clrBlue;
			break;
		case	en_defPosSurface :
			clrJudge = clrYellow;
			break;
		default :
			break;
		}

		Pen pen(clrJudge);
		SolidBrush  brush(clrJudge);

		// Draw
		int width, height;
		width = 6;
		height = 6;
		cpTemp.Offset(-3, -3);

		Gdiplus::RectF rcRect( (REAL)cpTemp.x, (REAL)cpTemp.y, (REAL)width, (REAL)height ) ;

		DrawShapeCircle(graphics, &pen, &brush, rcRect);
	}
}

void CDefectMapDlg::DrawShapeCircle(Graphics& graphics, Pen* penRank, SolidBrush* brs, RectF rect)
{

	graphics.FillEllipse(brs, rect);
	graphics.DrawEllipse(penRank, rect);

}

void CDefectMapDlg::DrawShapeRectangle(Graphics& graphics, Pen* penRank, CRect rcRect)
{
	Gdiplus::Point ptStart;
	Gdiplus::Point ptEnd;

	for (int j = 0; j < 4; j++) {
		switch (j) {
		case	0:
			ptStart.X = rcRect.left;
			ptStart.Y = rcRect.top;
			ptEnd.X = rcRect.right;
			ptEnd.Y = rcRect.top;
			break;
		case	1:
			ptStart.X = rcRect.left;
			ptStart.Y = rcRect.top;
			ptEnd.X = rcRect.left;
			ptEnd.Y = rcRect.bottom;
			break;
		case	2:
			ptStart.X = rcRect.left;
			ptStart.Y = rcRect.bottom;
			ptEnd.X = rcRect.right;
			ptEnd.Y = rcRect.bottom;
			break;
		case	3:
			ptStart.X = rcRect.right;
			ptStart.Y = rcRect.top;
			ptEnd.X = rcRect.right;
			ptEnd.Y = rcRect.bottom;
			break;
		}

		graphics.DrawLine(penRank, ptStart, ptEnd);
	}
}
void CDefectMapDlg::DrawShapeTriangle(Graphics& graphics, Pen* penRank, SolidBrush* brs, RectF rect)
{

}

// 22.12.15 Ahn Add Start
void CDefectMapDlg::OnBnClickedBtnDispImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_pView->IsRun() == TRUE ) {
		return;
	}

	if (m_pImgDispDlg == NULL) {
		m_pImgDispDlg = new CImgDisplayDlg(this) ;
		m_pImgDispDlg->Create(IDD_DLG_DISP_IMAGE, this);

		CRect rectParent;
		GetWindowRect(rectParent);

		CRect rcRect;
		m_pImgDispDlg->GetWindowRect(rcRect);
		rcRect.OffsetRect(rectParent.Width(), 0);
		m_pImgDispDlg->MoveWindow(rcRect);

	}

	m_pImgDispDlg->ShowWindow(SW_SHOW);
}
// 22.12.15 Ahn Add End


// 22.12.27 Ahn Add Start
void CDefectMapDlg::CloseImgDispDlg()
{
	m_pImgDispDlg = NULL;
}

void CDefectMapDlg::RequestCloseImgDispDlg()
{
	if (m_pImgDispDlg != NULL) {
		m_pImgDispDlg->OnBnClickedOk();
	}
}
// 22.12.27 Ahn Add End
