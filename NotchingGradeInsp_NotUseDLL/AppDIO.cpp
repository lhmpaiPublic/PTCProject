#include "pch.h"
#include "AppDIO.h"
#include "NotchingGradeInsp.h"
#include "StdIoCtrl.h"

CCriticalSection	CAppDIO::m_cs;
CAppDIO::CAppDIO(void)
{

}

CAppDIO::~CAppDIO(void)
{
	
}

BOOL CAppDIO::IsDioDebugMode()
{
	return(theApp.m_pIoCtrl->IsDebugMode());
}

int	CAppDIO::InputBit(DWORD bitpat, BOOL* bit)
{
	CSingleLock lock(&m_cs, TRUE);
	int nRet = 0;

	*bit = 0;
	if (bitpat == (DWORD)-1 || theApp.m_pIoCtrl == NULL) {
		return (-1);
	}

	WORD port = 0;
	BYTE mask = (BYTE)(bitpat) & 0xff;;
	if (((bitpat) & 0x0000ff00) > 0) {
		port++;
		mask = (BYTE)(bitpat >> 8) & 0xff;
	}
	if (((bitpat) & 0x00ff0000) > 0) {
		port++;
		mask = (BYTE)(bitpat >> 16) & 0xff;
	}
	if (((bitpat) & 0xff000000) > 0) {
		port++;
		mask = (BYTE)(bitpat >> 24) & 0xff;
	}

	BYTE value;
	nRet = theApp.m_pIoCtrl->Inport(port, &value);

	*bit = (value & mask) ? TRUE : FALSE;

	return (nRet);
}

int	CAppDIO::OutputBit(DWORD bitpat, BOOL bit)
{
	CSingleLock lock(&m_cs, TRUE);
	int nRet = 0;

	if (bitpat == (DWORD)-1 || theApp.m_pIoCtrl == NULL) {
		return (-1);
	}

	WORD port = 0;
	BYTE mask = (BYTE)(bitpat) & 0xff;;
	if (((bitpat) & 0x0000ff00) > 0) {
		port++;
		mask = (BYTE)(bitpat >> 8) & 0xff;
	}
	if (((bitpat) & 0x00ff0000) > 0) {
		port++;
		mask = (BYTE)(bitpat >> 16) & 0xff;
	}
	if (((bitpat) & 0xff000000) > 0) {
		port++;
		mask = (BYTE)(bitpat >> 24) & 0xff;
	}


	BYTE value;
	nRet = theApp.m_pIoCtrl->ReadOutport(port, &value);
	if (bit) {
		value |= mask;
	}
	else {
		value &= ~mask;
	}

	nRet = theApp.m_pIoCtrl->OutPort(port, value);

	return (nRet);
}

int	CAppDIO::InputByte(DWORD bitpat, BYTE* data)
{
	CSingleLock lock(&m_cs, TRUE);
	int nRet = 0;

	*data = 0;
	if (bitpat == (DWORD)-1 || theApp.m_pIoCtrl == NULL) {
		return (-1);
	}

	WORD port = 0;
	if (((bitpat) & 0x0000ff00) > 0) port++;
	if (((bitpat) & 0x00ff0000) > 0) port++;
	if (((bitpat) & 0xff000000) > 0) port++;

	BYTE value;
	nRet = theApp.m_pIoCtrl->Inport(port, &value);

	*data = value;

	return (nRet);
}


int CAppDIO::OutputWord(WORD outValue)
{
	CSingleLock lock(&m_cs, TRUE);
	int nRet = 0;

	if (theApp.m_pIoCtrl == NULL) {
		return (-1);
	}

	nRet = theApp.m_pIoCtrl->OutPortWord(outValue) ;

	return (nRet);
}

int	CAppDIO::InputWord(WORD* outValue)
{
	CSingleLock lock(&m_cs, TRUE);
	int nRet = 0;

	*outValue = 0;
	if ( theApp.m_pIoCtrl == NULL) {
		return (-1);
	}

	WORD value;
	nRet = theApp.m_pIoCtrl->InportWord( &value );
	*outValue = value;

	return (nRet);

}