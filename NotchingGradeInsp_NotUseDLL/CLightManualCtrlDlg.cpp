// CLightManualCtrlDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CLightManualCtrlDlg.h"
#include "afxdialogex.h"
#include "CLightControl.h"


// CLightManualCtrlDlg 대화 상자

IMPLEMENT_DYNAMIC(CLightManualCtrlDlg, CDialogEx)

CLightManualCtrlDlg::CLightManualCtrlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_LIGHT_CTRL, pParent)
{


}

CLightManualCtrlDlg::~CLightManualCtrlDlg()
{
}

void CLightManualCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_REF_LIGHT_01, m_nLightRef_1);
	DDX_Text(pDX, IDC_ED_REF_LIGHT_02, m_nLightRef_2);
	DDX_Text(pDX, IDC_ED_DIFF_REF_LIGHT_01, m_nLightDiffRef_1);
	DDX_Text(pDX, IDC_ED_DIFF_REF_LIGHT_02, m_nLightDiffRef_2);
}


BEGIN_MESSAGE_MAP(CLightManualCtrlDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SET_ALL, &CLightManualCtrlDlg::OnBnClickedBtnSetAll)
	ON_BN_CLICKED(IDOK, &CLightManualCtrlDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLightManualCtrlDlg 메시지 처리기


void CLightManualCtrlDlg::OnBnClickedBtnSetAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLightControl* pLightCtrl;
	pLightCtrl = theApp.m_pLightCtrl;
	int nRefLevel[MAX_LIGHT_CHANEL];
	int nDiffRefLevel[MAX_LIGHT_CHANEL];

	UpdateData(TRUE);
	nDiffRefLevel[0] = m_nLightDiffRef_1;
	nDiffRefLevel[1] = m_nLightDiffRef_2;
	nRefLevel[0] = m_nLightRef_1;
	nRefLevel[1] = m_nLightRef_2;


	pLightCtrl->SetLevelAll(0, nRefLevel, 2);
	pLightCtrl->SetLevelAll(1, nDiffRefLevel, 2);

}


BOOL CLightManualCtrlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CLightControl* pLightCtrl;
	pLightCtrl = theApp.m_pLightCtrl;
	int nRefLevel[MAX_LIGHT_CHANEL];
	int nDiffRefLevel[MAX_LIGHT_CHANEL];
	memset(nRefLevel, 0, sizeof(int) * MAX_LIGHT_CHANEL);
	memset(nDiffRefLevel, 0, sizeof(int) * MAX_LIGHT_CHANEL);
	pLightCtrl->GetLevelAll( 0, nRefLevel, 2);
	pLightCtrl->GetLevelAll( 1, nDiffRefLevel, 2);

	m_nLightRef_1 = nRefLevel[0];
	m_nLightRef_2 = nRefLevel[1];
	m_nLightDiffRef_1 = nDiffRefLevel[0];
	m_nLightDiffRef_2 = nDiffRefLevel[1];
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLightManualCtrlDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
