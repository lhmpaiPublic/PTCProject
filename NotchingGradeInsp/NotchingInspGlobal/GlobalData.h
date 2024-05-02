#pragma once

#include "GlobalDef.h"
#include "SystemSetting.h"
#include "CDebugSet.h"
#include <queue>
#include "CHistoryLotCtrl.h" 
#include "NotchingGradeInsp.h"
#include "CSectorNgJudge.h" 

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
		en_BrightError		= 0x000013,
		en_CountBordError	= 0x000014,

		en_DiskCapacityAlarm	= 0x100001,
		en_DiskCapacityWarning  = 0x100002,

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
		case	en_DiskCapacityAlarm:
		//CountBord Error
		case	en_CountBordError:
			m_bInitialReq = TRUE;
			break;


		// Light Error
		case	en_DiskCapacityWarning:
			break;

		}

		theApp.ErrOutput(strErrMsg, nErrorType );
	};
	BOOL IsHeavyError() 
	{
		return m_bInitialReq ;
	};

	void ErrorClear() 
	{
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

typedef struct stSeqDataOutAll {
public:
	_SEQ_OUT_DATA m_SeqOutData;

	DWORD dwTopNgLotEndCount;
	DWORD dwBottomNgLotEndCount;
	DWORD dwAlarm;
	DWORD dwAlarmCode;

} _SEQ_OUT_DATA_ALL ;

typedef struct stCellJudge
{
	DWORD dwCellTriggerID;
	DWORD dwCellJudge;
	DWORD dwCellNgCode;
} _CELL_JUDGE;


//////////////////////////////////////////////////////////////////////////
#ifndef NEW_PLCTYPE

typedef struct stSeqDataOutLotEnd {
public:
	DWORD dwTopNgLotEndCount;			// Lot End시 Top NG Count 
	DWORD dwBottomNgLotEndCount;		// Lot End시 Bottom NG Count  18 개
} _SEQ_OUT_DATA_LOT_END;

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

} _SEQ_OUT_DATA_SMS;


#define SEQDATASMS_DUMMY 210
#define SEQDATASMS_KEYID 64

typedef struct stSeqDataOutAllSms {
public:
	_SEQ_OUT_DATA_SMS m_SeqOutData;

	WORD dwTopNgLotEndCount;
	WORD dwBottomNgLotEndCount;
	WORD dwAlarm;
	WORD dwAlarmCode;

} _SEQ_OUT_DATA_ALL_SMS;

typedef struct stSeqDataInSms {
public:
	WORD wCell_KeyID_Dummy[SEQDATASMS_DUMMY]; //Cell Key까지 더미 데이터
	WORD wCell_KeyID[SEQDATASMS_KEYID]; //통합비전의 Cell(한번 검사에 필요한 영역)에 대한 부여된 ID
} _SEQ_In_DATA_SMS;

#endif //NEW_PLCTYPE


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


typedef struct stDimensionResult
{
	double dOverlayWidth_Top;
	double dCuttingWidth_Top;
	double dInsulationWidth_Top;
	double dTabWidth_Top;
	double dOverlayWidth_Btm;
	double dCuttingWidth_Btm;

	int nJudge_OverlayWidth_Top;
	int nJudge_CuttingWidth_Top;
	int nJudge_InsulationWidth_Top;
	int nJudge_TabWidth_Top;
	int nJudge_OverlayWidth_Btm;
	int nJudge_CuttingWidth_Btm;


} _DIMENSION_RESULT;


class CLotInfo 
{
public:
	CLotInfo();
	~CLotInfo();

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

	//Trigger Tab Id Total Count
	int		m_nTabIdTotalCount;

	//Encoder Count
	UINT m_uEncoderCount;

	//BCD ID input time
	UINT64 m_nBCDIDInputTime;

	//Image Total Count
	UINT m_uImageTotalCount;

	//Trigger Tab Id 초기화
	BOOL m_bInitTabId;

	//다음에 사용될 BCD ID
	int m_nUseBCDID;

	//제일 마지막 받은 BCD ID 
	int m_nLastBCDId;

	//ConnectZone
	BOOL m_bConnectZone;

	//Start/Stop 버튼을 클릭했을 경우 설정됨
	BOOL m_bInspStartStop;

	int		m_nTopNG;
	int		m_nBottomNG;
	int		m_nMarkingCount;

	int		m_nFrameCount;

	SYSTEMTIME m_LotStartTime;
	SYSTEMTIME m_LotEndTime;
	CTime	m_ctLastAcqTime;
	BOOL	m_bProcError;

	_SEQ_OUT_DATA			m_SeqDataOut;
	_CELL_JUDGE				m_stCellJudge;

#ifndef NEW_PLCTYPE
	_SEQ_OUT_DATA_LOT_END	m_SeqDataLotEnd;

	_SEQ_OUT_DATA_SMS			m_SeqDataOutSms;

	//지멘스 Read 블럭으로
	_SEQ_In_DATA_SMS m_ReadDataSms;
#endif //NEW_PLCTYPE

	_ALARM_CODE_CELL_JUDGE_SMS	m_stAlarmCodeAndCellJudgeSms;

	SYSTEMTIME m_LastDeleteCompletTime;

	CSectorNgJudge	m_secNgJudge;
	int				m_nContinueCount;
	// 22.08.09 Ahn Add End

	_DIMENSION_RESULT m_stDimResult;


	void ClearAllCount() ;
};

class CRecipeInfo;
class CSystemSetting;
class CLogDisplayDlg;
class CHistoryLotCtrl;

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
	void SaveErrorLog_Format(const char* format, ...);

	CString GetIndexFileName(CString strLotId);
	int CreateIndexFile(CString strBinFileName);
	CString GetBinaryFileName(CString strLotId);

	int LoadRecipeInfo();
	int LotStartProcess( BOOL bSigInMode = FALSE, int nDebugMode = 0 );
	int LotInfoSave();
	int LotEndProcess();

	enum {
		en_Mode_Start = 0,
		en_Mode_Run,
		en_Mode_End,
		en_Mode_Max,
	};

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
	CString m_strRecipeLogPath; 
	CString m_strResultPath;

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

	CHistoryLotCtrl *m_pHistoryLotCtrl; 
	int AddHistoryLot( CHistoryLotCtrl::_LOT_INFO &lotInfo ) ;

	UserModeDefine::eMode UserMode;
	CFont	m_font;
	int		m_nNowLightLevel[MAX_LIGHT_UNIT][MAX_CAMERA_NO];

	double m_dDiskTotal;
	double m_dDiskFree;
	double m_dDiskPercent;

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


	int FileCtrl_DuplicateNG(int nMode, int nID=0, int nJudge=0);


protected :
	void InitRecipe();


private :
	long nFrameCounter[GRABBER_COUNT];

	//전역 스트링 세팅이 필요한 값을 저장한다.
	_GLOBAL_SETTING_STRING m_globalStr;
};

extern CGlobalData AprData ;

