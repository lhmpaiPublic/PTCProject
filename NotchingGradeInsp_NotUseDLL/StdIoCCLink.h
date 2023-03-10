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

	WORD	m_MaxPort;	// ����ϴ� �ִ� ��Ʈ ��
	WORD	m_SeqStNo;	// ������ ����
	long	m_pPath;	// �����ϴ� ȸ���� ��� ������
	int		m_MyPort;

};

