#pragma once

class CPlcBase;
class CPlcDataCtrl : public CPlcImp
{
	//In Data
	//Bit Data In
	WORD BitIn_Alive;
	WORD BitIn_Ready;
	WORD BitIn_Run;
	WORD BitIn_EncoderReset;
	WORD BitIn_TabZeroReset;
	WORD BitIn_InkMarkingActive;
	WORD BitIn_ConnectZone;
	WORD BitIn_RecipeChange;
	WORD BitIn_LotStartReq;
	WORD BitIn_LotEndReq;
	WORD BitIn_AlarmResetReq;
	WORD BitIn_AlarmNgAck;

	WORD WordIn_RecipeNo;
	WORD WordIn_RecipeName;
	WORD WordIn_CELL_ID;

	WORD WordIn_FoilExpInTopTarget;
	WORD WordIn_FoilExpInBtmTarget;
	WORD WordIn_FoilExpOutTopTarget;
	WORD WordIn_FoilExpOutBtmTarget;
	WORD WordIn_FoilExpBothTopTarget;
	WORD WordIn_FoilExpBothBtmTarget;
	WORD WordIn_SpeterTopTarget;
	WORD WordIn_SpeterBtmTarget;

	WORD WordIn_PrmContinuousCnt;
	WORD WordIn_PrmSectorNgTabCnt;
	WORD WordIn_PrmSectorBaseCnt;



	//Bit Data Out

	WORD BitOut_Alive;
	WORD BitOut_Ready;
	WORD BitOut_EncoderSet;
	WORD BitOut_RecipeChangeAck;
	WORD BitOut_LotStartReqAck;
	WORD BitOut_LotEndReqAck;
	WORD BitOut_TabZeroReset;
	WORD BitOut_AlarmResetAck;
	WORD BitOut_AlarmNgResetAck;

	WORD BitOut_DiskSpaceWarning;
	WORD BitOut_DiskSpaceAlarm;


	WORD WordOut_DataReportV1_Ea;
	WORD WordOut_DataReportV2_OK;
	WORD WordOut_DataReportV3_NG;
	WORD WordOut_DataReportV4_OkRate;
	WORD WordOut_DataReportV5_NgRate;
	WORD WordOut_DataReportV6_RunRate;
	WORD WordOut_Continue_Alarm_Cnt;
	WORD WordOut_Heavy_Alarm_Cnt;


	WORD WordOut_FoilExpInTop_Alarm_Cnt;
	WORD WordOut_FoilExpInBtm_Alarm_Cnt;
	WORD WordOut_FoilExpOutTop_Alarm_Cnt;
	WORD WordOut_FoilExpOutBtm_Alarm_Cnt;
	WORD WordOut_FoilExpBothTop_Alarm_Cnt;
	WORD WordOut_FoilExpBothBtm_Alarm_Cnt;
	WORD WordOut_SpeterTop_Alarm_Cnt;
	WORD WordOut_SpeterBtm_Alarm_Cnt;

	WORD WordOut_Top_Defect_Count_Real;
	WORD WordOut_Btm_Defect_Count_Real;
	WORD WordOut_Top_Defect_Count_LotEnd;
	WORD WordOut_Btm_Defect_Count_LotEnd;



	WORD WordOut_FoilExpInTopTarget;
	WORD WordOut_FoilExpInBtmTarget;
	WORD WordOut_FoilExpOutTopTarget;
	WORD WordOut_FoilExpOutBtmTarget;
	WORD WordOut_FoilExpBothTopTarget;
	WORD WordOut_FoilExpBothBtmTarget;
	WORD WordOut_SpeterTopTarget;
	WORD WordOut_SpeterBtmTarget;

	WORD WordOut_PrmContinuousCnt;
	WORD WordOut_PrmSectorNgTabCnt;
	WORD WordOut_PrmSectorBaseCnt;



	WORD WordOut_AlarmExist;
	WORD WordOut_AlarmCode_Buffer1;
	WORD WordOut_AlarmCode_Buffer2;
	WORD WordOut_AlarmCode_Buffer3;
	WORD WordOut_AlarmCode_Buffer4;
	WORD WordOut_AlarmCode_Buffer5;
	WORD WordOut_AlarmCode_Buffer6;
	WORD WordOut_AlarmCode_Buffer7;
	WORD WordOut_AlarmCode_Buffer8;
	WORD WordOut_AlarmCode_Buffer9;
	WORD WordOut_AlarmCode_Buffer10;
	WORD WordOut_AlarmCode_Buffer11;
	WORD WordOut_AlarmCode_Buffer12;
	WORD WordOut_AlarmCode_Buffer13;
	WORD WordOut_AlarmCode_Buffer14;
	WORD WordOut_AlarmCode_Buffer15;
	WORD WordOut_AlarmCode_Buffer16;
	WORD WordOut_AlarmCode_Buffer17;
	WORD WordOut_AlarmCode_Buffer18;
	WORD WordOut_AlarmCode_Buffer19;
	WORD WordOut_AlarmCode_Buffer20;
	WORD WordOut_AlarmCode_Buffer21;
	WORD WordOut_AlarmCode_Buffer22;
	WORD WordOut_AlarmCode_Buffer23;
	WORD WordOut_AlarmCode_Buffer24;


	WORD WordOut_Cell_Trigger_ID;
	WORD WordOut_Judge;
	WORD WordOut_NG_Code;


	WORD WordOut_DuplicateNG_Cell_ID;

public:
	CPlcDataCtrl();
	virtual ~CPlcDataCtrl();

	//Out
	virtual int SigOutEncoderZeroSet(int nMode) { return 0; }
	virtual int SigOutRecipeChangeAck(int nMode) { return 0; }
	virtual int SigOutLotEndAck(int nMode) { return 0; }
	virtual int SigOutLotStartAck(int nMode) { return 0; }
	virtual int WriteAlarmCode(WORD nAlarmCode) { return 0; }
	virtual int SigOutReady(int nMode) { return 0; }
	virtual int SigOutAlivePulse(int nInMode) { return 0; }
	virtual int SigOutTabZeroReset(int nMode) { return 0; }
	virtual int SigOutAlarmResetAck(int nMode) { return 0; }
	virtual int WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode) { return 0; }
	virtual void SetInkMarkAcktive(BOOL bUse) {}
	virtual void SetConnectZone(BOOL bUse) {}
	virtual int SigOutAlivePulseReady(int nInMode, BOOL bIsReady) { return 0; }
	virtual int SigOutAlarmExist(int nMode) { return 0; }
	virtual void EnableWorkSet(BOOL bMode) {}
	virtual int SigOutDiskCapacityAlarm(int nMode) { return 0; }
	virtual int SigOutDiskCapacityWarning(int nMode) { return 0; }
	virtual int SignalBitOut(int nIntegration, int nMode, BOOL bLocal = FALSE) { return 0; }


	//In
	virtual int SigInReady() { return 0; }
	virtual int SigInRun() { return 0; }
	virtual int SigInLotEnd() { return 0; }
	virtual int SigInTabZeroReset() { return 0; }
	virtual int SigInLotStart() { return 0; }
	virtual int SigInAlarmReset() { return 0; }
	virtual int SigInAlarmNgAck() { return 0; }
	virtual int SigInRecipeChange() { return 0; }
	virtual int SigInInkMarkActive() { return 0; }
	virtual int SigInConnectZone() { return 0; }
	virtual BOOL GetConnectZone() { return TRUE; }
	virtual BOOL GetInkMarkActive() { return TRUE; }

	//임시
	virtual int ReadBlockAllData(CSequenceData* pSeqData) { return 0; }
	virtual int WritePLC_Block_device(int address, short* pData, int nNumOfData) { return 0; }
	virtual int WritePLC_Block_device(int address, int* pData, int nNumOfData) { return 0; }
	virtual int ReadPLC_Block_device(int address, short* pData, int nNumOfData) { return 0; }
	virtual int WriteBlockAllData(int nMode) { return 0; }
	virtual int ReadAllPort_BitIn(BOOL* pSigBitIn) { return 0; }
	virtual int ReadAllPort_BitOut(BOOL* pSigBitOut) { return 0; }
};

