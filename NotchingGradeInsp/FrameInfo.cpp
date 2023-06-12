//#include "StdAfx.h"
#include "pch.h"
#include "FrameInfo.h"

CFrameInfo::CFrameInfo(void)
{
	m_nFrameCount = -1 ;
	m_nHeadNo = -1 ;
	m_pImagePtr = NULL ;
	m_bDummyData = FALSE;
	// 23.02.20 Ahn Add Start
	m_bOverFlow = FALSE;
	// 23.02.20 Ahn Add End

	//SPC+ INSP 객체 포인터를 NULL 초기화한다.
	m_SpcInspMgr = NULL;
}

CFrameInfo::~CFrameInfo(void)
{
	if( m_pImagePtr != NULL ){
		delete []m_pImagePtr ;
		m_pImagePtr = NULL ;
	}
}


BYTE * CFrameInfo::GetImagePtr()
{
	return m_pImagePtr ;
}

void CFrameInfo::SetImgPtr( BYTE *pImgPtr )
{
	m_pImagePtr = pImgPtr ;
}
