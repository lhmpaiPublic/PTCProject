
#include "pch.h"
#include "Win32File.h"

#include <io.h>
#include <direct.h>
#include <sys/stat.h>

#include "WinTool.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define		READMODE				0			// Read
#define		WRITEMODE				1			// Save


CWin32File::CWin32File()
{

}

CWin32File::~CWin32File()
{

}


int	CWin32File::BinaryFile( LPCTSTR lpszPath, LPCTSTR lpszFname, char *data, UINT size, int mode, BOOL bErrflag /*= FALSE*/ )
{

	CString	filename = _T( "" ) ;
	struct _stat	st_buf ;
	UINT	filesize ;
	UINT	openmode = 0 ;
	CFile	file ;
	CFileStatus	status ;
	CFileException	errorex ;

	if ( strlen( lpszPath ) > 0 ) {
		filename = lpszPath ;
		filename += _T( "\\" ) ;
		filename += lpszFname ;
	} else {
		filename = lpszFname ;
	}

	filesize = ( UINT)size ;

	if ( mode == READMODE ) {
		if ( file.GetStatus( filename, status ) == 0 ) {
			// SAVE_ERROR_TEXT( filename ) ;
			if ( bErrflag == TRUE ) {
				CString	strDisp = _T( "File not found.[" ) ;
				strDisp += filename ;
				strDisp += _T( "]" ) ;
			//	theApp.SetPcErrorDetail( ERROR_DETAIL_OTHER ) ;
			//	// MyDisperror( strDisp, FALSE ) ;
			}
			return ( -1 ) ;
		}
		memset( data, 0, size ) ;
		openmode = CFile::modeCreate | CFile::modeNoTruncate ;
		if ( _access( filename.GetBuffer( 0 ), 0 ) != 0 ) {
			filename.ReleaseBuffer() ;
			// SAVE_ERROR_TEXT( filename ) ;
			if ( bErrflag == TRUE ) {
				CString	strDisp = _T( "File not found.[" ) ;
				strDisp += filename ;
				strDisp += _T( "]" ) ;
			//	theApp.SetPcErrorDetail( ERROR_DETAIL_OTHER ) ;
			//	// MyDisperror( strDisp, FALSE ) ;							
			}
			return ( -1 ) ;
		}
		filename.ReleaseBuffer() ;

		if ( _stat( filename.GetBuffer( 0 ), &st_buf ) != 0 ) {
			filename.ReleaseBuffer() ;
			// SAVE_ERROR_TEXT( filename ) ;
			return ( -1 ) ;
		}
		filename.ReleaseBuffer() ;
		BOOL	bSame = TRUE ;
		if ( st_buf.st_size > ( int)size ) {
			bSame = FALSE ;
		}
#ifdef	_DEBUG
		if ( st_buf.st_size != ( int)size ) {
			bSame = FALSE ;
		}
#endif	// end of _DEBUG
		if ( bSame != TRUE ) {
			if ( bErrflag == TRUE ) {
				CString	strDisp = _T( "" ) ;
				strDisp.Format( _T("File size is different.(FILE:%d/%d)["), ( int)st_buf.st_size, ( int)size ) ;
				strDisp += filename ;
				strDisp += _T( "]" ) ;
				// MyDisperror( strDisp, FALSE ) ;
			}
		}
		if ( st_buf.st_size < ( int)size ) {
			filesize = ( UINT)st_buf.st_size ;
		}

	} else {
		openmode = CFile::modeCreate | CFile::modeWrite ;
		CWin32File::CreateDirectory( filename, TRUE ) ;
	}

	if ( file.Open( filename, openmode, &errorex ) == 0 ) {
		if ( bErrflag == TRUE ) {
			AfxMessageBox( GetFileExceptionString( errorex ) ) ;
		}
		// SAVE_ERROR_TEXT( filename ) ;
		return ( -1 ) ;
	}

	if ( mode == READMODE ) {
		file.Read( data, filesize ) ;
	} else {
		file.Write( data, filesize ) ;
	}

	file.Close() ;

	return ( 0 ) ;

}


int CWin32File::CopyFile( LPCTSTR szSource, LPCTSTR szDestination, BOOL bFailIfExists /*= FALSE */ ) 
{
	CString	strS, strD, strError ;

	strS = szSource ;
	strD = szDestination ;

	try {
		if ( strS == strD ) {
			strError.Format( _T("CopyFile::micky mouse operation \r\n[%s]->[%s]"), strS, strD ) ;
			// SAVE_ERROR_TEXT( strError ) ;
			return ( -1 ) ;
		}

		if ( !CWin32File::Exists( szSource ) ) {
			strError.Format( _T("CopyFile::source file is not there [%s]"), strS ) ;
			// SAVE_ERROR_TEXT( strError ) ;
			return ( -1 ) ;
		}

		{
			TCHAR	szdrive[ _MAX_DRIVE ] ;
			TCHAR	szDir[ MAX_PATH ] ;

			CWin32File::GetDrivename( szDestination, szdrive ) ;
			CWin32File::GetDirname( szDestination, szDir ) ;
			CString	str ;

			str.Format( _T("%s%s"), szdrive, szDir ) ;
			if ( !CWin32File::Exists( str ) ) {
				if ( CreateDirectory( str ) < 0 ) {
					return ( -1 ) ;
				}
			}
		}

		if ( !::CopyFile( szSource, szDestination, bFailIfExists )  ) {
			CString	str ;
			return ( -1 ) ;
		}
	} catch ( ... ) {
		return ( -1 ) ;
	}

	return ( 0 ) ;
}


int CWin32File::CopyFileOne( LPCTSTR lpszSrcname, LPCTSTR lpszTarname, BOOL overwrite /*= TRUE*/, BOOL confirm /*= FALSE*/, BOOL bMoveMode /*= FALSE*/ )
{

	int	iRet = 0 ;
	CString	srcname = lpszSrcname ;
	CString	tarname = lpszTarname ;

	if ( srcname.GetLength() <= 0 ) {
		// SAVE_ERROR_TEXT( srcname ) ;
		return ( -1 ) ;
	}
	if ( tarname.GetLength() <= 0 ) {
		// SAVE_ERROR_TEXT( tarname ) ;
		return ( -1 ) ;
	}
	if ( _access( srcname.GetBuffer( 0 ), 0 ) != 0 ) {
		srcname.ReleaseBuffer() ;
		return ( -2 ) ;
	}
	srcname.ReleaseBuffer() ;
	if ( _access( tarname.GetBuffer( 0 ), 0 ) == 0 ) {
		tarname.ReleaseBuffer() ;
		if ( overwrite != TRUE ) {
			return ( -1 ) ;
		}
		if ( confirm == TRUE ) {
			CString	szDispMsg = _T( "" ) ;
			szDispMsg = _T( "The file of this name already exists at the copy destination.\n\n" ) ;
			szDispMsg += _T( "[" ) ;
			szDispMsg += tarname ;
			szDispMsg += _T( "]" ) ;
			if ( AfxMessageBox( szDispMsg, ( MB_YESNO | MB_ICONQUESTION ) ) != IDYES ) {
				return ( -3 ) ;
			}
		}

		if ( CWin32File::DeleteFile( tarname ) != 0 ) {
			// SAVE_ERROR_TEXT( tarname ) ;
			iRet = -1 ;
		}
	} else {
		tarname.ReleaseBuffer() ;
	}

	CWin32File::CreateDirectory( tarname, TRUE ) ;
	if ( bMoveMode == TRUE ) {
		if ( ::MoveFile( srcname, tarname ) == 0 ) {
			// SAVE_ERROR_TEXT( srcname ) ;
			// SAVE_ERROR_TEXT( tarname ) ;
			return ( -1 ) ;
		}
	} else {
		if ( ::CopyFile( srcname, tarname, FALSE ) == 0 ) {
			// SAVE_ERROR_TEXT( srcname ) ;
			// SAVE_ERROR_TEXT( tarname ) ;
			return ( -1 ) ;
		}
	}

	return ( iRet ) ;

}


int	CWin32File::GetFileStatus( LPCTSTR lpszPath, LPCTSTR lpszFname, CFileStatus *pFileStatus )
{
	int	nRet = 0 ;
	CString	strFileName ;
	if ( strlen( lpszPath ) > 0 ) {
		strFileName = lpszPath ;
		strFileName += _T( "\\" ) ;
		strFileName += lpszFname ;
	} else {
		strFileName = lpszFname ;
	}
	int	nNameLength = strFileName.GetLength() ;
	if ( nNameLength <= 0 ) {
		return ( -1 ) ;
	}
	ASSERT( pFileStatus != NULL ) ;
	if ( pFileStatus == NULL ) {
		return ( -1 ) ;
	}
	try {
		if ( CFile::GetStatus( strFileName, *pFileStatus ) != TRUE ) {
			nRet = -1 ;
		}
	} catch ( ... ) {
		nRet = -1 ;
	}
	return ( nRet ) ;
}


int CWin32File::GetFileSize( LPCTSTR szFilename, int *pnSize /*= NULL*/ ) 
{
	CFileStatus	cfs ;

	if ( CFile::GetStatus( szFilename, cfs ) != TRUE ) {
		//CString	str, strError ;
		//str = ::FormatErrorMsg( ::GetLastError() ) ;
		//strError.Format( _T("GetFilesize:%s \r\n %s"), str, szFilename ) ; 
		// SAVE_ERROR_TEXT( strError ) ;
		return ( -1 ) ;
	}
	if ( pnSize != NULL ) {
		*pnSize = ( int)cfs.m_size ;
	}

	return ( ( int)cfs.m_size ) ;
}


int CWin32File::GetDirectorySize( LPCTSTR lpctDirName, int *pnSize /*= NULL*/ )
{

	CStdioFile	cf ;
	int	nSizeByte = 0 ;
	CFileStatus status ;
	CString	strFileName ;
	CString	strDirName = lpctDirName ;
	CStringList strList ;
	BOOL bIsDirectory = FALSE ;
	ExistFile( strDirName, &bIsDirectory ) ;
	if ( bIsDirectory != TRUE ) {
		return ( -1 ) ;
	}
	if ( strDirName.Right( 1 ).Compare( _T( "\\" ) ) != 0 ) {
		strDirName += _T( "\\" ) ;
	}

	if ( GetFileList( strDirName, strList, _T( "*" ), 0, TRUE ) < 0 ) {
		return ( -1 ) ;
	}

	int nSizeMB = 0 ;
// @ x64 Compatible @ st
//	int nMaxFiles = strList.GetCount() ;
	int nMaxFiles = ( int)( strList.GetCount() ) ;
// @ x64 Compatible @ ed
	POSITION	pos ;
	pos = strList.GetHeadPosition() ;
	for ( int i = 0 ; i < nMaxFiles ; i++ ) {
		strFileName = strList.GetNext( pos ) ;
		if ( GetFileStatus( strDirName, strFileName, &status ) < 0 ) {
			return ( -1 ) ;
		}
		nSizeByte += ( int)status.m_size ;
		if ( nSizeByte > 1024 * 1024 ) {
			nSizeMB++ ;
			nSizeByte -= 1024 * 1024 ;
		}
	}

	if ( pnSize != NULL ) {
		*pnSize = nSizeMB ;
	}

	return ( nSizeMB ) ;
}


int CWin32File::DeleteFile( LPCTSTR szFilename ) 
{
	TRY {
		if ( !::DeleteFile( szFilename ) ) {
			return ( -1 ) ;
		}
	} CATCH( CFileException, e ) {
		return ( -1 ) ;
	} END_CATCH

	return ( 0 ) ;
}


int CWin32File::CreateDirectory( LPCTSTR szDirName, BOOL bIncludedFilename /*= FALSE*/ ) 
{

	CString	setname = _T( "" ) ;
	CString	tempname = _T( "" ) ;

	int	i ;
	int	maxlen ;
	int	ret = 0 ;
	int	net_flag = 0 ;
	BYTE	code ;

	if ( bIncludedFilename ) {
		TCHAR	szDrive[ _MAX_DRIVE ], szDir[ _MAX_PATH ] ;
		CWin32File::GetDrivename( szDirName, szDrive ) ;
		CWin32File::GetDirname( szDirName, szDir ) ;
		tempname.Format( _T("%s%s"), szDrive, szDir ) ;
	} else {
		tempname = szDirName ;
	}

	maxlen = tempname.GetLength() ;
	if ( maxlen > 2 ) {
		if ( strncmp( szDirName, "\\\\", 2 ) == 0 ) {
			net_flag = 3 ;
		}
	}

	for ( i = 0 ; i <= maxlen ; i++ ) {
		if ( i >= maxlen ) {
			code = 0x00 ;
		} else {
			code = tempname.GetAt( i ) ;
		}
		if ( ( code == '\\' ) || ( code == 0x00 ) ) {
			setname = tempname.Left( i ) ;

			if ( net_flag <= 0 ) {
				if ( setname.GetLength() > 3 ) {
					if ( !CWin32File::Exists( setname ) ) {
						if ( !::CreateDirectory( setname, NULL ) != 0 ) {
							//CString	str, strError ;
							//str = ::FormatErrorMsg( ::GetLastError() ) ;
							//strError.Format( _T("CreateDirectory:%s [%s]"), str, szDirName ) ; 
							// SAVE_ERROR_TEXT( strError ) ;
							return ( -1 ) ;
						}
					}
				}
			} else {
				net_flag -= 1 ;
			}
		}
	}

	return ( ret ) ;
	
}


BOOL CWin32File::Exists( LPCTSTR szFilename, int mode /*= CWin32File::enExist*/ ) 
{
	return (_access( szFilename, mode ) == 0 ) ;
}

BOOL CWin32File::FolderFileExists(LPCTSTR szFilename)
{
int ret = _taccess(szFilename, CWin32File::enExist);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
		LOGDISPLAY_SPEC(3)(" FolderFileExists=<%s>=ret<%d>= ", szFilename, ret);
#endif //SPCPLUS_CREATE
	return (ret != -1);
}


void CWin32File::GetPcname( LPCTSTR szFilename, LPTSTR szPcname ) 
{
	CString strFilename = szFilename ;
	CString strPcname = _T( "" ) ;

	int i = strFilename.Find( _T( "\\\\" ) ) ;
	if ( i == 0 ) {
		int j = strFilename.Find( _T( "\\" ), 2 ) ;
		if ( j < 0 ) {
			j = strFilename.GetLength() ;
		}
		strPcname = strFilename.Mid( 0, j ) ;
		//strcpy( szPcname, strPcname ) ;
		strcpy_s( szPcname, strPcname.GetLength(), strPcname ) ;
	}

	return ;
}


void CWin32File::GetDrivename( LPCTSTR szFilename, LPTSTR szDrive ) 
{
	TCHAR drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ] ;

	_splitpath_s( szFilename, drive, dir, fname, ext );
	strcpy_s( szDrive, sizeof( drive ), drive ) ;
	
	return ;
}


void CWin32File::GetDirname( LPCTSTR szFilename, LPTSTR szDir ) 
{
	TCHAR drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ] ;

	_splitpath_s( szFilename, drive, dir, fname, ext );
	strcpy_s( szDir, sizeof( dir ), dir ) ;
	
	return ;
}


void CWin32File::GetFilename( LPCTSTR szFilename, LPTSTR szName ) 
{
	TCHAR drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ] ;

	_splitpath_s( szFilename, drive, dir, fname, ext );
	strcpy_s( szName, sizeof( fname ), fname ) ;
	return ;
}


void CWin32File::GetExtname( LPCTSTR szFilename, LPTSTR szExtname ) 
{
	TCHAR drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ] ;

	_splitpath_s( szFilename, drive, dir, fname, ext );
	strcpy_s( szExtname, sizeof( ext ),  ext ) ;
	return ;
}


CString CWin32File::GetFilename( LPCTSTR szFullPath, BOOL bExtention /*= TRUE*/ ) 
{
	TCHAR szRet[ _MAX_FNAME + _MAX_EXT ] ;
	TCHAR drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ] ;

	_splitpath_s( szFullPath, drive, dir, fname, ext ) ;

	if ( bExtention ) {
		sprintf_s( szRet, "%s%s", fname, ext ) ;
	} else {
		sprintf_s( szRet, "%s", fname ) ;
	}

	return ( szRet ) ;
}


CString	CWin32File::GetFileExceptionString( CFileException& rError ) 
{
	CString	str ;
	switch ( rError.m_cause ) {
	case	CFileException::none :
		str = _T( "Error None" ) ;
		break ; 

	case	CFileException::fileNotFound :
		str = _T( "File Undefined" ) ;
		break ;

	case	CFileException::badPath :
		str = _T( "Illegal Pass" ) ;
		break ;

	case	CFileException::tooManyOpenFiles :
		str = _T( "The number of open files exceeds" ) ;
		break ;

	case	CFileException::accessDenied :
		str = _T( "File access prohibition" ) ;
		break ;

	case	CFileException::invalidFile :
		str = _T( "Illegal file steering wheel use" ) ;
		break ;

	case	CFileException::removeCurrentDir :
		str = _T( "The current directory under work cannot be deleted." ) ;
		break ;

	case	CFileException::directoryFull :
		str = _T( "Directory full" ) ;
		break ;

	case	CFileException::badSeek :
		str = _T( "File pointer set error" ) ;
		break ;

	case	CFileException::hardIO :
		str = _T( "Hardware error" ) ;
		break ;

	case	CFileException::sharingViolation :
		str = _T( "The share.exe unloading or the common area has been locked." ) ;
		break ;

	case	CFileException::lockViolation :
		str = _T( "Lock demand in area which has been locked" ) ;
		break ;

	case	CFileException::diskFull :
		str = _T( "Disk full" ) ;
		break ;
	case	CFileException::endOfFile :
		str = _T( "Ending attainment of file" ) ;
		break ;
	case	CFileException::genericException :
	default :
		str = _T( "Undefined Error" ) ;
		break ;
	} // switch 

	str += ":" ;
	str += rError.m_strFileName ;

	return ( str ) ;
}

//문자열에 알파벳과 숫자만 검색하여 특수문자 있을 경우 FALSE
int CWin32File::IsNumAlpha(CString strData)
{
	//알파벳 또는 숫자가 아닌 문자가 몇개 인가 ? 
	int bRet = 0;
	for (int idx = 0; idx < strData.GetLength(); idx++)
	{
		if (isalnum(strData.GetAt(idx)) == 0)
		{
			bRet++;
		}
	}
	return bRet;
}

int CWin32File::DeleteFilesByDate( CString &strDir, CTimeSpan &dt, BOOL bDeleteDir, BOOL bForce /*= FALSE*/ )
{
	CString	strTarget, filename ;
	CTime	curtime, filetime ;
	CTimeSpan  span ;
	CFileFind ff ;
	CTime	ft ;
	BOOL	bRet ;

	if ( Exists( strDir ) == FALSE ) {
		return ( 0 ) ;
	}

	curtime = CTime::GetCurrentTime() ;
	strTarget.Format( _T("%s\\*.*"), strDir.GetBuffer( 0 ) ) ;

	bRet = ff.FindFile( strTarget ) ;
	if ( bRet == FALSE ) {
		ff.Close() ;
		return ( 0 ) ;
	}

	while ( bRet != FALSE ) {
		bRet = ff.FindNextFile() ;

		if ( ff.IsSystem() ) {
			continue ;
		}
		if ( ff.IsDots() ) {
			continue ;
		}
		if ( ff.IsReadOnly() ) {
			continue ;
		}

		filename.Format( _T("%s\\%s"), strDir.GetBuffer( 0 ), ff.GetFileName() ) ;

		ff.GetLastWriteTime( filetime ) ;
		span = curtime - filetime ;
		
		TRACE( _T("%s: %04d/%02d/%02d %02d:%02d:%02d\n"), filename.GetBuffer( 0 ), filetime.GetYear(), filetime.GetMonth(), filetime.GetDay(), filetime.GetHour(), filetime.GetMinute(), filetime.GetSecond() ) ;
		TRACE( _T("span:   days:%d hours:%d \n"), span.GetDays(), span.GetHours() ) ;
		TRACE( _T("dtspan: days:%d hours:%d \n"), dt.GetDays(), dt.GetHours() ) ;

		if ( !bForce ) {
			if ( ( dt.GetDays() == 0 ) || ( span < dt ) ) {
				continue ;
			}
		}

		if ( ff.IsDirectory() != TRUE ) {
			filename.Format( _T("%s\\%s"), strDir.GetBuffer( 0 ), ff.GetFileName() ) ;

			if ( DeleteFile( filename.GetBuffer( 0 ) ) < 0 ) {
				CString str ;
				str.Format( _T("%s:%s"), filename.GetBuffer( 0 ) , ::FormatErrorMsg( ::GetLastError() ) );
				AfxMessageBox( str.GetBuffer( 0 ) ) ;
			}
		} else {

			filename.Format( _T("%s\\%s"), strDir.GetBuffer( 0 ), ff.GetFileName() ) ;

			if ( DeleteFilesByDate( filename, dt, bDeleteDir ) != 0 ) {
				CString str ;
				str.Format( _T("%s:%s"), filename.GetBuffer( 0 ), ::FormatErrorMsg( ::GetLastError() ) );
				AfxMessageBox( str.GetBuffer( 0 ) ) ;					
			} else {
				if ( bDeleteDir ) {
					CString	strFind ;
					strFind.Format( _T("%s\\"), filename.GetBuffer( 0 ) ) ;
					if ( ExistFile( strFind.GetBuffer( 0 ) ) == FALSE ) {
						if ( ::RemoveDirectory( filename.GetBuffer( 0 ) ) == 0 ) {
							CString str ;
							str.Format( _T("%s:%s"), filename.GetBuffer( 0 ) , ::FormatErrorMsg( ::GetLastError() ) );
							AfxMessageBox( str.GetBuffer( 0 ) ) ;
						}
					} // IF
				} // i
			} // IF
		} // IF
	}

	ff.Close() ;
	return ( 0 ) ;
}


BOOL CWin32File::ExistFile( LPCTSTR lpszFile, BOOL *pbIsDirectory /*= NULL*/ )
{
	try {
		if ( pbIsDirectory != NULL ) {
			*pbIsDirectory = FALSE ;
		}
		if ( strlen( lpszFile ) <= 0 ) {
			// SAVE_ERROR_TEXT( lpszFile ) ;
			return ( FALSE ) ;
		}
		CFileFind	finder ;
		if ( finder.FindFile( lpszFile ) ) {
			if ( pbIsDirectory != NULL ) {
				finder.FindNextFile() ;
				*pbIsDirectory = finder.IsDirectory() ;
			}
			return ( TRUE ) ;
		}
	} catch ( ... ) {
		// SAVE_ERROR_TEXT( lpszFile ) ;
		ASSERT( FALSE ) ;
		return ( FALSE ) ;
	}

	return ( FALSE ) ;
}


int CWin32File::SelectFile( CString& strFile, int nMode /*= READMODE*/, LPCTSTR szDefExt /*= NULL*/, LPCTSTR szFilters /*= _T( "" )*/, LPCTSTR szDefPath/*= NULL*/, CWnd* pParent /*= NULL*/ ) 
{

	CString	strFilters = szFilters ;
	CString strDefExt = szDefExt ;
	CString strDefPath = szDefPath ;

	CString strTmp ;

	if ( strFilters.GetLength() <= 0 ) {
		strFilters = _T( "AllFiles(*.*)|*.*||" ) ;
	}
	BOOL	bOpenMode = !!nMode ;
	
	char	backuppath[ _MAX_PATH ] ;
	memset( backuppath, 0x00, sizeof( backuppath ) ) ;
	::GetCurrentDirectory( _MAX_PATH, backuppath ) ;


	char drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ] ;

	if ( strDefPath.GetLength() > 0 ) {
		_splitpath_s( strDefPath.GetBuffer( 0 ), drive, dir, fname, ext ) ;

		if ( strlen( drive ) < 0 ) 
		{
			TRACE( _T("Not Local Drive \r\n") ) ;
		} 
		else 
		{
			if ( strlen( dir ) < 3 ) {
				strDefPath.Format( _T("%s\\"), drive ) ;
			} else {
				strDefPath.Format( _T("%s%s"), drive, dir ) ;
			}
		}
	} else {
		strDefPath = strFile ;
		int	iPosition = strDefPath.ReverseFind( '\\' ) ;
		if ( iPosition >= 0 ) {
			strDefPath = strDefPath.Left( iPosition ) ;
		}
		if ( strDefPath.GetLength() > 0 ) {
			::SetCurrentDirectory( strDefPath ) ;
		}
	}

	strTmp = strFile ;

	CFileDialog dlg( bOpenMode, strDefExt, strTmp, OFN_HIDEREADONLY, strFilters, pParent ) ;
	dlg.m_ofn.lpstrInitialDir = strDefPath.GetBuffer( 0 ) ;
	if ( dlg.DoModal() != IDOK ) {
		::SetCurrentDirectory( backuppath ) ;
		return ( 1 ) ;
	}
	::SetCurrentDirectory( backuppath ) ;

	strTmp = dlg.GetPathName() ;
	if ( strTmp.GetLength() > 0 ) {
		strFile = strTmp ;
	}

	return ( 0 ) ;

}


int CWin32File::MoveFiles( LPCTSTR lpszSrcname, LPCTSTR lpszTarname, BOOL copymode /*= FALSE*/, BOOL confirm /*= TRUE*/ )
{

	int	iRet = 0 ;
	CString	strError ;
	SHFILEOPSTRUCT	fileOp ;
	char	*srcfilenames = NULL ;
	char	*tarfilenames = NULL ;
	CString	srcname = lpszSrcname ;
	CString	tarname = lpszTarname ;
	int	srclen = srcname.GetLength() + 10 ;
	int	tarlen = tarname.GetLength() + 10 ;

	if ( srcname.GetLength() <= 0 ) {
		strError.Format( _T("MoveFiles::source path is not there [%s]"), srcname ) ;
		// SAVE_ERROR_TEXT( strError ) ;
		return ( -1 ) ;
	}
	if ( tarname.GetLength() <= 0 ) {
		strError.Format( _T("MoveFiles::target path is not there [%s]"), tarname ) ;
		// SAVE_ERROR_TEXT( strError ) ;
		return ( -1 ) ;
	}
	if ( ( srcname.Find( _T( "*" ) ) < 0 )
	  && ( srcname.Find( _T( "?" ) ) < 0 ) ) {
		if ( _access( srcname.GetBuffer( 0 ), 0 ) != 0 ) {
			srcname.ReleaseBuffer() ;
			strError.Format( _T("MoveFiles::source path is not there [%s]"), srcname ) ;
			// SAVE_ERROR_TEXT( strError ) ;
			return ( -1 ) ;
		}
		srcname.ReleaseBuffer() ;
	}

	srcfilenames = new char[ srclen ] ;
	tarfilenames = new char[ tarlen ] ;
	memset( srcfilenames, 0x00, srclen ) ;
	memset( tarfilenames, 0x00, tarlen ) ;
	strcpy_s( srcfilenames, srcname.GetLength(), srcname.GetBuffer( 0 ) ) ;
	srcname.ReleaseBuffer() ;
	strcpy_s( tarfilenames, tarname.GetLength(), tarname.GetBuffer( 0 ) ) ;
	tarname.ReleaseBuffer() ;
	HWND	hWnd = NULL ;
	CWnd	*pWnd = AfxGetMainWnd() ;
	if ( pWnd != NULL ) {
		hWnd = pWnd->m_hWnd ;
	}
	fileOp.hwnd = hWnd ;
	if ( copymode == TRUE ) {
		fileOp.wFunc = FO_COPY ;
		fileOp.lpszProgressTitle = _T( "Data is being copied." ) ;
	} else {
		fileOp.wFunc = FO_MOVE ;
		fileOp.lpszProgressTitle = _T( "Data is moving." ) ;
	}
	fileOp.pFrom = srcfilenames ;
	fileOp.pTo = tarfilenames ;
	fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI ;
	if ( confirm == FALSE ) {
		fileOp.fFlags |= FOF_NOCONFIRMATION ;
	}
	fileOp.fAnyOperationsAborted = FALSE ;
	fileOp.hNameMappings = NULL ;

	TRY
	{
		if ( SHFileOperation( &fileOp ) != 0 ) {
			if ( srcfilenames != NULL ) {
				delete [] srcfilenames ;
				srcfilenames = NULL ;
			}
			if ( tarfilenames != NULL ) {
				delete [] tarfilenames ;
				tarfilenames = NULL ;
			}
			CString	str ;
			str = ::FormatErrorMsg( ::GetLastError() ) ;
			strError.Format( _T("MoveFiles:%s \r\n [%s]->[%s]"), str, srcname, tarname ) ; 
			// SAVE_ERROR_TEXT( strError ) ;
			return ( -1 ) ;
		}
	}
	CATCH( CFileException, e )
	{
		iRet = -1 ;
		CString	str ;
		str = ::FormatErrorMsg( ::GetLastError() ) ;
		strError.Format( _T("MoveFiles:%s \r\n [%s]->[%s]"), str, srcname, tarname ) ; 
		// SAVE_ERROR_TEXT( strError ) ;
		return ( -1 ) ;
	}
	END_CATCH

	if ( srcfilenames != NULL ) {
		delete [] srcfilenames ;
		srcfilenames = NULL ;
	}
	if ( tarfilenames != NULL ) {
		delete [] tarfilenames ;
		tarfilenames = NULL ;
	}

	return ( iRet ) ;

}


int CWin32File::LoadFile( LPCTSTR szFilename, CStringList *lstStore )
{

	ASSERT( lstStore != NULL ) ;

	CStdioFile	cf ;
	CString	strRead = _T( "" ) ;

	if ( !cf.Open( szFilename, ( CFile::typeText | CFile::modeRead ) ) ) {
		CString str ;
		str.Format( _T("FileOpenError[%s]"), szFilename ) ;
		// SAVE_ERROR_TEXT( str ) ;
		return ( -1 ) ;
	}

	while ( 1 ) {
		try {
			if ( cf.ReadString( strRead ) == NULL ) {
				break ;
			}
			strRead.Replace( '\n', '\0' ) ;
			strRead.Replace( '\r', '\0' ) ;
			lstStore->AddTail( strRead ) ;
		} catch ( CFileException &e ) {
			TCHAR	szChar[ 256 ] ;
			e.GetErrorMessage( szChar, 255 ) ;
			// SAVE_ERROR_TEXT( szChar ) ;
			cf.Close() ;
			return ( -1 ) ;
		}
	}
	cf.Close() ;

	return ( 0 ) ;

}


int CWin32File::UnloadFile( LPCTSTR szFilename, CStringList &strStore, BOOL bNoTruncate /*= FALSE*/ )
{

	CStdioFile	cf ;
	CString	str = _T( "" ) ;
	POSITION	pos ;
	UINT	nOpenFlags = CFile::modeCreate | CFile::modeWrite | CFile::typeText ;
	if ( bNoTruncate == TRUE ) {
		nOpenFlags |= CFile::modeNoTruncate ;
	}

	CWin32File::CreateDirectory( szFilename, TRUE ) ;
	if ( !cf.Open( szFilename, nOpenFlags ) ) {
		CString str ;
		str.Format( _T("FileOpenError[%s]"), szFilename ) ;
		// SAVE_ERROR_TEXT( str ) ;
		return ( -1 ) ;
	}
	cf.SeekToEnd() ;

	for ( pos = strStore.GetHeadPosition() ; pos != NULL ; ) {
		str = strStore.GetNext( pos ) ;
		str += _T( "\n" ) ;
		if ( WriteFile( cf, str ) != 0 ) {
			cf.Close() ;
			// SAVE_ERROR_TEXT( str ) ;
			return ( -1 ) ;
		}
	}
	cf.Close() ;

	return ( 0 ) ;

}


int CWin32File::WriteFile( CStdioFile &cf, LPCTSTR lpszString )
{
	try {
		cf.WriteString( lpszString ) ;
	} catch ( CFileException &e ) {
		TCHAR	szChar[ 256 ] ;
		e.GetErrorMessage( szChar, 255 ) ;
		CString str ;
		str.Format( _T("%s[%s]"), szChar, lpszString ) ;
		// SAVE_ERROR_TEXT( str ) ;
		return ( -1 ) ;
	}
	return ( 0 ) ;
}


int CWin32File::DeleteDirectory( LPCTSTR lpszPathName, BOOL bTrush /*= FALSE*/, BOOL bConfirm /*= FALSE*/ ) 
{

	CString	strPath = lpszPathName ;
	int	srclen = strPath.GetLength() + 10 ;

	if ( strPath.GetLength() <= 0 ) {
		return ( -1 ) ;
	}
	if ( _access( strPath.GetBuffer( 0 ), 0 ) != 0 ) {
		strPath.ReleaseBuffer() ;
		return ( -2 ) ;
	}
	strPath.ReleaseBuffer() ;

	int	iRet = 0 ;

	if ( bTrush ) {
		strPath.TrimRight( '\\' ) ;
		strPath += _T( "\\*.*" ) ;
		SHFILEOPSTRUCT	fileOp ;
		char	*pszPath = NULL ;
		pszPath = new char[ srclen ] ;
		memset( pszPath, 0x00, srclen ) ;
		strcpy_s( pszPath, strPath.GetLength(), strPath.GetBuffer( 0 ) ) ;
		strPath.ReleaseBuffer() ;
		HWND	hWnd = NULL ;
		CWnd	*pWnd = AfxGetMainWnd() ;
		if ( pWnd != NULL ) {
			hWnd = pWnd->m_hWnd ;
		}

		fileOp.hwnd = hWnd ;
		fileOp.wFunc = FO_DELETE ;
		fileOp.lpszProgressTitle = _T( "Delete Directory" ) ;
		fileOp.pFrom = pszPath ;
		fileOp.pTo = pszPath ;
		fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI ;
		if ( bConfirm == FALSE ) {
			fileOp.fFlags |= FOF_NOCONFIRMATION ;
		}
		fileOp.fAnyOperationsAborted = FALSE ;
		fileOp.hNameMappings = NULL ;

		if ( SHFileOperation( &fileOp ) != 0 ) {
			CString str = lpszPathName ; 
			str += ::FormatErrorMsg( ::GetLastError() ) ;
			// SAVE_ERROR_TEXT( str ) ;
			iRet = -1 ;
		}

		delete [] pszPath ;
		pszPath = NULL ;
	} else {
		if ( bConfirm ) {
			CString strMsg ;
			strMsg.Format( _T("Are you sure want to delete the folder?\r\n%s"), lpszPathName ) ;
			if ( AfxMessageBox( strMsg, MB_YESNO ) != IDYES ) {
				return ( 0 ) ;
			}
		}

		CFileFind find ;
		CString strPathName = lpszPathName ;
		strPathName.TrimRight( '\\' ) ;
		strPathName += _T( "\\*.*" ) ;
		BOOL bFind = find.FindFile( strPathName ) ;
		while ( bFind ) {
			bFind = find.FindNextFile() ;
			if ( find.IsDots() ) {
				continue ;
			}
			CString strDeleteFile = lpszPathName ;
			strDeleteFile.TrimRight( '\\' ) ;
			strDeleteFile += _T( "\\" ) + find.GetFileName() ;
			if ( find.IsDirectory() ) {
				DeleteDirectory( strDeleteFile, bTrush, FALSE ) ;
			} else {
				::DeleteFile( strDeleteFile ) ;
			}
		}
		find.Close() ;
		::RemoveDirectory( lpszPathName ) ;
	}

	return ( iRet ) ;
}


int CWin32File::DeleteFiles( LPCTSTR lpszPathName, BOOL bTrush /*= FALSE*/, BOOL bConfirm /*= FALSE*/ ) 
{

	if ( bTrush ) {
		return ( DeleteDirectory( lpszPathName, bTrush, bConfirm ) ) ;
	}

	CFile cf ;
	CFileStatus status ;
	if ( cf.GetStatus( lpszPathName, status ) == TRUE ) {
		if ( status.m_attribute == CFile::directory ) {
			return ( DeleteDirectory( lpszPathName, bTrush, bConfirm ) ) ;
		} else {
			if ( bConfirm ) {
				CString strMsg ;
				strMsg.Format( _T("Are you sure want to delete the file?\r\n%s"), lpszPathName ) ;
				if ( AfxMessageBox( strMsg, MB_YESNO ) != IDYES ) {
					return ( 0 ) ;
				}
			}
			return ( DeleteFile( lpszPathName ) ) ;
		}
	}

	return ( -1 ) ;
}


int CWin32File::CheckWin32FileName( CString &filename, BOOL bReplace /*= TRUE*/, char obj /*='-'*/ )
{

	int	i, j ;
	int nRet = 1 ;
	CString	tempname = filename ;
	int	maxlength = tempname.GetLength() ;

	BYTE	buff[ 256 ] ;
	memset( buff, 0x00, sizeof( buff ) ) ;
	int	nCheckMax = 0 ;
	buff[ nCheckMax++ ] = '/' ;
	buff[ nCheckMax++ ] = ':' ;
	buff[ nCheckMax++ ] = '*' ;
	buff[ nCheckMax++ ] = '?' ;
	buff[ nCheckMax++ ] = '"' ;
	buff[ nCheckMax++ ] = '<' ;
	buff[ nCheckMax++ ] = '>' ;
	buff[ nCheckMax++ ] = '|' ;

	BYTE code ;
	i = 0 ;
	while ( 1 ) {
		if ( i >= maxlength ) {
			break ;
		}
		code = tempname.GetAt( i ) ;
		if ( code == 0x00 ) {
			break ;
		}
		if ( iscntrl( code ) != 0 ) {
			if ( !bReplace ) {
				return ( 0 ) ;
			}
			tempname.SetAt( i, obj ) ;
			nRet = 0 ;
		} else {
			for ( j = 0 ; j < nCheckMax ; j++ ) {
				if ( code == buff[ j ] ) {
					if ( !bReplace ) {
						return ( 0 ) ;
					}
					tempname.SetAt( i, obj ) ;
					nRet = 0 ;
					break ;
				}
			}
		}
		i++ ;
	}
	filename = tempname ;

	return ( nRet ) ;

}


double CWin32File::GetDiskSpace( LPCTSTR lpszDrive, double* pdTotal /*= NULL*/, double* pdFree /*= NULL*/, double* pdPercent /*= NULL*/ )
{

	double dRet = 0.0 ;
	ULARGE_INTEGER i64FreeBytesAvailable, i64TotalBytes, i64TotalFreeBytes ;
	double dTotalGB, dFreeGB, dUseGB ;
	TCHAR szDrive[ _MAX_PATH ] ;
	memset( szDrive, 0x00, sizeof( szDrive ) ) ;
	GetDrivename( lpszDrive, szDrive ) ;

	if ( strlen( szDrive ) <= 0 ) {
		strncpy_s( szDrive, lpszDrive, sizeof( szDrive ) ) ;
		if ( strlen( szDrive ) <= 0 ) {
			return ( -1.0 ) ;
		}
	}
	
	if ( ::GetDiskFreeSpaceEx( szDrive,
							   &i64FreeBytesAvailable,
							   &i64TotalBytes,
							   &i64TotalFreeBytes ) == FALSE ) {
		return ( -1.0 ) ;
	}

	dTotalGB	= ( double)( LONGLONG)i64TotalBytes.QuadPart/1024/1024/1024 ;
	dFreeGB		= ( double)( LONGLONG)i64FreeBytesAvailable.QuadPart/1024/1024/1024 ;
	dUseGB		= ( double)( LONGLONG)( i64TotalBytes.QuadPart - i64FreeBytesAvailable.QuadPart )/1024/1024/1024 ;

	dRet = ( dUseGB / dTotalGB ) * 100.0 ;

	if ( pdTotal != NULL ) {
		*pdTotal = dTotalGB ;
	}

	if ( pdFree != NULL ) {
		*pdFree = dFreeGB ;
	}

	if ( pdPercent != NULL ) {
		*pdPercent = dRet ;
	}

	return ( dRet ) ;
}


int CWin32File::GetFileList( LPCTSTR lpszPath, CStringList& rstrFileNameList, LPCTSTR lpszExt /*= _T( "*" )*/, int nMaxFiles /*= 0*/, BOOL bSubFolder /*= FALSE*/, BOOL bListClear /*= TRUE*/ ) 
{

	if ( bListClear ) {
		rstrFileNameList.RemoveAll() ;
	}

	CString strPath = lpszPath ;
	CString strExt = lpszExt ;
	if ( strPath.Right( 1 ).Compare( "\\" ) != 0 ) {
		strPath += _T( "\\" ) ;
	}
	strPath += _T( "*" ) ;
	if ( strExt.Left( 1 ).Compare( "." ) != 0 ) {
		strExt = _T( "." ) + strExt ;
	}
	strPath += strExt ;

	CFileFind	finder ;
	BOOL bWorking = finder.FindFile( strPath ) ;
	if ( bWorking == 0 ) {
		return ( 0 ) ;
	}
	while ( bWorking ) {
		bWorking = finder.FindNextFile() ;
		if (finder.IsDots()) {
			continue;
		}
		if ( finder.IsDirectory() ) {
			if ( bSubFolder ) {
				CString strPath = finder.GetFilePath(); // 22.04.11 Ahn Test
				GetFileList( finder.GetFilePath(), rstrFileNameList, lpszExt, nMaxFiles, bSubFolder, FALSE ) ;
			}
			continue ;
		}
		if ( finder.IsDots() ) {
			continue ;
		}
		rstrFileNameList.AddTail( finder.GetFileName() ) ;
		if ( nMaxFiles > 0 ) {
			if ( rstrFileNameList.GetCount() >= nMaxFiles ) {
				break ;
			}
		}
	}
	finder.Close() ;

	return ( 0 ) ;
}

//KANG 22.01.07 Add Start
// 22.04.11 Ahn Modify Start
//int CWin32File::GetFolderList(LPCTSTR lpszPath, CStringList& rstrFolderNameList, LPCTSTR lpszExt /*= _T( "*" )*/, int nMaxFiles /*= 0*/, BOOL bSubFolder /*= FALSE*/, BOOL bListClear /*= TRUE*/)
int CWin32File::GetFolderList(LPCTSTR lpszPath, CStringList& rstrFolderNameList, LPCTSTR lpszExt /*= _T( "*" )*/, int nMaxFiles /*= 0*/, BOOL bSubFolder /*= FALSE*/, BOOL bListClear /*= TRUE*/, BOOL bFullPath /*=FALSE*/)
// 22.04.11 Ahn Modify End
{
	// 22.03.21 Ahn Add Start
	int nRet = 0;
	// 22.03.21 Ahn Add End
	if (bListClear) {
		rstrFolderNameList.RemoveAll();
	}

	CString strPath = lpszPath;
	CString strExt = lpszExt;
	if (strPath.Right(1).Compare("\\") != 0) {
		strPath += _T("\\");
	}
	strPath += _T("*");
	if (strExt.Left(1).Compare(".") != 0) {
		strExt = _T(".") + strExt;
	}
	strPath += strExt;

	CFileFind	finder;
	BOOL bWorking = finder.FindFile(strPath);
	if (bWorking == 0) {
		return (0);
	}
	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) {
			continue;
		}
		if (finder.IsDirectory()) {
			if (bSubFolder) {
				GetFolderList(finder.GetFilePath(), rstrFolderNameList, lpszExt, nMaxFiles, bSubFolder, FALSE, bFullPath);
			}
			if (bFullPath == FALSE) {
				rstrFolderNameList.AddTail(finder.GetFileName());
			} else {
				CString strFullPath;
				strFullPath.Format(_T("%s\\%s"), lpszPath, finder.GetFileName());
				rstrFolderNameList.AddTail(strFullPath);
			}
			nRet++;
			if (nMaxFiles > 0) {
				if (rstrFolderNameList.GetCount() >= nMaxFiles) {
					break;
				}
			}
		}

	}
	finder.Close();

	return (nRet);
}

int CWin32File::GetFolderCount(LPCTSTR lpszPath, LPCTSTR lpszExt /*= _T( "*" )*/, int nMaxFiles /*= 0*/, BOOL bSubFolder /*= FALSE*/, BOOL bListClear /*= TRUE*/)
{
	int nRet = 0;
	CString strPath = lpszPath;
	CString strExt = lpszExt;
	if (strPath.Right(1).Compare("\\") != 0) {
		strPath += _T("\\");
	}
	strPath += _T("*");
	if (strExt.Left(1).Compare(".") != 0) {
		strExt = _T(".") + strExt;
	}
	strPath += strExt;

	CFileFind	finder;
	BOOL bWorking = finder.FindFile(strPath);
	if (bWorking == 0) {
		return (0);
	}
	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) {
			continue;
		}
		if (finder.IsDirectory()) {
			if (bSubFolder) {
				GetFolderCount(finder.GetFilePath(), lpszExt, nMaxFiles, bSubFolder, FALSE);
			}
			nRet++;
		}

	}
	finder.Close();

	return (nRet);
}
//KANG 22.01.07 Add End

int CWin32File::PickupFileList_inFolder( LPCTSTR lpszPathName, CStringList *pstrList, BOOL bPickupFolder /*= FALSE*/, BOOL bPickupFile /*= TRUE*/ )
{
	int	nPathLength = ( int)( strlen( lpszPathName ) ) ;
	ASSERT( nPathLength > 0 ) ;
	if ( nPathLength <= 0 ) {
		CString	strLocal ;
		strLocal.Format( _T("%d,%d,%s"), ( int)bPickupFolder, ( int)bPickupFile, lpszPathName ) ;
		// SAVE_ERROR_TEXT( strLocal ) ;
		return ( -1 ) ;
	}
	ASSERT( pstrList != NULL ) ;
	if ( pstrList == NULL ) {
		CString	strLocal ;
		strLocal.Format( _T("%d,%d,%s"), ( int)bPickupFolder, ( int)bPickupFile, lpszPathName ) ;
		// SAVE_ERROR_TEXT( strLocal ) ;
		return ( -1 ) ;
	}
	if ( ( bPickupFolder != TRUE ) && ( bPickupFile != TRUE ) ) {
		CString	strLocal ;
		strLocal.Format( _T("%d,%d,%s"), ( int)bPickupFolder, ( int)bPickupFile, lpszPathName ) ;
		// SAVE_ERROR_TEXT( strLocal ) ;
		ASSERT( FALSE ) ;
		return ( -1 ) ;
	}
	int	nRet = 0 ;
	CString	strOne ;
	pstrList->RemoveAll() ;
	CFileFind	finder ;
	BOOL	bWorking = finder.FindFile( lpszPathName ) ;
	while ( bWorking ) {
		bWorking = finder.FindNextFile() ;
		if ( finder.IsDirectory() != FALSE ) {
			if ( finder.IsDots() != FALSE ) {
				continue ;
			}
			if ( bPickupFolder != TRUE ) {
				continue ;
			}
			strOne = finder.GetFileName() ;
			pstrList->AddTail( strOne ) ;
		} else {
			if ( bPickupFile != TRUE ) {
				continue ;
			}
			strOne = finder.GetFileName() ;
			pstrList->AddTail( strOne ) ;
		}
	}
	return ( nRet ) ;
}


int	CWin32File::TextSave1Line( LPCTSTR lpszPath, LPCTSTR lpszFname, LPCTSTR lpszSavemsg, LPCTSTR lpszModemsg, BOOL bErrflag, int iMaxFileSize /*= MAXFILESIZE*/, LPCTSTR lpszIndexMsg /*= _T( "" )*/ )
{

	CString	filename = _T( "" ) ;
	struct _stat	st_buf ;
	UINT	filesize ;
	UINT	openmode = 0 ;
	CFile	file ;
	CFileStatus status ;
	CFileException errorex ;

	if ( ( _stricmp( lpszModemsg, _T( "at" ) ) != 0 )
	  && ( _stricmp( lpszModemsg, _T( "wt" ) ) != 0 ) ) {
		return ( -1 ) ;
	}

	if ( strlen( lpszPath ) > 0 ) {
		filename = lpszPath ;
		filename += _T( "\\" ) ;
		filename += lpszFname ;
	} else {
		filename = lpszFname ;
	}

	filesize = ( UINT)( strlen( lpszSavemsg ) ) ;
	if ( filesize <= 0 ) {
		return ( -2 ) ;
	}

	BOOL	bIndexSave = TRUE ;
	openmode = CFile::modeCreate | CFile::modeWrite ;
	if ( _stricmp( lpszModemsg, _T( "at" ) ) == 0 ) {
		openmode = openmode | CFile::modeNoTruncate ;
		bIndexSave = FALSE ;
	}

	if ( _access( filename.GetBuffer( 0 ), 0 ) == 0 ) {
		filename.ReleaseBuffer() ;
		if ( _stat( filename.GetBuffer( 0 ), &st_buf ) != 0 ) {
			filename.ReleaseBuffer() ;
			return ( -3 ) ;
		}
		filename.ReleaseBuffer() ;
		if ( iMaxFileSize > 0 ) {
			if (st_buf.st_size > iMaxFileSize) {
				if ( ::DeleteFile( filename ) == FALSE ) {
					//
				}
				bIndexSave = TRUE ;
			}
		}
	} else {
		filename.ReleaseBuffer() ;
		bIndexSave = TRUE ;
	}

	CWin32File::CreateDirectory( filename, TRUE ) ;

	if ( file.Open( filename, openmode, &errorex ) == 0 ) {
		if ( bErrflag == TRUE ) {
			AfxMessageBox( GetFileExceptionString( errorex ) ) ;
		}
		return ( -4 ) ;
	}

	file.SeekToEnd() ;

	if ( bIndexSave == TRUE ) {
		CString	strIndexMsg = lpszIndexMsg ;
		int	iSize = strIndexMsg.GetLength() ;
		if ( iSize > 0 ) {
			file.Write( strIndexMsg.GetBuffer( 0 ), iSize ) ;
			strIndexMsg.ReleaseBuffer() ;
		}
	}

	file.Write( lpszSavemsg, filesize ) ;
	file.Close() ;

	return ( 0 ) ;

}


int	CWin32File::TextRead1Line( LPCTSTR lpszPath, LPCTSTR lpszFname, LPTSTR lpszReadmsg, UINT readsize, BOOL bErrflag )
{

	CString	filename = _T( "" ) ;
	struct _stat	st_buf ;
	UINT	openmode = 0 ;
	CFile	file ;
	CFileStatus	status ;
	CFileException	errorex ;

	if ( strlen( lpszPath ) > 0 ) {
		filename = lpszPath ;
		filename += _T( "\\" ) ;
		filename += lpszFname ;
	} else {
		filename = lpszFname ;
	}

	openmode = CFile::modeCreate | CFile::modeNoTruncate ;

	if ( file.GetStatus( filename, status ) == 0 ) {	
		// SAVE_ERROR_TEXT( filename ) ;
		return ( -1 ) ;
	}
	if ( _access( filename.GetBuffer( 0 ), 0 ) != 0 ) {
		filename.ReleaseBuffer() ;
		// SAVE_ERROR_TEXT( filename ) ;
		return ( -1 ) ;
	}
	filename.ReleaseBuffer() ;

	if ( _stat( filename.GetBuffer( 0 ), &st_buf ) != 0 ) {
		filename.ReleaseBuffer() ;
		// SAVE_ERROR_TEXT( filename ) ;
		return ( -1 ) ;
	}
	filename.ReleaseBuffer() ;
	if ( st_buf.st_size == 0 ) {
		// SAVE_ERROR_TEXT( filename ) ;
		return ( -1 ) ;
	}
	if ( ( UINT)st_buf.st_size < readsize ) {
		readsize = ( UINT)st_buf.st_size ;
	}

	if ( file.Open( filename, openmode, &errorex ) == 0 ) {
		if ( bErrflag == TRUE ) {
			AfxMessageBox( GetFileExceptionString( errorex ) ) ;
		}
		// SAVE_ERROR_TEXT( filename ) ;
		return ( -1 ) ;
	}

	file.Read( lpszReadmsg, readsize ) ;
	file.Close() ;

	return ( 0 ) ;

}


int CWin32File::DeleteDirectoryOnly( LPCTSTR lpszFolder ) 
{
	int	iRet = 1 ;

	TRY
	{
		if ( ::RemoveDirectory( lpszFolder ) == FALSE ) {
			iRet = 0 ;
			// SAVE_ERROR_TEXT( lpszFolder ) ;
		}
	}
	CATCH( CFileException, e )
	{
		iRet = 0 ;
		// SAVE_ERROR_TEXT( lpszFolder ) ;
	}
	END_CATCH

	return ( iRet ) ;

}


int CWin32File::DeleteEmptyDirectory( LPCTSTR lpszPathSrc, BOOL bContainSubDir /*= TRUE*/, BOOL bDeleteBasePath /*= TRUE*/, BOOL *pbPathEmpty /*= NULL*/ )
{
	CString	strPath = lpszPathSrc ;
	if ( strPath.GetLength() <= 0 ) {
		// SAVE_ERROR_TEXT( strPath ) ;
		return ( -1 ) ;
	}
	int	iRet = 0 ;
	BOOL	bFolderEmpty = TRUE ;
	BOOL	bPartEmpty = TRUE ;
	strPath += _T( "\\*.*" ) ;
	CFileFind	finder ;
	BOOL	bWorking = finder.FindFile( strPath ) ;
	if ( bWorking != 0 ) {
		CString	szNowFile = _T( "" ) ;
		while ( bWorking ) {
			bWorking = finder.FindNextFile() ;
			if ( finder.IsDots() == TRUE ) {
				continue ;
			}
			szNowFile = finder.GetFilePath() ;
			if ( finder.IsDirectory() == FALSE ) {
				bFolderEmpty = FALSE ;
			} else {
				if ( bContainSubDir == TRUE ) {
					bPartEmpty = TRUE ;
					if ( DeleteEmptyDirectory( szNowFile, bContainSubDir, TRUE, &bPartEmpty ) < 0 ) {
						iRet = -1 ;
						CString	strLocal ;
						strLocal.Format( _T("%d"), ( int)bContainSubDir ) ;
						// SAVE_ERROR_TEXT( strLocal ) ;
						// SAVE_ERROR_TEXT( szNowFile ) ;
					}
					if ( bPartEmpty != TRUE ) {
						bFolderEmpty = FALSE ;
					}
				} else {
					bFolderEmpty = FALSE ;
				}
			}
		}
	}
	finder.Close() ;
	if ( bFolderEmpty == TRUE ) {
		if ( bDeleteBasePath == TRUE ) {
			if ( DeleteDirectoryOnly( lpszPathSrc ) == FALSE ) {
				// SAVE_ERROR_TEXT( lpszPathSrc ) ;
			}
		}
	}
	if ( pbPathEmpty != NULL ) {
		*pbPathEmpty = bFolderEmpty ;
	}
	return ( iRet ) ;
}

int CWin32File::DeleteFolder(CString strPath)
{
	int nRet = 0 ;

	CFileFind finder ;
	BOOL bContinue = TRUE ;

	if ( strPath.Right(1) != _T( "\\" ) ) 
		strPath += _T( "\\" ) ;
	strPath += _T( "*.*" ) ;
	bContinue = finder.FindFile( strPath ) ;

	while ( bContinue ) {
		bContinue = finder.FindNextFile() ;
		if ( finder.IsDots() ) {
			continue ;
		}
		else if ( finder.IsDirectory() ) {
			DeleteFolder( finder.GetFilePath() ) ;
			::RemoveDirectory( (LPCTSTR)finder.GetFilePath() ) ;
		}
		else	// Delete File.
		{
			::DeleteFile( (LPCTSTR)finder.GetFilePath() ) ;
		}
	}
	finder.Close() ;
	strPath = strPath.Left( strPath.ReverseFind( '\\' ) ) ;
	::RemoveDirectory( (LPCTSTR)strPath ) ;

	return nRet ;
}
