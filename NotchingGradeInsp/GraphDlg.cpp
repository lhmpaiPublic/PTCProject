// GraphDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "GraphDlg.h"
#include "afxdialogex.h"
#include "CTabRsltInfo.h"
#include "CDefectDataCtrl.h"

// CGraphDlg 대화 상자

IMPLEMENT_DYNAMIC(CGraphDlg, CDialogEx)

CGraphDlg::CGraphDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_GRAPH, pParent)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = pView->GetDocument();
	}
	//m_pGraphCam2 = NULL;
	m_nCount = 0; //TEST

}

CGraphDlg::~CGraphDlg()
{
	//if (m_pGraphCam2 != NULL) {
	//	delete m_pGraphCam2;
	//	m_pGraphCam2 = NULL;
	//}
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_CAM1, m_cGraphCam1);
	DDX_Control(pDX, IDC_GRAPH_CAM2, m_cGraphCam2);
	DDX_Control(pDX, IDC_GRAPH_CAM3, m_cGraphCam3);
	DDX_Control(pDX, IDC_GRAPH_CAM4, m_cGraphCam4);

}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()

END_MESSAGE_MAP()


// CGraphDlg 메시지 처리기


BOOL CGraphDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cGraphCam1.DrawFrameRect(TRUE);
	m_cGraphCam1.SetDataColor(0, RGB(128, 255, 128));
	m_cGraphCam1.SetDataColor(1, RGB(128, 128, 255));
	m_cGraphCam1.SetDataColor(2, RGB(255, 128, 128));
	//m_cGraphCam1.SetMaxVerScaleSize(m_nMaxSize);

	m_cGraphCam2.DrawFrameRect(TRUE);
	m_cGraphCam2.SetDataColor(0, RGB(128, 255, 128));
	m_cGraphCam2.SetDataColor(1, RGB(128, 128, 255));
	m_cGraphCam2.SetDataColor(2, RGB(255, 128, 128));
	//m_cGraphCam2.SetMaxVerScaleSize(m_nMaxSize);

	m_cGraphCam3.DrawFrameRect(TRUE);
	m_cGraphCam3.SetDataColor(0, RGB(128, 255, 128));
	m_cGraphCam3.SetDataColor(1, RGB(128, 128, 255));
	m_cGraphCam3.SetDataColor(2, RGB(255, 128, 128));
	//m_cGraphCam3.SetMaxVerScaleSize(m_nMaxSize);

	m_cGraphCam4.DrawFrameRect(TRUE);
	m_cGraphCam4.SetDataColor(0, RGB(128, 255, 128));
	m_cGraphCam4.SetDataColor(1, RGB(128, 128, 255));
	m_cGraphCam4.SetDataColor(2, RGB(255, 128, 128));
	//m_cGraphCam4.SetMaxVerScaleSize(m_nMaxSize);


	m_cGraphCam1.DrawDataLine(FALSE);
	m_cGraphCam1.DrawDataPoint(TRUE);
	m_cGraphCam1.DrawPointZeroData(FALSE);

	m_cGraphCam2.DrawDataLine(FALSE);
	m_cGraphCam2.DrawDataPoint(TRUE);
	m_cGraphCam2.DrawPointZeroData(FALSE);

	m_cGraphCam3.DrawDataLine(FALSE);
	m_cGraphCam3.DrawDataPoint(TRUE);
	m_cGraphCam3.DrawPointZeroData(FALSE);

	m_cGraphCam4.DrawDataLine(FALSE);
	m_cGraphCam4.DrawDataPoint(TRUE);
	m_cGraphCam4.DrawPointZeroData(FALSE);

	//m_cGraphCam1.SetVerScaleLableSpace(40);
	//m_cGraphCam1.UseVerScaleLabel(TRUE);

	//if (m_pGraphCam2 == NULL) {
	//	m_pGraphCam2 = new CGraphCtrl(3);
	//	CRect rect = CRect(0,0,0,0);
	//	if (m_pGraphCam2->Create(rect, this, 102) == FALSE) {
	//		delete m_pGraphCam2;
	//		m_pGraphCam2 = NULL;
	//		return -1;
	//	}
	//}
	//m_pGraphCam2->SetDataColor(0, RGB(128, 255, 128));
	//m_pGraphCam2->SetDataColor(1, RGB(128, 128, 255));
	//m_pGraphCam2->SetDataColor(2, RGB(255, 128, 128));
	//m_pGraphCam2->SetMaxVerScaleSize(30);

	//// 22.06.20 Ahn Add Start
	//m_cGraphCam1.SetThresHold1(60);
	//m_cGraphCam2.SetThresHold1(60);
	//m_cGraphCam3.SetThresHold1(60);
	//m_cGraphCam4.SetThresHold1(60);
	//// 22.06.20 Ahn Add Start


	SetTimer(T_GRAPH_REFRESH, 100, NULL);	//TEST

	UpdateData(FALSE);
	OnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CGraphDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.


	if (m_cGraphCam1.GetSafeHwnd() != nullptr) {
		m_cGraphCam1.MoveWindow( 5, 5, ( cx  / 2 ) - 10 , ( cy / 2 ) - 20 );
		m_cGraphCam1.ScaleChange(10);
	}
	if (m_cGraphCam2.GetSafeHwnd() != nullptr) {
		m_cGraphCam2.MoveWindow( (cx / 2 ) , 5, (cx  / 2 ) - 10 , (cy / 2) - 20);
		m_cGraphCam2.ScaleChange(10);
	}
	if (m_cGraphCam3.GetSafeHwnd() != nullptr) {
		m_cGraphCam3.MoveWindow( 5, (cy / 2) + 5, (cx / 2) - 10, (cy / 2) - 20);
		m_cGraphCam3.ScaleChange(10);
	}
	if (m_cGraphCam4.GetSafeHwnd() != nullptr) {
		m_cGraphCam4.MoveWindow( (cx / 2), (cy / 2) + 5, (cx / 2) - 10, (cy / 2) - 20);
		m_cGraphCam4.ScaleChange(10);
	}


	CRect rectControl;
	CWnd* pWnd = NULL;
	int nWidth = 0, nHeight = 0;
	int nXPos = 0, nYPos = 0;

	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM1_INDIVIDUAL);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = 10;
		nYPos = (cy / 2) - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM1_DATA_TOTAL);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - (nWidth * 4) - 10;
		nYPos = (cy / 2) - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM1_DATA1);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - ( nWidth * 3 ) - 10 ;
		nYPos = (cy / 2) - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM1_DATA2);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - (nWidth * 2) - 10;
		nYPos = (cy / 2) - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM1_DATA3);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - (nWidth * 1) - 10;
		nYPos = (cy / 2) - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM2_INDIVIDUAL);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = 10;
		nYPos = cy - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM2_DATA_TOTAL);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - (nWidth * 4) - 10;
		nYPos = cy - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM2_DATA1);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - (nWidth * 3) - 10;
		nYPos = cy - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM2_DATA2);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - (nWidth * 2) - 10;
		nYPos = cy - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_CAM2_DATA3);
	if (pWnd != nullptr) {
		pWnd->GetWindowRect(rectControl);
		nWidth = rectControl.Width();
		nHeight = rectControl.Height();
		nXPos = cx - (nWidth * 1) - 10;
		nYPos = cy - 40;
		pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
	}
	Invalidate(FALSE);
}


void CGraphDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 22.06.23 Delete Start
	//if (nIDEvent == T_GRAPH_REFRESH) {
	//	KillTimer(T_GRAPH_REFRESH);

	//	if (m_pDoc->GetInspState() == enInspRun) {
	//		CDefectDataCtrl* pDefCtrl;
	//		pDefCtrl = m_pDoc->GetErrorDataCtrl();
	//		if (pDefCtrl != nullptr) {
	//			CTabRsltInfo* pTabInfo;
	//			int nSize = pDefCtrl->GetSize();
	//			if (nSize > m_nCount) {
	//				pTabInfo = pDefCtrl->GetTabRsltInfoPtr(m_nCount);

	//				if (pTabInfo != nullptr) {
	//					int nTabNo = pTabInfo->m_nTabNo;
	//					//if (pTabInfo->m_nHeadNo == CAM_POS_TOP) {
	//					//	m_cGraphCam1.SetFixHScroll(TRUE);
	//					//	m_cGraphCam1.SetData(0, nTabNo, (int)pTabInfo->m_nDefectMaxSize[CAM_POS_TOP][TYPE_FOILEXP] );
	//					//	m_cGraphCam1.SetDataPointShape(nTabNo, CGraphCtrl::en_Shape_Ellipse);
	//					//	//m_cGraphCam1.SetDataPointShape(nTabNo + MAX_DEFECT_IN_TAB_NUM, CGraphCtrl::en_Shape_Ellipse);
	//					//	m_cGraphCam1.OnRefresh(nTabNo);

	//					//	m_cGraphCam2.SetFixHScroll(TRUE);
	//					//	m_cGraphCam2.SetData(0, nTabNo, (int)pTabInfo->m_nDefectMaxSize[CAM_POS_TOP][TYPE_SURFACE] );
	//					//	m_cGraphCam2.SetDataPointShape(nTabNo, CGraphCtrl::en_Shape_Ellipse);
	//					//	//m_cGraphCam2.SetDataPointShape(nTabNo + MAX_DEFECT_IN_TAB_NUM, CGraphCtrl::en_Shape_Ellipse);
	//					//	m_cGraphCam2.OnRefresh(nTabNo);
	//					//}
	//					//else {
	//					//	m_cGraphCam3.SetFixHScroll(TRUE);
	//					//	m_cGraphCam3.SetData(0, nTabNo, (int)pTabInfo->m_nDefectMaxSize[CAM_POS_BOTTOM][TYPE_FOILEXP]);
	//					//	m_cGraphCam3.SetDataPointShape(nTabNo, CGraphCtrl::en_Shape_Ellipse);
	//					//	//m_cGraphCam3.SetDataPointShape(nTabNo + MAX_DEFECT_IN_TAB_NUM, CGraphCtrl::en_Shape_Ellipse);
	//					//	m_cGraphCam3.OnRefresh(nTabNo);

	//					//	m_cGraphCam4.SetFixHScroll(TRUE);
	//					//	m_cGraphCam4.SetData(0, nTabNo, (int)pTabInfo->m_nDefectMaxSize[CAM_POS_BOTTOM][TYPE_SURFACE]);
	//					//	m_cGraphCam4.SetDataPointShape(nTabNo, CGraphCtrl::en_Shape_Ellipse);
	//					//	//m_cGraphCam4.SetDataPointShape(nTabNo + MAX_DEFECT_IN_TAB_NUM, CGraphCtrl::en_Shape_Ellipse);
	//					//	m_cGraphCam4.OnRefresh(nTabNo);
	//					//}
	//					//else {
	//					//	m_pGraphCam2->SetFixHScroll(TRUE);
	//					//	m_pGraphCam2->SetData(RANK_OK, nTabNo, pTabInfo->m_nCount[TYPE_FOILEXP_OUT][RANK_OK]);
	//					//	m_pGraphCam2->SetData(RANK_NG, nTabNo, pTabInfo->m_nCount[TYPE_FOILEXP_OUT][RANK_NG]);
	//					//	m_pGraphCam2->OnRefresh(nTabNo);
	//					//}
	//					m_nCount++;
	//				}
	//			}
	//		}
	//	}
	//	else {
	//		m_cGraphCam1.SetFixHScroll(FALSE);
	//		m_cGraphCam2.SetFixHScroll(FALSE);
	//		m_cGraphCam3.SetFixHScroll(FALSE);
	//		m_cGraphCam4.SetFixHScroll(FALSE);
	//		//	m_pGraphCam2->SetFixHScroll(FALSE);
	//	}

	//	if (m_nCount < MAX_GRAPH_DATA_NUM) {
	//		SetTimer(T_GRAPH_REFRESH, 100, NULL);
	//	}
	//	return;
	//}
	// 22.06.23 Delete End

	CDialogEx::OnTimer(nIDEvent);
}


void CGraphDlg::OnRefresh()
{
	// TODO: 여기에 구현 코드 추가.
	UpdateData(TRUE);


	UpdateData(FALSE);
}

void CGraphDlg::DataClearAll()
{
	// 22.03.25 Ahn Add Start
	m_nCount = 0;
	// 22.03.25 Ahn Add End
	m_cGraphCam1.DataClear();
	m_cGraphCam2.DataClear();
	m_cGraphCam3.DataClear();
	m_cGraphCam4.DataClear();
	//	m_pGraphCam2->DataClear();

	OnRefresh();
}

// 22.06.20 Ahn Add Start
void CGraphDlg::SetMaxVerSize( int nMode, int nSize)
{
	m_nMaxSize = nSize;
	if ( nMode == en_FoilExpGraph) {
		m_cGraphCam1.SetMaxVerScaleSize(nSize);
		m_cGraphCam2.SetMaxVerScaleSize(nSize);
	}
	else {
		m_cGraphCam3.SetMaxVerScaleSize(nSize);
		m_cGraphCam4.SetMaxVerScaleSize(nSize);
	}
}

void CGraphDlg::SetThreshold(int nMode, int nThreshold)
{
	if ( nMode == en_FoilExpGraph) {
		m_cGraphCam1.SetThresHold1(nThreshold);
		m_cGraphCam2.SetThresHold1(nThreshold);
	}
	else {
		m_cGraphCam3.SetThresHold1(nThreshold);
		m_cGraphCam4.SetThresHold1(nThreshold);
	}
}
// 22.06.20 Ahn Add End

