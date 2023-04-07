#include "pch.h"
#include "CCounterThread.h"
#include "ImageProcessCtrl.h"
#include "AppDIO.h"
#include "CCounterQueueCtrl.h"
#include "GlobalData.h"

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

	while (1) {
		if (pThis == NULL) {
			break;
		}
		if (pThis->m_bKill == TRUE) {
			break;
		}
		if (pThis->m_bRunFlag == FALSE) {
			::Sleep(1);
			continue;
		}

		{
		//	dio.OutputBit(CAppDIO::eOut_PULSE, FALSE); // 22.04.06 Ahn Delete 
			// 입력 취득
			if (pThis->m_pParent->IsInspection() == TRUE) {
				BOOL bTriggerBit;
				dio.InputBit(CAppDIO::eIn_TRIGGER, &bTriggerBit);
				if (bTriggerBit == TRUE) {
					WORD wInSignal = 0x00;
					dio.InputWord(&wInSignal);
					// 22.04.06 Ahn Modify Start
					WORD wTempID;
					wTempID = 0x3F & (wInSignal >> 1);

					if ( wTempID != wLastInfo ) {
					// 22.04.06 Ahn Modify End
						WORD wID = 0x3F & (wInSignal >> 1);
						CCounterInfo cntInfo;
						cntInfo.nTabID = wID;
						pCntQueInPtr->PushBack(cntInfo);
				
						// 22.04.06 Ahn Modify Start
						wLastInfo = wID;
						// 22.04.06 Ahn Modify End
						CString strMsg;
						strMsg.Format(_T("Input ID[%d]"), cntInfo.nTabID);
						AprData.SaveMemoryLog(strMsg);
						::Sleep(5);
					}
					// Cell 추적 Queue Data -> Local Queue 
					//if(....)
				}
			}
		}
		::Sleep(1);

	}

	return 0;
}
