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
		//�̹��� Cut ������ ����
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


// Queue���� �޾ƿ� Frame Image�� Tab ���� �����ؼ� ó���� Queue�� ���� �ϴ� Thread
UINT CImageProcThread::CtrlThreadImgCuttingTab(LPVOID Param)
{
	//�̹��� ó�� ������ ��ü �ֻ��� ������Ʈ ������
	CImageProcThread* pThis = (CImageProcThread*)Param;

	//Fram Top ť ������Ʈ ������
	CQueueCtrl* pQueueFrame_Top = pThis->m_pParent->GetQueueFrmPtr(0);

	//Fram Bottom ť ������Ʈ ������
	CQueueCtrl* pQueueFrame_Bottom = pThis->m_pParent->GetQueueFrmPtr(1);

	//Counterť ������Ʈ ������
	CCounterQueueCtrl* pCntQueueInCtrl = pThis->m_pParent->GetCounterQueInPtr(); // �� ī���� �� ť

	//������ť ���� ��ü Top/Bottom  ��ü�� ������ �ӽ� ���� ����
	CThreadQueueCtrl* pThreadQue[MAX_CAMERA_NO];

	//������ť ���� ��ü���� Top�� ���� ������ť ��ü�� �����´�.
	pThreadQue[CAM_POS_TOP] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_TOP);

	//������ť ������ü���� Bottom�� ���� ������ť ��ü�� �����´�.
	pThreadQue[CAM_POS_BOTTOM] = pThis->m_pParent->GetThreadQueuePtr(CAM_POS_BOTTOM);

	BOOL bReserved = FALSE; // ũ�Ⱑ �۾Ƽ� ������ ���� �κ��� ���� ���� �̹��� �޾Ƽ� ó�� �� �������� ���� Flag.
	//����� ������ ��ȣ
	BOOL bReservFrmNo = -1;
	//������ ����
	int nLastLengh = 0;
	//������ ����
	int nLastWidth = 0;

	//���� Tab ���� ����
	CTabInfo reservTabInfo;
	CString strMsg;
	CString strTemp;
	CTabInfo RsvTabInfo;

	//ó�� �÷��� �ʱ�ȭ
	int nFindPos = 0;
	BOOL bHeadFlag = FALSE;
	BOOL bTailFlag = FALSE;
	BOOL bStopFlag = FALSE;

	// 22.02.22 Ahn Add Start
#if defined( DEBUG_NOISE_COUNTERMEASURE )
	BYTE* btLastBtmImg = NULL;
#endif
	// 22.02.22 Ahn Add End

	// 22.04.06 Ahn Add Start - ù �� ����
	//First Tab�� ó���ϴ°��� TRUE �� �ʱ�ȭ�ϳ�.
	BOOL bFirstTab = TRUE;
	// 22.04.06 Ahn Add End

	while (1) {
		if (pThis == NULL) {
			break;
		}
		//�̹��� ó�� �ֻ��� ��ü�� Kill�̸�
		if (pThis->m_bKill == TRUE) {
			//Bottom ī�޶� ����װ� �ƴϰ��
#if !defined( BOTTOM_CAMERA_DEBUG )
			if ((pQueueFrame_Top != NULL) && (pQueueFrame_Bottom != NULL)) {
				if ((pQueueFrame_Top->GetSize() <= 0) || (pQueueFrame_Bottom->GetSize() <= 0)) {
					break;
				}
			}
			//Bottom ī�޶� ����� �� ���
#else
			if ((pQueueFrame_Top != NULL)) {
				if ((pQueueFrame_Top->GetSize() <= 0)) {
					break;
				}
			}
#endif
		}

		//ũ��
		//Frame ť top ũ��
		int nSizeFrmL = pQueueFrame_Top->GetSize();
		//Frame ť bottom ũ��
		int nSizeFrmR = pQueueFrame_Bottom->GetSize();

		//Head, Tail �÷��� ����
		//Frame ť top�� ��ü�� ������ Head�� TRUE
		bHeadFlag = !pQueueFrame_Top->IsEmpty();
		//Frame ť bottom ��ü�� ������ Tail TRUE
		bTailFlag = !pQueueFrame_Bottom->IsEmpty();

		//Top - Bottom ������ ���� ���� FRAME_ACQ_ERROR_CHK_CNT �̻��ϰ��
		if (abs(nSizeFrmL - nSizeFrmR) > FRAME_ACQ_ERROR_CHK_CNT) {
			// ���� ó�� 
		//	pThis->SetFameSizeError(); // 
		
			//.������ ������ �̻� �α׸� ����Ѵ�.
			CString strErrMsg;
			strErrMsg.Format(_T("������ ������ �̻� : Top [%d], Bottom[%d], �˻� ���� [%d], ���� ó��!!!!"), nSizeFrmL, nSizeFrmR , pThis->m_pParent->IsInspection() );
		//	AprData.SaveErrorLog(strErrMsg);
			AprData.m_ErrStatus.SetError(CErrorStatus::en_CameraError, strErrMsg);
		}

		//Bottom ī�޶� ����� �϶�
#if defined( BOTTOM_CAMERA_DEBUG )
		bTailFlag = TRUE;
#endif
		// 22.02.22 Ahn Add Start
		//����� ������ī���� 
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
		//����� ������ ī����
#if defined( DEBUG_NOISE_COUNTERMEASURE )
		if ((bHeadFlag && bTailFlag) || (bMakeDummyBtm == TRUE)) {
#else
		if (bHeadFlag && bTailFlag) {
#endif
			// 22.02.22 Ahn Modify End
			//Frame To ������ü�� �����´�.
			CFrameInfo* pFrmInfo_Top = pQueueFrame_Top->Pop();

			//Top ���������� ���� ���� ������ �����´�.
			int nHeight = pFrmInfo_Top->m_nHeight;
			int nFrmWIdth = pFrmInfo_Top->m_nWidth;
			int nWidth = nFrmWIdth;

			// 22.02.22 Ahn Add Start
			//����� ������ ī����
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
			//BOTTOM ī�޶� ����װ� �ƴ� ���
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

			//Top ������ ī��Ʈ 
			int nFrameCountL = pFrmInfo_Top->m_nFrameCount;
			//Bottom ������ ī��Ʈ
			int nFrameCountR = pFrmInfo_Bottom->m_nFrameCount;

			// 22.11.30 Ahn Modify Start
			//�۷ι� Lot Data Top ī���� ����
			//������ ? :
			AprData.m_NowLotData.m_nFrameCount = pFrmInfo_Top->m_nFrameCount ;
			// 22.11.30 Ahn Modify End

			//Head �̹��� ������ : Top ������ �������� �����´�.
			BYTE* pHeadPtr = pFrmInfo_Top->GetImagePtr();
			// Tail �̹��� ������ : Bottom ������ �������� �����´�.
			BYTE* pTailPtr = pFrmInfo_Bottom->GetImagePtr();

			// 22.02.22 Ahn Add Start
			//����� ������ ī����
#if defined( DEBUG_NOISE_COUNTERMEASURE )
			if (btLastBtmImg == NULL) {
				btLastBtmImg = new BYTE[nWidth * nHeight];
			}
			memcpy(btLastBtmImg, pTailPtr, sizeof(BYTE) * nWidth * nHeight);
#endif
			// 22.02.22 Ahn Add End

			//ó�� �ð� üũ�� ���� ��ü ���� �� �ʱ�ȭ
			CTimeAnalyzer ctAna;
			ctAna.Clear();
			//ó���ð� üũ ����
			ctAna.StopWatchStart();

			// Tab���� �߶� ����
			// Projection ��� 
			{
				//ó�� �ð� üũ�� ���� ��ü ���� �� üũ ����
				CTimeAnalyzer ctAna;
				ctAna.StopWatchStart();

				// 22.05.09 Ahn Add Start
				//���� ���� ���� �ʱ�ȭ
				int nBndElectrode = 0;
				int nBneElectrodeBtm = 0;

				//����� ��� �̹��� ���μ��� ó���Ѵ�. GetBoundaryOfElectorde
#if defined( ANODE_MODE )

				nBndElectrode = CImageProcess::GetBoundaryOfElectorde(pHeadPtr, nWidth, nHeight, AprData.m_pRecipeInfo, CImageProcess::en_FindFromLeft);
#endif
				// 22.05.09 Ahn Add End
				//�̹��� ���μ��� vector Tab ���� ��ü ����
				CImageProcess::_VEC_TAB_INFO vecTabInfo;

				//���� ���� �ʱ�ȭ
				int nLevel = 0;

				//Btm ���� ���� �ʱ�ȭ
				int nBtmLevel = 0;

				//Tab ��ġ �� ����� ��� �߰�
				int nTabFindPos = nBndElectrode + AprData.m_pRecipeInfo->TabCond.nCeramicHeight ;

				// 22.11.18 Ahn Modify Start
				//int nLocalRet = CImageProcess::DivisionTab_FromImageToTabInfo(pHeadPtr, pTailPtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo );
				
				//Tab ���� �̹��� �������� Tab ����
				int nLocalRet = CImageProcess::DivisionTab_FromImageToTabInfo( pHeadPtr, pTailPtr, nWidth, nHeight, nTabFindPos, &nLevel, *AprData.m_pRecipeInfo, &RsvTabInfo, &vecTabInfo, nFrameCountL);
				// 22.11.18 Ahn Modify End
				// 21.12.28 Ahn Modify Start
				//int nLocalRet2 = CImageProcess::FindTabLevel(pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);

				//������ vector Tab ���� ũ�� vector ������
				int nVecSize = (int)vecTabInfo.size();

				//Error �÷��� �ʱ�ȭ
				BOOL bErrorAll = FALSE;

				//Tab ���� ����� ���ٸ�
				if (nVecSize <= 0) {
					//Tab ���� ã�� ���� �α� ���
					AprData.SaveDebugLog(_T("!!!!Tab Find Faile!!!!"));
					// ���� ���� 
					bErrorAll = TRUE;
				}
				//��尡 ����� ���
#if defined( ANODE_MODE )
				nBneElectrodeBtm = CImageProcess::GetBoundaryOfElectordeBottom( pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo );
#else
				//������ ���
				int nLocalRet2 = CImageProcess::FindTabLevel(pTailPtr, nWidth, nHeight, &nBtmLevel, AprData.m_pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
#endif
				// 21.12.28 Ahn Modify End
				//�̹��� ���μ��� ó���ð��� �������.
				double dTime = ctAna.WhatTimeIsIt_Double();

				//Tab Cutting �ð� ������ �α� ����Ѵ�.
				CString strLog;
				strLog.Format(_T("TabCutting Time [%.2lf]msec, ���ذ��Top[%d], Bottom[%d] BtmLevel[%d]"), dTime, nBndElectrode, nBneElectrodeBtm, nBtmLevel);
				AprData.SaveTactLog(strLog);

				//������ vector Tab ���� ũ�� vector ������ ��ŭ ���� ���鼭 ó���Ѵ�.
				for (int i = 0; i < nVecSize; i++) {
					//ī���� ���� ��ü ����
					CCounterInfo cntInfo;

					//Tab ���� ��ü ������ ����
					CTabInfo* pTabInfo = &vecTabInfo[i];

					//Tab �������� Counter ������ �����´�.
					cntInfo = pCntQueueInCtrl->Pop();

					//Tab �������� ���� ��
					int nLeft = pTabInfo->nTabLeft - pTabInfo->nLeft;
					//Tab �������� ������ ��
					int nRight = pTabInfo->nRight - pTabInfo->nTabRight;

					// 22.05.03 Ahn Modify Start
					// ���� ��ȣ
					int nErrorNo = 0;
					//if ((nLeft < (AprData.m_pRecipeInfo->TabCond.nRadiusW * 2)) || (nRight < (AprData.m_pRecipeInfo->TabCond.nRadiusW * 2))) {
					
					//���� ���� ������ ������ Tob  �������� �۰ų�, ������ ���� ������ Tab ���� ������
					if ((nLeft < AprData.m_pRecipeInfo->TabCond.nRadiusW) || (nRight < AprData.m_pRecipeInfo->TabCond.nRadiusW)) {

					// 22.05.03 Ahn Modify End
						//���� �÷��׸� �����Ѵ�.
						pTabInfo->m_bErrorFlag = TRUE;
						//������ȣ 1 ����
						nErrorNo = 1;
					}

					//���� Lot Data�� ó�� �����̸鼭 �ý��� ù Tab ó���� TRUE�̸�
					if ((AprData.m_NowLotData.m_bProcError == TRUE) && (AprData.m_System.m_bFirstTabDoNotProc == TRUE)) {

						//�����÷��� ����
						pTabInfo->m_bErrorFlag = TRUE;

						//����  Lot Data ó�� ������ FALSE�� �����Ѵ�.
						AprData.m_NowLotData.m_bProcError = FALSE;

						//���� ��ȣ�� 2 ����
						nErrorNo = 2;
					}

					// 21.12.28 Ahn Add Start
					//���� ALL�̸�
					if (bErrorAll == TRUE) {
						//���� �÷��� ����
						pTabInfo->m_bErrorFlag = TRUE;
						//���� ��ȣ 3
						nErrorNo = 3;
					}

					// 21.12.28 Ahn Add End
					// 22.06.22 Ahn Add Start
					//���� ���� o���� ������
					if (nLevel <= 0 ) {
						//�����÷��� ����
						pTabInfo->m_bErrorFlag = TRUE;

						//������ȣ 4 
						nErrorNo = 4;
					}
					// 22.06.22 Ahn Add End

					// 22.09.30 Ahn Add Start
					//������ Top ������ ���� ���̿��� - 100 �� �� ���� ũ��
					if( nLevel >= (nWidth - 100 )){
						//���� �÷��� ����
						pTabInfo->m_bErrorFlag = TRUE;

						//���� ��ȣ 5
						nErrorNo = 5;
					}

					// 22.09.30 Ahn Add End
					//������ ���� ��ü ����
					CFrameInfo* pInfo;
					pInfo = new CFrameInfo;

					//Tab �������� Fram ���� ���� �����Ѵ�.
					//Tab �������� �̹��� ������ ����  �����Ѵ�.
					pInfo->SetImgPtr(pTabInfo->pImgPtr);

					//���̰��� �����´�.
					pInfo->m_nHeight = pTabInfo->nImageLength;

					//Head ��ȣ�� �����´�.
					pInfo->m_nHeadNo = pFrmInfo_Top->m_nHeadNo;

					//Fram Top ���� ���� ����
					pInfo->m_nWidth = nWidth;

					// 22.11.18 Ahn Modify Start
					//pBtmInfo->m_nFrameCount = nFrameCountL;
					//Tab �������� ������  Frame ī��Ʈ ���� �����Ѵ�.
					pInfo->m_nFrameCount = pTabInfo->nFrameCount;

					// 22.11.18 Ahn Modify End
					//���� Lot Data Tab Count�� Fram TabNo ��ȣ �� ����
					pInfo->nTabNo = AprData.m_NowLotData.m_nTabCount;

					//Tab �������� Tab Start ��ġ In Frame�� Frame ������ ����
					pInfo->nTabStartPosInFrame = pTabInfo->nTabStartPosInFrame;

					//Fram ������ Level ���� ����
					pInfo->m_nTabLevel = nLevel;

					//Fram ������ TopFrame ����
					pInfo->m_nInspMode = CFrameInfo::en_TopFrame;

					//Left Tab ����
					pInfo->m_nTabLeft = pTabInfo->nTabLeft;
					//Right Tab ����
					pInfo->m_nTabRight = pTabInfo->nTabRight;

					//ī���� �������� TabID�� �����´�
					pInfo->m_nTabId_CntBoard = cntInfo.nTabID;

					//���� �÷��� ���� Tab���� ���� �����´�.
					pInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;

					//?
					pInfo->m_nBndElectrode = nBndElectrode;

					//����� �α����
					//Tab ��ȣ, Error��, Level ��, Tab Left ��, Tab Right ��, ����, ī���� ID  �� ���
					CString strMsg;
					// 22.05.03 Ahn Modify Start
					strMsg.Format(_T("TabNo[%d], Error[%d], nLevel[%d], nTabLeft[%d], nTabRight[%d], nLength[%d], CntID[%d] ")
						, pInfo->nTabNo, nErrorNo, pInfo->m_nTabLevel, pInfo->m_nTabLeft, pInfo->m_nTabRight, pInfo->m_nHeight, pInfo->m_nTabId_CntBoard);
					// 22.05.03 Ahn Modify End
					AprData.SaveDebugLog(strMsg);

					//������ ����
					//Bottom �̹��� ���� ó�� ��ü ����
					CFrameInfo* pBtmInfo;
					pBtmInfo = new CFrameInfo;

					//Tab �������� Bottom �̹��� �����͸� �����´�.
					pBtmInfo->SetImgPtr(pTabInfo->pImgBtmPtr);

					//�̹��� ���� ����
					pBtmInfo->m_nHeight = pTabInfo->nImageLength;

					//Fram Bottom�������� �����ȣ�� ������ ����
					pBtmInfo->m_nHeadNo = pFrmInfo_Bottom->m_nHeadNo;

					//�̹��� ����
					pBtmInfo->m_nWidth = nWidth;

					// 22.11.18 Ahn Modify Start
					//pBtmInfo->m_nFrameCount = nFrameCountL;
					//Tab �������� ������ ī���͸� �����´�.
					pBtmInfo->m_nFrameCount = pTabInfo->nFrameCount ;

					// 22.11.18 Ahn Modify End
					//Tab ��ȣ�� �����´�.
					pBtmInfo->nTabNo = AprData.m_NowLotData.m_nTabCount;

					//Tab Start ��ġ In Frame
					pBtmInfo->nTabStartPosInFrame = pTabInfo->nTabStartPosInFrame;

					//Bottom ����
					pBtmInfo->m_nTabLevel = nBtmLevel;

					//Bottom Frame �÷��� ����
					pBtmInfo->m_nInspMode = CFrameInfo::en_BottomFrame;

					//Tab ID �����´�.
					pBtmInfo->m_nTabId_CntBoard = cntInfo.nTabID;

					//���� �÷��� ����
					pBtmInfo->m_bErrorFlag = pTabInfo->m_bErrorFlag;

					//?
					pBtmInfo->m_nBndElectrode = nBneElectrodeBtm;// 22.05.11 Ahn Add 

					// 22.02.22 Ahn Add Start
					//����� ������ ī����
#if defined( DEBUG_NOISE_COUNTERMEASURE )
					if (bMakeDummyBtm == TRUE) {
						pBtmInfo->m_bErrorFlag = pFrmInfo_Bottom->m_bErrorFlag;
					}
#endif
					// 22.02.22 Ahn Add End
					//Bottom ī�޶� ����� �϶�
#if defined( BOTTOM_CAMERA_DEBUG )
					pBtmInfo->m_bErrorFlag = TRUE;
#endif

					// 22.05.18 Ahn Add Start
					//Top ť ������ ����
					int nTopQueCnt = pThreadQue[CAM_POS_TOP]->GetSize();

					//Bottom ť ������ ����
					int nBtmQueCnt = pThreadQue[CAM_POS_BOTTOM]->GetSize();

					//Top ť ������ IMAGE_PROC_SKIP_COUNT ���� ũ��, Bottom ť ���� �� IMAGE_PROC_SKIP_COUNT ���� ũ��
					if ((nTopQueCnt > IMAGE_PROC_SKIP_COUNT) && (nBtmQueCnt > IMAGE_PROC_SKIP_COUNT) 
						//Top/Bottom Size�� IMAGE_PROC_SKIP_COUNT ���� Ŭ �� 
						|| ( (nSizeFrmL > IMAGE_PROC_SKIP_COUNT ) && (nSizeFrmR > IMAGE_PROC_SKIP_COUNT) ) ){
						//������ ���� ���� ����
						pInfo->m_bErrorFlag = TRUE;
						//Bottom ���� �÷��� ����
						pBtmInfo->m_bErrorFlag = TRUE ;
					}
					// 22.05.18 Ahn Add Start

					// 22.12.09 Ahn Add Start
					//�ð� �����Լ�  ����
					LARGE_INTEGER tmp;
					LARGE_INTEGER start;
					QueryPerformanceFrequency(&tmp);
					double dFrequency = (double)tmp.LowPart + ((double)tmp.HighPart * (double)0xffffffff);
					QueryPerformanceCounter(&start);

					//Fram ������ ������ ����
					pInfo->m_stTime = start;
					pInfo->m_dFrecuency = dFrequency;

					//Bottom ������ ������ ����
					pBtmInfo->m_stTime = start;
					pBtmInfo->m_dFrecuency = dFrequency;

					// 22.12.09 Ahn Add End
					//������ ť ī�޶� Top�� ���� ������ ������ ����
					pThreadQue[CAM_POS_TOP]->push(pInfo) ;

					//������ ť ī�޶� Bottom  ������ ������ ������ ����
					pThreadQue[CAM_POS_BOTTOM]->push(pBtmInfo) ;

					//���� Lot Data Tab Count�� ���� ��Ų��
					AprData.m_NowLotData.m_nTabCount++;
				}
				//Tab ���� ����Ʈ�� ����
				vecTabInfo.clear();
			}

			//Fram Top ���� ��ü ����
			delete pFrmInfo_Top;
			pFrmInfo_Top = NULL;

			//Fram Bottom ���� ��ü  ����
			delete pFrmInfo_Bottom;
			pFrmInfo_Bottom = NULL;

			//ó���� �ɸ� �ð���  �����´�.
			double dSecond = ctAna.WhatTimeIsIt_Double();
			//Tact Time�� �����Ѵ�.
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
	//�̹��� ó�� ������ ��ü
	CImageProcThread* pThis = (CImageProcThread*)Param;

	// 22.08.10 Ahn Delete Start
	//CQueueCtrl* pQueueCtrl = pThis->m_pParent->GetQueuePtr();
	// 22.08.10 Ahn Delete End
	
	// 22.05.31 Ahn Add Start
	//�̹��� ���� ť ���� ��ü�� �����´�.
	CImageSaveQueueCtrl* pImgSaveQueueCtrl = pThis->m_pParent->GetImageSaveQueuePtr();

	// 22.05.31 Ahn Add End
	// 22.12.09 Ahn Add Start
	//TacTime ������ ���� ��ü�� �����´�.
	CTacTimeDataCtrl* pTactCtrl = pThis->m_pParent->GetTactDataCtrlPtr();
	// 22.12.09 Ahn Add End

	//�ӽú��� : ��� ť ��Ʈ�� ���� ��ü
	CQueueCtrl* pRsltQueueCtrl[GRABBER_COUNT];
	//Grabber ���� ��ŭ ��� ť ��ü�� �����´�.
	for (int i = 0; i < GRABBER_COUNT; i++) {
		pRsltQueueCtrl[i] = pThis->m_pParent->GetResultPtr(i);
	}

	//CFrameRsltInfo* pFrmRsltInfo;
	//������ ť ���� ��ü : ī�޶� ��� ��ŭ ����
	CThreadQueueCtrl* pThdQue[MAX_CAMERA_NO];
	for (int i = 0; i < MAX_CAMERA_NO; i++) {
		pThdQue[i] = pThis->m_pParent->GetThreadQueuePtr(i);
	}

	//�̹��� ó�� ������ ���� Top(ī�޶� ���� ���)
	CImageProcThreadUnit* pUnitTop = NULL ;

	//�̹��� ó�� ������ ���� Bottom(ī�޶� ���� ���)
	CImageProcThreadUnit* pUnitBtm = NULL ;

	//��Ʈ�� ���� ����
	BOOL bBitmapSave = FALSE;

	//Judge NG  ����
	BOOL bJudgeNG = FALSE;

	//Judge ����
	char szJudge[2][4] = { "OK", "NG" };

	//Top Botton ����
	char szPos[2][8] = { "TOP","BTM" };

	//Marking ����
	BOOL bMarkingActive = FALSE;

	//clear ����
	BOOL bClearFlag = FALSE;

	while (1) {
		if (pThis == NULL) {
			break;
		}
		if (pThis->m_bKill == TRUE) {
			break;
		}

		//ī�޶� ������ť Top Bottom ��ü �˻�
		if (!pThdQue[CAM_POS_TOP]->IsEmpty() && !pThdQue[CAM_POS_BOTTOM]->IsEmpty()) {

			//TOP ī�޶� ���� ������ �����´�.
			pUnitTop = pThdQue[CAM_POS_TOP]->pop();
			//BOTTOM ���� ������ �����´�.
			pUnitBtm = pThdQue[CAM_POS_BOTTOM]->pop();

			while (1) 
			{
				if (pThis->m_bKill == TRUE) {
					break;
				}

				// 22.12.09 Ahn Add Start
				//����ð� ���� �ð�
				LARGE_INTEGER stTime ;
				// 22.12.09 Ahn Add End
				//Top ���� ó���� End �̸鼭 Bottom ó���� End �̸�
				if ( (pUnitTop->IsProcEnd() == TRUE) && (pUnitBtm->IsProcEnd() == TRUE) ){

					//����Top ó�� ��ü���� Fram ��� ������ �����´�.
					CFrameRsltInfo *pTopInfo = pUnitTop->GetResultPtr();

					//���� Bottom ó�� ��ü���� Fram ��� ������ �����´�.
					CFrameRsltInfo *pBtmInfo = pUnitBtm->GetResultPtr();

					//Bottom Judge ��
					int nBtmJudge = pBtmInfo->m_pTabRsltInfo->m_nJudge;		
					//Top Judge ��
					int nTopJudge = pTopInfo->m_pTabRsltInfo->m_nJudge;

					// 22.12.09 Ahn Add Start 
					//Top ��� �������� ���� �ð��� �����´�.
					stTime = pTopInfo->m_stTime ;
					// 22.12.09 Ahn Add End

					// NG Tab ����
					if ((nTopJudge == JUDGE_NG) || (nBtmJudge == JUDGE_NG)) {

						//Alarm Code �ʱ�ȭ
						WORD wAlarmCode = 0x0000;

						//Judge NG �÷��� �ʱ�ȭ
						bJudgeNG = TRUE;

						//Top �� NG�̸�
						if (nTopJudge == JUDGE_NG) {
							//�˶� �ڵ� 
							wAlarmCode = pTopInfo->m_pTabRsltInfo->m_wNgReason;
							//���� Lot Data Top NG ���� ���� ��Ų��.
							AprData.m_NowLotData.m_nTopNG++;
						}

						//Bottom Judge NG �̸�
						if (nBtmJudge == JUDGE_NG) {

							//�˶��ڵ� | ���� �߰�
							wAlarmCode |= pBtmInfo->m_pTabRsltInfo->m_wNgReason;

							//���� Lot Data  Bottom NG �� ���� ��Ų��.
							AprData.m_NowLotData.m_nBottomNG++ ;
						}

						//Tab Count  NG ����
						AprData.m_NowLotData.m_nTabCountNG++ ;
		
						// 22.08.09 Ahn Add Start
						//���� Lot ������ Contiue ī��Ʈ ����
						AprData.m_NowLotData.m_nContinueCount++ ;

						//Tab Judge ��ü
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
							strMessage.Format(_T("���� NG Alarm �߻�. %d Tab���� NG �߻�"), AprData.m_NowLotData.m_nContinueCount);
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
								strMessage.Format(_T("���� NG Alarm �߻�. %d / %d Tab NG �߻�"), nSecterNgCount, AprData.m_pRecipeInfo->nSectorCount);
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
					// ��� Queue�� ����

					// Counter ��ȣ ���
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

					{ // CSV ���� �ۼ�
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
							strMarking.Format(_T("��"));
						}
						else {
							strMarking.Format(_T("��"));
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
				//1 �и� ����Ʈ ����.(������ ����� �ѱ�)
				Sleep(AprData.m_nSleep);
			}
		}
		//1 �и� ����Ʈ ����(������ ����� �ѱ��)
		Sleep(AprData.m_nSleep);
	}
	// ������ ����
	AfxEndThread(0);
	//������ Kill �÷��� FALSE ����
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
	// ��ŷ �׽�Ʈ�� ��� �� ��ŷ ��ȣ ���.
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

	// ������� ����
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
