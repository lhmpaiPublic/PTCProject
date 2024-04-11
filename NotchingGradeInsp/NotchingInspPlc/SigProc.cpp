#include "pch.h"
#include "NotchingGradeInsp.h"
#include "SigProc.h"
#include "TimeAnalyzer.h"
#include "WinTool.h"
#include "GlobalData.h"

CCriticalSection CSigProc::m_cs;

CSigProc::CSigProc()
{
	m_pPioCtrl = NULL;

	m_SigInRun = FALSE;

	if (theApp.m_pPioCtrl != NULL) 
	{
		m_pPioCtrl = theApp.m_pPioCtrl;
	}

	m_bSmsAlive = FALSE;

	//컨넥트 존 설정 변수 초기화
	m_bSigInConnectZone = FALSE;
}

CSigProc::~CSigProc()
{
	
}

int CSigProc::GetPortBit(int nIntegration, int* piPort, BYTE* pByte)
{
	ASSERT(piPort != NULL);
	ASSERT(pByte != NULL);

	int	nRet = 0;

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec)
	{
		*piPort = (int)((nIntegration >> 8) & 0xff);
		*pByte = (BYTE)(nIntegration & 0xff);
	}
	else
	{
		*piPort = nIntegration;
		*pByte = (BYTE)(0x01);
	}

	return (nRet);
}

int CSigProc::SignalPortIn(WORD port, BYTE* data, BOOL bExtSt /*= FALSE*/, BOOL bLocal /*= FALSE*/)
{
	CPioCtrl* pPioCtrl = m_pPioCtrl;
	if ((pPioCtrl == NULL) || (data == NULL)) {
		//에러로그
		return (-1);
	}
	return (pPioCtrl->InPortByte(port, data, bExtSt));
}

int CSigProc::SignalPortCheck(int iInput, BOOL bExtSt /*= FALSE*/, BOOL bLocal /*= FALSE*/)
{
	int		iPort = 0x00;
	BYTE	cByte = 0x00;
	if (GetPortBit(iInput, &iPort, &cByte) < 0) {
		//에러로그
		return (-1);
	}


	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{

		short data = 0;
		int nSize = sizeof(short) / sizeof(short);
		if (ReadPLC_Block_device(iPort + AprData.m_System.m_nBitIn, &data, nSize) != -1)
		{
			if(data == 1)
			{
				return (1);
			}
		}


	}
	else
	{
		BYTE	data = 0x00;
		if (SignalPortIn(iPort, &data, bExtSt, bLocal) != 0) {
			//에러로그
			return (-1);
		}

		if ((data & cByte) != 0x00) {
			return (1);
		}
	}

	return (0);
}

int CSigProc::SignalBitOut(int nIntegration, int nMode, BOOL bLocal /*= FALSE*/)
{
	CPioCtrl* pPioCtrl = m_pPioCtrl;

	if (pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}

	int	iPort = 0x00;
	BYTE	cByte = 0x00;
	if (GetPortBit(nIntegration, &iPort, &cByte) < 0) {
		//에러로그
		return (-1);
	}

	int	nRet = 0;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		short nData = (nMode == 1) ? 1 : 0;
		short* pData = (short*)(&nData);
		int nSize = sizeof(short) / sizeof(short);
		nRet = WritePLC_Block_device(nIntegration + AprData.m_System.m_nBitOut, pData, nSize);

	}
	else
	{
		nRet = pPioCtrl->Out_Port_Bit(iPort, cByte, nMode);
	}


	if (nRet < 0) {
		//에러로그
		return (-1);
	}

	//////////////////////////////////////////////////////////////////////////


	return (nRet);
}

BYTE CSigProc::SignalPortGetOut(WORD port)
{
	if (m_pPioCtrl == NULL) {
		//에러로그
		return (0x00);
	}
	return (m_pPioCtrl->GetOutData(port));
}

int CSigProc::CheckPortOut(int sig)
{
	int iPort = 0x00;
	BYTE cByte = 0x00;
	if (GetPortBit(sig, &iPort, &cByte) < 0) {
		//에러로그
		return (-1);
	}
	if ((cByte & SignalPortGetOut(iPort)) != 0x00) {
		return (TRUE);
	}
	else {
		return (FALSE);
	}
}

int CSigProc::WritePLC_device(int address, short data)
{
	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}

	if (m_pPioCtrl->WritePLC_device(address, data) < 0) {
		//에러로그
		return (-1);
	}
	return (0);
}

int CSigProc::ReadPLC_device(int address, short* data)
{
	ASSERT(data != NULL);
	if (data == NULL) {
		//에러로그
		return (-1);
	}
	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}

	if (m_pPioCtrl->ReadPLC_device(address, data) < 0) {
		//에러로그
		return (-1);
	}
	return (0);
}

int CSigProc::ReadAllPort_BitIn( BOOL* pSigBitIn )
{
	ASSERT(pSigBitIn);
	int nRet = 0;
	if (m_pPioCtrl == NULL) return -1;

	//로그출력 값
	CString bitVal = "";

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		m_pPioCtrl->ReadAllPort_BitIn( (BYTE*)m_sSmsSigBItIN, MAX_SMS_BITIO_IN);
		for (int i = 0; i < MAX_SMS_BITIO_IN; i++)
		{

			if (m_sSmsSigBItIN[i] == 0x01)
			{
				pSigBitIn[i] = TRUE;

				CString str;
				str.Format("[%d]:TRUE ", i);
				bitVal = str + bitVal;
			}
		}

	}
	else
	{
		m_pPioCtrl->ReadAllPort_BitIn(m_btSigBItIN, MAX_USE_PORT);

		int nBitPos = 0;
		for (int port = 0; port < MAX_USE_PORT; port++) {
			for (int bit = 0; bit < MAX_PORT_BIT; bit++) {
				if (m_btSigBItIN[port] & (0x01 << bit)) {
					nBitPos = (port * MAX_PORT_BIT) + bit;
					pSigBitIn[nBitPos] = TRUE;

					CString str;
					str.Format("[%d]:TRUE ", nBitPos);
					bitVal = str + bitVal;
				}
			}
		}
	}

	return nRet;
}

int CSigProc::ReadAllPort_BitOut( BOOL* pSigBitOut )
{
	ASSERT(pSigBitOut);
	int nRet = 0;
	if (m_pPioCtrl == NULL) return -1;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		m_pPioCtrl->ReadAllPort_BitOut((BYTE*)m_sSmsSigBItOut, MAX_SMS_BITIO_OUT);
		for (int i = 0; i < MAX_SMS_BITIO_OUT; i++)
		{
			if (m_sSmsSigBItOut[i] == 0x01)
			{
				pSigBitOut[i] = TRUE;

			}

		}

	}
	else
	{
		m_pPioCtrl->ReadAllPort_BitOut(m_btSigBItOut, MAX_USE_PORT);
		int nBitPos = 0;
		for (int port = 0; port < MAX_USE_PORT; port++) {
			for (int bit = 0; bit < MAX_PORT_BIT; bit++) {
				if (m_btSigBItOut[port] & (0x01 << bit)) {
					nBitPos = (port * MAX_PORT_BIT) + bit;
					pSigBitOut[nBitPos] = TRUE;

				}
			}
		}
	}

	return nRet;
}

int CSigProc::SetAlarmCode(WORD wAlarmCode)
{
	m_pPioCtrl->SetAlarmCode(wAlarmCode);

	return 0;
}
WORD CSigProc::GetAlarmCode()
{
	return m_pPioCtrl->GetAlarmCode();
}

int CSigProc::WriteAlarmCode(WORD nAlarmCode)
{
	int nNumOfData = 1;

	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}


	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		int address = GetWordAddress(enWordWrite_AlarmExist, MODE_WRITE);

		short nAlarmCheck = 0x0001;
		nNumOfData = 25;  // Alarm Exist 1word + Alarm 24word
		short pData[25] = { 0, };
		for (int i = 1; i < nNumOfData; i++) // Array 0번 = Alarm Exist, Array 1번 부터 Alarm code
		{
			if ((nAlarmCode & nAlarmCheck) != 0)
			{
				pData[0] = 0x0001; // Alarm Exist, NG가 한 개라도 있으면 Ture, 아니면 초기값 False

				pData[i] = 0x0001; // Alarm Code
			}
			nAlarmCheck = nAlarmCheck << 1;
		}


		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)&pData, nNumOfData) < 0) {
			//에러로그
			return (-1);
		}
	}
	else
	{
		int address = GetWordAddress(enWordWrite_AlarmCode_Buffer1, MODE_WRITE);

		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)&nAlarmCode, nNumOfData) < 0) {
			//에러로그
			return (-1);
		}
	}



	return (0);
}


// NG Alarm과 Cell Trigger ID, Judge, NG Code를 블럭으로 한번에 보냄
int CSigProc::WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode)
{	

	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}


	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		int address = GetWordAddress(enWordWrite_AlarmExist, MODE_WRITE);

		short nAlarmCheck = 0x0001;
		int nSizeOfAlarmCode = 25;  // Alarm Exist 1word + Alarm 24word
		int nSizeOfData = 43;
		short pData[43] = { 0, };
		for (int i = 1; i < nSizeOfAlarmCode; i++) // Array 0번 = Alarm Exist, Array 1번 부터 Alarm code
		{
			if ((nAlarmCode & nAlarmCheck) != 0)
			{
				pData[0] = 0x0001; // Alarm Exist, NG가 한 개라도 있으면 Ture, 아니면 초기값 False
				pData[i] = 0x0001; // Alarm Code
			}
			nAlarmCheck = nAlarmCheck << 1;
		}

		pData[40] = (short)nID;
		pData[41] = (short)nJudge;
		pData[42] = (short)nNgCode;

		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)&pData, nSizeOfData) < 0) {
			//에러로그
			return (-1);
		}

	}
	else
	{

	}
	return 0;
}






int CSigProc::WritePLC_Block_device(int address, short* pData, int nNumOfData)
{
	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}
	if (m_pPioCtrl->WritePLC_Block_device(address, pData, nNumOfData) < 0) {
		//에러로그
		return (-1);
	}
	return (0);
}

int CSigProc::WritePLC_Block_device(int address, int* pData, int nNumOfData)
{
	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}
	if (m_pPioCtrl->WritePLC_Block_device(address, pData, nNumOfData) < 0) {
		//에러로그
		return (-1);
	}
	return (0);
}

int CSigProc::ReadPLC_Block_device(int address, short* pData, int nNumOfData)
{
	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}
	if (m_pPioCtrl->ReadPLC_Block_device(address, pData, nNumOfData) < 0) {
		//에러로그
		return (-1);
	}
	return (0);
}

int CSigProc::ReadPLC_Block_deviceR(int address, short* pData, int nNumOfData)
{
	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}

	if (m_pPioCtrl->ReadPLC_device(address, pData, nNumOfData, 0x16, 0x00) < 0) {
		//에러로그
		return (-1);
	}
	return (0);
}


int CSigProc::WaitSignal(int nIntegration, BOOL mode, int timeout /*= 1000*/, BOOL bCheckAutoSignal /*= FALSE*/, BOOL bErrorProcess /*= TRUE*/, BOOL bExtSt /*= FALSE*/)
{

	if (m_bDebugIO) {
		return (0);
	}

	CTimeAnalyzer	ct;
	ct.StopWatchStart();

	int	nRet = 0;

	while (TRUE) {
		if (SignalPortCheck(nIntegration, bExtSt) == mode) {
			break;
		}

		::PumpMessages(NULL, NULL, WM_TIMER, WM_TIMER);
		::PumpMessages();
		if (timeout > 0) {
			if (ct.WhatTimeIsIt() > timeout) {
				//에러로그
				nRet = -3;
				if (bErrorProcess == TRUE) {
					//에러로그
				}
				break;
			}
		}
		::Sleep(10);
	}
	return (nRet);

}




//////////////////////////////////////////////////////////////////////////
// [ PLC SIGNAL INPUT ]

int CSigProc::SigInAlivePulse()
{
	int nRet = 0;
	int nAddress;


	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_Alive;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread( nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_Alive;
		nRet = SignalPortCheck(nAddress);
	}



	return nRet;
}

int CSigProc::SigInReady()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_Ready;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_Ready;
		nRet = SignalPortCheck(nAddress);

	}

	return nRet;
}

int CSigProc::SigInRun()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_Run;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = m_SigInRun;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_Run;
		nRet = SignalPortCheck(nAddress);
	}

	if (m_SigInRun != nRet)
	{
		m_SigInRun = (nRet == TRUE) ? TRUE : FALSE;

		//디버그 로그 기록
		AprData.SaveDebugLog_Format(_T("PLC SigIn	%s"), (nRet == TRUE)? "RUN" : "Not Run");

		//DIO Input Log
		LOGDISPLAY_SPEC(11)(_T(">>> PLC Run SigIn Stat<<<  < %s>"), (nRet == TRUE)? "RUN" : "Not Run");

	}

	return nRet;
}

int CSigProc::SigInEncoderZeroReset()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_EncoderReset;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_EncoderReset;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}

int CSigProc::SigInTabZeroReset()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_TabZeroReset;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_TabZeroReset;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}


int CSigProc::SigInInkMarkActive()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_InkMarkingActive;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_InkMarkingActive;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}



int CSigProc::SigInRecipeChange()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_RecipeChange;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_RecipeChange;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}

int CSigProc::SigInLotStart()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_LotStartReq;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_LotStartReq;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}

int CSigProc::SigInLotEnd()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_LotEndReq;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_LotEndReq;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}


int CSigProc::SigInAlarmReset()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_AlarmResetReq;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_AlarmResetReq;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}

int CSigProc::SigInAlarmNgAck()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_AlarmNgAck;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_AlarmNgAck;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}

int CSigProc::SigInConnectZone()
{
	int nRet = 0;
	int nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitIn_ConnectZone;

		short nInputData;
		if (m_pPioCtrl->InPortByteThread(nAddress, nInputData) < 0)
		{
			nRet = -1;
		}
		else
		{
			nRet = nInputData;
		}

	}
	else
	{
		nAddress = enBitIn_ConnectZone;
		nRet = SignalPortCheck(nAddress);
	}

	return nRet;
}







//////////////////////////////////////////////////////////////////////////
// [ PLC SIGNAL OUTPUT ]
int CSigProc::SigOutAlivePulse(int nInMode)
{

	int nMode = 0;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_Alive;
		
		if(m_bSmsAlive == TRUE )
		{
			m_bSmsAlive = FALSE;
			nMode = FALSE;
		}
		else
		{
			m_bSmsAlive = TRUE;
			nMode = TRUE;
		}
	}
	else {
		nAddress = enBitOut_Alive;

		int nLocalRet = SignalBitOut(nAddress, -1);
		if (nInMode == FALSE) {
			nMode = FALSE;
		}
		else {
			if (nLocalRet == TRUE) {
				nMode = FALSE;
			}
			else {
				nMode = TRUE;
			}
		}
	}

	int nRet = SignalBitOut(nAddress, nMode);

	return nRet;
}

int CSigProc::SigOutAlivePulseReady(int nInMode, BOOL bIsReady)
{
	int nRet = 0;
	int nMode = 0;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		nAddress = enSmsBitOut_Alive;

		if (m_bSmsAlive == TRUE)
		{
			m_bSmsAlive = FALSE;
			nMode = FALSE;
		}
		else
		{
			m_bSmsAlive = TRUE;
			nMode = TRUE;
		}

		short nData[2]; // [0] : Alive, [1] : Ready
		nData[0] = (nMode == 1) ? 1 : 0;
		nData[1] = (bIsReady == 1) ? 1 : 0;
		short* pData = (short*)(&nData);
		int nSize = sizeof(nData) / sizeof(short);
		nRet = WritePLC_Block_device(nAddress + AprData.m_System.m_nBitOut, pData, nSize);

	}
	else
	{
		nAddress = enBitOut_Alive;

		int nLocalRet = SignalBitOut(nAddress, -1);
		if (nInMode == FALSE)
		{
			nMode = FALSE;
		}
		else
		{
			if (nLocalRet == TRUE)
			{
				nMode = FALSE;
			}
			else
			{
				nMode = TRUE;
			}
		}

		nRet = SignalBitOut(nAddress, nMode);
	}

	return nRet;
}

int CSigProc::SigOutReady(int nMode)
{

	int nRet = 0;

	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_Ready;
	}
	else {
		nAddress = enBitOut_Ready;
	}

	nRet = SignalBitOut(nAddress, nMode);

	return nRet;
}
int CSigProc::SigOutEncoderZeroSet(int nMode)
{
	int nRet = 0;

	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_EncoderSet;
	}
	else {
		nAddress = enBitOut_EncoderSet;
	}

	nRet = SignalBitOut(nAddress, nMode);

	return nRet;
}
int CSigProc::SigOutRecipeChangeAck(int nMode)
{
	AprData.SaveDebugLog_Format(_T("PLC Recipe Change Ack Mode : %d"), nMode); 

	int nRet = 0;

	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_RecipeChangeAck;
	}
	else {
		nAddress = enBitOut_RecipeChangeAck;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}
int CSigProc::SigOutLotStartAck(int nMode)
{
	AprData.SaveDebugLog_Format(_T("PLC Lot Start Ack Mode : %d"), nMode);

	int nRet = 0;

	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_LotStartReqAck;
	}
	else {
		nAddress = enBitOut_LotStartReqAck;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}
int CSigProc::SigOutLotEndAck(int nMode)
{
	AprData.SaveDebugLog_Format(_T("PLC Lot End Ack Mode : %d"), nMode);

	int nRet = 0;

	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_LotEndReqAck;
	}
	else {
		nAddress = enBitOut_LotEndReqAck;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}

int CSigProc::SigOutTabZeroReset(int nMode)
{
	AprData.SaveDebugLog_Format(_T("PLC Tab Sero Reset Ack Mode : %d"), nMode);

	int nRet = 0;

	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		// 240221 kjk. dataReset to PLC
		SigOutDataReset();
		nAddress = enSmsBitOut_TabZeroReset;
	}
	else {
		nAddress = enBitOut_TabZeroReset;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}

int CSigProc::SigOutAlarmResetAck(int nMode)
{
	AprData.SaveDebugLog_Format(_T("PLC Alarm Reset Ack Mode : %d"), nMode);

	int nRet = 0;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_AlarmResetAck;
	}
	else {
		nAddress = enBitOut_AlramResetAck;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}

int CSigProc::SigOutAlarmNgResetAck(int nMode)
{
	AprData.SaveDebugLog_Format(_T("PLC Alarm Ng Reset Ack Mode : %d"), nMode);

	int nRet = 0;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_AlarmNgResetAck;
	}
	else {
		nAddress = enBitOut_AlramNgResetAck;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}

// 230221 kjk 문사원 요청에 의해
// LOT End 시 PLC 에 데이터 초기화 값 전달.
BOOL CSigProc::SigOutDataReset()
{
	int nAddress = CSigProc::GetWordAddress(CSigProc::enWordWrite_DataReportV1_Ea, MODE_WRITE);

#ifndef NEW_PLCTYPE
	AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV1 = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV2 = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV3 = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV4 = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV5 = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wDataReportV6 = 0;

	AprData.m_NowLotData.m_SeqDataOutSms.wContinueAlarmCount = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wSectorAlarmCount = 0;

	AprData.m_NowLotData.m_SeqDataOutSms.wAlarmCode[0] = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wAlarmCode[1] = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wAlarmCode[2] = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wAlarmCode[3] = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wAlarmCode[4] = 0;

	AprData.m_NowLotData.m_SeqDataOutSms.wTopNgRealTimeCount = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wBottomNgRealTimeCount = 0;

	AprData.m_NowLotData.m_SeqDataOutSms.wTopNgLotEndCount = 0;
	AprData.m_NowLotData.m_SeqDataOutSms.wBottomNgLotEndCount = 0;

	short* pData = (short*)(&AprData.m_NowLotData.m_SeqDataOutSms);
	int nSize = sizeof(_SEQ_OUT_DATA_SMS) / sizeof(WORD);
	if (WritePLC_Block_device(nAddress, pData, nSize) != 0)
	{
		AprData.SaveDebugLog(_T("[SigOutDataReset] DATA SEND Error")); 
	}

#endif //NEW_PLCTYPE

	return 0;
}


int CSigProc::SigOutDiskCapacityAlarm(int nMode)
{
	int nRet = 0;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_DiskSpaceAlarm;
	}
	else {
		nAddress = enBitOut_DiskSpaceAlarm;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}

int CSigProc::SigOutDiskCapacityWarning(int nMode)
{
	int nRet = 0;

	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_DiskSpaceWarning;
	}
	else {
		nAddress = enBitOut_DiskSpaceWarning;
	}

	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}


int CSigProc::SigOutAlarmExist(int nMode)
{
	int nRet = 0;
	int address = GetWordAddress(enWordWrite_AlarmExist, MODE_WRITE);
	int nNumOfData = 1;

	if (m_pPioCtrl == NULL)
	{
		//에러로그
		return (-1);
	}


 	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
 	{
		// 지멘스 일때는 알람 코드와 같이 보냄
//		short pData = (nMode==1) ? 0x01 : 0x00;  
//		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)& pData, nNumOfData) < 0) {
//			//에러로그
//			return (-1);
//		}
 	}
 	else
 	{
 		short pData = (nMode == 1) ? 0x01 : 0x00;
 		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)& pData, nNumOfData) < 0) {
 			//에러로그
 			return (-1);
 		}
 	}




	return nRet;
}

int CSigProc::ReadBlockAllData_Melsec(CSequenceData* pSeqData)
{

	ASSERT(pSeqData);
	// RecipeInfo
	WORD btData[enWordReadMaxSize];
	memset(btData, 0, sizeof(btData));

	WORD* pData = btData;
	int nSize = enWordReadMaxSize;
	int nAddress = AprData.m_System.m_nWordIn + enWordRead_RecipeNo;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0)
	{
		return -1;
	}

#ifndef NEW_PLCTYPE
	memcpy(m_wMonitoringReadData_Melsec, btData, sizeof(btData));
#endif //NEW_PLCTYPE

	// RecipeNo 
	WORD btTemp = btData[enWordRead_RecipeNo];
	pSeqData->wRecipeNo = (DWORD)btTemp;
	CString strBuffer;
	int nMax = enWordRead_RecipeName_Len;
	WORD* pwData = &btData[enWordRead_RecipeName];
	for (int i = 0; i < nMax; i += 2)
	{
		btTemp = (BYTE)(*pwData & 0xff);;
		strBuffer += _T(" ");
		strBuffer.SetAt(i, (char)btTemp);
		if ((i + 1) < nMax) {
			btTemp = (BYTE)((*pwData >> 8) & 0xff);
			strBuffer += _T(" ");
			strBuffer.SetAt((i + 1), (char)btTemp);
		}
		pwData++;
	}
	strBuffer.TrimRight();
	strBuffer.TrimLeft();
	AprData.m_SeqDataIN.strRecipeName = strBuffer; // 22.08.11 Ahn Recovered // Use save log
	pSeqData->strRecipeName = strBuffer;

	nMax = enWordRead_CELL_ID_Len;
	pwData = &btData[enWordRead_CELL_ID];

	for (int i = 0; i < nMax; i += 2)
	{
		btTemp = (BYTE)(*pwData & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt(i, (char)btTemp);
		if ((i + 1) < nMax)
		{
			btTemp = (BYTE)((*pwData >> 8) & 0xff);
			strBuffer += _T(" ");
			strBuffer.SetAt((i + 1), (char)btTemp);
		}
		pwData++;
	}
	strBuffer.TrimRight();
	strBuffer.TrimLeft();
	AprData.m_SeqDataIN.strCell_ID = strBuffer;

	pwData = &btData[enWordRead_FoilExpInTopTarget];
	pSeqData->wFoilExpInTopTarget = *pwData;

	pwData = &btData[enWordRead_FoilExpInBtmTarget];
	pSeqData->wFoilExpInBottomTarget = *pwData;

	pwData = &btData[enWordRead_FoilExpOutTopTarget];
	pSeqData->wFoilExpOutTopTarget = *pwData;

	pwData = &btData[enWordRead_FoilExpOutBtmTarget];
	pSeqData->wFoilExpOutBottomTarget = *pwData;

	pwData = &btData[enWordRead_FoilExpBothTopTarget];
	pSeqData->wFoilExpBothTopTarget = *pwData;

	pwData = &btData[enWordRead_FoilExpBothBtmTarget];
	pSeqData->wFoilExpBothBottomTarget = *pwData;

	pwData = &btData[enWordRead_SpeterTopTarget];
	pSeqData->wSpeterTopTarget = *pwData;

	pwData = &btData[enWordRead_SpeterBtmTarget];
	pSeqData->wSpeterBottomTarget = *pwData;

	pwData = &btData[enWordRead_PrmContinuousCnt];
	pSeqData->wContinousCount = *pwData;

	pwData = &btData[enWordRead_PrmSectorNgTabCnt];
	pSeqData->wSectorNgCount = *pwData;

	pwData = &btData[enWordRead_PrmSectorBaseCnt];
	pSeqData->wSectorBaseCount = *pwData;



	return 0;
}

void CSigProc::EnableWorkSet(BOOL bMode)
{
	if (m_pPioCtrl != NULL) {
		m_pPioCtrl->EnableWorkSet(bMode);
	}
}

int CSigProc::ReadBlockWriteDataAll_Melsec(_SEQ_OUT_DATA_ALL* pSeqOutDataAll)
{
	_SEQ_OUT_DATA* pSeqOutData;
	pSeqOutData = &pSeqOutDataAll->m_SeqOutData ;

	ASSERT(pSeqOutData);

	WORD btData[enWordWriteMaxSize];
	memset(btData, 0, sizeof(btData));

	WORD* pData = btData;
	int nSize = enWordWriteMaxSize;
	int nAddress = AprData.m_System.m_nWordOut + enWordWrite_DataReportV1_Ea;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
		return -1;
	}

#ifndef NEW_PLCTYPE
	memcpy(m_wMonitoringWriteData_Melsec, btData, sizeof(btData));
#endif //NEW_PLCTYPE


	WORD btTemp;
	btTemp = btData[enWordWrite_DataReportV1_Ea];
	pSeqOutData->dwDataReportV1 = (DWORD)btTemp;

	btTemp = btData[enWordWrite_DataReportV2_OK];
	pSeqOutData->dwDataReportV2 = (DWORD)btTemp;

	btTemp = btData[enWordWrite_DataReportV3_NG];
	pSeqOutData->dwDataReportV3 = (DWORD)btTemp;

	btTemp = btData[enWordWrite_DataReportV4_OkRate];
	pSeqOutData->dwDataReportV4 = (DWORD)btTemp;

	btTemp = btData[enWordWrite_DataReportV6_RunRate];
	pSeqOutData->dwDataReportV5 = (DWORD)btTemp;

	btTemp = btData[enWordWrite_Continue_Alarm_Cnt];
	pSeqOutData->dwDataReportV6 = (DWORD)btTemp;

	btTemp = btData[enWordWrite_FoilExpInTop_Alarm_Cnt];
	pSeqOutData->dwFoilExpInTopCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_FoilExpInBottom_Alarm_Cnt];
	pSeqOutData->dwFoilExpInBottomCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_FoilExpOutTop_Alarm_Cnt];
	pSeqOutData->dwFoilExpOutTopCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_FoilExpOutTop_Alarm_Cnt];
	pSeqOutData->dwFoilExpOutBottomCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_FoilExpBothTop_Alarm_Cnt];
	pSeqOutData->dwFoilExpBothTopCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_FoilExpBothTop_Alarm_Cnt];
	pSeqOutData->dwFoilExpBothBottomCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_SpeterTop_Alarm_Cnt];
	pSeqOutData->dwSpeterTopCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_SpeterBtm_Alarm_Cnt];
	pSeqOutData->dwSpeterBottomCount = (DWORD)btTemp;





	btTemp = btData[enWordWrite_Top_Defect_Count_Real];
	pSeqOutData->dwTopNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_Btm_Defect_Count_Real];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_Top_Defect_Count_LotEnd];
	pSeqOutData->dwTopNgLotEndCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_Btm_Defect_Count_LotEnd ];
	pSeqOutData->dwBottomNgLotEndCount = (DWORD)btTemp;



	btTemp = btData[enWordWrite_AlarmExist];
	pSeqOutData->dwAlarmExist = (DWORD)btTemp;

	btTemp = btData[enWordWrite_AlarmCode_Buffer1];
	pSeqOutData->dwAlarmCode[0] = (DWORD)btTemp;




	return 0;

}


int CSigProc::ReportJudge(int nID, int nJudge, int nCode)
{
	int address = GetWordAddress(en_WordWrite_Cell_Trigger_ID, MODE_WRITE);

	short pData[6];
	int nNumOfData = 6;
	memset(pData, 0x00, sizeof(short) * nNumOfData);

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		pData[1] = (short)nID;
		pData[3] = (short)nJudge;
		pData[5] = (short)nCode;
	}
	else {
		nNumOfData = 3;
		pData[0] = (short)nID;
		pData[1] = (short)nJudge;
		pData[2] = (short)nCode;
	}

	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}
	if (m_pPioCtrl->WritePLC_Block_device(address, pData, nNumOfData) < 0) {
		//에러로그
		return (-1);
	}
	return (0);
}

int CSigProc::ReadBlockAllData(CSequenceData* pSeqData)
{
	int nRet = 0; 
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		nRet = ReadBlockAllData_Melsec( pSeqData);
	}
	else {
		nRet = ReadBlockAllData_Siemens( pSeqData);
	}
	return nRet;
}

int CSigProc::ReadBlockAllData_Siemens(CSequenceData* pSeqData)
{
	int nRet = 0;
	ASSERT(pSeqData);
	WORD btData[enSmsWordReadMaxSize];
	memset(btData, 0, sizeof(btData));

	WORD* pData = btData;
	int nSize = enSmsWordReadMaxSize;
	int nAddress = AprData.m_System.m_nWordIn + enSmsWordRead_RecipeNo;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
		return -1;
	}

#ifndef NEW_PLCTYPE
	memcpy(m_wMonitoringReadData_Siemens, btData, sizeof(btData));
#endif //NEW_PLCTYPE


	// RecipeNo 
	WORD btTemp = btData[enSmsWordRead_RecipeNo];
	pSeqData->wRecipeNo = (DWORD)btTemp;

	int i;
	int Cnt = 0;
	CString strBuffer;
	int nMax = enSmsWordRead_RecipeName_Len;
	WORD* pwData;
	pwData = &btData[enSmsWordRead_RecipeName];


	for (i = 0, Cnt = 0; i < nMax; i++, Cnt+=2 )
	{
		btTemp = (BYTE)(*pwData & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt(Cnt, (char)btTemp);

		btTemp = (BYTE)((*pwData >> 8) & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt((Cnt + 1), (char)btTemp);

		pwData++;
	}






	strBuffer.TrimRight();
	strBuffer.TrimLeft();
	AprData.m_SeqDataIN.strRecipeName = strBuffer; 
	pSeqData->strRecipeName = strBuffer;

	nMax = enSmsWordRead_CELL_ID_Len;
	pwData = &btData[enSmsWordRead_CELL_ID];

	for (i = 0, Cnt = 0; i < nMax; i++, Cnt += 2)
	{
		btTemp = (BYTE)(*pwData & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt(Cnt, (char)btTemp);

		btTemp = (BYTE)((*pwData >> 8) & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt((Cnt + 1), (char)btTemp);

		pwData++;
	}

	strBuffer.TrimRight();
	strBuffer.TrimLeft();
	AprData.m_SeqDataIN.strCell_ID = strBuffer;
	pSeqData->strCell_ID = strBuffer;


	pwData = &btData[enSmsWordRead_FoilExpInTopTarget];
	pSeqData->wFoilExpInTopTarget = *pwData;

	pwData = &btData[enSmsWordRead_FoilExpInBtmTarget];
	pSeqData->wFoilExpInBottomTarget = *pwData;

	pwData = &btData[enSmsWordRead_FoilExpOutTopTarget];
	pSeqData->wFoilExpOutTopTarget = *pwData;

	pwData = &btData[enSmsWordRead_FoilExpOutBtmTarget];
	pSeqData->wFoilExpOutBottomTarget = *pwData;

	pwData = &btData[enSmsWordRead_FoilExpBothTopTarget];
	pSeqData->wFoilExpBothTopTarget = *pwData;

	pwData = &btData[enSmsWordRead_FoilExpBothBtmTarget];
	pSeqData->wFoilExpBothBottomTarget = *pwData;

	pwData = &btData[enSmsWordRead_SpeterTopTarget];
	pSeqData->wSpeterTopTarget = *pwData;

	pwData = &btData[enSmsWordRead_SpeterBtmTarget];
	pSeqData->wSpeterBottomTarget = *pwData;

	pwData = &btData[enSmsWordRead_PrmContinuousCnt];
	pSeqData->wContinousCount = *pwData;

	pwData = &btData[enSmsWordRead_PrmSectorNgTabCnt];
	pSeqData->wSectorNgCount = *pwData;;

	pwData = &btData[enSmsWordRead_PrmSectorBaseCnt];
	pSeqData->wSectorBaseCount = *pwData;;

	return nRet;
}

#ifndef NEW_PLCTYPE
int CSigProc::ReadBlockWriteDataAll_Siemens(_SEQ_OUT_DATA_ALL_SMS* pSeqOutDataAll)
{
	_SEQ_OUT_DATA_SMS* pSeqOutData;
	pSeqOutData = &pSeqOutDataAll->m_SeqOutData;

	ASSERT(pSeqOutData);
	// RecipeInfo
	WORD btData[enSmsWordWriteMaxSize];
	memset(btData, 0, sizeof(btData));

	WORD* pData = btData;
	int nSize = enSmsWordWriteMaxSize;
	int nAddress = AprData.m_System.m_nWordOut + enSmsWordWrite_DataReportV1_Ea;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
		return -1;
	}

	memcpy(m_wMonitoringWriteData_Siemens, btData, sizeof(btData));


	WORD btTemp;
	btTemp = btData[enSmsWordWrite_DataReportV1_Ea];
	pSeqOutData->wDataReportV1 = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV2_OK];
	pSeqOutData->wDataReportV2 = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV3_NG];
	pSeqOutData->wDataReportV3 = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV4_OkRate];
	pSeqOutData->wDataReportV4 = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV5_NgRate];
	pSeqOutData->wDataReportV5 = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV6_RunRate];
	pSeqOutData->wDataReportV6 = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_Continue_Alarm_Cnt];
	pSeqOutData->wContinueAlarmCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_Heavy_Alarm_Cnt];
	pSeqOutData->wSectorAlarmCount = (WORD)btTemp;




	btTemp = btData[enSmsWordWrite_FoilExpInTop_Alarm_Cnt];
	pSeqOutData->wFoilExpInTopCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpInBtm_Alarm_Cnt];
	pSeqOutData->wFoilExpInBottomCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpOutTop_Alarm_Cnt];
	pSeqOutData->wFoilExpOutTopCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt];
	pSeqOutData->wFoilExpOutBottomCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpBothTop_Alarm_Cnt];
	pSeqOutData->wFoilExpBothTopCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt];
	pSeqOutData->wFoilExpBothBottomCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_SpeterTop_Alarm_Cnt];
	pSeqOutData->wSpeterTopCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_SpeterBtm_Alarm_Cnt];
	pSeqOutData->wSpeterBottomCount = (WORD)btTemp;


	btTemp = btData[enSmsWordWrite_Top_Defect_Count_Real];
	pSeqOutData->wTopNgRealTimeCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_Btm_Defect_Count_Real];
	pSeqOutData->wBottomNgRealTimeCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_Top_Defect_Count_LotEnd];
	pSeqOutData->wTopNgLotEndCount = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_Btm_Defect_Count_LotEnd];
	pSeqOutData->wBottomNgLotEndCount = (WORD)btTemp;


	
	btTemp = btData[enSmsWordWrite_FoilExpInTopTarget];
	pSeqOutData->wFoilExpInTopTarget = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpInBtmTarget];
	pSeqOutData->wFoilExpInBottomTarget = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpOutTopTarget];
	pSeqOutData->wFoilExpOutTopTarget = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpOutBtmTarget];
	pSeqOutData->wFoilExpOutBottomTarget = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpBothTopTarget];
	pSeqOutData->wFoilExpBothTopTarget = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpBothBtmTarget];
	pSeqOutData->wFoilExpBothBottomTarget = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_SpeterTopTarget];
	pSeqOutData->wSpeterTopTarget = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_SpeterBtmTarget];
	pSeqOutData->wSpeterBottomTarget = (WORD)btTemp;




	btTemp = btData[enSmsWordWrite_PrmContinuousCnt];
	pSeqOutData->wPrmContinuousCnt = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_PrmSectorNgTabCnt];
	pSeqOutData->wPrmSectorNgTabCnt = (WORD)btTemp;

	btTemp = btData[enSmsWordWrite_PrmSectorBaseCnt];
	pSeqOutData->wPrmSectorBaseCnt = (WORD)btTemp;






	btTemp = btData[enSmsWordWrite_AlarmExist];
	pSeqOutData->wAlarmExist = (WORD)btTemp;

	for(int i=0; i<24; i++ )
	{
		btTemp = btData[enSmsWordWrite_AlarmCode_Buffer1+i];
		pSeqOutData->wAlarmCode[i] = (WORD)btTemp;
	}

	return 0;
}
#endif //NEW_PLCTYPE

int CSigProc::GetWordAddress(int nAddress, int nMode/*Read or Write*/)
{
	int nRetAdd = nAddress;

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		return nRetAdd;
	}
	else {
		if (nMode == MODE_READ) {
			switch (nAddress) {
			case	enWordRead_RecipeNo:
				nRetAdd = enSmsWordRead_RecipeNo;
				break;
			case	enWordRead_RecipeName :
				nRetAdd = enSmsWordRead_RecipeName;
				break;
			case	enWordRead_CELL_ID :
				nRetAdd = enSmsWordRead_CELL_ID;
				break;


			case	enWordRead_FoilExpInTopTarget:
				nRetAdd = enSmsWordRead_FoilExpInTopTarget;
				break;
			case	enWordRead_FoilExpInBtmTarget:
				nRetAdd = enSmsWordRead_FoilExpInBtmTarget;
				break;
			case	enWordRead_FoilExpOutTopTarget:
				nRetAdd = enSmsWordRead_FoilExpOutTopTarget;
				break;
			case	enWordRead_FoilExpOutBtmTarget:
				nRetAdd = enSmsWordRead_FoilExpOutBtmTarget;
				break;
			case	enWordRead_FoilExpBothTopTarget:
				nRetAdd = enSmsWordRead_FoilExpBothTopTarget;
				break;
			case	enWordRead_FoilExpBothBtmTarget:
				nRetAdd = enSmsWordRead_FoilExpBothBtmTarget;
				break;
			case	enWordRead_SpeterTopTarget :
				nRetAdd = enSmsWordRead_SpeterTopTarget;
				break;
			case	enWordRead_SpeterBtmTarget :
				nRetAdd = enSmsWordRead_SpeterBtmTarget;
				break;




			case	enWordRead_PrmContinuousCnt :
				nRetAdd = enSmsWordRead_PrmContinuousCnt;
				break;
			case	enWordRead_PrmSectorNgTabCnt :
				nRetAdd = enSmsWordRead_PrmSectorNgTabCnt;
				break;
			case	enWordRead_PrmSectorBaseCnt :
				nRetAdd = enSmsWordRead_PrmSectorBaseCnt;
				break;

//			case	enWordRead_AlarmExistAck :
//				nRetAdd = enSmsWordRead_AlarmExistAck;
//				break;


			default :
				nRetAdd = enSmsWordReadMaxSize ;
				break;
			}

			nRetAdd += AprData.m_System.m_nWordIn;

		}
		else {
			switch (nAddress) {
			case	enWordWrite_DataReportV1_Ea :
				nRetAdd = enSmsWordWrite_DataReportV1_Ea;
				break;
			case	enWordWrite_DataReportV2_OK :
				nRetAdd = enSmsWordWrite_DataReportV2_OK;
				break;
			case	enWordWrite_DataReportV3_NG :
				nRetAdd = enSmsWordWrite_DataReportV3_NG;
				break;
			case	enWordWrite_DataReportV4_OkRate :
				nRetAdd = enSmsWordWrite_DataReportV4_OkRate;
				break;
			case	enWordWrite_DataReportV5_NgRate :
				nRetAdd = enSmsWordWrite_DataReportV5_NgRate;
				break;
			case	enWordWrite_DataReportV6_RunRate :
				nRetAdd = enSmsWordWrite_DataReportV6_RunRate;
				break;
			case	enWordWrite_Continue_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_Continue_Alarm_Cnt;
				break;
			case	enWordWrite_Heavy_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_Heavy_Alarm_Cnt;
				break;






			case	enWordWrite_FoilExpInTop_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_FoilExpInTop_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpInBottom_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_FoilExpInBtm_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpOutTop_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_FoilExpOutTop_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpOutBottom_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpBothTop_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_FoilExpBothTop_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpBothBottom_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt;
				break;
			case	enWordWrite_SpeterTop_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_SpeterTop_Alarm_Cnt;
				break;
			case	enWordWrite_SpeterBtm_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_SpeterBtm_Alarm_Cnt;
				break;



			case	enWordWrite_Top_Defect_Count_Real :
				nRetAdd = enSmsWordWrite_Top_Defect_Count_Real;
				break;
			case	enWordWrite_Btm_Defect_Count_Real :
				nRetAdd = enSmsWordWrite_Btm_Defect_Count_Real;
				break;
			case	enWordWrite_Top_Defect_Count_LotEnd :
				nRetAdd = enSmsWordWrite_Top_Defect_Count_LotEnd;
				break;
			case	enWordWrite_Btm_Defect_Count_LotEnd :
				nRetAdd = enSmsWordWrite_Btm_Defect_Count_LotEnd;
				break;



			case	enWordWrite_FoilExpInTopTarget :
				nRetAdd = enSmsWordWrite_FoilExpInTopTarget;
				break;
			case	enWordWrite_FoilExpInBottomTarget:
				nRetAdd = enSmsWordWrite_FoilExpInBtmTarget;
				break;
			case	enWordWrite_FoilExpOutTopTarget:
				nRetAdd = enSmsWordWrite_FoilExpOutTopTarget;
				break;
			case	enWordWrite_FoilExpOutBottomTarget :
				nRetAdd = enSmsWordWrite_FoilExpOutBtmTarget;
				break;
			case	enWordWrite_FoilExpBothTopTarget:
				nRetAdd = enSmsWordWrite_FoilExpBothTopTarget;
				break;
			case	enWordWrite_FoilExpBothBottomTarget:
				nRetAdd = enSmsWordWrite_FoilExpBothBtmTarget;
				break;
			case	enWordWrite_SpeterTopTarget :
				nRetAdd = enSmsWordWrite_SpeterTopTarget;
				break;
			case	enWordWrite_SpeterBtmTarget :
				nRetAdd = enSmsWordWrite_SpeterBtmTarget;
				break;


			case	enWordWrite_PrmContinuousCnt:
				nRetAdd = enSmsWordWrite_PrmContinuousCnt;
				break;
			case	enWordWrite_PrmSectorNgTabCnt:
				nRetAdd = enSmsWordWrite_PrmSectorNgTabCnt;
				break;
			case	enWordWrite_PrmSectorBaseCnt:
				nRetAdd = enSmsWordWrite_PrmSectorBaseCnt;
				break;



			case	enWordWrite_AlarmExist :
				nRetAdd = enSmsWordWrite_AlarmExist;
				break;
			case	enWordWrite_AlarmCode_Buffer1 :
				nRetAdd = enSmsWordWrite_AlarmCode_Buffer1;
				break;
			case	enWordWrite_AlarmCode_Buffer2 :
				nRetAdd = enSmsWordWrite_AlarmCode_Buffer2;
				break;
			case	enWordWrite_AlarmCode_Buffer3 :
				nRetAdd = enSmsWordWrite_AlarmCode_Buffer3;
				break;
			case	enWordWrite_AlarmCode_Buffer4 :
				nRetAdd = enSmsWordWrite_AlarmCode_Buffer4;
				break;
			case	enWordWrite_AlarmCode_Buffer5 :
				nRetAdd = enSmsWordWrite_AlarmCode_Buffer5;
				break;





			case	en_WordWrite_Cell_Trigger_ID :
				nRetAdd = en_SmsWordWrite_Cell_Trigger_ID;
				break;
			case	en_WordWrite_Judge :
				nRetAdd = en_SmsWordWrite_Judge;
				break;
			case	en_WordWrite_NG_Code :
				nRetAdd = en_SmsWordWrite_NG_Code;
				break;
			}

			nRetAdd += AprData.m_System.m_nWordOut;

		}

	}

	return nRetAdd;
}
