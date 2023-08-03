#include "pch.h"
#include "CThreadQueueCtrl.h"
#include "CImageProcThreadUnit.h"
#include "FrameInfo.h"
#include "ImageProcessCtrl.h"
#include "GlobalData.h"			// 23.09.10 Ahn Add 

#define WATCHTHREAD_TIMEOUT 100
#define THREADQUEUE_MAXCOUNT 5
CThreadQueueCtrl::CThreadQueueCtrl(CImageProcessCtrl* pParent)
{
	::InitializeCriticalSection(&m_csQueue);
	m_pParent = pParent;

	//스래드 객체 초기화
	m_pWatchThread = NULL;
	//동기화 객체 초기화
	::InitializeCriticalSection(&m_csWatchQueue);
	//감시 스래드 생성
	CreateCheckThradQueThread();
}

CThreadQueueCtrl::~CThreadQueueCtrl()
{
	::DeleteCriticalSection(&m_csQueue);

	//스래드 종료
	if (m_pWatchThread)
	{
		SetEvent(pEvent_ThreadQueueCtrl);
		CGlobalFunc::ThreadExit(&m_pWatchThread->m_hThread, 5000);
		m_pWatchThread->m_hThread = NULL;
	}
	//동기화 객체 제거
	::DeleteCriticalSection(&m_csWatchQueue);
}
void CThreadQueueCtrl::CreateCheckThradQueThread()
{
	if (m_pWatchThread == NULL)
	{
		//이벤트 객체 생성
		pEvent_ThreadQueueCtrl = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_pWatchThread = AfxBeginThread((AFX_THREADPROC)WatchThreadProc,
			(LPVOID)this,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);

		if (m_pWatchThread != NULL) {
			m_pWatchThread->m_bAutoDelete = FALSE;
			m_pWatchThread->ResumeThread();
		}
	}
}

//스래드가 Resum되기 전 저장 큐 객체
//스래드 함수
UINT CThreadQueueCtrl::WatchThreadProc(LPVOID pParam)
{
	CThreadQueueCtrl* pThreadQueueCtrl = (CThreadQueueCtrl*)pParam;
	pThreadQueueCtrl->ThreadQueueCtrl_WatchThread();
	AfxEndThread(0);
	return 0;
}

//객체 멤버를 접근 스래드 처리 함수
//스래드 프로세싱 감시 스래드 함수
void CThreadQueueCtrl::ThreadQueueCtrl_WatchThread()
{
	UINT ret = 0;
	while (true)
	{
		//감시 주기 이벤트 타임아웃 체크
		ret = WaitForSingleObject(pEvent_ThreadQueueCtrl, WATCHTHREAD_TIMEOUT);
		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			//이미지 프로세싱 큐 갯수를 가져온다.
			int ThreadQueueSize = GetSize();
			//스래드의 갯수 보다 작은 만큼 채운다.
			for (int buff = ThreadQueueSize; buff < THREADQUEUE_MAXCOUNT; buff++)
			{
				//작동하지 않는 스래드 큐 크기
				int nRet = GetWatchQueueSize();
				if (nRet) 
				{
					//스래드 객체를 가져온다.
					CImageProcThreadUnit* pThread = GetWatchQueueData();
					if (pThread)
					{
						//스래드 작동 큐로 이동한다.
						push(pThread);
					}
				}
				else
				{
					break;
				}
				
			}
		}
		//스래드 종료 처리
		else
		{
			break;
		}
	}
}

int CThreadQueueCtrl::push( CFrameInfo *pFrmInfo )
{
	ASSERT(pFrmInfo);

	//스래드 저장큐의 갯수를 가져와서 MAX_THREAD_QUEUE_SIZE 작을 때 저장한다.
	//생성된 총 Unit 큐 사이즈
	int totalQueueSize = getTotalQueueCount();

	//이미지 저리 결과 마킹 최종 결과를 생성하기 위한 스래드 생성
	CImageProcThreadUnit* pThread = new CImageProcThreadUnit(pFrmInfo);

	int nOverflowMax = AprData.m_System.m_nOverflowCountMax;
	// 큐가 오버되었으면
	if (nOverflowMax <= totalQueueSize)
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("TabNo[%d]- CImageProcThreadUnit OverFlow : Q-Size<%d/%d>"), pFrmInfo->nTabNo, totalQueueSize, nOverflowMax) ;

		LOGDISPLAY_SPEC(5)("<<Proc>> TabNo<%d> Qsize<%d> : CThreadQueueCtrl::push Overflow",
			pFrmInfo->nTabNo, totalQueueSize);
		//저장큐가 Over Flow 값 설정
		pFrmInfo->m_bOverFlow = TRUE;

	}

	pThread->Begin();

	::EnterCriticalSection(&m_csWatchQueue);
	//스래드객체 저장큐에 저장
	m_pWatchQueBuffer.push(pThread);

	::LeaveCriticalSection(&m_csWatchQueue);

	return getTotalQueueCount();
}

void CThreadQueueCtrl::push(CImageProcThreadUnit* pThread)
{
	::EnterCriticalSection(&m_csQueue);

	pThread->ProcStart();

	//스래드객체 저장큐에 저장
	m_pThradQue.push(pThread);

	::LeaveCriticalSection(&m_csQueue);
}

CImageProcThreadUnit* CThreadQueueCtrl::pop()
{
	::EnterCriticalSection(&m_csQueue);

	CImageProcThreadUnit* pThread = NULL ;
	if (m_pThradQue.empty() == FALSE) {
		pThread = m_pThradQue.front();
		m_pThradQue.pop();
	}
	::LeaveCriticalSection(&m_csQueue);
	return pThread;
}

BOOL CThreadQueueCtrl::IsEmpty()
{
	::EnterCriticalSection(&m_csQueue);
	BOOL bRet = m_pThradQue.empty();
	::LeaveCriticalSection(&m_csQueue);

	return bRet;
}

void CThreadQueueCtrl::ResetQueue()
{
	::EnterCriticalSection(&m_csQueue);
	int size = (int)m_pThradQue.size();
	int i = 0;
	CImageProcThreadUnit* pProcUnit;
	for (i = 0; i < size; i++) {
		pProcUnit = m_pThradQue.front();
		m_pThradQue.pop();
		if (pProcUnit != NULL) {

			pProcUnit->ForceStop();
			pProcUnit->Kill();

			delete pProcUnit;
			pProcUnit = NULL;
		}
	}
	::LeaveCriticalSection(&m_csQueue);
}

int CThreadQueueCtrl::GetSize()
{
	::EnterCriticalSection(&m_csQueue);
	int nRet = (int)m_pThradQue.size();
	::LeaveCriticalSection(&m_csQueue);

	return nRet;

}

int CThreadQueueCtrl::GetWatchQueueSize()
{
	::EnterCriticalSection(&m_csWatchQueue);
	int nRet = (int)m_pWatchQueBuffer.size();
	::LeaveCriticalSection(&m_csWatchQueue);

	return nRet;

}

CImageProcThreadUnit* CThreadQueueCtrl::GetWatchQueueData()
{
	::EnterCriticalSection(&m_csWatchQueue);
	CImageProcThreadUnit* data = m_pWatchQueBuffer.front();
	if (data) m_pWatchQueBuffer.pop();
	::LeaveCriticalSection(&m_csWatchQueue);

	return data;

}

//총 생성 큐 갯수
int CThreadQueueCtrl::getTotalQueueCount()
{
	return GetWatchQueueSize() + GetSize();
}