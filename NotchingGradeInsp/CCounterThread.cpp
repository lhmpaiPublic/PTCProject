#include "pch.h"
#include "CCounterThread.h"
#include "ImageProcessCtrl.h"
#include "AppDIO.h"
#include "CCounterQueueCtrl.h"
#include "GlobalData.h"
#include "SigProc.h"

std::vector<MarkSendInfo> CCounterThread::m_MarkSendInfoData;
typedef std::vector<MarkSendInfo>::iterator MarkSendInfoData_iterator;
BOOL CCounterThread::m_bMarkSendInfoDataSynch = FALSE;

void CCounterThread::MarkSendInfo_Push_back(int TabId, WORD MarkingOutputData, bool bSendComplate)
{
	if ((TabId >= 0) && (TabId < 64))
	{
		//push 하기 전에 보낸 데이터를 지운다.
		//이전 마킹 데이터가 있을 때
		if (CCounterThread::m_bMarkSendInfoDataSynch == FALSE)
		{
			CCounterThread::m_bMarkSendInfoDataSynch = TRUE;
			if (CCounterThread::m_MarkSendInfoData.size())
			{
				//DIO Input Log
				LOGDISPLAY_SPEC(7)(_T("이전 마킹 데이터가 있을 때"));

				//지울 최종 포인터
				MarkSendInfoData_iterator itdelete = CCounterThread::m_MarkSendInfoData.begin();
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
					LOGDISPLAY_SPEC(7)(_T("지울 데이터가 있다면size<%d> id<%d>까지"), CCounterThread::m_MarkSendInfoData.size(), itdelete->TabId);

					//시작점 부터 true 설정된 데이터까지 지운다.
					CCounterThread::m_MarkSendInfoData.erase(CCounterThread::m_MarkSendInfoData.begin(), itdelete);
				}
			}
			CCounterThread::m_bMarkSendInfoDataSynch = FALSE;
		}

		//저장한다.
		MarkSendInfo MarkSendInfoData;
		MarkSendInfoData.TabId = TabId;
		MarkSendInfoData.MarkingOutputData = MarkingOutputData;
		MarkSendInfoData.bSendComplate = bSendComplate;
		CCounterThread::m_MarkSendInfoData.push_back(MarkSendInfoData);
		//DIO Input Log
		LOGDISPLAY_SPEC(7)(_T("마킹 데이터 id<%d>OutputData<%d>"), TabId, bSendComplate);
	}

}

CCounterThread::CCounterThread(CImageProcessCtrl* pParent)
{
	m_pParent = pParent;
	m_MarkSendInfoData.clear();
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
	CCounterQueueCtrl* pCntQueInPtr = pThis->m_pParent->GetCounterQueInPtr() ;

	CAppDIO dio;
	//최종 읽은 값
	WORD wLastInfo = 0x00;

#if DIO_BOARD_NO // 0이 아니면
	WORD wLastInfo_Output = 0x00;
#endif

	//Trigger 펄스 값 읽기 위한 변수
	WORD backupwInSignal = 0x00;
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

	//마킹 input ID 및 읽었을 때 TickTime
	std::vector<int> inputReadId;
	std::vector<DWORD> inputIdReadTime;
	//
	BOOL bOutputBitStatus = FALSE;

	//테스트 타임 id 생성
	DWORD markingTestTimeOut = GetTickCount();

	UINT ret = 0;
	while (1) 
	{
		//타임 주기 이벤트
		ret = WaitForSingleObject(pThis->getEvent_CounterThread(), COUNTERINFOTHREAD_TIMEOUT);
		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return 0;
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
			if (pThis->m_bRunFlag == FALSE)
			{
				continue;
			}

			//마킹을 위한 플로우 추가
			//마킹을 위한 시간을 세팅
			DWORD nowTickCount = GetTickCount();

			//마킹을 보내고 있는 중일 때
			//마킹 보내는 시간은 총 Set 하고 펄스를 TRUE 만드는 시간 5초 대기 + 펄스 TRUE -> FALSE (켠상태 - 끈상태 만들기) 15초 총20초
			if (markingSendTimeOut > nowTickCount)
			{
				//DIO Input Log
				LOGDISPLAY_SPEC(7)(_T("DIO test markingSendTimeOut Enter"));

				//마킹 데이터 유효한가?
				if (bMarkingDataSend)
				{
					if (bOutputBitStatus == FALSE)
					{
						//DIO Input Log
						LOGDISPLAY_SPEC(7)(_T("마킹 정보를 쓰고 eOut_PULSE 15sec 유지한다."));

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
					LOGDISPLAY_SPEC(7)(_T("마킹 eOut_PULSE FALSE 시간을 10 sec 유지"));

					dio.OutputBit(CAppDIO::eOut_PULSE, FALSE);
					bOutputBitStatus = FALSE;
					//마킹 eOut_PULSE FALSE 시간을 10 sec 유지
					markingSendFALSETimeOut = GetTickCount() + 10;
				}

				//마킹 정보를 보내고 Out_PULSE FALSE 세팅 후 20초 후
				//마킹 정보를 보내기 위한 세팅을 한다.
				if (markingSendFALSETimeOut < nowTickCount)
				{
					// 받은 input id data가 있으면
					if (inputIdReadTime.size() && inputReadId.size())
					{
						//마킹 타임이 지났을 때
						if (inputIdReadTime[0] < nowTickCount)
						{
							//DIO Input Log
							LOGDISPLAY_SPEC(7)(_T("input id가 time out 된 상태면 두개를 쌍으로 지운다."));

							//input id가 time out 된 상태면 두개를 쌍으로 지운다.
							inputIdReadTime.erase(inputIdReadTime.begin());
							inputReadId.erase(inputReadId.begin());

						}
						//마킹 타임이 유효 할 때
						else
						{
							if (CCounterThread::m_bMarkSendInfoDataSynch == FALSE)
							{
								CCounterThread::m_bMarkSendInfoDataSynch = TRUE;
								//DIO Input Log
								LOGDISPLAY_SPEC(7)(_T("마킹 타임이 유효 할 때"));

								for (int idx = 0; idx < (int)CCounterThread::m_MarkSendInfoData.size(); idx++)
								{
									//input id와 마킹할 id가 같으면
									if (CCounterThread::m_MarkSendInfoData[idx].TabId == inputReadId[0])
									{
										//DIO Input Log
										LOGDISPLAY_SPEC(7)(_T("input id와 마킹할 id가 같으면 보낸다inputid<%d>sendid<%d>"), inputReadId[0], CCounterThread::m_MarkSendInfoData[idx].TabId);

										//마킹 데이터 넣고
										dio.OutputWord(CCounterThread::m_MarkSendInfoData[idx].MarkingOutputData);
										//마킹 정보를 보내고 5sec Out_PULSE TRUE 세팅
										Sleep(2);
										//sednd 후 지우기 위한 플래그 true;
										CCounterThread::m_MarkSendInfoData[idx].bSendComplate = true;
										//마킹 보내는 타임 설정
										markingSendTimeOut = GetTickCount() + 5 + 15;
										//마킹을 보내는 플래그 설정 켜기
										bMarkingDataSend = TRUE;

										//두개를 쌍으로 지운다.
										inputIdReadTime.erase(inputIdReadTime.begin());
										inputReadId.erase(inputReadId.begin());

										break;
									}
									//input id와 마킹할 id가 다르면
									else
									{
										//DIO Input Log
										LOGDISPLAY_SPEC(7)(_T("input id와 마킹할 id가 다르면inputid<%d>sendid<%d>"), inputReadId[0], CCounterThread::m_MarkSendInfoData[idx].TabId);

									}
								}
								CCounterThread::m_bMarkSendInfoDataSynch = FALSE;
							}
						}

					}
					else
					{
						//DIO Input Log
						LOGDISPLAY_SPEC(7)(_T("받은 input id 정보가 없다"));
					}
				}
				else
				{
					//DIO Input Log
					LOGDISPLAY_SPEC(7)(_T("Out_PULSE FALSE 세팅 후 20초 전"));
				}
			}

			////마킹 테스트 input ID 생성 코드
			//if (markingTestTimeOut < nowTickCount)
			//{
			//	if (nextTabID == 255)
			//	{
			//		nextTabID = 0;
			//	}
			//	//초기값 세팅 상태에서 만 검사한다.
			//	else
			//	{
			//		//다음에 받을 ID를 세팅한다.
			//		nextTabID++;
			//		//64 이상이면 0으로 
			//		if (nextTabID >= 64)
			//		{
			//			nextTabID = 0;
			//		}
			//	}
			//	CCounterInfo cntInfo;
			//	cntInfo.nTabID = nextTabID;
			//	pCntQueInPtr->PushBack(cntInfo);

			//	//Id  받은 시간
			//	inputIdReadTime.push_back(GetTickCount() + MAXMARKING_TIMEOUT);
			//	//받은 id
			//	inputReadId.push_back(nextTabID);

			//	markingTestTimeOut = GetTickCount() + 120;

			//	//DIO Input Log
			//	LOGDISPLAY_SPEC(7)(_T("DIO test id<%d>time<%d>"), nextTabID, inputIdReadTime[inputIdReadTime.size()-1]);
			//}

			// 입력 취득
			if(pThis->m_pParent->IsInspection() == TRUE)
			{
				CSigProc* pSigProc = theApp.m_pSigProc;
				if (pSigProc != NULL && (pSigProc->GetConnectZone() == FALSE))
				{

					BOOL bTriggerBit;
					dio.InputBit(CAppDIO::eIn_TRIGGER, &bTriggerBit);
					//Trigger 펄스 bit true
					if (bTriggerBit == TRUE)
					{
						WORD wInSignal = 0x00;
						dio.InputWord(&wInSignal);

						// 22.04.06 Ahn Modify Start
						WORD wTempID;
						wTempID = 0x3F & (wInSignal >> 1);

						//이전에 받았던 id와 다르다면 추가
						if (wTempID != wLastInfo)
						{

							//DIO Input Log
							LOGDISPLAY_SPEC(7)(_T("DIO Signal Word before<%d> now<%d>"), backupwInSignal, wInSignal);
							backupwInSignal = wInSignal;

							//DIO Input Log
							LOGDISPLAY_SPEC(7)(_T("*0**DIO ID before<%d> now<%d>"), wLastInfo, wTempID);

							//만약에 input id 저장소와 시간 저장소의 크기가 다르면 모두 지운다.
							if (inputIdReadTime.size() != inputReadId.size())
							{
								inputIdReadTime.clear();
								inputReadId.clear();
							}

							//누락된 input 아이디를 찾는다.
							//초기값이 없다면 nextTabID 입력만
							if (nextTabID == 255)
							{
								nextTabID = wTempID + 1;
								if (nextTabID >= 64)
								{
									nextTabID = 0;
								}
							}
							//초기값 세팅 상태에서 만 검사한다.
							else
							{
								//다음 들어올 ID와 받은 ID가 다르다면
								//누락 로그 출력한다.
								if (nextTabID != wTempID)
								{
									//ID가 누락 되었다면 
									//Id  누락 시간은 바로 빼기 위해서
									inputIdReadTime.push_back(GetTickCount());
									//누락 id
									inputReadId.push_back(nextTabID);

									//메모리 로그 기록
									CString strMsg;
									strMsg.Format(_T("Input ID [%d] 누락"), nextTabID);
									AprData.SaveMemoryLog(strMsg);

									//DIO Input Log
									LOGDISPLAY_SPEC(0)(_T("Input ID [%d] 누락"), nextTabID);
								}
								//다음에 받을 ID를 세팅한다.
								nextTabID = wTempID + 1;
								//64 이상이면 0으로 
								if (nextTabID >= 64)
								{
									nextTabID = 0;
								}
							}

							int beforeQueueCount = pCntQueInPtr->GetSize();
							CCounterInfo cntInfo;
							cntInfo.nTabID = wTempID;
							pCntQueInPtr->PushBack(cntInfo);

							//Id  받은 시간
							inputIdReadTime.push_back(GetTickCount() + MAXMARKING_TIMEOUT);
							//받은 id
							inputReadId.push_back(wTempID);

							//이전 id 갱신
#if DIO_BOARD_NO // 0이 아니면
							wLastInfo_Output = wLastInfo = wTempID;
#else
							wLastInfo = wTempID;
#endif
							

							//메모리 로그 기록
							CString strMsg;
							strMsg.Format(_T("Input ID[%d], Queue Count<%d>-><%d>"), cntInfo.nTabID, beforeQueueCount, pCntQueInPtr->GetSize());
							AprData.SaveMemoryLog(strMsg);

						}
						// Cell 추적 Queue Data -> Local Queue 
						//if(....)
					}
					//Trigger 펄스 bit false
					else
					{

#if DIO_BOARD_NO // 0이 아니면
						if (wLastInfo_Output == wLastInfo)
						{
							++wLastInfo_Output;
							if (wLastInfo_Output >= 64)
							{
								wLastInfo_Output = 0;
							}
							WORD wInSignal = wLastInfo_Output<<1;
							LOGDISPLAY_SPEC(7)(_T("DIO ID Output <%d> Signal <%d>"), wLastInfo_Output, wInSignal);
							dio.OutputWord(wInSignal);
						}
#endif
						
					}
				}
				else
				{
					//Connect Zone 상태일 때 버퍼를 비운다.
					while (pCntQueInPtr->GetSize())
						pCntQueInPtr->Pop();
					//DIO Input Log
					LOGDISPLAY_SPECTXT(1)(_T("Input TabID 무시 PLC 신호 == ConnectZone"));
				}
			}
			else
			{
				//DIO Input Log
				LOGDISPLAY_SPECTXT(1)(_T("DIO Check Image Porcess Insp FALSE 상태"));
			}

		}
		else
		{
			break;
		}

	}

	return 0;
}
