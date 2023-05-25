#include "pch.h"
#include "CCounterThread.h"
#include "ImageProcessCtrl.h"
#include "AppDIO.h"
#include "CCounterQueueCtrl.h"
#include "GlobalData.h"
#include "SigProc.h"

CCounterThread::CCounterThread(CImageProcessCtrl* pParent)
{
	m_pParent = pParent;
}
CCounterThread::~CCounterThread()
{
}
void CCounterThread::Begin()
{
	m_bKill = FALSE;

	if (m_pThread == NULL) {

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
	DWORD	dwCode;
	LONG	ret;

	if (m_pThread != NULL) {
		// 긚깒긞긤륉뫴롦벦
		ret = ::GetExitCodeThread(m_pThread->m_hThread, &dwCode);
		if (ret && dwCode == STILL_ACTIVE) {
			// -----긚깒긞긤벍띿뭷-----
			m_bKill = TRUE;
			WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		}
		delete m_pThread;
		m_pThread = NULL;
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

// 22.04.06 Ahn Add Start
void CCounterThread::ThreadRun(BOOL bRunFlag)
{
	m_bRunFlag = bRunFlag;
}
// 22.04.06 Ahn Add End
UINT CCounterThread::CtrlThreadCounter(LPVOID pParam)
{
	CCounterThread* pThis = (CCounterThread*)pParam;
	CCounterQueueCtrl* pCntQueInPtr = pThis->m_pParent->GetCounterQueInPtr() ;

	CAppDIO dio;
	WORD wLastInfo = 0x00;
	int nIncrease = 0 ;

	WORD backupwInSignal = 0x00;

	WORD nextTabID = 255;

	while (1) {
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

			::Sleep(1);
			continue;
		}

		// 입력 취득
		if (pThis->m_pParent->IsInspection() == TRUE) 
		{
			CSigProc* pSigProc = theApp.m_pSigProc;
			if (pSigProc != NULL && (pSigProc->GetConnectZone() == FALSE))
			{

				BOOL bTriggerBit;
				dio.InputBit(CAppDIO::eIn_TRIGGER, &bTriggerBit);
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
						LOGDISPLAY_SPEC(1)(_T("DIO Signal Word before<%d> now<%d>"), backupwInSignal, wInSignal);
						backupwInSignal = wInSignal;

						//받은 Trigger TabID 총 갯수 증가
						AprData.m_NowLotData.m_nInputTabIDTotalCnt++;

						//DIO Input Log
						LOGDISPLAY_SPEC(1)(_T("*0**DIO ID before<%d> now<%d>"), wLastInfo, wTempID);

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

						//DIO Input Log
						LOGDISPLAY_SPEC(1)(_T("Input ID 받음[%d] Queue Count<%d>-><%d>, Recive TabID TotalCount<%d>"),
							cntInfo.nTabID, beforeQueueCount, pCntQueInPtr->GetSize(), AprData.m_NowLotData.m_nInputTabIDTotalCnt);

						//이전 id 갱신
						wLastInfo = wTempID;

						//메모리 로그 기록
						CString strMsg;
						strMsg.Format(_T("Input ID[%d], Recive TotalCount<%d>, Queue Count<%d>-><%d>"),
							cntInfo.nTabID, AprData.m_NowLotData.m_nInputTabIDTotalCnt, beforeQueueCount, pCntQueInPtr->GetSize());
						AprData.SaveMemoryLog(strMsg);


						::Sleep(5);
					}
					// Cell 추적 Queue Data -> Local Queue 
					//if(....)
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
		::Sleep(1);

	}

	return 0;
}
