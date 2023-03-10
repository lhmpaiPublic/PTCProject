#pragma once

class CImageProcessCtrl;
class CCounterThread
{
public:
	CCounterThread(CImageProcessCtrl* pParent);
	~CCounterThread();
	void Begin();
	void Kill(void);
	void SetFreeRun(BOOL bMode);
	CImageProcessCtrl* m_pParent;

	// 22.04.06 Ahn Add Start
	void ThreadRun(BOOL bRunFlag );
	// 22.04.06 Ahn Add End

protected:
	BOOL	m_bKill;
	int		m_nThreadId;
	CWinThread* m_pThread;				//!< ƒXƒŒƒbƒh(CWinThread *)x1
	BOOL	m_bFreeRunn;
// 22.04.06 Ahn Add Start
	BOOL	m_bRunFlag;
// 22.04.06 Ahn Add End

public :	
	static UINT CtrlThreadCounter(LPVOID pParam);

	static int GeTabIdFormCounter();
//	static WORD GetCounterSignal(int nTabId, int nJudge1, int nJudge2, int nType1, int nType2, int nMarkSel1, int nMarkSel2);
	static int GetTabId_FromSignal(WORD wInPutSignal);
};

