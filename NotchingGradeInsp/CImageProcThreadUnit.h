#pragma once

//class CThreadQueueCtrl;

class CFrameInfo;
class CFrameRsltInfo;
class CImageProcessCtrl;
// CImageProcThreadUnit

class CImageProcThreadUnit : public CWinThread
{
	DECLARE_DYNCREATE(CImageProcThreadUnit)

	//이미지 프로세싱 처리 결과를 이용하여 마키 설정 및 통신처리를 위한 스래드 함수
	//최종 OK , NG 판정 처리
	static UINT CtrlImageProcThread(LPVOID pParam);

	CImageProcThreadUnit(CFrameInfo *pFrmInfo = NULL);           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CImageProcThreadUnit();

	static CImageProcThreadUnit* gImageProcThreadUnit;
protected:

	HANDLE	m_hEventProcStart;
	BOOL m_bEventProcStart;
	HANDLE	m_hEventRun;
	HANDLE	m_hEventProcEnd;
	HANDLE	m_hEventKillThread;
	HANDLE	m_hEventForceStop;
	HANDLE	m_hEventKilled;
	HANDLE	m_hSendResult;

	//m_hEventProcEnd의 타임아웃 카운트
	int ProcEnd_WaitCount;

	CWinThread* m_pThread;
	int m_nErrorCode;

	//타임아웃 여부 변수
	BOOL m_bTimeOut;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	BOOL m_bThreadValid;
	CFrameInfo* m_pFrmInfo;
	CFrameRsltInfo* m_pFrmRsltInfo;

	int Begin();
	//스래드를 생성 후 스타트 하기 위한 함수
	void ProcStart();

	int Kill();
	BOOL IsProcEnd();
	//EVENT 결과 
	int eventProcEnd_WaitTime();

	int ForceStop();	
	void SendResult();
	CFrameRsltInfo* GetResultPtr() {return m_pFrmRsltInfo; } ;

	void SetEventProcEnd() {
		::SetEvent(m_hEventProcEnd);
		ExitInstance();
	};

	BOOL getTimeOut() { return m_bTimeOut; }

	bool bThreadClose;
protected:
	DECLARE_MESSAGE_MAP()
};


