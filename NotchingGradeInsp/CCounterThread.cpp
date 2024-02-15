#include "pch.h"
#include "CCounterThread.h"
#include "ImageProcessCtrl.h"
#include "AppDIO.h"
#include "CCounterQueueCtrl.h"
#include "GlobalData.h"
#include "SigProc.h"
#include "WinTool.h"
#include "TriggerSocket.h"

std::vector<MarkSendInfo> CCounterThread::m_MarkSendInfoData;
typedef std::vector<MarkSendInfo>::iterator MarkSendInfoData_iterator;
BOOL CCounterThread::m_bMarkSendInfoDataSynch = FALSE;
__int64 CCounterThread::deepSwitchOff = 0;

std::vector<int> CCounterThread::m_inputReadId;

typedef std::vector<int>::iterator inputReadId_iterator;

void CCounterThread::MarkSendInfo_Push_back(int TabId, WORD MarkingOutputData, bool bSendComplate)
{
	if ((TabId >= 0) && (TabId < 64))
	{
		//마킹 정보를 저장하기 위해서 들어오는 카운트
		CCounterThread::deepSwitchOff++;

		//push 하기 전에 보낸 데이터를 지운다.
		//이전 마킹 데이터가 있을 때
		if (CCounterThread::m_bMarkSendInfoDataSynch == FALSE)
		{
			CCounterThread::m_bMarkSendInfoDataSynch = TRUE;
			if (CCounterThread::m_MarkSendInfoData.size())
			{

				//지울 최종 포인터
				MarkSendInfoData_iterator itdelete = CCounterThread::m_MarkSendInfoData.end();
				//시작 점
				MarkSendInfoData_iterator it = CCounterThread::m_MarkSendInfoData.begin();
				//end 까지 돌면서 true 인 지울 end 포인터를 백업한다.
				do
				{
					if (it->bSendComplate == true)
					{
						itdelete = it;
					}
					it++;
				} while (CCounterThread::m_MarkSendInfoData.end() != it);

				//지울 데이터가 있다면
				if (CCounterThread::m_MarkSendInfoData.end() != itdelete)
				{
					//DIO Input Log
					LOGDISPLAY_SPEC(7)(_T("@@지울 데이터가 있다면size<%d> id<%d>까지"), CCounterThread::m_MarkSendInfoData.size(), itdelete->TabId);

					//시작점 부터 true 설정된 데이터까지 지운다.
					CCounterThread::m_MarkSendInfoData.erase(CCounterThread::m_MarkSendInfoData.begin(), itdelete);
				}
			}
			CCounterThread::m_bMarkSendInfoDataSynch = FALSE;
		}

		//딮 스위치를 꺼서 Trigger BCD Id를 계속 못받으면 
		//CCounterThread::deepSwitchOff 카운터가 증가하고 10이상이면 계속적으로 안들어온다 생각으로 
		if (CCounterThread::deepSwitchOff < 10)
		{
			//저장한다.
			MarkSendInfo MarkSendInfoData;
			MarkSendInfoData.TabId = TabId;
			MarkSendInfoData.MarkingOutputData = MarkingOutputData;
			MarkSendInfoData.bSendComplate = bSendComplate;
			CCounterThread::m_MarkSendInfoData.push_back(MarkSendInfoData);
			//DIO Input Log
			LOGDISPLAY_SPEC(7)(_T("@@마킹 데이터 id<%d> OutputData<%d>"), TabId, MarkingOutputData);
		}
		else
		{
			if (CCounterThread::m_MarkSendInfoData.size())
			{
				//마킹 데이터 전체 삭제
				CCounterThread::m_MarkSendInfoData.clear();
				//DIO Input Log
				LOGDISPLAY_SPEC(7)(_T("@@마킹 데이터 전체 삭제"));
			}
		}
	}

}

void CCounterThread::RecivePacket(char* data, int len)
{
	int nCnt = 0;

	char szBuf[1024];

	int nID = 0;
	int nEncodeCnt = 0;

	int nStx = -1;
	int nEtx = -1;
	for (int i = 0; i < len; i++)
	{
		if (data[i] == 0x02)
		{
			nStx = i;
			continue;
		}

		if (nStx >= 0 && data[i] == 0x03)
		{
			nEtx = i;
			int nCnt = 0;
			for (int n = nStx; n < nEtx; n++, nCnt++)
			{
				if (data[n] != 0x7D)
				{
					szBuf[nCnt] = data[n];
				}
				else
				{
					szBuf[nCnt] = data[n+1] ^ 0x20;
					n++;
				}
			}
			nID = MAKEWORD(szBuf[2], szBuf[3]);
			nEncodeCnt = MAKELONG(MAKEWORD(szBuf[4], szBuf[5]), MAKEWORD(szBuf[6], szBuf[7]));

			//Tab Id 정보를 추가한다.
			CCounterInfo cntInfo;
			cntInfo.nTabID = nID;
			cntInfo.nEnCoderCount = nEncodeCnt;
			m_pCntQueInPtr->PushBack(cntInfo);
			//받은 id
			m_inputReadId.push_back(nID);
		}
	}

	AprData.SaveDebugLog_Format(_T("ID:%d, Encode Count = %d"), nID, nEncodeCnt);


}

CCounterThread::CCounterThread(CImageProcessCtrl* pParent)
{
	m_pParent = pParent;
	m_MarkSendInfoData.clear();
	m_TriggerSocket = NULL;

	ConnectTrigger(_T("192.168.0.198"), 34700);
	m_pCntQueInPtr = m_pParent->GetCounterQueInPtr();
}
CCounterThread::~CCounterThread()
{
}
void CCounterThread::Begin()
{
	m_bKill = FALSE;

	if (m_pThread == NULL) {

		//이벤트 객체 생성
		pEvent_CounterThread = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadCounter,
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
void CCounterThread::Kill(void)
{
	if (m_pThread)
	{
		setEvent_CounterThread();
		CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
		m_pThread->m_hThread = NULL;
	}
}

//WORD CCounterThread::GetCounterSignal(int nTabId, int nJudge1, int nJudge2, int nType1, int nType2, int nMarkSel1, int nMarkSel2 ) 
//{
//	WORD wOutput = 0x00;
//
//	if (nJudge1 || nJudge2) {
//		wOutput |= CAppDIO::eOut_TAB_JUDGE_SURFACE;
//	}
//	int nMarkSel = (nMarkSel1 || nMarkSel2);
//	switch (nMarkSel) {
//	case	0:
//		break;
//	case	1:
//		wOutput |= CAppDIO::eOut_MARK_SEL_01;
//		break;
//	case	2:
//		wOutput |= CAppDIO::eOut_MARK_SEL_01;
//		wOutput |= CAppDIO::eOut_MARK_SEL_02;
//		break;
//	}
//
//	wOutput |= CAppDIO::eOut_PULSE;
//
//	wOutput |= ( nTabId << 2 ) & CAppDIO::eInOut_ID_Mask; // 3F = 0111 111
//	return wOutput;
//}

int CCounterThread::GetTabId_FromSignal(WORD wInPutSignal)
{
	int nRet = 0; 			

	nRet = (int)(CAppDIO::eInIn_ID_Mask & (wInPutSignal >> 1) );


	return nRet;
}

void CCounterThread::ThreadRun(BOOL bRunFlag)
{
	m_bRunFlag = bRunFlag;
}


//스래드 타임아웃 시간
#define COUNTERINFOTHREAD_TIMEOUT 5
#define MAXMARKING_TIMEOUT 500
UINT CCounterThread::CtrlThreadCounter(LPVOID pParam)
{
	CCounterThread* pThis = (CCounterThread*)pParam;
	CCounterQueueCtrl* pCntQueInPtr = pThis->m_pParent->GetCounterQueInPtr();

	CAppDIO dio;
	//최종 읽은 값
	WORD wLastInfo = 0xFF;
	//DIO Trigger Bit 신호가 TRUE일 때 
	//받은 값 백업 용
	//새로 받은 Tab Id와 비교하여 누락 여부 확인
	//누락된 값 및 범위 확인
	WORD wLastTabId = 0xFF;

	//Trigger 신호가 Off 상태에서 Tab Id 가 변화가 있는 지 확인
	//Off 신호가 10번의 루프(5/1000 초 주기로 상태 확인)마다 상태 변화 확인 TriggerOffCount 가 %10으로 로그 출력
	//Trigger 신호 가 Off 상태에서 Tab Id 가 들어오는 지 확인
	WORD wLastTabIdTriggerOff = 0xFF;

#if DIO_BOARD_NO // 0이 아니면
	WORD wLastInfo_Output = 0x00;
#endif

	//다음에 찾을 TabID - ID 누력 여부 확인용
	WORD nextTabID = 255;

	//========= 마킹 정보를 보내기 위한 변수 ==============
	//마킹 Send 펄스 값을 TRUE 한 타임이 20sec 지나면 FALSE
	DWORD markingSendTimeOut = GetTickCount();
	//마킹 FALSE 시간을 10 sec 유지
	DWORD markingSendFALSETimeOut = GetTickCount();
	//마킹 유효성 여부
	bool bMarkingDataSend = false;
	//마킹 보내는 중 20 sec 후 신호 제거

	//
	BOOL bOutputBitStatus = FALSE;

	//테스트 타임 id 생성
	DWORD markingTestTimeOut = GetTickCount();

	UINT ret = 0;

	//m_nTabIdTotalCount 를 백업 해둔다.
	int nTabIdTotalCount_backup = 0;

	//스래드 주기 카운터
	DWORD ThreadLoopCount = 0;

	//ConnectZone count
	int ConnectZoneCount = 0;

	while (1)
	{
		//카운터 리셋이 일어 났을 때 0 이 된다. 
		//리셋 되면 초기화 해야할 객체를 초기화 한다.
		if ((nTabIdTotalCount_backup != 0) && (AprData.m_NowLotData.m_nTabIdTotalCount != nTabIdTotalCount_backup))
		{
			//input Id 받은 시간 목록 초기화
			m_inputReadId.clear();
			pCntQueInPtr->ResetQueue();
			nTabIdTotalCount_backup = AprData.m_NowLotData.m_nTabIdTotalCount;
		}
		//타임 주기 이벤트
		ret = WaitForSingleObject(pThis->getEvent_CounterThread(), COUNTERINFOTHREAD_TIMEOUT);
		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			break;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			if (pThis == NULL)
			{
				//DIO Input Log
				LOGDISPLAY_SPECTXT(0)(_T("DIO CCounterThread 객체 NULL"));

				break;
			}
			if (pThis->m_bKill == TRUE)
			{
				//DIO Input Log
				LOGDISPLAY_SPECTXT(0)(_T("DIO Kill"));

				break;
			}

			//스래드 주기 카운터 증가
			ThreadLoopCount++;
			if (ThreadLoopCount > 10000)
			{
				ThreadLoopCount = 1;
			}

			//마킹을 위한 플로우 추가
			//마킹을 위한 시간을 세팅
			DWORD nowTickCount = GetTickCount();

			//마킹을 보내고 있는 중일 때
			//마킹 보내는 시간은 총 Set 하고 펄스를 TRUE 만드는 시간 5초 대기 + 펄스 TRUE -> FALSE (켠상태 - 끈상태 만들기) 15초 총20초
			if (markingSendTimeOut > nowTickCount)
			{
				//마킹 데이터 유효한가?
				if (bMarkingDataSend)
				{
					if (bOutputBitStatus == FALSE)
					{
						//DIO Input Log
						LOGDISPLAY_SPEC(7)(_T("@@Loop(%d)== 마킹 정보를 쓰고 eOut_PULSE 15sec 유지한다."), ThreadLoopCount);

						dio.OutputBit(CAppDIO::eOut_PULSE, TRUE);
						bOutputBitStatus = TRUE;
						//마킹 정보를 쓰고 eOut_PULSE 15sec 유지한다.
						markingSendTimeOut = GetTickCount() + 15;
					}
					bMarkingDataSend = FALSE;
				}
			}
			else
			{
				if (bOutputBitStatus == TRUE)
				{
					//DIO Input Log
					LOGDISPLAY_SPEC(7)(_T("@@Loop(%d)== 마킹 eOut_PULSE FALSE 시간을 10 sec 유지"), ThreadLoopCount);

					dio.OutputBit(CAppDIO::eOut_PULSE, FALSE);
					bOutputBitStatus = FALSE;
					//마킹 eOut_PULSE FALSE 시간을 10 sec 유지
					markingSendFALSETimeOut = GetTickCount() + 10;
				}

				//마킹 정보를 보내고 Out_PULSE FALSE 세팅 후 10초 후
				//마킹 정보를 보내기 위한 세팅을 한다.
				if (markingSendFALSETimeOut < nowTickCount)
				{
					// 받은 input id data가 있으면
					if (m_inputReadId.size())
					{

							if (CCounterThread::m_bMarkSendInfoDataSynch == FALSE)
							{
								CCounterThread::m_bMarkSendInfoDataSynch = TRUE;
								//DIO Input Log
								LOGDISPLAY_SPEC(7)(_T("@@(%d)마킹 타임이 유효 할 때 input id size<%d>, marking data size<%d>"), ThreadLoopCount, m_inputReadId.size(), CCounterThread::m_MarkSendInfoData.size());

								for (int idx = 0; idx < (int)CCounterThread::m_MarkSendInfoData.size(); idx++)
								{
									//input id와 마킹할 id가 같으면
									if (CCounterThread::m_MarkSendInfoData[idx].bSendComplate == false)
									{
										//지울 최종 포인터
										inputReadId_iterator itdelete = m_inputReadId.end();
										//시작 점
										inputReadId_iterator it = m_inputReadId.begin();
										//end 까지 돌면서 true 인 지울 end 포인터를 백업한다.
										while (it != m_inputReadId.end())
										{
											if ((*it) == CCounterThread::m_MarkSendInfoData[idx].TabId)
											{
												itdelete = it;
												break;
											}
											it++;
										}

										//Marking Data에 대한 id를 찾았으면
										if (m_inputReadId.end() != itdelete)
										{
											//DIO Input Log
											LOGDISPLAY_SPEC(7)(_T("@@(%d)== input id와 마킹할 id가 같으면 보낸다inputid<%d>sendid<%d>"), ThreadLoopCount, m_inputReadId[0], CCounterThread::m_MarkSendInfoData[idx].TabId);

											CString strMsg;
											strMsg.Format(_T("Output Send BCD Id[%d]_OutPutValue[0x%x]"), CCounterThread::m_MarkSendInfoData[idx].TabId, CCounterThread::m_MarkSendInfoData[idx].MarkingOutputData);
											AprData.SaveMemoryLog(strMsg);

											//마킹 데이터 넣고
											dio.OutputWord(CCounterThread::m_MarkSendInfoData[idx].MarkingOutputData);
											//마킹 정보를 보내고 5sec Out_PULSE TRUE 세팅
											Sleep(1);
											//sednd 후 지우기 위한 플래그 true;
											CCounterThread::m_MarkSendInfoData[idx].bSendComplate = true;
											//마킹 보내는 타임 설정
											markingSendTimeOut = GetTickCount() + 5 + 15;
											//마킹을 보내는 플래그 설정 켜기
											bMarkingDataSend = TRUE;

											//DIO Input Log
											LOGDISPLAY_SPEC(7)(_T("@@지울 데이터가 있다면 id<%d>까지"), (*itdelete));

											//시작점 부터 true 설정된 데이터까지 지운다.
											m_inputReadId.erase(m_inputReadId.begin(), itdelete);
											//마킹 정보를 보냈으면 빠져나온다.
											break;
										}

									}
								}
								CCounterThread::m_bMarkSendInfoDataSynch = FALSE;
							}
					}
				}
				else
				{
					//DIO Input Log
					LOGDISPLAY_SPEC(7)(_T("@@Loop(%d)== Out_PULSE FALSE 세팅 후 20초 전"), ThreadLoopCount);
				}
			}

#ifdef USE_PLCCONNECTZONE

			// 입력 취득
#if DIO_BOARD_NO // 0이 아니면
			if (TRUE)
			{
#else
			CSigProc* pSigProc = theApp.m_pSigProc;
			if (pSigProc != NULL && (pSigProc->GetConnectZone() == FALSE))
			{
				ConnectZoneCount = 0;
#endif

#endif //USE_PLCCONNECTZONE

				BOOL bTriggerBit;
				dio.InputBit(CAppDIO::eIn_TRIGGER, &bTriggerBit);
				//Trigger 펄스 bit true
				if (bTriggerBit == TRUE)
				{
#if DIO_BOARD_NO // 0이 아니면
					WORD wTempID = wLastInfo_Output;
#else
					WORD wInSignal = 0x00;
					dio.InputWord(&wInSignal);

					// 22.04.06 Ahn Modify Start
					WORD wTempID;
					wTempID = 0x3F & (wInSignal >> 1);
#endif

					//이전에 받았던 id와 다르다면 추가
					if (wTempID != wLastTabId)
					{
						CLogDisplayDlg::LogDisplayDatText(_T("TriggerBCDId_Read"), _T("== Num(%d) = Trigger On Read ==== BCD Id <%d>"), ThreadLoopCount, wTempID);

						//BCD ID 얻는 시점에 TabNo는?
						int TabNo = AprData.m_NowLotData.m_nTabCount;

						//DIO Input Log
						LOGDISPLAY_SPEC(7)(_T("@@(%d)### DIO ID before<%d> ^^ now<%d> TabNo<%d>"), ThreadLoopCount, wLastTabId, wTempID, TabNo+1);

						//누락된 input 아이디를 찾는다.
						//초기값이 없다면 nextTabID 입력만
						if (nextTabID == 255)
						{
							//초기화 한다.
							pCntQueInPtr->ResetQueue();

							//Tab Use Id 초기화 세팅
							AprData.m_NowLotData.m_bInitTabId = TRUE;

							nextTabID = wTempID + 1;
							if (nextTabID >= 64)
							{
								nextTabID = 0;
							}
							
							//메모리 로그 기록
							CString strMsg = "";
							strMsg.Format(_T("First BCD Id Use BCD Id<%d>"), wTempID);
							AprData.SaveMemoryLog(strMsg);
						}
						//초기값 세팅 상태에서 만 검사한다.
						else
						{
							//다음 들어올 ID와 받은 ID가 다르다면
							//누락 로그 출력한다.
							if (nextTabID != wTempID)
							{
								//얻은 Tab Id 범위 확인용
								if ((wTempID >= 0) && (wTempID < 64))
								{
									//누락 Tab id Total 증가를 위한 세팅
									//Tab Id 누락 된 카운트 증가를 위해서 백업한다.
									int nextTabIDbackup = nextTabID;
									int nextTabIDbackupLog = nextTabID;
									//누락이 1개 일어 났을 때 마킹 정보를 바로 보낸다.
									int omissCount = 0;
									while (nextTabIDbackup != wTempID)
									{
										//누락된 카운트를 올린다.
										omissCount++;

										//DIO Input Log
										LOGDISPLAY_SPEC(7)(_T("@@(%d)### 누락 BCD ID [%d] 누락 갯수<%d>"), ThreadLoopCount, nextTabIDbackup, omissCount);

										//메모리 로그 기록
										CString strMsg;
										strMsg.Format(_T("Lose input BCD ID  [%d]"), nextTabIDbackup);
										AprData.SaveMemoryLog(strMsg);

										nextTabIDbackupLog = nextTabIDbackup;
										//다음 id로 증가 및 유효 카운트 검사
										nextTabIDbackup++;
										if (nextTabIDbackup >= 64)
											nextTabIDbackup = 0;

									}
									//누락 카운트가 0으로 초기화 한 데이터 일경우 카운터하지 않는다.
									//Tab Id Queue도 초기화 한다.
									if ((wTempID == 0) && (omissCount > 5))
									{
										//초기화 한다.
										pCntQueInPtr->ResetQueue();

										//Tab Use Id 초기화 세팅
										AprData.m_NowLotData.m_bInitTabId = TRUE;
										//DIO Input Log
										LOGDISPLAY_SPEC(7)(_T("@@(%d)### Input ID 초기화 TabId[%d]- ResetQ - BeforId[%d]"), ThreadLoopCount, wTempID, wLastTabId);

										//메모리 로그 기록
										CString strMsg = "";
										strMsg.Format(_T("Over All Vision BCD Id Initialize Before BCD Id<%d>=Init BCD Id<%d>"), wLastTabId, wTempID);
										AprData.SaveMemoryLog(strMsg);
									}
									//초기화가 아닌 경우
									else
									{
										//DIO Input Log
										LOGDISPLAY_SPEC(7)(_T("@@(%d)### Input ID 누락 카운트 추가 지금 카운트[%d]- 추가[%d] - 추가된 카운트[%d]"), ThreadLoopCount, AprData.m_NowLotData.m_nTabIdTotalCount, omissCount, (AprData.m_NowLotData.m_nTabIdTotalCount + omissCount));

										AprData.m_NowLotData.m_nTabIdTotalCount += omissCount;										
									}
									//Tab id Count 백업
									nTabIdTotalCount_backup = AprData.m_NowLotData.m_nTabIdTotalCount;

									CLogDisplayDlg::LogDisplayDatText(_T("TriggerBCDId_Read"), _T("== Num(%d) = BCD Id Lost from <%d> to <%d> "), ThreadLoopCount, nextTabID, nextTabIDbackupLog);

								}

								LOGDISPLAY_SPEC(7)(_T("@@Loop(%d) = Input ID 누락 before<%d> ^^ now<%d>"), ThreadLoopCount, wLastTabId, wTempID);
							}
							//다음에 받을 ID를 세팅한다.
							nextTabID = wTempID + 1;
							//64 이상이면 0으로 
							if (nextTabID >= 64)
							{
								nextTabID = 0;
							}
						}

						CCounterInfo cntInfo;
						//Tab Id 
						cntInfo.nTabID = wTempID;
						//제일 마지막 받은 BCD ID 
						AprData.m_NowLotData.m_nLastBCDId = cntInfo.nTabID;
						//Tab No(번호)
						cntInfo.nTabNo = TabNo;
						//Tab Total Count 
						//Tab Total Count를 증가 시킨다.
						AprData.m_NowLotData.m_nTabIdTotalCount++;
						cntInfo.nTabIdTotalCount = AprData.m_NowLotData.m_nTabIdTotalCount;

						//Tab Id 정보를 추가한다.
						//pCntQueInPtr->PushBack(cntInfo);

						//Tab id Count 백업
						nTabIdTotalCount_backup = AprData.m_NowLotData.m_nTabIdTotalCount;
						
						//받은 id
						//m_inputReadId.push_back(wTempID);
						//마킹정보 들어오는 카운트 수
						CCounterThread::deepSwitchOff = 0;


						//이전 id 갱신
#if DIO_BOARD_NO // 0이 아니면
						wLastInfo = wTempID;
						wLastTabId = wTempID;
#else
						wLastInfo = wTempID;
						wLastTabId = wTempID;
#endif
						int nCntQueSize = pCntQueInPtr->GetSize();

						//메모리 로그 기록
						CString strMsg;
						strMsg.Format(_T("Input BCD ID[%d], BCD ID BuffSize<%d> Receive "), cntInfo.nTabID, nCntQueSize);
						AprData.SaveMemoryLog(strMsg);

						//DIO Input Log
						LOGDISPLAY_SPEC(7)(_T("@@(%d)### Input ID Add TabId[%d],TotalCount[%d],Queue Count<%d>"), ThreadLoopCount, cntInfo.nTabID, cntInfo.nTabIdTotalCount, nCntQueSize);

						if (nCntQueSize >= FRAME_ACQ_ERROR_CHK_CNT)
						{
							// 에러 처리 : BCD ID는 들어오는데 Frame이 없거나 Process 처리 문제로 BCD를 사용하지 못하고 쌓이는 경우 에러
							CString strErrMsg;
							strErrMsg.Format(_T("Frame Error : BCD Que Over[%d>=%d], Process End!!!!"), nCntQueSize, FRAME_ACQ_ERROR_CHK_CNT );
							AprData.SaveErrorLog(strErrMsg);
							AprData.SaveDebugLog_Format(strErrMsg);

							//카메라의 에러를 세팅한다.
							//AprData.m_ErrStatus.SetError(CErrorStatus::en_CameraError, strErrMsg);
						}




					}
					// Cell 추적 Queue Data -> Local Queue 
					//if(....)
				}
				//Trigger 펄스 bit false
				else
				{
					WORD wInSignal = 0x00;
					dio.InputWord(&wInSignal);

					// 22.04.06 Ahn Modify Start
					WORD wTempID;
					wTempID = 0x3F & (wInSignal >> 1);

					if (wLastTabIdTriggerOff != wTempID)
					{
						CLogDisplayDlg::LogDisplayDatText(_T("TriggerBCDId_Read"), _T("== Num(%d) = Trigger Off Read ==== BCD Id <%d>"), ThreadLoopCount, wTempID);

						//DIO Input Log
						LOGDISPLAY_SPEC(7)(_T("@@(%d)### Trigger Off Read input Tabid<%d><%d>"), ThreadLoopCount, wTempID, wLastTabIdTriggerOff);

						wLastTabIdTriggerOff = wTempID;
					}

#if DIO_BOARD_NO // 0이 아니면
					if (wLastInfo_Output == wLastInfo)
					{
						++wLastInfo_Output;
						if (wLastInfo_Output >= 64)
						{
							wLastInfo_Output = 0;
						}
					}
#endif

				}
#ifdef USE_PLCCONNECTZONE
			}
			else
			{
				//ConnectZone 세팅 상태 체크
				AprData.m_NowLotData.m_bConnectZone = TRUE;

				ConnectZoneCount++;
				//DIO Input Log
				LOGDISPLAY_SPEC(7)(_T("@@(%d)### ConnectZone <%d>==== "), ThreadLoopCount, ConnectZoneCount);
				//Connect Zone 상태일 때 버퍼를 비운다.
				pCntQueInPtr->ResetQueue();

				//마킹 하기위한 값 제거
				if (inputReadId.size())
				{
					inputReadId.clear();
				}

				WORD wInSignal = 0x00;
				dio.InputWord(&wInSignal);

				// 22.04.06 Ahn Modify Start
				WORD wTempID;
				wTempID = 0x3F & (wInSignal >> 1);

				if (wLastInfo != wTempID)
				{
					//DIO Input Log
					LOGDISPLAY_SPEC(7)(_T("@@(%d)### ConnectZone input Tabid<%d>"), ThreadLoopCount, wTempID);

					wLastInfo = wTempID;
				}
				
			}
#endif //USE_PLCCONNECTZONE

		}
		else
		{
			break;
		}

	}

	//Tab Id 정보 로그
	LOGDISPLAY_SPEC(7)("@@Thread **** CtrlThreadCounter 종료됩니다 @@@@ ");

	return 0;
}


int CCounterThread::ConnectTrigger(const CString& ip, int port, int mode)
{
	if (m_TriggerSocket != NULL) {
		delete m_TriggerSocket;
		m_TriggerSocket = NULL;
	}
	m_TriggerSocket = new CTriggerSocket(ip, mode, this);

	int	bRet;
	int	errorcode = 0;
	if (mode == CTriggerSocket::TCP_MODE) {
		if (m_TriggerSocket->Create() == 0) {
			CString strError;
			DWORD dwErrCode = ::GetLastError();
			strError = FormatErrorMsg(dwErrCode);
			//DEBUG_LOG.txt
			AprData.SaveDebugLog_Format(_T("<<CLightSocket>>에러 - strError<%s>"), strError);

			return (-1);
		}
		bRet = m_TriggerSocket->Connect(ip, port);
	}
	else {
		if (m_TriggerSocket->Create(port, SOCK_DGRAM, FD_READ) == 0) {
			//			errorcode = GetLastError() ;
			CString strError;
			strError = ::FormatErrorMsg(::GetLastError());
			return (-1);
		}
		bRet = m_TriggerSocket->Connect(ip, port);
	}

	if (bRet == FALSE) {
		DWORD dwErrorCode = GetLastError();
		CString	strErMsg = _T("");
		switch (dwErrorCode) {
		case	WSANOTINITIALISED:
			strErMsg.Format(_T("이 API를 사용하기 전에 AfxSocketInit 호출이 성공적인 완료가 필요합니다."));
			break;
		case	WSAENETDOWN:
			strErMsg.Format(_T("Windows 소켓 구현이 네트워크 서브 시스템의 이상을 검출했습니다."));
			break;
		case	WSAEADDRINUSE:
			strErMsg.Format(_T("지정한 주소는 사용중입니다."));
			break;
		case	WSAEINPROGRESS:
			strErMsg.Format(_T("실행중인 Windows 소켓 작업이 차단되어 있습니다."));
			break;
		case	WSAEADDRNOTAVAIL:
			strErMsg.Format(_T("지정된 주소는 로컬 컴퓨터에서 사용할 수 없습니다."));
			break;
		case	WSAEAFNOSUPPORT:
			strErMsg.Format(_T("지정한 주소 제품군이 소켓에서 지원하지 않습니다."));
			break;
		case	WSAECONNREFUSED:
			strErMsg.Format(_T("연결을 시도했으나 거부되었습니다."));
			break;
		case	WSAEDESTADDRREQ:
			strErMsg.Format(_T("목적지 주소가 필요합니다."));
			break;
		case	WSAEFAULT:
			strErMsg.Format(_T("인수 SockAddr_in가 잘못되었습니다."));
			break;
		case	WSAEINVAL:
			strErMsg.Format(_T("호스트 주소가 잘못되었습니다."));
			break;
		case	WSAEISCONN:
			strErMsg.Format(_T("소켓은 이미 연결되어 있습니다."));
			return(0); //break ;
		case	WSAEMFILE:
			strErMsg.Format(_T("유효한 파일 디스크립터가 아닙니다."));
			break;
		case	WSAENETUNREACH:
			strErMsg.Format(_T("현재 호스트에서 네트워크에 연결할 수 없습니다."));
			break;
		case	WSAENOBUFS:
			strErMsg.Format(_T("사용가능한 버퍼 공간이 없습니다. 소켓을 연결할 수 없습니다."));
			break;
		case	WSAENOTSOCK:
			strErMsg.Format(_T("디스크립터가 소켓이 아닙니다."));
			break;
		case	WSAETIMEDOUT:
			strErMsg.Format(_T("연결을 시도했지만 시간에 연결할 수 없습니다."));
			break;
		case	WSAEWOULDBLOCK:
			return (0);
		default:
			strErMsg.Format(_T("소켓 오류：%lu"), (DWORD)dwErrorCode);
			break;
		}
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<CLightSocket Connect Error>>에러 - strErMsg<%s>"), strErMsg);

		return (-1);
	}
	return 0;
}