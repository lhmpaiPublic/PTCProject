// RevoxSocket.cpp : インプリメンテーション ファイル
//


//#include "stdafx.h"
//#include "mpcomm.h"
//#include "LDA2Socket.h"
#include "pch.h"
#include "LightSocket.h"
#include "TimeAnalyzer.h"
#include "GlobalData.h"
//#include "GlobalDef.h"
#include "WinTool.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//!
//!			関数名:	CLightSocket::CLightSocket
//!	@brief	機能:	コンストラクタ
//!	@param	引数:	LPCTSTR		lpszIpAddress = _T( "" )	: サーバＩＰアドレス
//!	@param			int			protocol
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
CLightSocket::CLightSocket( LPCTSTR lpszIpAddress, int protocol )
{

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

	if ( m_btRecv == NULL ) {
		m_btRecv = new char[ TCP_MAX_DATA ] ;
	}
}


//!
//!			関数名:	CLightSocket::~CLightSocket
//!	@brief	機能:	デストラクタ
//!	@param	引数:	なし
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
CLightSocket::~CLightSocket()
{
	if ( m_btRecv != NULL ) {
		delete [] m_btRecv ;
		m_btRecv = NULL ;
	}

	m_bConnected = FALSE ;
	m_bDisConnected = TRUE ;

	::DeleteCriticalSection( &csFunc ) ;
}


//!
//!			関数名:	CLightSocket::OnAccept
//!	@brief	機能:	Accept メンバ関数を呼び出して、待機中の接続要求を受け入れ可能であることをリスナ ソケットに通知するためにフレームワークが呼び出します。
//!	@param	引数:	int	nErrorCode	: ソケットでの直前のエラー
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
void CLightSocket::OnAccept( int nErrorCode )
{
	CAsyncSocket::OnAccept( nErrorCode ) ;
}


//!
//!			関数名:	CLightSocket::OnClose
//!	@brief	機能:	接続されていたソケットがそのプロセスにより閉じられたことをソケットに通知するために、フレームワークが呼び出します。
//!	@param	引数:	int	nErrorCode	: ソケットでの直前のエラー
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
void CLightSocket::OnClose( int nErrorCode )
{
	m_bConnected = FALSE ;
	m_bDisConnected = TRUE ;

	CAsyncSocket::OnClose( nErrorCode ) ;
	Close() ;
}


//!
//!			関数名:	CLightSocket::OnConnect
//!	@brief	機能:	接続に成功したか、エラーになったかにはかかわらず、接続の試みが完了したことを接続しようとしているソケットに通知するために、フレームワークが呼び出します。
//!	@param	引数:	int	nErrorCode	: ソケットでの直前のエラー
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
void CLightSocket::OnConnect( int nErrorCode )
{
	if ( nErrorCode == 0 ) {
	// -----正常-----
		m_bConnected = TRUE ;
		m_bDisConnected = FALSE ;
	} else {
	// -----異常-----
		m_bConnected = FALSE ;
		m_bDisConnected = TRUE ;
	}

	CAsyncSocket::OnConnect( nErrorCode ) ;
}


//!
//!			関数名:	CLightSocket::OnReceive
//!	@brief	機能:	Receive メンバ関数を呼び出して取得できるデータがバッファにあることをソケットに通知するために、フレームワークが呼び出します。
//!	@param	引数:	int	nErrorCode	: ソケットでの直前のエラー
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
void CLightSocket::OnReceive( int nErrorCode )
{
	if ( nErrorCode == 0 ) {
	// -----正常-----
		//
	} else {
	// -----異常-----
		m_bConnected = FALSE ;
		m_bDisConnected = TRUE ;
	}
	CTime ctStart = CTime::GetCurrentTime() ;
	CTime ctEnd ;
	CTimeSpan ctsDiff ;
	{
		::EnterCriticalSection( &csFunc ) ;

		int nRet = Receive( &m_btRecv[ m_RecvCnt ], TCP_MAX_DATA - m_RecvCnt ) ;
		if ( nRet < 0 ) {
			BOOL bNoError = FALSE ;
			DWORD dwErrorCode = GetLastError() ;
			CString strErMsg , strError ;
			bNoError = SetError( dwErrorCode, &strErMsg ) ;

			AprData.SaveDebugLog_format(_T("address<%s> ErrorMsg<%s>"), this->m_strIpAddress, strErMsg);

			if ( bNoError != TRUE ) {
				// クリア
				m_RecvCnt = 0 ;
				::LeaveCriticalSection( &csFunc ) ;
				return ;
			}
			::LeaveCriticalSection( &csFunc ) ;// inoue ad 2015.11.05 (A-1)  排他処理を追加
			return ;
		} else {
			m_RecvCnt += nRet ;
		}

		::LeaveCriticalSection( &csFunc ) ;
	}

	CAsyncSocket::OnReceive( nErrorCode ) ;
}


//!
//!			関数名:	CLightSocket::OnSend
//!	@brief	機能:	Send メンバ関数を呼び出して現在データを送信できることをソケットに通知するために、フレームワークが呼び出します。
//!	@param	引数:	int	nErrorCode	: ソケットでの直前のエラー
//!	@return	関数値:	なし
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
void CLightSocket::OnSend( int nErrorCode )
{
	if ( nErrorCode == 0 ) {
	// -----正常-----
		//
	} else {
	// -----異常-----
		m_bConnected = FALSE ;
		m_bDisConnected = TRUE ;
	}

	SendRetry() ;

	CAsyncSocket::OnSend( nErrorCode ) ;
}


//!
//!			関数名:	CLightSocket::PacketSend
//!	@brief	機能:	パケットを送信する
//!	@param	引数:	char* pBuff		: 送信バッファへのポインタ
//!	@param			int length		: 送信バッファの長さ
//!	@param			long lTimeOver	: タイムオーバー時間[msec]
//!	@return	関数値:	int	0	: 正常
//!	@return				-1	: 異常
//!	@return				-2	: 非接続状態
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CLightSocket::PacketSend( char* pBuff, int length, long lTimeOver )
{

	if ( m_Protocol == TCP_MODE ) {
		if ( m_bConnected != TRUE ) {
		// -----非接続状態-----
			return ( -2 ) ;
		}
	}

	if ( m_pSendBuffer != NULL ) {
		return ( -1 ) ;
	}

	m_SendBuffCnt = length + 3 ;
	m_pSendBuffer = new BYTE[ m_SendBuffCnt + 1 ] ;

	// inoue st 2015.03.23 (A-1)　64bitでメモリアクセス異常が出るため修正
	//memset( m_pSendBuffer , 0x00 , sizeof( m_pSendBuffer ) ) ;
	memset( m_pSendBuffer , 0x00 , sizeof( BYTE ) * ( m_SendBuffCnt + 1 )) ;
	// inoue ed 2015.03.23 (A-1)
	m_SendCnt = 0 ;

	BYTE* ptr = m_pSendBuffer ;
	int	c ;
	*ptr++ = enSTX ;	// [STX]
	for ( c = 0 ; c < length ; c++ ) {
		*ptr++ = pBuff[ c ] ;
	}
// 21.07.05 Ahn Modify Start
//	*ptr++ = enETX ;	// [ETX]
	*ptr++ = enCR;
	*ptr++ = enLF;
// 21.07.05 Ahn Modify End

	m_SendFlag = TRUE ;
	SendRetry() ;

	int ret = 0 ;
	CTimeAnalyzer ta ;
	ta.StopWatchStart() ;

	while ( m_SendFlag ) {
		::PumpMessages() ;
		::PumpMessages( NULL, NULL, WM_SOCKET_NOTIFY, WM_SOCKET_DEAD ) ;
		::Sleep( 1 ) ;
		if ( ta.WhatTimeIsIt() > lTimeOver ) {
			ret = -1 ;
			break ;
		}
	}

	if ( m_pSendBuffer ) {
		delete [] m_pSendBuffer ;
		m_pSendBuffer = NULL ;
	}
	m_SendBuffCnt = 0 ;
	m_SendCnt = 0 ;

	return ( ret ) ;
}


//!
//!			関数名:	CLightSocket::DataRecv
//!	@brief	機能:	データを受信する
//!	@param	引数:	char* pBuff		: 受信バッファへのポインタ
//!	@param			int length		: 受信バッファの長さ
//!	@param			long lTimeOver	: タイムオーバー時間[msec]
//!	@return	関数値:	int	0	: 正常
//!	@return				-1	: 異常
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CLightSocket::DataRecv( char* pBuff, int length, long lTimeOver )
{
	CTimeAnalyzer ta ;
	ta.StopWatchStart() ;

	char buff_temp[ 256 ] ;
	memset( buff_temp, 0x00, sizeof( buff_temp ) ) ;

	int n = 0 ;
	while ( 1 ) {
		if ( m_Protocol == TCP_MODE ) {
			if ( m_bDisConnected == TRUE ) {
				return ( -1 ) ;
			}
		}

		{
			::EnterCriticalSection( &csFunc ) ;

//			if ( length == m_RecvCnt ) 
		// 21.07.05 Ahn Modify Start
		//	if ( m_btRecv[ m_RecvCnt - 1 ] == enETX )
			if ( (m_btRecv[m_RecvCnt - 3] == enCR ) && ( m_btRecv[ m_RecvCnt - 2] == enLF) )
		// 21.07.05 Ahn Modify End
			{
			// -----受信完了-----
				m_btRecv[ m_RecvCnt ] = 0x00 ;
				// 21.08.23 Son Mod Start
				// 중간에 NULL값이 있는 경우 이후의 값을 정상적으로 읽어오지 않음. 예) CH이나 Bright 값이 0이 중간에 들어갈 경우 
				//	strcpy( buff_temp, m_btRecv ) ;
				for (int i = 0; i < m_RecvCnt; i++)
				{
					buff_temp[i] = m_btRecv[i];
				}
				// 21.08.23 Son Mod End
				m_RecvCnt = 0 ;
				::LeaveCriticalSection( &csFunc ) ;
				break ;
			}

			::LeaveCriticalSection( &csFunc ) ;
		}

		::PumpMessages() ;
		::PumpMessages( NULL, NULL, WM_SOCKET_NOTIFY, WM_SOCKET_DEAD ) ;

		if ( ta.WhatTimeIsIt() > lTimeOver ) {
			return ( -1 ) ;
		}

		n++ ;
	}

	// コマンドの先頭ACK(0x06)と末尾ETX(0x03)を削除
	// 応答コマンドの先頭は、必ずACK(0x06)で始まり、末尾はETX(0x03)までとします。
	if ( buff_temp[ 0 ] != enACK )
	{
		return ( -1 ) ;
	}
/*
	if ( buff_temp[ length - 1 ] != enETX )
	{
		CString strError ;
		strError.Format( _T("Format Error ETX: %s") , buff_temp ) ;
		SAVE_ERROR_TEXT( strError ) ;
		return ( -1 ) ;
	}
*/
	// コピー
	for ( int i = 1 ; i < length ; i++ )
	{
		pBuff[ i - 1 ] = buff_temp[ i ] ;
	}

	return ( 0 ) ;
}


//!
//!			関数名:	CLightSocket::SetError
//!	@brief	機能:	エラーコードからエラー内容文字列を取得する
//!	@param	引数:	DWORD	code	: エラーコード
//!	@param			CString	*msg	: エラー内容文字列格納域へのポインタ(NULL:格納しない)
//!	@return	関数値:	int	0	: 正常
//!	@return				-1	: 異常
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CLightSocket::SetError( DWORD code, CString *msg )
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


//!
//!			関数名:	CLightSocket::SendRetry
//!	@brief	機能:	パケット送信待ち
//!	@param	引数:	なし
//!	@return	関数値:	int	0	: 正常
//!	@return				-1	: 異常
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CLightSocket::SendRetry( void )
{
	BYTE* pBuff = ( BYTE*)m_pSendBuffer ;
	int nRet = 0 ;
	int nLen = m_SendBuffCnt - m_SendCnt ;
	DWORD dwSize = m_SendBuffCnt ;
	while ( nLen > 0 ) {
		nRet = this->Send( pBuff + ( dwSize - nLen ), nLen, 0 ) ;
		if ( nRet < 0 ) {
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


//!
//!			関数名:	CLightSocket::ClearBuffer
//!	@brief	機能:	受信データ数をクリアする
//!	@param	引数:	なし
//!	@return	関数値:	int	0	: 正常
//!	@return				-1	: 異常
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CLightSocket::ClearBuffer( void )
{
	m_RecvCnt = 0 ;
	return ( 0 ) ;
}
