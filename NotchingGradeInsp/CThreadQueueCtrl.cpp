#include "pch.h"
#include "CThreadQueueCtrl.h"
#include "CImageProcThreadUnit.h"
#include "FrameInfo.h"
#include "ImageProcessCtrl.h"
#include "GlobalData.h"			// 23.09.10 Ahn Add 

CThreadQueueCtrl::CThreadQueueCtrl(CImageProcessCtrl* pParent)
{
	::InitializeCriticalSection(&m_csQueue);
	m_pParent = pParent;
}

CThreadQueueCtrl::~CThreadQueueCtrl()
{
	::DeleteCriticalSection(&m_csQueue);
}

int CThreadQueueCtrl::push( CFrameInfo *pFrmInfo )
{
	::EnterCriticalSection(&m_csQueue);
	ASSERT(pFrmInfo);

	//이미지 저리 결과 마킹 최종 결과를 생성하기 위한 스래드 생성
	CImageProcThreadUnit* pThread = new CImageProcThreadUnit(pFrmInfo);

	//스래드 저장큐의 갯수를 가져와서 MAX_THREAD_QUEUE_SIZE 작을 때 저장한다.
	int nSize = (int)m_pThradQue.size();

	int nOverflowMax = AprData.m_System.m_nOverflowCountMax;
	// 큐가 오버되었으면
	if (nOverflowMax <= nSize)
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("TabNo[%d]- CImageProcThreadUnit OverFlow : Q-Size<%d/%d>"), pFrmInfo->nTabNo, nSize, nOverflowMax) ;

		LOGDISPLAY_SPEC(5)("<<Proc>> TabNo<%d> Qsize<%d> : CThreadQueueCtrl::push Overflow",
			pFrmInfo->nTabNo, nSize);
		//저장큐가 Over Flow 값 설정
		pFrmInfo->m_bOverFlow = TRUE;

		pThread->Begin_Exception();
	}
	else
	{
		pThread->Begin();
	}

	//스래드객체 저장큐에 저장
	m_pThradQue.push(pThread);

	//스래드 저장큐 객수를 가져온다.
	nSize = (int)m_pThradQue.size();
	::LeaveCriticalSection(&m_csQueue);

	return nSize;
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
