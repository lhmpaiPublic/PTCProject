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
	WORD	m_wChannelNo;	// ä�ι�ȣ
	WORD	m_wMaxPort;		// �ִ� ��Ʈ��
	WORD	m_wMyStNo;		// �ڱ���ȣ
	WORD	m_wExtStNo;		// Ÿ����ȣ
	WORD	m_wSeqStNo;		// ������ ��ȣ
	WORD	m_wOffset_In;	//!< �Է� ����̽� ��ȣ ������(WORD)x1
	WORD	m_wOffset_Out;	//!< ��� ����̽� ��ȣ ������(WORD)x1
	long	m_pPath;		// ���µ� ȸ���� �н� ������

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

