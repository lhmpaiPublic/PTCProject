
#pragma once

#include <afxmt.h>
 
#define MAXBUF         50000
#define InBufSize      50000
#define OutBufSize     50000
#define ASCII_XON      0x11
#define ASCII_XOFF     0x13
#define WM_MYRECEIVE   (WM_USER+1) //데이터 수신 메시지
#define WM_MYCLOSE     (WM_USER+2) // 종료 메시지

// CLight232 명령 대상입니다.

class CLight232 : public CCmdTarget
{
	DECLARE_DYNAMIC(CLight232)

public:
	CLight232();
	virtual ~CLight232();

	//Attubutes
public:
        HANDLE  m_hComDev;
        HWND    m_hWnd;
 
        BOOL    m_bIsOpenned;
        CString m_sComPort;
        CString m_sBaudRate;
        CString m_sParity;
        CString m_sDataBit;
        CString m_sStopBit;
        BOOL    m_bFlowChk;
 
        OVERLAPPED m_OLR,m_OLW;
        char    m_sInBuf[MAXBUF*2];
        int            m_nLength;
        CEvent* m_pEvent;
 
        // Operations
public:
        void Clear();
        int Receive(LPSTR inbuf, int len);
        BOOL Send(LPCTSTR outbuf, int len);
        BOOL Send(LPSTR outbuf, int len);
        BOOL Create(HWND hWnd);
        void HandleClose();
        void Close();
        void ResetSerial();
        CLight232(CString port,CString baudrate,CString parity,CString databit,CString stopbit);

protected:
	DECLARE_MESSAGE_MAP()
};


