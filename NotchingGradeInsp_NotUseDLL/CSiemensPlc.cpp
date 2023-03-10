#include "pch.h"
#include "CSiemensPlc.h"
#include "GlobalData.h"


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

	SetSlaveId(10); // 23.03.02 pyj add
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
		nRet = m_pLGIS_Plc->WriteMultipleRegisters(nAdd, num, (uint16_t*)data);

		strMsg.Format(_T("WriteDataReg nAdd:%d, num:%d, data:%d "), nAdd, num, data[0]);
		AprData.SaveDebugLog(strMsg); //pyjtest

	}

//	AprData.SaveDebugLog(_T("WriteDataReg end")); //pyjtest

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
		strMsg.Format(_T("ReadDataReg Addr:%d, num:%d"), nAdd, num);
		AprData.SaveDebugLog(strMsg); //pyjtest

		nRet = m_pLGIS_Plc->ReadHoldingRegisters(nAdd, num, (uint16_t*)data);

// 		strMsg.Format(_T("ReadDataReg data:%d"), data[0]);
// 		AprData.SaveDebugLog(strMsg); //pyjtest

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
	CString strMsg;
	strMsg.Format(_T("OpenPio start IP:%s, Port:%d"), m_strIPAddress, m_nPort);

	AprData.SaveDebugLog(strMsg); //pyjtest


	ClosePio();
	m_pLGIS_Plc = new CLGIS_Plc((std::string)CT2CA(m_strIPAddress), m_nPort);
	
	if (!m_pLGIS_Plc->Connect())
	{
		AprData.SaveDebugLog(_T("Siemens Plc Connect Fail")); //pyjtest
		ClosePio();
	}

	AprData.SaveDebugLog(_T("OpenPio end")); //pyjtest



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

// 23.03.02 Ahn Modify Star
int CSiemensPlc::InPort2(WORD port, BYTE* data, BOOL bExtSt /*= FALSE*/)
{
	int nRet = 0; 
	int offset = (int)port + m_wOffset_In ;

	nRet =  m_pLGIS_Plc->ReadHoldingRegisters(offset, 1, (uint16_t*)data);

	return nRet;
}

int CSiemensPlc::OutPort2(WORD port, BYTE data, BOOL bLockCtrl /*= TRUE*/, BOOL bGetBit /*= FALSE */ )
{
	int nRet = 0;
	int offset = (int)port + (int)m_wOffset_Out;

//	AprData.SaveDebugLog(_T("OutPort start")); //pyjtest

	if (bGetBit == FALSE) {
		nRet = m_pLGIS_Plc->WriteMultipleRegisters(offset, 1, (uint16_t*)data);
	}
	else {
		m_pLGIS_Plc->ReadHoldingRegisters(offset, 1, (uint16_t*)data);
		nRet = (int)data;
	}

//	AprData.SaveDebugLog(_T("OutPort end")); //pyjtest

	return nRet;
}

int CSiemensPlc::Out_Port_Bit2(int nPort, BYTE bBitPos, int nMode)
{
	int nRet = 0; 

	if (nPort > 16) return -1;

	BYTE	data = btOutPortData[nPort];
	BOOL bGetBit = FALSE;

	switch (nMode) {
	case	FALSE:
		data &= ~bBitPos;
		break;
	case	TRUE:
		data |= bBitPos;
		break;
	case	-1:
	{
		int	iOutState = FALSE;
		if ((data & bBitPos) != 0x00) {
			iOutState = TRUE;
		}
		return (iOutState);
	}
	case	-2:
		bGetBit = TRUE;
		break;
	default:
		{
			//에러로그
		}
		return (-1);
	}

	nRet = OutPort2(nPort, data, TRUE, bGetBit);

	if (nRet < 0) {
		nRet = -1;
		//에러로그
	}

	return nRet ;
}
// 23.03.02 Ahn Modify End

// 23.03.02 Ahn Modify Start
int CSiemensPlc::ReadPortAllBitIn2(BYTE* data, short size)
{ 
	int nRet = 0;
	int offset = (int)m_wOffset_In;

	uint16_t usData[32];
	memset(usData, 0x00, sizeof(uint16_t) * 32);

	int nPortSize = 20 ;

	nRet = m_pLGIS_Plc->ReadHoldingRegisters(offset, nPortSize, usData);

	int nCount = 0 ;
	for( int nPort = 0 ; nPort < MAX_USE_PORT; nPort++ ){
		for (int nBit = 0; nBit < MAX_PORT_BIT; nBit++) {
			nCount = (nPort * MAX_PORT_BIT) + nBit;
			if (usData[nCount] == 0x0001) {
				data[nPort] = (0x0001) << nBit;
			}
		}
	}

	return nRet;
}

int CSiemensPlc::ReadPortAllBitOut2(BYTE* data, short size)
{ 
	int nRet = 0;
	int offset = (int)m_wOffset_Out;

	uint16_t usData[32];
	memset(usData, 0x00, sizeof(uint16_t) * 32);

	nRet = m_pLGIS_Plc->ReadHoldingRegisters(offset, size, usData);

	int nCount = 0;
	for (int nPort = 0; nPort < MAX_USE_PORT; nPort++) {
		for (int nBit = 0; nBit < MAX_PORT_BIT; nBit++) {
			nCount = (nPort * MAX_PORT_BIT) + nBit ;
			if (usData[nCount] == 0x0001) {
				data[nPort] = (0x01) << nBit;
			}
		}
	}

	return nRet;
}

int CSiemensPlc::WriteDataReg2(int offset, int data[], int num)
{
	int nRet = 0;
	short* pData = new short[num];
	
	for (int i = 0; i < num; i++) {
		pData[i] = (short)(data[i] & 0xffff) ;
	}

	nRet = WriteDataReg(offset, pData, num);

	delete pData;
	pData = nullptr;

	return nRet;
}

// 23.03.02 Ahn Modify End
