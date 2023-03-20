#include "pch.h"
#include "CAprHash.h"

CAprHash::CAprHash(int prSeed /*= HASH_SEED*/)
{
	m_prSeed = prSeed;
	m_layerCount = 1;

	if (m_prSeed <= HASHFS_MAX_LAYER_THLETHHOLD) {
		m_layerSize = m_prSeed;
	}
	else {
		m_layerCount = HASHFS_MAX_LAYER_COUNT;
		m_layerSize = m_prSeed / ((int)pow(m_prSeed, (double)0.5));
	}
}

CAprHash::~CAprHash()
{

}

int CAprHash::getHashValue(LPCTSTR szKeyStr)
{
	int strLength = (int) strlen(szKeyStr);
	if (strLength <= 0) {
		return(0);
	}

	int tmpVal = 0;
	for (int i = 0; i < strLength; i++) {
		tmpVal = ((tmpVal * 0x00ff) + (0x00ff & szKeyStr[i])) % m_prSeed;
	}

	return(tmpVal);
}

CString CAprHash::GetHashDir(LPCTSTR szString)
{
	CString strDir = _T("");
	CString strTemp = _T("");
	int iHashValue = getHashValue(szString);
	for (int iIndex = 0; iIndex < m_layerCount; iIndex++) {
		strTemp.Format(_T("\\%08d")
			, tranHashValue2NumberInLayer(iHashValue, iIndex));
		strDir += strTemp;
	}

	return(strDir);
}

int CAprHash::tranHashValue2NumberInLayer(int hashValue, int layerNumber)
{
	if (layerNumber == 0) {
		return(hashValue / m_layerSize);
	}
	else {
		return(hashValue % m_layerSize);
	}
}
