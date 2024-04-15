#include "pch.h"
#include "MelsecPlcIo.h"
#include "mdfunc.h"
#include "GlobalData.h"
#include "LogDisplayDlg.h"


// [ Melsec Address Start ]
// [ BIT IN ]
enum MelsBitIn
{
	enMelsBitIn_Alive = 0x00000001,
	enMelsBitIn_Ready = 0x00000002,
	enMelsBitIn_Run = 0x00000004,
	enMelsBitIn_Dummy4 = 0x00000008,
	enMelsBitIn_Dummy5 = 0x00000010,
	enMelsBitIn_EncoderReset = 0x00000020,
	enMelsBitIn_TabZeroReset = 0x00000040,
	enMelsBitIn_Dummy8 = 0x00000080,

	enMelsBitIn_Dummy9 = 0x00000101,
	enMelsBitIn_InkMarkingActive = 0x00000102,
	enMelsBitIn_ConnectZone = 0x00000104,
	enMelsBitIn_Dummy12 = 0x00000108,
	enMelsBitIn_Dummy13 = 0x00000110,
	enMelsBitIn_Dummy14 = 0x00000120,
	enMelsBitIn_Dummy15 = 0x00000140,
	enMelsBitIn_Dummy16 = 0x00000180,

	enMelsBitIn_Dummy17 = 0x00000201,
	enMelsBitIn_Dummy18 = 0x00000202,
	enMelsBitIn_RecipeChange = 0x00000204,
	enMelsBitIn_LotStartReq = 0x00000208,
	enMelsBitIn_LotEndReq = 0x00000210,

	enMelsBitIn_AlarmResetReq = 0x00000220,
	enMelsBitIn_AlarmNgAck = 0x00000240
};

// [ BIT OUT ]
enum MelsBitOut
{
	enMelsBitOut_Alive = 0x00000001,
	enMelsBitOut_Ready = 0x00000002,
	enMelsBitOut_Dummy3 = 0x00000004,
	enMelsBitOut_Dummy4 = 0x00000008,
	enMelsBitOut_Dummy5 = 0x00000010,
	enMelsBitOut_EncoderSet = 0x00000020,
	enMelsBitOut_TabZeroReset = 0x00000040,
	enMelsBitOut_Dummy8 = 0x00000080,
	enMelsBitOut_DiskSpaceWarning = 0x00000101,
	enMelsBitOut_DiskSpaceAlarm = 0x00000102,
	enMelsBitOut_Dummy11 = 0x00000104,
	enMelsBitOut_Dummy12 = 0x00000108,
	enMelsBitOut_Dummy13 = 0x00000110,
	enMelsBitOut_Dummy14 = 0x00000120,
	enMelsBitOut_Dummy15 = 0x00000140,
	enMelsBitOut_Dummy16 = 0x00000180,
	enMelsBitOut_Dummy17 = 0x00000201,
	enMelsBitOut_Dummy18 = 0x00000202,
	enMelsBitOut_RecipeChangeAck = 0x00000204,
	enMelsBitOut_LotStartReqAck = 0x00000208,
	enMelsBitOut_LotEndReqAck = 0x00000210,
	enMelsBitOut_AlramResetAck = 0x00000220,
	enMelsBitOut_AlramNgResetAck = 0x00000240
};

//////////////////////////////////////////////////////////////////////////
// [ DATA READ ]
enum MelsWordRead
{
	enMelsWordRead_RecipeNo = 0x0000,
	enMelsWordRead_RecipeName = 0x0002,
	enMelsWordRead_CELL_ID = 0x0010,
	enMelsWordRead_FoilExpInTopTarget = 0x002E,
	enMelsWordRead_FoilExpInBtmTarget = 0x0030,
	enMelsWordRead_FoilExpOutTopTarget = 0x0032,
	enMelsWordRead_FoilExpOutBtmTarget = 0x0034,
	enMelsWordRead_FoilExpBothTopTarget = 0x0036,
	enMelsWordRead_FoilExpBothBtmTarget = 0x0038,
	enMelsWordRead_SpeterTopTarget = 0x003A,
	enMelsWordRead_SpeterBtmTarget = 0x003C,
	enMelsWordRead_PrmContinuousCnt = 0x0040,
	enMelsWordRead_PrmSectorNgTabCnt = 0x0042,
	enMelsWordRead_PrmSectorBaseCnt = 0x0044,
	enMelsWordRead_AlarmExistAck = 0x0050,

	enMelsWordReadMaxSize = 160,

};

// [ DATA WRITE ]
enum MelsWordWrite
{
	enMelsWordWrite_DataReportV1_Ea = 0x0000,
	enMelsWordWrite_DataReportV2_OK = 0x0002,
	enMelsWordWrite_DataReportV3_NG = 0x0004,
	enMelsWordWrite_DataReportV4_OkRate = 0x0006,
	enMelsWordWrite_DataReportV5_NgRate = 0x0008,
	enMelsWordWrite_DataReportV6_RunRate = 0x000A,
	enMelsWordWrite_Continue_Alarm_Cnt = 0x000C,
	enMelsWordWrite_Heavy_Alarm_Cnt = 0x000E,

	enMelsWordWrite_FoilExpInTop_Alarm_Cnt = 0x0010,
	enMelsWordWrite_FoilExpInBottom_Alarm_Cnt = 0x0012,
	enMelsWordWrite_FoilExpOutTop_Alarm_Cnt = 0x0014,
	enMelsWordWrite_FoilExpOutBottom_Alarm_Cnt = 0x0016,
	enMelsWordWrite_FoilExpBothTop_Alarm_Cnt = 0x0018,
	enMelsWordWrite_FoilExpBothBottom_Alarm_Cnt = 0x001A,
	enMelsWordWrite_SpeterTop_Alarm_Cnt = 0x001C,
	enMelsWordWrite_SpeterBtm_Alarm_Cnt = 0x001E,
	enMelsWordWrite_Top_Defect_Count_Real = 0x0020,
	enMelsWordWrite_Btm_Defect_Count_Real = 0x0022,
	enMelsWordWrite_Top_Defect_Count_LotEnd = 0x0024,
	enMelsWordWrite_Btm_Defect_Count_LotEnd = 0x0026,

	enMelsWordWrite_FoilExpInTopTarget = 0x002E,
	enMelsWordWrite_FoilExpInBottomTarget = 0x0030,
	enMelsWordWrite_FoilExpOutTopTarget = 0x0032,
	enMelsWordWrite_FoilExpOutBottomTarget = 0x0034,
	enMelsWordWrite_FoilExpBothTopTarget = 0x0036,
	enMelsWordWrite_FoilExpBothBottomTarget = 0x0038,
	enMelsWordWrite_SpeterTopTarget = 0x003A,
	enMelsWordWrite_SpeterBtmTarget = 0x003C,

	enMelsWordWrite_PrmContinuousCnt = 0x0040,
	enMelsWordWrite_PrmSectorNgTabCnt = 0x0042,
	enMelsWordWrite_PrmSectorBaseCnt = 0x0044,


	enMelsWordWrite_AlarmExist = 0x0050,
	enMelsWordWrite_AlarmCode_Buffer1 = 0x0052,
	enMelsWordWrite_AlarmCode_Buffer2 = 0x0054,
	enMelsWordWrite_AlarmCode_Buffer3 = 0x0056,
	enMelsWordWrite_AlarmCode_Buffer4 = 0x0058,
	enMelsWordWrite_AlarmCode_Buffer5 = 0x005A,

	enMelsWordWrite_Cell_Trigger_ID = 0x0070,
	enMelsWordWrite_Judge = 0x0072,
	enMelsWordWrite_NG_Code = 0x0074,

	enWordWriteMaxSize = 160,
};

CMelsecPlcIo::CMelsecPlcIo(WORD wChannelNo, WORD wMaxPort, WORD wMyStNo, WORD wExtStNo, WORD wSeqStNo, WORD wOffset_In, WORD wOffset_Out)
{
	m_wChannelNo = wChannelNo;
	m_wMaxPort = wMaxPort;
	m_wMyStNo = wMyStNo;
	m_wExtStNo = wExtStNo;
	m_wSeqStNo = wSeqStNo;

	m_wOffset_In = wOffset_In;
	m_wOffset_Out = wOffset_Out;

	m_pPath = NULL;

	m_bOpened = FALSE;

	m_bEnableWorkSet = TRUE;

	//PLC Open
	OpenPlcIo();
}


CMelsecPlcIo::~CMelsecPlcIo()
{
	
}

int CMelsecPlcIo::OpenPlcIo(void)
{
	int ret = 0;
	ret = LocalPioOpen();

	if (ret != 0)
	{
		//에러로그
	}
	else
	{
		//
		ReadBitData(0xff, "B", 0, 4);
	}

	return ret;
}

int CMelsecPlcIo::LocalPioOpen(BOOL bLockCtrl)
{

	int	iRet = 0;

	iRet = (int)mdOpen(m_wChannelNo, -1, &m_pPath);

	if (iRet == 0) 
	{
		m_bOpened = TRUE;

	}

	// WorkingSize
	if (ChangeWorkingSetSize() != 0) 
	{
		m_bOpened = FALSE;

		//로그출력
		LOGDISPLAY_SPEC(2)("PLC Melsec ChangeWorkingSetSize failed");
	}


	return (iRet);

}

int CMelsecPlcIo::ChangeWorkingSetSize(void)
{
	int	nRet = 0;

	if (m_bEnableWorkSet == FALSE) 
	{
		// Grabber 동장중 실행시 IRQL 발생 가능성 높음
		return 0;
	}

	DWORD	dwProcessId = 0;
	HANDLE	hProcess = 0;
#if defined( _WIN64 )
	SIZE_T	dwMinimumWorkingSetSize = 0;
	SIZE_T	dwMaximumWorkingSetSize = 0;
#else
	DWORD	dwMinimumWorkingSetSize = 0;
	DWORD	dwMaximumWorkingSetSize = 0;
#endif
	dwProcessId = GetCurrentProcessId();
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess == NULL) 
	{
		//에러로그
		return (-1);
	}

	if (GetProcessWorkingSetSize(hProcess, &dwMinimumWorkingSetSize, &dwMaximumWorkingSetSize) == 0) 
	{
		CloseHandle(hProcess);
		//에러로그
		return (-1);
	}

#define	MIN_WORKING_SETSIZE		1 * 1024 * 1024	// 1MB
#define	MAX_WORKING_SETSIZE		3 * 1024 * 1024	// 3MB

	dwMinimumWorkingSetSize = (DWORD)MIN_WORKING_SETSIZE;
	dwMaximumWorkingSetSize = (DWORD)MAX_WORKING_SETSIZE;

	if (SetProcessWorkingSetSize(hProcess, dwMinimumWorkingSetSize, dwMaximumWorkingSetSize) == 0) 
	{
		CloseHandle(hProcess);
		//에러로그
	}
	CloseHandle(hProcess);

	return (nRet);
}

int CMelsecPlcIo::ReadBitData(short stno, CString device, int adrs, int num)
{
	int iRet = 0;

	short	devno, size, size2, * buff, devtype;

	if (device.Compare(_T("M")) == 0) 
	{
		devtype = MELSEC_DEVICE_M;
	}
	else if (device.Compare(_T("B")) == 0) 
	{
		devtype = MELSEC_DEVICE_B;
	}
	else if (device.Compare(_T("X")) == 0) 
	{
		devtype = MELSEC_DEVICE_X;
	}
	else {
		AfxMessageBox(_T("디바이스명이 잘못되었습니다."));
		return (0);
	}

	buff = new short[num];

	devno = (short)(adrs * 8);
	size2 = size = num;

	{
		iRet = mdReceive(m_pPath
			, stno
			, devtype
			, devno
			, &size
			, buff
		);
	}

	short localBuff[10] = { 0, };
	memcpy(localBuff, buff, num);

	delete[] buff;

	return iRet;

}

void CMelsecPlcIo::ClosePlcIo(void)
{
	mdClose(m_pPath);
}

//Out
int CMelsecPlcIo::SigOutEncoderZeroSet(int nMode)
{ 
	setBitOut_EncoderSet(nMode);
	return 0;
}
int CMelsecPlcIo::SigOutRecipeChangeAck(int nMode)
{
	setBitOut_RecipeChangeAck(nMode);
	return 0; 
}
int CMelsecPlcIo::SigOutLotEndAck(int nMode)
{
	setBitOut_LotEndReqAck(nMode);
	return 0; 
}
int CMelsecPlcIo::SigOutLotStartAck(int nMode)
{ 
	setBitOut_LotStartReqAck(nMode);
	return 0; 
}
int CMelsecPlcIo::WriteAlarmCode(WORD nAlarmCode)
{ 
	//하나라도 알람 비트가 있으면 TRUE
	BOOL bAlarmExist = FALSE;
	for (int i = 0; i < COUNT_ALRAMBUFF; i++)
	{
		WORD BufferCode = (nAlarmCode >> i) & 0x1;
		bAlarmExist |= BufferCode;
		setWordOut_AlarmCode_Buffer(i, BufferCode); // Alarm Code
	}
	setWordOut_AlarmExist(bAlarmExist);
	return 0;
}
int CMelsecPlcIo::SigOutReady(int nMode)
{ 
	setBitOut_Ready(nMode);
	return 0;
}
int CMelsecPlcIo::SigOutAlivePulse(int nInMode)
{ 
	m_bSmsAlive ^= 0x1;
	setBitOut_Alive(m_bSmsAlive);
	return 0;
}
int CMelsecPlcIo::SigOutTabZeroReset(int nMode)
{ 
	setWordOut_DataReportV1_Ea(0);
	setWordOut_DataReportV2_OK(0);
	setWordOut_DataReportV3_NG(0);
	setWordOut_DataReportV4_OkRate(0);
	setWordOut_DataReportV5_NgRate(0);
	setWordOut_DataReportV6_RunRate(0);

	setWordOut_Continue_Alarm_Cnt(0);
	setWordOut_Heavy_Alarm_Cnt(0);

	setWordOut_AlarmCode_Buffer(0, 0);
	setWordOut_AlarmCode_Buffer(1, 0);
	setWordOut_AlarmCode_Buffer(2, 0);
	setWordOut_AlarmCode_Buffer(3, 0);
	setWordOut_AlarmCode_Buffer(4, 0);

	setWordOut_Top_Defect_Count_Real(0);
	setWordOut_Btm_Defect_Count_Real(0);

	setWordOut_Top_Defect_Count_LotEnd(0);
	setWordOut_Btm_Defect_Count_LotEnd(0);

	setBitOut_TabZeroReset(nMode);
	return 0; 
}
int CMelsecPlcIo::SigOutAlarmResetAck(int nMode)
{ 
	setBitOut_AlarmResetAck(nMode);
	return 0; 
}
int CMelsecPlcIo::WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode)
{
	//하나라도 알람 비트가 있으면 TRUE
	BOOL bAlarmExist = FALSE;
	for (int i = 1; i < COUNT_ALRAMBUFF; i++)
	{
		WORD BufferCode = (nAlarmCode >> i) & 0x1;
		bAlarmExist |= BufferCode;
		setWordOut_AlarmCode_Buffer(i, BufferCode); // Alarm Code
	}
	setWordOut_AlarmExist(bAlarmExist);
	setWordOut_Cell_Trigger_ID(nID);
	setWordOut_Judge(nJudge);
	setWordOut_NG_Code(nNgCode);
	return 0; 
}

//통합비전 Cell Key Id를 가져온다.
int CMelsecPlcIo::GetCellKey(int num)
{
	return getWordIn_CellKey(num);
}

//마킹 설정 값 변수
BOOL CMelsecPlcIo::GetInkMarkActive()
{
	return getBitIn_InkMarkingActive();
}
BOOL CMelsecPlcIo::GetConnectZone()
{
	return getBitIn_ConnectZone();
}
int CMelsecPlcIo::SigOutAlivePulseReady(int nInMode, BOOL bIsReady)
{
	m_bSmsAlive ^= 0x1;
	setBitOut_Alive(m_bSmsAlive);
	setBitOut_Ready(bIsReady);
	return 0; 
}
int CMelsecPlcIo::SigOutAlarmExist(int nMode)
{
	setWordOut_AlarmExist(nMode);
	return 0; 
}
void CMelsecPlcIo::EnableWorkSet(BOOL bMode)
{
	m_bEnableWorkSet = bMode;
}
int CMelsecPlcIo::SigOutDiskCapacityAlarm(int nMode)
{ 
	setBitOut_DiskSpaceAlarm(nMode);
	return 0; 
}
int CMelsecPlcIo::SigOutDiskCapacityWarning(int nMode)
{ 
	setBitOut_DiskSpaceWarning(nMode);
	return 0;
}

int CMelsecPlcIo::SignalBitOut(int nIntegration, int nMode, BOOL bLocal)
{
	return 0;
}

//Lot End 처리 함수
void CMelsecPlcIo::SigOutLotEnd(int TopDefectCnt, int BtmDefectCnt)
{

}

//In
int CMelsecPlcIo::SigInReady()
{ 
	return getBitIn_Ready();
}
int CMelsecPlcIo::SigInRun()
{
	return getBitIn_Run();
}
int CMelsecPlcIo::SigInLotEnd()
{ 
	return getBitIn_LotEndReq();
}
int CMelsecPlcIo::SigInTabZeroReset()
{ 
	return getBitIn_TabZeroReset();
}
int CMelsecPlcIo::SigInLotStart()
{
	return getBitIn_LotStartReq();
}
int CMelsecPlcIo::SigInAlarmReset()
{ 
	return getBitIn_AlarmResetReq();
}
int CMelsecPlcIo::SigInAlarmNgAck()
{
	return getBitIn_AlarmNgAck();
}
int CMelsecPlcIo::SigInRecipeChange()
{
	return getBitIn_RecipeChange();
}
int CMelsecPlcIo::SigInInkMarkActive()
{ 
	return getBitIn_InkMarkingActive();
}
int CMelsecPlcIo::SigInConnectZone()
{
	return getBitIn_ConnectZone();
}

//임시
int CMelsecPlcIo::ReadBlockAllData(CSequenceData* pSeqData)
{ 
	pSeqData->wRecipeNo = getWordIn_RecipeNo();
	pSeqData->strRecipeName = getWordIn_RecipeName();
	pSeqData->strCell_ID = getWordIn_CELL_ID();

	pSeqData->wFoilExpInTopTarget = getWordIn_FoilExpInTopTarget();
	pSeqData->wFoilExpInBottomTarget = getWordIn_FoilExpInBtmTarget();
	pSeqData->wFoilExpOutTopTarget = getWordIn_FoilExpOutTopTarget();
	pSeqData->wFoilExpOutBottomTarget = getWordIn_FoilExpOutBtmTarget();
	pSeqData->wFoilExpBothTopTarget = getWordIn_FoilExpBothTopTarget();
	pSeqData->wFoilExpBothBottomTarget = getWordIn_FoilExpBothBtmTarget();
	pSeqData->wSpeterTopTarget = getWordIn_SpeterTopTarget();
	pSeqData->wSpeterBottomTarget = getWordIn_SpeterBtmTarget();

	pSeqData->wAlarmExistAck = getWordOut_AlarmExist(false);

	pSeqData->wContinousCount = getWordIn_PrmContinuousCnt();
	pSeqData->wSectorNgCount = getWordIn_PrmSectorNgTabCnt();
	pSeqData->wSectorBaseCount = getWordIn_PrmSectorBaseCnt();

	return 0;
}

int CMelsecPlcIo::WriteBlockData(void* pGlobalData)
{
	CGlobalData* pGData = (CGlobalData*)pGlobalData;
	// 검사 수량
	setWordOut_DataReportV1_Ea((WORD)pGData->m_NowLotData.m_SeqDataOut.dwDataReportV1);
	// OK 수량
	setWordOut_DataReportV2_OK((WORD)pGData->m_NowLotData.m_SeqDataOut.dwDataReportV2);
	// NG 수량
	setWordOut_DataReportV3_NG((WORD)pGData->m_NowLotData.m_SeqDataOut.dwDataReportV3);
	// 양품률
	setWordOut_DataReportV4_OkRate((WORD)pGData->m_NowLotData.m_SeqDataOut.dwDataReportV4);
	// 불량률
	setWordOut_DataReportV5_NgRate((WORD)pGData->m_NowLotData.m_SeqDataOut.dwDataReportV5);
	// 가동률
	setWordOut_DataReportV6_RunRate((WORD)pGData->m_NowLotData.m_SeqDataOut.dwDataReportV6);

	// 연속 알람 발생 수량
	setWordOut_Continue_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwContinueAlarmCount);
	// 구간 알람 발생 수량
	setWordOut_Heavy_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwSectorAlarmCount);
	setWordOut_FoilExpInTop_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpInTopCount);
	setWordOut_FoilExpInBtm_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpInBottomCount);
	setWordOut_FoilExpOutTop_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpOutTopCount);
	setWordOut_FoilExpOutBtm_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpOutBottomCount);
	setWordOut_FoilExpBothTop_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpBothTopCount);
	setWordOut_FoilExpBothBtm_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpBothBottomCount);
	setWordOut_SpeterTop_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwSpeterTopCount);
	setWordOut_SpeterBtm_Alarm_Cnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwSpeterBottomCount);

	// 실시간 Tob NG Count
	setWordOut_Top_Defect_Count_Real((WORD)pGData->m_NowLotData.m_SeqDataOut.dwTopNgRealTimeCount);
	// 실시간 Bottom NG Count
	setWordOut_Btm_Defect_Count_Real((WORD)pGData->m_NowLotData.m_SeqDataOut.dwBottomNgRealTimeCount);
	// Lot End 시
	setWordOut_Top_Defect_Count_LotEnd((WORD)pGData->m_NowLotData.m_SeqDataOut.dwTopNgLotEndCount);
	// Lot End 시
	setWordOut_Btm_Defect_Count_LotEnd((WORD)pGData->m_NowLotData.m_SeqDataOut.dwBottomNgLotEndCount);

	setWordOut_FoilExpInTopTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpInTopTarget);
	setWordOut_FoilExpInBtmTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpInBottomTarget);
	setWordOut_FoilExpOutTopTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpOutTopTarget);
	setWordOut_FoilExpOutBtmTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpOutBottomTarget);
	setWordOut_FoilExpBothTopTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpBothTopTarget);
	setWordOut_FoilExpBothBtmTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwFoilExpBothBottomTarget);
	setWordOut_SpeterTopTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwSpeterTopTarget);
	setWordOut_SpeterBtmTarget((WORD)pGData->m_NowLotData.m_SeqDataOut.dwSpeterBottomTarget);

	setWordOut_PrmContinuousCnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwPrmContinuousCnt);
	setWordOut_PrmSectorNgTabCnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwPrmSectorNgTabCnt);
	setWordOut_PrmSectorBaseCnt((WORD)pGData->m_NowLotData.m_SeqDataOut.dwPrmSectorBaseCnt);

	return 0;
}

int CMelsecPlcIo::ReadAllPort_BitIn(BOOL* pSigBitIn)
{ 

	return 0; 
}
int CMelsecPlcIo::ReadAllPort_BitOut(BOOL* pSigBitOut)
{ 

	return 0; 
}

CString CMelsecPlcIo::GetInWordData(int idx)
{
	CString str = _T("");

	return str;
}
CString CMelsecPlcIo::GetOutWordData(int idx)
{
	CString str = _T("");

	return str;
}