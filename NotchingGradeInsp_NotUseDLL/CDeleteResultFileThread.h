#pragma once



class CDeleteResultFileThread
{
public :
	CDeleteResultFileThread();
	~CDeleteResultFileThread();

	void Begin(void);
	void Kill(void);

	enum {
		en_CsvFile = 0,
		en_BinaryFile,
		en_Ok_image,
		en_Ng_image,
		en_RecipeLog,
		en_InspLog,
		en_MaxDelPath,
	};

	static BOOL SaveFinalSaveDate();
	static BOOL CheckDeleteAvilable();

	BOOL IsComplet() { return m_bComplet; };

protected:
	BOOL	m_bKill;
	int		m_nThreadId;
	BOOL	m_bDeleteRun;
	BOOL	m_bComplet;					// 22.07.11 Ahn Add Start 
	CWinThread* m_pThread;				//!< ƒXƒŒƒbƒh(CWinThread *)x1

	int		m_nTarYear;
	int		m_nTarMonth;
	int		m_nTarDay;

	SYSTEMTIME m_stTarTime;

protected:
	static UINT CtrlDeleteRsltFileThread(LPVOID pParam);
	static int DeleteTargetDirectory(CString strTarPath, BOOL *bStop );	
	static int DeleteDir_byDay(CString strTarPath, SYSTEMTIME stDelTime, BOOL bSameMonth, BOOL* bStop);
	static int DeleteDir_byMonth(CString strTarPath, SYSTEMTIME stDelTime,  BOOL* bStop);
	static BOOL GetYearAndMonthFromPath(CString strPath, int &nYear, int &nMonth);
	static BOOL GetDayFromPath(CString strPath, int& nDay);

	static BOOL CheckDiskCapacityOver(double dSize);
	static BOOL GetMostOlderDateOfResult(int & nYearMonth, int &nDay) ;
	static BOOL DeleteDir_CapaOverMode( int nYearMonth, int nDay, BOOL* pbStop);
	static BOOL DeleteEmptyDir(CString strPath);
};

