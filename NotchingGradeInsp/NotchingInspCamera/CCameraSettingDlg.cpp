// CCameraSettingDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CCameraSettingDlg.h"
#include "afxdialogex.h"
#include "SetValue.h"
#include "GlobalData.h"
#include "ImageProcessCtrl.h"

// CCameraSettingDlg 대화 상자

IMPLEMENT_DYNAMIC(CCameraSettingDlg, CDialogEx)

CCameraSettingDlg::CCameraSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CAMERA_SETTING, pParent)
	, m_dEdSpeed(1000.0)
	, m_dEdExposureTime(4.1)
	, m_dEdGian(5.0)
{
	m_dLineRate = 200000.0 ;
}

CCameraSettingDlg::~CCameraSettingDlg()
{
}

void CCameraSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_SPEED, m_dEdSpeed);
	DDX_Text(pDX, IDC_ED_EXPOSURE_TIME, m_dEdExposureTime);
	DDX_Text(pDX, IDC_ED_GAIN, m_dEdGian);
	DDX_Control(pDX, IDC_ED_DUMMY, m_EdDummy);
}


BEGIN_MESSAGE_MAP(CCameraSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SET_SPEED, &CCameraSettingDlg::OnBnClickedBtnSetSpeed)
	ON_BN_CLICKED(IDC_BTN_SET_EXPTIME, &CCameraSettingDlg::OnBnClickedBtnSetExptime)
	ON_BN_CLICKED(IDC_BTN_SET_GAIN, &CCameraSettingDlg::OnBnClickedBtnSetGain)
	ON_BN_CLICKED(IDC_BTN_SET_ALL, &CCameraSettingDlg::OnBnClickedBtnSetAll)
	ON_BN_CLICKED(IDOK, &CCameraSettingDlg::OnBnClickedOk)
	ON_EN_SETFOCUS(IDC_ED_SPEED, &CCameraSettingDlg::OnEnSetfocusEdSpeed)
	ON_EN_SETFOCUS(IDC_ED_EXPOSURE_TIME, &CCameraSettingDlg::OnEnSetfocusEdExposureTime)
	ON_EN_SETFOCUS(IDC_ED_GAIN, &CCameraSettingDlg::OnEnSetfocusEdGain)
END_MESSAGE_MAP()


// CCameraSettingDlg 메시지 처리기

BOOL CCameraSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (theApp.m_pImgProcCtrl != nullptr) {
		m_dLineRate = theApp.m_pImgProcCtrl->GetLineRate();
		m_dEdGian = theApp.m_pImgProcCtrl->GetGain();
		m_dEdExposureTime = theApp.m_pImgProcCtrl->GetExposureTime();

		double dResoY = AprData.m_System.m_dResolY;
		if (m_dLineRate > 0) {
			m_dEdSpeed = m_dLineRate * (dResoY / 2) / 1000.0;
		}
		UpdateData(FALSE);
	}

	return TRUE;
}

void CCameraSettingDlg::OnBnClickedBtnSetSpeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dLineRate = m_dLineRate;
	theApp.m_pImgProcCtrl->SetLineRate(dLineRate);
}


void CCameraSettingDlg::OnBnClickedBtnSetExptime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dExpTime = m_dEdExposureTime;
	theApp.m_pImgProcCtrl->SetExposureTime(dExpTime);

}


void CCameraSettingDlg::OnBnClickedBtnSetGain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dGain = m_dEdGian;
	theApp.m_pImgProcCtrl->SetGain(0, dGain);
}


void CCameraSettingDlg::OnBnClickedBtnSetAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedBtnSetSpeed();
	OnBnClickedBtnSetExptime();
	OnBnClickedBtnSetGain();
}


void CCameraSettingDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void CCameraSettingDlg::OnEnSetfocusEdSpeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_EdDummy.SetFocus();
	CSetValue setValue;
	CString strMsg = _T("속도를 입력해 주세요.");

	UpdateData(TRUE);

//	double dValue = m_dEdSpeed ;
	double dMax = 1500.0;
	double dMin = 100.0;
	int nRet = setValue.SetValue(strMsg, m_dEdSpeed, dMax, dMin);
	double dResoY = AprData.m_System.m_dResolY;
	if (nRet == 0) {
		m_dLineRate = ( m_dEdSpeed * 1000.0 ) / ( dResoY / 2 ) ;
	}

	UpdateData(FALSE);
}


void CCameraSettingDlg::OnEnSetfocusEdExposureTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	CSetValue setValue;
	CString strMsg = _T("노광시간을 입력해 주세요.");

	UpdateData(TRUE);
	//	double dValue;
	double dMax = 10.0;
	double dMin = 1.0;
	setValue.SetValue(strMsg, m_dEdExposureTime, dMax, dMin);
	UpdateData(FALSE);
}


void CCameraSettingDlg::OnEnSetfocusEdGain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	CSetValue setValue;
	CString strMsg = _T("Gain 값을 입력해 주세요.");
	UpdateData(TRUE);

//	double dValue;
	double dMax = 9.0;
	double dMin = 0.1;
	setValue.SetValue(strMsg, m_dEdGian, dMax, dMin);
	UpdateData(FALSE);
}
