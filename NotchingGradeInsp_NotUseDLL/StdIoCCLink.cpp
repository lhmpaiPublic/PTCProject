#include "pch.h"
#include "StdIoCCLink.h"
#include "mdfunc.h"


CStdIoCCLink::CStdIoCCLink(WORD MaxPort /*= 16*/, WORD SeqStNo /*= 0*/)
{
	if ((MaxPort > enPortMax) || (MaxPort < 4)) {
		m_MaxPort = 16;
	}
	if (MaxPort % 4 != 0) {
		m_MaxPort = 16;
	}
	if (SeqStNo > enSeqNoMax) {
		SeqStNo = 0;
	}

	m_MyPort = 1;			// 자국번호 설정
	m_MaxPort = MaxPort;	// 사용 포트 수
	m_SeqStNo = SeqStNo;	// 시퀀스의 국번
}

CStdIoCCLink::~CStdIoCCLink()
{
	if (m_bOpened == TRUE) {
		Close();
	}
}


int CStdIoCCLink::Open(BOOL debug /*=FALSE*/)
{
	if (m_bOpened != FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::Open() Is Opened");
		return(-1);
	}

	m_bDebugMode = debug;
	if (debug == TRUE) {
		m_bOpened = TRUE;
		return(0);
	}

	SHORT ret = mdOpen(81, -1, &m_pPath);	//회선 오픈
	if (ret != 0) {
		__super::ErrorProc(ret, "CC-Link OPEN Error");
		return(-1);
	}

	m_bOpened = TRUE;

	//초기화
	for (int i = 0; i < m_MaxPort; i++) {
		if (OutPort(i, 0x00) != 0) {
			break;
		}
	}

	return(0);
}


int CStdIoCCLink::Close()
{
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::Close() Not Opened");
	}

	if (m_bDebugMode == TRUE) {
		m_bOpened = FALSE;
		return(0);
	}

	int ret = 0;
	for (int i = 0; i < m_MaxPort; i++) {
		if (OutPort(i, 0x00) != 0) {
			ret = -1;
		}
	}

	SHORT retv = mdClose(m_pPath);
	if (ret != 0) {
		__super::ErrorProc(retv, "CC-Link Close Error");
		return(-1);
	}

	m_bOpened = FALSE;

	return(0);
}

#if 0 
int CStdIoCCLink::OutPort(WORD port, DWORD value)
{
	BYTE* p = (BYTE*)&value;
	int ret = 0;

	ret = this->OutPortb(port + 0, *(p + 0));
	ret = this->OutPortb(port + 1, *(p + 1));
	ret = this->OutPortb(port + 2, *(p + 2));
	ret = this->OutPortb(port + 3, *(p + 3));

	return(ret);
}
#endif


int CStdIoCCLink::OutPort(WORD port, BYTE value)
{
	int ret = 0;

	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::Outportb() Not Opened");
		return(-1);
	}

	//포트 번호 체크
	if (__super::IsPortEorror(port) == TRUE) {
		return(-1);
	}

	m_OutMap[port] = value;

	if (m_bDebugMode == TRUE) {
		return(0);
	}

	short nSeqNumber = 0xff;								 		//국번:자국
	short type = 0x02; 												//디바이스 타입:자국RY(DevY)
	short nDevNumber = (port * 8) + ((m_MyPort - 1) * 4 * 8);		//쓰기 선두
	short nDataSize = sizeof(BYTE) * 1;

	short pBuff[1];
	pBuff[0] = value;

	ret = WriteRegister(nSeqNumber, type, nDevNumber, nDataSize, pBuff);
	if (ret != 0) {
		__super::ErrorProc(ret, "CStdIoCCLink::Outportb() Not Opened");
		return(-1);
	}

	return(0);
}


int CStdIoCCLink::ReadOutport(WORD port, BYTE* value)
{
	int ret = 0;

	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::ReadOutport() Not Opened");
		return(-1);
	}

	//포트번호 체크
	if (__super::IsPortEorror(port) == TRUE) {
		return (-1);
	}

	*value = m_OutMap[port];

	if (m_bDebugMode == TRUE) {
		return(0);
	}

	short nSeqNumber = 0xff; 										//국번:자국
	short type = 0x02; 												//디바이스 타입:자국RY(DevY)
	short nDevNumber = (port * 8) + ((m_MyPort - 1) * 4 * 8);		//쓰기 선두
	short nDataSize = sizeof(BYTE) * 1;
	short pBuff[10];

	ret = this->ReadRegister(nSeqNumber, type, nDevNumber, nDataSize, pBuff);
	*value = (BYTE)(pBuff[0] & 0x00ff);

	return(ret);
}

#if 0
int CStdIoCCLink::Inport(WORD port, DWORD* value)
{
	BYTE* p = (BYTE*)value;
	int ret = 0;

	ret = this->Inportb(port + 0, p + 0);
	ret = this->Inportb(port + 1, p + 1);
	ret = this->Inportb(port + 2, p + 2);
	ret = this->Inportb(port + 3, p + 3);

	return(ret);
}
#endif

int CStdIoCCLink::Inport(WORD port, BYTE* value)
{
	int ret = 0;

	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::Inportb() Not Opened");
		ret = -1;
	}

	//포트번호 체크
	if (__super::IsPortEorror(port) == TRUE) {
		return (-1);
	}

	*value = m_InMap[port];

	if (m_bDebugMode == TRUE) {
		return(0);
	}

	short nSeqNumber = 0xff; 										//국번:자국
	short type = 0x01;												//디바이스 타입:자국RX(DevX)
	short nDevNumber = (port * 8) + ((m_MyPort - 1) * 4 * 8);		//쓰기 선두
	short nDataSize = sizeof(BYTE) * 1;
	short pBuff[10];

	ret = this->ReadRegister(nSeqNumber, type, nDevNumber, nDataSize, pBuff);
	*value = (BYTE)(pBuff[0] & 0x00ff);

	return(ret);
}


int CStdIoCCLink::WriteReg(WORD dev, WORD data)
{
	int ret = 0;

	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::WriteReg() Not Opened");
		return(-1);
	}

	if (__super::IsRegistEorror(dev) == TRUE) {
		__super::ErrorProc(-1, "CStdIoCCLink::WriteReg() IsRegistEorror");
		return(-1);
	}

	short type = 0x0d;			//디바이스D "DevD"
	short nDevNumber = dev;		//쓰기 선두
	short nDataSize = sizeof(short) * 1;
	short pData[1];

	pData[0] = data;

	ret = WriteRegister(m_SeqStNo, type, nDevNumber, nDataSize, pData);
	if (ret != 0) {
		__super::ErrorProc(ret, "CStdIoCCLink::WriteReg() WriteRegister");
		return(-1);
	}

	return(0);
}


int CStdIoCCLink::WriteReg_Block(WORD dev, WORD* data, int nNumOfData)
{
	int ret = 0;

	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::WriteReg() Not Opened");
		return(-1);
	}

	if (__super::IsRegistEorror(dev) == TRUE) {
		__super::ErrorProc(-1, "CStdIoCCLink::WriteReg() IsRegistEorror");
		return(-1);
	}

	short type = 0x0d;			//디바이스D "DevD"
	short nDevNumber = dev;		//쓰기 선두
	short nDataSize = (short)(2 * nNumOfData);
	short* pData;

	pData = (short*)data;

	ret = WriteRegister(m_SeqStNo, type, nDevNumber, nDataSize, pData);
	if (ret != 0) {
		__super::ErrorProc(ret, "CStdIoCCLink::WriteReg() WriteRegister");
		return(-1);
	}

	return(0);
}


int CStdIoCCLink::WriteRegister(short nSeqNumber, short type, short nDevNumber, short nDataSize, void* pBuff)
{
	if (m_bDebugMode == TRUE) {
		return(0);
	}

	//회선이 열려있지 않은 경우는 오픈
	if (m_pPath == NULL) {
		SHORT ret = mdOpen(81, -1, &m_pPath);
		if (ret != 0) {
			__super::ErrorProc(ret, "CC-Link OPEN Error");
			return(-1);
		}
	}

	short nWriteSize = nDataSize;													//쓰기 바이트 수
	SHORT ret = mdSend(m_pPath, nSeqNumber, type, nDevNumber, &nWriteSize, pBuff);	//쓰기

	if (ret != 0) {
		__super::ErrorProc(ret, "CC-Link WRITE Error");
		return(-1);
	}

	if (nWriteSize != nDataSize) {
		__super::ErrorProc(-1, "CC-Link WRITE size Error");
		return(-1);
	}

	return(0);
}


#if 0
int CStdIoCCLink::WriteDataReg(int offset, int data[], int num)
{
	int conv = sizeof(int) / sizeof(short);
	short* pBuff = new short[num * conv];

	for (int i = 0; i < num; i++) {
		pBuff[i * conv] = (data[i] & 0x0000ffff);
		pBuff[i * conv + 1] = (data[i] & 0xffff0000) >> 16;
	}

	int ret = this->WriteDataReg(offset, pBuff, num * conv);

	delete[] pBuff;
	return (ret);
}
#endif


#if 0
int CStdIoCCLink::ReadDataReg(int offset, int data[], int num)
{
	int conv = sizeof(int) / sizeof(short);
	short* pBuff = new short[num * conv];

	int ret = this->ReadDataReg(offset, pBuff, num * conv);

	for (int i = 0; i < num; i++) {
		data[i] = pBuff[i * conv] | (pBuff[i * conv + 1] << 16);
	}
	delete[] pBuff;

	return(ret);
}
#endif


int CStdIoCCLink::ReadReg(WORD dev, WORD* data)
{
	int ret = 0;
	*data = 0;

	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::ReadReg() Not Opened");
		return(-1);
	}

	if (__super::IsRegistEorror(dev) == TRUE && dev != 142 && dev != 143) {	 // PLC가 보유하고 있는 엔코더 카운터의 값을 취득하게 한다.
		__super::ErrorProc(-1, "CStdIoCCLink::ReadReg() IsRegistEorror");
		return(-1);
	}

	short type = 0x0d;		// デバイスD "DevD"
	short nDevNumber = dev;		//  書き込む先頭
	short nDataSize = sizeof(short) * 1;
	short pData[1] = { 0 };

	ret = ReadRegister(m_SeqStNo, type, nDevNumber, nDataSize, pData);
	if (ret != 0) {
		__super::ErrorProc(ret, "CC-Link ReadReg Error");
		return(-1);
	}

	*data = pData[0];

	return(0);
}


int CStdIoCCLink::ReadRegister(short nSeqNumber, short type, short nDevNumber, short nDataSize, void* pBuff)
{
	if (m_bDebugMode == TRUE) {
		return(0);
	}

	//회선이 열려있지 않는 경우 오픈
	if (m_pPath == NULL) {
		SHORT ret = mdOpen(81, -1, &m_pPath);
		if (ret != 0) {
			__super::ErrorProc(ret, "CC-Link OPEN Error");
			return(-1);
		}
	}

	short nReadSize = nDataSize;		//읽기 바이트 수
	SHORT ret = mdReceive(m_pPath, nSeqNumber, type, nDevNumber, &nReadSize, pBuff);

	if (ret != 0) {
		__super::ErrorProc(ret, "CC-Link READ Error");
		return(-1);
	}

	if (nReadSize != nDataSize) {
		__super::ErrorProc(-1, "CC-Link READ size Error");
		return(-1);
	}

	return(0);
}


int CStdIoCCLink::ReadBitData(int adrs)
{
	int	ret = ReadBitData(0xff, _T("X"), adrs);

	if (ret == -1) {
		__super::ErrorProc(ret, "CC-Link ReadBitData Error");
		return(-1);
	}

	return (ret);
}


int CStdIoCCLink::ReadBitData(short stno, CString device, int adrs)
{
	int ret = 0;
	short pBuff[10];

	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoCCLink::ReadBitData() Not Opened");
		return(-1);
	}

	short type;
	if (device.Compare(_T("M")) == 0) { type = 0x04; }
	else if (device.Compare(_T("B")) == 0) { type = 0x17; }
	else if (device.Compare(_T("X")) == 0) { type = 0x01; }
	else {
		return (0);
	}

	short nDevNumber = adrs;		//읽기 선두
	short nDataSize = sizeof(BYTE) * 1;

	ret = this->ReadRegister(stno, type, nDevNumber, nDataSize, pBuff);

	if (ret == 0) {
		ret = (pBuff[0] >> (adrs % 8)) & 0x01;		//지정 비트의 값
	}

	return(ret);
}


int CStdIoCCLink::ChangeWorkingSetSize(void)
{
	int	nRet = 0;

	DWORD	dwProcessId = 0;				//프로세스 식별자
	HANDLE	hProcess = 0;					//프로세스 핸들
	PSIZE_T	dwMinimumWorkingSetSize = 0;	//최소 워킹 세트 크기
	PSIZE_T	dwMaximumWorkingSetSize = 0;	//최대 워킹 세트 크기
	CString	strtmp = _T("");

	dwProcessId = GetCurrentProcessId();
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		return (-1);
	}

	if (GetProcessWorkingSetSize(hProcess, dwMinimumWorkingSetSize, dwMaximumWorkingSetSize) == 0)
	{
		CloseHandle(hProcess);
		return (-1);
	}

#define	MIN_WORKING_SETSIZE		2*1024*1024		// 2M
#define	MAX_WORKING_SETSIZE		5*1024*1024		// 5M

	* dwMinimumWorkingSetSize = (DWORD)MIN_WORKING_SETSIZE;
	*dwMaximumWorkingSetSize = (DWORD)MAX_WORKING_SETSIZE;

	if (SetProcessWorkingSetSize(hProcess, *dwMinimumWorkingSetSize, *dwMaximumWorkingSetSize) == 0) {
		CloseHandle(hProcess);
		return (-1);
	}
	CloseHandle(hProcess);

	return (nRet);
}


#if 0
int CStdIoCCLink::ClearDataReg(int size)
{
	short* pBuff = new short[size];
	memset(pBuff, 0x00, sizeof(short) * size);

	int ret = this->WriteDataReg(0, pBuff, size);

	delete[] pBuff;
	return(ret);
}
#endif


#if 0
void CStdIoCCLink::SetSeqStationNo(WORD stno)
{
	m_SeqStNo = stno;
}
#endif
