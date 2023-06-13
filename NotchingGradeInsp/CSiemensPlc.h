#pragma once
#include "CMelsecBase.h"
#include "..\LGIS\LGIS_Plc.h"

#ifdef _DEBUG
#pragma comment(lib, "LGISD.lib")
#else
#pragma comment(lib, "LGIS.lib")
#endif

class CSiemensPlc : public CMelsecBase
{
public:
	CSiemensPlc(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort = 4000, int nOffsetIn = 0, int nOffsetOut = 100, int nWordIn = 20, int nWordOut = 120);
	~CSiemensPlc();

private:
	CString m_strIPAddress;
	int m_nPort;
	CWnd* m_pReceiveMsgWnd;
	int m_nReConnetTimeOut;

	CLGIS_Plc* m_pLGIS_Plc;
	// 23.03.03 Ahn Add Start
	WORD	m_wOffset_In;	//!< 입력 신호 Offset
	WORD	m_wOffset_Out;	//!< 출력 신호 Offset
	WORD	m_wOffsetWord_In;
	WORD	m_wOffsetWord_Out;
	// 23.03.03 Ahn Add End

public:
	// set slave id(default: 1)
	void SetSlaveId(int nId);
	// set receive handle for network state
	void SetReceiveMsgWnd(CWnd* pWnd);
	// set network re-connection time out
	void SetReConnetTimeOut(int nTimeOut); // nTimeOut: default 2000 ms

public:
	// write data
	int WriteDataReg(int offset, short data[], int num);
	// read data
	int ReadDataReg(int offset, short data[], int num);

	// get network state
	BOOL IsOpened();
	// get error msg
	CString GetErrorMsg();
	// get error no
	int GetErrorNo();

	// Coils 미사용
public:
	// Port에 신호 관련 값을 한번에 취득 함수
	int InPort(WORD port, BYTE* data, BOOL bExtSt = FALSE) { return -1; };
	// 해당 Port에 data 값을 한번에 작성 함수
	int OutPort(WORD port, BYTE data, BOOL bLockCtrl = TRUE, BOOL bGetBit = FALSE) { return -1; };

	// Port에 해당 bit 위치 신호를 ON/OFF하는 함수
	int Out_Port_Bit(int nPort, BYTE bBitPos, int nMode) { return -1; };

	// DataLink용
public:
	int WriteDataReg(int offset, int data[], int num) { return -1; };
	int ReadDataReg(int offset, int data[], int num) { return -1; };
	int ReadPortAllBitIn(BYTE* data, short size) { return -1; };
	int ReadPortAllBitOut(BYTE* data, short size) { return -1; };

private:
	// connection network
	int OpenPio(void);
	// disconnection network
	int ClosePio(void);
};

