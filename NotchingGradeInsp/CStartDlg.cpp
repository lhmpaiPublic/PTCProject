// CStartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CStartDlg.h"
#include "afxdialogex.h"
#include "CDebugSetDlg.h"
#include "GlobalData.h"
#include "SystemSettingDlg.h"
// CStartDlg 대화 상자

IMPLEMENT_DYNAMIC(CStartDlg, CDialogEx)

CStartDlg::CStartDlg(CWnd* pParent /*=nullptr*/, int nStartMode /*=0*/)
	: CDialogEx(IDD_DLG_START, pParent)
	, m_strMessage(_T(""))
{
	m_nStartMode = nStartMode;
	lWait_Time = 3;

	if (nStartMode == en_InspStart)
	{
		lWait_Time = 5 ; 
	}

}

CStartDlg::~CStartDlg()
{
}

void CStartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_MESSAGE, m_strMessage);
	DDX_Control(pDX, IDC_PROGRESS_TIMEER, m_ProgressTimer);
}


BEGIN_MESSAGE_MAP(CStartDlg, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CStartDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_DEBUG_SETTING, &CStartDlg::OnBnClickedBtnDebugSetting)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CStartDlg::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_SET, &CStartDlg::OnBnClickedBtnSystemSet)
END_MESSAGE_MAP()

// CStartDlg 메시지 처리기

void CStartDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == T_ID_START) {
		KillTimer(T_ID_START);

		TimeEnd = CTime::GetCurrentTime() ;
		CTimeSpan ts = TimeEnd - TimeStart ;
		if (ts.GetTotalSeconds() > lWait_Time) {
			PostMessage(WM_COMMAND, IDOK, 0L ) ;
		}

		m_ProgressTimer.SetPos(nPos++);
		UpdateData(FALSE);

		SetTimer(T_ID_START, 200, NULL );
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CStartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	TimeStart = CTime::GetCurrentTime();
	TimeEnd = TimeStart;

	int nSize = lWait_Time * 60 * 10 ;
	int nCount = nSize / 200;
	nPos = 0;
	SetTimer( T_ID_START, 200, NULL );

	m_ProgressTimer.SetRange(0, nCount);
	m_ProgressTimer.SetPos(nPos++);
	UpdateData(FALSE);

	CWnd* pWnd;
	switch (m_nStartMode) {
	case	en_AppStart :
		break;
	case	en_InspStart :
		// 21.12.28 Ahn Add Start
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_DEBUG_SETTING);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(SW_HIDE) ;
		}
		m_strMessage.Format(_T("5초 후 자동 개시"));
		UpdateData(FALSE);
		// 21.12.28 Ahn Add End
		break;
	default :
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CStartDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void CStartDlg::OnBnClickedBtnDebugSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	KillTimer(T_ID_START);

	CDebugSetDlg dlg;
	if (dlg.DoModal() == IDOK) {
		AprData.m_DebugSet.FileCtrl(MODE_READ);
	}	
}


void CStartDlg::OnBnClickedBtnCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CStartDlg::OnBnClickedBtnSystemSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	KillTimer(T_ID_START);

	CSystemSettingDlg dlg(this);
	if (dlg.DoModal() == IDOK) {
	}
}
