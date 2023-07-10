#include "pch.h"
#include "PioCtrl.h"
#include "WinTool.h"
#include "GlobalData.h"		// 23.02.17 Son Add

CCriticalSection CPioCtrl::m_csPioThread;

CPioCtrl::CPioCtrl(WORD ChnNo, WORD DrvNo, WORD GrpNo)
{
	pAprPio = NULL;

	WORD wMaxPort = MAX_PORT;
	WORD wMyStNo = LIO_STATION_NO; //MAIN_STATION_NO; (MAIN_STATION_NO 사용 시 HEX 기준 Address 20개 밀림)
	WORD wExStNo = EX_STATION_NO;
	WORD wSeqStNo = 0x00;
	WORD wOffsetIn = 0x00;
	WORD wOffsetOut = 0x00;

	// 23.02.17 Son Add Start
	wOffsetIn = AprData.m_System.m_nBitIn;
	wOffsetOut = AprData.m_System.m_nBitOut;
	// 23.02.17 Son Add End

	switch (ChnNo) {
	case	CHN_NO_NETH1:

		wSeqStNo = 0xff;
		// 23.02.17 Son Del Start
		//	wOffsetIn = OFFSET_IN;
		//	wOffsetOut = OFFSET_OUT;
		// 23.02.17 Son Del End
		break;
	case	CHN_NO_NETG1:

		wSeqStNo = 0xff;
		// 23.02.17 Son Del Start
		//	wOffsetIn = OFFSET_IN;
		//	wOffsetOut = OFFSET_OUT;
		// 23.02.17 Son Del End
		break;
	case	CHN_NO_CCLINK2:

		wSeqStNo = 0x00;
		break;
	case	CHN_NO_CCLINK1:
	default:

		wSeqStNo = 0x00;
		//wMaxPort = 4;
		wMaxPort = 8;
		wMyStNo = LIO_STATION_NO;
		break;
	}

	// 22.03.17 Ahn Modify Start
	//pAprPio = new CMelsecDataLink(ChnNo, wMaxPort, wMyStNo, wExStNo, wSeqStNo, wOffsetIn, wOffsetOut);
#if defined( MELSEC_ETHERNET )
	pAprPio = (CMelsecBase*)new CMelsecEthernet(ChnNo, wMaxPort, wMyStNo, wExStNo, wSeqStNo, wOffsetIn, wOffsetOut);
#else
	// 23.02.28 Son Mod Start
	//pAprPio = (CMelsecBase*)new CMelsecDataLink(ChnNo, wMaxPort, wMyStNo, wExStNo, wSeqStNo, wOffsetIn, wOffsetOut);
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		//로그 항상출력
		LOGDISPLAY_SPECTXT(0)("Pio Ctrl : Melsec 생성");

		pAprPio = (CMelsecBase*)new CMelsecDataLink(ChnNo, wMaxPort, wMyStNo, wExStNo, wSeqStNo, wOffsetIn, wOffsetOut);
	}
	else {
		//로그 항상출력
		LOGDISPLAY_SPECTXT(0)("Pio Ctrl : Siemens 생성");

		CString strIPAddress = AprData.m_System.m_strPLCIPAddress;
		int nPort = AprData.m_System.m_nPLCPort;
		pAprPio = (CMelsecBase*)new CSiemensPlc(strIPAddress, 500, NULL, nPort, AprData.m_System.m_nBitIn, AprData.m_System.m_nBitOut, AprData.m_System.m_nWordIn, AprData.m_System.m_nWordOut);
		//pAprPio = (CMelsecBase*)new CSiemensPlc(strIPAddress, 500, NULL, nPort );

		PioTheadRun();
	}
	// 23.02.28 Son Mod End
#endif
	// 22.03.17 Ahn Modify End

	memset(&PioDataIF, 0x00, sizeof(PioDataIF));

	PioDataIF.CheckPortFlag[0]
		= PioDataIF.CheckPortFlag[1]
		= PioDataIF.CheckPortFlag[2]
		= PioDataIF.CheckPortFlag[3] = TRUE;

	m_pThread = NULL;

}


CPioCtrl::~CPioCtrl()
{
	if (PioThreadCheck() == TRUE) {
		PioTheadStop();
	}
	if (pAprPio != NULL) {
		delete pAprPio;
		pAprPio = NULL;
	}
}

// 22.03.23 Ahn Add Start
int CPioCtrl::ReadAllPort_BitIn( BYTE *data, short size)
{

// 	int nRet = 0;
// 	if (pAprPio != NULL) {
// 		nRet = pAprPio->ReadPortAllBitIn(data, size);
// 	}



	int	nRet = 0;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{		
		int nAddress = AprData.m_System.m_nBitIn;
		nRet = ReadPLC_Block_device(nAddress, (short*)data, size);

	}
	else
	{
		nRet = pAprPio->ReadPortAllBitIn(data, size);
	}




	return nRet;
}
int CPioCtrl::ReadAllPort_BitOut( BYTE *data, short size)
{

// 	int nRet = 0;
// 	if (pAprPio != NULL) {
// 		nRet = pAprPio->ReadPortAllBitOut(data, size);
// 	}


	int	nRet = 0;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		int nAddress = AprData.m_System.m_nBitOut;

		nRet = ReadPLC_Block_device(nAddress, (short*)data, size);
	}
	else
	{
		nRet = pAprPio->ReadPortAllBitOut(data, size);
	}

 	return nRet;
}
// 22.03.23 Ahn Add End

// 22.03.24 Ahn Add Start
WORD  CPioCtrl::GetAlarmCode() {
	return pAprPio->GetAlarmCode();
}
int CPioCtrl::SetAlarmCode(WORD wAlarmCode)
{
	pAprPio->SetAlarmCode(wAlarmCode);

	return 0;
}

int CPioCtrl::WriteAlarmCode(int address)
{
	int nRet = 0;
	nRet = pAprPio->WriteAlarmCode(address);
	return nRet;
}
// 22.03.24 Ahn Add End
void CPioCtrl::PioTheadRun()
{

	PioDataIF.EndFlag = FALSE;

	//이벤트 객체 생성
	pEvent_PioCtrl = CreateEvent(NULL, FALSE, FALSE, NULL);

	if ((m_pThread = AfxBeginThread((AFX_THREADPROC)ThreadProc_InPortCheck,
		(LPVOID)this,
		THREAD_PRIORITY_LOWEST,
		0,
		CREATE_SUSPENDED,
		NULL)))
	{

		m_pThread->m_bAutoDelete = FALSE;
		m_pThread->ResumeThread();

	}
}


void CPioCtrl::PioTheadStop()
{

	// source file
	if (m_pThread)
	{
		setEvent_PioCtrl();
		CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
		m_pThread->m_hThread = NULL;
	}


	return;

}


int CPioCtrl::InPortByteThread(int nAddress , OUT short& data )
{
	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == TRUE)
	{
		return -1;
	}

	CSingleLock	cs(&m_csPioThread, TRUE);
	data = PioDataIF.InputDataSms[nAddress];


	if (data < 0 || data > 1)
	{
		return -1;
	}

// 	CString strMsg;
// 	strMsg.Format(_T("data = %d, PioDataIF.InputDataSms[0] = %d"), data, PioDataIF.InputDataSms[nAddress]);
// 	AprData.SaveDebugLog(strMsg); //pyjtest


	return 0;

}


int CPioCtrl::OutPortByte(WORD port, BYTE data, BOOL bLockCtrl /*= TRUE*/)
{

	int	ret = 0;

	if (pAprPio != NULL) {
		if (pAprPio->OutPort(port, data, bLockCtrl) != 0) {
			//에러로그
			ret = -1;
		}
	}
	else {
		//에러로그
		ret = -1;
	}

	return (ret);

}


BYTE CPioCtrl::GetOutData(int port)
{
	return (pAprPio->btOutPortData[port]);
}


bool CPioCtrl::PioThreadCheck()
{

	bool	ret;

	if (m_pThread != NULL) {
		ret = TRUE;
	}
	else {
		ret = FALSE;
	}

	return (ret);

}


int CPioCtrl::InPortByte(WORD port, BYTE* data, BOOL bExtSt /*= FALSE*/)
{

	int	ret = 0;

	if (pAprPio != NULL) {
		BYTE	Indata = 0x00;
		BOOL	bReadOk = FALSE;
		int	iMaxIndex = 10;
		int iIndex;
		for (iIndex = 0; iIndex < iMaxIndex; iIndex++) {
			if (pAprPio->InPort(port, &Indata, bExtSt) != 0) {
				//에러로그
			}
			else {
				bReadOk = TRUE;
				break;
			}
		}
		if (bReadOk != TRUE) {
			//에러로그
			ret = -1;
		}
		*data = Indata;
	}
	else {
		//에러로그
		ret = -1;
	}

	return (ret);

}


bool CPioCtrl::PioPortProcess(int port, BYTE data, int stus)
{

	BYTE	readdata;
	bool	ret;

	InPortByte(port, &readdata);

	if ((readdata & data) == data) {
		if (stus == 1) {
			ret = TRUE;
		}
		else {
			ret = FALSE;
		}
	}
	else {
		if (stus == 1) {
			ret = FALSE;
		}
		else {
			ret = TRUE;
		}
	}

	return (ret);

}


//스래드 타임아웃 시간
#define PIOCTRL_THREADTIMEOUT 100
UINT ThreadProc_InPortCheck(LPVOID Param)
{
	int	i;
	PIOTHREAD_DATAIF* data;
	BYTE buff[MAX_PORT];
	short nBuffSms[MAX_SMS_IO_IN];
	CPioCtrl* ctrl;

	ctrl = (CPioCtrl*)Param;

	data = &(ctrl->PioDataIF);

	data->BusyFlag = TRUE;
	for (i = 0; i < MAX_PORT; i++) {
		data->InputData[i] = 0x00;
		buff[i] = 0x00;
	}

	for (i = 0; i < MAX_SMS_IO_IN; i++)
	{
		data->InputDataSms[i] = 0;
		nBuffSms[i] = 0;
	}

	UINT ret = 0;
	while (TRUE)
	{
		//타임 주기 이벤트
		ret = WaitForSingleObject(ctrl->getEvent_PioCtrl(), PIOCTRL_THREADTIMEOUT);
		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return 0;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			if (ctrl->pAprPio == NULL)
			{

				break;
			}

			if (data->EndFlag == TRUE)
			{
				//PLC Thread
				LOGDISPLAY_SPEC(0)(_T("PLC Run EndFlag TRUE(종료)"));
				break;
			}

			if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
			{
				//			DWORD dwStart = GetTickCount();


				if (ctrl->ReadPLC_Block_device(AprData.m_System.m_nBitIn, (short*)nBuffSms, MAX_SMS_IO_IN) != -1)
				{
					CSingleLock	cs(&CPioCtrl::m_csPioThread, TRUE);
					memcpy(data->InputDataSms, nBuffSms, sizeof(data->InputDataSms));

				}



				// 			DWORD dwEnd = GetTickCount() - dwStart;
				// 
				// 			CString strMsg;
				// 			strMsg.Format(_T("[ThreadProc_InPortCheck] ReadPLC_Block_device = %d ms, PLC Alive = %d"), dwEnd, data->InputDataSms[0]);
				// 			AprData.SaveDebugLog(strMsg); //pyjtest





			}
			else
			{
				for (i = 0; i < MAX_PORT; i++)
				{
					if (data->CheckPortFlag[i] != TRUE)
					{
						continue;
					}
					ctrl->pAprPio->InPort(i, &buff[i]);
				}

				{
					CSingleLock	cs(&CPioCtrl::m_csPioThread, TRUE);
					memcpy(data->InputData, buff, sizeof(data->InputData));
				}
			}


			::PumpMessages();
		}
		else
		{
			break;
		}

	}

	AfxEndThread(0);
	data->BusyFlag = FALSE;
	data->EndFlag = FALSE;

	return (0);

}


int CPioCtrl::WritePLC_device(int address, short data)
{

	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio->WriteDataReg(address, &data, 1) != 0) {
		//에러로그
		return (-1);
	}

	return (0);

}


int CPioCtrl::ReadPLC_device(int address, short* data)
{

	if (data == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio->ReadDataReg(address, data, 1) != 0) {
		//에러로그
		return (-1);
	}

	return (0);

}


int CPioCtrl::Out_Port_Bit(int nPort, BYTE bBitPos, int nMode)
{
	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}

	int	nRet = 0;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens) {
		//nRet = pAprPio->WriteDataReg(nPort, (short*)bBitPos, nMode);

		bBitPos = (nMode == 0) ? 0x01 : 0x00;
		nRet = pAprPio->WriteDataReg(nPort, (short*)bBitPos, 1);
	}
	else
	{
		nRet = pAprPio->Out_Port_Bit(nPort, bBitPos, nMode);
	}

//	int	nRet = pAprPio->Out_Port_Bit(nPort, bBitPos, nMode);
	if (nRet < 0) {
		//에러로그
		return (-1);
	}
	return (nRet);
}


int CPioCtrl::Lock_Ctrl(BOOL bLockMode)
{
	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}
	int	nRet = pAprPio->Lock_Ctrl(bLockMode);
	if (nRet < 0) {
		//에러로그
		return (-1);
	}
	return (nRet);
}

// 22.07.06 Ahn Add Start
int CPioCtrl::WritePLC_Block_device(int address, int* pData, int nNumOfData)
{
	ASSERT(pData != NULL);
	ASSERT(nNumOfData > 0);

	if (pData == NULL) {
		//에러로그
		return (-1);
	}
	if (nNumOfData <= 0) {
		//에러로그
		return (-1);
	}
	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}
	int	nRet = 0;

	if (pAprPio->WriteDataReg(address, pData, nNumOfData) != 0) {
		//에러로그
		return (-1);
	}

	return (nRet);
}
//int ReadPLC_Block_device(int address, int* pData, int nNumOfData);
// 22.07.06 Ahn Add End


int CPioCtrl::WritePLC_Block_device(int address, short* pData, int nNumOfData)
{
	ASSERT(pData != NULL);
	ASSERT(nNumOfData > 0);
	if (pData == NULL) {
		//에러로그
		return (-1);
	}
	if (nNumOfData <= 0) {
		//에러로그
		return (-1);
	}
	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}
	int	nRet = 0;

	if (pAprPio->WriteDataReg(address, pData, nNumOfData) != 0) {
		//에러로그
		return (-1);
	}

	return (nRet);
}


int CPioCtrl::ReadPLC_Block_device(int address, short* pData, int nNumOfData)
{
	ASSERT(pData != NULL);
	ASSERT(nNumOfData > 0);
	if (pData == NULL) {
		//에러로그
		return (-1);
	}
	if (nNumOfData <= 0) {
		//에러로그
		return (-1);
	}
	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}
	int	nRet = 0;

	if (pAprPio->ReadDataReg(address, pData, nNumOfData) != 0) {
		//에러로그
		return (-1);
	}

	return (nRet);
}


int CPioCtrl::WritePLC_device(int address, short data, int num, short device, short stno)
{
	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio->WriteDataReg(address, &data, 1, device, stno) != 0) {
		//에러로그
		return (-1);
	}

	return (0);

}


int CPioCtrl::ReadPLC_device(int address, short* data, int num, short device, short stno)
{
	if (data == NULL) {
		//에러로그
		return (-1);
	}

	if (num <= 0) {
		//에러로그
		return (-1);
	}

	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio->ReadDataReg(address, data, num, device, stno) != 0) {
		//에러로그
		return (-1);
	}

	return (0);

}


void CPioCtrl::AddReadAddrForIO2(int dev, int addr)
{
	pAprPio->AddReadAddrForIO2(dev, addr);
}


void CPioCtrl::AddWriteAddrForIO2(int dev, int addr)
{
	pAprPio->AddWriteAddrForIO2(dev, addr);
}

int CPioCtrl::InPortAnotherDeviceBit(short stno, CString device, int adrs)
{

	int	ret = 0;

	if (pAprPio != NULL) {
		BYTE	Indata = 0x00;
		BOOL	bReadOk = FALSE;
		int	iMaxIndex = 10;
		for (int iIndex = 0; iIndex < iMaxIndex; iIndex++) {
			ret = pAprPio->ReadBitData(stno, device, adrs);

			if (ret != 0) {
				//
			}
			else {
				bReadOk = TRUE;
				break;
			}
		}

		if (bReadOk != TRUE) {
			//에러로그
			ret = -1;
		}
	}
	else {
		//에러로그
		ret = -1;
	}

	return (ret);

}


int CPioCtrl::WritePLC_Block_device(int address, short* pData, int nNumOfData, short device, short stno)
{

	if (pData == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio->WriteDataReg(address, pData, nNumOfData, device, stno) != 0) {
		//에러로그
		return (-1);
	}

	return (0);

}


int CPioCtrl::ReadPLC_Block_device(int address, short* pData, int nNumOfData, short device, short stno)
{

	if (pData == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio == NULL) {
		//에러로그
		return (-1);
	}

	if (pAprPio->ReadDataReg(address, pData, nNumOfData, device, stno) != 0) {
		//에러로그
		return (-1);
	}

	return (0);

}


// 22.05.26 Ahn Add Start
void CPioCtrl::EnableWorkSet(BOOL bMode)
{
	if (pAprPio != NULL ) {
		pAprPio->EnableWorkSet(bMode);
	}
}
// 22.05.06 Ahn Add End


// 22.06.30 Ahn Add Start
BOOL CPioCtrl::IsOpened()
{
	if (pAprPio != NULL) {
		return pAprPio->IsOpened();
	}
	return FALSE;
}
// 22.06.30 Ahn Add End