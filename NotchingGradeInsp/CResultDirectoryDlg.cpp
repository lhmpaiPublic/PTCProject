// CResultDirectoryDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CResultDirectoryDlg.h"
#include "afxdialogex.h"
#include "shellapi.h"
#include "GlobalData.h"

// CResultDirectoryDlg 대화 상자

IMPLEMENT_DYNAMIC(CResultDirectoryDlg, CDialogEx)

CResultDirectoryDlg::CResultDirectoryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DIRECTORY, pParent)
	, m_strCsvPath(_T(""))
	, m_strNgImagePath(_T(""))
	, m_strOkImagePath(_T(""))
{

}

CResultDirectoryDlg::~CResultDirectoryDlg()
{
}

void CResultDirectoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_CSV_PATH, m_strCsvPath);
	DDX_Text(pDX, IDC_ED_NG_IMAGE_PATH, m_strNgImagePath);
	DDX_Text(pDX, IDC_ED_OK_IMAGE_PATH, m_strOkImagePath);
}


BEGIN_MESSAGE_MAP(CResultDirectoryDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CSV_OPEN, &CResultDirectoryDlg::OnBnClickedBtnCsvOpen)
	ON_BN_CLICKED(IDC_BTN_OPEN_NG_IMG, &CResultDirectoryDlg::OnBnClickedBtnOpenNgImg)
	ON_BN_CLICKED(IDC_BTN_OPEN_OK_IMG, &CResultDirectoryDlg::OnBnClickedBtnOpenOkImg)
END_MESSAGE_MAP()

// CResultDirectoryDlg 메시지 처리기
void CResultDirectoryDlg::OnBnClickedBtnCsvOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_strCsvPath.GetLength() <= 0) {
		return ;
	}
	ShellExecute(NULL, "open", m_strCsvPath, NULL, NULL, SW_SHOWNORMAL);
}

void CResultDirectoryDlg::OnBnClickedBtnOpenNgImg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_strNgImagePath.GetLength() <= 0) {
		return ;
	}
	ShellExecute(NULL, "open", m_strNgImagePath, NULL, NULL, SW_SHOWNORMAL);
}

void CResultDirectoryDlg::OnBnClickedBtnOpenOkImg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_strOkImagePath.GetLength() <= 0) {
		return ;
	}
	ShellExecute(NULL, "open", m_strOkImagePath, NULL, NULL, SW_SHOWNORMAL);
}

BOOL CResultDirectoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_strCsvPath = AprData.m_strFeederPath;
	m_strOkImagePath = AprData.m_strImagePath + _T("\\OK");
	m_strNgImagePath = AprData.m_strImagePath + _T("\\NG");

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
