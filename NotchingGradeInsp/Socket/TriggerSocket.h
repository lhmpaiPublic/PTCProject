#if !defined(AFX_TRIGGERSOCKET_H__6B5E3A95_59B6_4A8D_BDBF_B125388A859E__INCLUDED_)
#define AFX_TRIGGERSOCKET_H__6B5E3A95_59B6_4A8D_BDBF_B125388A859E__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RevoxSocket.h 
//


/////////////////////////////////////////////////////////////////////////////


#include <afxsock.h>
#include <afxtempl.h>

class CTriggerSocketCall
{
public:
	virtual void RecivePacket(char* data, int len) {}
	virtual void OnConnectSocket(BOOL bConnected) {}
};

class CTriggerSocket : public CAsyncSocket
{
public:

	enum enumProtocol {
		TCP_MODE = 0,
		UDP_MODE = 1,
	} ;

public:
	CTriggerSocket( LPCTSTR lpszIpAddress, int protocol, CTriggerSocketCall* pTriggerSocketCall) ;
	virtual ~CTriggerSocket() ;

public:
	int SetError( DWORD code, CString *msg ) ;
	int ClearBuffer( void ) ;

public:
	CString m_strIpAddress ;	
	int		m_Protocol ;		
	BOOL	m_bConnected ;		
	BOOL	m_bDisConnected ;	

public:
	public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);

protected:
	enum {
		TCP_MAX_DATA = 1024
	} ;

	CTriggerSocketCall* m_pTriggerSocketCall;
protected:
	int		SendRetry( void ) ;

protected:
	BYTE*	m_pSendBuffer ;	
	int		m_SendBuffCnt ;	
	int		m_SendCnt ;		
	int		m_RecvCnt ;		
	int		m_SendFlag ;	
	char*	m_btRecv ;		

protected:
	CString m_strClientIpAddress ;		
	CString m_strClientComputerName ;	

protected:
	CRITICAL_SECTION csFunc ;

} ;


/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TRIGGERSOCKET_H__6B5E3A95_59B6_4A8D_BDBF_B125388A859E__INCLUDED_)
