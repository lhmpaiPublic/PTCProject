// CExcelFileSaveDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CExcelFileSaveDlg.h"
#include "afxdialogex.h"
#include "Win32File.h"

#define T_ID_PRG_TIMER 110 

// CExcelFileSaveDlg 대화 상자

IMPLEMENT_DYNAMIC(CExcelFileSaveDlg, CDialogEx)

CExcelFileSaveDlg::CExcelFileSaveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_EXCEL_SAVE, pParent)
	, m_EdStrSavePath(_T("D:\\DAT\\ExcelFile\\Foil노출_History.xls"))
{
	m_pExcelCtrl = NULL;
	//23.01.10 Ahn Add Start
	m_bProcStart = FALSE ;
	//23.01.10 Ahn Add End
}

CExcelFileSaveDlg::~CExcelFileSaveDlg()
{
	if (m_pExcelCtrl != NULL) {
		delete m_pExcelCtrl;
		m_pExcelCtrl = NULL;
	}
}

void CExcelFileSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_ECEXL, m_ExcelPrgCtrl);
	DDX_Text(pDX, IDC_ED_SAVE_PATH, m_EdStrSavePath);
}


BEGIN_MESSAGE_MAP(CExcelFileSaveDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SET_PATH, &CExcelFileSaveDlg::OnBnClickedBtnSetPath)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CExcelFileSaveDlg::OnBnClickedBtnSave)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CExcelFileSaveDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CExcelFileSaveDlg 메시지 처리기

void CExcelFileSaveDlg::OnBnClickedBtnSetPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strSavePath;
	CString strExcelSavePath;

	TCHAR szFilter[256] = _T("Excel (*.xls)|*.xls;");
	// 23.01.13 Ahn Modify Start
	//strExcelSavePath = _T("D:\\DAT\\ExcelFile\\Foil노출_History.xls");
	if (m_EdStrSavePath.GetLength() <= 0) {
		strExcelSavePath = _T("D:\\DAT\\ExcelFile\\Foil노출_History.xls");
	}
	else {
		strExcelSavePath = m_EdStrSavePath;
	}
	// 23.01.13 Ahn Modify Start

	CFileDialog dlg(TRUE, _T("xls"), strExcelSavePath, OFN_HIDEREADONLY, szFilter, this);
	if (dlg.DoModal() == IDOK) {
		strSavePath = dlg.GetPathName();
		m_EdStrSavePath = strSavePath;
		UpdateData(FALSE);
	}
}


void CExcelFileSaveDlg::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_EdStrSavePath.GetLength() <= 0) {
		return;
	}

	if (m_pExcelCtrl != NULL) {
		delete 	m_pExcelCtrl;
	}

	if (CWin32File::ExistFile(m_EdStrSavePath) == TRUE) {
		if (MessageBox(_T("같은 파일이 존재합니다. 덮어 쓰시겠습니까?"), _T("엑셀 파일저장"), MB_YESNO) == IDNO) {
			return;
		}
	}

	//23.01.10 Ahn Add Start
	if (m_bProcStart == TRUE) return ;

	m_bProcStart = TRUE ;
	//23.01.10 Ahn Add End


	m_ExcelPrgCtrl.SetPos(0);

	m_pExcelCtrl = new CExcelResultFileCtrl();

	m_pExcelCtrl->SetFileName(m_EdStrSavePath);
	m_pExcelCtrl->pVecDefSearchList = &m_VecDefSearchList ;
	int nSize = (int)m_VecDefSearchList.size() ;

	m_ExcelPrgCtrl.SetRange(0, nSize);
	m_ExcelPrgCtrl.SetPos(0);

	m_pExcelCtrl->Begin();
	// excel.SaveExeltFile( m_EdStrSavePath, m_VecDefSearchList );
}


void CExcelFileSaveDlg::CopyData(CExcelResultFileCtrl::VEC_DEF_SEARCH_LIST vecSearchList)
{
	m_VecDefSearchList = vecSearchList;
}


void CExcelFileSaveDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == T_ID_PRG_TIMER) {
		KillProgressTimer();

		//OnTimer 로그출력
		LOGDISPLAY_SPEC(6)("CExcelFileSaveDlg::OnTimer");

		if (m_pExcelCtrl != NULL) {
			int nPos = m_pExcelCtrl->m_nSaveCount;
			m_ExcelPrgCtrl.SetPos(nPos);

			if (m_pExcelCtrl->IsComplet() == TRUE) {
				m_pExcelCtrl->ResetComplet();
				MessageBox(_T("Excel 파일 저장 완료"));
				KillProgressTimer();
				OnOK();
			}
		}
		SetProgressTimer();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CExcelFileSaveDlg::SetProgressTimer()
{
	 SetTimer(T_ID_PRG_TIMER, 1000, NULL);
}
void CExcelFileSaveDlg::KillProgressTimer()
{
	KillTimer(T_ID_PRG_TIMER);
}



BOOL CExcelFileSaveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetProgressTimer();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CExcelFileSaveDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//MessageBox( _T(""))
	if (m_pExcelCtrl != NULL) {
		m_pExcelCtrl->Kill();
	}

	CDialogEx::OnCancel();
}

// 23.01.09 Ahn Add Start
void CExcelFileSaveDlg::SetSaveFilePath(CString strFilePath)
{
	m_EdStrSavePath = strFilePath ;
}
// 23.01.09 Ahn Add End
