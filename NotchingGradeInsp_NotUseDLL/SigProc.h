#pragma once

#include "PioCtrl.h"
#include "GlobalData.h" // 22.08.18 Ahn Add 
// 22.07.25 Ahn Modify Start
#define	MAX_ADR_BIT_IN	( 32 )
#define	MAX_ADR_BIT_OUT	( 32 )
// 22.07.25 Ahn Modify End


class CSequenceData ;

class CSigProc
{
public:
	// Melsec Address Start
	enum{
		enBitIn_Alive			= 0x00000001,	// B63C0
		enBitIn_Ready			= 0x00000002,	// B63C1
		enBitIn_Run				= 0x00000004,	// B63C2
		enBitIn_Dummy4			= 0x00000008,	// B63C3
		enBitIn_Dummy5			= 0x00000010,	// B63C4
		enBitIn_EncoderReset	= 0x00000020,	// B63C5
		enBitIn_TabZeroReset	= 0x00000040,	// B63C6
		enBitIn_Dummy8			= 0x00000080,	// B63C7

		enBitIn_Dummy9			= 0x00000101,	// B63C8	
		enBitIn_InkMarkingActive= 0x00000102,	// B63C9
		enBitIn_ConnectZone		= 0x00000104,	// B63CA  // 22.01.12 Ahn 연결구간 비검사구간. 
		enBitIn_Dummy12			= 0x00000108,	// B63CB
		enBitIn_Dummy13			= 0x00000110,	// B63CC
		enBitIn_Dummy14			= 0x00000120,	// B63CD
		enBitIn_Dummy15			= 0x00000140,	// B63CE
		enBitIn_Dummy16			= 0x00000180,	// B63CF

		enBitIn_Dummy17			= 0x00000201,	// B63D0
		enBitIn_Dummy18			= 0x00000202,	// B63D1
		enBitIn_RecipeChange	= 0x00000204,	// B63D2
		enBitIn_LotStartReq		= 0x00000208,	// B63D3
		enBitIn_LotEndReq		= 0x00000210,	// B63D4

		enBitIn_AlarmResetReq	= 0x00000301,	// B63D8 
	};
	enum {
		enBitOut_Alive			= 0x00000001,	// B6B40
		enBitOut_Ready			= 0x00000002,	// B6B41
		enBitOut_Dummy3			= 0x00000004,	// B6B42
		enBitOut_Dummy4			= 0x00000008,	// B6B43
		enBitOut_Dummy5			= 0x00000010,	// B6B44
		enBitOut_EncoderSet		= 0x00000020,	// B6B45
		enBitOut_TabZeroReset	= 0x00000040,	// B6B46
		enBitOut_Dummy8			= 0x00000080,	// B6B47
		enBitOut_DiskSpaceWarning= 0x00000101,	// B6B48
		enBitOut_DiskSpaceAlarm	= 0x00000102,	// B6B49
		enBitOut_Dummy11		= 0x00000104,	// B6B4A
		enBitOut_Dummy12		= 0x00000108,	// B6B4B
		enBitOut_Dummy13		= 0x00000110,	// B6B4C
		enBitOut_Dummy14		= 0x00000120,	// B6B4D
		enBitOut_Dummy15		= 0x00000140,	// B6B4E
		enBitOut_Dummy16		= 0x00000180,	// B6B4F
		enBitOut_Dummy17		= 0x00000201,	// B6B50
		enBitOut_Dummy18		= 0x00000202,	// B6B51
		enBitOut_RecipeChangeAck= 0x00000204,	// B6B52
		enBitOut_LotStartReqAck	= 0x00000208,	// B6B53
		enBitOut_LotEndReqAck	= 0x00000210,	// B6B54
		enBitOut_AlramResetAck	= 0x00000301,	// B6B55
	};

	enum {
		// 23.02.17 Son Mod Start
		//enWordRead_RecipeNo				= 0x9680,		
		//enWordRead_RecipeNo_Len			= 2,				//	2
		//enWordRead_RecipeName			= 0x9682,
		//enWordRead_RecipeName_Len		= 8,				// 10
		//enWordRead_CELL_ID				= 0x9690,
		//enWordRead_CELL_ID_Len			= 10,				// 30
		//enWordRead_Speed				= 0x96B0,
		//enWordRead_Speed_Len			= 2,				// 32
		//enWordRead_DrossTopTarget		= 0x96B2,
		//enWordRead_DrossTopTarget_Len	= 2,				// 34
		//enWordRead_DrossBtmTarget		= 0x96B4,
		//enWordRead_DrossBtmTarget_Len	= 2,				// 36
		//enWordRead_FoilExpTopTarget		= 0x96B6,
		//enWordRead_FoilExpTopTarget_Len	= 2,				// 38
		//enWordRead_FoilExpBtmTarget		= 0x96B8,
		//enWordRead_FoilExpBtmTarget_Len = 2,				// 40
		//enWordRead_SpeterTopTarget		= 0x96BA,
		//enWordRead_SpeterTopTarget_Len	= 2,				// 42
		//enWordRead_SpeterBtmTarget		= 0x96BC,
		//enWordRead_SpeterBtmTarget_Len	= 2,				// 44
		//enWordRead_LotInfoLength		= 44,
		//en_WordRead_LotInfo_Length		= 60 ,				
		//enWordRead_AlarmExistAck		= 0x96D0,	
		//enWordRead_AlarmExistAck_Len	= 2,				// 46
		//// 22.08.10 Ahn Add Start
		//enWordRead_PrmContinuousCnt		= 0x96c0, 
		//enWordRead_PrmContinuousCnt_Len	= 2,
		//enWordRead_PrmSectorNgTabCnt	= 0x96c2,
		//enWordRead_PrmSectorNgTabCnt_Len= 2,
		//enWordRead_PrmSectorBaseCnt		= 0x96c4,
		//enWordRead_PrmSectorBaseCnt_Len	= 2,
		//// 22.08.10 Ahn Add End
		//enWordReadMaxSize 				= 100,
		enWordRead_RecipeNo = 0x0000,
		enWordRead_RecipeNo_Len = 2,				//	2
		enWordRead_RecipeName = 0x0002,
		enWordRead_RecipeName_Len = 8,				// 10
		enWordRead_CELL_ID = 0x0010,
		enWordRead_CELL_ID_Len = 20,				// 30
		enWordRead_Speed = 0x0030,
		enWordRead_Speed_Len = 2,				// 32
		enWordRead_DrossTopTarget = 0x0032,
		enWordRead_DrossTopTarget_Len = 2,				// 34
		enWordRead_DrossBtmTarget = 0x0034,
		enWordRead_DrossBtmTarget_Len = 2,				// 36
		enWordRead_FoilExpTopTarget = 0x0036,
		enWordRead_FoilExpTopTarget_Len = 2,				// 38
		enWordRead_FoilExpBtmTarget = 0x0038,
		enWordRead_FoilExpBtmTarget_Len = 2,				// 40
		enWordRead_SpeterTopTarget = 0x003A,
		enWordRead_SpeterTopTarget_Len = 2,				// 42
		enWordRead_SpeterBtmTarget = 0x003C,
		enWordRead_SpeterBtmTarget_Len = 2,				// 44
		enWordRead_LotInfoLength = 44,
		// en_WordRead_LotInfo_Length = 60,				// 23.03.03 Ahn Delete 
		// 22.08.10 Ahn Add Start
		enWordRead_PrmContinuousCnt = 0x0040,
		enWordRead_PrmContinuousCnt_Len = 2,
		enWordRead_PrmSectorNgTabCnt = 0x0042,
		enWordRead_PrmSectorNgTabCnt_Len = 2,
		enWordRead_PrmSectorBaseCnt = 0x0044,
		enWordRead_PrmSectorBaseCnt_Len = 2,
		// 22.08.10 Ahn Add End
		enWordRead_AlarmExistAck = 0x0050,
		enWordRead_AlarmExistAck_Len = 2,				// 46
		enWordReadMaxSize = 60,
		// 23.02.17 Son Mod End

	};

	enum {
		// 23.02.17 Son Mod Start
		//enWordWrite_DataReportV1_Ea = 0xC380,
		//enWordWrite_DataReportV1_Len = 2,
		//enWordWrite_DataReportV2_OK = 0xC382,
		//enWordWrite_DataReportV2_Len = 2,
		//enWordWrite_DataReportV3_NG = 0xC384,
		//enWordWrite_DataReportV3_Len = 2,
		//enWordWrite_DataReportV4_OkRate = 0xC386,
		//enWordWrite_DataReportV4_Len = 2,
		//enWordWrite_DataReportV5_NgRate = 0xC388,
		//enWordWrite_DataReportV5_Len = 2,
		//enWordWrite_DataReportV6_RunRate = 0xC38A,
		//enWordWrite_DataReportV6_Len = 2,
		//enWordWrite_Continue_Alarm_Cnt = 0xC38C,
		//enWordWrite_Continue_Alarm_Cnt_Len = 2,
		//enWordWrite_Heavy_Alarm_Cnt = 0xC38E,
		//enWordWrite_Heavy_Alarm_Cnt_Len = 2,
		//enWordWrite_DrossTop_Alarm_Cnt = 0xC390,
		//enWordWrite_DrossTop_Alarm_Cnt_Len = 2,
		//enWordWrite_DrossBtm_Alarm_Cnt = 0xC392,
		//enWordWrite_DrossBtm_Alarm_Cnt_Len = 2,
		//enWordWrite_FoilExpTop_Alarm_Cnt = 0xC394,
		//enWordWrite_FoilExpTop_Alarm_Cnt_Len = 2,
		//enWordWrite_FoilExpBtm_Alarm_Cnt = 0xC396,
		//enWordWrite_FoilExpBtm_Alarm_Cnt_Len = 2,
		//enWordWrite_SpeterTop_Alarm_Cnt = 0xC398,
		//enWordWrite_SpeterTop_Alarm_Cnt_Len = 2,
		//enWordWrite_SpeterBtm_Alarm_Cnt = 0xC39A,
		//enWordWrite_SpeterBtm_Alarm_Cnt_Len = 2,
		//// 22.07.05 Ahn Add Start
		//enWordWrite_Top_Defect_Count_Real = 0xC39C,
		//enWordWrite_Top_Defect_Count_Real_Len = 2,
		//enWordWrite_Btm_Defect_Count_Real = 0xC39E,
		//enWordWrite_Btm_Defect_Count_Real_Len = 2,
		//enWordWrite_Top_Defect_Count_LotEnd = 0xC3A0,
		//enWordWrite_Top_Defect_Count_LotEnd_Len = 2,
		//enWordWrite_Btm_Defect_Count_LotEnd = 0xC3A2,
		//enWordWrite_Btm_Defect_Count_LotEnd_Len = 2,
		//// 22.07.05 Ahn Add End

		//enWordWrite_DrossTopTarget = 0xC3B2,
		//enWordWrite_DrossTopTarget_Len = 2,
		//enWordWrite_DrossBtmTarget = 0xC3B4,
		//enWordWrite_DrossBtmTarget_Len = 2,
		//enWordWrite_FoilExpTopTarget = 0xC3B6,
		//enWordWrite_FoilExpTopTarget_Len = 2,
		//enWordWrite_FoilExpBtmTarget = 0xC3B8,
		//enWordWrite_FoilExpBtmTarget_Len = 2,
		//enWordWrite_SpeterTopTarget = 0xC3BA,
		//enWordWrite_SpeterTopTarget_Len = 2,
		//enWordWrite_SpeterBtmTarget = 0xC3BC,
		//enWordWrite_SpeterBtmTarget_Len = 2,

		//enWordWrite_AlarmExist = 0xC3D0,
		//enWordWrite_AlarmExist_Len = 2,
		//enWordWrite_AlarmCode_Buffer1 = 0xC3D2,
		//enWordWrite_AlarmCode_Buffer1_Len = 2,
		//enWordWrite_AlarmCode_Buffer2 = 0xC3D4,
		//enWordWrite_AlarmCode_Buffer2_Len = 2,
		//enWordWrite_AlarmCode_Buffer3 = 0xC3D6,
		//enWordWrite_AlarmCode_Buffer3_Len = 2,
		//enWordWrite_AlarmCode_Buffer4 = 0xC3D8,
		//enWordWrite_AlarmCode_Buffer4_Len = 2,
		//enWordWrite_AlarmCode_Buffer5 = 0xC3DA,
		//enWordWrite_AlarmCode_Buffer5_Len = 2,

		//// 22.12.12 Ahn Add Start - Judge Report
		//en_WordWrite_Cell_Trigger_ID		= 0xC3F0, 
		//en_WordWrite_Cell_Trigger_ID_Len	= 2,
		//en_WordWrite_Judge					= 0xC3F2,
		//en_WordWrite_Judge_Len				= 2,
		//en_WordWrite_NG_Code				= 0xC3F4,
		//en_WordWrite_NG_Code_Len			= 2,
		//// 22.12.12 Ahn Add End

		//enWordWroteMaxSize = 100, 
		enWordWrite_DataReportV1_Ea = 0x0000,
		enWordWrite_DataReportV1_Len = 2,
		enWordWrite_DataReportV2_OK = 0x0002,
		enWordWrite_DataReportV2_Len = 2,
		enWordWrite_DataReportV3_NG = 0x0004,
		enWordWrite_DataReportV3_Len = 2,
		enWordWrite_DataReportV4_OkRate = 0x0006,
		enWordWrite_DataReportV4_Len = 2,
		enWordWrite_DataReportV5_NgRate = 0x0008,
		enWordWrite_DataReportV5_Len = 2,
		enWordWrite_DataReportV6_RunRate = 0x000A,
		enWordWrite_DataReportV6_Len = 2,
		enWordWrite_Continue_Alarm_Cnt = 0x000C,
		enWordWrite_Continue_Alarm_Cnt_Len = 2,
		enWordWrite_Heavy_Alarm_Cnt = 0x000E,
		enWordWrite_Heavy_Alarm_Cnt_Len = 2,
		enWordWrite_DrossTop_Alarm_Cnt = 0x0010,
		enWordWrite_DrossTop_Alarm_Cnt_Len = 2,
		enWordWrite_DrossBtm_Alarm_Cnt = 0x0012,
		enWordWrite_DrossBtm_Alarm_Cnt_Len = 2,
		enWordWrite_FoilExpTop_Alarm_Cnt = 0x0014,
		enWordWrite_FoilExpTop_Alarm_Cnt_Len = 2,
		enWordWrite_FoilExpBtm_Alarm_Cnt = 0x0016,
		enWordWrite_FoilExpBtm_Alarm_Cnt_Len = 2,
		enWordWrite_SpeterTop_Alarm_Cnt = 0x0018,
		enWordWrite_SpeterTop_Alarm_Cnt_Len = 2,
		enWordWrite_SpeterBtm_Alarm_Cnt = 0x001A,
		enWordWrite_SpeterBtm_Alarm_Cnt_Len = 2,
		// 22.07.05 Ahn Add Start
		enWordWrite_Top_Defect_Count_Real = 0x001C,
		enWordWrite_Top_Defect_Count_Real_Len = 2,
		enWordWrite_Btm_Defect_Count_Real = 0x001E,
		enWordWrite_Btm_Defect_Count_Real_Len = 2,
		enWordWrite_Top_Defect_Count_LotEnd = 0x0020,
		enWordWrite_Top_Defect_Count_LotEnd_Len = 2,
		enWordWrite_Btm_Defect_Count_LotEnd = 0x0022,
		enWordWrite_Btm_Defect_Count_LotEnd_Len = 2,
		// 22.07.05 Ahn Add End

		enWordWrite_DrossTopTarget = 0x0032,
		enWordWrite_DrossTopTarget_Len = 2,
		enWordWrite_DrossBtmTarget = 0x0034,
		enWordWrite_DrossBtmTarget_Len = 2,
		enWordWrite_FoilExpTopTarget = 0x0036,
		enWordWrite_FoilExpTopTarget_Len = 2,
		enWordWrite_FoilExpBtmTarget = 0x0038,
		enWordWrite_FoilExpBtmTarget_Len = 2,
		enWordWrite_SpeterTopTarget = 0x003A,
		enWordWrite_SpeterTopTarget_Len = 2,
		enWordWrite_SpeterBtmTarget = 0x003C,
		enWordWrite_SpeterBtmTarget_Len = 2,

		enWordWrite_AlarmExist = 0x0050,
		enWordWrite_AlarmExist_Len = 2,
		enWordWrite_AlarmCode_Buffer1 = 0x0052,
		enWordWrite_AlarmCode_Buffer1_Len = 2,
		enWordWrite_AlarmCode_Buffer2 = 0x0054,
		enWordWrite_AlarmCode_Buffer2_Len = 2,
		enWordWrite_AlarmCode_Buffer3 = 0x0056,
		enWordWrite_AlarmCode_Buffer3_Len = 2,
		enWordWrite_AlarmCode_Buffer4 = 0x0058,
		enWordWrite_AlarmCode_Buffer4_Len = 2,
		enWordWrite_AlarmCode_Buffer5 = 0x005A,
		enWordWrite_AlarmCode_Buffer5_Len = 2,

		// 22.12.12 Ahn Add Start - Judge Report
		en_WordWrite_Cell_Trigger_ID = 0x0070,
		en_WordWrite_Cell_Trigger_ID_Len = 2,
		en_WordWrite_Judge = 0x0072,
		en_WordWrite_Judge_Len = 2,
		en_WordWrite_NG_Code = 0x0074,
		en_WordWrite_NG_Code_Len = 2,
		// 22.12.12 Ahn Add End

		enWordWroteMaxSize = 100,
		// 23.02.17 Son Mod End
	};

	enum { // Alarm Code
		en_Alarm_ContinueNg		= 0x0001,
		en_Alarm_Heavy			= 0x0002,
		en_Alarm_SectorNg		= 0x0004,
		en_Aalrm_Dross_Top		= 0x0008,
		en_Alarm_Dross_Btm		= 0x0010,
		en_Alarm_FoilExp_Top	= 0x0020,
		en_Alarm_FoilExp_Btm	= 0x0040,
		en_Alarm_Spatter_Top	= 0x0080,
		en_Alarm_Spatter_Btm	= 0x0100,
	};
	//Wxxx.0 연속 불량 알람
	//Wxxx.1 심각 불량 알람
	//Wxxx.2 구간 불량 알람
	//Wxxx.3 Dross Top - 불량 알람
	//Wxxx.4 Dross Bot - 불량 알람
	//Wxxx.5 Foil exposure Top - 불량 알람
	//Wxxx.6 Foil exposure Bot - 불량 알람
	//Wxxx.7 Spatter Top - 불량 알람
	//Wxxx.8 Spatter Bot - 불량 알람

	// Melsec Address End





	// 23.03.02 Ahn Add Start
	//////////////////////////////////////////////////////////////////////////
	// Siemens Address Start
	enum {
		enSmsBitIn_Alive				= 0,	//
		enSmsBitIn_Ready				= 1,	//
		enSmsBitIn_Run					= 2,	//
		enSmsBitIn_EncoderReset			= 3,	//
		enSmsBitIn_TabZeroReset			= 4,	//
		enSmsBitIn_InkMarkingActive		= 5,	//
		enSmsBitIn_ConnectZone			= 6,	// 
		enSmsBitIn_RecipeChange			= 7,	// 
		enSmsBitIn_LotStartReq			= 8,	// 
		enSmsBitIn_LotEndReq			= 9,	// 
		enSmsBitIn_AlarmResetReq		= 10,	//  
	};
	enum {
		enSmsBitOut_Alive				= 0,	// 
		enSmsBitOut_Ready				= 1,	// 
		enSmsBitOut_EncoderSet			= 2,	// 
		enSmsBitOut_RecipeChangeAck		= 3,	// 
		enSmsBitOut_LotStartReqAck		= 4,	// 
		enSmsBitOut_LotEndReqAck		= 5,	// 
		enSmsBitOut_TabZeroReset		= 6,	// 
		enSmsBitOut_AlarmResetAck		= 7,	// 
		enSmsBitOut_DiskSpaceWarning	= 8,	// Address Map에 없음
		enSmsBitOut_DiskSpaceAlarm		= 9,	// Address Map에 없음
	};
	// Siemens Address End
	// Siemens Word Address Start
	enum{
		enSmsWordRead_RecipeNo					= 0,		enSmsWordRead_RecipeNo_Len				= 1,				//	1
		enSmsWordRead_RecipeName				= 1,		enSmsWordRead_RecipeName_Len			= 4,				//  5
		enSmsWordRead_CELL_ID					= 5,		enSmsWordRead_CELL_ID_Len				= 10,				// 15
		enSmsWordRead_DrossTopTarget			= 15,		enSmsWordRead_DrossTopTarget_Len		= 1,				// 16
		enSmsWordRead_DrossBtmTarget			= 16,		enSmsWordRead_DrossBtmTarget_Len		= 1,				// 17
		enSmsWordRead_FoilExpTopTarget			= 17,		enSmsWordRead_FoilExpTopTarget_Len		= 1,				// 18
		enSmsWordRead_FoilExpBtmTarget			= 18,		enSmsWordRead_FoilExpBtmTarget_Len		= 1,				// 19
		enSmsWordRead_SpeterTopTarget			= 19,		enSmsWordRead_SpeterTopTarget_Len		= 1,				// 20
		enSmsWordRead_SpeterBtmTarget			= 20,		enSmsWordRead_SpeterBtmTarget_Len		= 1,				// 21
		enSmsWordRead_LotInfoLength				= 21,
		enSmsWordRead_AlarmExistAck				= 21,		enSmsWordRead_AlarmExistAck_Len			= 1,				// 22
		enSmsWordRead_PrmContinuousCnt			= 22,		enSmsWordRead_PrmContinuousCnt_Len		= 1,
		enSmsWordRead_PrmSectorNgTabCnt			= 23,		enSmsWordRead_PrmSectorNgTabCnt_Len		= 1,
		enSmsWordRead_PrmSectorBaseCnt			= 24,		enSmsWordRead_PrmSectorBaseCnt_Len		= 1,
		enSmsWordRead_ConnectZoneTabCount		= 25,		enSmsWordRead_ConnectZoneTabCount_Len		= 1,
		enSmsWordReadMaxSize					= 26,
	};

	enum {
		enSmsWordWrite_DataReportV1_Ea			= 0,		enSmsWordWrite_DataReportV1_Len			= 1,
		enSmsWordWrite_DataReportV2_OK			= 1,		enSmsWordWrite_DataReportV2_Len			= 1,
		enSmsWordWrite_DataReportV3_NG			= 2,		enSmsWordWrite_DataReportV3_Len			= 1,
		enSmsWordWrite_DataReportV4_OkRate		= 3,		enSmsWordWrite_DataReportV4_Len			= 1,
		enSmsWordWrite_DataReportV5_NgRate		= 4,		enSmsWordWrite_DataReportV5_Len			= 1,
		enSmsWordWrite_DataReportV6_RunRate		= 5,		enSmsWordWrite_DataReportV6_Len			= 1,
		enSmsWordWrite_Continue_Alarm_Cnt		= 6,		enSmsWordWrite_Continue_Alarm_Cnt_Len	= 1,
		enSmsWordWrite_Heavy_Alarm_Cnt			= 7,		enSmsWordWrite_Heavy_Alarm_Cnt_Len		= 1,
		enSmsWordWrite_DrossTop_Alarm_Cnt		= 8,		enSmsWordWrite_DrossTop_Alarm_Cnt_Len	= 1,
		enSmsWordWrite_DrossBtm_Alarm_Cnt		= 9,		enSmsWordWrite_DrossBtm_Alarm_Cnt_Len	= 1,
		enSmsWordWrite_FoilExpTop_Alarm_Cnt		= 10,		enSmsWordWrite_FoilExpTop_Alarm_Cnt_Len = 2,
		enSmsWordWrite_FoilExpBtm_Alarm_Cnt		= 11,		enSmsWordWrite_FoilExpBtm_Alarm_Cnt_Len = 2,
		enSmsWordWrite_SpeterTop_Alarm_Cnt		= 12,		enSmsWordWrite_SpeterTop_Alarm_Cnt_Len	= 2,
		enSmsWordWrite_SpeterBtm_Alarm_Cnt		= 13,		enSmsWordWrite_SpeterBtm_Alarm_Cnt_Len	= 2,
		enSmsWordWrite_DrossTopTarget			= 14,		enSmsWordWrite_DrossTopTarget_Len		= 1,
		enSmsWordWrite_DrossBtmTarget			= 15,		enSmsWordWrite_DrossBtmTarget_Len		= 1,
		enSmsWordWrite_FoilExpTopTarget			= 16,		enSmsWordWrite_FoilExpTopTarget_Len		= 1,
		enSmsWordWrite_FoilExpBtmTarget			= 17,		enSmsWordWrite_FoilExpBtmTarget_Len		= 1,
		enSmsWordWrite_SpeterTopTarget			= 18,		enSmsWordWrite_SpeterTopTarget_Len		= 1,
		enSmsWordWrite_SpeterBtmTarget			= 19,		enSmsWordWrite_SpeterBtmTarget_Len		= 1,
		enSmsWordWrite_AlarmExist				= 20,		enSmsWordWrite_AlarmExist_Len			= 1,
		enSmsWordWrite_AlarmCode_Buffer1		= 21,		enSmsWordWrite_AlarmCode_Buffer1_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer2		= 22,		enSmsWordWrite_AlarmCode_Buffer2_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer3		= 23,		enSmsWordWrite_AlarmCode_Buffer3_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer4		= 24,		enSmsWordWrite_AlarmCode_Buffer4_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer5		= 25,		enSmsWordWrite_AlarmCode_Buffer5_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer6 = 26, enSmsWordWrite_AlarmCode_Buffer6_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer7 = 27, enSmsWordWrite_AlarmCode_Buffer7_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer8 = 28, enSmsWordWrite_AlarmCode_Buffer8_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer9 = 29, enSmsWordWrite_AlarmCode_Buffer9_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer10 = 30, enSmsWordWrite_AlarmCode_Buffer10_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer11 = 31, enSmsWordWrite_AlarmCode_Buffer11_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer12 = 32, enSmsWordWrite_AlarmCode_Buffer12_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer13 = 33, enSmsWordWrite_AlarmCode_Buffer13_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer14 = 34, enSmsWordWrite_AlarmCode_Buffer14_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer15 = 35, enSmsWordWrite_AlarmCode_Buffer15_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer16 = 36, enSmsWordWrite_AlarmCode_Buffer16_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer17 = 37, enSmsWordWrite_AlarmCode_Buffer17_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer18 = 38, enSmsWordWrite_AlarmCode_Buffer18_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer19 = 39, enSmsWordWrite_AlarmCode_Buffer19_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer20 = 40, enSmsWordWrite_AlarmCode_Buffer20_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer21 = 41, enSmsWordWrite_AlarmCode_Buffer21_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer22 = 42, enSmsWordWrite_AlarmCode_Buffer22_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer23 = 43, enSmsWordWrite_AlarmCode_Buffer23_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer24 = 44, enSmsWordWrite_AlarmCode_Buffer24_Len = 1,

		// 22.07.05 Ahn Add Start
		enSmsWordWrite_Top_Defect_Count_Real = 45, enSmsWordWrite_Top_Defect_Count_Real_Len = 1,
		enSmsWordWrite_Btm_Defect_Count_Real = 46, enSmsWordWrite_Btm_Defect_Count_Real_Len = 1,
		enSmsWordWrite_Top_Defect_Count_LotEnd = 47, enSmsWordWrite_Top_Defect_Count_LotEnd_Len = 1,
		enSmsWordWrite_Btm_Defect_Count_LotEnd = 48, enSmsWordWrite_Btm_Defect_Count_LotEnd_Len = 1,
		// 22.07.05 Ahn Add End

		// 22.12.12 Ahn Add Start - Judge Report
		en_SmsWordWrite_Cell_Trigger_ID			= 49,		en_SmsWordWrite_Cell_Trigger_ID_Len		= 1,
		en_SmsWordWrite_Judge					= 50,		en_SmsWordWrite_Judge_Len				= 1,
		en_SmsWordWrite_NG_Code					= 51,		en_SmsWordWrite_NG_Code_Len				= 1,
		// 22.12.12 Ahn Add End

		enSmsWordWriteMaxSize = 52,

		enSmsWordWrite_Size = 16,



	};
	// Siemens Word Address End
	// 23.03.02 Ahn Add End

private:
	//int m_sIOBitIN[MAX_ADR_BIT_IN];
	//int m_sIOBitOUT[MAX_ADR_BIT_OUT];

	//int m_nMaxBitIN ;
	//int m_nMaxBitOUT ;

	// 22.07.29 Ahn Add Start
	BYTE	m_btSigBItIN[MAX_USE_PORT];
	BYTE	m_btSigBItOut[MAX_USE_PORT];
	// 22.07.29 Ahn Add End

	short	m_sSmsSigBItIN[MAX_SMS_IO_IN];
	short	m_sSmsSigBItOut[MAX_SMS_IO_OUT];

	BOOL	m_bSmsAlive;

	static CCriticalSection	m_cs;

public:
	CSigProc();
	virtual ~CSigProc();

	static int GetPortBit(int iIntegration, int* piPort, BYTE* pByte); // 22.08.03 Ahn Modify to satic 
	int SignalPortIn(WORD port, BYTE* data, BOOL bExtSt = FALSE, BOOL bLocal = FALSE);
	int SignalPortCheck(int iInput, BOOL bExtSt = FALSE, BOOL bLocal = FALSE);
	int SignalBitOut(int iIntegration, int iMode, BOOL bLocal = FALSE);
	BYTE SignalPortGetOut(WORD port);
	int CheckPortOut(int sig);

	int WritePLC_device(int address, short data);
	int ReadPLC_device(int address, short* data);
	int WritePLC_Block_device(int address, short* pData, int nNumOfData);
	int ReadPLC_Block_device(int address, short* pData, int nNumOfData);
	// 22.07.06 Ahn Add Start
	int WritePLC_Block_device(int address, int* pData, int nNumOfData);
	// 22.07.06 Ahn Add End
	int ReadPLC_Block_deviceR(int address, short* pData, int nNumOfData);
	int WaitSignal(int iIntegration, BOOL mode, int timeout = 1000, BOOL bCheckAutoSignal = FALSE, BOOL bErrorProcess = TRUE, BOOL	bExtSt = FALSE);

	// 23.03.03 Ahn Delete Start
	//int WriteAlarmCode();
	// 23.03.03 Ahn Delete End
		// 22.03.24 Ahn Add Start
	int SetAlarmCode(WORD nAlarmCode);
	WORD GetAlarmCode();
	// 22.03.24 Ahn Add End
	// 22.03.28 Ahn Add Start
	int WriteAlarmCode(WORD nAlarmCode);
	// 22.03.28 Ahn Add End

	// 22.03.23 Ahn Add Start
	int ReadAllPort_BitIn( BOOL *pSigBitIn );
	int ReadAllPort_BitOut( BOOL* pSigBitOut );
	// 22.03.23 Ahn Add End

	// 22.12.12 Ahn Add Start
	int ReportJudge(int nID, int nJudge, int nCode);
	// 22.12.12 Ahn Add End

	// 23.03.03 Ahn Add Start
	static int GetWordAddress( int nAddress, int nMode/*Read or Write*/);
	// 23.03.03 Ahn Add End
public :
	// Signal In
	int SigInAlivePulse();
	int SigInReady();
	int SigInRun();
	int SigInEncoderZeroReset();
	int SigInTabZeroReset();
	int SigInRecipeChange();
	int SigInLotStart();
	int SigInLotEnd();
	// 22.02.17 Ahn Add Start
	int SigInInkMarkingActive();
	// 22.02.17 Ahn Add End
	// 22.10.04 Ahn Add Start
	int SigInAlarmReset();
	// 22.10.04 Ahn Add End

	// Signal Out
	int SigOutAlivePulse(int nMode);
	int SigOutReady(int nMode);
	int SigOutEncoderZeroSet(int nMde);
	int SigOutRecipeChangeAck(int nMode);
	int SigOutLotStartAck(int nMode);
	int sigOutLotEndAck(int nMode);
	// 22.02.23 Ahn Add Start
	int SigOutTabZeroReset(int nMode);
	// 22.02.23 Ahn Add End
	int SigOutAlarmResetAck(int nMode);



	// 22.08.05 Ahn Add Start
	int SigOutDiskCapacityAlarm(int nMode);
	int SigOutDiskCapacityWarning(int nMode);
	// 22.08.05 Ahn Add End

	int SigOutAlarmExist(int nMode);


	// Word
	int ReadBlockAllData(CSequenceData *pSeqData);
	int WriteBlockAllData(int nMode);
	// 22.08.12 Ahn Add Start
	//int ReadBlockWriteData(_SEQ_OUT_DATA* pSeqOutData); // 23.03.03 Ahn Delete 
	// 22.08.12 Ahn Add End
	// 22.08.19 Ahn Add Start
	int ReadBlockWriteDataAll(_SEQ_OUT_DATA_ALL* pSeqOutDataAll);
	// 22.08.19 Ahn Add End

	// 23.03.03 Ahn Add Start 
	int ReadBlockAllData_Melsec(CSequenceData* pSeqData);
	int WriteBlockAllData_Melsec(int nMode);
	//int ReadBlockWriteData_Melsec(_SEQ_OUT_DATA* pSeqOutData); // 23.03.03 Ahn Delete 
	int ReadBlockWriteDataAll_Melsec(_SEQ_OUT_DATA_ALL* pSeqOutDataAll);

	int ReadBlockAllData_Siemens(CSequenceData* pSeqData);
	int WriteBlockAllData_Siemens(int nMode);
	//int ReadBlockWriteData_Siemens(_SEQ_OUT_DATA* pSeqOutData); // 23.03.03 Ahn Delete 
	int ReadBlockWriteDataAll_Siemens(_SEQ_OUT_DATA_ALL* pSeqOutDataAll);
	// 23.03.03 Ahn Add End


	// 22.05.26 Ahn Add Start
	void EnableWorkSet(BOOL bMode);
	// 22.05.26 Ahn Add Start

protected:
	CPioCtrl* m_pPioCtrl;			// 괿?굈맕뚥긆긳긙긃긏긣귉궻?귽깛?걁긓깛긹귺몧뭫궴궻괿?괻뾭걂(CAprPioCtrl *)x1
	BOOL	m_bDebugIO;
};

