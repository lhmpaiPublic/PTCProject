#include "pch.h"
#include "StdIoCtrl.h"
#include "StdIoCCLink.h"
#include "StdIoApi.h"
//#include "StdIoFbi.h"
//#include "StdIoMXComponent.h"
#include "StdIoAXL.h"

int	CStdIoCtrl::m_nPortMax = 0;
int	CStdIoCtrl::m_nRegistMin = 0;
int	CStdIoCtrl::m_nRegistMax = 0;
CStdIoCtrl::eIOTYPE CStdIoCtrl::m_Type = eIO_Ccl;

CStdIoCtrl::CStdIoCtrl()
{
	m_bOpened = FALSE;
	m_bDebugMode = FALSE;

	memset(m_OutMap, 0, sizeof(m_OutMap));
	memset(m_InMap, 0, sizeof(m_InMap));
	memset(m_RegMap, 0, sizeof(m_RegMap));

}


CStdIoCtrl::~CStdIoCtrl()
{

}


CStdIoCtrl* CStdIoCtrl::GetInstance(eIOTYPE type, int PortMax, int SeqNo, int RegMin, int RegMax)
{
	ASSERT(PortMax <= enMaxMapPort);

	CStdIoCtrl* ptr = NULL;

	switch (type) {
		case eIO_Ccl:		ptr = new CStdIoCCLink(PortMax, SeqNo);				break;
		case eIO_Api:		ptr = new CStdIoApi(PortMax);						break;
		//case eIO_Fbi:		ptr = new CStdIoFbi(PortMax);						break;
		//case eIO_MXCtrl:	ptr = new CStdIoMXComponent(PortMax, SeqNo);		break;
		case eIO_AXL:		ptr = new CStdIoAXL(PortMax, AXT_SIO_DB32P, 1 );	break; //pyjtest
	default:
		ASSERT(0); 																break; 
	}

	m_nPortMax = PortMax;
	m_nRegistMin = RegMin;
	m_nRegistMax = RegMax;
	m_Type = type;

	return(ptr);
}


BOOL CStdIoCtrl::IsOpened()
{
	return(m_bOpened);
}


BOOL CStdIoCtrl::IsDebugMode()
{
	return(m_bDebugMode);
}


void CStdIoCtrl::ErrorProc(int nRet, char* cap)
{
	if (m_bDebugMode == TRUE) {
		return;
	}

	CString	cstr;
	cstr.Format( _T("입출력 에러 : 코드 : %d, %xH ) %s "), nRet, nRet & 0xffff, cap);
	//에러 출력
	return;
}


BOOL CStdIoCtrl::IsPortEorror(WORD port)
{
	if (port >= m_nPortMax) {
		return(TRUE);
	}
	return(FALSE);
}


BOOL CStdIoCtrl::IsRegistEorror(WORD Regist)
{
	if (m_nRegistMax == 0) {
		return(FALSE);
	}

	if (Regist < m_nRegistMin) {
		return(TRUE);
	}
	if (Regist > m_nRegistMax) {
		return(TRUE);
	}

	return(FALSE);
}


int CStdIoCtrl::WriteReg(WORD dev, WORD data)
{
	if (m_bDebugMode == TRUE) {
		return(0);
	}
	return(-1);
}

int CStdIoCtrl::WriteReg_Block(WORD dev, WORD* data, int nNumOfData)
{
	if (m_bDebugMode == TRUE) {
		return(0);
	}
	return(-1);
}


int CStdIoCtrl::ReadReg(WORD dev, WORD* data)
{
	if (m_bDebugMode == TRUE) {
		memset(data, 0x00, sizeof(WORD));
		return(0);
	}
	return(-1);
}


void CStdIoCtrl::SetInportMap(WORD port, BYTE value)
{
	m_InMap[port] = value;
}


int CStdIoCtrl::GetLimitPort(eIOTYPE type)
{
	int LimitPort = 0;
	switch (type) {
		case eIO_Ccl:		LimitPort = CStdIoCCLink::enPortMax;		break;
		case eIO_Api:		LimitPort = CStdIoApi::enPortMax;			break;
		//case eIO_Fbi:		LimitPort = CStdIoFbi::enPortMax;			break;
		//case eIO_MXCtrl:	LimitPort = CStdIoMXComponent::enPortMax;	break;
		case eIO_AXL:		LimitPort = CStdIoAXL::enPortMax;			break;
	default:			ASSERT(0); 									break;
	}
	return(LimitPort);
}

int CStdIoCtrl::GetLimitSeqNo(eIOTYPE type)
{
	int LimitSecNo = 0;
	switch (type) {
		case eIO_Ccl:		LimitSecNo = CStdIoCCLink::enSeqNoMax;		break;
		case eIO_Api:		LimitSecNo = 128;							break;
		//case eIO_Fbi:		LimitSecNo = 128;							break;
		//case eIO_MXCtrl:	LimitSecNo = CStdIoMXComponent::enSeqNoMax;	break;
		case eIO_AXL:		LimitSecNo = 128;							break;
	default:			ASSERT(0); 									break;
	}
	return(LimitSecNo);
}