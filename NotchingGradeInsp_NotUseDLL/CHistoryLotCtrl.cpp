#include "pch.h"
#include "CHistoryLotCtrl.h"
#include "Win32File.h"
#include "GlobalData.h"


CHistoryLotCtrl::CHistoryLotCtrl( CString strPath, CString strFileName )
{
	m_strPath = strPath ;
	m_strFileName = strFileName ;

	int nPos = strPath.ReverseFind('\\');
	if (nPos > 1) {
		m_strFullPath.Format(_T("%s\\%s"), strPath, strFileName);
	}
	else {
		m_strFullPath.Format(_T("%s%s"), strPath, strFileName);
	}
}

CHistoryLotCtrl::~CHistoryLotCtrl()
{
}


// 파일 
BOOL CHistoryLotCtrl::IsFileSizeNormal(CString strFileName)
{
	BOOL bRet = FALSE;

	int nSizeBase = sizeof(_LOT_INFO);
	int nSize = CWin32File::GetFileSize(strFileName);

	if (nSize < 0) {
		CString strMsg;
		strMsg.Format(_T("파일 사이즈가 0 보다 작습니다."));
		AprData.SaveErrorLog(strMsg);
		return FALSE;
	}

	if ((nSize % nSizeBase) > 0) {
		//파일 내용 이상.
		CString strMsg;
		strMsg.Format(_T("파일 사이즈가 일치하지 않습니다. FileSize[%d], 단위[%d]"), nSize, nSizeBase);
		AprData.SaveErrorLog(strMsg);
		return FALSE;
	}

	return TRUE;
}

int CHistoryLotCtrl::AddLotFileList(_LOT_INFO& LotInfo)
{
	int nRet = 0;
	CWin32File file;

	int nMode = MODE_WRITE;
	int nBaseSize = sizeof(_LOT_INFO);

	if (file.ExistFile(m_strFullPath) == TRUE) {
		// 파일 존재시 Size 확인

		if (IsFileSizeNormal(m_strFullPath) == FALSE) {
			//파일 내용 이상.
			return -1;
		}
	}

	PushLotInfo(LotInfo);

	int nLocalRet = SaveLotFileList(NULL) ;

	if (nLocalRet < 0) {
		nRet = -2 ;
	}

	//file.BinaryFile(strPath, strFile, (char*)pInfo, nSize, nMode);

	return nRet ;
}

int CHistoryLotCtrl::PushLotInfo(_LOT_INFO& LotInfo)
{

	int nSize = (int)m_vecLotInfo.size();
	if (nSize > MAX_SAVE_LOT_INFO_SIZE) {
		VEC_LOT_INFO::iterator iter;
		for (iter = m_vecLotInfo.begin(); ( MAX_SAVE_LOT_INFO_SIZE - 1 )<= m_vecLotInfo.size(); ) {
			iter = m_vecLotInfo.erase(m_vecLotInfo.begin());
		}
	}

	m_vecLotInfo.push_back(LotInfo);

	return 0;
}

int CHistoryLotCtrl::GetAtLotInfo(int nIndex, _LOT_INFO* pLotInfo)
{
	int nSize = (int)m_vecLotInfo.size();

	if ( (nSize <= 0)  || ( nSize <= nIndex )){
		return -1;
	}

	VEC_LOT_INFO::iterator iter;

	iter = m_vecLotInfo.begin() + nIndex ;

	*pLotInfo = *iter ;

	return 0;
}


int CHistoryLotCtrl::PopLotInfo(_LOT_INFO* pLotInfo)
{
	if (m_vecLotInfo.empty() == FALSE) {
		*pLotInfo = m_vecLotInfo.front();
		m_vecLotInfo.erase(m_vecLotInfo.begin());
	}
	return 0;
}

void CHistoryLotCtrl::ClearQueue()
{
	VEC_LOT_INFO::iterator iter;
	for (iter = m_vecLotInfo.begin() ; iter != m_vecLotInfo.end() ; iter++ ) {
		iter = m_vecLotInfo.erase(iter);
	}
}

int CHistoryLotCtrl::size()
{
	int nRet = (int) m_vecLotInfo.size();

	return nRet;
}


int CHistoryLotCtrl::ReadLotFileList(VEC_LOT_INFO* pVecLotList)
{
	// File Size 
	ClearQueue();
	
	CWin32File file;
	BOOL bExistFile = file.ExistFile(m_strFullPath) ;

	if (bExistFile == FALSE) return -1;
	CFile binFile;
	CFileStatus fs;

	if (IsFileSizeNormal(m_strFullPath) != FALSE) {		
		//m_vecLotInfo.
		if (!binFile.GetStatus(m_strFullPath,fs)) {
			return -1;
		}

		if (binFile.Open( m_strFullPath, CFile::modeRead | CFile::shareDenyNone) != 0) {
			int bytes;
			while (1) {
				_LOT_INFO LotInfo;
				bytes = binFile.Read(&LotInfo, sizeof(_LOT_INFO));
				if (bytes == sizeof(_LOT_INFO)) {
					PushLotInfo(LotInfo);
				}
				else {
					break;
				}
			}
		}
		binFile.Close();
	}

	return 0;
}

int CHistoryLotCtrl::SaveLotFileList(VEC_LOT_INFO* pVecLotList)
{
	int nSize = (int)m_vecLotInfo.size();

	// 22.07.12 Ahn Modify Start
	//CStdioFile binFile;
	CFile binFile;
	// 22.07.12 Ahn Modify End

	CFileStatus fs;

	CString strTempFile;
	//strTempFile.Format(_T("%sTempLotInfo.tmp"), m_strPath );
	strTempFile = m_strFullPath;
	strTempFile.Replace(_T(".bin"), _T(".tmp"));

	if (CWin32File::ExistFile(strTempFile) == TRUE) {
		CWin32File::DeleteFiles(strTempFile);
	}

	VEC_LOT_INFO::iterator iter;
	if (binFile.Open(strTempFile, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::typeBinary, NULL) == 0) {
			binFile.Close();
	} else {
		for (iter = m_vecLotInfo.begin(); iter != m_vecLotInfo.end(); iter++ ) {


			binFile.SeekToEnd();
			_LOT_INFO Info;
			Info = *iter;

			binFile.Write(&Info, sizeof(_LOT_INFO));
		}

		binFile.Close();

		if (CWin32File::ExistFile(m_strFullPath) == TRUE) {
			CWin32File::DeleteFiles(m_strFullPath);
		}

		::rename(strTempFile, m_strFullPath);
	}

	return 0;
}

// 22.06.27 Ahn Add End
