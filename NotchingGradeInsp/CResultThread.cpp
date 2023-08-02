#include "pch.h"
#include "CResultThread.h"
#include "QueueCtrl.h"
#include "ImageProcessCtrl.h"
#include "GlobalData.h"
#include "GlobalDef.h"
#include "FrameRsltInfo.h"
#include "BitmapStd.h"
#include "CImageProcess.h"
#include "CCropImgQueueCtrl.h"
//#include "CDefectInfo.h" // 22.06.23 Ahn Add Start

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
//SPC+ 사용을 위한 해더파일
#include "SpcInspManager.h"
#include "SpcCreateJSONFileThread.h"
#include "SpcInspInData.h"
#include "SpcInDataDefectInfo.h"
#endif //SPCPLUS_CREATE

CResultThread::CResultThread(CImageProcessCtrl* pParent, int nHeadNo)
{
	m_pParent = pParent;
	m_pThread = NULL;

#if defined( IMAGE_DRAW_DIRECT_VERSION )
	for (int i = 0; i < GRABBER_COUNT; i++) {
		m_DisphWnd = NULL;
	}
#endif
	m_nHeadNo = nHeadNo;

}
CResultThread::~CResultThread()
{
}

void CResultThread::Begin()
{
	m_bKill = FALSE;

	//	m_DisphWnd = NULL;
	if (m_pThread == NULL) {

		//이벤트 객체 생성
		pEvent_ResultThread = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadResultProc,
			(LPVOID)this,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);

		if (m_pThread != NULL) {
			m_pThread->m_bAutoDelete = FALSE;
			m_pThread->ResumeThread();
		}
	}
}
void CResultThread::Kill(void)
{
	// source file
	if (m_pThread)
	{
		setEvent_ResultThread();
		CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
		m_pThread->m_hThread = NULL;
	}

}

void CResultThread::SetDefectDataCtrl(CDefectDataCtrl *pDefDataCtrl)
{
	m_pDefDataCtrl = pDefDataCtrl;
}


#if defined( IMAGE_DRAW_DIRECT_VERSION )
void CResultThread::SetDispWnd(HWND hwnd)
{
	m_DisphWnd = hwnd;
}

void CResultThread::DrawDefectRect(CDC* pDC, CRect rcDef, COLORREF color)
{
	if (pDC == NULL) {
		return;
	}

	CRect	rc;
	CBrush	myBrush, * poldBrush;
	myBrush.CreateSolidBrush(color);
	poldBrush = pDC->SelectObject(&myBrush);

	pDC->FrameRect(rcDef, &myBrush);

	pDC->SelectObject(poldBrush);
	myBrush.DeleteObject();
}

// 22.02.25 Ahn Add Start
void CResultThread::DrawString(CDC* pDC, int x, int y, COLORREF color, CString strLine)
{
	if (pDC == NULL) {
		return;
	}

//	CRect	rc;
//	CBrush	myBrush, * poldBrush;
//	myBrush.CreateSolidBrush(color);
//	poldBrush = pDC->SelectObject(&myBrush);
	//CPen pen, * pOldPen;
	//pen.CreatePen(PS_SOLID, 2, color);
	
	//pOldPen = pDC->SelectObject(&pen);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(color);
	pDC->TextOut(x, y, strLine);

	//pDC->SelectObject(pOldPen);
	//pen.DeleteObject();

//	pDC->SelectObject(poldBrush);
//	myBrush.DeleteObject();
}
// 22.02.25 Ahn Add End


void CResultThread::DrawImage(HWND HWnd, CFrameRsltInfo* pRsltInfo, BYTE* pImgPtr, int nWidth, int nHeight, int nMagnif)
{
	ASSERT(HWnd);
	ASSERT(pImgPtr);
	ASSERT(pRsltInfo);
	CTabRsltInfo* pTabRsltInfo = pRsltInfo->m_pTabRsltInfo;;
	ASSERT(pTabRsltInfo);

	HDC hdc = ::GetDC(HWnd);

	if (hdc == nullptr) {
		return;
	}
	CDC* pDC = CDC::FromHandle(hdc);
	CWnd* pWnd = CWnd::FromHandle(HWnd);
	CRect rect;
	CRect rcImg;
	CRect rcClient;
	if (pWnd == NULL) {
		::ReleaseDC(HWnd, hdc);
		return;

	}
	if (pDC == NULL) {
		::ReleaseDC(HWnd, hdc);
		return;
	}
	pWnd->GetWindowRect(&rect);
	rcClient.left = 0;
	rcClient.right = rect.Width();
	rcClient.top = 0;
	rcClient.bottom = rect.Height();

	CBitmapStd *pbmpstd;
	rcImg.left = 0 ;
	rcImg.right = nWidth ;
	rcImg.top = 0 ;
	rcImg.bottom = nHeight ;
	pbmpstd = new CBitmapStd(nWidth, nHeight, 8);
	pbmpstd->SetImage(nWidth, nHeight, pImgPtr);
	pbmpstd->DrawImageFunction(pDC, &rcClient, &rcImg, SRCCOPY, FALSE);

	double dRateX = (double)rect.Width() / (double)(nWidth * nMagnif);
	double dRateY = (double)rect.Height() / (double)(nHeight * nMagnif);

	//pbmpstd->drawText()
	int x, y;
	CString strLine;
	COLORREF color;
	if (pRsltInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) {
		color = RGB(255, 0, 0);
		strLine = _T("JUDGE : NG");
	}else{
		color = RGB(0, 255, 0);
		strLine = _T("JUDGE : OK");
	}

	if (pRsltInfo->m_nHeadNo == CAM_POS_TOP) {
		x = 20;
	}
	else {
		x = 150;
	}
	y = 20 ;
	int yPitch = 30 ;

	DrawString(pDC, x, y, color, strLine);

	int nDefCount = pTabRsltInfo->GetSize();

	// 22.05.11 Ahn Delete Start
	//if (nDefCount > MAX_DISP_DEF_COUNT) nDefCount = MAX_DISP_DEF_COUNT;
	// 22.05.11 Ahn Delete End
	// 22.05.11 Ahn Add Start
	int nDispCnt = 0;
	// 22.05.11 Ahn Add End
	int nLastPosY = y + yPitch ;
	for (int i = 0; i < nDefCount; i++) {
		COLORREF col;

		CDefectInfo* pDefInfo;
		pDefInfo = pTabRsltInfo->m_vecDefInfo[i];
		if ( (pDefInfo == nullptr) || ( nDispCnt >= MAX_DISP_DEF_COUNT)) break;
		if (pDefInfo->nType == TYPE_SURFACE) continue;

		if (pDefInfo->nRank == JUDGE_NG) {
			col = RGB(230, 50, 50);
		} else {
			col = RGB(50, 230, 50);
		}
		strLine.Format(_T("Size_F %d:%.1lf um"), nDispCnt +1, pDefInfo->dJudgeSize);
		DrawString( pDC, x, y + ((nDispCnt +1) * yPitch), col, strLine);

		CRect defRect;
		defRect.top = (int)(pDefInfo->rcPos.top * dRateY);
		defRect.bottom = (int)(pDefInfo->rcPos.bottom * dRateY);
		defRect.left = (int)(pDefInfo->rcPos.left * dRateX);
		defRect.right = (int)(pDefInfo->rcPos.right * dRateX);

		int nInfate = 5;
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		DrawDefectRect(pDC, defRect, col);
		nDispCnt++;

	}

	y = 300;
	// 22.06.24 Ahn Modify Start
	for (int i = 0, nIdx = 0; i < nDefCount; i++) {
	// 22.06.24 Ahn Modify End
		COLORREF col;

		CDefectInfo* pDefInfo;
		pDefInfo = pTabRsltInfo->m_vecDefInfo[i];
		if ((pDefInfo == nullptr) || ((nIdx + nDispCnt) >= 10)) break;
		if (pDefInfo->nType != TYPE_SURFACE) continue;

		if (pDefInfo->nRank == JUDGE_NG) {
			col = RGB(230, 50, 50);
		}
		else {
			col = RGB(50, 230, 50);
		}

		// Spetter 결함 사이즈 표시
		if (nIdx < MAX_DISP_DEF_COUNT) {
			strLine.Format(_T("Size_S %d:%.1lf um"), (nIdx + nDispCnt  ), pDefInfo->dJudgeSize);
			DrawString(pDC, x, nLastPosY + ((nIdx + nDispCnt ) * yPitch), col, strLine);
		}
		nIdx++;

		CRect defRect;
		defRect.top = (int)(pDefInfo->rcPos.top * dRateY);
		defRect.bottom = (int)(pDefInfo->rcPos.bottom * dRateY);
		defRect.left = (int)(pDefInfo->rcPos.left * dRateX);
		defRect.right = (int)(pDefInfo->rcPos.right * dRateX);

		int nInfate = 5;
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		DrawDefectRect(pDC, defRect, col);
	}

	::ReleaseDC(HWnd, hdc);

	delete pbmpstd;
}

// 23.02.03 Ahn Add Start


void CResultThread::DrawImage_Test(CDC* pDC, CFrameRsltInfo* pRsltInfo, int nWidth, int nHeight, int nMagnif)
{
	ASSERT(pDC);
	ASSERT(pRsltInfo);
	CTabRsltInfo* pTabRsltInfo = pRsltInfo->m_pTabRsltInfo;;
	ASSERT(pTabRsltInfo);

	CRect rect;
	CRect rcImg;
	CRect rcClient;

	rcClient.left = 0;
	rcClient.right = nWidth;
	rcClient.top = 0;
	rcClient.bottom = nHeight;

	rcImg.left = 0;
	rcImg.right = nWidth;
	rcImg.top = 0;
	rcImg.bottom = nHeight;

	double dRateX = (double)nWidth / (double)(nWidth * nMagnif);
	double dRateY = (double)nHeight / (double)(nHeight * nMagnif);

	int x, y;
	CString strLine;
	COLORREF color;
	if (pRsltInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) {
		color = RGB(255, 0, 0);
		strLine = _T("JUDGE : NG");
	}
	else {
		color = RGB(0, 255, 0);
		strLine = _T("JUDGE : OK");
	}

	if (pRsltInfo->m_nHeadNo == CAM_POS_TOP) {
		x = 20;
	}
	else {
		x = 150;
	}
	y = 20;
	int yPitch = 30;

	DrawString(pDC, x, y, color, strLine);

	int nDefCount = pTabRsltInfo->GetSize();
	int nDispCnt = 0;
	int nLastPosY = y + yPitch;
	for (int i = 0; i < nDefCount; i++) {
		COLORREF col;

		CDefectInfo* pDefInfo;
		pDefInfo = pTabRsltInfo->m_vecDefInfo[i];
		if ((pDefInfo == nullptr) || (nDispCnt >= MAX_DISP_DEF_COUNT)) break;
		if (pDefInfo->nType == TYPE_SURFACE) continue;

		if (pDefInfo->nRank == JUDGE_NG) {
			col = RGB(230, 50, 50);
		}
		else {
			col = RGB(50, 230, 50);
		}
		strLine.Format(_T("Size_F %d:%.1lf um"), nDispCnt + 1, pDefInfo->dJudgeSize);
		DrawString(pDC, x, y + ((nDispCnt + 1) * yPitch), col, strLine);

		CRect defRect;
		defRect.top = (int)(pDefInfo->rcPos.top * dRateY);
		defRect.bottom = (int)(pDefInfo->rcPos.bottom * dRateY);
		defRect.left = (int)(pDefInfo->rcPos.left * dRateX);
		defRect.right = (int)(pDefInfo->rcPos.right * dRateX);

		int nInfate = 5;
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		DrawDefectRect(pDC, defRect, col);
		nDispCnt++;

	}

	y = 300;
	for (int i = 0, nIdx = 0; i < nDefCount; i++) {
		COLORREF col;

		CDefectInfo* pDefInfo;
		pDefInfo = pTabRsltInfo->m_vecDefInfo[i];
		if ((pDefInfo == nullptr) || ((nIdx + nDispCnt) >= 10)) break;
		if (pDefInfo->nType != TYPE_SURFACE) continue;

		if (pDefInfo->nRank == JUDGE_NG) {
			col = RGB(230, 50, 50);
		}
		else {
			col = RGB(50, 230, 50);
		}

		// Spetter 결함 사이즈 표시
		if (nIdx < MAX_DISP_DEF_COUNT) {
			strLine.Format(_T("Size_S %d:%.1lf um"), (nIdx + nDispCnt), pDefInfo->dJudgeSize);
			DrawString(pDC, x, nLastPosY + ((nIdx + nDispCnt) * yPitch), col, strLine);
		}
		nIdx++;

		CRect defRect;
		defRect.top = (int)(pDefInfo->rcPos.top * dRateY);
		defRect.bottom = (int)(pDefInfo->rcPos.bottom * dRateY);
		defRect.left = (int)(pDefInfo->rcPos.left * dRateX);
		defRect.right = (int)(pDefInfo->rcPos.right * dRateX);

		int nInfate = 5;
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		DrawDefectRect(pDC, defRect, col);
	}
}

void CResultThread::SaveResultImage(HWND HWnd, CFrameRsltInfo* pRsltInfo, BYTE* pImgPtr, int nWidth, int nHeight, int nMagnif, CBitmapStd* bmpAll)
{
	ASSERT(HWnd);
	ASSERT(pImgPtr);
	ASSERT(pRsltInfo);
	CTabRsltInfo* pTabRsltInfo = pRsltInfo->m_pTabRsltInfo;;
	ASSERT(pTabRsltInfo);

	HDC hdc = ::GetDC(HWnd);

	if (hdc == nullptr) {
		return;
	}
	CDC* pDC = CDC::FromHandle(hdc);
	CWnd* pWnd = CWnd::FromHandle(HWnd);

	CDC cdcSave ;
	BYTE* pbyteTmpMemDC = NULL, * pbyteBmpBits = NULL;
	HBITMAP		hBmpSave, hBmpSaveOld;
	BITMAPINFO* pBmpInfoSave;

	// 23.02.08 Ahn Add Start
	CBitmapStd::ImageReverse(pImgPtr, nWidth, nHeight);
	// 23.02.08 Ahn Add End

//	bmpTemp.ImageRevers();
	CSize size;
	size.cx = nWidth * nMagnif;
	size.cy = nHeight * nMagnif;

	bmpAll->MakeImageArea(nWidth, nHeight, 24);
	bmpAll->CopyImgBit8toBit24(pImgPtr, nWidth, nHeight);

	cdcSave.CreateCompatibleDC(pDC);
	pBmpInfoSave = bmpAll->GetBitmapinfoPtr();
	hBmpSave = ::CreateDIBSection(pDC->m_hDC, pBmpInfoSave, DIB_RGB_COLORS, (void**)&pbyteTmpMemDC, NULL, 0);
	hBmpSaveOld = (HBITMAP)::SelectObject(cdcSave.m_hDC, hBmpSave);
	cdcSave.BitBlt(0, 0, pBmpInfoSave->bmiHeader.biWidth, pBmpInfoSave->bmiHeader.biHeight, &cdcSave, 0, 0, BLACKNESS);

	CRect crectDst;
	crectDst.SetRect(0, 0, nWidth, nHeight);

	bmpAll->DrawImageFunction(&cdcSave, CRect(0, 0, crectDst.Width(), crectDst.Height()), crectDst);

	pbyteBmpBits = bmpAll->GetImgPtr();
	bmpAll->UnlockBitmapinfoPtr();

	CResultThread::DrawImage_Test( &cdcSave, pRsltInfo, nWidth, nHeight, nMagnif );

	CopyMemory(pbyteBmpBits, pbyteTmpMemDC, (nWidth * nHeight) * 3);

	// 23.02.09 Ahn Add Start
	//메인 이미지 출력 잠시 제거
	CRect rcClient;
	CRect rcImg( 0, 0, nWidth, nHeight) ;
	pWnd->GetClientRect(rcClient);
	bmpAll->DrawImageFunction(pDC, &rcClient, &rcImg, SRCCOPY, FALSE);
	// 23.02.09 Ahn Add End

	// 됶뺳
	::SelectObject(cdcSave.m_hDC, hBmpSaveOld);
	::DeleteObject(pDC->m_hDC);
	::DeleteObject(cdcSave);
	::DeleteObject(hBmpSave);
}
// 23.02.03 Ahn Add End
 
// 22.02.25 Ahn Add Start
void CResultThread::CaptureImage(HWND HWnd, CString strPath)
{
	ASSERT(HWnd);

	CBitmapStd bmp;
	HBITMAP		hBitmap;
	BITMAPINFO* pBmpInfo;
	BYTE* pImg;
	HDC memhDC;
	BITMAP bm;
	CRect rect;
	HDC hdc = ::GetDC(HWnd);

	if (hdc == nullptr) {
		return;
	}
	CDC* pDC = CDC::FromHandle(hdc);
	CWnd* pWnd = CWnd::FromHandle(HWnd);
	::GetClientRect(HWnd, &rect);

	memhDC = CreateCompatibleDC(hdc); // Memory Device Context 
	// Window Device Context 호환 BITMAP 작성
	hBitmap = CreateCompatibleBitmap(hdc, rect.Width(), rect.Height());
	SelectObject(memhDC, hBitmap);
	// BITMAP에 윈도우의 클라이언트 영역을 복사
	BitBlt(memhDC, 0, 0, rect.Width(), rect.Height(), hdc, 0, 0, SRCCOPY);
	// Graphic Object의 정보 취득
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	bmp.MakeImageArea(bm.bmWidth, bm.bmHeight, 24); 
	//Image 영역 복사
	pBmpInfo = bmp.GetBitmapinfoPtr();
	pImg = bmp.GetImgPtr();
	GetDIBits(hdc, hBitmap, 0, bm.bmHeight, pImg, pBmpInfo, DIB_RGB_COLORS);
	// Release
	if (pWnd != NULL && pWnd->m_hWnd != NULL) {
		::ReleaseDC(pWnd->m_hWnd, hdc);
	}
	DeleteObject(hBitmap);
	DeleteObject(memhDC);
	bmp.UnlockBitmapinfoPtr();	

	bmp.SaveBitmap(strPath);
}
// 22.02.25 Ahn Add End 


#endif

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
void CResultThread::SaveCropImage(BYTE* pImgPtr, int nWidth, int nHeight, CFrameRsltInfo* pFrmInfo, CCropImgQueueCtrl* pQueueCtrl, CDefectQueueCtrl* pDefectQueue, CSpcInspManager* insp)
#else
void CResultThread::SaveCropImage(BYTE* pImgPtr, int nWidth, int nHeight, CFrameRsltInfo* pFrmInfo, CCropImgQueueCtrl* pQueueCtrl, CDefectQueueCtrl* pDefectQueue)
#endif //SPCPLUS_CREATE

{
	ASSERT(pDefectQueue);
	ASSERT(pQueueCtrl);
	ASSERT(pImgPtr);

	CTabRsltInfo *pTabInfo = pFrmInfo->m_pTabRsltInfo;
	int nDefCount = pTabInfo->GetDefectCount();
	CDefectInfo* pDefInfo;
	//CString strMidPath;
	////strFilePath.Format( _T("%s\\CROP\\"), pTabInfo->m_chCropPath ) ;
	//strMidPath.Format(_T("%04d%02d\\%02d\\%02d\\%s")
	//	, pTabInfo->sysTime.wYear, pTabInfo->sysTime.wMonth
	//	, pTabInfo->sysTime.wDay, pTabInfo->sysTime.wHour, AprData.m_NowLotData.m_strLotNo);

	CString strTime;
	strTime.Format(_T("%04d%02d%02d_%02d%02d%02d%03d")
		, pTabInfo->sysTime.wYear, pTabInfo->sysTime.wMonth, pTabInfo->sysTime.wDay
		, pTabInfo->sysTime.wHour, pTabInfo->sysTime.wMinute, pTabInfo->sysTime.wSecond, pTabInfo->sysTime.wMilliseconds);

	CString strPos;
	strPos.Format(_T("%s"), (pTabInfo->m_nHeadNo == 0) ? _T("TAB") : _T("BTM"));
	CString strDefType;

	// 22.12.15 Ahn Add Start
	CString strTabJudge ;
	strTabJudge = (pTabInfo->m_nJudge == JUDGE_NG) ? _T("NG") : _T("OK");
	// 22.12.15 Ahn Add End

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ INSP===================================================================================================
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====DefectInfo CellNo<%d>, CAM<%s>, Defect Info Count<%d>"
		, pTabInfo->m_nTabNo
		, (pTabInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP" : "BOTTOM"
		, nDefCount);
	//===========================================================================================================
#endif //SPCPLUS_CREATE

	for (int i = 0; i < nDefCount; i++) {
		pDefInfo = pTabInfo->GetDefectInfo(i);

		if (pDefInfo != NULL) 
		{			
			// 22.11.28 Ahn Modify End
			CRect rcCrop ;
			CPoint cpNgPos = pTabInfo->GetDefectCenterPos(i);
			int nSizeX = (int)pDefInfo->nSizeX ;
			int nSizeY = (int)pDefInfo->nSizeY ;
			int nSizeL = 128 ;
			CString strJudge;

			// 23.02.08 Ahn Modify Start
			//// 22.11.21 Ahn Modify Start - JUDGE_GRAY
			//strJudge = (pDefInfo->nRank == 0) ? _T("OK") : _T("NG");
			//// 22.11.21 Ahn Modify End
			strJudge = (pDefInfo->nRank == JUDGE_NG) ? _T("NG") : (pDefInfo->nRank == JUDGE_OK) ? _T("OK") : _T("GRAY");
			// 23.02.08 Ahn Modify Start

			rcCrop.left = cpNgPos.x - 128;
			rcCrop.right = cpNgPos.x + 128;
			rcCrop.top = cpNgPos.y - 128;
			rcCrop.bottom = cpNgPos.y + 128;

			strDefType = (pDefInfo->nType == en_ModeSurface) ? _T("Surface") : _T("FoilExp");

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
			//SPC+ INSP===================================================================================================
			//Defect Info 객체 포인터가 NULL이 아니면
			if (insp)
			{
				CString strSpcCropFileName = "";
				CString strSpcCropFilePath = "";
				//Insp InData 객체 포인터
				CSpcInspInData* InspInData = insp->getSpcInspInData();

				//Top 정보 객체 생성
				CSpcInDataDefectInfo* SpcInDataDefectInfo = new CSpcInDataDefectInfo(insp);
				//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고)
				//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
				SpcInDataDefectInfo->setDefectCameraNumber((pTabInfo->m_nHeadNo == 0) ? _T("1") : _T("2"));

				//Rule Base Defect Type(불량명 기입) 
				// 결함 종류 0 : Foil Exposure, 1 : Foil ExposureOut, 2 : Surface
				CString DefectTypeRuleBaseName = (pDefInfo->nType == 0) ? "Foil-Exposure" : (pDefInfo->nType == 1) ? "Foil-ExposureOut" : "Surface";
				SpcInDataDefectInfo->setDefectTypeRuleBaseName(DefectTypeRuleBaseName);
				SpcInDataDefectInfo->setDefectTypeRuleBaseNameReal(DefectTypeRuleBaseName);

				//Image 기준 불량 발생 위치 X [pxl]_절대 위치 Image좌,상단(0,0)
				SpcInDataDefectInfo->setDefectAbsoluteImgX(CGlobalFunc::floatToString(pDefInfo->dSizeX));
				//Image 기준 불량 발생 위치 Y [pxl]_절대 위치 Image좌,상단(0,0)
				SpcInDataDefectInfo->setDefectAbsoluteImgY(CGlobalFunc::floatToString(pDefInfo->dSizeY));

				//Image 기준 Defect의 Width [pxl]
				SpcInDataDefectInfo->setDefectImgWidth(CGlobalFunc::floatToString(pDefInfo->nSizeX));
				//Image 기준 Defect의 Length [pxl]
				SpcInDataDefectInfo->setDefectImgLength(CGlobalFunc::floatToString(pDefInfo->nSizeY));
				//Image 기준 불량 발생 면적 [pxl]
				SpcInDataDefectInfo->setDefectAreaPixels(CGlobalFunc::floatToString(pDefInfo->nSize));


				//Crop Image 파일명
				strSpcCropFileName = SpcInDataDefectInfo->ImagCropFileName();
				SpcInDataDefectInfo->setDefectCropImageFileName(strSpcCropFileName);

				//불량명 전체를 저장한다.
				//상위에 추가할 내용
				insp->addDefectKindName(DefectTypeRuleBaseName);

				//추가한다.
				insp->addSpcInDataDefectInfo(SpcInDataDefectInfo);

				strSpcCropFilePath = InspInData->ImageFilePath()+CString("\\");

				CImageProcess::SaveCropImage(pImgPtr, nWidth, nHeight, rcCrop, strSpcCropFilePath, strSpcCropFileName);

			}

#endif //SPCPLUS_CREATE

			//기존 CROP IMAGE
			CString strFileName;
			strFileName.Format(_T("%s_%s_%s_%s_%d_%s_%s_%s_%d_%s.jpg")
				// 22.12.15 Ahn Add End
				, INSPECTION_TYPE
				, strTime
				, AprData.m_System.m_strMachineID
				, AprData.m_NowLotData.m_strLotNo
				, pTabInfo->m_nTabNo + 1
				, strPos
				, strTabJudge // 22.12.15 Ahn Add 
				, strJudge
				, i
				, strDefType
			);

			CString strFilePath;
			strFilePath.Format(_T("%s\\%s\\%s\\%s\\CROP\\"), AprData.m_strImagePath, strJudge, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);

			CImageProcess::SaveCropImage(pImgPtr, nWidth, nHeight, rcCrop, strFilePath, strFileName);

			// 22.06.10 Ahn Add Star
			if (pQueueCtrl != NULL)
			{
				CCropImgData data;
				data.m_bEnable = TRUE;
				data.m_strFileName.Format( _T("%s%s"), strFilePath, strFileName) ;
				data.m_strDispName.Format(_T("%s_%d_%d"), strPos, pTabInfo->m_nTabNo + 1, i + 1);
				pQueueCtrl->PushBack(data);
			}
			// 22.06.10 Ahn Add End
			pDefInfo->nDefNo = i ; // 22.07.07 Ahn Add
			//strcpy_s(pDefInfo->szImgFileName, strFileName.GetBuffer(0));
			
			// 22.05.27 Ahn Modify Start
			//::_tcsnccpy_s(pDefInfo->szImgFileName, _countof(pDefInfo->szImgFileName), strFileName.GetBuffer(0), _TRUNCATE);
			//strFileName.ReleaseBuffer();
			CString strFullPath;
			strFullPath.Format(_T("%s%s"), strFilePath, strFileName);
			::_tcsnccpy_s(pDefInfo->szImgFileName, _countof(pDefInfo->szImgFileName), strFullPath.GetBuffer(0), _TRUNCATE);
			::_tcsnccpy_s(pDefInfo->szRecipeName, _countof(pDefInfo->szRecipeName), AprData.m_NowLotData.m_strRecipeName.GetBuffer(0), _TRUNCATE);
			// 22.05.27 Ahn Modify End

			pDefInfo->bImageFlag = TRUE;
			// 22.09.15 Ahn Add Start
			pDefInfo->bMarking = pTabInfo->m_bMarkingFlag ;
			// 22.09.15 Ahn Add End

			// 22.06.23 Ahn Add Start
			CDefectInfo* pDefRsltInfo = new CDefectInfo;
			// 22.08.10 Ahn Modify Start
			//pDefRsltInfo = pDefInfo;
			*pDefRsltInfo = *pDefInfo;
			// 22.08.10 Ahn Modify End
			pDefectQueue->PushBack(pDefRsltInfo);
			// 22.06.23 Ahn Add End
		}
	}
}
// 22.05.25 Ahn Add End

#define RESULTTHREAD_TIMEOUT 10
UINT CResultThread::CtrlThreadResultProc(LPVOID pParam)
{
	CResultThread* pThis = (CResultThread*)pParam;
	CQueueCtrl* pQueueResult = pThis->m_pParent->GetResultPtr(pThis->m_nHeadNo);
	CCropImgQueueCtrl* pCropImgQue = pThis->m_pParent->GetCropImageQueuePtr();
	CDefectQueueCtrl* pDefectQueue = pThis->m_pParent->GetDefectQueuePtr(); // 22.06.23 Ahn Add

	UINT ret = 0;
	//스래드 대기 여부
	BOOL bThreadWait = TRUE;
	while (1) 
	{
		//타임 주기 이벤트
		//대기 상태일 때
		if (bThreadWait)
			ret = WaitForSingleObject(pThis->getEvent_ResultThread(), RESULTTHREAD_TIMEOUT);
		//대기하지 않고 바로 처리한다.
		else
			ret = WAIT_TIMEOUT;

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return 0;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			HWND hWnd = pThis->m_DisphWnd;

			if (pThis == NULL)
			{
				break;
			}
			int nSize = pQueueResult->GetSize();
			if (pThis->m_bKill == TRUE)
			{
				if (nSize <= 0)
				{
					break;
				}
			}

			if (nSize > 0)
			{
				//프레임 처리 시간 세팅
				LARGE_INTEGER tmp;
				LARGE_INTEGER loc_stTime;
				QueryPerformanceFrequency(&tmp);
				double dFrequency = (double)tmp.LowPart + ((double)tmp.HighPart * (double)0xffffffff);
				QueryPerformanceCounter(&loc_stTime);

				CFrameRsltInfo* pRsltInfo = (CFrameRsltInfo*)pQueueResult->Pop();

				//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
			//SPC+ INSP===================================================================================================
			//SPC+ 객체 포인터 받는다.(정보를 추가하기 위해)
				CSpcInspManager* insp = dynamic_cast<CSpcInspManager*>(pRsltInfo->m_SpcInspMgr);

#endif //SPCPLUS_CREATE

				if (pRsltInfo == NULL)
				{
					continue;
				}

				int nHeadNo = pRsltInfo->m_nHeadNo;
				BYTE* pImgPtr = pRsltInfo->GetImagePtr();
				if (pImgPtr != NULL)
				{
#if defined( IMAGE_DRAW_NOTIFY_VERSION )
					pThis->m_pParent->SetLastBmpStd(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, nHeadNo);
#else
#if defined( IMAGE_DRAW_DIRECT_VERSION )
					
					if (hWnd != NULL) // 22.04.01 Ahn Add 
					{
						BYTE* pResizePtr;
						int nMagnif = 11; //8;
						int nReWidth = (pRsltInfo->m_nWidth / nMagnif);
						int nReHeight = (pRsltInfo->m_nHeight / nMagnif);
						int nReSize = nReWidth * nReHeight;
						pResizePtr = new BYTE[nReSize + 1];
						memset(pResizePtr, 0x00, sizeof(BYTE) * nReSize + 1);

						// 23.02.24 Ahn Add Start
						CPoint cpSharpness;
						cpSharpness.x = pRsltInfo->m_nTabLevel;
						cpSharpness.y = pRsltInfo->m_nTabLeft / 2;
						pRsltInfo->dSharpness = CImageProcess::GetIqSharpnessValue(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, cpSharpness);
						pRsltInfo->nBrightAverage = CImageProcess::GetBrightAverage(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, cpSharpness);
						// 23.02.24 Ahn Add End


						// 22.05.04 Test
						CImageProcess::ResizeImage(pImgPtr, pResizePtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, nMagnif);

						int nOverflowMax = AprData.m_System.m_nOverflowCountMax;

						// 22.12.19 Ahn Delete Start - Overlay Image
						CBitmapStd bmpStd;
						if (nSize > nOverflowMax)
						{
							if ((pRsltInfo->nTabNo % (nOverflowMax - 1)) == 0)
							{
								// 23.02.06 Ahn Modify Start
								// DrawImage(hWnd, pRsltInfo, pResizePtr, nReWidth, nReHeight, nMagnif);
								SaveResultImage(hWnd, pRsltInfo, pResizePtr, nReWidth, nReHeight, nMagnif, &bmpStd);
								// 23.02.06 Ahn Modify End


							}
						}
						else
						{
							// 23.02.06 Ahn Modify Start
							// DrawImage(hWnd, pRsltInfo, pResizePtr, nReWidth, nReHeight, nMagnif);
							SaveResultImage(hWnd, pRsltInfo, pResizePtr, nReWidth, nReHeight, nMagnif, &bmpStd);
							// 23.02.06 Ahn Modify End


						}

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
						SaveCropImage(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, pRsltInfo, pCropImgQue, pDefectQueue, insp);
#else
						SaveCropImage(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, pRsltInfo, pCropImgQue, pDefectQueue);
#endif //SPCPLUS_CREATE

						// Overlay 저장
						if (pRsltInfo->m_pTabRsltInfo->m_bCropImgFlag == TRUE)
						{
							//이미지 저장 포맷
							CString strImageFormat = AprData.getGSt()->GetOutImageFormat();

							CString strPath;
							CString strTime;
							strTime.Format(_T("%04d%02d%02d_%02d%02d%02d.%d")
								, pRsltInfo->m_pTabRsltInfo->sysTime.wYear
								, pRsltInfo->m_pTabRsltInfo->sysTime.wMonth
								, pRsltInfo->m_pTabRsltInfo->sysTime.wDay
								, pRsltInfo->m_pTabRsltInfo->sysTime.wHour
								, pRsltInfo->m_pTabRsltInfo->sysTime.wMinute
								, pRsltInfo->m_pTabRsltInfo->sysTime.wSecond
								, pRsltInfo->m_pTabRsltInfo->sysTime.wMilliseconds
							);
							strPath.Format(_T("%s\\Overlay\\%s_%s_%s%s")
								, pRsltInfo->m_pTabRsltInfo->m_chImagePath
								, AprData.m_NowLotData.m_strLotNo
								, (pRsltInfo->m_pTabRsltInfo->m_nHeadNo == CAM_POS_TOP) ? _T("TAB") : _T("BOTTOM")
								, strTime
								, strImageFormat
							);
							if (strPath.GetLength() > 0)
							{
								bmpStd.SaveBitmap(strPath);
							}
						}
						
						delete[] pResizePtr;
					}

					pThis->m_pDefDataCtrl->PushBackTabQueue(pRsltInfo->m_pTabRsltInfo);
#endif

#endif 
				}
				else
				{
					//이미지 프로세싱 결과를 처리하는 스래드(이미지 저장등)
					LOGDISPLAY_SPEC(0)("**ERROR Image Save ** 저장할 이미지 정보가 없다");
				}
				//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
			//SPC+ INSP===================================================================================================
			//SPC+ 파일 생성을 위한 스래드에 추가한다.
			//Create JSON 여부를 판단하고 세팅한다.
			//Tab 있는 Frame과 Tab 없는 Frame 이 JSON INSP 1개를 생성하기 때문에 필요함
				if (insp->getCreateJSONFile())
				{
					//SPC+ 정보 출력 로그
					LOGDISPLAY_SPEC(3)("SPC+=====FrameCount<%d>, Frame Kind : %s = Cell Count : %d === JSON 생성 OK "
						, pRsltInfo->m_nFrameCount
						, (pRsltInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP" : "BOTTOM", pRsltInfo->nTabNo);

					//InData 객체를 가져온다.
					///In Data(송신 데이터) 객체 포인터
					CSpcInspInData* InspInData = insp->getSpcInspInData();
					//총 Defect 갯수
					InspInData->setTotalAppearanceNgCount(CGlobalFunc::intToString(insp->getDefectKindNameSize()));
					//Defec명 추가
					//Defect 명 중복을 제거하고 넘긴다.
					std::vector<CString> DefectKindNameUnique = insp->getDefectKindNameUnique();
					//불량명을 추가한다.
					for (int idx = 0; idx < DefectKindNameUnique.size(); idx++)
					{
						InspInData->appendAppearanceReasonAll(DefectKindNameUnique[idx]);
						InspInData->appendAppearanceReasonAllReal(DefectKindNameUnique[idx]);
						if (idx == 0)
						{
							//외관 대표 불량명(우선순위 대표 불량명 없을시 첫번째 검출 항목)	
							InspInData->setAppearanceReasonMain(DefectKindNameUnique[idx]);
							InspInData->setAppearanceReasonMainReal(DefectKindNameUnique[idx]);
						}
					}

					//결함의 순서에 index를 부여한다.
					//Defect 정보가 0번 카메라, 1번 카메라 정보를 모두 받고 JSON파일를 만들기 전 세팅한다.
					insp->setIndexSpcInDataDefectInfo();

					//JSON 파일 생성을 위한 스래드에 추가한다.
					CSpcCreateJSONFileThread::AddSpcPlusManager(insp);
				}
				else
				{
					//SPC+ 정보 출력 로그
					LOGDISPLAY_SPEC(3)("SPC+=====FrameCount<%d>, Frame Kind : %s = Cell Count : %d === JSON 생성 NONE"
						, pRsltInfo->m_nFrameCount
						, (pRsltInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP" : "BOTTOM", pRsltInfo->nTabNo);
				}
				//===========================================================================================================
#endif //SPCPLUS_CREATE

				//==== Tac Time ========================================================================================================================================
				double dTactTime = CGlobalFunc::GetDiffTime(loc_stTime, dFrequency);

				//Image Capture 정보 출력 로그
				LOGDISPLAY_SPEC(4)(_T("Result Pos<%s> TabNo<%d> Tactime <%f>"), (pRsltInfo->m_nHeadNo == 0) ? "TOP" : "BOTTOM", pRsltInfo->nTabNo, dTactTime);
				//=======================================================================================================================================================

				pRsltInfo->m_pTabRsltInfo = NULL;
				delete pRsltInfo;
				pRsltInfo = NULL;

			}
			//큐에 데이터가 있으면 기다리지 않고 실행하도록 설정
			if (pQueueResult->GetSize())
				bThreadWait = FALSE;
			//없으면 대기
			else
				bThreadWait = TRUE;

		}
		else
		{
			break;
		}
	}
	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	return 0;
}
