// InspDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "InspDlg.h"
#include "afxdialogex.h"
#include "CImageDispDlg.h"
#include "ImageProcessCtrl.h"
#include "GlobalData.h"
#include "QueueCtrl.h"
#include "SigProc.h"
#include "CThreadQueueCtrl.h"
#include "CCounterQueueCtrl.h" // 22.04.06 Ahn Add
#include "CImageSaveQueueCtrl.h" // 22.05.31 Ahn Add
#include "MainFrm.h"	// 22.07.22 Ahn Add
// CInspDlg 대화 상자

IMPLEMENT_DYNAMIC(CInspDlg, CDialogEx)

#define T_ID_RENEWAL	100 


CInspDlg::CInspDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_INSP, pParent)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = pView->GetDocument();
	}
	m_nIspState = -1;
	m_strDebugStatus = _T("");
#if defined( IMAGE_DRAW_NOTIFY_VERSION )
	m_pImgTabDlg = NULL;
	m_pImgBtmDlg = NULL;
#endif
}

CInspDlg::~CInspDlg()
{
#if defined( IMAGE_DRAW_NOTIFY_VERSION )
	if (m_pImgTabDlg != NULL) {
		delete m_pImgTabDlg;
		m_pImgTabDlg = NULL;
	}
	if (m_pImgBtmDlg != NULL) {
		delete m_pImgBtmDlg;
		m_pImgBtmDlg = NULL;
	}
#endif
}

void CInspDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_DEBUG_STATUS, m_strDebugStatus);
}


BEGIN_MESSAGE_MAP(CInspDlg, CDialogEx)
	ON_WM_SIZE()
	//ON_BN_CLICKED(IDC_BTN_LOT_CHANGE, &CInspDlg::OnBnClickedBtnLotChange)
	// 22.06.13 Ahn Delete End
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CInspDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// 22.06.13 Ahn Delete Start
	//ChangeStatus(m_pDoc->GetInspState());

	//m_rdInspRun.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	//m_rdInspRun.SetImage(IDB_INSP_RUN);
	//m_rdInspRun.m_bTransparent = FALSE;
	//m_rdInspRun.SetMouseCursorHand();
	//m_rdInspRun.m_bTopImage = TRUE;
	//m_rdInspRun.SetTooltip(_T("검사 시작"));

	//m_rdInspStop.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	//m_rdInspStop.SetImage(IDB_INSP_STOP);
	//m_rdInspStop.m_bTransparent = FALSE;
	//m_rdInspStop.SetMouseCursorHand();
	//m_rdInspStop.m_bTopImage = TRUE;
	//m_rdInspStop.SetTooltip(_T("검사 종료"));

	//m_rdInspPause.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	//m_rdInspPause.SetImage(IDB_INSP_PAUSE);
	//m_rdInspPause.m_bTransparent = FALSE;
	//m_rdInspPause.SetMouseCursorHand();
	//m_rdInspPause.m_bTopImage = TRUE;
	//m_rdInspPause.SetTooltip(_T("검사 멈춤"));
	// 22.06.13 Ahn Delete End

	UpdateData(FALSE);

#if defined( IMAGE_DRAW_NOTIFY_VERSION )
	if (m_pImgTabDlg == NULL) {
		m_pImgTabDlg = new CImageDispDlg(this);
		m_pImgTabDlg->Create(IDD_DISP_IMAGE_DLG, this);
		m_pImgTabDlg->ShowWindow(SW_SHOW);
	}
	if (m_pImgBtmDlg == NULL) {
		m_pImgBtmDlg = new CImageDispDlg(this);
		m_pImgBtmDlg->Create(IDD_DISP_IMAGE_DLG, this);
		m_pImgBtmDlg->ShowWindow(SW_SHOW);
	}
#endif

#if defined( IMAGE_DRAW_DIRECT_VERSION )
	CWnd* pWnd;
	pWnd = (CWnd*)GetDlgItem(IDC_ST_IMAGE_BTM);
	theApp.m_pImgProcCtrl->SetImgDispHWnd(pWnd->m_hWnd, 1);

	pWnd = (CWnd*)GetDlgItem(IDC_ST_IMAGE_TAB);
	theApp.m_pImgProcCtrl->SetImgDispHWnd(pWnd->m_hWnd, 0);
#endif

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == FALSE) {
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_LOT_CHANGE);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(SW_HIDE);
		}
	}

	SetRenewalTimer();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CInspDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rectControl, rectParent;
	CButton* pButton = NULL;

	int nWidth = 0, nHeight = 0;
	int nXPos = 0, nYPos = 0;
	this->GetWindowRect(rectParent);

	CWnd* pWnd;
	// 22.07.22 Ahn Add Start
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr)
	{
		CPoint cpStartPos = pFrame->GetInspDIspPosition();
	// 22.07.22 Ahn Add End

		pWnd = (CWnd*)GetDlgItem(IDC_ST_IMAGE_BTM);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rectControl);
			nXPos = cpStartPos.x + 5;
			nYPos = 5;
			nHeight = rectParent.Height() - 30;
			nWidth = rectControl.Width();
			pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ST_IMAGE_TAB);

		int nImgBtm = 0;
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rectControl);
			nXPos = cpStartPos.x + rectControl.Width() + 10;
			nYPos = 5;
			nHeight = rectParent.Height() - 30;
			nWidth = rectControl.Width();
			nImgBtm = nHeight;
			pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		}

		pWnd = (CWnd*)GetDlgItem(IDC_ST_DEBUG_STATUS);
		if (pWnd != nullptr) {
			pWnd->GetWindowRect(rectControl);
			nWidth = rectParent.Width() - cpStartPos.x;
			nHeight = rectControl.Height();
			nXPos = cpStartPos.x;
			nYPos = nImgBtm + 10 ;
			pWnd->MoveWindow(nXPos, nYPos, nWidth, nHeight);
		}
	}

}

void CInspDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == m_TID_Renewal) {
		KillRenewalTimer();

		int nFramdCnt1, nFrameCnt2;
		int nFrmQue1, nFrmQue2;
		int nQueueCnt1, nQueueCnt2;
		int nRsltQueCnt1, nRsltQUeCnt2;
		nFramdCnt1 = AprData.GetFrameCounter(0);
		nFrameCnt2 = AprData.GetFrameCounter(1);

		nFrmQue1 = theApp.m_pImgProcCtrl->GetQueueFrmPtr(0)->GetSize();
		nFrmQue2 = theApp.m_pImgProcCtrl->GetQueueFrmPtr(1)->GetSize();
		nQueueCnt1 = theApp.m_pImgProcCtrl->GetThreadQueuePtr(0)->GetSize();
		nQueueCnt2 = theApp.m_pImgProcCtrl->GetThreadQueuePtr(1)->GetSize();
		nRsltQueCnt1 = theApp.m_pImgProcCtrl->GetResultPtr(0)->GetSize();
		nRsltQUeCnt2 = theApp.m_pImgProcCtrl->GetResultPtr(1)->GetSize();
		
		// 22.05.31 Ahn Modify Start
		int nImgSaveQueCnt = (int)theApp.m_pImgProcCtrl->GetImageSaveQueuePtr()->GetSize() ;
		m_strDebugStatus.Format(_T("FrmCnt[%d / %d], FrmQue[ %d / %d], ThreadQueue[%d/%d], ResultQueue[ %d / %d], ImgSaveQueue[%d]"), nFramdCnt1, nFrameCnt2, nFrmQue1, nFrmQue2, nQueueCnt1, nQueueCnt2, nRsltQueCnt1, nRsltQUeCnt2, nImgSaveQueCnt);
		// 22.05.31 Ahn Modify End

		UpdateData(FALSE);

		theApp.m_pImgProcCtrl->SaveErrorData();
		SetRenewalTimer();
	}

	CDialogEx::OnTimer(nIDEvent);
}



BOOL CInspDlg::SetRenewalTimer()
{
	m_TID_Renewal = SetTimer(T_ID_RENEWAL, 100, NULL);
	return FALSE;
}
BOOL CInspDlg::KillRenewalTimer()
{
	if (m_TID_Renewal != 0) {
		KillTimer(m_TID_Renewal);
		m_TID_Renewal = 0;
		return TRUE;
	}

	return FALSE;
}


void CInspDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	KillRenewalTimer();
}


// 22.06.13 Ahn Delete Start
//void CInspDlg::OnBnClickedBtnLotChange()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	if (m_pDoc->GetInspState() == enInspRun) {
//		m_pView->SetLotChangeReq(TRUE);
//	}
//
//}
// 22.06.13 Ahn Delete End
