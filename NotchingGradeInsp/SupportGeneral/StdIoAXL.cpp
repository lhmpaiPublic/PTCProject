#include "pch.h"
#include "StdIoAXL.h"
#include "CONTEC/CDio.h"


CStdIoAXL::CStdIoAXL( WORD MaxPort, DWORD dwDeviceID, long BoardNo /*= 0*/ )
{
	m_dwDeviceID = dwDeviceID;
	m_wMaxPort = MaxPort;
	m_iBoardNo = BoardNo;
	m_nModuleNo = 0;

	::InitializeCriticalSection(&m_csDio);

}


CStdIoAXL::~CStdIoAXL(void)
{
	if (m_bOpened == TRUE) {
		Close();
	}
}


int CStdIoAXL::Open(BOOL debug /*=FALSE*/)
{
	DWORD Code = AxlOpenNoReset(7);

	if (Code == AXT_RT_SUCCESS)
	{
		TRACE("Library is initialized .\n");
		DWORD dwStatus;

		// Inspect if DIO module exsits
		Code = AxdInfoIsDIOModule(&dwStatus);
		if (dwStatus == STATUS_EXIST)
		{
			TRACE("DIO module exists.\n");
			long IModuleCounts;
			Code = AxdInfoGetModuleCount(&IModuleCounts);
			if (Code == AXT_RT_SUCCESS)
				TRACE("Number of DIO module: %d\n", IModuleCounts);
			else
				TRACE("AxdInfoGetModuleCount() : ERROR code Ox%x\n", Code);

			long IInputCounts;
			long IOutputCounts;

			long iBoardNo;
			long IModulePos;
			DWORD dwDeviceID;

			BOOL bFindBoard = FALSE;
			for (int ModuleNo = 0; ModuleNo < IModuleCounts; ModuleNo++)
			{
				AxdInfoGetInputCount(ModuleNo, &IInputCounts);
				AxdInfoGetOutputCount(ModuleNo, &IOutputCounts);

				if (AxdInfoGetModule(ModuleNo, &iBoardNo, &IModulePos, &dwDeviceID) == AXT_RT_SUCCESS)
				{
					if ((dwDeviceID == m_dwDeviceID) && (iBoardNo == m_iBoardNo)) {
						m_nModuleNo = ModuleNo;
						m_bOpened = TRUE;
						bFindBoard = TRUE;
						break;
					}
				}
			}
			if (!bFindBoard) {
				AfxMessageBox("DIO모듈을 찾지 못했습니다.");
				return FALSE;
			}
		}
		else
		{
			AfxMessageBox("DIO모듈이 존재하지 않습니다.");
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox("DIO 초기화에 실패하였습니다.");
		return FALSE;
	}
//TEST
//	BYTE value1=0xff, value2=0xff;
//	OutPort(0, value1);
//	OutPort(1, value2);
//	Inport(0, &value1);
//	Inport(1, &value2);
//	OutPortBit(0, 7, TRUE);
//	OutPortBit(1, 0, TRUE);
//	BOOL bBit1 = 0, bBit2 = 0;
//	for (int i = 0; i < 8; i++)
//	{
//		InportBit(0, i, &bBit1);
//		InportBit(1, i, &bBit2);
//	}
//TEST
	return TRUE;
}


int CStdIoAXL::Close()
{
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoAXL::Close() Not Opened");
	}

	if (m_bDebugMode == TRUE) {
		m_bOpened = FALSE;
		return(0);
	}

	for (int i = 0; i < m_wMaxPort; i++) {
		if (OutPort(i, 0x00) != 0) {
			break;
		}
	}

	long ret = CloseDevice();
	if (ret != TRUE) {
		__super::ErrorProc((short)ret, "CStdIoAXL::Close() is FAILED");
		return(-1);
	}

	m_bOpened = FALSE;
	m_dwDeviceID = 0;
	m_iBoardNo = 0;
	m_nModuleNo = 0;

	return(0);
}


BOOL CStdIoAXL::CloseDevice()
{
	// Verify if the integrated library is available to use (If it is initialized)  
	if (AxlIsOpened())
	{
		AxlClose();
		return	TRUE;
	}
	else
	{
		return	FALSE;
	}
}

int CStdIoAXL::OutPort(WORD port, BYTE value)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Outportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d "), port, m_nPortMax);
		//에러출력
		return (-1);
	}

	if (m_bDebugMode == TRUE) {
		m_OutMap[port] = value;
		return(0);
	}

	::EnterCriticalSection(&m_csDio);

	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DO32P:
	case AXT_SIO_DO32T:
	case AXT_SIO_RDO32:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB128MLII:
	{
		iOffset = (port / 2) + (port % 2);
		AxdoWriteOutportByte(m_nModuleNo, iOffset, value);
		break;
	}
	default:
		nRet = -1;
		break;
	}
	::LeaveCriticalSection(&m_csDio);

	return(nRet);
}


int CStdIoAXL::ReadOutport(WORD port, BYTE* value)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::ReadOutport() Not Opened");
		return(-1);
	}


	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d) "), port, m_nPortMax);
		//에러 출력
		return(-1);
	}

	if (m_bDebugMode == TRUE) {
		*value = m_OutMap[port];
		return(0);
	}
	::EnterCriticalSection(&m_csDio);

	DWORD	dwData;
	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DO32P:
	case AXT_SIO_DO32T:
	case AXT_SIO_RDO32:
	case AXT_SIO_RDO16AMLII:
	case AXT_SIO_RDO16BMLII:
	case AXT_SIO_RDO32RTEX:
	case AXT_SIO_DO32T_P:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB32RTEX:
	case AXT_SIO_RDB32T:
	case AXT_SIO_RDB96MLII:
	case AXT_SIO_RDB128MLII:
	case AXT_SIO_RSIMPLEIOMLII:
	{
		iOffset = port / 2;
		AxdoReadOutportWord(m_nModuleNo, iOffset, &dwData);
		if ((port % 2) == 0) {
			*value = (BYTE)(dwData & 0xff);
		}
		else {
			*value = (BYTE)((dwData >> 8) & 0xff);
		}
		break;
	}
	default:
		nRet = -1;
		break;
	}
	::LeaveCriticalSection(&m_csDio);

	return(nRet);
}


int CStdIoAXL::Inport(WORD port, BYTE* value)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Inportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d) "), port, m_nPortMax);
		//에러 출력
		return (-1);
	}

	if (m_bDebugMode) {
		*value = m_InMap[port];
		return(0);
	}
	::EnterCriticalSection(&m_csDio);

	DWORD	dwData;
	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DI32:
	case AXT_SIO_RDI32:
	case AXT_SIO_RDI16MLII:
	case AXT_SIO_RDI32RTEX:
	case AXT_SIO_DI32_P:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB32RTEX:
	case AXT_SIO_RDB32T:
	case AXT_SIO_RDB96MLII:
	case AXT_SIO_RDB128MLII:
	case AXT_SIO_RSIMPLEIOMLII:
	{
		iOffset = port / 2;
		AxdiReadInportWord(m_nModuleNo, iOffset, &dwData);
		if ((port % 2) == 0) {
			*value = (BYTE)(dwData & 0xff);
		}
		else {
			*value = (BYTE)((dwData >> 8) & 0xff);
		}
		break;
	}
	default:
		nRet = -1;
		break;
	}
	::LeaveCriticalSection(&m_csDio);
	return(nRet);
}


// 

int CStdIoAXL::OutPortWord(WORD value)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Outportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(0) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d "), 0, m_nPortMax);
		//에러출력
		return (-1);
	}
	::EnterCriticalSection(&m_csDio);

	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DO32P:
	case AXT_SIO_DO32T:
	case AXT_SIO_RDO32:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB128MLII:
	{
		iOffset = 0;
	// 21.08.31 Ahn Modify Start
	//	AxdoWriteOutportByte(m_nModuleNo, iOffset, value);
		AxdoWriteOutportWord(m_nModuleNo, iOffset, value);
	// 21.08.31 Ahn Modify End
		break;
	}
	default:
		nRet = -1;
		break;
	}
	::LeaveCriticalSection(&m_csDio);
	return(nRet);
}


int CStdIoAXL::ReadOutportWord(WORD* value)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::ReadOutport() Not Opened");
		return(-1);
	}

	if (__super::IsPortEorror(0) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d) "), 0, m_nPortMax);
		//에러 출력
		return(-1);
	}
	::EnterCriticalSection(&m_csDio);

	DWORD	dwData;
	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DO32P:
	case AXT_SIO_DO32T:
	case AXT_SIO_RDO32:
	case AXT_SIO_RDO16AMLII:
	case AXT_SIO_RDO16BMLII:
	case AXT_SIO_RDO32RTEX:
	case AXT_SIO_DO32T_P:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB32RTEX:
	case AXT_SIO_RDB32T:
	case AXT_SIO_RDB96MLII:
	case AXT_SIO_RDB128MLII:
	case AXT_SIO_RSIMPLEIOMLII:
	{
		iOffset = 0;
		AxdoReadOutportWord(m_nModuleNo, iOffset, &dwData);
		*value = (WORD)dwData;

		break;
	}
	default:
		nRet = -1;
		break;
	}
	::LeaveCriticalSection(&m_csDio);
	return(nRet);
}


int CStdIoAXL::InportWord(WORD* value)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Inportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(0) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d) "), 0, m_nPortMax);
		//에러 출력
		return (-1);
	}
	::EnterCriticalSection(&m_csDio);

	DWORD	dwData;
	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DI32:
	case AXT_SIO_RDI32:
	case AXT_SIO_RDI16MLII:
	case AXT_SIO_RDI32RTEX:
	case AXT_SIO_DI32_P:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB32RTEX:
	case AXT_SIO_RDB32T:
	case AXT_SIO_RDB96MLII:
	case AXT_SIO_RDB128MLII:
	case AXT_SIO_RSIMPLEIOMLII:
	{
		iOffset = 0 ;
		AxdiReadInportWord(m_nModuleNo, iOffset, &dwData);
		*value = (WORD)dwData;
		break;
	}
	default:
		nRet = -1;
		break;
	}
	::LeaveCriticalSection(&m_csDio);

	return(nRet);
}

#if 0
int CStdIoAXL::OutPortBit(WORD port, int nIndex, BOOL bit)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Outportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d "), port, m_nPortMax);
		//에러출력
		return (-1);
	}

	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DO32P:
	case AXT_SIO_DO32T:
	case AXT_SIO_RDO32:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB128MLII:
	{
		iOffset = ((port / 2) + (port % 2) * 8) + nIndex;
		AxdoWriteOutportBit(m_nModuleNo, iOffset, (DWORD)bit);
		break;
	}
	default:
		nRet = -1;
		break;
	}

	return(nRet);
}
#endif

#if 0 
int CStdIoAXL::InportBit(WORD port, int nIndex, BOOL* bit)
{
	int nRet = 0;
	if (m_bOpened == FALSE) {
		__super::ErrorProc(-1, "CStdIoApi::Inportb() Not Opened");
		return (-1);
	}

	if (__super::IsPortEorror(port) == TRUE) {
		CString strError;
		strError.Format(_T("입출력 에러 ( 포트가 오버되었습니다. : %d, MaxPort : %d) "), port, m_nPortMax);
		//에러 출력
		return (-1);
	}

	DWORD	dwData;
	long	iOffset = 0;
	switch (m_dwDeviceID)
	{
	case AXT_SIO_DI32:
	case AXT_SIO_RDI32:
	case AXT_SIO_RDI16MLII:
	case AXT_SIO_RDI32RTEX:
	case AXT_SIO_DI32_P:
	case AXT_SIO_DB32P:
	case AXT_SIO_DB32T:
	case AXT_SIO_RDB32RTEX:
	case AXT_SIO_RDB32T:
	case AXT_SIO_RDB96MLII:
	case AXT_SIO_RDB128MLII:
	case AXT_SIO_RSIMPLEIOMLII:
	{
		iOffset = ((port / 2) + (port % 2) * 8) + nIndex;
		AxdiReadInportBit(m_nModuleNo, iOffset, &dwData);
		break;
	}
	default:
		nRet = -1;
		break;
	}
	*bit = (BOOL)dwData;

	return(nRet);
}
#endif
