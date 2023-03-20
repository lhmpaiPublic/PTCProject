#include "pch.h"
#include "CTacTimeDataCtrl.h"

CTacTimeDataCtrl::CTacTimeDataCtrl()
{
	::InitializeCriticalSection(&m_csQueue) ;
	m_nMaxListSize = en_MaxTactListSize  ;
	m_vecTactTime.clear();
}

CTacTimeDataCtrl::~CTacTimeDataCtrl()
{
	m_vecTactTime.clear();

	::DeleteCriticalSection(&m_csQueue) ;
}

void CTacTimeDataCtrl::SetMaxListSize(int nMaxSize)
{
	if (nMaxSize > en_MinTacListSize) {
		m_nMaxListSize = nMaxSize;
	}
}


int CTacTimeDataCtrl::AddNewTactData(CTactTimeData data)
{
	int nRet = 0 ;
	::EnterCriticalSection(&m_csQueue);

	m_vecTactTime.push_back(data);

	int nSize = (int)m_vecTactTime.size() ;
	if (m_nMaxListSize < nSize) {
		_VEC_TACT_DATA::iterator iter ;
		for (iter = m_vecTactTime.begin(); iter != m_vecTactTime.end(); ) {
			iter = m_vecTactTime.begin();
			iter = m_vecTactTime.erase(iter);
		}
	}

	::LeaveCriticalSection(&m_csQueue) ;
	return nRet ;
}

BOOL CTacTimeDataCtrl::IsEmpty()
{
	BOOL bEmpty;
	::EnterCriticalSection(&m_csQueue);

	bEmpty = m_vecTactTime.empty();

	::LeaveCriticalSection(&m_csQueue);
	return bEmpty;
}

void CTacTimeDataCtrl::ResetQueue()
{
	::EnterCriticalSection(&m_csQueue);

	m_vecTactTime.clear() ;

	::LeaveCriticalSection(&m_csQueue);
}

int CTacTimeDataCtrl::GetSize()
{
	int nRet = (int)m_vecTactTime.size();

	return nRet;
}
