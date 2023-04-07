#include "pch.h"
#include "CMelsecEthernetSock.h"

#include "TimeAnalyzer.h"
#include "WinTool.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include "GlobalData.h"

#define MAX_BUFFER_SIZE 256


CMelsecEthernetSock::CMelsecEthernetSock(LPCTSTR lpszIpAddress, int protocol) {
	if (m_strIpAddress.GetLength() >= 7) {
		m_strIpAddress.Format( _T("%s"), lpszIpAddress);
	}
	m_Protocol = protocol;

	if (m_btRecv == NULL) {
		m_btRecv = new char[TCP_MAX_DATA];
		memset(m_btRecv, 0x00, sizeof(char) * TCP_MAX_DATA);
	}

	::InitializeCriticalSection(&csFunc);
}

CMelsecEthernetSock::~CMelsecEthernetSock()
{
	if (m_btRecv != NULL) {
		delete[] m_btRecv;
		m_btRecv = NULL;
	}

	m_bConnected = FALSE;
	m_bDisConnected = TRUE;

	::DeleteCriticalSection(&csFunc);
}

BOOL CMelsecEthernetSock::MakeCommand(int nMode, int nDevType, int nDevNo, int nDataSize, char* pDevData, int* pnDataLength, char* pBuff)
{
	ASSERT(pBuff);
	BOOL bRet = TRUE;

	int nDataLength = 0;

	MakeHeaderCommand(nMode, nDevType, pBuff);
	//MakeHeaderCommand(nMode, nDevType, *pnDataLength, pBuff);
	MakeBodyCommand(nMode, nDevNo, nDevType, pBuff);

	int nWriteReqSize;
	int nSendSize = nDataSize * 2; // Word 영역은 32비트가 하나 이기 때문에 

	// Data
	int i;
	if (nMode == MODE_WRITE) {
		switch (nDevType) {
		case	en_DevCodeW:
			pBuff[en_DevCountL_Pos] = (BYTE)(nDataSize & 0xff);
			pBuff[en_DevCountH_Pos] = (BYTE)((nDataSize >>8) & 0xff);
			for ( i = 0; i < nSendSize; i += 2) {
				pBuff[en_DevDataPos + i ] = (BYTE)pDevData[i];
				pBuff[en_DevDataPos + i + 1 ] = (BYTE)pDevData[i + 1];
			}
			nDataLength = en_WriteLength + nSendSize;

			nWriteReqSize = en_RDataLengthL + nSendSize;
			pBuff[en_DataLengthL_Pos] = (nWriteReqSize & 0xff) ;
			pBuff[en_DataLengthH_Pos] = (nWriteReqSize>>8 & 0xff);
			break;
		case	en_DevCodeB:			
			pBuff[en_DevCountL_Pos] = (BYTE)(nDataSize & 0xff);
			pBuff[en_DevCountH_Pos] = (BYTE)((nDataSize >> 8) & 0xff);
			pBuff[en_DataLengthL_Pos] = BYTE( (en_WDataLengthL + nDataSize) & 0xff ) ;
			pBuff[en_DataLengthH_Pos] = 0x00;
			nDataLength = en_WriteLength + nDataSize ;
			break;
		case	en_DevCodeD:
		case	en_DevCodeM:
		default:
			break;
		}
	}
	else {
		// Read Mode
		pBuff[en_DevCountL_Pos] = (BYTE)(nDataSize & 0xff);
		pBuff[en_DevCountH_Pos] = (BYTE)((nDataSize >> 8) & 0xff);
		nDataLength = en_ReadLength;
	}
	*pnDataLength = nDataLength;

	return bRet;
}


BOOL CMelsecEthernetSock::MakeHeaderCommand(int nMode, int nDevType, char* pBuff)
{
	BOOL bRet = FALSE;

	ASSERT(pBuff);

	// Data Body
	pBuff[en_SubHeadL_Pos] = (BYTE)en_SubHeaderL;
	pBuff[en_SubHeadH_Pos] = (BYTE)en_SubHeaderH;
	pBuff[en_NetworkNo_Pos] = (BYTE)en_NetworkNo;
	pBuff[en_PlcNo_Pos] = (char)en_PlcNo;
	pBuff[en_IoModuleNoL_Pos] = (char)en_IoModuleNoL;
	pBuff[en_IoModuleNoH_Pos] = (BYTE)en_IoModuleNoH;
	pBuff[en_ReqModuleNo_Pos] = (BYTE)en_ReqModuleNo;
	pBuff[en_CpuCheckTimerL_Pos] = (BYTE)en_CpuCheckTimerL;
	pBuff[en_CpuCheckTimerH_Pos] = (BYTE)en_CpuCheckTimerH;


	if (nMode == MODE_READ) {
		pBuff[en_DataLengthL_Pos] = (BYTE)en_RDataLengthL;
		pBuff[en_DataLengthH_Pos] = (BYTE)en_RDataLengthH;
		// Command
		pBuff[en_CmdL_Pos] = (BYTE)en_CmdReadL;
		pBuff[en_CmdH_Pos] = (BYTE)en_CmdReadH;
	}
	else {
		pBuff[en_DataLengthL_Pos] = en_WDataLengthL;
		pBuff[en_DataLengthH_Pos] = en_WDataLengthH;
		// Command
		pBuff[en_CmdL_Pos] = (BYTE)en_CmdWriteL;
		pBuff[en_CmdH_Pos] = (BYTE)en_CmdWriteH;
	}

	return bRet;
}

BOOL CMelsecEthernetSock::MakeBodyCommand(int nMode, int nDevNo, int nDevCode, char* pBuff)
{
	ASSERT(pBuff);

	BOOL bRet = FALSE;

	pBuff[en_SubCmdL_Pos] = (BYTE)en_SubCmdL;
	pBuff[en_SubCmdH_Pos] = (BYTE)en_SubCmdH;

	pBuff[en_DevHead_L_Pos] = (BYTE)(nDevNo & 0xFF);
	pBuff[en_DevHead_M_Pos] = (BYTE)(nDevNo >> 8) & 0xFF;
	pBuff[en_DevHead_H_Pos] = (BYTE)(nDevNo >> 16) & 0xFF;

	pBuff[en_DevCode_Pos] = (BYTE)nDevCode; // D : 0xA8 B :0xA0

	return bRet;
}

BOOL CMelsecEthernetSock::MakeBodyCommand(int nMode, int nDevNo, int nDevCode, char* pBuff, int nDataSize, char* pSendData)
{
	ASSERT(pBuff);

	BOOL bRet = FALSE;
	MakeBodyCommand(nMode, nDevNo, nDevCode, pBuff);

	pBuff[en_DevCountL_Pos] = nDataSize & 0x00ff;
	pBuff[en_DevCountH_Pos] = ( nDataSize >> 8 ) & 0x00ff;

	// 그대로 카피 
	//memcpy(&pBuff[en_DevDataPos], pSendData, sizeof(char) * nDataSize );	

	for (int i = 0; i < nDataSize; i += 2) {
		pBuff[en_DevDataPos + i] = pSendData[i] ;					// L
		pBuff[en_DevDataPos + i + 1] = pSendData[i+1] ;				// H
	}

	return bRet;
}


int CMelsecEthernetSock::SendAndRecv(char* pSendBuff, int nCmdLen, char* pRecvBuff, int nRecvLen, long lTimeOver )
{
	if (pSendBuff == NULL) {
		ASSERT(FALSE);
		return (-1);
	}
	if (pRecvBuff == NULL) {
		ASSERT(FALSE);
		return (-1);
	}

	ClearBuffer();

	if (PacketSend(pSendBuff, nCmdLen, lTimeOver) != 0) {
		return (-1);
	}

	if (DataRecv(pRecvBuff, nRecvLen, lTimeOver) != 0) {
		return (-1);
	}

	return 0;
}
int CMelsecEthernetSock::InPort(int nDevNo, WORD* data, int nSize,  BOOL bExtSt /*= FALSE*/)
{
	ASSERT(data);

	int nRet = 0;
	if (m_bConnected == FALSE) {
		return -1;
	}
	char szBuff[MAX_BUFFER_SIZE];
	char szRecv[MAX_BUFFER_SIZE];
	int nCmdLength = 0;
	int nRecvLength = MAX_BUFFER_SIZE;
	int nBuffSize = sizeof(char) * MAX_BUFFER_SIZE;
	memset(szBuff, 0x00, nBuffSize);
	memset(szRecv, 0x00, nBuffSize);
	BOOL bLocalRet;
	int nRecvSize = nSize / 2;
	bLocalRet = MakeCommand(MODE_READ, en_DevCodeB, nDevNo, nRecvSize, NULL, &nCmdLength, szBuff);
	//bLocalRet2 = MakeBodyCommand(MODE_READ, nDevNo, en_DevCodeB, &szBuff[nCmdLength]);

	szBuff[en_DevCountL_Pos] = (BYTE)(nRecvSize & 0xff);
	szBuff[en_DevCountH_Pos] = (BYTE)((nRecvSize >> 8 ) & 0xff );
	if (!bLocalRet) {
		// 커맨드 이상
		return -1;
	}

#if defined( DEBUG_MELSEC_ETHERNET )
	CString strMsg, strTemp;
	strMsg = _T("");
	for (int i = 0; i < nCmdLength; i++) {
		strTemp.Format(_T("%02x"), (BYTE)szBuff[i]);
		strMsg += strTemp;
	}
	CString strLog;
	strLog.Format(_T("In Port Bit : %s"), strMsg);
	AprData.SaveErrorLog(strLog);

#endif

	int nLocalRet = SendAndRecv(szBuff, nCmdLength, szRecv, nRecvLength, 200);
	if (nLocalRet < 0) {
		// Error
		return -2;
	}

	nLocalRet = CheckRecvCmd(szRecv);
	if (nLocalRet < 0) {
		return -3;
	}

	// 22.03.22 Ahn Modify Start
	WORD wTemp;// = szRecv[en_ReadDataStartPos + 1];
	//*data = (wTemp & 0xffff);
	for (int i = 0; i < nSize; i++) {
		wTemp = (WORD)szRecv[en_ReadDataStartPos + i + 1];
		data[i] = (wTemp & 0xffff);
	}
	// 22.03.22 Ahn Modify End

	return nRet;
}
int CMelsecEthernetSock::OutPort(int nDevNo, WORD *data, int nSize, BOOL bLockCtrl /*= TRUE*/, BOOL bGetBit /*= FALSE*/)
{
	int nRet = 0;
	if (m_bConnected == FALSE) {
		return -1;
	}
	char szBuff[MAX_BUFFER_SIZE];
	char szRecv[MAX_BUFFER_SIZE];
	int nCmdLength = 0;
	int nRecvLength = MAX_BUFFER_SIZE;
	int nBuffSize = sizeof(char) * MAX_BUFFER_SIZE;
	memset(szBuff, 0x00, nBuffSize);
	memset(szRecv, 0x00, nBuffSize);
	BOOL bLocalRet;
	int nSendSize = nSize / 2;
	bLocalRet = MakeCommand(MODE_WRITE, en_DevCodeB, nDevNo, nSendSize, NULL, &nCmdLength, szBuff);

	// 22.03.24 Ahn Modify End
	//szBuff[en_DevCountL_Pos] = 0x02;
	//szBuff[en_DevCountH_Pos] = 0x00;
	//szBuff[en_DevCountL_Pos] = (BYTE)(nSendSize & 0xff);
	//szBuff[en_DevCountH_Pos] = (BYTE)((nSendSize >> 8) & 0xff);

	// 22.03.22 Ahn Modify Start
	//szBuff[en_DevDataPos] = (char)(data[0] & 0xff );
	//szBuff[en_DevDataPos+1] = (char)(( data[0] >> 8) & 0xff);
	//szBuff[en_DevDataPos+2] = (char)(data[1] & 0xff);
	//szBuff[en_DevDataPos+3] = (char)((data[1] >> 8) & 0xff);	
	// 22.03.22 Ahn Modify End
	for (int i = 0; i < nSize; i++) {
		int nPos = i * 2;
		szBuff[en_DevDataPos + nPos ]	= (char)(data[i] & 0xff );
		szBuff[en_DevDataPos + nPos +1 ]	= (char)(( data[i] >> 8) & 0xff);
	}
	// 22.03.24 Ahn Modify End

	if (!bLocalRet) {
		// 커맨드 이상
		return -1;
	}

#if defined( DEBUG_MELSEC_ETHERNET )
	CString strMsg, strTemp;
	strMsg = _T("");
	for (int i = 0; i < nCmdLength; i++) {
		strTemp.Format(_T("%02x"), (BYTE)szBuff[i]);
		strMsg += strTemp;
	}
	CString strLog;
	strLog.Format(_T("Out Port Bit : %s"), strMsg);
	AprData.SaveErrorLog(strLog);

#endif

	int nLocalRet = SendAndRecv(szBuff, nCmdLength, szRecv, nRecvLength, 200);
	if (nLocalRet < 0) {
		// Error
		return -2;
	}

	nLocalRet = CheckRecvCmd(szRecv);
	if (nLocalRet < 0) {
		return -3;
	}

	return nRet;

}



int CMelsecEthernetSock::InPort(int nDevNo, BYTE* data, BOOL bExtSt /*= FALSE*/)
{
	ASSERT(data);

	int nRet = 0; 
	if (m_bConnected == FALSE) {
		return -1;
	}
	char szBuff[MAX_BUFFER_SIZE] ;
	char szRecv[MAX_BUFFER_SIZE] ;
	int nCmdLength = 0;
	int nRecvLength = MAX_BUFFER_SIZE ;
	int nBuffSize = sizeof(char) * MAX_BUFFER_SIZE;
	memset(szBuff, 0x00, nBuffSize);
	memset(szRecv, 0x00, nBuffSize);
	BOOL bLocalRet ;
	bLocalRet  = MakeCommand(MODE_READ, en_DevCodeB, nDevNo, 0, NULL, &nCmdLength, szBuff);
	//bLocalRet2 = MakeBodyCommand(MODE_READ, nDevNo, en_DevCodeB, &szBuff[nCmdLength]);

	// 22.03.22 Ahn Add Start
	szBuff[en_DevCountL_Pos] = 0x01;
	szBuff[en_DevCountH_Pos] = 0x00;
	// 22.03.22 Ahn Add End
	if (!bLocalRet) {
		// 커맨드 이상
		return -1;
	}

#if defined( DEBUG_MELSEC_ETHERNET )
	CString strMsg, strTemp;
	strMsg = _T("");
	for (int i = 0; i < nCmdLength; i++) {
		strTemp.Format(_T("%02x"), (BYTE)szBuff[i]);
		strMsg += strTemp;
	}
	CString strLog;
	strLog.Format(_T("In Port Bit : %s"), strMsg);
	AprData.SaveErrorLog(strLog);

#endif

	int nLocalRet = SendAndRecv(szBuff, nCmdLength, szRecv, nRecvLength, 200);
	if (nLocalRet < 0) {
		// Error
		return -2;
	}

	nLocalRet = CheckRecvCmd( szRecv );
	if (nLocalRet < 0) {
		return -3;
	}

	WORD wTemp = szRecv[en_ReadDataStartPos + 1];
	*data = (wTemp & 0xff );
	//data[1] = (wTemp >> 8 & 0xff);

	return nRet;
}

int CMelsecEthernetSock::OutPort(int nDevNo, BYTE data, BOOL bLockCtrl /*= TRUE*/, BOOL bGetBit /*= FALSE*/)
{
	int nRet = 0;
	if (m_bConnected == FALSE) {
		return -1;
	}
	char szBuff[MAX_BUFFER_SIZE];
	char szRecv[MAX_BUFFER_SIZE];
	int nCmdLength = 0;
	int nRecvLength = MAX_BUFFER_SIZE;
	int nBuffSize = sizeof(char) * MAX_BUFFER_SIZE;
	memset(szBuff, 0x00, nBuffSize);
	memset(szRecv, 0x00, nBuffSize);
	BOOL bLocalRet;
	bLocalRet = MakeCommand(MODE_WRITE, en_DevCodeB, nDevNo, 1, NULL, &nCmdLength, szBuff);

	// 22.03.22 Ahn Modify End
	szBuff[en_DevCountL_Pos] = 0x01;
	szBuff[en_DevCountH_Pos] = 0x00;
	// 22.03.22 Ahn Modify End
	szBuff[en_DevDataPos] = data ;

	if (!bLocalRet) {
		// 커맨드 이상
		return -1;
	}

#if defined( DEBUG_MELSEC_ETHERNET )
	CString strMsg, strTemp ;
	strMsg = _T("");
	for (int i = 0; i < nCmdLength; i++) {
		strTemp.Format(_T("%02x"), (BYTE)szBuff[i]);
		strMsg += strTemp;
	}
	CString strLog;
	strLog.Format(_T("Out Port Bit : %s"), strMsg);
	AprData.SaveErrorLog(strLog);

#endif

	int nLocalRet = SendAndRecv(szBuff, nCmdLength, szRecv, nRecvLength, 200 );
	if (nLocalRet < 0) {
		// Error
		return -2;
	}

	nLocalRet = CheckRecvCmd(szRecv);
	if (nLocalRet < 0) {
		return -3;
	}

	return nRet;

}

int CMelsecEthernetSock::ReadDataReg(int nDevNo, short data[], int num)
{
	int nRet = 0;

	if (m_bConnected == FALSE) {
		return -1;
	}
	char szBuff[MAX_BUFFER_SIZE];
	char szRecv[MAX_BUFFER_SIZE];
	int nCmdLength = 0;
	int nRecvLength = MAX_BUFFER_SIZE;
	int nBuffSize = sizeof(char) * MAX_BUFFER_SIZE;
	memset(szBuff, 0x00, nBuffSize);
	memset(szRecv, 0x20, nBuffSize);
	BOOL bLocalRet;
	bLocalRet = MakeCommand(MODE_READ, en_DevCodeW, nDevNo, num, NULL, &nCmdLength, szBuff) ;

	if (!bLocalRet) {
		return -1;
	}

#if defined( DEBUG_MELSEC_ETHERNET )
	CString strMsg, strTemp;
	strMsg = _T("");
	for (int i = 0; i < nCmdLength; i++) {
		strTemp.Format(_T("%02x"), (BYTE)szBuff[i]);
		strMsg += strTemp;
	}

	CString strLog;
	strLog.Format(_T("Out Bit : %s"), strMsg);
	AprData.SaveErrorLog(strLog);
#endif

	//nCmdLength += ( num / 2 ) ; // 22.03.22 ?????? 

	int nLocalRet = SendAndRecv( szBuff, nCmdLength, szRecv, nRecvLength, 400 ) ;
	if (nLocalRet < 0) {
		// Error
		return -2;
	}

	nLocalRet = CheckRecvCmd(szRecv);
	if (nLocalRet < 0) {
		return -3;
	}

	// 22.03.21 Ahn Modify Start
	memcpy((char*)data, szRecv + en_ReadDataStartPos+1, sizeof(char)* (num * 2) );
	// 22.03.21 Ahn Modify End

	return nRet;
}

int CMelsecEthernetSock::WriteDataReg(int nDevNo, short data[], int num)
{
	int nRet = 0;
	if (m_bConnected == FALSE) {
		return -1;
	}

	if (m_bConnected == FALSE) {
		return -1;
	}
	char szBuff[MAX_BUFFER_SIZE];
	char szRecv[MAX_BUFFER_SIZE];
	int nCmdLength = 0;
	int nRecvLength = MAX_BUFFER_SIZE;
	int nBuffSize = sizeof(char) * MAX_BUFFER_SIZE;
	memset(szBuff, 0x00, nBuffSize);
	memset(szRecv, 0x00, nBuffSize);
	BOOL bLocalRet;

	int nSendLength = num / 2 ;
	bLocalRet = MakeCommand(MODE_WRITE, en_DevCodeW, nDevNo, nSendLength, (char*)&data, &nCmdLength, szBuff);

	if (!bLocalRet) {
		return -1;
	}

#if defined( DEBUG_MELSEC_ETHERNET )
	CString strMsg, strTemp;
	strMsg = _T("");
	for (int i = 0; i < nCmdLength; i++) {
		strTemp.Format(_T("%02x"), (BYTE)szBuff[i]);
		strMsg += strTemp;
	}

	CString strLog;
	strLog.Format(_T("Out Word : %s"), strMsg);
	AprData.SaveErrorLog(strLog);

#endif


	//memcpy(&szBuff[nCmdLength], (char*)data, num);
	//nCmdLength += (num / 2);

	int nLocalRet = SendAndRecv( szBuff, nCmdLength, szRecv, nRecvLength, 200 ) ;
	if (nLocalRet < 0) {
		// Error
		return -2;
	}

	nLocalRet = CheckRecvCmd(szRecv);
	if (nLocalRet < 0) {
		return -3;
	}
	return nRet;
}

int CMelsecEthernetSock::CheckRecvCmd( char* pRecv)
{
	ASSERT(pRecv);

	if (pRecv[en_RecvCodeL_Pos] != 0x00) {
		return -1;
	}

	if (pRecv[en_RecvCodeL_Pos] != 0x00) {
		return -2;
	}

	return 0;
}

void CMelsecEthernetSock::OnAccept(int nErrorCode)
{
	CAsyncSocket::OnAccept(nErrorCode);
}

void CMelsecEthernetSock::OnClose(int nErrorCode)
{
	m_bConnected = FALSE;
	m_bDisConnected = TRUE;

	CAsyncSocket::OnClose(nErrorCode);
	Close();
}


void CMelsecEthernetSock::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0) {
		// -----정상-----
		m_bConnected = TRUE;
		m_bDisConnected = FALSE;
	}
	else {
		// -----이상-----
		m_bConnected = FALSE;
		m_bDisConnected = TRUE;
	}

	CAsyncSocket::OnConnect(nErrorCode);
}


void CMelsecEthernetSock::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0) {
		// -----정상-----
			//
	}
	else {
		// -----이상-----
		m_bConnected = FALSE;
		m_bDisConnected = TRUE;
	}
	CTime ctStart = CTime::GetCurrentTime();
	CTime ctEnd;
	CTimeSpan ctsDiff;
	{
		::EnterCriticalSection(&csFunc);

		int nRet = Receive(&m_btRecv[m_RecvCnt], TCP_MAX_DATA - m_RecvCnt);
		if (nRet < 0) {
			BOOL bNoError = FALSE;
			DWORD dwErrorCode = GetLastError();
			CString strErMsg, strError;
			bNoError = SetError(dwErrorCode, &strErMsg);

			strError.Format(_T("%s %s"), this->m_strIpAddress, strErMsg);
			//SAVE_ERROR_LOG( strError ) ;

			if (bNoError != TRUE) {
				// クリア
				m_RecvCnt = 0;
				::LeaveCriticalSection(&csFunc);
				return;
			}
			::LeaveCriticalSection(&csFunc);
			return;
		}
		else {
			m_RecvCnt += nRet;
		}

		::LeaveCriticalSection(&csFunc);
	}

	CAsyncSocket::OnReceive(nErrorCode);
}


void CMelsecEthernetSock::OnSend(int nErrorCode)
{
	if (nErrorCode == 0) {
		// -----정상-----
			//
	}
	else {
		// -----이상-----
		m_bConnected = FALSE;
		m_bDisConnected = TRUE;
	}

	SendRetry();

	CAsyncSocket::OnSend(nErrorCode);
}


int CMelsecEthernetSock::PacketSend(char* pBuff, int length, long lTimeOver)
{

	if (m_Protocol == TCP_MODE) {
		if (m_bConnected != TRUE) {
			// -----비접속상태-----
			return (-2);
		}
	}

	if (m_pSendBuffer != NULL) {
		return (-1);
	}

	m_SendBuffCnt = length + 3;
	m_pSendBuffer = new BYTE[m_SendBuffCnt + 1];

	memset(m_pSendBuffer, 0x00, sizeof(BYTE) * (m_SendBuffCnt + 1));
	m_SendCnt = 0;

	BYTE* ptr = m_pSendBuffer;
	int	c;

	for (c = 0; c < length; c++) {
		*ptr++ = pBuff[c];
	}

	m_SendFlag = TRUE;
	SendRetry();

	int ret = 0;
	CTimeAnalyzer ta;
	ta.StopWatchStart();

	while (m_SendFlag) {
		::PumpMessages();
		::PumpMessages(NULL, NULL, WM_SOCKET_NOTIFY, WM_SOCKET_DEAD);
		::Sleep(1);
		if (ta.WhatTimeIsIt() > lTimeOver) {
			ret = -1;
			break;
		}
	}

	if (m_pSendBuffer) {
		delete[] m_pSendBuffer;
		m_pSendBuffer = NULL;
	}
	m_SendBuffCnt = 0;
	m_SendCnt = 0;

	return (ret);
}


int CMelsecEthernetSock::DataRecv(char* pBuff, int length, long lTimeOver)
{
	CTimeAnalyzer ta;
	ta.StopWatchStart();

	int n = 0;
	int nRecvSize = 0;
	while (1) {
		if (m_Protocol == TCP_MODE) {
			if (m_bDisConnected == TRUE) {
				return (-1);
			}
		}

		{
			::EnterCriticalSection(&csFunc);

			//if ((m_btRecv[m_RecvCnt - 3] == enCR) && (m_btRecv[m_RecvCnt - 2] == enLF))
			if( (BYTE)m_btRecv[0] == 0xD0 ) // 22.03.21 Ahn Test !!!!!!!!!!!!!
			{
				// -----수신완료-----
				m_btRecv[m_RecvCnt] = 0x00;
				// 중간에 NULL값이 있는 경우 이후의 값을 정상적으로 읽어오지 않음. 예) CH이나 Bright 값이 0이 중간에 들어갈 경우 
				for (int i = 0; i < m_RecvCnt; i++)
				{
					pBuff[i] = m_btRecv[i];
				}
				nRecvSize = m_RecvCnt;
				m_RecvCnt = 0;
				::LeaveCriticalSection(&csFunc);
				break;
			}

			::LeaveCriticalSection(&csFunc);
		}

		::PumpMessages();
		::PumpMessages(NULL, NULL, WM_SOCKET_NOTIFY, WM_SOCKET_DEAD);

		if (ta.WhatTimeIsIt() > lTimeOver) {
			return (-1);
		}

		n++;
	}


#if defined( DEBUG_MELSEC_ETHERNET )

	CString strMsg, strTemp;
	strMsg = _T("");
	for (int i = 0; i < nRecvSize; i++) {
		strTemp.Format(_T("%02x"), (BYTE)m_btRecv[i]);
		strMsg += strTemp;
	}

	CString strLog;
	strLog.Format(_T("Data REcve : %s"), strMsg);
	AprData.SaveErrorLog(strLog);

#endif

	if ((pBuff[en_RecvCodeL_Pos] != 00) || (pBuff[en_RecvCodeH_Pos] != 00)) {
		// Error ;
		//CString strErrorCode;
		//strErrorCode.Format(_T("%02x%02x"), (BYTE)pBuff[en_RecvCodeL_Pos], (BYTE)pBuff[en_RecvCodeH_Pos]);
		//int a = 0;
		return -1;
	}

	return (0);
}


int CMelsecEthernetSock::SetError(DWORD code, CString* msg)
{
	int	nRet = -1;
	DWORD	dwErrorCode = code;
	CString	strErMsg;

	switch (dwErrorCode) {
	case	WSANOTINITIALISED:
		strErMsg = _T("이 API 사용 하기 전에 AfxSocketInit 의 호출이 정상종료가 필요합니다.");
		break;
	case	WSAENETDOWN:
		strErMsg = _T("Windows 소켓의 인플먼드가 네트워크 서브시스템의 이상을 검출 하였습니다.");
		break;
	case	WSAENOTCONN:
		strErMsg = _T("소켓이 접속되지 않았습니다.");
		break;
	case	WSAEINPROGRESS:
		strErMsg = _T("실행중인 Windows 소켓 호출이 블록킹 되었습니다.");
		break;
	case	WSAENOTSOCK:
		strErMsg = _T("디스크리프터가 소켓에 없습니다.");
		break;
	case	WSAEOPNOTSUPP:
		strErMsg = _T("MSG_OOB 지정 되었으나, SOCK_STREAM 형의 소켓이 아닙니다.");
		break;
	case	WSAESHUTDOWN:
		strErMsg = _T("소켓은 셧다운 되었습니다. nHow 의 0 또는 2 를 넘겨서 ShutDown 을 기동한 후에는, 소켓에서 Receive 를 호출할 수 없습니다.");
		break;
	case	WSAEWOULDBLOCK:
		strErMsg = _T("소켓은 비블록킹되어 있어서, Receive 조작이 블록킹 되었습니다.");
		nRet = 0;
		break;
	case	WSAEMSGSIZE:
		strErMsg = _T("데이터그램이 이 너무 길어서, 지정한 버퍼에 다 넣을 수 없기때문에, 손실 되었습니다.");
		break;
	case	WSAEINVAL:
		strErMsg = _T("소켓은  Bind를 사용하여 바인드 되지 않았습니다.");
		break;
	case	WSAECONNABORTED:
		strErMsg = _T("타임 아웃 또는 그외의 장해에 의하여, 가상회로는 Abort 되었습니다.");
		break;
	case	WSAECONNRESET:
		strErMsg = _T("리모트 측에서 가상회로가 리셋 되었습니다.");
		break;
	default:
		strErMsg.Format(_T("소켓 에러 : %lu"), (DWORD)dwErrorCode);
		break;
	}
	if (msg != NULL) {
		*msg = strErMsg;
	}
	return (nRet);
}


int CMelsecEthernetSock::SendRetry(void)
{
	BYTE* pBuff = (BYTE*)m_pSendBuffer;
	int nRet = 0;
	int nLen = m_SendBuffCnt - m_SendCnt;
	DWORD dwSize = m_SendBuffCnt;
	while (nLen > 0) {
		nRet = this->Send(pBuff + (dwSize - nLen), nLen, 0);
		if (nRet < 0) {
			int nError = GetLastError();
			if (nError == WSAEWOULDBLOCK) {
				return (0);
			}
			else {
				return (-1);
			}
		}
		nLen -= nRet;
		m_SendCnt += nRet;
	}
	m_SendFlag = FALSE;
	return (0);
}


int CMelsecEthernetSock::ClearBuffer(void)
{
	m_RecvCnt = 0;
	memset(m_btRecv, 0x00, sizeof(char) * TCP_MAX_DATA);
	return (0);
}

