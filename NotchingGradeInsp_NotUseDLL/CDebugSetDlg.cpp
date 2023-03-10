// CDebugSetDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CDebugSetDlg.h"
#include "afxdialogex.h"


// CDebugSetDlg 대화 상자

IMPLEMENT_DYNAMIC(CDebugSetDlg, CDialogEx)

CDebugSetDlg::CDebugSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DEBUG_SET, pParent)
{
}

CDebugSetDlg::~CDebugSetDlg()
{
}

void CDebugSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_LIST_CHECK_DEBUG, m_chkListDebug);
}


BEGIN_MESSAGE_MAP(CDebugSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDebugSetDlg::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_LIST_CHECK_DEBUG, &CDebugSetDlg::OnLbnSelchangeListCheckDebug)
END_MESSAGE_MAP()


// CDebugSetDlg 메시지 처리기


void CDebugSetDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_DebugSet.FileCtrl(MODE_WRITE);

	CDialogEx::OnOK();
}


BOOL CDebugSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_DebugSet.FileCtrl(MODE_READ);

	m_chkListDebug.SubclassDlgItem(IDC_LIST_CHECK_DEBUG, this );

	CString strName;
	for (int i = 0; i < CDebugSet::en_Debug_Total; i++) {
		strName = m_DebugSet.GetDebugName(i);
		m_chkListDebug.InsertString(i, strName);
		m_chkListDebug.SetCheck(i, m_DebugSet.GetDebug(i));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CDebugSetDlg::OnLbnSelchangeListCheckDebug()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_chkListDebug.GetCurSel();

	BOOL bCheck = m_chkListDebug.GetCheck(nSel);
	m_DebugSet.SetDebug(nSel, bCheck);

}
