#include "pch.h"
#include "CImageSaveQueueCtrl.h"
#include "GlobalData.h"


void CImgSaveInfo::initImgPtr(int nSize)
{
	memset(m_pImagePtr, 0x00, sizeof(BYTE) * ((nSize) + 1));
}
void CImgSaveInfo::initImgPtr(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	memset(m_pImagePtr, 0x00, sizeof(BYTE) * ((nWidth * nHeight) + 1));
}

BYTE* CImgSaveInfo::GetImgPtr()
{
	return m_pImagePtr; 
};
void CImgSaveInfo::SetImgPtr(BYTE* pImgPtr, int nWidth, int nHeight, int nJpgQuality)
{
	m_nWidth = nWidth; 
	m_nHeight = nHeight; 
	m_nJpgQuality = nJpgQuality; 
	memset(m_pImagePtr, 0x00, sizeof(BYTE) * ((nWidth * nHeight) + 1));
	CopyMemory(m_pImagePtr, pImgPtr, sizeof(BYTE) * m_nWidth * m_nHeight);
};


CImageSaveQueueCtrl::CImageSaveQueueCtrl()
{
	::InitializeCriticalSection(&m_csQueue);
}

CImageSaveQueueCtrl::~CImageSaveQueueCtrl(void)
{
	::DeleteCriticalSection(&m_csQueue);
}

void CImageSaveQueueCtrl::ResetQueue()
{
	::EnterCriticalSection(&m_csQueue);
	int size = (int)m_queSaveInfo.size();
	int i = 0;
	m_lCount = 0;
	CImgSaveInfo* pSavenfo;
	for (i = 0; i < size; i++) {
		pSavenfo = m_queSaveInfo.front();
		m_queSaveInfo.pop();
		if (pSavenfo != NULL) {
			delete pSavenfo;
			pSavenfo = NULL;
		}
	}
	::LeaveCriticalSection(&m_csQueue);
}
int CImageSaveQueueCtrl::PushBack(CImgSaveInfo* pSavenfo)
{
	int nFrmCnt = 0;
	::EnterCriticalSection(&m_csQueue);
	if (pSavenfo != NULL) {
		m_queSaveInfo.push(pSavenfo);
	}
	::LeaveCriticalSection(&m_csQueue);
	return nFrmCnt;
}
CImgSaveInfo* CImageSaveQueueCtrl::Pop()
{
	::EnterCriticalSection(&m_csQueue);
	CImgSaveInfo* pInfo = NULL;
	if (m_queSaveInfo.empty() == false) {
		pInfo = m_queSaveInfo.front();
		m_queSaveInfo.pop();
	}
	::LeaveCriticalSection(&m_csQueue);
	return pInfo;
}
int CImageSaveQueueCtrl::GetSize()
{
	::EnterCriticalSection(&m_csQueue);
	int nSize = (int)m_queSaveInfo.size();
	::LeaveCriticalSection(&m_csQueue);
	return nSize;
}

BOOL CImageSaveQueueCtrl::IsEmpty()
{
	::EnterCriticalSection(&m_csQueue);
	BOOL bRet = TRUE;
	if (!m_queSaveInfo.empty()) {
		bRet = FALSE;
	}
	::LeaveCriticalSection(&m_csQueue);
	return bRet;
}
