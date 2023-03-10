#pragma once
#include <afxsock.h>
#include <afxtempl.h>

class CMelsecEthernetSock : public CAsyncSocket
{
public:
	enum enumProtocol {
		TCP_MODE = 0,
		UDP_MODE = 1,
	};

	enum {
		enDLE = 0x10,
		enSTX = 0x4C,	// STX 
		enETX = 0x03,	// ETX 
		enCR = 0x0D,	// CR 
		enLF = 0x0A,	// LF 
		enACK = 0x4C,	// ACK 
	};

	enum {
		// Header
		en_SubHeaderL	= 0x50,
		en_SubHeaderH	= 0x00,
		en_NetworkNo	= 0x00,		// 00, // 22.03.21 Test
		en_PlcNo		= 0xFF,		// FF// 22.03.21 Test
		en_IoModuleNoL	= 0xFF,
		en_IoModuleNoH	= 0x03,
		en_ReqModuleNo	= 0x00,
		en_RDataLengthL	= 0x0C,
		en_RDataLengthH	= 0x00,
		en_WDataLengthL = 0x0E,
		en_WDataLengthH = 0x00,

		en_CpuCheckTimerL = 0x10,
		en_CpuCheckTimerH = 0x00,

		// Data Body
		en_CmdReadL		= 0x01,
		en_CmdReadH		= 0x04,
		en_CmdWriteL	= 0x01,
		en_CmdWriteH	= 0x14,

		en_SubCmdL		= 0x00,
		en_SubCmdH		= 0x00,

		en_DevCodeM		= 0x90,
		en_DevCodeB		= 0xA0,
		en_DevCodeD		= 0xA8,
		en_DevCodeW		= 0xb4,

		// Recive Data
		en_RecvSubHeaderL = 0xD0,
		en_RecvSubHeaderH = 0x00,
	};
	enum {
		en_SubHeadL_Pos		= 0,
		en_SubHeadH_Pos		= 1,
		en_NetworkNo_Pos	= 2, 
		en_PlcNo_Pos		= 3,
		en_IoModuleNoL_Pos	= 4, 
		en_IoModuleNoH_Pos	= 5,
		en_ReqModuleNo_Pos	= 6,
		en_DataLengthL_Pos	= 7,
		en_DataLengthH_Pos	= 8,
		en_CpuCheckTimerL_Pos= 9,
		en_CpuCheckTimerH_Pos= 10,

		en_CmdL_Pos			= 11,
		en_CmdH_Pos			= 12,
		en_SubCmdL_Pos		= 13,
		en_SubCmdH_Pos		= 14,

		en_DevHead_L_Pos	= 15,
		en_DevHead_M_Pos	= 16,
		en_DevHead_H_Pos	= 17,

		en_DevCode_Pos		= 18,
		en_DevCountL_Pos	= 19,
		en_DevCountH_Pos	= 20,

		en_DevDataPos		= 21,
		en_ReadLength		= 21, 
		en_WriteLength		= 21, 
		en_RecvCodeL_Pos	= 9,
		en_RecvCodeH_Pos	= 10,
	//	en_Dev

		en_ReadDataStartPos = 10,
	};

	enum {
		en_Bit_01 = 0x0001,
		en_Bit_02 = 0x0002,
		en_Bit_03 = 0x0004,
		en_Bit_04 = 0x0008,
		en_Bit_05 = 0x0010,
		en_Bit_06 = 0x0020,
		en_Bit_07 = 0x0040,
		en_Bit_08 = 0x0080,
		en_Bit_09 = 0x0100,
		en_Bit_10 = 0x0200,
		en_Bit_11 = 0x0400,
		en_Bit_12 = 0x0800,
		en_Bit_13 = 0x1000,
		en_Bit_14 = 0x2000,
		en_Bit_15 = 0x4000,
		en_Bit_16 = 0x8000,
	};

public:
	CMelsecEthernetSock(LPCTSTR lpszIpAddress, int protocol);
	virtual ~CMelsecEthernetSock();


public:
	int PacketSend(char* pBuff, int length, long lTimeOver);
	int DataRecv(char* pBuff, int length, long lTimeOver);
	int SetError(DWORD code, CString* msg);
	int ClearBuffer(void);

public:
	CString m_strIpAddress;		//!< ServerＩＰ Address(CString)x1
	int		m_Protocol;			//!< Protocol (enumProtocol)(int)x1
	BOOL	m_bConnected;		//!< Connecte Status(FALSE:Disconnect/TRUE:Connected)(BOOL)x1
	BOOL	m_bDisConnected;	//!< Disconnected Status (BOOL)x1


	unsigned short	m_nNetworkNo;
	unsigned short	m_nDataLength;

public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

public :
	int InPort(int nDevNo, BYTE* data, BOOL bExtSt = FALSE);
	int OutPort(int nDevNo, BYTE data, BOOL bLockCtrl = TRUE, BOOL bGetBit = FALSE);
	// 22.03.23 Ahn Add Start
	int InPort(int nDevNo, WORD* data, int nSize, BOOL bExtSt = FALSE);
	int OutPort(int nDevNo, WORD* data, int nSize, BOOL bLockCtrl = TRUE, BOOL bGetBit = FALSE);
	// 22.03.23 Ahn Add End

	int WriteDataReg(int offset, short data[], int num);
	int ReadDataReg(int offset, short data[], int num);

private:
	BOOL MakeCommand(int nMode, int nDevType, int nDevNo, int nDataSize, char* pDevData, int* pnDataLength, char* pBuff);
	BOOL MakeHeaderCommand(int nMode, int nDevType, char* pBuff);
	BOOL MakeBodyCommand(int nMode, int nDevNo, int nDevCode, char* pBuff);
	BOOL MakeBodyCommand(int nMode, int nDevNo, int nDevCode, char* pBuff, int nDataSize, char *pSendData);

	int SendAndRecv(char* pSendBuff, int nSendLength, char* pRecvBuff, int nRecvLength, long lTimeOver );
	int CheckRecvCmd( char* pRecv);

protected:
	enum {
		TCP_MAX_DATA = 256,	//!< パケット最大サイズ[Bytes]
	};

protected:
	int		SendRetry(void);

protected:
	BYTE*	m_pSendBuffer;	//!< 送信バッファ(BYTE*)x1
	int		m_SendBuffCnt;	//!< 送信バッファサイズ(int)x1
	int		m_SendCnt;		//!< 送信済みバッファ(int)x1
	int		m_RecvCnt;		//!< 受信中データバイト数(int)x1
	int		m_SendFlag;		//!<
	char*	m_btRecv;		//!<

protected:
	CString m_strClientIpAddress;		//!< ＩＰアドレス(CString)x1
	CString m_strClientComputerName;	//!< コンピュータ名(CString)x1

	CRITICAL_SECTION csFunc;
};

