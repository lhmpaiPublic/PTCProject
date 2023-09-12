#include "pch.h"
#include "CCounterQueueCtrl.h"
#include "GlobalData.h"


CCounterQueueCtrl::CCounterQueueCtrl()
{
	::InitializeCriticalSection(&m_csQueue);
	m_nMaxQueueSize = MAX_COUNTER_QUEUE_SIZE ;
}
CCounterQueueCtrl::~CCounterQueueCtrl(void)
{
	ResetQueue();
	::DeleteCriticalSection(&m_csQueue);
}
void CCounterQueueCtrl::ResetQueue()
{
	::EnterCriticalSection(&m_csQueue);
	int size = (int)m_CntInfo.size();
	int i = 0;
	m_lCount = 0;
	//CCounterInfo pCntInfo;
	for (i = 0; i < size; i++) {
	//	pCntInfo = m_CntInfo.front();
		m_CntInfo.pop();
	}
	::LeaveCriticalSection(&m_csQueue);

}
int CCounterQueueCtrl::PushBack(CCounterInfo CntInfo)
{
	int nCnt = 0;
	::EnterCriticalSection(&m_csQueue);
	
	m_CntInfo.push(CntInfo);

	// 21.12.27 Ahn Add Start
	int nSize = (int)m_CntInfo.size();;
	if (m_nMaxQueueSize < nSize) {
		CCounterInfo CntInfobefore = Pop();
	}
	// 21.12.27 Ahn Add End

	::LeaveCriticalSection(&m_csQueue);
	return 0;
}
int CCounterQueueCtrl::GetSize()
{
	int nSize = 0;
	::EnterCriticalSection(&m_csQueue);
	nSize = (int)m_CntInfo.size();
	::LeaveCriticalSection(&m_csQueue);
	return nSize;
}
BOOL CCounterQueueCtrl::IsEmpty()
{
	BOOL bEmpty = TRUE;
	::EnterCriticalSection(&m_csQueue);
	bEmpty = m_CntInfo.empty();
	::LeaveCriticalSection(&m_csQueue);
	return bEmpty ;
}
CCounterInfo CCounterQueueCtrl::Pop()
{
	::EnterCriticalSection(&m_csQueue);
	CCounterInfo CntInfo;
	if (!m_CntInfo.empty()) {
		CntInfo = m_CntInfo.front();
		m_CntInfo.pop();
	}
	::LeaveCriticalSection(&m_csQueue);
	return CntInfo;
}


int CCounterQueueCtrl::CalcMaxQueueSize(int nAcqBufSize, int nOneTabSize)
{
	int nRetSize = 0;

	if (nOneTabSize <= 0) {
		return MAX_COUNTER_QUEUE_SIZE ;		 
	}

	nRetSize = (int)( (nAcqBufSize / nOneTabSize) + 0.5 ) ;

	return nRetSize;
}

void CCounterQueueCtrl::SetMaxQueueSize(int nSize)
{
	if (nSize <= 0) {
		nSize = MAX_COUNTER_QUEUE_SIZE;
	}

	m_nMaxQueueSize = nSize;
}
