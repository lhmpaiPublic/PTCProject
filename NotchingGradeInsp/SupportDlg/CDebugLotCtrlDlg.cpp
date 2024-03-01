// CDebugLotCtrlDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CDebugLotCtrlDlg.h"
#include "afxdialogex.h"
#include "GlobalData.h"
#include "NotchingGradeInspView.h"

// CDebugLotCtrlDlg 대화 상자

IMPLEMENT_DYNAMIC(CDebugLotCtrlDlg, CDialogEx)

CDebugLotCtrlDlg::CDebugLotCtrlDlg(CNotchingGradeInspView* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DEBUG_CTRL, pParent)
{
	m_pView = pParent;
}

CDebugLotCtrlDlg::~CDebugLotCtrlDlg()
{
}

void CDebugLotCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDebugLotCtrlDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOT_CHANGE, &CDebugLotCtrlDlg::OnBnClickedBtnLotChange)
	ON_BN_CLICKED(IDC_BTN_LOT_END, &CDebugLotCtrlDlg::OnBnClickedBtnLotEnd)
END_MESSAGE_MAP()


// CDebugLotCtrlDlg 메시지 처리기


void CDebugLotCtrlDlg::OnBnClickedBtnLotChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pView != nullptr) {
		m_pView->m_bDebugLotStartReq = TRUE;
	}
}


void CDebugLotCtrlDlg::OnBnClickedBtnLotEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pView != nullptr) {
		m_pView->m_bDebugLotEndReq = TRUE;
	}
}

