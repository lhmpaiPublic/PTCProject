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
		delete m_pImagePtr;
		m_pImagePtr = NULL;
	}

	if (m_pTabRsltInfo != NULL) {
		delete m_pTabRsltInfo;
		m_pTabRsltInfo = NULL;
	}

}

void CFrameRsltInfo::Copy(CFrameInfo* pFrmInfo)
{
	m_nFrameCount = pFrmInfo->m_nFrameCount;
	m_nHeadNo = pFrmInfo->m_nHeadNo;
	m_nHeight = pFrmInfo->m_nHeight;		// Image 길이
	m_nWidth = pFrmInfo->m_nWidth;		// Image 폭
	m_nBand = pFrmInfo->m_nBand;		// 1 : Mono, 3 : Color
	nOverlapSize = pFrmInfo->nOverlapSize;	// 오버렙된 크기
	m_nTabLevel = pFrmInfo->m_nTabLevel;	// Tab의 Ceramic 어깨선/끝선 위치
	m_nInspMode = pFrmInfo->m_nInspMode;	// en_HeadFrame/ en_TailFrame 검사 모드

	m_nTabLeft = pFrmInfo->m_nTabLeft;
	m_nTabRight = pFrmInfo->m_nTabRight;

	m_dTopPosY = pFrmInfo->m_dTopPosY;	// Frame의 시작 Pixel의 Y 좌표

	nTabNo = pFrmInfo->nTabNo;		// 
	nTopPosYInFrame = pFrmInfo->nTopPosYInFrame;

	// Tab 위치  
	nTabStartHor = pFrmInfo->nTabStartHor;
	nTabEndHor = pFrmInfo->nTabEndHor;
	nTabStartPosInFrame = pFrmInfo->nTabStartPosInFrame; // 

	m_nTabId_CntBoard = pFrmInfo->m_nTabId_CntBoard;

	// 외관 판정 시 Error 및 Over로 인한 부분을 처리하기 함.
	m_bErrorFlag = pFrmInfo->m_bErrorFlag | pFrmInfo->m_bOverFlow;	

	if (m_pTabRsltInfo != NULL)
	{
		m_pTabRsltInfo->m_nHeadNo = m_nHeadNo;
		m_pTabRsltInfo->m_nTabNo = nTabNo;
		m_pTabRsltInfo->m_nCellId = m_nTabId_CntBoard;
	}

	m_pImagePtr = pFrmInfo->GetImagePtr();
	pFrmInfo->SetImgPtr(NULL); // 안하면 죽음.

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	//SPC+ 객체도 복사한다.
	m_SpcInspMgr = pFrmInfo->m_SpcInspMgr;
	m_bSaveFlag = pFrmInfo->m_bSaveFlag;
#endif //SPCPLUS_CREATE
}
