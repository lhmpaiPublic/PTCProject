// CTactTImeGraphDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CTactTImeGraphDlg.h"
#include "afxdialogex.h"
#include "GraphCtrl.h"
#include "ImageProcessCtrl.h"
#include "CTacTimeDataCtrl.h"
// CTactTImeGraphDlg 대화 상자

IMPLEMENT_DYNAMIC(CTactTimeGraphDlg, CDialogEx)

CTactTimeGraphDlg::CTactTimeGraphDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_TACT_TIME_GRAPH, pParent)
{
//	m_pGraphTact = NULL;
}

CTactTimeGraphDlg::~CTactTimeGraphDlg()
{
	//if (m_pGraphTact != NULL) {
	//	delete m_pGraphTact;
	//	m_pGraphTact = NULL;
	//}
}

void CTactTimeGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// 23.02.08 Son Mod Start
	//DDX_Control(pDX, IDC_GRAPH_TACT_TIME, m_graphTact);
	DDX_Control(pDX, IDC_GRAPH_TACT_TIME, m_pGraphTact);
	// 23.02.08 Son Mod End

}


BEGIN_MESSAGE_MAP(CTactTimeGraphDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTactTImeGraphDlg 메시지 처리기


BOOL CTactTimeGraphDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//if (m_pGraphTact == NULL) {
	//	m_pGraphTact = new CGraphCtrl();

	//	CRect rect = CRect(0, 0, 0, 0);
	//	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_GRAPH_TACT_TIME);
	//
	//	if (pWnd != NULL) {
	//		pWnd->GetClientRect(&rect);
	//	}

	//	if (m_pGraphTact->Create(rect, this, IDC_GRAPH_TACT_TIME) == FALSE) {
	//		delete m_pGraphTact;
	//		m_pGraphTact = NULL;
	//		return -1;
	//	}

	//}

	// 23.02.08 Son Mod Start
	//CGraphCtrl* m_pGraphTact = &m_graphTact ;

	//m_pGraphTact->DrawFrameRect(TRUE);
	//m_pGraphTact->SetDataColor(0, RGB(128, 255, 128));

	//m_pGraphTact->SetImageSize(0);
	//m_pGraphTact->SetGraphSize(200);
	//m_pGraphTact->SetVisibleHScollbar(TRUE);
	//m_pGraphTact->SetBkColor(RGB(0, 0, 0));
	//m_pGraphTact->DrawDataLine(TRUE);
	//m_pGraphTact->DrawDataPoint(FALSE);

	//m_pGraphTact->DrawHorAxis(TRUE);
	//m_pGraphTact->DrawPointZeroData(FALSE);

	//m_pGraphTact->SetThresHold1(400) ;

	//m_pGraphTact->SetMaxVerScaleSize(800);
	//m_pGraphTact->SetVerMajorScaleInterval(100);
	////m_pGraphTact->SetHorScaleTitle(_T("Cell No"));
	//m_pGraphTact->UseVerScaleLabel(TRUE);

	//m_pGraphTact->UpdateData();
	//CRect rcDlg ;

	//m_pGraphTact->ShowWindow(SW_SHOW);

	////GetClientRect(&rcDlg);
	////m_cGraphTact.OnSize()
	//m_pGraphTact->OnRefresh(0);
	//AddData();
	m_pGraphTact.DrawFrameRect(TRUE);
	m_pGraphTact.SetDataColor(0, RGB(128, 255, 128));
	m_pGraphTact.SetMaxVerScaleSize(800);
	m_pGraphTact.SetVerMajorScaleInterval(100);
	m_pGraphTact.SetGraphSize(200);
	m_pGraphTact.SetVisibleHScollbar(TRUE);
	m_pGraphTact.SetBkColor(RGB(0, 0, 0));
	m_pGraphTact.DrawDataLine(TRUE);
	m_pGraphTact.DrawDataPoint(FALSE);

	m_pGraphTact.DrawHorAxis(TRUE);
	m_pGraphTact.DrawPointZeroData(FALSE);

	m_pGraphTact.SetThresHold1(400);

	m_pGraphTact.UseVerScaleLabel(TRUE);

	m_pGraphTact.UpdateData();
	CRect rcDlg;

	GetClientRect(&rcDlg);
	m_pGraphTact.OnSize(0, rcDlg.Width() - 20, rcDlg.Height() - 60);
	m_pGraphTact.SetImageSize(200);
	m_pGraphTact.OnRefresh(0);
	AddData();

	m_pGraphTact.AdjustSize();
	m_pGraphTact.Invalidate(FALSE);
	// 23.02.08 Son Mod End
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTactTimeGraphDlg::AddData()
{
	//int i = 0;
	//int cnt = 0 ;
	//int data = 0 ;
	//for (i = 0; i < 400; i++) {
	//	data = rand() % 500 ;
	//	m_pGraphTact.SetData( 0, i, data );
	//	m_pGraphTact.SetImageSize(i+1);
	//}
	//m_pGraphTact.OnRefresh(i-1);

	CTacTimeDataCtrl* pTtCtrl;
	pTtCtrl = theApp.m_pImgProcCtrl->GetTactDataCtrlPtr();
	CTacTimeDataCtrl::_VEC_TACT_DATA::iterator iter;
	CTacTimeDataCtrl::_VEC_TACT_DATA* pVecData;
	pVecData = pTtCtrl->GetTactTimeDataPtr() ;

	int size = (int)pTtCtrl->GetSize();
	int i = 0; 
	for (iter = pVecData->begin(); iter != pVecData->end(); iter++) {
	// 23.02.08 Son Mod Start
	//	m_graphTact.SetData(0, i, (int)iter->dTactTime);
	//	m_graphTact.SetImageSize(i + 1);
		m_pGraphTact.SetData(0, i, (int)iter->dTactTime);
		m_pGraphTact.SetImageSize(i + 1);
	// 23.02.08 Son Mod End
		i++; 
	}
	// 23.02.08 Son Mod Start
	//m_graphTact.OnRefresh(i-1);
	m_pGraphTact.OnRefresh(i - 1);
	// 23.02.08 Son Mod End
}

void CTactTimeGraphDlg::DataClearAll()
{
	// 23.02.08 Son Mod Start
	//m_graphTact.DataClear(); 
	m_pGraphTact.DataClear();
	// 23.02.08 Son Mod End
}

void CTactTimeGraphDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	//if ( (m_pGraphTact != NULL) && (m_pGraphTact->m_hWnd != NULL)){
	//	//CRect rcRect;
	//	//m_pGraphTact->GetClientRect(&rcRect);
	//	//m_pGraphTact->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rcRect.Width(), rcRect.Height()));
	//}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
