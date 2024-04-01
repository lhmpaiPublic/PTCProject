#pragma once

#ifdef _DEBUG
#pragma comment(lib, "LGISD.lib")
#else
#pragma comment(lib, "LGIS.lib")
#endif

class CSiemensPlcIo : public CPlcIoBase
{
public:
	CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort);
	~CSiemensPlcIo();

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
	int OpenPio(void);
	// disconnection network
	void ClosePio(void);
};

