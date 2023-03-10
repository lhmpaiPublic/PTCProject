#pragma once

#define	LIO_STATION_NO			(1)
#define	MAIN_STATION_NO			(2)
#define	EX_STATION_NO			(7)
// 22.03.23 Ahn Modify Start
//#define MAX_PORT	16			//�����Ʈ��	
#define MAX_PORT	4			//�����Ʈ��	
// 22.03.23 Ahn Modify End
#define	OFFSET_IN	0x63A0		//�Էµ���̽� ��ȣ ������
#define	OFFSET_OUT	0x6B20		//��µ���̽� ��ȣ ������

#include "MelsecDataLink.h"
#include "CMelsecEthenet.h"
// 23.02.28 Son Add Start
#include "CSiemensPlc.h"
// 23.02.28 Son Add End

typedef struct {
	BOOL CheckPortFlag[MAX_PORT];
	BYTE InputData[MAX_PORT];
	BOOL EndFlag;	// ������ ����
	BOOL BusyFlag;	// ������ �۾���
} PIOTHREAD_DATAIF;

class CMelsecBase;


class CPioCtrl
{
public:
	CPioCtrl(WORD ChnNo, WORD DrvNo, WORD GrpNo);
	virtual ~CPioCtrl();
	bool PioPortProcess(int port, BYTE data, int stus);
	int InPortByte(WORD port, BYTE* data, BOOL bExtSt = FALSE);
	BYTE GetOutData(int port);
	int OutPortByte(WORD port, BYTE data, BOOL bLockCtrl = TRUE);
	int Out_Port_Bit(int nPort, BYTE bBitPos, int nMode);
	int Lock_Ctrl(BOOL bLockMode);

	int WritePLC_device(int address, short data);
	int ReadPLC_device(int address, short* data);
	int WritePLC_Block_device(int address, short* pData, int nNumOfData);
	int ReadPLC_Block_device(int address, short* pData, int nNumOfData);

	// 22.07.06 Ahn Add Start
	int WritePLC_Block_device(int address, int* pData, int nNumOfData);
	//int ReadPLC_Block_device(int address, int* pData, int nNumOfData);
	// 22.07.06 Ahn Add End

	int WritePLC_device(int address, short data, int num, short device, short stno);
	int ReadPLC_device(int address, short* data, int num, short device, short stno);
	int WritePLC_Block_device(int address, short* pData, int nNumOfData, short device, short stno);
	int ReadPLC_Block_device(int address, short* pData, int nNumOfData, short device, short stno);

	// 22.03.23 Ahn Add Start
	int ReadAllPort_BitIn(BYTE *data, short size);
	int ReadAllPort_BitOut(BYTE *data, short size);
	// 22.03.23 Ahn Add End
	// 22.03.24 Ahn Add Start
	WORD GetAlarmCode();
	int SetAlarmCode(WORD wAlarmCode);
	int WriteAlarmCode(int address);
	// 22.03.24 Ahn Add End

	friend UINT ThreadProc_InPortCheck(LPVOID);
	bool PioThreadCheck();
	void PioTheadStop();
	void PioTheadRun();
	int InPortByteThread(WORD port, BYTE* data);

	void AddReadAddrForIO2(int dev, int addr);
	void AddWriteAddrForIO2(int dev, int addr);

	int InPortAnotherDeviceBit(short stno, CString device, int adrs);

	// 22.05.26 Ahn Add Start
	void EnableWorkSet(BOOL bMode) ;
	// 22.05.06 Ahn Add End

	// 22.06.30 Ahn Add Start
	BOOL IsOpened();
	// 22.06.30 Ahn Add End
private:
	static CCriticalSection	m_csPioThread;
	CWinThread* m_pThread;
	PIOTHREAD_DATAIF PioDataIF;
	// 22.03.17 Ahn Modify Start
	//CMelsecDataLink* pAprPio;
	CMelsecBase* pAprPio;
	// 22.03.17 Ahn Modify End

};

