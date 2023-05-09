#include "pch.h"
#include "CSiemensPlc.h"
#include "GlobalData.h"
#include "LogDisplayDlg.h"


// 23.03.03 Ahn Modify Start
//CSiemensPlc::CSiemensPlc(CString strIPAddress, int nPort /*= 4000*/)
CSiemensPlc::CSiemensPlc(CString strIPAddress, int nPort /*= 4000*/, int nOffsetIn/*= 0*/, int nOffsetOut/*= 100*/, int nWordIn/*=20*/, int nWordOut/*=120*/)
// 23.03.03 Ahn Modify End
	: m_strIPAddress(strIPAddress)
	, m_nPort(nPort)
	, m_pLGIS_Plc(NULL)
{

	// 23.03.03 Ahn Add Start
	m_wOffset_In = nOffsetIn ;	//!< 입력 신호 Offset
	m_wOffset_Out = nOffsetOut;	//!< 출력 신호 Offset
	m_wOffsetWord_In = nWordIn ;
	m_wOffsetWord_Out = nWordOut ;
	// 23.03.03 Ahn Add End

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

void CSiemensPlc::SetTimeOut(int nTimeOut)
{
	if (m_pLGIS_Plc)
	{
		m_pLGIS_Plc->SetTimeOut(nTimeOut);
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
				AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, strMsg);

				//Error Log
				LOGDISPLAY_SPECTXT(0)(_T("Siemens Write Error"));
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
				AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, strMsg);

				//Error Log
				LOGDISPLAY_SPECTXT(0)(_T("Siemens Read Error"));
			}
		}
		else
		{
			strMsg.Format(_T("Siemens Open Error"));
			AprData.SaveErrorLog(strMsg);
			AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, strMsg);

		}

	}
	return nRet;
	//23.03.02 Ahn Add End
}

BOOL CSiemensPlc::IsOpened()
{
	if (m_pLGIS_Plc)
		return m_pLGIS_Plc->IsConnected();

	return false;
}

int CSiemensPlc::OpenPio(void)
{
	ClosePio();
	m_pLGIS_Plc = new CLGIS_Plc((std::string)CT2CA(m_strIPAddress), m_nPort);
	
	if (!m_pLGIS_Plc->Connect())
	{
		ClosePio();
		//로그출력
		LOGDISPLAY_SPECTXT(0)("PLC Siemens Open failed");
	}

	return 0;
}

int CSiemensPlc::ClosePio(void)
{
	if (m_pLGIS_Plc)
	{
		m_pLGIS_Plc->Close();
		delete m_pLGIS_Plc;
		m_pLGIS_Plc = NULL;
	}

	return 0;
}
