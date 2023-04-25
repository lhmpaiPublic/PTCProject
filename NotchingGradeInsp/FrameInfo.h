#pragma once

#include <vector>

//이미지 데이터 값 및 카메라 프레임에 대한 속성 정보
//Top/Bottom 구분
//Tab Ceramic 어깨선/끝선 위치, ID, 번호 정보
//이미지의 폭 넓이정보
//Frame의 시작 Pixel 좌표
//m_pImagePtr : 이미지 데이터 값
//m_bOverFlow 오버플로우 체크
class CFrameInfo
{
public:
	CFrameInfo(void);
	~CFrameInfo(void);
	//임시 로그 카운터
	int TempLogCount;

	enum {
		en_TopFrame = 0,
		en_BottomFrame = 1,
	};
protected :
	BYTE	*m_pImagePtr ;
	
public :
	BOOL	m_bErrorFlag ;	// Image 처리 Pass
	long	m_nFrameCount; // Frame 번호
	int		m_nHeadNo;
	int		m_nHeight;		// Image 길이
	int		m_nWidth;		// Image 폭
	int		m_nBand;		// 1 : Mono, 3 : Color
	int		nOverlapSize;	// 오버렙된 크기
	int		m_nTabLevel;	// Tab의 Ceramic 어깨선/끝선 위치
	int		m_nInspMode;	// en_HeadFrame/ en_TailFrame 검사 모드

	int		m_nTabLeft;
	int		m_nTabRight;

	double  m_dTopPosY;	// Frame의 시작 Pixel의 Y 좌표
	BYTE* GetImagePtr();
	void SetImgPtr(BYTE* pImgPtr);
	int		nTabNo;				// 
	int		m_nTabId_CntBoard;	// 카운터 보드에서 부여받은 Marking용 Tab ID
	int		nTopPosYInFrame;

	// Tab 위치  
	int nTabStartHor;
	int nTabEndHor;
	int nTabStartPosInFrame; // 

	int m_nBndElectrode; // 전극 경계위치 // 22.05.10 Ahn Add 

	BOOL	m_bDummyData; // Debug Mode용 Data 처리 불요.
	
	virtual void Copy(CFrameInfo* pFrmInfo) {};

	// 22.12.09 Ahn Add Start
	//SYSTEMTIME m_sysStTime;
	LARGE_INTEGER m_stTime ;
	double		  m_dFrecuency ;
	// 22.12.09 Ahn Add End

	// 23.02.20 Ahn Add Start
	BOOL	m_bOverFlow;
	// 23.02.20 Ahn Add End
};
