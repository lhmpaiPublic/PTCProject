#include "pch.h"
#include "LightSocket.h"
#include "CLightControl.h"
#include "TimeAnalyzer.h"
#include "WinTool.h"
#include "Ping.h"
#include "GlobalData.h"


CLightControl::CLightControl(void)
{
	m_bOpened = FALSE;
	for (int i = 0; i < MAX_LIGHT_UNIT ; i++) {
		m_pClient[i] = NULL;
		m_pFineLevel[ i ] = NULL;
	}
	m_nMaxLight = MAX_LIGHT_UNIT;
	// 23.02.07 Ahn Modify Start
	//// 22.09.15 Ahn Modify Start
	////if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	//// 22.09.15 Ahn Modify End
// 	if (AprData.m_System.m_nMachineMode == ANODE_MODE) {  // 
// 	// 23.02.07 Ahn Modify Start
// 		m_nMaxLight = 1 ;
// 	}
}
CLightControl::~CLightControl()
{
}

int CLightControl::Pump(void)
{
	::PumpMessages();
	::PumpMessages(NULL, NULL, WM_SOCKET_NOTIFY, WM_SOCKET_DEAD);
	return (0);
}


int CLightControl::SetUnit(int no, const CString& ip, int mode/*TCP*/)
{

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Light) == TRUE) {
		return 2;
	}

	if (m_pClient[no] != NULL) {
		delete m_pClient[no];
		m_pClient[no] = NULL;
	}
	m_pClient[no] = new CLightSocket(ip, mode);

	int	bRet;
	int	errorcode = 0;
	if (mode == CLightSocket::TCP_MODE) {
		if (m_pClient[no]->Create() == 0) {
			CString strError;
			DWORD dwErrCode = ::GetLastError();
			strError = ::FormatErrorMsg(dwErrCode);
			//DEBUG_LOG.txt
			AprData.SaveDebugLog_Format(_T("<<CLightSocket>>에러 - strError<%s>"), strError);

			return (-1);
		}
		bRet = m_pClient[no]->Connect(ip, TCP_PORT);
	}
	else {
		if (m_pClient[no]->Create(UDP_PORT + no, SOCK_DGRAM, FD_READ) == 0) {
			//			errorcode = GetLastError() ;
			CString strError;
			strError = ::FormatErrorMsg(::GetLastError());
			return (-1);
		}
		bRet = m_pClient[no]->Connect(ip, UDP_PORT);
	}

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
		//DEBUG_LOG.txt
		AprData.SaveDebugLog_Format(_T("<<CLightSocket Connect Error>>에러 - strErMsg<%s>"), strErMsg);

		return (-1);
	}
	return 0;
}

int CLightControl::DeleteUnit(void)
{

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Light) == TRUE) {
		return 2;
	}

	for (int i = 0; i < MAX_LIGHT_UNIT; i++) {
		if (m_pClient[i]) {
			delete m_pClient[i];
			m_pClient[i] = NULL;
		}
	}

	return 0;
}

int CLightControl::Open(void)
{
	BOOL bTimeOut = FALSE;

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Light) == TRUE) {
		return 2;
	}

	if ( m_bOpened == FALSE) {
	//	CString strIpAddress = _T("192.168.10.10");
		CString strIpAddress[MAX_LIGHT_UNIT];
		for ( int i = 0; i < MAX_LIGHT_UNIT; i++) {
			strIpAddress[i].Format( _T("192.168.10.16%d"), i+1);
		}

		int nPort = 1000;

		Close();
		{
			CPing cp;
			cp.SetMaxLoopCount(3);
			cp.SetNotReceiveResponse_ReplyTimeout(FALSE);
			cp.SetWaitReplyTimeout(1, 100);
			BOOL bDone[MAX_LIGHT_UNIT];

			int i = 0;
			{			
				// 22.07.06 Ahn Modify Start
				//for ( i = 0; i < MAX_LIGHT_UNIT; i++){
				for (i = 0; i < m_nMaxLight; i++) {
						// 22.07.06 Ahn Modify End
					if (bDone[i] == TRUE) {
					//	continue;
					}
					if (cp.Ping(strIpAddress[i]) == 0) {
						if (cp.IsConnect() == TRUE) {
							bDone[i] = TRUE;
						}
					}
				}
			}
		}

		for (int i = 0; i < m_nMaxLight; i++) {
			SetUnit(i, strIpAddress[i], CLightSocket::TCP_MODE);
		}

		CTimeAnalyzer ta;
		ta.StopWatchStart();
		int cnt = m_nMaxLight;

		while (cnt) {
			Pump();
			Sleep(10);
			int  n;
			for (n = 0; n < cnt; n++) {
				if (GetConnect(n) == FALSE) {
					break;
				}
			}
			if (n == cnt) {
				break;
			}
			if (ta.WhatTimeIsIt() > 5000) {
				bTimeOut = TRUE;
				break;
			}
		}

		if (bTimeOut == TRUE) {
			return -1;
		}
	}

	m_bOpened = TRUE;

	return 0;
}

int CLightControl::Close(void)
{

	DeleteUnit();
	m_bOpened = FALSE;
	return 0;
}

//int CLightControl::SetLevel(int ch, int nLevel)
//{
//	SetLevel_8Bit(ch, (BYTE)nLevel);
//	return 0;
//}

int CLightControl::SetLevel(int unit, int ch, int nLevel)
{

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Light) == TRUE) {
		return 2;
	}

	// 22.12.22 Ahn Modify Start
	//SetLevel_8Bit(unit, ch, (BYTE)nLevel);

	int nRet = SetLevel_8Bit(unit, ch, (BYTE)nLevel);

	if ( nRet < 0 )
	{
		CString strMsg;
		strMsg.Format(_LANG(_T("조명값 변경 이상 발생."), _T("Set lamp level alarm.")));
		AprData.m_ErrStatus.SetError(CErrorStatus::en_LampError, strMsg); //GMLV 음극 조명 컨트롤러 1개만 있어서 에러 발생, 임시 막음
	}
	// 22.12.22 Ahn Modify End

	return nRet;
}

int CLightControl::SetLevelAll(int unit, int *pLevel, int nLength )
{

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Light) == TRUE) {
		return 2;
	}

	char send_buff[16];
	memset(send_buff, 0x00, sizeof(send_buff));
	char recv_buff[256];
	memset(recv_buff, 0x00, sizeof(recv_buff));
	BYTE btCMD = en_CMD_ALL_CH_WRITE;
	int nCmdLen = en_ALL_CH_WRITE_Length;
	int nRecvLen = en_ALL_CH_READ_RESP_Length;
	send_buff[0] = btCMD;
	if ((nLength > 8) && (nLength < 0)) nLength = 8;
	for (int i = 0; i < nLength; i++) {
		send_buff[i+1] = (char)pLevel[i];  // 1
	}

	for (int i = 0; i < 9; i++) {
		send_buff[9] ^= send_buff[i];
	}

	if (SendNRecvPacket(unit, send_buff, recv_buff, nCmdLen, nRecvLen, 1000) < 0) {
		return (-1);
	}

	// 응답 체크
	BOOL bSuccess = FALSE;
	for (int i = 0; i < nLength; i++) {

		if (recv_buff[0] == btCMD)
		{
			int temp = 0x00;

			if ((int)recv_buff[i+2] == pLevel[ i ])	{
				bSuccess = TRUE;
			}
		}

		if (bSuccess == FALSE)
		{
			CString strError;
			strError.Format(_T("응답Error %s"), recv_buff);
			return (-1);
		}

		m_pLevel[i] = pLevel[i];
		m_pFineLevel[i] = (int)pLevel[i];
	}
	return 0;
}

int CLightControl::GetLevelAll(int unit, int* pLevel, int nLength)
{

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Light) == TRUE) {
		return 2;
	}

	char send_buff[16];
	memset(send_buff, 0x00, sizeof(send_buff));
	char recv_buff[256];
	memset(recv_buff, 0x00, sizeof(recv_buff));
	BYTE btCMD = en_CMD_ALL_CH_READ;
	int nCmdLen = en_CH_READ_REQ_Length;
	int nRecvLen = en_ALL_CH_READ_RESP_Length;
	send_buff[0] = btCMD;
	send_buff[1] ^= send_buff[0];

	if (SendNRecvPacket(unit, send_buff, recv_buff, nCmdLen, nRecvLen, 1000) < 0) {
		return (-1);
	}

	// 응답 체크
	BOOL bSuccess = FALSE;
	for (int i = 0; i < nLength; i++) {
		if (recv_buff[0] == btCMD){
			int temp = 0x00;
			pLevel[ i ] = (BYTE)recv_buff[unit + 1 ];
		//	if ((int)recv_buff[i + 2] == pLevel[i]) {
				bSuccess = TRUE;
		//	}
		}

		if (bSuccess == FALSE){
			CString strError;
			strError.Format(_T("응답Error %s"), recv_buff);
			return (-1);
		}

		m_pLevel[i] = pLevel[i];
		m_pFineLevel[i] = (int)pLevel[i];
	}
	return 0;
}

int CLightControl::CheckLevel(int ch, int nLevel)
{
	return 0;
}

int CLightControl::SendNRecvPacket(int unit, char* pSendBuff, char* pRecvBuff, int nCmdLen, int nRecvLen, long lTimeOver)
{
	int nRet = 0;

	for (int try_cnt = 2; try_cnt >= 0; try_cnt--) {
		CLightSocket* pSock = GetClient(unit);
		if (SendNRecvPacket(pSock, pSendBuff, pRecvBuff, nCmdLen, nRecvLen, lTimeOver) != 0) {
			nRet = -1;
			continue;
		}
		break;
	}
	return (nRet);
}

int CLightControl::SendNRecvPacket(CLightSocket* pSock, char* pSendBuff, char* pRecvBuff, int nCmdLen, int nRecvLen, long lTimeOver)
{

	if (pSock == NULL) {
		ASSERT(FALSE);
		return (-1);
	}
	if (pSendBuff == NULL) {
		ASSERT(FALSE);
		return (-1);
	}
	if (pRecvBuff == NULL) {
		ASSERT(FALSE);
		return (-1);
	}

	pSock->ClearBuffer();
	//if (pSock->PacketSend(pSendBuff, (int)strlen(pSendBuff), lTimeOver) != 0) {
	if (pSock->PacketSend(pSendBuff, nCmdLen, lTimeOver) != 0) {
			return (-1);
	}
	if (pSock->DataRecv(pRecvBuff, nRecvLen, lTimeOver) != 0) {
		return (-1);
	}

	return 0;
}
//
//int CLightControl::SendNRecvPacket(int unit, char* pSendBuff, char* pRecvBuff, int nCmdLen, int nRecvLen, long lTimeOver)
//{
//
//	int nRet = 0;
//
//	for (int try_cnt = 2; try_cnt >= 0; try_cnt--) {
//
//
//		CLightSocket* pSock = GetClient(unit);
//		if (SendNRecvPacket(pSock, pSendBuff, pRecvBuff, nCmdLen, nRecvLen, lTimeOver) != 0) {
//			nRet = -1;
//			continue;
//		}
//
//		break;
//	}
//
//	return (nRet);
//}
//int CLightControl::SendNRecvPacket(CLightSocket* pSock, char* pSendBuff, char* pRecvBuff, int nSendLen, int nRecvLen, long lTimeOver)
//{
//
//	if (pSock == NULL) {
//		ASSERT(FALSE);
//		return (-1);
//	}
//	if (pSendBuff == NULL) {
//		ASSERT(FALSE);
//		return (-1);
//	}
//	if (pRecvBuff == NULL) {
//		ASSERT(FALSE);
//		return (-1);
//	}
//
//	pSock->ClearBuffer();
//	//if (pSock->PacketSend(pSendBuff, (int)strlen(pSendBuff), lTimeOver) != 0) {
//	if (pSock->PacketSend(pSendBuff, nSendLen, lTimeOver) != 0) {
//		return (-1);
//	}
//	if (pSock->DataRecv(pRecvBuff, nRecvLen, lTimeOver) != 0) {
//		return (-1);
//	}
//
//	return 0;
//}

int CLightControl::SetLampON(int ch, BOOL bON)
{

	char send_buff[16];
	memset(send_buff, 0x00, sizeof(send_buff));
	char recv_buff[256];
	memset(recv_buff, 0x00, sizeof(recv_buff));
	BYTE btCMD = en_CMD_CH_MODE;

	send_buff[0] = btCMD;
	send_buff[1] = 1 ;
	send_buff[2] = send_buff[0] ^ send_buff[1];

	if (SendNRecvPacket(ch, send_buff, recv_buff, en_CH_MODE_Length, en_CH_MODE_Length, 1000) < 0) {
		return (-1);
	}

	return 0;
}
BOOL CLightControl::IsLampError(int ch)
{
	return 0;
}
BOOL CLightControl::IsLampErrorAll(void)
{
	return 0;
}

//
//int CLightControl::SetLevel_8Bit(int ch, BYTE level)
//{
////	CString strCommond = _T("WDA");
//
//	char send_buff[16];
//	memset(send_buff, 0x00, sizeof(send_buff));
//	char recv_buff[256];
//	memset(recv_buff, 0x00, sizeof(recv_buff));
//	BYTE btCMD = en_CMD_CH_WRITE;
//	send_buff[0] = btCMD;
//	send_buff[1] = ch;
//	send_buff[2] = (char)level;
//	send_buff[3] = (char)(send_buff[0] ^ send_buff[1] ^ send_buff[2]);
//
//	if (SendNRecvPacket(ch, send_buff, recv_buff, en_CH_WRITE_Length, en_CH_WRITE_Length, 1000) < 0) {
//		return (-1);
//	}
//
//	// 응답 체크
//	BOOL bSuccess = FALSE;
//
//	if (recv_buff[0] == btCMD)
//	{
//		int temp = 0x00;
//
//		if( (int)recv_buff[2] == level )
//		{
//
//			bSuccess = TRUE;
//		}
//	}
//
//	if (bSuccess == FALSE)
//	{
//		CString strError;
//		strError.Format(_T("응답Error %s"), recv_buff);
//		return (-1);
//	}
//
//	m_pLevel[ch] = level;
//	m_pFineLevel[ch] = (int)level;
//	return 0;
//}


int CLightControl::SetLevel_8Bit(int unit, int ch, BYTE level)
{
	//	CString strCommond = _T("WDA");

	char send_buff[16];
	memset(send_buff, 0x00, sizeof(send_buff));
	char recv_buff[256];
	memset(recv_buff, 0x00, sizeof(recv_buff));
	BYTE btCMD = en_CMD_CH_WRITE;
	send_buff[0] = btCMD;
	send_buff[1] = ch;
	send_buff[2] = (char)level;
	send_buff[3] = (char)(send_buff[0] ^ send_buff[1] ^ send_buff[2]);

	if (SendNRecvPacket(unit, send_buff, recv_buff, en_CH_WRITE_Length, en_CH_WRITE_Length, 1000) < 0) {
		return (-1);
	}

	// 응답 체크
	BOOL bSuccess = FALSE;

	if (recv_buff[0] == btCMD)
	{
		int temp = 0x00;

		if (((BYTE)recv_buff[2]) == (BYTE)level)
		{

			bSuccess = TRUE;
		}
	}

	if (bSuccess == FALSE)
	{
		CString strError;
		strError.Format(_T("응답Error %s"), recv_buff);
		return (-1);
	}

	m_pLevel[ch] = level;
	m_pFineLevel[ch] = (int)level;
	return 0;
}


CLightSocket* CLightControl::GetClient(int ch)
{
	if (ch >= m_nMaxLight) { // 22.07.06 Ahn Add
		return (NULL);
	}
	return (m_pClient[ch]);
}


int CLightControl::GetConnect(int ch)
{
	if (m_nMaxLight <= ch) return 0; // 22.07.06 Ahn Add
	if (m_pClient[ch / MAX_LIGHT_CHANEL] == NULL) {
		return (-1);
	}
	if (m_pClient[ch / MAX_LIGHT_CHANEL]->m_Protocol == CLightSocket::UDP_MODE) {
		return (TRUE);
	}
	return (m_pClient[ch / MAX_LIGHT_CHANEL]->m_bConnected);
}


int CLightControl::Reconnect(int no)
{
	if (m_pClient[no] == NULL) {
		return (-1);
	}

	CString ip = m_pClient[no]->m_strIpAddress;
	int mode = m_pClient[no]->m_Protocol;

	// 接続
	SetUnit(no, ip, mode);

	CTimeAnalyzer ta;
	ta.StopWatchStart();
	int	ret = 0;
	while (1) {
		Pump();
		if (m_pClient[no]->m_bConnected == TRUE) {
			break;
		}
		if (ta.WhatTimeIsIt() > 2000) {
			ret = -1;
			break;
		}
	}

	return (ret);
}


// 22.06.71 Ahn Add Start
BOOL CLightControl::IsOpened()
{
	return m_bOpened;
}
// 22.06.71 Ahn Add End
