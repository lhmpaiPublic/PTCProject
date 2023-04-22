#include "pch.h"
#include "GlobalDef.h"
#include "CImageProcess.h"
#include "GlobalData.h" 
#include "CTabRsltInfo.h"
#include "CRecipeCtrl.h"
#include "TimeAnalyzer.h"
#include "BitmapStd.h"
#include "Win32File.h"

#if defined( USE_PROC_HALCON )
#include "HalconCpp.h"
using namespace HalconCpp;
#endif
CRegionInfo::CRegionInfo()
{
	m_bUseFifoMode = FALSE;
	m_nLeft = 0;
	m_nRight = 0;
	m_nTop = 0;
	m_nBottom = 0;
	pRoiPtr = NULL;
	pVecFifoPtr = new VEC_FIFO;
	pVecFifoPtr->clear();

}

CRegionInfo::~CRegionInfo()
{
	//if( pRoiPtr != NULL ){
	//	delete []pRoiPtr;
	//	pRoiPtr = NULL;
	//}
	if (pVecFifoPtr != NULL) {
		pVecFifoPtr->clear();
		delete pVecFifoPtr;
		pVecFifoPtr = NULL;
	}
};

CRegionInfo::VEC_FIFO* CRegionInfo::GetFifoPtr()
{
	return pVecFifoPtr;
}

void CRegionInfo::SetRect(CRect rect) {
	m_nLeft = rect.left;
	m_nRight = rect.right;
	m_nTop = rect.top;
	m_nBottom = rect.bottom;
}

// Round 부분의 평균화 처리는 내부 외부 별도로 처리.
// 좌우 휘도 차이가 있는 부분은 중심부와 Threslevel과의 관계로 상/하 평균값을 적용
int CImageProcess::MeanImageDirection_Round(BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc, int nMeanSize, int nThresLevel)
{
	int nRet = 0;
	ASSERT(pImage);
	ASSERT(pMeanImg);

	if (nMeanSize < 5) {
		return -1;
	}

	if ((nMeanSize % 2) == 0) {
		nMeanSize += 1;
	}	

	int  nMeanSizeHalf = nMeanSize / 2;
	int nStartX = rectProc.left ;
	int nEndX = rectProc.right ;
	int nStartY = rectProc.top - 50 - nMeanSizeHalf + 1;
	int nEndY = rectProc.bottom + 50 +  nMeanSizeHalf;

	if (nStartY <= 0) {
		return -1;
	}
	if (nEndY > nHeight) {
		return -2;
	}
	if (nStartX < 0) {
		nStartX = 0;
	}

	if (nWidth < nEndX) {
		nEndX = nWidth ;
	}

	int x, y;

	int* pnTopSum = new int[nWidth];
	int* pnBtmSum = new int[nWidth];
	memset(pnTopSum, 0, sizeof(int) * nWidth);
	memset(pnBtmSum, 0, sizeof(int) * nWidth);
	int nVerSumTop;
	int nVerSumBtm;
	int nMeanEndY = nStartY + nMeanSizeHalf ;
	for (x = nStartX; x < nEndX; x++) {
		nVerSumTop = 0;
		nVerSumBtm = 0;
		for (y = nStartY; y < nMeanEndY; y++) {
			nVerSumTop += *(pImage + (nWidth * y) + x);
			nVerSumBtm += *(pImage + (nWidth * ( y + nMeanSizeHalf - 1 )) + x);
		}
		pnTopSum[x] += nVerSumTop;
		pnBtmSum[x] += nVerSumBtm;
	}

	int nTopAve, nBtmAve;
	int nCntValue;
	int nLowOne;
	int nHighOne;

//	nStartY = rectProc.top;
//	nEndY = rectProc.bottom;

	for (y = nStartY; y < nEndY; y++) {
		for (x = nStartX; x < nEndX; x++) {
			nCntValue = (int)*(pImage + (nWidth * y) + x) ;
			nTopAve = pnTopSum[x] / nMeanSizeHalf;
			nBtmAve = pnBtmSum[x] / nMeanSizeHalf;
			if (nCntValue > nThresLevel) {
				if (nTopAve > nBtmAve) {
					nHighOne = nTopAve;
				}
				else {
					nHighOne = nBtmAve;
				}
				if (nHighOne > 255) {
					nHighOne = 255;
				}
				else if (nHighOne < 0) {
					nHighOne = 0;
				}
				*(pMeanImg + (nWidth * y) + x) = (BYTE)nHighOne;
			}
			else {
				if (nTopAve < nBtmAve) {
					nLowOne = nTopAve;
				}
				else {
					nLowOne = nBtmAve;
				}
				if (nLowOne > 255) {
					nLowOne = 255;
				}
				else if (nLowOne < 0) {
					nLowOne = 0;
				}
				*(pMeanImg + (nWidth * y) + x) = (BYTE)nLowOne;
			}

			pnTopSum[x] = pnTopSum[x] - *(pImage + (nWidth * (y - (nMeanSizeHalf - 1 ))) + x) + *(pImage + (nWidth * ( y + 1) )+ x);
			pnBtmSum[x] = pnBtmSum[x] - nCntValue + *(pImage + (nWidth * (y + nMeanSizeHalf )) + x);

		}
	}

	delete[] pnTopSum;
	pnTopSum = NULL;
	delete[] pnBtmSum;
	pnBtmSum = NULL;

	return nRet; 
}

int CImageProcess::MeanImageDirection(BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc, int nMeanSize, int nDir)
{
	int nRet = 0;
	ASSERT(pImage);
	ASSERT(pMeanImg);


	if (nMeanSize < 5) {
		return -1;
	}

	if ((nMeanSize % 2) == 0) {
		nMeanSize += 1;
	}

	int nStartX = rectProc.left;
	int nStartY = rectProc.top;
	int nEndX = rectProc.right;
	int nEndY = rectProc.bottom;

	if (nStartX < 0)nStartX = 0;
	if (nStartY < 0)nStartY = 0;
	if (nEndX > nWidth) nEndX = nWidth-1;
	if (nEndY > nHeight) nEndY = nHeight-1;


	int nMeanWidth = nEndX - nStartX;
	int nMeanHeight = nEndY - nStartY;

	int nMeanSizeHalf = nMeanSize / 2;

	int nSum, nAverage;
	int x, y;
	BYTE* pLinePtr;
	BYTE* pTarLinePtr;


	if (nDir == DIR_HOR) {
		for (y = nStartY; y < nEndY; y++) {
			nSum = 0;
			pLinePtr = pImage + (nWidth * y);
			pTarLinePtr = pMeanImg + (nWidth * y);

			for (x = nStartX; x < nStartX + nMeanSize; x++) {
				nSum += *(pLinePtr + x);
			}
			nAverage = nSum / nMeanSize;
			for (x = nStartX; x < nStartX + nMeanSizeHalf + 1; x++) {
				*(pTarLinePtr + x) = nAverage;
			}

			for (x = nStartX + nMeanSizeHalf + 1; x < nEndX - nMeanSizeHalf; x++) {
				nSum -= *(pLinePtr + x - nMeanSizeHalf);
				nSum += *(pLinePtr + x + nMeanSizeHalf + 1);
				*(pTarLinePtr + x) = nSum / nMeanSize;
			}

			nAverage = nSum / nMeanSize;
			int nStPosX = nStartX + nMeanWidth; ;
			for (x = nStPosX; x > nStPosX - nMeanSizeHalf; x--) {
				*(pTarLinePtr + x) = nAverage;
			}
		}
	}
	else { // DIR_VER
		//int nDistance = nWidth * nMeanSize;
		if (nWidth < nEndX) {
			return -1;
		}
		int* pnLineSum = new int[nWidth];
		memset(pnLineSum, 0, sizeof(int) * nWidth);
		int nVerSum;
		for (x = nStartX; x < nEndX; x++) {
			nVerSum = 0;
			for (y = nStartY; y < nMeanSize; y++) {
				nVerSum += *(pImage + (nWidth * y) + x);
			}
			pnLineSum[x] += nVerSum;
		}

		for (y = nStartY; y < nMeanSizeHalf; y++) {
			for (x = nStartX; x < nEndX; x++) {
				*(pMeanImg + (nWidth * y) + x) = pnLineSum[x] / nMeanSize;
			}
		}

		for (y = nStartY + nMeanSizeHalf; y < nEndY - nMeanSizeHalf; y++) {
			for (x = nStartX; x < nEndX; x++) {
				*(pMeanImg + (nWidth * y) + x) = pnLineSum[x] / nMeanSize;
				pnLineSum[x] = pnLineSum[x] - *(pImage + (nWidth * (y - nMeanSizeHalf)) + x) + *(pImage + (nWidth * (y + nMeanSizeHalf)) + x);
			}
		}

		for (y = nEndY - nMeanSizeHalf; y < nEndY; y++) {
			for (x = nStartX; x < nEndX; x++) {
				*(pMeanImg + (nWidth * y) + x) = pnLineSum[x] / nMeanSize;
			}
		}
		delete[]pnLineSum;
	}

	return nRet;
}

int CImageProcess::MeanImageDirection(BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, int nMeanSize, int nDir)
{
	int nRet = 0;
	CRect rectProc;
	rectProc.left = 0;
	rectProc.top = 0;
	rectProc.right = nWidth;
	rectProc.bottom = nHeight;

	nRet = MeanImageDirection(pImage, pMeanImg, nWidth, nHeight, rectProc, nMeanSize, nDir);

	return nRet;
}


BOOL CImageProcess::ImageMean(BYTE* pOrgPtr, BYTE* pTarPtr, int nWidth, int nHeight, int nMeanX, int nMeanY)
{
	ASSERT(pOrgPtr);
	ASSERT(pTarPtr);

	int x, y;

	int nTotalW = nMeanX;
	int nTotalH = nMeanY;
	if ((nMeanX % 2) == 0) {
		nTotalW = nMeanX + 1;
	}
	if ((nMeanY % 2) == 0) {
		nTotalH = nMeanY + 1;
	}
	int nSqrTotal = nTotalH * nTotalW;
	int nHalfH = nTotalH / 2;
	int nHalfW = nTotalW / 2;

	//	BYTE* pLinePtr;
	BYTE* pTarLinePtr;

	long nVerSum = 0;
	long nTopSum = 0;
	long nRightSum = 0;
	long nTotalSum = 0;

	long* vecLineSum;
	vecLineSum = new long[nWidth];
	memset(vecLineSum, 0, sizeof(long) * nWidth);

	for (x = 0; x < nWidth; x++) {
		nVerSum = 0;
		for (y = 0; y < nTotalH; y++) {
			nVerSum += *(pOrgPtr + (nWidth * y) + x);
		}
		vecLineSum[x] += nVerSum;
	}

	nVerSum = 0;
	nRightSum = 0;
	long nTop, nBtm;
	for (x = 0; x < nTotalW; x++) {
		nTotalSum += vecLineSum[x];
	}
	for (y = nHalfH; y < (nHeight - nHalfH); y++) {
		pTarLinePtr = (pTarPtr + (nWidth * y));

		for (x = nHalfW; x < (nWidth - nHalfW); x++) {
			*(pTarLinePtr + x) = (BYTE)(nTotalSum / nSqrTotal);
			nVerSum = vecLineSum[x - nHalfW];
			nRightSum = vecLineSum[x + nHalfW + 1];
			nTotalSum = nTotalSum - nVerSum + nRightSum;
		}

		nTotalSum = 0;
		nTop = nWidth * (y - nHalfH);
		nBtm = nWidth * (y + (nHalfH + 1));
		for (x = 0; x < nTotalW; x++) {
			vecLineSum[x] = vecLineSum[x] - *(pOrgPtr + nTop + x) + *(pOrgPtr + nBtm + x);
			nTotalSum += vecLineSum[x];
		}

		for (x = nTotalW; x < nWidth; x++) {
			vecLineSum[x] = vecLineSum[x] - *(pOrgPtr + nTop + x) + *(pOrgPtr + nBtm + x);
		}
	}

	delete[]vecLineSum;

	return TRUE;
}

int CImageProcess::ImageMean_Part(BYTE* pOrgPtr, BYTE* pTarPtr, int nWidth, int nHeight, CRect rcRange, int nMeanX, int nMeanY, int nMode )
{
	ASSERT(pOrgPtr);
	ASSERT(pTarPtr);

	int x, y;

	int nTotalW = nMeanX;
	int nTotalH = nMeanY;
	if ((nMeanX % 2) == 0) {
		nTotalW = nMeanX + 1;
	}
	if ((nMeanY % 2) == 0) {
		nTotalH = nMeanY + 1;
	}
	int nSqrTotal = nTotalH * nTotalW;
	int nHalfH = nTotalH / 2;
	int nHalfW = nTotalW / 2;
	rcRange.InflateRect(nHalfH, nHalfW);

	int nStartX = rcRange.left;
	int nEndX = rcRange.right;
	int nStartY = rcRange.top;
	int nEndY = rcRange.bottom;

	if (nStartX < 0) nStartX = 0;
	if (nEndX > nWidth) nEndX = nWidth;
	if (nStartY < 0) nStartY = 0;
	// 23.02.14 Ahn Modify Start
	//if (nEndY > nHeight) nEndY = nHeight ;
	if (nEndY > nHeight) nEndY = nHeight - 1;
	// 23.02.14 Ahn Modify End

	//	BYTE* pLinePtr;
	BYTE* pTarLinePtr;

	long nVerSum = 0;
	long nTopSum = 0;
	long nRightSum = 0;
	long nTotalSum = 0;

	long* vecLineSum;
	vecLineSum = new long[nWidth];
	memset(vecLineSum, 0, sizeof(long) * nWidth);

	for (x = nStartX; x < nEndX; x++) {
		nVerSum = 0;
		for (y = nStartY; y < nStartY + nTotalH; y++) {
			nVerSum += *(pOrgPtr + (nWidth * y) + x);
		}
		vecLineSum[x] += nVerSum;
	}

	nVerSum = 0;
	nRightSum = 0;
	long nTop, nBtm;
	for (x = nStartX; x < nStartX + nTotalW; x++) {
		nTotalSum += vecLineSum[x];
	}
	// 23.02.14 Ahn Modify End
	//for (y = nStartY + nHalfH; y < (nEndY - nHalfH - 1); y++) {
	for (y = nStartY + nHalfH; y < (nEndY - nHalfH ); y++) {
	// 23.02.14 Ahn Modify End
		pTarLinePtr = (pTarPtr + (nWidth * y));

		for (x = nStartX + nHalfW; x < (nEndX - nHalfW); x++) {
			*(pTarLinePtr + x) = (BYTE)(nTotalSum / nSqrTotal);
			nVerSum = vecLineSum[x - nHalfW];
			nRightSum = vecLineSum[x + nHalfW + 1];
			nTotalSum = nTotalSum - nVerSum + nRightSum;
		}

		nTotalSum = 0;
		nTop = nWidth * (y - nHalfH);
		nBtm = nWidth * (y + (nHalfH + 1) );
		for (x = nStartX; x < nStartX + nTotalW; x++) {
			vecLineSum[x] = vecLineSum[x] - *(pOrgPtr + nTop + x) + *(pOrgPtr + nBtm + x);
			nTotalSum += vecLineSum[x];
		}

		for (x = nStartX + nTotalW; x < nEndX; x++) {
			vecLineSum[x] = vecLineSum[x] - *(pOrgPtr + nTop + x) + *(pOrgPtr + nBtm + x);
		}
	}

	// 0 ~ HalfSize까지는 HalfSize + 1의 값으로 모두 Copy
	// (nHeight - HalfSize) ~ nHeight까지는 (nHeight - HalfSize - 1)1의 값으로 모두 Copy
	BYTE* pSrcPtr = pTarPtr + ( nWidth  * ( nStartY + nHalfH + 1 ) ) + nStartX ;

	if (nMode & en_FillTop) {
		for (y = nStartY; y < nHalfH; y++) {
			pTarLinePtr = pTarPtr + (nWidth * y) + nStartX;
			memcpy(pTarLinePtr, pSrcPtr, sizeof(BYTE) * rcRange.Width());
		}
	}
	pSrcPtr = pTarPtr + (nWidth * (nEndY - nHalfH - 2)) + nStartX;

	if (nMode & en_FillBottom) {
		// 22.02.15 Ahn Modify Start
		//for (y = (nEndY - nHalfH - 1); y < nEndY; y++) {
		for (y = (nEndY - nHalfH ); y <= nEndY; y++) {
		// 22.02.15 Ahn Modify Start
			pTarLinePtr = pTarPtr + (nWidth * y) + nStartX;
			memcpy(pTarLinePtr, pSrcPtr, sizeof(BYTE) * rcRange.Width());
		}
	}

	delete[]vecLineSum;

	return TRUE;
}


int CImageProcess::GetProjection(BYTE* pImage, int* pProjection, int nWidth, int nHeight, int nStartX, int nEndX, int nStartY, int nEndY, int nDir, int nSampling, BOOL bModeSum)
{
	ASSERT(pImage);
	ASSERT(pProjection);
	int x, y;
	int nDestWidth = nEndX - nStartX;
	int nDestHeight = nEndY - nStartY;

	int nTemp;
	if (nEndY > nHeight) {
		nTemp = nEndY - nHeight;
		nEndY = nHeight;
		nStartY -= nTemp;
	}

	BYTE* pLine;
	if (nDir == DIR_HOR) {

		int nCount = ((nEndX - nStartX) / nSampling);
		memset(pProjection, 0x00, sizeof(int) * (nDestHeight));
		for (y = nStartY; y < nEndY; y++) {
			pLine = pImage + (nWidth * y);
			for (x = nStartX; x < nEndX; x += nSampling) {
				pProjection[y - nStartY] += *(pLine + x);
			}
		}
		if (bModeSum == FALSE) {
			for (y = nStartY; y < nEndY; y++) {
				pProjection[y - nStartY] = pProjection[y - nStartY] / nCount;
			}
		}
	}
	else {
		memset(pProjection, 0x00, sizeof(int) * (nDestWidth));
		int nCount = ((nEndY - nStartY) / nSampling);
		if (nCount <= 0) nCount = 1; // // 22.07.01 Ahn Add

		for (y = nStartY; y < nEndY; y += nSampling) {
			pLine = pImage + (nWidth * y);
			for (x = nStartX; x < nEndX; x++) {
				pProjection[x - nStartX] += *(pLine + x);
			}
		}
		if (bModeSum == FALSE) {
			for (x = nStartX; x < nEndX; x++) {
				pProjection[x - nStartX] = pProjection[x - nStartX] / nCount;
			}
		}
	}
	return 0;
}
//
//int CImageProcess::GetAverageFromPrjData(int* pPrjData, int* pAveData, int nLength, int nAveRage)
//{
//	int i;
//	int nAverage = 0;
//	int nSum = 0;
//
//	ASSERT(pPrjData);
//	ASSERT(pAveData);
//
//	for (i = 0; i < nAveRage; i++) {
//		nSum += pPrjData[i];
//	}
//
//	int nHalfRange = nAveRage / 2;
//	nAverage = (int)(nSum / nAveRage);
//	for (i = 0; i <= nHalfRange; i++) {
//		pAveData[i] = nAverage;
//	}
//
//	for (i = nHalfRange; i < nLength - nHalfRange; i++) {
//		pAveData[i] = (int)(nSum / nAveRage);
//		nSum -= pPrjData[i - nHalfRange];
//		nSum += pPrjData[i + nHalfRange + 1];
//	}
//
//	for (i = nLength - nHalfRange; i < nLength; i++) {
//		pAveData[i] = nAverage;
//	}
//	return 0;
//}


int CImageProcess::GetProjection(BYTE* pImage, int* pProjection, int nWidth, int nHeight, CRect rectPrj, int nDir, int nSampling, BOOL bModeSum)
{
	ASSERT(pImage);
	ASSERT(pProjection);
	int x, y;
	int nStartX = rectPrj.left;
	int nEndX = rectPrj.right;
	int nStartY = rectPrj.top;
	int nEndY = rectPrj.bottom;

	int nDestWidth = nEndX - nStartX;
	int nDestHeight = nEndY - nStartY;

	int nTemp;
	if (nEndY > nHeight)
	{
		nTemp = nEndY - nHeight;
		nEndY = nHeight;
		nStartY -= nTemp;
	}

	if (nStartY < 0)
	{
		nStartY = 0;
	}

	if (nStartX < 0)
	{
		nStartX = 0;
	}
	
	if (nEndX > nWidth)
	{
		nEndX = nWidth;
	}

	BYTE* pLine;
	int nCount = 0 ; // 22.06.24 Ahn Add 
	if (nDir == DIR_HOR)
	{
		nCount = ((nEndX - nStartX) / nSampling);

		if (nCount <= 0)
			nCount = 1;

		memset(pProjection, 0x00, sizeof(int) * (nDestHeight));
		for (y = nStartY; y < nEndY; y++)
		{
			pLine = pImage + (nWidth * y);
			for (x = nStartX; x < nEndX; x += nSampling)
			{
				pProjection[y - nStartY] += *(pLine + x);
			}
		}
		if (bModeSum == FALSE)
		{
			for (y = nStartY; y < nEndY; y++)
			{
				pProjection[y - nStartY] = pProjection[y - nStartY] / nCount;
			}
		}
	}
	else
	{
		memset(pProjection, 0x00, sizeof(int) * (nDestWidth));
		nCount = ((nEndY - nStartY) / nSampling);
		if (nCount <= 0) nCount = 1;

		for (y = nStartY; y < nEndY; y += nSampling)
		{
			pLine = pImage + (nWidth * y);
			for (x = nStartX; x < nEndX; x++)
			{
				pProjection[x - nStartX] += *(pLine + x);
			}
		}
		if (bModeSum == FALSE)
		{
			for (x = nStartX; x < nEndX; x++)
			{
				pProjection[x - nStartX] = pProjection[x - nStartX] / nCount;
			}
		}
	}
	return nCount;  // 22.06.24 Ahn Modify
}

int CImageProcess::Threshold(BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nMin, int nMax)
{
	ASSERT(pImgPtr);
	ASSERT(pRsltPtr);
	int x, y;

	if ((pImgPtr == NULL) || (pRsltPtr == NULL)) {
		AfxMessageBox(_T("Point is NULL"));
		return -1;
	}
	BYTE* pLinePtr;
	BYTE* pTarLinPtr;
	BYTE btOrg;
	DWORD dwCount = 0;
	memset(pRsltPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	// 22.07.07 Ahn Add Start
	if (nMin <= THRESHOLD_MIN_VALUE) {
		nMin = THRESHOLD_MIN_VALUE;
	}
	// 22.07.07 Ahn Add End
	for (y = 0; y < nHeight; y++) {
		pLinePtr = pImgPtr + (nWidth * y);
		pTarLinPtr = pRsltPtr + (nWidth * y);
		// 22.07.07 Ahn Add Start
		if (dwCount > MAX_BLOCK_PIXEL) {
			return OVERFLOW_OCCURED ;
		}
		// 22.07.07 Ahn Add End
		for (x = 0; x < nWidth; x++) {
			btOrg = *(pLinePtr + x);
			if ((btOrg >= (BYTE)nMin) && (btOrg <= nMax)) {
				*(pTarLinPtr + x) = en_Black_Bit;
				dwCount++;
			}
		}
	}

	return dwCount;
}
//
//int CImageProcess::Threshold_WhiteAndBlack(BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nBlack, int nWhite)
//{
//	ASSERT(pImgPtr);
//	ASSERT(pRsltPtr);
//
//	int x, y;
//	BYTE* pLinePtr;
//	BYTE* pTarLinPtr;
//	BYTE btOrg;
//	DWORD dwBlackCnt = 0;
//	DWORD dwWhiteCnt = 0;
//	memset(pRsltPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
//
//	for (y = 0; y < nHeight; y++) {
//		pLinePtr = pImgPtr + (nWidth * y);
//		pTarLinPtr = pRsltPtr + (nWidth * y);
//		for (x = 0; x < nWidth; x++) {
//			btOrg = *(pLinePtr + x);
//			// Black
//			if ((btOrg >= (BYTE)0) && (btOrg <= (BYTE)nBlack)) {
//				*(pTarLinPtr + x) |= en_Black_Bit;
//				dwBlackCnt++;
//			}
//			else if ((btOrg >= (BYTE)nWhite) && (btOrg <= 255)) {
//				*(pTarLinPtr + x) |= en_White_Bit;
//				dwWhiteCnt++;
//			}
//		}
//	}
//
//	return (dwBlackCnt + dwWhiteCnt);
//}
// 22.07.14 Ahn Modify Start
//int CImageProcess::Threshold(BYTE* pImgPtr, CRegionInfo* pRoiInfo, int nWidth, int nHeight, int nMin, BOOL bClearRslt)
int CImageProcess::Threshold(BYTE* pImgPtr, CRegionInfo* pRoiInfo, int nWidth, int nHeight, int nThreshold, BOOL bClearRslt, BOOL bModeDark /*= FALSE*/ )
// 22.07.14 Ahn Modify End
{
	ASSERT(pImgPtr);
	ASSERT(pRoiInfo);

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;
	nStartX = pRoiInfo->m_nLeft;
	nStartY = pRoiInfo->m_nTop;
	nEndX = pRoiInfo->m_nRight;
	nEndY = pRoiInfo->m_nBottom;

	BYTE bBit = pRoiInfo->m_bProcPit;

	if (nStartX < 0) { nStartX = 0; }
	if (nStartY < 0) { nStartY = 0; }
	if (nEndX > nWidth) { nEndX = nWidth; }
	if (nEndY > nHeight) { nEndY = nHeight; }

	BYTE* pRoiPtr = pRoiInfo->GetRoiPtr();
	ASSERT(pRoiPtr);
	BYTE* pRoiLinePtr;
	BYTE* pLinePtr;
	BYTE btOrg;
	DWORD dwCount = 0;
	if (bClearRslt == TRUE) {
		memset(pRoiPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	}

	if (bModeDark == FALSE) {
		// 22.07.07 Ahn Add Start
		if (nThreshold <= THRESHOLD_MIN_VALUE) {
			nThreshold = THRESHOLD_MIN_VALUE;
			//return 1; 
		}
		// 22.07.07 Ahn Add End

		for (y = nStartY; y < nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * y);
			pRoiLinePtr = pRoiPtr + (nWidth * y);
			// 22.07.07 Ahn Add Start
			if (dwCount > MAX_BLOCK_PIXEL) {
				return OVERFLOW_OCCURED;
			}
			// 22.07.07 Ahn Add End
			for (x = nStartX; x < nEndX; x++) {
				btOrg = *(pLinePtr + x);
				if (btOrg >= (BYTE)nThreshold) {
					*(pRoiLinePtr + x) |= bBit;
					dwCount++;
					CPixel pnt;
					pnt.x = x;
					pnt.y = y;
					pnt.btBright = btOrg;
					pnt.btOrgBri = 0;
					pRoiInfo->GetFifoPtr()->push_back(pnt);
				}
			}
		}
	}
	else {
		for (y = nStartY; y < nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * y);
			pRoiLinePtr = pRoiPtr + (nWidth * y);
			if (dwCount > MAX_BLOCK_PIXEL) {
				return OVERFLOW_OCCURED;
			}

			for (x = nStartX; x < nEndX; x++) {
				btOrg = *(pLinePtr + x);
				if (btOrg <= (BYTE)nThreshold) {
					*(pRoiLinePtr + x) |= bBit;
					dwCount++;
					CPixel pnt;
					pnt.x = x;
					pnt.y = y;
					pnt.btBright = btOrg;
					pnt.btOrgBri = 0;
					pRoiInfo->GetFifoPtr()->push_back(pnt);
				}
			}
		}
	}



	return 0;
}

int CImageProcess::Threshold_RoundMask(BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nInspWidth, int nMin, int nMaskOffset, int nLimitRight, int nMode, BOOL bClearRslt, BOOL bModeSide /*=en_HeadSide*/)
{
	ASSERT(pImgPtr);
	ASSERT(pRoiInfo);

	BYTE* pRoiPtr = pRoiInfo->GetRoiPtr();

	ASSERT(pRoiPtr);
	ASSERT(vecLine);

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;
	nStartX = pRoiInfo->m_nLeft;
	nStartY = pRoiInfo->m_nTop;
	nEndX = pRoiInfo->m_nRight;
	nEndY = pRoiInfo->m_nBottom;

	if (vecLine->size() <= 0) return -1;

	ASSERT(pRoiPtr);
	BYTE* pRoiLinePtr;
	BYTE* pLinePtr;
	BYTE btOrg;
	DWORD dwCount = 0;
	if (bClearRslt == TRUE) {
		memset(pRoiPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	}
	BOOL bOverflow = FALSE;
	int nBnd = 0;
	int mode;
	if( bModeSide == en_BottomSide){
		if (nMode == en_ModeFoilExp) {
			mode = en_ModeFoilExpOut;
		} else {
			mode = en_ModeFoilExp;
		}
	}
	else {
		mode = nMode ;
	}
	int nSize = (int) vecLine->size();
	if ( nSize < (nEndY - nStartY)) {
	//	ASSERT(FALSE);
		return -1;
	}
	if ( mode == en_ModeFoilExp) { // In
		for (y = nStartY, nBnd = 0; y < nEndY; y++, nBnd++) {
			pLinePtr = pImgPtr + (nWidth * y);
			pRoiLinePtr = pRoiPtr + (nWidth * y);
			nEndX = (*vecLine)[nBnd].x - nMaskOffset ;
			int nInspStartX = nEndX - nInspWidth;
			if (nInspStartX < 0) nInspStartX = 0;

			if (MAX_BLOCK_PIXEL < dwCount) {
				// Overflow 
				bOverflow = TRUE;
				break;
			}

			for (x = nInspStartX; x < nEndX; x++) {
				btOrg = *(pLinePtr + x);
				if (btOrg >= (BYTE)nMin){
					*(pRoiLinePtr + x) |= en_Black_Bit;
					dwCount++;
					CPixel pnt;
					pnt.x = x;
					pnt.y = y;
					pnt.btBright = btOrg;
					pnt.btOrgBri = 0;
					pRoiInfo->GetFifoPtr()->push_back(pnt);
				}
			}
		}
	} else if( mode == en_ModeFoilExpOut ){ // Out 
		for (y = nStartY, nBnd = 0; y < nEndY; y++, nBnd++) {
			if (MAX_BLOCK_PIXEL < dwCount) {
				// Overflow 
				bOverflow = TRUE;
				break;
			}

			pLinePtr = pImgPtr + (nWidth * y);
			pRoiLinePtr = pRoiPtr + (nWidth * y);
			nStartX = (*vecLine)[nBnd].x + nMaskOffset;
			nEndX = nStartX + nInspWidth;
			if (bModeSide == en_HeadSide){ 
				if (nEndX > nLimitRight) {
					nEndX = nLimitRight;
				}
			}
			for (x = nStartX; x < nEndX; x++) {
				if (pRoiInfo->m_nRight < x) break;
				btOrg = *(pLinePtr + x);
				if (btOrg >= (BYTE)nMin) {
					*(pRoiLinePtr + x) |= en_Black_Bit;
					dwCount++;
					CPixel pnt;
					pnt.x = x;
					pnt.y = y;
					pnt.btBright = (int)btOrg ;
					pnt.btOrgBri = 0;
					pRoiInfo->GetFifoPtr()->push_back(pnt);
				}
			}
		}
	}

	if (bOverflow == TRUE) {
		return 0x0008;
	}
	return 0;
}

// 22.01.17 Ahn Add Start
// 음극용 Threshold를 기준으로 Image와 Fifo를 작성.
int CImageProcess::Threshold_RoundMask_Negative(BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nStartOffset, int nEndOffset, int nMin, int nLimitRight, BOOL bClearRslt, BOOL bModeSide /* = CImageProcess::en_TopSide */ )
{
	ASSERT(pImgPtr);
	ASSERT(pRoiInfo);

	BYTE* pRoiPtr = pRoiInfo->GetRoiPtr();

	ASSERT(pRoiPtr);
	ASSERT(vecLine);

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;
	nStartX = pRoiInfo->m_nLeft;
	nStartY = pRoiInfo->m_nTop;
	nEndX = pRoiInfo->m_nRight;
	nEndY = pRoiInfo->m_nBottom;

	if (vecLine->size() <= 0) return -1;

	ASSERT(pRoiPtr);
	BYTE* pRoiLinePtr;
	BYTE* pLinePtr;
	BYTE btOrg;
	DWORD dwCount = 0;
	if (bClearRslt == TRUE) {
		memset(pRoiPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	}
	BOOL bOverflow = FALSE;
	int nBnd = 0;

	if (bModeSide == en_BottomSide) {
		SWAP(int, nStartOffset, nEndOffset);
	}

	int nSize = (int)vecLine->size();
	if (nSize < (nEndY - nStartY)) {
		//	ASSERT(FALSE);
		return -1;
	}
	for (y = nStartY, nBnd = 0; y < nEndY; y++, nBnd++) {
		pLinePtr = pImgPtr + (nWidth * y);
		pRoiLinePtr = pRoiPtr + (nWidth * y);
		int nInspStartX = (*vecLine)[nBnd].x - nStartOffset;
		nEndX = (*vecLine)[nBnd].x + nEndOffset;
		if (nInspStartX < 0) nInspStartX = 0;

		if (MAX_BLOCK_PIXEL < dwCount) {
			// Overflow 
			bOverflow = TRUE;
			break;
		}

		for (x = nInspStartX; x < nEndX; x++) {
			btOrg = *(pLinePtr + x);
			if (btOrg >= (BYTE)nMin) {
				*(pRoiLinePtr + x) |= en_Black_Bit;
				dwCount++;
				CPixel pnt;
				pnt.x = x;
				pnt.y = y;
				pnt.btBright = btOrg;
				pnt.btOrgBri = 0;
				pRoiInfo->GetFifoPtr()->push_back(pnt);
			}
		}
	}
	
	if (bOverflow == TRUE) {
		return 0x0008;
	}
	return 0;
}
// 22.01.17 Ahn Add End

//// 22.01.19 Ahn Add Start
//int CImageProcess::Threshold_Default(BYTE* pImgPtr, BYTE* pRsltPtr, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nStartOffset, int nEndOffset, int nThreshold, CRect rcRange, BOOL bClearRslt)
//{
//	int nRet = 0; 
//
//	ASSERT(pImgPtr);
//	ASSERT(pRsltPtr);
//
//	int x, y;
//	int nStartX, nStartY, nEndX, nEndY;
//	nStartX = rcRange.left;
//	nStartY = rcRange.top;
//	nEndX = rcRange.right;
//	nEndY = rcRange.bottom;
//
//	BYTE* pRsltLinePtr;
//	BYTE* pLinePtr;
//	BYTE btOrg;
//	DWORD dwCount = 0;
//	if (bClearRslt == TRUE) {
//		memset(pRsltPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
//	}
//	BOOL bOverflow = FALSE;
//	int nBnd = 0;
//
//
//	int nSize = (int)vecLine->size();
//	if (nSize < (nEndY - nStartY)) {
//		//	ASSERT(FALSE);
//		return -1;
//	}
//	for (y = nStartY, nBnd = 0; y < nEndY; y++, nBnd++) {
//		pLinePtr = pImgPtr + (nWidth * y);
//		pRsltLinePtr = pRsltPtr + (nWidth * y);
//		int nInspStartX = (*vecLine)[nBnd].x - nStartOffset;
//		nEndX = (*vecLine)[nBnd].x + nEndOffset;
//		if (nInspStartX < 0) nInspStartX = 0;
//
//		for (x = nInspStartX; x < nEndX; x++) {
//			btOrg = *(pLinePtr + x);
//			if (btOrg >= (BYTE)nThreshold) {
//				*(pRsltLinePtr + x) |= en_Black_Bit;
//			}
//		}
//	}
//
//	return nRet;
//}
//
//int CImageProcess::CollectFifo(BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nStartOffset, int nEndOffset, BOOL bClearRslt, int bModeSide )
//{
//	ASSERT(pImgPtr);
//	ASSERT(pRoiInfo);
//
//	BYTE* pRoiPtr = pRoiInfo->GetRoiPtr();
//
//	ASSERT(pRoiPtr);
//	ASSERT(vecLine);
//
//	int x, y;
//	int nStartX, nStartY, nEndX, nEndY;
//	nStartX = pRoiInfo->m_nLeft;
//	nStartY = pRoiInfo->m_nTop;
//	nEndX = pRoiInfo->m_nRight;
//	nEndY = pRoiInfo->m_nBottom;
//
//	if (vecLine->size() <= 0) return -1;
//
//	BYTE* pRoiLinePtr;
//	BYTE* pLinePtr;
//	BYTE btOrg;
//	BYTE btDet;
//	DWORD dwCount = 0;
//	if (bClearRslt == TRUE) {
//	//	memset(pRoiPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
//		pRoiInfo->GetFifoPtr()->clear();
//	}
//	BOOL bOverflow = FALSE;
//	int nBnd = 0;
//
//	if (bModeSide == en_BottomSide) {
//		SWAP(int, nStartOffset, nEndOffset);
//	}
//
//	int nSize = (int)vecLine->size();
//	if (nSize < (nEndY - nStartY)) {
//		//	ASSERT(FALSE);
//		return -1;
//	}
//	for (y = nStartY, nBnd = 0; y < nEndY; y++, nBnd++) {
//		pLinePtr = pImgPtr + (nWidth * y);
//		pRoiLinePtr = pRoiPtr + (nWidth * y);
//		int nInspStartX = (*vecLine)[nBnd].x - nStartOffset;
//		nEndX = (*vecLine)[nBnd].x + nEndOffset;
//		if (nInspStartX < 0) nInspStartX = 0;
//
//		if (MAX_BLOCK_PIXEL < dwCount) {
//			// Overflow 
//			bOverflow = TRUE;
//			break;
//		}
//
//		for (x = nInspStartX; x < nEndX; x++) {
//			btOrg = *(pLinePtr + x);
//			btDet = *(pRoiLinePtr + x);
//			//if (btOrg >= (BYTE)nMin) 
//			if( btDet != 0x00 ) {
//				*(pRoiLinePtr + x) |= en_Black_Bit;
//				dwCount++;
//				CPixel pnt;
//				pnt.x = x;
//				pnt.y = y;
//				pnt.btBright = btOrg;
//				pnt.btOrgBri = 0;
//				pRoiInfo->GetFifoPtr()->push_back(pnt);
//			}
//		}
//	}
//
//	if (bOverflow == TRUE) {
//		return 0x0008;
//	}
//	return 0;
//}
//
//// 22.01.19 Ahn Add End


//
//int CImageProcess::Threshold_WhiteAndBlack(BYTE* pImgPtr, CRegionInfo roiInfo, int nWidth, int nHeight, int nBlack, int nWhite, BOOL bClearRslt)
//{
//	ASSERT(pImgPtr);
//
//	int x, y;
//	int nStartX, nStartY, nEndX, nEndY;
//
//	nStartX = roiInfo.m_nLeft;
//	nStartY = roiInfo.m_nTop;
//	nEndX = roiInfo.m_nRight;
//	nEndY = roiInfo.m_nBottom;
//	if (nStartX < 0) { nStartX = 0; }
//	if (nStartY < 0) { nStartY = 0; }
//	if (nEndX > nWidth) { nEndX = nWidth; }
//	if (nEndY > nHeight) { nEndY = nHeight; }
//
//	BYTE* pRoiPtr = roiInfo.GetRoiPtr();
//	ASSERT(pRoiPtr);
//
//	BYTE* pRoiLinePtr;
//	BYTE* pLinePtr;
//	BYTE btOrg;
//	DWORD dwBlackCnt = 0;
//	DWORD dwWhiteCnt = 0;
//	if (bClearRslt == TRUE) {
//		memset(pRoiPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
//	}
//
//	for (y = nStartY; y < nEndY; y++) {
//		pLinePtr = pImgPtr + (nWidth * y);
//		pRoiLinePtr = pRoiPtr + (nWidth * y);
//		for (x = nStartX; x < nEndX; x++) {
//			btOrg = *(pLinePtr + x);
//			if (*(pRoiLinePtr + x) > 0) {
//				// Black
//				if ((btOrg > (BYTE)0) && (btOrg <= (BYTE)nBlack)) {
//					*(pRoiLinePtr + x) |= en_Black_Bit;
//					dwBlackCnt++;
//				} // White
//				else if ((btOrg > (BYTE)nWhite) && (btOrg <= 255)) {
//					*(pRoiLinePtr + x) |= en_White_Bit;
//					dwWhiteCnt++;
//				}
//			}
//		}
//	}
//	return (dwBlackCnt + dwWhiteCnt);
//}

int CImageProcess::RemoveNoise(BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nFilterNo, BYTE btBit)
{
	ASSERT(pImgPtr);
	ASSERT(pRsltPtr);

	BYTE btFilter[3][3];
	memset(btFilter, 0x00, sizeof(btFilter));
	int nTarValue = 0;
	switch (nFilterNo) {
	case	en_Filter_8:
		btFilter[0][0] = 0xff;	btFilter[0][1] = 0xff; btFilter[0][2] = 0xff;
		btFilter[1][0] = 0xff;	btFilter[1][1] = 0xff; btFilter[1][2] = 0xff;
		btFilter[2][0] = 0xff;	btFilter[2][1] = 0xff; btFilter[2][2] = 0xff;
		nTarValue = 2;
		break;
	case	en_Filter_4:
		btFilter[0][0] = 0x00;	btFilter[0][1] = 0xff; btFilter[0][2] = 0x00;
		btFilter[1][0] = 0xff;	btFilter[1][1] = 0xff; btFilter[1][2] = 0xff;
		btFilter[2][0] = 0x00;	btFilter[2][1] = 0xff; btFilter[2][2] = 0x00;
		nTarValue = 2;
		break;

	default:
		btFilter[0][0] = 0xff;	btFilter[0][1] = 0xff; btFilter[0][2] = 0xff;
		btFilter[1][0] = 0xff;	btFilter[1][1] = 0xff; btFilter[1][2] = 0xff;
		btFilter[2][0] = 0xff;	btFilter[2][1] = 0xff; btFilter[2][2] = 0xff;
		nTarValue = 9;
		break;

	}
	int x, y;
	int nStartX, nStartY, nEndX, nEndY;

	nStartX = 1;
	nStartY = 1;
	nEndX = nWidth - 1;
	nEndY = nHeight - 1;

	BYTE* pLinePtr, * pTopPtr, * pBtmPtr;
	BYTE* pRsltLinePtr;
	for (y = nStartY; y < nEndY; y++) {
		pRsltLinePtr = pRsltPtr + (nWidth * y);
		pTopPtr = pImgPtr + (nWidth * (y - 1));
		pLinePtr = pImgPtr + (nWidth * y);
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		for (x = nStartX; x < nEndX; x++) {
			BYTE btCC = *(pTopPtr + x);
			if ((btCC & btBit)) {
				int nCnt = 0;
				for (int i = -1; i < 2; i++) {
					if (*(pTopPtr + x + i) & btBit) {
						nCnt++;
					}
					if (*(pLinePtr + x + i) & btBit) {
						nCnt++;
					}
					if (*(pBtmPtr + x + i) & btBit) {
						nCnt++;
					}
				}
				if (nCnt >= nTarValue) {
					*(pRsltLinePtr + x) = 0xff;
				};
			}
		}
	}
	return 0;
}


int CImageProcess::RemoveNoise(BYTE* pImgPtr, int nWidth, int nHeight, int nFilterNo, CRect rcRange, BYTE btBit)
{
	ASSERT(pImgPtr);

	BYTE btFilter[3][3];
	memset(btFilter, 0x00, sizeof(btFilter));
	int nTarValue = 0;
	switch (nFilterNo) {
	case	en_Filter_8:
		btFilter[0][0] = 0xff;	btFilter[0][1] = 0xff; btFilter[0][2] = 0xff;
		btFilter[1][0] = 0xff;	btFilter[1][1] = 0xff; btFilter[1][2] = 0xff;
		btFilter[2][0] = 0xff;	btFilter[2][1] = 0xff; btFilter[2][2] = 0xff;
		nTarValue = 2;
		break;
	case	en_Filter_4:
		btFilter[0][0] = 0x00;	btFilter[0][1] = 0xff; btFilter[0][2] = 0x00;
		btFilter[1][0] = 0xff;	btFilter[1][1] = 0xff; btFilter[1][2] = 0xff;
		btFilter[2][0] = 0x00;	btFilter[2][1] = 0xff; btFilter[2][2] = 0x00;
		nTarValue = 2;
		break;

	default:
		btFilter[0][0] = 0xff;	btFilter[0][1] = 0xff; btFilter[0][2] = 0xff;
		btFilter[1][0] = 0xff;	btFilter[1][1] = 0xff; btFilter[1][2] = 0xff;
		btFilter[2][0] = 0xff;	btFilter[2][1] = 0xff; btFilter[2][2] = 0xff;
		nTarValue = 9;
		break;

	}
	int x, y;
	int nStartX, nStartY, nEndX, nEndY;

	nStartX = rcRange.left;
	nStartY = rcRange.top;
	nEndX = rcRange.right;
	nEndY = rcRange.bottom;

	if (nStartX < 1) nStartX = 1;
	if (nStartY < 1) nStartY = 1;
	if (nEndX >= nWidth) nEndX = nWidth - 1;
	if (nEndY >= nHeight) nEndY = nHeight - 1;

	BYTE* pLinePtr, * pTopPtr, * pBtmPtr;
	//BYTE* pRsltLinePtr;
	for (y = nStartY; y < nEndY; y++) {
		pTopPtr = pImgPtr + (nWidth * (y - 1));
		pLinePtr = pImgPtr + (nWidth * y);
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		for (x = nStartX; x < nEndX; x++) {
			BYTE btCC = *(pLinePtr + x);
			if ((btCC & btBit)) {
				int nCnt = 0;
				for (int i = -1; i < 2; i++) {
					if (*(pTopPtr + x + i) & btBit) {
						nCnt++;
					}
					if (*(pLinePtr + x + i) & btBit) {
						nCnt++;
					}
					if (*(pBtmPtr + x + i) & btBit) {
						nCnt++;
					}
				}
				if (nCnt < nTarValue) {
					*(pLinePtr + x) = 0x00;
				}
			}
		}
	}
	return 0;
}

// 22.01.19 Ahn Add Start
int CImageProcess::Erosion(BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nMode, CRect rcRange, BYTE btBit)
{
	if (pRsltPtr == NULL) return - 1;
	if (pImgPtr == NULL) return -2;

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;

	nStartX = rcRange.left;
	nStartY = rcRange.top;
	nEndX = rcRange.right;
	nEndY = rcRange.bottom;

	if (nStartX < 1) nStartX = 1;
	if (nStartY < 1) nStartY = 1;
	if (nEndX >= nWidth) nEndX = nWidth - 1;
	if (nEndY >= nHeight) nEndY = nHeight - 1;

	BYTE* pLinePtr, * pTopPtr, * pBtmPtr;
	BYTE* pRsltLinePtr;
	memset(pRsltPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	//BYTE* pRsltLinePtr;
	for (y = nStartY; y < nEndY; y++) {
		pTopPtr = pImgPtr + (nWidth * (y - 1));
		pLinePtr = pImgPtr + (nWidth * y);
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		pRsltLinePtr = pRsltPtr + (nWidth * y);
		for (x = nStartX; x < nEndX; x++) {
			BYTE btCC = *(pLinePtr + x);
			if ((btCC & btBit)) {
				BOOL bFind = FALSE;
				if (!(*(pTopPtr + x ) & btBit) ){
					*(pRsltLinePtr + x) = 0x00;
					continue;
				}
				if(!(*(pLinePtr + x - 1) & btBit) || !(*(pLinePtr + x + 1) & btBit) ) {
					*(pRsltLinePtr + x) = 0x00;
					continue;
				}
				if (!( *(pBtmPtr + x ) & btBit) ) {
					*(pRsltLinePtr + x) = 0x00;
					continue;
				}
				*(pRsltLinePtr + x) = btCC;
			}
		}
	}
	return 0;
}
int CImageProcess::Exptention(BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nMode, CRect rcRange, BYTE btBit)
{
	if (pRsltPtr == NULL) return -1;
	if (pImgPtr == NULL) return -2;

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;

	nStartX = rcRange.left;
	nStartY = rcRange.top;
	nEndX = rcRange.right;
	nEndY = rcRange.bottom;

	if (nStartX < 1) nStartX = 1;
	if (nStartY < 1) nStartY = 1;
	if (nEndX >= nWidth) nEndX = nWidth - 1;
	if (nEndY >= nHeight) nEndY = nHeight - 1;

	BYTE* pLinePtr, * pTopPtr, * pBtmPtr;
	BYTE* pRsltLinePtr;
	memcpy(pRsltPtr, pImgPtr, sizeof(BYTE) * nWidth * nHeight);
	//BYTE* pRsltLinePtr;
	for (y = nStartY; y < nEndY; y++) {
		pTopPtr = pImgPtr + (nWidth * (y - 1));
		pLinePtr = pImgPtr + (nWidth * y);
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		pRsltLinePtr = pRsltPtr + (nWidth * y);
		for (x = nStartX; x < nEndX; x++) {
			BYTE btCC = *(pLinePtr + x);
			if ((btCC == 0x00)) {
				BOOL bFind = FALSE;
				if ((*(pTopPtr + x ) & btBit)) {
					*(pRsltLinePtr + x) = *(pTopPtr + x );
					continue;
				}
				if ((*(pLinePtr + x - 1) & btBit) || (*(pLinePtr + x + 1) & btBit)) {
					*(pRsltLinePtr + x) = (*(pLinePtr + x - 1) | *(pLinePtr + x + 1) ) ;
					continue;
				}
				if ((*(pBtmPtr + x ) & btBit)) {
					*(pRsltLinePtr + x) = *(pBtmPtr + x );
					continue;
				}
			}
		}
	}
	return 0;
}
// 22.01.19 Ahn Add End



int CImageProcess::PreFilter2x2(BYTE* pImgPtr, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc)
{
	int nStartX = rectProc.left;
	int nStartY = rectProc.top;
	int nEndX = rectProc.right;
	int nEndY = rectProc.bottom;

	if ((rectProc.Height() <= 0) || (rectProc.Width() <= 0)) {
		return -1;
	}

	BYTE* pLinePtr, * pBtmPtr;
	BYTE* pRsltLinePtr;
	int x, y;

	int nSum;
	for (y = nStartY; y < nEndY - 1; y++) {
		pRsltLinePtr = pMeanImg + (nWidth * y);
		pLinePtr = pImgPtr + (nWidth * y);
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		for (x = nStartX; x < nEndX - 1; x++) {
			nSum = *(pLinePtr + x) + *(pLinePtr + x + 1) + *(pBtmPtr + x) + *(pBtmPtr + x + 1);
			*(pRsltLinePtr + x) = (int)(nSum / 4);
		}
	}
	return 0;
}

int CImageProcess::PreFilter(BYTE* pImgPtr, BYTE* pMeanPtr, int nWidth, int nHeight, CRect rcProc)
{
	int nStartX = rcProc.left;
	int nStartY = rcProc.top;
	int nEndX = rcProc.right;
	int nEndY = rcProc.bottom;

	if ((rcProc.Height() <= 0) || (rcProc.Width() <= 0)) {
		return -1;
	}

	BYTE* pLinePtr, *pTopPtr, *pBtmPtr;
	BYTE* pRsltLinePtr;
	int x, y;

	int nSum;
	for (y = nStartY+1; y < nEndY - 1; y++) {
		pRsltLinePtr = pMeanPtr + (nWidth * y);
		pLinePtr = pImgPtr + (nWidth * y);
		pTopPtr = pImgPtr + (nWidth * ( y - 1));
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		for (x = nStartX; x < nEndX - 1; x++) {
			nSum = *(pLinePtr + x) + *(pTopPtr + x ) + *(pBtmPtr + x ) ;
			*(pRsltLinePtr + x) = (int)(nSum / 3);
		}
	}
	return 0;
}

int CImageProcess::PreFilterCross(BYTE* pImgPtr, BYTE* pProcImgPtr, int nWidth, int nHeight, CRect rcProc)
{
	int nStartX = rcProc.left;
	int nStartY = rcProc.top;
	int nEndX = rcProc.right;
	int nEndY = rcProc.bottom;

	if ((rcProc.Height() <= 0) || (rcProc.Width() <= 0)) {
		return -1;
	}

	if (nStartX < 1) nStartX = 1;
	if (nStartY < 1) nStartY = 1;
	if (nEndX >= nWidth) nEndX = nWidth - 1;
	if (nEndY >= nHeight) nEndY = nHeight - 1;

	BYTE* pLinePtr, * pTopPtr, * pBtmPtr;
	BYTE* pRsltLinePtr;
	int x, y;

	int nSum;
	int nTopBtm;
	for (y = nStartY ; y < nEndY ; y++) {
		pRsltLinePtr = pProcImgPtr + (nWidth * y);
		pLinePtr = pImgPtr + (nWidth * y);
		pTopPtr = pImgPtr + (nWidth * (y - 1));
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		for (x = nStartX; x < nEndX ; x++) {
			nTopBtm = *(pTopPtr + x) + *(pBtmPtr + x);
			if (abs((nTopBtm/2 )- *(pLinePtr + x - 1)) < abs((nTopBtm / 2) - *(pLinePtr + x + 1))) {
				nSum = *(pLinePtr + x) + *(pLinePtr + x - 1) + nTopBtm;
			}
			else {
				nSum = *(pLinePtr + x) + *(pLinePtr + x + 1) + nTopBtm;
			}
			*(pRsltLinePtr + x) = (BYTE)(nSum / 4);
		}
	}
	return 0;
}

int CImageProcess::PreFilter1x2(BYTE* pImgPtr, BYTE* pProcImgPtr, int nWidth, int nHeight, CRect rcProc)
{
	int nStartX = rcProc.left;
	int nStartY = rcProc.top;
	int nEndX = rcProc.right;
	int nEndY = rcProc.bottom;

	if ((rcProc.Height() <= 0) || (rcProc.Width() <= 0)) {
		return -1;
	}

	if (nStartX < 1) nStartX = 1;
	if (nStartY < 1) nStartY = 1;
	if (nEndX >= nWidth) nEndX = nWidth - 1;
	if (nEndY >= nHeight) nEndY = nHeight - 1;

	BYTE* pLinePtr, * pTopPtr, * pBtmPtr;
	BYTE* pRsltLinePtr;
	int x, y;

	int nSum;
	for (y = nStartY; y < nEndY; y++) {
		pRsltLinePtr = pProcImgPtr + (nWidth * y);
		pLinePtr = pImgPtr + (nWidth * y);
		pTopPtr = pImgPtr + (nWidth * (y - 1));
		pBtmPtr = pImgPtr + (nWidth * (y + 1));
		for (x = nStartX; x < nEndX; x++) {
			nSum = *(pTopPtr + x) + *(pBtmPtr + x) + *(pLinePtr+x) ;
			*(pRsltLinePtr + x) = (BYTE)(nSum / 3);
		}
	}
	return 0;

}

BOOL CImageProcess::GetOrgImageBright(BYTE* pImgPtr, int nWidth, int nHeight, CRegionInfo::VEC_FIFO* pVecFifo)
{
	ASSERT(pVecFifo);
	ASSERT(pImgPtr);

	//int nImgMaxSize = nWidth * nHeight;

	int nSize = (int)pVecFifo->size();
	for (int i = 0; i < nSize;i++) {
		CPixel* pPix = &(*pVecFifo)[i] ;
		pPix->btOrgBri = *( pImgPtr + ( nWidth * pPix->y ) + pPix->x ) ;
	}

	return 0;
}


BOOL CImageProcess::IsLinked(_LINE_DATA* pCurData, _LINE_DATA* pPreData)
{
	if ((pCurData->start) > (pPreData->start + pPreData->count) || (pCurData->count + pCurData->start) < pPreData->start) {
		return FALSE;
	}
	return TRUE;
}

int CImageProcess::GetOneLineFromFifo(VEC_LINE_DATA* pOneLine, CRegionInfo::VEC_FIFO* pVecFifo, int nStartPos)
{
	int nSize = (int)pVecFifo->size();

	if (nSize < nStartPos) return 0;

	_LINE_DATA data;
	memset(&data, 0x00, sizeof(_LINE_DATA));
	int idx = nStartPos;

	CPixel* pPrePnt = &(*pVecFifo)[idx++];
	CPixel* pCrntPnt = NULL;
	data.start = pPrePnt->x;
	data.ypos = pPrePnt->y;
	data.count = 1;
	data.nBriSum = pPrePnt->btBright;
	data.btBriMax = pPrePnt->btBright;
	data.btBriMin = pPrePnt->btBright;
	data.nOrgBriSum = pPrePnt->btOrgBri;
	data.btOrgBriMax = pPrePnt->btOrgBri;
	data.btOrgBriMin = pPrePnt->btOrgBri;

	BOOL bRun = TRUE;
	if (nSize <= idx) {
		pOneLine->push_back(data);
		return 1;
	}
	do {
		pCrntPnt = &(*pVecFifo)[idx++];
		if (pPrePnt->y != pCrntPnt->y) {
			idx--;
			pOneLine->push_back(data);
			break;
		}
		else if (abs(pCrntPnt->x - pPrePnt->x) == 1) {
			data.count++;
			data.nBriSum += pCrntPnt->btBright;
			data.nOrgBriSum += pCrntPnt->btOrgBri;
			if (data.btBriMax < pCrntPnt->btBright) {
				data.btBriMax = pCrntPnt->btBright;				
			} else if( data.btBriMin > pCrntPnt->btBright ){
				data.btBriMin = pCrntPnt->btBright;				
			}
			if (data.btOrgBriMax < pCrntPnt->btOrgBri) {
				data.btOrgBriMax = pCrntPnt->btOrgBri;
			} else if (data.btOrgBriMin > pCrntPnt->btOrgBri) {
				data.btOrgBriMin = pCrntPnt->btOrgBri;
			}
		}
		else {
			pOneLine->push_back(data);
			data.count = 1;
			data.start = pCrntPnt->x;
			data.ypos = pCrntPnt->y;
			data.nBriSum = pPrePnt->btBright;
			data.btBriMax = pPrePnt->btBright;
			data.btBriMin = pPrePnt->btBright;
			data.btOrgBriMin = pPrePnt->btOrgBri;
			data.btOrgBriMax = pPrePnt->btOrgBri;
			data.nOrgBriSum = pPrePnt->btOrgBri;
		}
		pPrePnt = pCrntPnt;
		if (nSize <= idx) {
			pOneLine->push_back(data);
			bRun = FALSE;
		}

	} while (bRun);

	return (idx - nStartPos);

}

BOOL CImageProcess::MergeLineData(_LINE_DATA* pSrcData, _LINE_DATA* pTarData) // pSrcData 에 pTarData를 Merge 하고 pTarData를 제거함.
{
	BOOL bRet = TRUE;

	pTarData->pPtr = pSrcData->pPtr;
	if (pSrcData->pPtr->rcRect.left > pTarData->start) {
		pSrcData->pPtr->rcRect.left = pTarData->start;
	}
	if (pSrcData->pPtr->rcRect.right < pTarData->start + (pTarData->count - 1)) {
		pSrcData->pPtr->rcRect.right = pTarData->start + (pTarData->count - 1);
	}
	if (pSrcData->pPtr->rcRect.top > pTarData->ypos) {
		pSrcData->pPtr->rcRect.top = pTarData->ypos;
	}

	pSrcData->pPtr->rcRect.bottom = pTarData->ypos;
	pSrcData->pPtr->nPixelCnt += pTarData->count;
	pSrcData->pPtr->nBriSum += pTarData->nBriSum;
	pSrcData->pPtr->nOrgBriSum += pTarData->nOrgBriSum;
	if (pSrcData->pPtr->nBriMax < pTarData->btBriMax) {
		pSrcData->pPtr->nBriMax = pTarData->btBriMax;
	}
	if (pSrcData->pPtr->nOrgBriMax < pTarData->btOrgBriMax) {
		pSrcData->pPtr->nOrgBriMax = pTarData->btOrgBriMax;
	}
	if (pSrcData->pPtr->nBriMin > pTarData->btBriMin) {
		pSrcData->pPtr->nBriMin = pTarData->btBriMin;
	}
	if (pSrcData->pPtr->nOrgBriMin > pTarData->btOrgBriMin) {
		pSrcData->pPtr->nOrgBriMin = pTarData->btOrgBriMin;
	}

	return bRet;
}

BOOL CImageProcess::LoopLabeling(CRegionInfo* pRoiInfo, int nWidth, int nHeight, _VEC_BLOCK* pVecBlock, BYTE btBit, int MinSize, double dResolX, double dResolY )
{
	_VEC_BLOCK_PTR VecBlockTemp;
	VecBlockTemp.clear();
	VEC_LINE_DATA* pVecPreLine;
	VEC_LINE_DATA* pVecNewLine;
	VEC_LINE_DATA* pTempLine;

	int nTotalCnt = 0;

	CRegionInfo::VEC_FIFO* pVecFifo = pRoiInfo->GetFifoPtr();
	ASSERT(pVecFifo);
	int nSize = (int)pVecFifo->size();

	if (nSize <= 1) {
		return TRUE;
	}

	pVecPreLine = new VEC_LINE_DATA;
	pVecNewLine = new VEC_LINE_DATA;
	pVecPreLine->clear();
	pVecPreLine->clear();

	int i = 0;
	int preY = 0;

	CPixel tmp;
	tmp.x = -10;
	tmp.y = 0;
	CPixel* pPrePnt = &tmp;
	CPixel* pCrntPnt = NULL;

	_LINE_DATA data;
	memset(&data, 0x00, sizeof(_LINE_DATA));

	int nMode = 0 ;
	switch (btBit) {
	case CImageProcess::en_FoilExp_Bit : 
		nMode = TYPE_FOILEXP;
		break;
	case CImageProcess::en_Dross_Bit :
		nMode = TYPE_FOILEXP_OUT;
		break;
	// 22.05.10 Ahn Add Start
	case CImageProcess::en_Surface_Bit :
		nMode = TYPE_SURFACE;
		break;
	// 22.05.10 Ahn Add End
	default :
		break;
	}

	for (; i < nSize; ) {
		int nCnt = 0;
		pVecNewLine->clear();
		i += GetOneLineFromFifo(pVecNewLine, pVecFifo, i);

		int nPreSize = (int)pVecPreLine->size();
		int nCrntSize = (int)pVecNewLine->size();

		if ((nPreSize > 0) && (nCrntSize > 0)) {
			_LINE_DATA* pNewData = &(*pVecNewLine)[0];
			_LINE_DATA* pPreData = &(*pVecPreLine)[0];
			if (abs(pNewData->ypos - pPreData->ypos) == 1) {
				for (int j = 0; j < nCrntSize; j++) {
					pNewData = &(*pVecNewLine)[j];
					for (int k = 0; k < nPreSize; k++) {
						pPreData = &(*pVecPreLine)[k];
						//	if (pPreData->pPtr->bDeleteFlag == TRUE) continue;
						if (IsLinked(pNewData, pPreData) == TRUE) {
							if (pPreData->pPtr == NULL) continue;
							if (pNewData->pPtr == NULL) { // 새결함
								//MergeLineData(pPreData, pNewData);
								pNewData->pPtr = pPreData->pPtr;
								if (pPreData->pPtr->rcRect.left > pNewData->start){
									pPreData->pPtr->rcRect.left = pNewData->start;
								}
								if (pPreData->pPtr->rcRect.right < pNewData->start + (pNewData->count - 1)){
									pPreData->pPtr->rcRect.right = pNewData->start + (pNewData->count - 1);
								}
								if (pPreData->pPtr->rcRect.top > pNewData->ypos){
									pPreData->pPtr->rcRect.top = pNewData->ypos;
								}

								pPreData->pPtr->rcRect.bottom = pNewData->ypos;
								pPreData->pPtr->nPixelCnt += pNewData->count;
								pPreData->pPtr->nBriSum += pNewData->nBriSum;
								pPreData->pPtr->nOrgBriSum += pNewData->nOrgBriSum;
								if (pPreData->pPtr->nBriMax < pNewData->btBriMax) {
									pPreData->pPtr->nBriMax = pNewData->btBriMax;
								}
								if (pPreData->pPtr->nOrgBriMax < pNewData->btOrgBriMax) {
									pPreData->pPtr->nOrgBriMax = pNewData->btOrgBriMax;
								}
								if (pPreData->pPtr->nBriMin > pNewData->btBriMin) {
									pPreData->pPtr->nBriMin = pNewData->btBriMin;									
								}
								if (pPreData->pPtr->nOrgBriMin > pNewData->btOrgBriMin) {
									pPreData->pPtr->nOrgBriMin = pNewData->btOrgBriMin;
								}
							}
							else { // NewData에 이미 연결된 PreData가 있는 경우 PreData->pPtr값을 합치고 제거함.
								if (pPreData->pPtr == pNewData->pPtr) {
									continue;
								}
								if (pPreData->pPtr->bDeleteFlag == TRUE) {
									pPreData->pPtr = pNewData->pPtr;
									continue;
								}
								if (pNewData->pPtr->bDeleteFlag == TRUE) {
									continue;
								}

								pPreData->pPtr->bDeleteFlag = TRUE;
								
								if (pNewData->pPtr->rcRect.left > pPreData->pPtr->rcRect.left) {
									pNewData->pPtr->rcRect.left = pPreData->pPtr->rcRect.left;
								}
								if (pNewData->pPtr->rcRect.right < pPreData->pPtr->rcRect.right) {
									pNewData->pPtr->rcRect.right = pPreData->pPtr->rcRect.right;
								}
								if (pNewData->pPtr->rcRect.top > pPreData->pPtr->rcRect.top) {
									pNewData->pPtr->rcRect.top = pPreData->pPtr->rcRect.top;
								}
								pNewData->pPtr->nPixelCnt += pPreData->pPtr->nPixelCnt;
								pNewData->pPtr->nBriSum += pPreData->pPtr->nBriSum;
								pNewData->pPtr->nOrgBriSum += pPreData->pPtr->nOrgBriSum;

								if (pNewData->pPtr->nBriMax < pPreData->btBriMax) {
									pNewData->pPtr->nBriMax = pPreData->btBriMax;
								}
								if (pNewData->pPtr->nOrgBriMax < pPreData->btOrgBriMax) {
									pNewData->pPtr->nOrgBriMax = pPreData->btOrgBriMax;

								}
								if (pNewData->pPtr->nBriMin > pPreData->btBriMin) {
									pNewData->pPtr->nBriMin = pPreData->btBriMin;
								}
								if (pNewData->pPtr->nOrgBriMin > pPreData->btOrgBriMin) {
									pNewData->pPtr->nOrgBriMin = pPreData->btOrgBriMin;
								}

								pPreData->pPtr = pNewData->pPtr;
							}
						}
					}
				}
			}
		}
		if (nCrntSize > 0) {
			// nVecPreLine 값은 모두 BlockData로 저장
			for (int j = 0; j < nCrntSize; j++) {
				_LINE_DATA* pData = &(*pVecNewLine)[j];
				if (pData->pPtr == NULL) {
					CBlockData* pBlock = new CBlockData();
					pBlock->rcRect.left = pData->start;
					pBlock->rcRect.top = pData->ypos;
					pBlock->rcRect.right = pData->start + (pData->count-1);
					pBlock->rcRect.bottom = pData->ypos;
					pBlock->nPixelCnt = pData->count;
					pBlock->nBriSum = pBlock->nBriMin  = pBlock->nBriMax = pData->btBriMax;
					pBlock->nOrgBriSum = pBlock->nOrgBriMin = pBlock->nOrgBriMax = pData->btOrgBriMax;
					pData->pPtr = pBlock;
					VecBlockTemp.push_back(pBlock);
				}
			}
		}
		else {
			int a = 0;
		}

		pTempLine = pVecPreLine;
		pVecPreLine = pVecNewLine;
		pVecNewLine = pTempLine;
		pVecNewLine->clear();
	}

	while (!VecBlockTemp.empty()) {
		CBlockData* pData = VecBlockTemp.front();
		VecBlockTemp.erase(VecBlockTemp.begin());
		// 21.08.25 Ahn Modify Start
		if ( (pData->bDeleteFlag == FALSE) && ( MinSize < pData->nPixelCnt )){
		//if ( pData->bDeleteFlag == FALSE ) {
		// 21.08.25 Ahn Modify End
			if (pData->rcRect.left > pData->rcRect.right) {
				SWAP(int, pData->rcRect.left, pData->rcRect.right);
			}
			pData->nWidth = pData->rcRect.right - pData->rcRect.left;
			pData->nHeight = pData->rcRect.bottom- pData->rcRect.top;
			pData->nType = nMode;
			pData->nBriAve = (int)( pData->nBriSum / pData->nPixelCnt );
			pData->nBriMax = pData->nBriMax;
			pData->nBriMin = pData->nBriMin;
			pData->nOrgBriAve = (int)(pData->nOrgBriSum / pData->nPixelCnt);
			pData->nOrgBriMin = pData->nOrgBriMin;
			pData->nOrgBriMax = pData->nOrgBriMax;
			pData->dWidth = (pData->nWidth + 1) * dResolX;
			pData->dHeight = (pData->nHeight + 1) * dResolY;

			pVecBlock->push_back(*pData);
		}
		// 22.01.20 Ahn Add Start
		else {
			if ( (MinSize >= pData->nPixelCnt) && (pData->bDeleteFlag == FALSE )) { // 22.01.27 Ahn Add 
				BYTE* pImgPtr = pRoiInfo->GetRoiPtr();
				if (pImgPtr != NULL) {
					int nSize = pData->nPixelCnt;
					int x, y;

					BYTE* pLinePtr;
				
					for ( y = pData->rcRect.top; y <= pData->rcRect.bottom; y++) {
						pLinePtr = pImgPtr + ( nWidth * y ) ;
						for ( x = pData->rcRect.left; x <= pData->rcRect.right; x++) {
							*(pLinePtr + x) = 0x00;
						}
					}
				}
			}
		}
		// 22.01.20 Ahn Add End
		delete pData;
		pData = NULL;
	}
	VecBlockTemp.clear();

	delete pVecPreLine;
	delete pVecNewLine;

	return 0;
}

int CImageProcess::Edge_Sobel(BYTE* pImgPtr, BYTE* pProcImg, int nWidth, int nHeight, CRect rcProc, int nMode)
{
	ASSERT(pImgPtr);
	ASSERT(pProcImg);

	int nStartX = rcProc.left;
	int nEndX = rcProc.right;
	int nStartY = rcProc.top;
	int nEndY = rcProc.bottom;

	int x, y;
	BYTE* pLinePtr;
	BYTE* pLineTop;
	BYTE* pLineBtm;
	BYTE* pTarLinePtr;
	if (nStartY <= 0) nStartY = 1;
	if (nEndY >= nHeight) nEndY = nHeight - 1 ;
	if (nStartX <= 0) nStartX = 1;
	if (nEndX >= nWidth) nEndX = nWidth - 1;

	int nLeft, nRight;
	int nTop, nBottom;
	int nSumH, nSumV;
	for (y = nStartY; y < nEndY; y++) {
		pLinePtr = pImgPtr + (nWidth * y);
		pTarLinePtr = pProcImg + (nWidth * y);
		pLineTop = pLinePtr - nWidth;
		pLineBtm = pLinePtr + nWidth;
		for (x = nStartX; x < nEndX; x++) {
			nLeft = (*(pLinePtr + x - 1) * 2) + *(pLineTop + x - 1) + *(pLineBtm + x - 1);
			nRight = (*(pLinePtr + x + 1) * 2) + *(pLineTop + x + 1) + *(pLineBtm + x + 1);
			nSumH = abs(nLeft - nRight) / 3;
			if (nSumH >= 255) {
				*(pTarLinePtr + x) = 255;
				continue;
			}

			nTop = *(pLineTop + x - 1) + (*(pLineTop + x) * 2) + *(pLineTop + x + 1);
			nBottom = *(pLineBtm + x - 1) + (*(pLineBtm + x) * 2) + *(pLineBtm + x + 1);
			nSumV = abs(nTop - nBottom) / 3;
			if (nSumV >= 255) {
				*(pTarLinePtr + x) = 255;
				continue;
			}

			*(pTarLinePtr + x) = ( nSumV > nSumH )?(BYTE)nSumV : (BYTE)nSumH ;
		}
	}
	if (nStartY <= 0) {
		y = 0;
		pTarLinePtr = pProcImg + (nWidth * y);
		pLinePtr = pImgPtr + (nWidth * y);
		pLineTop = pLinePtr + nWidth;
		pLineBtm = pLinePtr + nWidth;
		for (x = nStartX; x < nEndX; x++) {
			nLeft = (*(pLinePtr + x - 1) * 2) + *(pLineTop + x - 1) + *(pLineBtm + x - 1);
			nRight = (*(pLinePtr + x + 1) * 2) + *(pLineTop + x + 1) + *(pLineBtm + x + 1);
			nSumH = abs(nLeft - nRight) / 3;

			nTop = *(pLineTop + x - 1) + (*(pLineTop + x) * 2) + *(pLineTop + x + 1);
			nBottom = *(pLineBtm + x - 1) + (*(pLineBtm + x) * 2) + *(pLineBtm + x + 1);
			nSumV = abs(nTop - nBottom) / 3;

			int nValue = (nSumV > nSumH) ? nSumV : nSumH;
			if (nValue > 255) {
				*(pTarLinePtr + x) = 255;
			}
			else {
				*(pTarLinePtr + x) = (BYTE)nValue;
			}
		}
	}
	if (nEndY >= nHeight) {
		y = nHeight;
		pTarLinePtr = pProcImg + (nWidth * y);
		pLinePtr = pImgPtr + (nWidth * y);
		pLineTop = pLinePtr - nWidth;
		pLineBtm = pLinePtr - nWidth;
		for (x = nStartX; x < nEndX; x++) {
			nLeft = (*(pLinePtr + x - 1) * 2) + *(pLineTop + x - 1) + *(pLineBtm + x - 1);
			nRight = (*(pLinePtr + x + 1) * 2) + *(pLineTop + x + 1) + *(pLineBtm + x + 1);
			nSumH = abs(nLeft - nRight) / 3;

			nTop = *(pLineTop + x - 1) + (*(pLineTop + x) * 2) + *(pLineTop + x + 1);
			nBottom = *(pLineBtm + x - 1) + (*(pLineBtm + x) * 2) + *(pLineBtm + x + 1);
			nSumV = abs(nTop - nBottom) / 3;

			int nValue = (nSumV > nSumH) ? nSumV : nSumH;
			if (nValue > 255) {
				*(pTarLinePtr + x) = 255;
			}
			else {
				*(pTarLinePtr + x) = (BYTE)nValue;
			}

		}
	}

	return 0;
}

//
//int CImageProcess::EdgeDetectImageToImage(BYTE* pImgPtr, BYTE* pProcImg, int nWidth, int nHeight, CRect rectProc, int nCompWidth, int nMagnif, int nDir)
//{
//
//	int x, y, i;
//	int nSumCnt = nCompWidth;
//	int nDiff = 0;
//	int nXStart, nXEnd, nYStart, nYEnd;
//
//	if (nMagnif < 1) {
//		nMagnif = 1;
//	}
//	else if (nMagnif > 3) {
//		nMagnif = 3;
//	}
//
//	if (nDir == DIR_VER) { // 세로 엣치 찾기
//		int nSumLeft, nSumRight;
//		nXStart = rectProc.left + nSumCnt;
//		nXEnd = rectProc.right - nSumCnt;
//		nYStart = rectProc.top;
//		nYEnd = rectProc.bottom;
//
//		for (y = nYStart; y < nYEnd; y++) {
//			BYTE* pLinePtr = pImgPtr + (y * nWidth);
//			BYTE* pProcLinePtr = pProcImg + (y * nWidth);
//			int nMax = 0;
//			for (x = nXStart; x < nXEnd; x++) {
//				nSumLeft = 0;
//				nSumRight = 0;
//				for (i = 0; i < nSumCnt; i++) {
//					nSumLeft += *(pLinePtr + x - i);
//					nSumRight += *(pLinePtr + x + i);
//				}
//				int nAveLeft = (nSumLeft / nSumCnt);
//				int nAveRight = (nSumRight / nSumCnt);
//				nDiff = abs(nAveLeft - nAveRight) * nMagnif;
//				*(pProcLinePtr + x) = (nDiff > 255 ? 255 : nDiff);
//			}
//		}
//	}
//	else {// 가로 엣지 찾기
//		int nSumUpper, nSumLower;
//		nXStart = rectProc.left;
//		nXEnd = rectProc.right;
//		nYStart = rectProc.top + nSumCnt;
//		nYEnd = rectProc.bottom - nSumCnt;
//
//		for (x = nXStart; x < nXEnd; x++) {
//			int nMax = 0;
//			for (y = nYStart; y < nYEnd; y++) {
//				nSumUpper = 0;
//				nSumLower = 0;
//				for (i = 0; i < nSumCnt; i++) {
//					nSumUpper += *(pImgPtr + ((y - i) * nWidth) + x);
//					nSumLower += *(pImgPtr + ((y + i) * nWidth) + x);
//				}
//				int nAveUpper = (nSumUpper / nSumCnt);
//				int nAveLower = (nSumLower / nSumCnt);
//				nDiff = abs(nAveUpper - nAveLower) * nMagnif;
//				*(pProcImg + (y * nWidth) + x) = (nDiff > 255 ? 255 : nDiff);
//			}
//		}
//	}
//
//	return 0;
//}

int CImageProcess::EdgeDetectImageToBoth_RndInfo(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO *pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nCompWidth, int nMagnif, int nDir)
{
	int x, y, i;
	int nSumCnt = nCompWidth;
	int nDiff = 0;
	int nXStart, nXEnd, nYStart, nYEnd;

	if (nSumCnt <= 0) return -1; // 22.07.01 Ahn Add 

	if (nMagnif < 1) {
		nMagnif = 1;
	}
	else if (nMagnif > 3) {
		nMagnif = 3;
	}
	int nMaxPos[10];
	memset(nMaxPos, 0, sizeof(nMaxPos));

	int *pnRsltArr;
	pnRsltArr = new int[nHeight];
	memset(pnRsltArr, 0, sizeof(int) * nHeight);

	if (pProcImg == NULL) {

		if (nDir == DIR_VER) { // 세로 엣치 찾기
			int nSumLeft, nSumRight;
			nXStart = rectProc.left + nSumCnt;
			nXEnd = rectProc.right - nSumCnt;
			nYStart = rectProc.top;
			nYEnd = rectProc.bottom;

			int nPitch = (nYEnd - nYStart) / 10;
			int nMinPosX = nXEnd;
			int nMaxPosX = nXStart;
			int nCnt = 0;

			for (y = nYStart, nCnt = 0; nCnt < 10; y += nPitch, nCnt++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				//BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				for (x = nXStart; x < nXEnd; x++) {
					nSumLeft = 0;
					nSumRight = 0;
					for (i = 0; i < nSumCnt; i++) {
						nSumLeft += *(pLinePtr + x - i);
						nSumRight += *(pLinePtr + x + i);
					}
					int nAveLeft = (nSumLeft / nSumCnt);
					int nAveRight = (nSumRight / nSumCnt);
					nDiff = abs(nAveLeft - nAveRight) * nMagnif;
					/**(pProcLinePtr + x) = (nDiff > 255 ? 255 : nDiff);*/
					if (nDiff >= nMax) {
						nMax = nDiff;
						pnRsltArr[y] = x;
						nMaxPos[nCnt] = x;
					}
				}
			}

			for (int i = 0; i < 10; i++) {
				if (nMaxPos[i] < nMinPosX) {
					nMinPosX = nMaxPos[i];
				}
				if (nMaxPos[i] > nMaxPosX) {
					nMaxPosX = nMaxPos[i];
				}
			}

			nXStart = nMinPosX - (nSumCnt * 2);
			nXEnd = nMaxPosX + (nSumCnt * 2);
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				for (x = nXStart; x < nXEnd; x++) {
					nSumLeft = 0;
					nSumRight = 0;
					for (i = 0; i < nSumCnt; i++) {
						nSumLeft += *(pLinePtr + x - i);
						nSumRight += *(pLinePtr + x + i);
					}
					int nAveLeft = (nSumLeft / nSumCnt);
					int nAveRight = (nSumRight / nSumCnt);
					nDiff = abs(nAveLeft - nAveRight) * nMagnif;
				//	*(pProcLinePtr + x) = (nDiff > 255 ? 255 : nDiff);
					if (nDiff >= nMax) {
						nMax = nDiff;
						pnRsltArr[y] = x;
					}
				}
			}
		}
		else {// 가로 엣지 찾기
			int nSumUpper, nSumLower;
			nXStart = rectProc.left;
			nXEnd = rectProc.right;
			nYStart = rectProc.top + nSumCnt;
			nYEnd = rectProc.bottom - nSumCnt;

			for (x = nXStart; x < nXEnd; x++) {
				int nMax = 0;
				for (y = nYStart; y < nYEnd; y++) {
					nSumUpper = 0;
					nSumLower = 0;
					for (i = 0; i < nSumCnt; i++) {
						nSumUpper += *(pImgPtr + ((y - i) * nWidth) + x);
						nSumLower += *(pImgPtr + ((y + i) * nWidth) + x);
					}
					int nAveUpper = (nSumUpper / nSumCnt);
					int nAveLower = (nSumLower / nSumCnt);
					nDiff = abs(nAveUpper - nAveLower) * nMagnif;
					//*(pProcImg + (y * nWidth) + x) = (nDiff > 255 ? 255 : nDiff);
					if (nDiff >= nMax) {
						nMax = nDiff;
						pnRsltArr[y] = x;
					}
				}
			}
		}
	}
	else {

		if (nDir == DIR_VER) { // 세로 엣치 찾기
			int nSumLeft, nSumRight;
			nXStart = rectProc.left + nSumCnt;
			nXEnd = rectProc.right - nSumCnt;
			nYStart = rectProc.top;
			nYEnd = rectProc.bottom;

			int nPitch = (nYEnd - nYStart) / 10;
			int nMinPosX = nXEnd;
			int nMaxPosX = nXStart;
			int nCnt = 0;

			for (y = nYStart, nCnt = 0; nCnt < 10; y+= nPitch, nCnt++ ) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				//BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				for (x = nXStart; x < nXEnd; x++) {
					nSumLeft = 0;
					nSumRight = 0;
					for (i = 0; i < nSumCnt; i++) {
						nSumLeft += *(pLinePtr + x - i);
						nSumRight += *(pLinePtr + x + i);
					}
					int nAveLeft = (nSumLeft / nSumCnt);
					int nAveRight = (nSumRight / nSumCnt);
					nDiff = abs(nAveLeft - nAveRight) * nMagnif;
					/**(pProcLinePtr + x) = (nDiff > 255 ? 255 : nDiff);*/
					if (nDiff >= nMax) {
						nMax = nDiff;
						pnRsltArr[y] = x;
						nMaxPos[nCnt] = x;
					}
				}
			}

			for (int i = 0; i < 10; i++) {
				if (nMaxPos[i] < nMinPosX) {
					nMinPosX = nMaxPos[i];
				}
				if (nMaxPos[i] > nMaxPosX) {
					nMaxPosX = nMaxPos[i];
				}
			}

			nXStart = nMinPosX - ( nSumCnt * 2 );
			nXEnd = nMaxPosX + ( nSumCnt * 2 ) ;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				for (x = nXStart; x < nXEnd; x++) {
					nSumLeft = 0;
					nSumRight = 0;
					for (i = 0; i < nSumCnt; i++) {
						nSumLeft += *(pLinePtr + x - i);
						nSumRight += *(pLinePtr + x + i);
					}
					int nAveLeft = (nSumLeft / nSumCnt);
					int nAveRight = (nSumRight / nSumCnt);
					nDiff = abs(nAveLeft - nAveRight) * nMagnif;
					*(pProcLinePtr + x) = (nDiff > 255 ? 255 : nDiff);
					if (nDiff >= nMax) {
						nMax = nDiff;
						pnRsltArr[y] = x;
					}
				}
			}
		}
		else {// 가로 엣지 찾기
			int nSumUpper, nSumLower;
			nXStart = rectProc.left;
			nXEnd = rectProc.right;
			nYStart = rectProc.top + nSumCnt;
			nYEnd = rectProc.bottom - nSumCnt;

			for (x = nXStart; x < nXEnd; x++) {
				int nMax = 0;
				for (y = nYStart; y < nYEnd; y++) {
					nSumUpper = 0;
					nSumLower = 0;
					for (i = 0; i < nSumCnt; i++) {
						nSumUpper += *(pImgPtr + ((y - i) * nWidth) + x);
						nSumLower += *(pImgPtr + ((y + i) * nWidth) + x);
					}
					int nAveUpper = (nSumUpper / nSumCnt);
					int nAveLower = (nSumLower / nSumCnt);
					nDiff = abs(nAveUpper - nAveLower) * nMagnif;
					*(pProcImg + (y * nWidth) + x) = (nDiff > 255 ? 255 : nDiff);
					if (nDiff >= nMax) {
						nMax = nDiff;
						pnRsltArr[y] = x;
					}
				}
			}
		}
	}


	CPoint cp;

	for (y = rectProc.top; y < rectProc.bottom; y++ ){
		cp.x = pnRsltArr[y];
		cp.y = y;
		pVecRndInfo->push_back(cp);
	}

	int nSize = (int)pVecRndInfo->size();
	int nLeftDiff, nRightDiff;
	int nDiffRange = 3;
	for (y = 1; y < nSize-1; y++) {
		nLeftDiff = abs( (*pVecRndInfo)[y].x - (*pVecRndInfo)[y - 1].x ) ;
		nRightDiff = abs( (*pVecRndInfo)[y].x - (*pVecRndInfo)[y  + 1].x ) ;
		if ((nLeftDiff > nDiffRange) && (nRightDiff > nDiffRange)) {
			(*pVecRndInfo)[y].x = int( ((*pVecRndInfo)[y - 1].x + (*pVecRndInfo)[y + 1].x) / 2 );
		}
	}

	delete []pnRsltArr;
	return 0;
}

// 22.01.13 Ahn Add Start
int CImageProcess::EdgeDetectByRndInfo_Negative(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nThresMin, int nThresMax, int nPosition, int nDefaultLv, int nSide)
{
	int x, y;
	int nDiff = 0;
	int nXStart, nXEnd, nYStart, nYEnd;

	if ((rectProc.bottom - rectProc.top) <= 0) {
		return -1;
	}

	int* pnRsltArr;
	pnRsltArr = new int[nHeight];
	memset(pnRsltArr, 0, sizeof(int) * nHeight);

	int nMaxPos[10];
	memset(nMaxPos, 0, sizeof(nMaxPos));

	int nSumCnt = 10;
	nXStart = rectProc.left + nSumCnt;
	nXEnd = rectProc.right - nSumCnt;
	nYStart = rectProc.top;
	nYEnd = rectProc.bottom;

	int nPitch = (nYEnd - nYStart) / 10;
	int nMinPosX = nXEnd;
	int nMaxPosX = nXStart;
	int nCnt = 0;

	if (pProcImg != NULL) {
		if (nPosition == CImageProcess::en_TopSide) { // 탑

			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				int nCnt = 0;

				// 22.11.07 Ahn Modify Start
				for (x = nXStart; x < nXEnd; x++) {
				//for (x = nXEnd - 1 ; x >= nXStart; x--) {
				// 22.11.07 Ahn Modify End
					BYTE btLevel = *(pLinePtr + x);
					// 22.05.30 Ahn Modify Start
					if (btLevel > nThresMax) {
						// 23.01.06 Ahn Modify Start
						//bNotFound = FALSE;
						//nLastPosX = pnRsltArr[y] = x;
						bNotFound = TRUE;
						// 23.01.06 Ahn Modify Start
						break;
					}
					else 
					{
						if (btLevel < nThresMin) {
							nCnt++;
							if (nCnt > 2) {
								bNotFound = FALSE;
								nLastPosX = pnRsltArr[y] = x;
								pProcLinePtr[x] = 255;
								break;
							}
						}
						else {
							nCnt = 0;
						}
					}
					// 22.05.30 Ahn Modify End
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
					pProcLinePtr[nLastPosX] = 255;
				}
			}
		}
		else {
			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				int nCnt = 0;
				for (x = nXEnd - 1; x >= nXStart; x--) {
					BYTE btLevel = *(pLinePtr + x);
					// 22.05.30 Ahn Modify Start
					if (btLevel > nThresMax) {
						// 23.01.06 Ahn Modify Start
						//bNotFound = FALSE;
						//nLastPosX = pnRsltArr[y] = x;
						bNotFound = TRUE;
						// 23.01.06 Ahn Modify End
						break;
					}
					else {
						if (btLevel < nThresMin) {
							nCnt++;
							if (nCnt > 2) {
								bNotFound = FALSE;
								nLastPosX = pnRsltArr[y] = x;
								pProcLinePtr[x] = 255;
								break;
							}
						}
						else {
							nCnt = 0;
						}
					}
					// 22.05.30 Ahn Modify End
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
					pProcLinePtr[nLastPosX] = 255;
				}

			}
		}
	}
	else {
		if (nPosition == CImageProcess::en_TopSide) { // 탑
	
			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;

				for (x = nXStart; x < nXEnd; x++) {
					BYTE btLevel = *(pLinePtr + x);
					// 22.05.30 Ahn Modify Start
					if (btLevel > nThresMax) {
						bNotFound = FALSE;
						pnRsltArr[y] = x ;
						break;
					}
					else {
						if (btLevel < nThresMin) {
							nLastPosX = pnRsltArr[y] = x;
							bNotFound = FALSE;
							break;
						}
					}
					// 22.05.30 Ahn Modify End
				}
				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
				}
			}
		}
		else {

			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				for (x = nXEnd - 1; x >= nXStart; x--) {
					BYTE btLevel = *(pLinePtr + x);
					// 22.05.30 Ahn Modify Start
					if (btLevel > nThresMax) {
						break;
					}
					else {
						if (btLevel < nThresMin) {
							nLastPosX = pnRsltArr[y] = x;
							bNotFound = FALSE;
							break;
						}
					}
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
				}
			}
		}
	}


	CPoint cp;

	for (y = rectProc.top; y < rectProc.bottom; y++) {
		cp.x = pnRsltArr[y];
		cp.y = y;
		pVecRndInfo->push_back(cp);
	}

	int nSize = (int)pVecRndInfo->size();
	int nLeftDiff, nRightDiff;
	int nDiffRange = 3;
	for (y = 1; y < nSize - 1; y++) {
		nLeftDiff = abs((*pVecRndInfo)[y].x - (*pVecRndInfo)[y - 1].x);
		nRightDiff = abs((*pVecRndInfo)[y].x - (*pVecRndInfo)[y + 1].x);
		if ((nLeftDiff > nDiffRange) && (nRightDiff > nDiffRange)) {
			(*pVecRndInfo)[y].x = int(((*pVecRndInfo)[y - 1].x + (*pVecRndInfo)[y + 1].x) / 2);
		}
	}

	delete[]pnRsltArr;
	return 0;
}
// 22.01.13 Ahn Add End


int CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nThreshold, int nPosition, int nDefaultLv, int nSide )
{
	int x, y ;
	int nDiff = 0;
	int nXStart, nXEnd, nYStart, nYEnd;

	if ((rectProc.bottom - rectProc.top) <= 0) {
		return -1;
	}

	int* pnRsltArr;
	pnRsltArr = new int[nHeight];
	memset(pnRsltArr, 0, sizeof(int) * nHeight);

	int nMaxPos[10];
	memset(nMaxPos, 0, sizeof(nMaxPos));

	int nSumCnt = 10;
	nXStart = rectProc.left + nSumCnt;
	nXEnd = rectProc.right - nSumCnt;
	nYStart = rectProc.top;
	nYEnd = rectProc.bottom;

	int nPitch = (nYEnd - nYStart) / 10;
	int nMinPosX = nXEnd;
	int nMaxPosX = nXStart;
	int nCnt = 0;

	// 22.09.29 Ahn Modify Start
	if (nXEnd > nWidth) {
		nXEnd = nWidth;
	}
	if (nXStart < 0) {
		nXStart = 0;
	}
	// 22.09.29 Ahn Modify End


	if (pProcImg == NULL) {
		if (nPosition == CImageProcess::en_TopSide) { // 탑
			if (nSide == CImageProcess::en_FindLeft) { // 탭 기준 왼쪽 Edge 검출
				for (y = nYStart, nCnt = 0; nCnt < 10; y += nPitch, nCnt++) {
					BYTE* pLinePtr = pImgPtr + (y * nWidth);
					int nMax = 0;
					for (x = nXEnd - 1; x >= nXStart; x--) {
						BYTE btLevel = *(pLinePtr + x);
						if (btLevel > nThreshold) {
							pnRsltArr[y] = x;
							nMaxPos[nCnt] = x;
							break;
						}
					}
				}
			}
			else { // 탭 기준 오른쪽 엣지 검출
				for (y = nYEnd - 1, nCnt = 0; nCnt < 10; y -= nPitch, nCnt++) {
					BYTE* pLinePtr = pImgPtr + (y * nWidth);
					int nMax = 0;
					for (x = nXEnd - 1; x >= nXStart; x--) {
						BYTE btLevel = *(pLinePtr + x);
						if (btLevel > nThreshold) {
							pnRsltArr[y] = x;
							nMaxPos[nCnt] = x;
							break;
						}
					}
				}
			}

			for (int i = 0; i < 10; i++) {
				if (nMaxPos[i] <= 0) continue;
				if (nMaxPos[i] < nMinPosX) {
					nMinPosX = nMaxPos[i];
				}
				if (nMaxPos[i] > nMaxPosX) {
					nMaxPosX = nMaxPos[i];
				}
			}

			nXStart = nMinPosX - (nSumCnt * 2);
			nXEnd = nMaxPosX + (nSumCnt * 2);

			// 22.09.29 Ahn Modify Start
			if (nXEnd > nWidth) {
				nXEnd = nWidth ;
			}
			if (nXStart < 0) {
				nXStart = 0;
			}
			// 22.09.29 Ahn Modify End

			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;

				for (x = nXEnd - 1; x >= nXStart; x--) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel > nThreshold) {
						nLastPosX = pnRsltArr[y] = x;
						bNotFound = FALSE;
						break;
					}
				}
				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
				}
			}
		}
		else {
			for (y = nYStart, nCnt = 0; nCnt < 10; y += nPitch, nCnt++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				int nMax = 0;
				for (x = nXStart; x < nXEnd; x++) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel > nThreshold) {
						pnRsltArr[y] = x;
						nMaxPos[nCnt] = x;
						break;
					}
				}
			}

			for (int i = 0; i < 10; i++) {
				if (nMaxPos[i] <= 0) continue;
				if (nMaxPos[i] < nMinPosX) {
					nMinPosX = nMaxPos[i];
				}
				if (nMaxPos[i] > nMaxPosX) {
					nMaxPosX = nMaxPos[i];
				}
			}

			nXStart = nMinPosX - (nSumCnt * 2);
			nXEnd = nMaxPosX + (nSumCnt * 2);
			int nLastPosX = nDefaultLv;

			// 22.09.29 Ahn Modify Start
			if (nXEnd > nWidth) {
				nXEnd = nWidth;
			}
			if (nXStart < 0) {
				nXStart = 0;
			}
			// 22.09.29 Ahn Modify End

			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				for (x = nXStart; x < nXEnd; x++) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel > nThreshold) {
						nLastPosX = pnRsltArr[y] = x;
						bNotFound = FALSE;
						break;
					}
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
				}

			}
		}
	}
	else {

		if (nPosition == CImageProcess::en_TopSide) {
			if (nSide == CImageProcess::en_FindLeft) {
				for (y = nYStart, nCnt = 0; nCnt < 10; y += nPitch, nCnt++) {
					BYTE* pLinePtr = pImgPtr + (y * nWidth);
					int nMax = 0;
					for (x = nXEnd - 1; x >= nXStart; x--) {
						BYTE btLevel = *(pLinePtr + x);
						if (btLevel > nThreshold) {
							pnRsltArr[y] = x;
							nMaxPos[nCnt] = x;
							break;
						}
					}
				}
			}
			else {
				for (y = nYEnd - 1, nCnt = 0; nCnt < 10; y -= nPitch, nCnt++) {
					BYTE* pLinePtr = pImgPtr + (y * nWidth);
					int nMax = 0;
					for (x = nXEnd - 1; x >= nXStart; x--) {
						BYTE btLevel = *(pLinePtr + x);
						if (btLevel > nThreshold) {
							pnRsltArr[y] = x;
							nMaxPos[nCnt] = x;
							break;
						}
					}
				}
			}

			for (int i = 0; i < 10; i++) {
				if (nMaxPos[i] <= 0) continue;
				if (nMaxPos[i] < nMinPosX) {
					nMinPosX = nMaxPos[i];
				}
				if (nMaxPos[i] > nMaxPosX) {
					nMaxPosX = nMaxPos[i];
				}
			}

			nXStart = nMinPosX - (nSumCnt * 2);
			nXEnd = nMaxPosX + (nSumCnt * 2);

			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;

				for (x = nXEnd - 1; x >= nXStart; x--) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel > nThreshold) {
						*(pProcLinePtr + x) = 255;
						nLastPosX = pnRsltArr[y] = x;
						bNotFound = FALSE;
						break;
					}
				}
				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
				}
			}
		}
		else {
			for (y = nYStart, nCnt = 0; nCnt < 10; y += nPitch, nCnt++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				int nMax = 0;
				for (x = nXStart; x < nXEnd; x++) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel > nThreshold) {
						pnRsltArr[y] = x;
						nMaxPos[nCnt] = x;
						break;
					}
				}
			}

			for (int i = 0; i < 10; i++) {
				if (nMaxPos[i] <= 0) continue;
				if (nMaxPos[i] < nMinPosX) {
					nMinPosX = nMaxPos[i];
				}
				if (nMaxPos[i] > nMaxPosX) {
					nMaxPosX = nMaxPos[i];
				}
			}

			nXStart = nMinPosX - (nSumCnt * 2);
			nXEnd = nMaxPosX + (nSumCnt * 2);
			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				for (x = nXStart; x < nXEnd; x++) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel > nThreshold) {
						*(pProcLinePtr + x) = 255;
						nLastPosX = pnRsltArr[y] = x;
						bNotFound = FALSE;
						break;
					}
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
				}
			}
		}
	}


	CPoint cp;

	for (y = rectProc.top; y < rectProc.bottom; y++) {
		cp.x = pnRsltArr[y];
		cp.y = y;
		pVecRndInfo->push_back(cp);
	}

	int nSize = (int)pVecRndInfo->size();
	int nLeftDiff, nRightDiff;
	int nDiffRange = 3;
	for (y = 1; y < nSize - 1; y++) {
		nLeftDiff = abs((*pVecRndInfo)[y].x - (*pVecRndInfo)[y - 1].x);
		nRightDiff = abs((*pVecRndInfo)[y].x - (*pVecRndInfo)[y + 1].x);
		if ((nLeftDiff > nDiffRange) && (nRightDiff > nDiffRange)) {
			(*pVecRndInfo)[y].x = int(((*pVecRndInfo)[y - 1].x + (*pVecRndInfo)[y + 1].x) / 2);
		}
	}

	delete[]pnRsltArr;
	return 0;
}
int CImageProcess::EdgeDetectImageToArray(BYTE* pImgPtr, int* pnResltArr, int nWidth, int nHeight, CRect rectProc, int nCompWidth, int nMagnif, int nDir)
{

	int x, y, i;
	int nSumCnt = nCompWidth;
	int nDiff = 0;
	int nXStart, nXEnd, nYStart, nYEnd;

	if (nMagnif < 1) {
		nMagnif = 1;
	}
	else if (nMagnif > 3) {
		nMagnif = 3;
	}

	if (nDir == DIR_VER) { // 세로 엣치 찾기
		memset(pnResltArr, 0, sizeof(int) * nHeight);

		int nSumLeft, nSumRight;
		nXStart = rectProc.left + nSumCnt;
		nXEnd = rectProc.right - nSumCnt;
		nYStart = rectProc.top;
		nYEnd = rectProc.bottom;

		for (y = nYStart; y < nYEnd; y++) {
			BYTE* pLinePtr = pImgPtr + (y * nWidth);
			int nMax = 0;
			for (x = nXStart; x < nXEnd; x++) {
				nSumLeft = 0;
				nSumRight = 0;
				for (i = 0; i < nSumCnt; i++) {
					nSumLeft += *(pLinePtr + x - i);
					nSumRight += *(pLinePtr + x + i);
				}
				int nAveLeft = (nSumLeft / nSumCnt);
				int nAveRight = (nSumRight / nSumCnt);
				nDiff = abs(nAveLeft - nAveRight) * nMagnif;
				if (nDiff > nMax) {
					nMax = nDiff;
					pnResltArr[y] = x;
					//	nArrMaxVal[y] = nDiff;
				}
			}
		}
	}
	else {// 가로 엣지 찾기
		int nSumUpper, nSumLower;
		nXStart = rectProc.left;
		nXEnd = rectProc.right;
		nYStart = rectProc.top + nSumCnt;
		nYEnd = rectProc.bottom - nSumCnt;
		memset(pnResltArr, 0, sizeof(int) * nWidth);

		for (x = nXStart; x < nXEnd; x++) {
			int nMax = 0;
			for (y = nYStart; y < nYEnd; y++) {
				nSumUpper = 0;
				nSumLower = 0;
				for (i = 0; i < nSumCnt; i++) {
					nSumUpper += *(pImgPtr + ((y - i) * nWidth) + x);
					nSumLower += *(pImgPtr + ((y + i) * nWidth) + x);
				}
				int nAveUpper = (nSumUpper / nSumCnt);
				int nAveLower = (nSumLower / nSumCnt);
				nDiff = abs(nAveUpper - nAveLower) * nMagnif;
				if (nDiff > nMax) {
					nMax = nDiff;
					pnResltArr[y] = x;
				}
			}
		}
	}
	return 0;
}

int CImageProcess::SaveCropImage(BYTE* pImgPtr,int nWidth, int nHeight, CRect rcCrop, CString strFilePath, CString strFileName )
{
	ASSERT(pImgPtr);


	if (rcCrop.left < 0) {
		rcCrop.left = 0 ;
	}
	if (rcCrop.top < 0) {
		rcCrop.top = 0 ;
	}

	if ( (rcCrop.right <= 0) || (rcCrop.bottom <= 0) ) return -1;

	if (rcCrop.right >= nWidth) {
		rcCrop.right = nWidth ;
	}
	if (rcCrop.bottom >= nHeight) {
		rcCrop.bottom = nHeight ;
	}

	if (CWin32File::ExistFile(strFilePath) == FALSE) {
		CWin32File::CreateDirectory(strFilePath);
	}

	int nRmd = rcCrop.Width() % 8;
	if (nRmd  >  0) {
		rcCrop.right = rcCrop.right - nRmd;
	}

	nRmd = rcCrop.Height() % 8;
	if (nRmd > 0) {
		rcCrop.bottom= rcCrop.bottom - nRmd;
	}

	int nStartX = rcCrop.left;
	int nStartY = rcCrop.top;
	int nEndX = rcCrop.right;
	int nEndY = rcCrop.bottom;

	BYTE* pCropPtr;
	CBitmapStd bmp( rcCrop.Width(), rcCrop.Height());
	pCropPtr = bmp.GetImgPtr();

	int nCropW = rcCrop.Width();
	int cx = 0;
	int cy = 0 ;
	//for (int y = nStartY, cy = 0; y < nEndY; y++, cy++) {
	for (int y = ( nEndY - 1 ), cy = 0; y > nStartY; y--, cy++) {
		BYTE* pLinePtr = pImgPtr + (nWidth * y);
		BYTE* pCropLinePtr = pCropPtr +(nCropW * cy);
		for (int x = nStartX, cx = 0 ; x < nEndX; x++, cx++ ) {
			*(pCropLinePtr + cx) = *(pLinePtr + x) ;
		}
	}

	CString strFileFullPath;
	strFileFullPath.Format(_T("%s%s"), strFilePath, strFileName);
	bmp.SetJpegQuality(100);
	bmp.SaveBitmap(strFileFullPath);
	
	return 0;;
}

int CImageProcess::EdgeDetectImageToBoth_CropArea(BYTE* pImgPtr, BYTE* pProcImg, int* pnResltArr, int nWidth, int nHeight, CRect rectProc, int nCompWidth, int nMagnif, int nDir)
{

	int x, y, i;
	int nSumCnt = nCompWidth;
	int nDiff = 0;
	int nXStart, nXEnd, nYStart, nYEnd;

	if (nMagnif < 1) {
		nMagnif = 1;
	}
	else if (nMagnif > 3) {
		nMagnif = 3;
	}

	int nCropHeight = rectProc.Height();
	int nCropWidth = rectProc.Width();
	memset(pProcImg, 0, sizeof(BYTE) * nCropHeight * nCropWidth);
	int cx, cy;

	if (nDir == DIR_VER) { // 세로 엣치 찾기
		memset(pnResltArr, 0, sizeof(int) * nHeight);

		int nSumLeft, nSumRight;
		nXStart = rectProc.left + nSumCnt;
		nXEnd = rectProc.right - nSumCnt;
		nYStart = rectProc.top;
		nYEnd = rectProc.bottom;

		for (y = nYStart, cy = 0; y < nYEnd; y++, cy++) {
			BYTE* pLinePtr = pImgPtr + (y * nWidth);
			BYTE* pProcLinePtr = pProcImg + (cy * nCropWidth);
			int nMax = 0;
			for (x = nXStart, cx = nSumCnt; x < nXEnd; x++, cx++) {
				nSumLeft = 0;
				nSumRight = 0;
				for (i = 0; i < nSumCnt; i++) {
					nSumLeft += *(pLinePtr + x - i);
					nSumRight += *(pLinePtr + x + i);
				}
				int nAveLeft = (nSumLeft / nSumCnt);
				int nAveRight = (nSumRight / nSumCnt);
				nDiff = abs(nAveLeft - nAveRight) * nMagnif;
				*(pProcLinePtr + cx) = (nDiff > 255 ? 255 : nDiff);
				if (nDiff > nMax) {
					nMax = nDiff;
					pnResltArr[cy] = cx;
					//	nArrMaxVal[y] = nDiff;
				}
			}
		}
	}
	else {// 가로 엣지 찾기
		int nSumUpper, nSumLower;
		nXStart = rectProc.left;
		nXEnd = rectProc.right;
		nYStart = rectProc.top + nSumCnt;
		nYEnd = rectProc.bottom - nSumCnt;
		memset(pnResltArr, 0, sizeof(int) * nWidth);

		for (x = nXStart, cx = 0; x < nXEnd; x++, cx++) {
			int nMax = 0;
			for (y = nYStart, cy = nSumCnt; y < nYEnd; y++, cy++) {
				nSumUpper = 0;
				nSumLower = 0;
				for (i = 0; i < nSumCnt; i++) {
					nSumUpper += *(pImgPtr + ((y - i) * nWidth) + x);
					nSumLower += *(pImgPtr + ((y + i) * nWidth) + x);
				}
				int nAveUpper = (nSumUpper / nSumCnt);
				int nAveLower = (nSumLower / nSumCnt);
				nDiff = abs(nAveUpper - nAveLower) * nMagnif;
				*(pProcImg + (cy * nCropWidth) + cx) = (nDiff > 255 ? 255 : nDiff);
				if (nDiff > nMax) {
					nMax = nDiff;
					pnResltArr[cy] = cx;
				}
			}
		}
	}
	return 0;
}

// 2점과 반지름으로 원의 중심을 계산하는 함수.
int CImageProcess::CalcCirclePos(CPoint point1, CPoint point2, int nRadius, CPoint* pntCenter, int nDir)// nDir 1사분면 1, 2사분면 -1 
{
	double dPi = 3.14195;
	double r = nRadius;

	double dcx, dcy, dlx, dly;
	dlx = (double)(point2.x - point1.x);
	dly = (double)(point2.y - point1.y);
	dcx = (double)(dlx) / 2.0 + (double)point1.x;
	dcy = (double)(dly) / 2.0 + (double)point1.y;

	double dl = sqrt(pow(dlx, 2) + pow(dly, 2));
	double dh = sqrt(pow(r, 2) - pow((dl / 2), 2));

	double dAngle = (point2.y - point1.y) / dl;

	double radian = asin(dAngle);
	double degree = radian / dPi * 180.0;

	double dRotate = degree + 270.0;
	radian = (double)(dRotate / 180.0) * dPi;

	double dNewCx, dNewCy;
	double dTempY, dTempX;
	dTempX = dcx;
	dTempY = dcy - dh;

	dNewCx = dh * cos(radian) + dcx;
	dNewCy = (dh * sin(radian) * nDir) + dcy;

	pntCenter->x = (int)(dNewCx + 0.5);
	pntCenter->y = (int)(dNewCy + 0.5);

	return 0;
}

// 3점으로 원의 중심점과 원은 반지름을 계산하는 함수
int CImageProcess::CalcCirclePosFrom3Point(CPoint p1, CPoint p2, CPoint p3, int nDir, CPoint* pntCenter, int* pnRadius)
{
	int nRet = 0;

	double p1x = (double)p1.x;
	double p1y = (double)p1.y;
	double p2x = (double)p2.x;
	double p2y = (double)p2.y;
	double p3x = (double)p3.x;
	double p3y = (double)p3.y;

	double dm1 = (p2x - p1x) / (p2y - p1y);
	double dm2 = (p3x - p2x) / (p3y - p2y);

	double cx = (((p3y - p1y) + ((p2x + p3x) * dm2)) - (p1x + p2x) * dm1) / (2.0 * (dm2 - dm1));
	double cy = -dm1 * (cx - (p1x + p2x) / 2) + (p1y + p2y) / 2;

	double dR = sqrt(pow((p1x - cx), 2) + pow((p1y - cy), 2));
	pntCenter->x = (LONG)cx;
	pntCenter->y = (LONG)cy;
	*pnRadius = (int)dR;

	return nRet;
}

double CImageProcess::CalcRoundDef_Size(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY)
{
	//defInfo.rcPos.left;
	double dRet = 0.0;
	//CImageProcess::CalcCirclePos( ptFirst, ptSecond,  )

	// 먼저 결함 센터와 일치하는 라운드 x, y 좌표를 찾아서 기울기를 구함.
	// y를 먼저 찾고 거기서 부터 x 를 찾는게 빠름.
	CPoint cpDefCenter;
	// 22.07.20 Ahn Modify Start
	//CRect rcDefect( pBlockData->rcRect.left, pBlockData->rcRect.top, pBlockData->rcRect.right, pBlockData->rcRect.bottom );
	CRect rcDefect = pBlockData->rcRect;
	// 22.07.20 Ahn Modify End
	cpDefCenter.x = (pBlockData->rcRect.left + pBlockData->rcRect.right) / 2;
	cpDefCenter.y = (pBlockData->rcRect.top + pBlockData->rcRect.bottom) / 2;

	// 센터 좌표에서 가장 가까운 Round 지점을 찾음.
	int nSize = (int)pVecRndInfo->size() - 1;
	double dDist, dDistMin = 9999.9;
	int nMinPos = nSize - 1;
	int nEndPos = nSize - rcRnd.Width();
	if (nEndPos < 0) nEndPos = 0;

	if (nPosition == CImageProcess::en_FindLeft) {
		for (int i = nSize ; i > nEndPos; i--) {
			int nDx = (cpDefCenter.x - (*pVecRndInfo)[i].x);
			int nDy = (cpDefCenter.y - (*pVecRndInfo)[i].y);
			dDist = sqrt((nDx * nDx) + (nDy * nDy));

			if (dDistMin >= dDist) {
				dDistMin = dDist;
				nMinPos = i;
			}
		}
	}
	else {
		for (int i = 0; i < rcRnd.Width(); i++) {
			int nDx = (cpDefCenter.x - (*pVecRndInfo)[i].x);
			int nDy = (cpDefCenter.y - (*pVecRndInfo)[i].y);
			dDist = sqrt((nDx * nDx) + (nDy * nDy));

			if (dDistMin >= dDist) {
				dDistMin = dDist;
				nMinPos = i;
			}
		}
	}

	int nStartPos;	
	double dDeltaX;
	double dDeltaY;
	double dIntcptY = 0.0;
	double a, b, c;
	double dSizeH, dSizeV;
	CPoint cpDistMin;
	{
		nStartPos = nMinPos - 10 ;
		nEndPos = nMinPos + 10 ;

		if (nStartPos < 0) nStartPos = 0;
		if (nEndPos > nSize) nEndPos = nSize;
		int x1, x2, y1, y2;
		x2 = (*pVecRndInfo)[nStartPos].x;
		y2 = (*pVecRndInfo)[nStartPos].y;
		x1 = (*pVecRndInfo)[nEndPos].x;
		y1 = (*pVecRndInfo)[nEndPos].y;
		dDeltaX = x2 - x1;
		dDeltaY = y2 - y1;
		cpDistMin = (*pVecRndInfo)[nMinPos];

		a = dDeltaY;
		b = dDeltaX;
		c = ((a * cpDistMin.x) + (b * cpDistMin.y)) * -1.0;

		double dMaxDistH = 0.0 , dMinDistH = 999.0;
		double dMaxDistV = 0.0, dMinDistV = 999.0;
		double dDist;
		// 결함의 중심에서 부터 x방향 y 방향으로 1씩 증가하며 
		for (int x = pBlockData->rcRect.left; x <= pBlockData->rcRect.right; x++) {
			if (*(pImgPtr + (nWidth * cpDefCenter.y) + x) != 128)continue;
			dDist = abs((a * x) + (b * cpDefCenter.y) + c) / sqrt(pow(a, 2) + pow(b, 2));
			if (dDist >= dMaxDistH) {
				dMaxDistH = dDist;
			}
			if (dDist <= dMinDistH) {
				dMinDistH = dDist;
			}
		}
		dSizeH = (dMaxDistH - dMinDistH + 1) * dResoX;

		for (int y = pBlockData->rcRect.top; y <= pBlockData->rcRect.bottom; y++) {
			if (*(pImgPtr + (nWidth * y) + cpDefCenter.x) != 128)continue;
			dDist = abs((a * cpDefCenter.x) + (b * y) + c) / sqrt(pow(a, 2) + pow(b, 2));
			if (dDist >= dMaxDistV) {
				dMaxDistV = dDist;
			}
			if (dDist <= dMinDistV) {
				dMinDistV = dDist;
			}
		}
		dSizeV = (dMaxDistV - dMinDistV + 1 ) * dResoX;
		pBlockData->dWidth = (dSizeH > dSizeV )?dSizeH:dSizeV;
	}

	return dRet ;
}

double CImageProcess::CalcRoundDef_Size_LeftFoilExp(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType)
{
	double dRet = 0.0;
	// 22.07.20 Ahn Modify Start
	//CRect rcDefect(pBlockData->rcRect.left, pBlockData->rcRect.top, pBlockData->rcRect.right, pBlockData->rcRect.bottom);
	CRect rcDefect = pBlockData->rcRect;
	// 22.07.20 Ahn Modify End

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;
	nStartX = rcDefect.left;
	nEndX = rcDefect.right+1;
	nStartY = rcDefect.top;
	nEndY = rcDefect.bottom+1;

	// 23.01.13 Ahn Modify Start
	int nOffsetY = (*pVecRndInfo)[0].y ;
	if ((*pVecRndInfo)[0].y > 0) {
		nStartY = rcDefect.top - nOffsetY;
		nEndY = rcDefect.bottom + 1 - nOffsetY ;
	}
	// 23.01.13 Ahn Modify End


	int nMaxSizeY = (int)pVecRndInfo->size();
	if (nEndY >= nMaxSizeY) {
		nEndY = nMaxSizeY;
	}

	BYTE bPtr;
	BYTE* pLinePtr;
	CImageProcess::VEC_ROUND_INFO vecLine;

	// 바깥쪽 외접점 수집
	// 22.02.17 Ahn Add Start
	if (nDefType == TYPE_FOILEXP_OUT) {
		// Dross 
		for (y = nStartY; y < nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffsetY) );
			nStartX = (*pVecRndInfo)[y].x;
			for (x = nEndX; x >= nStartX; x--) {
				bPtr = *(pLinePtr + x);
				if (bPtr > 0) {
					CPoint cp;
					cp.x = x;
					cp.y = y;
					vecLine.push_back(cp);
					break;
				}
			}
		}
	}
	else {
		// 22.02.17 Ahn Add End
		for (y = nStartY; y < nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffsetY));
			nEndX = (*pVecRndInfo)[y].x;
			for (x = nStartX; x < nEndX; x++) {
				bPtr = *(pLinePtr + x);
				if (bPtr > 0) {
					CPoint cp;
					cp.x = x;
					cp.y = y;
					vecLine.push_back(cp);
					break;
				}
			}
		}
	}
	// 23.01.13 Ahn Modify Start
	//double dMaxDist = CalcRoundDef_Distance( en_FindLeft, pBlockData, 0, rcRnd, rcDefect, vecLine, pVecRndInfo, dResoX, dResoY);
	double dMaxDist = CalcRoundDef_Distance(en_FindLeft, pBlockData, nOffsetY, rcRnd, rcDefect, vecLine, pVecRndInfo, dResoX, dResoY);
	// 23.01.13 Ahn Modify End
	pBlockData->dWidth = pBlockData->dJudgeSize = dMaxDist;

	// 22.04.15 Ahn Add Start
	CImageProcess::VEC_ROUND_INFO vecInLine;
	vecInLine.clear();
	BYTE bPtrR;
	if (nDefType == TYPE_FOILEXP) {
		for (y = nStartY; y < nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffsetY));
			nEndX = (*pVecRndInfo)[y].x;
			for (x = nStartX; x < nEndX; x++) {
				bPtr = *(pLinePtr + x);
				bPtrR = *(pLinePtr + x + 1);
				if ((bPtr > 0) && (bPtrR == 0)) {
					CPoint cp;
					cp.x = x;
					cp.y = y;
					vecInLine.push_back(cp);
					break;
				}
			}
		}
		// 23.01.13 Ahn Modify Start
		//double dDistance = CalcRoundDef_Distance_FindMin(en_FindLeft, pBlockData, 0, rcRnd, rcDefect, vecInLine, pVecRndInfo, dResoX, dResoY);
		double dDistance = CalcRoundDef_Distance_FindMin(en_FindLeft, pBlockData, nOffsetY, rcRnd, rcDefect, vecInLine, pVecRndInfo, dResoX, dResoY);
		// 23.01.13 Ahn Modify End
		pBlockData->dDistance = dDistance - (dResoX);
	}
	// 22.04.15 Ahn Add End

	return dRet;
}

// 22.08.22 Ahn Add Start
double CImageProcess::CalcRoundDef_FindMin(int nMode, CBlockData* defInfo, double dSlope, int nOffset, CRect rcRnd, CRect rcDefect, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, double dResoY)
{
	double dRet = 0; 

	// 기울기
	if (nMode == en_FindLeft) {

	}
	else {

	}

	return dRet;
}

double CImageProcess::CalcInclination_FromRoundInfo(CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo)
{
	ASSERT(pBlockData);
	ASSERT(pVecRndInfo);
	double dRet = 0.0;
	CRect rcDefect = pBlockData->rcRect;
	CPoint cpDefCenter;
	cpDefCenter = rcDefect.CenterPoint();

	// Round가 아니면 Pass
	if ((pBlockData->nDefPos & 0x01) != 0x01) {
		return dRet;
	}

	// 기울기 계산 (기울기 계산시에 가로세로 비율도 포함해야함) 
	// 
	CImageProcess::VEC_ROUND_INFO::iterator iter;

	int nSize = (int)pVecRndInfo->size();
	if (nSize <= 0) return 0;
	int nMinDist = 99999;
	BOOL bFound = FALSE;
	CPoint cpMinPos;

	for (int i = nSize - 1; i < 0; i--) {
		if (iter->y < rcRnd.top) break;
		int nDistX = (*pVecRndInfo)[i].x - cpDefCenter.x ;
		int nDistY = (*pVecRndInfo)[i].y - cpDefCenter.y;
		int nDist = (int)sqrt(pow(nDistX, 2) + pow(nDistY, 2));

		if (nMinDist >= nDist) {
			cpMinPos.x = iter->x;
			cpMinPos.y = iter->y;
			nMinDist = nDist;
			bFound = TRUE;
		}
	}
	// 기울기 




	return dRet;
}

double CImageProcess::CalcRoundDef_SizeLeftOnlyBright(BYTE* pImgPtr, int nWidth, int nHeight, CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType)
{
	ASSERT(pBlockData);
	ASSERT(pImgPtr);
	ASSERT(pVecRndInfo);
	double dRet = 0.0;
	CRect rcDefect = pBlockData->rcRect;

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;

	nStartX = rcDefect.left;
	nStartY = rcDefect.top;
	nEndX = rcDefect.right + 1;
	nEndY = rcDefect.bottom + 1;

	CPoint cpDefCenter;
	cpDefCenter = rcDefect.CenterPoint();

	int nMaxSizeY = (int)pVecRndInfo->size();
	if (nEndY >= nMaxSizeY) {
		nEndY = nMaxSizeY;
	}

	BYTE bPtr;
	BYTE* pLinePtr;
	CImageProcess::VEC_ROUND_INFO vecLine;

	for (y = nStartY; y < nEndY; y++) { 
		pLinePtr = pImgPtr + (nWidth * y);
		nEndX = (*pVecRndInfo)[y].x;
		for (x = nStartX; x < nEndX; x++) {
			bPtr = *(pLinePtr + x);
			if (bPtr > 0) {
				CPoint cp;
				cp.x = x;
				cp.y = y;
				vecLine.push_back(cp);
				break;
			}
		}
	}


	double dInclination = CalcInclination_FromRoundInfo(rcRnd, pBlockData, pVecRndInfo);
	
	return dRet;
}
double CImageProcess::CalcRoundDef_SizeRightOnlyBright(BYTE* pImgPtr, int nWidth, int nHeight, CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType)
{
	double dRet = 0.0;
	CRect rcDefect = pBlockData->rcRect;

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;
	int nOffset = (*pVecRndInfo)[0].y;
	nStartX = rcDefect.left;
	nStartY = rcDefect.top - nOffset;
	nEndX = rcDefect.right;
	nEndY = rcDefect.bottom - nOffset;

	BYTE bPtr;
	BYTE* pLinePtr;
	CImageProcess::VEC_ROUND_INFO vecLine;

	// 바깥쪽 외접점 수집
	// 22.02.17 Ahn Add Start
	if (nDefType == TYPE_FOILEXP_OUT) {
		for (y = nStartY; y <= nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffset));
			nStartX = (*pVecRndInfo)[y].x;
			for (x = nEndX; x >= nStartX; x--) {
				bPtr = *(pLinePtr + x);
				if (bPtr > 0) {
					CPoint cp;
					cp.x = x;
					cp.y = y;
					vecLine.push_back(cp);
					break;
				}
			}
		}
	}
	else {
		// 22.02.17 Ahn Add Start
		for (y = nStartY; y <= nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffset));
			nEndX = (*pVecRndInfo)[y].x;
			for (x = nStartX; x < nEndX; x++) {
				bPtr = *(pLinePtr + x);
				if (bPtr > 0) {
					CPoint cp;
					cp.x = x;
					cp.y = y;
					vecLine.push_back(cp);
					break;
				}
			}
		}
	}

	return dRet;
}
// 22.08.22 Ahn Add End



// 22.04.15 Ahn Add Start
double CImageProcess::CalcRoundDef_Distance( int nMode, CBlockData* defInfo, int nOffset, CRect rcRnd, CRect rcDefect, CImageProcess::VEC_ROUND_INFO vecLine, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, double dResoY)
{
	double dRet = 0.0;
	double dMaxDist = 0.0;
	double dDist;
	int nSize = (int)vecLine.size();
	int nMaxSize = (int)pVecRndInfo->size();
	double dDistX, dDistY;
	double dMinDist = 9999999.0;
	int nStartX, nStartY, nEndX, nEndY;
	int nDefType = defInfo->nType ;

	nStartX = rcDefect.left;
	nStartY = rcDefect.top - nOffset;
	nEndX = rcDefect.right;
	nEndY = rcDefect.bottom - nOffset;

	int nMinPosY = 0;
	if (nStartY < 0) {
		nMinPosY = (*pVecRndInfo)[0].y;
	}
	else {
		nMinPosY = (*pVecRndInfo)[nStartY].y;
	}

	int py;
	int px;
	int nGrowCnt = 0;
	if (nDefType == TYPE_FOILEXP_OUT) {
		if (nMode == en_FindLeft) {
			// 22.04.26 Ahn Modify Start
			nEndY = rcRnd.bottom ;
			if (nEndY > nMaxSize  ) {
				nEndY = nMaxSize;
			}
			// 22.04.26 Ahn Modify End
			for (int i = 0; i < nSize; i++) {
				py = vecLine[i].y;
				px = vecLine[i].x;
				dMinDist = 99999999.0;
				BOOL bMinFind = FALSE;

				int j = 0;
				//for (int y = py, j = py + nOffset; j > rcRnd.top; y--, j--) {
				// 22.04.26 Ahn Modify Start
				//for (int y = py, j = py + nOffset; j < rcRnd.bottom; y++) {
				for (int y = py ; y < nEndY ; y++) {
				// 22.04.26 Ahn Modify End
					dDistX = ((*pVecRndInfo)[y].x - px) * dResoX;
					dDistY = ((*pVecRndInfo)[y].y - (py + nOffset)) * dResoY;
					dDist = sqrt((dDistX * dDistX) + (dDistY * dDistY));
					if (dMinDist > dDist) {
						dMinDist = dDist;
						nMinPosY = y;
						nGrowCnt = 0;
						bMinFind = TRUE;
					}
					else {
						nGrowCnt++;
					}
					if (nGrowCnt > 3) {
						break;
					}
				}
				if ((dMaxDist < dMinDist) && (bMinFind == TRUE)) {
					dMaxDist = dMinDist;
				}
			}
		}
		else {
			// 22.04.26 Ahn Modify Start
			//nStartY = rcRnd.top;
			nStartY = (*pVecRndInfo)[0].y;
			if (nStartY < 0 ) {
				nStartY = 0 ;
			}
			// 22.04.26 Ahn Modify End
			for (int i = 0; i < nSize; i++) {
				py = vecLine[i].y;
				px = vecLine[i].x;
				dMinDist = 99999999.0;
				BOOL bMinFind = FALSE;

				int j = 0;
				// 22.04.26 Ahn Modify End
				//for (int y = py, j = py + nOffset; j > rcRnd.top; y--, j--) {
				for (int y = py, j = py + nOffset; j > nStartY; y--, j--) {
				//for (int y = py ; y > nStartY ; y--, j--) {
				// 22.04.26 Ahn Modify End
					dDistX = ((*pVecRndInfo)[y].x - px) * dResoX;
					dDistY = ((*pVecRndInfo)[y].y - (py + nOffset)) * dResoY;
					dDist = sqrt((dDistX * dDistX) + (dDistY * dDistY));
					if (dMinDist > dDist) {
						dMinDist = dDist;
						nMinPosY = y;
						nGrowCnt = 0;
						bMinFind = TRUE;
					}
					else {
						nGrowCnt++;
					}
					if (nGrowCnt > 3) {
						break;
					}
				}
				if ((dMaxDist < dMinDist) && (bMinFind == TRUE)) {
					dMaxDist = dMinDist;
				}
			}
		}
	}
	else
	{
		if (nMode == en_FindLeft) {
			// 22.04.26 Ahn Modify Start
			nStartY = rcRnd.top;
			if (nStartY < 0) {
				nStartY = 0;
			}
			// 22.04.26 Ahn Modify End
			for (int i = nSize - 1; i >= 0; i--) {
				py = vecLine[i].y;
				px = vecLine[i].x;
				if (py > nMaxSize) {
					py = nMaxSize;
				}
				dMinDist = 9999999.0;
				BOOL bFindMin = FALSE; // 22.04.08 Ahn Add 
				// 22.04.26 Ahn Modify End
				//for (int y = py; y > rcRnd.top; y--) {
				// 23.01.13 Ahn Modify Start
				//for (int y = py; y > nStartY ; y--) {
				for (int y = py, j = py + nOffset; j > nStartY; y--, j--) {
				// 23.01.13 Ahn Modify End
				// 22.04.26 Ahn Modify End
					dDistX = ((*pVecRndInfo)[y].x - px) * dResoX;
					dDistY = ((*pVecRndInfo)[y].y - (py + nOffset)) * dResoY;
					dDist = sqrt((dDistX * dDistX) + (dDistY * dDistY));
					if (dMinDist > dDist) {
						dMinDist = dDist;
						nMinPosY = y;
						nGrowCnt = 0;
						bFindMin = TRUE;	// 22.04.08 Ahn Add
					}
					else {
						nGrowCnt++;
					}
					if ((nGrowCnt > 3)) {
						break;
					}
				}
				if( (dMaxDist < dMinDist) && (bFindMin == TRUE)){
					dMaxDist = dMinDist;
				}
			}
		}
		else {
			for (int i = 0; i < nSize; i++) {
				py = vecLine[i].y;
				px = vecLine[i].x;
				dMinDist = 99999999.0;
				BOOL bMinFind = FALSE; 

				for (int y = py; y < nMaxSize; y++) {
					dDistX = ((*pVecRndInfo)[y].x - px) * dResoX;
					dDistY = ((*pVecRndInfo)[y].y - (py + nOffset)) * dResoY;
					dDist = sqrt((dDistX * dDistX) + (dDistY * dDistY));
					if (dMinDist > dDist) {
						dMinDist = dDist;
						nMinPosY = y;
						nGrowCnt = 0;
						bMinFind = TRUE;
					}
					else {
						nGrowCnt++;
					}
					if (nGrowCnt > 3) {
						break;
					}
				}
				if ((dMaxDist < dMinDist) && (bMinFind == TRUE)) {
					dMaxDist = dMinDist;
				}
			}
		}
	}

	dRet = dMaxDist;

	return dRet;
}

double CImageProcess::CalcRoundDef_Distance_FindMin(int nMode, CBlockData* defInfo, int nOffset, CRect rcRnd, CRect rcDefect, CImageProcess::VEC_ROUND_INFO vecLine, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, double dResoY)
{
	double dRet = 0.0;
	double dMaxMin = 9999999.0;
	double dDist;
	int nSize = (int)vecLine.size();
	int nMaxSize = (int)pVecRndInfo->size();
	double dDistX, dDistY;
	double dMinDist = 9999999.0;
	int nStartX, nStartY, nEndX, nEndY;
	int nDefType = defInfo->nType;

	nStartX = rcDefect.left;
	nStartY = rcDefect.top - nOffset;
	nEndX = rcDefect.right;
	nEndY = rcDefect.bottom - nOffset;

	int nMinPosY = 0;
	if (nStartY < 0) {
		nMinPosY = (*pVecRndInfo)[0].y;
	}
	else {
		nMinPosY = (*pVecRndInfo)[nStartY].y;
	}

	int py;
	int px;
	int nGrowCnt = 0;
	if (nDefType == TYPE_FOILEXP_OUT) {
		for (int i = 0; i < nSize; i++) {
			py = vecLine[i].y;
			px = vecLine[i].x;
			dMinDist = 99999999.0;
			BOOL bMinFind = FALSE;

			int j = 0;
			for (int y = py, j = py + nOffset; j > rcRnd.top; y--, j--) {
				dDistX = ((*pVecRndInfo)[y].x - px) * dResoX;
				dDistY = ((*pVecRndInfo)[y].y - (py + nOffset)) * dResoY;
				dDist = sqrt((dDistX * dDistX) + (dDistY * dDistY));
				if (dMinDist > dDist) {
					dMinDist = dDist;
					nMinPosY = y;
					nGrowCnt = 0;
					bMinFind = TRUE;
				}
				else {
					nGrowCnt++;
				}
				if (nGrowCnt > 3) {
					break;
				}
			}
			if ((dMaxMin > dMinDist) && (bMinFind == TRUE)) {
				dMaxMin = dMinDist;
			}
		}
	}
	else
	{
		if (nMode == en_FindLeft) {
			for (int i = nSize - 1; i >= 0; i--) {
				py = vecLine[i].y;
				px = vecLine[i].x;
				if (py > nMaxSize) {
					py = nMaxSize;
				}
				dMinDist = 9999999.0;
				BOOL bFindMin = FALSE; // 22.04.08 Ahn Add 
				//for (int y = py; y > rcRnd.top; y--) {
				for (int y = py, j = py + nOffset; j > rcRnd.top; y--, j--) {
					dDistX = ((*pVecRndInfo)[y].x - px) * dResoX;
					dDistY = ((*pVecRndInfo)[y].y - (py + nOffset)) * dResoY;
					dDist = sqrt((dDistX * dDistX) + (dDistY * dDistY));
					if (dMinDist > dDist) {
						dMinDist = dDist;
						nMinPosY = y;
						nGrowCnt = 0;
						bFindMin = TRUE;	// 22.04.08 Ahn Add
					}
					else {
						nGrowCnt++;
					}
					if ((nGrowCnt > 3)) {
						break;
					}
				}
				if ((dMaxMin > dMinDist) && (bFindMin == TRUE)) {
					dMaxMin = dMinDist;
				}
			}
		}
		else {
			for (int i = 0; i < nSize; i++) {
				py = vecLine[i].y;
				px = vecLine[i].x;
				dMinDist = 99999999.0;
				BOOL bMinFind = FALSE;

				for (int y = py; y < nMaxSize; y++) {
					dDistX = ((*pVecRndInfo)[y].x - px) * dResoX;
					dDistY = ((*pVecRndInfo)[y].y - (py + nOffset)) * dResoY;
					dDist = sqrt((dDistX * dDistX) + (dDistY * dDistY));
					if (dMinDist > dDist) {
						dMinDist = dDist;
						nMinPosY = y;
						nGrowCnt = 0;
						bMinFind = TRUE;
					}
					else {
						nGrowCnt++;
					}
					if (nGrowCnt > 3) {
						break;
					}
				}
				if ((dMaxMin > dMinDist) && (bMinFind == TRUE)) {
					dMaxMin = dMinDist;
				}
			}
		}
	}

	dRet = dMaxMin;

	return dRet;
}
// 22.04.15 Ahn Add End

double CImageProcess::CalcRoundDef_Size_RightFoilExp(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CRect rcRnd, CBlockData* pBlockData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nPosition, double dResoX, double dResoY, int nDefType)
{
	double dRet = 0.0;
	// 22.07.20 Ahn Modify Start
	//CRect rcDefect(pBlockData->rcRect.left, pBlockData->rcRect.top, pBlockData->nRight, pBlockData->nBottom);
	CRect rcDefect = pBlockData->rcRect;
	// 22.07.20 Ahn Modify End

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;
	int nOffset = (*pVecRndInfo)[0].y ;
	nStartX = rcDefect.left ;
	nStartY = rcDefect.top - nOffset;
	nEndX = rcDefect.right;
	nEndY = rcDefect.bottom - nOffset;

	BYTE bPtr;
	BYTE* pLinePtr;
	CImageProcess::VEC_ROUND_INFO vecLine;

	// 바깥쪽 외접점 수집
	// 22.02.17 Ahn Add Start
	if (nDefType == TYPE_FOILEXP_OUT) {
		for (y = nStartY; y <= nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffset));
			nStartX = (*pVecRndInfo)[y].x;
			for (x = nEndX; x >= nStartX; x--) {
				bPtr = *(pLinePtr + x);
				if (bPtr > 0) {
					CPoint cp;
					cp.x = x;
					cp.y = y;
					vecLine.push_back(cp);
					break;
				}
			}
		}
	}
	else {
	// 22.02.17 Ahn Add Start
		for (y = nStartY; y <= nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffset));
			nEndX = (*pVecRndInfo)[y].x;
			for (x = nStartX; x < nEndX; x++) {
				bPtr = *(pLinePtr + x);
				if (bPtr > 0) {
					CPoint cp;
					cp.x = x;
					cp.y = y ;
					vecLine.push_back(cp);
					break;
				}
			}
		}
	}

	double dMaxDist = CalcRoundDef_Distance( en_FindRight, pBlockData, nOffset, rcRnd, rcDefect, vecLine, pVecRndInfo, dResoX, dResoY);
	pBlockData->dWidth = pBlockData->dJudgeSize = dMaxDist;

	// 22.04.15 Ahn Add Start
	CImageProcess::VEC_ROUND_INFO vecInLine;
	vecInLine.clear();
	BYTE bPtrR;
	if (nDefType == TYPE_FOILEXP) {
		for (y = nStartY; y < nEndY; y++) {
			pLinePtr = pImgPtr + (nWidth * (y + nOffset)) ;
			nEndX = (*pVecRndInfo)[y].x;
			for (x = nStartX; x < nEndX; x++) {
				bPtr = *(pLinePtr + x);
				bPtrR = *(pLinePtr + x + 1);
				if ((bPtr > 0) && (bPtrR == 0)) {
					CPoint cp;
					cp.x = x;
					cp.y = y;
					vecInLine.push_back(cp);
					break;
				}
			}
		}
		double dDistance = CalcRoundDef_Distance_FindMin(en_FindRight, pBlockData, nOffset, rcRnd, rcDefect, vecInLine, pVecRndInfo, dResoX, dResoY);
		pBlockData->dDistance = dDistance - (dResoX);
	}
	// 22.04.15 Ahn Add End

	return dRet;
}


// 결함 Size를 Edge에서 부터 측정함.
int CImageProcess::CalcSizeToEdgeLine(CBlockData* pData, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dResoX, int nMode )
{
	ASSERT(pData);
	ASSERT(pVecRndInfo);
	int nRet = 0;
	int nMaxPos = 3;
	int nStartPos = (*pVecRndInfo)[0].y  ;
	int nBoundarySize = (int)(*pVecRndInfo).size();

	int nCenterY = ( (pData->rcRect.bottom + pData->rcRect.top) / 2 ) ;
	int nCenterPos = nCenterY - nStartPos;
	//if ( nCenterPos >= nBoundarySize){ // 22.06.27 Ahn Modify
	if ((nCenterPos >= nBoundarySize) || (nCenterPos < 0)) { // 22.06.27 Ahn Modify
			return -1;
	}

	int nSize = 0;
	int nDistance = 0;
	// 22.07.20 Ahn Add Start
	//if (pData->nType == en_ModeFoilBoth) {
	//	nSize = pData->nWidth;
	//}else {
	// }
	// 22.07.20 Ahn Add End
	if (nMode == CAM_POS_TOP) {
			// 21.12.21 Ahn Add Start
		if (pData->nType == en_ModeFoilExpOut) {
			nSize = pData->rcRect.right - (*pVecRndInfo)[nCenterPos].x + 1;
			// 22.02.23 Ahn Add Start
			nDistance = pData->rcRect.left - (*pVecRndInfo)[nCenterPos].x + 1;
			// 22.02.23 Ahn Add End
		}
		else {
			nSize = (*pVecRndInfo)[nCenterPos].x - pData->rcRect.left + 1;
			// 22.02.23 Ahn Add Start
			nDistance = (*pVecRndInfo)[nCenterPos].x - pData->rcRect.right + 1;
			// 22.02.23 Ahn Add End
		}
		// 21.12.21 Ahn Add End
	}
	else {
		// 21.12.21 Ahn Add Start
		if (pData->nType == en_ModeFoilExpOut) {
			nSize = (*pVecRndInfo)[nCenterPos].x - pData->rcRect.left + 1;
			// 22.02.23 Ahn Add Start
			nDistance = (*pVecRndInfo)[nCenterPos].x - pData->rcRect.right + 1;
			// 22.02.23 Ahn Add End
		}
		else {
			nSize = pData->rcRect.right - (*pVecRndInfo)[nCenterPos].x + 1;
			// 22.02.23 Ahn Add Start
			nDistance = pData->rcRect.left - (*pVecRndInfo)[nCenterPos].x + 1;
			// 22.02.23 Ahn Add End
		}
		// 21.12.21 Ahn Add End
	}
	pData->dJudgeSize = pData->dWidth;
	pData->dDistance = ( nDistance * AprData.m_System.m_dResolX[ CAM_POS_TOP ] ); // 22.02.23 Ahn Add 

	if (nSize > 0) {
		int nMaxSize = nSize;
		if (pData->nHeight > 3) {
		}

		double dRetSize = (nMaxSize * dResoX);
		pData->dJudgeSize = dRetSize ;
	}

	return nRet ;
}

int CImageProcess::CheckDistanceToEdgeLine(CBlockData* pBlock, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, double dDistance, double dResoX )
{
	int nRet = 0;
	ASSERT(pVecRndInfo);
	
	int nSize = (int)pVecRndInfo->size();
	if (nSize <= 0) return -1;

	double dDistL, dDistR ;
	int nCenterY = ( pBlock->rcRect.bottom + pBlock->rcRect.top ) / 2;
	int nCenterX = (pBlock->rcRect.left + pBlock->rcRect.right) / 2;
	int nStartY = (*pVecRndInfo)[0].y;

	if (nCenterY > ( nSize + nStartY ) ) return -2;

	int nLeft = pBlock->rcRect.top - nStartY;
	int nRight = pBlock->rcRect.bottom - nStartY;
	if (nLeft > nSize || nRight > nSize) {
		return -1;
	}

	int nMin = (*pVecRndInfo)[nLeft].x;
	int nMax = (*pVecRndInfo)[nRight].x;
	if (nMin > nMax) {
		SWAP(int, nMin, nMax);
	}

	dDistL = (double)abs(nMin - pBlock->rcRect.left) * dResoX;
	double dDistLx = (double)abs(nMax - pBlock->rcRect.left) * dResoX;
	dDistR = (double)abs(nMin - pBlock->rcRect.right) * dResoX;
	double dDistRx = (double)abs(nMax - pBlock->rcRect.right) * dResoX;
	// 22.07.26 Ahn Modify Start
	if( (dDistance > 0 ) && (pBlock->nType == en_ModeFoilExp ) )
	{
		if ((dDistL > dDistance) && (dDistR > dDistance) && (dDistLx > dDistance) && (dDistRx > dDistance))
		{
			pBlock->bDeleteFlag = TRUE;
		}
	}
	// 22.07.26 Ahn Modify Start
	return nRet;
}

double CImageProcess::CalcDefectMaxSize(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CBlockData* pBlock, double dResoX, double dResoY)
{
	ASSERT(pBlock);

	int nLength = pBlock->nHeight; 
	int nSizeW = pBlock->nWidth;
	if (nSizeW >= 10) { // 측정 대상
		
	}

	// 오른쪽에서 왼쪽으로 검색
	int nPitch = 1 ;
	int x, y;
	BYTE* pLinePtr;
	int nRightPos = pBlock->rcRect.left;
	int nLeftPos = pBlock->rcRect.right;
	int nDefSize = 0;
	int nMaxSize = 0;
	int nMaxPos = 0;

	for (y = pBlock->rcRect.top; y <= pBlock->rcRect.bottom; y+= nPitch) {
		// Find Right Pos 
		pLinePtr = pImgPtr + (nWidth * y);
		nRightPos = pBlock->rcRect.right;
		for (x = pBlock->rcRect.right; x >= pBlock->rcRect.left; x--) {
			if (*(pLinePtr + x) != 0) {
				nRightPos = x ;
				break;
			}
		}

		nLeftPos = pBlock->rcRect.left;
		for (x = nRightPos-1; x >= pBlock->rcRect.left-1; x--) {
			if (*(pLinePtr + x) == 0) {
				nLeftPos = x;
				break;
			}
		}
		nDefSize = nRightPos - nLeftPos ;
		if (nDefSize > nMaxSize) {
			nMaxSize = nDefSize;
			nMaxPos = y;
		}

	}
	pBlock->dJudgeSize = (nMaxSize * dResoX);
	pBlock->nMaxPosY = nMaxPos;

	return 0;
}

double CImageProcess::CalcDefectMaxSize_Round(BYTE* pImgPtr, int nWidth, int nHeight, int nType, CBlockData* pBlock, CRect rcRect, double dResoX, double dResoY, int nPosition)
{
	double dRet = 0.0;

	ASSERT(pBlock);
	if (pBlock == NULL) return -1.0;

	CRect rcDefArea;
	rcDefArea.top = pBlock->rcRect.top;
	rcDefArea.bottom = pBlock->rcRect.bottom;
	rcDefArea.left = pBlock->rcRect.left;
	rcDefArea.right = pBlock->rcRect.right;

	int x, y ;
	int nPitch = 1; // 1  or 2
	
	CPoint cpLeft;
	CPoint cpRight;

	double dMaxSize = 0.0;
	CPoint cpStart;
	CPoint cpEnd;

	if (nPosition == CImageProcess::en_FindLeft) {
		double dSlopeRate = AprData.m_System.m_dResolY / AprData.m_System.m_dResolX[CAM_POS_TOP];
		double dSlopeRateR = -1.0 / dSlopeRate;

		int nMaxCount = 0;
		double dyp;
		for (x = rcDefArea.left; x < rcDefArea.right; x++) {
			int nCount = 0;
			dyp = ( ( (dSlopeRateR * x) - rcDefArea.top ) * -1 );
			for (int x1 = x; x1 >= rcDefArea.left; x1--) {
				double dy = ((double)x1 * dSlopeRateR) + dyp ;
				if ((int)dy > rcDefArea.bottom) break;
				if (*(pImgPtr + (nWidth * (int)dy) + x1) != 0) {
					nCount++;
				}
			}
			if (nCount > nMaxCount) {
				nMaxCount = nCount;
			}
		}

		for (y = rcDefArea.top; y < rcDefArea.bottom; y++) {
			int nCount = 0;
			dyp = ( (dSlopeRateR * rcDefArea.right) - y ) * -1 ;
			for (int x1 = x; x1 >= rcDefArea.left; x1--) {
				double dy = ((double)x1 * dSlopeRateR) + dyp;
				if ((int)dy > rcDefArea.bottom) break;
				if (*(pImgPtr + (nWidth * (int)dy) + x1) != 0) {
					nCount++;
				}
			}
			if (nCount > nMaxCount) {
				nMaxCount = nCount;
			}
		}
		double dRate = ( AprData.m_System.m_dResolY + AprData.m_System.m_dResolX[CAM_POS_TOP] ) / 2 ;
		pBlock->dJudgeSize = nMaxCount * dRate ;
	}
	else {
		double dSlopeRate = AprData.m_System.m_dResolX[CAM_POS_TOP] / AprData.m_System.m_dResolY;
		double dSlopeRateR = -1.0 / dSlopeRate;

		int nMaxCount = 0;
		double dyp;
		for (x = rcDefArea.left; x < rcDefArea.right; x++) {
			int nCount = 0;
			dyp = rcDefArea.bottom - ( dSlopeRate * x )  ; // y 절편
			for (int x1 = x; x1 >= rcDefArea.left; x1--) {
				double dy = ((double)x1 * dSlopeRate) + dyp;
				if ((int)dy < rcDefArea.top) break;
				if (*(pImgPtr + (nWidth * (int)dy) + x1) != 0) {
					nCount++;
				}
			}
			if (nCount > nMaxCount) {
				nMaxCount = nCount;
			}
		}
		for (y = rcDefArea.bottom; y > rcDefArea.top; y--) {
			int nCount = 0;
			dyp = y - (dSlopeRate * rcDefArea.right); // y 절편
			for (int x1 = rcDefArea.right; x1 >= rcDefArea.left; x1--) {
				double dy = ((double)x1 * dSlopeRate) + dyp;
				if ((int)dy < rcDefArea.top) break;
				if (*(pImgPtr + (nWidth * (int)dy) + x1) != 0) {
					nCount++;
				}
			}
			if (nCount > nMaxCount) {
				nMaxCount = nCount;
			}
		}
		double dRate = (AprData.m_System.m_dResolY + AprData.m_System.m_dResolX[CAM_POS_TOP]) / 2;
		pBlock->dJudgeSize = nMaxCount * dRate;
	}

	return dRet;
}

// Tab Find 관련 함수
int CImageProcess::DivideSectionBetweenThreshold(int* pnPrjData, int nPrjLength, int nThresMin, int nThresMax, int nMinLength, VEC_SECTOR* pVecSector)
{
	int i;
	int nCount = 0;
	BOOL bFound = FALSE;
	pVecSector->clear();

	int nBefore = en_Max;
	int nStartPos = 0;
	ST_SECTOR sect;

	for (i = 0; i < nPrjLength; i++)
	{
		if ((pnPrjData[i] >= nThresMin) && (pnPrjData[i] <= nThresMax))
		{
			nCount++;
		}
		else
		{
			if (nCount >= nMinLength)
			{
				sect.nStartPos = nStartPos;
				sect.nEndPos = i - 1;
				sect.bDelete = FALSE;
				pVecSector->push_back(sect);
			}
			nCount = 0;
			nStartPos = i + 1;
		}
	}

	if (nCount >= nMinLength)
	{
		sect.nStartPos = nStartPos;
		sect.nEndPos = i - 1;
		sect.bDelete = FALSE;
		pVecSector->push_back(sect);
	}

	nCount = (int)pVecSector->size();

	return nCount;
}

int CImageProcess::FillArea_byRndInfo(BYTE* pImage, BYTE* pMeanPtr, int nWidth, int nHeight, CRect rcArea, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, BYTE btLevel, int nProcPos )
{
	ASSERT(pImage);
	ASSERT(pMeanPtr);
	int x, y;
	int nStartX = rcArea.left;
	int nEndX = rcArea.right;
	int nStartY = rcArea.top;
	int nEndY = rcArea.bottom;

	int nSize = (int)(pVecRndInfo->size()-1);
	int nVecStartPos = 0 ;
	if (nSize <= 0) return -1;

	BYTE* pLinePtr;
	BYTE* pMeanLinePtr;
	for (y = nStartY; y < nEndY; y++) {
		pLinePtr = pImage + (nWidth * y);
		pMeanLinePtr = pMeanPtr + (nWidth * y);
//		if (y > nSize) break;
//		nStartX = (*pVecRndInfo)[y].x;
		for (x = nStartX; x < nEndX; x++) {
			if (*(pLinePtr + x) < btLevel){
				*(pMeanLinePtr + x) = 120;
			}
		}
	}
	return 0;
}


long CImageProcess::GetAreaSum(BYTE* pImage, int nWidth, int nHeight, CRect rectPrj, int nSampling, BOOL bModeSum)
{
	ASSERT(pImage);
	int x, y;
	int nStartX = rectPrj.left;
	int nEndX = rectPrj.right;
	int nStartY = rectPrj.top;
	int nEndY = rectPrj.bottom;

	int nDestWidth = nEndX - nStartX;
	int nDestHeight = nEndY - nStartY;
	long lSum = 0;
	long lRet = 0;

	int nTemp;
	if (nEndY > nHeight) {
		nTemp = nEndY - nHeight;
		nEndY = nHeight;
		nStartY -= nTemp;
	}

	if (nStartY < 0) {
		nStartY = 0; 
	}

	BYTE* pLine;

	int nCount = (nEndY-nStartY)* ((nEndX - nStartX) / nSampling);
	for (y = nStartY; y < nEndY; y++) {
		pLine = pImage + (nWidth * y);
		for (x = nStartX; x < nEndX; x += nSampling) {
			lSum += *(pLine + x);
		}
	}
	if (bModeSum == FALSE) {
		lRet = lSum / nCount;
	}
	else {
		lRet = lSum;
	}

	return lRet;
}

// 이미지의 선택된 영역중 Min Max 사이인 휘도의 합을 구하는 함수.
long CImageProcess::GetAreaSum_BetweenThreshold(BYTE* pImage, int nWidth, int nHeight, CRect rectPrj, int nMin, int nMax, int nSampling, BOOL bModeSum)
{
	ASSERT(pImage);
	int x, y;
	int nStartX = rectPrj.left;
	int nEndX = rectPrj.right;
	int nStartY = rectPrj.top;
	int nEndY = rectPrj.bottom;

	int nDestWidth = nEndX - nStartX;
	int nDestHeight = nEndY - nStartY;
	long lSum = 0;
	long lRet = 0;

	int nTemp;
	if (nEndY > nHeight) {
		nTemp = nEndY - nHeight;
		nEndY = nHeight;
		nStartY -= nTemp;
	}

	if (nStartY < 0) {
		nStartY = 0;
	}

	BYTE* pLine =  NULL ;
	int nValue = 0;
//	int nCount = ((nEndX - nStartX) / nSampling);
	int nCount = 0; 
	for (y = nStartY; y < nEndY; y++) {
		pLine = pImage + (nWidth * y);
		for (x = nStartX; x < nEndX; x += nSampling) {
			nValue = (int)( *( pLine + x ) ) ;
			if ((nValue < nMax) && (nValue > nMin)) {
				lSum += nValue;
				nCount++;
			}
		}
	}
	if (nCount <= 0) {
		return -1;
	}
	if (bModeSum == FALSE) {
		lRet = lSum / nCount;
	}
	else {
		lRet = lSum;
	}

	return lRet;
}

// 22.04.14 Ahn Add Start
// 23.02.24 Ahn Modify Start
//int CImageProcess::FindBoundary_FromPrjData(int* pnPrjData, int nLength, int nTargetBright, int nMode)
int CImageProcess::FindBoundary_FromPrjData(int* pnPrjData, int nLength, int nTargetBright, int nMode, BOOL bFindDark /*=FALSE*/)
// 23.02.24 Ahn Modify End
{
	ASSERT(pnPrjData);

	int i;
	int nLevel = 0 ;
	// 23.02.24 Ahn Add Start
//	if (bFindDark == FALSE) {
	if (bFindDark == TRUE) {
	// 23.02.24 Ahn Add End
		if (nMode == en_FindFromRight) {
			for (i = nLength - 1; i > 0; i--) {
				if (pnPrjData[i] > nTargetBright) {
					nLevel = i;
					break;
				}
			}
		}
		else {
			for (i = 0; i < nLength; i++) {
				if (pnPrjData[i] > nTargetBright) {
					nLevel = i;
					break;
				}
			}
		}
	// 23.02.24 Ahn Add Start
	}
	else {
		if (nMode == en_FindFromRight) {
			for (i = nLength - 1; i > 0; i--) {
				if (pnPrjData[i] < nTargetBright) {
					nLevel = i;
					break;
				}
			}
		}
		else {
			for (i = 0; i < nLength; i++) {
				if (pnPrjData[i] < nTargetBright) {
					nLevel = i;
					break;
				}
			}
		}
	}
	// 23.02.24 Ahn Add End

	return nLevel;


// 	ASSERT(pnPrjData);
// 
// 	int i;
// 	int nLevel = 0;
// 	if (nMode == en_FindFromRight) {
// 		for (i = nLength - 1; i > 0; i--) {
// 			if (pnPrjData[i] > nTargetBright) {
// 				nLevel = i;
// 				break;
// 			}
// 		}
// 	}
// 	else {
// 		for (i = 0; i < nLength; i++) {
// 			if (pnPrjData[i] > nTargetBright) {
// 				nLevel = i;
// 				break;
// 			}
// 		}
// 	}
// 
// 	return nLevel;



}
// 22.04.14 Ahn Add End

// 22.04.13 Ahn Add Start
int CImageProcess::FindTabLevel_Simple(BYTE* pImgPtr, int nWidth, int nHeight, int nFindPos, CRecipeInfo* pRecipeInfo, VEC_SECTOR* pVecSector, int* pnLevel)
{
	ASSERT(pImgPtr);
	ASSERT(pVecSector);
	ASSERT(pnLevel);

	int nRet = 0;
	int nSampling = 10;
	int nStartPos = nFindPos ;
	int nEndPos = nStartPos + (nSampling * 10);

	if (nEndPos > nWidth) {
		nEndPos = nWidth;
	}

	int thMin = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; //(int)(pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2;
	int thMax = 255;

	pVecSector->clear();
	int nLocalRet = 0;
	nLocalRet = CImageProcess::FindTabPos(pImgPtr, nWidth, nHeight, nStartPos, nEndPos, thMin, thMax, pVecSector);
	if (nLocalRet <= 0) {
		return -1;
	}
	nLocalRet = CImageProcess::CombineTabSector(pVecSector, *pRecipeInfo);
	if (nLocalRet < 0) {
		return -2;
	}

	int i = 0;
	int nSize = (int)pVecSector->size();
	int nTabWidth;
	int nFindIdx = 0;
	int nLevelFindPos = 0;

	ST_SECTOR* pstSector = NULL;
	for (i = 0; i < nSize; i++) {
		nTabWidth = (*pVecSector)[i].nEndPos - (*pVecSector)[i].nStartPos;
		if ( nTabWidth > (pRecipeInfo->TabCond.nTabWidth / 2)) {
			pstSector = &(*pVecSector)[i];
			break;
		}
	}

	if (pstSector == NULL)
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<FindTabLevel_Simple>>이미지 Sector 에러 => 레시피에 설정한 TabWidth/2 값 보다 큰 TabWidth 가 없다."));

		return -3;
	}

	int nLastEdge = 0;
	int nMaxDistPos = 0 ;
	int nMaxStartPos = 0 ;
	int nMaxEndPos = 0 ;
	int nDistance;
	for (i = 0; i < nSize ; i++) {
		nDistance = (*pVecSector)[i].nStartPos - nLastEdge ;
		if (nMaxDistPos < nDistance) {
			nMaxStartPos = nLastEdge;
			nMaxEndPos = (*pVecSector)[i].nStartPos;
			nMaxDistPos = nDistance; // 22.04.26 Ahn Add
		}
		nLastEdge = (*pVecSector)[i].nEndPos;
	}

	nDistance = nHeight - (*pVecSector)[nSize-1].nEndPos; // 22.04.26 Ahn Modify
	if (nDistance > nMaxDistPos) {
		nMaxStartPos = (*pVecSector)[nSize-1].nEndPos;
		nMaxEndPos = nHeight ; // 22.04.26 Ahn Modify
	}

	if ((nMaxStartPos >= 0) && (nMaxEndPos > 0)) {
		int nCenterPos = (int)( (nMaxStartPos + nMaxEndPos) / 2 ) ;

		CRect rcPrj;
		int* pnPrjData;
		pnPrjData = new int[nWidth];
		memset(pnPrjData, 0x00, sizeof(int) * nWidth);
		rcPrj.top = nCenterPos - 20 ;
		rcPrj.bottom = nCenterPos + 20 ;
		rcPrj.left = 0;
		rcPrj.right = nWidth;

		// 22.06.24 Ahn Modify Start
		//CImageProcess::GetProjection( pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, FALSE ) ;
		int nCount = CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, TRUE);
		// 22.06.03 Ahn Modify Start
		//*pnLevel = CImageProcess::FindBoundary_FromPrjData(pnPrjData, nWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP], en_FindFromLeft);
		//int nUpperBright = nCount * pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; 

		BOOL bUseDarkRoll = (pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;

		int nUpperBright = nCount * ( (pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP] ) / 2  ); 
		*pnLevel = CImageProcess::FindBoundary_FromPrjData(pnPrjData, nWidth, nUpperBright, en_FindFromRight, bUseDarkRoll);
		// 22.06.24 Ahn Moidyf End

		delete[] pnPrjData;
		pnPrjData = NULL;
	}

	return nRet;

}
// 22.04.13 Ahn Add End

// 음극 Tab을 찾고 Tab 중심에서 검사 라인을 유추함.
int CImageProcess::FindTab_Negative(BYTE* pImgPtr, int nWidth, int nHeight, int nTabFindPos, CRecipeInfo *pRecipeInfo, VEC_SECTOR *pVecSector, int *pnLevel )
{
	ASSERT(pImgPtr);
	ASSERT(pVecSector);
	ASSERT(pnLevel);

	int nRet = 0;
	int nSampling = 10 ;
	int nStartPos = nTabFindPos;
	int nEndPos = nStartPos + (nSampling * 10);

	if (nEndPos > nWidth) {
		nEndPos = nWidth;
	}
	// 23.02.14 Ahn Modify Start
	//// 22.11.07 Ahn Modify Start
	////int thMin = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; //(int)(pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2;
	//int thMin = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];// 40; // pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; //(int)(pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2;
	//// 22.11.07 Ahn Modify End
	int thMin = pRecipeInfo->TabCond.nTabMinBright; // pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP];
	// 23.02.14 Ahn Modify End
	int thMax = 255;

	pVecSector->clear();
	int nLocalRet = 0;
	nLocalRet = CImageProcess::FindTabPos(pImgPtr, nWidth, nHeight, nStartPos, nEndPos, thMin, thMax, pVecSector);
	if (nLocalRet <= 0) return -1;
	nLocalRet = CImageProcess::CombineTabSector(pVecSector, *pRecipeInfo);
	if (nLocalRet < 0) return -2;

	int i = 0; 
	int nSize = (int)pVecSector->size();
	int nTabWidth;
	int nFindIdx = 0 ;

	ST_SECTOR* pstSector = NULL ;
	for (i = 0; i < nSize; i++) {
		nTabWidth = (*pVecSector)[i].nEndPos - (*pVecSector)[i].nStartPos;
		if( abs(nTabWidth - pRecipeInfo->TabCond.nTabWidth) < (pRecipeInfo->TabCond.nTabWidth / 3 )) {
			pstSector = &(*pVecSector)[i];
			break;
		}
	}

	if (pstSector == NULL) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<FindTab_Negative>>이미지 Sector 에러 => 레시피에 설정한 TabWidth/2 값 보다 큰 TabWidth 가 없다."));

		return -3;
	}

	CRect rcPrj;
	int* pnPrjData;
	pnPrjData = new int[nWidth];
	memset(pnPrjData, 0x00, sizeof(int) * nWidth);

	rcPrj.top = pstSector->nStartPos;
	rcPrj.bottom = pstSector->nEndPos;
	rcPrj.left = 0;
	rcPrj.right = nWidth;

	// 22.06.24 Ahn Modify Start AveMode:0 -> SumMode:1
	//CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, 0);
	//int nBundary = CImageProcess::GetBundary_FromPrjData(pnPrjData, nStartPos, 20, 0);
	int nCount = CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, 0);
	// 경계 검출
	int nUpper = 20 * nCount;
	// 22.07.13 Ahn Modify Start
	//int nBundary = CImageProcess::GetBundary_FromPrjData(pnPrjData, nStartPos, 20, 0);
	int nBundary = CImageProcess::GetBundary_FromPrjData(pnPrjData, nWidth, 20, 0);
	// 22.07.13 Ahn Modify End
	// 22.06.24 Ahn Modify End
	// 22.01.05 Ahn Modify Start
	*pnLevel = nBundary - pRecipeInfo->TabCond.nNegCoatHeight ;
	// 22.01.05 Ahn Modify Start
	
	delete[] pnPrjData;
	return nRet;
}

int CImageProcess::FindLevelBottom_Negative(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int* pnLevel, int nFindDir )
{
	int nRet = 0;

	CRect rcPrj;
	int* pnPrjData;
	pnPrjData = new int[nWidth];
	memset(pnPrjData, 0x00, sizeof(int) * nWidth);

	int nSampling = 10;

	rcPrj.left = 0;
	rcPrj.right = nWidth;
	int nCenterY = nHeight / 2;
	rcPrj.top = nCenterY - 100;
	rcPrj.bottom = nCenterY + 100;

	CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, 0);

	int nPrjArray[2000];
	// 22.12.30 Ahn Modify Start
	//memcpy(nPrjArray, pnPrjData, sizeof(int) * nWidth);
	memcpy(nPrjArray, pnPrjData, sizeof(int) * 2000);
	// 22.12.30 Ahn Modify End

	int i ;
	BOOL bFound = FALSE;
	if (nFindDir == CImageProcess::en_FindFromLeft) {
		// 높은 휘도 감지
		for (i = 0; i < nWidth; i++) {
			if (pnPrjData[i] <= pRecipeInfo->TabCond.nCeramicBrightHigh[ CAM_POS_BOTTOM ]) { 
				*pnLevel = i;
				bFound = TRUE;
				break;
			}
		}
	}else{
		// 낮은 휘도 감지
		for (i = nWidth - 1; i > 0; i--) {
			if (pnPrjData[i] <= pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM]) {
				*pnLevel = i;
				bFound = TRUE;
				break;
			}
		}
	}

	if (bFound == FALSE) {
		nRet = -1;
	}
	else {
		nRet = *pnLevel;
	}

	delete[] pnPrjData;

	return nRet;
}

// 23.02.24 Ahn Add Start
int CImageProcess::FindLevelBottom_BrightRoll(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int* pnLevel, int nFindDir)
{
	int nRet = 0;

	CRect rcPrj;
	int* pnPrjData;
	pnPrjData = new int[nWidth];
	memset(pnPrjData, 0x00, sizeof(int) * nWidth);

	int nSampling = 10;

	rcPrj.left = 0;
	rcPrj.right = nWidth;
	int nCenterY = nHeight / 2;
	rcPrj.top = nCenterY - 100;
	rcPrj.bottom = nCenterY + 100;

	CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, 0);

	int nPrjArray[2000];
	memcpy(nPrjArray, pnPrjData, sizeof(int) * 2000);

	int i;
	BOOL bFound = FALSE;
	if (nFindDir == CImageProcess::en_FindFromLeft) {
		// 높은 휘도 감지
		for (i = 0; i < nWidth; i++) {
			if (pnPrjData[i] <= pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM]) {
				*pnLevel = i;
				bFound = TRUE;
				break;
			}
		}
	}
	else {
		// 낮은 휘도 감지
		for (i = nWidth - 1; i > 0; i--) {
			if (pnPrjData[i] <= pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM]) {
				*pnLevel = i;
				bFound = TRUE;
				break;
			}
		}
	}

	if (bFound == FALSE) {
		nRet = -1;
	}
	else {
		nRet = *pnLevel;
	}

	delete[] pnPrjData;

	return nRet;
}
// 23.02.24 Ahn Add End

int CImageProcess::GetBundary_FromPrjData(int* pnPrjData, int nLength, int nCompLen, int nMode)
{
	int nTop = 0;
	int nBtm = 0 ;
	if (nLength <= nCompLen) return -1;

	int i;
	for ( i = 0; i < nCompLen; i++) {
		nTop += pnPrjData[i];
		nBtm += pnPrjData[i + nCompLen ];
	}
	int nEndPos = nLength - nCompLen;

	int nMaxPos = -1 ;
	int nMaxValue = 0 ;
	int nDiff = 0;
	for (i = nCompLen; i < nEndPos; i++) {
		nDiff = abs(nTop - nBtm);
		if (nDiff > nMaxValue) {
			nMaxValue = nDiff;
			nMaxPos = i;
		}
		nTop += pnPrjData[i + 1] - pnPrjData[i - nCompLen];
		nBtm += pnPrjData[i + nCompLen] - pnPrjData[i] ;
	}


	return nMaxPos;
}


int CImageProcess::FindTabLevel(BYTE* pImagePtr, int nWidth, int nHeight, int* pnLevel, CTabCondition tabCond, int nMode, int nDir)
{
	int nRet = 0;
	// 22.08.09 Ahn Add Start

	if (CImageProcess::en_FindRight == nDir) {
		int nCheckCnt = TAB_LEVEL_FIND_POINT;
		int lSum[TAB_LEVEL_FIND_POINT][2];
		int nCheckPitch = (tabCond.nTabPitch - 200) / (nCheckCnt - 1);
		int nRndPitch = tabCond.nRadiusH / 2;
		if (nDir == CImageProcess::en_FindRight) {
			nRndPitch = tabCond.nCeramicTailHeight;
		}

		memset(lSum, 0, sizeof(long) * TAB_LEVEL_FIND_POINT);
		int nMin = 255 * nWidth;
		int nMinPos = -1;
		int i, j, nLine;

		for (i = 0; i < nCheckCnt; i++) {
			nLine = nCheckPitch * i;
			if (nLine >= nHeight) {
				nLine = nHeight - 1;
			}

			CRect rect;
			rect.left = 300;
			rect.right = rect.left + nWidth / 2;
			rect.top = nLine;
			rect.bottom = rect.top + 100;

			//	lSum[i][0] = GetAreaSum(pImagePtr, nWidth, nHeight, rect, 10, FALSE);
				// Ceramic Coating 부의 밝기값 Min Max인 휘도의 합을 구이여 오는 함수.
			lSum[i][0] = GetAreaSum_BetweenThreshold(pImagePtr, nWidth, nHeight, rect, tabCond.nCeramicBrightLow[nDir], tabCond.nCeramicBrightHigh[nDir], 10, TRUE);
			lSum[i][1] = nLine;
		}

		for (i = 0; i < nCheckCnt - 1; i++) {
			for (j = i + 1; j < nCheckCnt; j++) {
				if (lSum[i][0] > lSum[j][0]) {
					SWAP(int, lSum[i][0], lSum[j][0]);
					SWAP(int, lSum[i][1], lSum[j][1]);
				}
			}
		}
		int nFindPos = lSum[1][1]; // 최소값 보다 하나 큰 값 선택.
		nRet = FindTabLevel_Deitail(pImagePtr, nWidth, nHeight, tabCond, nFindPos, pnLevel, nMode, nDir);
	} else {
		int* pnLeft = new int[nWidth];
		int* pnRight = new int[nWidth];

		CRect rcLeft;
		CRect rcRight;
		rcLeft.top = 0;
		rcLeft.bottom = 1000;
		rcRight.left = rcLeft.left = 0; 
		rcRight.right = rcLeft.right = nWidth - 1 ;
		rcRight.top = nHeight - 1 - 1000;
		rcRight.bottom = nHeight - 1 ;

		GetProjection(pImagePtr, pnLeft, nWidth, nHeight, rcLeft, DIR_VER, 10, FALSE ) ;
		GetProjection(pImagePtr, pnRight, nWidth, nHeight, rcRight, DIR_VER, 10, FALSE ) ;

		int nLeftLevel = 0 ;
		int nRightLevel = 0 ;
		for (int i = nWidth - 1; i > 0; i--) {
			if (pnLeft[i] > tabCond.nCeramicBrightLow[CAM_POS_TOP]) {
				nLeftLevel = i;
				break;
			}
		}

		for (int i = nWidth - 1; i > 0; i--) {
			if ( pnRight[i] > tabCond.nCeramicBrightLow[CAM_POS_TOP]) {
				nRightLevel = i;
				break;
			}
		}

		if (abs(nRightLevel - nLeftLevel) < 100) {
			*pnLevel = (nRightLevel + nLeftLevel) / 2;
		}
		else {
			int nDiffLeft, nDiffRight;
			int nCenter = nWidth / 2;

			nDiffLeft = ( nCenter - nLeftLevel ) / 2 ;
			nDiffRight = ( nCenter - nRightLevel ) / 2;

			if (nDiffLeft < nDiffRight) {
				*pnLevel = nLeftLevel;
			}
			else {
				*pnLevel = nRightLevel;
			}
		}

		delete []pnLeft;
		delete []pnRight;
	}
	// 22.08.09 Ahn Add End

	return nRet;
}

int CImageProcess::FindTabLevel_Deitail(BYTE* pImagePtr, int nWidth, int nHeight, CTabCondition tabCond, int nFindPos, int* nLevel, int nMode, int nDir)
{
	int nTabPitch = tabCond.nTabPitch;
	int nTabWidth = tabCond.nTabWidth;
	int thMin = tabCond.nCeramicBrightLow[nDir];
	int thMax = tabCond.nCeramicBrightHigh[nDir];

	CRect rect;
	rect.top = nFindPos;
	rect.left = 0;
	rect.right = nWidth;
	rect.bottom = nFindPos + 100;
	// DIR_VER 
	int nLength = rect.right - rect.left;
	int* pnPrj = new int[nLength];
	int nRet = 0;

	GetProjection(pImagePtr, pnPrj, nWidth, nHeight, rect, DIR_VER, 1, FALSE);

	VEC_SECTOR vecSector;
	int nCount;
	// 22.06.08 Ahn Modify Start
	int nCompWidth = 20 ;
	// 22.06.08 Ahn Modify End

	if (nMode == en_FindThresMode) 
	{
		nCount = DivideSectionBetweenThreshold(pnPrj, nLength, thMin, thMax, 10, &vecSector);

		double dSizeRange = 0.5;
		int nFindNo = -1;

		if (nCount > 0) {
			if (nCount == 1) {
				if (nDir == en_FindLeft) {
					nFindNo = vecSector[0].nEndPos;
				}
				else {
					nFindNo = vecSector[0].nStartPos;
				}
			}
			else {
				// 조건 
				int nMinDist = 999;
				int nMinPos = nCount -1 ;

				*nLevel = nMinPos;
				if (nDir == en_BottomSide) {
					nFindNo = vecSector[0].nStartPos;
				}
				else {
					nFindNo = vecSector[nMinPos].nEndPos;
				}

			}
		}
		nRet = *nLevel = nFindNo;
	}

	if ( ( nMode == en_FineMaxDiffMode ) || ( nLevel <= 0 ) ) {
		nRet = *nLevel = FindTabLevel_FromProjection(pnPrj, nWidth, nCompWidth);
	}
	delete[]pnPrj;

	return nRet;
}

// nCompWidth v
int CImageProcess::FindTabLevel_FromProjection(int* pnPrjData, int nLength, int nCompWidth)
{
	ASSERT(pnPrjData);
	int i ;
	if (nCompWidth <= 0) {
		nCompWidth = 20 ;
	}
	int nEndPos = nLength - nCompWidth ;

	int nDiff = 0;
	int nLSum = 0;
	int nRSum = 0;
	int nMax = 0;
	int nMaxPos = 0;
	for (i = 0; i < nCompWidth; i++) {
		nLSum += pnPrjData[i];
		nRSum += pnPrjData[i + nCompWidth];
	}

	for ( i = nCompWidth; i < nEndPos ; i++) {
		nDiff = abs(nLSum - nRSum);
		nLSum = nLSum - pnPrjData[ i - nCompWidth ] + pnPrjData[i ];
		nRSum = nRSum - pnPrjData[ i ] + pnPrjData[ i + nCompWidth ];
		if (nMax < nDiff) {
			nMax = nDiff;
			nMaxPos = i;
		}
	}
	return nMaxPos;
}

// 23.01.20 Ahn Modify Start
int CImageProcess::FindTabPos_New(BYTE* pImagePtr, int nWidth, int nHeight, int nStartPos, int nEndPos, int nThresMin, int nThresMax, VEC_SECTOR* pVecSector)
{
	return 0;
}
// 23.01.20 Ahn Modify End

// Image의 주어진 rect 위치의 Projection값을 얻어와 Threshold Min~Max 사이의 연속된 Vector들을 얻어오는 함수.
int CImageProcess::FindTabPos(BYTE* pImagePtr, int nWidth, int nHeight, int nStartPos, int nEndPos, int nThresMin, int nThresMax, VEC_SECTOR* pVecSector)
{
	CRect rect;
	rect.left = nStartPos;
	rect.right = nEndPos;
	rect.top = 0;
	rect.bottom = nHeight;

	int nCount;

	if (nThresMin > nThresMax) {
		SWAP(int, nThresMax, nThresMin);
	}

	// DIR_HOR 
	int* pnPrj = new int[nHeight];

	GetProjection(pImagePtr, pnPrj, nWidth, nHeight, rect, DIR_HOR, 1, FALSE);
	nCount = DivideSectionBetweenThreshold(pnPrj, nHeight, nThresMin, nThresMax, 10, pVecSector);

	delete[]pnPrj;
	return nCount;
}

int CImageProcess::CombineTabSector(VEC_SECTOR* pVecSector, CRecipeInfo& RecipeInfo)
{
	ASSERT(pVecSector);

	int nSize = (int)pVecSector->size();
	int nTabCheckRange = (int)( RecipeInfo.TabCond.nTabWidth * 1.2 ) ;

	for (int i = 0; i < nSize - 1 ; i++) {
		if ((*pVecSector)[i].bDelete == TRUE) continue;
		for (int j = i + 1; j < nSize; j++) {
			if ((*pVecSector)[j].bDelete == TRUE) continue;
			if ( ((*pVecSector)[j].nEndPos - (*pVecSector)[i].nStartPos ) <= nTabCheckRange) {
				(*pVecSector)[i].nEndPos = (*pVecSector)[j].nEndPos;
				(*pVecSector)[j].bDelete = TRUE;
			}
		}
	}

	VEC_SECTOR::iterator iter ;
	for (iter = pVecSector->begin(); iter != pVecSector->end(); ) {
		if( iter->bDelete == TRUE  ){
			iter = pVecSector->erase(iter);
		}
		else {
			iter++;
		}
	}

	return 0;
}

// Tab이 이물이나 휘도차로 인하여 분할되어 있는 경우 Tab 폭이내이면 합침.
int CImageProcess::ConvertSectToTabInfo(VEC_SECTOR vecSector, _VEC_TAB_INFO* VecTabInfo, int nFrameCount, CTabCondition tabCond)
{
	int i, j, nSize;
	nSize = (int)vecSector.size();
	VecTabInfo->clear();

	if (nSize <= 0) return -1;

	int nTabWidth = tabCond.nTabWidth;
	int nTabAddRange = tabCond.nTabWidth;
	int nTabWidthRange = nTabWidth;

	for (i = 0; i < nSize - 1; i++) {
		if (vecSector[i].bDelete) continue;
		for (j = i + 1; j < nSize; j++) {
			if (vecSector[j].bDelete) continue;
			int nDiff = abs(vecSector[j].nStartPos - vecSector[i].nEndPos);
			int nWidth = vecSector[i].nStartPos - vecSector[j].nEndPos;
			if ((nDiff < nTabAddRange) && (nWidth < nTabWidthRange)) {
				vecSector[i].nEndPos = vecSector[j].nEndPos;
				vecSector[j].bDelete = TRUE;
			} else {
				break;
			}
		}
	}

	for (i = 0; i < nSize; i++) {
		if (vecSector[i].bDelete == TRUE) continue;
		CTabInfo tabInfo;
		tabInfo.nTabLeft = vecSector[i].nStartPos;
		tabInfo.nTabRight = vecSector[i].nEndPos;
		tabInfo.nFrameCount = nFrameCount;
		tabInfo.nTabWidth = tabInfo.nTabRight - tabInfo.nTabLeft;

		VecTabInfo->push_back(tabInfo);
	}

	return (int)VecTabInfo->size();
}

int CImageProcess::GetBoundaryLineFromEdgeArray(int* pnEdgeArr, int nLength, int nLeft, int nRight, CRect* rcLeft, CRect* rcRight)
{
	ASSERT(pnEdgeArr);
	int nPeakArr[DEFAULT_IMG_SIZE_W];
	memset(nPeakArr, 0, sizeof(nPeakArr));
	int nMaxCnt = 0;
	int nMaxIdx = 0;
	int i;
	for (i = 0; i < nLeft; i++) {
		pnEdgeArr[pnEdgeArr[i]]++;
		if (pnEdgeArr[pnEdgeArr[i]] > nMaxCnt) {
			nMaxCnt = pnEdgeArr[pnEdgeArr[i]];
			nMaxIdx = pnEdgeArr[i];
		}
	}
	rcLeft->top = 0;
	rcLeft->bottom = nLeft;
	rcLeft->left = nMaxIdx;
	rcLeft->right = nMaxIdx;

	nMaxCnt = 0;
	nMaxIdx = 0;

	memset(nPeakArr, 0, sizeof(nPeakArr));
	for (i = nRight; i < nLength; i++) {
		pnEdgeArr[pnEdgeArr[i]]++;
		if (pnEdgeArr[pnEdgeArr[i]] > nMaxCnt) {
			nMaxCnt = pnEdgeArr[pnEdgeArr[i]];
			nMaxIdx = pnEdgeArr[i];
		}
	}
	rcRight->top = nRight;
	rcRight->bottom = nLength;
	rcRight->left = nMaxIdx;
	rcRight->right = nMaxIdx;

	return 0;
}

// 받아온 Boundary Arrary에서 Peek 값의 위치를 가지고
int CImageProcess::GetBoundaryLineFromEdgeArray(int* pnEdgeArr, int nLength, int* nBoundaryLine)
{
	ASSERT(pnEdgeArr);
	int nPeakArr[DEFAULT_IMG_SIZE_W];
	memset(nPeakArr, 0, sizeof(nPeakArr));
	int nMaxCnt = 0;
	int nMaxIdx = 0;
	int i;
	for (i = 0; i < nLength; i++) {
		pnEdgeArr[pnEdgeArr[i]]++;
		if (pnEdgeArr[pnEdgeArr[i]] > nMaxCnt) {
			nMaxCnt = pnEdgeArr[pnEdgeArr[i]];
			nMaxIdx = pnEdgeArr[i];
		}
	}

	*nBoundaryLine = nMaxIdx;

	return 0;
}


int CImageProcess::SmoothVecRoundData(VEC_ROUND_INFO* pVecRoundData, BOOL bMode)
{
	ASSERT(pVecRoundData);
	if(pVecRoundData == nullptr) return -1;

	int nSize = (int)pVecRoundData->size();
	int i;
	int nMeanSize = 20;
	int nHalf = nMeanSize / 2 ;

	if (nSize < nHalf) {
		return -1;
	}

	int* pnAveArr = new int[nSize];
	memset(pnAveArr, 0, sizeof(int) * nSize);

	int nLeftSum = 0;
	int nRightSum = 0;
	// 0 ~ 9 , 11~20
	for (i = 0; i < nHalf; i++) {
		nLeftSum += (*pVecRoundData)[i].x;
		nRightSum += (*pVecRoundData)[i + nHalf + 1].x;
	}
	// 10 ~ End - 10 
	for (i = nHalf; i < (nSize - nHalf-1); i++) {
		pnAveArr[i] = (nLeftSum + nRightSum) / nMeanSize;
		nLeftSum = nLeftSum - (*pVecRoundData)[i - nHalf].x + (*pVecRoundData)[i].x;
		nRightSum = nRightSum - (*pVecRoundData)[i+1].x + (*pVecRoundData)[i + nHalf + 1].x;
	}
	pnAveArr[i] = (nLeftSum + nRightSum) / nMeanSize;

	// 22.01.18 Ahn Add Add Start
	for (i = nHalf; i < nSize - nHalf; i++) {
		if (abs((*pVecRoundData)[i].x - pnAveArr[i]) > 10) {
			(*pVecRoundData)[i].x = pnAveArr[i];
		}
	}

	nLeftSum = 0;
	nRightSum = 0;
	// 0 ~ 9 , 11~20
	for (i = 0; i < nHalf; i++) {
		nLeftSum += (*pVecRoundData)[i].x;
		nRightSum += (*pVecRoundData)[i + nHalf + 1].x;
	}
	// 10 ~ End - 10 
	for (i = nHalf; i < (nSize - nHalf - 1); i++) {
		pnAveArr[i] = (nLeftSum + nRightSum) / nMeanSize;
		nLeftSum = nLeftSum - (*pVecRoundData)[i - nHalf].x + (*pVecRoundData)[i].x;
		nRightSum = nRightSum - (*pVecRoundData)[i + 1].x + (*pVecRoundData)[i + nHalf + 1].x;
	}
	pnAveArr[i] = (nLeftSum + nRightSum) / nMeanSize;
	// 22.01.18 Ahn Add Add End

	for (i = nHalf; i < nSize - nHalf; i++) {
		(*pVecRoundData)[i].x = pnAveArr[i];
	}

	int nStart ;
	int nEnd ;
	int nLength = 20;
	if (bMode == CImageProcess::en_FindLeft) {
		int nSum = 0;
		nStart = nSize - nLength;
		nEnd = nSize;
		for (i = nStart; i < nStart + 5; i++) {
			nSum += (*pVecRoundData)[i].x;
		}

		for (i = nStart + 3 ; i < nSize - 3 ; i++ ) {
			(*pVecRoundData)[i].x = nSum / 5; ;
			nSum = nSum - (*pVecRoundData)[i - 2].x + (*pVecRoundData)[i + 3].x;

		}

		// 22.03.04 Ahn Add Start
		for (i = nSize; i < nSize - 40 ; i-- ) {
			if ((*pVecRoundData)[i].x > (*pVecRoundData)[i - 1].x) {
				SWAP(int, (*pVecRoundData)[i].x, (*pVecRoundData)[i - 1].x);
			}
		}
		// 22.03.04 Ahn Add End
	}
	else {
		int nSum = 0;
		nStart = 0;
		nEnd = nLength - 2 ;
		for (i = nStart; i < 5; i++) {
			nSum += (*pVecRoundData)[i].x;
		}

		for (i = nStart + 3; i < nEnd; i++ ) {
			(*pVecRoundData)[i].x = nSum / 5; ;
			nSum = nSum - (*pVecRoundData)[i - 2].x + (*pVecRoundData)[i + 3].x;
		}

		// 22.03.04 Ahn Add Start
		for (i = 0; i < 40; i++) {
			if ((*pVecRoundData)[i].x < (*pVecRoundData)[i + 1].x) {
				SWAP( int, (*pVecRoundData)[i].x, (*pVecRoundData)[i + 1].x);
			}
		}
		// 22.03.04 Ahn Add End
	}

	delete[]pnAveArr;
	return 0;
}
int CImageProcess::DrawPixel_BoundaryLine(BYTE* pImgPtr, int nWidth, int nHeight, CImageProcess::VEC_ROUND_INFO &VecRndInfo, BYTE btBit)
{
	int nRet = 0;


	int nSize = (int)VecRndInfo.size();

	for (int i = 0; i < nSize; i++) {
		int x = VecRndInfo[i].x;
		int y = VecRndInfo[i].y;
		if ((x > nWidth) || (y > nHeight)|| (x < 0 ) || (y < 0 )) {
			break;
		}
		*(pImgPtr + (nWidth * y ) + x) = btBit ;
	}


	return nRet;
}

// 22.01.17 Ahn Add Start
int CImageProcess::GetBoundaryRoundInfo(BYTE* pOrgImg, BYTE* pRsltImg, int nWidth, int nHeight, CRect rcRound, int nThreshold, CImageProcess::VEC_ROUND_INFO* pVecRoundInfo, int nFindDir)
{
	int nStartX = rcRound.left;
	int nEndX = rcRound.right;
	int nStartY = rcRound.top;
	int nEndY = rcRound.bottom;

	int nCompWidth = 10;
	if (rcRound.right + nCompWidth > nWidth) {
		nEndX = rcRound.right + (rcRound.right + nCompWidth) - nWidth - nCompWidth;
	}
	BYTE* pLinePtr;

	int nStPosY = (int)pVecRoundInfo->size();
	BOOL bFound = FALSE;
	int nLastX = 0;
	if (pRsltImg == NULL) {
		if (nFindDir == CImageProcess::en_FindFromLeft) {
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nStartX; x < nEndX; x++) {
					int nValue = *(pLinePtr + x);
					if ((nValue <= nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						pVecRoundInfo->push_back(cp);
						nLastX = x;
						bFound = TRUE;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
		else {
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nEndX; x > nStartX; x--) {
					int nValue = *(pLinePtr + x);
					if ((nValue > nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						pVecRoundInfo->push_back(cp);
						nLastX = x;
						bFound = TRUE;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
	}
	else {
		if (nFindDir == CImageProcess::en_FindFromLeft) {
			nLastX = nEndX; // 22.01.19 Ahn Add Start
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nStartX; x < nEndX; x++) {
					int nValue = *(pLinePtr + x);
					if ((nValue < nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						nLastX = x;
						bFound = TRUE;
						pVecRoundInfo->push_back(cp);
						*(pRsltImg + (nWidth * y) + x) = 255;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
		else {
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nEndX; x > nStartX; x--) {
					int nValue = *(pLinePtr + x);
					if ((nValue > nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						nLastX = x;
						bFound = TRUE;
						pVecRoundInfo->push_back(cp);
						*(pRsltImg + (nWidth * y) + x) = 255;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
	}

	return 0;
}
// 22.01.17 Ahn Add End
// 23.01.20 Ahn Add Start
int CImageProcess::GetBoundaryRoundInfo_BaseBright(BYTE* pOrgImg, BYTE* pRsltImg, int nWidth, int nHeight, CRect rcRound, int nThreshold, CImageProcess::VEC_ROUND_INFO* pVecRoundInfo, int nFindDir)
{
	int nStartX = rcRound.left;
	int nEndX = rcRound.right;
	int nStartY = rcRound.top;
	int nEndY = rcRound.bottom;

	int nCompWidth = 10;
	if (rcRound.right + nCompWidth > nWidth) {
		nEndX = rcRound.right + (rcRound.right + nCompWidth) - nWidth - nCompWidth;
	}
	BYTE* pLinePtr;

	int nStPosY = (int)pVecRoundInfo->size();
	BOOL bFound = FALSE;
	int nLastX = 0;
	if (pRsltImg == NULL) {
		if (nFindDir == CImageProcess::en_FindFromLeft) {
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nStartX; x < nEndX; x++) {
					int nValue = *(pLinePtr + x);
					if ((nValue <= nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						pVecRoundInfo->push_back(cp);
						nLastX = x;
						bFound = TRUE;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
		else {
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nEndX; x > nStartX; x--) {
					int nValue = *(pLinePtr + x);
					if ((nValue > nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						pVecRoundInfo->push_back(cp);
						nLastX = x;
						bFound = TRUE;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
	}
	else {
		if (nFindDir == CImageProcess::en_FindFromLeft) {
			nLastX = nEndX; // 22.01.19 Ahn Add Start
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nStartX; x < nEndX; x++) {
					int nValue = *(pLinePtr + x);
					if ((nValue < nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						nLastX = x;
						bFound = TRUE;
						pVecRoundInfo->push_back(cp);
						*(pRsltImg + (nWidth * y) + x) = 255;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
		else {
			for (int y = nStartY; y < nEndY; y++) {
				pLinePtr = pOrgImg + (nWidth * y);
				bFound = FALSE;
				for (int x = nEndX; x > nStartX; x--) {
					int nValue = *(pLinePtr + x);
					if ((nValue > nThreshold)) {
						CPoint cp;
						cp.y = y;
						cp.x = x;
						nLastX = x;
						bFound = TRUE;
						pVecRoundInfo->push_back(cp);
						*(pRsltImg + (nWidth * y) + x) = 255;
						break;
					}
				}
				if (bFound == FALSE) {
					CPoint cp;
					cp.y = y;
					cp.x = nLastX;
					pVecRoundInfo->push_back(cp);
				}
			}
		}
	}
	return 0;
}
// 23.01.20 Ahn Add End

int CImageProcess::GetBoundaryRoundInfo_ByBriDist(BYTE* pOrgImg, BYTE* pRsltImg, int nWidth, int nHeight, CRect rcRound, int nThreshold, int nTopCutPos, CImageProcess::VEC_ROUND_INFO* pVecRoundInfo, int nMode, int nFindDir /*=en_FindFromRight*/, BOOL bRolMode )
{
	int nStartX = rcRound.left  ;
	int nEndX = rcRound.right;
	int nStartY = rcRound.top ;
	int nEndY = rcRound.bottom - 1;

	int nCompWidth = 10;
	if (rcRound.right + nCompWidth > nWidth ) {
		nEndX = rcRound.right + (rcRound.right + nCompWidth) - nWidth  - nCompWidth ;
	}
	//BYTE* pLinePtr;

	int nStPosY = (int)pVecRoundInfo->size();
	BOOL bFound = FALSE;
	int nLastX = 0;
	CRect rcRange;
	rcRange.left = nStartX;
	rcRange.right = nEndX;
	rcRange.top = nStartY;
	rcRange.bottom = nEndY;

	// 23.01.20 Ahn Modify Start
	if (bRolMode == TRUE) {
		nFindDir = CImageProcess::en_FindRight;
		GetBoundaryRoundInfo_BaseBright(pOrgImg, pRsltImg, nWidth, nHeight, rcRange, nThreshold, pVecRoundInfo, nFindDir);
	}
	else {
		GetBoundaryRoundInfo(pOrgImg, pRsltImg, nWidth, nHeight, rcRange, nThreshold, pVecRoundInfo, nFindDir);
	}
	// 23.01.20 Ahn Modify End

	// 22.03.30 Ahn Add Start - 3
	int nSize = (int)pVecRoundInfo->size();
	int nAve;
	int nDiff;
	for (int y = 1 ; y < nSize -1; y++) {
		nAve = ( (*pVecRoundInfo)[y - 1].x + (*pVecRoundInfo)[y + 1].x ) / 2;
		nDiff = abs(nAve - (*pVecRoundInfo)[y].x);
		if (nDiff > 20) {
			(*pVecRoundInfo)[y].x = nAve;
		}
	}
	// 22.03.30 Ahn Add End

	int nEdPosY = (int)pVecRoundInfo->size();
	CPoint* pFront, * pRear;
	int nCount = 0;
	if (nMode == CImageProcess::en_FindLeft) {
		for (int y = nStPosY; y < nEdPosY - 1; y++) {
			pFront = &(*pVecRoundInfo)[y];
			pRear = &(*pVecRoundInfo)[y + 1];
			int nDiff = pRear->x - pFront->x;
			if (abs(nDiff) > 2) {
				// 기울기 최저값 찾기
				int nFindStart = y;
				int nOldSlope = (pRear->x - pFront->x) / (pRear->y - pFront->y);
				for (y = y + 2; y < nEdPosY; y++) {
					pRear = &(*pVecRoundInfo)[y];
					int nNewSlope = (pRear->x - pFront->x) / (pRear->y - pFront->y);
					if (pRsltImg != NULL) {
						*(pRsltImg + (nWidth * pRear->y) + pRear->x) = 50;
					}
					if (abs(nOldSlope - nNewSlope) < 2) {
						int nDistX = pRear->x - pFront->x;
						int nDistY = (pRear->y - pFront->y);
						int nPitch = nDistX / nDistY;
						for (int i = nFindStart + 1, nCount = 0; i < y; i++, nCount++) {
							if (pRsltImg != NULL) {
								*(pRsltImg + (nWidth * (*pVecRoundInfo)[i].y) + (*pVecRoundInfo)[i].x) = 50;
								(*pVecRoundInfo)[i].x = pFront->x + (nPitch * nCount);
								*(pRsltImg + (nWidth * (*pVecRoundInfo)[i].y) + (*pVecRoundInfo)[i].x) = 150;
							}
							else {
								(*pVecRoundInfo)[i].x = pFront->x + (nPitch * nCount);
							}
						}
						break;
					}
					nOldSlope = nNewSlope;
				}
			}
		}
	}
	else {
		for (int y = nEdPosY - 1; y > nStPosY ; y--) {
			pFront = &(*pVecRoundInfo)[y];
			pRear = &(*pVecRoundInfo)[y - 1];
			int nDiff = pRear->x - pFront->x;
			if (abs(nDiff) > 1) {
				// 기울기 최저값 찾기
				int nFindStart = y;
				int nOldSlope = (pRear->x - pFront->x) / (pFront->y - pRear->y);
				//for (y = y + 2; y < nEdPosY; y++) {
				for (y = y - 2 ; y >= nStPosY; y--) {
						pRear = &(*pVecRoundInfo)[y];
					int nNewSlope = (pRear->x - pFront->x) / (pFront->y - pRear->y);
					if (pRsltImg != NULL) {
						*(pRsltImg + (nWidth * pRear->y) + pRear->x) = 50;
					}
					nDiff = pRear->x - pFront->x;
					if (abs(nOldSlope - nNewSlope) < 2) {
						int nDistX = pRear->x - pFront->x;
						int nDistY = (pFront->y - pRear->y);
						int nPitch = nDistX / nDistY;
						for (int i = nFindStart, nCount = 0; i > y; i--, nCount++) {
							if (pRsltImg != NULL) {
								*(pRsltImg + (nWidth * (*pVecRoundInfo)[i].y) + (*pVecRoundInfo)[i].x) = 50;
								(*pVecRoundInfo)[i].x = pFront->x + (nPitch * nCount);
								*(pRsltImg + (nWidth * (*pVecRoundInfo)[i].y) + (*pVecRoundInfo)[i].x) = 150;
							}
							else {
								(*pVecRoundInfo)[i].x = pFront->x + (nPitch * nCount);
							}
						}
						break;
					}
					nOldSlope = nNewSlope;
				}
			}
		}
	}

	// 
	if (nMode == CImageProcess::en_FindLeft) {

		int nSize = (int)pVecRoundInfo->size();
		int y = nSize - 1;
		while(!pVecRoundInfo->empty()){
			if ((*pVecRoundInfo)[y].x >= nTopCutPos) {
				pVecRoundInfo->erase(pVecRoundInfo->begin() + y);
				y--;
			}
			else {
				break;
			}
		}
	}
	else {
		int y = 0;
		while (!pVecRoundInfo->empty()) {
			if ((*pVecRoundInfo)[y].x >= nTopCutPos) {
				pVecRoundInfo->erase(pVecRoundInfo->begin());
			}
			else {
				break;
			}

		}
	}

	return 0;
}

int CImageProcess::AddDefectInfoByBlockInfo(CImageProcess::_VEC_BLOCK* pBlockInfo, CRecipeInfo* pRecipeInfo, CTabRsltInfo* pTabRsltInfo, int nCamPos, int nSaveCount, int nFrameStartPos, double dResoY )
{
	ASSERT(pBlockInfo);
	ASSERT(pTabRsltInfo);
	if (pBlockInfo == NULL) return -1 ;
	if (pTabRsltInfo == NULL) return -1 ;

	int nSize;
	nSize = (int)pBlockInfo->size();
	CBlockData* pData;
	int nMinSize[MAX_INSP_TYPE];
	nMinSize[TYPE_FOILEXP_OUT] = pRecipeInfo->nFoilExpOutMinSize[nCamPos] ;
	nMinSize[TYPE_FOILEXP] = pRecipeInfo->nFoilExpInMinSize[nCamPos] ;

	//// 22.11.21 Ahn Add Start
	//int nFrameCnt = pTabRsltInfo->nFrameCount ;
	//// 22.11.21 Ahn Add End

	int nCount = 0; 
	for (int i = 0; i < nSize; i++)
	{
		pData = &(*pBlockInfo)[i];
		if (pData->bDeleteFlag == TRUE) continue;
		if (pData->nPixelCnt < nMinSize[pData->nType]) continue;
		CDefectInfo* pDefInfo;
		pDefInfo = new CDefectInfo();

		pDefInfo->nSize = pData->nPixelCnt;
		pDefInfo->nAvgBright = pData->nBriAve;
		pDefInfo->nMaxBright = pData->nBriMax;
		pDefInfo->nMinBright = pData->nBriMin;
		pDefInfo->nAveOrgBir = pData->nOrgBriAve;
		pDefInfo->nMinOrgBir = pData->nOrgBriMin;
		pDefInfo->nMaxOrgBir = pData->nOrgBriMax;
		pDefInfo->rcPos = pData->rcRect;
		pDefInfo->nSizeX = pData->nWidth ;
		pDefInfo->nSizeY = pData->nHeight;
		pDefInfo->nHeadNo = pTabRsltInfo->m_nHeadNo;
		pDefInfo->nTabNo = pTabRsltInfo->m_nTabNo;
		pDefInfo->dSizeX = pData->dWidth; //(double)pDefInfo->nSizeX * AprData.m_System.m_dResolX[nCamPos];
		pDefInfo->dSizeY = pData->dHeight; //(double)pDefInfo->nSizeY * AprData.m_System.m_dResolY;
		pDefInfo->nType = pData->nType;
		pDefInfo->dJudgeSize = pData->dJudgeSize;
		pDefInfo->dDistance = pData->dDistance;
		pDefInfo->bDeleteFlag = FALSE;
		// 판정 필요
		// // 22.11.21 Ahn Add Start
		CPoint cpCenter = pData->rcRect.CenterPoint();
		int nDefPosY = cpCenter.y ;
		pDefInfo->dDefectPosY = ( (double)( nFrameStartPos + nDefPosY ) * dResoY ) / 1000.0 ;
		// // 22.11.21 Ahn Add End
		// 22.05.25 Ahn Add Start
		pDefInfo->sysTime = pTabRsltInfo->sysTime ;		
		::_tcsnccpy_s( pDefInfo->szLotID, _countof(pDefInfo->szLotID), AprData.m_NowLotData.m_strLotNo.GetBuffer(0), _TRUNCATE);
		// 22.05.25 Ahn Add End

		// 22.05.10 Ahn Add Start
		if( pData->nType == en_ModeSurface )
		{
			if (pData->dWidth > pData->dHeight)
			{
				pDefInfo->dJudgeSize = pData->dWidth;
			}
			else
			{
				pDefInfo->dJudgeSize = pData->dHeight;
			}
			if ( (pRecipeInfo->dSurfaceNgSize[pDefInfo->nHeadNo] > 0.0 ) && ( pDefInfo->dJudgeSize > pRecipeInfo->dSurfaceNgSize[pDefInfo->nHeadNo]) )
			{
				pDefInfo->nRank = JUDGE_NG;
				pTabRsltInfo->m_nJudge = JUDGE_NG;
				pTabRsltInfo->m_wNgReason |= ( ( pDefInfo->nHeadNo == CAM_POS_TOP ) ? CTabRsltBase::en_Reason_Surface_Top : CTabRsltBase::en_Reason_Surface_Btm ); // 22.07.08 Ahn Add

				//DEBUG_LOG.txt
				AprData.SaveDebugLog_Format(_T("<<AddDefectInfoByBlockInfo>>Surface NG => Head No<%d>."), pDefInfo->nHeadNo);

			}
			else if (pTabRsltInfo->m_nJudge < JUDGE_NG)
			{
				if ((pRecipeInfo->dSurfaceGraySize > 0.0) && ( pDefInfo->dJudgeSize > pRecipeInfo->dSurfaceGraySize )) {
					pDefInfo->nRank = JUDGE_GRAY;
					pTabRsltInfo->m_nJudge = JUDGE_GRAY;
				}
			}

			pDefInfo->nDispPos = en_defPosSurface;
		}
		else
		{
			// 22.05.10 Ahn Add End
			// 22.07.20 Ahn Modify Start
			//pDefInfo->nRank = CTabRsltInfo::GetDefJudge(pRecipeInfo->dFoilExpInNgSize, pRecipeInfo->dDefJudgeHeight, pDefInfo->dJudgeSize, pDefInfo->dSizeY);
			// 22.12.14 Ahn Add Start
			if ((pData->nDefPos == en_LeftRound) || (pData->nDefPos == en_RightRound)) {
				pDefInfo->nDispPos = en_defPosRound ;
			}
			else {
				pDefInfo->nDispPos = en_defPosEdge ;
			}
			// 22.12.14 Ahn Add End

			double dNgSize;
			switch (pDefInfo->nType)
			{
			case	en_ModeFoilExp :
				dNgSize = pRecipeInfo->dFoilExpInNgSize[pDefInfo->nHeadNo];
				break;
			case	en_ModeFoilExpOut:
				dNgSize = pRecipeInfo->dFoilExpOutNgSize[pDefInfo->nHeadNo];
				break;
			case	en_ModeFoilBoth :
				dNgSize = pRecipeInfo->dFoilExpBothNgSize[pDefInfo->nHeadNo];
				break;
			case	en_ModeSurface :
				dNgSize = pRecipeInfo->dSurfaceNgSize[pDefInfo->nHeadNo];
				break;
			}
			pDefInfo->nRank = CTabRsltInfo::GetDefJudge( dNgSize, 0, pDefInfo->dJudgeSize, pDefInfo->dSizeY );
			// 22.07.20 Ahn Modify End
			if (pDefInfo->nRank == JUDGE_NG)
			{
				BOOL bNg = TRUE;
				if ( pDefInfo->nType == en_ModeFoilExp )
				{
					if ((pRecipeInfo->dIgnoreDistance < pDefInfo->dDistance) && (pRecipeInfo->dIgnoreDistance > 0) 
						&& (( pDefInfo->dSizeX < pRecipeInfo->dIgnoreSize ) && (pRecipeInfo->dIgnoreSize > 0)))
					{
					// 22.09.14 Ahn Modify End
						pDefInfo->dJudgeSize = pDefInfo->dSizeX;
						pDefInfo->nRank = JUDGE_OK;
						pDefInfo->bDeleteFlag = TRUE;
						bNg = FALSE;
					}
				}
				if ( bNg == TRUE )
				{
					pTabRsltInfo->m_nJudge = JUDGE_NG;


// 					if (pDefInfo->nType == en_ModeFoilExp)
// 					{
// 						pTabRsltInfo->m_wNgReason |= ((pDefInfo->nHeadNo == CAM_POS_TOP) ? CTabRsltBase::en_Reason_FoilExp_Top : CTabRsltBase::en_Reason_FoilExp_Btm); // 22.07.08 Ahn Add
// 
// 						CString strMsg;
// 						strMsg.Format(_T("[NG] en_ModeFoilExp : Head No = %d"));
// 						AprData.SaveDebugLog(strMsg); //pyjtest
// 					}
// 					else
// 					{
// 						pTabRsltInfo->m_wNgReason |= ((pDefInfo->nHeadNo == CAM_POS_TOP) ? CTabRsltBase::en_Reason_FoilExpOut_Top : CTabRsltBase::en_Reason_FoilExpOut_Btm); // 22.07.08 Ahn Add
// 
// 						CString strMsg;
// 						strMsg.Format(_T("[NG] en_ModeFoilExp Out : Head No = %d"));
// 						AprData.SaveDebugLog(strMsg); //pyjtest
// 					}



					CString strMsg;
					switch (pDefInfo->nType)
					{
					case	en_ModeFoilExp:
						if (pDefInfo->nHeadNo == CAM_POS_TOP)
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_FoilExpIn_Top;
							AprData.m_NowLotData.m_SeqDataOut.dwFoilExpInTopCount++;
						}
						else
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_FoilExpIn_Btm;
							AprData.m_NowLotData.m_SeqDataOut.dwFoilExpInBottomCount++;
						}
						strMsg.Format(_T("[NG] en_ModeFoilExpIn : Head No = %d"));
						break;

					case	en_ModeFoilExpOut:
						if (pDefInfo->nHeadNo == CAM_POS_TOP)
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_FoilExpOut_Top;
							AprData.m_NowLotData.m_SeqDataOut.dwFoilExpOutTopCount++;
						}
						else
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_FoilExpOut_Btm;
							AprData.m_NowLotData.m_SeqDataOut.dwFoilExpOutBottomCount++;
						}
						strMsg.Format(_T("[NG] en_ModeFoilExpOut : Head No = %d"));
						break;

					case	en_ModeFoilBoth:
						if (pDefInfo->nHeadNo == CAM_POS_TOP)
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_FoilExpBoth_Top;
							AprData.m_NowLotData.m_SeqDataOut.dwFoilExpBothTopCount++;
						}
						else
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_FoilExpBoth_Btm;
							AprData.m_NowLotData.m_SeqDataOut.dwFoilExpBothBottomCount++;
						}
						strMsg.Format(_T("[NG] en_ModeFoilExpBoth : Head No = %d"));
						break;

					case	en_ModeSurface:
						if (pDefInfo->nHeadNo == CAM_POS_TOP)
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_Surface_Top;
							AprData.m_NowLotData.m_SeqDataOut.dwSpeterTopCount++;
						}
						else
						{
							pTabRsltInfo->m_wNgReason |= CTabRsltBase::en_Reason_Surface_Btm;
							AprData.m_NowLotData.m_SeqDataOut.dwSpeterBottomCount++;
						}
						strMsg.Format(_T("[NG] en_ModeFoilExpBoth : Head No = %d"));
						break;
					}

					AprData.SaveDebugLog(strMsg);


				}
			}
			// 22.11.21 Ahn Modify Start - JUDGE_GRAY
			else if (pTabRsltInfo->m_nJudge < JUDGE_NG)
			{
				if ((pRecipeInfo->dFoileGraySize  > 0.0) && (pDefInfo->dJudgeSize > pRecipeInfo->dFoileGraySize))
				{
					pDefInfo->nRank = JUDGE_GRAY;
					pTabRsltInfo->m_nJudge = JUDGE_GRAY;
				}
			}
			// 22.11.21 Ahn Modify End
		}

		if ( pDefInfo->bDeleteFlag == FALSE)
		{
			pTabRsltInfo->AddDefectInfo(pDefInfo);
			nCount++;	// 22.05.25 Ahn Add Start
		}
		else
		{
			delete pDefInfo;
			pDefInfo = NULL;
		}
		// 22.05.25 Ahn Add Start 
		if( ( nSaveCount > 0  ) && ( nCount >= nSaveCount ) )
		{
			break;
		}
		// 22.05.25 Ahn Add End
	}

	return 0;
}

int CImageProcess::BlockLink(CImageProcess::_VEC_BLOCK* pBlockInfo, CRecipeInfo* pRecipeInfo, int nDefType, int nCamPos )
{
	int nSize = (int)pBlockInfo->size();
	CBlockData* pPtr;
	CBlockData* pTar;
	int nRangX = pRecipeInfo->pntLinkRange[nDefType][nCamPos].x + 1;
	int nRangY = pRecipeInfo->pntLinkRange[nDefType][nCamPos].y + 1;

	CRect rcPtr;
	CRect rcTar;
	CRect rcTemp;
	for (int i = 0; i < nSize - 1; i++) {
		pPtr = &(*pBlockInfo)[i];
		if (pPtr->bDeleteFlag == TRUE) continue;
		for (int j = i + 1; j < nSize; j++) {
			pTar = &(*pBlockInfo)[j];
			if (pTar->bDeleteFlag == TRUE) continue;

			//rcPtr.SetRect(pPtr->nLeft, pPtr->nTop, pPtr->nRight, pPtr->nBottom);
			rcPtr = pPtr->rcRect;
			rcPtr.InflateRect(nRangX, nRangY, nRangX, nRangY);

			//rcTar.SetRect(pTar->nLeft, pTar->nTop, pTar->nRight, pTar->nBottom);
			rcTar = pTar->rcRect;

			if (rcTemp.IntersectRect(&rcPtr, &rcTar) == FALSE) {
				continue;
			}

			pTar->bDeleteFlag = TRUE;
			pPtr->nPixelCnt += pTar->nPixelCnt;
			pPtr->nBriSum += pTar->nBriSum;
			pPtr->nBriAve = (int)(pPtr->nBriSum / pPtr->nPixelCnt);

			if (pPtr->nBriMax < pTar->nBriMax) 	pPtr->nBriMax = pTar->nBriMax;
			if (pPtr->nBriMin > pTar->nBriMin) pPtr->nBriMin = pTar->nBriMin;
			if (pPtr->rcRect.left > pTar->rcRect.left) pPtr->rcRect.left = pTar->rcRect.left;
			if (pPtr->rcRect.right < pTar->rcRect.right) pPtr->rcRect.right = pTar->rcRect.right;
			if (pPtr->rcRect.top > pTar->rcRect.top) pPtr->rcRect.top = pTar->rcRect.top;
			if (pPtr->rcRect.bottom < pTar->rcRect.bottom) pPtr->rcRect.bottom = pTar->rcRect.bottom;
			pPtr->nWidth = (pPtr->rcRect.right - pPtr->rcRect.left) + 1;
			pPtr->nHeight = (pPtr->rcRect.bottom - pPtr->rcRect.top) + 1;

			pPtr->dWidth = pPtr->nWidth * AprData.m_System.m_dResolX[nCamPos];
			pPtr->dHeight = pPtr->nHeight * AprData.m_System.m_dResolY;
		}
	}

	return 0;
}

int CImageProcess::ResizeImage(BYTE* pImgPtr, BYTE* pResizePtr, int nWidth, int nHeight, int nZoomOut)
{
	ASSERT(pImgPtr);
	ASSERT(pResizePtr);

	int nMagnif = nZoomOut;
	int nReWidth = (int)nWidth / nMagnif;
	int nReHeight = (int)nHeight / nMagnif;
	int x, y;
	int sx, sy;
	BYTE* pLinePtr;
	BYTE* pOrgLinePtr;
	BYTE* pTempLinePtr;
	int nSum;

	for (y = 0; y < nReHeight ; y++) {

		pLinePtr = pResizePtr + (nReWidth * y);
		pOrgLinePtr = pImgPtr + (nWidth * (y * nMagnif));

		for (x = 0; x < nReWidth ; x++) {
			nSum = 0;
			for (sy = 0; sy < nZoomOut; sy++) {
				pTempLinePtr = pOrgLinePtr + (sy * nWidth);
				for (sx = 0; sx < nZoomOut; sx++) {
					nSum += *(pTempLinePtr + (x * nMagnif) + sx);
				}
			}
			*(pLinePtr + x) = nSum / (nMagnif * nMagnif);
		}
	}

	return 0;

}

int CImageProcess::CheckRect(CRect* pRect, int nWidth, int nHeight)
{
	int nRet = 0;

	if (pRect->left < 0) pRect->left = 0;
	if (pRect->right > nWidth) pRect->right = nWidth;
	if (pRect->top < 0) pRect->top = 0;
	if (pRect->bottom > nHeight) pRect->bottom = nHeight;

	return nRet;
}

// Mean 처리된 Image Point를 받아서 경계선 검출
// 찾지 못하면 rcRange의 오른쪽 값을 돌려줌.
int CImageProcess::FindTabHeadCeramicBoundary(BYTE *pImagePtr, int nWidth, int nHeight, CRect rcRange )
{
	int nRet = 0;

	int nSearchRange = rcRange.right - rcRange.left;
	int* pnPrjData = new int[nSearchRange];
	int nSampling = 10;

	CImageProcess::GetProjection(pImagePtr, pnPrjData, nWidth, nHeight, rcRange, DIR_VER, nSampling, FALSE);

	int nCompWidth = 10;
	int nLeftSum = 0; 
	int nRightSum = 0;
	int nMaxDiff = 0;
	int nMaxPos = 0;
	for (int i = 0; i < nCompWidth; i++) {
		nLeftSum += pnPrjData[i];
		nRightSum += pnPrjData[i+ nCompWidth];
	}

	for (int x = nCompWidth; x < nSearchRange - nCompWidth; x++) {
		int nDiff = abs(nLeftSum - nRightSum);
		if (nDiff >= nMaxDiff) {
			nMaxDiff = nDiff;
			nMaxPos = x;
		}
		nLeftSum = nLeftSum + pnPrjData[ x ] - pnPrjData[x - nCompWidth];
		nRightSum = nRightSum - pnPrjData[ x ]+ pnPrjData[x + nCompWidth ];
	}

	nRet = nMaxPos + rcRange.left ;

	delete []pnPrjData;
	return nRet;
}

int CImageProcess::DivisionTab_byFixSize(BYTE* pImgPtr, BYTE* pImgBtmPtr, int nWidth, int nHeight, int nFixSize, int nStartPos, int nEndPos, _VEC_TAB_INFO* pVecTabInfo)
{
	ASSERT(pImgPtr);
	ASSERT(pVecTabInfo);

	int nRet = nEndPos ;

	int nSize = (nEndPos - nStartPos) / nFixSize;

	int nLastSavePos = nStartPos; 

	//Image Tab  정보 출력 로그
	LOGDISPLAY_SPEC(5)("DivisionTab_byFixSize = TabInfo<%d>개 저장", nSize);

	for (int i = 0; i < nSize; i++) {
		CTabInfo tabInfo;
		tabInfo.m_bErrorFlag = TRUE;
		tabInfo.nImageLength = nFixSize;
		tabInfo.pImgPtr = new BYTE[nWidth * nFixSize];
		memcpy(tabInfo.pImgPtr, pImgPtr + (nWidth * nLastSavePos), sizeof(BYTE) * nWidth * nFixSize);
		tabInfo.pImgBtmPtr = new BYTE[nWidth * nFixSize];
		memcpy(tabInfo.pImgBtmPtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE) * nWidth * nFixSize);
		nLastSavePos += nFixSize ;

		pVecTabInfo->push_back(tabInfo);
	}


	if (nLastSavePos < nEndPos) {
		nRet = nLastSavePos;
	}

	return nRet;
}

// 22.11.18 Ahn Modify Start
int CImageProcess::DivisionTab_FromImageToTabInfo(BYTE* pImgPtr, BYTE *pImgBtmPtr, int nWidth, int nHeight, int nFindPos, int *pnLevel, CRecipeInfo& RecipeInfo, CTabInfo* pResvTabInfo, _VEC_TAB_INFO* pVecTabInfo, int nFrameCount )
// 22.11.18 Ahn Modify End
{
	int nRet = 0;
	ASSERT(pImgPtr);
	ASSERT(pResvTabInfo);
	ASSERT(pVecTabInfo);
	pVecTabInfo->clear();
	int nLocalRet;
	int nLevel = 0 ; 

	CImageProcess::VEC_SECTOR vecSector;
	int nBaseTabPitch = RecipeInfo.TabCond.nTabPitch;
	int nBaseTabWidth = RecipeInfo.TabCond.nTabWidth;
	int nBaseTabBlank = nBaseTabPitch - nBaseTabWidth;
	int nBaseTabHalfPitch = nBaseTabPitch / 2;
	int nPairSholderLength = (nBaseTabPitch - nBaseTabWidth) / 2;

	//Image Tab  정보 출력 로그
	LOGDISPLAY_SPEC(5)("Tab정보를 찾을 Base 레시피 정보 :RecipeName<%s>, TabPitch %d, TabWidth %d"
		, RecipeInfo.m_strRecipeName, nBaseTabPitch, nBaseTabWidth);

	int nTabFindPos = ( nWidth - 220 ) ; 
	//Image Tab  정보 출력 로그
	LOGDISPLAY_SPEC(5)("Sactor Tab찾을 시작위치 : %d =  이미지 nWidth<%d> - 220 "
		, nTabFindPos, nWidth);

	//모드가 양극일 경우 처리
	if (AprData.m_System.m_nMachineMode == ANODE_MODE) { 
	//22.09.15 Ahn Modify End
		// 2/3 지점에서 수직선을 내려 Tab을 먼저 찾음.
		// Tab 중심에서 수평선을 그어 흑연 코팅 끝을 확인하고, 어깨선 레벨을 찾음.
		//int nTabFindPos = (nWidth / 3) * 2;	// 22.05.30 Ahn Delete 
		nLocalRet = CImageProcess::FindTab_Negative(pImgPtr, nWidth, nHeight, nTabFindPos, &RecipeInfo, &vecSector, &nLevel);
		if (nLocalRet < 0) {
		//	return nLocalRet; // 21.12.28 Ahn Delete 
		}
		*pnLevel = nLevel;

		//Image Tab  정보 출력 로그
		LOGDISPLAY_CHECK(5)
		{
			if (nLocalRet == -2)
				LOGDISPLAY_ALLTXT("=== 정보없음 === 양극 Tab Sector 정보 없음");
			else
			{
				for (int i = 0; i < vecSector.size(); i++)
				{
					LOGDISPLAY_ALL("양극 Tab Sector count : %d => 시작점 : %d,  끝점 : %d, 모드 : %s", 
						i, vecSector[i].nStartPos, vecSector[i].nEndPos, (vecSector[i].nMode == 0) ? "LOW" : "HIGHT");
				}
			}
		}
	}
	//모드가 음극일 경우 처리
	else {
		nLocalRet = CImageProcess::FindTabLevel_Simple(pImgPtr, nWidth, nHeight, nTabFindPos, &RecipeInfo, &vecSector, &nLevel);
		*pnLevel = nLevel;

		//Image Tab  정보 출력 로그
		LOGDISPLAY_CHECK(5)
		{
			if (nLocalRet == -2)
				LOGDISPLAY_ALLTXT("=== 정보없음 === 음극 Tab Sector 정보 없음");
			else
			{
				for (int i = 0; i < vecSector.size(); i++)
				{
					LOGDISPLAY_ALL("음극 Tab Sector count : %d => 시작점 : %d,  끝점 : %d, 모드 : %s",
						i, vecSector[i].nStartPos, vecSector[i].nEndPos, (vecSector[i].nMode == 0) ? "LOW" : "HIGHT");
				}
			}
		}
	}

	int nLastSavePos = 0;
	int nSize = (int)vecSector.size();

	bool bSectorInfo = TRUE;
	if (nSize == 0)
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("==에러== 이미지에서 Sector 정보 찾지 못함"));
		bSectorInfo = FALSE;
	}

	int nCase = -1;
	if( pResvTabInfo->pImgPtr != NULL ){
		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPECTXT(5)("지난 Frame에서 보내지 못하고 남은 Image가 있음");

		// 지난 Frame에서 보내지 못하고 남은 Image가 있음.
		CTabInfo tabInfo;		
		BYTE *pTempPtr = pResvTabInfo->pImgPtr;
		BYTE *pTempBtmPtr = pResvTabInfo->pImgBtmPtr;

		// 예약 Tab과 첫번째 Tab이 붙은 Tab인가??
		// 예약 정보에 Tab이 존재하는가?

		int nSendLength = 0;

		if (nSize <= 0) {
			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPECTXT(5)("새 이미지에서 Sector 정보가 없음.");

			// case 2,3,4는 해당되지 않음
			if (pResvTabInfo->nImageLength > nBaseTabPitch) {
				nCase = 0; // 통으로 보냄.
			}
			else if (pResvTabInfo->nTabWidth <= 0) {
				nCase = 1; // 사이즈로 잘라보냄
			}
		}
		else { // 새 Frame에 탭 정보 있음.
			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPECTXT(5)("새 이미지에서 Sector 정보가 있음.");

			// 22.10.06 Ahn Add Start
			int nDistance = (pResvTabInfo->nImageLength - pResvTabInfo->nTabRight) + vecSector[0].nStartPos ;
			//int n
			// 22.10.06 Ahn Add End
			if (pResvTabInfo->nTabWidth > 0) { // 예약 Tab에 Tab정보 있음. // 2또는 3
				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPECTXT(5)("ResvTabInfo(이전 이미지 Tab)  정보 있음 진행");

				// 22.09.06 Ahn Modify Start
				//int nTabWidth = (pResvTabInfo->nImageLength - pResvTabInfo->nTabRight) + vecSector[0].nEndPos;
				int nTabWidth = (pResvTabInfo->nImageLength - pResvTabInfo->nTabLeft) + vecSector[0].nEndPos;
				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPEC(5)("Tab Width 계산<%d> : 전이미지정보(nImageLength-nTabLeft<%d-%d>) + 새이미지 Sector(0번)의 EndPos<%d>", 
					nTabWidth, pResvTabInfo->nImageLength, pResvTabInfo->nTabLeft, vecSector[0].nEndPos);

				// 22.09.06 Ahn Modify End
				if (nTabWidth <= (int)((double)nBaseTabWidth * 1.2)) { // 22.06.22 Ahn Modify 1.2 -> 1.4
					//한탭으로 판단.
					nCase = 3;
					//Image Tab  정보 출력 로그
					LOGDISPLAY_SPEC(5)("Tab Width가 BaseTabWidth<%d> * 1.2) 보다 작으면 Case %d => 한탭으로 판단..", nBaseTabWidth, nCase);
				}
				else { // 앞에꺼만 보내면 됨.	
					nCase = 2;
					//Image Tab  정보 출력 로그
					LOGDISPLAY_SPEC(5)("Tab Width가 BaseTabWidth<%d> * 1.2) 보다 크면 Case %d => 앞에꺼만 보내면 됨.", nBaseTabWidth, nCase);
				}
			}
			else { // 앞에 탭 정보 없음.
				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPECTXT(5)("ResvTabInfo(이전 이미지 Tab) 없음 진행");

				// 따로 보낼까?
				if ((pResvTabInfo->nImageLength + vecSector[0].nEndPos) > (int)((double)nBaseTabPitch * 1.2)) {
					//Image Tab  정보 출력 로그
					LOGDISPLAY_SPEC(5)("전이미지정보(nImageLength<%d>) + 새이미지 Sector(0번)의 EndPos<%d> 가 nBaseTabPitch<%d> * 1.2 보다 크면",
						pResvTabInfo->nImageLength, vecSector[0].nEndPos, nBaseTabPitch);


				// 22.10.06 Ahn Modify Start
				//	nCase = 1;
					nCase = 5;
					//Image Tab  정보 출력 로그
					LOGDISPLAY_SPEC(5)("Case %d => 처리하지 않고 있음", nCase);
				// 22.10.06 Ahn Modify End
				}else{

					//Image Tab  정보 출력 로그
					LOGDISPLAY_SPEC(5)("전이미지정보(nImageLength<%d>) + 새이미지 Sector(0번)의 EndPos<%d> 가 nBaseTabPitch<%d> * 1.2 보다 작거나같다면",
						pResvTabInfo->nImageLength, vecSector[0].nEndPos, nBaseTabPitch);

					nCase = 4;
					//Image Tab  정보 출력 로그
					LOGDISPLAY_SPEC(5)("Case %d => 앞에 Tab 정보가 없어 그냥 뒤에꺼 앞에 붙여서 보냄.", nCase);
				}
			}
		}



		BOOL bResvSend = FALSE;
		switch (nCase) {
		case	0 : // 통으로 보냄
			tabInfo.nLeft = 0;
			tabInfo.nRight = pResvTabInfo->nImageLength;
			tabInfo.nImageLength = pResvTabInfo->nImageLength;
			tabInfo.m_bErrorFlag = TRUE;
			tabInfo.pImgPtr = new BYTE[nWidth * tabInfo.nImageLength];
			tabInfo.pImgBtmPtr = new BYTE[nWidth * tabInfo.nImageLength];
			// 22.11.18 Ahn Add Start
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount ;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;
			// 22.11.18 Ahn Add End
			memcpy(tabInfo.pImgPtr, pTempPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			memcpy(tabInfo.pImgBtmPtr, pTempBtmPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			nLastSavePos = 0 ; 

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPEC(5)("nCase 0 ErrorFlage TRUE: Top/Bottom Image 생성크기[새이미지 Width + 이전이미지 길이]<%d>, Tab[left<%d>Right<%d>ImageLength<%d>"
				, nWidth * tabInfo.nImageLength, tabInfo.nLeft, tabInfo.nRight, tabInfo.nImageLength);

			break;
		case	1: // 앞뒤 다 없어거나, 그냥 사이즈에 맞춰 잘라보냄.
			tabInfo.nLeft = 0;
			nSendLength = nBaseTabPitch - pResvTabInfo->nImageLength ;
			tabInfo.nRight = pResvTabInfo->nImageLength + nSendLength ;
			tabInfo.nImageLength = pResvTabInfo->nImageLength + nSendLength ;
			tabInfo.m_bErrorFlag = TRUE;
			tabInfo.pImgPtr = new BYTE[nWidth * tabInfo.nImageLength];
			tabInfo.pImgBtmPtr = new BYTE[nWidth * tabInfo.nImageLength];
			// 22.11.18 Ahn Add Start
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPEC(5)("nCase 1 ErrorFlage TRUE: Top/Bottom Image 생성크기[새이미지 Width + BaseTabPitch]<%d>, Tab[left<%d>Right<%d>ImageLength<%d>"
				, nWidth* tabInfo.nImageLength, tabInfo.nLeft, tabInfo.nRight, tabInfo.nImageLength);


			// 22.11.18 Ahn Add End
			memcpy(tabInfo.pImgPtr, pTempPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			memcpy(tabInfo.pImgBtmPtr, pTempBtmPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			memcpy(tabInfo.pImgPtr+(nWidth * pResvTabInfo->nImageLength), pImgPtr, sizeof(BYTE) * nWidth * nSendLength);
			memcpy(tabInfo.pImgBtmPtr + (nWidth * pResvTabInfo->nImageLength), pImgBtmPtr, sizeof(BYTE) * nWidth * nSendLength);
			nLastSavePos = nSendLength;
			break;
		case	2 : // 예약Tab 정보에 Tab이 있어서 뒤에 꺼 붙여보냄.
			tabInfo.nTabLeft = pResvTabInfo->nTabLeft;
			tabInfo.nTabRight = pResvTabInfo->nTabRight;
			// 22.11.18 Ahn Add Start
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;
			// 22.11.18 Ahn Add End
			bResvSend = TRUE;

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPECTXT(5)("nCase 2는 이전 정보와 합하고 Sector 정보 유지");

			break;
		case	3 :	// 앞뒤 Tab 정보가 하나로 판단 붙여서 보냄
			tabInfo.nTabLeft = pResvTabInfo->nTabLeft;
			tabInfo.nTabRight = pResvTabInfo->nImageLength + vecSector[0].nEndPos ;
			// 22.11.18 Ahn Add Start
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;
			// 22.11.18 Ahn Add End
			vecSector.erase(vecSector.begin());
			bResvSend = TRUE;

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPECTXT(5)("nCase 3은 이전 정보와 합하고 Sector 정보를 삭제함");
			break;
		case	4 :	// 앞에 Tab 정보가 없어 그냥 뒤에꺼 앞에 붙여서 보냄.
			tabInfo.nTabLeft = pResvTabInfo->nImageLength + vecSector[0].nStartPos;
			tabInfo.nTabRight = pResvTabInfo->nImageLength + vecSector[0].nEndPos;
			// 22.11.18 Ahn Add Start
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;
			// 22.11.18 Ahn Add End
			vecSector.erase(vecSector.begin());
			bResvSend = TRUE;

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPECTXT(5)("nCase 4는 이전 정보와 합하고 Sector 정보를 삭제함");
			break;
		default :
			break;
		}

		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPEC(5)("Case %d => TabLeft : %d, TabRight : %d, nFrameCount : %d, TabStartPosInFrame : %d",
			nCase, tabInfo.nTabLeft, tabInfo.nTabRight, tabInfo.nFrameCount, tabInfo.nTabStartPosInFrame);

		if (bResvSend == TRUE) {

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPECTXT(5)("Case 2 - 4는 TabInfo를 이용하여 메모리를 생성하고 이미지를 저장한다. ");

			tabInfo.nCenter = (tabInfo.nTabRight + tabInfo.nTabLeft) / 2;
			tabInfo.nRight = tabInfo.nCenter + nBaseTabHalfPitch;			// 센터기준으로 할지 오른쪽 기준으로 자를지....
			nSendLength = tabInfo.nRight - pResvTabInfo->nImageLength;

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPEC(5)("Send Length <%d>:  nRight(Tab의 Center + Base Tab HalfPitch)<%d> - 전이미지 Length<%d>"
				,nSendLength, tabInfo.nRight, pResvTabInfo->nImageLength);
			// 22.03.30 Ahn Add Start
			if (nSendLength > nHeight) {
				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPEC(5)("Send Length <%d>가 받은 이미지 Height<%d>  보다 크면 Send Length를 Height로(최대  Height 못벗어나게)"
					, nSendLength, nHeight);

				nSendLength = nHeight;
			}
			// 22.03.30 Ahn Add End
			tabInfo.nImageLength = nSendLength + pResvTabInfo->nImageLength;
			tabInfo.pImgPtr = new BYTE[nWidth * tabInfo.nImageLength];
			memcpy(tabInfo.pImgPtr, pResvTabInfo->pImgPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			memcpy(tabInfo.pImgPtr + (nWidth * pResvTabInfo->nImageLength), pImgPtr, sizeof(BYTE) * nWidth * nSendLength);
			tabInfo.pImgBtmPtr = new BYTE[nWidth * tabInfo.nImageLength];
			memcpy(tabInfo.pImgBtmPtr, pResvTabInfo->pImgBtmPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			memcpy(tabInfo.pImgBtmPtr + (nWidth * pResvTabInfo->nImageLength), pImgBtmPtr, sizeof(BYTE) * nWidth * nSendLength);
			nLastSavePos = nSendLength;

			// 22.11.18 Ahn Add Start
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount ;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame ;
			// 22.11.18 Ahn Add End

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPEC(5)("Case %d => nCenter(Tab의 중심) : %d, nRight(Tab의 Center + Base Tab HalfPitch) : %d, nImageLength : %d, nFrameCount : %d, TabStartPosInFrame : %d",
				nCase, tabInfo.nCenter, tabInfo.nRight, tabInfo.nImageLength, tabInfo.nFrameCount, tabInfo.nTabStartPosInFrame);

		}

		//텝정보를 저장한다.
		pVecTabInfo->push_back(tabInfo);
		LOGDISPLAY_SPEC(5)("텝정보저장 총갯수<%d>", pVecTabInfo->size());

		if (pResvTabInfo->pImgPtr != NULL) {
			delete[]pResvTabInfo->pImgPtr;
			pResvTabInfo->pImgPtr = NULL;

		}
		if (pResvTabInfo->pImgBtmPtr != NULL) {
			delete[]pResvTabInfo->pImgBtmPtr;
			pResvTabInfo->pImgBtmPtr = NULL;
		}
		pResvTabInfo->ResetData();
	}

	nSize = (int)vecSector.size();

	if (bSectorInfo && (nSize == 0))
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<DivisionTab_FromImageToTabInfo>>처리 - 이미지처리 Case<%d> ** 이미지에서 Sector 정보 찾지았지만 기존이미지 통합과정에서 삭제됨"), nCase);
	}

	if (nSize <= 0) {
	 // Tab을 찾지 못하여 길이 우선으로 잘라서 보냄.

		int nSendAbleLeng = (nHeight - nLastSavePos);
		int nSendAbleCount = nSendAbleLeng / nBaseTabPitch ;

		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPECTXT(5)("새 이미지에 Sector 정보가 없을 때 처리구간 시작");

		nLastSavePos = DivisionTab_byFixSize(pImgPtr, pImgBtmPtr, nWidth, nHeight, nBaseTabPitch, nLastSavePos, nHeight, pVecTabInfo);

		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPECTXT(5)("새 이미지에 Sector 정보가 없을 때 처리구간 끝");

	}
	else
	{
		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPECTXT(5)("새 이미지에 Sector 정보가 있을 때 처리구간 시작");

		int nLeftSize = ( ( vecSector[0].nStartPos - nPairSholderLength ) - nLastSavePos);
		int nDivCnt = 0;

		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPEC(5)("Sector 0번째 에서  Left Size  구함 => Left Size : %d, Base Tab Pitch : %d", nLeftSize, nBaseTabPitch);

		//Last Save Pos를 다시 구한다.
		if (nLeftSize > nBaseTabPitch ) 
		{
			nDivCnt = nLeftSize / nBaseTabPitch ;
			if (nDivCnt > 0) 
			{
				nLastSavePos = DivisionTab_byFixSize(pImgPtr, pImgBtmPtr, nWidth, nHeight, nBaseTabPitch, nLastSavePos, nLastSavePos + nLeftSize, pVecTabInfo);

				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPECTXT(5)("Left Size / Base Tab Pitch 나누기한 결과 0 보다 클때 DivisionTab_byFixSize 함수 호출");
			}
		}		

		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPECTXT(5)("새 이미지에 Sector 정보로 Tab 정보를 만들고 메모리할당하여 Image를 저장한다.[nCase 2번만 정보가 생긴다]");

		for (int i = 0; i < nSize; i++) 
		{
			CTabInfo tabInfo;
			if (vecSector[i].nStartPos <= nLastSavePos) 
			{
				//DEBUG_LOG.txt
				AprData.SaveDebugLog_Format(_T("<<DivisionTab_FromImageToTabInfo>>처리 - 이미지처리 Case<%d> Scetor 정보 처리 ** Scetor Start Pos가 Last Save Pos 보다 작거나 같을 때 continue(패스) 처리번호<%d/%d>"), nCase, i, nSize);

				continue;
			}
			tabInfo.nTabLeft = vecSector[i].nStartPos - nLastSavePos;
			tabInfo.nTabRight = vecSector[i].nEndPos - nLastSavePos;
			tabInfo.nTabWidth = tabInfo.nTabRight - tabInfo.nTabLeft;
			tabInfo.nLeft = 0;
			tabInfo.nCenter = (tabInfo.nTabRight + tabInfo.nTabLeft) / 2;
			// 21.09.02 Ahn Add Start
			// Tab Width  미달인 경우, 센터기준이 아닌 오른쪽 기준으로 자름.
			if (vecSector[i].nStartPos == 0) {
				tabInfo.nRight = tabInfo.nTabRight + nPairSholderLength;

				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPEC(5)("Sector %d번 =>  Start Pos<%d> 가 0 이면 Right<%d> = TabRight<%d> + nPairSholderLength<%d>)"
					, i, vecSector[i].nStartPos, tabInfo.nRight, tabInfo.nTabRight, nPairSholderLength);
			}
			else {
				tabInfo.nRight = tabInfo.nCenter + nBaseTabHalfPitch;

				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPEC(5)("Sector %d번 =>  Start Pos<%d> 가 0 이 아니면 Right<%d> = nCenter<%d> + nBaseTabHalfPitch<%d>)"
					, i, vecSector[i].nStartPos, tabInfo.nRight, tabInfo.nCenter, nBaseTabHalfPitch);
			}
			// 21.09.02 Ahn Add Start
			tabInfo.nRight = tabInfo.nCenter + nBaseTabHalfPitch;
			tabInfo.nImageLength = tabInfo.nRight ;

			if ((tabInfo.nImageLength + nLastSavePos) > nHeight)
			{ 
				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPEC(5)("Sector %d번 =>  보낼 이미지 사이즈가 남은 이미지 사이즈 보다 큰경우 ResvTabInfo(남은) 이미지 저장 처리하고 Sector 처리 종료.", i);

				// 보낼 이미지 사이즈가 남은 이미지 사이즈 보다 큰경우 
				int nBackupSize = tabInfo.nImageLength -( (tabInfo.nImageLength + nLastSavePos) - nHeight ) ;
				*pResvTabInfo = tabInfo;
				if (pResvTabInfo->pImgPtr != NULL) {
					delete []pResvTabInfo->pImgPtr;
				}
				if (pResvTabInfo->pImgBtmPtr != NULL) {
					delete[]pResvTabInfo->pImgBtmPtr;
				}
				pResvTabInfo->pImgPtr = new BYTE[nWidth * nBackupSize];
				pResvTabInfo->pImgBtmPtr = new BYTE[nWidth * nBackupSize];
				pResvTabInfo->nImageLength = nBackupSize;

				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPEC(5)("ReservTabInfo-0 left[%d], nTabLeft[%d], nTabRight[%d], nRight[%d]",
					pResvTabInfo->nLeft, pResvTabInfo->nTabLeft, pResvTabInfo->nTabRight, pResvTabInfo->nRight);

				// 22.11.18 Ahn Add Start
				pResvTabInfo->nFrameCount = nFrameCount;
				pResvTabInfo->nTabStartPosInFrame = nLastSavePos;
				// 22.11.18 Ahn Add End 
				memcpy(pResvTabInfo->pImgPtr, pImgPtr + ( nWidth * nLastSavePos ) , sizeof(BYTE) * nWidth * nBackupSize);
				memcpy(pResvTabInfo->pImgBtmPtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE)* nWidth* nBackupSize);

				//Image Tab  정보 출력 로그
				LOGDISPLAY_SPEC(5)("Sector %d번 => ResvTabInfo에[Image 저장 - 크기<%d>, nFrameCount<%d>, nTabStartPosInFrame<%d>]", 
					i, nWidth* nBackupSize, pResvTabInfo->nFrameCount, pResvTabInfo->nTabStartPosInFrame);

				//DEBUG_LOG.txt
				AprData.SaveDebugLog_Format(_T("<<DivisionTab_FromImageToTabInfo>>처리 - 이미지처리 Case<%d> Scetor 정보 처리 ** 보낼 이미지 사이즈가 남은 이미지 사이즈 보다 큰경우 처리종료 처리번호<%d/%d>"), nCase, i, nSize);

				return 0;
			}

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPEC(5)("Sector %d번 =>  보낼 이미지 사이즈가 보낼 이미지 사이즈가 남은 이미지 작거나 같을 경우", i);

			tabInfo.pImgPtr = new BYTE[tabInfo.nImageLength * nWidth];
			memcpy(tabInfo.pImgPtr, pImgPtr + (nWidth * nLastSavePos), sizeof(BYTE) * nWidth * tabInfo.nImageLength);
			tabInfo.pImgBtmPtr = new BYTE[tabInfo.nImageLength * nWidth];
			memcpy(tabInfo.pImgBtmPtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE)* nWidth* tabInfo.nImageLength);

			// 22.11.18 Ahn Add Start
			tabInfo.nFrameCount = nFrameCount ;
			tabInfo.nTabStartPosInFrame = nLastSavePos ;
			// 22.11.18 Ahn Add End

			nLastSavePos = nLastSavePos + tabInfo.nImageLength;
			pVecTabInfo->push_back(tabInfo);

			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPEC(5)("Sector %d Tab Info => TabLeft : %d, TabRight : %d, nCenter : %d, nImageLength : %d, FrameCount : %d, nTabStartPosInFrame : %d",
				i, tabInfo.nTabLeft, tabInfo.nTabRight, tabInfo.nCenter, tabInfo.nImageLength, tabInfo.nFrameCount, tabInfo.nTabStartPosInFrame);
		}	
		
		int nRightSize = (nHeight - nLastSavePos) ;
		if ( nRightSize > nBaseTabPitch ){
			int nSendLeng = nRightSize / nBaseTabPitch;
			//Image Tab  정보 출력 로그
			LOGDISPLAY_SPEC(5)("nLastSavePos DivisionTab_byFixSize 함수 호출 => nBaseTabPitch : %d < nRightSize = (nHeight - nLastSavePos) : %d 라면 LastSavePos구함",
				nBaseTabPitch, nRightSize);

			nLastSavePos = DivisionTab_byFixSize(pImgPtr, pImgBtmPtr, nWidth, nHeight, nBaseTabPitch, nLastSavePos, nHeight,  pVecTabInfo);
		}

	}

	int nLeftSize = nHeight - nLastSavePos;
	if ( nLeftSize > 0) {
		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPECTXT(5)("nLeftSize = nHeight - nLastSavePos 가 0보다 크면 ResvTabInfo(남은) 이미지 정보 저장");

		pResvTabInfo->pImgPtr = new BYTE[nWidth * nLeftSize];
		pResvTabInfo->pImgBtmPtr = new BYTE[nWidth * nLeftSize];
		pResvTabInfo->nImageLength = nLeftSize;

		//Image Tab  정보 출력 로그
		LOGDISPLAY_SPEC(5)("ReservTabInfo-1 left[%d], nTabLeft[%d], nTabRight[%d], nRight[%d]",
			pResvTabInfo->nLeft, pResvTabInfo->nTabLeft, pResvTabInfo->nTabRight, pResvTabInfo->nRight);

		// 22.11.18 Ahn Add Start
		pResvTabInfo->nFrameCount = nFrameCount ;
		pResvTabInfo->nTabStartPosInFrame = nLastSavePos ;
		// 22.11.18 Ahn Add End 

		memcpy(pResvTabInfo->pImgPtr, pImgPtr + (nWidth * nLastSavePos) , sizeof(BYTE) * nWidth * nLeftSize);
		memcpy(pResvTabInfo->pImgBtmPtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE)* nWidth* nLeftSize);

	}

	return nRet;
}


int CImageProcess::GetHistoList(BYTE* pImgPtr, int nWidth, int nHeight, CRect rcRange, int* nHistoArr)
{

	int nStartX = rcRange.left;
	int nEndX = rcRange.right;
	int nStartY = rcRange.top;
	int nEndY = rcRange.bottom;

	memset(nHistoArr, 0, sizeof(int) * 256);

	BYTE* pLinePtr;
	for (int y = nStartY; y < nEndY; y++) {
		pLinePtr = pImgPtr + (nWidth * y);
		for (int x = nStartX; x < nEndX;x++) {
			nHistoArr[*(pLinePtr + x)]++;
		}
	}

	return 0;
}


int CImageProcess::GetTabHeadPos(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int& nLeft, int& nRight, int& nLevel)
{
	nLevel = -1;
	CImageProcess::FindTabLevel(pImgPtr, nWidth, nHeight, &nLevel, pRecipeInfo->TabCond, pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP], CImageProcess::en_FindLeft);
	CImageProcess::VEC_SECTOR vecSec;
	int nTabFindStPos = nLevel + pRecipeInfo->TabCond.nTabCeramicHeight - 40;
	int nTabFindEdPos = nLevel + pRecipeInfo->TabCond.nTabCeramicHeight;

	int thMin = (int)(pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2;
	int thMax = pRecipeInfo->TabCond.nCeramicBrightHigh[CAM_POS_TOP];

	CImageProcess::FindTabPos(pImgPtr, nWidth, nHeight, nTabFindStPos, nTabFindEdPos, thMin, thMax, &vecSec);
	CImageProcess::_VEC_TAB_INFO vecTabInfo;
	CImageProcess::ConvertSectToTabInfo(vecSec, &vecTabInfo, 0, pRecipeInfo->TabCond);

	if (vecTabInfo.empty()) {
		return -1;
	}

	nLeft = vecTabInfo[0].nTabLeft;
	nRight = vecTabInfo[0].nTabRight;

	nLevel = nLevel;

	return nLevel;

}

int CImageProcess::DiffProcImage(BYTE* pImgPtr, BYTE* pMeanImg, BYTE* pProcImg, int nWidth, int nHeight, CRect rcProc, double dMagnif, BYTE btThresLevel, BOOL bDarkEmphMode)
{

	int nStartX = rcProc.left;
	int nEndX = rcProc.right;
	int nStartY = rcProc.top;
	int nEndY = rcProc.bottom;

	if (nStartX < 0) { nStartX = 0; }
	if (nStartY < 0) { nStartY = 0; }
	if (nEndX > nWidth) { nEndX = nWidth; }
	if (nEndY > nHeight) { nEndY = nHeight; }

	double dMagnificition = 1 + (dMagnif * 0.05);

	BYTE* pLinePtr;
	BYTE* pMeanLinePtr;
	BYTE* pProcLinePtr;
	BYTE btOrgLevel;
	int nTemp;
	int nValue;
	double dStdF[256];
	double dDEF = 60000.0;
	if (bDarkEmphMode == TRUE) {
		dDEF = 30000.0;
	}
	for (int i = 0; i < 256; i++) {
		dStdF[i] = (pow((255 - i), 2) / dDEF) + 1.0;
	}

	for (int y = nStartY; y < nEndY; y++) {
		pLinePtr = pImgPtr + (nWidth * y);
		pMeanLinePtr = pMeanImg + (nWidth * y);
		pProcLinePtr = pProcImg + (nWidth * y);
		for (int x = nStartX; x < nEndX; x++) {
			btOrgLevel = *(pLinePtr + x);
			{
				nTemp = (int)(((btOrgLevel - *(pMeanLinePtr + x)) * dMagnificition) * dStdF[btOrgLevel]);
				// 22.06.30 Ahn Add Start
				//nValue = (int)((double)nTemp) + 128;
				nValue = (int)((double)nTemp) ;
				// 22.06.30 Ahn Add End

				if (nValue > 255) {
					*(pProcLinePtr + x) = 255;
				}
				else if (nValue < 0) {
					*(pProcLinePtr + x) = 0;
				}
				else {
					*(pProcLinePtr + x) = (BYTE)nValue;
				}
			}

			if (*(pProcLinePtr + x) < btOrgLevel) {
				*(pProcLinePtr + x) = btOrgLevel;
			}

		}
	}
	return 0;
}

int CImageProcess::CopyPartImage(BYTE* pTarImgPtr, BYTE* pSrcImgPtr, int nWidth, int nHeight, CRect rcCopy)
{
	int nStartX = rcCopy.left;
	int nStartY = rcCopy.top;
	int nEndX = rcCopy.right;
	int nEndY = rcCopy.bottom;

	int cx = 0;
	int cy = 0;
	for (int y = nStartY; y < nEndY; y++) {
		BYTE* pLinePtr = pTarImgPtr + (nWidth * y);
		BYTE* pCropLinePtr = pSrcImgPtr + (nWidth * y);
		for (int x = nStartX; x < nEndX; x++) {
			*(pLinePtr + x) = *(pCropLinePtr + x);
		}
	}

	return 0;
}

// 22.05.24 Ahn Add Start
int CImageProcess::SortingBlockInfo(_VEC_BLOCK* pVecBlock)
{
	int nRet = 0;

	ASSERT(pVecBlock);

	int nSize = (int)pVecBlock->size();

	if (nSize < 2) {
		return 0;
	}

	CBlockData blockData;
	for (int i = 0; i < nSize - 1; i++) {
		for (int j = i + 1; j < nSize; j++) {
			if ((*pVecBlock)[i].dJudgeSize < (*pVecBlock)[j].dJudgeSize) {
				blockData = (*pVecBlock)[i];
				(*pVecBlock)[i] = (*pVecBlock)[j];
				(*pVecBlock)[j] = blockData;
			}
		}
	}

	return nRet;
}

// 22.07.20 Ahn Add Start
int CImageProcess::MergeBlockInfo(CBlockData& DataIn, CBlockData& DataOut, int nCamPos)
{
	DataIn.rcRect.right = DataOut.rcRect.right;
	if (DataIn.rcRect.top > DataOut.rcRect.top) {
		DataIn.rcRect.top = DataOut.rcRect.top;
	}

	if (DataIn.rcRect.bottom < DataOut.rcRect.bottom) {
		DataIn.rcRect.bottom = DataOut.rcRect.bottom;
	}

	DataIn.dDistance = 0 ; 
	
	DataIn.nHeight = ( DataIn.rcRect.bottom - DataIn.rcRect.top ) + 1 ;
	DataIn.nWidth = ( DataIn.rcRect.right - DataIn.rcRect.left ) + 1 ;
	DataIn.nPixelCnt += DataOut.nPixelCnt;

	double dResoX = AprData.m_System.m_dResolX[nCamPos];
	DataIn.dWidth = DataIn.nWidth * dResoX ;
	DataIn.dHeight = DataIn.nHeight * AprData.m_System.m_dResolY ;
	DataIn.nType = en_ModeFoilBoth ;
	DataIn.dJudgeSize = DataIn.dWidth;

	if (DataIn.nBriMax < DataOut.nBriMax) {
		DataIn.nBriMax = DataOut.nBriMax;
	}
	if (DataIn.nBriMin > DataOut.nBriMin) {
		DataIn.nBriMin = DataOut.nBriMin;
	}
	if (DataIn.nOrgBriMax < DataOut.nOrgBriMax) {
		DataIn.nOrgBriMax = DataOut.nOrgBriMax;
	}
	if (DataIn.nOrgBriMin > DataOut.nOrgBriMin) {
		DataIn.nOrgBriMin = DataOut.nOrgBriMin;
	}

	DataIn.nBriSum = (DataIn.nBriSum + DataOut.nBriSum);
	DataIn.nOrgBriSum = (DataIn.nOrgBriSum + DataOut.nOrgBriSum);
	DataIn.nBriAve = (int)( DataIn.nBriSum / DataIn.nPixelCnt );
	DataIn.nOrgBriAve = (int)(DataIn.nOrgBriSum / DataIn.nPixelCnt);

	return 0;
}


BOOL CImageProcess::IsLinkedBlock(CRect rcPtr, CRect rcTar, CPoint cpRange)
{
	CRect rcTemp;
	rcPtr.InflateRect(cpRange.x, cpRange.y, cpRange.x, cpRange.y);

	if (rcTemp.IntersectRect(&rcPtr, &rcTar) == FALSE) {
		return FALSE ;
	}

	return TRUE ;
}

int CImageProcess::MergeAndLink_BlockInfo(_VEC_BLOCK* pDestVlock, _VEC_BLOCK vecFirst, _VEC_BLOCK vecSecond, CRecipeInfo* pRecipeInfo, int nCamPos)
{
	int nRet = 0;

	ASSERT(pDestVlock);
	ASSERT(pRecipeInfo);

	int nSizeFst = (int)vecFirst.size();
	int nSizeSnd = (int)vecSecond.size();
	int nTotal = nSizeFst + nSizeSnd;

	CPoint cpRange = pRecipeInfo->pntLinkRange[nCamPos][0] ;

	_VEC_BLOCK::iterator iterF = vecFirst.begin();
	_VEC_BLOCK::iterator iterS = vecSecond.begin();
	pDestVlock->clear();

	for (; (iterF != vecFirst.end()) || ((iterS != vecSecond.end())); ) {
		if ((iterF != vecFirst.end()) && ((iterS != vecSecond.end()))) {
			// 두 결함이 붙어 있는가? 			
			if( IsLinkedBlock(iterF->rcRect, iterS->rcRect, cpRange) == TRUE ){
				MergeBlockInfo(*iterF, *iterS, nCamPos);
				iterS = vecSecond.erase(iterS);
			} else {
				// 링크되지 않은경우 Bottom이 작은 결함 Merge 하고 삭제. 
				if (iterF->rcRect.bottom > iterS->rcRect.bottom) {
					pDestVlock->push_back(*iterS);
					iterS = vecSecond.erase(iterS);
				} else {
					pDestVlock->push_back(*iterF);
					iterF = vecFirst.erase(iterF);
				}
			}
		}
		else if (iterS != vecSecond.end()) {
			pDestVlock->push_back(*iterS);
			iterS = vecSecond.erase(iterS);
		}
		else if (iterF != vecFirst.end()) {
			pDestVlock->push_back(*iterF);
			iterF = vecFirst.erase(iterF);
		}
	}
	return nRet;
}
// 22.07.20 Ahn Add End
int CImageProcess::MergeBlockList(_VEC_BLOCK* pDestVlock, _VEC_BLOCK vecFirst, _VEC_BLOCK vecSecond)
{
	int nRet = 0;

	ASSERT(pDestVlock);

	int nSizeFst = (int)vecFirst.size();
	int nSizeSnd = (int)vecSecond.size();
	int nTotal = nSizeFst + nSizeSnd;

	_VEC_BLOCK::iterator iterF = vecFirst.begin() ;
	_VEC_BLOCK::iterator iterS = vecSecond.begin() ;

	pDestVlock->clear();

	for (; (iterF != vecFirst.end()) || ((iterS != vecSecond.end())); ){
		if( (iterF != vecFirst.end()) && ((iterS != vecSecond.end())) ){
			if (iterF->dJudgeSize > iterS->dJudgeSize) {
				pDestVlock->push_back(*iterF);
				iterF = vecFirst.erase(iterF);
			}
			else {
				pDestVlock->push_back(*iterS);
				iterS = vecSecond.erase(iterS);
			}
		} else if (iterS != vecSecond.end()) {
			pDestVlock->push_back(*iterS);
			iterS = vecSecond.erase(iterS);
		} else if (iterF != vecFirst.end()) {
			pDestVlock->push_back(*iterF);
			iterF = vecFirst.erase(iterF);
		}
	}
	return nRet;
}
// 22.05.24 Ahn Add End

// 22.05.09 Ahn Add Start
int CImageProcess::ImageProcessDetectSurface(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, CRect rcArea, CTabRsltInfo* pTabRsltInfo, int nCamPos, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt )
{
	int nRet = 0 ;
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

#if defined( SAVE_TACT_LOG ) 
	CTimeAnalyzer ctAna;
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	
	BYTE* pThresPtr;
	int nSizeAll = nWidth * nHeight;
	pThresPtr = new BYTE [nSizeAll];
	int nThresMin = pRecipeInfo->nThresSurface[nCamPos];
	CRegionInfo roiSpetter ;

	CheckRect( &rcArea, nWidth, nHeight) ;

	roiSpetter.SetProcBit(en_Surface_Bit);
	roiSpetter.SetRect(rcArea);
	roiSpetter.SetRoiPtr(pThresPtr);

	CImageProcess::_VEC_BLOCK vecBlockSpetter;
	vecBlockSpetter.clear();

	CImageProcess::Threshold(pImgPtr, &roiSpetter, nWidth, nHeight, nThresMin, TRUE);
	// 22.06.08 Ahn Modify Start
	//CImageProcess::LoopLabeling(&roiSpetter, nWidth, nHeight, &vecBlockSpetter, CImageProcess::en_Surface_Bit, pRecipeInfo->nThresSurface[nCamPos], AprData.m_System.m_dResolX[nCamPos], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiSpetter, nWidth, nHeight, &vecBlockSpetter, CImageProcess::en_Surface_Bit, pRecipeInfo->m_nSurfaceMinSize[nCamPos], AprData.m_System.m_dResolX[nCamPos], AprData.m_System.m_dResolY);
	// 22.06.08 Ahn Modify End

	// 22.05.24 Ahn Add Start
	CImageProcess::SortingBlockInfo(&vecBlockSpetter);
	// 22.05.24 Ahn Add End

	// 22.11.21 Ahn Modify Start
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockSpetter, pRecipeInfo, pTabRsltInfo, nCamPos, MAX_SAVE_DEFECT_COUNT );
	// 22.12.01 Ahn Modify Start
	//int nFrameStartPos = pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight ;
	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	// 22.12.01 Ahn Modify End
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockSpetter, pRecipeInfo, pTabRsltInfo, nCamPos, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY );
	// 22.11.21 Ahn Modify End

#if defined( SAVE_TACT_LOG ) 
	CString strLog;
	double dTime = ctAna.WhatTimeIsIt_Double();
	strLog.Format(_T("Spetter Proc Time [%.2lf]"), dTime);
#endif

	if (bSimMode == TRUE) {
		memcpy(pImgPtrArr[nArrCnt], pThresPtr, sizeof(BYTE) * nSizeAll);
	}

	delete[] pThresPtr;
	pThresPtr = NULL;

	return 0 ;
}
// 22.05.09 Ahn Add End

// 23.02.10 Ahn Add Start
int CImageProcess::ImageProcessTopSide_BrightRoll(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode /*= 0 */ , BYTE** pImgPtrArr /*= NULL */ , int nArrCnt /*= 0*/)
{
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

#if defined( SAVE_TACT_LOG ) 
	CTimeAnalyzer ctAna;
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	int nOffset = 50;

	int nLeftOffset;
	int nRightOffset = 100 ; 
	int nTabRoundOffsetR;

	nLeftOffset = (int)((double)pRecipeInfo->TabCond.nNegCoatHeight * 1.5);
	nTabRoundOffsetR = pRecipeInfo->TabCond.nNegCoatHeight;

	CRect rcAll;
	CRect rcLeft;
	CRect rcRight;

	rcAll.left = rcRight.left = rcLeft.left = nLineLevel - nLeftOffset;
	rcAll.right = nLineLevel + (int)(pRecipeInfo->TabCond.nRadiusW);
	rcRight.right = rcLeft.right = nLineLevel + nRightOffset;


	if (rcAll.right >= nWidth) {
		rcAll.right = nWidth - 1;
	}
	if (rcAll.left < 0) {
		rcAll.left = 0;
	}

	rcAll.top = rcLeft.top = 0;
	rcAll.bottom = rcRight.bottom = nHeight ;
	rcLeft.bottom = nTabLeft - pRecipeInfo->TabCond.nRadiusH ; //pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	rcRight.top = nTabRight + pRecipeInfo->TabCond.nRadiusH;  //pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight); 
	CImageProcess::CheckRect(&rcRight, nWidth, nHeight);

	int* pnResultArr;
	pnResultArr = new int[nHeight];
	memset(pnResultArr, 0, sizeof(int) * nHeight);

	CImageProcess::VEC_ROUND_INFO vecLeftRndInfo;
	CImageProcess::VEC_ROUND_INFO vecRightRndInfo;
	vecLeftRndInfo.clear();
	vecRightRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nSizeAll = nWidth * nHeight;
	BYTE* pMeanPtr = new BYTE[nSizeAll];

	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll);
	int nMeanSize = MEAN_SIZE_HOR;

	int nHeadBndry;
	nHeadBndry = nLineLevel + nTabRoundOffsetR;

	int nMaskRight = nHeadBndry - 30;

	BYTE* pStdPtr = new BYTE[nHeight * nWidth];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);

	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);

#if defined( SAVE_TACT_LOG ) 
	double dMeanProc = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);

	int nRoundOffset = 50;	// 라운드 부분이 Slip 발생으로 늘어질경우를 대비한 Offset값
	int nEdgeCompWidth = 10;	//	Edge 검출 Parameter 상하 비교 폭

	// Tab 왼쪽(위) 부분의 Edge 검출
	CRect rcTemp;
	rcTemp = rcLeft;
	int nLocalRet = 0;

	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr; 

	int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];
	int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; 

	BYTE* pBndryPtr = NULL ;
	if (bSimMode == TRUE) {
		pBndryPtr = pImgPtrArr[0];
	}
	nLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, pBndryPtr, &vecLeftRndInfo, nWidth, nHeight, rcLeft, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);

	if (nLocalRet < 0)
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_BrightRoll>>에러 Edge found faile left round, ret = %d"),
			nLocalRet);

		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		// 22.04.18 Ahn Add End
		delete[] pMeanPtr; // 22.04.26 Ahn Add
		return -1;
	}
#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize > 0) {
		rcLeft.top = vecLeftRndInfo[0].y;
		rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	}
	else
	{
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		delete[] pMeanPtr; 

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_BrightRoll>>에러 - Edge found faile left SmoothVecRoundData Size = %d"),
			nLeftSize);

		return -2; // 처리 불가.
	}

#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage2 = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	// Tab 오른쪽(아래) 부분의 Edge 검출

	nLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcRight, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);

	if (nLocalRet < 0) 
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_BrightRoll>>에러 - Edge found faile right round, ret = %d"),
			nLocalRet);

		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		delete[] pMeanPtr; 
		return -2;
	}
	CImageProcess::SmoothVecRoundData(&vecRightRndInfo, CImageProcess::en_FindRight);

	int nRightSize = (int)vecRightRndInfo.size();
	if (nRightSize > 0) {
		rcRight.top = vecRightRndInfo[0].y;
		rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	}
	else 
	{
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		delete[] pMeanPtr; 

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_BrightRoll>>에러 - Edge found faile right SmoothVecRoundData, nRightSize = %d"),
			nRightSize);

		return -3;
	}

#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage3 = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	delete[]pnResultArr;

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nSizeAll];
	BYTE* pDiffPtr;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
	roiDross.SetRoiPtr(pThresPtr);
	roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	int nDrossThLower = pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	int nFoilExpThLower = pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	int nFoilOutMaskOffset = pRecipeInfo->nFoilOutMaskOffset[CAM_POS_TOP];
	int nFoileExpMaskOffset = pRecipeInfo->nFoilExpMaskOffset[CAM_POS_TOP];
	int nFoilOutInspWidth = pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	int nFoilExpInspWidth = pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];

	CRect rcLeftDiff = rcLeft;
	CRect rcRightDiff = rcRight;

	{
		pDiffPtr = new BYTE[nSizeAll];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];

		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeft, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRight, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
	}

	roiFoilExp.SetRect(rcLeft);
	roiFoilExp.SetRect(rcRight);

	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecLeftRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFoileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecRightRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFoileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);

#if defined( SAVE_TACT_LOG ) 
	double dThresTime = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

#if defined( SAVE_TACT_LOG ) 
	double dLabelTime = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	pTabRsltInfo->RemoveAll();
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) {
		if (nMode == TYPE_FOILEXP_OUT) {
			pVecBlockPtr = &vecBlockDross;
		}
		else {
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			CPoint ptCenter;
			ptCenter.x = (pData->rcRect.right + pData->rcRect.left) / 2;
			ptCenter.y = (pData->rcRect.bottom + pData->rcRect.top) / 2;

			CPoint cpLT;
			CPoint cpLB;
			cpLB.x = cpLT.x = pData->rcRect.left;
			cpLT.y = pData->rcRect.top;
			cpLB.y = pData->rcRect.bottom;

			if (rcLeft.PtInRect(cpLT) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecLeftRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
			}
			else if (rcRight.PtInRect(cpLB) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecRightRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
			}
		}
	}


	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, pRecipeInfo, CAM_POS_TOP);

	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);

#if defined( SAVE_TACT_LOG ) 
	double dDefCollectTime = ctAna.WhatTimeIsIt_Double();
	CString strTactLog;
	strTactLog.Format(_T("MeanTime[%.2lf], EdgeTime[%.2lf], Edge2[%.2lf], Edge3[%.2lf], Thres[%.2lf], Labeling[%.2lf], Defect Collect TIme [%.2lf]")
		, dMeanProc, dEdgeDtImage, dEdgeDtImage2, dEdgeDtImage3, dThresTime, dLabelTime, dDefCollectTime);
	AprData.SaveTactLog(strTactLog);
#endif

	if (bSimMode == TRUE) {
		BYTE* pBndPtr = NULL;
		pBndPtr = new BYTE[nWidth * nHeight];
		memset(pBndPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecLeftRndInfo, 0x80);
		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecRightRndInfo, 0x80);
		memcpy(pImgPtrArr[0], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[1], pBndPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[2], pDiffPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[3], pThresPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[4], pStdPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[5], pProcPtr, sizeof(BYTE) * nWidth * nHeight);

		delete[] pBndPtr;
		pBndPtr = NULL;
	}

	delete[]pStdPtr;
	pStdPtr = NULL;
	delete[]pProcPtr;
	pProcPtr = NULL;

	delete[]pMeanPtr;
	pMeanPtr = NULL;
	delete[]pThresPtr;
	pThresPtr = NULL;
	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		delete[] pDiffPtr;
		pDiffPtr = NULL;
	}
	return 0;
}
int CImageProcess::ImageProcessBottomSide_BrightRoll(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode /*= 0*/, BYTE** pImgPtrArr /*= NULL*/, int nArrCnt /*= 0*/)
{
	// 23.02.24 Ahn Add Start
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

	RECT rcAll;
	rcAll.left = nLineLevel - pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	rcAll.right = nLineLevel + pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM] + pRecipeInfo->TabCond.nNegVGrooveHeight;

	rcAll.top = 0;
	rcAll.bottom = nHeight;

	if (nLineLevel < 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<ImageProcessBottomSide_BrightRoll>>에러 - !!! Tab Level 이상 !!!"));
		return -1;
	}
	if (rcAll.left < 0) {
		rcAll.left = 0;
	}
	if (rcAll.right >= nWidth) {
		rcAll.right = (nWidth - 1);
	}

	int nSizeAll = nWidth * nHeight;
	BYTE* pTempPtr = new BYTE[nSizeAll];
	memset(pTempPtr, 0x00, sizeof(BYTE) * nSizeAll);

	VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nEdgeWidth = 10;

	// 평균화 처리 
	BYTE* pMeanPtr = pTempPtr;
#if defined( SAVE_TACT_LOG )
	CTimeAnalyzer ctAna;
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	int nMeanSize = MEAN_SIZE_HOR;

	BYTE* pStdPtr = new BYTE[nHeight * nWidth];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);

	BYTE* pDiffPtr;
	{
		pDiffPtr = new BYTE[nSizeAll];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_BOTTOM];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, pRecipeInfo->bDarkEmpMode);
	}

#if defined( SAVE_TACT_LOG )
	double dMean = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	// 22.05.30 Ahn Add Start
	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr;
	// 22.05.30 Ahn Add End

	int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
	int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM];

	int nLocalRet = FALSE;
	nLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, nThresMax, CImageProcess::en_BottomSide, nLineLevel, CImageProcess::en_FindLeft);

	if (nLocalRet < 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessBottomSide_BrightRoll>>에러 - Edge found faile, ret = %d"),
			nLocalRet);
		return -3;
	}

	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft);


#if defined( SAVE_TACT_LOG )
	double dEdge = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CRegionInfo roiFoilExp;
	//	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nWidth * nHeight];
	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);

	roiFoilExp.SetRect(rcAll);
	//	roiDross.SetRect(rcAll);

	int nDrossThLower = pRecipeInfo->nDrossThresLower[CAM_POS_BOTTOM];
	int nFoilExpThLower = pRecipeInfo->nFoilExpThresLower[CAM_POS_BOTTOM];
	int nFoilOutMaskOffset = pRecipeInfo->nFoilOutMaskOffset[CAM_POS_BOTTOM];
	int nFileExpMaskOffset = pRecipeInfo->nFoilExpMaskOffset[CAM_POS_BOTTOM];

	int nFoilOutInspWidth = pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	int nFoilExpInspWidth = pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	int nRet = 0;

	{
		roiFoilExp.SetRect(rcAll);
		int nMskRight = rcAll.right;
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiFoilExp, &vecAllRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilOutInspWidth, nFoilExpThLower, nMskRight, TRUE, CImageProcess::en_BottomSide);
	}

#if defined( SAVE_TACT_LOG )
	double dThres = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	_VEC_BLOCK vecBlockFoilExp;
	_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());

	// Overflow 는 4번 Bit살려서 보냄
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, AprData.m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);

#if defined( SAVE_TACT_LOG )
	double dLabel = ctAna.WhatTimeIsIt_Double();

	CString strLog;
	strLog.Format(_T(" Bottom Side Mean[%.2lf], Edge[%.2lf], Thres[%.2lf], Label[%.2lf]"), dMean, dEdge, dThres, dLabel);
	AprData.SaveTactLog(strLog);
#endif
	pTabRsltInfo->RemoveAll();

	_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	{
		{
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();

		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];

			CImageProcess::CheckDistanceToEdgeLine(pData, &vecAllRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);

			if (pData->bDeleteFlag == TRUE) continue;
			//if (AprData.m_System.m_nMachineMode == ANODE_MODE) 
			{
				//	CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
			}
		}
	}

	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);

	if (bSimMode == TRUE) {
		BYTE* pBndPtr = NULL;
		pBndPtr = new BYTE[nWidth * nHeight];
		memset(pBndPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecAllRndInfo, 0x80);
		memcpy(pImgPtrArr[0], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[1], pBndPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[2], pDiffPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[3], pThresPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[4], pStdPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[5], pProcPtr, sizeof(BYTE) * nWidth * nHeight);
		//	memcpy(pImgPtrArr[7], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);

		delete[] pBndPtr;
		pBndPtr = NULL;
	}

	delete[] pStdPtr;
	pStdPtr = NULL;
	delete[] pProcPtr;
	pProcPtr = NULL;

	delete[]pThresPtr;
	pThresPtr = NULL;
	delete[]pTempPtr;
	pTempPtr = NULL;
	delete[] pDiffPtr;
	pDiffPtr = NULL;

	return nRet;
	// 23.02.24 Ahn Add End
}
// 23.02.10 Ahn Add End

// Head부 검사 처리 함수.
int CImageProcess::ImageProcessTopSide_AreaDiff(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

#if defined( SAVE_TACT_LOG ) 
	CTimeAnalyzer ctAna;
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	int nOffset = 50 ;

	// 22.01.05 Ahn Add Start
	int nLeftOffset;
	int nRightOffset; // 22.05.09 Ahn Add
	int nTabRoundOffsetR;
	//22.09.15 Ahn Modify Start
	//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	//22.09.15 Ahn Modify End
		nLeftOffset = (int)((double)pRecipeInfo->TabCond.nNegCoatHeight * 1.5);
		nTabRoundOffsetR = pRecipeInfo->TabCond.nNegCoatHeight;
	}
	else {
		// 22.05.09 Ahn Modify Start
		//nLeftOffset = pRecipeInfo->TabCond.nCeramicHeight;
		nLeftOffset = (int)((pRecipeInfo->dFoilExpInspWidth[CAM_POS_TOP] * 1000.0) / AprData.m_System.m_dResolX[CAM_POS_TOP]);
		nRightOffset = (int)((pRecipeInfo->dFoilOutInspWidth[CAM_POS_TOP] * 1000.0) / AprData.m_System.m_dResolX[CAM_POS_TOP]);
		// 22.05.09 Ahn Modify End
		nTabRoundOffsetR = pRecipeInfo->TabCond.nRadiusW;
	}
	// 22.01.05 Ahn Add End

	CRect rcAll;
	CRect rcLeft;
	CRect rcRight;
	// 22.01.05 Ahn Modify Start
	//rcAll.left = rcRight.left = rcLeft.left = nLineLevel - m_pRecipeInfo->TabCond.nCeramicHeight;
	rcAll.left = rcRight.left = rcLeft.left = nLineLevel - nLeftOffset;
	// 22.01.05 Ahn Modify End
	rcAll.right = nLineLevel + (int)(pRecipeInfo->TabCond.nRadiusW ) ;
	// 22.05.09 Ahn Modify Start
	//rcRight.right = rcLeft.right = nLineLevel + 100;
	rcRight.right = rcLeft.right = nLineLevel + nRightOffset ;
	// 22.05.09 Ahn Modify End

	// 22.06.03 Ahn Modify Start
	if (rcAll.right >= nWidth) {
		rcAll.right = nWidth - 1;
	}
	if (rcAll.left < 0) {
		rcAll.left = 0;
	}

	rcAll.top = rcLeft.top = 0;
	rcAll.bottom = rcRight.bottom = nHeight;
	rcLeft.bottom = nTabLeft + 50; //pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	rcRight.top = nTabRight - 50;//pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CRect rcLeftRnd;
	CRect rcRightRnd;
	rcLeftRnd = rcLeft;
	rcRightRnd = rcRight;
	rcLeftRnd.right = rcRightRnd.right = nLineLevel + pRecipeInfo->TabCond.nRadiusW;// Tab Level 필요 
	rcLeftRnd.top = rcLeft.bottom - pRecipeInfo->TabCond.nRadiusH;
	rcRightRnd.bottom = rcRightRnd.top + pRecipeInfo->TabCond.nRadiusH;

	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight); // 22.06.22 Ahn Add
	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight);
	CImageProcess::CheckRect(&rcLeftRnd, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRight, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRightRnd, nWidth, nHeight);

	int* pnResultArr;
	pnResultArr = new int[nHeight];
	memset(pnResultArr, 0, sizeof(int) * nHeight);

	CImageProcess::VEC_ROUND_INFO vecLeftRndInfo;
	CImageProcess::VEC_ROUND_INFO vecRightRndInfo;
	vecLeftRndInfo.clear();
	vecRightRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nSizeAll = nWidth * nHeight;
	BYTE* pMeanPtr = new BYTE[nSizeAll];

	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll);
	int nMeanSize = MEAN_SIZE_HOR;

	//int nHeadBndry = nLineLevel + pRecipeInfo->TabCond.nTabCeramicHeight;
	int nHeadBndry;
	//22.09.15 Ahn Modify End
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	//22.09.15 Ahn Modify End
		nHeadBndry = nLineLevel + pRecipeInfo->TabCond.nTabCeramicHeight;
	}
	else {
		nHeadBndry = nLineLevel + nTabRoundOffsetR;
	}

	int nMaskRight = nHeadBndry - 30;

	BYTE* pStdPtr = new BYTE[nHeight * nWidth];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeftRnd, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRightRnd, 15, 15, CImageProcess::en_FillBottom);

	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeftRnd, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRightRnd, 3, 3, CImageProcess::en_FillBottom);

#if defined( SAVE_TACT_LOG ) 
	double dMeanProc = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	// 22.04.26 Ahn Add Start
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeftRnd);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRightRnd);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);
	// 22.04.26 Ahn Add End

	// Mean처리한 Image로 Tab의 Center 에서 Tab의 높이를 구함.
	int nRoundOffset = 50;	// 라운드 부분이 Slip 발생으로 늘어질경우를 대비한 Offset값
	int nEdgeCompWidth = 10;	//	Edge 검출 Parameter 상하 비교 폭

	// Tab 왼쪽(위) 부분의 Edge 검출
	CRect rcTemp;
	rcTemp = rcLeft;
	rcTemp.bottom = rcLeftRnd.top;
	rcTemp.right = rcLeftRnd.right;
	int nCropWidth = rcLeftRnd.Width();
	int nCropHeight = rcLeftRnd.Height();
	int nLocalRet = 0;

	// 22.01.06 Ahn Add Start
	BYTE* pEdgePtr;
	{	
		//pEdgePtr = pStdPtr ; 
		pEdgePtr = pMeanPtr; // 22.04.26 Ahn Modify
	}
	// 22.01.06 Ahn Add End

		
	int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];
	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nEdgeCompWidth, 2, DIR_VER);
	}
	else {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, NULL, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindLeft);
	}

	if (nLocalRet < 0) {
		CString strMsg;
		strMsg.Format(_T("Edge found faile left round, ret = %d"), nLocalRet);
		// 22.04.18 Ahn Add Start
		AprData.SaveErrorLog(strMsg);
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		// 22.04.18 Ahn Add End
		delete[] pMeanPtr; // 22.04.26 Ahn Add
		return -1;
	}
#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	// Round는 가로 세로 2번의 Edge 검출이 필요함.
	rcLeftRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, NULL, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft);
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize > 0) {
		rcLeft.top = vecLeftRndInfo[0].y;
		rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	}
	else {
		// 22.04.18 Ahn Add Start
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		// 22.04.18 Ahn Add End
		delete[] pMeanPtr; // 22.04.26 Ahn Add

		// 22.05.02 Ahn Add Start
		CString strMsg;
		strMsg.Format(_T("nLeftSize = %d"), nLeftSize);
		AprData.SaveErrorLog(strMsg);
		// 22.05.02 Ahn Add End

		return -2; // 처리 불가.
	}

#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage2 = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	// Tab 오른쪽(아래) 부분의 Edge 검출

	nCropWidth = rcRightRnd.Width();
	nCropHeight = rcRightRnd.Height();
	// Round는 가로 세로 2번의 Edge 검출이 필요함.
	rcRightRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, NULL, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight);

	rcTemp = rcRight;
	rcTemp.top = rcRightRnd.bottom;
	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcTemp, nEdgeCompWidth, 2, DIR_VER);
	}
	else {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);
	}

	if (nLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_AreaDiff>>에러 - Edge found faile right round, ret = %d"),
			nLocalRet);

		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		// 22.04.18 Ahn Add End
		delete[] pMeanPtr; // 22.04.26 Ahn Add
		return -2;
	}
	CImageProcess::SmoothVecRoundData(&vecRightRndInfo, CImageProcess::en_FindRight);

	int nRightSize = (int)vecRightRndInfo.size();
	if (nRightSize > 0) {
		rcRight.top = vecRightRndInfo[0].y;
		rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	}
	else {
		//	ASSERT(FALSE);
		// 22.04.18 Ahn Add Start
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		// 22.04.18 Ahn Add End
		delete[] pMeanPtr; // 22.04.26 Ahn Add

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_AreaDiff>>에러 SmoothVecRoundData - nRightSize = %d"),
			nRightSize);

		return -3;
	}

#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage3 = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	//	delete[]pRsltPtr;
	delete[]pnResultArr;

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nSizeAll];
	BYTE* pDiffPtr;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
	roiDross.SetRoiPtr(pThresPtr);
	roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	int nDrossThLower = pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	int nFoilExpThLower = pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	int nFoilOutMaskOffset = pRecipeInfo->nFoilOutMaskOffset[CAM_POS_TOP];
	int nFoileExpMaskOffset = pRecipeInfo->nFoilExpMaskOffset[CAM_POS_TOP];
	int nFoilOutInspWidth = pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	int nFoilExpInspWidth = pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];

	CRect rcLeftDiff = rcLeft;
	CRect rcRightDiff = rcRight;

	rcLeftDiff.bottom = rcLeftRnd.top;
	rcRightDiff.top = rcRightRnd.bottom;
	rcLeftDiff = rcLeft;
	rcRightDiff = rcRight;

	//if (pRecipeInfo->bDisableProcDiff == FALSE)
	{ 
		pDiffPtr = new BYTE[nSizeAll];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];

		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeft, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRight, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftRnd, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightRnd, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
	}
	//else {
	//	pDiffPtr = pImgPtr;
	//}

	CRect rcLeftAll;
	CRect rcRightAll;
	rcLeftAll = rcLeft;
	rcRightAll = rcRight;
	rcLeftAll.right = rcLeftRnd.right;
	rcRightAll.right = rcRightRnd.right;

	roiFoilExp.SetRect(rcLeftAll);
	roiDross.SetRect(rcLeftAll);

	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecLeftRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFoileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecLeftRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, nMaskRight, en_ModeFoilExpOut, FALSE);

	roiFoilExp.SetRect(rcRightAll);
	roiDross.SetRect(rcRightAll);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecRightRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFoileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecRightRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, nMaskRight, en_ModeFoilExpOut, FALSE);

#if defined( SAVE_TACT_LOG ) 
	double dThresTime = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());

	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, pRecipeInfo->nFoilExpOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

#if defined( SAVE_TACT_LOG ) 
	double dLabelTime = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	pTabRsltInfo->RemoveAll();
	// 22.07.20 Ahn Move Start
	//if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
	//	CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
	//	CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);
	// 22.07.20 Ahn Move End

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) {
		if (nMode == TYPE_FOILEXP_OUT) {
			pVecBlockPtr = &vecBlockDross;
		}
		else {
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			CPoint ptCenter;
			ptCenter.x = (pData->rcRect.right + pData->rcRect.left) / 2;
			ptCenter.y = (pData->rcRect.bottom + pData->rcRect.top) / 2;
			// 22.11.18 Ahn Add Start
			//pTabRsltInfo->m_nFrameCount
			// 22.11.18 Ahn Add End

			if (rcLeftRnd.PtInRect(ptCenter) == TRUE) {
				pData->nDefPos = en_LeftRound; // 22.08.25 Ahn Add
				// 22.09.20 Ahn Test Start
				CImageProcess::CalcRoundDef_Size_LeftFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcLeftRnd, pData, &vecLeftRndInfo, CImageProcess::en_FindLeft, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
				//CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcLeftRnd, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
				// 22.09.20 Ahn Test End
			}
			else if (rcRightRnd.PtInRect(ptCenter) == TRUE) {
				pData->nDefPos = en_RightRound; // 22.08.25 Ahn Add
				CImageProcess::CalcRoundDef_Size_RightFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcRightRnd, pData, &vecRightRndInfo, CImageProcess::en_FindRight, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
			} else {
				// Round 결함이 아닌 경우, Edge에서 거리를 계산, 3 점의 위치를 측정하여 가장 큰 값을 취득함.
				//22.09.15 Ahn Modify End
				//if (AprData.m_System.m_nMachineMode == ANODE_MODE) { // 22.01.06 Ahn Add Start
				if (AprData.m_System.m_nMachineMode == CATHODE_MODE) { // 22.01.06 Ahn Add Start
				//22.09.15 Ahn Modify End
					if (pData->rcRect.top >= vecRightRndInfo[0].y) {
						CImageProcess::CalcSizeToEdgeLine(pData, &vecRightRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
						pData->nDefPos = en_Right;
					}
					else {
						CImageProcess::CalcSizeToEdgeLine(pData, &vecLeftRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
						pData->nDefPos = en_Left;
					}
				}
			}
		}
	}

	// 22.05.24 Ahn Modify Start
	// 결함 Merge후 설정 개수 이하 제거 처리 추가
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP);
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockDross, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP);

	// 22.05.24 Ahn Add Start
	// 22.07.20 Ahn Modify Start
	//CImageProcess::SortingBlockInfo(&vecBlockFoilExp);
	//CImageProcess::SortingBlockInfo(&vecBlockDross);

	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	//CImageProcess::MergeBlockList(&vecBlockMerge, vecBlockFoilExp, vecBlockDross);
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, pRecipeInfo, CAM_POS_TOP );
	//CImageProcess::SortingBlockInfo(&vecBlockMerge); // 22.11.07 Ahn Delete
	// 22.07.20 Ahn Modify End

	// 22.11.21 Ahn Modify Start
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT);
	// 22.12.01 Ahn Modify Start
	//int nFrameStartPos = pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight ;
	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	// 22.12.01 Ahn Modify End
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);
	// 22.11.21 Ahn Modify End
	// 22.05.24 Ahn Modify End

#if defined( SAVE_TACT_LOG ) 
	double dDefCollectTime = ctAna.WhatTimeIsIt_Double();
	CString strTactLog;
	strTactLog.Format(_T("MeanTime[%.2lf], EdgeTime[%.2lf], Edge2[%.2lf], Edge3[%.2lf], Thres[%.2lf], Labeling[%.2lf], Defect Collect TIme [%.2lf]")
		, dMeanProc, dEdgeDtImage, dEdgeDtImage2, dEdgeDtImage3, dThresTime, dLabelTime, dDefCollectTime);
	AprData.SaveTactLog(strTactLog);
#endif

	if (bSimMode == TRUE) {
		BYTE* pBndPtr = NULL;
		pBndPtr = new BYTE[nWidth * nHeight];
		memset(pBndPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecLeftRndInfo, 0x80);
		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecRightRndInfo, 0x80);
		memcpy(pImgPtrArr[0], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[1], pBndPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[2], pDiffPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[3], pThresPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[4], pStdPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[5], pProcPtr, sizeof(BYTE) * nWidth * nHeight);

		delete[] pBndPtr;
		pBndPtr = NULL;
	}

	delete[]pStdPtr;
	pStdPtr = NULL;
	delete[]pProcPtr;
	pProcPtr = NULL;

	delete[]pMeanPtr;
	pMeanPtr = NULL;
	delete[]pThresPtr;
	pThresPtr = NULL;
	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		delete[] pDiffPtr;
		pDiffPtr = NULL;
	}
	return 0;
}


int CImageProcess::ImageProcessBottomSide_AreaDiff(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);
	CRect rcAll;
	rcAll.left = nLineLevel - pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM] * 2;
	rcAll.right = nLineLevel + pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	rcAll.top = 0;
	rcAll.bottom = nHeight;

	if (nLineLevel < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<ImageProcessBottomSide_AreaDiff>>에러 - !!! Tab Level 이상 !!!"));

		return -1;
	}
	if (rcAll.left < 0) {
		rcAll.left = 0;
	}
	if (rcAll.right >= nWidth) {
		rcAll.right = (nWidth - 1);
	}

	int nSizeAll = nWidth * nHeight;
	BYTE* pMeanPtr = new BYTE[nSizeAll];
	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll);

	CImageProcess::VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nEdgeWidth = 10;

	// 평균화 처리 
#if defined( SAVE_TACT_LOG )
	CTimeAnalyzer ctAna;
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	int nMeanSize = MEAN_SIZE_HOR;

	BYTE* pStdPtr = new BYTE[nHeight * nWidth];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	// 22.04.26 Ahn Add Start
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);
	// 22.04.26 Ahn Add End

	BYTE* pDiffPtr;
	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		pDiffPtr = new BYTE[nSizeAll];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_BOTTOM];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, pRecipeInfo->bDarkEmpMode);
	}
	//else {
	//	pDiffPtr = pImgPtr;
	//}
#if defined( SAVE_TACT_LOG )
	double dMean = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	// 22.01.06 Ahn Modify Start
	BYTE* pEdgePtr ;
	// 22.03.30 Ahn Delete End
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	//	pEdgePtr = pMeanPtr;
	//}
	//else 
	// 22.03.30 Ahn Delete End
	{
		// 22.04.26 Ahn Modify Start
		//pEdgePtr = pStdPtr;
		pEdgePtr = pMeanPtr;
		// 22.04.26 Ahn Modify Start
	}
	// 22.01.06 Ahn Modify End

	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM] == CImageProcess::en_Edge_MaxDiff) {
		EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nEdgeWidth, 2, DIR_VER);
	}
	else {
		int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
		CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, CImageProcess::en_BottomSide, nLineLevel, CImageProcess::en_FindLeft);
	}

	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft);


#if defined( SAVE_TACT_LOG )
	double dEdge = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nWidth * nHeight];
	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
	roiDross.SetRoiPtr(pThresPtr);
	roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	roiFoilExp.SetRect(rcAll);
	roiDross.SetRect(rcAll);

	int nDrossThLower = pRecipeInfo->nDrossThresLower[CAM_POS_BOTTOM];
	int nFoilExpThLower = pRecipeInfo->nFoilExpThresLower[CAM_POS_BOTTOM];
	int nFoilOutMaskOffset = pRecipeInfo->nFoilOutMaskOffset[CAM_POS_BOTTOM];
	int nFileExpMaskOffset = pRecipeInfo->nFoilExpMaskOffset[CAM_POS_BOTTOM];

	int nFoilOutInspWidth = pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	int nFoilExpInspWidth = pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	int nRet = 0;

	nRet |= CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecAllRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, 0, en_ModeFoilExp, FALSE, CImageProcess::en_TailSide);
	nRet |= CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecAllRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, 0, en_ModeFoilExpOut, FALSE, CImageProcess::en_TailSide);

#if defined( SAVE_TACT_LOG )
	double dThres = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	//CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());

	// Overflow 는 4번 Bit살려서 보냄
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, AprData.m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, AprData.m_pRecipeInfo->nFoilExpOutMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);

#if defined( SAVE_TACT_LOG )
	double dLabel = ctAna.WhatTimeIsIt_Double();

	CString strLog;
	strLog.Format(_T(" Bottom Side Mean[%.2lf], Edge[%.2lf], Thres[%.2lf], Label[%.2lf]"), dMean, dEdge, dThres, dLabel);
	AprData.SaveTactLog(strLog);
#endif

	pTabRsltInfo->RemoveAll();

	// 22.07.20 Ahn Delete Start
	//if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_BOTTOM] == TRUE) {
	//	CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_BOTTOM);
	//	CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_BOTTOM);
	//}
	// 22.07.20 Ahn Delete End

	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) {
		if (nMode == TYPE_FOILEXP_OUT) {
			pVecBlockPtr = &vecBlockDross;
		}
		else {
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			//22.09.15 Ahn Modify End
			//if (AprData.m_System.m_nMachineMode == ANODE_MODE) { // 22.01.06 Ahn Add Start
			if (AprData.m_System.m_nMachineMode == CATHODE_MODE) { 
			//22.09.15 Ahn Modify End
				CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
			}
		}
	}

	// 22.07.20 Ahn Delete End
	//// 22.05.25 Ahn Modify Start
	////CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM);
	////CImageProcess::AddDefectInfoByBlockInfo(&vecBlockDross, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM);
	//CImageProcess::SortingBlockInfo(&vecBlockFoilExp);
	//CImageProcess::SortingBlockInfo(&vecBlockDross);
	//CImageProcess::_VEC_BLOCK vecBlockMerge;
	//vecBlockMerge.clear();
	//CImageProcess::MergeBlockList(&vecBlockMerge, vecBlockFoilExp, vecBlockDross);
	// 22.07.20 Ahn Delete End
	// 22.07.20 Ahn Add Start
	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, pRecipeInfo, CAM_POS_BOTTOM);
	CImageProcess::SortingBlockInfo(&vecBlockMerge);

	// 22.07.20 Ahn Add End
	// 22.11.21 Ahn Modify Start
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT);
	// 22.12.01 Ahn Modify Start
	//int nFrameStartPos = pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight ;
	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	// 22.12.01 Ahn Modify End
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);
	// 22.11.21 Ahn Modify End

	// 22.05.25 Ahn Modify Start

	if (bSimMode == TRUE) {
		BYTE* pBndPtr = NULL;
		pBndPtr = new BYTE[nWidth * nHeight];
		memset(pBndPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecAllRndInfo, 0x80);
		memcpy(pImgPtrArr[0], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[1], pBndPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[2], pDiffPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[3], pThresPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[4], pStdPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[5], pProcPtr, sizeof(BYTE) * nWidth * nHeight);
		//	memcpy(pImgPtrArr[7], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);

		delete[] pBndPtr;
		pBndPtr = NULL;
	}

	delete[] pStdPtr;
	pStdPtr = NULL;
	delete[] pProcPtr;
	pProcPtr = NULL;

	if (pThresPtr != nullptr) {
		delete[]pThresPtr;
		pThresPtr = NULL;
	}
	if (pMeanPtr != nullptr) {
		delete[]pMeanPtr;
		pMeanPtr = NULL;
	}
	//if (pRecipeInfo->bDisableProcDiff == FALSE) {
	if( pDiffPtr != nullptr ){
		delete[] pDiffPtr;		
		pDiffPtr = NULL;
	}
//	int nFoilExpSize = (int)vecBlockFoilExp.size();
//	int nDrossSize = (int)vecBlockDross.size();

	return nRet;

}

// Head부 검사 처리 함수.
// 22.02.08 Ahn Add Start
int CImageProcess::ImageProcessTopSide_Negative(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

#if defined( SAVE_TACT_LOG ) 
	CTimeAnalyzer ctAna;
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	int nOffset = 50;

	int nLeftOffset;
	
// 22.06.22 Ahn Delete Start
//	int nMeanLeftOffest;
//	int nMeanRightOffset;
// 22.06.22 Ahn Delete End
	int nTabRoundOffsetR;

	nLeftOffset = pRecipeInfo->TabCond.nNegCoatHeight + pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	// 22.06.22 Ahn Delete Start
	//nMeanLeftOffest = pRecipeInfo->TabCond.nNegCoatHeight;
	//nMeanRightOffset = 50;
	// 22.06.22 Ahn Delete End
	nTabRoundOffsetR = pRecipeInfo->TabCond.nNegCoatHeight;

	CRect rcAll;
	CRect rcLeft;
	CRect rcRight;
	rcAll.left = rcRight.left = rcLeft.left = nLineLevel - nLeftOffset;
	rcAll.right = nLineLevel + (int)(pRecipeInfo->TabCond.nRadiusW);
	rcRight.right = rcLeft.right = nLineLevel + 100;

	// 22.06.22 Ahn Delete Start
	//CRect rcEdgeMean;
	//rcEdgeMean.bottom = nHeight;
	//rcEdgeMean.top = 0;
	//rcEdgeMean.left = nLineLevel - nMeanLeftOffest;
	//rcEdgeMean.right = nLineLevel + nMeanRightOffset;
	// 22.06.22 Ahn Delete Start

	if ((rcAll.right > nWidth) || (rcAll.left < 0)) {
		return -1;
	}

	rcAll.top = rcLeft.top = 0;
	rcAll.bottom = rcRight.bottom = nHeight;
	rcLeft.bottom = nTabLeft + 50; //pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	rcRight.top = nTabRight - 50;//pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CRect rcLeftRnd;
	CRect rcRightRnd;
	rcLeftRnd = rcLeft;
	rcRightRnd = rcRight;
	rcLeftRnd.right = rcRightRnd.right = nLineLevel + nTabRoundOffsetR; //pRecipeInfo->TabCond.nRadiusW;// Tab Level 필요 
	rcLeftRnd.top = rcLeft.bottom - pRecipeInfo->TabCond.nRadiusH;
	rcRightRnd.bottom = rcRightRnd.top + pRecipeInfo->TabCond.nRadiusH;

	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight); // 22.06.22 Ahn Add
	CImageProcess::CheckRect(&rcLeft, nWidth, nHeight);
	CImageProcess::CheckRect(&rcLeftRnd, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRight, nWidth, nHeight);
	CImageProcess::CheckRect(&rcRightRnd, nWidth, nHeight);

	int* pnResultArr;
	pnResultArr = new int[nHeight];
	memset(pnResultArr, 0, sizeof(int) * nHeight);

	CImageProcess::VEC_ROUND_INFO vecLeftRndInfo;
	CImageProcess::VEC_ROUND_INFO vecRightRndInfo;
	vecLeftRndInfo.clear();
	vecRightRndInfo.clear();
	int nOffsetX = 0;
	int nOffsetY = 0;

	int nSizeAll = nWidth * nHeight;
	BYTE* pMeanPtr = new BYTE[nSizeAll];

	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll);
	int nMeanSize = MEAN_SIZE_HOR;

	//int nHeadBndry = nLineLevel + pRecipeInfo->TabCond.nTabCeramicHeight;
	int nHeadBndry;
	// 22.06.22 Ahn Delete Start
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	//	CImageProcess::MeanImageDirection(pImgPtr, pMeanPtr, nWidth, nHeight, rcAll, nMeanSize, DIR_VER);
	//	nHeadBndry = nLineLevel + pRecipeInfo->TabCond.nTabCeramicHeight;
	//}
	//else 
	// 22.06.22 Ahn Delete Start
	{
		nHeadBndry = nLineLevel + nTabRoundOffsetR;

	}

	int nMaskRight = nHeadBndry - 30;

	BYTE* pStdPtr = new BYTE[nHeight * nWidth];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);
	// 22.05.30 Ahn Delete Start
	//CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeftRnd, 15, 15, CImageProcess::en_FillTop);
	//CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRightRnd, 15, 15, CImageProcess::en_FillBottom);
	// 22.05.30 Ahn Delete End

	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);
	// 22.05.30 Ahn Delete Start
	//CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeftRnd, 3, 3, CImageProcess::en_FillTop);
	//CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRightRnd, 3, 3, CImageProcess::en_FillBottom);
	// 22.05.30 Ahn Delete End

	// 22.05.30 Ahn Add Start 
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);
	//CImageProcess::GetMaxImage(pMaxnPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	//CImageProcess::GetMaxImage(pMaxnPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);
	// 22.05.30 Ahn Add End

	// Mean처리한 Image로 Tab의 Center 에서 Tab의 높이를 구함.

#if defined( SAVE_TACT_LOG ) 
	double dMeanProc = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	int nRoundOffset = 50;	// 라운드 부분이 Slip 발생으로 늘어질경우를 대비한 Offset값
	int nEdgeCompWidth = 10;	//	Edge 검출 Parameter 상하 비교 폭

	// Tab 왼쪽(위) 부분의 Edge 검출
	CRect rcTemp;
	rcTemp = rcLeft;
	rcTemp.bottom = rcLeftRnd.top;
	rcTemp.right = rcLeftRnd.right;
	int nCropWidth = rcLeftRnd.Width();
	int nCropHeight = rcLeftRnd.Height();
	int nLocalRet = 0;

	BYTE* pEdgePtr;
	// 22.05.30 Ahn Modify Start
	//pEdgePtr = pProcPtr;
	pEdgePtr = pMeanPtr;
	// 22.05.30 Ahn Modify End


	int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];
	int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; // 22.05.30 Ahn Add
	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nEdgeCompWidth, 2, DIR_VER);
	}
	else {
		// 22.05.30 Ahn Modify Start
		nLocalRet = CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, NULL, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindLeft);
		// 22.05.30 Ahn Modify End
	}

	if (nLocalRet < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_Negative>>에러 - Edge found faile left round, ret = %d"), nLocalRet);

		// 22.04.18 Ahn Add Start
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[]pnResultArr;
		// 22.04.18 Ahn Add End
		return -1;
	}
#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	// Round는 가로 세로 2번의 Edge 검출이 필요함.
	rcLeftRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, NULL, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft, CImageProcess::en_FindFromLeft);
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize > 0) {
		rcLeft.top = vecLeftRndInfo[0].y;
		rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	}
	else {
		//	AprData.SaveErrorLog();
		//	ASSERT(FALSE);
		// 22.04.18 Ahn Add Start
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[]pnResultArr;
		// 22.04.18 Ahn Add End
		return -2; // 처리 불가.
	}

#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage2 = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	// Tab 오른쪽(아래) 부분의 Edge 검출

	nCropWidth = rcRightRnd.Width();
	nCropHeight = rcRightRnd.Height();
	// Round는 가로 세로 2번의 Edge 검출이 필요함.
	rcRightRnd.right = nMaskRight;
	//CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, NULL, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight);
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, NULL, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight, CImageProcess::en_FindFromLeft);


	rcTemp = rcRight;
	rcTemp.top = rcRightRnd.bottom;
	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcTemp, nEdgeCompWidth, 2, DIR_VER);
	}
	else {
		// 22.05.30 Ahn Modify Start
		nLocalRet = CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcTemp, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);
		// 22.05.30 Ahn Modify End
	}

	if (nLocalRet < 0) {
		
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_Negative>>에러 - Edge found faile right round, ret = %d"), nLocalRet);

		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[]pnResultArr;
		// 22.04.18 Ahn Add End
		return -2;
	}
	CImageProcess::SmoothVecRoundData(&vecRightRndInfo, CImageProcess::en_FindRight);

	int nRightSize = (int)vecRightRndInfo.size();
	if (nRightSize > 0) {
		rcRight.top = vecRightRndInfo[0].y;
		rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	}
	else {
		//	ASSERT(FALSE);
		// 22.04.18 Ahn Add Start
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[]pnResultArr;
		// 22.04.18 Ahn Add End
		return -3;
	}

#if defined( SAVE_TACT_LOG ) 
	double dEdgeDtImage3 = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	//	delete[]pRsltPtr;
	delete[]pnResultArr;

	BYTE* pThresPtr = new BYTE[nSizeAll];
	BYTE* pDiffPtr;

	// 22.02.08 Ahn Modify Start
	CRegionInfo roiInfo;
	CString strThres;

	memset(pThresPtr, 0x00, sizeof(BYTE)* nWidth* nHeight);
	roiInfo.SetRoiPtr(pThresPtr);
	roiInfo.SetProcBit(CImageProcess::en_FoilExp_Bit);
	// 22.02.08 Ahn Modify End

	// 21.08.25 Ahn Add Start
	rcRight.right = rcLeft.right = nHeadBndry;
	// 21.08.25 Ahn Add End

	int nDrossThLower = pRecipeInfo->nDrossThresLower[CAM_POS_TOP];
	int nFoilExpThLower = pRecipeInfo->nFoilExpThresLower[CAM_POS_TOP];
	int nStartX = pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	int nEndX = pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];

	CRect rcLeftDiff = rcLeft;
	CRect rcRightDiff = rcRight;

	rcLeftDiff.bottom = rcLeftRnd.top;
	rcRightDiff.top = rcRightRnd.bottom;
	rcLeftDiff = rcLeft;
	rcRightDiff = rcRight;

	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		pDiffPtr = new BYTE[nSizeAll];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll);
		//	CImageProcess::MeanImageDirection_Round(pImgPtr, pMeanPtr, nWidth, nHeight, rcLeftRnd, 40, pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]);
		//	CImageProcess::MeanImageDirection_Round(pImgPtr, pMeanPtr, nWidth, nHeight, rcRightRnd, 40, pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftDiff, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightDiff, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
	}

	{
		roiInfo.SetRect(rcLeft);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecLeftRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
		roiInfo.SetRect(rcRight);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecRightRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
	}

	// 22.02.08 Ahn Add End

#if defined( SAVE_TACT_LOG ) 
	double dThresTime = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	vecBlockFoilExp.clear();

	// 22.02.08 Ahn Modify Start
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiInfo.GetFifoPtr());
	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, pRecipeInfo->nFoilExpOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	// 22.02.08 Ahn Modify End

#if defined( SAVE_TACT_LOG ) 
	double dLabelTime = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	pTabRsltInfo->RemoveAll();
	if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
	//	CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);	// 
	}

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	//for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) 
	{
		pVecBlockPtr = &vecBlockFoilExp ;

		int nDefSize = (int)pVecBlockPtr->size();
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			//CPoint ptCenter;
			//ptCenter.x = (pData->nRight + pData->nLeft) / 2;
			//ptCenter.y = (pData->nBottom + pData->nTop) / 2;

			CPoint cpLT;
			CPoint cpLB;
			cpLB.x = cpLT.x = pData->rcRect.left;
			cpLT.y = pData->rcRect.top;
			cpLB.y = pData->rcRect.bottom;

			CRect rcDef;
			CRect rcTemp;
			// 22.07.20 Ahn Modify Start
			//rcDef.SetRect(pData->rcRect.left, pData->rcRect.top, pData->rcRect.right, pData->rcRect.bottom);
			rcDef = pData->rcRect;
			// 22.07.20 Ahn Modify End

			// 22.02.07 Ahn Modify Start
			if (rcLeft.PtInRect(cpLT) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecLeftRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				//if (rcTemp.IntersectRect(rcLeftRnd, rcDef) == TRUE) 
				if (rcLeftRnd.PtInRect(cpLT) == TRUE)
				{
					CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
				}
				else {
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
			else if (rcRight.PtInRect(cpLB) == TRUE) {
				CImageProcess::CheckDistanceToEdgeLine(pData, &vecRightRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				if (rcLeftRnd.PtInRect(cpLB) == TRUE)
				{
					CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindRight);
				}
				else {
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
		}
	}

	// 22.11.21 Ahn Modify Start
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT);
	// 22.12.01 Ahn Modify Start
	//int nFrameStartPos = pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight ;
	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	// 22.12.01 Ahn Modify End
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);
	// 22.11.21 Ahn Modify End
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockDross, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP); // 22.02.08 Ahn Delete

#if defined( SAVE_TACT_LOG ) 
	double dDefCollectTime = ctAna.WhatTimeIsIt_Double();
	CString strTactLog;
	strTactLog.Format(_T("MeanTime[%.2lf], EdgeTime[%.2lf], Edge2[%.2lf], Edge3[%.2lf], Thres[%.2lf], Labeling[%.2lf], Defect Collect TIme [%.2lf]")
		, dMeanProc, dEdgeDtImage, dEdgeDtImage2, dEdgeDtImage3, dThresTime, dLabelTime, dDefCollectTime);
	AprData.SaveTactLog(strTactLog);
#endif

	if (bSimMode == TRUE) {
		BYTE* pBndPtr = NULL;
		pBndPtr = new BYTE[nWidth * nHeight];
		memset(pBndPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecLeftRndInfo, 0x80);
		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecRightRndInfo, 0x80);
		memcpy(pImgPtrArr[0], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[1], pBndPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[2], pDiffPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[3], pThresPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[4], pStdPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[5], pProcPtr, sizeof(BYTE) * nWidth * nHeight);
		//	memcpy(pImgPtrArr[7], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);

		delete[] pBndPtr;
		pBndPtr = NULL;
	}

	delete[]pStdPtr;
	pStdPtr = NULL;
	delete[]pProcPtr;
	pProcPtr = NULL;

	delete[]pMeanPtr;
	pMeanPtr = NULL;
	delete[]pThresPtr;
	pThresPtr = NULL;
	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		delete[] pDiffPtr;
		pDiffPtr = NULL;
	}
	return 0;
}


int CImageProcess::ImageProcessBottomSide_Negative(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);
	CRect rcAll;
	//rcAll.left = nLineLevel - pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM] * 2;
	//rcAll.right = nLineLevel + pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	rcAll.left = nLineLevel - pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	rcAll.right = nLineLevel + pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM] + pRecipeInfo->TabCond.nNegVGrooveHeight;


	rcAll.top = 0;
	rcAll.bottom = nHeight;

	if (nLineLevel < 0) {

		//DEBUG_LOG.txt
		AprData.SaveDebugLog(_T("<<ImageProcessBottomSide_Negative>>에러 - !!! Tab Level 이상 !!!"));

		return -1;
	}
	if (rcAll.left < 0) {
		rcAll.left = 0;
	}
	if (rcAll.right >= nWidth) {
		rcAll.right = (nWidth - 1);
	}

	int nSizeAll = nWidth * nHeight;
	BYTE* pTempPtr = new BYTE[nSizeAll];
	memset(pTempPtr, 0x00, sizeof(BYTE) * nSizeAll);

	CImageProcess::VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nEdgeWidth = 10;

	// 평균화 처리 
	BYTE* pMeanPtr = pTempPtr;
#if defined( SAVE_TACT_LOG )
	CTimeAnalyzer ctAna;
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	int nMeanSize = MEAN_SIZE_HOR;

	// 22.09.15 Ahn Modify Start
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	// 22.09.15 Ahn Modify End
		MeanImageDirection(pImgPtr, pMeanPtr, nWidth, nHeight, rcAll, nMeanSize, DIR_VER);
	}

	BYTE* pStdPtr = new BYTE[nHeight * nWidth];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	// 22.05.30 Ahn Add Start
	CImageProcess::GetMinImage( pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);
	// 22.05.30 Ahn Add End

	BYTE* pDiffPtr;
	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		pDiffPtr = new BYTE[nSizeAll];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_BOTTOM];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, pRecipeInfo->bDarkEmpMode);
	}
	//else {
	//	pDiffPtr = pImgPtr;
	//}
#if defined( SAVE_TACT_LOG )
	double dMean = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif

	// 22.05.30 Ahn Add Start
	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr;
	// 22.05.30 Ahn Add End

	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM] == CImageProcess::en_Edge_MaxDiff) {
		EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nEdgeWidth, 2, DIR_VER);
	}
	else {
		int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
		int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; // 22.05.30 Ahn Add

		// 22.05.30 Ahn Modify Start
		CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, CImageProcess::en_BottomSide, nLineLevel, nThresMax, CImageProcess::en_FindLeft);
		// 22.05.30 Ahn Modify End
	}

	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft);


#if defined( SAVE_TACT_LOG )
	double dEdge = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CRegionInfo roiFoilExp;
//	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nWidth * nHeight];
	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	roiFoilExp.SetRoiPtr(pThresPtr);
	roiFoilExp.SetProcBit(CImageProcess::en_FoilExp_Bit);
//	roiDross.SetRoiPtr(pThresPtr);
//	roiDross.SetProcBit(CImageProcess::en_Dross_Bit);

	roiFoilExp.SetRect(rcAll);
//	roiDross.SetRect(rcAll);

	int nDrossThLower = pRecipeInfo->nDrossThresLower[CAM_POS_BOTTOM];
	int nFoilExpThLower = pRecipeInfo->nFoilExpThresLower[CAM_POS_BOTTOM];
	int nFoilOutMaskOffset = pRecipeInfo->nFoilOutMaskOffset[CAM_POS_BOTTOM];
	int nFileExpMaskOffset = pRecipeInfo->nFoilExpMaskOffset[CAM_POS_BOTTOM];

	int nFoilOutInspWidth = pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM];
	int nFoilExpInspWidth = pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	int nRet = 0;

	//nRet |= CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecAllRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFileExpMaskOffset, 0, CImageProcess::en_ModeFoilExp, FALSE, CImageProcess::en_TailSide);
	//nRet |= CImageProcess::Threshold_RoundMask(pDiffPtr, &roiDross, &vecAllRndInfo, nWidth, nHeight, nFoilOutInspWidth, nDrossThLower, nFoilOutMaskOffset, 0, CImageProcess::en_ModeFoilExpOut, FALSE, CImageProcess::en_TailSide);
	{
		roiFoilExp.SetRect(rcAll);
		CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiFoilExp, &vecAllRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilOutInspWidth, nFoilExpThLower, rcAll.right, TRUE, CImageProcess::en_BottomSide);
	}

#if defined( SAVE_TACT_LOG )
	double dThres = ctAna.WhatTimeIsIt_Double();
	ctAna.Clear();
	ctAna.StopWatchStart();
#endif
	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	//CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	//CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());

	// Overflow 는 4번 Bit살려서 보냄
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, AprData.m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
	//CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, AprData.m_pRecipeInfo->nFoilOutMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);

#if defined( SAVE_TACT_LOG )
	double dLabel = ctAna.WhatTimeIsIt_Double();

	CString strLog;
	strLog.Format(_T(" Bottom Side Mean[%.2lf], Edge[%.2lf], Thres[%.2lf], Label[%.2lf]"), dMean, dEdge, dThres, dLabel);
	AprData.SaveTactLog(strLog);
#endif
	pTabRsltInfo->RemoveAll();

	// 22.07.22 Ahn Delete Start
	//if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_BOTTOM] == TRUE) {
	//	CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_BOTTOM);
	////	CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_BOTTOM);
	//}
	// 22.07.22 Ahn Delete End

	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	//for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) 
	{
		//if (nMode == TYPE_FOILEXP_OUT) {
		//	pVecBlockPtr = &vecBlockDross;
		//}
		//else 
		{
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
		//for (int i = 0; i < nDefSize; i++) {
		//	pData = &(*pVecBlockPtr)[i];
		//	if (pData->bDeleteFlag == TRUE) continue;
		//	if (AprData.m_System.m_nMachineMode == ANODE_MODE) { // 22.01.06 Ahn Add Start
		//		CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
		//	}
		//}
		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];

			CImageProcess::CheckDistanceToEdgeLine(pData, &vecAllRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);

			if (pData->bDeleteFlag == TRUE) continue;
			//if (AprData.m_System.m_nMachineMode == ANODE_MODE) 
			{
				//	CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
			}
		}
	}

	//// 22.07.20 Ahn Add Start
	//CImageProcess::_VEC_BLOCK vecBlockMerge;
	//vecBlockMerge.clear();
	//CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, pRecipeInfo, CAM_POS_BOTTOM);
	//CImageProcess::SortingBlockInfo(&vecBlockMerge);
	//// 22.07.20 Ahn Add End

	// 22.11.21 Ahn Modify Start
	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM, MAX_SAVE_DEFECT_COUNT);
	// 22.12.01 Ahn Modify Start
	//int nFrameStartPos = pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight ;
	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	// 22.12.01 Ahn Modify End
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);
	// 22.11.21 Ahn Modify End

	//CImageProcess::AddDefectInfoByBlockInfo(&vecBlockDross, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM);

	if (bSimMode == TRUE) {
		BYTE* pBndPtr = NULL;
		pBndPtr = new BYTE[nWidth * nHeight];
		memset(pBndPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);

		DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecAllRndInfo, 0x80);
		memcpy(pImgPtrArr[0], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[1], pBndPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[2], pDiffPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[3], pThresPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[4], pStdPtr, sizeof(BYTE) * nWidth * nHeight);
		memcpy(pImgPtrArr[5], pProcPtr, sizeof(BYTE) * nWidth * nHeight);
		//	memcpy(pImgPtrArr[7], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);

		delete[] pBndPtr;
		pBndPtr = NULL;
	}

	delete[] pStdPtr;
	pStdPtr = NULL;
	delete[] pProcPtr;
	pProcPtr = NULL;

	delete[]pThresPtr;
	pThresPtr = NULL;
	delete[]pTempPtr;
	pTempPtr = NULL;
	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		delete[] pDiffPtr;
		pDiffPtr = NULL;
	}
	//int nFoilExpSize = (int)vecBlockFoilExp.size();
	//int nDrossSize = (int)vecBlockDross.size();

	return nRet;

}
// 22.02.08 Ahn Add End

// 22.04.26 Ahn Add Start
int CImageProcess::GetMinImage(BYTE* pResultPtr, BYTE* pSrcPtr, BYTE* pTarPtr, int nWidth, int nHeight, CRect rcProc)
{
	int nStartX = rcProc.left;
	int nEndX = rcProc.right;
	int nStartY = rcProc.top;
	int nEndY = rcProc.bottom;

	if (nStartX < 0) { nStartX = 0; }
	if (nStartY < 0) { nStartY = 0; }
	if (nEndX > nWidth) { nEndX = nWidth; }
	if (nEndY > nHeight) { nEndY = nHeight; }

	BYTE* pMinLinetPtr;
	BYTE* pSrcLinePtr;
	BYTE* pTarLinePtr;

	int y, x;
	int nLineY;
	BYTE btSrc, btTar;
	for (y = nStartY; y < nEndY; y++) {
		nLineY = (nWidth * y);
		pMinLinetPtr = pResultPtr + nLineY ;
		pSrcLinePtr = pSrcPtr + nLineY ;
		pTarLinePtr = pTarPtr + nLineY ;
		for (x = nStartX; x < nEndX; x++) {
			btSrc = *(pSrcLinePtr + x);
			btTar = *(pTarLinePtr + x);
			if (btSrc > btTar) {
				*(pMinLinetPtr + x) = btTar;
			}
			else {
				*(pMinLinetPtr + x) = btSrc;
			}
		}
	}

	return 0;
}
// 22.04.26 Ahn Add Start
// 22.05.30 Ahn Add Start
int CImageProcess::GetMaxImage(BYTE* pResultPtr, BYTE* pSrcPtr, BYTE* pTarPtr, int nWidth, int nHeight, CRect rcProc)
{
	int nStartX = rcProc.left;
	int nEndX = rcProc.right;
	int nStartY = rcProc.top;
	int nEndY = rcProc.bottom;

	if (nStartX < 0) { nStartX = 0; }
	if (nStartY < 0) { nStartY = 0; }
	if (nEndX > nWidth) { nEndX = nWidth; }
	if (nEndY > nHeight) { nEndY = nHeight; }

	BYTE* pMaxLinetPtr;
	BYTE* pSrcLinePtr;
	BYTE* pTarLinePtr;

	int y, x;
	int nLineY;
	BYTE btSrc, btTar;
	for (y = nStartY; y < nEndY; y++) {
		nLineY = (nWidth * y);
		pMaxLinetPtr = pResultPtr + nLineY;
		pSrcLinePtr = pSrcPtr + nLineY;
		pTarLinePtr = pTarPtr + nLineY;
		for (x = nStartX; x < nEndX; x++) {
			btSrc = *(pSrcLinePtr + x);
			btTar = *(pTarLinePtr + x);
			if (btSrc < btTar) {
				*(pMaxLinetPtr + x) = btTar;
			}
			else {
				*(pMaxLinetPtr + x) = btSrc;
			}
		}
	}

	return 0;
}
// 22.05.30 Ahn Add End

// 22.05.09 Ahn Add Start
int CImageProcess::GetBoundaryOfElectorde(BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nFindDir)
{
	CRect rect;
	int nPrjWidth = 2000;
	rect.left = (nWidth - nPrjWidth );
	rect.right = nWidth;
	rect.top = 0;
	rect.bottom = nHeight - 1;
	int nSamplingSize = nHeight / 100;
	int* pnPrj = new int[nPrjWidth];

	//DarkRoll  사용여부 체크
	//레시피 정보에 Tab Condition Roll Bright Mode Top
	BOOL bUseDarkRoll = (pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;

	//프로젝션 이미지 데이터 생성 샘플링 값 높이/100
	CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
	//프로젝션 데이터로 바운드리 크기를 찾는다.
	int nBndElectrode = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP], nFindDir, bUseDarkRoll ) ;

	//찾는 위치가 Left
	if (nFindDir == en_FindFromLeft) {
		nBndElectrode += (nWidth - nPrjWidth);
	}

	delete[] pnPrj;

	return nBndElectrode;

}

int CImageProcess::GetBoundaryOfElectordeBottom(BYTE* pImgPtr, int nWidth, int nHeight, int *pnLevel, CRecipeInfo* pRecipeInfo)
{
	ASSERT(pRecipeInfo);
	ASSERT(pnLevel);

	CRect rect;
	int nPrjWidth = 2000;
	rect.left = 0;
	rect.right = nPrjWidth ;
	rect.top = 0;
	rect.bottom = nHeight - 1;
	int nSamplingSize = nHeight / 100;
	int* pnPrj = new int[nPrjWidth];

	CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
	// 23.02.24 Ahn Modify Start
	//int nBndElectrode = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM], CImageProcess::en_FindFromRight);
	//
	//*pnLevel = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM], CImageProcess::en_FindFromLeft);
	
	BOOL bUseDarkRoll = (pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_BOTTOM] == 1) ? FALSE : TRUE;

	int nBndElectrode = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM], CImageProcess::en_FindFromRight, bUseDarkRoll);
	*pnLevel = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM], CImageProcess::en_FindFromLeft, bUseDarkRoll);
	// 23.02.24 Ahn Modify End

	//int nBndElectrode = *pnLevel + pRecipeInfo->TabCond.nCeramicTailHeight;

	delete[] pnPrj;

	return nBndElectrode;

}

// 22.05.09 Ahn Add End


	// 22.07.14 Ahn Add Start
int CImageProcess::FindEdgePosFromProjection(int* pnPrjData, int nWidth, int nThreshold)
{
	int nEdgePos = -1;

	for (int i = nWidth - 1; i > 0; i-- ) {
		if (pnPrjData[i] > nThreshold) {
			nEdgePos = i ;
			break;
		}
	}


	return nEdgePos;
}

int CImageProcess::SearchEdgeLine(BYTE* pMeanPtr, int nWidth, int nHeight, CRect rcRect, CImageProcess::VEC_ROUND_INFO* vecEdgeInfo, int& nAveLine,  int nEdgeThreshold, int nDir)
{
	int nRet = 0; 

	CheckRect(&rcRect, nWidth, nHeight);
	int y;
	CPoint cpnt;

	int* pnAveArr = new int[nHeight];
	int* pnEdgeLine = new int[nHeight];
	memset(pnEdgeLine, 0x00, sizeof(int) * nHeight);
	memset(pnAveArr, 0x00, sizeof(int) * nHeight);

	if (nDir == en_FindFromLeft) {
		int nStartPos = rcRect.left;
		int nEndPos = rcRect.right - 1;

		for (y = 0; y < nHeight; y++) {
			BYTE* pLinePtr = pMeanPtr + (nWidth * y);
			int nFindPos = -1;
			for (int x = nStartPos; x < nEndPos; x++) {
				if (*(pLinePtr + x) >= nEdgeThreshold) {
					nFindPos = x; 
					break;
				}
			}
			pnEdgeLine[y] = nFindPos;
		}
	}
	else {
		int nStartPos = rcRect.right - 1;
		int nEndPos = rcRect.left;

		for (y = 0; y < nHeight; y++ ) {
			BYTE* pLinePtr = pMeanPtr + (nWidth * y);
			int nFindPos = -1;
			for (int x = nStartPos; x >= nEndPos; x--) {
				if (*(pLinePtr + x) >= nEdgeThreshold) {
					nFindPos = x;
					break;
				}
			}
			pnEdgeLine[y] = nFindPos;
		}
	}

	int nLeftSum = 0;
	int nRightSum = 0;
	int nMeanSize = 20;
	int nHalf = nMeanSize / 2;
	int i;
	// 0 ~ 9 , 11~20
	for (i = 0; i < nHalf; i++) {
		nLeftSum += pnEdgeLine[i];
		nRightSum += pnEdgeLine[i + nHalf + 1];
	}
	// 10 ~ End - 10 
	for (i = nHalf; i < (nHeight - nHalf - 1); i++) {
		pnAveArr[i] = (nLeftSum + nRightSum) / nMeanSize;
		nLeftSum = nLeftSum - pnEdgeLine[i - nHalf] + pnEdgeLine[i];
		nRightSum = nRightSum - pnEdgeLine[i + 1] + pnEdgeLine[i + nHalf + 1];
	}

	pnAveArr[i] = (nLeftSum + nRightSum) / nMeanSize;

	for (i = nHalf; i < nHeight - nHalf; i++) {
		if (abs(pnEdgeLine[i] - pnAveArr[i]) > 10) {
			pnEdgeLine[i] = pnAveArr[i];
		}
	}

	vecEdgeInfo->clear();
	int nSumLine = 0;
	for (int i = 0; i < nHeight; i++) {
		CPoint cpnt;
		cpnt.x = pnEdgeLine[i];
		cpnt.y = i;
		vecEdgeInfo->push_back(cpnt);
		nSumLine += cpnt.x;
	}

	nAveLine = nSumLine / nHeight;

	delete[] pnAveArr;
	delete[] pnEdgeLine;

	return nRet ;
}

// 22.07.14 Ahn Add End


// 22.07.14 Ahn Add Start
int CImageProcess::Threshold_ByEdgeLine(BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nInspWidth, int nMinThres, int nMaxThres, int nMaskOffset, int nMode, BOOL bClearRslt, BOOL bModeSide )
{
	ASSERT(pImgPtr);
	ASSERT(pRoiInfo);

	BYTE* pRoiPtr = pRoiInfo->GetRoiPtr();
	BYTE btBit = pRoiInfo->m_bProcPit;

	ASSERT(pRoiPtr);
	ASSERT(vecLine);

	int x, y;
	int nStartX, nStartY, nEndX, nEndY;
	nStartX = pRoiInfo->m_nLeft;
	nStartY = pRoiInfo->m_nTop;
	nEndX = pRoiInfo->m_nRight;
	nEndY = pRoiInfo->m_nBottom;

	if (vecLine->size() <= 0) return -1;

	ASSERT(pRoiPtr);
	BYTE* pRoiLinePtr;
	BYTE* pLinePtr;
	BYTE btOrg;
	DWORD dwCount = 0;
	if (bClearRslt == TRUE) {
		memset(pRoiPtr, 0x00, sizeof(BYTE) * nWidth * nHeight);
	}
	BOOL bOverflow = FALSE;
	int nBnd = 0;
	int mode;
	if (bModeSide == en_LeftSide) {
		if (nMode == en_ModeWave) {
			mode = en_ModeWave;
		}
		else {
			mode = en_ModeBur;
		}
	}
	else {
		mode = nMode;
	}
	int nSize = (int)vecLine->size();
	if (nSize < (nEndY - nStartY)) {
		//	ASSERT(FALSE);
		return -1;
	}
	if (mode == en_ModeWave) { // In
		for (y = nStartY, nBnd = 0; y < nEndY; y++, nBnd++) {
			pLinePtr = pImgPtr + (nWidth * y);
			pRoiLinePtr = pRoiPtr + (nWidth * y);
			nEndX = (*vecLine)[nBnd].x - nMaskOffset;
			int nInspStartX = nEndX - nInspWidth;
			if (nInspStartX < 0) nInspStartX = 0;

			if (MAX_BLOCK_PIXEL < dwCount) {
				// Overflow 
				bOverflow = TRUE;
				break;
			}

			for (x = nInspStartX; x < nEndX; x++) {
				btOrg = *(pLinePtr + x);
				if ( (btOrg >= (BYTE)nMinThres) && ( btOrg <= (BYTE)nMaxThres ) ){
					*(pRoiLinePtr + x) |= btBit;
					dwCount++;
					CPixel pnt;
					pnt.x = x;
					pnt.y = y;
					pnt.btBright = btOrg;
					pnt.btOrgBri = 0;
					pRoiInfo->GetFifoPtr()->push_back(pnt);
				}
			}
		}
	}
	else if (mode == en_ModeBur) { // Out 
		for (y = nStartY, nBnd = 0; y < nEndY; y++, nBnd++) {
			if (MAX_BLOCK_PIXEL < dwCount) {
				// Overflow 
				bOverflow = TRUE;
				break;
			}

			pLinePtr = pImgPtr + (nWidth * y);
			pRoiLinePtr = pRoiPtr + (nWidth * y);
			nStartX = (*vecLine)[nBnd].x + nMaskOffset  ;
			nEndX = nStartX + nInspWidth;

			for (x = nStartX; x < nEndX; x++) {
				if (pRoiInfo->m_nRight < x) break;
				btOrg = *(pLinePtr + x);
				if ((btOrg > (BYTE)nMinThres) && (btOrg < (BYTE)nMaxThres)) {
					*(pRoiLinePtr + x) |= btBit;
					dwCount++;
					CPixel pnt;
					pnt.x = x;
					pnt.y = y;
					pnt.btBright = (int)btOrg;
					pnt.btOrgBri = 0;
					pRoiInfo->GetFifoPtr()->push_back(pnt);
				}
			}
		}
	}

	if (bOverflow == TRUE) {
		return 0x0008;
	}
	return 0;
}
// 22.07.14 Ahn Add End

// 23.01.20 Ahn Add Start
int CImageProcess::EdgeDetectImageToBoth_BaseBright(BYTE* pImgPtr, BYTE* pProcImg, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, int nWidth, int nHeight, CRect rectProc, int nThresMin, int nThresMax, int nPosition, int nDefaultLv, int nSide)
{
	int x, y;
	int nDiff = 0;
	int nXStart, nXEnd, nYStart, nYEnd;

	if ((rectProc.bottom - rectProc.top) <= 0) {
		return -1;
	}

	int* pnRsltArr;
	pnRsltArr = new int[nHeight];
	memset(pnRsltArr, 0, sizeof(int) * nHeight);

	int nMaxPos[10];
	memset(nMaxPos, 0, sizeof(nMaxPos));

	int nSumCnt = 10;
	nXStart = rectProc.left + nSumCnt;
	nXEnd = rectProc.right - nSumCnt;
	nYStart = rectProc.top;
	nYEnd = rectProc.bottom;

	int nPitch = (nYEnd - nYStart) / 10;
	int nMinPosX = nXEnd;
	int nMaxPosX = nXStart;
	int nCnt = 0;

	if (pProcImg != NULL) {
		if (nPosition == CImageProcess::en_TopSide) { // 탑

			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				int nCnt = 0;

				for (x = nXEnd ; x > nXStart ; x--) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel <= nThresMax) {
						pnRsltArr[y] = x;
						bNotFound = FALSE;
						pProcLinePtr[x] = 255;
						break;
					}
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
					pProcLinePtr[nLastPosX] = 255;
				}
			}
		}
		else {
			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				BYTE* pProcLinePtr = pProcImg + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				int nCnt = 0;
				for (x = nXStart ; x < nXEnd; x++) {
					BYTE btLevel = *(pLinePtr + x);
					// 22.05.30 Ahn Modify Start
					if (btLevel <= nThresMax) {
						if (btLevel < nThresMin) {
							bNotFound = FALSE;
							nLastPosX = pnRsltArr[y] = x;
							pProcLinePtr[x] = 255;
							break;
						}
					}
					// 22.05.30 Ahn Modify End
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
					pProcLinePtr[nLastPosX] = 255;
				}

			}
		}
	}
	else {
		if (nPosition == CImageProcess::en_TopSide) { // 탑

			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;

				for (x = nXEnd; x > nXStart; x--) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel <= nThresMax) {
						pnRsltArr[y] = x;
						bNotFound = FALSE;
						//pProcLinePtr[x] = 255;
						break;
					}
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
					//pProcLinePtr[nLastPosX] = 255;
				}
			}
		}
		else {

			int nLastPosX = nDefaultLv;
			for (y = nYStart; y < nYEnd; y++) {
				BYTE* pLinePtr = pImgPtr + (y * nWidth);
				int nMax = 0;
				pnRsltArr[y] = nXStart;
				BOOL bNotFound = TRUE;
				for (x = nXStart; x < nXEnd; x++) {
					BYTE btLevel = *(pLinePtr + x);
					if (btLevel <= nThresMax) {
						if (btLevel < nThresMin) {
							bNotFound = FALSE;
							nLastPosX = pnRsltArr[y] = x;
							break;
						}
					}
				}

				if (bNotFound == TRUE) {
					pnRsltArr[y] = nLastPosX;
				}
			}
		}
	}


	CPoint cp;

	for (y = rectProc.top; y < rectProc.bottom; y++) {
		cp.x = pnRsltArr[y];
		cp.y = y;
		pVecRndInfo->push_back(cp);
	}

	int nSize = (int)pVecRndInfo->size();
	int nLeftDiff, nRightDiff;
	int nDiffRange = 3;
	for (y = 1; y < nSize - 1; y++) {
		nLeftDiff = abs((*pVecRndInfo)[y].x - (*pVecRndInfo)[y - 1].x);
		nRightDiff = abs((*pVecRndInfo)[y].x - (*pVecRndInfo)[y + 1].x);
		if ((nLeftDiff > nDiffRange) && (nRightDiff > nDiffRange)) {
			(*pVecRndInfo)[y].x = int(((*pVecRndInfo)[y - 1].x + (*pVecRndInfo)[y + 1].x) / 2);
		}
	}

	delete[]pnRsltArr;
	return 0;
}
// 23.01.20 Ahn Add End

// 23.02.16 Ahn Add Start
double CImageProcess::GetIqSharpnessValue(BYTE* pOrgImg, int nWidth, int nHeight, CPoint cpStartPoint)
{
	double dRet = 0.0;

	int nStartY = cpStartPoint.y ;
	if(nStartY < 0) { nStartY = 0; }

	int nEndY = nStartY + EDGE_IMG_HEIGHT ;
	if (nEndY > nHeight) { nEndY = nHeight; }

	int nStartX = cpStartPoint.x + 44;
	if (nStartX < 0) { nStartX = 0; }
	int nEndX = nStartX + 88 ;
	if (nEndX > nWidth) { nEndX = nWidth; }

	double dMax1, dMax2 = 0 , dDc = 0 ;
	for (int y = nStartY; y < nEndY; y++) {
		BYTE* pLinePtr = pOrgImg + (nWidth * y) ;
		dMax1 = 0 ;
		for (int x = nStartX ; x < nEndX; x++) {
			BYTE btFront = *(pLinePtr + x );
			BYTE btRear = *(pLinePtr + x + 1);
			double dSharpness = abs(btFront - btRear) * 12;

			if (dMax1 < dSharpness) dMax1 = dSharpness;
		}
		dMax2 += dMax1;
		dDc++;
	}
	dRet = dMax2 / dDc;

	return dRet;
}
int	CImageProcess::GetBrightAverage(BYTE* pOrgImg, int nWidth, int nHeight, CPoint cpStartPoint)
{
	int nRet = 0; 

	int nStartX = cpStartPoint.x;
	int nStartY = cpStartPoint.y;
	if (nStartX < 0) { nStartX = 0; }
	if (nStartY < 0) { nStartY = 0; }

	int nEndX = nStartX + EDGE_IMG_WIDTH;
	int nEndY = nStartY + EDGE_IMG_HEIGHT;
	if (nEndX > nWidth) { nEndX = nWidth; }
	if (nEndY > nHeight) { nEndY = nHeight; }


	int nSum = 0 ;
	int nCount = 0 ;
	for (int y = nStartY; y < nEndY; y++) {
		BYTE* pLinePtr = pOrgImg + (nWidth * y);
		for (int x = nStartX; x < nEndX; x++) {
			nSum += (int)*(pLinePtr + x) ;
			nCount++;
		}
	}
	nRet = nSum / nCount ;

	return nRet;
}
// 23.02.16 Ahn Add End 
