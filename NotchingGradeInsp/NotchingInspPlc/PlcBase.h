#pragma once

#define	CHN_NO_NETH1	51		//!< Melsec Net-H
#define	CHN_NO_NETG1	151		//!< Melsec Net-G
#define	CHN_NO_CCLINK1	81		//!< CC-Link
#define	CHN_NO_CCLINK2	82		//!< CC-Link

class CPlcBase
{
public :
	CPlcBase(WORD wChannelNo = CHN_NO_NETG1, WORD wMaxPort = 16, WORD wMyStNo = 1, WORD wExtStNo = 1, WORD wSeqStNo = 0, WORD wOffset_In = 0, WORD wOffset_Out = 0);
	virtual ~CPlcBase();

	virtual int InPort(WORD port, BYTE* data, BOOL bExtSt = FALSE) = 0 ;
	virtual int OutPort(WORD port, BYTE data, BOOL bLockCtrl = TRUE, BOOL bGetBit = FALSE) = 0;
	virtual int WriteDataReg(int offset, short data[], int num) = 0;
	virtual int ReadDataReg(int offset, short data[], int num) = 0;
// 22.07.06 Ahn Add Start
	virtual int WriteDataReg(int offset, int data[], int num) = 0;
	virtual int ReadDataReg(int offset, int data[], int num) = 0;
// 22.07.06 Ahn Add End
	virtual int Out_Port_Bit(int nPort, BYTE bBitPos, int nMode) = 0;

	virtual int Lock_Ctrl(BOOL bLockMode) { return 0; };

	virtual int ReadDataReg(int offset, short data[], int num, short device, short stno = 0xff) { return 0; };
	virtual int WriteDataReg(int offset, short data[], int num, short device, short stno = 0xff) { return 0; };

	virtual void AddReadAddrForIO2(int dev, int addr) { return ; } ;
	virtual void AddWriteAddrForIO2(int dev, int addr) { return ; } ;
	virtual int ReadBitData(int adrs) { return 0; };
	virtual int ReadBitData(short stno, CString device, int adrs) {	return 0; };

	// 22.03.23 Ahn Add Start
	virtual int ReadPortAllBitIn(BYTE *data, short size) = 0 ;
	virtual int ReadPortAllBitOut(BYTE *data, short size) = 0 ;
	// 22.03.23 Ahn Add End

	BYTE btOutPortData[16];	
	// 22.03.24 Ahn Add Start
	BYTE btInPortData[16];
	WORD wAlarmCode;
	virtual WORD GetAlarmCode() { return wAlarmCode; };
	virtual void SetAlarmCode( WORD data) {	wAlarmCode = data; };
	virtual int WriteAlarmCode(int offset) { return 0; };
	// 22.03.24 Ahn Add Start

	// 22.05.26 Ahn Add Start
	virtual void EnableWorkSet(BOOL bMode) { return; };
	// 22.05.26 Ahn Add End

	// 22.06.30 Ahn Add Start
	virtual BOOL IsOpened() = 0 ;// { return FALSE; };
	// 22.06.30 Ahn Add End

private:
	//virtual void PioErrorProcess(int iRet, char* cap) = 0;
	//virtual int LocalPioOpen(BOOL bLockCtrl = TRUE) = 0;
	virtual int OpenPio(void) = 0;
	virtual int ClosePio(void) = 0;

};

