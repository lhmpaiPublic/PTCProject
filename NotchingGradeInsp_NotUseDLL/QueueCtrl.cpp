//#include "StdAfx.h"
#include "pch.h"
#include "QueueCtrl.h"
#include "FrameInfo.h"
#include "GlobalData.h"

CQueueCtrl::CQueueCtrl()
{
	::InitializeCriticalSection( &m_csQueue ) ;
}

CQueueCtrl::~CQueueCtrl(void)
{
	::DeleteCriticalSection( &m_csQueue ) ;
}

void CQueueCtrl::ResetQueue() 
{
	::EnterCriticalSection( &m_csQueue ) ;
	int size = (int)m_queFrameInfo.size() ;
	int i= 0 ;
	m_lCount = 0 ;
	CFrameInfo *pFrmInfo ;
	for( i = 0 ; i < size ; i++ ){
		pFrmInfo = m_queFrameInfo.front() ;
		m_queFrameInfo.pop() ;
		if (pFrmInfo != NULL) {
			delete pFrmInfo;
			pFrmInfo = NULL;
		}
	}
	::LeaveCriticalSection( &m_csQueue ) ;
}
int CQueueCtrl::PushBack( CFrameInfo *pFrmInfo ) 
{
	int nFrmCnt = 0;
	::EnterCriticalSection( &m_csQueue ) ;
	if( pFrmInfo != NULL )	{
		if (pFrmInfo->nTabNo <= 0) {
			pFrmInfo->nTabNo = (int)(m_lCount++ / MAX_CAMERA_NO);
		}
		m_queFrameInfo.push( pFrmInfo ) ;
		int nHeadNo = pFrmInfo->m_nHeadNo  ;
		nFrmCnt = pFrmInfo->m_nFrameCount;
		AprData.SetFrameCounter( nHeadNo, nFrmCnt );
	}
	::LeaveCriticalSection( &m_csQueue ) ;
	return nFrmCnt;
}
CFrameInfo *CQueueCtrl::Pop() 
{
	::EnterCriticalSection( &m_csQueue ) ;
	CFrameInfo *pInfo =  NULL ;
	if( m_queFrameInfo.empty() == false ){
		pInfo = m_queFrameInfo.front() ;
		m_queFrameInfo.pop() ;
	}
	::LeaveCriticalSection( &m_csQueue ) ;
	return pInfo ;
}
int CQueueCtrl::GetSize() 
{
	::EnterCriticalSection( &m_csQueue ) ;
	int nSize = (int)m_queFrameInfo.size() ;
	::LeaveCriticalSection( &m_csQueue ) ;
	return nSize ;
}

BOOL CQueueCtrl::IsEmpty() 
{
	::EnterCriticalSection( &m_csQueue ) ;
	BOOL bRet = TRUE  ;
	if( !m_queFrameInfo.empty() ){
		bRet = FALSE ;
	}
	::LeaveCriticalSection( &m_csQueue ) ;
	return bRet ;
}
