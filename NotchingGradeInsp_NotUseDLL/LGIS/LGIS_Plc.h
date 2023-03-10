#pragma once

#include <afxmt.h>
#include <cstring>
#include <stdint.h>
#include <string>

#ifdef _WIN32
// WINDOWS socket
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
using X_SOCKET = SOCKET;
using ssize_t = int;

#define X_ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define X_CLOSE_SOCKET(s) closesocket(s)
#define X_ISCONNECTSUCCEED(s) ((s) != SOCKET_ERROR)

#else
// Berkeley socket
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using X_SOCKET = int;

#define X_ISVALIDSOCKET(s) ((s) >= 0)
#define X_CLOSE_SOCKET(s) close(s)
#define X_ISCONNECTSUCCEED(s) ((s) >= 0)
#endif

using SOCKADDR = struct sockaddr;
using SOCKADDR_IN = struct sockaddr_in;

#define MAX_MSG_LENGTH 260

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

#define CONNECTION_ERROR -1

class AFX_EXT_CLASS CLGIS_Plc
{
public:
    CLGIS_Plc(std::string host, uint16_t port = 502);
    ~CLGIS_Plc();

private:
    bool m_bConnected{};
    uint16_t m_nPort{};
    uint32_t m_nMsgId{};
    int m_nSlaveid{};
    std::string m_strHost;

    X_SOCKET m_Socket{};
    SOCKADDR_IN m_Server{};

#ifdef _WIN32
    WSADATA m_Wsadata;
#endif

    int m_nTimeOut{};
    static CCriticalSection m_csNetwork;

public:
    bool m_bErr{};
    int m_nErrNo{};
    std::string m_strErrorMsg;

    bool Connect();
    void Close();

    bool IsConnected();
    void SetSlaveId(int id);
    void SetTimeOut(int nTimeOut);

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
    void BuildRequest(uint8_t* to_send, uint16_t address, int func) const;

    int ReadData(uint16_t address, uint16_t amount, int func);
    int WriteData(uint16_t address, uint16_t amount, int func, const uint16_t* value);

    ssize_t SendData(uint8_t* to_send, size_t length);
    ssize_t ReceiveData(uint8_t* buffer) const;

    void CheckErrorHandle(const uint8_t* msg, int func);

    void SetConnectionError();
    void SetInputError();
};

