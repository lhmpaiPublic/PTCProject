#include "pch.h"
#include "CDeleteSetting.h"
#include "GlobalData.h"

CDeleteSetting::CDeleteSetting()
{
}
CDeleteSetting::~CDeleteSetting()
{
}

int CDeleteSetting::SetUseBackup(BOOL nUseBackup)
{
	int nRet = 0;
	return nRet;
}

BOOL CDeleteSetting::GetUseBackup()
{
	BOOL bRet = FALSE;
	return bRet;
}

int CDeleteSetting::SetBackupDate(int nValue)
{
	int nRet = 0;
	m_nBackupDate = nValue;
	return nRet;
}

int CDeleteSetting::GetBackupDate()
{
	return m_nBackupDate;
}

int CDeleteSetting::SetDeleteDate(int nValue)
{
	int nRet = 0;
	m_nDeleteDate = nValue;
	return nRet;
}

int CDeleteSetting::GetDeleteDate()
{
	return m_nDeleteDate;
}

int CDeleteSetting::SetStartTime(int nValue)
{
	int nRet = 0;
	m_nStartTime = nValue ;
	return nRet;
}

int CDeleteSetting::GetStartTime()
{
	return m_nStartTime ;
}

int CDeleteSetting::SetStopTime(int nValue)
{
	int nRet = 0;
	m_nStopTime = nValue;
	return nRet;
}

int CDeleteSetting::GetStopTime()
{
	return m_nStopTime;
}

int CDeleteSetting::SetDestPath(CString strPath)
{
	int nRet = 0;
	m_strBackupDestPath = strPath;
	return nRet;
}

CString CDeleteSetting::GetDestPath()
{
	return m_strBackupDestPath;
}

BOOL CDeleteSetting::GetStopDuringRun()
{
	return m_bStopDuringRun;
}

int CDeleteSetting::SetStopDuringRun(BOOL bValue)
{
	m_bStopDuringRun = bValue ;
	return 0;
}

int CDeleteSetting::FileCtrl(int nMode)
{
	CString strFileName;
	strFileName.Format(_T("%s\\DeleteSetting.ini"), AprData.m_strDataPath);

	CString strSection;
	CString strKey;

	strSection = _T("DELETE_AND_BACKUP");

	if (nMode == MODE_READ) {
		char buff[256];

		strKey = _T("USE_BACKUP_MODE");
		::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
		 m_bUseBackup= atoi(buff);

		 strKey = _T("BACKUP_DATE");
		 ::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
		 m_nBackupDate = atoi(buff);

		 strKey = _T("DELETE_DATE");
		 ::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
		 m_nDeleteDate= atoi(buff);

		 strKey = _T("START_TIME");
		 ::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
		 m_nStartTime = atoi(buff);

		 strKey = _T("STOP_TIME");
		 ::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
		 m_nStopTime = atoi(buff);

		 strKey = _T("DESTINATION_PATH");
		 ::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
		 m_strBackupDestPath.Format( _T("%s") , buff);
	}
	else {
		CString strData;
		strSection = _T("DEBUG_SET");
		strKey = _T("USE_BACKUP_MODE");
		strData.Format(_T("%d"), m_bUseBackup );
		::WritePrivateProfileString(strSection, strKey, strData, strFileName);

		strKey = _T("BACKUP_DATE");
		strData.Format(_T("%d"), m_nBackupDate);
		::WritePrivateProfileString(strSection, strKey, strData, strFileName);

		strKey = _T("DELETE_DATE");
		strData.Format(_T("%d"), m_nDeleteDate );
		::WritePrivateProfileString(strSection, strKey, strData, strFileName);

		strKey = _T("START_TIME");
		strData.Format(_T("%d"), m_nStartTime );
		::WritePrivateProfileString(strSection, strKey, strData, strFileName);

		strKey = _T("STOP_TIME");
		strData.Format(_T("%d"), m_nStopTime );
		::WritePrivateProfileString(strSection, strKey, strData, strFileName);

		strKey = _T("DESTINATION_PATH");
		strData.Format(_T("%s"), m_strBackupDestPath );
		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
	}

	return 0;
}


int CDeleteSetting::LoadParameter()
{
	int nRet = 0;

	FileCtrl(MODE_READ);
	return nRet;
}

int CDeleteSetting::SaveParameter()
{
	int nRet = 0;
	FileCtrl(MODE_WRITE);

	return nRet;
}
