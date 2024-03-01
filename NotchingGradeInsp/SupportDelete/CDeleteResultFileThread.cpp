#include "pch.h"
#include "GlobalData.h"
#include "Win32File.h"
#include "CDeleteResultFileThread.h"

#include <io.h>
//#include <direct.h>
//#include <sys/stat.h>
//#include "WinTool.h"



CDeleteResultFileThread::CDeleteResultFileThread()
{
	m_bKill = FALSE;
	m_nThreadId = 0;
	m_pThread = NULL ;				
//	m_strTarDir = strTarDir;
	m_bComplet = FALSE ;

	int nRet = 0;
	int nDay, nMonth, nYear;

	CTime time = CTime::GetCurrentTime();
	nDay = time.GetDay();
	nMonth = time.GetMonth();
	nYear = time.GetYear();

	int nDeletePeriod = AprData.m_System.m_nResultSavePeriod;

	time = time - CTimeSpan( nDeletePeriod , 0, 0, 0);
	nDay = time.GetDay();
	nMonth = time.GetMonth();
	nYear = time.GetYear();

}
CDeleteResultFileThread::~CDeleteResultFileThread()
{
	if (m_pThread != NULL) {
		delete m_pThread;
		m_pThread = NULL;
	}
}

void CDeleteResultFileThread::Begin()
{
	m_bKill = FALSE;

	//	m_DisphWnd = NULL ;

	if (m_pThread == NULL) {

		m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlDeleteRsltFileThread,
			(LPVOID)this,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);
		if (m_pThread != NULL) {
			m_pThread->m_bAutoDelete = FALSE;
			m_pThread->ResumeThread();
		}
	}
}
void CDeleteResultFileThread::Kill(void)
{
	DWORD	dwCode;
	LONG	ret;

	if (m_pThread != NULL) {
		ret = ::GetExitCodeThread(m_pThread->m_hThread, &dwCode);
		if (ret && dwCode == STILL_ACTIVE) {
			m_bKill = TRUE;
			WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		}
		delete m_pThread;
		m_pThread = NULL;
	}
}

// 22.07.22 Ahn Add Start
BOOL CDeleteResultFileThread::CheckDiskCapacityOver(double dSize)
{
	BOOL bRet = FALSE;

	double dDiskTotal;
	double dDiskFree;
	double dDiskPercent;
	CWin32File::GetDiskSpace(AprData.m_strResultPath, &dDiskTotal, &dDiskFree, &dDiskPercent);

	if ( dDiskPercent > dSize ) {
		bRet = TRUE;
	}

	return bRet;
}
BOOL CDeleteResultFileThread::GetMostOlderDateOfResult(int& nYearMonth, int& nDay)
{
	int nRet = 0 ;

	// 용량이 큰 Image Folder에서 찾음.
	CString strPath ;
	CWin32File winFile;

	CStringList strYmList;
	CStringList strDateList;
	// 연월 폴더 검색
	int nMinYm = 999999;
	BOOL bFoundYm = FALSE;

	int nMinDay = 31;
	BOOL bFoundDay = FALSE;

	for (int i = 0; i < en_MaxDelPath; i++) {

		CString strPath;
		switch (i) {
		case	en_CsvFile:
			strPath = AprData.m_strFeederPath;
			break;
		case	en_BinaryFile:
			strPath = AprData.m_strResultPath + _T("\\BINARY");
			break;
		case	en_Ok_image:
			strPath = AprData.m_strImagePath + _T("\\OK");
			break;
		case	en_Ng_image:
			strPath = AprData.m_strImagePath + _T("\\NG");
			break;
		case	en_RecipeLog:
			strPath = AprData.m_strResultPath + _T("\\RECIPE_LOG");
			break;
		case	en_InspLog:
			strPath = AprData.m_strResultPath + _T("\\LOG");
			break;
		}
		winFile.GetFolderList(strPath, strYmList);
		POSITION pos = strYmList.GetHeadPosition();
		CString strYearMonth;
		for (; pos != NULL; ) {
			strYearMonth = strYmList.GetNext(pos);
			int nNowYm = atoi(strYearMonth);
			if (nNowYm <= nMinYm) {
				nMinYm = nNowYm;
				bFoundYm = TRUE;
			}
		}
		// 일 폴더 검색
		CString strDatePath;
		CString strDate;
		if (bFoundYm == TRUE) {
			strPath.Format(_T("%s\\%06d"), strPath, nMinYm);
			winFile.GetFolderList(strPath, strDateList);
			POSITION pos = strDateList.GetHeadPosition();
			for (; pos != NULL; ) {
				strDate = strDateList.GetNext(pos);
				int nNowDay = atoi(strDate);
				if (nNowDay <= nMinDay) {
					nMinDay = nNowDay;
					bFoundDay = TRUE;
				}
			}
		}
		if (bFoundDay && bFoundYm) {
			nDay = nMinDay;
			nYearMonth = nMinYm;
		}
		else if ((bFoundYm == TRUE) && (bFoundDay == FALSE)) {
			// 날짜 없이 연월 만 있는 경우 해당 디렉토리 삭제.
			::RemoveDirectory(strPath);
		}
	}

	return bFoundDay;
}

BOOL CDeleteResultFileThread::DeleteDir_CapaOverMode(int nYearMonth, int nDay, BOOL* pbStop)
{
	CString strDate;
	strDate.Format(_T("%06d\\%02d"), nYearMonth, nDay);
	for (int i = 0; i < en_MaxDelPath; i++) {

		if ( *pbStop == TRUE) {
			break;
		}

		CString strPath;
		switch (i) {
		case	en_CsvFile:
			strPath = AprData.m_strFeederPath;
			break;
		case	en_BinaryFile:
			strPath = AprData.m_strResultPath + _T("\\BINARY");
			break;
		case	en_Ok_image:
			strPath = AprData.m_strImagePath + _T("\\OK");
			break;
		case	en_Ng_image:
			strPath = AprData.m_strImagePath + _T("\\NG");
			break;
		case	en_RecipeLog:
			strPath = AprData.m_strResultPath + _T("\\RECIPE_LOG");
			break;
		case	en_InspLog :
			strPath = AprData.m_strResultPath + _T("\\LOG");
			break;
		}

		CString strDeleteFile;
		strDeleteFile.Format( _T("%s\\%s"), strPath , strDate );

		DeleteTargetDirectory(strDeleteFile, pbStop);

		int nFolderCount = CWin32File::GetFolderCount(strDeleteFile);
		if ( nFolderCount <= 0) {
			::RemoveDirectory(strDeleteFile);
			CString strDelYm;
			strDelYm.Format(_T("%s\\%06d"), strPath, nYearMonth);
			nFolderCount = CWin32File::GetFolderCount(strDelYm);
			if (nFolderCount <= 0) {
				::RemoveDirectory(strDelYm);
				AprData.SaveDebugLog_Format(_T("[ Delete Logs ] Reason : DiskCapaOver = %s"), strDelYm);
			}
			AprData.SaveDebugLog_Format(_T("[ Delete Logs ] Reason : DiskCapaOver = %s"), strDeleteFile );

		}
	}

	return TRUE ;
}

// 22.07.22 Ahn Add End

UINT CDeleteResultFileThread::CtrlDeleteRsltFileThread(LPVOID pParam)
{
	CDeleteResultFileThread* pThis = (CDeleteResultFileThread*)pParam;

	//while (1) 
	int nRet = 0 ;

	BOOL* pbStop = &(pThis->m_bKill) ;

	SYSTEMTIME sysTime;
	int nDeleteDay;
	nDeleteDay = AprData.m_System.m_nResultSavePeriod ;

	CTime time = CTime::GetCurrentTime();
	time = time - CTimeSpan(nDeleteDay, 0, 0, 0);
	sysTime.wYear = time.GetYear();
	sysTime.wMonth = time.GetMonth();
	sysTime.wDay = time.GetDay();

	// 용량이 80%를 초과하면 오래된 날짜 순으로 삭제함.
	BOOL bDiskCapaOver = FALSE;
	double dSize = AprData.m_System.m_nDiskCapacity; //80.0 ;
	bDiskCapaOver = CheckDiskCapacityOver( dSize ) ;


	AprData.SaveDebugLog_Format(_T("[ Delete Logs ] Period(day) = %d, Disk Capa = %d"), AprData.m_System.m_nResultSavePeriod, AprData.m_System.m_nDiskCapacity);


	if( bDiskCapaOver == TRUE ){
		// 가장 오래된 날짜를 찾아 해당 날짜를 삭제함.
		if (AprData.m_System.m_nDiskCapacity <= 0)
			return 0;

		int nYearMonth;
		int nDay ;
		while ( bDiskCapaOver ){
			if (*pbStop == TRUE) {
				break;
			}

			BOOL bFound = GetMostOlderDateOfResult(nYearMonth, nDay);

			if (bFound == TRUE) {
				DeleteDir_CapaOverMode(nYearMonth, nDay, pbStop);
			}
			bDiskCapaOver = CheckDiskCapacityOver(dSize);
		}
	}
	else {
		if (AprData.m_System.m_nResultSavePeriod <= 0)
			return 0;


		for (int i = 0; i < en_MaxDelPath; i++){
			if (pThis == NULL) {
				break;
			}
			if (pThis->m_bKill == TRUE) {
				break;
			}

			CString strPath;
			switch (i) {
			case	en_CsvFile:
				strPath = AprData.m_strFeederPath;
				break;
			case	en_BinaryFile:
				strPath = AprData.m_strResultPath + _T("\\BINARY");
				break;
			case	en_Ok_image:
				strPath = AprData.m_strImagePath + _T("\\OK");
				break;
			case	en_Ng_image:
				strPath = AprData.m_strImagePath + _T("\\NG");
				break;
				// 22.07.22 Ahn Add Start
			case	en_RecipeLog:
				strPath = AprData.m_strResultPath + _T("\\RECIPE_LOG");
				break;
			case	en_InspLog :
				strPath = AprData.m_strResultPath + _T("\\LOG");
				break;
				// 22.07.22 Ahn Add End
			}

			nRet |= DeleteDir_byMonth(strPath, sysTime, pbStop);
			Sleep(1);
		}
	}

	
	bDiskCapaOver = CheckDiskCapacityOver(dSize);

	if ( (nRet >= 0) && (bDiskCapaOver == FALSE)){
		// 결과 저장.
		pThis->m_bComplet = TRUE;
	}
	else {
		pThis->m_bComplet = FALSE ;
	}

	return 0 ;
}

// 22.07.11 Ahn Add Start
int CDeleteResultFileThread::DeleteTargetDirectory(CString strTarPath, BOOL* pbStop)
{
	ASSERT(pbStop);

	int nRet = 0;

	CString	strPath = strTarPath;
	int	srclen = strPath.GetLength() + 10;

	if (strPath.GetLength() <= 0) {
		return (-1);
	}
	if (_access(strPath.GetBuffer(0), 0) != 0) {
		strPath.ReleaseBuffer();
		return (-2);
	}
	strPath.ReleaseBuffer();

	int	iRet = 0;
	CFileFind find;
	CString strPathName = strPath;
	strPathName.TrimRight('\\');
	strPathName += _T("\\*.*");
	BOOL bFind = find.FindFile(strPathName);

	while (bFind) {
		bFind = find.FindNextFile();

		if (*pbStop == TRUE) break;

		if (find.IsDots()) {
			continue;
		}

		CString strDeleteFile = strPath;
		strDeleteFile.TrimRight('\\');
		strDeleteFile += _T("\\") + find.GetFileName();
		if (find.IsDirectory()) {
			DeleteTargetDirectory(strDeleteFile, pbStop);
		}
		else {
			::DeleteFile(strDeleteFile);
		}
	}
	find.Close();
	::RemoveDirectory(strPath);

	return nRet;
}


int CDeleteResultFileThread::DeleteDir_byDay(CString strTarPath, SYSTEMTIME stDelTime, BOOL bSameMonth, BOOL* pbStop)
{
	ASSERT(pbStop);
	// 경로는 YYYYMM/DD 형태로 로 이루어 져있어야함.

	int nRet = 0 ; 

	CString	strPath = strTarPath;
	int	srclen = strPath.GetLength() + 10;

	if (strPath.GetLength() <= 0) {
		return (-1);
	}
	if (_access(strPath.GetBuffer(0), 0) != 0) {
		strPath.ReleaseBuffer();
		return (-2);
	}

	int	iRet = 0;
	CFileFind find;
	CString strPathName = strPath;
	strPathName.TrimRight('\\');
	strPathName += _T("\\*.*");
	BOOL bFind = find.FindFile(strPathName);

	while (bFind) {
		bFind = find.FindNextFile();

		if (*pbStop == TRUE) break;

		if (find.IsDots()) {
			continue;
		}

		CString strDeleteFile = strPath;
		strDeleteFile.TrimRight('\\');
		strDeleteFile += _T("\\") + find.GetFileName();
		if (find.IsDirectory()) {
			int nDay;
			if (GetDayFromPath(strDeleteFile, nDay) == TRUE) {
				if ((bSameMonth == TRUE) && (nDay > stDelTime.wDay)) {
					continue;
				}
			}

			DeleteTargetDirectory(strDeleteFile, pbStop);
		}
		else {
			::DeleteFile(strDeleteFile);
		}
	}
	find.Close();
//	::RemoveDirectory(strPath);


	return nRet;
}


int CDeleteResultFileThread::DeleteDir_byMonth(CString strTarPath, SYSTEMTIME stDelTime, BOOL *pStop )
{

	ASSERT(pStop);

	CString	strPath = strTarPath;
	int	srclen = strPath.GetLength() + 10;

	if (strPath.GetLength() <= 0) {
		return (1);
	}
	if (_access(strPath.GetBuffer(0), 0) != 0) {
		strPath.ReleaseBuffer();
		return (2);
	}
	strPath.ReleaseBuffer();

	int	iRet = 0;
	CFileFind find;
	CString strPathName = strPath;
	strPathName.TrimRight('\\');
	strPathName += _T("\\*.*");
	BOOL bFind = find.FindFile(strPathName);
	while (bFind) {
		bFind = find.FindNextFile();

		if (*pStop == TRUE) break;

		if (find.IsDots()) {
			continue;
		}

		CString strDeleteFile = strPath;
		strDeleteFile.TrimRight('\\');
		strDeleteFile += _T("\\") + find.GetFileName();
		if (find.IsDirectory()) {
			int nYear, nMonth;
			if (GetYearAndMonthFromPath(strDeleteFile, nYear, nMonth) == TRUE) {
				if ((stDelTime.wYear >= nYear) && (stDelTime.wMonth > nMonth)) {
					DeleteDir_byDay(strDeleteFile, stDelTime, FALSE, pStop);
					// 빈 Directory 삭제
					DeleteEmptyDir(strDeleteFile);

					AprData.SaveDebugLog_Format(_T("[ Delete Logs ] Reason : Old Data = %s"), strDeleteFile);

				}
				else if ((stDelTime.wYear == nYear) && (stDelTime.wMonth == nMonth)) {
					DeleteDir_byDay(strDeleteFile, stDelTime, TRUE, pStop);
					// 빈 Directory 삭제
					DeleteEmptyDir(strDeleteFile);
				}
			}

		}
		else {
			::DeleteFile(strDeleteFile);
		}
	}
	find.Close();

	// 나가면서 경로 삭제.

	return 0;
}
// 22.07.11 Ahn Add End
// 22.07.28 Ahn Add Start
BOOL CDeleteResultFileThread::DeleteEmptyDir(CString strPath)
{
	BOOL bRet = FALSE;
	int nCount = CWin32File::GetFolderCount(strPath);
	if (nCount <= 0) {
		::RemoveDirectory(strPath);
		bRet = TRUE;
	}
	return bRet;
}
// 22.07.28 Ahn Add End

BOOL CDeleteResultFileThread::SaveFinalSaveDate()
{
	CString strFileName;
	CString strSection;
	CString strKey;
	CString strData;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	strSection = _T("LAST_DELETE_DATE_INFO");
	strKey = _T("YEAR");
	strData.Format(_T("%d"), sysTime.wYear);
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);

	strKey = _T("MONTH");
	strData.Format(_T("%d"), sysTime.wMonth);
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);

	strKey = _T("DAY");
	strData.Format(_T("%d"), sysTime.wDay);
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);

	return TRUE;
}

BOOL CDeleteResultFileThread::CheckDeleteAvilable()
{
	BOOL bRet = FALSE;
	char buff[256];
	CString strFileName;
	CString strSection;
	CString strKey;
	CString strData;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	strSection = _T("LAST_DELETE_DATE_INFO");
	strKey = _T("YEAR");
	::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
	int nYear = atoi(buff);

	strKey = _T("MONTH");
	::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
	int nMonth = atoi(buff);

	strKey = _T("DAY");
	::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
	int nDay = atoi(buff);

	//return sysTime;

	return 0;
}

BOOL CDeleteResultFileThread::GetYearAndMonthFromPath(CString strPath, int& nYear, int& nMonth)
{
	// D:\\???\\????\\YYYYMM 
	nYear = 0;
	nMonth = 0;
	CString strYYYYMM;
	int nPos = strPath.ReverseFind('\\');
	strYYYYMM = strPath.Right(6);

	if (strYYYYMM.GetLength() > 6) {
		return FALSE;
	}
	CString strYear = strYYYYMM.Left(4);
	CString strMonth = strYYYYMM.Right(2);

	nYear = atoi(strYear.GetBuffer());
	nMonth = atoi(strMonth.GetBuffer());

	return TRUE ;
}

BOOL CDeleteResultFileThread::GetDayFromPath(CString strPath, int& nDay)
{
	nDay = 0;
	CString strDD;

	int nPos = strPath.ReverseFind('\\');
	int nLength = strPath.GetLength() - nPos;

	//if (nLength > 2) {
	//	return FALSE;
	//}	
	strDD = strPath.Right(2);

	nDay = atoi(strDD.GetBuffer());

	return TRUE;
}
