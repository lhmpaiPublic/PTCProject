#pragma once

#ifdef _DEBUG
#pragma comment(lib, "LGISD.lib")
#else
#pragma comment(lib, "LGIS.lib")
#endif

//지멘스 BIT 영역 읽기 갯수
#define SIENENS_READBIT (MAX_SMS_BITIO_IN)
//지멘스 WORD 영역 읽기 갯수
#define SIENENS_READWORD_MAX 33 //(MAX_SMS_WORDIO_IN)

//지멘스 Recipe Name 읽기 갯수
#define SIEMENS_READRECIPENAME 4
//지멘스 CELL ID 읽기 갯수
#define SIEMENS_READCELLID 10

//지멘스 BIT 영역 쓰기 갯수
#define SIENENS_WRITEBIT (MAX_SMS_BITIO_OUT)
//지멘스 WORD 영역 쓰기 갯수
#define SIENENS_WRITEWORD_MAX 149 //(MAX_SMS_WORDIO_OUT)
//지멘스 쓰기 BIT + WORD
#define SIENENS_WRITEBITWORD_MAX (SIENENS_WRITEBIT+SIENENS_WRITEWORD_MAX) //(MAX_SMS_WORDIO_OUT)

//지멘스 WORD 영역 - 검사 정보 쓰기 갯수
#define SIENENS_WRITEWORD_INSP 6 
//지멘스 WORD 영역 - Alarm 정보 쓰기 갯수
#define SIENENS_WRITEWORD_ALARM 10
//지멘스 WORD 영역 - Defect 정보 쓰기 갯수
#define SIENENS_WRITEWORD_DEFECT 4
//지멘스 WORD 영역 - Target 정보 쓰기 갯수
#define SIENENS_WRITEWORD_TARGET 8
//지멘스 WORD 영역 - Frm Cnt 정보 쓰기 갯수
#define SIENENS_WRITEWORD_FRMCNT 3
//지멘스 WORD 영역 - Empty 1 정보 쓰기 갯수
#define SIENENS_WRITEWORD_EMPTY1 10
//지멘스 WORD 영역 - Alarm Code 정보 쓰기 갯수
#define SIENENS_WRITEWORD_ALARMCODE 25
//지멘스 WORD 영역 - Empty 2 정보 쓰기 갯수
#define SIENENS_WRITEWORD_EMPTY2 15
//지멘스 WORD 영역 - 검사 추가  정보 쓰기 갯수
#define SIENENS_WRITEWORD_INSPADD 3
//지멘스 WORD 영역 - Empty 3 정보 쓰기 갯수
#define SIENENS_WRITEWORD_EMPTY3 2
//지멘스 WORD 영역 - DuplicateNG Cell ID 정보 쓰기 갯수
#define SIENENS_WRITEWORD_DuplicateNGCellID 64


class CSiemensPlcIo : public CPlcImp
{
public:
	CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort);
	~CSiemensPlcIo();

	//PLC 읽기 Data
	short m_ReadBitData[SIENENS_READBIT];
	short m_ReadWordData[SIENENS_WRITEWORD_MAX];

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
};

