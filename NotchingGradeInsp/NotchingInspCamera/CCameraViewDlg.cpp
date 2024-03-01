// CCameraViewDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CCameraViewDlg.h"
#include "afxdialogex.h"
#include "ImageProcessCtrl.h"


// CCameraViewDlg 대화 상자

IMPLEMENT_DYNAMIC(CCameraViewDlg, CDialogEx)

CCameraViewDlg::CCameraViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CAM_VIEW, pParent)
	, m_radSelectCam(0)
	, m_bTriggerMode(FALSE)
{
	m_bRun = FALSE ;
	m_bDisableRun = FALSE;
	for( int i = 0 ; i < GRABBER_COUNT ; i++ ){
		m_ImgViewhWnd[i] = NULL;
	}
}

CCameraViewDlg::~CCameraViewDlg()
{
}

void CCameraViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RAD_CAM_01, m_radSelectCam);
	DDX_Check(pDX, IDC_CHK_TRIGGER_MODE, m_bTriggerMode);
}


BEGIN_MESSAGE_MAP(CCameraViewDlg, CDialogEx)
//	ON_BN_CLICKED(IDC_BTN_CAM_01, &CCameraViewDlg::OnBnClickedBtnCam01)
//	ON_BN_CLICKED(IDC_BTN_CAM_02, &CCameraViewDlg::OnBnClickedBtnCam02)
	ON_BN_CLICKED(IDC_BTN_START, &CCameraViewDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_RAD_CAM_01, &CCameraViewDlg::OnBnClickedRadCam01)
	ON_BN_CLICKED(IDC_RAD_CAM_02, &CCameraViewDlg::OnBnClickedRadCam02)
	ON_BN_CLICKED(IDC_CHK_TRIGGER_MODE, &CCameraViewDlg::OnBnClickedChkTriggerMode)
END_MESSAGE_MAP()


// CCameraViewDlg 메시지 처리기


BOOL CCameraViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i = 0; i < GRABBER_COUNT; i++) {
		CWnd* pWnd;
		int nID = IDC_ST_CAM_01;
		pWnd = GetDlgItem( nID + i );
		if (pWnd != nullptr) {
			m_ImgViewhWnd[i] = pWnd->m_hWnd;
//			theApp.m_pImgProcCtrl->SetMonitorDispHwnd(m_ImgViewhWnd[i], i);
		}
	}

	m_bTriggerMode = theApp.m_pImgProcCtrl->GetTriggerMode();
	if (m_bTriggerMode == en_Int_Trg ) {
		CWnd* pWnd = GetDlgItem(IDC_CHK_TRIGGER_MODE);
		if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
			pWnd->SetWindowText(_T("Int Trigger"));
		}
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCameraViewDlg::OnBnClickedBtnStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_BTN_START);
	CString strName ;
	if (pWnd == nullptr) return;
	if (m_bRun == FALSE) {
		if (theApp.m_pImgProcCtrl->IsInspection() == FALSE) {
			if (theApp.m_pImgProcCtrl->GrabStart_Monitoring() < 0) {
				UpdateData(FALSE);
				return;
			}
		}
		m_bRun = TRUE;
		strName = _T("Stop");
		pWnd->SetWindowTextA(strName);
	}
	else {
		m_bRun = FALSE;
		if (theApp.m_pImgProcCtrl->IsInspection() == FALSE) {
			theApp.m_pImgProcCtrl->GrabStop();
		}
		strName = _T("Start");
		pWnd->SetWindowTextA(strName);
	}
}

void CCameraViewDlg::SetDisableRun(BOOL bMode)
{

}

BOOL CCameraViewDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::DestroyWindow();
}

void CCameraViewDlg::OnBnClickedRadCam01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd = GetDlgItem(IDC_ST_CAM_01);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_ST_CAM_02);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(SW_HIDE);
	}
}

void CCameraViewDlg::OnBnClickedRadCam02()
{
	 //TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd = GetDlgItem(IDC_ST_CAM_02);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(SW_SHOW);
	}
	pWnd = GetDlgItem(IDC_ST_CAM_01);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		pWnd->ShowWindow(SW_HIDE);
	}
}


void CCameraViewDlg::OnBnClickedChkTriggerMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_CHK_TRIGGER_MODE);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		if (m_bTriggerMode == en_Ext_Trg) {
			pWnd->SetWindowText("Ext Trigger");
			theApp.m_pImgProcCtrl->SetTriggerMode(en_Ext_Trg);
		}
		else {
			pWnd->SetWindowText("Int Trigger");
			theApp.m_pImgProcCtrl->SetTriggerMode(en_Int_Trg);
		}

	}

}
