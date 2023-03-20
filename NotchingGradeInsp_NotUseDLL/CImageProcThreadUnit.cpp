// CImageProcThreadUnit.cpp: 구현 파일
//

#include "pch.h"
#include "CImageProcThreadUnit.h"
#include "CImageProcess.h"
#include "ImageProcessCtrl.h"
#include "FrameInfo.h"
#include "FrameRsltInfo.h"
#include "GlobalData.h"
#include "CRecipeCtrl.h"
#include "BitmapStd.h"
#include "TimeAnalyzer.h"

// CImageProcThreadUnit
UINT CImageProcThreadUnit::CtrlImageProcThread(LPVOID pParam)
{
	CImageProcThreadUnit* pCtrl = (CImageProcThreadUnit*)pParam;
	CFrameInfo* pFrmInfo = pCtrl->m_pFrmInfo ;

	CFrameRsltInfo* pFrameRsltInfo;
	pFrameRsltInfo = new CFrameRsltInfo;
	pCtrl->m_pFrmRsltInfo = pFrameRsltInfo;

	BOOL bBitmapSave = FALSE;
	char szJudge[2][4] = { "OK", "NG" };
	char szPos[2][8] = { "TOP","BTM" };

	while (TRUE) {
		if (::WaitForSingleObject(pCtrl->m_hEventKillThread, 0) == WAIT_OBJECT_0) {
			break;
		}
		if (::WaitForSingleObject(pCtrl->m_hEventForceStop, 0) == WAIT_OBJECT_0) {
			::SetEvent(pCtrl->m_hEventKillThread);
		}

		// Debug Mode용 Data 처리 불요
		if ( pFrmInfo->m_bDummyData == TRUE) {
			::SetEvent(pCtrl->m_hEventProcEnd);
			break;
		}

		if (::WaitForSingleObject(pCtrl->m_hEventProcEnd, 0) != WAIT_OBJECT_0) {
			if (::WaitForSingleObject(pCtrl->m_hEventProcStart, 0) == WAIT_OBJECT_0) {
				// 이미지 처리 개시
				if (pFrmInfo == NULL) {
					::SetEvent(pCtrl->m_hEventForceStop);
					pCtrl->m_nErrorCode = -1;
					break;
				}

				BYTE* pProcImg = NULL;
				int nWidth = pFrmInfo->m_nWidth;
				int nHeight = pFrmInfo->m_nHeight;
				int nTabLevel = pFrmInfo->m_nTabLevel;
				int nTabLeft = pFrmInfo->m_nTabLeft;
				int nTabRight = pFrmInfo->m_nTabRight;

				int nHeadNo = pFrmInfo->m_nHeadNo;
				BYTE* pOrgImg = pFrmInfo->GetImagePtr();

				pFrameRsltInfo->m_nHeadNo = nHeadNo;
				pFrameRsltInfo->nTabNo = pFrmInfo->nTabNo;
				pFrameRsltInfo->m_pTabRsltInfo->m_nHeadNo = nHeadNo;
				pFrameRsltInfo->m_pTabRsltInfo->m_nTabNo = pFrmInfo->nTabNo;

				// 22.11.18 Ahn Add Start
				pFrameRsltInfo->m_nFrameCount = pFrmInfo->m_nFrameCount ;
				pFrameRsltInfo->nTabStartPosInFrame = pFrmInfo->nTabStartPosInFrame ;
				// 22.11.18 Ahn Add End
				// 22.12.01 Ahn Add Start
				pFrameRsltInfo->m_pTabRsltInfo->nFrameCount = pFrmInfo->m_nFrameCount;
				pFrameRsltInfo->m_pTabRsltInfo->nTabStartPosInFrame = pFrmInfo->nTabStartPosInFrame;
				// 22.12.01 Ahn Add End
				// 22.12.09 Ahn Add Start
				pFrameRsltInfo->m_stTime = pFrmInfo->m_stTime;
				pFrameRsltInfo->m_dFrecuency = pFrmInfo->m_dFrecuency;
				// 22.12.09 Ahn Add End

				double dFoilExpTact = 0.0 ;
				double dSurfaceTact = 0.0 ;
				CTimeAnalyzer ctAna;
				ctAna.Clear();

				int nLocalRet = 0;

				// 22.02.24 Ahn Add Start
				SYSTEMTIME sysTime;
				::GetLocalTime(&sysTime);
				pFrameRsltInfo->m_pTabRsltInfo->sysTime = sysTime;
				CString strTime;
				// 22.12.16 Ahn Modify Start
				//strTime.Format(_T("%02d-%02d%02d%02d.%03d"), sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
				strTime.Format(_T("%04d%02d%02d%02d%02d%02d%03d"), sysTime.wYear,sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
				// 22.12.16 Ahn Modify End
				// 22.02.24 Ahn Add End

				// 23.02.20 Ahn Modify Start
				//if (pFrmInfo->m_bErrorFlag == FALSE) {
				if ((pFrmInfo->m_bErrorFlag == FALSE) && (pFrmInfo->m_bOverFlow == FALSE)) {
				// 23.02.20 Ahn Modify End
					if (pFrmInfo->m_nHeadNo == CAM_POS_TOP) {
						if ((nTabLeft > 0) && (nTabRight < nHeight)) {
							ctAna.StopWatchStart();
							// 22.05.30 Ahn Modify Startr
							//	nLocalRet = CImageProcess::ImageProcessTopSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
							// 22.09.15 Ahn Modify Start
							//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
							if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
							// 22.09.15 Ahn Modify End
								// 23.02.16 Ahn Modify Start
								//nLocalRet = CImageProcess::ImageProcessTopSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
								if (AprData.m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) {
									nLocalRet = CImageProcess::ImageProcessTopSide_BrightRoll(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
								} else {
									nLocalRet = CImageProcess::ImageProcessTopSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
								}
								// 23.02.16 Ahn Modify End
							}
							else {
								nLocalRet = CImageProcess::ImageProcessTopSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
							}
							// 22.05.30 Ahn Modify End
							dFoilExpTact = ctAna.WhatTimeIsIt_Double();							
						}						
						if (nLocalRet < 0) {
							CString strMsg;
							strMsg.Format(_T("Top Side Image Proc Failed nLcaoRet[%d], nLevel[%d]"), nLocalRet, pFrmInfo->m_nTabLevel);
							AprData.SaveErrorLog(strMsg);
						}
						// 22.05.10 Ahn Add Start 
						if (AprData.m_pRecipeInfo->bDisableSurface == FALSE) {
							CRect rcArea;
							rcArea.left = 0;
							// 22.07.12 Ahn Modify Start
							// 22.09.15 Ahn Modify Start
							//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
							if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
							// 22.09.15 Ahn Modify End
								rcArea.right = pFrmInfo->m_nTabLevel - AprData.m_pRecipeInfo->TabCond.nNegVGrooveHeight;
							}
							else {
								rcArea.right = pFrmInfo->m_nBndElectrode - AprData.m_pRecipeInfo->nSurfaceMaskOffset[CAM_POS_TOP];
							}
							// 22.07.12 Ahn Modify End
							rcArea.top = 0;
							rcArea.bottom = nHeight;
							ctAna.Clear();
							ctAna.StopWatchStart();
							nLocalRet = CImageProcess::ImageProcessDetectSurface(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, rcArea, pFrameRsltInfo->m_pTabRsltInfo, CAM_POS_TOP, FALSE);
							dSurfaceTact = ctAna.WhatTimeIsIt_Double();

							if (nLocalRet < 0) {
							}
						}
						// 22.05.10 Ahn Add Start 
					} else {
					//	nLocalRet = CImageProcess::ImageProcessTailSide_New(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
						ctAna.StopWatchStart();
						// 22.05.30 Ahn Modify Start
						//nLocalRet = CImageProcess::ImageProcessBottomSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
						// 22.09.15 Ahn Modify Start
						//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
						if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
						// 22.09.15 Ahn Modify End
							// 23.02.16 Ahn Modify Start
							//nLocalRet = CImageProcess::ImageProcessBottomSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
							if (AprData.m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_BOTTOM] == 1) {
								nLocalRet = CImageProcess::ImageProcessBottomSide_BrightRoll(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
							} else {
								nLocalRet = CImageProcess::ImageProcessBottomSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
							}
						}
						else {
							nLocalRet = CImageProcess::ImageProcessBottomSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
						}
						// 22.05.30 Ahn Modify End
						dFoilExpTact = ctAna.WhatTimeIsIt_Double();
						if (nLocalRet < 0) {
							CString strMsg;
							strMsg.Format(_T("Bottom Side Image Proc Failed nLcaoRet[%d], nLevel[%d]"), nLocalRet, pFrmInfo->m_nTabLevel);
							AprData.SaveErrorLog(strMsg);
						}	
						// 22.05.10 Ahn Add Start 
						if (AprData.m_pRecipeInfo->bDisableSurface == FALSE) {
							CRect rcArea;
							rcArea.left = pFrmInfo->m_nBndElectrode + AprData.m_pRecipeInfo->nSurfaceMaskOffset[CAM_POS_BOTTOM];
							rcArea.right = nWidth;
							rcArea.top = 0;
							rcArea.bottom = nHeight;
							ctAna.Clear();
							ctAna.StopWatchStart();
							nLocalRet = CImageProcess::ImageProcessDetectSurface(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, rcArea, pFrameRsltInfo->m_pTabRsltInfo, CAM_POS_BOTTOM, FALSE);
							dSurfaceTact = ctAna.WhatTimeIsIt_Double();
							if (nLocalRet < 0) {
							}
						}
						// 22.05.10 Ahn Add Start 
					}
				// 23.02.20 Ahn Add Start
				}
				else
				{
					pFrameRsltInfo->m_pTabRsltInfo->m_nJudge = JUDGE_NG; // 강제 NG 처리	
					pFrameRsltInfo->m_pTabRsltInfo->m_wNgReason |= ((pFrameRsltInfo->m_nHeadNo == CAM_POS_TOP) ? CTabRsltBase::en_Reason_FoilExp_Top : CTabRsltBase::en_Reason_FoilExp_Btm);

					 CString strMsg;
					 strMsg.Format(_T("[Error NG] m_bErrorFlag = %d, m_bOverFlow = %d"), pFrmInfo->m_bErrorFlag, pFrmInfo->m_bOverFlow);
					 AprData.SaveDebugLog(strMsg); //pyjtest

				}
				// 23.02.20 Ahn Add End
				CString strTactLog;
				strTactLog.Format(_T("Pos[%d], Foil노출 Tact[%.2lf], Surface Tact[%.2lf]"), pFrmInfo->m_nHeadNo, dFoilExpTact, dSurfaceTact);
				AprData.SaveTactLog(strTactLog);
				// 22.04.18 Ahn Add Start
				double dMaxSize = pFrameRsltInfo->m_pTabRsltInfo->SortingDefect(0);
				pFrameRsltInfo->m_pTabRsltInfo->m_dMaxSizeDef = dMaxSize;
				// 22.04.18 Ahn Add End

				int nJudge = JUDGE_OK;
				//BOOL bSaveOkDef = FALSE; // 23.02.10 Ahn Delete
				{
					double dSizeX = AprData.m_pRecipeInfo->dFoilExpInNgSize;
					double dSizeY = AprData.m_pRecipeInfo->dDefJudgeHeight;
					WORD wAlarmCode = 0x00;
					int nDefIdx = 0 ;
					double dMaxSize = 0.0;
					BOOL bSaveCrop = FALSE;
					nJudge = pFrameRsltInfo->m_pTabRsltInfo->m_nJudge ; // = pFrameRsltInfo->m_pTabRsltInfo->GetJudgement(dSizeX, dSizeY, dMaxSize, bJudgeType, nDefIdx);

					CPoint cpNgPos;
					cpNgPos.SetPoint(0, 0);
					// 22.11.21 Ahn Modify Start - JUDGE_GRAY
					//if (nJudge == JUDGE_NG) {
					//	bSaveCrop = TRUE ;
					//}
					switch (nJudge) {
					case	JUDGE_NG :
					case	JUDGE_GRAY:
						bSaveCrop = TRUE;
						break;
					case	JUDGE_OK :
					default :
						break;
					}
					//CString strFilePath;
					//strFilePath.Format(_T("%s\\"), AprData.m_strNowNgPath );
					//::_tcsnccpy_s(pFrameRsltInfo->m_pTabRsltInfo->m_chCropPath, _countof(pFrameRsltInfo->m_pTabRsltInfo->m_chCropPath), strFilePath.GetBuffer(0), _TRUNCATE);

					if (bSaveCrop == TRUE) {
						pFrameRsltInfo->m_pTabRsltInfo->m_bCropImgFlag = TRUE;
					}
				}

				CString strExtName = _T(".jpg");
				BOOL bSave = FALSE;

				// 22.11.21 Ahn Modify Start - JUDGE_GRAY
				//if ( (nJudge == JUDGE_NG) || (bSaveOkDef == TRUE ) ){
				if ( (nJudge == JUDGE_GRAY) || (nJudge == JUDGE_NG) ){
				// 22.11.21 Ahn Modify End
					bSave = TRUE;
				} else {
					if (AprData.m_pRecipeInfo->bSaveOnlyNgTab != TRUE) {
						if (AprData.m_pRecipeInfo->nBmpSaveInterval > 0) {
							if (AprData.m_pRecipeInfo->nBmpSaveInterval == 1) {
								bSave = TRUE;
							}
							else if ((pFrmInfo->nTabNo % AprData.m_pRecipeInfo->nBmpSaveInterval) == 0) {
								bSave = TRUE;
							}
						}
					}
				}
#if defined(DEBUG)
				if (pFrmInfo->nTabNo == 0) {
					bSave = TRUE;
				}
#endif
				if (bSave == TRUE) {
					// 22.05.31 Ahn Delete Start - Image Save Thread
					//CBitmapStd bmp(pFrmInfo->m_nWidth, pFrmInfo->m_nHeight, 8);
					//bmp.SetImage(pFrmInfo->m_nWidth, pFrmInfo->m_nHeight, pFrmInfo->GetImagePtr());
					//// Debug시에 이미지 퀄리티가 계속 저하 되는 것을 방지.
					//bmp.SetJpegQuality(AprData.m_System.m_nJpegSaveQuality);
					// 22.05.31 Ahn Delete End

					CString strFileName;
					CString strPath; 
					CString strJudge; 
					// 22.11.21 Ahn Modify Start - JUDGE_GRAY
					//if (nJudge == JUDGE_NG) {
					//	strPath = AprData.m_strNowNgPath;
					//	strJudge = _T("NG");
					//} else {
					//	strPath = AprData.m_strNowOkPath;
					//	strJudge = _T("OK");
					//}
					switch (nJudge) {
					case	JUDGE_OK :
						strPath = AprData.m_strNowOkPath;
						strJudge = _T("OK");
						break;
					case	JUDGE_NG :
						strPath = AprData.m_strNowNgPath;
						strJudge = _T("NG");
						break;
					case	JUDGE_GRAY :
						strPath = AprData.m_strNowOkPath;
						strJudge = _T("OK");
						break;
					}
					// 22.11.21 Ahn Modify End

					// 22.05.27 Ahn Modify Start
					strFileName.Format(_T("%s_%s_%s_%s_%d_%s_%s.jpg")
						, INSPECTION_TYPE
						, strTime
						, AprData.m_System.m_strMachineID
						, AprData.m_NowLotData.m_strLotNo
						, pFrmInfo->nTabNo + 1
						, (pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? _T("TOP") : _T("BTM")
						, strJudge
					);
					// 22.05.27 Ahn Modify End

					pFrameRsltInfo->m_pTabRsltInfo->m_bImageFlag = TRUE ;
					
					// 22.05.31 Ahn Delete Start - Image Save Thread
					// bmp.SaveBitmap(strFileName);
					// 22.05.31 Ahn Delete End - Image Save Thread
					::_tcsnccpy_s(pFrameRsltInfo->m_pTabRsltInfo->m_chImageFile, _countof(pFrameRsltInfo->m_pTabRsltInfo->m_chImageFile), strFileName.GetBuffer(0), _TRUNCATE);
					::_tcsnccpy_s(pFrameRsltInfo->m_pTabRsltInfo->m_chImagePath, _countof(pFrameRsltInfo->m_pTabRsltInfo->m_chImagePath), strPath.GetBuffer(0), _TRUNCATE);
				}

				pFrameRsltInfo->Copy(pFrmInfo);

				if (pFrmInfo != NULL) {
					delete pFrmInfo;
					pFrmInfo = NULL;
				}

				// 처리 완료
				//if (::WaitForSingleObject(pCtrl->m_hEventProcEnd, 0) != WAIT_OBJECT_0) {
				//	::SetEvent(pCtrl->m_hEventProcEnd);
				pCtrl->SetEventProcEnd();
				break;
				//}
			}
		}
		Sleep(AprData.m_nSleep);
	}

	pCtrl->m_bThreadValid = FALSE;
	::SetEvent(pCtrl->m_hEventKilled);

	return 0;
}

IMPLEMENT_DYNCREATE(CImageProcThreadUnit, CWinThread)

CImageProcThreadUnit::CImageProcThreadUnit( CFrameInfo *pFrmInfo )
{
//	m_pQueueCtrl = pQueueCtrl;

	m_hEventProcStart = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventRun = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventProcEnd = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventForceStop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventKillThread = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventKilled = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hSendResult = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	m_pFrmInfo = pFrmInfo;
	m_pThread = NULL;
	m_nErrorCode = 0;
}

CImageProcThreadUnit::~CImageProcThreadUnit()
{
	//DWORD dwExitCode;

	// 22.02.18 Ahn Delete Start
	//if (m_pThread != NULL) {
	//	if (m_pThread->m_hThread != NULL) { // 22.01.11 Ahn Add
	//		if ((GetExitCodeThread(m_pThread->m_hThread, &dwExitCode))
	//			&& (dwExitCode == STILL_ACTIVE)) {
	//			// Kill the worker thread by setting the "kill thread" event.
	//			// of the "kill thread" and "start recalc" events.
	//			if (::WaitForSingleObject(m_hEventKilled, 0) != WAIT_OBJECT_0) {
	//				SetEvent(m_hEventKillThread);
	//				WaitForSingleObject(m_hEventKilled, INFINITE);
	//			}
	//		}
	//	}
	//}
	// 22.02.18 Ahn Delete End

	::CloseHandle(m_hEventProcStart);
	::CloseHandle(m_hEventRun);
	::CloseHandle(m_hEventProcEnd);
	::CloseHandle(m_hEventForceStop);
	::CloseHandle(m_hEventKillThread);
	::CloseHandle(m_hEventKilled);
	::CloseHandle(m_hSendResult);
}


BOOL CImageProcThreadUnit::InitInstance()
{
	// TODO:  여기에서 각 스레드에 대한 초기화를 수행합니다.
	return TRUE;
}

int CImageProcThreadUnit::ExitInstance()
{
	// TODO:  여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CImageProcThreadUnit, CWinThread)
END_MESSAGE_MAP()


// CImageProcThreadUnit 메시지 처리기



int CImageProcThreadUnit::Begin()
{
	if (m_pThread == NULL) {
		m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlImageProcThread
			, (LPVOID)this
			, THREAD_PRIORITY_NORMAL// (THREAD_PRIORITY_LOWEST/THREAD_PRIORITY_NORMAL/THREAD_PRIORITY_HIGHEST)
			, 0
			, CREATE_SUSPENDED
			, NULL);
		if (m_pThread != NULL) {
			m_pThread->m_bAutoDelete = TRUE;
			m_pThread->ResumeThread();
			::SetEvent(m_hEventProcStart);
		}

	}
	return 0;
}
int CImageProcThreadUnit::Kill()
{
	if (m_pThread != NULL) {
		::SetEvent(m_hEventKillThread);
		WaitForSingleObject(m_hEventKilled, INFINITE);
	}
	return 0;
}

BOOL CImageProcThreadUnit::IsProcEnd()
{
	if (::WaitForSingleObject(m_hEventProcEnd, 0) == WAIT_OBJECT_0) {
		return TRUE;
	}
	return FALSE;
}
int CImageProcThreadUnit::ForceStop()
{
	::SetEvent(m_hEventForceStop);
	return 0;
}

void CImageProcThreadUnit::SendResult()
{
	::SetEvent(m_hSendResult);
}

