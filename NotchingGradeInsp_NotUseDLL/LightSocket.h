#if !defined(AFX_RevoxSocket_H__6B5E3A95_59B6_4A8D_BDBF_B125388A859E__INCLUDED_)
#define AFX_RevoxSocket_H__6B5E3A95_59B6_4A8D_BDBF_B125388A859E__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RevoxSocket.h : ヘッダー ファイル
//


/////////////////////////////////////////////////////////////////////////////
// CLightSocket コマンド ターゲット


#include <afxsock.h>
#include <afxtempl.h>


//#define WM_SOCKET_NOTIFY  0x0373
//#define WM_SOCKET_DEAD    0x0374


class CLightSocket : public CAsyncSocket
{
public:
	enum enumProtocol {
		TCP_MODE = 0,
		UDP_MODE = 1,
	} ;

	enum {
		enSTX = 0x4C ,	// STX コマンドの 先頭
		enETX = 0x03 ,	// ETX コマンド/応答コマンドの 末尾
		enCR =  0x0D ,	// CR 
		enLF =  0x0A ,	// LF 
		enACK = 0x4C ,	// ACK 応答コマンドの 先頭
	} ;

public:
	CLightSocket( LPCTSTR lpszIpAddress, int protocol ) ;
	virtual ~CLightSocket() ;

public:
	int PacketSend( char* pBuff, int length, long lTimeOver ) ;
	int DataRecv( char* pBuff, int length, long lTimeOver ) ;
	int SetError( DWORD code, CString *msg ) ;
	int ClearBuffer( void ) ;

public:
	CString m_strIpAddress ;	//!< サーバＩＰアドレス(CString)x1
	int		m_Protocol ;		//!< 通信プロトコル(enumProtocol)(int)x1
	BOOL	m_bConnected ;		//!< 接続中フラグ(FALSE:非接続中/TRUE:接続中)(BOOL)x1
	BOOL	m_bDisConnected ;	//!< 接続を切断されたかどうかのフラグ(FALSE:通常/TRUE:切断された)(BOOL)x1

// オーバーライド
public:
	// ClassWizard は仮想関数のオーバーライドを生成します
	//{{AFX_VIRTUAL(CLightSocket)
	public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

protected:
	enum {
		TCP_MAX_DATA = 64,	//!< パケット最大サイズ[Bytes]
	} ;

protected:
	int		SendRetry( void ) ;

protected:
	BYTE*	m_pSendBuffer ;	//!< 送信バッファ(BYTE*)x1
	int		m_SendBuffCnt ;	//!< 送信バッファサイズ(int)x1
	int		m_SendCnt ;		//!< 送信済みバッファ(int)x1
	int		m_RecvCnt ;		//!< 受信中データバイト数(int)x1
	int		m_SendFlag ;	//!<
	char*	m_btRecv ;		//!<

protected:
	CString m_strClientIpAddress ;		//!< ＩＰアドレス(CString)x1
	CString m_strClientComputerName ;	//!< コンピュータ名(CString)x1

protected:
	CRITICAL_SECTION csFunc ;

} ;


/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_RevoxSocket_H__6B5E3A95_59B6_4A8D_BDBF_B125388A859E__INCLUDED_)
