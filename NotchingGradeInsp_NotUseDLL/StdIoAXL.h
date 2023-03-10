#pragma once

#include "StdIoCtrl.h"

class CStdIoAXL :  public CStdIoCtrl
{
public:
	enum {
		enPortMax = 8,
	};

public:
	CStdIoAXL( WORD MaxPort, DWORD DeviceID, long BoardNo = 0 );
	virtual ~CStdIoAXL(void);

	int Open(BOOL debug = FALSE);
	int Close();
	int Inport(WORD port, BYTE* value);
	int OutPort(WORD port, BYTE value);
	int ReadOutport(WORD port, BYTE* value);
	int InportWord(WORD* value);
	int OutPortWord(WORD value);
	int ReadOutportWord(WORD* value);
#if 0 
	int OutPortBit(WORD port, int nIndex, BOOL bit);
#endif
#if 0 
	int InportBit(WORD port, int nIndex, BOOL* bit);
#endif

private:
	DWORD	m_dwDeviceID;
	WORD	m_wMaxPort;
	long	m_iBoardNo;
	int		m_nModuleNo;

	CRITICAL_SECTION m_csDio;

private:
	BOOL CloseDevice();

};

