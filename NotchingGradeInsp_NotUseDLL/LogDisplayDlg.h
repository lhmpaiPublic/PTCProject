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
	static CLogDisplayDlg* gInst() { return gInstObject; }
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
};
