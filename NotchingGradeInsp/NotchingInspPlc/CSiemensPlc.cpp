#include "pch.h"
#include "CSiemensPlc.h"
#include "GlobalData.h"
#include "LogDisplayDlg.h"


CSiemensPlc::CSiemensPlc(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort /*= 4000*/, int nOffsetIn/*= 0*/, int nOffsetOut/*= 100*/, int nWordIn/*=20*/, int nWordOut/*=120*/)
	: m_strIPAddress(strIPAddress)
	, m_nPort(nPort)
	, m_nReConnetTimeOut(nReConnetTimeOut)
	, m_pReceiveMsgWnd(pReceiveMsgWnd)
	, m_pLGIS_Plc(NULL)
{

	m_wOffset_In = nOffsetIn ;	//!< 입력 신호 Offset
	m_wOffset_Out = nOffsetOut;	//!< 출력 신호 Offset
	m_wOffsetWord_In = nWordIn ;
	m_wOffsetWord_Out = nWordOut ;

	OpenPio();

	SetSlaveId(AprData.m_System.m_nSlaveID); // 23.03.02 pyj add
}


CSiemensPlc::~CSiemensPlc()
{
	ClosePio();
}

void CSiemensPlc::SetSlaveId(int nId)
{
	if (m_pLGIS_Plc)
	{
		m_pLGIS_Plc->SetSlaveId(nId);
	}
}

void CSiemensPlc::SetReceiveMsgWnd(CWnd* pWnd)
{
	if (m_pLGIS_Plc)
	{
		m_pReceiveMsgWnd = pWnd;
		m_pLGIS_Plc->SetReceiveMsgWnd(m_pReceiveMsgWnd);
	}

}

void CSiemensPlc::SetReConnetTimeOut(int nTimeOut)
{
	if (m_pLGIS_Plc)
	{
		m_nReConnetTimeOut = nTimeOut;
		m_pLGIS_Plc->SetReConnetTimeOut(m_nReConnetTimeOut);
	}
}


int CSiemensPlc::WriteDataReg(int offset, short data[], int num)
{
	//23.03.02 Ahn Add Start
	//return m_pLGIS_Plc->WriteMultipleRegisters(offset, num, (uint16_t*)data);


	CString strMsg;

	int nRet = 0;
	int nAdd = offset;// +m_wOffset_Out; //+ m_wOffsetWord_Out;
	{	
		if (m_pLGIS_Plc)
		{
			nRet = m_pLGIS_Plc->WriteMultipleRegisters(nAdd, num, (uint16_t*)data);

			if (nRet < 0)
			{
				strMsg.Format(_T("Siemens Write Error"));
				AprData.SaveErrorLog(strMsg);
//				AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, strMsg);

			}

		}
		else
		{
			strMsg.Format(_T("Siemens Open Error"));
			AprData.SaveErrorLog(strMsg);
			AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, strMsg);

		}

	}

	return nRet ;
	//23.03.02 Ahn Add End

}

int CSiemensPlc::ReadDataReg(int offset, short data[], int num)
{
	//23.03.02 Ahn Add Start
	//return m_pLGIS_Plc->ReadHoldingRegisters(offset, num, (uint16_t*)data);
	int nRet = 0;
	int nAdd = offset;// +m_wOffsetWord_In;
	{

 		CString strMsg;
		if (m_pLGIS_Plc)
		{
			nRet = m_pLGIS_Plc->ReadHoldingRegisters(nAdd, num, (uint16_t*)data);

			if( nRet < 0)
			{
				strMsg.Format(_T("Siemens Read Error"));
				AprData.SaveErrorLog(strMsg);
//				AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, strMsg);
				LOGDISPLAY_SPEC(11)(_T("Siemens Read Error"));
			}
		}
		else
		{
			strMsg.Format(_T("Siemens Open Error"));
			AprData.SaveErrorLog(strMsg);
			AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, strMsg);
			LOGDISPLAY_SPEC(11)(_T("Siemens Read Error"));

		}

	}
	return nRet;
	//23.03.02 Ahn Add End
}

BOOL CSiemensPlc::IsOpened()
{
	BOOL b = false;
	if (m_pLGIS_Plc)
		b = m_pLGIS_Plc->CheckConnection();

	return b;
}

CString CSiemensPlc::GetErrorMsg()
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

int CSiemensPlc::GetErrorNo()
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

int CSiemensPlc::OpenPio(void)
{
	ClosePio();
//	m_pLGIS_Plc = new CLGIS_Plc((std::string)CT2CA(m_strIPAddress), m_nReConnetTimeOut, m_pReceiveMsgWnd, m_nPort);
	m_pLGIS_Plc = new CLGIS_Plc(m_strIPAddress, m_nPort, m_nReConnetTimeOut, m_pReceiveMsgWnd );
	
	if (!m_pLGIS_Plc->CheckConnection())
	{
		ClosePio();
		//로그출력
		LOGDISPLAY_SPEC(2)("PLC Siemens Open failed");
	}
	else
	{
		//로그출력
		LOGDISPLAY_SPECTXT(2)("PLC Siemens Open success");
	}

	return 0;
}

int CSiemensPlc::ClosePio(void)
{
	if (m_pLGIS_Plc)
	{
//		m_pLGIS_Plc->Close();
		delete m_pLGIS_Plc;
		m_pLGIS_Plc = NULL;
	}

	return 0;
}
