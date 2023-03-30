// LogDisplayDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "LogDisplayDlg.h"
#include "afxdialogex.h"


// CLogDisplayDlg 대화 상자

IMPLEMENT_DYNAMIC(CLogDisplayDlg, CDialogEx)

CLogDisplayDlg* CLogDisplayDlg::gInstObject = NULL;
CLogDisplayDlg::CLogDisplayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGDISPLAYDLG, pParent)
{
}

void CLogDisplayDlg::ExitLogDisplayDlg()
{
	// 로그출력 창 생성
	if (gInstObject != NULL)
	{
		delete gInstObject;
	}
}

CLogDisplayDlg::~CLogDisplayDlg()
{
	ExitThread();
}

void CLogDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListLog);
}


BEGIN_MESSAGE_MAP(CLogDisplayDlg, CDialogEx)
END_MESSAGE_MAP()


// CLogDisplayDlg 메시지 처리기


BOOL CLogDisplayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//source file
	m_isWorkingThread = true;

	m_pThread = AfxBeginThread(ThreadProc, this);

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
			}
		}
	}

	return 0;
}

void CLogDisplayDlg::LogDisplayMessage(const char* format, ...)
{
	va_list arg;
	int done;
	char str[MAX_PATH] = { 0, };
	va_start(arg, format);
	done = vsprintf_s(str, format, arg);
	va_end(arg);
	AddLogDisplayMessage(str);
}

void CLogDisplayDlg::ExitThread()
{
	// source file
	m_isWorkingThread = false;
	WaitForSingleObject(m_pThread->m_hThread, 5000);
}