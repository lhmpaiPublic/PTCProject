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
		//�����α�
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
			//�����α�
		}
		return (-1);
		}

		// 22.03.24 Ahn Delete Start
		//nRet = OutPort(nPort, data, TRUE, bGetBit);
		//if (nRet < 0) {
		//	nRet = -1;
		//	//�����α�
		//}
		btOutPortData[nPort] = data;
		// 22.03.24 Ahn Delete Start
	}
	return (nRet);

}
// 22.03.24 Ahn Modify Start
// �̹� �о�� ��Ʈ ���� ��ȯ��.
int CMelsecEthernet::InPort(WORD nPort, BYTE* data, BOOL bExtSt /*= FALSE*/)
{
	ASSERT(nPort >= 0);
	ASSERT(nPort < m_wMaxPort);
	ASSERT(data);
	if ((nPort < 0) || (nPort >= m_wMaxPort)) {
		//�����α�
		return (-1);
	}
	int	nRet = 0;

	{
		CSingleLock cs(GetCriticalSection(), TRUE);
		*data = btInPortData[nPort];
	}
	return (nRet);
}
// 1����Ʈ�� �о��
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
		//�����α�
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
		//�����α�
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
		//�����α�
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
			AprData.SaveErrorLog(strError);
			return (-1);
		}
		bRet = m_pEthernetSock->Connect( m_strIpAddress, MELSEC_TCP_PORT);
	}
	else {
		if (m_pEthernetSock->Create(MELSEC_UDP_PORT, SOCK_DGRAM, FD_READ) == 0) {
			CString strError;
			strError = ::FormatErrorMsg(::GetLastError());
			AprData.SaveErrorLog(strError);
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
			strErMsg.Format(_T("�� API�� ����ϱ� ���� AfxSocketInit ȣ���� �������� �Ϸᰡ �ʿ��մϴ�."));
			break;
		case	WSAENETDOWN:
			strErMsg.Format(_T("Windows ���� ������ ��Ʈ��ũ ���� �ý����� �̻��� �����߽��ϴ�."));
			break;
		case	WSAEADDRINUSE:
			strErMsg.Format(_T("������ �ּҴ� ������Դϴ�."));
			break;
		case	WSAEINPROGRESS:
			strErMsg.Format(_T("�������� Windows ���� �۾��� ���ܵǾ� �ֽ��ϴ�."));
			break;
		case	WSAEADDRNOTAVAIL:
			strErMsg.Format(_T("������ �ּҴ� ���� ��ǻ�Ϳ��� ����� �� �����ϴ�."));
			break;
		case	WSAEAFNOSUPPORT:
			strErMsg.Format(_T("������ �ּ� ��ǰ���� ���Ͽ��� �������� �ʽ��ϴ�."));
			break;
		case	WSAECONNREFUSED:
			strErMsg.Format(_T("������ �õ������� �źεǾ����ϴ�."));
			break;
		case	WSAEDESTADDRREQ:
			strErMsg.Format(_T("������ �ּҰ� �ʿ��մϴ�."));
			break;
		case	WSAEFAULT:
			strErMsg.Format(_T("�μ� SockAddr_in�� �߸��Ǿ����ϴ�."));
			break;
		case	WSAEINVAL:
			strErMsg.Format(_T("ȣ��Ʈ �ּҰ� �߸��Ǿ����ϴ�."));
			break;
		case	WSAEISCONN:
			strErMsg.Format(_T("������ �̹� ����Ǿ� �ֽ��ϴ�."));
			return(0); //break ;
		case	WSAEMFILE:
			strErMsg.Format(_T("��ȿ�� ���� ��ũ���Ͱ� �ƴմϴ�."));
			break;
		case	WSAENETUNREACH:
			strErMsg.Format(_T("���� ȣ��Ʈ���� ��Ʈ��ũ�� ������ �� �����ϴ�."));
			break;
		case	WSAENOBUFS:
			strErMsg.Format(_T("��밡���� ���� ������ �����ϴ�. ������ ������ �� �����ϴ�."));
			break;
		case	WSAENOTSOCK:
			strErMsg.Format(_T("��ũ���Ͱ� ������ �ƴմϴ�."));
			break;
		case	WSAETIMEDOUT:
			strErMsg.Format(_T("������ �õ������� �ð��� ������ �� �����ϴ�."));
			break;
		case	WSAEWOULDBLOCK:
			return (0);
		default:
			strErMsg.Format(_T("���� ������%lu"), (DWORD)dwErrorCode);
			break;
		}
		CString str;
		str.Format(_T("Socket Connect Error: ip %s, %s"), m_strIpAddress, strErMsg);
		//	AprData.SaveDebugLog(str);
		//m_pEthernetSock->m_bConnected = FALSE;
		//m_pEthernetSock->m_bDisConnected = TRUE;
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
