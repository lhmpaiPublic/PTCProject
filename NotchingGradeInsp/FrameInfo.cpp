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

	//Tab Pitch 가 이상이 있을 때
	m_bErrorTabPitch = FALSE;
	//Tab Pitch(mm)
	m_dTabPitch = 0.0f;

	//Tab Width(넓이)가 이상이 있을 때
	m_bErrorTabWitch = FALSE;
	//Tab Pitch(mm)
	m_dTabWidth = 0.0f;


//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	//SPC+ INSP 객체 포인터를 NULL 초기화한다.
	m_SpcInspMgr = NULL;
#endif // SPCPLUS_CREATE
}

CFrameInfo::~CFrameInfo(void)
{
	if( m_pImagePtr != NULL ){
		delete m_pImagePtr ;
		m_pImagePtr = NULL ;
	}
}


FrameImagePtr* CFrameInfo::GetImagePtr()
{
	return m_pImagePtr;
}

void CFrameInfo::SetImgPtr(FrameImagePtr* pImgPtr )
{
	m_pImagePtr = pImgPtr ;
}
