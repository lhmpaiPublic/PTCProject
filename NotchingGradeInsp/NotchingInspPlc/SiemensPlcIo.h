#pragma once

#ifdef _DEBUG
#pragma comment(lib, "LGISD.lib")
#else
#pragma comment(lib, "LGIS.lib")
#endif

//지멘스 BITE 영역 읽기 갯수
#define SIENENS_READBITDATA (MAX_SMS_BITIO_IN)
//지멘스 WORD 영역 읽기 갯수
#define SIENENS_READWORDDATA 33 //(MAX_SMS_WORDIO_IN)
//지멘스 Recipe Name  갯수
#define SIEMENS_READRECIPENAME 4
//지멘스 CELL ID 갯수
#define SIEMENS_READCELLID 10

#define SIENENS_WRITEBITDATA (MAX_SMS_BITIO_OUT)
#define SIENENS_WRITEWORDDATA (MAX_SMS_WORDIO_IN)

class CSiemensPlcIo : public CPlcImp
{
public:
	CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort);
	~CSiemensPlcIo();

	//PLC 읽기 Data
	short m_ReadBitData[SIENENS_READBITDATA];
	short m_ReadWordData[SIENENS_READWORDDATA];

	//PLC 쓰기 Data
	short m_WriteBitData[SIENENS_WRITEBITDATA];
	short m_WriteWordData[SIENENS_WRITEWORDDATA];
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
	void WritePlcDataMake();

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

