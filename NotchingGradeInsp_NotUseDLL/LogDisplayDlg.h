#pragma once


// CLogDisplayDlg 대화 상자
#include <queue>
class CLogDisplayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogDisplayDlg)

	//자신의 전역 객체 생성
	static CLogDisplayDlg* gInstObject;
	//로그 내용 저장용
	std::queue<CString> m_ListMsg;

	//로그 출력 여부
	static bool bLogPrint;
public:
	CLogDisplayDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLogDisplayDlg();

	//전역 인스턴스 객체 생성 및 접근
	static CLogDisplayDlg* gInst();

	//LogDlg 메모리 해제
	static void ExitLogDisplayDlg();

	//로그 메시지 추가 전역함수
	static void LogDisplayMessage(const char* format, ...);

	//로그 출력 선택
	static bool isLogPrint() { return bLogPrint; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGDISPLAYDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	//스래드 생성 인스턴스 객체
	CWinThread* m_pThread;
	//스래드를 계속 유지 여부 설정 변수
	bool m_isWorkingThread;
public:
	//로그 ListBox에 출력
	static UINT ThreadProc(LPVOID param);
	//스래드 종료함수
	void ExitThread();

	//로그 메시지 저장 객체 요구함수
	std::queue<CString>* getstrLog() { return &m_ListMsg; }

	//리스트 박스 객체 요구함수
	CListBox* getListBox() { return &m_ListLog; }

	//초기화 함수
	virtual BOOL OnInitDialog();

	//로그 메시지 추가함수
	void AddLogDisplayMessage(CString msg);

public:
	//리스트 박스 객체
	CListBox m_ListLog;

	//로그 출력여부 컨트롤 변수 및 함수
	CButton m_CheckIsLogPrint;
	afx_msg void OnBnClickedCheckIslogprint();

	//로그 내용을 지우는 함수
	afx_msg void OnBnClickedButLogclear();

	//로그 끝으로 이동 여부 함수, 변수
	afx_msg void OnBnClickedCheckMovelastlog();
	bool* getLogMoveLast() { return &bLogMoveLast; }
	bool bLogMoveLast;
	CButton m_CheckMoveLastLog;
};

#define LOGDISPLAY if(CLogDisplayDlg::isLogPrint()) CLogDisplayDlg::LogDisplayMessage

#define LOGDISPLAY_ALL CLogDisplayDlg::LogDisplayMessage
