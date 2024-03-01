#pragma once

#include "StdIoCtrl.h"

class CStdIoApi :  public CStdIoCtrl
{
public:
	enum {
		enPortMax = 8,
	};

public:
	CStdIoApi(WORD MaxPort);
	virtual ~CStdIoApi(void);

	int Open(BOOL debug = FALSE);
	int Close();
	int Inport(WORD port, BYTE* value);
	int OutPort(WORD port, BYTE byteV);
	int ReadOutport(WORD port, BYTE* value);

private:
	short	m_DeviceID;
	WORD	m_MaxPort;

};

