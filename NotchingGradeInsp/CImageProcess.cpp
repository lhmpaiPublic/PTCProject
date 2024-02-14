#include "pch.h"
#include "GlobalDef.h"
#include "CImageProcess.h"
#include "GlobalData.h" 
#include "CTabRsltInfo.h"
#include "CRecipeCtrl.h"
#include "TimeAnalyzer.h"
#include "BitmapStd.h"
#include "Win32File.h"
#include "LogDisplayDlg.h"

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
int CImageProcess::MeanImageDirection_Round(const BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc, int nMeanSize, int nThresLevel)
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

int CImageProcess::MeanImageDirection(const BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, CRect rectProc, int nMeanSize, int nDir)
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
			pLinePtr = (BYTE*)pImage + (nWidth * y);
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

int CImageProcess::MeanImageDirection(const BYTE* pImage, BYTE* pMeanImg, int nWidth, int nHeight, int nMeanSize, int nDir)
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


BOOL CImageProcess::ImageMean(const BYTE* pOrgPtr, BYTE* pTarPtr, int nWidth, int nHeight, int nMeanX, int nMeanY)
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

int CImageProcess::ImageMean_Part(const BYTE* pOrgPtr, BYTE* pTarPtr, int nWidth, int nHeight, CRect rcRange, int nMeanX, int nMeanY, int nMode )
{
	ASSERT(pOrgPtr);
	ASSERT(pTarPtr);

	int x, y;

	__int64 totalImageSize = nWidth * nHeight;
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
//=====================죽는 위치 -------------------------------------
	for (x = nStartX; x < nEndX; x++) 
	{
		nVerSum = 0;
		for (y = nStartY; y < nStartY + nTotalH; y++) 
		{
			int SumPoint = (nWidth * y) + x;
			if (SumPoint <= totalImageSize)
			{
				nVerSum += *(pOrgPtr + SumPoint);
			}
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
			CopyMemory(pTarLinePtr, pSrcPtr, sizeof(BYTE) * rcRange.Width());
		}
	}
	pSrcPtr = pTarPtr + (nWidth * (nEndY - nHalfH - 2)) + nStartX;

	if (nMode & en_FillBottom) {
		// 22.02.15 Ahn Modify Start
		//for (y = (nEndY - nHalfH - 1); y < nEndY; y++) {
		for (y = (nEndY - nHalfH ); y <= nEndY; y++) {
		// 22.02.15 Ahn Modify Start
			pTarLinePtr = pTarPtr + (nWidth * y) + nStartX;
			CopyMemory(pTarLinePtr, pSrcPtr, sizeof(BYTE) * rcRange.Width());
		}
	}

	delete[]vecLineSum;

	return TRUE;
}


int CImageProcess::GetProjection(const BYTE* pImage, int* pProjection, int nWidth, int nHeight, int nStartX, int nEndX, int nStartY, int nEndY, int nDir, int nSampling, BOOL bModeSum)
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
			pLine = (BYTE*)pImage + (nWidth * y);
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
			pLine = (BYTE*)pImage + (nWidth * y);
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


int CImageProcess::GetProjection(const BYTE* pImage, int* pProjection, int nWidth, int nHeight, CRect rectPrj, int nDir, int nSampling, BOOL bModeSum)
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
	//실제 얻은 이미지 높이보다 작은데 
	//왜 : 얻을 때 nHeight - 1 해서
	//실제 들어오지 않음
	if (nEndY > nHeight)
	{
		nTemp = nEndY - nHeight;
		nEndY = nHeight;
		nStartY -= nTemp;
	}
	//밑에서 0
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
	//Y축을 기준으로 X축 샘플링
	if (nDir == DIR_HOR)
	{
		//샘플링할 X축 갯수
		nCount = ((nEndX - nStartX) / nSampling);
		//최소 1개 이상은 있다
		if (nCount <= 0)
			nCount = 1;
		//프로젝션 샘플링 버퍼 초기화
		memset(pProjection, 0x00, sizeof(int) * (nDestHeight));
		for (y = nStartY; y < nEndY; y++)
		{
			pLine = (BYTE*)pImage + (nWidth * y);
			//샘플링 2000개 버퍼에 휘도 값을 플러스 한다.
			//X 축의 휘도값을 합치는 작업이다.
			for (x = nStartX; x < nEndX; x += nSampling)
			{
				//샘플링으로 선택된 라인의 휘도를 합한 값
				pProjection[y - nStartY] += *(pLine + x);
			}
		}
		//SUM 을 한 값이나 아니면 평균값이나 선택
		if (bModeSum == FALSE)
		{
			//샘플링한 휘도 값의 평균 값을 계산한다.
			//Y축 갯수 만큼 담아 온다.
			for (y = nStartY; y < nEndY; y++)
			{
				pProjection[y - nStartY] = pProjection[y - nStartY] / nCount;
			}
		}
	}
	//X 축 기준으로 Y 축 샘플링
	else
	{
		//프로젝션 샘플링 버퍼 초기화
		memset(pProjection, 0x00, sizeof(int) * (nDestWidth));
		//실제 이미지 끝점에서 샘플링 시작점 / 샘플링 값으로 나눔 갯수
		nCount = ((nEndY - nStartY) / nSampling);
		//나눈값이 0보다 작거나 같을 경우 최소 1개 샘플링
		if (nCount <= 0) nCount = 1;
		//Y의 스타트 에서 끝까지 샘플링 만큼 건너 뛰기
		for (y = nStartY; y < nEndY; y += nSampling)
		{
			//X의 포인터 이미지 샘플링 배수 만큼 이동
			pLine = (BYTE*)pImage + (nWidth * y);
			//샘플링 2000개 버퍼에 휘도 값을 플러스 한다.
			//Y 축의 휘도값을 합치는 작업이다.
			for (x = nStartX; x < nEndX; x++)
			{
				//샘플링으로 선택된 라인의 휘도를 합한 값
				pProjection[x - nStartX] += *(pLine + x);
			}
		}
		//SUM 을 한 값이나 아니면 평균값이나 선택
		if (bModeSum == FALSE)
		{
			//샘플링한 휘도 값의 평균 값을 계산한다.
			//X축 갯수 만큼 담아 온다.
			for (x = nStartX; x < nEndX; x++)
			{
				pProjection[x - nStartX] = pProjection[x - nStartX] / nCount;
			}
		}
	}
	return nCount;  // 22.06.24 Ahn Modify
}

int CImageProcess::Threshold(const BYTE* pImgPtr, BYTE* pRsltPtr, int nWidth, int nHeight, int nMin, int nMax)
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
		pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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
int CImageProcess::Threshold(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, int nWidth, int nHeight, int nThreshold, BOOL bClearRslt, BOOL bModeDark /*= FALSE*/ )
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
			pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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
			pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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

int CImageProcess::Threshold_RoundMask(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nInspWidth, int nMin, int nMaskOffset, int nLimitRight, int nMode, BOOL bClearRslt, BOOL bModeSide /*=en_HeadSide*/)
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
			pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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

			pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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
int CImageProcess::Threshold_RoundMask_Negative(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nStartOffset, int nEndOffset, int nMin, int nLimitRight, BOOL bClearRslt, BOOL bModeSide /* = CImageProcess::en_TopSide */ )
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
		pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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
	CopyMemory(pRsltPtr, pImgPtr, sizeof(BYTE) * nWidth * nHeight);
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

BOOL CImageProcess::GetOrgImageBright(const BYTE* pImgPtr, int nWidth, int nHeight, CRegionInfo::VEC_FIFO* pVecFifo)
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
					if (btLevel < nThresMax) {
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
					if (btLevel < nThresMax) {
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
					if (btLevel < nThresMax) {
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
					if (btLevel < nThresMax) {
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

int CImageProcess::SaveCropImage(const BYTE* pImgPtr,int nWidth, int nHeight, CRect rcCrop, CString strFilePath, CString strFileName )
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
		BYTE* pLinePtr = (BYTE *)pImgPtr + (nWidth * y);
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
//		double dTest = hypot((nMaxCount * AprData.m_System.m_dResolX[CAM_POS_TOP]), (nMaxCount * AprData.m_System.m_dResolY));

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

	//이전 맥스 값
	int nBefore = en_Max;
	//시작 위치
	int nStartPos = 0;
	//섹터 구조체 변수
	ST_SECTOR sect;

	//휘도 Y축 기준 X축 평균 
	//이미지 height 길이 갯수만큼 돌면서 : X 축 휘도 값 평균 확인
	for (i = 0; i < nPrjLength; i++)
	{
		//X축 평균 휘도가 브라이트 Min 보다 크고, Max 보다 작거나 같다면 (범위 안이면) 카운트 증가
		if ((pnPrjData[i] >= nThresMin) && (pnPrjData[i] <= nThresMax))
		{
			nCount++;
		}
		//휘도 값이 벗어 날 경우
		else
		{
			//nMinLength : 10으로 로컬 세팅됨
			//10 이상일 경우 휘도 범위가 지나가고 벗어나서 들어올 경우
			//섹터 정보를  저장한다.
			if (nCount >= nMinLength)
			{
				//섹터 정보
				//섹터의 시작 위치 정보
				sect.nStartPos = nStartPos;
				//섹터 끝 위치
				sect.nEndPos = i - 1;
				//섹터 삭제 : 삭제가 필요해서 설정한 값으로 생각
				sect.bDelete = FALSE;
				//섹터 정보 저장
				pVecSector->push_back(sect);
			}
			//휘도 정상범위 카운터 0 초기화
			nCount = 0;
			//시작 위치를 재 설정 : Height 지난 값 + 1
			nStartPos = i + 1;
		}
	}
	//for 문이 끝나는 시점에서 10 이상의 휘도 범위가 있었다면 
	//섹터 정보로 저장한다.
	if (nCount >= nMinLength)
	{
		//섹터 정보
		//섹터의 시작 위치 정보
		sect.nStartPos = nStartPos;
		//섹터 끝 위치
		sect.nEndPos = i - 1;
		//섹터 삭제 : 삭제가 필요해서 설정한 값으로 생각
		sect.bDelete = FALSE;
		//섹터 정보 저장
		pVecSector->push_back(sect);
	}
	//저장된 Sector 정보 사이즈 
	nCount = (int)pVecSector->size();

	return nCount;
}

int CImageProcess::FillArea_byRndInfo(const BYTE* pImage, BYTE* pMeanPtr, int nWidth, int nHeight, CRect rcArea, CImageProcess::VEC_ROUND_INFO* pVecRndInfo, BYTE btLevel, int nProcPos )
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
		pLinePtr = (BYTE*)pImage + (nWidth * y);
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
int CImageProcess::FindBoundary_FromPrjData(int* pnPrjData, int nLength, int nTargetBright, int nMode, BOOL bFindDark /*=FALSE*/, bool bLogOk)
// 23.02.24 Ahn Modify End
{
	ASSERT(pnPrjData);

	int i;
	int nLevel = 0 ;
	// 23.02.24 Ahn Add Start
//	if (bFindDark == FALSE) {

	int backpnPrjData = 0;
	int nLevel_back = 0;
	//블랙 롤 일 경우 찾는 방법
	if (bFindDark == TRUE) {
		backpnPrjData = 0;
	// 찾는 방향 : 오른쪽에서 왼쪽
		if (nMode == en_FindFromRight) {
			//휘도 샘플링 프로젝션 길이 만큼 돌면서
			for (i = nLength - 1; i > 0; i--) 
			{
				//휘도 샘플링 데이터 가 레시피 세라믹 휘도 최저 값 비교
				//샘플링 최저 값이 세라믹 휘도 보다 크면 그 값을 취하고 검사 종료
				if (pnPrjData[i] > nTargetBright) 
				{
					// 위치 값 저장하고 찾기를 빠져 나감
					nLevel = i;
					break;
				}
				//빽 데이터 비교(전 휘도 샘플링 데이터와 비교)
				if (pnPrjData[i] > backpnPrjData)
				{
					//휘도 중 제일 큰 데이터 만 저장 함
					backpnPrjData = pnPrjData[i];
					//휘도 중 제일 큰 값의 위치 값을 백업
					nLevel_back = i;
				}
				//설정 치 보다 큰 찾은 값이 없다면 사용할 위치 값
				nLevel = nLevel_back;
			}
		}
		//찾는 방향 왼쪽에서 오른쪽
		else {
			for (i = 0; i < nLength; i++)
			{
				//휘도 샘플링 데이터 가 레시피 세라믹 휘도 최저 값 비교
				//샘플링 최저 값이 세라믹 휘도 보다 크면 그 값을 취하고 검사 종료
				if (pnPrjData[i] > nTargetBright) 
				{
					// 위치 값 저장하고 찾기를 빠져 나감
					nLevel = i;
					break;
				}
				//빽 데이터 비교(전 휘도 샘플링 데이터와 비교)
				if (pnPrjData[i] > backpnPrjData)
				{
					//휘도 중 제일 큰 데이터 만 저장 함
					backpnPrjData = pnPrjData[i];
					nLevel_back = i;
				}
				//설정 치 보다 큰 찾은 값이 없다면 사용할 위치 값
				nLevel = nLevel_back;
			}
		}
	// 블랙롤이 아닐 경우
	}
	else 
	{
		backpnPrjData = 2147483647;
		// 찾는 방향 : 오른쪽에서 왼쪽
		if (nMode == en_FindFromRight) {
			//휘도 샘플링 프로젝션 길이 만큼 돌면서
			for (i = nLength - 1; i > 0; i--) 
			{
				//휘도 샘플링 데이터 가 레시피 세라믹 휘도 최저 값 비교
				//샘플링 최저 값이 세라믹 휘도 보다 작으면 그 값을 취하고 검사 종료
				if (pnPrjData[i] < nTargetBright) {
					// 위치 값 저장하고 찾기를 빠져 나감
					nLevel = i;
					break;
				}
				//빽 데이터 비교(전 휘도 샘플링 데이터와 비교)
				if (pnPrjData[i] < backpnPrjData)
				{
					//휘도 중 제일 큰 데이터 만 저장 함
					backpnPrjData = pnPrjData[i];
					nLevel_back = i;
				}
				//설정 치 보다 작은 찾은 값이 없다면 사용할 위치 값
				nLevel = nLevel_back;
			}
		}
		//찾는 방향 왼쪽에서 오른쪽
		else {
			//휘도 샘플링 프로젝션 길이 만큼 돌면서
			for (i = 0; i < nLength; i++) {
				//휘도 샘플링 데이터 가 레시피 세라믹 휘도 최저 값 비교
				//샘플링 최저 값이 세라믹 휘도 보다 작으면 그 값을 취하고 검사 종료
				if (pnPrjData[i] < nTargetBright)
				{
					// 위치 값 저장하고 찾기를 빠져 나감
					nLevel = i;
					break;
				}
				//빽 데이터 비교(전 휘도 샘플링 데이터와 비교)
				if (pnPrjData[i] < backpnPrjData)
				{
					//휘도 중 제일 큰 데이터 만 저장 함
					backpnPrjData = pnPrjData[i];
					nLevel_back = i;
				}
				//설정 치 보다 작은 찾은 값이 없다면 사용할 위치 값
				nLevel = nLevel_back;
			}
		}
	}
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

int CImageProcess::FindTabLevel_Simple(const BYTE* pImgPtr, int nWidth, int nHeight, int nFindPos, CRecipeInfo* pRecipeInfo, VEC_SECTOR* pVecSector, int* pnLevel)
{
	ASSERT(pImgPtr);
	ASSERT(pVecSector);
	ASSERT(pnLevel);

	//샘플링 할값 설정
	int nRet = 0;
	//샘플링 빈도 : 넓이 / 10
	int nSampling = 10;
	//샘플링 시작 점
	int nStartPos = nFindPos ;
	//샘플링 끝 점 : 시작 점에서 샘플링 빈도 * 10 : 10개 한다.
	int nEndPos = nStartPos + (nSampling * 10);

	//샘플링 끝 점이 실제 얻은 이미지 Width 보다 크면 Width 값
	if (nEndPos > nWidth) {
		nEndPos = nWidth;
	}

	//레시피 세라믹 위치 휘도 값 Min
	int thMin = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; //(int)(pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2;
	//세라믹 휘도 Max
	int thMax = 255;

	pVecSector->clear();

	//찾은 섹터의 갯수
	int nLocalRet = 0;

	//레시피 세라믹 휘도 값을 이용하여 롤과 탭을 구분한다.
	//nLocalRet : 찾은 Sector 갯수
	nLocalRet = CImageProcess::FindTabPos(pImgPtr, nWidth, nHeight, nStartPos, nEndPos, thMin, thMax, pVecSector);
	if (nLocalRet <= 0) {
		return -1;
	}

	//찾은 섹터 값을 이용하여 실제 Tab을 가지는 섹터 정보를 추출한다.
	//위에서 얻은 섹터 정보를 다음 섹터 정보와 합치는 작업을 하여 만들어 낸다.
	nLocalRet = CImageProcess::CombineTabSector(pVecSector, *pRecipeInfo);
	if (nLocalRet < 0) {
		return -2;
	}

	//레시피 Tab Width 를 이용하여  얻기 위한 세팅
	int i = 0;
	//섹터 갯수
	int nSize = (int)pVecSector->size();
	int nTabWidth;
	int nFindIdx = 0;
	int nLevelFindPos = 0;

	//섹터 정보가 1개라도 있으면 실제 Tab이 있는 섹터를 찾기 위해서 
	//레시피 Tab Width 를 이용하여  얻는다.
	ST_SECTOR* pstSector = NULL;
	for (i = 0; i < nSize; i++) {
		nTabWidth = (*pVecSector)[i].nEndPos - (*pVecSector)[i].nStartPos;
		if ( nTabWidth > (pRecipeInfo->TabCond.nTabWidth / 2)) {
			pstSector = &(*pVecSector)[i];
			break;
		}
	}
	//섹터 갯수는 있는데 Tab으로 인식하지 못했을 때는
	//찾는 섹터에서 첫번째 섹터를 취한다.
	if (pstSector == NULL)
	{
		//찾은 섹터가 있을 때
		if (nSize)
		{
			pstSector = &(*pVecSector)[0];
		}
		// 찾은 섹터가 없을 경우
		else
		{
			return -3;
		}
	}

	//마지막 엣지
	//지금의 섹터 이전 섹터 End pos(이전 섹터의 끝점)
	int nLastEdge = 0;
	//제일 큰 이전 섹터 End Pos 에서 지금 섹터의 Start Pos 거리
	//nDistance 최대 값 백업
	int nMaxDistPos = 0 ;
	//제일 긴 nMaxDistPos의 시작 위치 값 백업
	//nMaxDistPos 값을 다시 찾았을 때
	int nMaxStartPos = 0 ;
	//nMaxDistPos 값을 다시 찾았을 때의 지금 섹터 시작 점
	int nMaxEndPos = 0 ;
	//
	int nDistance;

	//얻은 섹터 갯수 만큼 
	for (i = 0; i < nSize ; i++) {
		//찾은 섹터의 첫번 째 점에서 이전 섹터 End 점까지 거리
		nDistance = (*pVecSector)[i].nStartPos - nLastEdge ;

		//백업한 거리 보다 더 큰 거리 값을 얻었으면
		if (nMaxDistPos < nDistance) {
			//최대 거리의 이전 섹터 끝 점을 시작 점으로 
			nMaxStartPos = nLastEdge;
			//최대 거리의 지금 섹터 시작 점을  끝 점으로
			nMaxEndPos = (*pVecSector)[i].nStartPos;
			//다음 섹터의 거리를 비교하기 위해서 백업한다.
			nMaxDistPos = nDistance;
		}
		//지금의 섹터의 End Pos를 백업한다.
		//다음 섹터의 스타트 점과의 거리를 계산하기 위한 값이다.
		nLastEdge = (*pVecSector)[i].nEndPos;
	}
	//nSize : 섹터의 갯수
	//nSize - 1 를 해야 인텍스가 된다(vector 0부터)
	//섹터의 마지막 인덱스의 End Pos에서 실제 이미지 Height(끝) 거리를 얻는다.
	//마지막 섹터에서 남은 거리
	nDistance = nHeight - (*pVecSector)[nSize-1].nEndPos; 
	//찾은 Max 백업 거리보다 크면 취한다.
	if (nDistance > nMaxDistPos) {

		//최대 거리의 이전 섹터 끝 점
		//최대 거리의 Max의 시작 점으로 마지막 섹터의 End Pos를
		nMaxStartPos = (*pVecSector)[nSize-1].nEndPos;
		//최대 거리의 Max의 끝 점으로 실제 이미지 넓이를 취한다.
		nMaxEndPos = nHeight ; 
	}

	//시작 점과 끝 점이 0이상 일 대
	if ((nMaxStartPos >= 0) && (nMaxEndPos > 0)) {
		//시작 점과 끝 점의 중간 값
		int nCenterPos = (int)( (nMaxStartPos + nMaxEndPos) / 2 ) ;

		//프로젝션 샘플링 범위 설정
		CRect rcPrj;
		//프로젝션 샘플링 데이터 버퍼 실제 이미지 X 넓이 만큼
		int* pnPrjData;
		pnPrjData = new int[nWidth];
		memset(pnPrjData, 0x00, sizeof(int) * nWidth);
		//프로젝션 샘플링 할 실제 이미지의 Y 범위(20 + 20 = 40 픽셀)
		rcPrj.top = nCenterPos - 20 ;
		rcPrj.bottom = nCenterPos + 20 ;
		//X의 실제 이미지 범위 값(전체)
		rcPrj.left = 0;
		rcPrj.right = nWidth;

		//프로젝션 샘플링 데이터를 가져 온다.
		//pImgPtr : 실제 이미지 데이터
		//pnPrjData : 샘플링 데이터를 가져 올 버퍼
		//nWidth : 실제 이미지 넓이
		//nHeight : 실제 이미지 높이
		//rcPrj : 프로젝션 할 실제 이미지의 위치 및 범위 값
		//DIR_VER : X 축으로 합친다.
		//nSampling : rcPrj <- 범위 에서 샘플링 됨(y 값 시작에서 끝까지 거리 를 샘플링 값으로 나눔) 1/nSampling
		//TRUE : 휘도 샘플링 데이터를 총합으로 가져온다. (FALSE ; 평균을 가져온다.)
		int nCount = CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, TRUE);


		//블랙롤 인가를 레시피 데이터에서 가져온다.
		//블랙 롤의 경우 nUpperBright 보다 작다면 샘플링 된 데이터 최대 값
		//블랙 롤이 아닐 경우 최소 값
		BOOL bUseDarkRoll = (pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;

		//nCount Y축 샘플링 갯수 에 세라믹 레시피 설정 휘도와 Roll 부 레시피 휘도 평균를 곱하여 
		int nUpperBright = nCount * ((pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2);//pyjtest : 주기적인 NG 발생건, 양극에서 이 값 계산으로 인해 기준 Edge 인식 못하는 경우가 발생하는 듯
//		int nUpperBright = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP];//pyjtest

		//롤과의 경계를 찾는데 오른쪽부터 찾아서 롤 휘도를 벗어나면 그 위치가 세라믹 코팅부
		//아니면 최대 값을 가지는 부분이 경계 선
		*pnLevel = CImageProcess::FindBoundary_FromPrjData(pnPrjData, nWidth, nUpperBright, en_FindFromRight, bUseDarkRoll);

		//프로젝션 샘플링 버퍼 삭제
		delete[] pnPrjData;
		pnPrjData = NULL;
	}

	return nRet;

}
// 22.04.13 Ahn Add End

// 음극 Tab을 찾고 Tab 중심에서 검사 라인을 유추함.
int CImageProcess::FindTab_Negative(const BYTE* pImgPtr, int nWidth, int nHeight, int nTabFindPos, CRecipeInfo *pRecipeInfo, VEC_SECTOR *pVecSector, int *pnLevel)
{
	ASSERT(pImgPtr);
	ASSERT(pVecSector);
	ASSERT(pnLevel);

	//
	int nRet = 0;
	//샘플링 값
	int nSampling = 10 ;
	//Tab을 찾기 위한 위치 스타트 점
	int nStartPos = nTabFindPos;
	//Tab Find 끝나는 점
	//샘플링 건너뛰기 갯수 만큼 : 샘플링 빈도 10 * 10 : 10개 한다.
	int nEndPos = nStartPos + (nSampling * 10);

	//샘플링 끝나는 점이 실제 이미지 크기 보다 크면 실제 이미지를 Max로 
	if (nEndPos > nWidth) {
		nEndPos = nWidth;
	}
	//레시피의 Bright  최저 값
	//레시피의 Tab Min 값을 이용하여 롤과 Tab 위치 찾기 위한 값
	int thMin = pRecipeInfo->TabCond.nTabMinBright; // pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP];
	//레시피 Bright 최대 값 255
	int thMax = 255;

	//섹터를 저장할 vector 초기화
	pVecSector->clear();
	//로컬 Ret
	int nLocalRet = 0;
	//로컬 Tab Pos 찾는다.
	//Tab의 휘도 값을 이용하여 롤과 Tab을 구분한다.
	nLocalRet = CImageProcess::FindTabPos(pImgPtr, nWidth, nHeight, nStartPos, nEndPos, thMin, thMax, pVecSector);
	//섹터 정보가 없다면 -1 리턴(오류)
	if (nLocalRet <= 0)
	{
		return -1;
	}
	//섹터 정보 에서 레시피 설정된 Tab Width (탭 넓이 의 1.2배) 보다 작을 때 섹터 정보를 삭제 한다.
	//휘도 범위 안을 10 이상 가지고 있는 Sector 정보에서 필요 없는 정보를 삭제 하는 함수
	nLocalRet = CImageProcess::CombineTabSector(pVecSector, *pRecipeInfo);
	//남아 있는 섹터 정보가 없다면 오류
	if (nLocalRet < 0)
	{
		return -2;
	}

	int i = 0; 
	//섹터 정보 갯수가 남아 있다면
	int nSize = (int)pVecSector->size();
	int nTabWidth;
	//찾은 인덱스 값
	int nFindIdx = 0 ;

	ST_SECTOR* pstSector = NULL ;
	//남은 섹터 정보 갯수만큼 돈다.
	for (i = 0; i < nSize; i++) 
	{
		//섹터에 남아 있는 휘도에서 얻은 끝 점(위치)에 스타트 점뺀 값이 Tab Width 구한다.
		nTabWidth = (*pVecSector)[i].nEndPos - (*pVecSector)[i].nStartPos;
		//얻은 Tab Width 에서 레시피 설정된 Tab Width 뺀 값이 
		//Tab 찾아서 빠져나감
		if( abs(nTabWidth - pRecipeInfo->TabCond.nTabWidth) < (pRecipeInfo->TabCond.nTabWidth / 3 )) 
		{
			//Tab 있는 섹터 정보 
			pstSector = &(*pVecSector)[i];
			break;
		}
	}
	//찾은 tab이 있는 섹터 정보를 찾지 못했으면
	if (pstSector == NULL) 
	{
		//사이즈가 있으면 첫번째 정보를 Tab 섹터 정보로 넘긴다.
		if (nSize)
		{
			pstSector = &(*pVecSector)[0];
		}
		//아니면 오류
		//섹터를 못찾았으면 들어오지 않는데?
		else
		{
			return -3;
		}
	}

	//CRect rcPrj;
	//int* pnPrjData;
	//pnPrjData = new int[nWidth];
	//memset(pnPrjData, 0x00, sizeof(int) * nWidth);

	//rcPrj.top = pstSector->nStartPos;
	//rcPrj.bottom = pstSector->nEndPos;
	//rcPrj.left = 0;
	//rcPrj.right = nWidth;

	//int nCount = CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, 0);
	//// 경계 검출
	//int nUpper = 20 * nCount;
	//int nBundary = CImageProcess::GetBundary_FromPrjData(pnPrjData, nWidth, 20, 0, pRecipeInfo->TabCond.nNegCoatHeight);
	//*pnLevel = nBundary - pRecipeInfo->TabCond.nNegCoatHeight ;

	//delete[] pnPrjData;





	CRect rcPrj;
	int* pnPrjData;
	//프로젯션 : 휘도로 찾은 저장 버퍼
	pnPrjData = new int[nWidth];
	memset(pnPrjData, 0x00, sizeof(int) * nWidth);

	//프로젝션을 할 설정 범위
	//높이 시작 : 0에서 
	rcPrj.top = 0;
	//높이 끝 : 섹터 정보 시작점에서 20 뺀 값까지만
	rcPrj.bottom = pstSector->nStartPos - 20;
	//넓이 시작
	rcPrj.left = 0;
	//넓이 끝
	rcPrj.right = nWidth;

	int nCount = 0;
	int nUpperBright = 0;
	//실제 이미지에서 프로젝션 설정 범위와 샘플링 10 휘도 샘플 데이터 pnPrjData X축 샘플링 데이터
	//프로젝션 휘도 총합을 가져온다.
	nCount = CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, TRUE);
	//블랙 롤일 경우 TRUE
	//블랙 롤의 경우 nUpperBright 보다 작다면 샘플링 된 데이터 최대 값
	//블랙 롤이 아닐 경우 최소 값
	BOOL bUseDarkRoll = (pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;
	//X 축 샘플링된 갯수 * 배수(레시피 세라믹 브라이트 로우 값 롤의 브라이트 하이 값 빼기 1/2
	//nCont : 배수는 샘플링한 갯수에 설정 휘도를 해야 총합과 비교 가능
	//nCount Y축 샘플링 갯수 에 세라믹 레시피 설정 휘도와 Roll 부 레시피 휘도 평균를 곱하여 
	nUpperBright = nCount * ((pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2);//pyjtest : 주기적인 NG 발생건, 양극에서 이 값 계산으로 인해 기준 Edge 인식 못하는 경우가 발생하는 듯
	//X 축 휘도로 찾은 값에서
	//세라믹 코팅 부 휘도와 Roll 휘도 평균 값을 오른쪽에서 부터 검사 해서 크면 된다.
	//이유는 롤의 윗 부분을 검사하는 부분이고, 롤이 아닌 부분을 만나면 바로 그 점이 세라믹 부분이다.
	int nLevelLeft = CImageProcess::FindBoundary_FromPrjData(pnPrjData, nWidth, nUpperBright, en_FindFromRight, bUseDarkRoll);


	//이전 프로젝션 데이터 초기화
	memset(pnPrjData, 0x00, sizeof(int) * nWidth);
	//프로젝션 높이 : 섹터의 끝점에서부터 검사하여
	rcPrj.top = pstSector->nEndPos + 20;
	//프로젝션 높이 : 실제 이미지 높이 끝까지
	rcPrj.bottom = nHeight;
	//넓이 시작 점
	rcPrj.left = 0;
	//넓이 끝 점
	rcPrj.right = nWidth;
	//프로젝터 범위 설정 값의 휘도 평균이 아닌 합한 총 값을 가져온다.
	nCount = CImageProcess::GetProjection(pImgPtr, pnPrjData, nWidth, nHeight, rcPrj, DIR_VER, nSampling, TRUE);
	//X 축 샘플링된 갯수 * 배수(레시피 세라믹 브라이트 로우 값 롤의 브라이트 하이 값 빼기 1/2
	//nCont : 배수는 샘플링한 갯수에 설정 휘도를 해야 총합과 비교 가능
	nUpperBright = nCount * ((pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP] + pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP]) / 2);//pyjtest : 주기적인 NG 발생건, 양극에서 이 값 계산으로 인해 기준 Edge 인식 못하는 경우가 발생하는 듯
	//X 축 휘도로 찾은 값에서
	//세라믹 코팅 부 휘도와 Roll 휘도 평균 값을 오른쪽에서 부터 검사 해서 크면 된다.
	//이유는 롤의 아래 부분을 검사하는 부분이고, 롤이 아닌 부분을 만나면 바로 그 점이 세라믹 부분이다.
	int nLevelRight = CImageProcess::FindBoundary_FromPrjData(pnPrjData, nWidth, nUpperBright, en_FindFromRight, bUseDarkRoll);


	//롤의 윗부분과 아랫부분의 평균 점이 세라믹 경계선이 된다.
	*pnLevel = (nLevelLeft + nLevelRight) / 2;

	//프로젝션 샘플링 버퍼 제거
	delete[] pnPrjData;
	pnPrjData = NULL;



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
	CopyMemory(nPrjArray, pnPrjData, sizeof(int) * 2000);
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
	CopyMemory(nPrjArray, pnPrjData, sizeof(int) * 2000);

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

int CImageProcess::GetBundary_FromPrjData(int* pnPrjData, int nLength, int nCompLen, int nMode, int nNegCoatHeight)
{
	int nTop = 0;
	int nBtm = 0 ;
	if (nLength <= nCompLen) return -1;

	int i;

	std::vector< CString> strCompLenVec;
	CString strCompLena = "";

	for ( i = 0; i < nCompLen; i++) 
	{
		nTop += pnPrjData[i];
		nBtm += pnPrjData[i + nCompLen ];

		CString str;
		str.Format("(%d*%d)-", nTop, nBtm);
		strCompLena += str;
		if (i % 50 == 0)
		{
			strCompLenVec.push_back(strCompLena);
			strCompLena = "";
		}
	}
	int nEndPos = nLength - nCompLen;

	int nMaxPos = -1 ;
	int nMaxValue = 0 ;
	int nDiff = 0;

	for (i = nCompLen; i < nEndPos; i++) 
	{
		nDiff = abs(nTop - nBtm);
		if (nDiff > nMaxValue) 
		{
			nMaxValue = nDiff;
			nMaxPos = i;
		}
		nTop += pnPrjData[i + 1] - pnPrjData[i - nCompLen];
		nBtm += pnPrjData[i + nCompLen] - pnPrjData[i] ;

		CString str;
		str.Format("(%d*%d)-", nTop, nBtm);
		strCompLena += str;
		if (i % 50 == 0)
		{
			strCompLenVec.push_back(strCompLena);
			strCompLena = "";
		}
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
int CImageProcess::FindTabPos(const BYTE* pImagePtr, int nWidth, int nHeight, int nStartPos, int nEndPos, int nThresMin, int nThresMax, VEC_SECTOR* pVecSector)
{
	//프로젝션 샘플링을 하기 위한 위치 정보 및 크기
	CRect rect;
	//넓이 시작 위치
	rect.left = nStartPos;
	//넓이  끝 위치
	rect.right = nEndPos;
	// 높이 시작 위치
	rect.top = 0;
	//높이 끝 위치 : 최대 실제 이미지 높이 세팅
	rect.bottom = nHeight;

	//갯수
	int nCount;
	//브라이트 Min : 레시피 설정 값
	//브라이트 Max : 255
	//Min이 크면 스왑
	if (nThresMin > nThresMax) {
		SWAP(int, nThresMax, nThresMin);
	}

	// DIR_HOR 
	//프로젝션 샘플링 버퍼
	int* pnPrj = new int[nHeight];

	//실제 이미지를 프로젝션 하여 데이터를 가져 온다.
	//Y축 기준 X축 프로젝션 샘플링을 1 설정으로 높이 모두 한다.
	//pnPrj : 휘도 샘플링 데이터 -> Height 버퍼에 X축 시작점에서 끝점까지 평균을 가져온다.
	GetProjection(pImagePtr, pnPrj, nWidth, nHeight, rect, DIR_HOR, 1, FALSE);

	//휘도 정보를 가지고 섹터 정보를 얻는다.
	//pnPrj : 휘도 X축 평균
	//nHeight : 실제 이미지 Y축 높이
	//nThresMin : 브라이트 최소 값
	//nThresMax : 브라이트 최대 값
	nCount = DivideSectionBetweenThreshold(pnPrj, nHeight, nThresMin, nThresMax, 10, pVecSector);

	delete[]pnPrj;
	return nCount;
}

int CImageProcess::CombineTabSector(VEC_SECTOR* pVecSector, CRecipeInfo& RecipeInfo)
{
	ASSERT(pVecSector);
	//찾은 섹터 갯수
	int nSize = (int)pVecSector->size();
	//레시피 설정값에 1.2배 정보 Range 값
	int nTabCheckRange = (int)( RecipeInfo.TabCond.nTabWidth * 1.2 ) ;

	//섹터 정보 갯수 만큼 
	//레시피 설정 Tab Width 기준보다 작을 때 삭제 해 나가는 구간
	for (int i = 0; i < nSize - 1 ; i++) {
		//섹터 정보 delete 정보가 TRUE 처리안함
		if ((*pVecSector)[i].bDelete == TRUE) continue;
		//겉의 for의 섹터 정보 다음 섹터 정보
		for (int j = i + 1; j < nSize; j++) {
			//섹터 정보 delete 정보가 TRUE 처리안함
			if ((*pVecSector)[j].bDelete == TRUE) continue;
			//섹터 정보 끝 점에서 시작 점의 길이가 레시피 설정 값의 1.2배보다 작다면
			//Tab Width 1.2배의 크기보다 작다면 합친다.
			if ( ((*pVecSector)[j].nEndPos - (*pVecSector)[i].nStartPos ) <= nTabCheckRange) {
				//겉의 섹터 끝 점에 다음 섹터의 끝 점
				(*pVecSector)[i].nEndPos = (*pVecSector)[j].nEndPos;
				//다음 섹터 정보는 삭제 로 설정한다.
				(*pVecSector)[j].bDelete = TRUE;
			}
		}
	}

	//섹터 정보 시작에서 끝까지
	//delete 가 TRUE 설정되었으면 섹터 정보를 삭제한다.
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

	int nSize = (int)pBlockInfo->size();

	CBlockData* pData;
	int nMinSize[MAX_INSP_TYPE];
	nMinSize[TYPE_FOILEXP_OUT] = pRecipeInfo->nFoilExpOutMinSize[nCamPos];
	nMinSize[TYPE_FOILEXP] = pRecipeInfo->nFoilExpInMinSize[nCamPos];


//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ INSP===================================================================================================
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("!! SPC+=====DefectInfo CellNo<%d>, CAM<%s>, BlockData Count<%d>"
		, pTabRsltInfo->m_nTabNo+1
		,(pTabRsltInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP":"BOTTOM"
		,nSize);
	//===========================================================================================================
#endif //SPCPLUS_CREATE


	AprData.SaveDebugLog_Format( _T("<AddDefectInfoByBlockInfo> <%s> TabNo:%d, BlockData Count:%d")
		, (pTabRsltInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP" : "BOTTOM"
		, pTabRsltInfo->m_nTabNo + 1
		, nSize);

	int nCount = 0; 
	for (int i = 0; i < nSize; i++)
	{
		pData = &(*pBlockInfo)[i];

		if (pData->bDeleteFlag == TRUE)
			continue;

		if (pData->nPixelCnt < nMinSize[pData->nType])
			continue;

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
		pDefInfo->nCellId = pTabRsltInfo->m_nCellId;
		pDefInfo->dSizeX = pData->dWidth; //(double)pDefInfo->nSizeX * AprData.m_System.m_dResolX[nCamPos];
		pDefInfo->dSizeY = pData->dHeight; //(double)pDefInfo->nSizeY * AprData.m_System.m_dResolY;
		pDefInfo->nType = pData->nType;
		pDefInfo->dJudgeSize = pData->dJudgeSize;
		pDefInfo->dDistance = pData->dDistance;
		pDefInfo->bDeleteFlag = FALSE;

		// 판정 필요
		CPoint cpCenter = pData->rcRect.CenterPoint();
		int nDefPosY = cpCenter.y ;
		pDefInfo->dDefectPosY = ( (double)( nFrameStartPos + nDefPosY ) * dResoY ) / 1000.0 ;
		pDefInfo->sysTime = pTabRsltInfo->sysTime ;		
		::_tcsnccpy_s( pDefInfo->szLotID, _countof(pDefInfo->szLotID), AprData.m_NowLotData.m_strLotNo.GetBuffer(0), _TRUNCATE);


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
				pTabRsltInfo->m_wNgReason |= ( ( pDefInfo->nHeadNo == CAM_POS_TOP ) ? CTabRsltBase::en_Reason_Surface_Top : CTabRsltBase::en_Reason_Surface_Btm );

			}
			else if (pTabRsltInfo->m_nJudge < JUDGE_NG)
			{
				if ((pRecipeInfo->dSurfaceGraySize > 0.0) && ( pDefInfo->dJudgeSize > pRecipeInfo->dSurfaceGraySize ))
				{
					pDefInfo->nRank = JUDGE_GRAY;
					//pTabRsltInfo->m_nJudge = JUDGE_GRAY; // 결함 후보가 한 탭에 다수일때 Gray 판정 결함이 있는 경우 최종 결과가 Gray로 OK 판정나서 제거 함
				}
			}

			pDefInfo->nDispPos = en_defPosSurface;
		}
		else
		{
			if (pDefInfo->dJudgeSize == 0.f )
			{
				pDefInfo->dJudgeSize = pData->dWidth;
			}

			if ((pData->nDefPos == en_LeftRound) || (pData->nDefPos == en_RightRound))
			{
				pDefInfo->nDispPos = en_defPosRound ;
			}
			else
			{
				pDefInfo->nDispPos = en_defPosEdge ;
			}

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
			pDefInfo->nRank = CTabRsltInfo::GetDefJudge( dNgSize, pRecipeInfo->dDefectYSize[pDefInfo->nHeadNo], pDefInfo->dJudgeSize, pDefInfo->dSizeY );
			// 22.07.20 Ahn Modify End
			if (pDefInfo->nRank == JUDGE_NG)
			{
				BOOL bNg = TRUE;
				if ( pDefInfo->nType == en_ModeFoilExp || pDefInfo->nType == en_ModeFoilExpOut )
				{
					if ((pRecipeInfo->dIgnoreDistance < pDefInfo->dDistance) && (pRecipeInfo->dIgnoreDistance > 0) 
						&& (( pDefInfo->dSizeX > pRecipeInfo->dIgnoreSize ) && (pRecipeInfo->dIgnoreSize > 0)))
					{

						pDefInfo->dJudgeSize = pDefInfo->dSizeX;
						pDefInfo->nRank = JUDGE_OK;
						pDefInfo->bDeleteFlag = TRUE;
						bNg = FALSE;
					}
				}


				if ( bNg == TRUE )
				{
					pTabRsltInfo->m_nJudge = JUDGE_NG;

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
						strMsg.Format(_T("[NG] en_ModeFoilExpIn : Head No = %d"), pDefInfo->nHeadNo);
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
						strMsg.Format(_T("[NG] en_ModeFoilExpOut : Head No = %d"), pDefInfo->nHeadNo);
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
						strMsg.Format(_T("[NG] en_ModeFoilExpBoth : Head No = %d"), pDefInfo->nHeadNo);
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
						strMsg.Format(_T("[NG] en_ModeFoilExpBoth : Head No = %d"), pDefInfo->nHeadNo);
						break;
					}

					AprData.SaveDebugLog(strMsg);


				}
			}
			// 22.11.21 Ahn Modify Start - JUDGE_GRAY
			//else if (pTabRsltInfo->m_nJudge < JUDGE_NG)
			else if (pDefInfo->nRank < JUDGE_NG)
			{
				if ((pRecipeInfo->dFoileGraySize  > 0.0) && (pDefInfo->dJudgeSize > pRecipeInfo->dFoileGraySize))
				{
					pDefInfo->nRank = JUDGE_GRAY;
					//pTabRsltInfo->m_nJudge = JUDGE_GRAY; // 결함 후보가 한 탭에 다수일때 Gray 판정 결함이 있는 경우 최종 결과가 Gray로 OK 판정나서 제거 함

					// JUDGE_GRAY도 결함 제거 기능에 포함
					if ((pRecipeInfo->dIgnoreDistance < pDefInfo->dDistance) && (pRecipeInfo->dIgnoreDistance > 0)
						&& ((pDefInfo->dSizeX > pRecipeInfo->dIgnoreSize) && (pRecipeInfo->dIgnoreSize > 0)))
					{
						pDefInfo->dJudgeSize = pDefInfo->dSizeX;
						pDefInfo->nRank = JUDGE_OK;
						pDefInfo->bDeleteFlag = TRUE;
					}


				}
			}
			// 22.11.21 Ahn Modify End
		}

		if (pDefInfo->nRank == JUDGE_OK)
		{
			pDefInfo->bDeleteFlag = TRUE;
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

	AprData.SaveDebugLog_Format(_T("<AddDefectInfoByBlockInfo> <%s> TabNo:%d, Result Count:%d")
		, (pTabRsltInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP" : "BOTTOM"
		, pTabRsltInfo->m_nTabNo + 1
		, nCount);


	return 0;
}

int CImageProcess::BlockLink(CImageProcess::_VEC_BLOCK* pBlockInfo, CRecipeInfo* pRecipeInfo, int nDefType, int nCamPos )
{
	int nSize = (int)pBlockInfo->size();
	CBlockData* pPtr;
	CBlockData* pTar;
//	int nRangX = pRecipeInfo->pntLinkRange[nDefType][nCamPos].x + 1;
//	int nRangY = pRecipeInfo->pntLinkRange[nDefType][nCamPos].y + 1;

	int nRangX = pRecipeInfo->pntLinkRange[nCamPos][nDefType].x;// +1;
	int nRangY = pRecipeInfo->pntLinkRange[nCamPos][nDefType].y;// +1;


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
			pPtr->dJudgeSize = pPtr->dWidth;

		}
	}

	return 0;
}

int CImageProcess::ResizeImage(const BYTE* pImgPtr, BYTE* pResizePtr, int nWidth, int nHeight, int nZoomOut)
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
		pOrgLinePtr = (BYTE*)pImgPtr + (nWidth * (y * nMagnif));

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

	if (pRect->left == pRect->right) pRect->right = pRect->left + 1;
	if (pRect->top == pRect->bottom) pRect->bottom = pRect->top + 1;

	if (pRect->left < 0) pRect->left = 0;
	if (pRect->right >= nWidth) pRect->right = nWidth;
	if (pRect->top < 0) pRect->top = 0;
	if (pRect->bottom >= nHeight) pRect->bottom = nHeight;

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

static int Tab_byFixSizeCount = 0;
int CImageProcess::DivisionTab_byFixSize(const BYTE* pImgPtr, const BYTE* pImgBtmPtr, int nWidth, int nHeight, int nFixSize, int nStartPos, int nEndPos, BOOL bIsPET, _VEC_TAB_INFO* pVecTabInfo)
{
	ASSERT(pImgPtr);
	ASSERT(pVecTabInfo);

	//끝 위치 Y
	int nRet = nEndPos ;

	//레시피 Tab Pitch 나누어 몇개인가 계산
	int nSize = (nEndPos - nStartPos) / nFixSize;

	//새로 얻은 이미지에서 이전 Tab Info Image 에 붙이고 남은 위치 가 시작점
	int nLastSavePos = nStartPos; 

	//레시피 Tab Pitch 로 자를 이미지 크기가 있다면 로그
	if( nSize > 0 )
	{
		AprData.SaveDebugLog_Format( _T("<DivisionTab_byFixSize> <CTabInfo> m_bErrorFlag = 1") );
	}

	//레시피 Tab Pitch 로 자를 이미지 크기가 있다면
	//갯 수만 큼 이미지 자른다.그리고 정보도 세팅한다.
	//Tab Info : 정상적인 Tab Info가 아니다.
	for (int i = 0; i < nSize; i++)
	{
		//얻을 Tab Info 클래스 객체
		CTabInfo tabInfo;
		//Sector 정보가 없으므로 에러 (즉 Tab Widtch가 없는 남은 이미지로 )
		tabInfo.m_bErrorFlag = TRUE;
		//PET 여부 설정 세팅
		tabInfo.m_bIsPET = bIsPET;
		//이미지 길이는 레시피 세팅된 Tab Pitch 값이다.
		tabInfo.nImageLength = nFixSize;

		//Top 이미지 데이터 저장 메모리 생성
		tabInfo.pImgPtr = new FrameImagePtr();
		memset(tabInfo.pImgPtr->m_pImagePtr, 0x00, sizeof(BYTE) * nWidth * nFixSize + 1);
		//새로운 이미지를 Tab Info Image 에 copy
		CopyMemory(tabInfo.pImgPtr->m_pImagePtr, pImgPtr + (nWidth * nLastSavePos), sizeof(BYTE) * nWidth * nFixSize);

		//Bottom 이미지 데이터 저장 메모리 생성
		tabInfo.pImgBtmPtr = new FrameImagePtr();
		memset(tabInfo.pImgBtmPtr->m_pImagePtr, 0x00, sizeof(BYTE) * nWidth * nFixSize + 1);
		//새로운 이미지를 Tab Info Image 에 copy
		CopyMemory(tabInfo.pImgBtmPtr->m_pImagePtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE) * nWidth * nFixSize);

		//copy 하고 남은 이미지 위치 점
		nLastSavePos += nFixSize ;

		LOGDISPLAY_SPEC(5)("<<Proc>> CImageProcess::DivisionTab_byFixSize-TabInfopush-count");
		//Tab Info push
		pVecTabInfo->push_back(tabInfo);
	}


	if (nLastSavePos < nEndPos) {
		nRet = nLastSavePos;
	}

	return nRet;
}

int CImageProcess::TabPitcPixelhCalculate(double RecipeTabPitch, double dResolY)
{
	int TabPitchPixel = 0;
	TabPitchPixel = (int)(RecipeTabPitch * (1000.0 / dResolY));
	return TabPitchPixel;
}

// 22.11.18 Ahn Modify Start
int CImageProcess::DivisionTab_FromImageToTabInfo(const BYTE* pImgPtr, const BYTE *pImgBtmPtr, int nWidth, int nHeight, int nFindPos, int *pnLevel, CRecipeInfo& RecipeInfo, CTabInfo* pResvTabInfo, _VEC_TAB_INFO* pVecTabInfo, int nFrameCount)
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

	//Tab 인식을 하지 못했을 때 사용할 검사 이미지 길이 계산하다.
	// 초기 계산 값이 커서 BCD Id가 남는 경우 발생 비교 하기 위한 로그
	//1.0 은 0으로 판단에 문제로 입력된 값
	int nTabPitchCalc = 0;
	if (AprData.m_System.m_dResolY1000P > 1.0)
	{
		double dTabPitch = RecipeInfo.TabCond.dTabPitch;
		//분해능을 이용하여 실제 길이에서 픽셀 단위의 Tab Pitch를 얻는다.
		nTabPitchCalc = TabPitcPixelhCalculate(dTabPitch, AprData.m_System.m_dResolY1000P);
		LOGDISPLAY_SPEC(7)("@@ Base Tab  calc val f<%f>n<%d> =============== ", dTabPitch, nTabPitchCalc);
	}

	//Tab의 기본 Pitch(레시피에 입력된 값(double, int 값 )
	int nBaseTabPitch = 0;

	//기준이되는 레시피 Tab Pitch 
	nBaseTabPitch = RecipeInfo.TabCond.nTabPitch;

	//기존 계산된 값과 지금 계산 값 비교 로그 출력
	LOGDISPLAY_SPEC(7)("@@ Base Tab  calc val<%d> setting val<%d> =============== ", nTabPitchCalc, nBaseTabPitch);

	//기준이되는 레시피 Tab 넓이값
	int nBaseTabWidth = RecipeInfo.TabCond.nTabWidth;
	//기준이되는 Tab 오른쪽에서 왼쪽까지 잘려나간 Tab이 아닌 부분의 Width
	int nBaseTabBlank = nBaseTabPitch - nBaseTabWidth;
	//기준이되는 레시피 Tab Pitch 1/2 값
	int nBaseTabHalfPitch = nBaseTabPitch / 2;
	//기준이 되는 레시피 Tab Pitch 에서 Tab 넓이를 뺀 값의 1/2 : Tab 없는 부분을 기준으로 짜르기 위한 길이
	//이전 Tab 끝과 다음 Tab 처음 사이의 길이의 반
	int nPairSholderLength = (nBaseTabPitch - nBaseTabWidth) / 2;

	//실제 이미지 X 넓이의 오른쪽 좌표에서 220 뺀값을 기준으로 Tab Find Pos 설정
	int nTabFindPos = ( nWidth - 220 ) ; 

	//Cell 기준의 검사 이미지 데이터 정보를 저장하기 위한 클래스 벡터(vector 컨테이너)
	//_PET_INFO 구조체 : PET 정보를 담을 벡터 컨테이너
	VEC_PET_INFO* pvstPetInfo = new VEC_PET_INFO;
	pvstPetInfo->clear();

	//PET 필름 Cell 부분을 확인한다.
	//TOP 이미지 데이터 만을 검사 한다.
	BOOL bIsPET = CImageProcess::FindPetFilm(pImgPtr, nWidth, nHeight, RecipeInfo, pvstPetInfo, CAM_POS_TOP);

	//PET 가 아닐 경우  Sector 정보를 얻는다.
	if (bIsPET == FALSE)
	{
		//모드가 음극 일 경우 처리
		if (AprData.m_System.m_nMachineMode == ANODE_MODE)
		{
			//섹터 정보를 얻는다.
			//실제 이미지 정보 이미지 데이터, 넓이, 높이, 
			//TabFindPos : Tab 찾기 위한 위치 시작 점
			//RecipeInfo : 레시피 정보
			//vecSector : Cell를 만들기 위한 Sector 정보 받음
			nLocalRet = CImageProcess::FindTab_Negative(pImgPtr, nWidth, nHeight, nTabFindPos, &RecipeInfo, &vecSector, &nLevel);
			//섹터 정보가 0보다 작다면
			if (nLocalRet < 0)
			{
				//	return nLocalRet; // 21.12.28 Ahn Delete 
			}
			//롤의 윗부분과 아랫부분의 평균 점이 세라믹 경계선이 된다.
			//X 넓이에서 롤과의 경계점을 찾은 값
			//Tab을 가운데로 양쪽에 롤 존재 : 1/2 평균 중점
			*pnLevel = nLevel;

		}
		//모드가 양극 일 경우 처리
		else
		{
			//섹터 정보를 얻는다.
			//실제 이미지 정보 이미지 데이터, 넓이, 높이, 
			//TabFindPos : Tab 찾기 위한 위치 시작 점
			//RecipeInfo : 레시피 정보
			//vecSector : Cell를 만들기 위한 Sector 정보 받음
			nLocalRet = CImageProcess::FindTabLevel_Simple(pImgPtr, nWidth, nHeight, nTabFindPos, &RecipeInfo, &vecSector, &nLevel);

			//롤의 윗부분과 아랫부분의 평균 점이 세라믹 경계선이 된다.
			//X 넓이에서 롤과의 경계점을 찾은 값
			//Tab을 가운데로 양쪽에 롤 존재 : 1/2 평균 중점
			*pnLevel = nLevel;

		}
	}
	else
	{		
		AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> [ Detected PET - TAB ]"));
	}
	//PET 정보가 있다면
	//정보 삭제
	if (pvstPetInfo != NULL) {
		pvstPetInfo->clear();
		delete pvstPetInfo;
		pvstPetInfo = NULL;
	}


	int nLastSavePos = 0;
	//찾은 섹터의 갯수
	int nSize = (int)vecSector.size();

	//섹터 정보가 있음
	bool bSectorInfo = TRUE;
	if (nSize == 0)
	{
		AprData.SaveDebugLog(_T("==에러== 이미지에서 Sector 정보 찾지 못함"));
		bSectorInfo = FALSE;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// pyjtest : 원본 이미지 저장
	//{
	//	CBitmapStd bmp(nWidth, nHeight);
	//	bmp.SetImage(nWidth, nHeight, pImgPtr);

	//	CTime time = CTime::GetCurrentTime();

	//	CString strPath = AprData.m_strImagePath + _T("\\NON");
	//	CWin32File::CreateDirectory(strPath);

	//	CString str;
	//	str.Format(_T("%s\\%04d%02d%02d_%02d%02d%02d(%03d)_FC[%d]_Org.bmp"), strPath, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), GetTickCount(), nFrameCount);
	//	bmp.SaveBitmap(str);
	//}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Tab Info를 만들기 위한 case 
	int nCase = -1;
	//이전에 얻은 Image 데이터가 있다면
	//Cell : 검사를 위한 Y축(TabPitch : 레시피 설정 범위의 )으로 자른 단위
	//이전 이미지 데이터에서 Cell를 만들고 남은 이미지 데이터
	if( pResvTabInfo->pImgPtr != NULL )
	{
		// 지난 Frame에서 보내지 못하고 남은 Image가 있음.
		//Tab Info를 담을 클래스 객체
		CTabInfo tabInfo;
		//남은 이전 Top(Tab이 있는 이미지 데이터)
		BYTE *pTempPtr = pResvTabInfo->pImgPtr->m_pImagePtr;
		//남은 이전 Bottom(Tab이 없는 이미지 데이터)
		BYTE *pTempBtmPtr = pResvTabInfo->pImgBtmPtr->m_pImagePtr;


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// pyjtest : 원본 이미지 저장
		//{
		//	CString str;
		//	str.Format(_T("pResvTabInfo_%d"), pResvTabInfo->nFrameCount);

		//	CImageProcess::SaveOriginImage(pTempPtr, nWidth, pResvTabInfo->nImageLength, str);
		//}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////


		// PEC 부분인가 설정
		tabInfo.m_bIsPET = bIsPET;

		//이전 이미지에 지금 얻은 이미지를 Y으로 가져올 길이
		//새로 들어온 이미지를 이전 이미지에 붙여서 보낼 높이이다.(Y축 높이)
		int nSendLength = 0;

		//새로온 이미지에서 Tab 정보가 없을 때
		//섹터 정보가 없을 경우
		if (nSize <= 0)
		{
			//백업된 이전 Tab Info에서 Image 길이가 Tab Pitch 보다 크면 
			//이전 남은 이미지 크기가 레시피 설정 텝 피치를 벗어날 경우
			if (pResvTabInfo->nImageLength > nBaseTabPitch)
			{
				//이전 데이터가 이미 Cell 크기를 가지고 있으므로 전체를 하나의 셀로 보낸다.
				nCase = 0;
			}
			//이전 남은 이미지 데이터 Tab Width(탭의 넓이) 0이하
			//탭이 없을 때
			else if (pResvTabInfo->nTabWidth <= 0)
			{
				//다음 이미지를 Cell 크기에 맞게 잘라 붙여서 보낸다.
				nCase = 1;
			}
		}
		//Tab 에 대한 Sector 정보가 있다.
		else
		{ 
			//이전 남은 Tab Info에 Tab Width 정보가 있다면
			if (pResvTabInfo->nTabWidth > 0)
			{
				//이전 Tab Info 길이에서 Tab Left를 뺀값(탭 시작점 남는 길이) 에 새로 얻은 이미지 섹터(tab 정보의 끝 위치까지)끝까지 더해서
				int nTabWidth = (pResvTabInfo->nImageLength - pResvTabInfo->nTabLeft) + vecSector[0].nEndPos;

				//레시피에 설정한 Tab Width의 1.2배 안에(작으면) 들어오면
				//새로운 이미지 정보의 섹터 정보에 의해 Cell 정보를 만들 수 있는지 판단
				if (nTabWidth <= (int)((double)nBaseTabWidth * 1.2))
				{
					//다음 섹터 정보에서 붙일 이미지 정보를 찾는다.
					nCase = 3;
				}
				//Tab Width를 넘어갈 경우 Sector 의 Tab 정보를 사용하지 않는다.
				//이진 이미지에 온전한 Tab width 정보가 있음
				else
				{ 
					//다음 이미지의 남은 Cell 길이 만큼만 붙여 넣는다.
					nCase = 2;
				}
			}
			//이전에 남은 Tab Info에 Tab Width 가 없다.
			//Tab Width 크기가 0이다.
			else
			{ 

				// 이전 Tab Info 전체 길이 + 새로운 Image Sector 정보의 End pos 가 레시피 설정 Tab Pitch를 넘어간다.
				//밑에서 사용 없음 : case 이 안나올 가능성
				if ((pResvTabInfo->nImageLength + vecSector[0].nEndPos) > (int)((double)nBaseTabPitch * 1.2))
				{
					nCase = 5;
				}
				// 이전 Tab Info 전체 길이 + 새로운 Image Sector 정보의 End pos 가 레시피 Tab Pitch 보다는 작아야 한다.
				else
				{
					nCase = 4;
				}
			}
		}

		AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> <nCase=%d>"), nCase);

		BOOL bResvSend = FALSE;
		switch (nCase)
		{
		case	0 : 
			//tab Info
			//Cell 의 왼쪽 시작 점(Y축 시작)
			tabInfo.nLeft = 0;
			//Cell의 오른쪽 끝점(y축 높이)
			tabInfo.nRight = pResvTabInfo->nImageLength;
			//Cell의 Y축 시작점에서 끝점까지 거리
			tabInfo.nImageLength = pResvTabInfo->nImageLength;
			//tab 정보가 잘못 되었음 설정
			tabInfo.m_bErrorFlag = TRUE;

			AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> <nCase=%d> <CTabInfo> m_bErrorFlag=%d"), nCase, tabInfo.m_bErrorFlag );

			//tab Info의 Image 데이터 정보 메모리 생성
			//Top : Tab 있는 Image
			tabInfo.pImgPtr = new FrameImagePtr();
			memset(tabInfo.pImgPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* tabInfo.nImageLength + 1);
			//Bottom : Tab이 없는 image
			tabInfo.pImgBtmPtr = new FrameImagePtr();
			memset(tabInfo.pImgBtmPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* tabInfo.nImageLength + 1);


			//tab Info에 만들어지는 Grab에서 얻은 순서(count) 세팅
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount ;
			//이전 남은 정보의 시작위치 점
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;

			//생성된 tab Info Image 메모리에 Copy
			//Top : Tab이 있는 이미지
			CopyMemory(tabInfo.pImgPtr->m_pImagePtr, pTempPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			//Bottom : Tab 이 없는 Image
			CopyMemory(tabInfo.pImgBtmPtr->m_pImagePtr, pTempBtmPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			//마지막 점
			nLastSavePos = 0 ; 


			AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> case 0 nLastSavePos = %d"), nLastSavePos);

			break;


		case	1: 
			//tab Info
			//Cell 의 왼쪽 시작 점(Y축 시작)
			tabInfo.nLeft = 0;
			//Cell를 만들기 위해 y축(잘라 붙여야 되는 길이 계산
			nSendLength = nBaseTabPitch - pResvTabInfo->nImageLength ;
			//Cell의 오른쪽 끝점
			tabInfo.nRight = pResvTabInfo->nImageLength + nSendLength ;
			//Cell의 시작점에서 끝점까지 거리
			tabInfo.nImageLength = pResvTabInfo->nImageLength + nSendLength ;
			//tab Info에 에러 세팅
			tabInfo.m_bErrorFlag = TRUE;

			AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> <nCase=%d> <CTabInfo> m_bErrorFlag=%d"), nCase, tabInfo.m_bErrorFlag);

			//tab Info의 Image 데이터 정보 메모리 생성
			//Top : Tab 있는 Image
			tabInfo.pImgPtr = new FrameImagePtr();
			memset(tabInfo.pImgPtr->m_pImagePtr, 0x00, sizeof(BYTE) * nWidth * tabInfo.nImageLength + 1);

			//Bottom : Tab이 없는 image
			tabInfo.pImgBtmPtr = new FrameImagePtr();
			memset(tabInfo.pImgBtmPtr->m_pImagePtr, 0x00, sizeof(BYTE) * nWidth * tabInfo.nImageLength + 1);
			
			//사용되는 Grab Image 번호
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;


			//생성된 tab Info Image 메모리에 Copy
			//Top 에 이전 Top 이미지 복사
			CopyMemory(tabInfo.pImgPtr->m_pImagePtr, pTempPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			//Bottom에 이전 Bottom 이미지 복사
			CopyMemory(tabInfo.pImgBtmPtr->m_pImagePtr, pTempBtmPtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			//Top에 새로얻은 Image를 nSendLength 만큼 덧 붙인다.
			CopyMemory(tabInfo.pImgPtr->m_pImagePtr+(nWidth * pResvTabInfo->nImageLength), pImgPtr, sizeof(BYTE) * nWidth * nSendLength);
			//Bottom에 새로얻은 Image를 nSendLength 만큼 덧 붙인다.
			CopyMemory(tabInfo.pImgBtmPtr->m_pImagePtr + (nWidth * pResvTabInfo->nImageLength), pImgBtmPtr, sizeof(BYTE) * nWidth * nSendLength);

			//새로 들어온 이미지에서 잘라 붙인 크기 만큼의 위치
			nLastSavePos = nSendLength;

			AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> case 1 nLastSavePos = %d"), nLastSavePos);

			break;

		case	2 : 
			//남은 이미지에 Tab Width 가 남아 있을 경우 세팅 값
			//tab Info
			// 이전 Tab Info에서 Tab의 왼쪽 점 세팅(Cell의 Tab 시작 점이 된다)
			tabInfo.nTabLeft = pResvTabInfo->nTabLeft;
			// 이전 Tab Info에서 Tab의 오른쪽 점 세팅
			//이전 남은 정보에 Tab Width가 모두 있다.
			tabInfo.nTabRight = pResvTabInfo->nTabRight;

			//이전 Tab Info의 Image 순서 세팅
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;
			
			//새로 들어온 이미지를 사용해야 하는가를 설정
			//새로운 이미지 Copy 해서 Cell 정보를 만든다.
			bResvSend = TRUE;

			break;


		case	3 :	

			//남은 이미지에 Tab Width 가 남아 있을 경우 세팅 값
			//tab Info
			// 이전 Tab Info에서 Tab의 왼쪽 점 세팅(Cell의 Tab 시작 점이 된다)
			tabInfo.nTabLeft = pResvTabInfo->nTabLeft;
			//Tab Width 정보가 작으므로 이전 전체 길이에 새로 얻은 섹터 정보의 Tab 정보를 더한다.
			//온전한 Tab Width 길이를 만든다.
			tabInfo.nTabRight = pResvTabInfo->nImageLength + vecSector[0].nEndPos ;


			//이전 Tab Info의 Image 순서 세팅
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;
			
			//사용한 섹터 정보를 지운다.
			vecSector.erase(vecSector.begin());

			//새로 들어온 이미지를 사용해야 하는가를 설정
			//새로운 이미지 Copy 해서 Cell 정보를 만든다.
			bResvSend = TRUE;

			break;


		case	4 :	

			//이전에 남은 Tab Info에 Tab Width 가 없다.
			//tab Info
			// 이진 남은 Tab Info에 Tab Width 가 없으므로 새로 얻은 Tab Width 시작점이 Tab Left가 된다.
			tabInfo.nTabLeft = pResvTabInfo->nImageLength + vecSector[0].nStartPos;
			//5 case가 아니므로 새로 얻은 Image의 Sector 정보의 Tab Width 가 Tab Right 가 된다.
			tabInfo.nTabRight = pResvTabInfo->nImageLength + vecSector[0].nEndPos;
			
			// 이전 Tab Info의 Image 순서 세팅
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame;

			//사용한 섹터 정보를 지운다.
			vecSector.erase(vecSector.begin());

			//새로 들어온 이미지를 사용해야 하는가를 설정
			//새로운 이미지 Copy 해서 Cell 정보를 만든다.
			bResvSend = TRUE;

			break;


		default :
			break;
		}

		//새로 얻은 이미지에서 Image 데이터를 덧 붙어야 할 경우
		if (bResvSend == TRUE)
		{
			//만들어 지는 tab Info
			//Cell에서 Tab Width 의 중앙(TabWidth 중앙)
			//2,3,4 case에서 Tab Width 왼쪽 오른쪽 점 정보은 모두 얻어서 
			tabInfo.nCenter = (tabInfo.nTabRight + tabInfo.nTabLeft) / 2;

			//Cell의 오른쪽 끝점 은 : Tab Width 중앙에서 레시피 설정 치 Tab Pitch의 반(Half) 를 더하면 나온다.
			tabInfo.nRight = tabInfo.nCenter + nBaseTabHalfPitch;

			//새로운 이미지 정보에서 남은 이전 Tab Info에 붙여질 Image 길이를 계산한다.
			nSendLength = tabInfo.nRight - pResvTabInfo->nImageLength;

			AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> nSendLength(%d) = tabInfo.nRight(%d) - pResvTabInfo->nImageLength(%d)"), nSendLength, tabInfo.nRight, pResvTabInfo->nImageLength);


			//새로운 Image 의 얻은 Y축 높이가(Image 높이) 붙여야 하는 Image 길이보다 작으면 
			//얻은 이미지 높이 값으로 세팅한다. 예외처리
			if (nSendLength > nHeight)
			{
				nSendLength = nHeight;
			}

			//붙여질 새로운 이미지의 길이에 남은 Image 길이가 Tab Info (한 Cell의) 전체 길이가 된다.
			tabInfo.nImageLength = nSendLength + pResvTabInfo->nImageLength;

			//tab Info의 Image 데이터 정보 메모리 생성
			//Top : Tab 있는 Image
			tabInfo.pImgPtr = new FrameImagePtr();
			memset(tabInfo.pImgPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* tabInfo.nImageLength + 1);

			//이전 남은 Image를 Tab Info 정보로 copy
			CopyMemory(tabInfo.pImgPtr->m_pImagePtr, pResvTabInfo->pImgPtr->m_pImagePtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			//새로운 Image 정보를 덧붙일 길이만큼 잘라 붙인다.
			CopyMemory(tabInfo.pImgPtr->m_pImagePtr + (nWidth * pResvTabInfo->nImageLength), pImgPtr, sizeof(BYTE) * nWidth * nSendLength);

			//tab Info의 Image 데이터 정보 메모리 생성
			//Bottom : Tab 없는 Image
			tabInfo.pImgBtmPtr = new FrameImagePtr();
			memset(tabInfo.pImgBtmPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* tabInfo.nImageLength + 1);

			//이전 남은 Image를 Tab Info 정보로 copy
			CopyMemory(tabInfo.pImgBtmPtr->m_pImagePtr, pResvTabInfo->pImgBtmPtr->m_pImagePtr, sizeof(BYTE) * nWidth * pResvTabInfo->nImageLength);
			//새로운 Image 정보를 덧붙일 길이만큼 잘라 붙인다.
			CopyMemory(tabInfo.pImgBtmPtr->m_pImagePtr + (nWidth * pResvTabInfo->nImageLength), pImgBtmPtr, sizeof(BYTE) * nWidth * nSendLength);

			//새로 들어온 이미지에서 잘라 붙인 크기 만큼의 위치
			//새로 얻은 이미지에서 Cell로만들고 남은 Image 좌표
			nLastSavePos = nSendLength;

			AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> bResvSend == TRUE nLastSavePos = %d, tabInfo.nRight = %d, pResvTabInfo->nImageLength = %d"), nLastSavePos, tabInfo.nRight, pResvTabInfo->nImageLength );


			// 이전 Tab Info의 Image 순서 세팅
			tabInfo.nFrameCount = pResvTabInfo->nFrameCount ;
			tabInfo.nTabStartPosInFrame = pResvTabInfo->nTabStartPosInFrame ;

		}

		//텝정보를 저장한다.
		//이전 남은 Tab Info가 있을 때 만들어 진 정보를 push한다.
		pVecTabInfo->push_back(tabInfo);
		AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> pVecTabInfo->push_back 1 Size<%d> "), pVecTabInfo->size());


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// pyjtest : 원본 이미지 저장
		//{
		//	CString str;
		//	str.Format(_T("Vec%d_%d"), pVecTabInfo->size(), tabInfo.nFrameCount);

		//	CImageProcess::SaveOriginImage(tabInfo.pImgPtr->m_pImagePtr, nWidth, tabInfo.nImageLength, str);
		//}

		//{
		//	CString str;
		//	str.Format(_T("Vec%d_%d"), pVecTabInfo->size(), nFrameCount);

		//	CImageProcess::SaveOriginImage(pImgPtr, nWidth, nHeight, str);
		//}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////












		// 미결함 NG 발생 시 원본 이미지 저장
		if (AprData.m_System.m_bNonNgSave == TRUE)
		{
			if (nLevel <= 0 || nLevel >= 2999 || tabInfo.m_bErrorFlag == TRUE)
			{
				CBitmapStd bmp(nWidth, nHeight);
				bmp.SetImage(nWidth, nHeight, pImgPtr);

				CTime time = CTime::GetCurrentTime();

				CString strPath = AprData.m_strImagePath + _T("\\NON");
				CWin32File::CreateDirectory(strPath);

				CString str;
				str.Format(_T("%s\\%04d%02d%02d_%02d%02d%02d(%03d).bmp"), strPath, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), GetTickCount());
				bmp.SaveBitmap(str);
			}
		}




		//사용 된 이전 남은 Tab Info 정보는 삭제한다.
		//Top Image 정보 삭제
		if (pResvTabInfo->pImgPtr != NULL)
		{
			delete pResvTabInfo->pImgPtr;
			pResvTabInfo->pImgPtr = NULL;

		}
		//Bottom Image 정보 삭제
		if (pResvTabInfo->pImgBtmPtr != NULL)
		{
			delete pResvTabInfo->pImgBtmPtr;
			pResvTabInfo->pImgBtmPtr = NULL;
		}
		//가지고 있던 이전 Tab Info 를 초기화한다.
		pResvTabInfo->ResetData();
	}

	nSize = (int)vecSector.size();

	if (bSectorInfo && (nSize == 0))
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<DivisionTab_FromImageToTabInfo>>처리 - 이미지처리 Case<%d> ** 이미지에서 Sector 정보 찾지았지만 기존이미지 통합과정에서 삭제됨"), nCase);
	}

	//남은 Tab Info Image 가 있을 때 2, 3, 4 case 시 들어온다. : 남은 섹터 정보
	//남은 Tab Info Image 가 없을 때 : nLastSavePos 0
	if (nSize <= 0) 
	{
		//잘려지고 남은 길이는 case 1 이다.
		int nSendAbleLeng = (nHeight - nLastSavePos);
		//얻은 Image의 Y 넓이가 레시피 설정  Pitch 몇개 인가 계산
		int nSendAbleCount = nSendAbleLeng / nBaseTabPitch ;

		//레시피에 설정한 Tab Pitch 로 자른다. : nSendAbleCount > 0 이면
		nLastSavePos = DivisionTab_byFixSize(pImgPtr, pImgBtmPtr, nWidth, nHeight, nBaseTabPitch, nLastSavePos, nHeight, bIsPET, pVecTabInfo);

		AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> nSize <= 0 nLastSavePos = %d"), nLastSavePos);

	}
	//섹터 정보가 있을 경우
	//남은 Tab Info Image 가 있을 때 : 0, 1 로 쓰지 않은 섹터 정보가 남아 있다.
	//남은 Tab Info Image 가 없을 때 : 섹터 정보가 있다.
	else
	{
		//nPairSholderLength : 이전 Tab 끝과 다음 Tab 처음 사이의 길이의 반 : Cell의 두 롤 부분의 한쪽 롤 길이
		//새로운 이미지에서 왼쪽의 시작 점 : nLastSavePos 0 이면 이전 Tab Info 가 없는 것이다.
		//새로 얻은 섹터 nStartPos 길이에서  한쪽 롤 길이와 이전 Tab Info에 잘라 붙이고 남은 길이를 합한 값을 뺀다.
		//Start Pos 에서 nPairSholderLength를 빼면 딱 Cell의 시작 점이 된다.
		int nLeftSize = ( ( vecSector[0].nStartPos - nPairSholderLength ) - nLastSavePos);
		int nDivCnt = 0;

		//새로 얻은 섹터 nStartPos 길이에서  한쪽 롤 길이와 이전 Tab Info에 잘라 붙이고 남은 길이를 합한 값을 뺀 값이 Cell 얻을 길이가 된다면 ?
		//가능성 없음 ? 한번에  큰 Image Pixel를 얻을 때는 나올 수 있다.
		if (nLeftSize > nBaseTabPitch ) 
		{
			//레시피 설정 Tab Pitch 로 나누어 몇개 나오나 계산
			nDivCnt = nLeftSize / nBaseTabPitch ;
			//남는 다면 계산한다.
			if (nDivCnt > 0) 
			{
				nLastSavePos = DivisionTab_byFixSize(pImgPtr, pImgBtmPtr, nWidth, nHeight, nBaseTabPitch, nLastSavePos, nLastSavePos + nLeftSize, bIsPET, pVecTabInfo);

				AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> nDivCnt > 0 nLastSavePos = %d"), nLastSavePos);

			}
		}		

		AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> <vecSector.Size=%d>"), nSize);

		//새로운 Image 에서 섹터 정보가 있다면 
		//섹터 정보 만큼 
		for (int i = 0; i < nSize; i++) 
		{
			//Tab Info 클래스 객체
			CTabInfo tabInfo;
			//섹터 정보의 Tab Width 시작 위치가 사용한 위치 보다 작거나 같다면
			//Tab Info를 만들지 않는다.
			if (vecSector[i].nStartPos <= nLastSavePos) 
			{
				//DEBUG_LOG.txt
				AprData.SaveDebugLog_Format(_T("<<DivisionTab_FromImageToTabInfo>>처리 - 이미지처리 Case<%d> Scetor 정보 처리 ** Scetor Start Pos가 Last Save Pos 보다 작거나 같을 때 continue(패스) 처리번호<%d/%d>"), nCase, i, nSize);
				continue;
			}

			//섹터 정보의 Tab Width 시작 위치가 사용한 위치 보다 크면
			//Tab Width의 왼쪽 점
			tabInfo.nTabLeft = vecSector[i].nStartPos - nLastSavePos;
			//Tab Width 오른쪽 점
			tabInfo.nTabRight = vecSector[i].nEndPos - nLastSavePos;
			//Tab Width 의 길이
			tabInfo.nTabWidth = tabInfo.nTabRight - tabInfo.nTabLeft;
			//Tab Info Cell의 왼쪽 점
			tabInfo.nLeft = 0;
			//Tab Width 중앙을 Center로
			tabInfo.nCenter = (tabInfo.nTabRight + tabInfo.nTabLeft) / 2;
			

			//탭의 시작점이 0이면
			if (vecSector[i].nStartPos == 0) {
				//nPairSholderLength : 이전 Tab 끝과 다음 Tab 처음 사이의 길이의 반 : Cell의 두 롤 부분의 한쪽 롤 길이
				//Tab width 에 한쪽 롤 크기 더한다.
				tabInfo.nRight = tabInfo.nTabRight + nPairSholderLength;

			}
			else
			{
				//Cell의 중앙 점에서 레시피 Tab Pitch 의 반을 더한다.
				tabInfo.nRight = tabInfo.nCenter + nBaseTabHalfPitch;

			}

			//Cell의 중앙 점에서 레시피 Tab Pitch 의 반을 더한다.
			tabInfo.nRight = tabInfo.nCenter + nBaseTabHalfPitch;
			//nRight 가 Cell 의 총 길이가 된다.
			tabInfo.nImageLength = tabInfo.nRight ;

			AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> (tabInfo.nImageLength<%d> + nLastSavePos<%d>) > nHeight<%d> "), tabInfo.nImageLength, nLastSavePos, nHeight);

			//지금 만들 Cell 전체 길이 nImageLength 에 이전 Cell에 사용 한 nLastSavePos 의 합의 길이가
			//새로 얻은 이미지 높이(y축 길이 : 진행 방향)보다 크면 백업
			if ((tabInfo.nImageLength + nLastSavePos) > nHeight)
			{ 
				//새로 얻은 이미지 Height 에서 모자라는 값 계산
				int nBackupSize = tabInfo.nImageLength -( (tabInfo.nImageLength + nLastSavePos) - nHeight ) ;

				//Tab Info 백업 정보로 세팅한다.
				*pResvTabInfo = tabInfo;
				//기존의 사용한 Top Image 메모리 데이터 삭제
				if (pResvTabInfo->pImgPtr != NULL) {
					delete pResvTabInfo->pImgPtr;
				}
				//기존 사용한 Botton Image 메모리 데이터 삭제
				if (pResvTabInfo->pImgBtmPtr != NULL) {
					delete pResvTabInfo->pImgBtmPtr;
				}

				//백업 Image 메모리를 생성한다.
				//Top Image 데이터 
				pResvTabInfo->pImgPtr = new FrameImagePtr();
				memset(pResvTabInfo->pImgPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* nBackupSize + 1);

				//Bottom Image 데이터
				pResvTabInfo->pImgBtmPtr = new FrameImagePtr();
				memset(pResvTabInfo->pImgBtmPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* nBackupSize + 1);
				//백업할 계산된 사이즈
				pResvTabInfo->nImageLength = nBackupSize;

				// 지금 image 순서
				pResvTabInfo->nFrameCount = nFrameCount;
				//지금 이미지에서 사용하고 남은 위치
				pResvTabInfo->nTabStartPosInFrame = nLastSavePos;

				//Image backup 저장한다.(다음 Image 정보와 합하여 사용 됨)
				// top 남은 이미지를 copy
				CopyMemory(pResvTabInfo->pImgPtr->m_pImagePtr, pImgPtr + ( nWidth * nLastSavePos ) , sizeof(BYTE) * nWidth * nBackupSize);
				//bottom 남은 이미지를 copy
				CopyMemory(pResvTabInfo->pImgBtmPtr->m_pImagePtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE)* nWidth* nBackupSize);

				//DEBUG_LOG.txt
				AprData.SaveDebugLog_Format(_T("<<DivisionTab_FromImageToTabInfo>>처리 - 이미지처리 Case<%d> Scetor 정보 처리 ** 보낼 이미지 사이즈가 남은 이미지 사이즈 보다 큰경우 처리종료 처리번호<%d/%d>"), nCase, i, nSize);

				return 0;
			}

			//지금 만들 Cell 전체 길이 nImageLength 에 이전 Cell에 사용 한 nLastSavePos 의 합의 길이가
			//새로 얻은 이미지 높이(y축 길이 : 진행 방향)보다 작거나 같으면 백업하지 않는다.
			//Tab Info Cell Image 객체 생성
			//Top Image 데이터 저장 메모리 생성
			tabInfo.pImgPtr = new FrameImagePtr();
			memset(tabInfo.pImgPtr->m_pImagePtr, 0x00, sizeof(BYTE)* tabInfo.nImageLength* nWidth + 1);
			//Image 데이터 copy
			CopyMemory(tabInfo.pImgPtr->m_pImagePtr, pImgPtr + (nWidth * nLastSavePos), sizeof(BYTE) * nWidth * tabInfo.nImageLength);

			//Bottom Image 데이터 저장 메모리 생성
			tabInfo.pImgBtmPtr = new FrameImagePtr();
			memset(tabInfo.pImgBtmPtr->m_pImagePtr, 0x00, sizeof(BYTE) * tabInfo.nImageLength * nWidth + 1);
			//Image 데이터 copy
			CopyMemory(tabInfo.pImgBtmPtr->m_pImagePtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE)* nWidth* tabInfo.nImageLength);

			//grab Image 얻은 순서
			tabInfo.nFrameCount = nFrameCount ;
			//얻은 Image 의 사용하고 남은 위치 점
			tabInfo.nTabStartPosInFrame = nLastSavePos ;

			//남은 이미지 점 다시 계산 for 문
			nLastSavePos = nLastSavePos + tabInfo.nImageLength;
			//만들어진 Cell 정보를 저장한다.
			pVecTabInfo->push_back(tabInfo);

			AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> pVecTabInfo->push_back(tabInfo) nLastSavePos = %d"), nLastSavePos);


			AprData.SaveDebugLog_Format(_T("<DivisionTab_FromImageToTabInfo> pVecTabInfo->push_back 2 Size<%d> "), pVecTabInfo->size() );




			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			// pyjtest : 원본 이미지 저장
			//{
			//	CString str;
			//	str.Format(_T("Vec%d_%d"), pVecTabInfo->size(), tabInfo.nFrameCount);

			//	CImageProcess::SaveOriginImage(tabInfo.pImgPtr->m_pImagePtr, nWidth, tabInfo.nImageLength, str);
			//}

			//{
			//	CString str;
			//	str.Format(_T("Vec%d_%d"), pVecTabInfo->size(), nFrameCount);

			//	CImageProcess::SaveOriginImage(pImgPtr, nWidth, nHeight, str);
			//}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////

		}	
		
		//새로 얻은 Image 의 높이에서 마지막 사용하고 남은 높이(크기)를 뺀 값이
		//레시피 설정 Tab Pitch 보다 크면 FixSize Tab Info 정보를 만든다.
		int nRightSize = (nHeight - nLastSavePos) ;
		if ( nRightSize > nBaseTabPitch )
		{
			//Tab Info 만들어 질 갯수
			int nSendLeng = nRightSize / nBaseTabPitch;
			//FixSize Tab Info 생성
			nLastSavePos = DivisionTab_byFixSize(pImgPtr, pImgBtmPtr, nWidth, nHeight, nBaseTabPitch, nLastSavePos, nHeight, bIsPET, pVecTabInfo);

			AprData.SaveDebugLog_Format(_T(">>>>>>>>>>>>>>>>>>>> nRightSize > nBaseTabPitch nLastSavePos = %d"), nLastSavePos);

		}

	}

	//새로 얻은 Image 의 높이에서 마지막 사용하고 남은 높이(크기)를 뺀 값이 있다면 다음 Image 취득 시 이용하기 위해서 백업한다.
	int nLeftSize = nHeight - nLastSavePos;
	if ( nLeftSize > 0 )
	{
		//백업 객체 의 Image 데이터 메모리 생성
		//Top Image 데이터
		pResvTabInfo->pImgPtr = new FrameImagePtr();
		memset(pResvTabInfo->pImgPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* nLeftSize + 1);
		//Bottom Imae 데이터
		pResvTabInfo->pImgBtmPtr = new FrameImagePtr();
		memset(pResvTabInfo->pImgBtmPtr->m_pImagePtr, 0x00, sizeof(BYTE)* nWidth* nLeftSize + 1);
		//사용하고 남은 Image 길이
		pResvTabInfo->nImageLength = nLeftSize;

		//Grab Image 얻은 순서
		pResvTabInfo->nFrameCount = nFrameCount ;
		//새로얻은 이미지에서 쓰고 남은 점
		pResvTabInfo->nTabStartPosInFrame = nLastSavePos ;

		//Top Image 데이터 copy
		CopyMemory(pResvTabInfo->pImgPtr->m_pImagePtr, pImgPtr + (nWidth * nLastSavePos) , sizeof(BYTE) * nWidth * nLeftSize);
		//Bottom Image 데이트 copy
		CopyMemory(pResvTabInfo->pImgBtmPtr->m_pImagePtr, pImgBtmPtr + (nWidth * nLastSavePos), sizeof(BYTE)* nWidth* nLeftSize);

		AprData.SaveDebugLog_Format( _T("<DivisionTab_FromImageToTabInfo> CopyMemory pResvTabInfo->pImgPtr") );

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
	// 인자 사용에 방향성 있음
	// TOP, BTM 이미지 기준으로 vecFirst 안쪽 결함(Foil-IN), vecSecond 바깥쪽(Foil Out) 결함으로 사용하여야함
	// ex) Top의 경우 vecFirst = Foil-In 결함, vecSecond = Foil-Out 결함
	// ex) Btm의 경우 vecFirst = Foil-Out 결함, vecSecond = Foil-In 결함


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

	for (; (iterF != vecFirst.end()) || ((iterS != vecSecond.end())); )
	{
		if ((iterF != vecFirst.end()) && ((iterS != vecSecond.end())))
		{
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
int CImageProcess::ImageProcessDetectSurface(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, CRect rcArea, CTabRsltInfo* pTabRsltInfo, int nCamPos, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt )
{
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessDetectSurface> Start : nWidth = %d, nHeight = %d, rcArea = (%d,%d,%d,%d), nCamPos = %d, bSimMode = %d, nArrCnt = %d"), 
//		nWidth, nHeight, rcArea.left, rcArea.top, rcArea.right, rcArea.bottom, nCamPos, bSimMode, nArrCnt);

	int nRet = 0 ;
	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

	
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
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessDetectSurface> Threshold : Finish"));

	CImageProcess::LoopLabeling(&roiSpetter, nWidth, nHeight, &vecBlockSpetter, CImageProcess::en_Surface_Bit, pRecipeInfo->m_nSurfaceMinSize[nCamPos], AprData.m_System.m_dResolX[nCamPos], AprData.m_System.m_dResolY);
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessDetectSurface> LoopLabeling : Finish"));

	CImageProcess::SortingBlockInfo(&vecBlockSpetter);
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessDetectSurface> SortingBlockInfo : Finish"));

	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockSpetter, pRecipeInfo, pTabRsltInfo, nCamPos, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY );
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessDetectSurface> AddDefectInfoByBlockInfo : Finish"));


	if (bSimMode == TRUE) {
		CopyMemory(pImgPtrArr[nArrCnt], pThresPtr, sizeof(BYTE) * nSizeAll);
	}

	delete[] pThresPtr;
	pThresPtr = NULL;

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessDetectSurface> End"));

	return 0 ;
}
// 22.05.09 Ahn Add End

// 23.02.10 Ahn Add Start
int CImageProcess::ImageProcessTopSide_BrightRoll(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode /*= 0 */ , BYTE** pImgPtrArr /*= NULL */ , int nArrCnt /*= 0*/)
{
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> Start : nWidth = %d, nHeight = %d, nLineLevel = %d, nTabLeft = %d, bSimMode = %d, nArrCnt = %d"), nWidth, nHeight, nLineLevel, nTabLeft, bSimMode, nArrCnt);

	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

	int nLeftOffset;
	int nRightOffset = 100 ; 
	int nTabRoundOffsetR;

	nLeftOffset = pRecipeInfo->TabCond.nNegVGrooveHeight; //(int)((double)pRecipeInfo->TabCond.nNegCoatHeight * 1.5);
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

	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> rcAll(%d,%d,%d,%d) rcLeft(%d,%d,%d,%d) rcRight(%d,%d,%d,%d)"),
		rcAll.left, rcAll.top, rcAll.right, rcAll.bottom,
		rcLeft.left, rcLeft.top, rcLeft.right, rcLeft.bottom,
		rcRight.left, rcRight.top, rcRight.right, rcRight.bottom
	);


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
	BYTE* pMeanPtr = new BYTE[nSizeAll + 1];

	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);
	int nMeanSize = MEAN_SIZE_HOR;

	int nHeadBndry;
	nHeadBndry = nLineLevel + nTabRoundOffsetR;

	int nMaskRight = nHeadBndry - 30;

	if (nMaskRight >= nWidth)
	{
		nMaskRight = nWidth - 1;
	}

	BYTE* pStdPtr = new BYTE[nHeight * nWidth + 1];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth + 1];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> ImageMean_Part1"));

	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> ImageMean_Part2"));

	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> GetMaxImage"));


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
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> EdgeDetectImageToBoth_BaseBright : nWidth = %d, nHeight = %d, nThresBnd = %d, nThresMax = %d, nLineLevel = %d, "), 
		nWidth, nHeight, nThresBnd, nThresMax, nLineLevel);

	nLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, pBndryPtr, &vecLeftRndInfo, nWidth, nHeight, rcLeft, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> EdgeDetectImageToBoth_BaseBright"));

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

	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> SmoothVecRoundData"));



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

	// Tab 오른쪽(아래) 부분의 Edge 검출

	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> EdgeDetectImageToBoth_BaseBright : nWidth = %d, nHeight = %d, nThresBnd = %d, nThresMax = %d, nLineLevel = %d"),
		nWidth, nHeight, nThresBnd, nThresMax, nLineLevel);

	nLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcRight, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);

	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> EdgeDetectImageToBoth_BaseBright"));


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
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> SmoothVecRoundData"));

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

	delete[]pnResultArr;

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nSizeAll + 1];
	BYTE* pDiffPtr;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
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
		pDiffPtr = new BYTE[nSizeAll + 1];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];

		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeft, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRight, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);

		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> DiffProcImage"));

	}


	roiFoilExp.SetRect(rcLeft);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecLeftRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFoileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);

	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> Threshold_RoundMask Left"));

	roiFoilExp.SetRect(rcRight);
	CImageProcess::Threshold_RoundMask(pDiffPtr, &roiFoilExp, &vecRightRndInfo, nWidth, nHeight, nFoilExpInspWidth, nFoilExpThLower, nFoileExpMaskOffset, nMaskRight, en_ModeFoilExp, FALSE);

	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> Threshold_RoundMask Right"));


	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> LoopLabeling"));

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
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> nMode =%d, nDefSize = %d"), nMode, nDefSize);

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
				//CImageProcess::CheckDistanceToEdgeLine(pData, &vecLeftRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				CImageProcess::CalcSizeToEdgeLine(pData, &vecLeftRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
			}
			else if (rcRight.PtInRect(cpLB) == TRUE) {
				//CImageProcess::CheckDistanceToEdgeLine(pData, &vecRightRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				CImageProcess::CalcSizeToEdgeLine(pData, &vecLeftRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
			}
		}
	}


	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, pRecipeInfo, CAM_POS_TOP);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> MergeAndLink_BlockInfo"));


	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> AddDefectInfoByBlockInfo"));

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

	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_BrightRoll> End"));

	return 0;
}
int CImageProcess::ImageProcessBottomSide_BrightRoll(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode /*= 0*/, BYTE** pImgPtrArr /*= NULL*/, int nArrCnt /*= 0*/)
{
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> Start : nWidth = %d, nHeight = %d, nLineLevel = %d, bSimMode = %d, nArrCnt = %d"), nWidth, nHeight, nLineLevel, bSimMode, nArrCnt);

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

	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> rcAll(%d,%d,%d,%d)"),
		rcAll.left, rcAll.top, rcAll.right, rcAll.bottom );


	int nSizeAll = nWidth * nHeight;
	BYTE* pTempPtr = new BYTE[nSizeAll + 1];
	memset(pTempPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);

	VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nEdgeWidth = 10;

	// 평균화 처리 
	BYTE* pMeanPtr = pTempPtr;
	int nMeanSize = MEAN_SIZE_HOR;

	BYTE* pStdPtr = new BYTE[nHeight * nWidth + 1];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth + 1];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> ImageMean_Part1"));
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> ImageMean_Part2"));
	CImageProcess::GetMaxImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> GetMaxImage"));

	BYTE* pDiffPtr;
	{
		pDiffPtr = new BYTE[nSizeAll + 1];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_BOTTOM];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, pRecipeInfo->bDarkEmpMode);
	}
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> DiffProcImage"));

	// 22.05.30 Ahn Add Start
	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr;
	// 22.05.30 Ahn Add End

	int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
	int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM];

	int nLocalRet = FALSE;
	nLocalRet = CImageProcess::EdgeDetectImageToBoth_BaseBright(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, nThresMax, CImageProcess::en_BottomSide, nLineLevel, CImageProcess::en_FindLeft);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> EdgeDetectImageToBoth_BaseBright"));

	if (nLocalRet < 0) {
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessBottomSide_BrightRoll>>에러 - Edge found faile, ret = %d"),
			nLocalRet);
		return -3;
	}

	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> SmoothVecRoundData"));


	CRegionInfo roiFoilExp;
	//	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nWidth * nHeight + 1];
	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
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
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> Threshold_RoundMask_Negative"));

	_VEC_BLOCK vecBlockFoilExp;
	_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());

	// Overflow 는 4번 Bit살려서 보냄
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, AprData.m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> LoopLabeling"));


	pTabRsltInfo->RemoveAll();

	_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	{
		{
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> nDefSize = %d"), nDefSize);

		for (int i = 0; i < nDefSize; i++)
		{
			pData = &(*pVecBlockPtr)[i];

			//CImageProcess::CheckDistanceToEdgeLine(pData, &vecAllRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);

			//if (pData->bDeleteFlag == TRUE) continue;
			//if (AprData.m_System.m_nMachineMode == ANODE_MODE) 
			{
				CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
			}
		}
	}

	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);
	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> AddDefectInfoByBlockInfo") );


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


	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_BrightRoll> End : nRet = %d"), nRet );

	return nRet;
	// 23.02.24 Ahn Add End
}
// 23.02.10 Ahn Add End

// Head부 검사 처리 함수.
int CImageProcess::ImageProcessTopSide_AreaDiff(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> Start : nWidth = %d, nHeight = %d, nLineLevel = %d, nTabLeft = %d, nTabRight = %d, bSimMode = %d, nArrCnt = %d"), nWidth, nHeight, nLineLevel, nTabLeft, nTabRight, bSimMode, nArrCnt);

	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

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

		if (pRecipeInfo->bEnableVGroove == TRUE)
		{
			nLeftOffset += pRecipeInfo->TabCond.nNegVGrooveHeight;
			nRightOffset += pRecipeInfo->TabCond.nNegVGrooveHeight;
		}
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
//	rcLeft.bottom = nTabLeft + 50; //pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
//	rcRight.top = nTabRight - 50;//pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	rcLeft.bottom = nTabLeft;
	rcRight.top = nTabRight;

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
	BYTE* pMeanPtr = new BYTE[nSizeAll + 1];

	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);
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

	if (nMaskRight >= nWidth)
	{
		nMaskRight = nWidth - 1;
	}




	BYTE* pStdPtr = new BYTE[nHeight * nWidth + 1];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth + 1];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeftRnd, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRightRnd, 15, 15, CImageProcess::en_FillBottom);

	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeftRnd, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRightRnd, 3, 3, CImageProcess::en_FillBottom);


	// 22.04.26 Ahn Add Start
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeftRnd);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRightRnd);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);
	// 22.04.26 Ahn Add End

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> ImageMean_Part, GetMinImage : Finish") );


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
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> EdgeDetectImageToBoth_RndInfo : Finish"));

	}
	else {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, NULL, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindLeft);
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> EdgeDetectImageToBoth_RndInfo_Threshold : Finish"));
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

	// Round는 가로 세로 2번의 Edge 검출이 필요함.
	rcLeftRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, NULL, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft);
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> GetBoundaryRoundInfo_ByBriDist : Finish"));
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> SmoothVecRoundData : Finish"));

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

	// Tab 오른쪽(아래) 부분의 Edge 검출

	nCropWidth = rcRightRnd.Width();
	nCropHeight = rcRightRnd.Height();
	// Round는 가로 세로 2번의 Edge 검출이 필요함.
	rcRightRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pEdgePtr, NULL, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight);
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> GetBoundaryRoundInfo_ByBriDist : Finish"));

	rcTemp = rcRight;
	rcTemp.top = rcRightRnd.bottom;
	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff) {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcTemp, nEdgeCompWidth, 2, DIR_VER);
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> EdgeDetectImageToBoth_RndInfo : Finish"));

	}
	else {
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, NULL, &vecRightRndInfo, nWidth, nHeight, rcTemp, nThresBnd, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> EdgeDetectImageToBoth_RndInfo_Threshold : Finish"));
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
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> SmoothVecRoundData : Finish"));

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


	//	delete[]pRsltPtr;
	delete[]pnResultArr;

	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nSizeAll + 1];
	BYTE* pDiffPtr;

	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
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

	{ 
		pDiffPtr = new BYTE[nSizeAll + 1];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];

		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeft, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRight, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftRnd, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightRnd, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> DiffProcImage : Finish"));
	}



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

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> Threshold_RoundMask : Finish"));


	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::RemoveNoise(pThresPtr, nWidth, nHeight, CImageProcess::en_Filter_8, rcAll, CImageProcess::en_FoilExp_Bit | CImageProcess::en_Dross_Bit);
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());

	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, pRecipeInfo->nFoilExpOutMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> LoopLabeling : Finish"));


	pTabRsltInfo->RemoveAll();


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

//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> nMode = %d, nDefSize = %d"), nMode, nDefSize);


		for (int i = 0; i < nDefSize; i++) {
			pData = &(*pVecBlockPtr)[i];
			if (pData->bDeleteFlag == TRUE) continue;
			CPoint ptCenter;
			ptCenter.x = (pData->rcRect.right + pData->rcRect.left) / 2;
			ptCenter.y = (pData->rcRect.bottom + pData->rcRect.top) / 2;
			// 22.11.18 Ahn Add Start
			//pTabRsltInfo->m_nFrameCount
			// 22.11.18 Ahn Add End

			if (rcLeftRnd.PtInRect(ptCenter) == TRUE)
			{
				pData->nDefPos = en_LeftRound; // 22.08.25 Ahn Add
				CImageProcess::CalcRoundDef_Size_LeftFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcLeftRnd, pData, &vecLeftRndInfo, CImageProcess::en_FindLeft, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
//				CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcLeftRnd, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
			}
			else if (rcRightRnd.PtInRect(ptCenter) == TRUE)
			{
				pData->nDefPos = en_RightRound; // 22.08.25 Ahn Add
				CImageProcess::CalcRoundDef_Size_RightFoilExp(pThresPtr, nWidth, nHeight, pData->nType, rcRightRnd, pData, &vecRightRndInfo, CImageProcess::en_FindRight, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, nMode);
//				CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcRightRnd, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
			}
			else
			{
				if (AprData.m_System.m_nMachineMode == CATHODE_MODE)
				{ 
					if (pData->rcRect.top >= vecRightRndInfo[0].y)
					{
						CImageProcess::CalcSizeToEdgeLine(pData, &vecRightRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
						CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
						pData->nDefPos = en_Right;
					}
					else
					{
						CImageProcess::CalcSizeToEdgeLine(pData, &vecLeftRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);
						CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
						pData->nDefPos = en_Left;
					}
				}
			}
		}
	}


	// 22.07.20 Ahn Move Start
	if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
		CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);
	}
	// 22.07.20 Ahn Move End

	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();
	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockFoilExp, vecBlockDross, pRecipeInfo, CAM_POS_TOP);

	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);


	if (pStdPtr != nullptr) {
		delete[]pStdPtr;
	}

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

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_AreaDiff> End"));

	return 0;
}


int CImageProcess::ImageProcessBottomSide_AreaDiff(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_AreaDiff> Start : nWidth = %d, nHeight = %d, nLineLevel = %d, bSimMode = %d, nArrCnt = %d"), nWidth, nHeight, nLineLevel, bSimMode, nArrCnt);

	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

	if (nLineLevel < 0)
	{
		AprData.SaveDebugLog(_T("<<ImageProcessBottomSide_AreaDiff>>에러 - !!! Tab Level 이상 !!!"));

		return -1;
	}

	CRect rcAll;
	rcAll.left = nLineLevel - pRecipeInfo->nFoilOutInspWidth[CAM_POS_BOTTOM] * 2;
	rcAll.right = nLineLevel + pRecipeInfo->nFoilExpInspWidth[CAM_POS_BOTTOM];
	rcAll.top = 0;
	rcAll.bottom = nHeight;

	if (pRecipeInfo->bEnableVGroove == TRUE)
	{
		rcAll.right += pRecipeInfo->TabCond.nNegVGrooveHeight;
	}


	if (rcAll.left < 0)
	{
		rcAll.left = 0;
	}
	if (rcAll.right >= nWidth)
	{
		rcAll.right = (nWidth - 1);
	}

	int nSizeAll = nWidth * nHeight;
	BYTE* pMeanPtr = new BYTE[nSizeAll + 1];
	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);

	CImageProcess::VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nEdgeWidth = 10;

	// 평균화 처리 
	int nMeanSize = MEAN_SIZE_HOR;

	BYTE* pStdPtr = new BYTE[nHeight * nWidth + 1];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth + 1];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);

	BYTE* pDiffPtr = new BYTE[nSizeAll + 1];
	memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);

	BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_BOTTOM];
	CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, pRecipeInfo->bDarkEmpMode);


	BYTE* pEdgePtr = pMeanPtr;


	if (pRecipeInfo->bEnableVGroove == FALSE)
	{
		int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
		CImageProcess::EdgeDetectImageToBoth_RndInfo_Threshold(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, CImageProcess::en_BottomSide, nLineLevel, CImageProcess::en_FindLeft);
	}
	else
	{
		int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
		int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM]; // 22.05.30 Ahn Add
		CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, nThresMax, CImageProcess::en_BottomSide, nLineLevel, CImageProcess::en_FindLeft);
	}


	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft);



	CRegionInfo roiFoilExp;
	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nWidth * nHeight + 1];
	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
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


	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiDross.GetFifoPtr());

	// Overflow 는 4번 Bit살려서 보냄
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, AprData.m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
	CImageProcess::LoopLabeling(&roiDross, nWidth, nHeight, &vecBlockDross, CImageProcess::en_Dross_Bit, AprData.m_pRecipeInfo->nFoilExpOutMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);


	pTabRsltInfo->RemoveAll();

	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++)
	{
		if (nMode == TYPE_FOILEXP_OUT)
		{
			pVecBlockPtr = &vecBlockDross;
		}
		else
		{
			pVecBlockPtr = &vecBlockFoilExp;
		}
		int nDefSize = (int)pVecBlockPtr->size();

		for (int i = 0; i < nDefSize; i++)
		{
			pData = &(*pVecBlockPtr)[i];

			if (pData->bDeleteFlag == TRUE)
				continue;

//			if (AprData.m_System.m_nMachineMode == CATHODE_MODE)
//			{ 
				CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
//			}
		}
	}


	if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_BOTTOM] == TRUE)
	{
		CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_BOTTOM);
		CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_BOTTOM);
	}


	CImageProcess::_VEC_BLOCK vecBlockMerge;
	vecBlockMerge.clear();

	CImageProcess::MergeAndLink_BlockInfo(&vecBlockMerge, vecBlockDross, vecBlockFoilExp, pRecipeInfo, CAM_POS_BOTTOM);
	CImageProcess::SortingBlockInfo(&vecBlockMerge);

	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockMerge, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);


	if (pStdPtr != nullptr) {
		delete[] pStdPtr;
		pStdPtr = NULL;
	}
	
	if (pProcPtr != nullptr) {
		delete[] pProcPtr;
		pProcPtr = NULL;
	}

	if (pThresPtr != nullptr) {
		delete[]pThresPtr;
		pThresPtr = NULL;
	}
	if (pMeanPtr != nullptr) {
		delete[]pMeanPtr;
		pMeanPtr = NULL;
	}
	if( pDiffPtr != nullptr ){
		delete[] pDiffPtr;		
		pDiffPtr = NULL;
	}

	return nRet;

}

// Head부 검사 처리 함수.
// 22.02.08 Ahn Add Start
int CImageProcess::ImageProcessTopSide_Negative(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, int nTabLeft, int nTabRight, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_Negative> Start : nWidth = %d, nHeight = %d, nLineLevel = %d, nTabLeft = %d, nTabRight = %d, bSimMode = %d, nArrCnt = %d"), nWidth, nHeight, nLineLevel, nTabLeft, nTabRight, bSimMode, nArrCnt);

	ASSERT(pImgPtr);
	ASSERT(pRecipeInfo);
	ASSERT(pTabRsltInfo);

	int nOffset = 50;
	int nLeftOffset;
	int nTabRoundOffsetR;

	nLeftOffset = pRecipeInfo->TabCond.nNegVGrooveHeight + pRecipeInfo->nFoilExpInspWidth[CAM_POS_TOP];
	nTabRoundOffsetR = pRecipeInfo->TabCond.nNegCoatHeight;

	CRect rcAll;
	CRect rcLeft;
	CRect rcRight;
	rcAll.left = rcRight.left = rcLeft.left = nLineLevel - nLeftOffset;
	rcAll.right = nLineLevel + (int)(pRecipeInfo->TabCond.nRadiusW);
	
	int nOutRange = pRecipeInfo->TabCond.nNegCoatHeight;
	rcRight.right = rcLeft.right = nLineLevel + nOutRange;

	if ((rcAll.right > nWidth) || (rcAll.left < 0)) {
		return -1;
	}

	rcAll.top = rcLeft.top = 0;
	rcAll.bottom = rcRight.bottom = nHeight;
	rcLeft.bottom = nTabLeft;
	rcRight.top = nTabRight;

	CRect rcLeftRnd;
	CRect rcRightRnd;
	rcLeftRnd = rcLeft;
	rcRightRnd = rcRight;
	rcLeftRnd.right = rcRightRnd.right = nLineLevel + nTabRoundOffsetR;
	rcLeftRnd.top = rcLeft.bottom - pRecipeInfo->TabCond.nRadiusH;
	rcRightRnd.bottom = rcRightRnd.top + pRecipeInfo->TabCond.nRadiusH;

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
	BYTE* pMeanPtr = new BYTE[nSizeAll + 1];
	memset(pMeanPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);

	int nMeanSize = MEAN_SIZE_HOR;
	int nHeadBndry = nLineLevel + nTabRoundOffsetR;
	int nInspRangeRight = nLineLevel + pRecipeInfo->nFoilOutInspWidth[CAM_POS_TOP];
	//	rcRightRnd.right = rcLeftRnd.right = rcAll.right = nInspRangeRight; // 삭제

	int nMaskRight = nHeadBndry - 30;

	if (nMaskRight >= nWidth)
	{
		nMaskRight = nWidth - 1;
	}

	rcRightRnd.right = rcLeftRnd.right = rcAll.right = nMaskRight; // 추가 및 변경


	BYTE* pRsltPtr = new BYTE[nSizeAll + 1];
	memset(pRsltPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);



	BYTE* pStdPtr = new BYTE[nHeight * nWidth + 1];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth + 1];
	memset(pStdPtr, 0x00f, sizeof(BYTE) * nWidth * nHeight + 1);
	memset(pProcPtr, 0x0f, sizeof(BYTE) * nWidth * nHeight + 1);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcLeft, 15, 15, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcRight, 15, 15, CImageProcess::en_FillBottom);

	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcLeft, 3, 3, CImageProcess::en_FillTop);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcRight, 3, 3, CImageProcess::en_FillBottom);

	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcLeft);
	CImageProcess::GetMinImage(pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcRight);

	// Mean처리한 Image로 Tab의 Center 에서 Tab의 높이를 구함.

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_Negative> ImageMean_Part, GetMinImage : Finish"));


	int nRoundOffset = 50;	// 라운드 부분이 Slip 발생으로 늘어질경우를 대비한 Offset값
	int nEdgeCompWidth = 10;	//	Edge 검출 Parameter 상하 비교 폭

	// Tab 왼쪽(위) 부분의 Edge 검출
	CRect rcTemp;
	rcTemp = rcLeft;
	rcTemp.bottom = rcLeftRnd.top;
	rcTemp.right = rcLeftRnd.right;
	int nCropWidth = rcLeftRnd.Width();
	int nCropHeight = rcLeftRnd.Height();
	int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_TOP];
	int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP]; // 22.05.30 Ahn Add

	int nLocalRet = 0;

	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr;

	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff)
	{
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr/*NULL*/, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nEdgeCompWidth, 2, DIR_VER);
	}
	else
	{
		nLocalRet = CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, pRsltPtr/*NULL*/, &vecLeftRndInfo, nWidth, nHeight, rcTemp, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindLeft);
	}

	if (nLocalRet < 0)
	{
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<ImageProcessTopSide_Negative>>에러 - Edge found faile left round, ret = %d"), nLocalRet);

		// 22.04.18 Ahn Add Start
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[]pnResultArr;
		// 22.04.18 Ahn Add End
		return -1;
	}


	// Round는 가로 세로 2번의 Edge 검출이 필요함.
	rcLeftRnd.right = nMaskRight;
	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, pRsltPtr/*NULL*/, nWidth, nHeight, rcLeftRnd, nThresBnd, nMaskRight, &vecLeftRndInfo, CImageProcess::en_FindLeft, CImageProcess::en_FindFromLeft);
	CImageProcess::SmoothVecRoundData(&vecLeftRndInfo, CImageProcess::en_FindLeft);

	int nLeftSize = (int)vecLeftRndInfo.size();
	if (nLeftSize > 0)
	{
		rcLeft.top = vecLeftRndInfo[0].y;
		rcLeft.bottom = vecLeftRndInfo[nLeftSize - 1].y;
	}
	else
	{
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[] pnResultArr;
		return -2;

	}


	// Tab 오른쪽(아래) 부분의 Edge 검출
	nCropWidth = rcRightRnd.Width();
	nCropHeight = rcRightRnd.Height();
	rcRightRnd.right = nMaskRight;	// Round는 가로 세로 2번의 Edge 검출이 필요함.

	CImageProcess::GetBoundaryRoundInfo_ByBriDist(pProcPtr, pRsltPtr/*NULL*/, nWidth, nHeight, rcRightRnd, nThresBnd, nMaskRight, &vecRightRndInfo, CImageProcess::en_FindRight, CImageProcess::en_FindFromLeft);

	rcTemp = rcRight;
	rcTemp.top = rcRightRnd.bottom;
	rcTemp.right = rcRightRnd.right;

	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP] == CImageProcess::en_Edge_MaxDiff)
	{
		nLocalRet = CImageProcess::EdgeDetectImageToBoth_RndInfo(pEdgePtr, pRsltPtr/*NULL*/, &vecRightRndInfo, nWidth, nHeight, rcTemp, nEdgeCompWidth, 2, DIR_VER);
	}
	else
	{
		nLocalRet = CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, pRsltPtr/*NULL*/, &vecRightRndInfo, nWidth, nHeight, rcTemp, nThresBnd, nThresMax, CImageProcess::en_TopSide, nLineLevel, CImageProcess::en_FindRight);
	}

	if (nLocalRet < 0)
	{		
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
	if (nRightSize > 0)
	{
		rcRight.top = vecRightRndInfo[0].y;
		rcRight.bottom = vecRightRndInfo[nRightSize - 1].y;
	}
	else
	{
		delete[] pStdPtr;
		delete[] pProcPtr;
		delete[]pnResultArr;
		return -3;
	}

	delete[]pnResultArr;

	BYTE* pThresPtr = new BYTE[nSizeAll + 1];
	BYTE* pDiffPtr;

	CRegionInfo roiInfo;
	CString strThres;

	memset(pThresPtr, 0x00, sizeof(BYTE)* nWidth* nHeight + 1);
	roiInfo.SetRoiPtr(pThresPtr);
	roiInfo.SetProcBit(CImageProcess::en_FoilExp_Bit);

	rcLeft.right = nHeadBndry;
	rcRight.right = nHeadBndry;

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

	pDiffPtr = new BYTE[nSizeAll + 1];
	memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);

	BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_TOP];
	CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcLeftDiff, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);
	CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcRightDiff, pRecipeInfo->dMagnification[CAM_POS_TOP], btThreshold, pRecipeInfo->bDarkEmpMode);

	roiInfo.SetRect(rcLeft);
	CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecLeftRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);
	roiInfo.SetRect(rcRight);
	CImageProcess::Threshold_RoundMask_Negative(pDiffPtr, &roiInfo, &vecRightRndInfo, nWidth, nHeight, nStartX, nEndX, nFoilExpThLower, nMaskRight, FALSE);

	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	vecBlockFoilExp.clear();

	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiInfo.GetFifoPtr());
	CImageProcess::LoopLabeling(&roiInfo, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, pRecipeInfo->nFoilExpInMinSize[CAM_POS_TOP], AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);


	pTabRsltInfo->RemoveAll();

	//// 라운드 결함인 경우 판정용 사이즈 계산.
	CImageProcess::_VEC_BLOCK* pVecBlockPtr;
	CBlockData* pData;
	//for (int nMode = 0; nMode < MAX_INSP_TYPE; nMode++) 
	{
		pVecBlockPtr = &vecBlockFoilExp ;

		int nDefSize = (int)pVecBlockPtr->size();
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_Negative> nDefSize = %d"), nDefSize);

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
				//CImageProcess::CheckDistanceToEdgeLine(pData, &vecLeftRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);
				//if (rcTemp.IntersectRect(rcLeftRnd, rcDef) == TRUE) 

				CImageProcess::CalcSizeToEdgeLine(pData, &vecLeftRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);

				if (rcLeftRnd.PtInRect(cpLT) == TRUE)
				{
					CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindLeft);
				}
				else
				{
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
			else if (rcRight.PtInRect(cpLB) == TRUE) {
				//CImageProcess::CheckDistanceToEdgeLine(pData, &vecRightRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);

				CImageProcess::CalcSizeToEdgeLine(pData, &vecRightRndInfo, AprData.m_System.m_dResolX[CAM_POS_TOP], CAM_POS_TOP);

				if (rcLeftRnd.PtInRect(cpLB) == TRUE)
				{
					CImageProcess::CalcDefectMaxSize_Round(pThresPtr, nWidth, nHeight, pData->nType, pData, rcDef, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY, CImageProcess::en_FindRight);
				}
				else
				{
					CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_TOP], AprData.m_System.m_dResolY);
				}
			}
		}
	}

	if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_TOP] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_TOP);
		//	CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_TOP);	// 
	//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_Negative> BlockLink : Finish"));

	}


	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_TOP, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);


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
	delete[]pRsltPtr;
	pRsltPtr = NULL;


//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessTopSide_Negative> End"));

	return 0;
}


int CImageProcess::ImageProcessBottomSide_Negative(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nLineLevel, CTabRsltInfo* pTabRsltInfo, BOOL bSimMode, BYTE** pImgPtrArr, int nArrCnt)
{
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> Start : nWidth = %d, nHeight = %d, nLineLevel = %d, bSimMode = %d, nArrCnt = %d"), nWidth, nHeight, nLineLevel, bSimMode, nArrCnt);

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
	BYTE* pTempPtr = new BYTE[nSizeAll + 1];
	memset(pTempPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);

	CImageProcess::VEC_ROUND_INFO vecAllRndInfo;
	vecAllRndInfo.clear();
	int nEdgeWidth = 10;

	// 평균화 처리 
	BYTE* pMeanPtr = pTempPtr;
	int nMeanSize = MEAN_SIZE_HOR;

	// 22.09.15 Ahn Modify Start
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	// 22.09.15 Ahn Modify End
		MeanImageDirection(pImgPtr, pMeanPtr, nWidth, nHeight, rcAll, nMeanSize, DIR_VER);
	}

	BYTE* pStdPtr = new BYTE[nHeight * nWidth + 1];
	BYTE* pProcPtr = new BYTE[nHeight * nWidth + 1];
	memset(pStdPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
	memset(pProcPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);

	CImageProcess::ImageMean_Part(pImgPtr, pStdPtr, nWidth, nHeight, rcAll, 15, 15, CImageProcess::en_FillAll);
	CImageProcess::ImageMean_Part(pImgPtr, pProcPtr, nWidth, nHeight, rcAll, 3, 3, CImageProcess::en_FillAll);

	// 22.05.30 Ahn Add Start
	CImageProcess::GetMinImage( pMeanPtr, pStdPtr, pProcPtr, nWidth, nHeight, rcAll);
	// 22.05.30 Ahn Add End

	BYTE* pDiffPtr;
	//if (pRecipeInfo->bDisableProcDiff == FALSE) 
	{
		pDiffPtr = new BYTE[nSizeAll + 1];
		memset(pDiffPtr, 0x00, sizeof(BYTE) * nSizeAll + 1);
		BYTE btThreshold = (BYTE)pRecipeInfo->nFoilExpThresOrigin[CAM_POS_BOTTOM];
		CImageProcess::DiffProcImage(pProcPtr, pStdPtr, pDiffPtr, nWidth, nHeight, rcAll, pRecipeInfo->dMagnification[CAM_POS_BOTTOM], btThreshold, pRecipeInfo->bDarkEmpMode);
	}
	//else {
	//	pDiffPtr = pImgPtr;
	//}

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> ImageMean_Part, GetMinImage, DiffProcImage : Finish") );

	// 22.05.30 Ahn Add Start
	BYTE* pEdgePtr;
	pEdgePtr = pMeanPtr;
	// 22.05.30 Ahn Add End

	if (pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM] == CImageProcess::en_Edge_MaxDiff) {
		EdgeDetectImageToBoth_RndInfo(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nEdgeWidth, 2, DIR_VER);
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> EdgeDetectImageToBoth_RndInfo : Finish") );

	}
	else {
		int nThresBnd = pRecipeInfo->TabCond.nRollBrightHigh[CAM_POS_BOTTOM];
		int nThresMax = pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM]; // 22.05.30 Ahn Add

		CImageProcess::EdgeDetectByRndInfo_Negative(pEdgePtr, NULL, &vecAllRndInfo, nWidth, nHeight, rcAll, nThresBnd, nThresMax, CImageProcess::en_BottomSide, nLineLevel,  CImageProcess::en_FindLeft);
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> EdgeDetectByRndInfo_Negative : Finish") );
	}

	CImageProcess::SmoothVecRoundData(&vecAllRndInfo, CImageProcess::en_FindLeft);
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> SmoothVecRoundData : Finish") );


	CRegionInfo roiFoilExp;
//	CRegionInfo roiDross;
	CString strThres;

	BYTE* pThresPtr = new BYTE[nWidth * nHeight + 1];
	memset(pThresPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
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
//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> Threshold_RoundMask_Negative : Finish") );

	}

	CImageProcess::_VEC_BLOCK vecBlockFoilExp;
	CImageProcess::_VEC_BLOCK vecBlockDross;
	vecBlockFoilExp.clear();
	vecBlockDross.clear();

	CImageProcess::GetOrgImageBright(pImgPtr, nWidth, nHeight, roiFoilExp.GetFifoPtr());
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> GetOrgImageBright : Finish") );

	// Overflow 는 4번 Bit살려서 보냄
	CImageProcess::LoopLabeling(&roiFoilExp, nWidth, nHeight, &vecBlockFoilExp, CImageProcess::en_FoilExp_Bit, AprData.m_pRecipeInfo->nFoilExpInMinSize[CAM_POS_BOTTOM], AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);


	pTabRsltInfo->RemoveAll();


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

//		AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> nDefSize = %d"), nDefSize );


		for (int i = 0; i < nDefSize; i++)
		{
			pData = &(*pVecBlockPtr)[i];

			//CImageProcess::CheckDistanceToEdgeLine(pData, &vecAllRndInfo, pRecipeInfo->dIgnoreDistance, AprData.m_System.m_dResolX[CAM_POS_BOTTOM]);

			if (pData->bDeleteFlag == TRUE) continue;
			//if (AprData.m_System.m_nMachineMode == ANODE_MODE) 
			{
				CImageProcess::CalcSizeToEdgeLine(pData, &vecAllRndInfo, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], CAM_POS_BOTTOM);
				CImageProcess::CalcDefectMaxSize(pThresPtr, nWidth, nHeight, pData->nType, pData, AprData.m_System.m_dResolX[CAM_POS_BOTTOM], AprData.m_System.m_dResolY);
			}
		}
	}


	// 22.07.22 Ahn Delete Start
	if (AprData.m_pRecipeInfo->bEnableDefectLink[CAM_POS_BOTTOM] == TRUE) {
		CImageProcess::BlockLink(&vecBlockFoilExp, pRecipeInfo, TYPE_FOILEXP, CAM_POS_BOTTOM);
	//	CImageProcess::BlockLink(&vecBlockDross, pRecipeInfo, TYPE_FOILEXP_OUT, CAM_POS_BOTTOM);
	}
	// 22.07.22 Ahn Delete End


	int nFrameStartPos = (pTabRsltInfo->nFrameCount * AprData.m_System.m_nCamViewHeight) + pTabRsltInfo->nTabStartPosInFrame;
	CImageProcess::AddDefectInfoByBlockInfo(&vecBlockFoilExp, pRecipeInfo, pTabRsltInfo, CAM_POS_BOTTOM, MAX_SAVE_DEFECT_COUNT, nFrameStartPos, AprData.m_System.m_dResolY);
//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> AddDefectInfoByBlockInfo :Finish") );


	//if (bSimMode == TRUE) {
	//	BYTE* pBndPtr = NULL;
	//	pBndPtr = new BYTE[nWidth * nHeight + 1];
	//	memset(pBndPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);

	//	DrawPixel_BoundaryLine(pBndPtr, nWidth, nHeight, vecAllRndInfo, 0x80);
	//	CopyMemory(pImgPtrArr[0], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);
	//	CopyMemory(pImgPtrArr[1], pBndPtr, sizeof(BYTE) * nWidth * nHeight);
	//	CopyMemory(pImgPtrArr[2], pDiffPtr, sizeof(BYTE) * nWidth * nHeight);
	//	CopyMemory(pImgPtrArr[3], pThresPtr, sizeof(BYTE) * nWidth * nHeight);
	//	CopyMemory(pImgPtrArr[4], pStdPtr, sizeof(BYTE) * nWidth * nHeight);
	//	CopyMemory(pImgPtrArr[5], pProcPtr, sizeof(BYTE) * nWidth * nHeight);
	//	//	memcpy(pImgPtrArr[7], pMeanPtr, sizeof(BYTE) * nWidth * nHeight);

	//	delete[] pBndPtr;
	//	pBndPtr = NULL;
	//}

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

//	AprData.SaveDebugLog_Format(_T("<CImageProcess> <ImageProcessBottomSide_Negative> End : nRet = %d"), nRet);

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
int CImageProcess::GetBoundaryOfElectorde(const BYTE* pImgPtr, int nWidth, int nHeight, CRecipeInfo* pRecipeInfo, int nFindDir)
{
	CRect rect;
	//프로젝션 할 버퍼 크기
	int nPrjWidth = 2000;
	//실제 얻은 이미지 크기 nWidth 에서 프로젝션할 nPrjWidth 차가 이미지 왼쪽 시작점
	rect.left = (nWidth - nPrjWidth );
	//실제 이미지의 오른쪽 크기 픽셀 점
	rect.right = nWidth;
	//실제 이미지 Hight 시작점
	rect.top = 0;
	//실제 얻은 이미지 크기에서 1 제거? 좌표값
	rect.bottom = nHeight - 1;
	//샘플링할 높이 픽셀 크기 : 실제 이미지 높이/100 
	int nSamplingSize = nHeight / 100;
	//프로젝션 해서 얻어 올 버퍼 
	//프로젝션 할 실제 넓이에서 2000을 뺀 위치에서 부터 실제 넗이 갯수 까지
	//X축 Width의 휘도 평균 값을 가져오기 위한 버퍼
	int* pnPrj = new int[nPrjWidth];

	//DarkRoll  사용여부 체크
	//레시피 정보에 Tab Condition Roll Bright Mode Top
	BOOL bUseDarkRoll = (pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) ? FALSE : TRUE;

	//프로젝션 이미지 데이터 생성 샘플링 값 높이/100
	//프로젝션 샘플링을 실제 이미지에서 pnPrj 로 가져 온다
	//넗이는 2000을 뺀 시작점에서 넓이 끝까지
	CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);

	//프로젝션 데이터로 바운드리 크기를 찾는다.
	//pnPrj : 휘도 샘플링 데이터
	//nPrjWidth : 휘도 샘플링 Width 값
	//nCeramicBrightLow : 코딩부 세라믹 최저값
	//nFindDir : 찾는 방향 (코딩부 세라믹 최저값을 이용하여 ~~ 찾음) : 왼쪽에서 오른쪽 찾음
	// return : nBndElectrode -> 휘도 샘플링 버퍼의 롤과 레시피 , 찾는 방향에 따른 휘도 위치 값을 받는다 : 레시피 기준 또는 샘플링의 데이터의 값 비교 로 얻음
	int nBndElectrode = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_TOP], nFindDir, bUseDarkRoll ) ;

	//찾는 위치가 Left
	//찾는 방향이 왼쪽에서 오른쪽
	//left 일 경우 실제 이미지의 잘라 버린 데이터 위치를 추가 해야 실제 얻은 미미지에서 의 위치가 된다.
	if (nFindDir == en_FindFromLeft) {
		nBndElectrode += (nWidth - nPrjWidth);
	}
	//샘플링 프로젝션 버퍼 메모리 해제
	delete[] pnPrj;

	return nBndElectrode;

}

int CImageProcess::GetBoundaryOfElectordeBottom(BYTE* pImgPtr, int nWidth, int nHeight, int *pnLevel, CRecipeInfo* pRecipeInfo)
{
	ASSERT(pRecipeInfo);
	ASSERT(pnLevel);

/*
	CRect rect;
	int nPrjWidth = 2000;
	rect.left = 0;
	rect.right = nPrjWidth ;
	rect.top = 0;
	rect.bottom = nHeight - 1;
	int nSamplingSize = nHeight / 100;
	int* pnPrj = new int[nPrjWidth];

	CImageProcess::GetProjection(pImgPtr, pnPrj, nWidth, nHeight, rect, DIR_VER, nSamplingSize, FALSE);
	
	BOOL bUseDarkRoll = (pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_BOTTOM] == 1) ? FALSE : TRUE;

	int nBndElectrode = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM], CImageProcess::en_FindFromRight, bUseDarkRoll);
	*pnLevel = CImageProcess::FindBoundary_FromPrjData(pnPrj, nPrjWidth, pRecipeInfo->TabCond.nCeramicBrightLow[CAM_POS_BOTTOM], CImageProcess::en_FindFromLeft, bUseDarkRoll);

	delete[] pnPrj;
*/

	int nLevel = 0;
	FindTabLevel(pImgPtr, nWidth, nHeight, &nLevel, pRecipeInfo->TabCond, pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
	int nBndElectrode = nLevel;
	*pnLevel = nLevel;

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
int CImageProcess::Threshold_ByEdgeLine(const BYTE* pImgPtr, CRegionInfo* pRoiInfo, VEC_ROUND_INFO* vecLine, int nWidth, int nHeight, int nInspWidth, int nMinThres, int nMaxThres, int nMaskOffset, int nMode, BOOL bClearRslt, BOOL bModeSide )
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
			pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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

			pLinePtr = (BYTE*)pImgPtr + (nWidth * y);
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
double CImageProcess::GetIqSharpnessValue(const BYTE* pOrgImg, int nWidth, int nHeight, CPoint cpStartPoint)
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
		BYTE* pLinePtr = (BYTE *)pOrgImg + (nWidth * y) ;
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
int	CImageProcess::GetBrightAverage(const BYTE* pOrgImg, int nWidth, int nHeight, CPoint cpStartPoint)
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
		BYTE* pLinePtr = (BYTE *)pOrgImg + (nWidth * y);
		for (int x = nStartX; x < nEndX; x++) {
			nSum += (int)*(pLinePtr + x) ;
			nCount++;
		}
	}
	if (nCount != 0)
	{
		nRet = nSum / nCount;
	}

	return nRet;
}
// 23.02.16 Ahn Add End 


BOOL CImageProcess::FindPetFilm(const BYTE* pOrgImg, int nImageWidth, int nImageHeight, CRecipeInfo& RecipeInfo, VEC_PET_INFO* vstPetInfo, int nCamPos)
{
	//PET 찾지 않는다면
	if (RecipeInfo.bDisablePET == TRUE)
	{
		return FALSE;
	}

	DWORD dwStart = GetTickCount();

	vstPetInfo->clear();
	BOOL bRet = FALSE;


	int InspCntX = RecipeInfo.nPetMatrixX[nCamPos]; // 3
	int InspCntY = RecipeInfo.nPetMatrixY[nCamPos]; // 3
	int nRangeX = 100;
	int nRangeY = 100;
	int nSideMargin = 200;
	int nBrightTh = RecipeInfo.nPetThreshold[nCamPos]; // 200;
	int nCheckCnt_Setting = RecipeInfo.nPetCheckCnt[nCamPos]; // 3;
	int nCheckCnt_Now = 0;

	int nHalfWidth = nImageWidth / 2;
	int nDivWidth = 0;
	int nDivHeight = 0;

	if (InspCntX > 1)
	{
		nDivWidth = (nHalfWidth - (nSideMargin * 2)) / (InspCntX - 1);
	}
	if (InspCntY > 1)
	{
		nDivHeight = (nImageHeight - (nSideMargin * 2)) / (InspCntY - 1);
	}


	int nStartX = 0;
	int nStartY = 0;

	int nBright = 0;
	CRect rcArea;

	for (int x = 0; x < InspCntX; x++)
	{
		for( int y = 0; y < InspCntY; y++)
		{
			if (nCamPos == CAM_POS_TOP)
			{
				nStartX = (nSideMargin / 2) + (nDivWidth * x);
			}
			else
			{
				nStartX = nHalfWidth + (nSideMargin / 2) + (nDivWidth * x);
			}
			
			nStartY = (nSideMargin/2) + (nDivHeight * y);

			if (nStartX < 0) { nStartX = 0; }
			if (nStartY < 0) { nStartY = 0; }

			int nEndX = nStartX + nRangeX;
			int nEndY = nStartY + nRangeY;
			if (nEndX < 0) { nEndX = 0; }
			if (nEndX >= nImageWidth) { nEndX = nImageWidth - 1; }
			if (nEndY < 0) { nEndY = 0; }
			if (nEndY >= nImageHeight) { nEndY = nImageHeight - 1; }

			int nSum = 0;
			int nCount = 0;
			for (int y = nStartY; y < nEndY; y++) {
				BYTE* pLinePtr = (BYTE *)pOrgImg + (nImageWidth * y);
				for (int x = nStartX; x < nEndX; x++) {
					nSum += (int)*(pLinePtr + x);
					nCount++;
				}
			}
			if (nCount != 0)
			{
				nBright = nSum / nCount;

				rcArea.left = nStartX;
				rcArea.top = nStartY;
				rcArea.right = nEndX;
				rcArea.bottom = nEndY;

				_PET_INFO stPet;
				stPet.rcArea = rcArea;
				stPet.nBright = nBright;
				stPet.bFind = (stPet.nBright >= nBrightTh) ? TRUE : FALSE;

				vstPetInfo->push_back(stPet);
			}
		}
	}

	int nPetCnt = 0;
	int nSize = (int)vstPetInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		BOOL nFind = vstPetInfo->at(i).bFind;
		if ( nFind == TRUE )
		{
			nCheckCnt_Now++;
			if (nCheckCnt_Now >= nCheckCnt_Setting)
			{
				bRet = TRUE;
				nCheckCnt_Now = 0;
				break;
			}
		}
	}


	DWORD dwTact = GetTickCount() - dwStart;
	CString strTict;
	strTict.Format(_T("%d"), dwTact);
//	AfxMessageBox(strTict);


	return bRet;
}



BOOL CImageProcess::CheckBright(const BYTE* pOrgImg, int nImageWidth, int nImageHeight, CRecipeInfo& RecipeInfo, _BRIGHT_INFO* stBrightInfo, int nCamPos)
{
	if (RecipeInfo.bDisableBrightCheck == TRUE)
	{
		return FALSE;
	}

	DWORD dwStart = GetTickCount();

	BOOL bRet = FALSE;

	int nStartX = RecipeInfo.nCheckBrightL[nCamPos];
	int nStartY = RecipeInfo.nCheckBrightT[nCamPos];
	int nEndX = RecipeInfo.nCheckBrightR[nCamPos];
	int nEndY = RecipeInfo.nCheckBrightB[nCamPos];
	int nRangeMin = RecipeInfo.nCheckBrightRangeMin[nCamPos];
	int nRangeMax = RecipeInfo.nCheckBrightRangeMax[nCamPos];
	
	if (nStartX < 0) { nStartX = 0; }
	if (nStartY < 0) { nStartY = 0; }
	if (nEndX < 0) { nEndX = 0; }
	if (nEndX >= nImageWidth) { nEndX = nImageWidth - 1; }
	if (nEndY < 0) { nEndY = 0; }
	if (nEndY >= nImageHeight) { nEndY = nImageHeight - 1; }


	int nSum = 0;
	int nCount = 0;
	for (int y = nStartY; y < nEndY; y++)
	{
		BYTE* pLinePtr = (BYTE*)pOrgImg + (nImageWidth * y);
		for (int x = nStartX; x < nEndX; x++)
		{
			nSum += (int)*(pLinePtr + x);
			nCount++;
		}
	}
	if (nCount != 0)
	{
		int nBrightNow = nSum / nCount;

		CRect rcArea;
		rcArea.left = nStartX;
		rcArea.top = nStartY;
		rcArea.right = nEndX;
		rcArea.bottom = nEndY;

		stBrightInfo->rcArea = rcArea;
		stBrightInfo->nBright = nBrightNow;

		if (nBrightNow >= nRangeMin && nBrightNow <= nRangeMax)
		{
			stBrightInfo->bError = FALSE; // OK
		}
		else
		{
			stBrightInfo->bError = TRUE; // NG
		}

	}

	bRet = stBrightInfo->bError;


	DWORD dwTact = GetTickCount() - dwStart;
	CString strTict;
	strTict.Format(_T("%d"), dwTact);
//	AfxMessageBox(strTict);


	return bRet;
}



BOOL CImageProcess::SaveOriginImage(const BYTE* pOrgImg, int nImageWidth, int nImageHeight, CString strComment)
{ //  원본 이미지 저장
	CBitmapStd bmp(nImageWidth, nImageHeight);
	bmp.SetImage(nImageWidth, nImageHeight, pOrgImg);

	CTime time = CTime::GetCurrentTime();

	CString strPath = AprData.m_strImagePath + _T("\\ORIGIN");
	CWin32File::CreateDirectory(strPath);

	CString str;
	str.Format(_T("%s\\%04d%02d%02d_%02d%02d%02d(%03d)_%s.bmp"),
		strPath,
		time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), GetTickCount(),
		strComment );

	bmp.SaveBitmap(str);

	return TRUE;
}
