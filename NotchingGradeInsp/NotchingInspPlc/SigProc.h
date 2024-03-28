#pragma once

#include "PioCtrl.h"
#include "GlobalData.h" 
#define	MAX_ADR_BIT_IN	( 32 )
#define	MAX_ADR_BIT_OUT	( 32 )


class CSequenceData ;

class CSigProc : public CPlcImp
{
public:
	//////////////////////////////////////////////////////////////////////////
	// [ Melsec Address Start ]
	// [ BIT IN ]
	enum
	{
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
		enBitIn_ConnectZone		= 0x00000104,	// B63CA 
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

		enBitIn_AlarmResetReq	= 0x00000220,	// B63D8 
		enBitIn_AlarmNgAck		= 0x00000240,	
	};

	// [ BIT OUT ]
	enum
	{
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
		enBitOut_AlramResetAck	= 0x00000220,	// B6B55
		enBitOut_AlramNgResetAck= 0x00000240,	// B6B55
	};

	//////////////////////////////////////////////////////////////////////////
	// [ DATA READ ]
	enum
	{
		enWordRead_RecipeNo = 0x0000,		enWordRead_RecipeNo_Len = 2,				//	2
		enWordRead_RecipeName = 0x0002,		enWordRead_RecipeName_Len = 8,				// 10
		enWordRead_CELL_ID = 0x0010,		enWordRead_CELL_ID_Len = 20,				// 30
		enWordRead_FoilExpInTopTarget = 0x002E, enWordRead_FoilExpInTopTarget_Len = 2,				// 34
		enWordRead_FoilExpInBtmTarget = 0x0030, enWordRead_FoilExpInBtmTarget_Len = 2,				// 34
		enWordRead_FoilExpOutTopTarget = 0x0032, enWordRead_FoilExpOutTopTarget_Len = 2,				// 34
		enWordRead_FoilExpOutBtmTarget = 0x0034, enWordRead_FoilExpOutBtmTarget_Len = 2,				// 34
		enWordRead_FoilExpBothTopTarget = 0x0036, enWordRead_FoilExpBothTopTarget_Len = 2,				// 38
		enWordRead_FoilExpBothBtmTarget = 0x0038, enWordRead_FoilExpBothBtmTarget_Len = 2,				// 40
		enWordRead_SpeterTopTarget = 0x003A,		enWordRead_SpeterTopTarget_Len = 2,				// 42
		enWordRead_SpeterBtmTarget = 0x003C,		enWordRead_SpeterBtmTarget_Len = 2,				// 44
		enWordRead_PrmContinuousCnt = 0x0040,		enWordRead_PrmContinuousCnt_Len = 2,
		enWordRead_PrmSectorNgTabCnt = 0x0042,		enWordRead_PrmSectorNgTabCnt_Len = 2,
		enWordRead_PrmSectorBaseCnt = 0x0044,		enWordRead_PrmSectorBaseCnt_Len = 2,
		enWordRead_AlarmExistAck = 0x0050,		enWordRead_AlarmExistAck_Len = 2,				// 46
		enWordReadMaxSize = 160,

	};

	// [ DATA WRITE ]
	enum
	{
		enWordWrite_DataReportV1_Ea = 0x0000, enWordWrite_DataReportV1_Len = 2,
		enWordWrite_DataReportV2_OK = 0x0002, enWordWrite_DataReportV2_Len = 2,
		enWordWrite_DataReportV3_NG = 0x0004, enWordWrite_DataReportV3_Len = 2,
		enWordWrite_DataReportV4_OkRate = 0x0006, enWordWrite_DataReportV4_Len = 2,
		enWordWrite_DataReportV5_NgRate = 0x0008, enWordWrite_DataReportV5_Len = 2,
		enWordWrite_DataReportV6_RunRate = 0x000A, enWordWrite_DataReportV6_Len = 2,
		enWordWrite_Continue_Alarm_Cnt = 0x000C, enWordWrite_Continue_Alarm_Cnt_Len = 2,
		enWordWrite_Heavy_Alarm_Cnt = 0x000E, enWordWrite_Heavy_Alarm_Cnt_Len = 2,

		enWordWrite_FoilExpInTop_Alarm_Cnt = 0x0010, enWordWrite_FoilExpInTop_Alarm_Cnt_Len = 2,
		enWordWrite_FoilExpInBottom_Alarm_Cnt = 0x0012, enWordWrite_FoilExpInBtm_Alarm_Cnt_Len = 2,
		enWordWrite_FoilExpOutTop_Alarm_Cnt = 0x0014, enWordWrite_FoilExpOutTop_Alarm_Cnt_Len = 2,
		enWordWrite_FoilExpOutBottom_Alarm_Cnt = 0x0016, enWordWrite_FoilExpOutBtm_Alarm_Cnt_Len = 2,
		enWordWrite_FoilExpBothTop_Alarm_Cnt = 0x0018, enWordWrite_FoilExpBothTop_Alarm_Cnt_Len = 2,
		enWordWrite_FoilExpBothBottom_Alarm_Cnt = 0x001A, enWordWrite_FoilExpBothBtm_Alarm_Cnt_Len = 2,
		enWordWrite_SpeterTop_Alarm_Cnt = 0x001C, enWordWrite_SpeterTop_Alarm_Cnt_Len = 2,
		enWordWrite_SpeterBtm_Alarm_Cnt = 0x001E, enWordWrite_SpeterBtm_Alarm_Cnt_Len = 2,
		enWordWrite_Top_Defect_Count_Real = 0x0020, enWordWrite_Top_Defect_Count_Real_Len = 2,
		enWordWrite_Btm_Defect_Count_Real = 0x0022, enWordWrite_Btm_Defect_Count_Real_Len = 2,
		enWordWrite_Top_Defect_Count_LotEnd = 0x0024, enWordWrite_Top_Defect_Count_LotEnd_Len = 2,
		enWordWrite_Btm_Defect_Count_LotEnd = 0x0026, enWordWrite_Btm_Defect_Count_LotEnd_Len = 2,

		enWordWrite_FoilExpInTopTarget = 0x002E, enWordWrite_FoilExpInTopTarget_Len = 2,
		enWordWrite_FoilExpInBottomTarget = 0x0030, enWordWrite_FoilExpInBottomTarget_Len = 2,
		enWordWrite_FoilExpOutTopTarget = 0x0032, enWordWrite_FoilExpOutTopTarget_Len = 2,
		enWordWrite_FoilExpOutBottomTarget = 0x0034, enWordWrite_FoilExpOutBottomTarget_Len = 2,
		enWordWrite_FoilExpBothTopTarget = 0x0036, enWordWrite_FoilExpBothTopTarget_Len = 2,
		enWordWrite_FoilExpBothBottomTarget = 0x0038, enWordWrite_FoilExpBothBottomTarget_Len = 2,
		enWordWrite_SpeterTopTarget = 0x003A, enWordWrite_SpeterTopTarget_Len = 2,
		enWordWrite_SpeterBtmTarget = 0x003C, enWordWrite_SpeterBtmTarget_Len = 2,

		enWordWrite_PrmContinuousCnt = 0x0040, enWordWrite_PrmContinuousCnt_Len = 2,
		enWordWrite_PrmSectorNgTabCnt = 0x0042, enWordWrite_PrmSectorNgTabCnt_Len = 2,
		enWordWrite_PrmSectorBaseCnt = 0x0044, enWordWrite_PrmSectorBaseCnt_Len = 2,


		enWordWrite_AlarmExist = 0x0050,		enWordWrite_AlarmExist_Len = 2,
		enWordWrite_AlarmCode_Buffer1 = 0x0052,		enWordWrite_AlarmCode_Buffer1_Len = 2,
		enWordWrite_AlarmCode_Buffer2 = 0x0054,		enWordWrite_AlarmCode_Buffer2_Len = 2,
		enWordWrite_AlarmCode_Buffer3 = 0x0056,		enWordWrite_AlarmCode_Buffer3_Len = 2,
		enWordWrite_AlarmCode_Buffer4 = 0x0058,		enWordWrite_AlarmCode_Buffer4_Len = 2,
		enWordWrite_AlarmCode_Buffer5 = 0x005A,		enWordWrite_AlarmCode_Buffer5_Len = 2,

		en_WordWrite_Cell_Trigger_ID = 0x0070,		en_WordWrite_Cell_Trigger_ID_Len = 2,
		en_WordWrite_Judge = 0x0072,		en_WordWrite_Judge_Len = 2,
		en_WordWrite_NG_Code = 0x0074,		en_WordWrite_NG_Code_Len = 2,

		enWordWriteMaxSize = 160,
	};

	//////////////////////////////////////////////////////////////////////////
	// [ Alarm Code ]
	enum
	{
		en_Alarm_ContinueNg			= 0x0001,
		en_Alarm_Heavy				= 0x0002,
		en_Alarm_SectorNg			= 0x0004,
		en_Alarm_FoilExpIn_Top		= 0x0008,
		en_Alarm_FoilExpIn_Btm		= 0x0010,
		en_Alarm_FoilExpOut_Top		= 0x0020,
		en_Alarm_FoilExpOut_Btm		= 0x0040,
		en_Alarm_FoilExpBoth_Top	= 0x0080,
		en_Alarm_FoilExpBoth_Btm	= 0x0100,
		en_Alarm_Spatter_Top		= 0x0200,
		en_Alarm_Spatter_Btm		= 0x0400,
	};


	//////////////////////////////////////////////////////////////////////////
	// [ Siemens Address Start ]
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
		enSmsBitIn_AlarmNgAck			= 11,	//  
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
		enSmsBitOut_AlarmNgResetAck		= 8,	// 

		enSmsBitOut_DiskSpaceWarning	= 9,	// Address Map에 없음
		enSmsBitOut_DiskSpaceAlarm		= 10,	// Address Map에 없음
	};
	// Siemens Address End
	// Siemens Word Address Start
	enum{
		enSmsWordRead_RecipeNo					= 0,		enSmsWordRead_RecipeNo_Len				= 1,		
		enSmsWordRead_RecipeName				= 1,		enSmsWordRead_RecipeName_Len			= 4,		
		enSmsWordRead_CELL_ID					= 5,		enSmsWordRead_CELL_ID_Len				= 10,		

		enSmsWordRead_FoilExpInTopTarget		= 20, enSmsWordRead_FoilExpInTopTarget_Len		= 1,			
		enSmsWordRead_FoilExpInBtmTarget		= 21, enSmsWordRead_FoilExpInBtmTarget_Len		= 1,			
		enSmsWordRead_FoilExpOutTopTarget		= 22, enSmsWordRead_FoilExpOutTopTarget_Len		= 1,			
		enSmsWordRead_FoilExpOutBtmTarget		= 23, enSmsWordRead_FoilExpOutBtmTarget_Len		= 1,			
		enSmsWordRead_FoilExpBothTopTarget		= 24, enSmsWordRead_FoilExpBothTopTarget_Len	= 1,
		enSmsWordRead_FoilExpBothBtmTarget		= 25, enSmsWordRead_FoilExpBothBtmTarget_Len	= 1,
		enSmsWordRead_SpeterTopTarget			= 26,		enSmsWordRead_SpeterTopTarget_Len		= 1,		
		enSmsWordRead_SpeterBtmTarget			= 27,		enSmsWordRead_SpeterBtmTarget_Len		= 1,		

		enSmsWordRead_PrmContinuousCnt			= 30,		enSmsWordRead_PrmContinuousCnt_Len		= 1,
		enSmsWordRead_PrmSectorNgTabCnt			= 31,		enSmsWordRead_PrmSectorNgTabCnt_Len		= 1,
		enSmsWordRead_PrmSectorBaseCnt			= 32,		enSmsWordRead_PrmSectorBaseCnt_Len		= 1,

//		enSmsWordRead_AlarmExistAck				= 21,		enSmsWordRead_AlarmExistAck_Len			= 1,		
//		enSmsWordRead_ConnectZoneTabCount		= 25,		enSmsWordRead_ConnectZoneTabCount_Len		= 1,
		
		enSmsWordReadMaxSize					= 33,
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


		enSmsWordWrite_FoilExpInTop_Alarm_Cnt = 8, enSmsWordWrite_FoilExpInTop_Alarm_Cnt_Len	= 1,
		enSmsWordWrite_FoilExpInBtm_Alarm_Cnt = 9, enSmsWordWrite_FoilExpInBtm_Alarm_Cnt_Len	= 1,
		enSmsWordWrite_FoilExpOutTop_Alarm_Cnt = 10, enSmsWordWrite_FoilExpOutTop_Alarm_Cnt_Len = 1,
		enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt = 11, enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt_Len = 1,
		enSmsWordWrite_FoilExpBothTop_Alarm_Cnt = 12, enSmsWordWrite_FoilExpBothTop_Alarm_Cnt_Len = 1,
		enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt = 13, enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt_Len = 1,
		enSmsWordWrite_SpeterTop_Alarm_Cnt		= 14,		enSmsWordWrite_SpeterTop_Alarm_Cnt_Len	= 1,
		enSmsWordWrite_SpeterBtm_Alarm_Cnt		= 15,		enSmsWordWrite_SpeterBtm_Alarm_Cnt_Len	= 1,
		
		enSmsWordWrite_Top_Defect_Count_Real = 16, enSmsWordWrite_Top_Defect_Count_Real_Len = 1,
		enSmsWordWrite_Btm_Defect_Count_Real = 17, enSmsWordWrite_Btm_Defect_Count_Real_Len = 1,
		enSmsWordWrite_Top_Defect_Count_LotEnd = 18, enSmsWordWrite_Top_Defect_Count_LotEnd_Len = 1,
		enSmsWordWrite_Btm_Defect_Count_LotEnd = 19, enSmsWordWrite_Btm_Defect_Count_LotEnd_Len = 1,



		enSmsWordWrite_FoilExpInTopTarget = 20, enSmsWordWrite_FoilExpInTopTarget_Len		= 1,
		enSmsWordWrite_FoilExpInBtmTarget = 21, enSmsWordWrite_FoilExpInBtmTarget_Len		= 1,
		enSmsWordWrite_FoilExpOutTopTarget = 22, enSmsWordWrite_FoilExpOutTopTarget_Len		= 1,
		enSmsWordWrite_FoilExpOutBtmTarget = 23, enSmsWordWrite_FoilExpOutBtmTarget_Len		= 1,
		enSmsWordWrite_FoilExpBothTopTarget = 24, enSmsWordWrite_FoilExpBothTopTarget_Len = 1,
		enSmsWordWrite_FoilExpBothBtmTarget = 25, enSmsWordWrite_FoilExpBothBtmTarget_Len = 1,
		enSmsWordWrite_SpeterTopTarget			= 26,		enSmsWordWrite_SpeterTopTarget_Len		= 1,
		enSmsWordWrite_SpeterBtmTarget			= 27,		enSmsWordWrite_SpeterBtmTarget_Len		= 1,
		
		enSmsWordWrite_PrmContinuousCnt = 28, enSmsWordWrite_PrmContinuousCnt_Len = 1,
		enSmsWordWrite_PrmSectorNgTabCnt = 29, enSmsWordWrite_PrmSectorNgTabCnt_Len = 1,
		enSmsWordWrite_PrmSectorBaseCnt = 30, enSmsWordWrite_PrmSectorBaseCnt_Len = 1,



		enSmsWordWrite_AlarmExist				= 40,		enSmsWordWrite_AlarmExist_Len			= 1,
		enSmsWordWrite_AlarmCode_Buffer1		= 41,		enSmsWordWrite_AlarmCode_Buffer1_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer2		= 42,		enSmsWordWrite_AlarmCode_Buffer2_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer3		= 43,		enSmsWordWrite_AlarmCode_Buffer3_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer4		= 44,		enSmsWordWrite_AlarmCode_Buffer4_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer5		= 45,		enSmsWordWrite_AlarmCode_Buffer5_Len	= 1,
		enSmsWordWrite_AlarmCode_Buffer6 = 46, enSmsWordWrite_AlarmCode_Buffer6_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer7 = 47, enSmsWordWrite_AlarmCode_Buffer7_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer8 = 48, enSmsWordWrite_AlarmCode_Buffer8_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer9 = 49, enSmsWordWrite_AlarmCode_Buffer9_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer10 = 50, enSmsWordWrite_AlarmCode_Buffer10_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer11 = 51, enSmsWordWrite_AlarmCode_Buffer11_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer12 = 52, enSmsWordWrite_AlarmCode_Buffer12_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer13 = 53, enSmsWordWrite_AlarmCode_Buffer13_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer14 = 54, enSmsWordWrite_AlarmCode_Buffer14_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer15 = 55, enSmsWordWrite_AlarmCode_Buffer15_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer16 = 56, enSmsWordWrite_AlarmCode_Buffer16_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer17 = 57, enSmsWordWrite_AlarmCode_Buffer17_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer18 = 58, enSmsWordWrite_AlarmCode_Buffer18_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer19 = 59, enSmsWordWrite_AlarmCode_Buffer19_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer20 = 60, enSmsWordWrite_AlarmCode_Buffer20_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer21 = 61, enSmsWordWrite_AlarmCode_Buffer21_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer22 = 62, enSmsWordWrite_AlarmCode_Buffer22_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer23 = 63, enSmsWordWrite_AlarmCode_Buffer23_Len = 1,
		enSmsWordWrite_AlarmCode_Buffer24 = 64, enSmsWordWrite_AlarmCode_Buffer24_Len = 1,


		en_SmsWordWrite_Cell_Trigger_ID = 80, en_SmsWordWrite_Cell_Trigger_ID_Len = 1,
		en_SmsWordWrite_Judge = 81, en_SmsWordWrite_Judge_Len = 1,
		en_SmsWordWrite_NG_Code = 82, en_SmsWordWrite_NG_Code_Len = 1,


		enSmsWordWrite_DuplicateNG_Cell_ID = 85, enSmsWordWrite_DuplicateNG_Cell_ID_Len = 64,

		enSmsWordWriteMaxSize = 149,

	};
	// Siemens Word Address End

private:

	BYTE	m_btSigBItIN[MAX_USE_PORT];
	BYTE	m_btSigBItOut[MAX_USE_PORT];

	short	m_sSmsSigBItIN[MAX_SMS_IO_IN];
	short	m_sSmsSigBItOut[MAX_SMS_IO_OUT];

	BOOL	m_bSmsAlive;
	BOOL	m_bSigInkMarkAcrive;
	BOOL	m_bSigInConnectZone;


	WORD	m_wMonitoringReadData_Melsec[enWordReadMaxSize];
	WORD	m_wMonitoringWriteData_Melsec[enWordWriteMaxSize];

	WORD	m_wMonitoringReadData_Siemens[enWordReadMaxSize];
	WORD	m_wMonitoringWriteData_Siemens[enWordWriteMaxSize];

	static CCriticalSection	m_cs;

	//PLC 시그널 상태
	BOOL m_SigInRun;
public:
	CSigProc();
	virtual ~CSigProc();

	//PLC 시그널 상태
	BOOL GetSigInRun() { return m_SigInRun; }

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
	int WritePLC_Block_device(int address, int* pData, int nNumOfData);
	int ReadPLC_Block_deviceR(int address, short* pData, int nNumOfData);
	int WaitSignal(int iIntegration, BOOL mode, int timeout = 1000, BOOL bCheckAutoSignal = FALSE, BOOL bErrorProcess = TRUE, BOOL	bExtSt = FALSE);

	int SetAlarmCode(WORD nAlarmCode);
	WORD GetAlarmCode();
	int WriteAlarmCode(WORD nAlarmCode);
	int WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode);

	int ReadAllPort_BitIn( BOOL *pSigBitIn );
	int ReadAllPort_BitOut( BOOL* pSigBitOut );
	int ReportJudge(int nID, int nJudge, int nCode);

	static int GetWordAddress( int nAddress, int nMode/*Read or Write*/);

	void SetInkMarkAcktive(BOOL bUse) { m_bSigInkMarkAcrive = bUse; }
	BOOL GetInkMarkActive() { return m_bSigInkMarkAcrive; }

	void SetConnectZone(BOOL bUse) { m_bSigInConnectZone = bUse; }
	BOOL GetConnectZone() { return m_bSigInConnectZone; }

	WORD GetMonitoringReadData_Melsec(int nPos) { return m_wMonitoringReadData_Melsec[nPos]; }
	WORD GetMonitoringWriteData_Melsec(int nPos) { return m_wMonitoringWriteData_Melsec[nPos]; }

	WORD GetMonitoringReadData_Siemens(int nPos) { return m_wMonitoringReadData_Siemens[nPos]; }
	WORD GetMonitoringWriteData_Siemens(int nPos) { return m_wMonitoringWriteData_Siemens[nPos]; }

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
	int SigInAlarmReset();
	int SigInInkMarkActive();
	int SigInConnectZone();
	int SigInAlarmNgAck();



	// Signal Out
	int SigOutAlivePulse(int nMode);
	int SigOutAlivePulseReady(int nInMode, BOOL bIsReady);
	int SigOutReady(int nMode);
	int SigOutEncoderZeroSet(int nMde);
	int SigOutRecipeChangeAck(int nMode);
	int SigOutLotStartAck(int nMode);
	int SigOutLotEndAck(int nMode);
	int SigOutTabZeroReset(int nMode);
	int SigOutAlarmResetAck(int nMode);
	int SigOutDiskCapacityAlarm(int nMode);
	int SigOutDiskCapacityWarning(int nMode);
	int SigOutAlarmExist(int nMode);
	int SigOutAlarmNgResetAck(int nMode);

	BOOL SigOutDataReset(); 

	// Word
	int ReadBlockAllData(CSequenceData *pSeqData);
	int WriteBlockAllData(int nMode);

	int ReadBlockAllData_Melsec(CSequenceData* pSeqData);
	int WriteBlockAllData_Melsec(int nMode);
	int ReadBlockWriteDataAll_Melsec(_SEQ_OUT_DATA_ALL* pSeqOutDataAll);

	int ReadBlockAllData_Siemens(CSequenceData* pSeqData);
	int WriteBlockAllData_Siemens(int nMode);
	int ReadBlockWriteDataAll_Siemens(_SEQ_OUT_DATA_ALL_SMS* pSeqOutDataAll);

	void EnableWorkSet(BOOL bMode);

protected:
	CPioCtrl* m_pPioCtrl;
	BOOL	m_bDebugIO;
};

