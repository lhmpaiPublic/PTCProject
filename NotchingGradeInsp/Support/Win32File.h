//	$Id: $
//!	@file	Win32File.h
//!	@brief	CWin32File 긏깋긚궻귽깛??긲긃귽긚
//!	@author	
//!	@date	
//!	@note	


// Win32File.h: CWin32File 긏깋긚궻귽깛??긲긃귽긚
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIN32FILE_H__D9F673B3_0BEE_41D3_86B0_8E1A4C000CBE__INCLUDED_)
#define AFX_WIN32FILE_H__D9F673B3_0BEE_41D3_86B0_8E1A4C000CBE__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define		MAXFILESIZE		10000000L	//!< 긲?귽깑궻띍묈긖귽긛[Bytes]


class CWin32File  
{
public:
// 09.09.26 Seo Add Start
	static int DeleteFolder( CString strPath );
// 09.09.26 Seo Add End
	CWin32File() ;
	virtual ~CWin32File() ;

	enum {
		enExist	= 0x00,
		enWrite	= 0x02,
		enRead	= 0x04,
	} enumAccessMode ;

	static BOOL Exists( LPCTSTR szFilename, int mode = CWin32File::enExist ) ;
	static BOOL FolderFileExists(LPCTSTR szFilename);
	static BOOL ExistFile( LPCTSTR lpszFile, BOOL *pbIsDirectory = NULL ) ;

	static int CreateDirectory( LPCTSTR szPathName, BOOL bIncludedFilename = FALSE ) ;

	static int GetFileStatus( LPCTSTR lpszPath, LPCTSTR lpszFname, CFileStatus *pFileStatus ) ;
	static int GetFileSize( LPCTSTR szFilename, int *pnSize = NULL ) ;
	static int GetDirectorySize( LPCTSTR szDirName, int *pnSize = NULL ) ;

	static int CopyFile( LPCTSTR szSource, LPCTSTR szDestination, BOOL bFailIfExists = FALSE ) ;
	static int CopyFileOne( LPCTSTR lpszSrcname, LPCTSTR lpszTarname, BOOL overwrite = TRUE, BOOL confirm = FALSE, BOOL bMoveMode = FALSE ) ;

	static int MoveFiles( LPCTSTR lpszSrcname, LPCTSTR lpszTarname, BOOL copymode = FALSE, BOOL confirm = TRUE ) ;

	static int CheckWin32FileName( CString &filename, BOOL bReplace = TRUE, char obj ='-' ) ;

	static int DeleteFile( LPCTSTR szFilename ) ; 
	static int DeleteDirectoryOnly( LPCTSTR lpszFolder ) ;
	static int DeleteEmptyDirectory( LPCTSTR lpszPathSrc, BOOL bContainSubDir = TRUE, BOOL bDeleteBasePath = TRUE, BOOL *pbPathEmpty = NULL ) ;
	static int DeleteDirectory( LPCTSTR lpszPathName, BOOL bTrush = FALSE, BOOL bConfirm = FALSE ) ; 
	static int DeleteFiles( LPCTSTR lpszPathName, BOOL bTrush = FALSE, BOOL bConfirm = FALSE ) ;
	static int DeleteFilesByDate( CString &strDir, CTimeSpan &dt, BOOL bDeleteDir, BOOL bForce = FALSE ) ;

	static int BinaryFile( LPCTSTR lpszPath, LPCTSTR lpszFname, char *data, UINT size, int mode, BOOL bErrflag = FALSE ) ;
	static int SelectFile( CString& strFile, int nMode, LPCTSTR szDefExt = NULL, LPCTSTR szFilters = _T( "" ), LPCTSTR szDefPath = NULL, CWnd* pParent = NULL ) ;

	static int TextSave1Line( LPCTSTR lpszPath, LPCTSTR lpszFname, LPCTSTR lpszSavemsg, LPCTSTR lpszModemsg, BOOL bErrflag, int iMaxFileSize = MAXFILESIZE, LPCTSTR lpszIndexMsg = _T( "" ) ) ;
	static int TextRead1Line( LPCTSTR lpszPath, LPCTSTR lpszFname, LPTSTR lpszReadmsg, UINT readsize, BOOL bErrflag ) ;
	static int WriteFile( CStdioFile &cf, LPCTSTR lpszString ) ;
	static int LoadFile( LPCTSTR szFilename, CStringList *lstStore ) ;
	static int UnloadFile( LPCTSTR szFilename, CStringList &strStore, BOOL bNoTruncate = FALSE ) ;

	static int PickupFileList_inFolder( LPCTSTR lpszPathName, CStringList *pstrList, BOOL bPickupFolder = FALSE, BOOL bPickupFile = TRUE ) ;
	static int GetFileList( LPCTSTR lpszPath, CStringList& rstrFileNameList, LPCTSTR lpszExt = _T( "*" ), int nMaxFiles = 0, BOOL bSubFolder = FALSE, BOOL bListClear = TRUE ) ;
//KANG 22.01.07 Add Start
	// 22.04.11 Ahn Modify Start
	//static int GetFolderList(LPCTSTR lpszPath, CStringList& rstrFoloderNameList, LPCTSTR lpszExt = _T("*"), int nMaxFiles = 0, BOOL bSubFolder = FALSE, BOOL bListClear = TRUE);
	static int GetFolderList(LPCTSTR lpszPath, CStringList& rstrFoloderNameList, LPCTSTR lpszExt = _T("*"), int nMaxFiles = 0, BOOL bSubFolder = FALSE, BOOL bListClear = TRUE, BOOL bFullPath = FALSE );
	// 22.04.11 Ahn Modify End
	static int GetFolderCount(LPCTSTR lpszPath, LPCTSTR lpszExt = _T("*"), int nMaxFiles = 0, BOOL bSubFolder = FALSE, BOOL bListClear = TRUE);
//KANG 22.01.07 Add End
	static double GetDiskSpace( LPCTSTR lpszDrive, double* pdTotal = NULL, double* pdFree = NULL, double* pdPercent = NULL ) ;

// furuya 2009.06.04 02 st
	static void GetPcname( LPCTSTR szFilename, LPTSTR szPcname ) ;
// furuya 2009.06.04 02 en
	static void	GetDrivename( LPCTSTR szFilename, LPTSTR szDrive ) ;
	static void GetDirname( LPCTSTR szFilename, LPTSTR szDir ) ;
	static void GetExtname( LPCTSTR szFilename, LPTSTR szExtname ) ;
	static void GetFilename( LPCTSTR szFilename, LPTSTR szName ) ;
	static CString GetFilename( LPCTSTR szFullPath, BOOL bExtention = TRUE ) ;
	static CString	GetFileExceptionString( CFileException& rError ) ;

	//문자열에 알파벳과 숫자만 검색하여 특수문자 있을 경우 FALSE
	static int	IsNumAlpha(CString strData);

};

#endif // !defined(AFX_WIN32FILE_H__D9F673B3_0BEE_41D3_86B0_8E1A4C000CBE__INCLUDED_)
