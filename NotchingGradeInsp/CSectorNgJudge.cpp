#include "pch.h"
#include "CSectorNgJudge.h"
#include "CtabRsltInfo.h"
#include "GlobalData.h"

int CSectorNgJudge::ResetAll()
{
	m_nNgCount = 0 ;

	m_vecSector.clear();

//	VEC_TAB_JUDGE::iterator iter;
//	for (iter = m_vecSector.begin(); iter != m_vecSector.end(); iter++) {
//		iter = m_vecSector.erase(iter);
//	}

	AprData.SaveDebugLog_Format(_T("<CSectorNgJudge> 구간 정보 ResetAll"));

	return 0;
}

int CSectorNgJudge::AddNgTab(CTabJudge tab, int nSectSize)
{
	int nNewTabNo = tab.nTabNo;
	if ( (tab.nJudge == JUDGE_NG) && ( tab.nReason >= CTabRsltBase::en_Reason_FoilExpIn_Top))
	{
		m_vecSector.push_back(tab) ;

//		AprData.SaveDebugLog_Format(_T("<CSectorNgJudge::AddNgTab> 구간 정보에 NG Tab 추가") );

	}

	VEC_TAB_JUDGE::iterator iter;
	iter = m_vecSector.begin();

	if (iter != m_vecSector.end())
	{
		int nDiff = nNewTabNo - iter->nTabNo ;
		if (nDiff >= nSectSize)
		{
			iter = m_vecSector.erase(iter) ;
		}
	}
	m_nNgCount = (int)m_vecSector.size() ;


	return m_nNgCount;
}

int CSectorNgJudge::AddOkTab(int nTabNo, int nSectSize)
{
	VEC_TAB_JUDGE::iterator iter;
	iter = m_vecSector.begin(); 
	for (; iter != m_vecSector.end(); )
	{
		if( ( nTabNo - iter->nTabNo ) > nSectSize)
		{
			iter = m_vecSector.erase(iter) ;
		}
		else
		{

			break ;
		}
	}
	return 0;
}
