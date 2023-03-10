// CDeleteSettingDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CDeleteSettingDlg.h"
#include "afxdialogex.h"
#include "SetValue.h"
#include "GlobalData.h"
#include "CResultFileManager.h"
#include "Win32File.h"
#include "CDeleteSetting.h"

// CDeleteSettingDlg 대화 상자

IMPLEMENT_DYNAMIC(CDeleteSettingDlg, CDialogEx)

CDeleteSettingDlg::CDeleteSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DELETE_SETTING, pParent)
	, m_bChkBackupNotUse(FALSE)
	, m_bChkStopDuringRun(FALSE)
	, m_strEdBackupPath(_T(""))
	, m_nEdBackupDate(0)
	, m_nEdDeleteDate(0)
	, m_nEdStartTime(0)
	, m_nEdEndTime(0)
{

}

CDeleteSettingDlg::~CDeleteSettingDlg()
{
}

void CDeleteSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_NOTUSE_BACKUP, m_bChkBackupNotUse);
	DDX_Check(pDX, IDC_CHK_STOP_DURING_RUN, m_bChkStopDuringRun);
	DDX_Control(pDX, IDC_ED_DUMMY_D, m_EdDummy);
	DDX_Text(pDX, IDC_ED_BACKUP_DIRECTORY, m_strEdBackupPath);
	DDX_Text(pDX, IDC_ED_BACKUP_DAY, m_nEdBackupDate);
	DDX_Text(pDX, IDC_ED_DELETE_DAY, m_nEdDeleteDate);
	DDX_Text(pDX, IDC_ED_START_TIME, m_nEdStartTime);
	DDX_Text(pDX, IDC_ED_END_TIME, m_nEdEndTime);
}


BEGIN_MESSAGE_MAP(CDeleteSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDeleteSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_NOTUSE_BACKUP, &CDeleteSettingDlg::OnBnClickedChkNotuseBackup)
	ON_BN_CLICKED(IDC_CHK_STOP_DURING_RUN, &CDeleteSettingDlg::OnBnClickedChkStopDuringRun)
	ON_EN_SETFOCUS(IDC_ED_BACKUP_DIRECTORY, &CDeleteSettingDlg::OnEnSetfocusEdBackupDirectory)
	ON_EN_SETFOCUS(IDC_ED_BACKUP_DAY, &CDeleteSettingDlg::OnEnSetfocusEdBackupDay)
	ON_EN_SETFOCUS(IDC_ED_DELETE_DAY, &CDeleteSettingDlg::OnEnSetfocusEdDeleteDay)
	ON_EN_SETFOCUS(IDC_ED_START_TIME, &CDeleteSettingDlg::OnEnSetfocusEdStartTime)
	ON_EN_SETFOCUS(IDC_ED_END_TIME, &CDeleteSettingDlg::OnEnSetfocusEdEndTime)
END_MESSAGE_MAP()


// CDeleteSettingDlg 메시지 처리기
void CDeleteSettingDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//// 22.03.21 Ahn Test Start
	CResultFileManager rsltManager;
	//CString strPath;
	CResultFileManager::_VEC_DIR_DATA vecDirData;

	//strPath.Format(_T("%s"), AprData.m_strFeederPath);
	rsltManager.MakeFolderList( &vecDirData, AprData.m_strFeederPath, 90 );

	CResultFileManager::_VEC_DIR_DATA::iterator iter;
	for (iter = vecDirData.begin(); iter != vecDirData.end(); iter++) {
		// 폴더 리스트 
		CString strFilePath;
		strFilePath = iter->GetDirectory();
		CStringList strList;
		if (strFilePath.GetLength() <= 0) {
			continue ;
		}

		//CWin32File::GetFileList(strFilePath, strList, _T("*"), 0, TRUE);
		CWin32File::GetFolderList(strFilePath, strList, _T("*"), 0, TRUE, TRUE, TRUE );
		int nFileCnt = (int)strList.GetCount();
		// strFilePath.Format( _T("%s")) ;
		// 파일 검색
		// 복사경로 생성
		// 파일 복사
	}
	//// 22.03.21 Ahn Test End

	CDialogEx::OnOK();
}


BOOL CDeleteSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
		

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

int CDeleteSettingDlg::OnReflesh()
{
	UpdateData(TRUE);

	BOOL bEnable = FALSE;
	CString strName;
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_ED_BACKUP_DAY);
	CWnd* pWndPath = (CWnd*)GetDlgItem(IDC_STATIC_PATH);

	if (m_bChkBackupNotUse == TRUE) {
		bEnable = FALSE;
		strName = _T("백업&&삭제 경로");
	} else {
		bEnable = TRUE;
		strName = _T("삭제 경로");
	}
	pWnd->EnableWindow(bEnable);
	pWndPath->SetWindowText(strName);

	UpdateData(FALSE);

	return 0;
}

int CDeleteSettingDlg::DataControl(int nMode)
{
	CDeleteSetting delSet;
	if (nMode == MODE_READ) {
		delSet.FileCtrl(MODE_READ);		

		m_bChkBackupNotUse = delSet.GetUseBackup();
		m_bChkStopDuringRun = delSet.GetStopDuringRun();
		m_strEdBackupPath = delSet.GetDestPath();
		m_nEdBackupDate = delSet.GetBackupDate();
		m_nEdDeleteDate = delSet.GetDeleteDate();
		m_nEdStartTime = delSet.GetStartTime();
		m_nEdEndTime = delSet.GetStopTime();

		UpdateData(FALSE);

		OnReflesh();
	}
	else {
		delSet.SetUseBackup( m_bChkBackupNotUse ) ;
		delSet.SetStopDuringRun(m_bChkStopDuringRun );
		delSet.SetDestPath( m_strEdBackupPath );
		delSet.SetBackupDate( m_nEdBackupDate );
		delSet.SetDeleteDate( m_nEdDeleteDate );
		delSet.SetStartTime( m_nEdStartTime );
		delSet.SetStopTime( m_nEdEndTime );

		delSet.FileCtrl(MODE_WRITE);
	}
	return 0;
}

void CDeleteSettingDlg::OnBnClickedChkNotuseBackup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	UpdateData(TRUE);

	OnReflesh();
}

void CDeleteSettingDlg::OnBnClickedChkStopDuringRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	UpdateData(TRUE);


}

void CDeleteSettingDlg::OnEnSetfocusEdBackupDirectory()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	UpdateData(TRUE);

	CSetValue SetValue(this);
	CString strValue = m_strEdBackupPath ;
	CString strMsg;
	int nMin = 0;
	int nMax = 512 ;
	SetValue.SetValue(strMsg, strValue, nMax, nMin);

	m_strEdBackupPath = strValue ;
	UpdateData(FALSE);
}


void CDeleteSettingDlg::OnEnSetfocusEdBackupDay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	UpdateData(TRUE);

}


void CDeleteSettingDlg::OnEnSetfocusEdDeleteDay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	UpdateData(TRUE);
}


void CDeleteSettingDlg::OnEnSetfocusEdStartTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	UpdateData(TRUE);
}


void CDeleteSettingDlg::OnEnSetfocusEdEndTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	UpdateData(TRUE);
}
