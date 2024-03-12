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

__int64 CCounterThread::deepSwitchOff = 0;

std::vector<int> CCounterThread::m_inputReadId;

CRITICAL_SECTION CCounterThread::m_csQueueReadId;

//마킹 데이터 동기화
CRITICAL_SECTION CCounterThread::m_csQueueMarkingData;

typedef std::vector<int>::iterator inputReadId_iterator;

//DIO 객체
CAppDIO m_dio;

void CCounterThread::MarkSendInfo_Push_back(int TabId, WORD MarkingOutputData, bool bSendComplate)
{
	if ((TabId >= 0) && (TabId < 64))
	{
		//마킹 정보를 저장하기 위해서 들어오는 카운트
		CCounterThread::deepSwitchOff++;

		//딮 스위치를 꺼서 Trigger BCD Id를 계속 못받으면 
		//CCounterThread::deepSwitchOff 카운터가 증가하고 10이상이면 계속적으로 안들어온다 생각으로 
		if (CCounterThread::deepSwitchOff < 10)
		{
			//저장한다.
			MarkSendInfo MarkSendInfoData;
			MarkSendInfoData.TabId = TabId;
			MarkSendInfoData.MarkingOutputData = MarkingOutputData;
			MarkSendInfoData.bSendComplate = bSendComplate;

			::EnterCriticalSection(&m_csQueueMarkingData);
			CCounterThread::m_MarkSendInfoData.push_back(MarkSendInfoData);
			::LeaveCriticalSection(&m_csQueueMarkingData);


			//DIO Input Log
			LOGDISPLAY_SPEC(7)(_T("@@마킹 데이터 추가 id	%d	OutputData	%d"), TabId, MarkingOutputData);
		}
		else
		{
			if (CCounterThread::m_MarkSendInfoData.size())
			{
				//DIO Input Log
				LOGDISPLAY_SPEC(7)(_T("@@마킹 데이터 전체 삭제"));
				//마킹 데이터 전체 삭제
				//시작 점
				MarkSendInfoData_iterator it = CCounterThread::m_MarkSendInfoData.begin();
				while (it != CCounterThread::m_MarkSendInfoData.end())
				{
					LOGDISPLAY_SPEC(7)(_T("@@마킹 데이터 삭제 id	%d	OutputData	%d"), it->TabId, it->MarkingOutputData);
					it++;
				}
				
				::EnterCriticalSection(&m_csQueueMarkingData);
				CCounterThread::m_MarkSendInfoData.clear();
				::LeaveCriticalSection(&m_csQueueMarkingData);
			}
		}
	}

}

int CCounterThread::GetInputReadId()
{
	int BCDIdVal = -1;

	//받은 id
	::EnterCriticalSection(&m_csQueueReadId);
	if (m_inputReadId.size())
	{
		BCDIdVal = m_inputReadId[m_inputReadId.size()-1];
	}
	::LeaveCriticalSection(&m_csQueueReadId);

	return BCDIdVal;
}

void CCounterThread::RecivePacket(char* data, int len)
{

	//BCD ID 얻는 시점에 TabNo는?
	int TabNo = AprData.m_NowLotData.m_nTabCount;
	if ((AprData.m_NowLotData.m_nTabIdTotalCount > (TabNo + 10)) || (AprData.m_NowLotData.m_nTabIdTotalCount < (TabNo - 10)))
	{
		AprData.m_NowLotData.m_nTabIdTotalCount = TabNo;
	}

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

			//누락된 input 아이디를 찾는다.
			//초기값이 없다면 nextTabID 입력만
			if (m_nextTabID == 255)
			{
				//Tab Use Id 초기화 세팅
				AprData.m_NowLotData.m_bInitTabId = TRUE;

				m_nextTabID = nID + 1;
				if (m_nextTabID >= 64)
				{
					m_nextTabID = 0;
				}
			}
			else
			{
				//다음 들어올 ID와 받은 ID가 다르다면
							//누락 로그 출력한다.
				if (m_nextTabID != nID)
				{
					//얻은 Tab Id 범위 확인용
					if ((nID >= 0) && (nID < 64))
					{
						//누락 Tab id Total 증가를 위한 세팅
						//Tab Id 누락 된 카운트 증가를 위해서 백업한다.
						int nextTabIDbackup = m_nextTabID;
						//누락이 1개 일어 났을 때 마킹 정보를 바로 보낸다.
						int omissCount = 0;
						while (nextTabIDbackup != nID)
						{
							//누락된 카운트를 올린다.
							omissCount++;

							//DIO Input Log
							LOGDISPLAY_SPEC(7)(_T("누락 BCD ID	%d	누락 갯수	%d"), nextTabIDbackup, omissCount);

							//디버그 로그 기록
							AprData.SaveDebugLog_Format(_T("Lose BCD ID	%d"), nextTabIDbackup);

							//다음 id로 증가 및 유효 카운트 검사
							nextTabIDbackup++;
							if (nextTabIDbackup >= 64)
								nextTabIDbackup = 0;

						}
						//누락 카운트가 0으로 초기화 한 데이터 일경우 카운터하지 않는다.
						//Tab Id Queue도 초기화 한다.
						if ((nID == 0) && (omissCount > 5))
						{
							//Tab Use Id 초기화 세팅
							AprData.m_NowLotData.m_bInitTabId = TRUE;
							//DIO Input Log
							LOGDISPLAY_SPEC(7)(_T("BCD ID 초기화 TabId	%d	ResetQ - Befor Id	%d"), nID, m_wLastTabId);

						}
						//초기화가 아닌 경우
						else
						{
							//DIO Input Log
							LOGDISPLAY_SPEC(7)(_T("BCD ID 누락 카운트 추가 지금 카운트	%d	- 추가	%d"), AprData.m_NowLotData.m_nTabIdTotalCount, omissCount);

							AprData.m_NowLotData.m_nTabIdTotalCount += omissCount;
						}

					}

					LOGDISPLAY_SPEC(7)(_T("BCD ID 누락 before	%d	now	%d"), m_wLastTabId, nID);
				}
				//다음에 받을 ID를 세팅한다.
				m_nextTabID = nID + 1;
				//64 이상이면 0으로 
				if (m_nextTabID >= 64)
				{
					m_nextTabID = 0;
				}
			}

			//제일 마지막 받은 BCD ID 
			AprData.m_NowLotData.m_nLastBCDId = nID;

			//BCD ID input time
			AprData.m_NowLotData.m_nBCDIDInputTime = GetTickCount();

			//Tab Total Count 
			//Tab Total Count를 증가 시킨다.
			AprData.m_NowLotData.m_nTabIdTotalCount++;

			AprData.m_NowLotData.m_uEncoderCount = nEncodeCnt;

			LOGDISPLAY_SPEC(11)(_T("FT1	ID:	%d	Encode Count	%d	TabTotal	%d	TabNo	%d"), nID, nEncodeCnt, AprData.m_NowLotData.m_nTabIdTotalCount, TabNo);

			AprData.SaveDebugLog_Format(_T("ID	%d, Encode Count	%d"), nID, nEncodeCnt);

			//받은 id
			::EnterCriticalSection(&m_csQueueReadId);
			m_inputReadId.push_back(nID);
			::LeaveCriticalSection(&m_csQueueReadId);

			//이전 받은 BCD iD
			m_nextTabID = 0;
			m_wLastTabId = nID;

			//마킹정보 들어오는 카운트 수
			CCounterThread::deepSwitchOff = 0;

		}
	}

}

CCounterThread::CCounterThread(CImageProcessCtrl* pParent)
{
	::InitializeCriticalSection(&m_csQueueReadId);

	::InitializeCriticalSection(&m_csQueueMarkingData);

	m_pParent = pParent;
	m_MarkSendInfoData.clear();
	m_TriggerSocket = NULL;

	//========= 마킹 정보를 보내기 위한 변수 초기화==============
	//마킹 Send 펄스 값을 TRUE 한 타임이 20sec 지나면 FALSE
	m_markingSendTimeOut = GetTickCount();
	//마킹 FALSE 시간을 10 sec 유지
	m_markingSendFALSETimeOut = GetTickCount();
	//마킹 유효성 여부
	m_bMarkingDataSend = false;
	//마킹 보내는 중 20 sec 후 신호 제거

	m_bOutputBitStatus = FALSE;

	//테스트 타임 id 생성
	m_markingTestTimeOut = GetTickCount();

	//다음에 찾을 TabID - ID 누력 여부 확인용
	m_nextTabID = 255;

	//DIO Trigger Bit 신호가 TRUE일 때 
	//받은 값 백업 용
	//새로 받은 Tab Id와 비교하여 누락 여부 확인
	//누락된 값 및 범위 확인
	m_wLastTabId = 0xFF;

}
CCounterThread::~CCounterThread()
{
	if (m_TriggerSocket)
	{
		delete m_TriggerSocket;
		m_TriggerSocket = NULL;
	}
	::DeleteCriticalSection(&m_csQueueReadId);

	::DeleteCriticalSection(&m_csQueueMarkingData);
}
void CCounterThread::Begin()
{
	m_bKill = FALSE;

//Encoder Counter 사용여부
#ifdef USE_BCDCOUNTER
	ConnectTrigger(_T("192.168.0.198"), 34700);
#endif //USE_BCDCOUNTER

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
	//스래드 생성 객체
	CCounterThread* pThis = (CCounterThread*)pParam;

	UINT ret = 0;
	while (1)
	{
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

//Tab Counter 사용 여부
#ifndef USE_BCDCOUNTER
			//트리거의 On 신호에 BCD ID를 읽기 위한 함수
			pThis->readTriggerBCDID();
#endif //USE_BCDCOUNTER

			//마킹 처리를 위한 함수
			pThis->MarkingProcess();

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
	if (m_TriggerSocket != NULL) 
	{
		delete m_TriggerSocket;
		m_TriggerSocket = NULL;
	}
	m_TriggerSocket = new CTriggerSocket(ip, mode, this);

	int	bRet;

	if (mode == CTriggerSocket::TCP_MODE)
	{
		if (m_TriggerSocket->Create() == 0) 
		{
			CString strError;
			DWORD dwErrCode = ::GetLastError();
			strError = FormatErrorMsg(dwErrCode);

			//Tab Id 정보 로그
			LOGDISPLAY_SPEC(11)("FT3	Socket Create ERROR	:	%s ", strError);

			return (-1);
		}
		bRet = m_TriggerSocket->Connect(ip, port);
	}
	else 
	{
		if (m_TriggerSocket->Create(port, SOCK_DGRAM, FD_READ) == 0) 
		{
			CString strError;
			strError = ::FormatErrorMsg(::GetLastError());

			LOGDISPLAY_SPEC(11)("FT3	Socket Create ERROR	:	%s ", strError);
			return (-1);
		}
		bRet = m_TriggerSocket->Connect(ip, port);
	}

	if (bRet == 0) 
	{
		if (m_TriggerSocket != NULL)
		{
			delete m_TriggerSocket;
			m_TriggerSocket = NULL;
		}

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
		//Tab Id 정보 로그
		LOGDISPLAY_SPEC(11)("FT3	ERROR	Socket Connect :	%s ", strErMsg);

		return (-1);
	}
	return 0;
}

//마킹처리를 위한 함수
BOOL CCounterThread::MarkingProcess()
{
	BOOL b = TRUE;
	//보낸 마킹 정보 삭제 하기
	if (CCounterThread::m_MarkSendInfoData.size())
	{
		//지울 최종 포인터
		MarkSendInfoData_iterator itdelete = CCounterThread::m_MarkSendInfoData.end();
		//시작 점
		MarkSendInfoData_iterator it = CCounterThread::m_MarkSendInfoData.begin();
		//end 까지 돌면서 true 인 지울 end 포인터를 백업한다.
		while (CCounterThread::m_MarkSendInfoData.end() != it)
		{
			if (it->bSendComplate == true)
			{
				itdelete = it;
			}
			it++;
		}

		//지울 데이터가 있다면
		if (CCounterThread::m_MarkSendInfoData.end() != itdelete)
		{
			//DIO Input Log
			LOGDISPLAY_SPEC(7)(_T("@@지울 데이터가 있다면size<%d> id<%d>까지"), CCounterThread::m_MarkSendInfoData.size(), itdelete->TabId);

			::EnterCriticalSection(&m_csQueueMarkingData);
			//시작점 부터 true 설정된 데이터까지 지운다.
			CCounterThread::m_MarkSendInfoData.erase(CCounterThread::m_MarkSendInfoData.begin(), itdelete);
			::LeaveCriticalSection(&m_csQueueMarkingData);

		}
	}

	//마킹을 위한 플로우 추가
	//마킹을 위한 시간을 세팅
	DWORD nowTickCount = GetTickCount();

	//마킹을 보내고 있는 중일 때
	//마킹 보내는 시간은 총 Set 하고 펄스를 TRUE 만드는 시간 5초 대기 + 펄스 TRUE -> FALSE (켠상태 - 끈상태 만들기) 15초 총20초
	if (m_markingSendTimeOut > nowTickCount)
	{
		//마킹 데이터 유효한가?
		if (m_bMarkingDataSend)
		{
			if (m_bOutputBitStatus == FALSE)
			{

				m_dio.OutputBit(CAppDIO::eOut_PULSE, TRUE);
				m_bOutputBitStatus = TRUE;
				//마킹 정보를 쓰고 eOut_PULSE 15sec 유지한다.
				m_markingSendTimeOut = GetTickCount() + 15;
			}
			m_bMarkingDataSend = FALSE;
		}
	}
	else
	{
		if (m_bOutputBitStatus == TRUE)
		{

			m_dio.OutputBit(CAppDIO::eOut_PULSE, FALSE);
			m_bOutputBitStatus = FALSE;
			//마킹 eOut_PULSE FALSE 시간을 10 sec 유지
			m_markingSendFALSETimeOut = GetTickCount() + 10;
		}

		//마킹 정보를 보내고 Out_PULSE FALSE 세팅 후 10초 후
		//마킹 정보를 보내기 위한 세팅을 한다.
		if (m_markingSendFALSETimeOut < nowTickCount)
		{

			::EnterCriticalSection(&m_csQueueMarkingData);
			for (int idx = 0; idx < (int)CCounterThread::m_MarkSendInfoData.size(); idx++)
			{
				//input id와 마킹할 id가 같으면
				if (CCounterThread::m_MarkSendInfoData[idx].bSendComplate == false)
				{
					//지울 최종 포인터
					inputReadId_iterator itdelete = m_inputReadId.end();

					// 받은 input id data가 있으면
					if (m_inputReadId.size())
					{
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
					}

					//Marking Data에 대한 id를 찾았으면
					if (m_inputReadId.end() != itdelete)
					{
						//DIO Input Log
						LOGDISPLAY_SPEC(11)(_T("FT2	send Marking Id TRUE inputid	%d	Data	%d"),
							CCounterThread::m_MarkSendInfoData[idx].TabId, CCounterThread::m_MarkSendInfoData[idx].MarkingOutputData);

						//마킹 데이터 넣고
						m_dio.OutputWord(CCounterThread::m_MarkSendInfoData[idx].MarkingOutputData);
						//마킹 정보를 보내고 5sec Out_PULSE TRUE 세팅
						Sleep(1);
						//sednd 후 지우기 위한 플래그 true;
						CCounterThread::m_MarkSendInfoData[idx].bSendComplate = true;
						//마킹 보내는 타임 설정
						m_markingSendTimeOut = GetTickCount() + 5 + 15;
						//마킹을 보내는 플래그 설정 켜기
						m_bMarkingDataSend = TRUE;

						//DIO Input Log
						LOGDISPLAY_SPEC(7)(_T("@@지울 데이터가 있다면 id<%d>까지"), (*itdelete));

						//시작점 부터 true 설정된 데이터까지 지운다.
						::EnterCriticalSection(&m_csQueueReadId);
						m_inputReadId.erase(m_inputReadId.begin(), itdelete);
						::LeaveCriticalSection(&m_csQueueReadId);
						//마킹 정보를 보냈으면 빠져나온다.
						break;
					}
					//남은 마킹 정보가 2개 이상이면 보낸다.
					else if ((CCounterThread::m_MarkSendInfoData.size() - idx) >= 2)
					{
						//DIO Input Log
						LOGDISPLAY_SPEC(11)(_T("FT2	send Marking Id FALSE inputid	%d	Data	%d"), CCounterThread::m_MarkSendInfoData[idx].TabId, CCounterThread::m_MarkSendInfoData[idx].MarkingOutputData);

						//마킹 데이터 넣고
						m_dio.OutputWord(CCounterThread::m_MarkSendInfoData[idx].MarkingOutputData);
						//마킹 정보를 보내고 5sec Out_PULSE TRUE 세팅
						Sleep(1);
						//sednd 후 지우기 위한 플래그 true;
						CCounterThread::m_MarkSendInfoData[idx].bSendComplate = true;
						//마킹 보내는 타임 설정
						m_markingSendTimeOut = GetTickCount() + 5 + 15;
						//마킹을 보내는 플래그 설정 켜기
						m_bMarkingDataSend = TRUE;

						//마킹 정보를 보냈으면 빠져나온다.
						break;
					}

				}
			}
			::LeaveCriticalSection(&m_csQueueMarkingData);
		}
	}
	return b;
}

//트리커 BCD ID를 읽기 위한 함수
BOOL CCounterThread::readTriggerBCDID()
{
	BOOL b = FALSE;
#ifdef USE_PLCCONNECTZONE

	CSigProc* pSigProc = theApp.m_pSigProc;
	if (pSigProc != NULL && (pSigProc->GetConnectZone() == FALSE))
	{

#endif //USE_PLCCONNECTZONE

		BOOL bTriggerBit;
		m_dio.InputBit(CAppDIO::eIn_TRIGGER, &bTriggerBit);

		//Trigger 펄스 bit true
		if (bTriggerBit == TRUE)
		{
			WORD wInSignal = 0x00;
			m_dio.InputWord(&wInSignal);

			WORD wTempID;
			wTempID = 0x3F & (wInSignal >> 1);

			//이전에 받았던 id와 다르다면 추가
			if (wTempID != m_wLastTabId)
			{
				//BCD ID 얻는 시점에 TabNo는?
				int TabNo = AprData.m_NowLotData.m_nTabCount;

				//DIO Input Log
				LOGDISPLAY_SPEC(7)(_T("BCD ID before	%d	now	%d	TabNo	%d"), m_wLastTabId, wTempID, TabNo + 1);

				//누락된 input 아이디를 찾는다.
				//초기값이 없다면 nextTabID 입력만
				if (m_nextTabID == 255)
				{
					//Tab Use Id 초기화 세팅
					AprData.m_NowLotData.m_bInitTabId = TRUE;

					m_nextTabID = wTempID + 1;
					if (m_nextTabID >= 64)
					{
						m_nextTabID = 0;
					}

					//디버그 로그 기록
					AprData.SaveDebugLog_Format(_T("First BCD Id Use BCD Id	%d"), wTempID);
				}
				//초기값 세팅 상태에서 만 검사한다.
				else
				{
					//다음 들어올 ID와 받은 ID가 다르다면
					//누락 로그 출력한다.
					if (m_nextTabID != wTempID)
					{
						//얻은 Tab Id 범위 확인용
						if ((wTempID >= 0) && (wTempID < 64))
						{
							//누락 Tab id Total 증가를 위한 세팅
							//Tab Id 누락 된 카운트 증가를 위해서 백업한다.
							int nextTabIDbackup = m_nextTabID;
							int nextTabIDbackupLog = m_nextTabID;
							//누락이 1개 일어 났을 때 마킹 정보를 바로 보낸다.
							int omissCount = 0;
							while (nextTabIDbackup != wTempID)
							{
								//누락된 카운트를 올린다.
								omissCount++;

								//DIO Input Log
								LOGDISPLAY_SPEC(7)(_T("누락 BCD ID	%d	누락 갯수	%d"), nextTabIDbackup, omissCount);

								//디버그 로그 기록
								AprData.SaveDebugLog_Format(_T("Lose input BCD ID	%d"), nextTabIDbackup);

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
								//Tab Use Id 초기화 세팅
								AprData.m_NowLotData.m_bInitTabId = TRUE;
								//DIO Input Log
								LOGDISPLAY_SPEC(7)(_T("BCD ID 초기화 TabId	%d	ResetQ Befor Id	%d"), wTempID, m_wLastTabId);

								//디버그 로그 기록
								AprData.SaveDebugLog_Format(_T("Over All Vision BCD Id Initialize Before BCD Id	%d	Init BCD Id	%d"), m_wLastTabId, wTempID);
							}
							//초기화가 아닌 경우
							else
							{
								//DIO Input Log
								LOGDISPLAY_SPEC(7)(_T("BCD ID 누락 카운트 Total Count	%d	추가	%d"), AprData.m_NowLotData.m_nTabIdTotalCount, omissCount);

								AprData.m_NowLotData.m_nTabIdTotalCount += omissCount;
							}
						}

						LOGDISPLAY_SPEC(7)(_T("BCD ID 누락 before	%d	now	%d"), m_wLastTabId, wTempID);
					}

					//다음에 받을 ID를 세팅한다.
					m_nextTabID = wTempID + 1;
					//64 이상이면 0으로 
					if (m_nextTabID >= 64)
					{
						m_nextTabID = 0;
					}
				}

				//제일 마지막 받은 BCD ID 
				AprData.m_NowLotData.m_nLastBCDId = wTempID;

				//BCD ID input time
				AprData.m_NowLotData.m_nBCDIDInputTime = GetTickCount();

				//Tab Total Count 
				//Tab Total Count를 증가 시킨다.
				AprData.m_NowLotData.m_nTabIdTotalCount++;

				AprData.m_NowLotData.m_uEncoderCount = 0;

				LOGDISPLAY_SPEC(11)(_T("FT1	ID:	%d	TabTotal	%d	TabNo	%d"), wTempID, AprData.m_NowLotData.m_nTabIdTotalCount, TabNo);

				AprData.SaveDebugLog_Format(_T("ID	%d	TabNo	%d"), wTempID, TabNo);


				//받은 id
				::EnterCriticalSection(&m_csQueueReadId);
				m_inputReadId.push_back(wTempID);
				::LeaveCriticalSection(&m_csQueueReadId);

				//마킹정보 들어오는 카운트 수
				CCounterThread::deepSwitchOff = 0;


				//이전 id 갱신
				m_wLastTabId = wTempID;

			}
			b = TRUE;
		}

#ifdef USE_PLCCONNECTZONE
	}
	else
	{
		//ConnectZone 세팅 상태 체크
		AprData.m_NowLotData.m_bConnectZone = TRUE;

		//마킹 하기위한 값 제거
		if (m_inputReadId.size())
		{
			m_inputReadId.clear();
		}

	}
#endif //USE_PLCCONNECTZONE
	return b;
}