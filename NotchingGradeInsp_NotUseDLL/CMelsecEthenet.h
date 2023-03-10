#pragma once
#include "CMelsecBase.h"
class CMelsecEthernetSock;

class CMelsecEthernet : public CMelsecBase
{
public:
	CMelsecEthernet(WORD wChannelNo = CHN_NO_NETG1, WORD wMaxPort = 16, WORD wMyStNo = 1, WORD wExtStNo = 1, WORD wSeqStNo = 0, WORD wOffset_In = 0, WORD wOffset_Out = 0);
	virtual ~CMelsecEthernet();

	int InPort(WORD port, BYTE* data, BOOL bExtSt = FALSE) ;
	int OutPort(WORD port, BYTE data, BOOL bLockCtrl = TRUE, BOOL bGetBit = FALSE);
	int WriteDataReg(int offset, short data[], int num);
	int ReadDataReg(int offset, short data[], int num) ;

	int Out_Port_Bit(int nPort, BYTE bBitPos, int nMode) ;
	//int In_Port_Bit(WORD port, BYTE* data, BOOL bExtSt = FALSE);

	int WriteAlarmCode(int offset);


	int ReadPortAllBitIn() ;
	int ReadPortAllBitOut();


	enum {
		MELSEC_UDP_PORT = 4001,
		MELSEC_TCP_PORT = 4000,
	};

	static CCriticalSection	m_csMelsec;	//!< (CCriticalSection)x1

private :
	WORD	m_wChannelNo;	// 채널번호
	WORD	m_wMaxPort;		// 최대 포트수
	WORD	m_wMyStNo;		// 자국번호
	WORD	m_wExtStNo;		// 타국번호
	WORD	m_wSeqStNo;		// 시퀀스 번호
	WORD	m_wOffset_In;	//!< 입력 디바이스 번호 오프셋(WORD)x1
	WORD	m_wOffset_Out;	//!< 출력 디바이스 번호 오프셋(WORD)x1
	long	m_pPath;		// 오픈된 회선의 패스 포인터

public:
	static CCriticalSection* GetCriticalSection(void)
	{
		return (&m_csMelsec);
	}

private:

	CMelsecEthernetSock* m_pEthernetSock;

	int OpenPio(void) ;
	int ClosePio(void) ;

	CString m_strIpAddress;
	int		m_nProtocol;

};

