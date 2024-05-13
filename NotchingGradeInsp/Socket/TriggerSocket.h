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

//생성된 클래스에서 이벤트를 받기 위한 가상 클래스
class CTriggerSocketCall
{
public:
	//패킷 수신 이벤트가 발생되고 받은 데이터를 전달한다.
	virtual void RecivePacket(char* data, int len) {}
	//Connect 호출 시 접속 처리 상태를 받는다.
	//접속 정상 : TRUE, 비정상 : FALSE
	virtual void OnConnectSocket(BOOL bConnected) {}
};

class CTriggerSocket : public CAsyncSocket
{
public:
	//소켓 프로토콜 타입
	enum enumProtocol
	{
		//TCP 모드
		TCP_MODE = 0,
		//UDP 모드
		UDP_MODE = 1,
	} ;

public:
	//생성자
	CTriggerSocket( LPCTSTR lpszIpAddress, int port, int protocol, CTriggerSocketCall* pTriggerSocketCall) ;
	//소멸자
	virtual ~CTriggerSocket() ;

public:
	//에러 체크
	int SetError( DWORD code, CString *msg ) ;
	//패킷 전송함수
	int PacketSend(char* pBuff, int length);
public:
	//IP 주소
	CString m_strIpAddress ;
	//PORT 
	int m_Port;
	//TCP/UDP 타입
	int		m_Protocol ;	
	//네트워크 염결 상태 변수
	BOOL	m_bConnected ;		
	BOOL	m_bDisConnected ;	

public:
	public:
		//이벤트 함수들
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);

protected:
	//데이터 버퍼 크기
	enum 
	{
		TCP_MAX_DATA = 1024
	} ;

	//생성한 객체 포인터
	//가상함수로 호출하여 패킷을 전달한다.
	CTriggerSocketCall* m_pTriggerSocketCall;
protected:
	//패킷을 전송하는 함수
	int		SendRetry( void ) ;

protected:
	//보낼 데이터 버퍼 
	BYTE*	m_pSendBuffer ;	
	//보낼 데이터 크기
	int		m_SendBuffCnt ;	
	//보낸 데이터 크기
	int		m_SendCnt ;		
	//받은 데이터 
	//OnRecive 함수 이벤트 시 읽어서 호출한 객체에 전달한다.
	static char m_btRecv[TCP_MAX_DATA];
protected:
	//데이터 전송 동기화 객체
	CRITICAL_SECTION m_csSendPacket ;

} ;


/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TRIGGERSOCKET_H__6B5E3A95_59B6_4A8D_BDBF_B125388A859E__INCLUDED_)
