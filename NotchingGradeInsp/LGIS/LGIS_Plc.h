#pragma once
#include <string>
#include <stdint.h>

using namespace std;

#define MAX_MSG_LENGTH 260
#define NETWORK_TIME_OUT 500

///Function Code
#define READ_COILS 0x01
#define READ_INPUT_BITS 0x02
#define READ_REGS 0x03
#define READ_INPUT_REGS 0x04
#define WRITE_COIL 0x05
#define WRITE_REG 0x06
#define WRITE_COILS 0x0F
#define WRITE_REGS 0x10

///Exception Codes
#define EX_ILLEGAL_FUNCTION 0x01 // Function Code not Supported
#define EX_ILLEGAL_ADDRESS 0x02  // Output Address not exists
#define EX_ILLEGAL_VALUE 0x03    // Output Value not in Range
#define EX_SERVER_FAILURE 0x04   // Slave Deive Fails to process request
#define EX_ACKNOWLEDGE 0x05      // Service Need Long Time to Execute
#define EX_SERVER_BUSY 0x06      // Server Was Unable to Accept MB Request PDU
#define EX_NEGATIVE_ACK 0x07
#define EX_MEM_PARITY_PROB 0x08
#define EX_GATEWAY_PROBLEMP 0x0A // Gateway Path not Available
#define EX_GATEWAY_PROBLEMF 0x0B // Target Device Failed to Response
#define EX_BAD_DATA 0XFF         // Bad Data lenght or Address

#define PLC_OK				0x00
#define CONNECTION_ERROR	-1
#define RECEIVE_TIME_OUT	-2

/// network state update
#define WM_PLC_NETWORK_UPDATE	(WM_APP + 1000)

typedef struct tagPLCNETWORKSTATUS
{
	tagPLCNETWORKSTATUS()
		: strIP(_T(""))
		, nPort(0)
		, nConnected(0)
	{
	}

	CString strIP;
	UINT nPort;
	int nConnected; // -1 : re-try failed, 0: disconnected, 1 : connected

} PLCNETWORKSTATUS, * LPPLCNETWORKSTATUS;

class AFX_EXT_CLASS CLGIS_Plc
{
public:
	CLGIS_Plc(CString strIP, int nPort, int nReConnetTimeOut = 1000, CWnd* pReceiveMsgWnd = NULL);
	~CLGIS_Plc();

	
private:
	CString m_strIP; 
	int m_nPort;
	uint32_t m_nMsgId{};
	int m_nSlaveid{};
	
	uint8_t m_ReceiveData[MAX_MSG_LENGTH];

public:
	bool m_bErr{};
	int m_nErrNo{};
	CString m_strErrorMsg;

	bool CheckConnection();
	void SetSlaveId(int id);
	void SetReConnetTimeOut(int nTimeOut); // nTimeOut: ms
	void SetReceiveMsgWnd(CWnd* pWnd);

	// read coil                        function 0x01
	int ReadCoils(uint16_t address, uint16_t amount, bool* buffer);
	// read input bits(discrete input)  function 0x02
	int ReadInputBits(uint16_t address, uint16_t amount, bool* buffer);
	// read holding registers           function 0x03
	int ReadHoldingRegisters(uint16_t address, uint16_t amount, uint16_t* buffer);
	// read input registers             function 0x04
	int ReadInputRegisters(uint16_t address, uint16_t amount, uint16_t* buffer);

	// write single coil                function 0x05
	int WriteSingleCoil(uint16_t address, const bool& to_write);
	// write single reg                 function 0x06
	int WriteSingleRegister(uint16_t address, const uint16_t& value);
	// write multiple coils             function 0x0F
	int WriteMultipleCoils(uint16_t address, uint16_t amount, const bool* value);
	// write multiple regs              function 0x10
	int WriteMultipleRegisters(uint16_t address, uint16_t amount, const uint16_t* value);

private:
	// thread 0: ReConnectionMonitoring, thread 1: ReceiveWating
	CWinThread* m_pThread[2];
	bool m_bThreadStatus[2];
	bool m_bUseMsg;

	int m_nReConnetTimeOut;
	CWnd* m_pReceiveMsgWnd;
	
	void InitClient();
	void FinalizeClient();
	bool OpenSocket(CString strIP, int nPort);

	static void CALLBACK CallbackReceive(LPVOID pUserData, LPVOID pData, int nDataLen, CString strIP, UINT nPort);
	void OnReceive(LPVOID pData, int nDataLen, CString strIP, UINT nPort);

	static void CALLBACK CallbackUpdate(LPVOID pUserData, CString strIP, UINT nPort, bool bConnected);
	void OnUpdate(CString strIP, UINT nPort, bool bConnected);

	static UINT ReConnectionMonitoringThread(LPVOID lParam);
	static UINT ReceiveWatingThread(LPVOID lParam);

	void BuildRequest(uint8_t* to_send, uint16_t address, int func) const;

	int ReadData(uint16_t address, uint16_t amount, int func);
	int WriteData(uint16_t address, uint16_t amount, int func, const uint16_t* value);

	int SendData(uint8_t* to_send, size_t length);

	void CheckErrorHandle(const uint8_t* msg, int func);

	bool IsConnected();
	void SetInputError();

};

