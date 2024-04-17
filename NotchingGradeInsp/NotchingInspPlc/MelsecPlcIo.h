#pragma once

#define	MELSEC_DEVICE_X	0x01	// X
#define	MELSEC_DEVICE_Y	0x02	// Y
#define MELSEC_DEVICE_M	0x04	// M
#define	MELSEC_DEVICE_D	0x0d	// D
#define	MELSEC_DEVICE_R	0x16	// R
#define MELSEC_DEVICE_B	0x17	// B
#define MELSEC_DEVICE_W	0x18	// W


#define	MELSEC_CHN_NO_NETH1	51		//!< Melsec Net-H
#define	MELSEC_CHN_NO_NETG1	151		//!< Melsec Net-G
#define	MELSEC_CHN_NO_CCLINK1	81		//!< CC-Link
#define	MELSEC_CHN_NO_CCLINK2	82		//!< CC-Link

#define	MELSEC_MAX_PORT 4

#define	MELSEC_LIO_STATION_NO			(1)
#define	MELSEC_MAIN_STATION_NO			(2)
#define	MELSEC_EX_STATION_NO			(7)

#define	MELSEC_SEQ_STATION_NO			0xFF

//In Bit 시작 인덱스
#define MELSEC_READBIT_STARTINDEX 0

//In Word 시작 인덱스
#define MELSEC_READWORD_STARTINDEX 0

//Out Bit 시작 인덱스
#define MELSEC_WRITEBIT_STARTINDEX 0

//Out Word 시작 인덱스
#define MELSEC_WRITEWORD_STARTINDEX 0

//멜섹 Bit Read 영역 크기
#define	MELSEC_BITINSIZE 4



class CMelsecPlcIo : public CDataPlcImp
{
	WORD	m_wChannelNo;
	WORD	m_wMaxPort;
	WORD	m_wMyStNo;
	WORD	m_wExtStNo;
	WORD	m_wSeqStNo;
	WORD	m_wOffset_BitIn;
	WORD	m_wOffset_BitOut;
	WORD	m_wOffset_WordIn;
	WORD	m_wOffset_WordOut;
	long	m_pPath;

	BOOL m_bOpened;

	BOOL m_bEnableWorkSet;

public:
	//
	CMelsecPlcIo(WORD wOffset_BitIn, WORD wOffset_BitOut, WORD wOffset_WordIn, WORD wOffset_WordOut, WORD wChannelNo = MELSEC_CHN_NO_NETG1, WORD wMaxPort = MELSEC_MAX_PORT, WORD wMyStNo = MELSEC_LIO_STATION_NO, WORD wExtStNo = MELSEC_EX_STATION_NO, WORD wSeqStNo = MELSEC_SEQ_STATION_NO);
	~CMelsecPlcIo();

public:

	//PLC 데이터 Read / Write 처리 함수
	void MelsecPlcProc();

	byte m_buffBitIn[MELSEC_BITINSIZE];
private:

	// connection network
	int OpenPlcIo(void);

	int LocalPioOpen(BOOL bLockCtrl = TRUE);

	int ChangeWorkingSetSize(void);

	BOOL IsOpened();

	//stno : 0xff , 또는 0x00
	//device : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용
	//num : 읽을 갯수
	//bIn : Read - true Write - false
	//return : size
	int ReadBitData(short stno, int devtype, int startport, byte buff[], int num, bool bIn);

	int ReadWordData(short stno, int devtype, int startport, byte buff[], int num, bool bIn);

	int ReadWordDataEx(short netNo, int devtype, int startport, byte buff[], int num, bool bIn);

	void chnageEndian(short data[], int size);

	// disconnection network
	void ClosePlcIo(void);

	//PLC read Data Parser 함수
	void ReadPlcBitDataParser(short* data);
	void ReadPlcWordDataParser(short* data);
	//레시피 명을 만든다.
	CString MakeRecipeName(short* data);
	//CELL ID를 만든다.
	CString MakeCellId(short* data);


	//PLC write Data Make 함수
	int WritePlcDataMake();


	virtual int PlcDataReadWritePorc();

	//Out
	virtual int SigOutEncoderZeroSet(int nMode);
	virtual int SigOutRecipeChangeAck(int nMode);
	virtual int SigOutLotEndAck(int nMode);
	virtual int SigOutLotStartAck(int nMode);
	virtual int WriteAlarmCode(WORD nAlarmCode);
	virtual int SigOutReady(int nMode);
	//Alive 상태 값을 저장해서 다음 호출 시 
	//Alive 상태를 변경한다. 0 <-> 1
	BOOL m_bSmsAlive;
	virtual int SigOutAlivePulse(int nInMode);
	virtual int SigOutTabZeroReset(int nMode);
	virtual int SigOutAlarmResetAck(int nMode);

	virtual int SigOutAlivePulseReady(int nInMode, BOOL bIsReady);

	//멜섹만 쓴다.
	virtual int SigOutAlarmExist(int nMode);
	//멜섹만 쓴다.
	virtual void EnableWorkSet(BOOL bMode);

	virtual int SigOutDiskCapacityAlarm(int nMode);
	virtual int SigOutDiskCapacityWarning(int nMode);

	//멜섹만 쓴다.
	virtual int SignalBitOut(int nIntegration, int nMode, BOOL bLocal = FALSE);

	//Lot End 처리 함수
	virtual void SigOutLotEnd(int TopDefectCnt, int BtmDefectCnt);

	//In
	virtual int SigInReady();
	virtual int SigInRun();
	virtual int SigInLotEnd();
	virtual int SigInTabZeroReset();
	virtual int SigInLotStart();
	virtual int SigInAlarmReset();
	virtual int SigInAlarmNgAck();
	virtual int SigInRecipeChange();
	virtual int SigInInkMarkActive();
	virtual int SigInConnectZone();

	//지멘스만 쓴다.
	virtual int WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode);

	//통합비전 Cell Key Id를 가져온다.
	virtual int GetCellKey(int num);

	//마킹 설정 값 변수
	virtual BOOL GetInkMarkActive();

	//ConnectZone 설정 플래그
	virtual BOOL GetConnectZone();

	virtual int ReadBlockAllData(CSequenceData* pSeqData);
	virtual int WriteBlockData(void* pGlobalData);

	virtual int ReadAllPort_BitIn(BOOL* pSigBitIn);
	virtual int ReadAllPort_BitOut(BOOL* pSigBitOut);

	//In Word 영역의 인덱스의 값을 String 으로 가져온다.
	virtual CString GetInWordData(int idx);
	//In Word 영역의 인덱스의 값을 String 으로 가져온다.
	virtual CString GetOutWordData(int idx);

};

