#pragma once

#include "GlobalDef.h"
#include "SystemSetting.h"
#include "CDebugSet.h"
#include <queue>
#include "CHistoryLotCtrl.h" // 22.06.27 Ahn Add 
#include "NotchingGradeInsp.h"	// 22.06.29 Son Add
//#define SAVE_DEBUG_LOG
//#define SAVE_DEBUG_LOG_PRJDATA 
//#define SAVE_MEMORY_LOG
#include "CSectorNgJudge.h" // 22.08.09 Ahn Add

class CErrorStatus {
public :
	BOOL m_bErrorFlag;
	int  m_nErrorType ;
	CString m_strErrorMsg;
	BOOL m_bInitialReq;

	enum {
		en_LampError		= 0x000001,
		en_CameraError		= 0x000002,		
		en_GrabberError		= 0x000003,
		en_InspectionError	= 0x000004,
		en_RecipeError		= 0x000005,
		en_ResultSaveError	= 0x000006,
		en_ContinuousNg		= 0x000007,
		en_SectorNg			= 0x000008,
		en_MelsecError		= 0x000009,
		en_DioError			= 0x000010,
		en_ProcessError		= 0x000011,
		en_MissTabID		= 0x000012,


		en_DiskCapacityAlarm	= 0x100001,
		en_DiskCapacityWarning  = 0x100002,

	//	en_MaxErrorType,
	};
	void SetError(int nErrorType, CString strErrMsg) {
		m_nErrorType = nErrorType;
		m_strErrorMsg = strErrMsg;
		switch (nErrorType) {
		// Heavy Error
		case	en_CameraError:
		case	en_GrabberError:
//		case	en_ProcessError:
		case	en_LampError:			//조명 에러 발생 시 READY OFF
			m_bInitialReq = TRUE;
			break;
		case	 en_DiskCapacityAlarm:
			break;	

		// Light Error
		}

		// 22.06.29 Son Add Start
		theApp.ErrOutput(strErrMsg, nErrorType );
		// 22.06.29 Son Add End
	};
	BOOL IsHeavyError() {
		return m_bInitialReq ;
	};

	void ErrorClear() {
		m_bErrorFlag = FALSE ;
		m_nErrorType = 0 ;
		m_strErrorMsg = _T("") ;
		m_bInitialReq = FALSE ;
	};
};

class CSequenceData {
public :
	DWORD wRecipeNo;
	CString strRecipeName;
	CString strCell_ID;

	DWORD wFoilExpInTopTarget;
	DWORD wFoilExpInBottomTarget;
	DWORD wFoilExpOutTopTarget;
	DWORD wFoilExpOutBottomTarget;
	DWORD wFoilExpBothTopTarget;
	DWORD wFoilExpBothBottomTarget;
	DWORD wSpeterTopTarget;
	DWORD wSpeterBottomTarget;

	DWORD wAlarmExistAck;

	DWORD wContinousCount;
	DWORD wSectorNgCount;
	DWORD wSectorBaseCount;

};
// 22.07.06 Ahn Add Start
typedef struct stSeqDataOut {
public:
	DWORD dwDataReportV1;				// 검사 수량
	DWORD dwDataReportV2;				// OK 수량
	DWORD dwDataReportV3;				// NG 수량
	DWORD dwDataReportV4;				// 양품률
	DWORD dwDataReportV5;				// 불량률
	DWORD dwDataReportV6;				// 가동률

	DWORD dwContinueAlarmCount;			// 연속 알람 발생 수량
	DWORD dwSectorAlarmCount;			// 구간 알람 발생 수량
	DWORD dwFoilExpInTopCount;
	DWORD dwFoilExpInBottomCount;
	DWORD dwFoilExpOutTopCount;
	DWORD dwFoilExpOutBottomCount;
	DWORD dwFoilExpBothTopCount;
	DWORD dwFoilExpBothBottomCount;
	DWORD dwSpeterTopCount;
	DWORD dwSpeterBottomCount;

	DWORD dwTopNgRealTimeCount;			// 실시간 Tob NG Count
	DWORD dwBottomNgRealTimeCount;		// 실시간 Bottom NG Count
	DWORD dwTopNgLotEndCount;			// Lot End 시
	DWORD dwBottomNgLotEndCount;		// Lot End 시

	DWORD dwDummy1[3];

	DWORD dwFoilExpInTopTarget;
	DWORD dwFoilExpInBottomTarget;
	DWORD dwFoilExpOutTopTarget;
	DWORD dwFoilExpOutBottomTarget;
	DWORD dwFoilExpBothTopTarget;
	DWORD dwFoilExpBothBottomTarget;
	DWORD dwSpeterTopTarget;
	DWORD dwSpeterBottomTarget;

	DWORD dwDummy2;

	DWORD dwPrmContinuousCnt;
	DWORD dwPrmSectorNgTabCnt;
	DWORD dwPrmSectorBaseCnt;

	DWORD dwDummy3[5];

	DWORD dwAlarmExist;
	DWORD dwAlarmCode[5];

	DWORD dwDummy4[10];


} _SEQ_OUT_DATA;
// 22.08.19 Ahn Add Start
typedef struct stSeqDataOutAll {
public:
	_SEQ_OUT_DATA m_SeqOutData;

	DWORD dwTopNgLotEndCount;
	DWORD dwBottomNgLotEndCount;
	DWORD dwAlarm;
	DWORD dwAlarmCode;

} _SEQ_OUT_DATA_ALL ;
// 22.08.19 Ahn Add Start
typedef struct stSeqDataOutLotEnd {
public:
	DWORD dwTopNgLotEndCount;			// Lot End시 Top NG Count 
	DWORD dwBottomNgLotEndCount;		// Lot End시 Bottom NG Count  18 개
} _SEQ_OUT_DATA_LOT_END;

typedef struct stCellJudge
{
	DWORD dwCellTriggerID;
	DWORD dwCellJudge;
	DWORD dwCellNgCode;
} _CELL_JUDGE;


//////////////////////////////////////////////////////////////////////////
// Siemens
typedef struct stSeqDataOutSms {
public:
	WORD wDataReportV1;				// 검사 수량
	WORD wDataReportV2;				// OK 수량
	WORD wDataReportV3;				// NG 수량
	WORD wDataReportV4;				// 양품률
	WORD wDataReportV5;				// 불량률
	WORD wDataReportV6;				// 가동률

	WORD wContinueAlarmCount;			// 연속 알람 발생 수량
	WORD wSectorAlarmCount;			// 구간 알람 발생 수량


	WORD wFoilExpInTopCount;
	WORD wFoilExpInBottomCount;
	WORD wFoilExpOutTopCount;
	WORD wFoilExpOutBottomCount;
	WORD wFoilExpBothTopCount;
	WORD wFoilExpBothBottomCount;
	WORD wSpeterTopCount;
	WORD wSpeterBottomCount;

	WORD wTopNgRealTimeCount;			// 실시간 Tob NG Count
	WORD wBottomNgRealTimeCount;		// 실시간 Bottom NG Count
	WORD wTopNgLotEndCount;			// Lot End 시
	WORD wBottomNgLotEndCount;		// Lot End 시

	WORD wFoilExpInTopTarget;
	WORD wFoilExpInBottomTarget;
	WORD wFoilExpOutTopTarget;
	WORD wFoilExpOutBottomTarget;
	WORD wFoilExpBothTopTarget;
	WORD wFoilExpBothBottomTarget;
	WORD wSpeterTopTarget;
	WORD wSpeterBottomTarget;

	WORD wPrmContinuousCnt;
	WORD wPrmSectorNgTabCnt;
	WORD wPrmSectorBaseCnt;

	WORD wDummy1[9];

	WORD wAlarmExist;
	WORD wAlarmCode[24];

	WORD wDummy4[15];

} _SEQ_OUT_DATA_SMS;

typedef struct stSeqDataOutAllSms {
public:
	_SEQ_OUT_DATA_SMS m_SeqOutData;

	WORD dwTopNgLotEndCount;
	WORD dwBottomNgLotEndCount;
	WORD dwAlarm;
	WORD dwAlarmCode;

} _SEQ_OUT_DATA_ALL_SMS;


typedef struct stSeqDataOutLotEndSms {
public:
	WORD dwTopNgLotEndCount;			// Lot End시 Top NG Count 
	WORD dwBottomNgLotEndCount;			// Lot End시 Bottom NG Count  18 개
} _SEQ_OUT_DATA_LOT_END_SMS;


typedef struct stCellJudgeSms
{
	WORD wCellTriggerID;
	WORD wCellJudge;
	WORD wCellNgCode;
} _CELL_JUDGE_SMS;

typedef struct stAlarmCodeAndCellJudgeSms
{
	WORD wAlarmExist;

	WORD wAlarmCode[24];

	WORD wDummy[15];

	WORD wCellTriggerID;
	WORD wCellJudge;
	WORD wCellNgCode;

} _ALARM_CODE_CELL_JUDGE_SMS;


typedef struct GlobalSetingString
{
private:
	CString outImageFormat;			//이미지 출력 포맷 글로벌 세팅 값
public:
	void SetOutImageFormat(CString str) { outImageFormat = str; }
	CString GetOutImageFormat() { return outImageFormat; }
} _GLOBAL_SETTING_STRING;
//////////////////////////////////////////////////////////////////////////



// 22.07.06 Ahn Add Start

class CLotInfo {
public:
	CString m_strLotNo;
	CString m_strDate;
	CString m_strRecipeName;
	CString m_strNextRecipeName;
	int		m_nRecipeNo;
	CString m_strMemo1;
	CString m_strMemo2;

	int		m_nTabCount;
	int		m_nTabCountNG;
	int		m_nTabCountOK;
	int		m_nLastTotalCnt;

	// 22.06.29 Ahn Add Start
	int		m_nTopNG;
	int		m_nBottomNG;
	int		m_nMarkingCount;
	// 22.06.29 Ahn Add End
	// 22.11.30 Ahn Add Start
	int		m_nFrameCount;
	// 22.11.30 Ahn Add End

	SYSTEMTIME m_LotStartTime;
	SYSTEMTIME m_LotEndTime;
	CTime	m_ctLastAcqTime;
	BOOL	m_bProcError;

	_SEQ_OUT_DATA			m_SeqDataOut;
	_SEQ_OUT_DATA_LOT_END	m_SeqDataLotEnd;
	_CELL_JUDGE				m_stCellJudge;

	_SEQ_OUT_DATA_SMS			m_SeqDataOutSms;
	_SEQ_OUT_DATA_LOT_END_SMS	m_SeqDataLotEndSms;
	_CELL_JUDGE_SMS				m_stCellJudgeSms;
	_ALARM_CODE_CELL_JUDGE_SMS	m_stAlarmCodeAndCellJudgeSms;


	// 22.07.11 Ahn Add Start
	SYSTEMTIME m_LastDeleteCompletTime;
	// 22.07.11 Ahn Add End

	// 22.08.09 Ahn Add 
	CSectorNgJudge	m_secNgJudge;
	int				m_nContinueCount;
	// 22.08.09 Ahn Add End

	void ClearAllCount() ;
};

class CRecipeInfo;
class CSystemSetting;
class CLogDisplayDlg;

// 22.06.27 Ahn Add Start
class CHistoryLotCtrl;
// 22.06.27 Ahn Add End

class CGlobalData
{
public:
	CGlobalData(void);
	~CGlobalData(void);

public :
	void ResetAllCounter() ;
	void ResetFrameCounter() ;

	BOOL GetDebug(CString strDebug );

	void SetFrameCounter(int nHeadNo, int nNo) ;
	long GetFrameCounter(int nHeadNo ){ return nFrameCounter[ nHeadNo ] ; } ;

	int SaveDebugLog( CString strMsg ) ;
	void SaveDebugLog_Format(const char* format, ...);
	
	int SaveErrorLog(CString strMsg);
	int SaveTactLog(CString strMsg);
	int SaveMemoryLog( CString strMsg ) ;
	int SaveLotLog(CString strMsg);
	// 22.02.16 Ahn Modify Start
	//int SaveFrameLog(CString strMsg);
	int SaveFrameLog(CString strMsg, int nNo);
	// 22.02.16 Ahn Modify End

	// 22.05.25 Son Add Start
	CString GetIndexFileName(CString strLotId);
	int CreateIndexFile(CString strBinFileName);
	// 22.05.25 Son Add End
	// 22.11.25 Ahn Add Start
	CString GetBinaryFileName(CString strLotId);
	// 22.11.25 Ahn Add End

	int LoadRecipeInfo();
	int LotStartProcess( BOOL bSigInMode = FALSE, int nDebugMode = 0 );
	// 22.06.24 Ahn Add Start
	int LotInfoSave();
	// 22.06.24 Ahn Add End
	int LotEndProcess();

	enum {
		en_Mode_Start = 0,
		en_Mode_Run,
		en_Mode_End,
		en_Mode_Max,
	};

//	int SaveLotInfo( int nMode, BOOL bSignalFlag );

	enum {
		en_mode_Initial = 0,
		en_mode_LotStart = 1,
		en_mode_Countting = 2,
		en_mode_LotEnd = 3,
		en_mode_Write_LastDelTime = 4,
		en_mode_Read_LastDelTime = 5,
		en_mode_RecipeSelect = 6,
	};
	int FileCtrl_LotInfo(int nMode);

	// 
	CSequenceData	m_SeqDataIN;

	CLotInfo	m_NowLotData;
	BOOL		m_bItPassMode;

	CDebugSet m_DebugSet;

	CErrorStatus m_ErrStatus; // 22.05.19 Ahn Add 



	int m_nSleep ;

	BOOL m_bProcMrFlag;

	double m_dTact_1 ;
	double m_dTact_2 ;
	double m_dTact_3 ;
	double m_dTact_4 ;
	// 22.03.24 Ahn Delete Start
	//BOOL	m_bLotStartReq;
	// 22.03.24 Ahn Delete End

	CRITICAL_SECTION m_csSaveLog ;
	CRITICAL_SECTION m_csSaveMemLog ;

	CRITICAL_SECTION m_csTt1 ;
	CRITICAL_SECTION m_csTt2 ;
	CRITICAL_SECTION m_csTt3 ;
	CRITICAL_SECTION m_csTt4 ;

	void SetTactTime_1( double dTime ) ;
	void SetTactTime_2( double dTime ) ;
	void SetTactTime_3( double dTime ) ;
	void SetTactTime_4( double dTime ) ;

	CString m_strLogPath;
	CString m_strImagePath;
	CString m_strDataPath;
	CString m_strFeederPath;
	// 22.06.08 Ahn Add Start
	CString m_strRecipeLogPath; 
	// 22.06.08 Ahn Add End
	// 22.06.30 Ahn Add Start
	CString m_strResultPath;
	// 22.06.30 Ahn Add End

	// 22.05.17 Ahn Modify Start
	//CString m_strNowResultPath;
	//CString m_strNowDatePath ;
	CString m_strNowNgPath;
	CString m_strNowOkPath;
	CString m_strNowCsvPath;
	CString m_strNowBinPath;
	// 22.05.17 Ahn Modify End
	CString m_strNowDatePath; // 22.05.26 Ahn Add 

	int m_nImgSaveInterval;

	CRecipeInfo* m_pRecipeInfo;
	CSystemSetting m_System ;

	CLotInfo m_beforeLotInfo;
	CLotInfo m_currentLotInfo;

	// 22.06.27 Ahn Add Start
	CHistoryLotCtrl *m_pHistoryLotCtrl; 
	int AddHistoryLot( CHistoryLotCtrl::_LOT_INFO &lotInfo ) ;
	//AddLotFileList
	// 22.06.27 Ahn Add End

// 22.05.25 Son Add Start
	UserModeDefine::eMode UserMode;
	CFont	m_font;
// 22.05.25 Son Add End
	int		m_nNowLightLevel[MAX_LIGHT_UNIT][MAX_CAMERA_NO];

	// 22.07.04 Ahn Add Start
	double m_dDiskTotal;
	double m_dDiskFree;
	double m_dDiskPercent;
	// 22.07.04 Ahn Add End

	// 22.08.10 Ahn Add Start
	int		m_nCoutinuouCount;
	int		m_nSectorNgCount;
	int		m_nSectorBaseCount;
	
	int		m_nMissTabIdNow;

	//글로벌 스트링 세팅 객체를 넘긴다.
	_GLOBAL_SETTING_STRING* getGSt() { return &m_globalStr; }

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	//SPC+ STATUS ===============================
	//검사기 상태가 변경될 때 호출
	//InspStatus = Live Signal : 1, 가동불가 : 0
	void SpcPluusStatus(CString InspStatus);
#endif //SPCPLUS_CREATE

protected :
	void InitRecipe();


private :
	long nFrameCounter[GRABBER_COUNT];

	//전역 스트링 세팅이 필요한 값을 저장한다.
	_GLOBAL_SETTING_STRING m_globalStr;
};

extern CGlobalData AprData ;

