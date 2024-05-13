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
	virtual int WriteDataReg(int offset, int data[], int num) = 0;
	virtual int ReadDataReg(int offset, int data[], int num) = 0;
	virtual int Out_Port_Bit(int nPort, BYTE bBitPos, int nMode) = 0;

	virtual int Lock_Ctrl(BOOL bLockMode) { return 0; };

	virtual int ReadDataReg(int offset, short data[], int num, short device, short stno = 0xff) { return 0; };
	virtual int WriteDataReg(int offset, short data[], int num, short device, short stno = 0xff) { return 0; };

	virtual void AddReadAddrForIO2(int dev, int addr) { return ; } ;
	virtual void AddWriteAddrForIO2(int dev, int addr) { return ; } ;
	virtual int ReadBitData(int adrs) { return 0; };
	virtual int ReadBitData(short stno, CString device, int adrs) {	return 0; };

	virtual int ReadPortAllBitIn(BYTE *data, short size) = 0 ;
	virtual int ReadPortAllBitOut(BYTE *data, short size) = 0 ;

	BYTE btOutPortData[16];	
	BYTE btInPortData[16];
	WORD wAlarmCode;
	virtual WORD GetAlarmCode() { return wAlarmCode; };
	virtual void SetAlarmCode( WORD data) {	wAlarmCode = data; };
	virtual int WriteAlarmCode(int offset) { return 0; };

	virtual void EnableWorkSet(BOOL bMode) { return; };

	virtual BOOL IsOpened() = 0 ;// { return FALSE; };

private:
	virtual int OpenPio(void) = 0;
	virtual int ClosePio(void) = 0;

};

