#pragma once


// CLightRS232Dlg dialog

class CLight232;

class CLightRS232Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLightRS232Dlg)

public:
	CLightRS232Dlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLightRS232Dlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RS232 };
#endif

	LRESULT OnReceive(WPARAM length, LPARAM lpara);
	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);

#if 1 //230922 kjk
	void getRS232Info();
	BOOL ConnectRS232();
	//int SendNRecvRS232(int unit, char* pSendBuff, char* pRecvBuff, int nCmdLen, int nRecvLen, long lTimeOver);

	CLight232* m_comm_top;
	CLight232* m_comm_bottom;

	BOOL comport_state;
	CString m_str_comport_top;
	CString m_str_comport_bottom;
	CString m_str_baudrate;
	CString m_str_DataBits;
	CString m_str_Parity;
	CString m_str_StopBits;
	CString m_str_HandShaking;

	BOOL send(int unit, char* pSendBuff, char* pRecvBuff, int nCmdLen, int nRecvLen, long lTimeOver);
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	enum {
		enSTX = 0x4C,
		enETX = 0x03,
		enCR = 0x0D,
		enLF = 0x0A,
		enACK = 0x4C,
	};
};
