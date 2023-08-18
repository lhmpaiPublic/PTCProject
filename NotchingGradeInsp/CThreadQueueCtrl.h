#pragma once

#include <queue>

class CImageProcThreadUnit;

//#define	MAX_CAM_NO 2


//class CFrameInfo {
//public:
//	int nTabNo;
//	BYTE* pImagePtr;
//	int nWidth;
//	int nHeight;
//	int nCamNo;
//};
class CFrameInfo;
class CImageProcessCtrl;

class CThreadQueueCtrl
{
public :
	CThreadQueueCtrl(CImageProcessCtrl *pParent );
	~CThreadQueueCtrl();
	int push(CFrameInfo* pFrmInfo) ;
	CImageProcThreadUnit* pop();
	BOOL IsEmpty();
	void ResetQueue();
	int GetSize();

	void push(CImageProcThreadUnit* pThread);

	//객체 멤버를 접근 스래드 처리 함수
	//스래드 프로세싱 감시 스래드 함수
	void ThreadQueueCtrl_WatchThread();

	//총 생성 큐 갯수
	int getTotalQueueCount();
protected :

	CImageProcessCtrl* m_pParent;
	CRITICAL_SECTION m_csQueue;
	bool m_bQueuePushPop;

	typedef std::queue< CImageProcThreadUnit*> THREAD_QUEUE;
	THREAD_QUEUE m_pThradQue;

	//스래드가 Resum되기 전 저장 큐 객체
	//스래드 객체 핸들
	CWinThread* m_pWatchThread;
	//스래드 생성함수
	void CreateCheckThradQueThread();
	//스래드 함수
	static UINT WatchThreadProc(LPVOID pParam);
	//동기화 객체
	CRITICAL_SECTION m_csWatchQueue;
	//저장 큐 객체
	THREAD_QUEUE m_pWatchQueBuffer;
	//저장객체 큐 사이즈
	int GetWatchQueueSize();
	//동기화 이벤트 객체
	HANDLE pEvent_ThreadQueueCtrl;
	//
	CImageProcThreadUnit* GetWatchQueueData();
};

