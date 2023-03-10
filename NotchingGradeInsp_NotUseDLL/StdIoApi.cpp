#include "pch.h"
#include "StdIoApi.h"
#include "CONTEC/CDio.h"

CStdIoApi::CStdIoApi(WORD MaxPort)
{
	m_DeviceID = 0;
	m_MaxPort = MaxPort;
}


CStdIoApi::~CStdIoApi(void)
{
	if (m_bOpened == TRUE) {
		Close();
	}
}

int CStdIoApi::Open(BOOL debug /*=FALSE*/)
{
	if (m_bOpened == TRUE) {
		__super::ErrorProc(-1, "CStdIoApi::Open() Is Opened");
		return(-1);
	}

	m_bDebugMode = debug;
	if (m_bDebugMode == TRUE) {
		m_bOpened = TRUE;
		return(0);
	}

	long ret = DioInit("DIO000", &m_DeviceID);
	if (ret != DIO_ERR_SUCCESS) {
		__super::ErrorProc((short)ret, "CStdIoApi::Open() is FAILED");
		return(-1);
	}
	m_bOpened = TRUE;

	for (int i = 0; i < m_MaxPort; i++) {
		if (OutPort(i, 0x00) != 0) {
			break;
		}
	}

	return(0);
}


int CStdIoApi::Close()
{
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Close() Not Opened");
	}

	if (m_bDebugMode == TRUE) {
		m_bOpened = FALSE;
		return(0);
	}

	for (int i = 0; i < m_MaxPort; i++) {
		if (OutPort(i, 0x00) != 0) {
			break;
		}
	}

	long ret = DioExit(m_DeviceID);
	if (ret != DIO_ERR_SUCCESS) {
		__super::ErrorProc((short)ret, "CStdIoApi::Close() is FAILED");
		return(-1);
	}

	m_bOpened = FALSE;
	m_DeviceID = 0;

	return(0);
}


int CStdIoApi::OutPort(WORD port, BYTE value)
{
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Outportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format( _T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d "), port, m_nPortMax);
		//에러출력
		return (-1);
	}

	m_OutMap[port] = value;

	if (m_bDebugMode == TRUE) {
		return(0);
	}

	long ret = DioOutByte(m_DeviceID, port, value);
	if (ret != DIO_ERR_SUCCESS) {
		__super::ErrorProc((short)ret, "CStdIoApi::Outportb() DioOutByte is FAILED");
		return(-1);
	}

	return(0);
}


int CStdIoApi::ReadOutport(WORD port, BYTE* value)
{
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::ReadOutport() Not Opened");
		return(-1);
	}

	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format( _T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d) "), port, m_nPortMax );
		//에러 출력
		return(-1);
	}

	*value = m_OutMap[port];

	if (m_bDebugMode == TRUE) {
		return(0);
	}

	BYTE Buf;
	long ret = DioEchoBackByte(m_DeviceID, port, &Buf);
	if (ret != DIO_ERR_SUCCESS) {
		__super::ErrorProc((short)ret, "CStdIoApi::ReadOutport() DioOutputByte is FAILED");
		return(-1);
	}

	*value = Buf;

	return(0);
}


int CStdIoApi::Inport(WORD port, BYTE* value)
{
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Inportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format( _T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d) "), port, m_nPortMax);
		//에러 출력
		return (-1);
	}

	if (m_bDebugMode) {
		*value = m_InMap[port];
		return(0);
	}

	long ret = DioInpByte(m_DeviceID, port, value);
	if (ret != DIO_ERR_SUCCESS) {
		__super::ErrorProc((short)ret, "CStdIoApi::Inportb() DioInpByte is FAILED");
		return(-1);
	}

	return(0);
}
