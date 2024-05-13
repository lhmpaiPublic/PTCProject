// CondDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "CondDlg.h"
#include "afxdialogex.h"


// CCondDlg 대화 상자

IMPLEMENT_DYNAMIC(CCondDlg, CDialogEx)

CCondDlg::CCondDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_COND, pParent)
{
	m_pParent = pParent;
	m_pView = pView;
	if (m_pView != NULL) {
		m_pDoc = pView->GetDocument();
	}
}

CCondDlg::~CCondDlg()
{
}

void CCondDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCondDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCondDlg 메시지 처리기


BOOL CCondDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CCondDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
