// Ping.h: CPing クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PING_H__E6AFA61D_7EF2_4AF5_B5CD_BC4C32C7ECCD__INCLUDED_)
#define AFX_PING_H__E6AFA61D_7EF2_4AF5_B5CD_BC4C32C7ECCD__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "winsock.h"


typedef	struct {
	char	szOpponent[ 64 ] ;
	WORD	wDataSize ;
	DWORD	dwTime ;
	BYTE	byteTTL ;
	BOOL	bTimeout ;
} _PINGRESULT ;

#define	ICMP_ECHOREPLY	0
#define	ICMP_ECHOREQ	8

// IP Header -- RFC 791
typedef struct tagIPHDR
{
	BYTE	VIHL ;			// Version and IHL
	BYTE	TOS ;			// Type Of Service
	short	TotLen ;		// Total Length
	short	ID ;			// Identification
	short	FlagOff ;		// Flags and Fragment Offset
	BYTE	TTL ;			// Time To Live
	BYTE	Protocol ;		// Protocol
	WORD	Checksum ;		// Checksum
	struct	in_addr iaSrc ;	// Internet Address - Source
	struct	in_addr iaDst ;	// Internet Address - Destination
} IPHDR, *PIPHDR ;


// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
	BYTE	Type ;			// Type
	BYTE	Code ;			// Code
	WORD	Checksum ;		// Checksum
	WORD	ID ;			// Identification
	WORD	Seq ;			// Sequence
	char	Data ;			// Data
} ICMPHDR, *PICMPHDR ;


#define REQ_DATASIZE 32		// Echo Request Data size

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
	ICMPHDR icmpHdr ;
	DWORD	dwTime ;
	char	cData[ REQ_DATASIZE ] ;
} ECHOREQUEST, *PECHOREQUEST ;


// ICMP Echo Reply
typedef struct tagECHOREPLY
{
	IPHDR		ipHdr ;
	ECHOREQUEST	echoRequest ;
	char		cFiller[ 256 ] ;
} ECHOREPLY, *PECHOREPLY ;


class CPing  
{
public:
	CPing() ;
	virtual ~CPing() ;
	// inoue st 2013.11.04 (A-1)　LAN通信の断線を検出
	//int		Ping( LPCTSTR pstrHost ) ;
	int		Ping( LPCTSTR pstrHost , BOOL bTrace = TRUE ) ;
	// inoue ed 2013.11.04 (A-1) 
// NASU 2008.11.21=====(A-1)=====Start=====
	//BOOL IsConnect( void ) ;
	BOOL IsConnect( int nMode = 0 ) ;
// NASU 2008.11.21=====(A-1)=====End=====

	//
	//	関数名:	CPing::IsUsable
	//	機能:	使用可能かどうかを取得する
	//	引数:	なし
	//	関数値:	BOOL	FALSE	: 使用不可能
	//					TRUE	: 使用可能
	//	解説:
	//	使用例:
	//	備考:
	//
	BOOL	IsUsable( void ) {
		return ( m_bUsableSocket ) ;
	}

// NASU 2008.11.21=====(A-1)=====Start=====
	int SetMaxLoopCount( int nMaxLoopCount ) ;
	int SetWaitReplyTimeout( long nTimeout_sec, long nTimeout_usec ) ;
	int SetNotReceiveResponse_ReplyTimeout( BOOL bNotReceiveResponse ) ;
// NASU 2008.11.21=====(A-1)=====End=====

protected:
	int	 WaitForEchoReply( SOCKET s ) ;
	WORD InternetChecksum( WORD* pBuf, int nLen ) ;
	int SendEchoRequest( SOCKET s, LPSOCKADDR_IN lpsocketaddr ) ;
	DWORD RecvEchoReply( SOCKET s, LPSOCKADDR_IN lpsaFrom, BYTE* pTTL ) ;

	enum {
		enMajorVer = 1,							// Winsockメジャーバージョン
		enMinorVer = 1,							// Winsockマイナーバージョン
	} ;
	enum {
		enPingLoopCount = 4,					// PINGの回数
	} ;

private:
	CString	m_strError ;						// エラーメッセージ(CString)x1
	_PINGRESULT	m_Result[ enPingLoopCount ] ;	// PINGの結果([インデックス番号])(_PINGRESULT)xenPingLoopCount
	BOOL	m_bUsableSocket ;					// ソケット使用可能(FALSE:使用不可能/TRUE:使用可能)(BOOL)x1
// NASU 2008.11.21=====(A-1)=====Start=====
	int m_nMaxLoopCount ;						//!< 最大ループ回数[回](1～enPingLoopCount(4))(int)x1
	long m_nTimeout_sec ;						//!< タイムアウト時間[sec](long)x1
	long m_nTimeout_usec ;						//!< タイムアウト時間[usec](long)x1
	BOOL m_bNotReceiveResponse_ReplyTimeout ;	//!< タイムアウト発生時に応答受信処理を行わないかどうか(FALSE:行う/TRUE:行わない)(BOOL)x1
// NASU 2008.11.21=====(A-1)=====End=====
};


#endif // !defined(AFX_PING_H__E6AFA61D_7EF2_4AF5_B5CD_BC4C32C7ECCD__INCLUDED_)
