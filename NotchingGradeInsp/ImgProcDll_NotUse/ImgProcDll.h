#pragma once

#define IMGPROCLIBRARY_EXPORTS 

#ifdef IMGPROCLIBRARY_EXPORTS
#define IMGPROCLIBRARY_API __declspec(dllexport)
#else
#define IMGPROCLIBRARY_API __declspec(dllimport)
#endif

#include <vector> 

#define MAX_BLOCK_PIXEL 32768 

class CPixel {
public:
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
			nDefJudge = 0; // 0 : JUDGE_OK 
			bDeleteFlag = FALSE;
			nMaxPosY = 0;
			bDistanceOut = FALSE ;
			dDistance = 0.0 ;
		//	m_VecCircum.clear() ;
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
	// Pos
//	int nLeft;
//	int nRight;
//	int nTop;
//	int nBottom;
	double dWidth;
	double dHeight;
	double dJudgeSize;
	int nDefJudge;
	RECT rcRect;
	int nMaxPosY;
	BOOL bDeleteFlag;
	BOOL bDistanceOut;	
	double dDistance;
	int nDefPos; // 0 : Left 1 :Left Round 2 : Right 3 : Right Round 
};

typedef std::vector< CBlockData > _VEC_BLOCK;
typedef std::vector< CBlockData* > _VEC_BLOCK_PTR;
typedef std::vector<POINT> VEC_ROUND_INFO;

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


class CRegionInfo {
	// 관심영역의 Pixel Info
	BYTE* pRoiPtr;
	// 관심 영역의 외접사각 

public:
	RECT m_rect;
	BYTE m_bProcPit;

	BOOL m_bUseFifoMode;
	typedef std::vector<CPixel> VEC_FIFO;
	VEC_FIFO* pVecFifoPtr;

	CRegionInfo() {
		m_bUseFifoMode = FALSE;
		m_rect.bottom = 0;
		m_rect.top = 0;
		m_rect.left = 0;
		m_rect.right = 0;
		pRoiPtr = NULL;
		pVecFifoPtr = new VEC_FIFO;
		pVecFifoPtr->clear();
	} ;
	~CRegionInfo() {
		if (pVecFifoPtr != NULL) {
			pVecFifoPtr->clear();
			delete pVecFifoPtr;
			pVecFifoPtr = NULL;
		}
	};

	void SetRect(RECT rect) {	m_rect = rect; };
	int GetWidth() { return ( m_rect.right - m_rect.left); };
	int GetHeight() { return ( m_rect.bottom - m_rect.top); };
	BYTE* GetRoiPtr() { return pRoiPtr; };
	void SetRoiPtr(BYTE* pImgPtr) { pRoiPtr = pImgPtr; };
	VEC_FIFO* GetFifoPtr() { return pVecFifoPtr; };
	void SetProcBit(BYTE bBit) { m_bProcPit = bBit; };
};

namespace ImgProcDll 
{

	#define DIR_HOR 0
	#define DIR_VER 1

	enum {
		en_FillTop = 0x0001,
		en_FillBottom = 0x0002,
		en_FillAll = 0x0003,
		en_NoFill = 0x0000,
	};

	enum {
		en_TopSide = 0,
		en_BottomSide = 1,
		en_LeftSide = 0,
		en_RightSid = 1, 
	};

	enum {
		en_ModeFoilExp = 0,
		en_ModeFoilExpOut = 1,
		en_ModeSurface = 2,
		en_ModeFoilBoth = 3, 
		en_ModeMax = 4,
	};

	enum { // Image 처리 정보 8 bit 정보
		en_Black_Bit = 0x80,
		en_White_Bit = 0x40,
		en_FoilExp_Bit = 0x80,
		en_Dross_Bit = 0x40,
		en_Surface_Bit = 0xff,
		//en_ProcBlack_Bit = 0x03,
		//en_ProcWhite_Bit = 0x04,
		//en_Delete_Bit = 0x05,
		en_DefMode_FoilExp = 0,
		en_DefMode_FoilOut = 1,
		en_DefMode_Surface = 2,
	};

	typedef std::vector< _LINE_DATA > VEC_LINE_DATA;

	extern "C" IMGPROCLIBRARY_API int InflateRECT(RECT * pRect, int dx, int dy);

	// Aramide Mow 검출 처리
	extern "C" IMGPROCLIBRARY_API int ProcAramideCompare(BYTE* pImgPtr, BYTE* pProcPtr, int nWidth, int nHeight, RECT rcMask);

	// Remove Noise
	extern "C" IMGPROCLIBRARY_API int RemoveNoise(BYTE * pImgPtr, BYTE * pRsltPtr, int nWidth, int nHeight, int nCnt);

	// 검출결과 Labeling 
	extern "C" IMGPROCLIBRARY_API BOOL IsLinked(_LINE_DATA * pCurData, _LINE_DATA * pPreData);
	extern "C" IMGPROCLIBRARY_API int GetOneLineFromFifo(VEC_LINE_DATA * pOneLine, CRegionInfo::VEC_FIFO * pVecFifo, int nStartPos);
	extern "C" IMGPROCLIBRARY_API int Labeling(CRegionInfo * pRoiInfo, int nWidth, int nHeight, _VEC_BLOCK * pVecBlock, int nMode, int MinSize, double dResolX, double dResolY);

	extern "C" IMGPROCLIBRARY_API int AddFIFO(BYTE* pImgPtr, CRegionInfo& info, int nWidth, int nHeight, BYTE btBit);

	// Block 처리
	extern "C" IMGPROCLIBRARY_API int BlockLink( _VEC_BLOCK* pBlockInfo, int nLinkX, int nLinkY, int nDefType, int nCamPos, BOOL bRemove = FALSE);
	extern "C" IMGPROCLIBRARY_API int BlockDelLongLength(_VEC_BLOCK * pBlockInfo, int nLongLength);
	extern "C" IMGPROCLIBRARY_API int BlockDelUnderSize(_VEC_BLOCK * pBlockInfo, int nMinSize);
	extern "C" IMGPROCLIBRARY_API int BlockDelete(_VEC_BLOCK * pBlockInfo, int nMinSize, int nDefType, int nCamPos, BOOL bRemove);

	// 원 찾기
	extern "C" IMGPROCLIBRARY_API  int CalcCirclePos(POINT point1, POINT point2, int nRadius, POINT * pntCenter, int nDir);
	extern "C" IMGPROCLIBRARY_API  int CalcCirclePosFrom3Point(POINT p1, POINT p2, POINT p3, int nDir, POINT * pntCenter, int* pnRadius);
	extern "C" IMGPROCLIBRARY_API   int Edge_Sobel(BYTE* pImgPtr, BYTE* pProcImg, int nWidth, int nHeight, RECT rcProc, int nMode);

	// Image Mean 처리 
	extern "C" IMGPROCLIBRARY_API int ImageMean(BYTE * pImgPtr, int nWidth, int nHeight, int x, int y);

	// 평균화 처리 
	extern "C" IMGPROCLIBRARY_API int MeanImageDirection_Round(BYTE * pImage, BYTE * pMeanImg, int nWidth, int nHeight, RECT rectProc, int nMeanSize, int nThresLevel);
	extern "C" IMGPROCLIBRARY_API int MeanImageDirection(BYTE * pImage, BYTE * pMeanImg, int nWidth, int nHeight, int nMeanSize, int nDir);
	extern "C" IMGPROCLIBRARY_API int MeanImageDirectionRect(BYTE * pImage, BYTE * pMeanImg, int nWidth, int nHeight, RECT rectProc, int nMeanSize, int nDir);

	// Image 중 일부 Mean 처리 Org => Tar
	extern "C" IMGPROCLIBRARY_API int ImageMean_Part(BYTE * pOrgPtr, BYTE * pTarPtr, int nWidth, int nHeight, RECT rcRange, int nMeanX, int nMeanY, int nMode);

	// Threshold 처리
	extern "C" IMGPROCLIBRARY_API int Threshold(BYTE * pImgPtr, CRegionInfo * pRoiInfo, int nWidth, int nHeight, int nThreshold, BOOL bClearRslt);

	// ROI 사용 
	extern "C" IMGPROCLIBRARY_API int Threshold_RoundMask(BYTE * pImgPtr, CRegionInfo * pRoiInfo, VEC_ROUND_INFO * vecLine, int nWidth, int nHeight, int nInspWidth, int nMin, int nMaskOffset, int nLimitRight, int nMode, BOOL bClearRslt, BOOL bModeSide = en_TopSide);
	extern "C" IMGPROCLIBRARY_API int Threshold_RoundMask_Negative(BYTE * pImgPtr, CRegionInfo * pRoiInfo, VEC_ROUND_INFO * vecLine, int nWidth, int nHeight, int nStartX, int nEndX, int nMin, int nLimitRight, BOOL bClearRslt, BOOL bModeSide);
	//extern "C" IMGPROCLIBRARY_API int Threshold_ByEdgeLine(BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nInspWidth, int nMinThres, int nMaxThres, int nMaskOffset, int nMode, BOOL bClearRslt, BOOL bModeSide = en_LeftSide);

}
