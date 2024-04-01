#include "pch.h"
#include "SiemensPlcIo.h"
#include "LogDisplayDlg.h"


CSiemensPlcIo::CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort)
	: m_strIPAddress(strIPAddress)
	, m_nPort(nPort)
	, m_nReConnetTimeOut(nReConnetTimeOut)
	, m_pReceiveMsgWnd(pReceiveMsgWnd)
	, m_pLGIS_Plc(NULL)
{
	if (OpenPio() == 0)
	{

		//이벤트 객체 생성
		pEvent_SiemensPlc = CreateEvent(NULL, FALSE, FALSE, NULL);
		//스래드 생성
		m_pThread_SiemensPlc = AfxBeginThread(SiemensPlc_ThreadProc, this);
	}
}


CSiemensPlcIo::~CSiemensPlcIo()
{
	ClosePio();
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

int CSiemensPlcIo::OpenPio(void)
{
	int ret = 0;

	m_pLGIS_Plc = new CLGIS_Plc(m_strIPAddress, m_nPort, m_nReConnetTimeOut, m_pReceiveMsgWnd );
	
	if (!m_pLGIS_Plc->CheckConnection())
	{
		ClosePio();
		//로그출력
		LOGDISPLAY_SPECTXT(0)("PLC Siemens Open failed");
		ret = -1;
	}

	return ret;
}

void CSiemensPlcIo::ClosePio(void)
{
	if (m_pLGIS_Plc)
	{
		delete m_pLGIS_Plc;
		m_pLGIS_Plc = NULL;
	}
}
