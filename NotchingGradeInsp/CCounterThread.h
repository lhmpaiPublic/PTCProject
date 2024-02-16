#pragma once

struct MarkSendInfo
{
	int TabId;
	WORD MarkingOutputData;
	bool bSendComplate;
};

class CImageProcessCtrl;
class CCounterQueueCtrl;
class CCounterThread : public CTriggerSocketCall
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

	//동기화 이벤트 객체
	HANDLE pEvent_CounterThread;
	static std::vector<MarkSendInfo> m_MarkSendInfoData;
	static void MarkSendInfo_Push_back(int TabId, WORD MarkingOutputData, bool bSendComplate = false);

	//딮 스위치 꺼졌을 때 Output 마킹정보를 받지 못하도록 하는 플래그
	//Trigger BCD Id 가 계속적으로 들어오지 않는데 Output Marking 정보가 계속 생성되어 싸이는 것을 방지하는 플래그
	static __int64 deepSwitchOff;
protected:
	BOOL	m_bKill;
	int		m_nThreadId;
	CWinThread* m_pThread;				//!< 긚깒긞긤(CWinThread *)x1
	BOOL	m_bFreeRunn;
// 22.04.06 Ahn Add Start
	BOOL	m_bRunFlag;
// 22.04.06 Ahn Add End

	CTriggerSocket* m_TriggerSocket;

	CCounterQueueCtrl* m_pCntQueInPtr;

	//마킹 input ID 읽었을 때
	static std::vector<int> m_inputReadId;

	static CRITICAL_SECTION m_csQueueReadId;
public :	
	static UINT CtrlThreadCounter(LPVOID pParam);

	int ConnectTrigger(const CString& ip, int port, int mode = CTriggerSocket::TCP_MODE);

	virtual void RecivePacket(char* data, int len);

	static int GetTabId_FromSignal(WORD wInPutSignal);

	// 동기화 이벤트 객체
	HANDLE getEvent_CounterThread() { return pEvent_CounterThread; }
	void setEvent_CounterThread() { SetEvent(pEvent_CounterThread); }
};

