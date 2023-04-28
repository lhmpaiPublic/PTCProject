#include "pch.h"
#include "CMelsecEthenet.h"
#include "CMelsecEthernetSock.h"
#include "TimeAnalyzer.h"
#include "WinTool.h"
#include "Ping.h"
#include "GlobalData.h"

CCriticalSection CMelsecEthernet::m_csMelsec;


CMelsecEthernet::CMelsecEthernet(WORD wChannelNo /*= CHN_NO_NETG1*/, WORD wMaxPort /*= 16*/, WORD wMyStNo /*= 1*/, WORD wExtStNo /*= 1*/, WORD wSeqStNo /*= 0*/, WORD wOffset_In /*= 0*/, WORD wOffset_Out /*= 0*/)
{

	m_strIpAddress = _T("192.168.10.10");
	m_nProtocol = CMelsecEthernetSock::TCP_MODE;
	m_pEthernetSock = NULL;

	//char buff[256];
	//CString strSection;
	//CString strKey;
	//CString strFileName;
	////strFIleName 
	//strFileName.Format(_T("%s\\MelsecSetting.ini"), AprData.m_strDataPath);
	//
	//::GetPrivateProfileString(strSection, strKey, "192.168.10.111", buff, 256,  strFileName);
	//m_strIpAddress.Format(_T("%s"), buff);
	
	m_wChannelNo = wChannelNo;
	m_wMaxPort = wMaxPort;
	m_wMyStNo = wMyStNo;
	m_wExtStNo = wExtStNo;
	m_wSeqStNo = wSeqStNo;

	m_wOffset_In = wOffset_In;
	m_wOffset_Out = wOffset_Out;

	OpenPio();
}

CMelsecEthernet::~CMelsecEthernet()
{
	ClosePio();
}

int  CMelsecEthernet::ReadPortAllBitIn()
{
	int nRet = 0;

	//WORD wData ;
	int nHeadDevN0 = (short)((m_wMyStNo - 1) * 4 * 8 + m_wOffset_In);
	int nSize = m_wMaxPort ;
	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		m_pEthernetSock->InPort(nHeadDevN0, (WORD*)btInPortData, nSize);
	}
	return nRet ;
}
int CMelsecEthernet::ReadPortAllBitOut()
{
	int nRet = 0; 

	if (m_pEthernetSock == NULL) return -1;
	int devno = (short)((m_wMyStNo - 1) * 4 * 8 + m_wOffset_Out);

	int nSize = m_wMaxPort ;
	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		m_pEthernetSock->OutPort(devno, (WORD*)btOutPortData, nSize, TRUE);
	}

	return nRet;
}

int CMelsecEthernet::Out_Port_Bit(int nPort, BYTE bBitPos, int nMode)
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

		// 22.03.24 Ahn Delete Start
		//nRet = OutPort(nPort, data, TRUE, bGetBit);
		//if (nRet < 0) {
		//	nRet = -1;
		//	//에러로그
		//}
		btOutPortData[nPort] = data;
		// 22.03.24 Ahn Delete Start
	}
	return (nRet);

}
// 22.03.24 Ahn Modify Start
// 이미 읽어둔 포트 값을 반환함.
int CMelsecEthernet::InPort(WORD nPort, BYTE* data, BOOL bExtSt /*= FALSE*/)
{
	ASSERT(nPort >= 0);
	ASSERT(nPort < m_wMaxPort);
	ASSERT(data);
	if ((nPort < 0) || (nPort >= m_wMaxPort)) {
		//에러로그
		return (-1);
	}
	int	nRet = 0;

	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		*data = btInPortData[nPort];
	}
	return (nRet);
}
// 1바이트씩 읽어옴
//int CMelsecEthernet::InPort(WORD port, BYTE* data, BOOL bExtSt /*= FALSE*/)
//{
//	ASSERT(data);
//
//	if (m_pEthernetSock == NULL) return -1;
//	int nRet = 0;
//	int devno = (short)(port * 8 + (m_wMyStNo - 1) * 4 * 8 + m_wOffset_In) ;
//
//	int nHeadDevN0 = (short) ((m_wMyStNo - 1) * 4 * 8 + m_wOffset_In) ;
//
//	if (port >= 4) return - 2;
//
//	WORD wData;
//
//	CSingleLock cs(GetCriticalSection(), TRUE);
//	m_pEthernetSock->InPort(nHeadDevN0, &wData, bExtSt);
//
//	*data = (BYTE)(wData >> (port*8)) & 0xff;
//	return nRet;
//}
// 22.03.24 Ahn Modify Start

int CMelsecEthernet::OutPort(WORD port, BYTE data, BOOL bLockCtrl /*= TRUE */, BOOL bGetBit /*= FALSE */ )
{
	int nRet = 0;

	if (m_pEthernetSock == NULL) return -1;
	int devno = (short)(port * 8 + (m_wMyStNo - 1) * 4 * 8 + m_wOffset_Out);

	CSingleLock cs(GetCriticalSection(), TRUE);
	m_pEthernetSock->OutPort(devno, data, bLockCtrl);

	btOutPortData[port] = data;

	return nRet;
}

// 22.03.24 Ahn Add Start
int CMelsecEthernet::WriteAlarmCode(int offset)
{
	int nRet = 0;
	int num = 1;

	if (m_pEthernetSock == NULL) return -1;

	int		conv, ret = 0;

	conv = sizeof(int) / sizeof(short);

	CSingleLock cs(GetCriticalSection(), TRUE);
	if (m_pEthernetSock->WriteDataReg(offset, (short*)wAlarmCode, num * conv) != 0) {
		//에러로그
		ret = -1;
	}

	return nRet;
}
// 22.03.24 Ahn Add End

int CMelsecEthernet::WriteDataReg(int offset, short data[], int num)
{
	int nRet = 0;


	ASSERT(data != NULL);
	if (m_pEthernetSock == NULL) return -1;

	int		conv, ret = 0;

	conv = sizeof(int) / sizeof(short);

	CSingleLock cs(GetCriticalSection(), TRUE);
	if (m_pEthernetSock->WriteDataReg(offset, data, num * conv) != 0) {
		//에러로그
		ret = -1;
	}

	return nRet;
}

int CMelsecEthernet::ReadDataReg(int offset, short data[], int num)
{
	int nRet = 0;

	ASSERT(data != NULL);
	if (m_pEthernetSock == NULL) return -1;

	//short* buff;
	int		conv, ret = 0;

	conv = sizeof(int) / sizeof(short);
	//buff = new short[num * conv];

	CSingleLock cs(GetCriticalSection(), TRUE);
	if (m_pEthernetSock->ReadDataReg(offset, data, num ) != 0) {
		//에러로그
		ret = -1;
	}

	//for (i = 0; i < num; i++) {
	//	data[i] = buff[i * conv] | (buff[i * conv + 1] << 16);
	//}
	//delete[] buff;

	return nRet;
}

int CMelsecEthernet::OpenPio(void)
{
	int bRet = 0;

	if (m_pEthernetSock == NULL) {
		m_pEthernetSock = new CMelsecEthernetSock(m_strIpAddress, m_nProtocol);
	}

	m_pEthernetSock->Close();

	if (m_nProtocol == CMelsecEthernetSock::TCP_MODE) {
		if (m_pEthernetSock->Create() == 0) {
			CString strError;
			DWORD dwErrCode = ::GetLastError();
			strError = ::FormatErrorMsg(dwErrCode);

			//로그출력
			LOGDISPLAY_SPEC(0)(_T("Melsec Open 에러-1 - 에러<%s>"), strError);

			return (-1);
		}
		bRet = m_pEthernetSock->Connect( m_strIpAddress, MELSEC_TCP_PORT);
	}
	else {
		if (m_pEthernetSock->Create(MELSEC_UDP_PORT, SOCK_DGRAM, FD_READ) == 0) {
			CString strError;
			strError = ::FormatErrorMsg(::GetLastError());

			//로그출력
			LOGDISPLAY_SPEC(0)(_T("Melsec Open 에러-2 - 에러<%s>"), strError);
			return (-1);
		}
		bRet = m_pEthernetSock->Connect(m_strIpAddress, MELSEC_UDP_PORT);
	}
	//m_pEthernetSock->m_bConnected = TRUE;
	//m_pEthernetSock->m_bDisConnected = FALSE;

	if (bRet == FALSE) {
		DWORD dwErrorCode = GetLastError();
		CString	strErMsg = _T("");
		switch (dwErrorCode) {
		case	WSANOTINITIALISED:
			strErMsg.Format(_T("이 API를 사용하기 전에 AfxSocketInit 호출이 성공적인 완료가 필요합니다."));
			break;
		case	WSAENETDOWN:
			strErMsg.Format(_T("Windows 소켓 구현이 네트워크 서브 시스템의 이상을 검출했습니다."));
			break;
		case	WSAEADDRINUSE:
			strErMsg.Format(_T("지정한 주소는 사용중입니다."));
			break;
		case	WSAEINPROGRESS:
			strErMsg.Format(_T("실행중인 Windows 소켓 작업이 차단되어 있습니다."));
			break;
		case	WSAEADDRNOTAVAIL:
			strErMsg.Format(_T("지정된 주소는 로컬 컴퓨터에서 사용할 수 없습니다."));
			break;
		case	WSAEAFNOSUPPORT:
			strErMsg.Format(_T("지정한 주소 제품군이 소켓에서 지원하지 않습니다."));
			break;
		case	WSAECONNREFUSED:
			strErMsg.Format(_T("연결을 시도했으나 거부되었습니다."));
			break;
		case	WSAEDESTADDRREQ:
			strErMsg.Format(_T("목적지 주소가 필요합니다."));
			break;
		case	WSAEFAULT:
			strErMsg.Format(_T("인수 SockAddr_in가 잘못되었습니다."));
			break;
		case	WSAEINVAL:
			strErMsg.Format(_T("호스트 주소가 잘못되었습니다."));
			break;
		case	WSAEISCONN:
			strErMsg.Format(_T("소켓은 이미 연결되어 있습니다."));
			return(0); //break ;
		case	WSAEMFILE:
			strErMsg.Format(_T("유효한 파일 디스크립터가 아닙니다."));
			break;
		case	WSAENETUNREACH:
			strErMsg.Format(_T("현재 호스트에서 네트워크에 연결할 수 없습니다."));
			break;
		case	WSAENOBUFS:
			strErMsg.Format(_T("사용가능한 버퍼 공간이 없습니다. 소켓을 연결할 수 없습니다."));
			break;
		case	WSAENOTSOCK:
			strErMsg.Format(_T("디스크립터가 소켓이 아닙니다."));
			break;
		case	WSAETIMEDOUT:
			strErMsg.Format(_T("연결을 시도했지만 시간에 연결할 수 없습니다."));
			break;
		case	WSAEWOULDBLOCK:
			return (0);
		default:
			strErMsg.Format(_T("소켓 오류：%lu"), (DWORD)dwErrorCode);
			break;
		}
		//로그출력
		LOGDISPLAY_SPEC(0)(_T("Melsec Open 에러-3 - 에러<%s>"), strErMsg);
		return (-1);
	}

	return bRet;
}

int CMelsecEthernet::ClosePio(void)
{
	int nRet = 0;

	if (m_pEthernetSock != NULL) {
		m_pEthernetSock->Close();
		delete m_pEthernetSock;
		m_pEthernetSock = NULL;
	}

	return nRet;
}
