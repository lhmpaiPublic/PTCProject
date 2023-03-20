#include "pch.h"
#include "ImageProcThread.h"
#include "ImageProcessCtrl.h"
#include "Bitmapstd.h"
#include "QueueCtrl.h"
#include "FrameInfo.h"
#include "FrameRsltInfo.h"
#include "Win32File.h"
#include "TimeAnalyzer.h"
#include "CImageProcess.h"
#include "GlobalData.h"
#include "GlobalDef.h"
#include "CRecipeCtrl.h"
#include "CCounterQueueCtrl.h" 
#include "SigProc.h"
#include "NotchingGradeInsp.h" // 22.04.01 Ahn Add 

#include "CThreadQueueCtrl.h"
#include "CImageProcThreadUnit.h"
#include "AppDIO.h"

// 22.05.31 Ahn Add Start
#include "CImageSaveQueueCtrl.h"
// 22.05.31 Ahn Add End
// 22.12.09 Ahn Add Start
#include "CTacTimeDataCtrl.h"
// 22.12.09 Ahn Add End

//using namespace HalconCpp;

CImageProcThread::CImageProcThread(CImageProcessCtrl *pParent)
{
	m_pParent = pParent ;
	m_pThread = NULL ;
}

CImageProcThread::~CImageProcThread(void)
{
}

void CImageProcThread::Begin( int nMode ) // nMode  0 : Image Merge Mode , 1 : Image Proc Mode 
{
	m_bKill = FALSE ;

//	m_DisphWnd = NULL ;
	if ( m_pThread == NULL ) {
		//이미지 Cut 스래드 생성
		if (nMode == 0) {
			m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadImgCuttingTab,
				(LPVOID)this,
				THREAD_PRIORITY_HIGHEST,
				0,
				CREATE_SUSPENDED,
				NULL);
		}
		//
		else {
			m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadImgProc,
				(LPVOID)this,
				THREAD_PRIORITY_HIGHEST,
				0,
				CREATE_SUSPENDED,
				NULL);
		}
		if ( m_pThread != NULL ) {
			m_pThread->m_bAutoDelete = FALSE ;
			m_pThread->ResumeThread() ;
		}
	}
}
void CImageProcThread::Kill( void ) 
{
	DWORD	dwCode ;
	LONG	ret ;

	if ( m_pThread != NULL ) {
		ret = ::GetExitCodeThread( m_pThread->m_hThread, &dwCode ) ;
		if ( ret && dwCode == STILL_ACTIVE ) {
			m_bKill = TRUE ;
			WaitForSingleObject( m_pThread->m_hThread, INFINITE ) ;
		}
		delete m_pThread ;
		m_pThread = NULL ;
	}

}


// Queue에서 받아온 Frame Image를 Tab 으로 구분해서 처리용 Queue로 저장 하는 Thread
UINT CImageProcThread::CtrlThreadImgCuttingTab(LPVOID Param)
{
	//이미지 처리 스래드 객체 최상위 오브젝트 포인터
	CImageProcThread* pThis = (CImageProcThread*)Param;

	//Fram Top 큐 오브젝트 포인터
	CQueueCtrl* pQueueFrame_Top = pThis->m_pParent->GetQueueFrmPtr(0);

	//Fram Bottom 큐 오브텍트 포인터
	CQueueCtrl* pQueueFrame_Bottom = pThis->m_pParent->GetQueueFrmPtr(1);

	//Counter큐 오브젝트 포인터
	CCounterQueueCtrl* pCntQueueInCtrl = pThis->m_pParent->GetCounterQueInPtr(); // 탭 카운터 용 큐

	//스래드큐 관리 객체 Top/Bottom  객체를 저장할 임시 변수 생성
	CThreadQueueCtrl* pThreadQue[MAX_CAMERA_NO];

	//스래드큐 관리 객체에서 Top에 대한 스래드큐 객체를 가져온다.
	pThreadQue[CAM_POS_TOP] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_TOP);

	//스래드큐 관리객체에서 Bottom에 대한 스래드큐 객체를 가져온다.
	pThreadQue[CAM_POS_BOTTOM] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_BOTTOM);

	BOOL bReserved = FALSE; // 크기가 작아서 보내지 못한 부분이 있음 다음 이미지 받아서 처리 할 것인지에 대한 Flag.
	//예약된 프레임 번호
	BOOL bReservFrmNo = -1;
	//마지막 길이
	int nLastLengh = 0;
	//마지막 넓이
	int nLastWidth = 0;

	//예약 Tab 정보 생성
	CTabInfo reservTabInfo;
	CString strMsg;
	CString strTemp;
	CTabInfo RsvTabInfo;

	//처리 플래그 초기화
	int nFindPos = 0;
	BOOL bHeadFlag = FALSE;
	BOOL bTailFlag = FALSE;
	BOOL bStopFlag = FALSE;

	// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
	BYTE* btLastBtmImg = NULL;
#endif
	// 22.02.22 Ahn Add End

	// 22.04.06 Ahn Add Start - 첫 탭 버림
	//First Tab을 처리하는가를 TRUE 로 초기화하낟.
	BOOL bFirstTab = TRUE;
	// 22.04.06 Ahn Add End

	while (1) {
		if (pThis == NULL) {
			break;
		}
		//이미지 처리 최상위 객체가 Kill이면
		if (pThis->m_bKill == TRUE) {
			//Bottom 카메라 디버그가 아니경우
#if !defined( BOTTOM_CAMERA_DEBUG )
			if ((pQueueFrame_Top != NULL) && (pQueueFrame_Bottom != NULL)) {
				if ((pQueueFrame_Top->GetSize() <= 0) || (pQueueFrame_Bottom->GetSize() <= 0)) {
					break;
				}
			}
			//Bottom 카메라 디버그 일 경우
#else
			if ((pQueueFrame_Top != NULL)) {
				if ((pQueueFrame_Top->GetSize() <= 0)) {
					break;
				}
			}
#endif
		}

		//크기
		//Frame 큐 top 크기
		int nSizeFrmL = pQueueFrame_Top->GetSize();
		//Frame 큐 bottom 크기
		int nSizeFrmR = pQueueFrame_Bottom->GetSize();

		//Head, Tail 플래그 생성
		//Frame 큐 top이 객체가 있으면 Head가 TRUE
		bHeadFlag = !pQueueFrame_Top->IsEmpty();
		//Frame 큐 bottom 객체가 있으면 Tail TRUE
		bTailFlag = !pQueueFrame_Bottom->IsEmpty();

		//Top - Bottom 사이즈 차이 값이 FRAME_ACQ_ERROR_CHK_CNT 이상일경우
		if (abs(nSizeFrmL - nSizeFrmR) > FRAME_ACQ_ERROR_CHK_CNT) {
			// 에러 처리 
		//	pThis->SetFameSizeError(); // 
		
			//.프레임 사이즈 이상 로그를 출력한다.
			CString strErrMsg;
			strErrMsg.Format(_T("프레임 사이즈 이상 : Top [%d], Bottom[%d], 검사 상태 [%d], 종료 처리!!!!"), nSizeFrmL, nSizeFrmR , pThis->m_pParent->IsInspection() );
		//	AprData.SaveErrorLog(strErrMsg);
			AprData.m_ErrStatus.SetError(CErrorStatus::en_CameraError, strErrMsg);
		}

		//Bottom 카메라 디버그 일때
#if defined( BOTTOM_CAMERA_DEBUG )
		bTailFlag = TRUE;
#endif
		// 22.02.22 Ahn Add Start
		//디버그 노이즈카운터 
#if defined( DEBUG_NOISE_COUNTERMEASURE )
		BOOL bMakeDummyBtm;
		//if ((bHeadFlag == TRUE) && (bTailFlag == FALSE)) {
		if (((nSizeFrmL - nSizeFrmR) > 0) && (btLastBtmImg != NULL)) {
			bMakeDummyBtm = TRUE;
			AprData.SaveDebugLog(_T("!!!! MAKE Dummy Data -- 00 !!!!"));
		}
		else {
			bMakeDummyBtm = FALSE;
		}
#endif
		// 22.02.22 Ahn Add End

				// 22.02.22 Ahn Modify Start
		//디버그 노이즈 카운터
#if defined( DEBUG_NOISE_COUNTERMEASURE )
		if ((bHeadFlag && bTailFlag) || (bMakeDummyBtm == TRUE)) {
#else
		if (bHeadFlag && bTailFlag) {
#endif
			// 22.02.22 Ahn Modify End
			//Frame To 정보객체를 가져온다.
			CFrameInfo* pFrmInfo_Top = pQueueFrame_Top->Pop();

			//Top 프레임정보 높이 넓이 정보를 가져온다.
			int nHeight = pFrmInfo_Top->m_nHeight;
			int nFrmWIdth = pFrmInfo_Top->m_nWidth;
			int nWidth = nFrmWIdth;

			// 22.02.22 Ahn Add Start
			//디버그 노이즈 카운터
#if defined( DEBUG_NOISE_COUNTERMEASURE )
			CFrameInfo* pFrmInfo_Bottom;
			if (bMakeDummyBtm == TRUE) {
				pFrmInfo_Bottom = new CFrameInfo;
				pFrmInfo_Bottom->m_nFrameCount = pFrmInfo_Top->m_nFrameCount;
				pFrmInfo_Bottom->m_nWidth = pFrmInfo_Top->m_nWidth;
				pFrmInfo_Bottom->m_nHeight = pFrmInfo_Top->m_nHeight;
				pFrmInfo_Bottom->m_nHeadNo = CAM_POS_BOTTOM;
				pFrmInfo_Bottom->m_nBand = 1;
				BYTE* pDummyImgPtr = new BYTE[nWidth * nHeight];
				//memset(pDummyImgPtr, 0x00, sizeof(BYTE)nWidth * nHeight);
				//if (btLastBtmImg != NULL) {
				AprData.SaveDebugLog(_T("!!!! Last bottom frame copy start !!!!"));
				memcpy(pDummyImgPtr, btLastBtmImg, sizeof(BYTE) * nWidth * nHeight);
				AprData.SaveDebugLog(_T("!!!! Last bottom frame copy end !!!!"));
				//}
				//else {
				//	memset(pDummyImgPtr, 0x00, sizeof(BYTE)* nWidth * nHeight);
				//}
				pFrmInfo_Bottom->SetImgPtr(pDummyImgPtr);
				AprData.SaveDebugLog(_T("!!!! MAKE Dummy Data !!!!"));
				pFrmInfo_Bottom->m_bErrorFlag = TRUE;
			}
			else {
				pFrmInfo_Bottom = pQueueFrame_Bottom->Pop();
			}

#else
// 22.02.22 Ahn Add End
// 22.01.04 Ahn Modify Start
			//BOTTOM 카메라 디버그가 아닐 경우
#if !defined( BOTTOM_CAMERA_DEBUG )
			CFrameInfo* pFrmInfo_Bottom = pQueueFrame_Bottom->Pop();
#else
			CFrameInfo* pFrmInfo_Bottom = new CFrameInfo();
			BYTE* pImg = new BYTE[nWidth * nHeight];
			memset(pImg, 0x00, sizeof(BYTE) * nWidth * nHeight);

			BOOL bSend = FALSE;
			pFrmInfo_Bottom = new CFrameInfo;
			pFrmInfo_Bottom->SetImgPtr(pImg);
			pFrmInfo_Bottom->m_nHeight = nHeight;
			pFrmInfo_Bottom->m_nWidth = nWidth;
			pFrmInfo_Bottom->m_nFrameCount = pFrmInfo_Top->m_nFrameCount;
			pFrmInfo_Bottom->m_nBand = 1;
			pFrmInfo_Bottom->m_nHeadNo = CAM_POS_BOTTOM;
#endif 
			// 22.01.04 Ahn Modify End
			// 22.02.22 Ahn Add Start
#endif
// 22.02.22 Ahn Add End

			//Top 프레임 카운트 
			int nFrameCountL = pFrmInfo_Top->m_nFrameCount;
			//Bottom 프레임 카운트
			int nFrameCountR = pFrmInfo_Bottom->m_nFrameCount;

			// 22.11.30 Ahn Modify Start
			//글로벌 Lot Data Top 카운터 설정
			//이유는 ? :
			AprData.m_NowLotData.m_nFrameCount = pFrmInfo_Top->m_nFrameCount ;
			// 22.11.30 Ahn Modify End

			//Head 이미지 데이터 : Top 프레임 정보에서 가져온다.
			BYTE* pHeadPtr = pFrmInfo_Top->GetImagePtr();
			// Tail 이미지 데이터 : Bottom 프레임 정보에서 가져온다.
			BYTE* pTailPtr = pFrmInfo_Bottom->GetImagePtr();

			// 22.02.22 Ahn Add Start
			//디버그 노이즈 카운터
#if defined( DEBUG_NOISE_COUNTERMEASURE )
			if (btLastBtmImg == NULL) {
				btLastBtmImg = new BYTE[nWidth * nHeight];
			}
			memcpy(btLastBtmImg, pTailPtr, sizeof(BYTE) * nWidth * nHeight);
#endif
			// 22.02.22 Ahn Add End

			//처리 시간 체크를 위한 객체 생성 및 초기화
			CTimeAnalyzer ctAna;
			ctAna.Clear();
			//처리시간 체크 시작
			ctAna.StopWatchStart();

			// Tab으로 잘라 보냄
			// Projection 사용 
			{
				//처리 시간 체크를 위한 객체 생성 및 체크 시작
				CTimeAnalyzer ctAna;
				ctAna.StopWatchStart();

				// 22.05.09 Ahn Add Start
				//내부 설정 변수 초기화
				int nBndElectrode = 0;
				int nBneElectrodeBtm = 0;

				//양극일 경우 이미지 프로세싱 처리한다. GetBoundaryOfElectorde
#if defined( ANODE_MODE )

				nBndElectrode = CImageProcess::GetBoundaryOfElectorde(pHeadPtr, nWidth, nHeight, AprData.m_pRecipeInfo, CImageProcess::en_FindFromLeft);
#endif
				// 22.05.09 Ahn Add End
				//이미지 프로세싱 vector Tab 정보 객체 생성
				CImageProcess::_VEC_TAB_INFO vecTabInfo;

				//레벨 변수 초기화
				int nLevel = 0;

				//Btm 레벨 변수 초기화
				int nBtmLevel = 0;

				//Tab 위치 값 양극일 경우 추가
				int nTabFindPos = nBndElectrode + AprData.m_pRecipeInfo->TabCond.nCeramicHeight ;

				// 22.11.18 Ahn Modify Start
				//int nLocalRet = CImageProcess::DivisionTab_FromImageToTabInfo(pHeadPtr, pTailPtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo );
				
				//Tab 분해 이미지 정보에서 Tab 정보
				int nLocalRet = CImageProcess::DivisionTab_FromImageToTabInfo( pHeadPtr, pTailPtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo, nFrameCountL);
				// 22.11.18 Ahn Modify End
				// 21.12.28 Ahn Modify Start
				//int nLocalRet2 = CImageProcess::FindTabLevel(pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);

				//가져온 vector Tab 정보 크기 vector 사이즈
				int nVecSize = (int)vecTabInfo.size();

				//Error 플래그 초기화
				BOOL bErrorAll = FALSE;

				//Tab 정보 사이즈가 없다면
				if (nVecSize <= 0) {
					//Tab 정보 찾기 실패 로그 출력
					AprData.SaveDebugLog(_T("!!!!Tab Find Faile!!!!"));
					// 강제 분할 
					bErrorAll = TRUE;
				}
				//모드가 양극일 경우
#if defined( ANODE_MODE )
				nBneElectrodeBtm = CImageProcess::GetBoundaryOfElectordeBottom( pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo );
#else
				//음극일 경우
				int nLocalRet2 = CImageProcess::FindTabLevel(pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
#endif
				// 21.12.28 Ahn Modify End
				//이미지 프로세싱 처리시간을 가져운다.
				double dTime = ctAna.WhatTimeIsIt_Double();

				//Tab Cutting 시간 정보를 로그 출력한다.
				CString strLog;
				strLog.Format(_T("TabCutting Time [%.2lf]msec, 전극경계Top[%d], Bottom[%d] BtmLevel[%d]"), dTime, nBndElectrode, nBneElectrodeBtm, nBtmLevel);
				AprData.SaveTactLog(strLog);

				//가져온 vector Tab 정보 크기 vector 사이즈 만큼 루프 돌면서 처리한다.
				for (int i = 0; i < nVecSize; i++) {
					//카운터 정보 객체 생성
					CCounterInfo cntInfo;

					//Tab 정보 객체 포인터 접근
					CTabInfo* pTabInfo = &vecTabInfo[i];

					//Tab 정보에서 Counter 정보를 가져온다.
					cntInfo = pCntQueueInCtrl->Pop();

					//Tab 정보에서 왼쪽 값
					int nLeft = pTabInfo->nTabLeft - pTabInfo->nLeft;
					//Tab 정보에서 오른쪽 값
					int nRight = pTabInfo->nRight - pTabInfo->nTabRight;

					// 22.05.03 Ahn Modify Start
					// 에러 번호
					int nErrorNo = 0;
					//if ((nLeft < (AprData.m_pRecipeInfo->TabCond.nRadiusW * 2)) || (nRight < (AprData.m_pRecipeInfo->TabCond.nRadiusW * 2))) {
					
					//왼쪽 값이 레시피 정보의 Tob  비율보다 작거나, 오를쪽 값이 레시피 Tab 비율 작으면
					if ((nLeft < AprData.m_pRecipeInfo->TabCond.nRadiusW) || (nRight < AprData.m_pRecipeInfo->TabCond.nRadiusW)) {

					// 22.05.03 Ahn Modify End
						//에러 플래그를 설정한다.
						pTabInfo->m_bErrorFlag = TRUE;
						//에러번호 1 세팅
						nErrorNo = 1;
					}

					//지금 Lot Data의 처리 에러이면서 시스템 첫 Tab 처리가 TRUE이면
					if ((AprData.m_NowLotData.m_bProcError == TRUE) && (AprData.m_System.m_bFirstTabDoNotProc == TRUE)) {

						//에러플레그 설정
						pTabInfo->m_bErrorFlag = TRUE;

						//지금  Lot Data 처리 에러는 FALSE로 변경한다.
						AprData.m_NowLotData.m_bProcError = FALSE;

						//에러 번호는 2 세팅
						nErrorNo = 2;
					}

					// 21.12.28 Ahn Add Start
					//에러 ALL이면
					if (bErrorAll == TRUE) {
						//에러 플레그 설정
						pTabInfo->m_bErrorFlag = TRUE;
						//에러 번호 3
						nErrorNo = 3;
					}

					// 21.12.28 Ahn Add End
					// 22.06.22 Ahn Add Start
					//레벨 값이 o보다 작으면
					if (nLevel <= 0 ) {
						//에러플레그 설정
						pTabInfo->m_bErrorFlag = TRUE;

						//에러번호 4 
						nErrorNo = 4;
					}
					// 22.06.22 Ahn Add End

					// 22.09.30 Ahn Add Start
					//레벨이 Top 프레임 정보 넓이에서 - 100 한 값 보다 크면
					if( nLevel >= (nWidth - 100 )){
						//에러 플래그 설정
						pTabInfo->m_bErrorFlag = TRUE;

						//에러 번호 5
						nErrorNo = 5;
					}

					// 22.09.30 Ahn Add End
					//프레임 정보 객체 생성
					CFrameInfo* pInfo;
					pInfo = new CFrameInfo;

					//Tab 정보에서 Fram 정보 값에 세팅한다.
					//Tab 정보에서 이미지 데이터 값을  세팅한다.
					pInfo->SetImgPtr(pTabInfo->pImgPtr);

					//높이값을 가져온다.
					pInfo->m_nHeight = pTabInfo->nImageLength;

					//Head 번호를 가져온다.
					pInfo->m_nHeadNo = pFrmInfo_Top->m_nHeadNo;

					//Fram Top 넓이 정보 세팅
					pInfo->m_nWidth = nWidth;

					// 22.11.18 Ahn Modify Start
					//pBtmInfo->m_nFrameCount = nFrameCountL;
					//Tab 정보에서 가져와  Frame 카운트 값을 세팅한다.
					pInfo->m_nFrameCount = pTabInfo->nFrameCount;

					// 22.11.18 Ahn Modify End
					//지금 Lot Data Tab Count를 Fram TabNo 번호 로 세팅
					pInfo->nTabNo = AprData.m_NowLotData.m_nTabCount;

					//Tab 정보에서 Tab Start 위치 In Frame을 Frame 정보에 세팅
					pInfo->nTabStartPosInFrame = pTabInfo->nTabStartPosInFrame;

					//Fram 정보에 Level 정보 세팅
					pInfo->m_nTabLevel = nLevel;

					//Fram 정보에 TopFrame 세팅
					pInfo->m_nInspMode = CFrameInfo::en_TopFrame;

					//Left Tab 세팅
					pInfo->m_nTabLeft = pTabInfo->nTabLeft;
					//Right Tab 세팅
					pInfo->m_nTabRight = pTabInfo->nTabRight;

					//카운터 정보에서 TabID를 가져온다
					pInfo->m_nTabId_CntBoard = cntInfo.nTabID;

					//에러 플래그 값을 Tab정보 에서 가져온다.
					pInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;

					//?
					pInfo->m_nBndElectrode = nBndElectrode;

					//디버그 로그출력
					//Tab 번호, Error값, Level 값, Tab Left 값, Tab Right 값, 길이, 카운터 ID  값 출력
					CString strMsg;
					// 22.05.03 Ahn Modify Start
					strMsg.Format(_T("TabNo[%d], Error[%d], nLevel[%d], nTabLeft[%d], nTabRight[%d], nLength[%d], CntID[%d] ")
						, pInfo->nTabNo, nErrorNo, pInfo->m_nTabLevel, pInfo->m_nTabLeft, pInfo->m_nTabRight, pInfo->m_nHeight, pInfo->m_nTabId_CntBoard);
					// 22.05.03 Ahn Modify End
					AprData.SaveDebugLog(strMsg);

					//프레임 정보
					//Bottom 이미지 정보 처리 객체 생성
					CFrameInfo* pBtmInfo;
					pBtmInfo = new CFrameInfo;

					//Tab 정보에서 Bottom 이미지 데이터를 가져온다.
					pBtmInfo->SetImgPtr(pTabInfo->pImgBtmPtr);

					//이미지 높이 정보
					pBtmInfo->m_nHeight = pTabInfo->nImageLength;

					//Fram Bottom정보에서 헤더번호를 가져와 세팅
					pBtmInfo->m_nHeadNo = pFrmInfo_Bottom->m_nHeadNo;

					//이미지 넓이
					pBtmInfo->m_nWidth = nWidth;

					// 22.11.18 Ahn Modify Start
					//pBtmInfo->m_nFrameCount = nFrameCountL;
					//Tab 정보에서 프레임 카운터를 가져온다.
					pBtmInfo->m_nFrameCount = pTabInfo->nFrameCount ;

					// 22.11.18 Ahn Modify End
					//Tab 번호를 가져온다.
					pBtmInfo->nTabNo = AprData.m_NowLotData.m_nTabCount;

					//Tab Start 위치 In Frame
					pBtmInfo->nTabStartPosInFrame = pTabInfo->nTabStartPosInFrame;

					//Bottom 레벨
					pBtmInfo->m_nTabLevel = nBtmLevel;

					//Bottom Frame 플래스 설정
					pBtmInfo->m_nInspMode = CFrameInfo::en_BottomFrame;

					//Tab ID 가져온다.
					pBtmInfo->m_nTabId_CntBoard = cntInfo.nTabID;

					//에러 플래그 설정
					pBtmInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;

					//?
					pBtmInfo->m_nBndElectrode = nBneElectrodeBtm;// 22.05.11 Ahn Add 

					// 22.02.22 Ahn Add Start
					//디버그 노이즈 카운터
#if defined( DEBUG_NOISE_COUNTERMEASURE )
					if (bMakeDummyBtm == TRUE) {
						pBtmInfo->m_bErrorFlag = pFrmInfo_Bottom->m_bErrorFlag;
					}
#endif
					// 22.02.22 Ahn Add End
					//Bottom 카메라 디버그 일때
#if defined( BOTTOM_CAMERA_DEBUG )
					pBtmInfo->m_bErrorFlag = TRUE;
#endif

					// 22.05.18 Ahn Add Start
					//Top 큐 스래드 갯수
					int nTopQueCnt = pThreadQue[CAM_POS_TOP]->GetSize();

					//Bottom 큐 스래드 갯수
					int nBtmQueCnt = pThreadQue[CAM_POS_BOTTOM]->GetSize();

					//Top 큐 갯수가 IMAGE_PROC_SKIP_COUNT 보다 크고, Bottom 큐 갯수 가 IMAGE_PROC_SKIP_COUNT 보다 크다
					if ((nTopQueCnt > IMAGE_PROC_SKIP_COUNT) && (nBtmQueCnt > IMAGE_PROC_SKIP_COUNT) 
						//Top/Bottom Size가 IMAGE_PROC_SKIP_COUNT 보다 클 때 
						|| ( (nSizeFrmL > IMAGE_PROC_SKIP_COUNT ) && (nSizeFrmR > IMAGE_PROC_SKIP_COUNT) ) ){
						//프레임 정보 에러 세팅
						pInfo->m_bErrorFlag = TRUE;
						//Bottom 에러 플래그 세팅
						pBtmInfo->m_bErrorFlag = TRUE ;
					}
					// 22.05.18 Ahn Add Start

					// 22.12.09 Ahn Add Start
					//시간 측정함수  세팅
					LARGE_INTEGER tmp;
					LARGE_INTEGER start;
					QueryPerformanceFrequency(&tmp);
					double dFrequency = (double)tmp.LowPart + ((double)tmp.HighPart * (double)0xffffffff);
					QueryPerformanceCounter(&start);

					//Fram 정보에 측정값 세팅
					pInfo->m_stTime = start;
					pInfo->m_dFrecuency = dFrequency;

					//Bottom 정보에 측정값 세팅
					pBtmInfo->m_stTime = start;
					pBtmInfo->m_dFrecuency = dFrequency;

					// 22.12.09 Ahn Add End
					//스래드 큐 카메라 Top에 정보 생성된 정보를 저장
					pThreadQue[CAM_POS_TOP]->push(pInfo) ;

					//스래드 큐 카메라 Bottom  정보에 생성되 정보를 저장
					pThreadQue[CAM_POS_BOTTOM]->push(pBtmInfo) ;

					//지금 Lot Data Tab Count를 증가 시킨다
					AprData.m_NowLotData.m_nTabCount++;
				}
				//Tab 정보 리스트를 삭제
				vecTabInfo.clear();
			}

			//Fram Top 정보 객체 삭제
			delete pFrmInfo_Top;
			pFrmInfo_Top = NULL;

			//Fram Bottom 정보 객체  삭제
			delete pFrmInfo_Bottom;
			pFrmInfo_Bottom = NULL;

			//처리에 걸린 시간을  가져온다.
			double dSecond = ctAna.WhatTimeIsIt_Double();
			//Tact Time에 세팅한다.
			AprData.SetTactTime_1(dSecond);
		}
		::Sleep(AprData.m_nSleep);
		}

	// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
	if (btLastBtmImg != NULL) {
		delete[] btLastBtmImg;
		btLastBtmImg = NULL;
	}
#endif

	if (RsvTabInfo.pImgPtr != NULL) {
		delete[] RsvTabInfo.pImgPtr;
	}
	if (RsvTabInfo.pImgBtmPtr != NULL) {
		delete[] RsvTabInfo.pImgBtmPtr;
	}

	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	return 0;
}

UINT CImageProcThread::CtrlThreadImgProc(LPVOID Param)
{
	//이미지 처리 스래드 객체
	CImageProcThread* pThis = (CImageProcThread*)Param;

	// 22.08.10 Ahn Delete Start
	//CQueueCtrl* pQueueCtrl = pThis->m_pParent->GetQueuePtr();
	// 22.08.10 Ahn Delete End
	
	// 22.05.31 Ahn Add Start
	//이미지 저장 큐 제어 객체를 가져온다.
	CImageSaveQueueCtrl* pImgSaveQueueCtrl = pThis->m_pParent->GetImageSaveQueuePtr();

	// 22.05.31 Ahn Add End
	// 22.12.09 Ahn Add Start
	//TacTime 데이터 제어 객체를 가져온다.
	CTacTimeDataCtrl* pTactCtrl = pThis->m_pParent->GetTactDataCtrlPtr();
	// 22.12.09 Ahn Add End

	//임시변수 : 결과 큐 컨트롤 저장 객체
	CQueueCtrl* pRsltQueueCtrl[GRABBER_COUNT];
	//Grabber 갯수 만큼 결과 큐 객체를 가져온다.
	for (int i = 0; i < GRABBER_COUNT; i++) {
		pRsltQueueCtrl[i] = pThis->m_pParent->GetResultPtr(i);
	}

	//CFrameRsltInfo* pFrmRsltInfo;
	//스래드 큐 제어 객체 : 카메라 댓수 만큼 생성
	CThreadQueueCtrl* pThdQue[MAX_CAMERA_NO];
	for (int i = 0; i < MAX_CAMERA_NO; i++) {
		pThdQue[i] = pThis->m_pParent->GetThreadQueuePtr(i);
	}

	//이미지 처리 스래드 유닛 Top(카메라 연결 장비)
	CImageProcThreadUnit* pUnitTop = NULL ;

	//이미지 처리 스래드 유닛 Bottom(카메라 연결 장비)
	CImageProcThreadUnit* pUnitBtm = NULL ;

	//비트맵 저장 여부
	BOOL bBitmapSave = FALSE;

	//Judge NG  여부
	BOOL bJudgeNG = FALSE;

	//Judge 구분
	char szJudge[2][4] = { "OK", "NG" };

	//Top Botton 구분
	char szPos[2][8] = { "TOP","BTM" };

	//Marking 여부
	BOOL bMarkingActive = FALSE;

	//clear 여부
	BOOL bClearFlag = FALSE;

	while (1) {
		if (pThis == NULL) {
			break;
		}
		if (pThis->m_bKill == TRUE) {
			break;
		}

		//카메라 스래드큐 Top Bottom 객체 검사
		if (!pThdQue[CAM_POS_TOP]->IsEmpty() && !pThdQue[CAM_POS_BOTTOM]->IsEmpty()) {

			//TOP 카메라 유닛 정보를 가져온다.
			pUnitTop = pThdQue[CAM_POS_TOP]->pop();
			//BOTTOM 유닛 정보를 가져온다.
			pUnitBtm = pThdQue[CAM_POS_BOTTOM]->pop();

			while (1) 
			{
				if (pThis->m_bKill == TRUE) {
					break;
				}

				// 22.12.09 Ahn Add Start
				//수행시간 시작 시간
				LARGE_INTEGER stTime ;
				// 22.12.09 Ahn Add End
				//Top 유닛 처리가 End 이면서 Bottom 처리가 End 이면
				if ( (pUnitTop->IsProcEnd() == TRUE) && (pUnitBtm->IsProcEnd() == TRUE) ){

					//유닛Top 처리 객체에서 Fram 결과 정보를 가져온다.
					CFrameRsltInfo *pTopInfo = pUnitTop->GetResultPtr();

					//유닛 Bottom 처리 객체에서 Fram 결과 정보를 가져온다.
					CFrameRsltInfo *pBtmInfo = pUnitBtm->GetResultPtr();

					//Bottom Judge 값
					int nBtmJudge = pBtmInfo->m_pTabRsltInfo->m_nJudge;		
					//Top Judge 값
					int nTopJudge = pTopInfo->m_pTabRsltInfo->m_nJudge;

					// 22.12.09 Ahn Add Start 
					//Top 결과 정보에서 시작 시간을 가져온다.
					stTime = pTopInfo->m_stTime ;
					// 22.12.09 Ahn Add End

					// NG Tab 보고
					if ((nTopJudge == JUDGE_NG) || (nBtmJudge == JUDGE_NG)) {

						//Alarm Code 초기화
						WORD wAlarmCode = 0x0000;

						//Judge NG 플래그 초기화
						bJudgeNG = TRUE;

						//Top 가 NG이면
						if (nTopJudge == JUDGE_NG) {
							//알람 코드 
							wAlarmCode = pTopInfo->m_pTabRsltInfo->m_wNgReason;
							//지금 Lot Data Top NG 값을 증가 시킨다.
							AprData.m_NowLotData.m_nTopNG++;
						}

						//Bottom Judge NG 이면
						if (nBtmJudge == JUDGE_NG) {

							//알람코드 | 연산 추가
							wAlarmCode |= pBtmInfo->m_pTabRsltInfo->m_wNgReason;

							//지금 Lot Data  Bottom NG 값 증가 시킨다.
							AprData.m_NowLotData.m_nBottomNG++ ;
						}

						//Tab Count  NG 증가
						AprData.m_NowLotData.m_nTabCountNG++ ;
		
						// 22.08.09 Ahn Add Start
						//지금 Lot 데이터 Contiue 카운트 증가
						AprData.m_NowLotData.m_nContinueCount++ ;

						//Tab Judge 객체
						CTabJudge tab ;
						tab.nJudge = JUDGE_NG ;
						tab.nReason = wAlarmCode ;
						tab.nTabNo = pTopInfo->nTabNo ;

						// 22.08.10 Ahn Modify Start
						//int nSecterNgCount = AprData.m_NowLotData.m_secNgJudge.AddNgTab(tab, AprData.m_pRecipeInfo->nSectorCount) ;
						//if ( (AprData.m_NowLotData.m_nContinueCount >= AprData.m_pRecipeInfo->nContinousNgCount)
						//	&& (AprData.m_pRecipeInfo->nContinousNgCount > 2) ) {
						if ((AprData.m_NowLotData.m_nContinueCount >= AprData.m_nCoutinuouCount)
							// 22.09.22 Ahn Modify Start
							&& (AprData.m_nCoutinuouCount >= 2)) {
							// 22.09.22 Ahn Modify End
						// 22.08.10 Ahn Modify End
							wAlarmCode |= CSigProc::en_Alarm_ContinueNg;
							CString strMessage;
							strMessage.Format(_T("연속 NG Alarm 발생. %d Tab연속 NG 발생"), AprData.m_NowLotData.m_nContinueCount);
							AprData.m_ErrStatus.SetError(CErrorStatus::en_ContinuousNg, strMessage);
						}

						// 22.08.10 Ahn Modify End
						//if ((AprData.m_pRecipeInfo->nSectorCount > 0) && (AprData.m_pRecipeInfo->nAlarmCount > 0)) {
						//	if (nSecterNgCount >= AprData.m_pRecipeInfo->nAlarmCount) {
						int nSecterNgCount = AprData.m_NowLotData.m_secNgJudge.AddNgTab(tab, AprData.m_nSectorBaseCount);
						if ((AprData.m_nSectorNgCount > 0) && (AprData.m_nSectorBaseCount> 0)) {
							// 22.09.22 Ahn Modify Start
							if (nSecterNgCount >= AprData.m_nSectorNgCount) {
							// 22.09.22 Ahn Modify End
						// 22.08.10 Ahn Modify End
								wAlarmCode |= CSigProc::en_Alarm_SectorNg;
								CString strMessage;
								strMessage.Format(_T("구간 NG Alarm 발생. %d / %d Tab NG 발생"), nSecterNgCount, AprData.m_pRecipeInfo->nSectorCount);
								AprData.m_ErrStatus.SetError(CErrorStatus::en_ContinuousNg, strMessage);
							}
						}
						// 22.08.09 Ahn Add End

						if (AprData.m_System.m_bEnableNgStop == TRUE) {
							if (AprData.m_pRecipeInfo->bNgStop == TRUE) {
								bClearFlag = TRUE;; // 22.03.03 Ahn Add 
								CSigProc* pSigProc = theApp.m_pSigProc;
								pSigProc->WriteAlarmCode(wAlarmCode);
								// 22.08.09 Ahn Add Start
								CString strLog;
								strLog.Format(_T("!!!! NG_STOP Signal Output [0x%x]!!!!"), wAlarmCode) ;
								AprData.SaveMemoryLog( strLog ) ;
								// 22.08.09 Ahn Add End
								// 22.12.12 Ahn Add Start
								int nId, nJudge, nCode ;
								nId = pTopInfo->m_nTabId_CntBoard ;
								nJudge = tab.nJudge ;
								nCode = wAlarmCode ;
								pSigProc->ReportJudge(nId, nJudge, nCode);
								// 22.12.12 Ahn Add End
							}
						}
					} else {
						AprData.m_NowLotData.m_nTabCountOK++ ;
						AprData.m_NowLotData.m_nContinueCount = 0 ; // 22.08.09 Ahn Add
						AprData.m_NowLotData.m_secNgJudge.AddOkTab(pTopInfo->nTabNo, AprData.m_pRecipeInfo->nSectorCount);
					}
					// 결과 Queue에 보냄

					// Counter 신호 출력
					WORD wOutPut;	
					CString strMarking = _T("OFF");
					{
						CAppDIO dio;
						int nMarkSel1 = 0 ;
						int nMarkSel2 = 0 ;

						// 22.07.19 Ahn Modify Start
						GetMarkingFlag(AprData.m_pRecipeInfo, nTopJudge, nBtmJudge, pTopInfo->m_pTabRsltInfo->m_wNgReason, pBtmInfo->m_pTabRsltInfo->m_wNgReason, nMarkSel1, nMarkSel2 );
						// 22.07.19 Ahn Modify End

						CSigProc* pSigProc = theApp.m_pSigProc;
						bMarkingActive = pSigProc->SigInInkMarkingActive(); 				
						if( (AprData.m_System.m_bChkEnableMarker == FALSE) || ( bMarkingActive == FALSE ) ) {
							nMarkSel1 = 0;
							nMarkSel2 = 0; 
						}

						wOutPut = CImageProcThread::GetCounterSignal(pTopInfo->m_nTabId_CntBoard, nTopJudge, nBtmJudge, nMarkSel1, nMarkSel2);
						dio.OutputWord(wOutPut);
						Sleep(20);
						dio.OutputBit(CAppDIO::eOut_PULSE, TRUE);
						CString strMsg;
						strMsg.Format(_T("Output ID[%d]_OutPutValue[0x%x]_TabNo[%d]"), pTopInfo->m_nTabId_CntBoard, wOutPut, pTopInfo->nTabNo ); // 22.04.06 Ahn Modify
						AprData.SaveMemoryLog(strMsg);

						if ( (wOutPut & CAppDIO::eOut_MARK_SEL_01 ) || (wOutPut & CAppDIO::eOut_MARK_SEL_02) ) {
							strMarking = _T("ON");
							pTopInfo->m_pTabRsltInfo->m_bMarkingFlag = TRUE;
							pBtmInfo->m_pTabRsltInfo->m_bMarkingFlag = TRUE;
							AprData.m_NowLotData.m_nMarkingCount++; // 22.06.29 Ahn Add 
						} else {
							strMarking = _T("OFF");
							pTopInfo->m_pTabRsltInfo->m_bMarkingFlag = FALSE;
							pBtmInfo->m_pTabRsltInfo->m_bMarkingFlag = FALSE;
						}
					}

					{ // CSV 파일 작성
						CString strCsvFileName;
						CString strFilePath;
						strFilePath.Format(_T("%s\\"), AprData.m_strNowCsvPath);

						strCsvFileName.Format(_T("%s.csv"), AprData.m_NowLotData.m_strLotNo);
						CString strResult;
						SYSTEMTIME* pSysTime;
						pSysTime = &(pTopInfo->m_pTabRsltInfo->sysTime);	

						int nSurfaceNgCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][RANK_NG] +pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][JUDGE_NG];
						int nFoilExpNgCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][RANK_NG] +pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][JUDGE_NG]
							+ pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][RANK_NG] +pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][JUDGE_NG];
						double 	dTopMaxSize = pTopInfo->m_pTabRsltInfo->m_dMaxSizeDef;
						double 	dBtmMaxSize = pBtmInfo->m_pTabRsltInfo->m_dMaxSizeDef;

						int nSurfaceGrayCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][JUDGE_GRAY] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_SURFACE][JUDGE_GRAY];
						int nFoilExpGrayCnt = pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][JUDGE_GRAY] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP][JUDGE_GRAY]
							+ pBtmInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][JUDGE_GRAY] + pTopInfo->m_pTabRsltInfo->m_nCount[TYPE_FOILEXP_OUT][JUDGE_GRAY];

						CString strMarking;
						CString strMarkReason = _T("") ;
						if (pBtmInfo->m_pTabRsltInfo->m_bMarkingFlag || pTopInfo->m_pTabRsltInfo->m_bMarkingFlag) {
							strMarking.Format(_T("○"));
						}
						else {
							strMarking.Format(_T("Χ"));
						}
						CString strTime;
						CString strJudge = _T("OK") ;
						CString strBtmJudge = _T("OK");
						CString strTopJudge = _T("OK");
						strTime.Format(_T("%02d:%02d:%02d(%03d)"), pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond, pSysTime->wMilliseconds );
						if (pTopInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) {
							strJudge = _T("NG");
							strTopJudge = _T("NG");
						}
						if (pBtmInfo->m_pTabRsltInfo->m_nJudge == JUDGE_NG) {
							strJudge = _T("NG");
							strBtmJudge = _T("NG");
						}
						// LotID, /*Total Count*/, Cell_No, Time, Pos(Top,Bottom),Top Judge,Bottom Judge,Surface Exposure, DefR, DefTheta, Top DefectSize, BTM Def Size
						//,  Top Surface max Size, Btm Surface Max Size, InkMarking, InkMarkingReason
						// 23.01.06 Ahn Modify Start
						//strResult.Format(_T("%s,%d,%s,%s,%s,%s,%d,%d,%.2lf,%.2lf,%s,%s\r\n")
						strResult.Format(_T("%s,%d,%s,%s,%s,%s,%d,%d,%.2lf,%.2lf,%s,%s,%d,%d\r\n")
							// 23.01.06 Ahn Modify End
							, AprData.m_NowLotData.m_strLotNo
							, pTopInfo->nTabNo + 1
							, strTime
							, strJudge
							, strTopJudge
							, strBtmJudge
							, nSurfaceNgCnt
							, nFoilExpNgCnt
							, dTopMaxSize
							, dBtmMaxSize
							, strMarking
							, strMarkReason
							// 23.01.06 Ahn Add Start
							, nSurfaceGrayCnt
							, nFoilExpGrayCnt
							// 23.01.06 Ahn Add End
						);
						CWin32File::TextSave1Line(strFilePath, strCsvFileName, strResult, _T("at"), FALSE) ;
					}

					// 22.05.31 Ahn Add Start - Image Save Thread 
					for (int i = 0; i < MAX_CAMERA_NO; i++) {
						CFrameRsltInfo* pFrmRsltInfo;
						if (i == CAM_POS_TOP) {
							pFrmRsltInfo = pTopInfo;
						}
						else {
							pFrmRsltInfo = pBtmInfo;
						}
						// 22.08.09 Ahn Move Start
						int nImgSize = pFrmRsltInfo->m_nWidth * pFrmRsltInfo->m_nHeight;
						// 22.08.09 Ahn Move End
						if (pFrmRsltInfo->m_pTabRsltInfo->m_bImageFlag == TRUE) {
							if (pImgSaveQueueCtrl->GetSize() < MAX_SAVE_IMAGE_QUEUE) {
								pFrmRsltInfo->m_nWidth;
								CImgSaveInfo *pSaveInfo = new CImgSaveInfo ;
								BYTE* pImgSavePtr;
								pImgSavePtr = new BYTE[nImgSize];
								memcpy(pImgSavePtr, pFrmRsltInfo->GetImagePtr(), sizeof(BYTE)* nImgSize);
								pSaveInfo->SetImgPtr(pImgSavePtr, pFrmRsltInfo->m_nWidth, pFrmRsltInfo->m_nHeight);
								pSaveInfo->m_strSavePath.Format(_T("%s\\%s"), pFrmRsltInfo->m_pTabRsltInfo->m_chImagePath, pFrmRsltInfo->m_pTabRsltInfo->m_chImageFile);
								pImgSaveQueueCtrl->PushBack(pSaveInfo);
							}
						}
					}
					// 22.05.31 Ahn Add End

					pRsltQueueCtrl[CAM_POS_TOP]->PushBack((CFrameInfo*)pTopInfo);
					pRsltQueueCtrl[CAM_POS_BOTTOM]->PushBack((CFrameInfo*)pBtmInfo);

					// 22.12.09 Ahn Add Start
					double dTactTime = GetDiffTime(pTopInfo->m_stTime, pTopInfo->m_dFrecuency) ;
					CTactTimeData data;
					data.nCellNo = pTopInfo->m_pTabRsltInfo->m_nTabNo ;
					data.dTactTime = dTactTime ;
					pTactCtrl->AddNewTactData(data) ;
					// 22.12.09 Ahn Add End

					delete pUnitTop;
					delete pUnitBtm;

					AprData.m_NowLotData.m_ctLastAcqTime = CTime::GetCurrentTime();

					// 22.04.06 Ahn Modify Start
					Sleep(10);
					CAppDIO dio;
					dio.OutputBit(CAppDIO::eOut_PULSE, FALSE);
					// 22.02.17 Ahn Modify End

					// 22.02.17 Ahn Modify Start
					if (bClearFlag == TRUE){
						CSigProc *pSigProc = theApp.m_pSigProc;
						WORD wResetCode = 0x00;
						pSigProc->WriteAlarmCode(wResetCode);

						AprData.SaveMemoryLog(_T("------ NG_STOP Signal OFF ------"));
						bJudgeNG = FALSE;
						bClearFlag = FALSE;// 22.03.28 Ahn Add
						Sleep(5);
					}

					break;
				}
				//1 밀리 세컨트 잠들다.(스래드 제어권 넘김)
				Sleep(AprData.m_nSleep);
			}
		}
		//1 밀리 세컨트 잠들다(스래드 제어권 넘기기)
		Sleep(AprData.m_nSleep);
	}
	// 스래드 종료
	AfxEndThread(0);
	//스래드 Kill 플래그 FALSE 세팅
	pThis->m_bKill = FALSE;

	return 0;
}

// 22.12.09 Ahn Add Start
double CImageProcThread::GetDiffTime(LARGE_INTEGER stTime, double dFrequency)
{
	LARGE_INTEGER edTime;
	QueryPerformanceCounter(&edTime);

	double	dv0, dv1;
	dv0 = (double)stTime.LowPart + ((double)stTime.HighPart * (double)0xffffffff);
	dv1 = (double)edTime.LowPart + ((double)edTime.HighPart * (double)0xffffffff);
	double	dtimev;
	dtimev = (dv1 - dv0) / dFrequency * (double)1000.0;
	return (dtimev);
}
// 22.12.09 Ahn Add End


WORD CImageProcThread::GetCounterSignal(int nTabId, int nJudge1, int nJudge2, int nMarkSel1, int nMarkSel2)
{
	WORD wOutput = 0x00;

	// 22.01.11 Ahn Add Start
	// 마킹 테스트용 모든 탭 마킹 신호 출력.
	if (AprData.m_System.m_bMarkingAllTab == TRUE) {
		nJudge1 = JUDGE_NG;
		nJudge2 = JUDGE_NG;
		// 22.08.11 Ahn Add Start
		nMarkSel1 = 1;
		nMarkSel2 = 0;
		// 22.08.11 Ahn Add Start
	}
	// 22.01.11 Ahn Add End

	if ( ( nJudge1 == JUDGE_NG )  || ( nJudge2 == JUDGE_NG ) ) {
		wOutput |= CAppDIO::eOut_TAB_JUDGE_SURFACE;	
		//wOutput |= CAppDIO::eOut_TAB_JUDGE_SIZE ; // 22.08.11 Ahn Test
		//22.07.26 Ahn Modify Start
		//int nMarkSel = (nMarkSel1 || nMarkSel2 );
		int nMarkSel = ( nMarkSel1 | (nMarkSel2 << 1));
		//22.07.26 Ahn Modify End
		switch (nMarkSel) {
		case	0:
			break;
		case	1:
			wOutput |= CAppDIO::eOut_MARK_SEL_01;
			break;
		case	2 :
		case	3 :
			wOutput |= CAppDIO::eOut_MARK_SEL_01;
			wOutput |= CAppDIO::eOut_MARK_SEL_02;
			break;
		}
	}

	//wOutput |= CAppDIO::eOut_PULSE; // 22.04.07 Ahn Delete 
	wOutput |= (nTabId << 2) & CAppDIO::eInOut_ID_Mask; // 3F = 111 11100
	return wOutput;
}

// 22.07.19 Ahn Modify Start
int CImageProcThread::GetMarkingFlag(CRecipeInfo* pRecipeInfo, int nTopJudge, int nBtmJudge, WORD wTopReson, WORD wBtmReson, int& nMarkSel1, int& nMarkSel2 )
{
	int nRet = 0 ; 
	ASSERT(pRecipeInfo);

	// 판정결과 적용
	pRecipeInfo = AprData.m_pRecipeInfo;
	if ((nTopJudge == JUDGE_NG) || (nBtmJudge == JUDGE_NG)) {
		WORD wReason = wTopReson | wBtmReson ;
		if ( ( wReason & CTabRsltBase::en_Reason_Surface_Top ) || ( wReason & CTabRsltBase::en_Reason_Surface_Btm ) ) {
			if (pRecipeInfo->nMarkingUse[en_ModeSurface] == TRUE) {
				if (pRecipeInfo->nMarkingType[en_ModeSurface] == 0) {
					nMarkSel1 = 1;
				} else {
					nMarkSel1 = 1;
					nMarkSel2 = 1;
				}
			}
		}
		if ((wReason & CTabRsltBase::en_Reason_FoilExp_Top) || (wReason & CTabRsltBase::en_Reason_FoilExp_Btm)) {
			if (pRecipeInfo->nMarkingUse[en_ModeFoilExp] == TRUE) {
				if (pRecipeInfo->nMarkingType[en_ModeFoilExp] == 0) {
					nMarkSel1 = 1;
				} else {
					nMarkSel1 = 1;
					nMarkSel2 = 1;
				}
			}
		}
		if ((wReason & CTabRsltBase::en_Reason_FoilExpOut_Top) || (wReason & CTabRsltBase::en_Reason_FoilExp_Btm)) {
			if (pRecipeInfo->nMarkingUse[en_ModeFoilExpOut] == TRUE) {
				if (pRecipeInfo->nMarkingType[en_ModeFoilExpOut] == 0) {
					nMarkSel1 = 1;
				}
				else {
					nMarkSel1 = 1;
					nMarkSel2 = 1;
				}
			}
		}
		//}
	}

	nRet = ( nMarkSel1 | ( nMarkSel2 << 1 ) );
	return nRet;
}
// 22.07.19 Ahn Modify End
