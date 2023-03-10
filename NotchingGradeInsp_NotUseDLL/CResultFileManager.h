#pragma once

#include <vector> 

class CProcFileData
{	
private:
	CString m_strDirectory;
	long m_lFileCount;
	long m_lDelCnt;

	int m_nYear;
	int m_nMonth;
	int m_nDay;

public :
	//CStringList m_strList;
	//CStringList *GetFolderListPtr() {
	//	return &m_strList ; 
	//};

	CString GetDirectory() {
		return m_strDirectory;
	};
	void SetDirectory(CString strDir) {
		m_strDirectory = strDir;
	};

	long GetFileCount() {
		return m_lFileCount;
	};

	void SetFileCount(long lCount) {
		m_lFileCount = lCount;
	};

	void SetDate(int nYear, int nMonth, int nDay) {
		m_nYear = nYear; 
		m_nMonth = nMonth; 
		m_nDay = nDay;
	};

};

class CResultFileManager
{
public :

	CResultFileManager();
	~CResultFileManager();
	typedef std::vector< CProcFileData > _VEC_DIR_DATA;

	int Run();
	BOOL IsRun();
	BOOL IsComplete() {	return m_bComplete ; };
	int MakeFolderList(_VEC_DIR_DATA *pVecDirData, CString strPath, int nCheckTime ) ;

	int BeginDeleteThread();
	int BeginBackupThread();
	int KillDeleteThread();
	int KillBackupThread();

	_VEC_DIR_DATA m_VecDelDirData;
	_VEC_DIR_DATA m_VecBackupDirData;

private :
	BOOL m_bBackup_RunFlag;	// ｻ霖ｦﾁﾟ
	BOOL m_bDelete_RunFlag;	// ｻ霖ｦﾁﾟ
	BOOL m_bComplete;	// 22.04.21 Ahn Add

protected:
	BOOL	m_bBackupRun;
	BOOL	m_bBackup_Kill;
	int		m_nBackup_ThreadId;
	CWinThread* m_pBackup_Thread;				//!< スレッド(CWinThread *)x1

	BOOL	m_bDeleteRun;
	BOOL	m_bDelete_Kill;
	int		m_nDelete_ThreadId;
	CWinThread* m_pDelete_Thread;				//!< スレッド(CWinThread *)x1

protected:
	static UINT CtrlRsltFileDeleteThread(LPVOID pParam);
	static UINT CtrlRsltFileBackupThread(LPVOID pParam);

};

