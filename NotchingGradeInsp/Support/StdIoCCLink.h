#pragma once

#include "StdIoCtrl.h"

class CStdIoCCLink :  public CStdIoCtrl
{
public:
	enum {
		enPortMax = 64,
		enSeqNoMax = 8,
	};
public:
	CStdIoCCLink(WORD MaxPort = 16, WORD SeqStNo = 0);
	virtual ~CStdIoCCLink();

	int Open(BOOL debug = FALSE);
	int Close();

	int Inport(WORD port, BYTE* value);
	int OutPort(WORD port, BYTE byteV);
	int ReadOutport(WORD port, BYTE* value);

	int ReadReg(WORD dev, WORD* data);
	int WriteReg(WORD dev, WORD data);
	int WriteReg_Block(WORD dev, WORD* data, int nNumOfData);

private:
	int WriteRegister(short nSeqNumber, short type, short nDevNumber, short nDataSize, void* pBuff);
	int ReadRegister(short nSeqNumber, short type, short nDevNumber, short nDataSize, void* pBuff);

	void SetSeqStationNo(WORD stno);
	int ReadBitData(int adrs);
	int ReadBitData(short stno, CString device, int adrs);

	virtual int ChangeWorkingSetSize(void);

	WORD	m_MaxPort;	// 사용하는 최대 포트 수
	WORD	m_SeqStNo;	// 시퀀스 국번
	long	m_pPath;	// 오픈하는 회선의 경로 포인터
	int		m_MyPort;

};

