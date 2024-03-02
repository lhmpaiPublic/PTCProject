// RevoxSocket.cpp 
//


#include "pch.h"
#include "TriggerSocket.h"
#include "WinTool.h"
#include "GlobalData.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CTriggerSocket::CTriggerSocket( LPCTSTR lpszIpAddress, int protocol, CTriggerSocketCall* pTriggerSocketCall)
{
	m_pTriggerSocketCall = pTriggerSocketCall;
	m_strIpAddress = lpszIpAddress ;
	m_Protocol = protocol ;
	m_bConnected = FALSE ;
	m_bDisConnected = FALSE ;

	m_strClientIpAddress = _T( "" ) ;
	m_strClientComputerName = _T( "" ) ;
	m_pSendBuffer = NULL ;
	m_SendBuffCnt = 0 ;
	m_SendCnt = 0 ;
	m_RecvCnt = 0 ;
	m_SendFlag = FALSE ;
	m_btRecv = NULL ;

	::InitializeCriticalSection( &csFunc ) ;

	if ( m_btRecv == NULL ) 
	{
		m_btRecv = new char[ TCP_MAX_DATA ] ;
	}
}


CTriggerSocket::~CTriggerSocket()
{
	if ( m_btRecv != NULL ) {
		delete [] m_btRecv ;
		m_btRecv = NULL ;
	}

	m_bConnected = FALSE ;
	m_bDisConnected = TRUE ;

	m_pTriggerSocketCall = NULL;

	::DeleteCriticalSection( &csFunc ) ;
}


void CTriggerSocket::OnAccept( int nErrorCode )
{
	CAsyncSocket::OnAccept( nErrorCode ) ;
}


void CTriggerSocket::OnClose( int nErrorCode )
{
	m_bConnected = FALSE ;
	m_bDisConnected = TRUE ;

	CAsyncSocket::OnClose( nErrorCode ) ;
	Close() ;
}


void CTriggerSocket::OnConnect( int nErrorCode )
{
	if ( nErrorCode == 0 ) 
	{
		m_bConnected = TRUE ;
		m_bDisConnected = FALSE ;

		int flags = 1;
		setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flags, sizeof(flags));
	} 
	else
	{
		m_bConnected = FALSE ;
		m_bDisConnected = TRUE ;
	}

	CAsyncSocket::OnConnect( nErrorCode ) ;
}


void CTriggerSocket::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		//
	}
	else
	{
		m_bConnected = FALSE;
		m_bDisConnected = TRUE;
	}

	int nRet = Receive(&m_btRecv[0], TCP_MAX_DATA);
	if (nRet < 0)
	{
		BOOL bNoError = FALSE;
		DWORD dwErrorCode = GetLastError();
		CString strErMsg, strError;
		bNoError = SetError(dwErrorCode, &strErMsg);


		if (bNoError != TRUE)
		{
			m_RecvCnt = 0;
			return;
		}
		return;
	}
	else
	{
		m_pTriggerSocketCall->RecivePacket(m_btRecv, nRet);
	}

	CAsyncSocket::OnReceive(nErrorCode);
}



void CTriggerSocket::OnSend( int nErrorCode )
{
	if ( nErrorCode == 0 ) 
	{
		//
	} 
	else 
	{
		m_bConnected = FALSE ;
		m_bDisConnected = TRUE ;
	}

	SendRetry() ;

	CAsyncSocket::OnSend( nErrorCode ) ;
}

int CTriggerSocket::SetError( DWORD code, CString *msg )
{
	int	nRet = -1 ;
	DWORD	dwErrorCode = code ;
	CString	strErMsg ;

	switch ( dwErrorCode ) {
	case	WSANOTINITIALISED :
		strErMsg = _T( "이 API 사용 하기 전에 AfxSocketInit 의 호출이 정상종료가 필요합니다." ) ;
		break ;
	case	WSAENETDOWN :
		strErMsg = _T( "Windows 소켓의 인플먼드가 네트워크 서브시스템의 이상을 검출 하였습니다." ) ;
		break ;
	case	WSAENOTCONN :
		strErMsg = _T( "소켓이 접속되지 않았습니다." ) ;
		break ;
	case	WSAEINPROGRESS :
		strErMsg = _T( "실행중인 Windows 소켓 호출이 블록킹 되었습니다." ) ;
		break ;
	case	WSAENOTSOCK :
		strErMsg = _T( "디스크리프터가 소켓에 없습니다." ) ;
		break ;
	case	WSAEOPNOTSUPP :
		strErMsg = _T( "MSG_OOB 지정 되었으나, SOCK_STREAM 형의 소켓이 아닙니다." ) ;
		break ;
	case	WSAESHUTDOWN :
		strErMsg = _T( "소켓은 셧다운 되었습니다. nHow 의 0 또는 2 를 넘겨서 ShutDown 을 기동한 후에는, 소켓에서 Receive 를 호출할 수 없습니다." ) ;
		break ;
	case	WSAEWOULDBLOCK :
		strErMsg = _T( "소켓은 비블록킹되어 있어서, Receive 조작이 블록킹 되었습니다." ) ;
		nRet = 0 ;
		break ;
	case	WSAEMSGSIZE :
		strErMsg = _T( "데이터그램이 이 너무 길어서, 지정한 버퍼에 다 넣을 수 없기때문에, 손실 되었습니다." ) ;
		break ;
	case	WSAEINVAL :
		strErMsg = _T( "소켓은  Bind를 사용하여 바인드 되지 않았습니다." ) ;
		break ;
	case	WSAECONNABORTED :
		strErMsg = _T( "타임 아웃 또는 그외의 장해에 의하여, 가상회로는 Abort 되었습니다." ) ;
		break ;
	case	WSAECONNRESET :
		strErMsg = _T( "리모트 측에서 가상회로가 리셋 되었습니다." ) ;
		break ;
	default :
		strErMsg.Format( _T("소켓 에러 : %lu"), ( DWORD)dwErrorCode ) ;
		break ;
	}
	if ( msg != NULL ) {
		*msg = strErMsg ;
	}
	return ( nRet ) ;
}


int CTriggerSocket::SendRetry( void )
{
	BYTE* pBuff = ( BYTE*)m_pSendBuffer ;
	int nRet = 0 ;
	int nLen = m_SendBuffCnt - m_SendCnt ;
	DWORD dwSize = m_SendBuffCnt ;
	while ( nLen > 0 ) 
	{
		nRet = this->Send( pBuff + ( dwSize - nLen ), nLen, 0 ) ;
		if ( nRet < 0 ) 
		{
			int nError = GetLastError() ;
			if ( nError == WSAEWOULDBLOCK ) {
				return ( 0 ) ;
			} else {
				return ( -1 ) ;
			}
		}
		nLen -= nRet ;
		m_SendCnt += nRet ;
	}
	m_SendFlag = FALSE ;
	return ( 0 ) ;
}


int CTriggerSocket::ClearBuffer( void )
{
	m_RecvCnt = 0 ;
	return ( 0 ) ;
}
