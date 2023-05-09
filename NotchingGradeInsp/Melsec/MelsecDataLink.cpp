#include "pch.h"
#include "MelsecDataLink.h"
#include "mdfunc.h"
#include "GlobalData.h"

CCriticalSection CMelsecDataLink::m_csMelsec;

CMelsecDataLink::CMelsecDataLink(WORD wChannelNo /*= CHN_NO_CCLINK1*/, WORD wMaxPort /*= 16*/, WORD wMyStNo /*= 1*/, WORD wExtStNo /*= 1*/, WORD wSeqStNo /*= 0*/, WORD wOffset_In /*= 0*/, WORD wOffset_Out /*= 0*/)
{

	if ((wMaxPort > 16) || (wMaxPort < 4)) {
		AfxMessageBox(_T("보드수 지정 이상"));
		wMaxPort = 16;
	}

	m_bEnableWorkSet = TRUE;
	// 22.06.30 Ahn Add Start
	m_bOpened = FALSE;
	// 22.06.30 Ahn Add End

	m_wChannelNo = wChannelNo;
	m_wMaxPort = wMaxPort;
	m_wMyStNo = wMyStNo;
	m_wExtStNo = wExtStNo;
	m_wSeqStNo = wSeqStNo;

	m_wOffset_In = wOffset_In;
	m_wOffset_Out = wOffset_Out;

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		btOutPortData[nIndex] = 0x00;
	}

	m_bDebugIO = FALSE;
	m_bDebugIO2 = FALSE;

	int	iRet = 0;
	m_pPath = NULL;
	if (!m_bDebugIO || !m_bDebugIO2) {
		iRet = LocalPioOpen();
	}

	if (iRet != 0) {
		//에러로그
		PioErrorProcess(iRet, _T("OPEN Error"));
	}
	else {
		OpenPio();		//초기화
	}


	m_listInPortForIO2.RemoveAll();
	m_listOutPortForIO2.RemoveAll();
	m_listReadAddrForIO2.RemoveAll();
	m_listWriteAddrForIO2.RemoveAll();

	return;

}


void CMelsecDataLink::AddReadAddrForIO2(int dev, int addr)
{
	DWORD dw;
	dw = addr;
	dw |= (dev << 8);
	m_listReadAddrForIO2.AddTail(dw);
}


void CMelsecDataLink::AddWriteAddrForIO2(int dev, int addr)
{
	DWORD dw;
	dw = addr;
	dw |= (dev << 8);
	m_listWriteAddrForIO2.AddTail(dw);
}


CMelsecDataLink::~CMelsecDataLink()
{

	ClosePio();

	int	iRet = 0;
	if (!m_bDebugIO || !m_bDebugIO2) {
		{
			CSingleLock cs(GetCriticalSection(), TRUE);
			iRet = mdClose(m_pPath);
		}
	}

	if (iRet != 0) {
		//에러로그
		PioErrorProcess(iRet, _T("Close Error"));
	}

	return;

}


int CMelsecDataLink::InPort(WORD port, BYTE* data, BOOL bExtSt /*=FALSE*/)
{

	ASSERT(data != NULL);

	if (port >= m_wMaxPort) {
		ASSERT(FALSE);
#ifdef	_DEBUG
		CString	str = _T("");
		str.Format("Access Port %d", (int)port);
		AfxGetMainWnd()->MessageBox(str, _T("Confirm"), (MB_OK | MB_ICONERROR));
#endif	
		//에러로그
		return (-1);
	}

	int	iRet = 0;
	*data = 0;

	short	sDevType;
	WORD	wTmpStNo;
	switch (m_wChannelNo) {
	case	CHN_NO_NETH1:
		if (bExtSt) {
			sDevType = DEVICE_B;
			wTmpStNo = m_wExtStNo;
		}
		else {
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
		}
		break;
	case	CHN_NO_NETG1:
		if (bExtSt) {
			sDevType = DEVICE_B;
			wTmpStNo = m_wExtStNo;
		}
		else {
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
		}
		break;
	case	CHN_NO_CCLINK1:
	case	CHN_NO_CCLINK2:
	default:
		if (bExtSt) {
			sDevType = DEVICE_Y;
			wTmpStNo = m_wExtStNo;
		}
		else {
			sDevType = DEVICE_X;
			wTmpStNo = m_wMyStNo;
		}
		break;
	}

	if (!m_bDebugIO || !m_bDebugIO2) {
		if (m_bDebugIO && !m_bDebugIO2) {
			BOOL bUse = FALSE;
			WORD tmp = 0x0000;
			POSITION pos = NULL;
			pos = m_listInPortForIO2.GetHeadPosition();
			while (pos != NULL) {
				tmp = m_listInPortForIO2.GetNext(pos);
				if (tmp == port) {
					bUse = TRUE;
					break;
				}
			}
			if (!bUse) {
				return (0);
			}
		}

		if (m_pPath == NULL) {
			iRet = LocalPioOpen();
			if (iRet != 0) {
				PioErrorProcess(iRet, _T("InPort OPEN Error111"));
				//에러로그
				return (-1);
			}
		}

		short	devno, size, size2, buff[10] = { 0, };
		devno = (short)(port * 8 + (wTmpStNo - 1) * 4 * 8 + m_wOffset_In);
		size2 = size = 1;

		// 22.04.04 Ahn Delete Start
		//if (ChangeWorkingSetSize() != 0) {
		//	//에러로그
		//}
		// 22.04.04 Ahn Delete End

		{
			CSingleLock cs(GetCriticalSection(), TRUE);
			iRet = mdReceive(m_pPath
				, 0xff
				, sDevType
				, devno
				, &size
				, buff
			);
		}

		if (iRet != 0) {
			PioErrorProcess(iRet, _T("InPort 111"));
			//에러로그
			return (-1);
		}
		else {
			if (size == size2) {
				*data = (BYTE)(buff[0] & 0x00ff);
			}
			else {
				AfxMessageBox(_T("size error"));
			}
		}
	}

	return (0);

}


int CMelsecDataLink::InPort_AnotherDevice(short stno, CString device, int boffset, BYTE* data, short num)
{

	ASSERT(data != NULL);

	int	iRet = 0;
	*data = 0;
	if (!m_bDebugIO) {
		if (m_pPath == NULL) {
			iRet = LocalPioOpen();
			if (iRet != 0) {
				PioErrorProcess(iRet, _T("InPort_AnotherDevice OPEN Error 222"));
				//에러로그
				return (-1);
			}
		}

		short	devno, size, size2, * buff, devtype;
		int		i;

		if (device.Compare(_T("M")) == 0) {
			devtype = DEVICE_M;
		}
		else if (device.Compare(_T("B")) == 0) {
			devtype = DEVICE_B;
		}
		else if (device.Compare(_T("X")) == 0) {
			devtype = DEVICE_X;
		}
		else {
			AfxMessageBox(_T("디바이스명이 잘못되었습니다."));
			return (0);
		}

		buff = new short[num];

		devno = (short)(boffset * 8);
		size2 = size = num;

		// 22.04.04 Ahn Delete Start
		//if (ChangeWorkingSetSize() != 0) {
		//	//에러로그
		//}
		// 22.04.04 Ahn Delete End
		{
			CSingleLock cs(GetCriticalSection(), TRUE);
			iRet = mdReceive(m_pPath
				, 0xff
				, devtype
				, devno
				, &size
				, buff
			);
		}

		if (iRet != 0) {
			PioErrorProcess(iRet, _T("InPort_AnotherDevice 222"));
			delete[]	buff;
			//에러로그
			return (-1);
		}
		else {
			if (size == size2) {
				for (i = 0; i < num; i++) {
					if (i & 0x01) {
						data[i] = (BYTE)((buff[i] & 0xff00) >> 8);
					}
					else {
						data[i] = (BYTE)(buff[i] & 0x00ff);
					}
				}
			}
			else {
				AfxMessageBox(_T("size error"));
			}
		}
		delete[] buff;
	}

	return (0);

}


int CMelsecDataLink::ReadBitData(int adrs)
{

	int	ret = ReadBitData(0xff, _T("X"), adrs);

	return (ret);

}


int CMelsecDataLink::ReadBitData(short stno, CString device, int adrs)
{

	int	byteoffset, bitoffset;
	BYTE	data;

	byteoffset = adrs / 8;
	bitoffset = adrs % 8;

	if (InPort_AnotherDevice(stno, device, byteoffset, &data, 1) != 0) {
		//에러로그
		return (-1);
	}

	data = (BYTE)((data >> bitoffset) & 0x01);

	return ((int)data);

}


int CMelsecDataLink::OutPort(WORD port, BYTE data, BOOL bLockCtrl /*= TRUE*/, BOOL bGetBit /*= FALSE*/)
{

	int	ret;

	if (port >= m_wMaxPort) {
#ifdef	_DEBUG
		CString str = _T("");
		str.Format("Access Port %d", (int)port);
		AfxGetMainWnd()->MessageBox(str, "確認", (MB_OK | MB_ICONERROR));
#endif	// end of _DEBUG
		//에러로그
		return (-1);
	}

	btOutPortData[port] = data;

	int	iRet = 0;
	if (!m_bDebugIO) {
		if (m_pPath == NULL) {
			iRet = LocalPioOpen(bLockCtrl);
			if (iRet != 0) {
				PioErrorProcess(iRet, _T("OutPort OPEN Error 333"));

				//로그출력
				LOGDISPLAY_SPECTXT(0)("PLC Melsec Open failed");

				return (-1);
			}
		}

		short	devno, size, size2, buff;
		short	sDevType;
		WORD	wTmpStNo;
		switch (m_wChannelNo) {
		case	CHN_NO_NETH1:
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
			break;
		case	CHN_NO_NETG1:
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
			break;
		case	CHN_NO_CCLINK1:
		case	CHN_NO_CCLINK2:
		default:
			if (bLockCtrl == TRUE) {
				sDevType = DEVICE_Y;
			}
			else {
				sDevType = DEVICE_X;
			}
			wTmpStNo = m_wMyStNo;
			break;
		}

		devno = (short)(port * 8 + (wTmpStNo - 1) * 4 * 8 + m_wOffset_Out);
		size2 = size = 1;
		buff = (short)(data);

		if (bGetBit == TRUE) {
			short sBuff[10] = { 0, };
			CSingleLock cs(GetCriticalSection(), TRUE);
			iRet = mdReceive(m_pPath
				, 0xff
				, sDevType
				, devno
				, &size
				, sBuff
			);
			if (iRet != 0) {
				PioErrorProcess(iRet, _T("OutPort 333"));
				//에러로그
				return (-1);
			}
			else {
				if (size == size2) {
					iRet = (BYTE)(sBuff[0] & 0x00ff);
				}
				else {
					AfxMessageBox(_T("size error"));
				}
				return (iRet);
			}
		}

		{
			CSingleLock cs(GetCriticalSection(), bLockCtrl);
			iRet = mdSend(m_pPath
				, 0xff
				, sDevType
				, devno
				, &size
				, &buff
			);
		}

		if (iRet != 0) {
			PioErrorProcess(iRet, _T("OutPort 333-1"));
			//에러로그
			return (-1);
		}
		else {
			if (size != size2) {
				AfxMessageBox(_T("size error"));
			}
		}


//		CString strMsg;
//		strMsg.Format(_T("CMelsecDataLink::OutPort buff=%d port=%d"), buff, port);
//		AprData.SaveDebugLog(strMsg); //pyjtest

	}
	ret = 0;
	if (iRet != 0) {
		ret = -1;
	}

	return (ret);

}


int CMelsecDataLink::WriteDataReg(int offset, short data[], int num)
{

	ASSERT(data != NULL);

	int	ret;
	short	sDevType;
	switch (m_wChannelNo) {
	case	CHN_NO_NETH1:
		sDevType = DEVICE_W;
		break;
	case	CHN_NO_NETG1:
		sDevType = DEVICE_W;
		break;
	case	CHN_NO_CCLINK1:
	case	CHN_NO_CCLINK2:
	default:
		sDevType = DEVICE_D;
		break;
	}

	if (m_bDebugIO) {
		return (0);
	}

	int	iRet = 0;
	if (m_pPath == NULL) {
		iRet = LocalPioOpen();
		if (iRet != 0) {
			PioErrorProcess(iRet, _T("WriteDataReg OPEN Error 444"));
			//에러로그
			return (-1);
		}
	}

	long	devno, size, size2;
	// 23.02.17 Son Mod Start
	//devno = (long)offset;
	devno = (long)(offset + AprData.m_System.m_nWordOut);


	// 23.02.17 Son Mod End
	size2 = size = (long)(2 * num);
	long nNetNo = 0;

	// 22.04.04 Ahn Delete End
	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		iRet = mdSendEx(m_pPath
			, nNetNo 
			, m_wSeqStNo
			, sDevType
			, devno
			, &size
			, data
		);
	}

	if (iRet != 0) {
		PioErrorProcess(iRet, _T("WriteDataReg 444"));
		//에러로그
		return (-1);
	}
	else {
		if (size != size2) {
			AfxMessageBox(_T("size error"));
		}
	}
	ret = 0;
	if (iRet != 0) {
		ret = -1;
	}

	return (ret);

}


int CMelsecDataLink::WriteDataReg(int offset, int data[], int num)
{

	ASSERT(data != NULL);

	short* buff;
	int		conv, i, ret = 0;

	if (m_bDebugIO) {
		return (0);
	}

	conv = sizeof(int) / sizeof(short);
	buff = new short[num * conv];

	for (i = 0; i < num; i++) {
		buff[i * conv] = (short)((data[i] & 0x0000ffff));
		buff[i * conv + 1] = (short)((data[i] & 0xffff0000) >> 16);
	}

	if (num <= 0) {
		//에러로그
		ret = -1;
	}

	if (ret == 0) {
		if (WriteDataReg(offset, buff, num * conv) != 0) {
			//에러로그
			ret = -1;
		}
	}
	delete[] buff;

	return (ret);

}


int CMelsecDataLink::ClearDataReg(int size)
{

	short* buff;
	int		ret = 0;

	buff = new short[size];
	memset(buff, 0x00, sizeof(short) * size);

	if (WriteDataReg(0, buff, size) != 0) {
		//에러로그
		ret = -1;
	}

	delete[] buff;
	return (ret);

}


int CMelsecDataLink::ReadDataReg(int offset, short data[], int num)
{
	ASSERT(data != NULL);

	int	ret;
	short	sDevType;
	switch (m_wChannelNo) {
	case	CHN_NO_NETH1:
		sDevType = DEVICE_W;
		break;
	case	CHN_NO_NETG1:
		sDevType = DEVICE_W;
		break;
	case	CHN_NO_CCLINK1:
	case	CHN_NO_CCLINK2:
	default:
		sDevType = DEVICE_D;
		break;
	}

	if (m_bDebugIO && m_bDebugIO2) {
		memset(data, 0x00, (sizeof(short) * num));
		return (0);
	}
	if (m_bDebugIO && !m_bDebugIO2) {
		DWORD target = (DWORD)(offset | (0x0d << 8));
		BOOL bUse = FALSE;
		DWORD tmp = 0x00000000;
		POSITION pos = NULL;
		pos = m_listReadAddrForIO2.GetHeadPosition();
		while (pos != NULL) {
			tmp = m_listReadAddrForIO2.GetNext(pos);
			if (tmp == target) {
				bUse = TRUE;
				break;
			}
		}
		if (!bUse) {
			memset(data, 0x00, (sizeof(short) * num));
			return (0);
		}
	}

	int	iRet = 0;
	if (m_pPath == NULL) {
		iRet = LocalPioOpen();
		if (iRet != 0) {
			PioErrorProcess(iRet, _T("ReadDataReg OPEN Error 555"));
			//에러로그
			return (-1);
		}
	}

	long	devno, size, size2;
	long	netNo = 0;
	long	seqStNo = (long)m_wSeqStNo;
	// 23.02.17 Son Mod Start
	//devno = (long)offset;
//	devno = (long)(offset + AprData.m_System.m_nWordIn);
	devno = (long)(offset);
	// 23.02.17 Son Mod End
	size2 = size = (long)(2 * num);
	if (num <= 0) {
		//에러로그
		return (-1);
	}

	// 22.04.04 Ahn Delete Start
	//if (ChangeWorkingSetSize() != 0) {
	//	//에러로그
	//}
	// 22.04.04 Ahn Delete End

	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		iRet = mdReceiveEx(m_pPath
			, netNo
			, seqStNo
			, sDevType
			, devno
			, &size
			, data
		);
	}

	if (iRet != 0) {
		PioErrorProcess(iRet, _T("ReadDataReg 555"));
		//에러로그
		return (-1);
	}
	else {
		if (size != size2) {
			AfxMessageBox(_T("size error"));
		}
	}
	ret = 0;
	if (iRet != 0) {
		ret = -1;
	}

	return (ret);

}


int CMelsecDataLink::ReadDataReg(int offset, int data[], int num)
{

	ASSERT(data != NULL);

	short* buff;
	int		conv, i, ret = 0;

	if (m_bDebugIO && m_bDebugIO2) {
		memset(data, 0x00, (sizeof(int) * num));
		return (0);
	}

	conv = sizeof(int) / sizeof(short);
	buff = new short[num * conv];

	if (ReadDataReg(offset, buff, num * conv) != 0) {
		//에러로그
		ret = -1;
	}

	for (i = 0; i < num; i++) {
		data[i] = buff[i * conv] | (buff[i * conv + 1] << 16);
	}
	delete[] buff;

	return (ret);

}


void CMelsecDataLink::PioErrorProcess(int iRet, char* cap)
{

	static BOOL	nowExecute = FALSE;
	CString	cstr = _T("");

	if (nowExecute != FALSE) {
		return;
	}
	nowExecute = TRUE;

	switch (iRet) {

	case	0x0000:
		break;

	default:
		cstr.Format("MelsecDataLinkLibrary Error. Code: %d (0x%x)-(%s)", iRet, (iRet & 0xffff), cap);
		//에러로그
		m_bOpened = FALSE; // 22.06.30 Ahn Add 
		AprData.SaveErrorLog(cstr);

		// 22.10.13 Ahn Add Start
		AprData.m_ErrStatus.SetError(CErrorStatus::en_MelsecError, cstr);
		// 22.10.13 Ahn Add End
		break;
	}

	//Error Log
	LOGDISPLAY_SPEC(0)(_T("MelsecDataLinkLibrary Error-<%s>"), cap);

	nowExecute = FALSE;

	return;

}


int CMelsecDataLink::OpenPio(void)
{
	int	ret = 0;

	if (m_bDebugIO) {
			//
	}
	else {
		BYTE data;
		CString	strLocal;
		for (int i = 0; i < m_wMaxPort; i++) {
			data = 0x00;
			if (OutPort((WORD)i, data) != 0) {
				//에러로그
				ret = -1;
			}
		}
	}

	return (ret);

}


int CMelsecDataLink::ClosePio(void)
{

	int	ret = 0;

	if (m_bDebugIO) {
			//
	}
	else {
		CString	strLocal;
		BYTE data;
		for (int i = 0; i < m_wMaxPort; i++) {
			data = 0x00;
			if (OutPort((WORD)i, data) != 0) {
				//에러로그
			}
		}
	}

	return (ret);

}


int CMelsecDataLink::LocalPioOpen(BOOL bLockCtrl /*= TRUE*/)
{

	int	iRet = 0;

	{
		CSingleLock cs(GetCriticalSection(), bLockCtrl);
		iRet = (int)mdOpen(m_wChannelNo, -1, &m_pPath);
	}

	// 22.06.30 Ahn Add Start
	if (iRet == 0) {
		m_bOpened = TRUE;

	}
	// 22.06.30 Ahn Add End

	// 22.04.04 Ahn Add Start
	// WorkingSizeの変更
	if (ChangeWorkingSetSize() != 0) {
		//에러로그
		m_bOpened = FALSE ; // 22.06.30 Ahn Add


		//로그출력
		LOGDISPLAY_SPECTXT(0)("PLC Melsec ChangeWorkingSetSize failed");
	}
	// 22.04.04 Ahn Add Start

	return (iRet);

}


int CMelsecDataLink::Out_Port_Bit(int nPort, BYTE bBitPos, int nMode)
{

	ASSERT(nPort >= 0);
	ASSERT(nPort < m_wMaxPort);
	if ((nPort < 0) || (nPort >= m_wMaxPort)) {
		//에러로그
		return (-1);
	}
	int	nRet = 0;
	{
		CSingleLock cs(GetCriticalSection(), TRUE);
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
		nRet = OutPort(nPort, data, TRUE, bGetBit);
		if (nRet < 0) {
			nRet = -1;
			//에러로그
		}


//		CString strMsg;
//		strMsg.Format(_T("CMelsecDataLink::Out_Port_Bit data=%d nPort=%d"), data, nPort);
//		AprData.SaveDebugLog(strMsg); //pyjtest
	}
	return (nRet);

}


int CMelsecDataLink::Lock_Ctrl(BOOL bLockMode)
{
	if (bLockMode == TRUE) {
		GetCriticalSection()->Lock();
	}
	else {
		GetCriticalSection()->Unlock();
	}
	return (0);
}


int CMelsecDataLink::ChangeWorkingSetSize(void)
{
	int	nRet = 0;

	// 22.05.26 Ahn Add Start
	if (m_bEnableWorkSet == FALSE) {
		// Grabber 동장중 실행시 IRQL 발생 가능성 높음
		return 0;
	}
	// 22.05.26 Ahn Add End

	DWORD	dwProcessId = 0;
	HANDLE	hProcess = 0;
#if defined( _WIN64 )
	SIZE_T	dwMinimumWorkingSetSize = 0;
	SIZE_T	dwMaximumWorkingSetSize = 0;
#else
	DWORD	dwMinimumWorkingSetSize = 0;
	DWORD	dwMaximumWorkingSetSize = 0;
#endif
	dwProcessId = GetCurrentProcessId();
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess == NULL) {
		//에러로그
		return (-1);
	}

	if (GetProcessWorkingSetSize(hProcess, &dwMinimumWorkingSetSize, &dwMaximumWorkingSetSize) == 0) {
		CloseHandle(hProcess);
		//에러로그
		return (-1);
	}

#define	MIN_WORKING_SETSIZE		1 * 1024 * 1024	// 1MB
#define	MAX_WORKING_SETSIZE		3 * 1024 * 1024	// 3MB

	dwMinimumWorkingSetSize = (DWORD)MIN_WORKING_SETSIZE;
	dwMaximumWorkingSetSize = (DWORD)MAX_WORKING_SETSIZE;

	if (SetProcessWorkingSetSize(hProcess, dwMinimumWorkingSetSize, dwMaximumWorkingSetSize) == 0) {
		CloseHandle(hProcess);
		//에러로그
	}
	CloseHandle(hProcess);

	return (nRet);
}


int CMelsecDataLink::ReadDataReg(int offset, short data[], int num, short device, short stno /*= 0xff*/)
{

	ASSERT(data != NULL);

	int	ret;

	if (m_bDebugIO && m_bDebugIO2) {
		memset(data, 0x00, (sizeof(short) * num));
		return (0);
	}
	if (m_bDebugIO && !m_bDebugIO2) {
		BOOL bUse = FALSE;
		DWORD target = (DWORD)(offset | (device << 8));
		DWORD tmp = 0x00000000;
		POSITION pos = NULL;
		pos = m_listReadAddrForIO2.GetHeadPosition();
		while (pos != NULL) {
			tmp = m_listReadAddrForIO2.GetNext(pos);
			if (tmp == target) {
				bUse = TRUE;
				break;
			}
		}
		if (!bUse) {
			memset(data, 0x00, (sizeof(short) * num));
			return (0);
		}
	}

	int	iRet = 0;
	if (m_pPath == NULL) {
		iRet = LocalPioOpen();
		if (iRet != 0) {
			PioErrorProcess(iRet, _T("ReadDataReg OPEN Error 666"));
			//에러로그
			return (-1);
		}
	}

	short	devno, size, size2;
	devno = (short)offset;
	size2 = size = (short)(2 * num);
	if (num <= 0) {
		//에러로그
		return (-1);
	}

	// 22.04.04 Ahn Delete Start
	//if (ChangeWorkingSetSize() != 0) {
	//	//에러로그
	//}
	// 22.04.04 Ahn Delete End

	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		iRet = mdReceive(m_pPath
			, stno
			, device
			, devno
			, &size
			, data
		);
	}

	if (iRet != 0) {
		PioErrorProcess(iRet, _T("ReadDataReg 666"));
		//에러로그
		return (-1);
	}
	else {
		if (size != size2) {
			AfxMessageBox(_T("size error"));
		}
	}
	ret = 0;
	if (iRet != 0) {
		ret = -1;
	}

	return (ret);

}


int CMelsecDataLink::WriteDataReg(int offset, short data[], int num, short device, short stno /*= 0xff*/)
{

	ASSERT(data != NULL);

	int	ret;

	if (m_bDebugIO && m_bDebugIO2) {
		return (0);
	}
	if (m_bDebugIO && !m_bDebugIO2) {
		BOOL bUse = FALSE;
		DWORD target = (DWORD)(offset | (device << 8));
		DWORD tmp = 0x00000000;
		POSITION pos = NULL;
		pos = m_listWriteAddrForIO2.GetHeadPosition();
		while (pos != NULL) {
			tmp = m_listWriteAddrForIO2.GetNext(pos);
			if (tmp == target) {
				bUse = TRUE;
				break;
			}
		}
		if (!bUse) {
			memset(data, 0x00, (sizeof(short) * num));
			return (0);
		}
	}

	int	iRet = 0;
	if (m_pPath == NULL) {
		iRet = LocalPioOpen();
		if (iRet != 0) {
			PioErrorProcess(iRet, _T("WriteDataReg OPEN Error 777"));
			//에러로그
			return (-1);
		}
	}

	short	devno, size, size2;

	devno = (short)offset;
	size2 = size = (short)(2 * num);

	// 22.04.04 Ahn Delete Start
	//if (ChangeWorkingSetSize() != 0) {
	//	//에러로그
	//}
	// 22.04.04 Ahn Delete End

	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		iRet = mdSend(m_pPath
			, stno
			, device
			, devno
			, &size
			, data
		);
	}

	if (iRet != 0) {
		PioErrorProcess(iRet, _T("WriteDataReg 777"));
		//에러로그
		return (-1);
	}
	else {
		if (size != size2) {
			AfxMessageBox(_T("size error"));
		}
	}
	ret = 0;
	if (iRet != 0) {
		ret = -1;
	}

	return (ret);

}

// 22.05.26 Ahn Add Start
void CMelsecDataLink::EnableWorkSet(BOOL bMode)
{
	m_bEnableWorkSet = bMode ;
}
// 22.05.26 Ahn Add End


// 22.06.30 Ahn Add Start
BOOL CMelsecDataLink::IsOpened()
{
	return m_bOpened;
}
// 22.06.30 Ahn Add End


// 22.07.29 Ahn Add Start
int CMelsecDataLink::ReadPortAllBitIn( BYTE *data, short size ) 
{
	ASSERT(data != NULL);
	BOOL bExtSt = FALSE;
	int port = 0x0000;


	if (size > m_wMaxPort) {
		ASSERT(FALSE);
#ifdef	_DEBUG
		CString	str = _T("");
		str.Format("Access Port %d", (int)size);
		AfxGetMainWnd()->MessageBox(str, _T("Confirm"), (MB_OK | MB_ICONERROR));
#endif	
		//에러로그
		return (-1);
	}

	int	iRet = 0;
	*data = 0;

	short	sDevType;
	WORD	wTmpStNo;
	switch (m_wChannelNo) {
	case	CHN_NO_NETH1:
		if (bExtSt) {
			sDevType = DEVICE_B;
			wTmpStNo = m_wExtStNo;
		}
		else 
		{
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
		}
		break;
	case	CHN_NO_NETG1:
		if (bExtSt) {
			sDevType = DEVICE_B;
			wTmpStNo = m_wExtStNo;
		}
		else {
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
		}
		break;
	case	CHN_NO_CCLINK1:
	case	CHN_NO_CCLINK2:
	default:
		if (bExtSt) {
			sDevType = DEVICE_Y;
			wTmpStNo = m_wExtStNo;
		}
		else {
			sDevType = DEVICE_X;
			wTmpStNo = m_wMyStNo;
		}
		break;
	}

	if (!m_bDebugIO || !m_bDebugIO2) {
		if (m_bDebugIO && !m_bDebugIO2) {
			BOOL bUse = FALSE;
			WORD tmp = 0x0000;
			POSITION pos = NULL;
			pos = m_listInPortForIO2.GetHeadPosition();
			while (pos != NULL) {
				tmp = m_listInPortForIO2.GetNext(pos);
				if (tmp == port) {
					bUse = TRUE;
					break;
				}
			}
			if (!bUse) {
				return (0);
			}
		}

		if (m_pPath == NULL) {
			iRet = LocalPioOpen();
			if (iRet != 0) {
				PioErrorProcess(iRet, _T("ReadPortAllBitIn OPEN Error 888"));
				//에러로그
				return (-1);
			}
		}

		short	devno, size2 ;
		devno = (short)(port * 8 + (wTmpStNo - 1) * 4 * 8 + m_wOffset_In);
		size2 = size ;


		{
			CSingleLock cs(GetCriticalSection(), TRUE);
			iRet = mdReceive(m_pPath
				, 0xff
				, sDevType
				, devno
				, &size
				, data
			);
		}

		if (iRet != 0) {
			PioErrorProcess(iRet, _T("ReadPortAllBitIn 888"));
			//에러로그
			return (-1);
		}
	}

	return 0;
};
int CMelsecDataLink::ReadPortAllBitOut( BYTE *data, short nSize )
{ 
	 // 22.08.11 Ahn Modify Start
	ASSERT(data);
	//if (nSize < m_wMaxPort) {
	//	memcpy(data, btOutPortData, sizeof(BYTE) * nSize);
	//}
	//else {
	//	memcpy(data, btOutPortData, sizeof(BYTE) * m_wMaxPort);
	//}
	// 
	BOOL bExtSt = FALSE;
	int port = 0x0000;


	if ( nSize > m_wMaxPort) {
		ASSERT(FALSE);
#ifdef	_DEBUG
		CString	str = _T("");
		str.Format("Access Port %d", (int)nSize);
		AfxGetMainWnd()->MessageBox(str, _T("Confirm"), (MB_OK | MB_ICONERROR));
#endif	
		//에러로그
		return (-1);
	}

	int	iRet = 0;
	*data = 0;

	short	sDevType;
	WORD	wTmpStNo;
	switch (m_wChannelNo) {
	case	CHN_NO_NETH1:
		if (bExtSt) {
			sDevType = DEVICE_B;
			wTmpStNo = m_wExtStNo;
		}
		else
		{
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
		}
		break;
	case	CHN_NO_NETG1:
		if (bExtSt) {
			sDevType = DEVICE_B;
			wTmpStNo = m_wExtStNo;
		}
		else {
			sDevType = DEVICE_B;
			wTmpStNo = m_wMyStNo;
		}
		break;
	case	CHN_NO_CCLINK1:
	case	CHN_NO_CCLINK2:
	default:
		if (bExtSt) {
			sDevType = DEVICE_Y;
			wTmpStNo = m_wExtStNo;
		}
		else {
			sDevType = DEVICE_X;
			wTmpStNo = m_wMyStNo;
		}
		break;
	}

	if (!m_bDebugIO || !m_bDebugIO2) {
		if (m_bDebugIO && !m_bDebugIO2) {
			BOOL bUse = FALSE;
			WORD tmp = 0x0000;
			POSITION pos = NULL;
			pos = m_listInPortForIO2.GetHeadPosition();
			while (pos != NULL) {
				tmp = m_listInPortForIO2.GetNext(pos);
				if (tmp == port) {
					bUse = TRUE;
					break;
				}
			}
			if (!bUse) {
				return (0);
			}
		}

		if (m_pPath == NULL) {
			iRet = LocalPioOpen();
			if (iRet != 0) {
				PioErrorProcess(iRet, _T("ReadPortAllBitOut 999"));
				//에러로그
				return (-1);
			}
		}

		short	devno, size2;
		devno = (short)(port * 8 + (wTmpStNo - 1) * 4 * 8 + m_wOffset_Out);
		size2 = nSize;

		{
			CSingleLock cs(GetCriticalSection(), TRUE);
			iRet = mdReceive(m_pPath
				, 0xff
				, sDevType
				, devno
				, &nSize
				, data
			);
		}

		if (iRet != 0) {
			PioErrorProcess(iRet, _T("ReadPortAllBitOut 999"));
			//에러로그
			return (-1);
		}

	}
	// 22.08.11 Ahn Modify End

	return (0);
};
// 22.07.29 Ahn Add End
