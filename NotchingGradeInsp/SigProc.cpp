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

	if (theApp.m_pPioCtrl != NULL) {
		m_pPioCtrl = theApp.m_pPioCtrl;
	}

	//m_nMaxBitIN = 0;
	//m_nMaxBitOUT = 0;
	//memset(m_sIOBitIN, 0x00, sizeof(m_sIOBitIN));
	//memset(m_sIOBitOUT, 0x00, sizeof(m_sIOBitOUT) );

	m_bSmsAlive = FALSE;
}

CSigProc::~CSigProc()
{
	
}

int CSigProc::GetPortBit(int nIntegration, int* piPort, BYTE* pByte)
{
	ASSERT(piPort != NULL);
	ASSERT(pByte != NULL);

	int	nRet = 0;
	// 23.03.02 Ahn Modify Start
	//*piPort = (int)((nIntegration >> 8) & 0xff);
	//*pByte = (BYTE)(nIntegration & 0xff);
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
	// 23.03.02 Ahn Modify End

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
//		AprData.SaveDebugLog(_T("SignalPortCheck start")); //pyjtest

		short data = 0;
		int nSize = sizeof(short) / sizeof(short);
		if (ReadPLC_Block_device(iPort + AprData.m_System.m_nBitIn, &data, nSize) != -1)
		{
// 			CString strMsg;
// 			strMsg.Format(_T("SignalPortCheck Addr:%d, nSize:%d, data:%d"), iPort + AprData.m_System.m_nBitIn, nSize, data  );
// 			AprData.SaveDebugLog(strMsg); //pyjtest


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


// 	int	nRet = pPioCtrl->Out_Port_Bit(iPort, cByte, nMode);
// 	if (nRet < 0) {
// 		//에러로그
// 		return (-1);
// 	}


// 	CString strMsg;
// 	strMsg.Format(_T("%d, %d, %d"), nIntegration, iPort, cByte);
// 	AprData.SaveDebugLog(strMsg); //pyjtest

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

// 22.03.23 Ahn Add Start
int CSigProc::ReadAllPort_BitIn( BOOL* pSigBitIn )
{
	ASSERT(pSigBitIn);
	int nRet = 0;
	if (m_pPioCtrl == NULL) return -1;

// 	m_pPioCtrl->ReadAllPort_BitIn( m_btSigBItIN, MAX_USE_PORT );
// 	int nBitPos = 0 ;
// 	for (int port = 0; port < MAX_USE_PORT; port++) {
// 		for (int bit = 0; bit < MAX_PORT_BIT; bit++) {
// 			if (m_btSigBItIN[port] & (0x01 << bit)) {
// 				nBitPos = (port * MAX_PORT_BIT) + bit;
// 				pSigBitIn[nBitPos] = TRUE;
// 			}
// 		}
// 	}



	//로그출력 값
	CString bitVal = "";

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		m_pPioCtrl->ReadAllPort_BitIn( (BYTE*)m_sSmsSigBItIN, MAX_SMS_IO_IN);
		for (int i = 0; i < MAX_SMS_IO_IN; i++)
		{
// 			CString strMsg;
// 			strMsg.Format(_T("m_sSmsSigBItIN[%d] = %d"), i, m_sSmsSigBItIN[i]);
// 			AprData.SaveDebugLog(strMsg); //pyjtest

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

// 	m_pPioCtrl->ReadAllPort_BitOut( m_btSigBItOut, MAX_USE_PORT);
// 	int nBitPos = 0;
// 	for (int port = 0; port < MAX_USE_PORT; port++) {
// 		for (int bit = 0; bit < MAX_PORT_BIT; bit++) {
// 			if (m_btSigBItOut[port] & (0x01 << bit)) {
// 				nBitPos = (port * MAX_PORT_BIT) + bit;
// 				pSigBitOut[nBitPos] = TRUE;
// 			}
// 		}
// 	}


	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
//		memset(m_sSmsSigBItOut, 0x00, sizeof(short) * MAX_SMS_IO_OUT);
		m_pPioCtrl->ReadAllPort_BitOut((BYTE*)m_sSmsSigBItOut, MAX_SMS_IO_OUT);
		for (int i = 0; i < MAX_SMS_IO_OUT; i++)
		{
// 			CString strMsg;
// 			strMsg.Format(_T("m_sSmsSigBItOut[%d] = %d"), i, m_sSmsSigBItOut[i]);
// 			AprData.SaveDebugLog(strMsg); //pyjtest

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
// 22.03.23 Ahn Add End

// 22.03.24 Ahn Add Start
int CSigProc::SetAlarmCode(WORD wAlarmCode)
{
	m_pPioCtrl->SetAlarmCode(wAlarmCode);

	return 0;
}
WORD CSigProc::GetAlarmCode()
{
	return m_pPioCtrl->GetAlarmCode();
}

// 22.03.28 Ahn Add Start
int CSigProc::WriteAlarmCode(WORD nAlarmCode)
{
	// 23.03.03 Ahn Modify Start
	//int address = enWordWrite_AlarmCode_Buffer1;
	int address = GetWordAddress(enWordWrite_AlarmCode_Buffer1, MODE_WRITE);
	// 23.03.03 Ahn Modify End


// 	int nNumOfData = 1;
// 	short* pData;
// 	pData = (short*)&nAlarmCode;
// 
// 	if (m_pPioCtrl == NULL) {
// 		//에러로그
// 		return (-1);
// 	}
// 
// 	if (m_pPioCtrl->WritePLC_Block_device(address, pData, nNumOfData) < 0) {
// 		//에러로그
// 		return (-1);
// 	}


	int nNumOfData = 1;

	if (m_pPioCtrl == NULL) {
		//에러로그
		return (-1);
	}


	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		short nAlarmCheck = 0x0001;
		nNumOfData = 10;
		short pData[10] = { 0, };
		for (int i = 0; i < nNumOfData; i++)
		{
			if ((nAlarmCode & nAlarmCheck) != 0)
			{
				pData[i] = 0x0001;
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
		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)&nAlarmCode, nNumOfData) < 0) {
			//에러로그
			return (-1);
		}
	}



	return (0);
}
// 22.03.28 Ahn Add End
// 23.03.03 Ahn Delete Start
//// 22.03.24 Ahn Modify Strat
//int CSigProc::WriteAlarmCode()
//{
//	int address = enWordWrite_AlarmCode_Buffer1;
//	int nNumOfData = 1;
////	short* pData;
////	pData = (short*)&nAlarmCode;
//
//	if (m_pPioCtrl == NULL) {
//		//에러로그
//		return (-1);
//	}
////	if (m_pPioCtrl->WritePLC_Block_device(address, NULL, nNumOfData) < 0) {
//	if( m_pPioCtrl->WriteAlarmCode(address) < 0 ) {
//		//에러로그
//		return (-1);
//	}
//	return (0);
//}
//// 22.03.24 Ahn Modify End
// 23.03.03 Ahn Delete End

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

// 22.07.06 Ahn Add Start
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
// 22.07.06 Ahn Add End


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
			nRet = -1;
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










//////////////////////////////////////////////////////////////////////////
// [ PLC SIGNAL OUTPUT ]
int CSigProc::SigOutAlivePulse(int nInMode)
{
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_Alive;

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

		// 23.02.02 Ahn Add End
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

// 	CString strMsg;
// 	strMsg.Format(_T("Alive : Addr %d, Mode %d"), nAddress, nMode);
// 	AprData.SaveDebugLog(strMsg); //pyjtest


	return nRet;
}
int CSigProc::SigOutReady(int nMode)
{

	CString strMsg;
	strMsg.Format(_T("SigOutReady %d"), nMode);
	AprData.SaveDebugLog(strMsg); //pyjtest

	int nRet = 0;
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_Ready;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_Ready;
	}
	else {
		nAddress = enBitOut_Ready;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);

	return nRet;
}
int CSigProc::SigOutEncoderZeroSet(int nMode)
{
	int nRet = 0;

	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_EncoderSet ;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_EncoderSet;
	}
	else {
		nAddress = enBitOut_EncoderSet;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);

	return nRet;
}
int CSigProc::SigOutRecipeChangeAck(int nMode)
{
	AprData.SaveDebugLog(_T("SigOutRecipeChangeAck")); //pyjtest

	int nRet = 0;
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_RecipeChangeAck;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_RecipeChangeAck;
	}
	else {
		nAddress = enBitOut_RecipeChangeAck;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}
int CSigProc::SigOutLotStartAck(int nMode)
{
	AprData.SaveDebugLog(_T("SigOutLotStartAck")); //pyjtest

	int nRet = 0;
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_LotStartReqAck;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_LotStartReqAck;
	}
	else {
		nAddress = enBitOut_LotStartReqAck;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}
int CSigProc::sigOutLotEndAck(int nMode)
{
	AprData.SaveDebugLog(_T("sigOutLotEndAck")); //pyjtest

	int nRet = 0;
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_LotEndReqAck;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_LotEndReqAck;
	}
	else {
		nAddress = enBitOut_LotEndReqAck;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}
// 22.02.23 Ahn Add Start
int CSigProc::SigOutTabZeroReset(int nMode)
{
	AprData.SaveDebugLog(_T("SigOutTabZeroReset")); //pyjtest

	int nRet = 0;
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_TabZeroReset;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_TabZeroReset;
	}
	else {
		nAddress = enBitOut_TabZeroReset;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}
// 22.02.23 Ahn Add End

int CSigProc::SigOutAlarmResetAck(int nMode)
{
	AprData.SaveDebugLog(_T("SigOutAlarmResetAck")); //pyjtest

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




	// 22.08.05 Ahn Add Start
int CSigProc::SigOutDiskCapacityAlarm(int nMode)
{
	int nRet = 0;
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_DiskSpaceAlarm;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_DiskSpaceAlarm;
	}
	else {
		nAddress = enBitOut_DiskSpaceAlarm;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}

int CSigProc::SigOutDiskCapacityWarning(int nMode)
{
	int nRet = 0;
	// 23.02.02 Ahn Add Start
	//int nAddress = enBitOut_DiskSpaceWarning;
	int nAddress;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		nAddress = enSmsBitOut_DiskSpaceWarning;
	}
	else {
		nAddress = enBitOut_DiskSpaceWarning;
	}
	// 23.02.02 Ahn Add End
	nRet = SignalBitOut(nAddress, nMode);
	return nRet;
}
// 22.08.05 Ahn Add End



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


// 	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
// 	{
		short pData = (nMode==1) ? 0x01 : 0x00;
		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)& pData, nNumOfData) < 0) {
			//에러로그
			return (-1);
		}
// 	}
// 	else
// 	{
// 		short pData = (nMode == 1) ? 0x01 : 0x00;
// 		if (m_pPioCtrl->WritePLC_Block_device(address, (short*)& pData, nNumOfData) < 0) {
// 			//에러로그
// 			return (-1);
// 		}
// 	}




	return nRet;
}



// 22.08.04 Ahn Add Start
int CSigProc::ReadBlockAllData_Melsec(CSequenceData* pSeqData)
{
// 22.08.04 Ahn Add End
	ASSERT(pSeqData);
	// RecipeInfo
	WORD btData[enWordReadMaxSize];
	WORD* pData = btData;
	// 22.03.22 Ahn Modify Start
	int nSize = enWordReadMaxSize / 2 ;
	//int nSize = enWordRead_LotInfoLength / 2;
	// 22.03.22 Ahn Modify End
	int nAddress = enWordRead_RecipeNo;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
		return -1;
	}
	// RecipeNo 
	// 23.02.17 Son Mod Start
	//WORD btTemp = btData[enWordRead_RecipeNo - nAddress];
	WORD btTemp = btData[enWordRead_RecipeNo];
	// 23.02.17 Son Mod End
	// 22.08.04 Ahn Modify Start
	//AprData.m_SeqDataIN.wRecipeNo = (DWORD)btTemp;
	pSeqData->wRecipeNo = (DWORD)btTemp;
	// 22.08.04 Ahn Modify End
	//	AprData.m_SeqDataIN.wRecipeNo = btData[enWordRead_RecipeNo - nAddress];
	int i;
	CString strBuffer;
	int nMax = enWordRead_RecipeName_Len;
	WORD* pwData;
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_RecipeName - nAddress];
	pwData = &btData[enWordRead_RecipeName];
	// 23.02.17 Son Mod End
	for (i = 0; i < nMax; i += 2) {
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
	// 22.07.13 Ahn Add Start
	strBuffer.TrimRight();
	strBuffer.TrimLeft();
	// 22.07.13 Ahn Add End
	// 22.08.04 Ahn Modify Start
	AprData.m_SeqDataIN.strRecipeName = strBuffer; // 22.08.11 Ahn Recovered // Use save log
	pSeqData->strRecipeName = strBuffer;
	// 22.08.04 Ahn Modify End

	nMax = enWordRead_CELL_ID_Len;
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_CELL_ID - nAddress];
	pwData = &btData[enWordRead_CELL_ID];
	// 23.02.17 Son Mod End

	for (i = 0; i < nMax; i += 2) {
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

	// 22.07.13 Ahn Add Start
	strBuffer.TrimRight();
	strBuffer.TrimLeft();
	// 22.07.13 Ahn Add End
	// 22.08.11 Ahn Modify Start
	//pSeqData->strRecipeName = strBuffer;
	AprData.m_SeqDataIN.strCell_ID = strBuffer;
	// 22.08.11 Ahn Modify End

	// RecipeName
	// Cell ID
	// Speed 나누기 100 할것(소수점 2자리)
	// Dross Top 목표값
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_DrossTopTarget - nAddress];
	pwData = &btData[enWordRead_DrossTopTarget];
	// 23.02.17 Son Mod End
	// 22.08.04 Ahn Modify Start
	//AprData.m_SeqDataIN.wDrossTopTarget = *pwData;
	pSeqData->wDrossTopTarget = *pwData;
	// 22.08.04 Ahn Modify End

	// Dross Bottom 목표값
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_DrossBtmTarget - nAddress];
	pwData = &btData[enWordRead_DrossBtmTarget];
	// 23.02.17 Son Mod End
	// 22.08.04 Ahn Modify Start
	//AprData.m_SeqDataIN.wDrossBottomTarget = *pwData;
	pSeqData->wDrossBottomTarget = *pwData;
	// 22.08.04 Ahn Modify End
	// FoilExp Top 목표값
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_FoilExpTopTarget - nAddress];
	pwData = &btData[enWordRead_FoilExpTopTarget];
	// 23.02.17 Son Mod End
	// 22.08.04 Ahn Modify Start
	//AprData.m_SeqDataIN.wFoilExpTopTarget = *pwData;
	pSeqData->wFoilExpTopTarget = *pwData;
	// 22.08.04 Ahn Modify End
	// FoilExp Bottom 목표값
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_FoilExpBtmTarget - nAddress];
	pwData = &btData[enWordRead_FoilExpBtmTarget];
	// 23.02.17 Son Mod End
	// 22.08.04 Ahn Modify Start
	//AprData.m_SeqDataIN.wFoilExpBottomTarget = *pwData;
	pSeqData->wFoilExpBottomTarget = *pwData;
	// 22.08.04 Ahn Modify End
	// Spatter Top 목표값
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_SpeterTopTarget - nAddress];
	pwData = &btData[enWordRead_SpeterTopTarget];
	// 23.02.17 Son Mod End
	// 22.08.04 Ahn Modify Start
	//AprData.m_SeqDataIN.wSpeterTopTarget = *pwData;
	pSeqData->wSpeterTopTarget = *pwData;
	// 22.08.04 Ahn Modify End
	// Spatter Bottom 목표값
	// 23.02.17 Son Mod Start
	//pwData = &btData[enWordRead_SpeterBtmTarget - nAddress];
	pwData = &btData[enWordRead_SpeterBtmTarget];
	// 23.02.17 Son Mod End
	// 22.08.04 Ahn Modify Start
	//AprData.m_SeqDataIN.wSpeterBottomTarget = *pwData;
	pSeqData->wSpeterBottomTarget = *pwData;
	// 22.08.04 Ahn Modify End
	// Alarm Ack

	// 23.02.17 Son Mod Start
	//// 22.08.10 Ahn Add Start
	//pwData = &btData[enWordRead_PrmContinuousCnt - nAddress];
	//pSeqData->wContinousCount;
	//pwData = &btData[enWordRead_PrmSectorNgTabCnt - nAddress];
	//pSeqData->wSectorNgCount;
	//pwData = &btData[enWordRead_PrmSectorBaseCnt - nAddress];
	//pSeqData->wSectorBaseCount;


	pwData = &btData[enWordRead_PrmContinuousCnt];
	pSeqData->wContinousCount = *pwData;
	pwData = &btData[enWordRead_PrmSectorNgTabCnt];
	pSeqData->wSectorNgCount = *pwData;
	pwData = &btData[enWordRead_PrmSectorBaseCnt];
	pSeqData->wSectorBaseCount = *pwData;



	return 0;
}

int CSigProc::WriteBlockAllData_Melsec(int nMode )
{

	//WORD btData[32];
	//WORD* pData = btData;
	//int nSize = 1;

	////int nAddress = enWordWrite_DataReportV1_Ea;
	//int nAddress = enWordWrite_DataReportV1_Ea;
	//if (nMode == 0) {
	//	btData[0] = 0x00;
	//	btData[1] = 0x00;
	////	memset(btData, 0x32, sizeof(BYTE) * 32);
	//}
	//else {
	////	btData[0] = 0x11;
	////	btData[1] = 0x11;
	//	memset(btData, 0x0000, sizeof(WORD) * 32);
	//	btData[0] = 0x5555 ;
	//	btData[1] = 0x5555;
	//}

	//if (WritePLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
	//}

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
	// RecipeInfo
	WORD btData[enWordReadMaxSize];
	WORD* pData = btData;
	// 22.03.22 Ahn Modify Start
	//int nSize = enWordReadMaxSize / 2 ;
	int nSize = enWordReadMaxSize / 2;
	// 22.03.22 Ahn Modify End
	int nAddress = enWordWrite_DataReportV1_Ea;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
		return -1;
	}

	WORD btTemp;
	// 23.03.03 Ahn Modify Start - Delete "- nAddress"
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
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_Btm_Defect_Count_LotEnd ];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_AlarmExist];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enWordWrite_AlarmCode_Buffer1];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;
	// 23.03.03 Ahn Modify End

	return 0;

}
// 22.08.19 Ahn Add End


// 22.12.12 Ahn Add Start - Judge Report
int CSigProc::ReportJudge(int nID, int nJudge, int nCode)
{
	// 23.03.03 Ahn Modify Start
	//int address = en_WordWrite_Cell_Trigger_ID;
	int address = GetWordAddress(en_WordWrite_Cell_Trigger_ID, MODE_WRITE);
	// 23.03.03 Ahn Modify Start
	//short* pData = new short[nNumOfData] ;
	short pData[6];
	int nNumOfData = 6;
	memset(pData, 0x00, sizeof(short) * nNumOfData);
	// 23.03.03 Ahn Modify Start
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
	// 23.03.03 Ahn Modify End


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
// 22.12.12 Ahn Add End

// 22.08.19 Ahn Add Start
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
int CSigProc::WriteBlockAllData(int nMode)
{
	int nRet = 0;
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		nRet = WriteBlockAllData_Melsec(nMode);
	}
	else {
		nRet = WriteBlockAllData_Siemens(nMode);
	}
	return nRet;
}

int CSigProc::ReadBlockWriteDataAll(_SEQ_OUT_DATA_ALL* pSeqOutDataAll)
{
	int nRet = 0;
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		nRet = ReadBlockWriteDataAll_Melsec( pSeqOutDataAll);
	}
	else {
		nRet = ReadBlockWriteDataAll_Siemens(pSeqOutDataAll);

	}
	return nRet;
}

int CSigProc::ReadBlockAllData_Siemens(CSequenceData* pSeqData)
{
	int nRet = 0;
	ASSERT(pSeqData);
	WORD btData[enSmsWordReadMaxSize];
	WORD* pData = btData;
	int nSize = enSmsWordReadMaxSize;
	int nAddress = AprData.m_System.m_nWordIn + enSmsWordRead_RecipeNo;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
		return -1;
	}

	// RecipeNo 
	WORD btTemp = btData[enSmsWordRead_RecipeNo];
	pSeqData->wRecipeNo = (DWORD)btTemp;

	int i;
	int Cnt = 0;
	CString strBuffer;
	int nMax = enSmsWordRead_RecipeName_Len;
	WORD* pwData;
	pwData = &btData[enSmsWordRead_RecipeName];
// 	for (i = 0; i < nMax; i += 2) {
// 		btTemp = (BYTE)(*pwData & 0xff);
// 		strBuffer += _T(" ");
// 		strBuffer.SetAt(i, (char)btTemp);
// 		if ((i + 1) < nMax) {
// 			btTemp = (BYTE)((*pwData >> 8) & 0xff);
// 			strBuffer += _T(" ");
// 			strBuffer.SetAt((i + 1), (char)btTemp);
// 		}
// 		pwData++;
// 	}

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

// 	for (i = 0; i < nMax; i += 2) {
// 		btTemp = (BYTE)(*pwData & 0xff);
// 		strBuffer += _T(" ");
// 		strBuffer.SetAt(i, (char)btTemp);
// 		if ((i + 1) < nMax)
// 		{
// 			btTemp = (BYTE)((*pwData >> 8) & 0xff);
// 			strBuffer += _T(" ");
// 			strBuffer.SetAt((i + 1), (char)btTemp);
// 		}
// 		pwData++;
// 	}

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

	// RecipeName
	// Cell ID
	// Speed 나누기 100 할것(소수점 2자리)
	// Dross Top 목표값
	pwData = &btData[enSmsWordRead_DrossTopTarget];
	pSeqData->wDrossTopTarget = *pwData;

	// Dross Bottom 목표값
	pwData = &btData[enSmsWordRead_DrossBtmTarget];
	pSeqData->wDrossBottomTarget = *pwData;
	pwData = &btData[enSmsWordRead_FoilExpTopTarget];
	pSeqData->wFoilExpTopTarget = *pwData;
	pwData = &btData[enSmsWordRead_FoilExpBtmTarget];
	pSeqData->wFoilExpBottomTarget = *pwData;
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

int CSigProc::WriteBlockAllData_Siemens(int nMode)
{
	int nRet = 0;

	return nRet;
}

int CSigProc::ReadBlockWriteDataAll_Siemens(_SEQ_OUT_DATA_ALL* pSeqOutDataAll)
{
	_SEQ_OUT_DATA* pSeqOutData;
	pSeqOutData = &pSeqOutDataAll->m_SeqOutData;

	ASSERT(pSeqOutData);
	// RecipeInfo
	WORD btData[enSmsWordWriteMaxSize];
	WORD* pData = btData;
	int nSize = enSmsWordWriteMaxSize;
	int nAddress = AprData.m_System.m_nWordOut + enWordWrite_DataReportV1_Ea;

	if (ReadPLC_Block_device(nAddress, (short*)pData, nSize) != 0) {
		return -1;
	}

	WORD btTemp;
	btTemp = btData[enSmsWordWrite_DataReportV1_Ea];
	pSeqOutData->dwDataReportV1 = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV2_OK];
	pSeqOutData->dwDataReportV2 = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV3_NG];
	pSeqOutData->dwDataReportV3 = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV4_OkRate];
	pSeqOutData->dwDataReportV4 = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_DataReportV6_RunRate];
	pSeqOutData->dwDataReportV5 = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_Continue_Alarm_Cnt];
	pSeqOutData->dwDataReportV6 = (DWORD)btTemp;


	//////////////////////////////////////////////////////////////////////////
	/// 임시 Address 맵 재 정의 후 수정 필요
	btTemp = btData[enSmsWordWrite_DrossTop_Alarm_Cnt];
	pSeqOutData->dwFoilExpBothTopCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_DrossBtm_Alarm_Cnt];
	pSeqOutData->dwFoilExpBothBottomCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpTop_Alarm_Cnt];
	pSeqOutData->dwFoilExpInTopCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_FoilExpBtm_Alarm_Cnt];
	pSeqOutData->dwFoilExpOutTopCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_SpeterTop_Alarm_Cnt];
	pSeqOutData->dwSpeterTopCount = (DWORD)btTemp;
	//////////////////////////////////////////////////////////////////////////



	btTemp = btData[enSmsWordWrite_SpeterBtm_Alarm_Cnt];
	pSeqOutData->dwSpeterBottomCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_Top_Defect_Count_Real];
	pSeqOutData->dwTopNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_Btm_Defect_Count_Real];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_Top_Defect_Count_LotEnd];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_Btm_Defect_Count_LotEnd];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_AlarmExist];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	btTemp = btData[enSmsWordWrite_AlarmCode_Buffer1];
	pSeqOutData->dwBottomNgRealTimeCount = (DWORD)btTemp;

	return 0;
}
// 22.08.19 Ahn Add End


// 23.03.03 Ahn Add Start
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
			case	enWordRead_DrossTopTarget :
				nRetAdd = enSmsWordRead_DrossTopTarget;
				break;
			case	enWordRead_DrossBtmTarget :
				nRetAdd = enSmsWordRead_DrossBtmTarget;
				break;
			case	enWordRead_FoilExpTopTarget :
				nRetAdd = enSmsWordRead_FoilExpTopTarget;
				break;
			case	enWordRead_FoilExpBtmTarget :
				nRetAdd = enSmsWordRead_FoilExpBtmTarget;
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
			case	enWordRead_AlarmExistAck :
				nRetAdd = enSmsWordRead_AlarmExistAck;
				break;
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

			///////////////////////////////////////////////////////////////////////////
			// 임시, 지멘스 Address 재 정의 후 수정 필요
			case	enWordWrite_FoilExpInTop_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_DrossTop_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpInBottom_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_DrossBtm_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpOutTop_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_FoilExpTop_Alarm_Cnt;
				break;
			case	enWordWrite_FoilExpOutBottom_Alarm_Cnt:
				nRetAdd = enSmsWordWrite_FoilExpBtm_Alarm_Cnt;
				break;
			case	enWordWrite_SpeterTop_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_SpeterTop_Alarm_Cnt;
				break;
			case	enWordWrite_SpeterBtm_Alarm_Cnt :
				nRetAdd = enSmsWordWrite_SpeterBtm_Alarm_Cnt;
				break;
			///////////////////////////////////////////////////////////////////////////


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

				///////////////////////////////////////////////////////////////////////////
				// 임시, 지멘스 Address 재 정의 후 수정 필요
			case	enWordWrite_FoilExpInTopTarget :
				nRetAdd = enSmsWordWrite_DrossTopTarget;
				break;
			case	enWordWrite_FoilExpInBottomTarget:
				nRetAdd = enSmsWordWrite_DrossBtmTarget;
				break;
			case	enWordWrite_FoilExpOutTopTarget:
				nRetAdd = enSmsWordWrite_FoilExpTopTarget;
				break;
			case	enWordWrite_FoilExpOutBottomTarget :
				nRetAdd = enSmsWordWrite_FoilExpBtmTarget;
				break;
			case	enWordWrite_SpeterTopTarget :
				nRetAdd = enSmsWordWrite_SpeterTopTarget;
				break;
			case	enWordWrite_SpeterBtmTarget :
				nRetAdd = enSmsWordWrite_SpeterBtmTarget;
				break;
			///////////////////////////////////////////////////////////////////////////

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
// 23.03.03 Ahn Add End