#include "pch.h"
#include "CTabRsltInfo.h"
#include "GlobalData.h"
#include "CRecipeCtrl.h"

void CTabRsltBase::SetTabInfo(int nHeadNo, int nTabNo, char *strPath )
{
	m_nHeadNo = nHeadNo;
	m_nTabNo = nTabNo;
//	m_strImagePath = strPath;
	::_tcsnccpy_s(m_chImagePath, _countof(m_chImagePath), strPath, _TRUNCATE);
}

CTabRsltBase::CTabRsltBase()
{
}

CTabRsltBase::~CTabRsltBase() 
{
}
CTabRsltInfo::CTabRsltInfo()
{
	m_nDefectCount = 0;
	m_bMarkingFlag = FALSE; // 22.01.11 Ahn Add
	m_vecDefInfo.clear();
}
CTabRsltInfo::~CTabRsltInfo()
{
	RemoveAll();
}
int CTabRsltInfo::GetDefectCount()
{
	int nRet = (int)m_vecDefInfo.size();

	return nRet;
}

// 22.02.25 Ahn Add Start
double CTabRsltInfo::SortingDefect(int nMode)
{
	double dRet = 0.0; 
	int nSize = (int)m_vecDefInfo.size();

	CDefectInfo* pSrc, * pTar, Temp;
	for (int i = 0; i < nSize - 1 ; i++) {
		pSrc = m_vecDefInfo[i];
		for (int j = i + 1; j < nSize; j++) {
			pTar = m_vecDefInfo[j];
			if (pTar->dJudgeSize > pSrc->dJudgeSize) {
				Temp = *pSrc;
				*pSrc = *pTar;
				*pTar = Temp;
			}
		}
	}

	if (nSize > 0) {
		dRet = m_vecDefInfo[0]->dJudgeSize;
	}

	return dRet;
}
// 22.02.25 Ahn Add End

// 22.05.25 Ahn Add Start
CDefectInfo* CTabRsltInfo::GetDefectInfo(int nIndex)
{
	int nSize = (int)m_vecDefInfo.size();
	if ((nSize <= nIndex) || (nIndex < 0)) {
		return NULL;
	}

	return m_vecDefInfo[nIndex];
}
// 22.05.25 Ahn Add End

void CTabRsltInfo::AddDefectInfo(CDefectInfo *defInfo)
{
	ASSERT(defInfo);
	m_vecDefInfo.push_back(defInfo);
	m_nDefectCount++;
	m_nCount[defInfo->nType][defInfo->nRank]++;

	//// 22.06.15 Ahn Add Start
	//if (m_nDefectMaxSize[defInfo->nHeadNo][defInfo->nType] < (int)defInfo->dJudgeSize) {
	//	m_nDefectMaxSize[defInfo->nHeadNo][defInfo->nType] = (int)defInfo->dJudgeSize ;
	//}
	//// 22.06.15 Ahn Add End
}
void CTabRsltInfo::RemoveAll()
{
//	if (m_vecDefInfo.size() <= 0) {
//	/	return;
//	}
	while (!m_vecDefInfo.empty()) {
		CDefectInfo* pInfo;
		pInfo = m_vecDefInfo.front();
		if (pInfo != NULL) {
			delete pInfo;
			pInfo = NULL;
		}
		m_vecDefInfo.erase(m_vecDefInfo.begin());
	}
	m_vecDefInfo.clear();
	m_nDefectCount = 0;
}
int CTabRsltInfo::GetSize()
{
	return (int)m_vecDefInfo.size() ;
}

int CTabRsltInfo::GetJudgement(CRecipeInfo *pRecipeInfo)
{
	int nJudge = JUDGE_OK;
	int nSize = (int)m_vecDefInfo.size();

	//for (int i = 0; i < nSize; i++) {
	//	m_vecDefInfo[ i ].
	//}

	return nJudge;
}

int CTabRsltInfo::GetDefJudge(double dSizeX, double dSizeY, double dDefSizeX, double dDefSizeY)
{
	int nJudge = JUDGE_OK ;
	if ((dDefSizeX > dSizeX) && (dSizeX > 0.0)) {
		nJudge = JUDGE_NG;
	}
	else if ((dDefSizeY > dSizeY) && (dSizeY > 0.0)) {
		nJudge = JUDGE_NG;
	}

	return nJudge;
}


int CTabRsltInfo::GetJudgement(double dSizeX, double dSizeY, double &dMaxSize, BOOL *pbJuegeType, int& bDefNo)
{
	int nJudge = JUDGE_OK;

	//if (dSizeX <= 0 && dSizeY <= 0) {
	//	return nJudge;
	//}

	dMaxSize = 0.0 ;
	int nSize = (int)m_vecDefInfo.size();
	double dBigSize = 0.0;
	bDefNo = -1;

	for (int i = 0; i < nSize; i++) {
	//	dBigSize = (m_vecDefInfo[i]->dSizeY > m_vecDefInfo[i]->dSizeX) ? m_vecDefInfo[i]->dSizeY : m_vecDefInfo[i]->dSizeY;
		 // 22.05.11 Ahn Add Start
		if (m_vecDefInfo[i]->nType == TYPE_SURFACE) continue ;
		// 22.05.11 Ahn Add End
		dBigSize = m_vecDefInfo[i]->dJudgeSize;
		if (dMaxSize < dBigSize) {
			dMaxSize = dBigSize;
			bDefNo = i;
		}

		int nLocalJudge = GetDefJudge(dSizeX, dSizeY, m_vecDefInfo[i]->dJudgeSize, m_vecDefInfo[i]->dSizeY);
		if (nLocalJudge == JUDGE_NG) {
			pbJuegeType[m_vecDefInfo[i]->nType] = TRUE;
			nJudge = JUDGE_NG;
		}
	}
	m_dMaxSizeDef = dMaxSize;
	return nJudge;
}


CPoint CTabRsltInfo::GetDefectCenterPos(int nDefIndex)
{
	CPoint cpCenter;
	cpCenter.SetPoint(0, 0);

	int nSize = (int)m_vecDefInfo.size();
	if ( (nSize > nDefIndex) && ( nDefIndex >= 0 ) ) {
		cpCenter = m_vecDefInfo[nDefIndex]->rcPos.CenterPoint();

	}

	return cpCenter;
}
