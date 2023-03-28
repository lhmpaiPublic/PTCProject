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
#include "LogDisplayDlg.h"

// CImageProcThreadUnit
UINT CImageProcThreadUnit::CtrlImageProcThread(LPVOID pParam)
{
	CImageProcThreadUnit* pCtrl = (CImageProcThreadUnit*)pParam;
	CFrameInfo* pFrmInfo = pCtrl->m_pFrmInfo ;

	//프레임의 
	CFrameRsltInfo* pFrameRsltInfo;
	pFrameRsltInfo = new CFrameRsltInfo;
	pCtrl->m_pFrmRsltInfo = pFrameRsltInfo;

	//이미지 저장여부 객체 생성
	BOOL bBitmapSave = FALSE;
	// Judge 설정 값
	char szJudge[2][4] = { "OK", "NG" };
	//TOP/BOTTOM 설정 값
	char szPos[2][8] = { "TOP","BTM" };

	while (TRUE) {
		//ImageProc: 이미지 처리 스래드가 종료 이벤트가 발생했는가 체크
		if (::WaitForSingleObject(pCtrl->m_hEventKillThread, 0) == WAIT_OBJECT_0) {
			break;
		}
		//ImageProc: Forc Stop 이 발생했는가 체크
		if (::WaitForSingleObject(pCtrl->m_hEventForceStop, 0) == WAIT_OBJECT_0) {
			//Forc Stop 가 발생되면 스래드 종료이벤트 발생
			::SetEvent(pCtrl->m_hEventKillThread);
		}

		// Debug Mode용 Data 처리 불요
		if ( pFrmInfo->m_bDummyData == TRUE) {
			//ImageProc: Proc End  이벤트 발생
			::SetEvent(pCtrl->m_hEventProcEnd);
			break;
		}
		//ImageProc: Proc End 가 발생했는가 체크
		if (::WaitForSingleObject(pCtrl->m_hEventProcEnd, 0) != WAIT_OBJECT_0) {
			//ImageProc:Proc Start 이벤트 발생 체크
			if (::WaitForSingleObject(pCtrl->m_hEventProcStart, 0) == WAIT_OBJECT_0) {
				// 이미지 처리 개시
				if (pFrmInfo == NULL) {
					//ImageProc:  Force Stop 이벤트 발생
					//CFrameInfo 객체가 없다면 Force Stop 이벤트 발생
					::SetEvent(pCtrl->m_hEventForceStop);
					//이미지 처리 스래드 에러코드 세팅
					pCtrl->m_nErrorCode = -1;
					break;
				}

				//이미지 Byte 객체
				BYTE* pProcImg = NULL;
				//이미지 넓이
				int nWidth = pFrmInfo->m_nWidth;
				//이미지 높이
				int nHeight = pFrmInfo->m_nHeight;
				//Tab  레벨
				int nTabLevel = pFrmInfo->m_nTabLevel;
				//Left Tab 값
				int nTabLeft = pFrmInfo->m_nTabLeft;
				//Right Tab 값
				int nTabRight = pFrmInfo->m_nTabRight;

				//이미지 해더 번호
				int nHeadNo = pFrmInfo->m_nHeadNo;
				//이미지 데이터 : CFrameInfo에 저장된 이미지 데이터를 가져온다.
				BYTE* pOrgImg = pFrmInfo->GetImagePtr();

				//프레임 헤더 번호
				pFrameRsltInfo->m_nHeadNo = nHeadNo;
				//프레임  Tab  번호
				pFrameRsltInfo->nTabNo = pFrmInfo->nTabNo;
				//Tab 결과 정보에 프레임 헤더 정보 제공
				pFrameRsltInfo->m_pTabRsltInfo->m_nHeadNo = nHeadNo;
				//Tab 결과정보에 Tab 번호 제공
				pFrameRsltInfo->m_pTabRsltInfo->m_nTabNo = pFrmInfo->nTabNo;

				// 22.11.18 Ahn Add Start
				//프레임 결과 Frame Count 에 프레임 카운트 제공
				pFrameRsltInfo->m_nFrameCount = pFrmInfo->m_nFrameCount ;
				//프레임 결과 정보에 Tab Start 위치(In) 
				pFrameRsltInfo->nTabStartPosInFrame = pFrmInfo->nTabStartPosInFrame ;
				// 22.11.18 Ahn Add End
				// 22.12.01 Ahn Add Start
				pFrameRsltInfo->m_pTabRsltInfo->nFrameCount = pFrmInfo->m_nFrameCount;
				//프레임 결과 정보/Tab  결과정보 프레임 In 위치 세팅
				pFrameRsltInfo->m_pTabRsltInfo->nTabStartPosInFrame = pFrmInfo->nTabStartPosInFrame;
				// 22.12.01 Ahn Add End
				// 22.12.09 Ahn Add Start
				//프레임 결과 정보에 st Time 세팅
				pFrameRsltInfo->m_stTime = pFrmInfo->m_stTime;
				//프레임 결과정보 전파 값 제공
				pFrameRsltInfo->m_dFrecuency = pFrmInfo->m_dFrecuency;

				// 22.12.09 Ahn Add End
				//Foil Tact 값
				double dFoilExpTact = 0.0 ;
				//Tact 표면
				double dSurfaceTact = 0.0 ;

				//실행시간 체크 객체 생성 및 초기화
				CTimeAnalyzer ctAna;
				ctAna.Clear();

				int nLocalRet = 0;

				// 22.02.24 Ahn Add Start
				//시스템 시간 가저와 m_pTabRsltInfo에 저장한다.
				SYSTEMTIME sysTime;
				::GetLocalTime(&sysTime);
				pFrameRsltInfo->m_pTabRsltInfo->sysTime = sysTime;

				//시스템시간을 파일명에 사용하기 위해서 포멧을 만든다.
				CString strTime;
				// 22.12.16 Ahn Modify Start
				//strTime.Format(_T("%02d-%02d%02d%02d.%03d"), sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
				//시스템 시간명으로 이미지 저정명 추카
				strTime.Format(_T("%04d%02d%02d%02d%02d%02d%03d"), sysTime.wYear,sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
				// 22.12.16 Ahn Modify End
				// 22.02.24 Ahn Add End

				// 23.02.20 Ahn Modify Start
				//if (pFrmInfo->m_bErrorFlag == FALSE) {
				
				//에러 ? 또는 Over Flow 가 아니면
				if ((pFrmInfo->m_bErrorFlag == FALSE) && (pFrmInfo->m_bOverFlow == FALSE))
				{
				// 23.02.20 Ahn Modify End

					//프레임의 헤더 번호가 CAM_POS_TOP과 같다면 실행
					if (pFrmInfo->m_nHeadNo == CAM_POS_TOP) {
						
						//Tab Left >0, Tab Right  < 0  이면 실행 Left이면 실행
						if ((nTabLeft > 0) && (nTabRight < nHeight)) {
							
							//처리시간 체크 Start
							ctAna.StopWatchStart();
							
							// 22.05.30 Ahn Modify Startr
							//	nLocalRet = CImageProcess::ImageProcessTopSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
							// 22.09.15 Ahn Modify Start
							//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
							
							// 0 이면 양극, 1이면 음극
							//양극이면
							if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
							// 22.09.15 Ahn Modify End
								// 23.02.16 Ahn Modify Start
								//nLocalRet = CImageProcess::ImageProcessTopSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
								
								//Roll Bright Mode Top 이면 ImageProcessTopSide_BrightRoll 실행
								if (AprData.m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_TOP] == 1) {
									//이미지 프로세서 Top Bright Roll 처리
									nLocalRet = CImageProcess::ImageProcessTopSide_BrightRoll(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
								}
								//Roll Bright Mode Top 가 아니면 ImageProcessTopSide_Negative 실행
								else {
									//이미지 프로세서 처리 Top Side Negative 처리
									nLocalRet = CImageProcess::ImageProcessTopSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
								}
								// 23.02.16 Ahn Modify End
							}
							//음극이면 ImageProcessTopSide_AreaDiff 실행
							else {
								// 이미지 프로세서 Top Side AreaDiff
								nLocalRet = CImageProcess::ImageProcessTopSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, nTabLeft, nTabRight, pFrameRsltInfo->m_pTabRsltInfo);
							}
							// 22.05.30 Ahn Modify End
							
							//이미지 프로세서 처리 시간을 가져온다.
							dFoilExpTact = ctAna.WhatTimeIsIt_Double();							
						}	
						
						//이미지 처리 프로세서 처리가 잘못었을 경우  Failed Log를 찍는다.
						if (nLocalRet < 0) {
							CString strMsg;
							strMsg.Format(_T("Top Side Image Proc Failed nLcaoRet[%d], nLevel[%d]"), nLocalRet, pFrmInfo->m_nTabLevel);
							AprData.SaveErrorLog(strMsg);
						}
						// 22.05.10 Ahn Add Start 
						
						//레시피 정보에 표면 끔이 아니면
						//이미지 프로세서 ImageProcessDetectSurface 를 실행한다.
						if (AprData.m_pRecipeInfo->bDisableSurface == FALSE) {
							
							CRect rcArea;
							rcArea.left = 0;

							// 22.07.12 Ahn Modify Start
							// 22.09.15 Ahn Modify Start
							//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
							
							//Right 영역 값 ImageProcessDetectSurface 처리 시에 적용
							//Machine Mode 가 양극이면
							if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
							// 22.09.15 Ahn Modify End
								rcArea.right = pFrmInfo->m_nTabLevel - AprData.m_pRecipeInfo->TabCond.nNegVGrooveHeight;
							}
							//Machine Mode 가 음극이면 
							else {
								rcArea.right = pFrmInfo->m_nBndElectrode - AprData.m_pRecipeInfo->nSurfaceMaskOffset[CAM_POS_TOP];
							}
							// 22.07.12 Ahn Modify End
							
							//Rect top 0 , botton  nHeight 세팅
							rcArea.top = 0;
							rcArea.bottom = nHeight;

							//실행시간 체크 객체 초기화 및 시작
							ctAna.Clear();
							ctAna.StopWatchStart();

							//이미지 처리 프로세서 함수 ImageProcessDetectSurface 
							nLocalRet = CImageProcess::ImageProcessDetectSurface(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, rcArea, pFrameRsltInfo->m_pTabRsltInfo, CAM_POS_TOP, FALSE);
							dSurfaceTact = ctAna.WhatTimeIsIt_Double();

							if (nLocalRet < 0) {
							}
						}
						// 22.05.10 Ahn Add Start 
					} 
					//프레임의 헤더 번호가 CAM_POS_TOP과 같지 않다면
					else {
					//	nLocalRet = CImageProcess::ImageProcessTailSide_New(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
						
						//실행시간 체크 시작
						ctAna.StopWatchStart();
						// 22.05.30 Ahn Modify Start
						//nLocalRet = CImageProcess::ImageProcessBottomSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
						// 22.09.15 Ahn Modify Start
						//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
						
						//Machine Mode 가 양극이면
						if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
						// 22.09.15 Ahn Modify End
							// 23.02.16 Ahn Modify Start
							//nLocalRet = CImageProcess::ImageProcessBottomSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
							
							//Tabcond Roll Bright Mode 가 Bottom 이면 ImageProcessBottomSide_BrightRoll 실행
							if (AprData.m_pRecipeInfo->TabCond.nRollBrightMode[CAM_POS_BOTTOM] == 1) {
								nLocalRet = CImageProcess::ImageProcessBottomSide_BrightRoll(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
							} 
							//Tabcond Roll Bright Mode 가 Top 이면 ImageProcessBottomSide_Negative 실행
							else {
								nLocalRet = CImageProcess::ImageProcessBottomSide_Negative(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
							}
						}
						//Machine Mode 가 음극이면 ImageProcessBottomSide_AreaDiff 실행
						else {
							nLocalRet = CImageProcess::ImageProcessBottomSide_AreaDiff(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, nTabLevel, pFrameRsltInfo->m_pTabRsltInfo);
						}

						// 22.05.30 Ahn Modify End
						//실행시간 체크 값을 가져온다.
						dFoilExpTact = ctAna.WhatTimeIsIt_Double();
						//이미지 프로세서 처리가 잘못되었으면 Failed 로그 출력
						if (nLocalRet < 0) {
							CString strMsg;
							strMsg.Format(_T("Bottom Side Image Proc Failed nLcaoRet[%d], nLevel[%d]"), nLocalRet, pFrmInfo->m_nTabLevel);
							AprData.SaveErrorLog(strMsg);
						}	

						// 22.05.10 Ahn Add Start 
						//Disable Surface 가 FALSE(아니면)
						if (AprData.m_pRecipeInfo->bDisableSurface == FALSE) {

							//영역 객체 생성 left 값
							CRect rcArea;
							rcArea.left = pFrmInfo->m_nBndElectrode + AprData.m_pRecipeInfo->nSurfaceMaskOffset[CAM_POS_BOTTOM];
							rcArea.right = nWidth;
							rcArea.top = 0;
							rcArea.bottom = nHeight;

							//실행시간 체크 초기화 및 시작
							ctAna.Clear();
							ctAna.StopWatchStart();

							//설정한 영역 검사 ImageProcessDetectSurface 함수를 호출한다.
							nLocalRet = CImageProcess::ImageProcessDetectSurface(pOrgImg, nWidth, nHeight, AprData.m_pRecipeInfo, rcArea, pFrameRsltInfo->m_pTabRsltInfo, CAM_POS_BOTTOM, FALSE);
							
							//처리시간을 가져온다.
							dSurfaceTact = ctAna.WhatTimeIsIt_Double();

							//ImageProcessDetectSurface 처리가 잘못되었으면
							if (nLocalRet < 0) {
							}
						}
						// 22.05.10 Ahn Add Start 
					}
				// 23.02.20 Ahn Add Start
				}
				//에러 ? 또는 Over Flow이면
				else
				{
					// Top 결과 정보  m_pTabRsltInfo  /NG 설정
					pFrameRsltInfo->m_pTabRsltInfo->m_nJudge = JUDGE_NG; // 강제 NG 처리	
					pFrameRsltInfo->m_pTabRsltInfo->m_wNgReason |= ((pFrameRsltInfo->m_nHeadNo == CAM_POS_TOP) ? CTabRsltBase::en_Reason_FoilExp_Top : CTabRsltBase::en_Reason_FoilExp_Btm);

					 //NG 로그 출력한다.
					 CString strMsg;
					 strMsg.Format(_T("[Error NG] m_bErrorFlag = %d, m_bOverFlow = %d"), pFrmInfo->m_bErrorFlag, pFrmInfo->m_bOverFlow);
					 AprData.SaveDebugLog(strMsg); //pyjtest

				}

				// 23.02.20 Ahn Add End
				//Tact 타임을 로그로 저장한다.
				CString strTactLog;
				strTactLog.Format(_T("Pos[%d], Foil노출 Tact[%.2lf], Surface Tact[%.2lf]"), pFrmInfo->m_nHeadNo, dFoilExpTact, dSurfaceTact);
				AprData.SaveTactLog(strTactLog);

				// 22.04.18 Ahn Add Start
				//ImageProc: Tab 에 대한 이미지 최대 사이즈 저장
				double dMaxSize = pFrameRsltInfo->m_pTabRsltInfo->SortingDefect(0);
				pFrameRsltInfo->m_pTabRsltInfo->m_dMaxSizeDef = dMaxSize;
				// 22.04.18 Ahn Add End

				//Judge
				int nJudge = JUDGE_OK;
				//BOOL bSaveOkDef = FALSE; // 23.02.10 Ahn Delete
				{
					//Foil Size 가져온다.
					double dSizeX = AprData.m_pRecipeInfo->dFoilExpInNgSize;
					double dSizeY = AprData.m_pRecipeInfo->dDefJudgeHeight;

					//알람코드 0 초기화
					WORD wAlarmCode = 0x00;
					//인덱스 초기화
					int nDefIdx = 0 ;
					//최대사이즈 초기화
					double dMaxSize = 0.0;
					//Save Crop 초기화
					BOOL bSaveCrop = FALSE;

					//Judge 값을 가져온다.
					nJudge = pFrameRsltInfo->m_pTabRsltInfo->m_nJudge ; // = pFrameRsltInfo->m_pTabRsltInfo->GetJudgement(dSizeX, dSizeY, dMaxSize, bJudgeType, nDefIdx);

					//점좌표 객체 생성 및 초기화
					CPoint cpNgPos;
					cpNgPos.SetPoint(0, 0);

					// 22.11.21 Ahn Modify Start - JUDGE_GRAY
					//if (nJudge == JUDGE_NG) {
					//	bSaveCrop = TRUE ;
					//}

					//Judge가 NG, GRAY, OK 에 대한 bSaveCrop 설정
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

					//bSaveCrop TRUE이면 Tab 결과 정보 Crop 이미지 플레그를 TRUE로 설정
					if (bSaveCrop == TRUE)
					{
						pFrameRsltInfo->m_pTabRsltInfo->m_bCropImgFlag = TRUE;
					}
				}

				//Save 상태값 초기화
				BOOL bSave = FALSE;

				// 22.11.21 Ahn Modify Start - JUDGE_GRAY
				//if ( (nJudge == JUDGE_NG) || (bSaveOkDef == TRUE ) ){
				
				//Judge GRAY 또는 NG이면 bSave TRUE 모든 파일 저장
				if ( (nJudge == JUDGE_GRAY) || (nJudge == JUDGE_NG) )
				{
				// 22.11.21 Ahn Modify End
					bSave = TRUE;
				} 
				//Judge가 OK이면bSaveOnlyNgTab가 FALsE 이고, nBmpSaveInterval이 0이상일 때 저장
				else
				{
					if (AprData.m_pRecipeInfo->bSaveOnlyNgTab != TRUE)
					{
						//비트맵 이미지 저장 레벨이 0 이상이면
						if (AprData.m_pRecipeInfo->nBmpSaveInterval > 0)
						{
							//비트맵 레벨이 1이면 저장
							if (AprData.m_pRecipeInfo->nBmpSaveInterval == 1)
							{
								bSave = TRUE;
							}
							//비트맵 레벨이 1이 아니면 체크 : Tab 번호를 레벨로 나눈다
							else if ((pFrmInfo->nTabNo % AprData.m_pRecipeInfo->nBmpSaveInterval) == 0)
							{
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
				//이미지를 저장 변수가  TRUE이면
				if (bSave == TRUE) 
				{
					//선택된 이미지 저장 포맷
					//SystemSetting Dlg 에서 설정한다.
					//전역 접근 객체에 저장한다.
					CString strImageFormat = AprData.getGSt()->GetOutImageFormat();

					// 22.05.31 Ahn Delete Start - Image Save Thread
					//CBitmapStd bmp(pFrmInfo->m_nWidth, pFrmInfo->m_nHeight, 8);
					//bmp.SetImage(pFrmInfo->m_nWidth, pFrmInfo->m_nHeight, pFrmInfo->GetImagePtr());
					//// Debug시에 이미지 퀄리티가 계속 저하 되는 것을 방지.
					//bmp.SetJpegQuality(AprData.m_System.m_nJpegSaveQuality);
					// 22.05.31 Ahn Delete End

					//파일 이름 객체 생성
					CString strFileName;
					//경로 객체 생성
					CString strPath; 
					//Judge 객체 생성
					CString strJudge; 
					// 22.11.21 Ahn Modify Start - JUDGE_GRAY
					//if (nJudge == JUDGE_NG) {
					//	strPath = AprData.m_strNowNgPath;
					//	strJudge = _T("NG");
					//} else {
					//	strPath = AprData.m_strNowOkPath;
					//	strJudge = _T("OK");
					//}

					//Judge 별 저장 경로를 가져온다.
					switch (nJudge)
					{
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
					//ImageProc: 이미지저장명 결정 생성
					strFileName.Format(_T("%s_%s_%s_%s_%d_%s_%s%s")
						, INSPECTION_TYPE
						, strTime
						, AprData.m_System.m_strMachineID
						, AprData.m_NowLotData.m_strLotNo
						, pFrmInfo->nTabNo + 1
						, (pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? _T("TOP") : _T("BTM")
						, strJudge
						, strImageFormat
					);

					// 22.05.27 Ahn Modify End
					//ImageProc: 이미지저장 플레그를 TRUE로 설정한다.
					pFrameRsltInfo->m_pTabRsltInfo->m_bImageFlag = TRUE ;
					
					// 22.05.31 Ahn Delete Start - Image Save Thread
					// bmp.SaveBitmap(strFileName);
					// 22.05.31 Ahn Delete End - Image Save Thread

					//ImageProc:  이미지파일저장명과 경로를 저장
					::_tcsnccpy_s(pFrameRsltInfo->m_pTabRsltInfo->m_chImageFile, _countof(pFrameRsltInfo->m_pTabRsltInfo->m_chImageFile), strFileName.GetBuffer(0), _TRUNCATE);
					::_tcsnccpy_s(pFrameRsltInfo->m_pTabRsltInfo->m_chImagePath, _countof(pFrameRsltInfo->m_pTabRsltInfo->m_chImagePath), strPath.GetBuffer(0), _TRUNCATE);

// 					CString strMsg;
// 					strMsg.Format(_T("Save Image Path = %s"), pFrameRsltInfo->m_pTabRsltInfo->m_chImagePath);
// 					AprData.SaveDebugLog(strMsg); //pyjtest
// 					strMsg.Format(_T("Save Image Name = %s"), pFrameRsltInfo->m_pTabRsltInfo->m_chImageFile);
// 					AprData.SaveDebugLog(strMsg); //pyjtest


				}

				//파일저장 프레임 결과 정보에 저장한다.
				pFrameRsltInfo->Copy(pFrmInfo);

				//프레임 정보 로컬 객체 삭제
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
			} //Proc Start 이벤트 샐행 루프 빠져나감
		} //Proc End 이벤트 
		Sleep(AprData.m_nSleep);
	}

	//이미지 처리 스래드 유효성을  FALSE 설정
	pCtrl->m_bThreadValid = FALSE;
	//CFrameInfo 객체의 Kill 이벤트 설정
	::SetEvent(pCtrl->m_hEventKilled);

	return 0;
}

IMPLEMENT_DYNCREATE(CImageProcThreadUnit, CWinThread)

CImageProcThreadUnit::CImageProcThreadUnit( CFrameInfo *pFrmInfo )
{
//	m_pQueueCtrl = pQueueCtrl;

	//ImageProc Proc Start 이벤트 객체 생성
	m_hEventProcStart = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventRun = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	//ImageProc: Image Proc Thread Proc End 이벤트객체 생성
	m_hEventProcEnd = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventForceStop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	//ImageProc: CtrlImageProcThread 종료처리 이벤트 객체 생성
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

	//이벤트 객체 종료
	::CloseHandle(m_hEventProcStart);
	::CloseHandle(m_hEventRun);
	//ImageProc: 이미지 처리 Proc End  이벤트 객체 종료
	::CloseHandle(m_hEventProcEnd);
	::CloseHandle(m_hEventForceStop);
	//ImageProc: 스드래종료 이벤트 객체 종료
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
			//Start 처리 이벤트 발생
			::SetEvent(m_hEventProcStart);
		}

	}
	return 0;
}
int CImageProcThreadUnit::Kill()
{
	if (m_pThread != NULL) {
		//ImageProc: 스래드 종료 이벤트 발생
		::SetEvent(m_hEventKillThread);
		//종료이벤트 발생 후 남은 처리를 기다린다.
		WaitForSingleObject(m_hEventKilled, INFINITE);
	}
	return 0;
}

BOOL CImageProcThreadUnit::IsProcEnd()
{
	//ImageProc: IsProcEnd 인가 알아본다.
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

