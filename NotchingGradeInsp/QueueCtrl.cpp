//#include "StdAfx.h"
#include "pch.h"
#include "QueueCtrl.h"
#include "FrameInfo.h"
#include "GlobalData.h"

CQueueCtrl::CQueueCtrl()
{
	::InitializeCriticalSection( &m_csQueue ) ;
	m_bQueuePushPop = false;
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

	while (true)
	{
		if (m_bQueuePushPop == false)
		{
			m_bQueuePushPop = true;


			if (pFrmInfo != NULL) 
			{
				m_queFrameInfo.push(pFrmInfo);
				int nHeadNo = pFrmInfo->m_nHeadNo;
				nFrmCnt = pFrmInfo->m_nFrameCount;
				AprData.SetFrameCounter(nHeadNo, nFrmCnt);

				if (pFrmInfo->nTabNo > 0)
				{
					LOGDISPLAY_SPEC(6)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - push-ResultFrameData",
						(pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? "Top" : "Btm", pFrmInfo->nTabNo, pFrmInfo->m_nTabId_CntBoard
						);
				}
			}

			m_bQueuePushPop = false;

			break;
		}
		else
		{
			Sleep(1);
		}

	}

	return nFrmCnt;
}
CFrameInfo *CQueueCtrl::Pop() 
{
	CFrameInfo* pInfo = NULL;
	while (true)
	{
		if (m_bQueuePushPop == false)
		{
			m_bQueuePushPop = true;

			if (m_queFrameInfo.empty() == false)
			{
				pInfo = m_queFrameInfo.front();
				m_queFrameInfo.pop();

				if (pInfo->nTabNo > 0)
				{
					LOGDISPLAY_SPEC(6)("<<%s>>>UnitThread TabNo<%d>-TabId<%d> - pop-ResultFrameData",
						(pInfo->m_nHeadNo == CAM_POS_TOP) ? "Top" : "Btm", pInfo->nTabNo, pInfo->m_nTabId_CntBoard
						);
				}
			}

			m_bQueuePushPop = false;

			break;
		}
		else
		{
			Sleep(1);
		}
	}
	return pInfo;
}
int CQueueCtrl::GetSize() 
{
	int nSize = (int)m_queFrameInfo.size() ;
	return nSize ;
}

BOOL CQueueCtrl::IsEmpty() 
{
	BOOL bRet = TRUE  ;
	if( !m_queFrameInfo.empty() )
	{
		bRet = FALSE ;
	}
	return bRet ;
}
