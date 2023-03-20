#include "pch.h"
#include "CJudgeResultCtrl.h"


CJudgeResultCtrl::CJudgeResultCtrl()
{
	::InitializeCriticalSection(&m_csJudge);
}
CJudgeResultCtrl::~CJudgeResultCtrl()
{
	::DeleteCriticalSection(&m_csJudge);
}

int CJudgeResultCtrl::GetNgTabCount()
{
	int nRet = (int)m_vecJudgeRslt.size();

	return nRet;
}

BOOL CJudgeResultCtrl::SetJudgeNG(int nTabNo, int nCamPos)
{
	::EnterCriticalSection(&m_csJudge);
	POSITION nPos = NULL ;
	BOOL bNewFlag = IsNewJudgeNG(nTabNo, nCamPos);
	::LeaveCriticalSection(&m_csJudge);

	if (bNewFlag == TRUE) {
		// 갱신 ...
	}

	return bNewFlag;
}

void CJudgeResultCtrl::ResetAll()
{
	m_vecJudgeRslt.clear();
}

BOOL CJudgeResultCtrl::IsNewJudgeNG(int nTabNo, int nCamPos)
{
	BOOL bNewFlag = TRUE;
	int nSize = (int)m_vecJudgeRslt.size() - 1;

	int nMaxFind = nSize - 5 ;
	if (nMaxFind < 0) {
		nMaxFind = 0;
	}

	for (int i = nSize; i >= nMaxFind; i--) {
		if (m_vecJudgeRslt[i].nTabNo == nTabNo) {
			bNewFlag = FALSE;
			break;
		}
	}

	return bNewFlag;
}
