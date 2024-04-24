#include "pch.h"
#include "DataPlcImp.h"

CDataPlcImp::CDataPlcImp()
{
	//In Data
	//Bit Data In
	m_BitIn_Alive = 0;
	m_BitIn_Ready = 0;
	m_BitIn_Run = 0;
	m_BitIn_EncoderReset = 0;
	m_BitIn_TabZeroReset = 0;
	m_BitIn_InkMarkingActive = 0;
	m_BitIn_ConnectZone = 0;
	m_BitIn_RecipeChange = 0;
	m_BitIn_LotStartReq = 0;
	m_BitIn_LotEndReq = 0;
	m_BitIn_AlarmResetReq = 0;
	m_BitIn_AlarmNgAck = 0;

	//Word Data In
	m_WordIn_RecipeNo = 0;
	m_WordIn_RecipeName = _T("");
	m_WordIn_CELL_ID = _T("");

	m_WordIn_FoilExpInTopTarget = 0;
	m_WordIn_FoilExpInBtmTarget = 0;
	m_WordIn_FoilExpOutTopTarget = 0;
	m_WordIn_FoilExpOutBtmTarget = 0;
	m_WordIn_FoilExpBothTopTarget = 0;
	m_WordIn_FoilExpBothBtmTarget = 0;
	m_WordIn_SpeterTopTarget = 0;
	m_WordIn_SpeterBtmTarget = 0;

	m_WordIn_PrmContinuousCnt = 0;
	m_WordIn_PrmSectorNgTabCnt = 0;
	m_WordIn_PrmSectorBaseCnt = 0;

	m_WordIn_AlarmExistAck = 0;

	//Bit Data Out
	m_BitOut_Alive = 0;
	m_BitOut_Ready = 0;
	m_BitOut_EncoderSet = 0;
	m_BitOut_RecipeChangeAck = 0;
	m_BitOut_LotStartReqAck = 0;
	m_BitOut_LotEndReqAck = 0;
	m_BitOut_TabZeroReset = 0;
	m_BitOut_AlarmResetAck = 0;
	m_BitOut_AlarmNgResetAck = 0;

	m_BitOut_DiskSpaceWarning = 0;
	m_BitOut_DiskSpaceAlarm = 0;

	//Word Data In Cell Key
	for (int i = 0; i < COUNT_CELLKEY; i++)
	{
		m_WordIn_CellKey[i] = 0;
	}

	//Word Data Out
	m_WordOut_DataReportV1_Ea = 0;
	m_WordOut_DataReportV2_OK = 0;
	m_WordOut_DataReportV3_NG = 0;
	m_WordOut_DataReportV4_OkRate = 0;
	m_WordOut_DataReportV5_NgRate = 0;
	m_WordOut_DataReportV6_RunRate = 0;
	m_WordOut_Continue_Alarm_Cnt = 0;
	m_WordOut_Heavy_Alarm_Cnt = 0;

	m_WordOut_FoilExpInTop_Alarm_Cnt = 0;
	m_WordOut_FoilExpInBtm_Alarm_Cnt = 0;
	m_WordOut_FoilExpOutTop_Alarm_Cnt = 0;
	m_WordOut_FoilExpOutBtm_Alarm_Cnt = 0;
	m_WordOut_FoilExpBothTop_Alarm_Cnt = 0;
	m_WordOut_FoilExpBothBtm_Alarm_Cnt = 0;
	m_WordOut_SpeterTop_Alarm_Cnt = 0;
	m_WordOut_SpeterBtm_Alarm_Cnt = 0;

	m_WordOut_Top_Defect_Count_Real = 0;
	m_WordOut_Btm_Defect_Count_Real = 0;
	m_WordOut_Top_Defect_Count_LotEnd = 0;
	m_WordOut_Btm_Defect_Count_LotEnd = 0;


	m_WordOut_FoilExpInTopTarget = 0;
	m_WordOut_FoilExpInBtmTarget = 0;
	m_WordOut_FoilExpOutTopTarget = 0;
	m_WordOut_FoilExpOutBtmTarget = 0;
	m_WordOut_FoilExpBothTopTarget = 0;
	m_WordOut_FoilExpBothBtmTarget = 0;
	m_WordOut_SpeterTopTarget = 0;
	m_WordOut_SpeterBtmTarget = 0;

	m_WordOut_PrmContinuousCnt = 0;
	m_WordOut_PrmSectorNgTabCnt = 0;
	m_WordOut_PrmSectorBaseCnt = 0;

	m_WordOut_AlarmExist = 0;

	for (int i = 0; i < COUNT_ALRAMBUFF; i++)
	{
		m_WordOut_AlarmCode_Buffer[i] = 0;
	}

	m_WordOut_Cell_Trigger_ID = 0;
	m_WordOut_Judge = 0;
	m_WordOut_NG_Code = 0;

	for (int i = 0; i < COUNT_DUPLICATENGCELLID; i++)
	{
		m_WordOut_DuplicateNG_Cell_ID[i] = 0;
	}


	//Word Data Out
	m_bWordOut_DataReportV1_Ea = FALSE;
	m_bWordOut_DataReportV2_OK = FALSE;
	m_bWordOut_DataReportV3_NG = FALSE;
	m_bWordOut_DataReportV4_OkRate = FALSE;
	m_bWordOut_DataReportV5_NgRate = FALSE;
	m_bWordOut_DataReportV6_RunRate = FALSE;
	m_bWordOut_Continue_Alarm_Cnt = FALSE;
	m_bWordOut_Heavy_Alarm_Cnt = FALSE;

	 m_bWordOut_FoilExpInTop_Alarm_Cnt = FALSE;
	m_bWordOut_FoilExpInBtm_Alarm_Cnt = FALSE;
	m_bWordOut_FoilExpOutTop_Alarm_Cnt = FALSE;
	m_bWordOut_FoilExpOutBtm_Alarm_Cnt = FALSE;
	m_bWordOut_FoilExpBothTop_Alarm_Cnt = FALSE;
	m_bWordOut_FoilExpBothBtm_Alarm_Cnt = FALSE;
	m_bWordOut_SpeterTop_Alarm_Cnt = FALSE;
	m_bWordOut_SpeterBtm_Alarm_Cnt = FALSE;

	m_bWordOut_Top_Defect_Count_Real = FALSE;
	m_bWordOut_Btm_Defect_Count_Real = FALSE;
	m_bWordOut_Top_Defect_Count_LotEnd = FALSE;
	m_bWordOut_Btm_Defect_Count_LotEnd = FALSE;

	m_bWordOut_FoilExpInTopTarget = FALSE;
	m_bWordOut_FoilExpInBtmTarget = FALSE;
	m_bWordOut_FoilExpOutTopTarget = FALSE;
	m_bWordOut_FoilExpOutBtmTarget = FALSE;
	m_bWordOut_FoilExpBothTopTarget = FALSE;
	m_bWordOut_FoilExpBothBtmTarget = FALSE;
	m_bWordOut_SpeterTopTarget = FALSE;
	m_bWordOut_SpeterBtmTarget = FALSE;

	m_bWordOut_PrmContinuousCnt = FALSE;
	m_bWordOut_PrmSectorNgTabCnt = FALSE;
	m_bWordOut_PrmSectorBaseCnt = FALSE;

	m_bWordOut_AlarmExist = FALSE;
	m_bWordOut_AlarmCode_Buffer = FALSE;

	m_bWordOut_Cell_Trigger_ID = FALSE;
	m_bWordOut_Judge = FALSE;
	m_bWordOut_NG_Code = FALSE;

	m_bWordOut_DuplicateNG_Cell_ID = FALSE;

}

CDataPlcImp::~CDataPlcImp()
{
	
}

