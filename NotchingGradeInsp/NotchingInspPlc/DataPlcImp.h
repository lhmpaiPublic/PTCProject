#pragma once

class CSequenceData;
class CDataPlcImp
{
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

	//Word Data In
	WORD m_WordIn_RecipeNo;
	CString m_WordIn_RecipeName;
	CString m_WordIn_CELL_ID;

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
	BOOL m_BitOut_Alive;
	BOOL m_BitOut_Ready;
	BOOL m_BitOut_EncoderSet;
	BOOL m_BitOut_RecipeChangeAck;
	BOOL m_BitOut_LotStartReqAck;
	BOOL m_BitOut_LotEndReqAck;
	BOOL m_BitOut_TabZeroReset;
	BOOL m_BitOut_AlarmResetAck;
	BOOL m_BitOut_AlarmNgResetAck;

	BOOL m_BitOut_DiskSpaceWarning;
	BOOL m_BitOut_DiskSpaceAlarm;

	//Word Data Out
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
	WORD m_WordOut_AlarmCode_Buffer[24];

	WORD m_WordOut_Cell_Trigger_ID;
	WORD m_WordOut_Judge;
	WORD m_WordOut_NG_Code;

	WORD m_WordOut_DuplicateNG_Cell_ID[64];

	//Bit Data Out
	BOOL m_bBitOut_Alive;
	BOOL m_bBitOut_Ready;
	BOOL m_bBitOut_EncoderSet;
	BOOL m_bBitOut_RecipeChangeAck;
	BOOL m_bBitOut_LotStartReqAck;
	BOOL m_bBitOut_LotEndReqAck;
	BOOL m_bBitOut_TabZeroReset;
	BOOL m_bBitOut_AlarmResetAck;
	BOOL m_bBitOut_AlarmNgResetAck;

	BOOL m_bBitOut_DiskSpaceWarning;
	BOOL m_bBitOut_DiskSpaceAlarm;

	//Word Data Out
	BOOL m_bWordOut_DataReportV1_Ea;
	BOOL m_bWordOut_DataReportV2_OK;
	BOOL m_bWordOut_DataReportV3_NG;
	BOOL m_bWordOut_DataReportV4_OkRate;
	BOOL m_bWordOut_DataReportV5_NgRate;
	BOOL m_bWordOut_DataReportV6_RunRate;
	BOOL m_bWordOut_Continue_Alarm_Cnt;
	BOOL m_bWordOut_Heavy_Alarm_Cnt;

	BOOL m_bWordOut_FoilExpInTop_Alarm_Cnt;
	BOOL m_bWordOut_FoilExpInBtm_Alarm_Cnt;
	BOOL m_bWordOut_FoilExpOutTop_Alarm_Cnt;
	BOOL m_bWordOut_FoilExpOutBtm_Alarm_Cnt;
	BOOL m_bWordOut_FoilExpBothTop_Alarm_Cnt;
	BOOL m_bWordOut_FoilExpBothBtm_Alarm_Cnt;
	BOOL m_bWordOut_SpeterTop_Alarm_Cnt;
	BOOL m_bWordOut_SpeterBtm_Alarm_Cnt;

	BOOL m_bWordOut_Top_Defect_Count_Real;
	BOOL m_bWordOut_Btm_Defect_Count_Real;
	BOOL m_bWordOut_Top_Defect_Count_LotEnd;
	BOOL m_bWordOut_Btm_Defect_Count_LotEnd;

	BOOL m_bWordOut_FoilExpInTopTarget;
	BOOL m_bWordOut_FoilExpInBtmTarget;
	BOOL m_bWordOut_FoilExpOutTopTarget;
	BOOL m_bWordOut_FoilExpOutBtmTarget;
	BOOL m_bWordOut_FoilExpBothTopTarget;
	BOOL m_bWordOut_FoilExpBothBtmTarget;
	BOOL m_bWordOut_SpeterTopTarget;
	BOOL m_bWordOut_SpeterBtmTarget;

	BOOL m_bWordOut_PrmContinuousCnt;
	BOOL m_bWordOut_PrmSectorNgTabCnt;
	BOOL m_bWordOut_PrmSectorBaseCnt;

	BOOL m_bWordOut_AlarmExist;
	BOOL m_bWordOut_AlarmCode_Buffer;

	BOOL m_bWordOut_Cell_Trigger_ID;
	BOOL m_bWordOut_Judge;
	BOOL m_bWordOut_NG_Code;

	BOOL m_bWordOut_DuplicateNG_Cell_ID;



public:
	CDataPlcImp();
	virtual ~CDataPlcImp();

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

	CString getWordIn_RecipeName() { return m_WordIn_RecipeName; }
	void setWordIn_RecipeName(CString WordIn_RecipeName) { m_WordIn_RecipeName = WordIn_RecipeName; }

	CString getWordIn_CELL_ID() { return m_WordIn_CELL_ID; }
	void setWordIn_CELL_ID(CString WordIn_CELL_ID) { m_WordIn_CELL_ID = WordIn_CELL_ID; }

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
	BOOL getBitOut_Alive() { m_bBitOut_Alive = false; return m_BitOut_Alive; }
	void setBitOut_Alive(BOOL BitOut_Alive) { m_BitOut_Alive = BitOut_Alive; m_bBitOut_Alive = true; }
	BOOL isBitOut_Alive() { return m_bBitOut_Alive; }

	BOOL getBitOut_Ready() { m_bBitOut_Ready = false; return m_BitOut_Ready; }
	void setBitOut_Ready(BOOL BitOut_Ready) { m_BitOut_Ready = BitOut_Ready; m_bBitOut_Ready = true; }
	BOOL isBitOut_Ready() { return m_bBitOut_Ready; }

	BOOL getBitOut_EncoderSet() { m_bBitOut_EncoderSet = false; return m_BitOut_EncoderSet; }
	void setBitOut_EncoderSet(BOOL BitOut_EncoderSet) { m_BitOut_EncoderSet = BitOut_EncoderSet;  m_bBitOut_EncoderSet = true; }
	BOOL isBitOut_EncoderSet() { return m_bBitOut_EncoderSet; }

	BOOL getBitOut_RecipeChangeAck() { m_bBitOut_RecipeChangeAck = false; return m_BitOut_RecipeChangeAck; }
	void setBitOut_RecipeChangeAck(BOOL BitOut_RecipeChangeAck) { m_BitOut_RecipeChangeAck = BitOut_RecipeChangeAck;  m_bBitOut_RecipeChangeAck = true; }
	BOOL isBitOut_RecipeChangeAck() { return m_bBitOut_RecipeChangeAck; }

	BOOL getBitOut_LotStartReqAck() { m_bBitOut_LotStartReqAck = false; return m_BitOut_LotStartReqAck; }
	void setBitOut_LotStartReqAck(BOOL BitOut_LotStartReqAck) { m_BitOut_LotStartReqAck = BitOut_LotStartReqAck;  m_bBitOut_LotStartReqAck = true; }
	BOOL isBitOut_LotStartReqAck() { return m_bBitOut_LotStartReqAck; }

	BOOL getBitOut_LotEndReqAck() { m_bBitOut_LotEndReqAck = false; return m_BitOut_LotEndReqAck; }
	void setBitOut_LotEndReqAck(BOOL BitOut_LotEndReqAck) { m_BitOut_LotEndReqAck = BitOut_LotEndReqAck;  m_bBitOut_LotEndReqAck = true; }
	BOOL isBitOut_LotEndReqAck() { return m_bBitOut_LotEndReqAck; }

	BOOL getBitOut_TabZeroReset() { m_bBitOut_TabZeroReset = false; return m_BitOut_TabZeroReset; }
	void setBitOut_TabZeroReset(BOOL BitOut_TabZeroReset) { m_BitOut_TabZeroReset = BitOut_TabZeroReset;  m_bBitOut_TabZeroReset = true; }
	BOOL isBitOut_TabZeroReset() { return m_bBitOut_TabZeroReset; }

	BOOL getBitOut_AlarmResetAck() { m_bBitOut_AlarmResetAck = false; return m_BitOut_AlarmResetAck; }
	void setBitOut_AlarmResetAck(BOOL BitOut_AlarmResetAck) { m_BitOut_AlarmResetAck = BitOut_AlarmResetAck;  m_bBitOut_AlarmResetAck = true; }
	BOOL isBitOut_AlarmResetAck() { return m_bBitOut_AlarmResetAck; }

	BOOL getBitOut_AlarmNgResetAck() { m_bBitOut_AlarmNgResetAck = false; return m_BitOut_AlarmNgResetAck; }
	void setBitOut_AlarmNgResetAck(BOOL BitOut_AlarmNgResetAck) { m_BitOut_AlarmNgResetAck = BitOut_AlarmNgResetAck;  m_bBitOut_AlarmNgResetAck = true; }
	BOOL isBitOut_AlarmNgResetAck() { return m_bBitOut_AlarmNgResetAck; }

	BOOL getBitOut_DiskSpaceWarning() { m_bBitOut_DiskSpaceWarning = false; return m_BitOut_DiskSpaceWarning; }
	void setBitOut_DiskSpaceWarning(BOOL BitOut_DiskSpaceWarning) { m_BitOut_DiskSpaceWarning = BitOut_DiskSpaceWarning;  m_bBitOut_DiskSpaceWarning = true; }
	BOOL isBitOut_DiskSpaceWarning() { return m_bBitOut_DiskSpaceWarning; }

	BOOL getBitOut_DiskSpaceAlarm() { m_bBitOut_DiskSpaceAlarm = false; return m_BitOut_DiskSpaceAlarm; }
	void setBitOut_DiskSpaceAlarm(BOOL BitOut_DiskSpaceAlarm) { m_BitOut_DiskSpaceAlarm = BitOut_DiskSpaceAlarm;  m_bBitOut_DiskSpaceAlarm = true; }
	BOOL isBitOut_DiskSpaceAlarm() { return m_bBitOut_DiskSpaceAlarm; }


	WORD getWordOut_DataReportV1_Ea() { m_bWordOut_DataReportV1_Ea = false; return m_WordOut_DataReportV1_Ea; }
	void setWordOut_DataReportV1_Ea(WORD WordOut_DataReportV1_Ea) { m_WordOut_DataReportV1_Ea = WordOut_DataReportV1_Ea;  m_bWordOut_DataReportV1_Ea = true; }
	BOOL isWordOut_DataReportV1_Ea() { return m_bWordOut_DataReportV1_Ea; }

	WORD getWordOut_DataReportV2_OK() { m_bWordOut_DataReportV2_OK = false; return m_WordOut_DataReportV2_OK; }
	void setWordOut_DataReportV2_OK(WORD WordOut_DataReportV2_OK) { m_WordOut_DataReportV2_OK = WordOut_DataReportV2_OK;  m_bWordOut_DataReportV2_OK = true; }
	BOOL isWordOut_DataReportV2_OK() { return m_bWordOut_DataReportV2_OK; }

	WORD getWordOut_DataReportV3_NG() { m_bWordOut_DataReportV3_NG = false; return m_WordOut_DataReportV3_NG; }
	void setWordOut_DataReportV3_NG(WORD WordOut_DataReportV3_NG) { m_WordOut_DataReportV3_NG = WordOut_DataReportV3_NG;  m_bWordOut_DataReportV3_NG = true; }
	BOOL isWordOut_DataReportV3_NG() { return m_bWordOut_DataReportV3_NG; }

	WORD getWordOut_DataReportV4_OkRate() { m_bWordOut_DataReportV4_OkRate = false; return m_WordOut_DataReportV4_OkRate; }
	void setWordOut_DataReportV4_OkRate(WORD WordOut_DataReportV4_OkRate) { m_WordOut_DataReportV4_OkRate = WordOut_DataReportV4_OkRate;  m_bWordOut_DataReportV4_OkRate = true; }
	BOOL isWordOut_DataReportV4_OkRate() { return m_bWordOut_DataReportV4_OkRate; }

	WORD getWordOut_DataReportV5_NgRate() { m_bWordOut_DataReportV5_NgRate = false; return m_WordOut_DataReportV5_NgRate; }
	void setWordOut_DataReportV5_NgRate(WORD WordOut_DataReportV5_NgRate) { m_WordOut_DataReportV5_NgRate = WordOut_DataReportV5_NgRate;  m_bWordOut_DataReportV5_NgRate = true; }
	BOOL isWordOut_DataReportV5_NgRate() { return m_bWordOut_DataReportV5_NgRate; }

	WORD getWordOut_DataReportV6_RunRate() { m_bWordOut_DataReportV6_RunRate = false; return m_WordOut_DataReportV6_RunRate; }
	void setWordOut_DataReportV6_RunRate(WORD WordOut_DataReportV6_RunRate) { m_WordOut_DataReportV6_RunRate = WordOut_DataReportV6_RunRate;  m_bWordOut_DataReportV6_RunRate = true; }
	BOOL isWordOut_DataReportV6_RunRate() { return m_bWordOut_DataReportV6_RunRate; }

	WORD getWordOut_Continue_Alarm_Cnt() { m_bWordOut_Continue_Alarm_Cnt = false; return m_WordOut_Continue_Alarm_Cnt; }
	void setWordOut_Continue_Alarm_Cnt(WORD WordOut_Continue_Alarm_Cnt) { m_WordOut_Continue_Alarm_Cnt = WordOut_Continue_Alarm_Cnt;  m_bWordOut_Continue_Alarm_Cnt = true; }
	BOOL isWordOut_Continue_Alarm_Cnt() { return m_bWordOut_Continue_Alarm_Cnt; }

	WORD getWordOut_Heavy_Alarm_Cnt() { m_bWordOut_Heavy_Alarm_Cnt = false; return m_WordOut_Heavy_Alarm_Cnt; }
	void setWordOut_Heavy_Alarm_Cnt(WORD WordOut_Heavy_Alarm_Cnt) { m_WordOut_Heavy_Alarm_Cnt = WordOut_Heavy_Alarm_Cnt;  m_bWordOut_Heavy_Alarm_Cnt = true; }
	BOOL isWordOut_Heavy_Alarm_Cnt() { return m_bWordOut_Heavy_Alarm_Cnt; }

	WORD getWordOut_FoilExpInTop_Alarm_Cnt() { m_bWordOut_FoilExpInTop_Alarm_Cnt = false; return m_WordOut_FoilExpInTop_Alarm_Cnt; }
	void setWordOut_FoilExpInTop_Alarm_Cnt(WORD WordOut_FoilExpInTop_Alarm_Cnt) { m_WordOut_FoilExpInTop_Alarm_Cnt = WordOut_FoilExpInTop_Alarm_Cnt;  m_bWordOut_FoilExpInTop_Alarm_Cnt = true; }
	BOOL isWordOut_FoilExpInTop_Alarm_Cnt() { return m_bWordOut_FoilExpInTop_Alarm_Cnt; }

	WORD getWordOut_FoilExpInBtm_Alarm_Cnt() { m_bWordOut_FoilExpInBtm_Alarm_Cnt = false; return m_WordOut_FoilExpInBtm_Alarm_Cnt; }
	void setWordOut_FoilExpInBtm_Alarm_Cnt(WORD WordOut_FoilExpInBtm_Alarm_Cnt) { m_WordOut_FoilExpInBtm_Alarm_Cnt = WordOut_FoilExpInBtm_Alarm_Cnt;  m_bWordOut_FoilExpInBtm_Alarm_Cnt = true; }
	BOOL isWordOut_FoilExpInBtm_Alarm_Cnt() { return m_bWordOut_FoilExpInBtm_Alarm_Cnt; }

	WORD getWordOut_FoilExpOutTop_Alarm_Cnt() { m_bWordOut_FoilExpOutTop_Alarm_Cnt = false; return m_WordOut_FoilExpOutTop_Alarm_Cnt; }
	void setWordOut_FoilExpOutTop_Alarm_Cnt(WORD WordOut_FoilExpOutTop_Alarm_Cnt) { m_WordOut_FoilExpOutTop_Alarm_Cnt = WordOut_FoilExpOutTop_Alarm_Cnt;  m_bWordOut_FoilExpOutTop_Alarm_Cnt = true; }
	BOOL isWordOut_FoilExpOutTop_Alarm_Cnt() { return m_bWordOut_FoilExpOutTop_Alarm_Cnt; }

	WORD getWordOut_FoilExpOutBtm_Alarm_Cnt() { m_bWordOut_FoilExpOutBtm_Alarm_Cnt = false; return m_WordOut_FoilExpOutBtm_Alarm_Cnt; }
	void setWordOut_FoilExpOutBtm_Alarm_Cnt(WORD WordOut_FoilExpOutBtm_Alarm_Cnt) { m_WordOut_FoilExpOutBtm_Alarm_Cnt = WordOut_FoilExpOutBtm_Alarm_Cnt;  m_bWordOut_FoilExpOutBtm_Alarm_Cnt = true; }
	BOOL isWordOut_FoilExpOutBtm_Alarm_Cnt() { return m_bWordOut_FoilExpOutBtm_Alarm_Cnt; }

	WORD getWordOut_FoilExpBothTop_Alarm_Cnt() { m_bWordOut_FoilExpBothTop_Alarm_Cnt = false; return m_WordOut_FoilExpBothTop_Alarm_Cnt; }
	void setWordOut_FoilExpBothTop_Alarm_Cnt(WORD WordOut_FoilExpBothTop_Alarm_Cnt) { m_WordOut_FoilExpBothTop_Alarm_Cnt = WordOut_FoilExpBothTop_Alarm_Cnt;  m_bWordOut_FoilExpBothTop_Alarm_Cnt = true; }
	BOOL isWordOut_FoilExpBothTop_Alarm_Cnt() { return m_bWordOut_FoilExpBothTop_Alarm_Cnt; }

	WORD getWordOut_FoilExpBothBtm_Alarm_Cnt() { m_bWordOut_FoilExpBothBtm_Alarm_Cnt = false; return m_WordOut_FoilExpBothBtm_Alarm_Cnt; }
	void setWordOut_FoilExpBothBtm_Alarm_Cnt(WORD WordOut_FoilExpBothBtm_Alarm_Cnt) { m_WordOut_FoilExpBothBtm_Alarm_Cnt = WordOut_FoilExpBothBtm_Alarm_Cnt;  m_bWordOut_FoilExpBothBtm_Alarm_Cnt = true; }
	BOOL isWordOut_FoilExpBothBtm_Alarm_Cnt() { return m_bWordOut_FoilExpBothBtm_Alarm_Cnt; }

	WORD getWordOut_SpeterTop_Alarm_Cnt() { m_bWordOut_SpeterTop_Alarm_Cnt = false; return m_WordOut_SpeterTop_Alarm_Cnt; }
	void setWordOut_SpeterTop_Alarm_Cnt(WORD WordOut_SpeterTop_Alarm_Cnt) { m_WordOut_SpeterTop_Alarm_Cnt = WordOut_SpeterTop_Alarm_Cnt;  m_bWordOut_SpeterTop_Alarm_Cnt = true; }
	BOOL isWordOut_SpeterTop_Alarm_Cnt() { return m_bWordOut_SpeterTop_Alarm_Cnt; }

	WORD getWordOut_SpeterBtm_Alarm_Cnt() { m_bWordOut_SpeterBtm_Alarm_Cnt = false; return m_WordOut_SpeterBtm_Alarm_Cnt; }
	void setWordOut_SpeterBtm_Alarm_Cnt(WORD WordOut_SpeterBtm_Alarm_Cnt) { m_WordOut_SpeterBtm_Alarm_Cnt = WordOut_SpeterBtm_Alarm_Cnt;  m_bWordOut_SpeterBtm_Alarm_Cnt = true; }
	BOOL isWordOut_SpeterBtm_Alarm_Cnt() { return m_bWordOut_SpeterBtm_Alarm_Cnt; }

	WORD getWordOut_Top_Defect_Count_Real() { m_bWordOut_Top_Defect_Count_Real = false; return m_WordOut_Top_Defect_Count_Real; }
	void setWordOut_Top_Defect_Count_Real(WORD WordOut_Top_Defect_Count_Real) { m_WordOut_Top_Defect_Count_Real = WordOut_Top_Defect_Count_Real;  m_bWordOut_Top_Defect_Count_Real = true; }
	BOOL isWordOut_Top_Defect_Count_Real() { return m_bWordOut_Top_Defect_Count_Real; }

	WORD getWordOut_Btm_Defect_Count_Real() { m_bWordOut_Btm_Defect_Count_Real = false; return m_WordOut_Btm_Defect_Count_Real; }
	void setWordOut_Btm_Defect_Count_Real(WORD WordOut_Btm_Defect_Count_Real) { m_WordOut_Btm_Defect_Count_Real = WordOut_Btm_Defect_Count_Real;  m_bWordOut_Btm_Defect_Count_Real = true; }
	BOOL isWordOut_Btm_Defect_Count_Real() { return m_bWordOut_Btm_Defect_Count_Real; }

	WORD getWordOut_Top_Defect_Count_LotEnd() { m_bWordOut_Top_Defect_Count_LotEnd = false; return m_WordOut_Top_Defect_Count_LotEnd; }
	void setWordOut_Top_Defect_Count_LotEnd(WORD WordOut_Top_Defect_Count_LotEnd) { m_WordOut_Top_Defect_Count_LotEnd = WordOut_Top_Defect_Count_LotEnd;  m_bWordOut_Top_Defect_Count_LotEnd = true; }
	BOOL isWordOut_Top_Defect_Count_LotEnd() { return m_bWordOut_Top_Defect_Count_LotEnd; }

	WORD getWordOut_Btm_Defect_Count_LotEnd() { m_bWordOut_Btm_Defect_Count_LotEnd = false; return m_WordOut_Btm_Defect_Count_LotEnd; }
	void setWordOut_Btm_Defect_Count_LotEnd(WORD WordOut_Btm_Defect_Count_LotEnd) { m_WordOut_Btm_Defect_Count_LotEnd = WordOut_Btm_Defect_Count_LotEnd;  m_bWordOut_Btm_Defect_Count_LotEnd = true; }
	BOOL isWordOut_Btm_Defect_Count_LotEnd() { return m_bWordOut_Btm_Defect_Count_LotEnd; }

	WORD getWordOut_FoilExpInTopTarget() { m_bWordOut_FoilExpInTopTarget = false; return m_WordOut_FoilExpInTopTarget; }
	void setWordOut_FoilExpInTopTarget(WORD WordOut_FoilExpInTopTarget) { m_WordOut_FoilExpInTopTarget = WordOut_FoilExpInTopTarget;  m_bWordOut_FoilExpInTopTarget = true; }
	BOOL isWordOut_FoilExpInTopTarget() { return m_bWordOut_FoilExpInTopTarget; }

	WORD getWordOut_FoilExpInBtmTarget() { m_bWordOut_FoilExpInBtmTarget = false; return m_WordOut_FoilExpInBtmTarget; }
	void setWordOut_FoilExpInBtmTarget(WORD WordOut_FoilExpInBtmTarget) { m_WordOut_FoilExpInBtmTarget = WordOut_FoilExpInBtmTarget;  m_bWordOut_FoilExpInBtmTarget = true; }
	BOOL isWordOut_FoilExpInBtmTarget() { return m_bWordOut_FoilExpInBtmTarget; }

	WORD getWordOut_FoilExpOutTopTarget() { m_bWordOut_FoilExpOutTopTarget = false; return m_WordOut_FoilExpOutTopTarget; }
	void setWordOut_FoilExpOutTopTarget(WORD WordOut_FoilExpOutTopTarget) { m_WordOut_FoilExpOutTopTarget = WordOut_FoilExpOutTopTarget;  m_bWordOut_FoilExpOutTopTarget = true; }
	BOOL isWordOut_FoilExpOutTopTarget() { return m_bWordOut_FoilExpOutTopTarget; }

	WORD getWordOut_FoilExpOutBtmTarget() { m_bWordOut_FoilExpOutBtmTarget = false; return m_WordOut_FoilExpOutBtmTarget; }
	void setWordOut_FoilExpOutBtmTarget(WORD WordOut_FoilExpOutBtmTarget) { m_WordOut_FoilExpOutBtmTarget = WordOut_FoilExpOutBtmTarget;  m_bWordOut_FoilExpOutBtmTarget = true; }
	BOOL isWordOut_FoilExpOutBtmTarget() { return m_bWordOut_FoilExpOutBtmTarget; }

	WORD getWordOut_FoilExpBothTopTarget() { m_bWordOut_FoilExpBothTopTarget = false; return m_WordOut_FoilExpBothTopTarget; }
	void setWordOut_FoilExpBothTopTarget(WORD WordOut_FoilExpBothTopTarget) { m_WordOut_FoilExpBothTopTarget = WordOut_FoilExpBothTopTarget;  m_bWordOut_FoilExpBothTopTarget = true; }
	BOOL isWordOut_FoilExpBothTopTarget() { return m_bWordOut_FoilExpBothTopTarget; }

	WORD getWordOut_FoilExpBothBtmTarget() { m_bWordOut_FoilExpBothBtmTarget = false; return m_WordOut_FoilExpBothBtmTarget; }
	void setWordOut_FoilExpBothBtmTarget(WORD WordOut_FoilExpBothBtmTarget) { m_WordOut_FoilExpBothBtmTarget = WordOut_FoilExpBothBtmTarget;  m_bWordOut_FoilExpBothBtmTarget = true; }
	BOOL isWordOut_FoilExpBothBtmTarget() { return m_bWordOut_FoilExpBothBtmTarget; }

	WORD getWordOut_SpeterTopTarget() { m_bWordOut_SpeterTopTarget = false; return m_WordOut_SpeterTopTarget; }
	void setWordOut_SpeterTopTarget(WORD WordOut_SpeterTopTarget) { m_WordOut_SpeterTopTarget = WordOut_SpeterTopTarget;  m_bWordOut_SpeterTopTarget = true; }
	BOOL isWordOut_SpeterTopTarget() { return m_bWordOut_SpeterTopTarget; }

	WORD getWordOut_SpeterBtmTarget() { m_bWordOut_SpeterBtmTarget = false; return m_WordOut_SpeterBtmTarget; }
	void setWordOut_SpeterBtmTarget(WORD WordOut_SpeterBtmTarget) { m_WordOut_SpeterBtmTarget = WordOut_SpeterBtmTarget;  m_bWordOut_SpeterBtmTarget = true; }
	BOOL isWordOut_SpeterBtmTarget() { return m_bWordOut_SpeterBtmTarget; }

	WORD getWordOut_PrmContinuousCnt() { m_bWordOut_PrmContinuousCnt = false; return m_WordOut_PrmContinuousCnt; }
	void setWordOut_PrmContinuousCnt(WORD WordOut_PrmContinuousCnt) { m_WordOut_PrmContinuousCnt = WordOut_PrmContinuousCnt;  m_bWordOut_PrmContinuousCnt = true; }
	BOOL isWordOut_PrmContinuousCnt() { return m_bWordOut_PrmContinuousCnt; }

	WORD getWordOut_PrmSectorNgTabCnt() { m_bWordOut_PrmSectorNgTabCnt = false; return m_WordOut_PrmSectorNgTabCnt; }
	void setWordOut_PrmSectorNgTabCnt(WORD WordOut_PrmSectorNgTabCnt) { m_WordOut_PrmSectorNgTabCnt = WordOut_PrmSectorNgTabCnt;  m_bWordOut_PrmSectorNgTabCnt = true; }
	BOOL isWordOut_PrmSectorNgTabCnt() { return m_bWordOut_PrmSectorNgTabCnt; }

	WORD getWordOut_PrmSectorBaseCnt() { m_bWordOut_PrmSectorBaseCnt = false; return m_WordOut_PrmSectorBaseCnt; }
	void setWordOut_PrmSectorBaseCnt(WORD WordOut_PrmSectorBaseCnt) { m_WordOut_PrmSectorBaseCnt = WordOut_PrmSectorBaseCnt;  m_bWordOut_PrmSectorBaseCnt = true; }
	BOOL isWordOut_PrmSectorBaseCnt() { return m_bWordOut_PrmSectorBaseCnt; }

	WORD getWordOut_AlarmExist() { m_bWordOut_AlarmExist = false; return m_WordOut_AlarmExist; }
	void setWordOut_AlarmExist(WORD WordOut_AlarmExist) { m_WordOut_AlarmExist = WordOut_AlarmExist;  m_bWordOut_AlarmExist = true; }
	BOOL isWordOut_AlarmExist() { return m_bWordOut_AlarmExist; }

	WORD getWordOut_AlarmCode_Buffer(int num) { m_bWordOut_AlarmCode_Buffer = false; return m_WordOut_AlarmCode_Buffer[num]; }
	void setWordOut_AlarmCode_Buffer(int num, WORD WordOut_AlarmCode_Buffer) { m_WordOut_AlarmCode_Buffer[num] = WordOut_AlarmCode_Buffer;  m_bWordOut_AlarmCode_Buffer = true; }
	BOOL isWordOut_AlarmCode_Buffer() { return m_bWordOut_AlarmCode_Buffer; }


	WORD getWordOut_Cell_Trigger_ID() { m_bWordOut_Cell_Trigger_ID = false; return m_WordOut_Cell_Trigger_ID; }
	void setWordOut_Cell_Trigger_ID(WORD WordOut_Cell_Trigger_ID) { m_WordOut_Cell_Trigger_ID = WordOut_Cell_Trigger_ID;  m_bWordOut_Cell_Trigger_ID = true; }
	BOOL isWordOut_Cell_Trigger_ID() { return m_bWordOut_Cell_Trigger_ID; }

	WORD getWordOut_Judge() { m_bWordOut_Judge = false; return m_WordOut_Judge; }
	void setWordOut_Judge(WORD WordOut_Judge) { m_WordOut_Judge = WordOut_Judge;  m_bWordOut_Judge = true; }
	BOOL isWordOut_Judge() { return m_bWordOut_Judge; }

	WORD getWordOut_NG_Code() { m_bWordOut_NG_Code = false; return m_WordOut_NG_Code; }
	void setWordOut_NG_Code(WORD WordOut_NG_Code) { m_WordOut_NG_Code = WordOut_NG_Code;  m_bWordOut_NG_Code = true; }
	BOOL isWordOut_NG_Code() { return m_bWordOut_NG_Code; }

	WORD getWordOut_DuplicateNG_Cell_ID(int num) { m_bWordOut_DuplicateNG_Cell_ID = false; return m_WordOut_DuplicateNG_Cell_ID[num]; }
	void setWordOut_DuplicateNG_Cell_ID(int num, WORD WordOut_DuplicateNG_Cell_ID) { m_WordOut_DuplicateNG_Cell_ID[num] = WordOut_DuplicateNG_Cell_ID;  m_bWordOut_DuplicateNG_Cell_ID = true; }
	BOOL isWordOut_DuplicateNG_Cell_ID() { return m_bWordOut_DuplicateNG_Cell_ID; }

};

