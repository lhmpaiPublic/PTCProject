// LogDisplayDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "LogDisplayDlg.h"
#include "afxdialogex.h"


// CLogDisplayDlg 대화 상자

IMPLEMENT_DYNAMIC(CLogDisplayDlg, CDialogEx)

CLogDisplayDlg* CLogDisplayDlg::gInstObject = NULL;
//로그 출력 여부
bool CLogDisplayDlg::bLogPrint = TRUE;

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
		else {
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
	DDX_Control(pDX, IDC_CHECK_ISLOGPRINT, m_CheckIsLogPrint);
	DDX_Control(pDX, IDC_CHECK_MOVELASTLOG, m_CheckMoveLastLog);
}


BEGIN_MESSAGE_MAP(CLogDisplayDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_ISLOGPRINT, &CLogDisplayDlg::OnBnClickedCheckIslogprint)
	ON_BN_CLICKED(IDC_BUT_LOGCLEAR, &CLogDisplayDlg::OnBnClickedButLogclear)
	ON_BN_CLICKED(IDC_CHECK_MOVELASTLOG, &CLogDisplayDlg::OnBnClickedCheckMovelastlog)
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

	//로그 출력여부 기본 체크
	bLogPrint = FALSE;
	m_CheckIsLogPrint.SetCheck(FALSE);

	//로그 끝으로 이동 기본 체크
	bLogMoveLast = TRUE;
	m_CheckMoveLastLog.SetCheck(TRUE);

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

	return 0;
}

void CLogDisplayDlg::ExitThread()
{
	// source file
	m_isWorkingThread = false;
	WaitForSingleObject(m_pThread->m_hThread, 5000);
}



void CLogDisplayDlg::OnBnClickedCheckIslogprint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_CheckIsLogPrint.GetCheck() == BST_CHECKED)
	{
		bLogPrint = TRUE;
	}
	else
	{
		bLogPrint = FALSE;
	}
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
