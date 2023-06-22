// LogDisplayDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "LogDisplayDlg.h"
#include "afxdialogex.h"
#include "Win32File.h"

//유저메시지 - Log 화면 출력 메시지
#define  WM_LOGMSGPRINT WM_USER + 12

#define LOGTEXTFILENAME "NotchingGradeInsp.txt"

#define LOGTEXTFILEFOLDER CString("NotchingLog")

// CLogDisplayDlg 대화 상자

IMPLEMENT_DYNAMIC(CLogDisplayDlg, CDialogEx)

CLogDisplayDlg* CLogDisplayDlg::gInstObject = NULL;

// 로그출력여부
CMap<int, int, BOOL, BOOL> CLogDisplayDlg::m_LogPrintStatMap;

//로그출력 선택 번호
int CLogDisplayDlg::printLogNum = 0;

BOOL CLogDisplayDlg::bCreate = FALSE;

CRITICAL_SECTION CLogDisplayDlg::m_csQueueLog;

CString strLogNameList =
"0 Init_FromExecute_Error_0 1,"
"1 TabID_ImageCount_TabCount_1 1,"
"2 PLC_Live_BitData_2 0,"
"3 SPCPlus_3 1,"
"4 Non_4 0,"
"5 Image_TabFind_ToResult_5 0,"
"6 InspView_TimerPorcStatus_6 0,"
"100 END 0"
;

#define LOGDISPLAY_LISTBOX
#define MAX_DISPLAYLOG 1024
#define LISTBOX_CLEARCOUNT 5000
CLogDisplayDlg* CLogDisplayDlg::gInst()
{
	return gInstObject;
}

void CLogDisplayDlg::LogDisplayMessage(const char* format, ...)
{
	va_list arg;
	int done;
	char str[MAX_DISPLAYLOG] = { 0, };
	va_start(arg, format);
	done = vsprintf_s(str, format, arg);
	va_end(arg);
	CString strData;

	SYSTEMTIME	sysTime;
	::GetLocalTime(&sysTime);

	strData.Format(_T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d :: Log = %s")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
		, str
	);

	if (gInstObject)
	{
		gInstObject->AddLogDisplayMessage(strData);
	}
}

void CLogDisplayDlg::LogDisplayMessageText(const char* data)
{
	CString strData;

	SYSTEMTIME	sysTime;
	::GetLocalTime(&sysTime);

	strData.Format(_T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d :: Log = %s")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
		, data
	);

	if (gInstObject)
	{
		gInstObject->AddLogDisplayMessage(strData);
	}
}

void CLogDisplayDlg::CreateLogDisplayDlg()
{
	// 로그출력 창 생성
	if (gInstObject == NULL)
	{
		gInstObject = new CLogDisplayDlg();
		gInstObject->Create(IDD_LOGDISPLAYDLG);
		bCreate = true;
	}
}

void CLogDisplayDlg::ExitLogDisplayDlg()
{
	// 로그출력 창 생성
	if (gInstObject != NULL)
	{
		bCreate = false;
		delete gInstObject;
		gInstObject = NULL;
	}
}

CLogDisplayDlg::CLogDisplayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGDISPLAYDLG, pParent)
	, bLogMoveLast(TRUE)
	, m_ComboSpecialLogNameStr(_T(""))
{
	::InitializeCriticalSection(&m_csQueueLog);
}

CLogDisplayDlg::~CLogDisplayDlg()
{
	getListBox()->m_hWnd = NULL;
	::DeleteCriticalSection(&m_csQueueLog);
}

void CLogDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListLog);
	DDX_Control(pDX, IDC_CHECK_MOVELASTLOG, m_CheckMoveLastLog);
	DDX_Control(pDX, IDC_COMBO_SPECIALLOGNAME, m_ComboSpecialLogName);
	DDX_CBString(pDX, IDC_COMBO_SPECIALLOGNAME, m_ComboSpecialLogNameStr);
	DDX_Control(pDX, IDC_CHECK_LOGSELECT, m_LogSelect);
	DDX_Control(pDX, IDC_CHECK_TextPrint, m_checkTextPrint);
}


BEGIN_MESSAGE_MAP(CLogDisplayDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUT_LOGCLEAR, &CLogDisplayDlg::OnBnClickedButLogclear)
	ON_BN_CLICKED(IDC_CHECK_MOVELASTLOG, &CLogDisplayDlg::OnBnClickedCheckMovelastlog)
	ON_CBN_SELCHANGE(IDC_COMBO_SPECIALLOGNAME, &CLogDisplayDlg::OnCbnSelchangeComboSpeciallogname)
	ON_BN_CLICKED(IDC_BUT_CLIPBOARDCOPY, &CLogDisplayDlg::OnBnClickedButClipboardcopy)
	ON_BN_CLICKED(IDC_CHECK_LOGSELECT, &CLogDisplayDlg::OnBnClickedCheckLogselect)
	ON_BN_CLICKED(IDC_CHECK_TextPrint, &CLogDisplayDlg::OnBnClickedCheckTextprint)
	ON_MESSAGE(WM_LOGMSGPRINT, &CLogDisplayDlg::OnLogMsgPrint)
END_MESSAGE_MAP()


// CLogDisplayDlg 메시지 처리기


BOOL CLogDisplayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//source file
	m_isWorkingThread = true;

	//스래드 생성
	m_pThread = AfxBeginThread(ThreadProc, this);

	m_LogSelect.SetCheck(BST_UNCHECKED);

	//로그 끝으로 이동 기본 체크
	bLogMoveLast = TRUE;
	m_CheckMoveLastLog.SetCheck(BST_CHECKED);
	//Text Log 출력 여부
	m_checkTextPrint.SetCheck(BST_CHECKED);
	bTextLogPrint = true;

	std::vector<CString> recVAl = CGlobalFunc::StringParser(strLogNameList, ',');

	for (int i = 0; i < recVAl.size(); i++)
	{
		std::vector<CString> subVAl = CGlobalFunc::StringParser(recVAl[i], ' ');
		if (subVAl.size() >= 2)
		{
			int num = _ttoi(subVAl[0]);
			if (num == 100)
			{
				m_ComboSpecialLogName.SetCurSel(0);
				BOOL val = FALSE;
				m_LogPrintStatMap.Lookup(0, val);

				if (val)
				{
					m_LogSelect.SetCheck(BST_CHECKED);
				}
				else
				{
					m_LogSelect.SetCheck(BST_UNCHECKED);
				}
				break;
			}
			else
			{
				m_ComboSpecialLogName.AddString(subVAl[1]);
				m_LogNameNumber.push_back(num);
				m_LogNameMap.SetAt(subVAl[1], num);
				if(subVAl.size() >= 3)
					m_LogPrintStatMap.SetAt(num, _ttoi(subVAl[2]));
				else
					m_LogPrintStatMap.SetAt(num, FALSE);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLogDisplayDlg::AddLogDisplayMessage(CString msg)
{
	// TODO: 여기에 구현 코드 추가.
	if (bCreate)
	{
		::EnterCriticalSection(&m_csQueueLog);
		m_ListMsg.push(msg);
		::LeaveCriticalSection(&m_csQueueLog);
	}

}

CString CLogDisplayDlg::GetLogDisplayMessage()
{
	// TODO: 여기에 구현 코드 추가.
	CString tempStr = "";
	if (bCreate)
	{
		tempStr = m_ListMsg.front();
		::EnterCriticalSection(&m_csQueueLog);
		m_ListMsg.pop();
		::LeaveCriticalSection(&m_csQueueLog);
	}
	return tempStr;
}

//source file
UINT CLogDisplayDlg::ThreadProc(LPVOID param)
{
	CLogDisplayDlg* pMain = (CLogDisplayDlg*)param;
	std::queue<CString>* strList = pMain->getstrLog();
	CListBox* listBox = pMain->getListBox();

	CString FilePath;
	char	LogTextpath[_MAX_PATH];
	memset(LogTextpath, 0x00, sizeof(LogTextpath));
	::GetCurrentDirectory(_MAX_PATH, LogTextpath);
	FilePath.Format(_T("%s\\%s")
		, LogTextpath
		, LOGTEXTFILEFOLDER
	);

	CWin32File file;
	while (pMain && pMain->m_isWorkingThread)
	{
		//Do something...
		Sleep(1);
		if (CLogDisplayDlg::bCreate && strList->size() && listBox->m_hWnd)
		{
			CString tempStr = "";
			for (int i = 0; i < strList->size(); i++)
			{
				CString popStr = pMain->GetLogDisplayMessage();
#ifdef LOGDISPLAY_LISTBOX
				CString* pSendStr = new CString(popStr);
				pMain->PostMessage(WM_LOGMSGPRINT, (WPARAM)pSendStr);
#endif //LOGDISPLAY_LISTBOX
				tempStr += popStr + CString("\r\n");
			}

			//텍스트 로그 출력
			if (pMain->getTextLogPrint())
			{

				CString FileName;
				SYSTEMTIME	sysTime;
				::GetLocalTime(&sysTime);

				FileName.Format(_T("%s-%04d%02d%02d-%02d-%02d.txt")
					, LOGTEXTFILENAME
					, sysTime.wYear
					, sysTime.wMonth
					, sysTime.wDay
					, sysTime.wHour
					, sysTime.wMinute
				);

				file.TextSave1Line(FilePath, FileName, tempStr, "at", FALSE, 999999999);
			}
		}
	}

	return 0;
}

void CLogDisplayDlg::ExitThread()
{
	// source file
	m_isWorkingThread = false;
	CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
	//WaitForSingleObject(m_pThread->m_hThread, 5000);
}



void CLogDisplayDlg::OnBnClickedButLogclear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 전체 내용 삭제
	m_ListLog.ResetContent();
}


void CLogDisplayDlg::OnBnClickedCheckMovelastlog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_CheckMoveLastLog.GetCheck() == BST_CHECKED)
	{
		bLogMoveLast = TRUE;
	}
	else
	{
		bLogMoveLast = FALSE;
	}
}


void CLogDisplayDlg::OnCbnSelchangeComboSpeciallogname()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//선택된 인덱스 얻기
	int index = m_ComboSpecialLogName.GetCurSel();
	CString strData;
	m_ComboSpecialLogName.GetLBText(index, strData);
	BOOL val = FALSE;
	m_LogPrintStatMap.Lookup(m_LogNameMap[strData], val);

	if (val)
	{
		m_LogSelect.SetCheck(BST_CHECKED);
	}
	else
	{
		m_LogSelect.SetCheck(BST_UNCHECKED);
	}
}

void CLogDisplayDlg::CopyStrToClipboard(CString str)
{
	HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (str.GetLength() + 1) * sizeof(TCHAR));
	PSTR pGlobal = (PSTR)GlobalLock(hGlobal);
	lstrcpy(pGlobal, TEXT(str));
	GlobalUnlock(hGlobal);

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}

void CLogDisplayDlg::OnBnClickedButClipboardcopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CStringA ansi_str; // ASCII 형식으로 문자열을 변경하기 위해서 사용
	CString str, total_str;
	// 리스트 박스에 추가된 항목의 개수를 얻는다.
	int count = m_ListLog.GetCount();
	for (int i = 0; i < count; i++) {
		// 리스트 박스의 각 항목에서 URL을 가져온다.
		m_ListLog.GetText(i, str);
		// 각 항목의 URL을 하나의 문자열로 합친다.
		total_str += str;
		// 각 URL을 구분하기 위해서 개행문자를 추가한다.
		total_str += L"\r\n";
	}
	// total_str에 합쳐진 URL 문자열은 유니코드 형식이기 때문에
	// ASCII 형식으로 변경해서 ansi_str에 넣습니다.
	ansi_str = total_str;

	CopyStrToClipboard(ansi_str);
}


void CLogDisplayDlg::OnBnClickedCheckLogselect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//선택된 인덱스 얻기
	int index = m_ComboSpecialLogName.GetCurSel();
	CString strData;
	m_ComboSpecialLogName.GetLBText(index, strData);
	int val = m_LogNameMap[strData];

	if (m_LogSelect.GetCheck() == BST_CHECKED)
	{
		m_LogPrintStatMap.SetAt(val, TRUE);
	}
	else
	{
		m_LogPrintStatMap.SetAt(val, FALSE);
	}

}


void CLogDisplayDlg::OnBnClickedCheckTextprint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_checkTextPrint.GetCheck() == BST_CHECKED)
	{
		bTextLogPrint = TRUE;
	}
	else
	{
		bTextLogPrint = FALSE;
	}
}

LRESULT CLogDisplayDlg::OnLogMsgPrint(WPARAM wParam, LPARAM lParam)
{
	CString* logStr = (CString*)wParam;
	if (logStr)
	{
		m_ListLog.AddString(*logStr);
		delete logStr;
		if (getLogMoveLast())
		{
			m_ListLog.SetTopIndex(m_ListLog.GetCount() - 1);
		}
		if (m_ListLog.GetCount() >= LISTBOX_CLEARCOUNT)
		{
			m_ListLog.ResetContent();
		}
	}
	return 0;
}