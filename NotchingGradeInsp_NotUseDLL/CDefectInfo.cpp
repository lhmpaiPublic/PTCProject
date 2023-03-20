#include "pch.h"
#include "CDefectInfo.h"



CDefectQueueCtrl::CDefectQueueCtrl()
{
	::InitializeCriticalSection(&m_csQueue);
}

CDefectQueueCtrl::~CDefectQueueCtrl(void)
{
	::DeleteCriticalSection(&m_csQueue);
}

void CDefectQueueCtrl::ResetQueue()
{
	::EnterCriticalSection(&m_csQueue);
	int size = (int)m_queDefectInfo.size();
	int i = 0;
	m_lCount = 0;
	CDefectInfo* pSavenfo;
	for (i = 0; i < size; i++) {
		pSavenfo = m_queDefectInfo.front();
		m_queDefectInfo.pop();
		if (pSavenfo != NULL) {
			delete pSavenfo;
			pSavenfo = NULL;
		}
	}
	::LeaveCriticalSection(&m_csQueue);
}
int CDefectQueueCtrl::PushBack(CDefectInfo* pSavenfo)
{
	int nFrmCnt = 0;
	::EnterCriticalSection(&m_csQueue);
	if (pSavenfo != NULL) {
		m_queDefectInfo.push(pSavenfo);
	}
	::LeaveCriticalSection(&m_csQueue);
	return nFrmCnt;
}
CDefectInfo* CDefectQueueCtrl::Pop()
{
	::EnterCriticalSection(&m_csQueue);
	CDefectInfo* pInfo = NULL;
	if (m_queDefectInfo.empty() == false) {
		pInfo = m_queDefectInfo.front();
		m_queDefectInfo.pop();
	}
	::LeaveCriticalSection(&m_csQueue);
	return pInfo;
}
int CDefectQueueCtrl::GetSize()
{
	::EnterCriticalSection(&m_csQueue);
	int nSize = (int)m_queDefectInfo.size();
	::LeaveCriticalSection(&m_csQueue);
	return nSize;
}

BOOL CDefectQueueCtrl::IsEmpty()
{
	::EnterCriticalSection(&m_csQueue);
	BOOL bRet = TRUE;
	if (!m_queDefectInfo.empty()) {
		bRet = FALSE;
	}
	::LeaveCriticalSection(&m_csQueue);
	return bRet;
}
