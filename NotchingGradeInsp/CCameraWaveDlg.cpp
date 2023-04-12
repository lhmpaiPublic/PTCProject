// CCameraWaveDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CCameraWaveDlg.h"
#include "afxdialogex.h"
#include "CCameraWaveDlg.h"
#include "NotchingGradeInsp.h"
#include "ImageProcessCtrl.h"
#include "CCameraViewDlg.h"
#include "GlobalData.h"

// CCameraWaveDlg 대화 상자

IMPLEMENT_DYNAMIC(CCameraWaveDlg, CDialogEx)

CCameraWaveDlg::CCameraWaveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CAM_WAVE, pParent)
	, m_bChkTrgMode(FALSE)
	, m_bChkWaveRun(FALSE)
	, m_nRadSelCamNo(0)
{
}

CCameraWaveDlg::~CCameraWaveDlg()
{
}

void CCameraWaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUST_CAM_WAVE, m_WaveGraph);

	DDX_Check(pDX, IDC_CHK_TRG_MODE, m_bChkTrgMode);
	DDX_Check(pDX, IDC_CHK_WAVE_RUN, m_bChkWaveRun);
	DDX_Radio(pDX, IDC_RAD_CAM_01, m_nRadSelCamNo);
}


BEGIN_MESSAGE_MAP(CCameraWaveDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHK_WAVE_RUN, &CCameraWaveDlg::OnBnClickedChkWaveRun)
	ON_BN_CLICKED(IDC_CHK_TRG_MODE, &CCameraWaveDlg::OnBnClickedChkTrgMode)
	ON_BN_CLICKED(IDC_RAD_CAM_01, &CCameraWaveDlg::OnBnClickedRadCam01)
	ON_BN_CLICKED(IDC_RAD_CAM_02, &CCameraWaveDlg::OnBnClickedRadCam01)
END_MESSAGE_MAP()


// CCameraWaveDlg 메시지 처리기


void CCameraWaveDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//if (m_WaveGraph.GetSafeHwnd() != nullptr) {
	//	m_WaveGraph.MoveWindow(5, 5, (cx - 5), (cy / 2) - 50);
	//	m_WaveGraph.ScaleChange(10);
	//}

	int nImageLeft = 5;
	int nImageTop = 80;
	int nImageHeigh = cy - 180;
	int nImageWidth = cx - 5 ;

	if (m_WaveGraph.m_hWnd != NULL ){
		m_WaveGraph.SetWindowPos(NULL, nImageLeft, nImageTop, nImageWidth, nImageHeigh, (SWP_NOZORDER));
	}

}


BOOL CCameraWaveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_WaveGraph.DrawFrameRect(TRUE);
	m_WaveGraph.SetDataColor(0, RGB(128, 255, 128));
	m_WaveGraph.SetMaxVerScaleSize(256);

	m_WaveGraph.SetDrawIndividual(TRUE);
	m_WaveGraph.SetDrawData(0, TRUE);
	m_WaveGraph.SetHorScaleInterval(100);

	//m_WaveGraph.SetGraphSize(CAMERA_VIEW_WIDTH);
	//m_WaveGraph.OnRefresh(CAMERA_VIEW_WIDTH);
	m_WaveGraph.SetGraphSize(AprData.m_System.m_nCamViewWidth);
	m_WaveGraph.OnRefresh(AprData.m_System.m_nCamViewWidth);

	m_WaveGraph.Invalidate(FALSE);

	SetTimer(T_ID_CAMERA_WAVE, 200, NULL);

	m_bChkTrgMode = theApp.m_pImgProcCtrl->GetTriggerMode();
	if (m_bChkTrgMode == CCameraViewDlg::en_Int_Trg) {
		CWnd* pWnd = GetDlgItem(IDC_CHK_TRIGGER_MODE);
		if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
			pWnd->SetWindowText(_T("Int Trigger"));
		}
	}
	// 21.09.09 Son Add Start
	m_WaveGraph.SetVerMajorScaleInterval(20);
	m_WaveGraph.SetVerMinorScaleInterval(5);
	m_WaveGraph.DrawVerSubLine(TRUE);
	m_WaveGraph.DrawHorSubLine(TRUE);
	m_WaveGraph.SetThresHold1(160);
	m_WaveGraph.SetThresHold2(50);
	// 21.09.09 Son Add End
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCameraWaveDlg::SetWaveData(BYTE* pWaveData, int nSize)
{
	ASSERT(pWaveData);

	if (m_WaveGraph.m_hWnd == nullptr) return;
	m_WaveGraph.SetFixHScroll(FALSE);

	for (int i = 0; i < nSize; i++) {
		m_WaveGraph.SetData(0, i, pWaveData[i]);
	}
	m_WaveGraph.SetMaxVerScaleSize( 256 );
	int nScaleIInterval = 20 ;
	m_WaveGraph.SetVerMajorScaleInterval(nScaleIInterval);
	m_WaveGraph.AdjustSize();
	m_WaveGraph.OnRefresh(0);
	m_WaveGraph.Invalidate(FALSE);
}


void CCameraWaveDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	if (nIDEvent == T_ID_CAMERA_WAVE) {
		KillTimer(T_ID_CAMERA_WAVE);

		//OnTimer 로그출력
		LOGDISPLAY_SPEC(6)("CCameraWaveDlg::OnTimer");

	//	BYTE btWave[CAMERA_VIEW_WIDTH];
	//	theApp.m_pImgProcCtrl->GetCameraWaveData(m_nRadSelCamNo, btWave);

		int nSysWidth = AprData.m_System.m_nCamViewWidth;
		BYTE* pbtWave = new BYTE[nSysWidth];
		theApp.m_pImgProcCtrl->GetCameraWaveData(m_nRadSelCamNo, pbtWave);

		for (int i = 0; i < nSysWidth; i++) {
		//	m_WaveGraph.SetData(0, i, btWave[i]);
			m_WaveGraph.SetData(0, i, pbtWave[i]);
		}
		m_WaveGraph.OnRefresh(0);
		m_WaveGraph.Invalidate(FALSE);

		// 22.01.10 Ahn Add Start
		delete[] pbtWave;
		pbtWave = NULL;
		// 22.01.10 Ahn Add End

		SetTimer(T_ID_CAMERA_WAVE, 100, NULL );
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CCameraWaveDlg::OnBnClickedChkWaveRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_CHK_WAVE_RUN);
	CString strName;
	if (pWnd == nullptr) return;

	if (m_bChkWaveRun == FALSE) {
		if (theApp.m_pImgProcCtrl->IsInspection() == FALSE) {
			if (theApp.m_pImgProcCtrl->GrabStart_Monitoring() < 0) {
				UpdateData(FALSE);
				return;
			}
		}
		m_bChkWaveRun = TRUE;
		theApp.m_pImgProcCtrl->SetWaveMode(TRUE);
		strName = _T("Stop");
		pWnd->SetWindowTextA(strName);
	}
	else {
		m_bChkWaveRun = FALSE;
		if (theApp.m_pImgProcCtrl->IsInspection() == FALSE) {
			theApp.m_pImgProcCtrl->GrabStop();
		}
		theApp.m_pImgProcCtrl->SetWaveMode(FALSE);
		strName = _T("Start");
		pWnd->SetWindowTextA(strName);
	}
}


void CCameraWaveDlg::OnBnClickedChkTrgMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_CHK_TRG_MODE);
	if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
		if (m_bChkTrgMode == CCameraViewDlg::en_Ext_Trg) {
			pWnd->SetWindowText("Ext Trigger");
			theApp.m_pImgProcCtrl->SetTriggerMode(CCameraViewDlg::en_Ext_Trg);
		}
		else {
			pWnd->SetWindowText("Int Trigger");
			theApp.m_pImgProcCtrl->SetTriggerMode(CCameraViewDlg::en_Int_Trg);
		}
	}
}


void CCameraWaveDlg::OnBnClickedRadCam01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
}
