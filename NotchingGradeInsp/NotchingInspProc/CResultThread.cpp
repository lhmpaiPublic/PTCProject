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

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
//SPC+ 사용을 위한 해더파일
#include "SpcInfo.h"
#include "SpcInspManager.h"
#include "SpcCreateJSONFileThread.h"
#include "SpcInspInData.h"
#include "SpcInDataDefectInfo.h"
#endif //SPCPLUS_CREATE

CResultThread::CResultThread(CImageProcessCtrl* pParent, int nHeadNo)
{
	m_pParent = pParent;
	m_pThread = NULL;
	pEvent_ResultThread = NULL;

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
		m_pThread = NULL;
	}

	if (pEvent_ResultThread)
	{
		CloseHandle(pEvent_ResultThread);
		pEvent_ResultThread = NULL;
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

void CResultThread::DrawString(CDC* pDC, int x, int y, COLORREF color, CString strLine)
{
	if (pDC == NULL) {
		return;
	}

	CFont font;
	VERIFY(font.CreateFont(
		100,                       // nHeight
		0,                        // nWidth
		0,                        // nEscapement
		0,                        // nOrientation
		FW_NORMAL,                // nWeight
		FALSE,                    // bItalic
		FALSE,                    // bUnderline
		0,                        // cStrikeOut
		ANSI_CHARSET,             // nCharSet
		OUT_DEFAULT_PRECIS,       // nOutPrecision
		CLIP_DEFAULT_PRECIS,      // nClipPrecision
		DEFAULT_QUALITY,          // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("Arial")));            // lpszFacename

	pDC->SelectObject(font);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(color);
	pDC->TextOut(x, y, strLine);
}

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

	int nDispCnt = 0;
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
		strLine.Format(_T("Size_F %d:%.1lf x %.1lf um"), nDispCnt +1, pDefInfo->dJudgeSize, pDefInfo->dSizeY);
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
			strLine.Format(_T("Size_S %d:%.1lf x %.1lf um um"), (nIdx + nDispCnt  ), pDefInfo->dJudgeSize, pDefInfo->dSizeY);
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

void CResultThread::DrawImage_Test(CDC* pDC, CFrameRsltInfo* pRsltInfo, int nWidth, int nHeight, int nMagnif)
{
	ASSERT(pDC);
	ASSERT(pRsltInfo);
	//Cell 의 검사 정보 객체 얻기
	CTabRsltInfo* pTabRsltInfo = pRsltInfo->m_pTabRsltInfo;;
	ASSERT(pTabRsltInfo);

	//이미지 영역 정보
	CRect rcImg;
	//이미지 출력을 위한 윈도우 클라이언트 영역 정보
	CRect rcClient;

	//윈도우 클라이언트 영역 정보 세팅
	rcClient.left = 0;
	rcClient.right = nWidth;
	rcClient.top = 0;
	rcClient.bottom = nHeight;

	//이미지 영역 정보 세팅
	rcImg.left = 0;
	rcImg.right = nWidth;
	rcImg.top = 0;
	rcImg.bottom = nHeight;

	//이미지 넓이의 축소 율 값 계산
	double dRateX = (double)nWidth / (double)(nWidth * nMagnif);
	//이미지 높이 축소 율 값 계산
	double dRateY = (double)nHeight / (double)(nHeight * nMagnif);

	int x, y;
	CString strLine;
	COLORREF color;
	//NG 일경우 컬러 값을 red, 테스트 출력 
	if (pRsltInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) {
		color = RGB(255, 0, 0);
		strLine = _T("JUDGE : NG");
	}
	//아닐 경우 green, 텍스트 출력
	else {
		color = RGB(0, 255, 0);
		strLine = _T("JUDGE : OK");
	}

	//윈도우에 이미지 출력 Tab이 있는 경우 x 값
	if (pRsltInfo->m_nHeadNo == CAM_POS_TOP) {
		x = 20;
	}
	//윈도우에 이미지 출력 Tab이 없는 경우 x 값
	else {
		x = 150;
	}

	//윈도우 이미지 출력 y값
	y = 20;
	//다음 라인에 테스트를 쓸 위치 Pitch 값
	int yPitch = 30;

	//이미지에 한 라인으로 text를 쓴다.
	DrawString(pDC, x, y, color, strLine);

	//defect 정보 갯수를 가져온다.
	int nDefCount = pTabRsltInfo->GetSize();

	//defect 순번 
	int nDispCnt = 0;

	//defect 텍스트를 쓸 다음 위치 정보
	int nLastPosY = y + yPitch;

	//defect 정보 갯수만큼 루프를 돈다.
	//defect 위치에 텍스트출력, 사각형 그리기 위한 소스
	for (int i = 0; i < nDefCount; i++) 
	{
		//컬러 정보 객체
		COLORREF col;
		//defect 정보를 받을 객체
		CDefectInfo* pDefInfo;
		//defect 정보를 1개 가져온다.
		pDefInfo = pTabRsltInfo->m_vecDefInfo[i];
		//defect 정보가 NULL 이거나 출력 갯수가 MAX_DISP_DEF_COUNT 개 이상이면 빠져나간다.
		if ((pDefInfo == nullptr) || (nDispCnt >= MAX_DISP_DEF_COUNT)) break;

		//defect 정보가 surface 일 경우 출력하지 않는다.
		if (pDefInfo->nType == TYPE_SURFACE) continue;

		//defect 정보가 NG 일경우 컬러 색
		if (pDefInfo->nRank == JUDGE_NG) {
			col = RGB(230, 50, 50);
		}
		//아닐 경우 컬러 색
		else {
			col = RGB(50, 230, 50);
		}
		//출력 텍스트를 만든다.
		strLine.Format(_T("Size_F %d:%.1lf x %.1lf um"), nDispCnt + 1, pDefInfo->dJudgeSize, pDefInfo->dSizeY );

		//텍스트를 출력한다.
		//pDC x, y : 카운트 * 텍스트 pitch, col : 색, strLine : 텍스트 내용
		DrawString(pDC, x, y + ((nDispCnt + 1) * yPitch), col, strLine);

		//defect 위치 값에 축소율 곱하여 위치를 잡는다.
		CRect defRect;
		defRect.top = (int)(pDefInfo->rcPos.top * dRateY);
		defRect.bottom = (int)(pDefInfo->rcPos.bottom * dRateY);
		defRect.left = (int)(pDefInfo->rcPos.left * dRateX);
		defRect.right = (int)(pDefInfo->rcPos.right * dRateX);

		int nInfate = 5;
		//CRect 중심에서 옆으로 이동하여 팽창합니다
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);

		//사각형 주위에 테두리를 그립니다
		//defect 위치에 사각형 그리기
		DrawDefectRect(pDC, defRect, col);
		//다음 defect 카운트
		nDispCnt++;

	}
	//defect 정보 갯수만큼 루프를 돈다.
	//defect 위치에 텍스트출력, 사각형 그리기 위한 소스
	y = 300;
	for (int i = 0, nIdx = 0; i < nDefCount; i++) 
	{
		//컬러 값
		COLORREF col;
		//defect 정보 객체
		CDefectInfo* pDefInfo;
		//defect 정보 얻기
		pDefInfo = pTabRsltInfo->m_vecDefInfo[i];

		//defect 정보가 NULL 이거나 이전 갯 수의 10를 넘기지 않는다.
		if ((pDefInfo == nullptr) || ((nIdx + nDispCnt) >= 10)) break;
		//defect 정보가 surface 이면 출력하지 않는다.
		if (pDefInfo->nType != TYPE_SURFACE) continue;
		//NG : 컬러 값
		if (pDefInfo->nRank == JUDGE_NG) {
			col = RGB(230, 50, 50);
		}
		//그외 컬러 값
		else {
			col = RGB(50, 230, 50);
		}

		// Spetter 결함 사이즈 표시
		//텍스트 출력 : MAX_DISP_DEF_COUNT 갯수 보다 작을 때 만 출력함
		if (nIdx < MAX_DISP_DEF_COUNT) {
			strLine.Format(_T("Size_S %d:%.1lf x %.1lf um"), (nIdx + nDispCnt), pDefInfo->dJudgeSize, pDefInfo->dSizeY);
			DrawString(pDC, x, nLastPosY + ((nIdx + nDispCnt) * yPitch), col, strLine);
		}
		nIdx++;
		//defect  영역 정보 계산 : 축소율 곱한다.
		CRect defRect;
		defRect.top = (int)(pDefInfo->rcPos.top * dRateY);
		defRect.bottom = (int)(pDefInfo->rcPos.bottom * dRateY);
		defRect.left = (int)(pDefInfo->rcPos.left * dRateX);
		defRect.right = (int)(pDefInfo->rcPos.right * dRateX);

		int nInfate = 5;
		//CRect 중심에서 옆으로 이동하여 팽창합니다
		defRect.InflateRect(nInfate, nInfate, nInfate, nInfate);
		//사각형 주위에 테두리를 그립니다
		//defect 위치에 사각형 그리기
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

	CBitmapStd::ImageReverse(pImgPtr, nWidth, nHeight);

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

	//메인 이미지 출력 잠시 제거
	CRect rcClient;
	CRect rcImg( 0, 0, nWidth, nHeight) ;
	pWnd->GetClientRect(rcClient);
	bmpAll->DrawImageFunction(pDC, &rcClient, &rcImg, SRCCOPY, FALSE);

	::SelectObject(cdcSave.m_hDC, hBmpSaveOld);
	::DeleteObject(pDC->m_hDC);
	::DeleteObject(cdcSave);
	::DeleteObject(hBmpSave);
}
 
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

#endif

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
void CResultThread::SaveCropImage(const BYTE* pImgPtr, int nWidth, int nHeight, CFrameRsltInfo* pFrmInfo, CCropImgQueueCtrl* pQueueCtrl, CDefectQueueCtrl* pDefectQueue, CSpcInspManager* insp)
#else
void CResultThread::SaveCropImage(const BYTE* pImgPtr, int nWidth, int nHeight, CFrameRsltInfo* pFrmInfo, CCropImgQueueCtrl* pQueueCtrl, CDefectQueueCtrl* pDefectQueue)
#endif //SPCPLUS_CREATE

{
	ASSERT(pDefectQueue);
	ASSERT(pQueueCtrl);
	ASSERT(pImgPtr);

	CTabRsltInfo* pTabInfo = pFrmInfo->m_pTabRsltInfo;
	int nDefCount = pTabInfo->GetDefectCount();
	CDefectInfo* pDefInfo;
	CString strTime;
	strTime.Format(_T("%04d%02d%02d_%02d%02d%02d%03d")
		, pTabInfo->sysTime.wYear, pTabInfo->sysTime.wMonth, pTabInfo->sysTime.wDay
		, pTabInfo->sysTime.wHour, pTabInfo->sysTime.wMinute, pTabInfo->sysTime.wSecond, pTabInfo->sysTime.wMilliseconds);

	CString strPos;
	strPos.Format(_T("%s"), (pTabInfo->m_nHeadNo == 0) ? _T("TAB") : _T("BTM"));
	CString strDefType;

	CString strTabJudge;
	strTabJudge = (pTabInfo->m_nJudge == JUDGE_NG) ? _T("NG") : _T("OK");

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ INSP===================================================================================================
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("!! SPC+=====DefectInfo TabNo<%d>, CAM<%s>, Defect Info Count<%d>"
		, pTabInfo->m_nTabNo+1
		, (pTabInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP" : "BOTTOM"
		, nDefCount);
	//===========================================================================================================
#endif //SPCPLUS_CREATE

	for (int i = 0; i < nDefCount; i++)
	{
		pDefInfo = pTabInfo->GetDefectInfo(i);

		if (pDefInfo != NULL)
		{

			CRect rcCrop;
			CPoint cpNgPos = pTabInfo->GetDefectCenterPos(i);
			int nSizeX = (int)pDefInfo->nSizeX;
			int nSizeY = (int)pDefInfo->nSizeY;
			int nSizeL = 128;
			CString strJudge;

			strJudge = (pDefInfo->nRank == JUDGE_NG) ? _T("NG") : (pDefInfo->nRank == JUDGE_OK) ? _T("OK") : _T("GRAY");

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
				// 결함 종류 0 : Foil Exposure, 1 : Foil ExposureOut, 2 : Surface(변경 전)
				// 결함 종류 0 : Foil-Inner Exposure, 1: Foil-Outer Exposure, 2 : Foil-Exposure (변경 후)
				CString DefectTypeRuleBaseName = (pDefInfo->nType == 0) ? "Foil-Inner Exposure" : (pDefInfo->nType == 1) ? "Foil-Outer Exposure" : "Foil-Exposure";

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
				//SPC+ CROP 이미지 출력 경로
				strSpcCropFilePath = InspInData->ImageFilePath(strSpcCropFilePath, FALSE) + CString("\\");

				//SPC+ 활성화 한 상태에서 만 출력한다.
				if (CSpcInfo::Inst()->getSPCStartFlag())
				{
					CImageProcess::SaveCropImage(pImgPtr, nWidth, nHeight, rcCrop, strSpcCropFilePath, strSpcCropFileName);
				}

			}

#endif //SPCPLUS_CREATE

			//CROP IMAGE 파일 명 생성
			CString strFileName;
			strFileName.Format(_T("%s_%s_%s_%s_%d_%s_%s_%s_%d_%s.jpg")
				, INSPECTION_TYPE
				, strTime
				, AprData.m_System.m_strMachineID
				, AprData.m_NowLotData.m_strLotNo
				, pTabInfo->m_nTabNo + 1
				, strPos
				, strTabJudge 
				, strJudge
				, i
				, strDefType
			);

			//CROP 이미지 출력 경로 생성
			CString strFilePath;
			strFilePath.Format(_T("%s\\%s\\%s\\%s\\CROP\\"), AprData.m_strImagePath, strJudge, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);

			//크롭 이미지를 저장한다.
			//크롭이미지는 퀄리티 100으로 저장
			CImageProcess::SaveCropImage(pImgPtr, nWidth, nHeight, rcCrop, strFilePath, strFileName);

			if (pQueueCtrl != NULL)
			{
				CCropImgData data;
				data.m_bEnable = TRUE;
				data.m_strFileName.Format(_T("%s%s"), strFilePath, strFileName);
				data.m_strDispName.Format(_T("%s_%d_%d"), strPos, pTabInfo->m_nTabNo + 1, i + 1);
				pQueueCtrl->PushBack(data);
			}
			pDefInfo->nDefNo = i; 

			CString strFullPath;
			strFullPath.Format(_T("%s%s"), strFilePath, strFileName);
			::_tcsnccpy_s(pDefInfo->szImgFileName, _countof(pDefInfo->szImgFileName), strFullPath.GetBuffer(0), _TRUNCATE);
			::_tcsnccpy_s(pDefInfo->szRecipeName, _countof(pDefInfo->szRecipeName), AprData.m_NowLotData.m_strRecipeName.GetBuffer(0), _TRUNCATE);

			pDefInfo->bImageFlag = TRUE;
			pDefInfo->bMarking = pTabInfo->m_bMarkingFlag;

			CDefectInfo* pDefRsltInfo = new CDefectInfo;
			*pDefRsltInfo = *pDefInfo;
			pDefectQueue->PushBack(pDefRsltInfo);

		}
		else
		{
			LOGDISPLAY_SPEC(8)("## SaveCropImage Defect Info delete NJULL");
		}
	}
}

#define RESULTTHREAD_TIMEOUT 10
UINT CResultThread::CtrlThreadResultProc(LPVOID pParam)
{
	CResultThread* pThis = (CResultThread*)pParam;
	CQueueCtrl* pQueueResult = CImageProcessCtrl::GetResultPtr(pThis->m_nHeadNo);
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
			break;
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
				CFrameRsltInfo* pRsltInfo = (CFrameRsltInfo*)pQueueResult->Pop();

				//SPC 객체 소스에서 컴파일 여부 결정

				if (pRsltInfo != NULL)
				{

#ifdef SPCPLUS_CREATE
					//SPC+ INSP===================================================================================================
					//SPC+ 객체 포인터 받는다.(정보를 추가하기 위해)
					CSpcInspManager* insp = dynamic_cast<CSpcInspManager*>(pRsltInfo->m_SpcInspMgr);

#endif //SPCPLUS_CREATE

					int nHeadNo = pRsltInfo->m_nHeadNo;
					BYTE* pImgPtr = pRsltInfo->GetImagePtr();
					if (pImgPtr != NULL)
					{
#if defined( IMAGE_DRAW_NOTIFY_VERSION )
						pThis->m_pParent->SetLastBmpStd(pImgPtr->m_pImagePtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, nHeadNo);
#else
#if defined( IMAGE_DRAW_DIRECT_VERSION )

						if (hWnd != NULL) 
						{
							BYTE* pResizePtr;
							int nMagnif = 5; //8;
							int nReWidth = (pRsltInfo->m_nWidth / nMagnif);
							int nReHeight = (pRsltInfo->m_nHeight / nMagnif);
							int nReSize = nReWidth * nReHeight;
							pResizePtr = new BYTE[nReSize + 1];
							memset(pResizePtr, 0x00, sizeof(BYTE) * nReSize + 1);

							CPoint cpSharpness;
							cpSharpness.x = pRsltInfo->m_nTabLevel;
							cpSharpness.y = pRsltInfo->m_nTabLeft / 2;
							pRsltInfo->dSharpness = CImageProcess::GetIqSharpnessValue(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, cpSharpness);
							pRsltInfo->nBrightAverage = CImageProcess::GetBrightAverage(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, cpSharpness);

							CImageProcess::ResizeImage(pImgPtr, pResizePtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, nMagnif);

							int nOverflowMax = AprData.m_System.m_nOverflowCountMax;

							CBitmapStd bmpStd;
							if (nSize > nOverflowMax)
							{
								if ((pRsltInfo->nTabNo % (nOverflowMax - 1)) == 0)
								{
									SaveResultImage(hWnd, pRsltInfo, pResizePtr, nReWidth, nReHeight, nMagnif, &bmpStd);

								}
							}
							else
							{
								SaveResultImage(hWnd, pRsltInfo, pResizePtr, nReWidth, nReHeight, nMagnif, &bmpStd);

							}

							//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
							SaveCropImage(pImgPtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, pRsltInfo, pCropImgQue, pDefectQueue, insp);
#else
							SaveCropImage(pImgPtr->m_pImagePtr, pRsltInfo->m_nWidth, pRsltInfo->m_nHeight, pRsltInfo, pCropImgQue, pDefectQueue);
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
								strPath.Format(_T("%s\\Overlay\\%s_%s_%d_%s%s")
									, pRsltInfo->m_pTabRsltInfo->m_chImagePath
									, AprData.m_NowLotData.m_strLotNo
									, (pRsltInfo->m_pTabRsltInfo->m_nHeadNo == CAM_POS_TOP) ? _T("TAB") : _T("BOTTOM")
									, pRsltInfo->m_pTabRsltInfo->m_nTabNo + 1
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

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
						LOGDISPLAY_SPEC(3)("!! SPC+===== JSON 생성 Insp AddSpcPlusManager === Trigger 받은 시간<%s>", InspInData->getVisionInputTime());
#endif //SPCPLUS_CREATE

						//JSON 파일 생성을 위한 스래드에 추가한다.
						CSpcCreateJSONFileThread::AddSpcPlusManager(insp);
					}
					else
					{
						//SPC+ 정보 출력 로그
						LOGDISPLAY_SPEC(3)("!! SPC+=====FrameCount<%d>, Frame Kind : %s = TabNo : %d === JSON 생성 NONE"
							, pRsltInfo->m_nFrameCount
							, (pRsltInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP" : "BOTTOM", pRsltInfo->nTabNo+1);
					}
					//===========================================================================================================
#endif //SPCPLUS_CREATE

					pRsltInfo->m_pTabRsltInfo = NULL;
					delete pRsltInfo;
					pRsltInfo = NULL;

					
				}
				else
				{
					LOGDISPLAY_SPEC(8)("## CtrlThreadResultProc CFrameRsltInfo Object delete NULL");
				}

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
