#include "pch.h"
#include "CThreadQueueCtrl.h"
#include "CImageProcThreadUnit.h"
#include "FrameInfo.h"
#include "ImageProcessCtrl.h"
#include "GlobalData.h"			// 23.09.10 Ahn Add 

#define WATCHTHREAD_TIMEOUT 30
CThreadQueueCtrl::CThreadQueueCtrl(CImageProcessCtrl* pParent)
{
	::InitializeCriticalSection(&m_csQueue);
	m_pParent = pParent;

	m_bQueuePushPop = false;

	for(int idx = 0; idx < THREADQUEUE_MAXCOUNT; idx++)
		ProcThreadUnitqueue[idx] = NULL;
	front = 0;
	rear = 0;
	maxQueueSize = THREADQUEUE_MAXCOUNT;

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
			enQueue();
		}
		//스래드 종료 처리
		else
		{
			break;
		}
	}
}

#define MAX_THREADQUEUECTRLSIZE 8
int CThreadQueueCtrl::push( CFrameInfo *pFrmInfo )
{
	ASSERT(pFrmInfo);

	//스래드 저장큐의 갯수를 가져와서 MAX_THREAD_QUEUE_SIZE 작을 때 저장한다.
	//생성된 총 Unit 큐 사이즈
	int totalQueueSize = getTotalQueueCount();

	if (pFrmInfo)
	{
		//이미지 저리 결과 마킹 최종 결과를 생성하기 위한 스래드 생성
		CImageProcThreadUnit* pThread = new CImageProcThreadUnit(pFrmInfo);

		if (pThread)
		{
			int nOverflowMax = AprData.m_System.m_nOverflowCountMax;
			// 큐가 오버되었으면
			if (MAX_THREADQUEUECTRLSIZE <= totalQueueSize)
			{
				//저장큐가 Over Flow 값 설정
				pFrmInfo->m_bOverFlow = TRUE;

				//DEBUG_LOG.txt
				CString strLog;
				strLog.Format(_T("TabNo[%d]- CImageProcThreadUnit OverFlow : Q-Size<%d/%d>"), pFrmInfo->nTabNo, totalQueueSize, nOverflowMax);
				AprData.SaveDebugLog_Format(strLog);
				AprData.SaveErrorLog(strLog);

				LOGDISPLAY_SPEC(8)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - Qsize<%d>-Overflow",
					(pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? "Top" : "Btm", pFrmInfo->nTabNo, pFrmInfo->m_nTabId_CntBoard, totalQueueSize
					);

			}
			LOGDISPLAY_SPEC(8)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - Qsize<%d>",
				(pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? "Top" : "Btm", pFrmInfo->nTabNo, pFrmInfo->m_nTabId_CntBoard, totalQueueSize
				);

			pThread->Begin();

			::EnterCriticalSection(&m_csWatchQueue);
			//스래드객체 저장큐에 저장
			m_pWatchQueBuffer.push(pThread);

			::LeaveCriticalSection(&m_csWatchQueue);

			totalQueueSize = getTotalQueueCount();
		}
	}

	return totalQueueSize;
}

int CThreadQueueCtrl::GetWatchQueueSize()
{
	int nRet = (int)m_pWatchQueBuffer.size();
	return nRet;

}

CImageProcThreadUnit* CThreadQueueCtrl::GetWatchQueueData()
{
	CImageProcThreadUnit* data = NULL;
	::EnterCriticalSection(&m_csWatchQueue);
	if (m_pWatchQueBuffer.size())
	{
		data = m_pWatchQueBuffer.front();
		m_pWatchQueBuffer.pop();
	}
	::LeaveCriticalSection(&m_csWatchQueue);

	return data;

}

//총 생성 큐 갯수
int CThreadQueueCtrl::getTotalQueueCount()
{
	return GetWatchQueueSize() + countQueue();
}

bool CThreadQueueCtrl::isEmpty()
{
	bool b = false;
	if (front == rear) 
	{
		b = true;
	}
	return b;
}

bool CThreadQueueCtrl::isFull() 
{
	bool b = false;
	if (front == (rear + 1) % maxQueueSize)
	{
		b = true;
	}
	return b;
}

void CThreadQueueCtrl::enQueue()
{
	if (m_bQueuePushPop == false)
	{
		m_bQueuePushPop = true;
		while (!isFull())
		{
			//스래드 객체를 가져온다.
			CImageProcThreadUnit* pImageProcThreadUnit = GetWatchQueueData();
			if (pImageProcThreadUnit)
			{
				LOGDISPLAY_SPEC(8)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - enQ-ResultWaitQ",
					(pImageProcThreadUnit->m_pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? "Top" : "Btm", pImageProcThreadUnit->m_pFrmInfo->nTabNo, pImageProcThreadUnit->m_pFrmInfo->m_nTabId_CntBoard
					);

				pImageProcThreadUnit->ProcStart();
				rear = ++rear % maxQueueSize;
				ProcThreadUnitqueue[rear] = pImageProcThreadUnit;
			}
			else
			{
				break;
			}
		}
		m_bQueuePushPop = false;
	}
}

CImageProcThreadUnit* CThreadQueueCtrl::deQueue()
{
	CImageProcThreadUnit* pImageProcThreadUnit = NULL;
	if (m_bQueuePushPop == false)
	{
		m_bQueuePushPop = true;
		if (!isEmpty())
		{
			front = ++front % maxQueueSize;
			pImageProcThreadUnit = ProcThreadUnitqueue[front];

			if (pImageProcThreadUnit)
			{
				LOGDISPLAY_SPEC(6)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - deQ-ResultWaitQ",
					(pImageProcThreadUnit->m_pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? "Top" : "Btm", pImageProcThreadUnit->m_pFrmInfo->nTabNo, pImageProcThreadUnit->m_pFrmInfo->m_nTabId_CntBoard
					);
			}

		}
		m_bQueuePushPop = false;
	}
	return pImageProcThreadUnit;

}

int CThreadQueueCtrl::countQueue()
{
	return abs(front - rear);
}
