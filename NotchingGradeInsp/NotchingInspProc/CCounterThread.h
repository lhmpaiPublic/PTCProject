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

	//마킹 데이터 동기화
	static CRITICAL_SECTION m_csQueueMarkingData;
	//마킹 데이터 정보
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
	BOOL	m_bRunFlag;

	CTriggerSocket* m_TriggerSocket;

	//마킹 input ID 읽었을 때
	static std::vector<int> m_inputReadId;
	//마킹 input ID 동기화 객체
	static CRITICAL_SECTION m_csQueueReadId;

	//CountBord Connect 상태 변수
	BOOL m_bCountBordConnection;
public :	
	//Trigger, Marking 스래드 함수
	static UINT CtrlThreadCounter(LPVOID pParam);

	//input ID를 가져온다.
	static int GetInputReadId();

	//카운터 보드 Socket 생성 및 접속 함수
	int ConnectTrigger(const CString& ip, int port, int mode = CTriggerSocket::TCP_MODE);
	//카운터 보드 소켓 통신 끊김 확인 함수
	void isConnectTrigger();

	//카운터 보드 socket Recive 호출 데이터 수신 함수
	//생성된 socket 클래스에서 데이터가 수신되면 호출된다
	virtual void RecivePacket(char* data, int len);

	//카운터 보드 socket OnConnect 시 호출 함수
	virtual void OnConnectSocket(BOOL bConnected);

	//CountBord Connect 상태
	BOOL isCountBordConnection() { return m_bCountBordConnection; }


	//Trigger 시그널 테스트 함수
	static int GetTabId_FromSignal(WORD wInPutSignal);

	// 동기화 이벤트 객체
	HANDLE getEvent_CounterThread() { return pEvent_CounterThread; }
	void setEvent_CounterThread() { SetEvent(pEvent_CounterThread); }

	//마킹처리를 위한 함수
	BOOL MarkingProcess();

	//========= 마킹 정보를 보내기 위한 변수 ==============
	//마킹 Send 펄스 값을 TRUE 한 타임이 20sec 지나면 FALSE
	DWORD m_markingSendTimeOut;
	//마킹 FALSE 시간을 10 sec 유지
	DWORD m_markingSendFALSETimeOut;
	//마킹 유효성 여부
	bool m_bMarkingDataSend;
	//마킹 보내는 중 20 sec 후 신호 제거

	//마킹 루프 Bit 확인 객체
	BOOL m_bOutputBitStatus;

	//테스트 타임 id 생성
	DWORD m_markingTestTimeOut;


	//트리커 BCD ID를 읽기 위한 함수
	BOOL readTriggerBCDID();

	//========== 트리커 BCD ID read =========================
	//최종 읽은 값
	WORD m_wLastInfo;

#if DIO_BOARD_NO // 0이 아니면
	WORD m_wLastInfo_Output;
#endif

	//다음에 찾을 TabID - ID 누력 여부 확인용
	WORD m_nextTabID;

	//m_nTabIdTotalCount 를 백업 해둔다.
	int m_nTabIdTotalCount_backup;

	//DIO Trigger Bit 신호가 TRUE일 때 
	//받은 값 백업 용
	//새로 받은 Tab Id와 비교하여 누락 여부 확인
	//누락된 값 및 범위 확인
	WORD m_wLastTabId;
};

