#pragma once


// CLogDisplayDlg 대화 상자
#include <queue>
class CLogDisplayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogDisplayDlg)

	static CLogDisplayDlg* gInstObject;
	std::queue<CString> m_ListMsg;
public:
	CLogDisplayDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLogDisplayDlg();
	static CLogDisplayDlg* gInst() 
	{
		// 로그출력 창 생성
		if(gInstObject == NULL)
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
	//LogDlg 메모리 해제
	static void ExitLogDisplayDlg();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGDISPLAYDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	CWinThread* m_pThread;
	bool m_isWorkingThread;
public:
	static UINT ThreadProc(LPVOID param);
	void ExitThread();
	CListBox m_ListLog;
	std::queue<CString>* getstrLog() { return &m_ListMsg; }
	CListBox* getListBox() { return &m_ListLog; }
	virtual BOOL OnInitDialog();

	void AddLogDisplayMessage(CString msg);

	void LogDisplayMessage(const char* format, ...);
};
