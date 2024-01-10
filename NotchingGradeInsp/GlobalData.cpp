#include "pch.h"
#include "GlobalData.h"
#include "Win32File.h"
#include "CImageProcess.h"
#include "CRecipeCtrl.h"
#include "SigProc.h"
#include "NotchingGradeInsp.h"
#include "CAprHash.h"				// 22.05.25 Son Add
#include "CHistoryLotCtrl.h"		// 22.06.27 Ahn Add

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
//SPC+ 사용 해더파일
#include "SpcInfo.h"
#include "SpcPlusManager.h"
#include "SpcStatusManager.h"
#include "SpcStatusInData.h"
#endif //SPCPLUS_CREATE


CGlobalData	AprData ;

CGlobalData::CGlobalData(void)
{
	::InitializeCriticalSection( &m_csSaveLog ) ;
	::InitializeCriticalSection( &m_csSaveMemLog ) ;	

	::InitializeCriticalSection( &m_csTt1 );
	::InitializeCriticalSection( &m_csTt2 );
	::InitializeCriticalSection( &m_csTt3 );
	::InitializeCriticalSection( &m_csTt4 );

	ResetAllCounter() ;
	// 22.07.06 Ahn Add Start
	m_NowLotData.ClearAllCount();
	// 22.07.06 Ahn Add End

	m_nSleep = 1;

	char chBuff[_MAX_DIR];
	::GetCurrentDirectory(_MAX_DIR, chBuff);

	m_strDataPath.Format( _T("%s\\Data"), chBuff);
	// 22.06.30 Ahn Add Start
	m_strResultPath = _T("D:\\DAT\\FOIL");
	// 22.06.30 Ahn Add End

	// 22.07.27 Ahn Modify Start
	// m_strLogPath.Format(_T("%s\\Data\\LOG"), chBuff);
	m_strLogPath.Format(_T("%s\\LOG"), m_strResultPath);
	// 22.07.27 Ahn Modify End
 
	// 22.05.17 Ahn Modify Start
	m_strImagePath.Format(_T("%s\\IMG\\INSP"), m_strResultPath);
	m_strFeederPath.Format( _T("%s\\FEEDER"), m_strResultPath); // Default Path ;
	// 22.05.17 Ahn Modify End

	m_strRecipeLogPath = _T("D:\\DAT\\FOIL\\RECIPE_LOG"); // 22.06.08 Ahn Add 

	m_pRecipeInfo = NULL;
	m_bItPassMode = FALSE;

	// 22.07.04 Ahn Add Start
	m_dDiskTotal = 0.0 ;
	m_dDiskFree = 0.0;
	m_dDiskPercent = 0.0;
	CWin32File::GetDiskSpace(m_strResultPath, &m_dDiskTotal, &m_dDiskFree, &m_dDiskPercent);
	// 22.07.04 Ahn Add End

// 22.05.25 Son Add Start
#if !defined(_DEBUG)
	UserMode = UserModeDefine::enOperator;
#else
	UserMode = UserModeDefine::enMaker; // 22.09.15 Ahn Modify
#endif
	m_font.CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Arial"));
// 22.05.25 Son Add End

	// 22.06.08 Ahn Add Start
	m_System.FileCtrl(MODE_READ);
	// 22.06.08 Ahn Add End

	CString strPath;
	CString strFileName;
	strPath = AprData.m_strDataPath ;
	strFileName = _T("LOT_HISTORY.bin");
	m_pHistoryLotCtrl = new CHistoryLotCtrl( strPath, strFileName ) ; // 22.06.27 Ahn Add
	m_pHistoryLotCtrl->ReadLotFileList(NULL);

	FileCtrl_LotInfo( en_mode_Initial ); // 22.06.24 Ahn Modify

	m_DebugSet.FileCtrl(MODE_READ);
	InitRecipe();

	m_ErrStatus.ErrorClear() ; // 22.05.19 Ahn Add 

	m_globalStr.SetOutImageFormat(".jpg");

	m_nMissTabIdNow = 0;

	//FileCtrl_DuplicateNG(MODE_READ);
}

CGlobalData::~CGlobalData(void)
{
	::DeleteCriticalSection( &m_csSaveLog ) ;
	::DeleteCriticalSection( &m_csSaveMemLog ) ;

	::DeleteCriticalSection( &m_csTt1 );
	::DeleteCriticalSection( &m_csTt2 );
	::DeleteCriticalSection( &m_csTt3 );
	::DeleteCriticalSection( &m_csTt4 );

	if (m_pRecipeInfo != NULL) {
		delete m_pRecipeInfo;
		m_pRecipeInfo = NULL;
	}

	if (m_pHistoryLotCtrl != NULL) {
		delete m_pHistoryLotCtrl ;
		m_pHistoryLotCtrl = NULL ;
	}
}

void CGlobalData::ResetAllCounter() 
{
	ResetFrameCounter() ;
}

void CGlobalData::ResetFrameCounter() 
{
	for (int i = 0; i < GRABBER_COUNT; i++) {
		nFrameCounter[i] = 0;
	}
}

void CGlobalData::SetFrameCounter( int nHeadNo, int nNo ) 
{
	nFrameCounter[nHeadNo] = nNo  ;
}

int CGlobalData::SaveDebugLog( CString strMsg )
{
	::EnterCriticalSection( &m_csSaveLog ) ;
	CWin32File file ;
	// 22.07.27 Ahn Modify Start
	//CString strFilePath = m_strLogPath;
	CString strFilePath ;
	// 22.07.27 Ahn Modify End
	CString strTime ;
	CString strSaveMsg ;

	SYSTEMTIME	sysTime ;
	::GetLocalTime( &sysTime ) ;

	strTime.Format( _T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d, ")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
		) ;

	// 22.07.27 Ahn Modify Start
	strFilePath.Format( _T("%s\\%04d%02d\\%02d"), m_strLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay ) ;
	// 22.07.27 Ahn Modify End
	strSaveMsg = strTime + strMsg + _T("\r\n") ;

	CString strFileName = "";
	strFileName.Format(_T("DEBUG_LOG-%04d%02d%02d-%02d.txt")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
	);

	file.TextSave1Line( strFilePath, strFileName, strSaveMsg, "at", FALSE, 999999999 ) ;

	::LeaveCriticalSection( &m_csSaveLog ) ;

	return 0 ;
}

void CGlobalData::SaveDebugLog_Format(const char* format, ...)
{
	va_list arg;
	int done;
	char str[MAX_PATH] = { 0, };
	va_start(arg, format);
	done = vsprintf_s(str, format, arg);
	va_end(arg);
	if (done > 0)
	{
		SaveDebugLog(str);
	}	
}

int CGlobalData::SaveErrorLog(CString strMsg)
{
	::EnterCriticalSection(&m_csSaveLog);
	CWin32File file;
	// 22.07.27 Ahn Modify Start
	//CString strFilePath = m_strLogPath;
	CString strFilePath;
	// 22.07.27 Ahn Modify End
	CString strTime;
	CString strSaveMsg;

	SYSTEMTIME	sysTime;
	::GetLocalTime(&sysTime);

	strTime.Format(_T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d, ")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
	);

	// 22.07.27 Ahn Modify Start
	strFilePath.Format(_T("%s\\%04d%02d\\%02d"), m_strLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	// 22.07.27 Ahn Modify End
	strSaveMsg = strTime + strMsg + _T("\r\n");

	CString strFileName;
	strFileName = _T("ERROR_LOG.txt") ;


	file.TextSave1Line(strFilePath, strFileName, strSaveMsg, "at", FALSE, 999999999);

	::LeaveCriticalSection(&m_csSaveLog);

	return 0;
}

int CGlobalData::SaveTactLog(CString strMsg)
{
	::EnterCriticalSection(&m_csSaveLog);
	CWin32File file;
	// 22.07.27 Ahn Modify Start
	//CString strFilePath = m_strLogPath;
	CString strFilePath;
	// 22.07.27 Ahn Modify End
	CString strTime;
	CString strSaveMsg;

	SYSTEMTIME	sysTime;
	::GetLocalTime(&sysTime);

	strTime.Format("%04d/%02d/%02d ,%02d:%02d:%02d.%03d, "
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
	);

	// 22.07.27 Ahn Modify Start
	strFilePath.Format(_T("%s\\%04d%02d\\%02d"), m_strLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	// 22.07.27 Ahn Modify End
	strSaveMsg = strTime + strMsg + _T("\r\n");

	CString strFileName;
	strFileName = _T("TactTime_LOG.txt");

	file.TextSave1Line(strFilePath, strFileName, strSaveMsg, "at", FALSE, 999999999);

	::LeaveCriticalSection(&m_csSaveLog);

	return 0;
}

int CGlobalData::SaveLotLog(CString strMsg)
{
	::EnterCriticalSection(&m_csSaveLog);
	CWin32File file;
	// 22.07.27 Ahn Modify Start
	//CString strFilePath = m_strLogPath;
	CString strFilePath;
	// 22.07.27 Ahn Modify End
	CString strTime;
	CString strSaveMsg;

	SYSTEMTIME	sysTime;
	::GetLocalTime(&sysTime);

	strTime.Format(_T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d, ")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
	);

	// 22.07.27 Ahn Modify Start
	strFilePath.Format(_T("%s\\%04d%02d\\%02d"), m_strLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	// 22.07.27 Ahn Modify End

	strSaveMsg = strTime + strMsg + _T("\r\n");

	CString strFileName;
	strFileName = _T("LOT_LOG.txt") ;

	file.TextSave1Line(strFilePath, strFileName, strSaveMsg, "at", FALSE, 999999999);

	::LeaveCriticalSection(&m_csSaveLog);

	return 0;
}

int CGlobalData::SaveFrameLog(CString strMsg, int nNo )
{
	::EnterCriticalSection(&m_csSaveLog);
	CWin32File file;
	// 22.07.27 Ahn Modify Start
	//CString strFilePath = m_strLogPath;
	CString strFilePath;
	// 22.07.27 Ahn Modify End
	CString strTime;
	CString strSaveMsg;

	SYSTEMTIME	sysTime;
	::GetLocalTime(&sysTime);

	strTime.Format(_T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d, ")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
	);

	// 22.07.27 Ahn Modify Start
	strFilePath.Format(_T("%s\\%04d%02d\\%02d"), m_strLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	// 22.07.27 Ahn Modify End

	strSaveMsg = strTime + strMsg + _T("\r\n");

	CString strFileName;
	strFileName.Format(_T("Frame_LOG_%02d.txt"), nNo );


	file.TextSave1Line(strFilePath, strFileName, strSaveMsg, "at", FALSE, 999999999);

	::LeaveCriticalSection(&m_csSaveLog);

	return 0;
}

int CGlobalData::SaveMemoryLog( CString strMsg )
{
	::EnterCriticalSection( &m_csSaveMemLog ) ;
	CWin32File file ;
	// 22.07.27 Ahn Modify Start
	//CString strFilePath = m_strLogPath;
	CString strFilePath;
	// 22.07.27 Ahn Modify End
	CString strFileName = _T("MemoryLog.txt") ;
	CString strTime ;
	CString strSaveMsg ;

	SYSTEMTIME	sysTime ;
	::GetLocalTime( &sysTime ) ;

	strTime.Format( _T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d, ")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
		) ;

	// 22.07.27 Ahn Modify Start
	strFilePath.Format(_T("%s\\%04d%02d\\%02d"), m_strLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	// 22.07.27 Ahn Modify End

	strSaveMsg = strTime + strMsg + _T("\r\n") ;

	file.TextSave1Line( strFilePath, strFileName, strSaveMsg, "at", FALSE, 999999999 ) ;

	::LeaveCriticalSection( &m_csSaveMemLog ) ;

	return 0 ;
}

void CGlobalData::SetTactTime_1( double dTime ) 
{
	::EnterCriticalSection( &m_csTt1 ) ;
	m_dTact_1 = dTime ;
	::LeaveCriticalSection( &m_csTt1 ) ;
}
void CGlobalData::SetTactTime_2( double dTime ) 
{
	::EnterCriticalSection( &m_csTt2 ) ;
	m_dTact_2 = dTime ;
	::LeaveCriticalSection( &m_csTt2 ) ;
}
void CGlobalData::SetTactTime_3( double dTime ) 
{
	::EnterCriticalSection( &m_csTt3 ) ;
	m_dTact_3 = dTime ;
	::LeaveCriticalSection( &m_csTt3 ) ;
}
void CGlobalData::SetTactTime_4( double dTime ) 
{
	::EnterCriticalSection( &m_csTt4 ) ;
	m_dTact_4 = dTime ;
	::LeaveCriticalSection( &m_csTt4 ) ;
}

BOOL CGlobalData::GetDebug(CString strDebug)
{
	BOOL bRet = FALSE;
	if (strDebug.Compare(_T("Stage")) == 0) {
		bRet = TRUE;
	}
	return bRet;
}

void CGlobalData::InitRecipe()
{
	m_pRecipeInfo = new CRecipeInfo;

	int nLocalRet = LoadRecipeInfo();
}


int CGlobalData::LoadRecipeInfo()
{
	ASSERT(m_pRecipeInfo);
	CRecipeCtrl recipeCtrl;
	CWin32File file;
	CString strFileName;
	CString strRecipeName;
	int nRet = 0 ;

	if (AprData.m_NowLotData.m_strRecipeName.GetLength() > 0) {
		strRecipeName = AprData.m_NowLotData.m_strRecipeName;
		strFileName.Format(_T("%s\\Recipe\\%s.ini"), AprData.m_strDataPath, AprData.m_NowLotData.m_strRecipeName);
	}
	else {
		strFileName.Format(_T("%s\\Recipe\\Default.ini"), AprData.m_strDataPath);
		strRecipeName = _T("Default");
	}
	if (file.ExistFile(strFileName) == FALSE) {
		strRecipeName = _T("Default");
	}
	else {
		CStringList strList;
		recipeCtrl.GetRecipeList(strList) ;
	}

	nRet = recipeCtrl.LoadRecipe(AprData.m_pRecipeInfo, strRecipeName);

	return nRet;
}

// 22.06.24 Ahn Add Start
int CGlobalData::LotInfoSave()
{
	
	CString strFileName;
	strFileName.Format(_T("%s\\%s.ini"), AprData.m_strNowBinPath, AprData.m_NowLotData.m_strLotNo);
	CString strSection;
	CString strKey;
	CString strData;
	strSection = _T("LOT_RESULT");
	strKey = _T("TOTAL_COUNT");
	strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCount);
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);
	strKey = _T("NG_COUNT");
	strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCountNG);
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);
	strKey = _T("OK_COUNT");
	// 22.05.27 Ahn Modify Start
	//strData.Format(_T("%d"),( AprData.m_NowLotData.m_nTabCountOK ) );
	strData.Format(_T("%d"), (AprData.m_NowLotData.m_nTabCount - AprData.m_NowLotData.m_nTabCountNG));
	// 22.05.27 Ahn Modify End
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);
	AprData.m_NowLotData.m_nLastTotalCnt = AprData.m_NowLotData.m_nTabCount;
	// 22.11.30 Ahn Add Start
	strKey = _T("FRAME_COUNT");
	strData.Format(_T("%d"), AprData.m_NowLotData.m_nFrameCount);
	::WritePrivateProfileString(strSection, strKey, strData, strFileName);
	// 22.11.30 Ahn Add End

	return 0;
}
// 22.06.24 Ahn Add End

int CGlobalData::LotStartProcess(BOOL bSigInMode, int nDebugMode )
{
	//m_NowLotData.m_strLotNo = _T("");

	SYSTEMTIME	sysTime;
	// 22.07.13 Ahn Modify Start
	//::GetLocalTime(&sysTime);

	//PLC 작동 오프젝트 객체을 얻는다.
	CSigProc* pSigProc = theApp.m_pSigProc;
	if (bSigInMode == TRUE) {
		//PLC 
		pSigProc->SigOutLotStartAck(TRUE);
		::GetLocalTime(&sysTime);
		m_NowLotData.m_LotStartTime = sysTime;
	}
	else
	{
		sysTime = m_NowLotData.m_LotStartTime;
	}
	// 22.07.01 Ahn Add Modify End
	// 22.07.13 Ahn Modify Start

	CString strTime;
	strTime.Format(_T("%d\\%02d\\%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	m_NowLotData.m_strDate = strTime;

	CString strRsltPath;
	CString strhhmmss;
	strhhmmss.Format(_T("%02d%02d%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Melsec) == TRUE)
	{
		if( (nDebugMode & 0x0001) == TRUE )
		{
			CString strLotID;
			strLotID.Format(_T("LOTID_%02d%02d%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			m_NowLotData.m_strLotNo = strLotID;
		}
	}
	else 
	{
		//PLC Lot 번호 가져오기
		pSigProc->ReadBlockAllData(&m_SeqDataIN); // 22.04.01 Ahn Modify
		AprData.m_NowLotData.m_strLotNo = AprData.m_SeqDataIN.strCell_ID ;

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
//SPC+ 사용 해더파일
		if((AprData.m_NowLotData.m_strLotNo.GetLength() > 5) && (CWin32File::IsNumAlpha(AprData.m_NowLotData.m_strLotNo) == 0))
		{
			SPCINFO->setLotId(AprData.m_NowLotData.m_strLotNo);
			LOGDISPLAY_SPEC(3)("!! SPC+=====Lot Id 설정<%s> ", AprData.m_NowLotData.m_strLotNo);
		}
		else
		{
			SPCINFO->setLotId("NA");
			LOGDISPLAY_SPEC(3)("!! SPC+=====Lot Id 설정에러 - 숫자또는영문이 아님");
		}
#endif //SPCPLUS_CREATE

		CString strRecipeName;
		strRecipeName = AprData.m_SeqDataIN.strRecipeName;
		strRecipeName.TrimLeft();
		strRecipeName.TrimRight();

		CString strMsg; 
		strMsg.Format(_T("READ_ROLL_DATA ROLL_ID[%s], RecipeName[%s]"), AprData.m_SeqDataIN.strCell_ID, strRecipeName );
		AprData.SaveLotLog(strMsg);
	}

	CString strYYYYMM;
	CString strHH;
	CString strDD;
	CString strSavePath ;
	strYYYYMM.Format(_T("%d%02d"), sysTime.wYear, sysTime.wMonth);
	strHH.Format(_T("%02d"), sysTime.wHour);
	strDD.Format(_T("%02d"), sysTime.wDay);

	AprData.m_strNowDatePath.Format(_T("%s\\%s\\%s"), strYYYYMM, strDD, strHH);

	// D:/DAT/FOIL/INSP/OK Or NG/YYYYMM/DDHH/LOTID/....
	// 22.07.13 Ahn Modify Start
	//strSavePath.Format("%s\\NG\\%s\\%s\\%s\\%s", AprData.m_strImagePath, strYYYYMM, strDD, strHH, AprData.m_NowLotData.m_strLotNo);
	strSavePath.Format(_T("%s\\NG\\%s\\%s"), AprData.m_strImagePath, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);
	// 22.07.13 Ahn Modify End
	AprData.m_strNowNgPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	strSavePath.Format(_T("%s\\CROP"), AprData.m_strNowNgPath );
	CWin32File::CreateDirectory(strSavePath);

	strSavePath.Format(_T("%s\\Overlay"), AprData.m_strNowNgPath );
	CWin32File::CreateDirectory(strSavePath);
	// 22.07.13 Ahn Modify Start
	//strSavePath.Format("%s\\OK\\%s\\%s\\%s\\%s", AprData.m_strImagePath, strYYYYMM, strDD, strHH, AprData.m_NowLotData.m_strLotNo);
	strSavePath.Format(_T("%s\\OK\\%s\\%s"), AprData.m_strImagePath, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);
	// 22.07.13 Ahn Modify End
	AprData.m_strNowOkPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	strSavePath.Format(_T("%s\\%s\\%s"), AprData.m_strFeederPath, strYYYYMM, strDD);
	AprData.m_strNowCsvPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	// 23.02.07 Ahn Add Start
	strSavePath.Format(_T("%s\\CROP"), AprData.m_strNowOkPath);
	CWin32File::CreateDirectory(strSavePath);

	strSavePath.Format(_T("%s\\Overlay"), AprData.m_strNowOkPath);
	CWin32File::CreateDirectory(strSavePath);
	// 23.02.07 Ahn Add End

	//22.06.28 Ahn Modify End
	// 22.07.13 Ahn Modify Start
	//strSavePath.Format(_T("%s\\BINARY\\%s\\%s\\%s\\%s"), AprData.m_strResultPath, strYYYYMM, strDD, strHH, AprData.m_NowLotData.m_strLotNo);
	strSavePath.Format(_T("%s\\BINARY\\%s\\%s"), AprData.m_strResultPath, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);
	// 22.07.13 Ahn Modify End
	AprData.m_strNowBinPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	// 22.05.17 Ahn Modify End

	// 22.07.07 Ahn Add Start
	if (bSigInMode == TRUE)
	{
		// 22.05.25 Son Add Start
		strSavePath.Format(_T("%s\\%s.bin"), AprData.m_strNowBinPath, AprData.m_NowLotData.m_strLotNo);
		CreateIndexFile(strSavePath);
		// 22.05.25 Son Add End
	}
	// 22.07.07 Ahn Add End

	CString strLine;
	// 22.07.01 Ahn Add Start
	//strLine.Format(_T("LOT ID,Cell No,검사시각,Judge,Top,Btm,Top Defect Size X,Top Defect Size Y, BTM Defect Size X, BTM Defect Size Y,Ink Marking,Marking Reason\r\n"));
	strLine.Format(_T("LOT ID,Cell No,Cell ID,Key ID,검사시각,Judge,Tab,Btm,Surface Count,FoilExp Count,Top Max Size,BTM Max Size,Ink Marking,Marking Reason\r\n"));
	// 22.07.01 Ahn Add End
	CString strFileName;
	strFileName.Format( _T("%s.csv"), AprData.m_NowLotData.m_strLotNo ) ;
	// 파일 존재하면 기록하지 않아도 됨.
	//22.06.28 Ahn Modify Start
	//CWin32File::TextSave1Line(AprData.m_strNowCsvPath, strFileName, strLine, _T("at"), FALSE);
	CString strCsvFileName;
	strCsvFileName.Format(_T("%s\\%s"), AprData.m_strNowCsvPath, strFileName);
	if (CWin32File::ExistFile(strCsvFileName) == FALSE) {
		CWin32File::TextSave1Line(AprData.m_strNowCsvPath, strFileName, strLine, _T("at"), FALSE);
	}

	CString strMsg;
	
	// 22.08.09 Ahn Add Start
	m_NowLotData.m_nContinueCount = 0; 
	m_NowLotData.m_secNgJudge.ResetAll();
	// 22.08.09 Ahn Add End
	
	// 22.08.10 Ahn Add Start
	m_nCoutinuouCount = (int)m_SeqDataIN.wContinousCount ;
	m_nSectorNgCount = (int)m_SeqDataIN.wSectorNgCount ;
	m_nSectorBaseCount = (int)m_SeqDataIN.wSectorBaseCount ;
	// 22.08.10 Ahn Add End

	if (bSigInMode == TRUE)
	{
		// 22.08.11 Ahn Move Start
		FileCtrl_LotInfo(en_mode_LotStart); // 22.06.24 Ahn Add
		// 22.08.11 Ahn Move End

		strMsg.Format(_T("Lot start Process : ID[ %s ], Time[%s]"), m_NowLotData.m_strLotNo, strhhmmss);
		SaveLotLog(strMsg);
	}

	return 0;
}

// 22.06.27 Ahn Add Start
int CGlobalData::AddHistoryLot(CHistoryLotCtrl::_LOT_INFO& lotInfo)
{
	int nRet = 0;
	nRet = m_pHistoryLotCtrl->AddLotFileList(lotInfo);

	return nRet;
}
// 22.06.27 Ahn Add End

int CGlobalData::LotEndProcess()
{
	// 결함수 집계 및 보고.

	//// 22.06.27 Ahn Add Start
	CHistoryLotCtrl::_LOT_INFO LotInfo;
	::_tcsnccpy_s(LotInfo.chLotNo, _countof(LotInfo.chLotNo), AprData.m_NowLotData.m_strLotNo.GetBuffer(0), _TRUNCATE);
	::_tcsnccpy_s(LotInfo.chRecipeName, _countof(LotInfo.chRecipeName), AprData.m_NowLotData.m_strRecipeName.GetBuffer(0), _TRUNCATE);
	::GetLocalTime(&LotInfo.EndTime);
	LotInfo.StartTime = AprData.m_NowLotData.m_LotStartTime ;
	LotInfo.nTabCount = AprData.m_NowLotData.m_nTabCount;
	LotInfo.nTabCountNG = AprData.m_NowLotData.m_nTabCountNG;
	LotInfo.nTabCountOK = AprData.m_NowLotData.m_nTabCount - AprData.m_NowLotData.m_nTabCountNG; //OK 수량 //AprData.m_NowLotData.m_nTabCountOK;
	LotInfo.nTopNgCount = AprData.m_NowLotData.m_nTopNG;
	LotInfo.nBottomNgCount = AprData.m_NowLotData.m_nBottomNG;
	LotInfo.nMarkingCount = AprData.m_NowLotData.m_nMarkingCount;

	AddHistoryLot( LotInfo );
	//// 22.06.27 Ahn Add End

	CString strMsg;
	strMsg.Format(_T("Lot End Process : ID[ %s ]"), m_NowLotData.m_strLotNo );
	SaveLotLog(strMsg);

	return 0;
}


//// Lot 정보를 Lot Change 시점에 저장함.
//int CGlobalData::SaveLotInfo( int nMode, BOOL bSignalMode )
//{
//	CString strSaveFile;
//	CString strFileName;
//	strFileName.Format(_T("LOT_INFO.ini"));
//
//	CString strSection;
//	CString strKey;
//	CString strData;
//
//	strSection = _T("LOT_INFO");
//	switch (nMode) {
//	case	en_Mode_Start :
//		strKey = _T("LOT_NO");
//		strData.Format("%s", AprData.m_NowLotData.m_strLotNo);
//		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
//
//		strKey = _T("RECIPE_NAME");
//		strData.Format("%s", AprData.m_NowLotData.m_strRecipeName);
//		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
//
//		strKey = _T("MEMO_1");
//		strData.Format("%s", AprData.m_NowLotData.m_strMemo1);
//		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
//
//		strKey = _T("MEMO_2");
//		strData.Format("%s", AprData.m_NowLotData.m_strMemo2);
//		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
//		break;
//	case	en_Mode_Run :
//	case	en_Mode_End:
//		strKey = _T("JUDGE_TAB_TOTAL_COUNT");
//		strData.Format("%d", AprData.m_NowLotData.m_nTabCount);
//		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
//
//		strKey = _T("JUDGE_TAB_OK_COUNT");
//		strData.Format("%d", (AprData.m_NowLotData.m_nTabCount - AprData.m_NowLotData.m_nTabCountNG));
//		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
//
//		strKey = _T("JUDGE_TAB_NG_COUNT");
//		strData.Format("%d", AprData.m_NowLotData.m_nTabCountNG);
//		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
//		break;
//	default :
//		break;
//	}
//
//	return 0;
//}

int CGlobalData::FileCtrl_LotInfo(int nMode)
{
	CString strSaveFile;
	CString strFileName;

	strFileName.Format(_T("LastLotInfo"));
	strSaveFile.Format(_T("%s\\%s.ini"), AprData.m_strDataPath, strFileName );

	CString strSection;
	CString strKey;
	CString strData;
	char buff[256];

	switch (nMode) {
	case	en_mode_Initial : // Data 읽어옴( 초기화 )
		strSection = _T("LOT_INFO");
		strKey = _T("LOT_NO");
		::GetPrivateProfileString(strSection, strKey, "LOT_ID_0000", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_strLotNo.Format(_T("%s"), buff);

		strKey = _T("RECIPE_NAME");
		::GetPrivateProfileString(strSection, strKey, "Default", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_strRecipeName.Format(_T("%s"), buff);
		// 22.08.05 Ahn Add Start
		strKey = _T("NEXT_RECIPE_NAME");
		::GetPrivateProfileString(strSection, strKey, "NotSelected", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_strNextRecipeName.Format(_T("%s"), buff);
		// 22.08.05 Ahn Add End
		strKey = _T("TOTAL_COUNT");
		::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_nTabCount = atoi(buff);	

		//Trigger Tab Id Total Count 로그한 TabCount로 세팅
		AprData.m_NowLotData.m_nTabIdTotalCount = AprData.m_NowLotData.m_nTabCount;

		strKey = _T("NG_COUNT");
		::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_nTabCountNG = atoi(buff);

		strKey = _T("OK_COUNT");
		::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_nTabCountOK = atoi(buff);

		// 22.11.30 Ahn Add Start
		strKey = _T("FRAME_COUNT");
		::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_nFrameCount = atoi(buff) ;
		// 22.11.30 Ahn Add End

		// 22.07.27 Ahn Modify Start
		{
			strSection = _T("LOT_START_TIME");
			strKey = _T("LOT_START_YEAR");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
			AprData.m_NowLotData.m_LotStartTime.wYear = (WORD)atoi(buff);

			strKey = _T("LOT_START_MONTH");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
			AprData.m_NowLotData.m_LotStartTime.wMonth = (WORD)atoi(buff);

			strKey = _T("LOT_START_DAY");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
			AprData.m_NowLotData.m_LotStartTime.wDay = (WORD)atoi(buff);

			strKey = _T("LOT_START_HOUR");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
			AprData.m_NowLotData.m_LotStartTime.wHour = (WORD)atoi(buff);

			strKey = _T("LOT_START_MINUTE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
			AprData.m_NowLotData.m_LotStartTime.wMinute = (WORD)atoi(buff);

			strKey = _T("LOT_START_SECOND");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
			AprData.m_NowLotData.m_LotStartTime.wSecond = (WORD)atoi(buff);

			if ((AprData.m_NowLotData.m_LotStartTime.wYear <= 0) || (AprData.m_NowLotData.m_LotStartTime.wMonth <= 0) || (AprData.m_NowLotData.m_LotStartTime.wDay <= 0)) {
				SYSTEMTIME sysTime;
				::GetSystemTime(&sysTime);
				AprData.m_NowLotData.m_LotStartTime = sysTime ;
			}
		}
		// 22.07.27 Ahn Modify End
		// 22.07.27 Ahn Modify Start
		strSection = _T("LAST_DELETE_COMPLET_DATE");
		strKey = _T("LAST_DELETE_COMPLET_YEAR");
		::GetPrivateProfileString(strSection, strKey, "2022", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wYear = (WORD)atoi(buff);

		strKey = _T("LAST_DELETE_COMPLET_MONTH");
		::GetPrivateProfileString(strSection, strKey, "1", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wMonth = (WORD)atoi(buff);

		strKey = _T("LAST_DELETE_COMPLET_DAY");
		::GetPrivateProfileString(strSection, strKey, "1", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wDay = (WORD)atoi(buff);

		strKey = _T("LAST_DELETE_COMPLET_HOUR");
		::GetPrivateProfileString(strSection, strKey, "1", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wHour = (WORD)atoi(buff);

		strKey = _T("LAST_DELETE_COMPLET_MINUTE");
		::GetPrivateProfileString(strSection, strKey, "1", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wMinute = (WORD)atoi(buff);
		// 22.07.27 Ahn Modify End

		break;
	case	en_mode_LotStart : // Lot Change 시점
		strSection = _T("LOT_INFO");
		strKey = _T("LOT_NO");
		strData.Format(_T("%s"), AprData.m_NowLotData.m_strLotNo);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("RECIPE_NAME");
		strData.Format(_T("%s"), AprData.m_NowLotData.m_strRecipeName);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("TOTAL_COUNT");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCount);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("NG_COUNT");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCountNG);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("OK_COUNT");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCountOK);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		// 22.11.30 Ahn Add Start
		strKey = _T("FRAME_COUNT");
		strData.Format( _T("%d"), AprData.m_NowLotData.m_nFrameCount);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);
		// 22.11.30 Ahn Add End

		strSection = _T("LOT_START_TIME");

		strKey = _T("LOT_START_YEAR");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LotStartTime.wYear );
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LOT_START_MONTH");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LotStartTime.wMonth);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LOT_START_DAY");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LotStartTime.wDay);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LOT_START_HOUR");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LotStartTime.wHour);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile); 

		strKey = _T("LOT_START_MINUTE");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LotStartTime.wMinute);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LOT_START_SECOND");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LotStartTime.wSecond);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		break;
	case	en_mode_Countting : //
		strSection = _T("LOT_INFO");
		strKey = _T("TOTAL_COUNT");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCount);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("NG_COUNT");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCountNG);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("OK_COUNT");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_nTabCountOK);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		// 22.11.30 Ahn Add Start
		strKey = _T("FRAME_COUNT");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_nFrameCount);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);
		// 22.11.30 Ahn Add End

		break;
	case en_mode_LotEnd :
		{
		// 보고 및 파일 삭제
		// 22.07.26 Ahn Modify Start
		//	CWin32File file;
		//	file.DeleteFile(strSaveFile);
		strSection = _T("LOT_INFO");
		// 22.07.28 Ahn Add Start
		strKey = _T("LOT_NO");
		strData.Format("");
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);
		// 22.07.28 Ahn Add Start

		strKey = _T("TOTAL_COUNT");
		strData.Format(_T("%d"), 0);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("NG_COUNT");
		strData.Format(_T("%d"), 0);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("OK_COUNT");
		strData.Format(_T("%d"), 0);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		// 22.11.30 Ahn Add Start
		strKey = _T("FRAME_COUNT");
		strData.Format(_T("0"));
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);
		// 22.11.30 Ahn Add End

		break;
		
		// 22.07.26 Ahn Modify End
		}
		break;
	case	en_mode_Write_LastDelTime :
		strSection = _T("LAST_DELETE_COMPLET_DATE");
		// 22.07.26 Ahn Modify Start
		strKey = _T("LAST_DELETE_COMPLET_YEAR");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LastDeleteCompletTime.wYear);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LAST_DELETE_COMPLET_MONTH");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LastDeleteCompletTime.wMonth );
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LAST_DELETE_COMPLET_DAY");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LastDeleteCompletTime.wDay );
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LAST_DELETE_COMPLET_HOUR");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LastDeleteCompletTime.wHour);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("LAST_DELETE_COMPLET_MINUTE");
		strData.Format(_T("%d"), AprData.m_NowLotData.m_LastDeleteCompletTime.wMinute);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);
		// 22.07.26 Ahn Modify End
		break;

	// 22.07.27 Ahn Add Start
	case	en_mode_Read_LastDelTime :
		strSection = _T("LAST_DELETE_COMPLET_DATE");
		// 22.07.26 Ahn Modify Start
		strKey = _T("LAST_DELETE_COMPLET_YEAR");
		::GetPrivateProfileString(strSection, strKey, "2022", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wYear = atoi(buff) ;

		strKey = _T("LAST_DELETE_COMPLET_MONTH");
		::GetPrivateProfileString(strSection, strKey, "1", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wMonth = atoi(buff);

		strKey = _T("LAST_DELETE_COMPLET_DAY");
		::GetPrivateProfileString(strSection, strKey, "1", buff, 256, strSaveFile);
		AprData.m_NowLotData.m_LastDeleteCompletTime.wDay = atoi(buff) ;
		break;
	// 22.07.27 Ahn Add End

	// 22.08.04 Ahn Add Start
	case	en_mode_RecipeSelect :
		strSection = _T("LOT_INFO");
		strKey = _T("RECIPE_NAME");
		strData.Format(_T("%s"), AprData.m_NowLotData.m_strRecipeName);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		strKey = _T("NEXT_RECIPE_NAME");
		strData.Format(_T("%s"), AprData.m_NowLotData.m_strNextRecipeName);
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);
		break;
	// 22.08.04 Ahn Add End
	}
	return 0;

}

// 22.05.25 Son Add Start
CString CGlobalData::GetIndexFileName(CString strLotId)
{
	CString strFullName = _T("");
	CString strBaseDir = _T("");
	CString strHashDir = _T("");

	strBaseDir.Format(_T("%s\\INDEX"), AprData.m_strResultPath);

	CAprHash hash;
	strHashDir = hash.GetHashDir(strLotId);

	strHashDir.MakeUpper();
	strLotId.MakeUpper();

	strFullName.Format(_T("%s%s\\%s.IDX")
		, strBaseDir
		, strHashDir
		, strLotId);

	return(strFullName);
}

// 22.11.25 Ahn Add Start
CString CGlobalData::GetBinaryFileName(CString strLotId)
{
	CString strRet = _T("");
	CString strIndexFileName = GetIndexFileName(strLotId);
	CString strReadFileName;
	CStringList strSearchFileList;
	CWin32File file;
	strSearchFileList.RemoveAll();

	if (CWin32File::ExistFile(strIndexFileName) == TRUE) {

		CStdioFile cf;
		CString strMsg;
		CString strReadFileName ;
		if (!cf.Open(strIndexFileName, (CFile::typeText | CFile::modeRead))) {
			return _T("") ;
		}
		while (1) {
			try {
				if (cf.ReadString(strReadFileName) == NULL) {
					break;
				}
				strSearchFileList.AddTail(strReadFileName);
			}
			catch (CFileException& e) {
				CHAR	szChar[256];
				e.GetErrorMessage(szChar, 255);
				strMsg.Format(_T("에러 - %s"), szChar);
				//MessageBox(strMsg);
				cf.Close();
				return _T("") ;
			}
		}
	}

	int nCount = (int)strSearchFileList.GetCount();
	if (nCount > 0) {
		strRet = strSearchFileList.GetTail();
	}

	return strRet ;
}
// 22.11.25 Ahn Add End


int CGlobalData::CreateIndexFile(CString strBinFileName)
{
	CString strIndexFileName = _T("");

	strIndexFileName = GetIndexFileName(AprData.m_NowLotData.m_strLotNo);

	// 22.07.04 Ahn Add Start
	if (CWin32File::ExistFile(strIndexFileName) == TRUE) {
		::DeleteFile(strIndexFileName);
	}
	// 22.07.04 Ahn Add End

	CStdioFile cf;

	CWin32File::CreateDirectoryA(strIndexFileName, TRUE);

	if (cf.Open(strIndexFileName, CFile::modeWrite | CFile::modeCreate, NULL) == 0) {
		AfxMessageBox(_T("indexFile Error"));
	}

	cf.SeekToEnd();
	cf.WriteString(strBinFileName);
	cf.Close();

	return 0;
}
// 22.05.25 Son Add End

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
//SPC+ STATUS ===============================
//검사기 상태가 변경될 때 호출
//InspStatus = Live Signal : 1, 가동불가 : 0
void CGlobalData::SpcPluusStatus(CString InspStatus)
{
	//SPC+ Status 객체 생성
	CSpcStatusManager status;
	//Status 값 세팅
	status.getSpcInspInData()->setInspStatus(InspStatus);

	//JSON 파일 생성
	status.makeJSONFile();
}
#endif //#ifdef SPCPLUS_CREATE


void CLotInfo::ClearAllCount()
{
	m_nTabCount = 0 ;

	//Trigger Tab Id Total Count
	m_nTabIdTotalCount = 0;

	//Trigger Tab Id 초기화
	m_bInitTabId = FALSE;

	//ConnectZone
	m_bConnectZone = FALSE;

	//Start/Stop 버튼을 클릭했을 경우 설정됨
	m_bInspStartStop = FALSE;

	m_nTabCountNG = 0 ;
	m_nTabCountOK = 0 ;
	m_nLastTotalCnt = 0 ;
	m_nTopNG = 0 ;
	m_nBottomNG = 0 ;
	m_nMarkingCount = 0 ;
	memset(&m_SeqDataOut, 0x0000, sizeof(_SEQ_OUT_DATA));
	memset(&m_SeqDataLotEnd, 0x0000, sizeof(_SEQ_OUT_DATA_LOT_END));

	memset(&m_SeqDataOutSms, 0x0000, sizeof(_SEQ_OUT_DATA_SMS));
	memset(&m_SeqDataLotEndSms, 0x0000, sizeof(_SEQ_OUT_DATA_LOT_END_SMS));

}


int CGlobalData::FileCtrl_DuplicateNG(int nMode, int nID, int nJudge )
{
	CString strSaveFile;
	CString strFileName;

	strFileName.Format(_T("DuplicateNgInfo"));
	strSaveFile.Format(_T("%s\\%s.ini"), AprData.m_strDataPath, strFileName);

	CString strSection;
	CString strKey;
	CString strData;
	char buff[256];

	switch (nMode)
	{
	case	MODE_READ:
		strSection = _T("DUPLICATE_NG_INFO");

		for (int i = 0; i < 64; i++)
		{
			strKey.Format(_T("CELL_ID_%02d"), i);
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strSaveFile);
			//AprData.m_NowLotData.m_SeqDataOutSms.wDuplicateNG_Cell_ID[i] = atoi(buff);
		}

		break;


	case	MODE_WRITE:
		strSection = _T("DUPLICATE_NG_INFO");
		strKey.Format(_T("CELL_ID_%02d"), nID);
		strData.Format(_T("%d"), nJudge );
		::WritePrivateProfileString(strSection, strKey, strData, strSaveFile);

		break;
	}


	return 0;

}
