// Ping.cpp: CPing クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "mpcomm.h"
#include "pch.h"
#include "Ping.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

//
//	関数名:	CPing::CPing
//	機能:	コンストラクタ
//	引数:	なし
//	関数値:	なし
//	解説:
//	使用例:
//	備考:
//
CPing::CPing()
{
	WSADATA	wsaData ;
	WORD	wVersionRequested = MAKEWORD( enMajorVer, enMinorVer ) ;
	int	nRet ;
	// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
	static BOOL bInit = FALSE ;
	// inoue ed 2013.11.04 (A-1) 
	nRet = WSAStartup( wVersionRequested, &wsaData ) ;
	if ( nRet ) {
		m_bUsableSocket = FALSE ;
		int	nError = ::WSAGetLastError() ;
		TRACE( "WSAStartup error %d\r\n", nError ) ;
	} else {
		m_bUsableSocket = TRUE ;
	}

	if ( wsaData.wVersion != wVersionRequested ) {
		m_bUsableSocket = FALSE ;
		TRACE( "Winsock version %d not supported\r\n", wsaData.wVersion ) ;
	} else {
		// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
		//TRACE( "Winsock version %x \r\n", wsaData.wVersion ) ;
		if( bInit = FALSE ){
			TRACE( "Winsock version %x \r\n", wsaData.wVersion ) ;
		}
		bInit = TRUE ;
		// inoue ed 2013.11.04 (A-1) 
	}

	m_strError = _T( "" ) ;
	memset( m_Result, 0x00, sizeof( m_Result ) ) ;
	int	nLoop ;
	for ( nLoop = 0 ; nLoop < enPingLoopCount ; nLoop++ ) {
		m_Result[ nLoop ].bTimeout = TRUE ;
	}
// NASU 2008.11.21=====(A-1)=====Start=====
	m_nMaxLoopCount = enPingLoopCount ;
	m_nTimeout_sec = 5L ;
	m_nTimeout_usec = 0L ;
	m_bNotReceiveResponse_ReplyTimeout = FALSE ;
// NASU 2008.11.21=====(A-1)=====End=====
}


//
//	関数名:	CPing::~CPing
//	機能:	デストラクタ
//	引数:	なし
//	関数値:	なし
//	解説:
//	使用例:
//	備考:
//
CPing::~CPing()
{
	if ( m_bUsableSocket == TRUE ) {
		WSACleanup() ;
	}
	m_bUsableSocket = FALSE ;
}


//
//	関数名:	CPing::Ping
//	機能:	PINGを実行する
//	引数:	LPCTSTR	pstrHost	: ホスト名orＩＰアドレス
//	関数値:	int	0	: 正常
//				-1	: 異常
//	解説:
//	使用例:
//	備考:
//
// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
//int		Ping( LPCTSTR pstrHost ) ;
int	CPing::Ping( LPCTSTR pstrHost , BOOL bTrace/*=TRUE*/) 
// inoue ed 2013.11.04 (A-1) 
{
	m_strError = _T( "" ) ;
	SOCKET	rawSocket ;
	LPHOSTENT	lpHost ;
	struct	sockaddr_in	saDest ;
	struct	sockaddr_in	saSrc ;
	DWORD	dwTimeSent ;
	DWORD	dwElapsed ;
	BYTE	byteTTL ;
	int	nLoop, nRet, r ;

	// 未入力の値を使用することになるので、暫定で入れる。
	dwTimeSent = ::GetTickCount() ;
	byteTTL = 0 ;
	memset( &saSrc , 0x00 , sizeof( saSrc ) ) ;

// make raw socket
	rawSocket = ::socket( AF_INET, SOCK_RAW, IPPROTO_ICMP ) ;
	if ( rawSocket == SOCKET_ERROR ) {
		m_strError = "socket error " ;
		return ( -1 ) ;
	}

// seek host
	lpHost = ::gethostbyname( pstrHost ) ;
	if ( lpHost == NULL ) {
		m_strError.Format( _T("host not fount [%s]"), pstrHost ) ;
		r = closesocket( rawSocket ) ; //Ver3.15.0.0 2016/10/04 sugimoto コードチェッカーのエラー検出部修正
		return ( -1 ) ;
	}

// set socket address
	saDest.sin_addr.s_addr = *( ( DWORD FAR *)( lpHost->h_addr ) ) ;
	saDest.sin_family = AF_INET ;
	saDest.sin_port = 0 ;

	nRet = 0 ;
	try {
		memset( m_Result, 0x00, sizeof( m_Result ) ) ;
	// NASU 2008.11.21=====(A-1)=====Start=====
		//for ( nLoop = 0 ; nLoop < enPingLoopCount ; nLoop++ ) {
		for ( nLoop = 0 ; nLoop < m_nMaxLoopCount ; nLoop++ ) {
	// NASU 2008.11.21=====(A-1)=====End=====
			m_Result[ nLoop ].bTimeout = TRUE ;
		}
		// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
		if( bTrace == TRUE ){
		// inoue ed 2013.11.04 (A-1) 
			TRACE( "Pinging %s with %d bytes of data:\r\n", pstrHost ) ;
		// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
		}
		// inoue ed 2013.11.04 (A-1) 
	// NASU 2008.11.21=====(A-1)=====Start=====
		//for ( nLoop = 0 ; nLoop < enPingLoopCount ; nLoop++ ) {
		for ( nLoop = 0 ; nLoop < m_nMaxLoopCount ; nLoop++ ) {
	// NASU 2008.11.21=====(A-1)=====End=====
		// send icmp echo request
			SendEchoRequest( rawSocket, &saDest ) ;

		// wait for data receive by using select()
			r = WaitForEchoReply( rawSocket ) ;

			if ( r == SOCKET_ERROR ) {
				m_strError.Format( _T("socket error loop :%d"), nLoop ) ;
				throw -1 ;
			}
			m_Result[ nLoop ].bTimeout = ( !r ) ;

		// NASU 2008.11.21=====(A-1)=====Start=====
			if ( m_Result[ nLoop ].bTimeout != FALSE ) {
			// -----タイムアウト発生-----
				if ( m_bNotReceiveResponse_ReplyTimeout == FALSE ) {
				// -----タイムアウト発生時に応答受信処理を行う-----
				// calculate time
					//dwElapsed = ::GetTickCount() - dwTimeSent ;

					//sprintf( m_Result[ nLoop ].szOpponent, "%s", inet_ntoa( saSrc.sin_addr ) ) ;
					//m_Result[ nLoop ].wDataSize = REQ_DATASIZE ;
					//m_Result[ nLoop ].dwTime = dwElapsed ;
					//m_Result[ nLoop ].byteTTL = byteTTL ;

					TRACE( "LOOP=%d Request timed out\r\n"
						, nLoop
						) ;
					continue ;
				}
			}
		// NASU 2008.11.21=====(A-1)=====End=====
		// receive response
			dwTimeSent = RecvEchoReply( rawSocket, &saSrc, &byteTTL ) ;

		// calculate time
			dwElapsed = ::GetTickCount() - dwTimeSent ;

		/*	sprintf( m_Result[ nLoop ].szOpponent, "%s", inet_ntoa( saSrc.sin_addr ) ) ;*/
			m_Result[ nLoop ].wDataSize = REQ_DATASIZE ;
			m_Result[ nLoop ].dwTime = dwElapsed ;
			m_Result[ nLoop ].byteTTL = byteTTL ;
			// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
			if( bTrace == TRUE ){
			// inoue ed 2013.11.04 (A-1) 

				TRACE( "LOOP=%d Reply from %s: bytes=%d time=%d TTL=%d\r\n"
					, nLoop
					, m_Result[ nLoop ].szOpponent
					, m_Result[ nLoop ].wDataSize
					, m_Result[ nLoop ].dwTime
					, m_Result[ nLoop ].byteTTL
					) ;
			// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
			}
			// inoue ed 2013.11.04 (A-1) 

		}
	} catch ( ... ) {
		nRet = -1 ;
	}

	r = closesocket( rawSocket ) ;
	if ( r == SOCKET_ERROR ) {
		nRet = -1 ;
	}

	return ( nRet ) ;
}


//
//	関数名:	CPing::SendEchoRequest
//	機能:	エコー要求ヘッダに情報を設定し、宛先に送信する
//	引数:	SOCKET			s			: Windowsソケットオブジェクトへのハンドル
//			LPSOCKADDR_IN	lpstToAddr	: 送信先アドレス情報
//	関数値:	int		SOCKET_ERROR	: ソケットエラー
//					その他			: 受信データ
//	解説:
//	使用例:
//	備考:
//
int CPing::SendEchoRequest( SOCKET s, LPSOCKADDR_IN lpstToAddr ) 
{
	static	ECHOREQUEST	echoReq ;
// inoue 2012.05.30 ---st---
//	static	nId = 1 ;
//	static	nSeq = 1 ;
	static	WORD nId = 1 ;		// 20120530 dai-kit: ビルドエラー回避
	static	WORD nSeq = 1 ;		// 20120530 dai-kit: ビルドエラー回避
// inoue 2012.05.30 ---ed---
	int	nRet ;

// エコー要求に情報を設定
	echoReq.icmpHdr.Type = ICMP_ECHOREQ ;
	echoReq.icmpHdr.Code = 0 ;
	echoReq.icmpHdr.Checksum = 0 ;
	echoReq.icmpHdr.ID = nId++ ;
	echoReq.icmpHdr.Seq = nSeq++ ;

// set send data
	for ( nRet = 0 ; nRet < REQ_DATASIZE ; nRet++ ) {
		echoReq.cData[ nRet ] = ' ' + nRet ;
	}

// set current tickcount
	echoReq.dwTime = ::GetTickCount() ;

// insert data to packet and calculate checksum
	echoReq.icmpHdr.Checksum = InternetChecksum( ( WORD *)&echoReq, sizeof( ECHOREQUEST ) ) ;

// send echo request
	nRet = sendto( s, ( LPSTR)&echoReq, sizeof( ECHOREQUEST ), 0, ( LPSOCKADDR)lpstToAddr, sizeof( SOCKADDR_IN ) ) ;
	if ( nRet == SOCKET_ERROR ) {
		int	nError = WSAGetLastError() ;
		TRACE( "sendto error %d\r\n", nError ) ;
	}

	return ( nRet ) ;
}


//
//	関数名:	CPing::RecvEchoReply
//	機能:	着信データを受信して、フィールド別に解析する
//	引数:	SOCKET			s			: Windowsソケットオブジェクトへのハンドル
//			LPSOCKADDR_IN	lpsaFrom	: 送信元アドレス情報格納域
//			BYTE*			pTTL		: 
//	関数値:	int		SOCKET_ERROR	: ソケットエラー
//					その他			: 時間[]
//	解説:
//	使用例:
//	備考:
//
DWORD CPing::RecvEchoReply( SOCKET s, LPSOCKADDR_IN lpsaFrom, BYTE* pTTL ) 
{
	ECHOREPLY	echoReply ;
	int	nRet ;
	int	nAddrLen = sizeof( struct sockaddr_in ) ;

// receive echo response
	nRet = recvfrom( s, ( LPSTR )&echoReply, sizeof( ECHOREPLY ), 0, ( LPSOCKADDR)lpsaFrom, &nAddrLen ) ;
	if ( nRet == SOCKET_ERROR ) {

	}
	*pTTL = echoReply.ipHdr.TTL ;
	return ( echoReply.echoRequest.dwTime ) ;
}


//
//	関数名:	CPing::WaitForEchoReply
//	機能:	select()を使用して、データが読み取り待機中かどうかを判別する
//	引数:	SOCKET			s			: Windowsソケットオブジェクトへのハンドル
//	関数値:	int	0				: タイムアウト
//				SOCKET_ERROR	: エラー
//	解説:
//	使用例:
//	備考:
//
int CPing::WaitForEchoReply( SOCKET s ) 
{
	struct	timeval	Timeout ;
	fd_set	readfds ;

	readfds.fd_count = 1 ;
	readfds.fd_array[ 0 ] = s ;
// NASU 2008.11.21=====(A-1)=====Start=====
	//Timeout.tv_sec = 5 ;
	//Timeout.tv_usec = 0 ;
	Timeout.tv_sec = m_nTimeout_sec ;
	Timeout.tv_usec = m_nTimeout_usec ;
// NASU 2008.11.21=====(A-1)=====End=====

	return ( select( 1, &readfds, NULL, NULL, &Timeout ) ) ;
}


//
//	関数名:	CPing::InternetChecksum
//	機能:	全16ビットワードの合計の補数
//	引数:	WORD	*pBuf	: データ配列へのポインタ
//			int		nLen	: データ長（バイト数）
//	関数値:	WORD	: 全16ビットワードの合計の補数
//	解説:
//	使用例:
//	備考:
//
WORD CPing::InternetChecksum( WORD* pBuf, int nLen ) 
{
	if ( nLen > 0 ) {
		ASSERT( pBuf != NULL ) ;
	}
	int	nSum = 0 ;
	while ( nLen > 0 ) {
		nSum += ( *pBuf++ ) ;
		nLen -= 2 ;
	}
	nSum = ( nSum >> 16 ) + ( nSum & 0xffff ) ;
	nSum += ( nSum >> 16 ) ;
	return ( ~nSum ) ;
}


// NASU 2008.11.21=====(A-1)=====Start=====
////
////	関数名:	CPing::IsConnect
////	機能:	pingの結果ホストが接続されているか調べます
////	引数:	なし
////	関数値:	BOOL	FALSE	: 接続されていません
////					TRUE	: 接続されています
////	解説:
////	使用例:
////	備考:
////
//BOOL CPing::IsConnect()
//{
//	for ( int i = 0 ; i < enPingLoopCount ; i++ ) {
//		if ( m_Result[ i ].bTimeout == TRUE ) {
//			return ( FALSE ) ;
//		}
//	}
//	return ( TRUE ) ;
//}
//!
//!			関数名:	CPing::IsConnect
//!	@brief	機能:	pingの結果ホストが接続されているか調べます
//!	@param	引数:	int		nMode = 0	: チェックするモード(0:１回でも応答がなければ非接続とする/1:１回でも応答があれば接続とする)
//!	@return	関数値:	BOOL	FALSE	: 接続されていません
//!	@return					TRUE	: 接続されています
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
BOOL CPing::IsConnect( int nMode /*= 0*/ )
{
	if ( m_nMaxLoopCount <= 0 ) {
		return ( FALSE ) ;
	}
	BOOL	bRet = FALSE ;
	if ( nMode == 1 ) {
	// -----１回でも応答があれば接続とする-----
		bRet = FALSE ;
		for ( int i = 0 ; i < m_nMaxLoopCount ; i++ ) {
			if ( m_Result[ i ].bTimeout == FALSE ) {
			// -----タイムアウト発生ではない-----
				bRet = TRUE ;
				break ;
			}
		}
	} else {
	// -----１回でも応答がなければ非接続とする-----
		bRet = TRUE ;
		for ( int i = 0 ; i < m_nMaxLoopCount ; i++ ) {
			if ( m_Result[ i ].bTimeout == TRUE ) {
			// -----タイムアウト発生-----
				bRet = FALSE ;
				break ;
			}
		}
	}
	return ( bRet ) ;
}
// NASU 2008.11.21=====(A-1)=====End=====


// NASU 2008.11.21=====(A-1)=====Start=====
//!
//!			関数名:	CPing::SetMaxLoopCount
//!	@brief	機能:	pingのループ回数を指定します
//!	@param	引数:	int		nMaxLoopCount	: pingのループ回数(1～enPingLoopCount(4))
//!	@return	関数値:	int	0	: 正常
//!	@return				1	: 異常
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CPing::SetMaxLoopCount( int nMaxLoopCount )
{
	int	nRet = 0 ;
	if ( ( nMaxLoopCount > 0 ) && ( nMaxLoopCount <= enPingLoopCount ) ) {
		m_nMaxLoopCount = nMaxLoopCount ;
	} else {
		nRet = -1 ;
	}
	return ( nRet ) ;
}


//!
//!			関数名:	CPing::SetWaitReplyTimeout
//!	@brief	機能:	pingのタイムアウト時間[sec][usec]を指定します
//!	@param	引数:	int		nTimeout_sec	: pingのタイムアウト時間[sec]
//!	@param			int		nTimeout_usec	: pingのタイムアウト時間[usec]
//!	@return	関数値:	int	0	: 正常
//!	@return				1	: 異常
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CPing::SetWaitReplyTimeout( long nTimeout_sec, long nTimeout_usec )
{
	int	nRet = 0 ;
	if ( nTimeout_sec < 0 ) {
		nRet = -1 ;
		return ( nRet ) ;
	}
	if ( nTimeout_usec < 0 ) {
		nRet = -1 ;
		return ( nRet ) ;
	}
	m_nTimeout_sec = nTimeout_sec ;
	m_nTimeout_usec = nTimeout_usec ;
	return ( nRet ) ;
}


//!
//!			関数名:	CPing::SetNotReceiveResponse_ReplyTimeout
//!	@brief	機能:	pingのタイムアウト発生時に応答受信処理を行わないかどうかを指定する
//!	@param	引数:	BOOL	bNotReceiveResponse	: タイムアウト発生時に応答受信処理を行わないかどうか(FALSE:行う/TRUE:行わない)
//!	@return	関数値:	int	0	: 正常
//!	@return				1	: 異常
//!	@date	更新:
//!	@note	解説:
//!	@note	使用例:
//!	@note	備考:
//!
int CPing::SetNotReceiveResponse_ReplyTimeout( BOOL bNotReceiveResponse )
{
	int	nRet = 0 ;
	m_bNotReceiveResponse_ReplyTimeout = bNotReceiveResponse ;
	return ( nRet ) ;
}
// NASU 2008.11.21=====(A-1)=====End=====
