
// NotchingGradeInsp.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"

#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "ImageProcessCtrl.h"
#include "PioCtrl.h"
#include "StdIoCtrl.h"
#include "CImageProcSimDlg.h"
#include "CLightControl.h"
#include "GlobalData.h"
#include "CDebugSetDlg.h"
#include "CStartDlg.h"
#include "SigProc.h" // 22.04.01 Ahn Add
#include "CDispErrorDlg.h"	// 22.06.29 Son Add
#include "Win32File.h" // 22.11.25 Ahn Add
#include "resource.h"
#include "SpcCreateJSONFileThread.h"
#include "SpcInfo.h"

#pragma comment( lib, "version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNotchingGradeInspApp

BEGIN_MESSAGE_MAP(CNotchingGradeInspApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CNotchingGradeInspApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// CNotchingGradeInspApp 생성

CNotchingGradeInspApp::CNotchingGradeInspApp() noexcept
{
	//AfxSetAllocStop(1192);
	m_bHiColorIcons = TRUE;

	m_nAppLook = 0;
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 애플리케이션을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 애플리케이션 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("NotchingGradeInsp.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.

	m_pImgProcCtrl = NULL;
	m_pPioCtrl = NULL;
	m_pImageSimDlg = NULL;
	m_pLightCtrl = NULL;
}

// 유일한 CNotchingGradeInspApp 개체입니다.

CNotchingGradeInspApp theApp;


// CNotchingGradeInspApp 초기화

#define NOTCHINGAPP_NAME _T("NotchingGradeInsp")
BOOL CNotchingGradeInspApp::InitInstance()
{
	//중복 실행 방지를 위한 Mutex 생성
	//App Name를 가져온다.
	CString AppName = AfxGetAppName();
	//App Name 에 대한 뮤텍스 생성
	HANDLE hMutex = CreateMutex(NULL, TRUE, AppName); // Mutex Name
	//App Name를 가지는 윈도우가 실행되어 있으면 ERROR_ALREADY_EXISTS 에러가 발생한다.
	DWORD errorCode = GetLastError();
	//생성한 뮤텍스를 제거한다.
	ReleaseMutex(hMutex);

	//프로그램이 이미 실행되어 있으면
	if (errorCode == ERROR_ALREADY_EXISTS)
	{
		//실행 메시지 출력
		AfxMessageBox(_T("이미 프로그램이 실행중입니다\r\n프로그램 실행을 확인하세요"));

		//메인 프레임 윈도우 명과 같은 윈도우 를 찾는다.
		//실행된 윈도우를 활성화 시키기 위한 부분
		CWnd* pWndPrev = CWnd::FindWindow(NULL, NOTCHINGMAINFRM_NAME); // Find using Window Caption
		if (pWndPrev)
		{
			//윈도우를 찾았으면 메시지 출력하고
			AfxMessageBox(_T("이미 프로그램이 실행중입니다\r\n프로그램 실행을 확인하세요"));
			//Ionic 화 되었나 확인을 위한 - 자식 객체를 찾는다
			CWnd* pWndChild = pWndPrev->GetLastActivePopup();
			//Iconic화 되어 있으면 해제 시키고
			if (pWndChild->IsIconic())
				pWndPrev->ShowWindow(SW_RESTORE);
			//자식 객체를 최상단으로 오도록 한다.
			pWndChild->SetForegroundWindow();
		}
		//중복 실행이 되지 않도록 프로그램은 중지하기 위해서 FALSE로 함수를 빠져나간다.
		return FALSE;
	}

	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, 0);
	}

	//로그창을 이용한 출력 기능 모듈을 생성한다.
	CLogDisplayDlg::CreateLogDisplayDlg();

	m_nImageProcGrabberImageGet = GetTickCount();
	m_nImageProcImageTabFind = GetTickCount();
	m_nImageProcInspDataGet = GetTickCount();
	m_nImageProcResultProcPush = GetTickCount();
	m_nImageProcResultProcCrop = GetTickCount();
	m_nImageProcResultImageSave = GetTickCount();

	LOGDISPLAY_SPEC(8)(_T("<<<<<<<<<<<<<<<<<<ImageProcThreadTime>>>>>>>>>>>>>>>> init proc time<%d>"),
		m_nImageProcGrabberImageGet);


//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	//전역 객체 생성
	objSpcInfo = new CSpcInfo();
	//SPC+ JSON파일 생성을 위한 스래드 모듈을 생성한다.
	CSpcCreateJSONFileThread::CreateSpcCreateJSONFileThread();
#endif //SPCPLUS_CREATE

	//CDebugSetDlg DebugSet;
	//if (DebugSet.DoModal() == IDOK) {
	//}
	CStartDlg dlg;
	if (dlg.DoModal() == IDCANCEL) {
		// 종료
		return FALSE;
	}

	// 22.06.29 Son Add Start
	m_pDispErrorDlg = new CDispErrorDlg(m_pMainWnd);
	m_pDispErrorDlg->Create(IDD_DLG_DISPERROR, m_pMainWnd);
	m_pDispErrorDlg->ShowWindow(SW_HIDE);
	// 22.06.29 Son Add End

	DeviceOpen();

	// 애플리케이션의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNotchingGradeInspDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CNotchingGradeInspView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//KANG 21.07.21 Add Start
	CWinAppEx::CleanState();
	//KANG 21.07.21 Add End

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->SetWindowText(NOTCHINGMAINFRM_NAME);

	//실행 파일 컴파일 정보
	CString strVersion;
	GetFileVersion(strVersion);

	CString ErrorLog;
	ErrorLog.Format(_T("============================= START PROGRAM (%s) ============================="), strVersion);
	AprData.SaveErrorLog(ErrorLog);
	AprData.SaveDebugLog_Format(ErrorLog);

	LOGDISPLAY_SPEC(8)(_T("============================= START PROGRAM (%s) ============================="), strVersion);


	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// 22.11.25 Ahn Add Start
	LoadLastDefList();
	// 22.11.25 Ahn Add End

	return TRUE;
}

int CNotchingGradeInspApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.

	DeviceClose();

	// 22.06.29 Son Add Start
	if (m_pDispErrorDlg != NULL) {
		if (m_pDispErrorDlg->m_hWnd != nullptr) {
			m_pDispErrorDlg->DestroyWindow();
		}
		delete m_pDispErrorDlg;
		m_pDispErrorDlg = NULL;
	}
	// 22.06.29 Son Add End

	AfxOleTerm(FALSE);

	GdiplusShutdown(m_gdiplusToken);

	//생성된 로그창 기능 모듈을 소멸한다.
	CLogDisplayDlg::ExitLogDisplayDlg();

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	//SPC+ JSON파일 생성을 위한 스래드 모듈을 소멸한다.
	CSpcCreateJSONFileThread::ExitSpcCreateJSONFileThread();
	//전역 객체 소멸
	delete objSpcInfo;
#endif //SPCPLUS_CREATE

	//사용한 Image 메모리풀 해제
	FrameImagePtr::ReleasePool();

	return CWinAppEx::ExitInstance();
}

// CNotchingGradeInspApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()

};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CNotchingGradeInspApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CNotchingGradeInspApp 사용자 지정 로드/저장 방법

void CNotchingGradeInspApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CNotchingGradeInspApp::LoadCustomState()
{
}

void CNotchingGradeInspApp::SaveCustomState()
{
}

// CNotchingGradeInspApp 메시지 처리기

int CNotchingGradeInspApp::DeviceOpen(void)
{
	CoInitialize(NULL);

	m_pImgProcCtrl = new CImageProcessCtrl();
	m_pImgProcCtrl->Initialize(NULL, 0);
	m_pImgProcCtrl->Initialize(NULL, 1);
	// 22.12.01 Ahn Modify Start
	m_pImgProcCtrl->SetFrameCount(AprData.m_NowLotData.m_nFrameCount);
	// 22.12.01 Ahn Modify End


	if (m_pPioCtrl != NULL) {
		delete m_pPioCtrl;
		m_pPioCtrl = NULL;
	}
	if (m_pPioCtrl == NULL)
	{
		if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == FALSE)
		{
			m_pPioCtrl = new CPioCtrl(CHN_NO_NETG1, 2, 1);
		}

		// 22.04.01 Ahn Add Start
		m_pSigProc = new CSigProc();
		// 22.04.01 Ahn Add End


	}

	CStdIoCtrl::eIOTYPE ioType = CStdIoCtrl::eIO_AXL;
	m_pIoCtrl = CStdIoCtrl::GetInstance(ioType, 2, 0, 0, 0);
	if (m_pIoCtrl == NULL) {
		return (-1);
	}

	BOOL bDebug = FALSE; // TEST
	CString strMsg; // 22.10.17 Ahn Add Start
	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_DIO) == FALSE) {
		if (m_pIoCtrl->Open(bDebug) == -1) {
			// 22.10.17 Ahn Add Start
			strMsg.Format(_T("I/O Board initialize failed."));
			AprData.m_ErrStatus.SetError(CErrorStatus::en_DioError, strMsg);
			// 22.10.17 Ahn Add End
			return (-1);
		}
	}

	if (m_pLightCtrl == NULL) {
		m_pLightCtrl = new CLightControl();
		if (m_pLightCtrl->Open() < 0) {
			// 22.10.17 Ahn Add Start
			strMsg.Format(_T("Light control connect error."));
			AprData.m_ErrStatus.SetError(CErrorStatus::en_LampError, strMsg);
			// 22.10.17 Ahn Add End
		}

		if (AprData.m_System.m_nRS232_Mode == 1);
		else m_pLightCtrl->Close(); //231102 // TCP/IP 일때만 close 함
	}

	return (0);
}

int CNotchingGradeInspApp::DeviceClose(void)
{
	AprData.SaveDebugLog_Format(_T("DeviceClose... Start") );

	if (m_pImgProcCtrl != NULL) {
		delete m_pImgProcCtrl;
		m_pImgProcCtrl = NULL;
	}

	// 22.04.01 Ahn Add Start
	if (m_pSigProc != NULL) {
		delete m_pSigProc;
		m_pSigProc = NULL;
	}
	// 22.04.01 Ahn Add End

	if (m_pPioCtrl != NULL) {
		delete m_pPioCtrl;
		m_pPioCtrl = NULL;
	}

	if (m_pIoCtrl != NULL) {
		delete m_pIoCtrl;
		m_pIoCtrl = NULL;
	}

	if (m_pImageSimDlg != NULL) {
		m_pImageSimDlg->DestroyWindow();
		delete m_pImageSimDlg;
		m_pImageSimDlg = NULL;
	}

	if (m_pLightCtrl != NULL) { 
		m_pLightCtrl->Close();
		delete m_pLightCtrl;
		m_pLightCtrl = NULL;
	}

	CoUninitialize();

	AprData.SaveDebugLog_Format(_T("DeviceClose... End"));

	return (0);
}


int CNotchingGradeInspApp::GetFileVersion(CString& strVersion)
{
	int vMj, vMn, vRv, vBd;
	char	fname[1000];
	::GetModuleFileName(theApp.m_hInstance, fname, 1000);

	VS_FIXEDFILEINFO vsf;
	DWORD dwHandle;
	DWORD cchver = GetFileVersionInfoSize(fname, &dwHandle);
	if (cchver == 0)
		return -1;
	char* pver = new char[cchver];
	BOOL bret = GetFileVersionInfo(fname, dwHandle, cchver, pver);
	if (!bret)
		return -1;
	UINT uLen;
	void* pbuf;
	bret = VerQueryValue(pver, _T("\\"), &pbuf, &uLen);
	if (!bret)
		return -1; 
	memcpy(&vsf, pbuf, sizeof(VS_FIXEDFILEINFO));
	delete[] pver;
	vMj = HIWORD(vsf.dwFileVersionMS);
	vMn = LOWORD(vsf.dwFileVersionMS);
	vRv = HIWORD(vsf.dwFileVersionLS);
	vBd = LOWORD(vsf.dwFileVersionLS);

	strVersion.Format(_T("%d-%d-%d-%d Build:%s"), vMj, vMn, vRv, vBd, GetAppBuildInfo());

	return 0;
}


CString& CNotchingGradeInspApp::GetAppBuildInfo(void)
{
	TCHAR	szAppName[MAX_PATH];
	::GetModuleFileName(NULL, szAppName, MAX_PATH);

	CFileStatus	FileStatus;
	CFile::GetStatus(szAppName, FileStatus);	

	static CString strBuildInfo;
	strBuildInfo = FileStatus.m_mtime.Format(_T("%y/%m/%d %H:%M:%S"));	

#if defined(_DEBUG)
	strBuildInfo += _T(" [Debug]");
#else
	strBuildInfo += _T(" [Release]");
#endif	// _DEBUG

	return strBuildInfo;
}

////KANG 22.01.07 Add Start
//CResultView* CNotchingGradeInspApp::GetResultViewPrt()
//{
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	if (pMain != NULL) {
//		return pMain->GetResultViewPtr();
//	}
//	else {
//		return NULL;
//	}
//}
////KANG 22.01.07 Add End

// 22.06.29 Son Add Start
int CNotchingGradeInspApp::ErrOutput(LPCSTR ptstr, int nErrorType )
{
	CSingleLock lock(&m_csError, TRUE);

	CString str = ptstr;
	switch (nErrorType) {
	case	CErrorStatus::en_DiskCapacityAlarm :
		m_pSigProc->SigOutDiskCapacityAlarm( TRUE );
		break;
	case	CErrorStatus::en_DiskCapacityWarning:
		m_pSigProc->SigOutDiskCapacityWarning(TRUE);
		break;
	default: 
		break;
	}

	if (m_pDispErrorDlg != NULL) {
		if (m_pDispErrorDlg->AddMessage(str) == FALSE) {
			return -1;
		}
	}
	return 0;
}
// 22.06.29 Son Add End

// 22.11.25 Ahn Add Start
int CNotchingGradeInspApp::LoadLastDefList()
{
	CDefectDataCtrl *pDefCtrl = m_pImgProcCtrl->GetDefectDataCtrlPtr();
	CString strFileName;
	CDefectDataCtrl::_VEC_DEFECT_LIST* pDefList;
	int nRet = 0; 
	AprData.m_NowLotData.m_strLotNo;
	strFileName = AprData.GetBinaryFileName(AprData.m_NowLotData.m_strLotNo) ;

	if (CWin32File::ExistFile(strFileName) == TRUE) {
		if (pDefCtrl != NULL) {
			pDefList = &(pDefCtrl->m_DrawDefList);
			nRet = pDefCtrl->LoadDefectInfo(strFileName, *pDefList);
		}
	}

	return nRet ;
}
// 22.11.25 Ahn Add End

void CNotchingGradeInspApp::ProgramVersionInfo()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}