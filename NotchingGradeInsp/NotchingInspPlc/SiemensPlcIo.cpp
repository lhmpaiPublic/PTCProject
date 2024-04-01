#include "pch.h"
#include "SiemensPlcIo.h"
#include "GlobalData.h"
#include "LogDisplayDlg.h"


CSiemensPlcIo::CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort)
	: m_strIPAddress(strIPAddress)
	, m_nPort(nPort)
	, m_nReConnetTimeOut(nReConnetTimeOut)
	, m_pReceiveMsgWnd(pReceiveMsgWnd)
	, m_pLGIS_Plc(NULL)
{
	m_pThread_SiemensPlc = NULL;
	pEvent_SiemensPlc = NULL;
	if (OpenPlcIo() == 0)
	{
		//슬레이브 아이디 
		SetSlaveId(AprData.m_System.m_nSlaveID);
		//이벤트 객체 생성
		pEvent_SiemensPlc = CreateEvent(NULL, FALSE, FALSE, NULL);
		//스래드 생성
		m_pThread_SiemensPlc = AfxBeginThread(SiemensPlc_ThreadProc, this);
	}
}


CSiemensPlcIo::~CSiemensPlcIo()
{
	ClosePlcIo();
	if (m_pThread_SiemensPlc)
	{
		setEvent_SiemensPlc();
		CGlobalFunc::ThreadExit(&m_pThread_SiemensPlc->m_hThread, 5000);
		m_pThread_SiemensPlc->m_hThread = NULL;
		m_pThread_SiemensPlc = NULL;
	}

	if (pEvent_SiemensPlc)
	{
		CloseHandle(pEvent_SiemensPlc);
		pEvent_SiemensPlc = NULL;
	}
}

//스래드 함수
#define SIEMENSPLC_TIMEOUT 100
UINT CSiemensPlcIo::SiemensPlc_ThreadProc(LPVOID param)
{
	CSiemensPlcIo* pMain = (CSiemensPlcIo*)param;

	int ret = WAIT_OBJECT_0;
	while (pMain)
	{
		//타임 주기 이벤트
		ret = WaitForSingleObject(pMain->getEvent_SiemensPlc(), SIEMENSPLC_TIMEOUT);

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			break;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			pMain->SiemensPlcProc();
		}
		else
		{
			break;
		}

	}
	AfxEndThread(0);
	return 0;
}

//스래드에서 호출하는 함수
void CSiemensPlcIo::SiemensPlcProc()
{
	if (IsOpened())
	{
		//Read 영역 읽기
		static const int ReadSize = MAX_SMS_BITIO_IN + MAX_SMS_WORDIO_IN;
		short	ReadData[ReadSize];
		ReadDataReg(AprData.m_System.m_nBitIn, ReadData, ReadSize);

		ReadPlcDataParser(ReadData, ReadSize);

		//Write 영역 쓰기
		static const int WriteSize = MAX_SMS_BITIO_OUT + MAX_SMS_WORDIO_IN;
		short	WriteData[WriteSize];
		WritePlcDataMake(WriteData, WriteSize);

		WriteDataReg(AprData.m_System.m_nBitOut, WriteData, WriteSize);
	}
	else
	{

	}
}

//PLC read Data Parser 함수
void CSiemensPlcIo::ReadPlcDataParser(short* data, int len)
{

}
//PLC write Data Make 함수
void CSiemensPlcIo::WritePlcDataMake(short data[], int len)
{

}

void CSiemensPlcIo::SetSlaveId(int nId)
{
	if (m_pLGIS_Plc)
	{
		m_pLGIS_Plc->SetSlaveId(nId);
	}
}

void CSiemensPlcIo::SetReceiveMsgWnd(CWnd* pWnd)
{
	if (m_pLGIS_Plc)
	{
		m_pReceiveMsgWnd = pWnd;
		m_pLGIS_Plc->SetReceiveMsgWnd(m_pReceiveMsgWnd);
	}

}

void CSiemensPlcIo::SetReConnetTimeOut(int nTimeOut)
{
	if (m_pLGIS_Plc)
	{
		m_nReConnetTimeOut = nTimeOut;
		m_pLGIS_Plc->SetReConnetTimeOut(m_nReConnetTimeOut);
	}
}


int CSiemensPlcIo::WriteDataReg(int offset, short data[], int num)
{
	CString strMsg;

	int nRet = 0;
	int nAdd = offset;// +m_wOffset_Out; //+ m_wOffsetWord_Out;
	{	
		if (m_pLGIS_Plc)
		{
			nRet = m_pLGIS_Plc->WriteMultipleRegisters(nAdd, num, (uint16_t*)data);

			if (nRet < 0)
			{
				//strMsg.Format(_T("Siemens Write Error"));

			}

		}
		else
		{
			//strMsg.Format(_T("Siemens Open Error"));

		}

	}

	return nRet ;
}

int CSiemensPlcIo::ReadDataReg(int offset, short data[], int num)
{
	int nRet = 0;
	int nAdd = offset;// +m_wOffsetWord_In;
	{

 		CString strMsg;
		if (m_pLGIS_Plc)
		{
			nRet = m_pLGIS_Plc->ReadHoldingRegisters(nAdd, num, (uint16_t*)data);

			if( nRet < 0)
			{
				//strMsg.Format(_T("Siemens Read Error"));
			}
		}
		else
		{
			//strMsg.Format(_T("Siemens Open Error"));
		}

	}
	return nRet;
}

BOOL CSiemensPlcIo::IsOpened()
{
	if (m_pLGIS_Plc)
		return m_pLGIS_Plc->CheckConnection();

	return false;
}

CString CSiemensPlcIo::GetErrorMsg()
{
	if (IsOpened())
	{
		if (m_pLGIS_Plc->m_bErr)
			return m_pLGIS_Plc->m_strErrorMsg;
		else
			return CString();
	}
	else
	{
		if (m_pLGIS_Plc)
			return m_pLGIS_Plc->m_strErrorMsg;
	}

	return _T("Not created PLC handle");
}

int CSiemensPlcIo::GetErrorNo()
{
	if (IsOpened())
	{
		if (m_pLGIS_Plc->m_bErr)
			return m_pLGIS_Plc->m_nErrNo;
		else
			PLC_OK;
	}
	else
	{
		if (m_pLGIS_Plc)
			return m_pLGIS_Plc->m_nErrNo;
	}

	return CONNECTION_ERROR;
}

int CSiemensPlcIo::OpenPlcIo(void)
{
	int ret = 0;

	m_pLGIS_Plc = new CLGIS_Plc(m_strIPAddress, m_nPort, m_nReConnetTimeOut, m_pReceiveMsgWnd );
	
	if (!m_pLGIS_Plc->CheckConnection())
	{
		ClosePlcIo();
		//로그출력
		LOGDISPLAY_SPECTXT(0)("PLC Siemens Open failed");
		ret = -1;
	}

	return ret;
}

void CSiemensPlcIo::ClosePlcIo(void)
{
	if (m_pLGIS_Plc)
	{
		delete m_pLGIS_Plc;
		m_pLGIS_Plc = NULL;
	}
}
