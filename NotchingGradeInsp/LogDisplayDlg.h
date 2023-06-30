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

	//로그출력 선택 번호
	static int printLogNum;

	//로그 명의 콤보 셀번호의 번호
	std::vector<int> m_LogNameNumber;
	//로그 명으로 로그 번호 찾기
	CMap<CString, LPCTSTR, int, int> m_LogNameMap;
	//로그 출력 상태 저장
	static CMap<int, int, BOOL, BOOL> m_LogPrintStatMap;

	static BOOL bCreate;

	//동기화 이벤트 객체
	HANDLE pEvent_LogDisplayDlg;
public:
	CLogDisplayDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLogDisplayDlg();

	//전역 인스턴스 객체 생성 및 접근
	static CLogDisplayDlg* gInst();

	//LogDlg 메모리 해제
	static void CreateLogDisplayDlg();

	//LogDlg 메모리 해제
	static void ExitLogDisplayDlg();

	//로그 메시지 추가 전역함수
	static void LogDisplayMessage(const char* format, ...);

	//로그 메시지 추가 전역함수
	static void LogDisplayMessageText(const char* data);

	//로그 번호
	static int getLogNumber(int a) { return m_LogPrintStatMap[a]; }

	static CRITICAL_SECTION m_csQueueLog;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGDISPLAYDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	//스래드 생성 인스턴스 객체
	CWinThread* m_pThread;
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

	CString GetLogDisplayMessage();

	//클립보드 복사
	void CopyStrToClipboard(CString str);

	// 동기화 이벤트 객체
	HANDLE getEvent_LogDisplayDlg(){ return pEvent_LogDisplayDlg; }
	void setEvent_LogDisplayDlg() { SetEvent(pEvent_LogDisplayDlg); }
public:
	//리스트 박스 객체
	CListBox m_ListLog;

	//로그 내용을 지우는 함수
	afx_msg void OnBnClickedButLogclear();

	//로그 끝으로 이동 여부 함수, 변수
	afx_msg void OnBnClickedCheckMovelastlog();
	bool* getLogMoveLast() { return &bLogMoveLast; }
	bool bLogMoveLast;
	CButton m_CheckMoveLastLog;
	CComboBox m_ComboSpecialLogName;
	CString m_ComboSpecialLogNameStr;
	afx_msg void OnCbnSelchangeComboSpeciallogname();
	afx_msg void OnBnClickedButClipboardcopy();
	CButton m_LogSelect;
	afx_msg void OnBnClickedCheckLogselect();
	CButton m_checkTextPrint;
	afx_msg void OnBnClickedCheckTextprint();
	//텍스트 로그 출력 여부
	bool* getTextLogPrint() { return &bTextLogPrint; }
	bool bTextLogPrint;
	afx_msg LRESULT OnLogMsgPrint(WPARAM wParam, LPARAM lParam);
};

//로그 항상 출력(초기화 또는 버튼, 에러 같은 특정 확인용)
#define LOGDISPLAY_ALL CLogDisplayDlg::LogDisplayMessage

//특정 설정 로그 출력
#define LOGDISPLAY_SPEC(a) if(TRUE == CLogDisplayDlg::getLogNumber(a)) CLogDisplayDlg::LogDisplayMessage

//로그 출력 설정 체크
#define LOGDISPLAY_CHECK(a) if(TRUE == CLogDisplayDlg::getLogNumber(a))

//로그 항상 출력(초기화 또는 버튼, 에러 같은 특정 확인용)
#define LOGDISPLAY_ALLTXT CLogDisplayDlg::LogDisplayMessageText

//특정 설정 로그 출력
#define LOGDISPLAY_SPECTXT(a) if(TRUE == CLogDisplayDlg::getLogNumber(a)) CLogDisplayDlg::LogDisplayMessageText
