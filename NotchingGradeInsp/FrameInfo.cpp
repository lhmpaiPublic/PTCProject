//#include "StdAfx.h"
#include "pch.h"
#include "FrameInfo.h"

CFrameInfo::CFrameInfo(void)
{
	m_nFrameCount = -1 ;
	m_nHeadNo = -1 ;
	m_bDummyData = FALSE;
	// 23.02.20 Ahn Add Start
	m_bOverFlow = FALSE;
	// 23.02.20 Ahn Add End

	for (int i = 0; i < MAX_TACTIMELIST; i++)
		m_tacTimeList[i] = 0.0f;

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	//SPC+ INSP 객체 포인터를 NULL 초기화한다.
	m_SpcInspMgr = NULL;
#endif // SPCPLUS_CREATE
}

CFrameInfo::~CFrameInfo(void)
{
}

void CFrameInfo::initImagePtr(int nSize)
{
	memset(m_pImagePtr, 0x00, sizeof(BYTE) * (nSize + 1));
}

void CFrameInfo::initImagePtr(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	memset(m_pImagePtr, 0x00, sizeof(BYTE) * ((nWidth * nHeight) + 1));
}

BYTE * CFrameInfo::GetImagePtr()
{
	return m_pImagePtr ;
}

void CFrameInfo::SetImgPtr( BYTE *pImgPtr, int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	memset(m_pImagePtr, 0x00, sizeof(BYTE) * ((nWidth * nHeight) + 1));
	CopyMemory(m_pImagePtr, pImgPtr, sizeof(BYTE) * m_nWidth * m_nHeight);
}
