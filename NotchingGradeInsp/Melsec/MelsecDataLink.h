#pragma once

#define	DEVICE_X	0x01	// X
#define	DEVICE_Y	0x02	// Y
#define DEVICE_M	0x04	// M
#define	DEVICE_D	0x0d	// D
#define	DEVICE_R	0x16	// R
#define DEVICE_B	0x17	// B
#define DEVICE_W	0x18	// W


//
//#define	CHN_NO_NETH1	51		//!< Net/H Chanel No
//#define	CHN_NO_NETG1	151		//!< Net/G Chanel No
//#define	CHN_NO_CCLINK1	81		//!< CC-Link Chanel No
//#define	CHN_NO_CCLINK2	82		//!< CC-Link Chanel No

//class  CMelsecBase;
#include "CMelsecBase.h"

class CMelsecDataLink:public CMelsecBase
{
public:
	CMelsecDataLink(WORD wChannelNo = CHN_NO_CCLINK1, WORD wMaxPort = 16, WORD wMyStNo = 1, WORD wExtStNo = 1, WORD wSeqStNo = 0, WORD wOffset_In = 0, WORD wOffset_Out = 0) ;
	//virtual ~CMelsecDataLink();
	~CMelsecDataLink();

	int InPort(WORD port, BYTE* data, BOOL bExtSt = FALSE);
	int InPort_AnotherDevice(short stno, CString device, int boffset, BYTE* data, short num);
	int ReadBitData(int adrs);
	int ReadBitData(short stno, CString device, int adrs);
	int OutPort(WORD port, BYTE data, BOOL bLockCtrl = TRUE, BOOL bGetBit = FALSE);
	int WriteDataReg(int offset, short data[], int num);
	int WriteDataReg(int offset, int data[], int num);
	int ClearDataReg(int size);
	int ReadDataReg(int offset, short data[], int num);
	int ReadDataReg(int offset, int data[], int num);
	int Out_Port_Bit(int nPort, BYTE bBitPos, int nMode);
	int Lock_Ctrl(BOOL bLockMode);
	int ChangeWorkingSetSize(void);

	int ReadDataReg(int offset, short data[], int num, short device, short stno = 0xff);
	int WriteDataReg(int offset, short data[], int num, short device, short stno = 0xff);

	//void AddInPortForIO2(WORD port);
	//void AddOutPortForIO2(WORD port);
	void AddReadAddrForIO2(int dev, int addr);
	void AddWriteAddrForIO2(int dev, int addr);

	int ReadPortAllBitIn(BYTE *data, short size) ;
	int ReadPortAllBitOut(BYTE* data, short size)  ;

	// 22.05.26 Ahn Add Start
	void EnableWorkSet(BOOL bMode);
	// 22.05.26 Ahn Add End
	// 22.06.30 Ahn Add Start
	BOOL IsOpened();
	// 22.06.30 Ahn Add End


	BOOL m_bDebugIO;
	BOOL m_bDebugIO2;

	BOOL m_bEnableWorkSet;

private:

	void PioErrorProcess(int iRet, char* cap);
	int OpenPio(void);
	int ClosePio(void);
	int LocalPioOpen(BOOL bLockCtrl = TRUE);

	WORD	m_wChannelNo;	// チャネル番号
	WORD	m_wMaxPort;	// 使用する最大ポート数
	WORD	m_wMyStNo;		// 自局の局番
	WORD	m_wExtStNo;	// 他局の局番
	WORD	m_wSeqStNo;	// シーケンサの局番
	WORD	m_wOffset_In;	//!< 入力デバイス番号オフセット(WORD)x1
	WORD	m_wOffset_Out;	//!< 出力デバイス番号オフセット(WORD)x1
	long	m_pPath;		// オープンされた回線のパスのポインタ

	CList< WORD, WORD >	m_listInPortForIO2;		// ＰＬＣ搬送取込モードで使用可能なポート
	CList< WORD, WORD >	m_listOutPortForIO2;		// ＰＬＣ搬送取込モードで使用可能なポート
	CList< DWORD, DWORD >	m_listReadAddrForIO2;	// ＰＬＣ搬送取込モードで使用可能なデバイスのアドレス(上位ビット:デバイス番号/下位ビット:アドレス)
	CList< DWORD, DWORD >	m_listWriteAddrForIO2;	// ＰＬＣ搬送取込モードで使用可能なデバイスのアドレス(上位ビット:デバイス番号/下位ビット:アドレス)

	static CCriticalSection	m_csMelsec;	//!< 排他制御用クリティカルセクション(CCriticalSection)x1

	// 22.06.30 Ahn Add Start
	BOOL m_bOpened;
	// 22.06.30 Ahn Add End

public:
	static CCriticalSection* GetCriticalSection(void)
	{
		return (&m_csMelsec);
	}
};

