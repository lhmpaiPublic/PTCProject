#pragma once

//지멘스 BIT 영역 읽기 맥스 갯수
#define SIENENS_READBIT (MAX_SMS_BITIO_IN)

//지멘스 WORD 영역 읽기 맥스 갯수
#define SIENENS_READWORD_MAX 33

//지멘스 WORD 영역 읽기 추가 - Cell Key 시작점
#define SIENENS_CELLKEY_START 210
//지멘스 WORD 영역 읽기 Cell Key 갯수
#define SIENENS_CELLKEY_COUNT 64


//지멘스 BIT 영역 쓰기 맥스 갯수
#define SIENENS_WRITEBIT (MAX_SMS_BITIO_OUT)

//지멘스 WORD 영역 쓰기 맥스 갯수
#define SIENENS_WRITEWORD_MAX 83


//지멘스 Recipe Name 읽기 갯수
#define SIEMENS_READRECIPENAME 8
//지멘스 CELL ID 읽기 갯수
#define SIEMENS_READCELLID 20

class CSiemensPlcIo : public CDataPlcImp
{
	//PLC Slave ID
	int m_nSlaveID;
	//Bit 입력 시작 위치
	int m_nBitIn;
	//Bit 출력 시작 위치
	int m_nBitOut;
	//Word 입력 시작 위치
	int m_nWordIn;
	//Word 출력 시작 위치
	int m_nWordOut;

	//Cell Key Word 입력 시작 위치
	int m_nWordInCellkey;

	//Alive 신호채크 변수
	DWORD dwMelsBitInAliveTime;
public:
	//생성자
	//strIPAddress : IP
	//nPort : Port
	//nSlaveID : Slave ID
	//nBitIn : In Bit 시작 인덱스
	//nBitOut : Out Bit 시작 인덱스
	//nWordIn : In Word 시작 인덱스
	//nWordOut : Out Word 시작 인덱스, 
	CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort, int nSlaveID, int nBitIn, int nBitOut, int nWordIn, int nWordOut);
	~CSiemensPlcIo();

	//PLC 읽기 Data
	//short 단위 Bit 영역 데이터
	short m_ReadBitData[SIENENS_READBIT];
	//short 단위 Word 영역 데이터
	short m_ReadWordData[SIENENS_READWORD_MAX];
	//short 단위 Word 영역 Cell key 데이터
	short m_ReadWordCellKeyData[COUNT_CELLKEY];

	//PLC 쓰기 Data
	//PLC 쓰기 Bit Data
	short m_WriteBitData[SIENENS_WRITEBIT];
	//PLC 쓰기 Word Data
	short m_WriteWordData[SIENENS_WRITEWORD_MAX];

	//PLC 쓰기 Bit Data 읽기
	short m_WriteBitDataRead[SIENENS_WRITEBIT];
	//PLC 쓰기 Word Data 읽기
	short m_WriteWordDataRead[SIENENS_WRITEWORD_MAX];
public:
	// 데이터를 쓰기위한 함수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//data : 배열 고정 버퍼 사용(short : 2바이트 단위 갯수)
	//num : short 단위 쓰기 갯수
	//return : 성공여부 (0 : 성공 0보다 작으면 실패)
	int WriteDataReg(int offset, short data[], int num);

	// 데이터를 읽기위한 함수
	//offset : 시스템 세팅 읽기 쓰기 시작점
	//data : 배열 고정 버퍼 사용(short : 2바이트 단위 갯수)
	//num : short 단위 읽을 갯수
	//return : 성공여부 (0 : 성공 0보다 작으면 실패)
	int ReadDataReg(int offset, short data[], int num);

public:
	// set slave id(default: 1)
	void SetSlaveId(int nId);
	// set receive handle for network state
	void SetReceiveMsgWnd(CWnd* pWnd);
	// set network re-connection time out
	void SetReConnetTimeOut(int nTimeOut); // nTimeOut: default 2000 ms

	// get network state
	//Open 상태를 넘긴다.
	BOOL IsOpened();
	// get error msg 체크 함수
	CString GetErrorMsg();
	// get error 번호
	int GetErrorNo();


	//PLC 데이터 Read / Write 처리 함수
	void SiemensPlcProc();

	//PLC read Data Parser 함수
	//Bit 영역 short 단위 읽은 데이터를 파싱하여 멤버변수에 세팅한다
	void ReadPlcBitDataParser(short* data);
	//Word 영역 short 단위 읽은 데이터를 파싱하여 멤버변수에 세팅한다
	void ReadPlcWordDataParser(short* data);

	//Word 영역 short 단위 읽은 데이터를 파싱하여 멤버변수에 세팅한다(Cell key 영역)
	void ReadPlcWordCellKeyDataParser(short* data);

	//레시피 명을 만든다.
	CString MakeRecipeName(short* data);
	//CELL ID를 만든다.
	CString MakeCellId(short* data);

	//PLC write Bit Data Make 함수
	int WritePlcBitDataMake();
	//PLC write Word Data Make 함수
	int WritePlcWordDataMake();

private:
	//접속IP 주소
	CString m_strIPAddress;
	//접속 Port 번호
	int m_nPort;
	//
	CWnd* m_pReceiveMsgWnd;
	//접속 시도 타임아웃 시간 설정
	int m_nReConnetTimeOut;

	//PLC DLL 객체
	CLGIS_Plc* m_pLGIS_Plc;

	// connection network
	int OpenPlcIo(void);
	// disconnection network
	void ClosePlcIo(void);
public:

	//************* 상위 클래스 오버라이딩 함수 ****************************************
	//PLC 데이터를 쓰고 읽기를 View 타이머에서 주기적으로 읽기 위해서 오버라이팅 한다.
	virtual int PlcDataReadWritePorc();
	//PLC Open 상태를 체크을 위한 함수
	virtual BOOL isPlcOpen();

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
	//멜섹만 쓴다.
	virtual int SignalBitOut(int nIntegration, int nMode, BOOL bLocal = FALSE);

	virtual int SigOutDiskCapacityAlarm(int nMode);
	virtual int SigOutDiskCapacityWarning(int nMode);

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

	//PLC In Alive 상태를 체크한다.
	virtual BOOL AliveBitInCheck();

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

