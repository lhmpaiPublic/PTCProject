#pragma once

//In Bit 시작 인덱스
#define READBIT_STARTINDEX 0
//지멘스 BIT 영역 읽기 맥스 갯수
#define SIENENS_READBIT (MAX_SMS_BITIO_IN)

//In Word 시작 인덱스
#define READWORD_STARTINDEX 20

//지멘스 WORD 영역 읽기 맥스 갯수
#define SIENENS_READWORD_MAX 274 //(MAX_SMS_WORDIO_IN)

//지멘스 읽기 BIT + WORD
#define SIENENS_READBITWORD_MAX (SIENENS_READBIT+SIENENS_READWORD_MAX)

//지멘스 WORD 영역 읽기 추가 - Cell Key 시작점
#define SIENENS_CELLKEY_START 210
//지멘스 WORD 영역 읽기 Cell Key 갯수
#define SIENENS_CELLKEY_COUNT 64


//Out Bit 시작 인덱스
#define WRITEBIT_STARTINDEX 0
//지멘스 BIT 영역 쓰기 맥스 갯수
#define SIENENS_WRITEBIT (MAX_SMS_BITIO_OUT)

//Out Word 시작 인덱스
#define WRITEWORD_STARTINDEX 20
//지멘스 WORD 영역 쓰기 맥스 갯수
#define SIENENS_WRITEWORD_MAX 149

//지멘스 쓰기 BIT + WORD
#define SIENENS_WRITEBITWORD_MAX (SIENENS_WRITEBIT+SIENENS_WRITEWORD_MAX)


//지멘스 Recipe Name 읽기 갯수
#define SIEMENS_READRECIPENAME 4
//지멘스 CELL ID 읽기 갯수
#define SIEMENS_READCELLID 10



class CSiemensPlcIo : public CDataPlcImp
{
	int m_nSlaveID;
	int m_nBitIn;
	int m_nBitOut;
	int m_nWordIn;
	int m_nWordOut;

public:
	//strIPAddress : IP, nPort : Port, nSlaveID : Slave ID, nBitIn : In Bit 시작 인덱스, nBitOut : Out Bit 시작 인덱스, nWordIn : In Word 시작 인덱스, nWordOut : Out Word 시작 인덱스, 
	CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort, int nSlaveID, int nBitIn = READBIT_STARTINDEX, int nBitOut = WRITEBIT_STARTINDEX, int nWordIn = READWORD_STARTINDEX, int nWordOut = WRITEWORD_STARTINDEX);
	~CSiemensPlcIo();

	//PLC 읽기 Data
	short m_ReadBitData[SIENENS_READBIT];
	short m_ReadWordData[SIENENS_READWORD_MAX];

	//PLC 쓰기 Data
	short m_WriteData[SIENENS_WRITEBITWORD_MAX];
public:
	// write data
	int WriteDataReg(int offset, short data[], int num);
	// read data
	int ReadDataReg(int offset, short data[], int num);

public:
	// set slave id(default: 1)
	void SetSlaveId(int nId);
	// set receive handle for network state
	void SetReceiveMsgWnd(CWnd* pWnd);
	// set network re-connection time out
	void SetReConnetTimeOut(int nTimeOut); // nTimeOut: default 2000 ms

	// get network state
	BOOL IsOpened();
	// get error msg
	CString GetErrorMsg();
	// get error no
	int GetErrorNo();

	//스래드 함수
	static UINT SiemensPlc_ThreadProc(LPVOID param);

	//스래드에서 호출하는 함수
	void SiemensPlcProc();

	//PLC read Data Parser 함수
	void ReadPlcBitDataParser(short* data);
	void ReadPlcWordDataParser(short* data);
	//레시피 명을 만든다.
	CString MakeRecipeName(short* data);
	//CELL ID를 만든다.
	CString MakeCellId(short* data);


	//PLC write Data Make 함수
	int WritePlcDataMake();

	// 동기화 이벤트 객체
	HANDLE getEvent_SiemensPlc() { return pEvent_SiemensPlc; }
	void setEvent_SiemensPlc() { SetEvent(pEvent_SiemensPlc); }

private:
	//스래드 생성 인스턴스 객체
	CWinThread* m_pThread_SiemensPlc;
	//동기화 이벤트 객체
	HANDLE pEvent_SiemensPlc;

	CString m_strIPAddress;
	int m_nPort;
	CWnd* m_pReceiveMsgWnd;
	int m_nReConnetTimeOut;

	//PLC DLL 객체
	CLGIS_Plc* m_pLGIS_Plc;

	// connection network
	int OpenPlcIo(void);
	// disconnection network
	void ClosePlcIo(void);

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

