#pragma once

class CSequenceData;

class CPlcImp
{
#ifdef NEW_PLCTYPE
	//In Data
	//Bit Data In
	BOOL m_BitIn_Alive;
	BOOL m_BitIn_Ready;
	BOOL m_BitIn_Run;
	BOOL m_BitIn_EncoderReset;
	BOOL m_BitIn_TabZeroReset;
	BOOL m_BitIn_InkMarkingActive;
	BOOL m_BitIn_ConnectZone;
	BOOL m_BitIn_RecipeChange;
	BOOL m_BitIn_LotStartReq;
	BOOL m_BitIn_LotEndReq;
	BOOL m_BitIn_AlarmResetReq;
	BOOL m_BitIn_AlarmNgAck;

	WORD m_WordIn_RecipeNo;
	WORD m_WordIn_RecipeName;
	WORD m_WordIn_CELL_ID;

	WORD m_WordIn_FoilExpInTopTarget;
	WORD m_WordIn_FoilExpInBtmTarget;
	WORD m_WordIn_FoilExpOutTopTarget;
	WORD m_WordIn_FoilExpOutBtmTarget;
	WORD m_WordIn_FoilExpBothTopTarget;
	WORD m_WordIn_FoilExpBothBtmTarget;
	WORD m_WordIn_SpeterTopTarget;
	WORD m_WordIn_SpeterBtmTarget;

	WORD m_WordIn_PrmContinuousCnt;
	WORD m_WordIn_PrmSectorNgTabCnt;
	WORD m_WordIn_PrmSectorBaseCnt;



	//Bit Data Out

	WORD m_BitOut_Alive;
	WORD m_BitOut_Ready;
	WORD m_BitOut_EncoderSet;
	WORD m_BitOut_RecipeChangeAck;
	WORD m_BitOut_LotStartReqAck;
	WORD m_BitOut_LotEndReqAck;
	WORD m_BitOut_TabZeroReset;
	WORD m_BitOut_AlarmResetAck;
	WORD m_BitOut_AlarmNgResetAck;

	WORD m_BitOut_DiskSpaceWarning;
	WORD m_BitOut_DiskSpaceAlarm;


	WORD m_WordOut_DataReportV1_Ea;
	WORD m_WordOut_DataReportV2_OK;
	WORD m_WordOut_DataReportV3_NG;
	WORD m_WordOut_DataReportV4_OkRate;
	WORD m_WordOut_DataReportV5_NgRate;
	WORD m_WordOut_DataReportV6_RunRate;
	WORD m_WordOut_Continue_Alarm_Cnt;
	WORD m_WordOut_Heavy_Alarm_Cnt;


	WORD m_WordOut_FoilExpInTop_Alarm_Cnt;
	WORD m_WordOut_FoilExpInBtm_Alarm_Cnt;
	WORD m_WordOut_FoilExpOutTop_Alarm_Cnt;
	WORD m_WordOut_FoilExpOutBtm_Alarm_Cnt;
	WORD m_WordOut_FoilExpBothTop_Alarm_Cnt;
	WORD m_WordOut_FoilExpBothBtm_Alarm_Cnt;
	WORD m_WordOut_SpeterTop_Alarm_Cnt;
	WORD m_WordOut_SpeterBtm_Alarm_Cnt;

	WORD m_WordOut_Top_Defect_Count_Real;
	WORD m_WordOut_Btm_Defect_Count_Real;
	WORD m_WordOut_Top_Defect_Count_LotEnd;
	WORD m_WordOut_Btm_Defect_Count_LotEnd;



	WORD m_WordOut_FoilExpInTopTarget;
	WORD m_WordOut_FoilExpInBtmTarget;
	WORD m_WordOut_FoilExpOutTopTarget;
	WORD m_WordOut_FoilExpOutBtmTarget;
	WORD m_WordOut_FoilExpBothTopTarget;
	WORD m_WordOut_FoilExpBothBtmTarget;
	WORD m_WordOut_SpeterTopTarget;
	WORD m_WordOut_SpeterBtmTarget;

	WORD m_WordOut_PrmContinuousCnt;
	WORD m_WordOut_PrmSectorNgTabCnt;
	WORD m_WordOut_PrmSectorBaseCnt;



	WORD m_WordOut_AlarmExist;
	WORD m_WordOut_AlarmCode_Buffer1;
	WORD m_WordOut_AlarmCode_Buffer2;
	WORD m_WordOut_AlarmCode_Buffer3;
	WORD m_WordOut_AlarmCode_Buffer4;
	WORD m_WordOut_AlarmCode_Buffer5;
	WORD m_WordOut_AlarmCode_Buffer6;
	WORD m_WordOut_AlarmCode_Buffer7;
	WORD m_WordOut_AlarmCode_Buffer8;
	WORD m_WordOut_AlarmCode_Buffer9;
	WORD m_WordOut_AlarmCode_Buffer10;
	WORD m_WordOut_AlarmCode_Buffer11;
	WORD m_WordOut_AlarmCode_Buffer12;
	WORD m_WordOut_AlarmCode_Buffer13;
	WORD m_WordOut_AlarmCode_Buffer14;
	WORD m_WordOut_AlarmCode_Buffer15;
	WORD m_WordOut_AlarmCode_Buffer16;
	WORD m_WordOut_AlarmCode_Buffer17;
	WORD m_WordOut_AlarmCode_Buffer18;
	WORD m_WordOut_AlarmCode_Buffer19;
	WORD m_WordOut_AlarmCode_Buffer20;
	WORD m_WordOut_AlarmCode_Buffer21;
	WORD m_WordOut_AlarmCode_Buffer22;
	WORD m_WordOut_AlarmCode_Buffer23;
	WORD m_WordOut_AlarmCode_Buffer24;


	WORD m_WordOut_Cell_Trigger_ID;
	WORD m_WordOut_Judge;
	WORD m_WordOut_NG_Code;


	WORD m_WordOut_DuplicateNG_Cell_ID;
#endif //NEW_PLCTYPE


public:
	CPlcImp();
	virtual ~CPlcImp();

#ifdef NEW_PLCTYPE
	//In Data
	//Bit Data In
	BOOL getBitIn_Alive() { return m_BitIn_Alive; }
	void setBitIn_Alive(BOOL BitIn_Alive) { m_BitIn_Alive = BitIn_Alive; }

	BOOL getBitIn_Ready() { return m_BitIn_Ready; }
	void setBitIn_Ready(BOOL BitIn_Ready) { m_BitIn_Ready = BitIn_Ready; }

	BOOL getBitIn_Run() { return m_BitIn_Run; }
	void setBitIn_Run(BOOL BitIn_Run) { m_BitIn_Run = BitIn_Run; }

	BOOL getBitIn_EncoderReset() { return m_BitIn_EncoderReset; }
	void setBitIn_EncoderReset(BOOL BitIn_EncoderReset) { m_BitIn_EncoderReset = BitIn_EncoderReset; }

	BOOL getBitIn_TabZeroReset() { return m_BitIn_TabZeroReset; }
	void setBitIn_TabZeroReset(BOOL BitIn_TabZeroReset) { m_BitIn_TabZeroReset = BitIn_TabZeroReset; }

	BOOL getBitIn_InkMarkingActive() { return m_BitIn_InkMarkingActive; }
	void setBitIn_InkMarkingActive(BOOL BitIn_InkMarkingActive) { m_BitIn_InkMarkingActive = BitIn_InkMarkingActive; }

	BOOL getBitIn_ConnectZone() { return m_BitIn_ConnectZone; }
	void setBitIn_ConnectZone(BOOL BitIn_ConnectZone) { m_BitIn_ConnectZone = BitIn_ConnectZone; }

	BOOL getBitIn_RecipeChange() { return m_BitIn_RecipeChange; }
	void setBitIn_RecipeChange(BOOL BitIn_RecipeChange) { m_BitIn_RecipeChange = BitIn_RecipeChange; }

	BOOL getBitIn_LotStartReq() { return m_BitIn_LotStartReq; }
	void setBitIn_LotStartReq(BOOL BitIn_LotStartReq) { m_BitIn_LotStartReq = BitIn_LotStartReq; }

	BOOL getBitIn_LotEndReq() { return m_BitIn_LotEndReq; }
	void setBitIn_LotEndReq(BOOL BitIn_LotEndReq) { m_BitIn_LotEndReq = BitIn_LotEndReq; }

	BOOL getBitIn_AlarmResetReq() { return m_BitIn_AlarmResetReq; }
	void setBitIn_AlarmResetReq(BOOL BitIn_AlarmResetReq) { m_BitIn_AlarmResetReq = BitIn_AlarmResetReq; }

	BOOL getBitIn_AlarmNgAck() { return m_BitIn_AlarmNgAck; }
	void setBitIn_AlarmNgAck(BOOL BitIn_AlarmNgAck) { m_BitIn_AlarmNgAck = BitIn_AlarmNgAck; }

	WORD getWordIn_RecipeNo() { return m_WordIn_RecipeNo; }
	void setWordIn_RecipeNo(WORD WordIn_RecipeNo) { m_WordIn_RecipeNo = WordIn_RecipeNo; }

	WORD getWordIn_RecipeName() { return m_WordIn_RecipeName; }
	void setWordIn_RecipeName(WORD WordIn_RecipeName) { m_WordIn_RecipeName = WordIn_RecipeName; }

	WORD getWordIn_CELL_ID() { return m_WordIn_CELL_ID; }
	void setWordIn_CELL_ID(WORD WordIn_CELL_ID) { m_WordIn_CELL_ID = WordIn_CELL_ID; }

	WORD getWordIn_FoilExpInTopTarget() { return m_WordIn_FoilExpInTopTarget; }
	void setWordIn_FoilExpInTopTarget(WORD WordIn_FoilExpInTopTarget) { m_WordIn_FoilExpInTopTarget = WordIn_FoilExpInTopTarget; }

	WORD getWordIn_FoilExpInBtmTarget() { return m_WordIn_FoilExpInBtmTarget; }
	void setWordIn_FoilExpInBtmTarget(WORD WordIn_FoilExpInBtmTarget) { m_WordIn_FoilExpInBtmTarget = WordIn_FoilExpInBtmTarget; }

	WORD getWordIn_FoilExpOutTopTarget() { return m_WordIn_FoilExpOutTopTarget; }
	void setWordIn_FoilExpOutTopTarget(WORD WordIn_FoilExpOutTopTarget) { m_WordIn_FoilExpOutTopTarget = WordIn_FoilExpOutTopTarget; }

	WORD getWordIn_FoilExpOutBtmTarget() { return m_WordIn_FoilExpOutBtmTarget; }
	void setWordIn_FoilExpOutBtmTarget(WORD WordIn_FoilExpOutBtmTarget) { m_WordIn_FoilExpOutBtmTarget = WordIn_FoilExpOutBtmTarget; }

	WORD getWordIn_FoilExpBothTopTarget() { return m_WordIn_FoilExpBothTopTarget; }
	void setWordIn_FoilExpBothTopTarget(WORD WordIn_FoilExpBothTopTarget) { m_WordIn_FoilExpBothTopTarget = WordIn_FoilExpBothTopTarget; }

	WORD getWordIn_FoilExpBothBtmTarget() { return m_WordIn_FoilExpBothBtmTarget; }
	void setWordIn_FoilExpBothBtmTarget(WORD WordIn_FoilExpBothBtmTarget) { m_WordIn_FoilExpBothBtmTarget = WordIn_FoilExpBothBtmTarget; }

	WORD getWordIn_SpeterTopTarget() { return m_WordIn_SpeterTopTarget; }
	void setWordIn_SpeterTopTarget(WORD WordIn_SpeterTopTarget) { m_WordIn_SpeterTopTarget = WordIn_SpeterTopTarget; }

	WORD getWordIn_SpeterBtmTarget() { return m_WordIn_SpeterBtmTarget; }
	void setWordIn_SpeterBtmTarget(WORD WordIn_SpeterBtmTarget) { m_WordIn_SpeterBtmTarget = WordIn_SpeterBtmTarget; }

	WORD getWordIn_PrmContinuousCnt() { return m_WordIn_PrmContinuousCnt; }
	void setWordIn_PrmContinuousCnt(WORD WordIn_PrmContinuousCnt) { m_WordIn_PrmContinuousCnt = WordIn_PrmContinuousCnt; }

	WORD getWordIn_PrmSectorNgTabCnt() { return m_WordIn_PrmSectorNgTabCnt; }
	void setWordIn_PrmSectorNgTabCnt(WORD WordIn_PrmSectorNgTabCnt) { m_WordIn_PrmSectorNgTabCnt = WordIn_PrmSectorNgTabCnt; }

	WORD getWordIn_PrmSectorBaseCnt() { return m_WordIn_PrmSectorBaseCnt; }
	void setWordIn_PrmSectorBaseCnt(WORD WordIn_PrmSectorBaseCnt) { m_WordIn_PrmSectorBaseCnt = WordIn_PrmSectorBaseCnt; }


	//Bit Data Out
	WORD getBitOut_Alive() { return m_BitOut_Alive; }
	void setBitOut_Alive(WORD BitOut_Alive) { m_BitOut_Alive = BitOut_Alive; }

	WORD getBitOut_Ready() { return m_BitOut_Ready; }
	void setBitOut_Ready(WORD BitOut_Ready) { m_BitOut_Ready = BitOut_Ready; }

	WORD getBitOut_EncoderSet() { return m_BitOut_EncoderSet; }
	void setBitOut_EncoderSet(WORD BitOut_EncoderSet) { m_BitOut_EncoderSet = BitOut_EncoderSet; }

	WORD getBitOut_RecipeChangeAck() { return m_BitOut_RecipeChangeAck; }
	void setBitOut_RecipeChangeAck(WORD BitOut_RecipeChangeAck) { m_BitOut_RecipeChangeAck = BitOut_RecipeChangeAck; }

	WORD getBitOut_LotStartReqAck() { return m_BitOut_LotStartReqAck; }
	void setBitOut_LotStartReqAck(WORD BitOut_LotStartReqAck) { m_BitOut_LotStartReqAck = BitOut_LotStartReqAck; }

	WORD getBitOut_LotEndReqAck() { return m_BitOut_LotEndReqAck; }
	void setBitOut_LotEndReqAck(WORD BitOut_LotEndReqAck) { m_BitOut_LotEndReqAck = BitOut_LotEndReqAck; }

	WORD getBitOut_TabZeroReset() { return m_BitOut_TabZeroReset; }
	void setBitOut_TabZeroReset(WORD BitOut_TabZeroReset) { m_BitOut_TabZeroReset = BitOut_TabZeroReset; }

	WORD getBitOut_AlarmResetAck() { return m_BitOut_AlarmResetAck; }
	void setBitOut_AlarmResetAck(WORD BitOut_AlarmResetAck) { m_BitOut_AlarmResetAck = BitOut_AlarmResetAck; }

	WORD getBitOut_AlarmNgResetAck() { return m_BitOut_AlarmNgResetAck; }
	void setBitOut_AlarmNgResetAck(WORD BitOut_AlarmNgResetAck) { m_BitOut_AlarmNgResetAck = BitOut_AlarmNgResetAck; }

	WORD getBitOut_DiskSpaceWarning() { return m_BitOut_DiskSpaceWarning; }
	void setBitOut_DiskSpaceWarning(WORD BitOut_DiskSpaceWarning) { m_BitOut_DiskSpaceWarning = BitOut_DiskSpaceWarning; }

	WORD getBitOut_DiskSpaceAlarm() { return m_BitOut_DiskSpaceAlarm; }
	void setBitOut_DiskSpaceAlarm(WORD BitOut_DiskSpaceAlarm) { m_BitOut_DiskSpaceAlarm = BitOut_DiskSpaceAlarm; }


	WORD getWordOut_DataReportV1_Ea() { return m_WordOut_DataReportV1_Ea; }
	void setWordOut_DataReportV1_Ea(WORD WordOut_DataReportV1_Ea) { m_WordOut_DataReportV1_Ea = WordOut_DataReportV1_Ea; }

	WORD getWordOut_DataReportV2_OK() { return m_WordOut_DataReportV2_OK; }
	void setWordOut_DataReportV2_OK(WORD WordOut_DataReportV2_OK) { m_WordOut_DataReportV2_OK = WordOut_DataReportV2_OK; }

	WORD getWordOut_DataReportV3_NG() { return m_WordOut_DataReportV3_NG; }
	void setWordOut_DataReportV3_NG(WORD WordOut_DataReportV3_NG) { m_WordOut_DataReportV3_NG = WordOut_DataReportV3_NG; }

	WORD getWordOut_DataReportV4_OkRate() { return m_WordOut_DataReportV4_OkRate; }
	void setWordOut_DataReportV4_OkRate(WORD WordOut_DataReportV4_OkRate) { m_WordOut_DataReportV4_OkRate = WordOut_DataReportV4_OkRate; }

	WORD getWordOut_DataReportV5_NgRate() { return m_WordOut_DataReportV5_NgRate; }
	void setWordOut_DataReportV5_NgRate(WORD WordOut_DataReportV5_NgRate) { m_WordOut_DataReportV5_NgRate = WordOut_DataReportV5_NgRate; }

	WORD getWordOut_DataReportV6_RunRate() { return m_WordOut_DataReportV6_RunRate; }
	void setWordOut_DataReportV6_RunRate(WORD WordOut_DataReportV6_RunRate) { m_WordOut_DataReportV6_RunRate = WordOut_DataReportV6_RunRate; }

	WORD getWordOut_Continue_Alarm_Cnt() { return m_WordOut_Continue_Alarm_Cnt; }
	void setWordOut_Continue_Alarm_Cnt(WORD WordOut_Continue_Alarm_Cnt) { m_WordOut_Continue_Alarm_Cnt = WordOut_Continue_Alarm_Cnt; }

	WORD getWordOut_Heavy_Alarm_Cnt() { return m_WordOut_Heavy_Alarm_Cnt; }
	void setWordOut_Heavy_Alarm_Cnt(WORD WordOut_Heavy_Alarm_Cnt) { m_WordOut_Heavy_Alarm_Cnt = WordOut_Heavy_Alarm_Cnt; }

	WORD getWordOut_FoilExpInTop_Alarm_Cnt() { return m_WordOut_FoilExpInTop_Alarm_Cnt; }
	void setWordOut_FoilExpInTop_Alarm_Cnt(WORD WordOut_FoilExpInTop_Alarm_Cnt) { m_WordOut_FoilExpInTop_Alarm_Cnt = WordOut_FoilExpInTop_Alarm_Cnt; }

	WORD getWordOut_FoilExpInBtm_Alarm_Cnt() { return m_WordOut_FoilExpInBtm_Alarm_Cnt; }
	void setWordOut_FoilExpInBtm_Alarm_Cnt(WORD WordOut_FoilExpInBtm_Alarm_Cnt) { m_WordOut_FoilExpInBtm_Alarm_Cnt = WordOut_FoilExpInBtm_Alarm_Cnt; }

	WORD getWordOut_FoilExpOutTop_Alarm_Cnt() { return m_WordOut_FoilExpOutTop_Alarm_Cnt; }
	void setWordOut_FoilExpOutTop_Alarm_Cnt(WORD WordOut_FoilExpOutTop_Alarm_Cnt) { m_WordOut_FoilExpOutTop_Alarm_Cnt = WordOut_FoilExpOutTop_Alarm_Cnt; }

	WORD getWordOut_FoilExpOutBtm_Alarm_Cnt() { return m_WordOut_FoilExpOutBtm_Alarm_Cnt; }
	void setWordOut_FoilExpOutBtm_Alarm_Cnt(WORD WordOut_FoilExpOutBtm_Alarm_Cnt) { m_WordOut_FoilExpOutBtm_Alarm_Cnt = WordOut_FoilExpOutBtm_Alarm_Cnt; }

	WORD getWordOut_FoilExpBothTop_Alarm_Cnt() { return m_WordOut_FoilExpBothTop_Alarm_Cnt; }
	void setWordOut_FoilExpBothTop_Alarm_Cnt(WORD WordOut_FoilExpBothTop_Alarm_Cnt) { m_WordOut_FoilExpBothTop_Alarm_Cnt = WordOut_FoilExpBothTop_Alarm_Cnt; }

	WORD getWordOut_FoilExpBothBtm_Alarm_Cnt() { return m_WordOut_FoilExpBothBtm_Alarm_Cnt; }
	void setWordOut_FoilExpBothBtm_Alarm_Cnt(WORD WordOut_FoilExpBothBtm_Alarm_Cnt) { m_WordOut_FoilExpBothBtm_Alarm_Cnt = WordOut_FoilExpBothBtm_Alarm_Cnt; }

	WORD getWordOut_SpeterTop_Alarm_Cnt() { return m_WordOut_SpeterTop_Alarm_Cnt; }
	void setWordOut_SpeterTop_Alarm_Cnt(WORD WordOut_SpeterTop_Alarm_Cnt) { m_WordOut_SpeterTop_Alarm_Cnt = WordOut_SpeterTop_Alarm_Cnt; }

	WORD getWordOut_SpeterBtm_Alarm_Cnt() { return m_WordOut_SpeterBtm_Alarm_Cnt; }
	void setWordOut_SpeterBtm_Alarm_Cnt(WORD WordOut_SpeterBtm_Alarm_Cnt) { m_WordOut_SpeterBtm_Alarm_Cnt = WordOut_SpeterBtm_Alarm_Cnt; }

	WORD getWordOut_Top_Defect_Count_Real() { return m_WordOut_Top_Defect_Count_Real; }
	void setWordOut_Top_Defect_Count_Real(WORD WordOut_Top_Defect_Count_Real) { m_WordOut_Top_Defect_Count_Real = WordOut_Top_Defect_Count_Real; }

	WORD getWordOut_Btm_Defect_Count_Real() { return m_WordOut_Btm_Defect_Count_Real; }
	void setWordOut_Btm_Defect_Count_Real(WORD WordOut_Btm_Defect_Count_Real) { m_WordOut_Btm_Defect_Count_Real = WordOut_Btm_Defect_Count_Real; }

	WORD getWordOut_Top_Defect_Count_LotEnd() { return m_WordOut_Top_Defect_Count_LotEnd; }
	void setWordOut_Top_Defect_Count_LotEnd(WORD WordOut_Top_Defect_Count_LotEnd) { m_WordOut_Top_Defect_Count_LotEnd = WordOut_Top_Defect_Count_LotEnd; }

	WORD getWordOut_Btm_Defect_Count_LotEnd() { return m_WordOut_Btm_Defect_Count_LotEnd; }
	void setWordOut_Btm_Defect_Count_LotEnd(WORD WordOut_Btm_Defect_Count_LotEnd) { m_WordOut_Btm_Defect_Count_LotEnd = WordOut_Btm_Defect_Count_LotEnd; }

	WORD getWordOut_FoilExpInTopTarget() { return m_WordOut_FoilExpInTopTarget; }
	void setWordOut_FoilExpInTopTarget(WORD WordOut_FoilExpInTopTarget) { m_WordOut_FoilExpInTopTarget = WordOut_FoilExpInTopTarget; }

	WORD getWordOut_FoilExpInBtmTarget() { return m_WordOut_FoilExpInBtmTarget; }
	void setWordOut_FoilExpInBtmTarget(WORD WordOut_FoilExpInBtmTarget) { m_WordOut_FoilExpInBtmTarget = WordOut_FoilExpInBtmTarget; }

	WORD getWordOut_FoilExpOutTopTarget() { return m_WordOut_FoilExpOutTopTarget; }
	void setWordOut_FoilExpOutTopTarget(WORD WordOut_FoilExpOutTopTarget) { m_WordOut_FoilExpOutTopTarget = WordOut_FoilExpOutTopTarget; }

	WORD getWordOut_FoilExpOutBtmTarget() { return m_WordOut_FoilExpOutBtmTarget; }
	void setWordOut_FoilExpOutBtmTarget(WORD WordOut_FoilExpOutBtmTarget) { m_WordOut_FoilExpOutBtmTarget = WordOut_FoilExpOutBtmTarget; }

	WORD getWordOut_FoilExpBothTopTarget() { return m_WordOut_FoilExpBothTopTarget; }
	void setWordOut_FoilExpBothTopTarget(WORD WordOut_FoilExpBothTopTarget) { m_WordOut_FoilExpBothTopTarget = WordOut_FoilExpBothTopTarget; }

	WORD getWordOut_FoilExpBothBtmTarget() { return m_WordOut_FoilExpBothBtmTarget; }
	void setWordOut_FoilExpBothBtmTarget(WORD WordOut_FoilExpBothBtmTarget) { m_WordOut_FoilExpBothBtmTarget = WordOut_FoilExpBothBtmTarget; }

	WORD getWordOut_SpeterTopTarget() { return m_WordOut_SpeterTopTarget; }
	void setWordOut_SpeterTopTarget(WORD WordOut_SpeterTopTarget) { m_WordOut_SpeterTopTarget = WordOut_SpeterTopTarget; }

	WORD getWordOut_SpeterBtmTarget() { return m_WordOut_SpeterBtmTarget; }
	void setWordOut_SpeterBtmTarget(WORD WordOut_SpeterBtmTarget) { m_WordOut_SpeterBtmTarget = WordOut_SpeterBtmTarget; }

	WORD getWordOut_PrmContinuousCnt() { return m_WordOut_PrmContinuousCnt; }
	void setWordOut_PrmContinuousCnt(WORD WordOut_PrmContinuousCnt) { m_WordOut_PrmContinuousCnt = WordOut_PrmContinuousCnt; }

	WORD getWordOut_PrmSectorNgTabCnt() { return m_WordOut_PrmSectorNgTabCnt; }
	void setWordOut_PrmSectorNgTabCnt(WORD WordOut_PrmSectorNgTabCnt) { m_WordOut_PrmSectorNgTabCnt = WordOut_PrmSectorNgTabCnt; }

	WORD getWordOut_PrmSectorBaseCnt() { return m_WordOut_PrmSectorBaseCnt; }
	void setWordOut_PrmSectorBaseCnt(WORD WordOut_PrmSectorBaseCnt) { m_WordOut_PrmSectorBaseCnt = WordOut_PrmSectorBaseCnt; }

	WORD getWordOut_AlarmExist() { return m_WordOut_AlarmExist; }
	void setWordOut_AlarmExist(WORD WordOut_AlarmExist) { m_WordOut_AlarmExist = WordOut_AlarmExist; }

	WORD getWordOut_AlarmCode_Buffer1() { return m_WordOut_AlarmCode_Buffer1; }
	void setWordOut_AlarmCode_Buffer1(WORD WordOut_AlarmCode_Buffer1) { m_WordOut_AlarmCode_Buffer1 = WordOut_AlarmCode_Buffer1; }

	WORD getWordOut_AlarmCode_Buffer2() { return m_WordOut_AlarmCode_Buffer2; }
	void setWordOut_AlarmCode_Buffer2(WORD WordOut_AlarmCode_Buffer2) { m_WordOut_AlarmCode_Buffer2 = WordOut_AlarmCode_Buffer2; }

	WORD getWordOut_AlarmCode_Buffer3() { return m_WordOut_AlarmCode_Buffer3; }
	void setWordOut_AlarmCode_Buffer3(WORD WordOut_AlarmCode_Buffer3) { m_WordOut_AlarmCode_Buffer3 = WordOut_AlarmCode_Buffer3; }

	WORD getWordOut_AlarmCode_Buffer4() { return m_WordOut_AlarmCode_Buffer4; }
	void setWordOut_AlarmCode_Buffer4(WORD WordOut_AlarmCode_Buffer4) { m_WordOut_AlarmCode_Buffer4 = WordOut_AlarmCode_Buffer4; }

	WORD getWordOut_AlarmCode_Buffer5() { return m_WordOut_AlarmCode_Buffer5; }
	void setWordOut_AlarmCode_Buffer5(WORD WordOut_AlarmCode_Buffer5) { m_WordOut_AlarmCode_Buffer5 = WordOut_AlarmCode_Buffer5; }

	WORD getWordOut_AlarmCode_Buffer6() { return m_WordOut_AlarmCode_Buffer6; }
	void setWordOut_AlarmCode_Buffer6(WORD WordOut_AlarmCode_Buffer6) { m_WordOut_AlarmCode_Buffer6 = WordOut_AlarmCode_Buffer6; }

	WORD getWordOut_AlarmCode_Buffer7() { return m_WordOut_AlarmCode_Buffer7; }
	void setWordOut_AlarmCode_Buffer7(WORD WordOut_AlarmCode_Buffer7) { m_WordOut_AlarmCode_Buffer7 = WordOut_AlarmCode_Buffer7; }

	WORD getWordOut_AlarmCode_Buffer8() { return m_WordOut_AlarmCode_Buffer8; }
	void setWordOut_AlarmCode_Buffer8(WORD WordOut_AlarmCode_Buffer8) { m_WordOut_AlarmCode_Buffer8 = WordOut_AlarmCode_Buffer8; }

	WORD getWordOut_AlarmCode_Buffer9() { return m_WordOut_AlarmCode_Buffer9; }
	void setWordOut_AlarmCode_Buffer9(WORD WordOut_AlarmCode_Buffer9) { m_WordOut_AlarmCode_Buffer9 = WordOut_AlarmCode_Buffer9; }

	WORD getWordOut_AlarmCode_Buffer10() { return m_WordOut_AlarmCode_Buffer10; }
	void setWordOut_AlarmCode_Buffer10(WORD WordOut_AlarmCode_Buffer10) { m_WordOut_AlarmCode_Buffer10 = WordOut_AlarmCode_Buffer10; }

	WORD getWordOut_AlarmCode_Buffer11() { return m_WordOut_AlarmCode_Buffer11; }
	void setWordOut_AlarmCode_Buffer11(WORD WordOut_AlarmCode_Buffer11) { m_WordOut_AlarmCode_Buffer11 = WordOut_AlarmCode_Buffer11; }

	WORD getWordOut_AlarmCode_Buffer12() { return m_WordOut_AlarmCode_Buffer12; }
	void setWordOut_AlarmCode_Buffer12(WORD WordOut_AlarmCode_Buffer12) { m_WordOut_AlarmCode_Buffer12 = WordOut_AlarmCode_Buffer12; }

	WORD getWordOut_AlarmCode_Buffer13() { return m_WordOut_AlarmCode_Buffer13; }
	void setWordOut_AlarmCode_Buffer13(WORD WordOut_AlarmCode_Buffer13) { m_WordOut_AlarmCode_Buffer13 = WordOut_AlarmCode_Buffer13; }

	WORD getWordOut_AlarmCode_Buffer14() { return m_WordOut_AlarmCode_Buffer14; }
	void setWordOut_AlarmCode_Buffer14(WORD WordOut_AlarmCode_Buffer14) { m_WordOut_AlarmCode_Buffer14 = WordOut_AlarmCode_Buffer14; }

	WORD getWordOut_AlarmCode_Buffer15() { return m_WordOut_AlarmCode_Buffer15; }
	void setWordOut_AlarmCode_Buffer15(WORD WordOut_AlarmCode_Buffer15) { m_WordOut_AlarmCode_Buffer15 = WordOut_AlarmCode_Buffer15; }

	WORD getWordOut_AlarmCode_Buffer16() { return m_WordOut_AlarmCode_Buffer16; }
	void setWordOut_AlarmCode_Buffer16(WORD WordOut_AlarmCode_Buffer16) { m_WordOut_AlarmCode_Buffer16 = WordOut_AlarmCode_Buffer16; }

	WORD getWordOut_AlarmCode_Buffer17() { return m_WordOut_AlarmCode_Buffer17; }
	void setWordOut_AlarmCode_Buffer17(WORD WordOut_AlarmCode_Buffer17) { m_WordOut_AlarmCode_Buffer17 = WordOut_AlarmCode_Buffer17; }

	WORD getWordOut_AlarmCode_Buffer18() { return m_WordOut_AlarmCode_Buffer18; }
	void setWordOut_AlarmCode_Buffer18(WORD WordOut_AlarmCode_Buffer18) { m_WordOut_AlarmCode_Buffer18 = WordOut_AlarmCode_Buffer18; }

	WORD getWordOut_AlarmCode_Buffer19() { return m_WordOut_AlarmCode_Buffer19; }
	void setWordOut_AlarmCode_Buffer19(WORD WordOut_AlarmCode_Buffer19) { m_WordOut_AlarmCode_Buffer19 = WordOut_AlarmCode_Buffer19; }

	WORD getWordOut_AlarmCode_Buffer20() { return m_WordOut_AlarmCode_Buffer20; }
	void setWordOut_AlarmCode_Buffer20(WORD WordOut_AlarmCode_Buffer20) { m_WordOut_AlarmCode_Buffer20 = WordOut_AlarmCode_Buffer20; }

	WORD getWordOut_AlarmCode_Buffer21() { return m_WordOut_AlarmCode_Buffer21; }
	void setWordOut_AlarmCode_Buffer21(WORD WordOut_AlarmCode_Buffer21) { m_WordOut_AlarmCode_Buffer21 = WordOut_AlarmCode_Buffer21; }

	WORD getWordOut_AlarmCode_Buffer22() { return m_WordOut_AlarmCode_Buffer22; }
	void setWordOut_AlarmCode_Buffer22(WORD WordOut_AlarmCode_Buffer22) { m_WordOut_AlarmCode_Buffer22 = WordOut_AlarmCode_Buffer22; }

	WORD getWordOut_AlarmCode_Buffer23() { return m_WordOut_AlarmCode_Buffer23; }
	void setWordOut_AlarmCode_Buffer23(WORD WordOut_AlarmCode_Buffer23) { m_WordOut_AlarmCode_Buffer23 = WordOut_AlarmCode_Buffer23; }

	WORD getWordOut_Cell_Trigger_ID() { return m_WordOut_Cell_Trigger_ID; }
	void setWordOut_Cell_Trigger_ID(WORD WordOut_Cell_Trigger_ID) { m_WordOut_Cell_Trigger_ID = WordOut_Cell_Trigger_ID; }

	WORD getWordOut_Judge() { return m_WordOut_Judge; }
	void setWordOut_Judge(WORD WordOut_Judge) { m_WordOut_Judge = WordOut_Judge; }

	WORD getWordOut_NG_Code() { return m_WordOut_NG_Code; }
	void setWordOut_NG_Code(WORD WordOut_NG_Code) { m_WordOut_NG_Code = WordOut_NG_Code; }

	WORD getWordOut_DuplicateNG_Cell_ID() { return m_WordOut_DuplicateNG_Cell_ID; }
	void setWordOut_DuplicateNG_Cell_ID(WORD WordOut_DuplicateNG_Cell_ID) { m_WordOut_DuplicateNG_Cell_ID = WordOut_DuplicateNG_Cell_ID; }

#endif //NEW_PLCTYPE

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

