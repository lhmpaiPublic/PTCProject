#pragma once
class CDeleteSetting
{

public :
	CDeleteSetting();
	~CDeleteSetting();
	int SetUseBackup(BOOL nUseBackup);
	BOOL GetUseBackup();
	int SetBackupDate(int nValue);
	int GetBackupDate();
	int SetDeleteDate(int nValue);
	int GetDeleteDate();
	int SetStartTime(int nValue);
	int GetStartTime();
	int SetStopTime(int nValue);
	int GetStopTime();
	int SetDestPath( CString strPath ) ;
	BOOL GetStopDuringRun();
	int SetStopDuringRun(BOOL bValue);
	CString GetDestPath();

	int FileCtrl(int nMode);

private:
	BOOL m_bUseBackup;
	BOOL m_bStopDuringRun;
	int m_nBackupDate;
	int m_nDeleteDate;
	int m_nStartTime;
	int m_nStopTime;
	CString m_strBackupDestPath;

public :
	int LoadParameter();
	int SaveParameter();
};

