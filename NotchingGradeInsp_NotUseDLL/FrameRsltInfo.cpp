#include "pch.h"
#include "FrameRsltInfo.h"
#include "CDefectDataCtrl.h"

CFrameRsltInfo::CFrameRsltInfo(void)
{

	m_pTabRsltInfo = new CTabRsltInfo();
}

CFrameRsltInfo::~CFrameRsltInfo(void)
{
	int i = 0;

	if (m_pImagePtr != NULL) {
		delete[] m_pImagePtr;
		m_pImagePtr = NULL;
	}

	if (m_pTabRsltInfo != NULL) {
		delete m_pTabRsltInfo;
		m_pTabRsltInfo = NULL;
	}

}

void CFrameRsltInfo::Copy(CFrameInfo* pFrmInfo)
{
	m_pImagePtr = pFrmInfo->GetImagePtr();
	pFrmInfo->SetImgPtr(NULL); // ���ϸ� ����.

	m_nFrameCount = pFrmInfo->m_nFrameCount;
	m_nHeadNo = pFrmInfo->m_nHeadNo;
	m_nHeight = pFrmInfo->m_nHeight;		// Image ����
	m_nWidth = pFrmInfo->m_nWidth;		// Image ��
	m_nBand = pFrmInfo->m_nBand;		// 1 : Mono, 3 : Color
	nOverlapSize = pFrmInfo->nOverlapSize;	// �������� ũ��
	m_nTabLevel = pFrmInfo->m_nTabLevel;	// Tab�� Ceramic �����/���� ��ġ
	m_nInspMode = pFrmInfo->m_nInspMode;	// en_HeadFrame/ en_TailFrame �˻� ���

	m_nTabLeft = pFrmInfo->m_nTabLeft;
	m_nTabRight = pFrmInfo->m_nTabRight;

	m_dTopPosY = pFrmInfo->m_dTopPosY;	// Frame�� ���� Pixel�� Y ��ǥ

	nTabNo = pFrmInfo->nTabNo;		// 
	nTopPosYInFrame = pFrmInfo->nTopPosYInFrame;

	// Tab ��ġ  
	nTabStartHor = pFrmInfo->nTabStartHor;
	nTabEndHor = pFrmInfo->nTabEndHor;
	nTabStartPosInFrame = pFrmInfo->nTabStartPosInFrame; // 

	if(m_pTabRsltInfo != NULL){
		m_pTabRsltInfo->m_nHeadNo = m_nHeadNo;
		m_pTabRsltInfo->m_nTabNo = nTabNo;
	}

	m_nTabId_CntBoard = pFrmInfo->m_nTabId_CntBoard;
}
