// CLightRS232Dlg.cpp : implementation file
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CLightRS232Dlg.h"
#include "afxdialogex.h"

#include "Light232.h" 
#include "GlobalData.h"

// CLightRS232Dlg dialog

IMPLEMENT_DYNAMIC(CLightRS232Dlg, CDialogEx)

CLightRS232Dlg::CLightRS232Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RS232, pParent)
{
	getRS232Info();
}

CLightRS232Dlg::~CLightRS232Dlg()
{
}

LRESULT CLightRS232Dlg::OnReceive(WPARAM length, LPARAM lpara)
{
	CString str;
	char data1[20000];
	char data2[20000];

	if (m_comm_top)
	{

		m_comm_top->Receive(data1, (int)length); //length ���̸�ŭ�����͸��޴´�.
		data1[length] = _T('\0');
		//	str += _T("\r\n");
		for (int i = 0; i < (int)length; i++)
		{
			str += data1[i];
		}

//		Text.Format(_T("Receive = %s\n"), str);
//		AddText(Text, TextColor, FontSize, FontName);
		str = "";
	}

	if (m_comm_bottom)
	{

		m_comm_bottom->Receive(data2, (int)length); //length ���̸�ŭ�����͸��޴´�.
		data2[length] = _T('\0');
		//	str += _T("\r\n");
		for (int i = 0; i < (int)length; i++)
		{
			str += data2[i];
		}

		//		Text.Format(_T("Receive = %s\n"), str);
		//		AddText(Text, TextColor, FontSize, FontName);
		str = "";
	}

	return 0;
}

LRESULT CLightRS232Dlg::OnThreadClosed(WPARAM length, LPARAM lpara)
{
	return LRESULT();
}

void CLightRS232Dlg::getRS232Info()
{
	// 230922 
// RS232 ������ ������ 

//	m_str_comport = _T("COM1");
//	m_str_baudrate = _T("19200");

	m_str_comport_top = AprData.m_System.m_strRS232_COMPORT_TOP;
	m_str_comport_bottom = AprData.m_System.m_strRS232_COMPORT_BOTTOM;
	m_str_baudrate.Format("%d", AprData.m_System.m_nRS232_Baudrate);
	m_str_DataBits = _T("8 Bit");
	m_str_Parity = _T("None");
	m_str_StopBits = _T("1 Bit");
	m_str_HandShaking = _T("None");
}

BOOL CLightRS232Dlg::ConnectRS232()
{
	if (m_comm_top && comport_state == TRUE) {
		m_comm_top->Close();
		if(m_comm_bottom) m_comm_bottom->Close();

		comport_state = FALSE;
		//Text.Format(_T("Connect = Disconnect \n"));
		//AddText(Text, TextColor, FontSize, FontName);
	}
	else {

		m_comm_top = new CLight232(_T("\\\\.\\") + m_str_comport_top, m_str_baudrate, m_str_Parity, m_str_DataBits, m_str_StopBits);         // initial Comm port
		if (m_comm_top->Create(GetSafeHwnd()) != 0) //�����Ʈ���������������ڵ����ѱ��.
		{
			//			Text.Format(_T("Connect = ��Ʈ����\n"));
			//			AddText(Text, TextColor, FontSize, FontName);

//  		comport_state = TRUE;

			m_comm_bottom = new CLight232(_T("\\\\.\\") + m_str_comport_bottom, m_str_baudrate, m_str_Parity, m_str_DataBits, m_str_StopBits);         // initial Comm port
			if (m_comm_bottom->Create(GetSafeHwnd()) != 0) //�����Ʈ���������������ڵ����ѱ��.
			{
				//			Text.Format(_T("Connect = ��Ʈ����\n"));
				//			AddText(Text, TextColor, FontSize, FontName);
				comport_state = TRUE;

			}
			else {
				comport_state = FALSE;

				//			Text.Format(_T("Connect = Error \n"));
				//			AddText(Text, TextColor, FontSize, FontName);
				return FALSE;
			}

			return TRUE;
		}
		else
		{
			comport_state = FALSE;

			//			Text.Format(_T("Connect = Error \n"));
			//			AddText(Text, TextColor, FontSize, FontName);
			return FALSE;
		}
	}
	return FALSE;
}

BOOL CLightRS232Dlg::send(int unit, char* pSendBuff, char* pRecvBuff, int nCmdLen, int nRecvLen, long lTimeOver)
{
#if 1 
	BYTE* m_pSendBuffer;	
	BYTE* m_pRcvBuffer;
	int		m_SendBuffCnt;	
	int		m_SendCnt;		
	//int		m_RecvCnt;		
	int		m_SendFlag;		
	//char* m_btRecv;
	CString send = "";
	CString rcv = "";
#endif
	
	m_SendBuffCnt = nRecvLen + 3;
	m_pSendBuffer = new BYTE[m_SendBuffCnt + 1];
	m_pRcvBuffer = new BYTE[m_SendBuffCnt + 1];

	memset(m_pSendBuffer, 0x00, sizeof(BYTE) * (m_SendBuffCnt + 1));
	memset(m_pRcvBuffer, 0x00, sizeof(BYTE) * (m_SendBuffCnt + 1));
	// inoue ed 2015.03.23 (A-1)
	m_SendCnt = 0;

	BYTE* ptr = m_pSendBuffer;
	int	c;
	*ptr++ = enSTX;	// [STX]
	for (c = 0; c < nCmdLen; c++) {
		*ptr++ = pSendBuff[c];
	}
	// 21.07.05 Ahn Modify Start
	//	*ptr++ = enETX ;	// [ETX]
	*ptr++ = enCR;
	*ptr++ = enLF;
	// 21.07.05 Ahn Modify End

	m_SendFlag = TRUE;

#if 1 // ���⼭ ����
	if (unit == 0) m_comm_top->Send((char*)m_pSendBuffer, m_SendBuffCnt);
	else if (unit == 1)m_comm_bottom->Send((char*)m_pSendBuffer, m_SendBuffCnt);
	else;

//	Sleep(15 + 5); // Altsystem ��� 19200 ��żӵ� 15ms + 5�߰�
	Sleep(150); // ���� 150 ���� ������ �����ϴ�. kjk
	
	if(unit == 0) m_comm_top->Receive((char*)m_pRcvBuffer, m_SendBuffCnt); //length ���̸�ŭ�����͸��޴´�.
	else if (unit == 1) m_comm_bottom->Receive((char*)m_pRcvBuffer, m_SendBuffCnt); //length ���̸�ŭ�����͸��޴´�.
	else;

	memcpy(pRecvBuff, (char*)m_pRcvBuffer, m_SendBuffCnt);

#endif 

	if (m_pSendBuffer) {
		delete[] m_pSendBuffer;
		m_pSendBuffer = NULL;
	}
	if (m_pRcvBuffer) {
		delete[] m_pRcvBuffer;
		m_pRcvBuffer = NULL;
	}

	m_SendBuffCnt = 0;
	m_SendCnt = 0;

	return FALSE;
}

void CLightRS232Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLightRS232Dlg, CDialogEx)

//	ON_MESSAGE(WM_MYRECEIVE, &CLightRS232Dlg::OnReceive)
	ON_MESSAGE(WM_MYCLOSE, &CLightRS232Dlg::OnThreadClosed)

END_MESSAGE_MAP()


