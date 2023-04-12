// LogDisplayDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "LogDisplayDlg.h"
#include "afxdialogex.h"


// CLogDisplayDlg 대화 상자

IMPLEMENT_DYNAMIC(CLogDisplayDlg, CDialogEx)

CLogDisplayDlg* CLogDisplayDlg::gInstObject = NULL;

// 로그출력여부
CMap<int, int, BOOL, BOOL> CLogDisplayDlg::m_LogPrintStatMap;

//로그출력 선택 번호
int CLogDisplayDlg::printLogNum = 0;

BOOL CLogDisplayDlg::bCreate = FALSE;

CString strLogNameList =
"0 Execute_ERROT_0 1,"
"1 PLC_Read_BitIn_1 0,"
"2 PLC_Read_BitOut_2 0,"
"3 PLC_Read_Block_3 0,"
"4 ImageProcess_TabInfo_4 0,"
"5 ImageCutting_5 0,"
"6 OnTimerLog_6 0,"
"7 Thread_OKNGMarking_7 0,"
"99 GeneralLog_99 0,"
"100 END 0"
;

CLogDisplayDlg* CLogDisplayDlg::gInst()
{
	// 로그출력 창 생성
	if (gInstObject == NULL)
	{
		gInstObject = new CLogDisplayDlg();
		if (gInstObject->Create(IDD_LOGDISPLAYDLG) == 0) {
			delete gInstObject;
			gInstObject = NULL;
		}
		else 
		{
			bCreate = TRUE;
			gInstObject->ShowWindow(SW_HIDE);
		}
	}
	return gInstObject;
}

void CLogDisplayDlg::LogDisplayMessage(const char* format, ...)
{
	va_list arg;
	int done;
	char str[MAX_PATH] = { 0, };
	va_start(arg, format);
	done = vsprintf_s(str, format, arg);
	va_end(arg);

	if (gInstObject)
	{
		gInstObject->AddLogDisplayMessage(str);
	}
	else
	{
		gInstObject = new CLogDisplayDlg();
		if (gInstObject->Create(IDD_LOGDISPLAYDLG) == 0) 
		{
			delete gInstObject;
			gInstObject = NULL;
		}
		else 
		{
			bCreate = TRUE;
			gInstObject->ShowWindow(SW_HIDE);
			gInstObject->AddLogDisplayMessage(str);
		}
	}
}

void CLogDisplayDlg::ExitLogDisplayDlg()
{
	// 로그출력 창 생성
	if (gInstObject != NULL)
	{
		delete gInstObject;
	}
}

CLogDisplayDlg::CLogDisplayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGDISPLAYDLG, pParent)
	, bLogMoveLast(TRUE)
	, m_ComboSpecialLogNameStr(_T(""))
{
}

CLogDisplayDlg::~CLogDisplayDlg()
{
	ExitThread();
}

void CLogDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListLog);
	DDX_Control(pDX, IDC_CHECK_MOVELASTLOG, m_CheckMoveLastLog);
	DDX_Control(pDX, IDC_COMBO_SPECIALLOGNAME, m_ComboSpecialLogName);
	DDX_CBString(pDX, IDC_COMBO_SPECIALLOGNAME, m_ComboSpecialLogNameStr);
	DDX_Control(pDX, IDC_CHECK_LOGSELECT, m_LogSelect);
}


BEGIN_MESSAGE_MAP(CLogDisplayDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUT_LOGCLEAR, &CLogDisplayDlg::OnBnClickedButLogclear)
	ON_BN_CLICKED(IDC_CHECK_MOVELASTLOG, &CLogDisplayDlg::OnBnClickedCheckMovelastlog)
	ON_CBN_SELCHANGE(IDC_COMBO_SPECIALLOGNAME, &CLogDisplayDlg::OnCbnSelchangeComboSpeciallogname)
	ON_BN_CLICKED(IDC_BUT_CLIPBOARDCOPY, &CLogDisplayDlg::OnBnClickedButClipboardcopy)
	ON_BN_CLICKED(IDC_CHECK_LOGSELECT, &CLogDisplayDlg::OnBnClickedCheckLogselect)
END_MESSAGE_MAP()


// CLogDisplayDlg 메시지 처리기


BOOL CLogDisplayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//source file
	m_isWorkingThread = true;

	//스래드 생성
	m_pThread = AfxBeginThread(ThreadProc, this);

	m_LogSelect.SetCheck(BST_UNCHECKED);

	//로그 끝으로 이동 기본 체크
	bLogMoveLast = TRUE;
	m_CheckMoveLastLog.SetCheck(BST_UNCHECKED);

	std::vector<CString> recVAl = StringParser(strLogNameList);

	for (int i = 0; i < recVAl.size(); i++)
	{
		std::vector<CString> subVAl = StringParser(recVAl[i], ' ');
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
	m_ListMsg.push(msg);
}
//source file
UINT CLogDisplayDlg::ThreadProc(LPVOID param)
{
	CLogDisplayDlg* pMain = (CLogDisplayDlg*)param;
	std::queue<CString>* strList = pMain->getstrLog();
	CListBox* listBox = pMain->getListBox();
	while (pMain && pMain->m_isWorkingThread)
	{
		//Do something...
		Sleep(30);
		if (CLogDisplayDlg::bCreate)
		{
			if (strList->size() && listBox->m_hWnd)
			{
				for (int i = 0; i < strList->size(); i++)
				{
					CString tempStr = strList->front();
					strList->pop();
					listBox->AddString(tempStr);
					if (*pMain->getLogMoveLast())
					{
						listBox->SetTopIndex(listBox->GetCount() - 1);
					}
				}
			}
		}
	}

	return 0;
}

//스트링 특정 char로 파서하는 함수
std::vector<CString> CLogDisplayDlg::StringParser(CString val, char s)
{
	std::vector<CString> parserVal;
	parserVal.clear();

	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, val, num++, s))
	{
		parserVal.push_back(strSub);
	}

	return parserVal;
}

void CLogDisplayDlg::ExitThread()
{
	// source file
	m_isWorkingThread = false;
	WaitForSingleObject(m_pThread->m_hThread, 5000);
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
