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
#define	MELSEC_BITINSIZE_MAX 4

//멜섹 Bit Write 영역 크기
#define	MELSEC_BITOUTSIZE_MAX 4

//멜섹 Recipe Name 읽기 갯수
#define MELSEC_READRECIPENAME 8
//멜섹 CELL ID 읽기 갯수
#define MELSEC_READCELLID 20
//멜섹 Alram Buff 갯수
#define MELSEC_COUNT_ALRAMBUFF 5

//멜섹 Word Read 영역 크기
#define	MELSEC_WORDINSIZE_MAX 160

//멜섹 Word Write 영역 DWORD 크기
#define	MELSEC_WORDOUTSIZE_MAX 160

//멜섹 Word Read 영역 크기
#define	MELSEC_DWORDINSIZE_MAX MELSEC_WORDINSIZE_MAX/2

//멜섹 Word Write 영역 DWORD 크기
#define	MELSEC_DWORDOUTSIZE_MAX MELSEC_WORDOUTSIZE_MAX/2


class CMelsecPlcIo : public CDataPlcImp
{
	//채널 번호
	DWORD	m_wChannelNo;
	//BYTE 읽기 갯수
	DWORD	m_wMaxPort;
	//스테이션 번호(My)
	DWORD	m_wMyStNo;
	//스테이션 번호(Ext)
	DWORD	m_wExtStNo;
	//스테이션 번호(Seq)
	DWORD	m_wSeqStNo;

	//Bit 읽기 시작점(8bit 단위 설정해야함)
	DWORD	m_wOffset_BitIn;
	//Bit 쓰기 시작점(8bit 단위 설정해야함)
	DWORD	m_wOffset_BitOut;
	//Word 읽기 시작점
	DWORD	m_wOffset_WordIn;
	//Word 쓰기 시작점
	DWORD	m_wOffset_WordOut;

	//멜섹 생성 객체 번호
	long	m_pPath;

	//멜섹 Open 상태 체크
	BOOL m_bOpened;

	BOOL m_bEnableWorkSet;

public:
	//
	CMelsecPlcIo(DWORD wOffset_BitIn, DWORD wOffset_BitOut, DWORD wOffset_WordIn, DWORD wOffset_WordOut, DWORD wChannelNo = MELSEC_CHN_NO_NETG1, DWORD wMaxPort = MELSEC_MAX_PORT, DWORD wMyStNo = MELSEC_LIO_STATION_NO, DWORD wExtStNo = MELSEC_EX_STATION_NO, DWORD wSeqStNo = MELSEC_SEQ_STATION_NO);
	~CMelsecPlcIo();

public:

	//PLC 데이터 Read / Write 처리 함수
	void MelsecPlcProc();

private:

	//PLC 읽기 Data
	//BYTE 단위 Bit 영역 데이터
	BYTE m_ReadBitData[MELSEC_BITINSIZE_MAX];
	//DWORD 단위 Word 영역 데이터
	DWORD m_ReadDwordData[MELSEC_DWORDINSIZE_MAX];

	//PLC 쓰기 Data
	//BYTE 단위 Bit 영역 데이터
	BYTE m_WriteBitData[MELSEC_BITOUTSIZE_MAX];
	//DWORD 단위 Word 영역 데이터
	DWORD m_WriteDwordData[MELSEC_DWORDOUTSIZE_MAX];

	// connection network
	int OpenPlcIo(void);

	//멜섹 Open
	int LocalPioOpen(BOOL bLockCtrl = TRUE);

	//Work Set 설정
	int ChangeWorkingSetSize(void);

	//멜섹 오프상태 확인 함수
	BOOL IsOpened();

	//stno : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(byte : 1바이트 단위 갯수)
	//num : 읽을 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 읽기 실패 여부(0 : 성공, 그외 실패)
	int ReadBitData(short stno, int devtype, int startport, byte buff[], int num, int offset);
	//stno : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(short : 2바이트 단위 갯수)
	//num : 읽을 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 읽기 실패 여부(0 : 성공, 그외 실패)
	int ReadWordData(short stno, int devtype, int startport, short buff[], int num, int offset);
	//netNo : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(short : 2바이트 단위 갯수)
	//num : 읽을 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 읽기 실패 여부(0 : 성공, 그외 실패)
	int ReadWordDataEx(long netNo, int devtype, int startport, short buff[], int num, int offset);
	//stno : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(int : 4바이트 단위 갯수)
	//num : 읽을 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 읽기 실패 여부(0 : 성공, 그외 실패)
	int ReadDwordData(short stno, int devtype, int startport, int buff[], int num, int offset);
	//netNo : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(int : 4바이트 단위 갯수)
	//num : 읽을 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 읽기 실패 여부(0 : 성공, 그외 실패)
	int ReadDwordDataEx(long netNo, int devtype, int startport, int buff[], int num, int offset);
	//stno : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(byte : 1바이트 단위 갯수)
	//num : 쓰기 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 쓰기 실패 여부(0 : 성공, 그외 실패)
	int WriteBitData(short stno, int devtype, int startport, byte buff[], int num, int offset);
	//stno : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(short : 2바이트 단위 갯수)
	//num : 쓰기 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 쓰기 실패 여부(0 : 성공, 그외 실패)
	int WriteWordData(short stno, int devtype, int startport, short buff[], int num, int offset);
	//netNo : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(short : 2바이트 단위 갯수)
	//num : 쓰기 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 쓰기 실패 여부(0 : 성공, 그외 실패)
	int WriteWordDataEx(long netNo, int devtype, int startport, short buff[], int num, int offset);
	//stno : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(int : 4바이트 단위 갯수)
	//num : 쓰기 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 쓰기 실패 여부(0 : 성공, 그외 실패)
	int WriteDwordData(short stno, int devtype, int startport, int buff[], int num, int offset);
	//netNo : 0xff , 또는 0x00
	//devtype : B : 바이너리, X : 헥사 , W : 워드 
	//startport : 시작 포트
	//buff : 배열 고정 버퍼 사용(int : 4바이트 단위 갯수)
	//num : 쓰기 갯수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//return : 쓰기 실패 여부(0 : 성공, 그외 실패)
	int WriteDwordDataEx(long netNo, int devtype, int startport, int buff[], int num, int offset);

	//Endian 전환 함수
	void chnageEndian(short data[], int size);

	// disconnection network
	void ClosePlcIo(void);

	//PLC read Data Parser 함수
	//Bit 영역 BYTE 단위 읽은 데이터를 파싱하여 멤버변수에 세팅한다
	void ReadPlcBitDataParser(BYTE* data);
	//Word 영역 DWORD 단위 읽은 데이터를 파싱하여 멤버변수에 세팅한다
	void ReadPlcWordDataParser(DWORD* data);
	//레시피 명을 만든다.
	CString MakeRecipeName(DWORD* data);
	//CELL ID를 만든다.
	CString MakeCellId(DWORD* data);

	//PLC write Bit Make 함수
	//Bit 쓰기 영역 데이터를 PLC에 쓰기위해서 BYTE 타임으로 멤버변수의 값을 세팅하여 가져온다.
	int WritePlcBitDataMake(BYTE** data);

	//PLC write Bit Make 함수
	//Word 쓰기 영역 데이터를 PLC에 쓰기위해서 DWORD 타임으로 멤버변수의 값을 세팅하여 가져온다.
	int WritePlcWordDataMake(DWORD** data);


	//************* 상위 클래스 오버라이딩 함수 ****************************************
	//PLC 데이터를 쓰고 읽기를 View 타이머에서 주기적으로 읽기 위해서 오버라이팅 한다.
	//PLC data를 읽고 쓰고 처리를 위한 함수
	virtual int PlcDataReadWritePorc();
	//PLC Open 상태를 체크을 위한 함수
	virtual BOOL isPlcOpen();

	//Out
	//Zero Set 세팅 함수
	virtual int SigOutEncoderZeroSet(int nMode);
	virtual int SigOutRecipeChangeAck(int nMode);
	virtual int SigOutLotEndAck(int nMode);
	virtual int SigOutLotStartAck(int nMode);
	virtual int WriteAlarmCode(WORD nAlarmCode);
	virtual int SigOutReady(int nMode);
	//Alive 상태 값을 저장해서 다음 호출 시 
	//Alive 상태를 변경한다. 0 <-> 1
	BOOL m_bMelsecAlive;
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

	//PLC에서 읽은 데이터를 가져온다.
	//CSequenceData 클래스 멤버에 세팅하여 가져온다.
	virtual int ReadBlockAllData(CSequenceData* pSeqData);

	//CGlobalData 클래스에 세팅된 데이터를 PLC 멤버변수에 세팅한다.
	//검사 데이터들이 전역으로 세팅되어 전체 호출하여 세팅한다.
	virtual int WriteBlockData(void* pGlobalData);
	//****************************************************************

	//***************** 모니터링 데이터 가져오기 *********************
	//기존 사용하는 방식 호환 기능
	//CDataPlcImp 클래스 멤버를 호출하여 직접 세팅할 수 있다.
	//Read 영역 Bit 데이터
	virtual int ReadAllPort_BitIn(BOOL* pSigBitIn);
	//Write Bit 영역 데이터
	virtual int ReadAllPort_BitOut(BOOL* pSigBitOut);

	//In Word 영역의 인덱스의 값을 String 으로 가져온다.
	virtual CString GetInWordData(int idx);
	//In Word 영역의 인덱스의 값을 String 으로 가져온다.
	virtual CString GetOutWordData(int idx);
	//****************************************************************

};

