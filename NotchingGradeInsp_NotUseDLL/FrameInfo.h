#pragma once

#include <vector>


class CFrameInfo
{
public:
	CFrameInfo(void);
	~CFrameInfo(void);

	enum {
		en_TopFrame = 0,
		en_BottomFrame = 1,
	};
protected :
	BYTE	*m_pImagePtr ;
	
public :
	BOOL	m_bErrorFlag ;	// Image ó�� Pass
	long	m_nFrameCount; // Frame ��ȣ
	int		m_nHeadNo;
	int		m_nHeight;		// Image ����
	int		m_nWidth;		// Image ��
	int		m_nBand;		// 1 : Mono, 3 : Color
	int		nOverlapSize;	// �������� ũ��
	int		m_nTabLevel;	// Tab�� Ceramic �����/���� ��ġ
	int		m_nInspMode;	// en_HeadFrame/ en_TailFrame �˻� ���

	int		m_nTabLeft;
	int		m_nTabRight;

	double  m_dTopPosY;	// Frame�� ���� Pixel�� Y ��ǥ
	BYTE* GetImagePtr();
	void SetImgPtr(BYTE* pImgPtr);
	int		nTabNo;				// 
	int		m_nTabId_CntBoard;	// ī���� ���忡�� �ο����� Marking�� Tab ID
	int		nTopPosYInFrame;

	// Tab ��ġ  
	int nTabStartHor;
	int nTabEndHor;
	int nTabStartPosInFrame; // 

	int m_nBndElectrode; // ���� �����ġ // 22.05.10 Ahn Add 

	BOOL	m_bDummyData; // Debug Mode�� Data ó�� �ҿ�.
	
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
