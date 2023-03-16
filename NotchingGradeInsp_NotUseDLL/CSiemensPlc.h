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
	// 23.03.03 Ahn Modify End
	//CSiemensPlc(CString strIPAddress, int nPort = 4000);
	CSiemensPlc(CString strIPAddress, int nPort = 4000, int nOffsetIn = 0 , int nOffsetOut = 100, int nWordIn = 20, int nWordOut = 120 );
	// 23.03.03 Ahn Modify End
	~CSiemensPlc();

private:
	CString m_strIPAddress;
	int m_nPort;

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
	// set maximum time-out for wating recive(default: 500ms)
	void SetTimeOut(int nTimeOut);

public:
	// write data
	int WriteDataReg(int offset, short data[], int num);
	// read data
	int ReadDataReg(int offset, short data[], int num);

	// get network state
	BOOL IsOpened();

	// Coils 미사용
public:
	int InPort(WORD port, BYTE* data, BOOL bExtSt = FALSE) { return -1; };
	int OutPort(WORD port, BYTE data, BOOL bLockCtrl = TRUE, BOOL bGetBit = FALSE) { return -1; };
	int Out_Port_Bit(int nPort, BYTE bBitPos, int nMode) { return -1; };


	// DataLink용
public:
	int ReadDataReg(int offset, int data[], int num) { return -1; };
	int WriteDataReg(int offset, int data[], int num) { return -1; };
	int ReadPortAllBitIn(BYTE* data, short size) { return -1; };
	int ReadPortAllBitOut(BYTE* data, short size) { return -1; };


private:
	// connection network
	int OpenPio(void);
	// disconnection network
	int ClosePio(void);


public:
	//static CCriticalSection* GetCriticalSection(void)
	//{
	//	return (&m_csSiemens);
	//}

};

