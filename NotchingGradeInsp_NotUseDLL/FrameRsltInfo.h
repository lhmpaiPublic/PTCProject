#pragma once

#include <vector>
#include "FrameInfo.h"
//class CDefectDataCtrl;
//class CFrameInfo ;

#include  "CTabRsltInfo.h"

//Top Frame, Bottom Frame °´Ã¼ »ó¼Ó
class CFrameRsltInfo : public CFrameInfo
{
public:
	CFrameRsltInfo(void);
	~CFrameRsltInfo(void);

public:
	double dStartPosX;
	double dStartPosY;

	// ê²°í•¨ ê²€ì¶œ 
	int nDefCount;
	int nMeasureCount;
	CTabRsltInfo *m_pTabRsltInfo;
	// 23.02.21 Ahn Add Start
	double dSharpness;
	int	   nBrightAverage;
	// 23.02.21 Ahn Add End

	void Copy(CFrameInfo* pFrmInfo);
};

// 22.06.09 Ahn Add Start
class CFrameCeInfo : public CFrameInfo
{
public:
	CFrameCeInfo(void) {};
	~CFrameCeInfo(void) {};

public:

	CString strTime;
	CString strJudge;
	CString strTopJudge;
	CString strBtmJudge;
	int nSurfaceNgCnt;
	int nFoilExpNgCnt;
	double dTopMaxSize;
	double dBtmMaxSize;
//	double dTopMaxSIzeY;
//	double dBtmMaxSIzeY;
	CString strMarking;

	// 22.06.20 Ahn Add Start
	int DefectMaxSize[MAX_CAMERA_NO][MAX_INSP_TYPE];
	// 22.06.20 Ahn Add End
};
// 22.06.09 Ahn Add End
