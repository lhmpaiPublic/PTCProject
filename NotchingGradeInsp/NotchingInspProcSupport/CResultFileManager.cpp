#include "pch.h"
#include "CResultFileManager.h"
#include "Win32File.h"
#include "GlobalData.h"


CResultFileManager::CResultFileManager()
{
	m_bComplete = FALSE;
}
CResultFileManager::~CResultFileManager()
{
}

int CResultFileManager::Run()
{
	int nRet = 0;	
	// Backup 

	// Delete

	return nRet;
}

int CResultFileManager::MakeFolderList(_VEC_DIR_DATA * pVecDirData, CString strPath, int nDeleteDay)
{
	ASSERT(pVecDirData);
	int nRet = 0;
	int nDay, nMonth, nYear;

	CTime time = CTime::GetCurrentTime();
	nDay = time.GetDay();
	nMonth = time.GetMonth();
	nYear = time.GetYear();

	time = time - CTimeSpan( nDeleteDay, 0, 0, 0);
	nDay = time.GetDay();
	nMonth = time.GetMonth();
	nYear = time.GetYear();
	pVecDirData->clear();

	CString strDir;
	strDir.Format(_T("%04d\\%02d\\%02d"), time.GetYear(), time.GetMonth(), time.GetDay() );

	CWin32File file;
	CString strTemp;
	CString strYear;
	CString strMonth;
	CString strDay;
	CStringList strListY;
	CStringList strListM;
	CStringList strListD;

	CString strPathY, strPathM, strPathD;
	file.GetFolderList(strPath, strListY);

	POSITION posy;
	POSITION posm;
	POSITION posd;
	int nDirYear, nDirMonth, nDirDay;

	for (posy = strListY.GetHeadPosition(); posy ;  ) {
		strYear = strListY.GetNext(posy) ;
		strPathY.Format(_T("%s\\%s"), AprData.m_strFeederPath, strYear);
		nDirYear = atoi(strYear);
		if (nDirYear > nYear) continue;
		file.GetFolderList(strPathY, strListM) ;
		for (posm = strListM.GetHeadPosition(); posm ; ) {
			strMonth = strListM.GetNext(posm) ;
			strPathM.Format(_T("%s\\%s"), strPathY, strMonth) ;
			file.GetFolderList(strPathM, strListD) ;
			nDirMonth = atoi(strMonth);

			if ( ( nDirMonth > nMonth ) && ( nDirYear >= nYear) ) continue;
			for (posd = strListD.GetHeadPosition(); posd;) {
				strDay = strListD.GetNext(posd);
				strPathD.Format(_T("%s\\%s"), strPathM, strDay);
				nDirDay = atoi(strDay);
				
				if( (nDirDay > nDay) && (nDirMonth == nMonth) )continue;
				// 삭제 대상 날짜를 리스트에 추가함.
			//	data.SetDirectory(strPathD);
			//	CStringList* pListPtr = data.GetFolderListPtr() ;
			//	pListPtr->RemoveAll();
			//	file.GetFolderList(strPathD, *pListPtr);
				CProcFileData data;
				data.SetDirectory(strPathD);
				data.SetDate(nYear, nMonth, nDay);
								
				pVecDirData->push_back(data);
			}
		}
	}

	return nRet;
}

UINT CResultFileManager::CtrlRsltFileBackupThread(LPVOID Param)
{
	CResultFileManager* pThis = (CResultFileManager*)Param;

	
	CResultFileManager rsltManager;
	CString strPath; 
	strPath.Format( _T("%s"), AprData.m_strFeederPath ) ;
	_VEC_DIR_DATA vecDirData;
	// 년/월/일 폴더 수집
	rsltManager.MakeFolderList(&vecDirData, strPath, 90);

	while (1) {
		if (pThis == NULL) {
			break;
		}

		if (pThis->m_bBackup_Kill == TRUE) {
			break;
		}
		
		//int nSize = (int)vecDirData.size();
		//for (int i = 0; i < nSize; i++) {
		_VEC_DIR_DATA::iterator iter;
		for( iter = vecDirData.begin() ; iter != vecDirData.end() ; iter++){
		// 폴더 리스트 
			CString strFilePath;
			strFilePath = iter->GetDirectory() ;
			CStringList strList;
			CWin32File::GetFileList(strFilePath, strList, _T("*"), 0, TRUE);
			// strFilePath.Format( _T("%s")) ;
			// 파일 검색
			// 복사경로 생성
			// 파일 복사

		}
	}

	AfxEndThread(0);
	pThis->m_bBackup_Kill = FALSE;
	pThis->m_bBackup_RunFlag = FALSE;
	pThis->m_bComplete = TRUE;

	return 0;
}

UINT CResultFileManager::CtrlRsltFileDeleteThread(LPVOID Param)
{
	CResultFileManager* pThis = (CResultFileManager*)Param;

	while (1) {
		if (pThis == NULL) {
			break;
		}
		if (pThis->m_bDelete_Kill == TRUE) {
			break;
		}

		// 22.04.19 Ahn Add Start
		// 폴더 리스트 
		CWin32File wFile;
		_VEC_DIR_DATA::iterator iter;
		iter = pThis->m_VecDelDirData.begin();
		for (; iter != pThis->m_VecDelDirData.end() ;  ) {
			// 파일 검색
			CString strPath;
			CStringList strFileList;
			strPath.Format(_T("%s"), iter->GetDirectory());
			if(strPath.GetLength() <= 0) {
				pThis->m_VecDelDirData.erase(iter++);
				continue;
			}
			wFile.GetFileList(strPath, strFileList);
			// 파일삭제
			pThis->m_VecDelDirData.erase(iter++);
			int nSize = (int) strFileList.GetSize();
			CString strFile;
			POSITION pos;
			pos = strFileList.GetHeadPosition();
			//for (; pos != NULL;) {
			//	strFile = strFileList.GetNext(pos);
			//	wFile.DeleteFiles(strFile);
			//}
		}
		// 22.04.19 Ahn Add End
	}

	AfxEndThread(0);
	pThis->m_bDelete_Kill = FALSE;
	pThis->m_bDelete_RunFlag = FALSE;
	pThis->m_bComplete = TRUE;

	return 0;
}


int CResultFileManager::BeginDeleteThread()
{
	m_bDelete_Kill = FALSE;

	//	m_DisphWnd = NULL ;
	if (m_pDelete_Thread == NULL) {

		m_pDelete_Thread = AfxBeginThread((AFX_THREADPROC)CtrlRsltFileDeleteThread,
			(LPVOID)this,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);

		m_bDelete_RunFlag = TRUE;

		if (m_pDelete_Thread != NULL) {
			m_pDelete_Thread->m_bAutoDelete = TRUE;
			m_pDelete_Thread->ResumeThread();
		}
	}
	return 0;
}

int CResultFileManager::BeginBackupThread()
{
	m_bBackup_Kill = FALSE;

	//	m_DisphWnd = NULL ;
	if (m_pBackup_Thread == NULL) {

		m_pBackup_Thread = AfxBeginThread((AFX_THREADPROC)CtrlRsltFileBackupThread,
			(LPVOID)this,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);

		m_bBackup_RunFlag = TRUE;

		if (m_pBackup_Thread != NULL) {
			m_pBackup_Thread->m_bAutoDelete = TRUE;
			m_pBackup_Thread->ResumeThread();
		}
	}
	return 0;
}

int CResultFileManager::KillDeleteThread()
{
	DWORD	dwCode;
	LONG	ret;

	if (m_pDelete_Thread != NULL) {
		ret = ::GetExitCodeThread(m_pDelete_Thread->m_hThread, &dwCode);
		if (ret && dwCode == STILL_ACTIVE) {
			m_bDelete_Kill = TRUE;
			WaitForSingleObject(m_pDelete_Thread->m_hThread, INFINITE);
		}
		delete m_pDelete_Thread;
		m_pDelete_Thread = NULL;
	}
	return 0;
}

int CResultFileManager::KillBackupThread()
{
	DWORD	dwCode;
	LONG	ret;

	if (m_pBackup_Thread != NULL) {
		ret = ::GetExitCodeThread(m_pBackup_Thread->m_hThread, &dwCode);
		if (ret && dwCode == STILL_ACTIVE) {
			m_bBackup_Kill = TRUE;
			WaitForSingleObject(m_pBackup_Thread->m_hThread, INFINITE);
		}
		delete m_pBackup_Thread;
		m_pBackup_Thread = NULL;
	}
	return 0;
}
