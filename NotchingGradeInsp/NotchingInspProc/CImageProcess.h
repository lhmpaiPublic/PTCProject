#pragma once

#include <vector> 

class CRecipeInfo;
class CTabCondition;
class CPixel {
public :
	int x;
	int y;
	BYTE btBright;
	BYTE btOrgBri;
};
class CBlockData {
public:
	CBlockData() {
		nIndex = 0;
		nType = 0;	
		nPixelCnt = 0;
		nHeight = 0;
		rcRect.left = 0;
		rcRect.right = 0;
		rcRect.top = 0;
		rcRect.bottom = 0;
		nBriAve = 0;
		nBriMax = 0;
		nBriMin = 0 ;
		nBriSum = 0;
		dWidth = 0.0;
		dHeight = 0.0;
		dJudgeSize = 0.0;
		nOrgBriSum = 0;
		nOrgBriAve = 0;
		nOrgBriMin = 0;
		nOrgBriMax = 0;
		nDefJudge = JUDGE_OK;
		bDeleteFlag = FALSE;
		nMaxPosY = 0;
		bDistanceOut = FALSE ;
		dDistance = 0.0 ;
	//	m_VecCircum.clear() ;// 22.04.15 Ahn Add

	};
	int nIndex;
	int nType;			// 0.Foil노출 , 1.Dross
	int nPixelCnt;
	int nWidth;
	int nHeight;
	int nBriAve;
	int nBriMax;
	int nBriMin;
	int nBriSum;
	int nOrgBriSum;
	int nOrgBriAve;
	int nOrgBriMin;
	int nOrgBriMax;
	double dWidth;
	double dHeight;
	double dJudgeSize;
	int nDefJudge;

	CRect rcRect;

	//int nExtPixCnt; // 외곽의 Pixel 수
	int nMaxPosY;
	BOOL bDeleteFlag;

	BOOL bDistanceOut;	
	double dDistance;

	//std::vector< CPixel > m_VecCircum;// 22.04.15 Ahn Add
	int nDefPos; // 0 : Left 1 :Left Round 2 : Right 3 : Right Round 
};

class CTabInfo {
public:
	CTabInfo() { nFrameCount = 0; nTabLeft = 0; nTabRight = 0; nLeft = 0; nRight = 0; nTabNo = 0; nTabNoInFrame = 0; nCenter = 0; nImageLength = 0; nTabStartPosInFrame = 0; nTabWidth = 0; m_bErrorFlag = FALSE;  m_GrabCallBCDId = 64;  m_GrabCallTime = 0;  pImgPtr = NULL; pImgBtmPtr = NULL; };
	void ResetData() { nFrameCount = 0; nTabLeft = 0; nTabRight = 0; nLeft = 0; nRight = 0; nTabNo = 0; nTabNoInFrame = 0; nCenter = 0; nImageLength = 0; nTabStartPosInFrame = 0; nTabWidth = 0; m_GrabCallBCDId = 64;  m_GrabCallTime = 0; pImgPtr = NULL; pImgBtmPtr = NULL; };
	int nFrameCount;	//
	int nTabNoInFrame;	// 
	int nTabNo;		//
	int nLeft;			//
	int nRight;		//
	int nCenter;
	int nTabLeft;		// 탭 머리 왼쪽
	int nTabRight;		// 탭 머리 오른쪽
	int nTabWidth;
	BOOL m_bErrorFlag;
	BOOL m_bIsPET;		// PET 인식

	//QWORD qwHeadStPos;	// 검사 시작 Frame 을 기준으로 Tab Head의 Pixel 위치
	int	nTabStartPosInFrame ; // Frame에서 Tab의 시작위치 (거리계산용)

	//Grab Image 얻은 시점의 BCD ID
	int m_GrabCallBCDId;

	//Grab Call : Encoder Count
	int m_GrabCallEncoderCount;

	//Grab Call 호출 타임(ms)
	UINT64 m_GrabCallTime;

//	int nWidth;
	int nImageLength;
	BYTE* pImgPtr;
	BYTE* pImgBtmPtr;
};

class CRegionInfo {
	// 관심영역의 Pixel Info
	BYTE* pRoiPtr;
	// 관심 영역의 외접사각 

public:
	int	m_nLeft;
	int	m_nRight;
	int	m_nTop;
	int m_nBottom;

	BYTE m_bProcPit;

	BOOL m_bUseFifoMode;
	typedef std::vector<CPixel> VEC_FIFO;
	VEC_FIFO* pVecFifoPtr;

	CRegionInfo();
	~CRegionInfo();

	void SetRect(CRect rect);
	int GetWidth() { return (m_nRight - m_nLeft); };
	int GetHeight() { return (m_nBottom - m_nTop); };
	BYTE* GetRoiPtr() { return pRoiPtr; };
	void SetRoiPtr(BYTE* pImgPtr) { pRoiPtr = pImgPtr; };
	VEC_FIFO* GetFifoPtr();
	void SetProcBit(BYTE bBit) { m_bProcPit = bBit; };
};


#define DIR_HOR 0
#define DIR_VER 1

#define EDGE_IMG_HEIGHT 128
#define EDGE_IMG_WIDTH	128

class CDefectDataCtrl;
class CTabRsltInfo;

class CImageProcess
{
public:
	typedef std::vector< CBlockData > _VEC_BLOCK;
	typedef std::vector< CBlockData* > _VEC_BLOCK_PTR;
	typedef std::vector<CPoint> VEC_ROUND_INFO;

	typedef struct {
		int start;
		int count;
		int ypos;
		int nBriSum;
		BYTE btBriMax;
		BYTE btBriMin;
		int nOrgBriSum;
		BYTE btOrgBriMax;
		BYTE btOrgBriMin;
		CBlockData* pPtr;
	} _LINE_DATA;

	typedef std::vector< _LINE_DATA > VEC_LINE_DATA;

	typedef struct
	{
		CRect rcArea;
		int nBright;
		BOOL bFind;

	} _PET_INFO;

	typedef std::vector< _PET_INFO > VEC_PET_INFO;

	typedef struct
	{
		CRect rcArea;
		int nBright;
		BOOL bError;

	} _BRIGHT_INFO;


	// >>>>>>>>>>>>>>>>>>>> 여기부터 제작 필요 >>>>>>>>>>>>>>>>>>>>>>>
	//Filter 처리
	static int FilterProc(BYTE* pImgae, BYTE* pResultPtr, int nFilterNo) { return 0; };

	//결함 검출처리 

	static BOOL GetOrgImageBright(const BYTE* pImgPtr, int nWidth, int nHeight, CRegionInfo::VEC_FIFO* pVecFifo);
	static BOOL MergeLineData(_LINE_DATA* pNewData, _LINE_DATA* pPreData); // pSrcData 에 pTarData를 Merge 하고 pTarData를 제거함.
	static BOOL LoopLabeling(CRegionInfo* pRoiInfo, int nWidth, int nHeight, _VEC_BLOCK* pVecBlock, BYTE btBit, int MinSize, double dResolX, double ResolY);
	static BOOL IsLinked(_LINE_DATA* pCurData, _LINE_DATA* pPreData);
	static int GetOneLineFromFifo(VEC_LINE_DATA* pOneLine, CRegionInfo::VEC_FIFO* pVecFifo, int nStartPos);

	enum {
		en_Filter_8 = 0,
		en_Filter_4,
		en_Filter_8_2,
		en_Filter_4_2,
		en_Filter_48_2,
		en_Filter_X,
		en_Filter_Top,
		en_Filter_Btm,
		en_Filter_Left,
		en_Filter_Righ,
		en_Filter_ALL,
		en_Filter_MAX
	};

	enum { // Image 처리 정보 8 bit 정보
		en_Black_Bit = 0x80,
		en_White_Bit = 0x40,
		en_FoilExp_Bit = 0x80,
		en_Dross_Bit = 0x40,
		en_Surface_Bit = 0xff,

		en_ProcBlack_Bit = 0x03,
		en_ProcWhite_Bit = 0x04,
		en_Delete_Bit = 0x05,
	};

	// 22.08.5 Ahn Add Start
	enum {
		en_Left = 0,
		en_LeftRound = 1,
		en_Right = 2,
		en_RightRound = 3,
	};
	// 22.08.5 Ahn Add End

	static int RemoveNoise(BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nFilterNo, BYTE btBit);
	static int RemoveNoise(BYTE* pImgPtr, int nWidth, int nHeight, int nFilterNo, CRect rcRange, BYTE btBit);
	// 22.01.19 Ahn Add Start
	static int Erosion(BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nMode, CRect rcRange, BYTE btBit);
	static int Exptention(BYTE* pImgPtr, BYTE* pRsltPtr, int nWindth, int nHeight, int nMode, CRect rcRange, BYTE btBit);
	// 22.01.19 Ahn Add End

	// <<<<<<<<<<<<<<<<<<< 여기까지 제작 필요 <<<<<<<<<<<<<<<<<<<<<<<<<
	// 전체 영역
	static int Threshold(const BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nMin, int nMax);
	// ROI 사용 
	static int Threshold(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, int nWidth, int nHeight, int nMin, BOOL bClearRslt, BOOL bModeDark = FALSE );
	static int Threshold_RoundMask(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nInspWidth, int nMin, int nMaskOffset, int nLimitRight, int nMode, BOOL bClearRslt, BOOL bModeSide = CImageProcess::en_HeadSide );
	static int Threshold_RoundMask_Negative(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nStartX, int nEndX, int nMin, int nLimitRight, BOOL bClearRslt, BOOL bModeSide = CImageProcess::en_HeadSide);
	enum {
		en_LeftSide = 0,
		en_RightSide = 1,

		en_ModeWave = 1,
		en_ModeCrak = 2,
		en_ModeBur	= 3,
	};
	static int Threshold_ByEdgeLine(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nInspWidth, int nMinThres, int nMaxThres, int nMaskOffset, int nMode, BOOL bClearRslt, BOOL bModeSide = CImageProcess::en_LeftSide);
	// 22.07.14 Ahn Add End
	
	// Projection 처리 
	static int GetProjection(const BYTE* pImgPtr, int* pProjection, int nWidth, int nHeight, CRect rectPrj, int nDir, int nSampling, BOOL bModeSum = TRUE);
	static int GetProjection(const BYTE* pImgPtr, int* pProjection, int nWidth, int nHeight, int nStartX, int nEndX, int nStartY, int nEndY, int nDir, int nSampling, BOOL bModeSum = TRUE);
	//static int GetAverageFromPrjData(int* pPrjData, int* pAveData, int nLength, int nAveRage);
	static int GetBundary_FromPrjData(int* pnPrjData, int nLength, int nCompWidth, int nMode, int nNegCoatHeight );

	// 평균화 처리 
	static int MeanImageDirection_Round(const BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc, int nMeanSize, int nThresLevel);
	static int MeanImageDirection(const BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc, int nMeanSize, int nDir);
	static int MeanImageDirection(const BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, int nMeanSize, int nDir);
	static int ImageMean(const BYTE* pOrgPtr, BYTE* pTarPtr, int nWidth, int nHeight, int nMeanX, int nMeanY);
	enum {
		en_FillTop = 0x0001,
		en_FillBottom = 0x0002,
		en_FillAll = 0x0003,
		en_NoFill = 0x0000,
	};
	static int ImageMean_Part(const BYTE* pOrgPtr, BYTE* pTarPtr, int nWidth, int nHeight, CRect rcRange, int nMeanX, int nMeanY, int nMode );
	static int PreFilter2x2(BYTE* pOrgPtr, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc);
	static int PreFilter(BYTE* pImgPtr, BYTE* pMeanPtr, int nWidth, int nHeight, CRect rcProc);
	static int PreFilterCross(BYTE* pOrgPtr, BYTE* pProcImgPtr, int nWidth, int nHeight, CRect rectProc);
	static int PreFilter1x2(BYTE* pOrgPtr, BYTE* pProcImgPtr, int nWidth, int nHeight, CRect rectProc);

	enum {
		en_Edge_MaxDiff = 0,
		en_Edge_SetBright = 1,
	};
	// Edge 검출처리 
	enum {
		en_sobel_Hor=0,
		en_sobel_Ver=1,
		en_sobel_Max=2,
	};
	static int Edge_Sobel(BYTE* pImgPtr, BYTE* pProcImg, int nWidth, int nHeight, CRect rcProc, int nMode);

	static int EdgeDetectImageToBoth_RndInfo(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nCompWidth, int nMagnif, int nDir);
	static int EdgeDetectImageToBoth_RndInfo_Threshold(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nThreshold, int nPosition, int nDefaultLv, int nSide );
	static int EdgeDetectByRndInfo_Negative(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nThresMin, int nThresMax, int nPosition, int nDefaultLv, int nSide);
	static int EdgeDetectImageToArray(BYTE* pImgPtr, int* pnResltArr, int nWidth, int nHeight, CRect rectProc, int nCompWidth, int nMagnif, int nDir);
	static int EdgeDetectImageToBoth_CropArea(BYTE* pImgPtr, BYTE* pProcImg, int* pnResltArr, int nWidth, int nHeight, CRect rectProc, int nCompWidth, int nMagnif, int nDir);
	// 23.01.20 Ahn Add Start
	static int EdgeDetectImageToBoth_BaseBright(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nThresMin, int nThresMax, int nPosition, int nDefaultLv, int nSide);
	// 23.01.20 Ahn Add End

	// 원 찾기
	static int CalcCirclePos(CPoint point1, CPoint point2, int nRadius, CPoint* pntCenter, int nDir);
	static int CalcCirclePosFrom3Point(CPoint p1, CPoint p2, CPoint p3, int nDir, CPoint* pntCenter, int* pnRadius);
	static double CalcRoundDef_Size(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CRect rcRnd, CBlockData *defInfo, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY);
	static int CalcSizeToEdgeLine(CBlockData* pDefData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, int nMode );

	// 22.02.17 Ahn Modify Start
	static double CalcRoundDef_Size_LeftFoilExp(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CRect rcRnd, CBlockData* defInfo, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType);
	static double CalcRoundDef_Size_RightFoilExp(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CRect rcRnd, CBlockData* defInfo, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType);
	// 22.02.17 Ahn Modify End
	// 22.08.22 Ahn Add Start
	static double CalcInclination_FromRoundInfo(CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo ) ;
	static double CalcRoundDef_SizeLeftOnlyBright(BYTE* pImgPtr, int nWidth, int nHeight, CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType);
	static double CalcRoundDef_SizeRightOnlyBright(BYTE* pImgPtr, int nWidth, int nHeight, CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType);
	static double CalcRoundDef_FindMin(int nMode, CBlockData* defInfo, double dSlope, int nOffset, CRect rcRnd, CRect rcDefect, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, double dResoY);
	// 22.08.22 Ahn Add End
	// 22.04.15 Ahn Add Start
	static double CalcRoundDef_Distance(int nMode, CBlockData* defInfo, int nOffset, CRect rcRnd, CRect rcDefect, CImageProcess::VEC_ROUND_INFO vecLine, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, double dResoY);
	static double CalcRoundDef_Distance_FindMin(int nMode, CBlockData* defInfo, int nOffset, CRect rcRnd, CRect rcDefect, CImageProcess::VEC_ROUND_INFO vecLine, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, double dResoY);
	// 22.04.15 Ahn Add End

	// 22.01.20 Ahn Add Start
	static int CheckDistanceToEdgeLine(CBlockData* pBlock, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dDistance, double dResoX );
	static double CalcDefectMaxSize(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CBlockData* pBlock, double dResoX, double dResoY);
	static double CalcDefectMaxSize_Round(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CBlockData* pBlock, CRect rcRect, double dResoX, double dResoY, int nPosition );
	// 22.01.20 Ahn Add End

	// 22.07.14 Ahn Add Start
	static int FindEdgePosFromProjection(int* pnPrjData, int nWidth, int nThreshold );
	static int SearchEdgeLine( BYTE *pMeanPtr, int nWidth, int nHeight, CRect rcRect, CImageProcess::VEC_ROUND_INFO* vecEdgeInfo, int &nAveLine, int nEdgeThreshold, int nDir );
	// 22.07.14 Ahn Add End

	// Tab 관련 함수들
	typedef struct Sector {
		int nStartPos;
		int nEndPos;
		int nMode;// 0 = LOW , 1 = High
		BOOL bDelete;
	} ST_SECTOR;

	typedef std::vector< ST_SECTOR > VEC_SECTOR;
	typedef std::vector< CTabInfo > _VEC_TAB_INFO;

	enum {
		en_Low = 0,
		en_High = 1,
		en_Max = 2,
	};

	enum {
		en_FindLeft = 0,
		en_FindRight = 1,
		en_FindTop = 0,
		en_FindBottom = 1,
		en_FIndStartPos = 0,
		en_FindEndPos = 1,
		en_FineMaxDiffMode = 0,
		en_FindThresMode = 1,
		en_FindFromLeft = 0,
		en_FindFromRight = 1,
	};

	enum {
		en_HeadSide = 0,
		en_TailSide = 1,
		en_TopSide = 0,
		en_BottomSide = 1, 
	};

	static int FillArea_byRndInfo(const BYTE *pImage, BYTE* pMeanPtr, int nWidth, int nHeight, CRect rcArea, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, BYTE btLevel, int nPocPos );
	static long GetAreaSum(BYTE* pImage, int nWidth, int nHeight, CRect rectPrj, int nSampling, BOOL bModeSum);
	static long GetAreaSum_BetweenThreshold(BYTE* pImage, int nWidth, int nHeight, CRect rectPrj, int nMin, int nMax, int nSampling, BOOL bModeSum);
	static int FindTabLevel(BYTE* pImagePtr, int nWidth, int nHeight, int* nLevel, CTabCondition tabCond, int nMode, int nDir);
	static int FindTabLevel_Deitail(BYTE* pImagePtr, int nWidth, int nHeight, CTabCondition tabCond, int nFindPos, int* nLevel, int nMode, int nDir);
	static int FindTabLevel_FromProjection(int* pnPrjData, int nLength, int nCompWidth); // CompWidth으로 좌우 비교하여 차이가 가장 많이나는 위치를 찾는 함수.

	// 22.04.13 Ahn Add Start
	static int FindTabLevel_Simple(const BYTE* pImgPtr, int nWidth, int nHeight, int nFindPos, CRecipeInfo* pRecipeInfo, VEC_SECTOR* pVecSector, int* pnLevel);
	// 23.02.24 Ahn Modify Start
	//static int FindBoundary_FromPrjData(int* pnPrjData, int nLength, int nTargetBright, int nMode);
	static int FindBoundary_FromPrjData(int* pnPrjData, int nLength, int nTargetBright, int nMode, BOOL bFindDark = FALSE, bool bLogOk = false);
	// 23.02.24 Ahn Modify End
	// 22.04.13 Ahn Add End

	static int FindTab_Negative(const BYTE* pImgPtr, int nWidth, int nHeight, int nTabFindPos, CRecipeInfo *pRecipeInfo, VEC_SECTOR *pVecSector, int *pnLevel);
	static int FindLevelBottom_Negative(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int* pnLevel, int nFindDir );
	// 23.02.24 Ahn Add Start
	static int FindLevelBottom_BrightRoll(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int* pnLevel, int nFindDir);
	// 23.02.24 Ahn Add End

	static int FindTabHeadCeramicBoundary(BYTE *pImagePtr, int nWidth, int nHeight, CRect rcRange );
	static int GetBoundaryLineFromEdgeArray(int* pnEdgeArr, int nLength, int nLeft, int nRight, CRect* rcReft, CRect* rcRight);
	static int GetBoundaryLineFromEdgeArray(int* pnEdgeArr, int nLength, int* pnBoundaryLine);
	static int SmoothVecRoundData(VEC_ROUND_INFO* pVecRoundData, BOOL bMode);
	static int DrawPixel_BoundaryLine(BYTE* pImgPtr, int nWidth, int nHeight, CImageProcess::VEC_ROUND_INFO &pVecRndInfo, BYTE btBit);

	//static int ImageProcessHeadSide_New(BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE **pImgPtrArr = NULL , int nArrCnt = 0);
	//static int ImageProcessTailSide_New(BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pDefInfoCtrl,BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL , int nArrCnt = 0 );

	//// 22.05.13 Ahn Add Start
	//// 양극 검사 Foil 노출만 검출 (Dross제거)
	//static int ImageProcessFoilExpTopSide_AreaDiff(BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	//static int ImageProcessFoilExpBottomSide_AreaDiff(BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	//// 22.05.13 Ahn Add End

	// 양극 검사 Dross & Foil 노출 구분 검출
	static int ImageProcessTopSide_AreaDiff(const BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	static int ImageProcessBottomSide_AreaDiff(const BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);

	static int ImageProcessTopSide_Negative(const BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	static int ImageProcessBottomSide_Negative(const BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	// 22.05.09 Ahn Add Start
	static int ImageProcessDetectSurface(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, CRect rcArea, CTabRsltInfo* pDefInfoCtrl, int nCamPos, BOOL bSimMode, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	// 22.05.09 Ahn Add End

	// 23.02.10 Ahn Add Start
	static int ImageProcessTopSide_BrightRoll(const BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	static int ImageProcessBottomSide_BrightRoll(const BYTE* pImgPtr, int nWidth, int nHeigth, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pDefInfoCtrl, BOOL bSimMode = 0, BYTE** pImgPtrArr = NULL, int nArrCnt = 0);
	// 23.02.10 Ahn Add End

	static int SaveCropImage(const BYTE* pImgPtr, int nWidth, int nHeight, CRect rcCrop, CString strFilePath, CString strFileName);
	static int CopyPartImage(BYTE* pTarImgPtr, BYTE* pSrcImgPtr, int nWidth, int nHeight, CRect rcCopy);
	static int ResizeImage(const BYTE* pImgPtr, BYTE* pResizePtr, int nWidth, int nHeight, int nZoomOut);

	// 23.01.20 Ahn Modify Start
	//static int GetBoundaryRoundInfo_ByBriDist( BYTE* pOrgImg, BYTE *pRsltImg, int nWidth, int nHeight, CRect rcRound, int nThreshold, int nTopCutPos,  CImageProcess::VEC_ROUND_INFO* pVecRoundInfo, int nMode, int nFindDir = en_FindFromRight);
	static int GetBoundaryRoundInfo_ByBriDist(BYTE* pOrgImg, BYTE* pRsltImg, int nWidth, int nHeight, CRect rcRound, int nThreshold, int nTopCutPos, CImageProcess::VEC_ROUND_INFO* pVecRoundInfo, int nMode, int nFindDir = en_FindFromRight, BOOL bModeBaseBright = FALSE );
	// 23.01.20 Ahn Modify End
	// 22.01.17 Ahn Add Start
	static int GetBoundaryRoundInfo(BYTE* pOrgImg, BYTE* pRsltImg, int nWidth, int nHeight, CRect rcRound, int nThreshold, CImageProcess::VEC_ROUND_INFO* pVecRoundInfo, int nFindDir );
	// 22.01.17 Ahn Add End

	// 22.05.09 Ahn Add Start
	static int GetBoundaryOfElectorde(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nFindDir );
	static int GetBoundaryOfElectordeBottom(BYTE* pImgPtr, int nWidth, int nHeight, int *pnLevel, CRecipeInfo* pRecipeInfo );
	// 22.05.09 Ahn Add End

	static int CheckRect(CRect* pRect, int nWidth, int nHeight);
	// 22.11.21 Ahn Modify Start
	//static int AddDefectInfoByBlockInfo(CImageProcess::_VEC_BLOCK* pBlockInfo, CRecipeInfo* pRecipeInfo, CTabRsltInfo* pTabRsltInfo, int nCamPos, int nSaveCount );
	static int AddDefectInfoByBlockInfo(CImageProcess::_VEC_BLOCK* pBlockInfo, CRecipeInfo* pRecipeInfo, CTabRsltInfo* pTabRsltInfo, int nCamPos, int nSaveCount, int nFrmStPos , double dResoY );
	// 22.11.21 Ahn Modify End
	static int BlockLink(CImageProcess::_VEC_BLOCK* pBlockInfo, CRecipeInfo* pRecipeInfo, int nDefType, int nCamPos);

	// 22.05.24 Ahn Add Start
	static int SortingBlockInfo(_VEC_BLOCK* pVecBlock);
	static int MergeBlockList(_VEC_BLOCK* pDestVlock, _VEC_BLOCK vecFirst, _VEC_BLOCK vecSecond);
	// 22.05.24 Ahn Add End

	// 22.07.20 Ahn Add Start
	static int MergeBlockInfo(CBlockData& DataIn, CBlockData& DataOut, int nCamPos);
	static BOOL IsLinkedBlock(CRect rcPtr, CRect rcTar, CPoint cpRange );
	static int MergeAndLink_BlockInfo(_VEC_BLOCK* pDestVlock, _VEC_BLOCK vecFirst, _VEC_BLOCK vecSecond, CRecipeInfo* pRecipeInfo, int nCamPos );
	// 22.07.20 Ahn Add End
	// Tab 분할용 함수 정의 

	static int DivideSectionBetweenThreshold(int* pnPrjData, int nPrjLength, int nThresMin, int nThresMax, int nMinLength, VEC_SECTOR* pVecSector);
	static int FindTabPos(const BYTE* pImagePtr, int nWidth, int nHeight, int nStartPos, int nEndPos, int nThresMin, int nThresMax, VEC_SECTOR* pVecSector);
	static int CombineTabSector(VEC_SECTOR* pVecSector, CRecipeInfo& RecipeInfo);
	static int ConvertSectToTabInfo(VEC_SECTOR vecSector, _VEC_TAB_INFO* VecTabInfo, int nFrameCount, CTabCondition tabCond);

	// 23.01.20 Ahn Modify Start
	static int FindTabPos_New(BYTE* pImagePtr, int nWidth, int nHeight, int nStartPos, int nEndPos, int nThresMin, int nThresMax, VEC_SECTOR* pVecSector);
	// 23.01.20 Ahn Modify End

	// 22.11.18 Ahn Modify Start
	//static int DivisionTab_FromImageToTabInfo(BYTE* pImgPtr, BYTE *pImgBtmPtr, int nWidth, int nHeight, int nFindPos, int *pnLevel, CRecipeInfo& recipeInfo, CTabInfo* pResvTabInfo, _VEC_TAB_INFO* VecTabInfo );
	static int DivisionTab_FromImageToTabInfo(const BYTE* pImgPtr, const BYTE* pImgBtmPtr, int nWidth, int nHeight, int nFindPos, int* pnLevel, CRecipeInfo& recipeInfo, CTabInfo* pResvTabInfo, _VEC_TAB_INFO* VecTabInfo, int nFrameCount);
	// 22.11.18 Ahn Modify End
	static int DivisionTab_byFixSize(const BYTE* pImgPtr, const BYTE *pImgBtmPtr, int nWidth, int nHeight, int nFixSize, int nStartPos, int nEndPos, BOOL bIsPET, _VEC_TAB_INFO* VecTabInfo);

	//Tab Pitch를 계산해서 사용하기
	static int TabPitcPixelhCalculate(double RecipeTabPitch, double dResolY);

	// Analyze Image
	static int GetHistoList(BYTE* pImgPtr, int nWidth, int nHeight, CRect rcRange, int* nHistoArr);

	static int GetTabHeadPos(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo *pRecipeInfo, int& nLeft, int& nRight, int& nLevel);

	static int DiffProcImage(BYTE* pImgPtr, BYTE* pMeanImg, BYTE* pProcImg, int nWidth, int nHeight, CRect rcProc, double dMagnif, BYTE btThresLevel , BOOL bDarkEmpMode );
	//static int nMeanImageRound(BYTE* pImgPtr, BYTE* pMeanImg, int nWidth, int nHeight, int nMeanSize, CImageProcess::VEC_ROUND_INFO *pRndInfo, CRect rcProc, int nDirection);

	static int GetMinImage(BYTE* pResultPtr, BYTE* pSrcPtr, BYTE* pTarPtr, int nWidth, int nHeight, CRect rcProc);
	static int GetMaxImage(BYTE* pResultPtr, BYTE* pSrcPtr, BYTE* pTarPtr, int nWidth, int nHeight, CRect rcProc); // 22.05.30 Ahn Add


	// 23.01.20 Ahn Add Start
	static int GetBoundaryRoundInfo_BaseBright(BYTE* pOrgImg, BYTE* pRsltImg, int nWidth, int nHeight, CRect rcRound, int nThreshold, CImageProcess::VEC_ROUND_INFO* pVecRoundInfo, int nFindDir);
	// 23.01.20 Ahn Add End

	// 23.02.16 Ahn Add Start
	static double	GetIqSharpnessValue(const BYTE* pOrgImg, int nWidth, int nHeight, CPoint cpStartPoint);
	static int		GetBrightAverage(const BYTE* pOrgImg, int nWidth, int nHeight, CPoint cpStartPoint);
	// 23.02.16 Ahn Add End

	static BOOL	FindPetFilm(const BYTE* pOrgImg, int nImageWidth, int nImageHeight, CRecipeInfo& RecipeInfo, VEC_PET_INFO* vstPetInfo, int nCamPos);
	static BOOL SaveOriginImage(const BYTE* pOrgImg, int nImageWidth, int nImageHeight, CString strComment);

	static BOOL CheckBright(const BYTE* pOrgImg, int nImageWidth, int nImageHeight, CRecipeInfo& RecipeInfo, _BRIGHT_INFO* stBrightInfo, int nCamPos);

};

