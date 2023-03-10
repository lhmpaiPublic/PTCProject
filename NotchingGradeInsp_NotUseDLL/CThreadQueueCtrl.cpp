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

	int nSize = (int)m_pThradQue.size();
	if (MAX_THREAD_QUEUE_SIZE <= nSize) {
		// 23.02.10 Ahn Add Start
		CString strLog;
		strLog.Format(_T("TabNo[%d]-Delete data due to queue size exceeded."), pFrmInfo->nTabNo );
		AprData.SaveDebugLog( strLog ) ;
		// 23.02.10 Ahn Add End
		// 23.02.20 Ahn Modify Start
		//delete pFrmInfo;
		//pFrmInfo = NULL;
		//::LeaveCriticalSection(&m_csQueue);
		//return nSize;
		pFrmInfo->m_bOverFlow = TRUE;
		// 23.02.20 Ahn Modify End
	}

	CImageProcThreadUnit* pThread = new CImageProcThreadUnit( pFrmInfo );
	pThread->Begin();

	m_pThradQue.push(pThread);
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
