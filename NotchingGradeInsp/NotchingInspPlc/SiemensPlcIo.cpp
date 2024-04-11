#include "pch.h"
#include "SiemensPlcIo.h"
#include "GlobalData.h"
#include "LogDisplayDlg.h"


// [ Siemens Address Start ]
enum SmsBitIn
{
	enSmsBitIn_Alive = 0,
	enSmsBitIn_Ready = 1,
	enSmsBitIn_Run = 2,
	enSmsBitIn_EncoderReset = 3,
	enSmsBitIn_TabZeroReset = 4,
	enSmsBitIn_InkMarkingActive = 5,
	enSmsBitIn_ConnectZone = 6,
	enSmsBitIn_RecipeChange = 7,
	enSmsBitIn_LotStartReq = 8,
	enSmsBitIn_LotEndReq = 9,
	enSmsBitIn_AlarmResetReq = 10,
	enSmsBitIn_AlarmNgAck = 11
};
enum SmsBitOut
{
	enSmsBitOut_Alive = 0,
	enSmsBitOut_Ready = 1,
	enSmsBitOut_EncoderSet = 2,
	enSmsBitOut_RecipeChangeAck = 3,
	enSmsBitOut_LotStartReqAck = 4,
	enSmsBitOut_LotEndReqAck = 5,
	enSmsBitOut_TabZeroReset = 6,
	enSmsBitOut_AlarmResetAck = 7,
	enSmsBitOut_AlarmNgResetAck = 8,

	enSmsBitOut_DiskSpaceWarning = 9,
	enSmsBitOut_DiskSpaceAlarm = 10,
};
// Siemens Address End
// Siemens Word Address Start
enum SmsWordRead
{
	enSmsWordRead_RecipeNo = 0, 
	enSmsWordRead_RecipeName = 1, 
	enSmsWordRead_CELL_ID = 5, 
	enSmsWordRead_FoilExpInTopTarget = 20, 
	enSmsWordRead_FoilExpInBtmTarget = 21, 
	enSmsWordRead_FoilExpOutTopTarget = 22, 
	enSmsWordRead_FoilExpOutBtmTarget = 23, 
	enSmsWordRead_FoilExpBothTopTarget = 24, 
	enSmsWordRead_FoilExpBothBtmTarget = 25, 
	enSmsWordRead_SpeterTopTarget = 26, 
	enSmsWordRead_SpeterBtmTarget = 27, 

	enSmsWordRead_PrmContinuousCnt = 30, 
	enSmsWordRead_PrmSectorNgTabCnt = 31, 
	enSmsWordRead_PrmSectorBaseCnt = 32, 

	//Cell Key
	enSmsWordRead_CellKey = 210,
};

enum SmsWordWrite
{
	enSmsWordWrite_DataReportV1_Ea = 0, 
	enSmsWordWrite_DataReportV2_OK = 1, 
	enSmsWordWrite_DataReportV3_NG = 2, 
	enSmsWordWrite_DataReportV4_OkRate = 3, 
	enSmsWordWrite_DataReportV5_NgRate = 4, 
	enSmsWordWrite_DataReportV6_RunRate = 5, 

	enSmsWordWrite_Continue_Alarm_Cnt = 6, 
	enSmsWordWrite_Heavy_Alarm_Cnt = 7, 
	enSmsWordWrite_FoilExpInTop_Alarm_Cnt = 8, 
	enSmsWordWrite_FoilExpInBtm_Alarm_Cnt = 9, 
	enSmsWordWrite_FoilExpOutTop_Alarm_Cnt = 10, 
	enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt = 11, 
	enSmsWordWrite_FoilExpBothTop_Alarm_Cnt = 12, 
	enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt = 13, 
	enSmsWordWrite_SpeterTop_Alarm_Cnt = 14, 
	enSmsWordWrite_SpeterBtm_Alarm_Cnt = 15, 

	enSmsWordWrite_Top_Defect_Count_Real = 16, 
	enSmsWordWrite_Btm_Defect_Count_Real = 17, 
	enSmsWordWrite_Top_Defect_Count_LotEnd = 18, 
	enSmsWordWrite_Btm_Defect_Count_LotEnd = 19, 

	enSmsWordWrite_FoilExpInTopTarget = 20, 
	enSmsWordWrite_FoilExpInBtmTarget = 21, 
	enSmsWordWrite_FoilExpOutTopTarget = 22, 
	enSmsWordWrite_FoilExpOutBtmTarget = 23, 
	enSmsWordWrite_FoilExpBothTopTarget = 24, 
	enSmsWordWrite_FoilExpBothBtmTarget = 25, 
	enSmsWordWrite_SpeterTopTarget = 26, 
	enSmsWordWrite_SpeterBtmTarget = 27, 

	enSmsWordWrite_PrmContinuousCnt = 28, 
	enSmsWordWrite_PrmSectorNgTabCnt = 29, 
	enSmsWordWrite_PrmSectorBaseCnt = 30, 

	enSmsWordWrite_AlarmExist = 40, 
	enSmsWordWrite_AlarmCode_Buffer = 41, 

	enSmsWordWrite_Cell_Trigger_ID = 80,
	enSmsWordWrite_Judge = 81,
	enSmsWordWrite_NG_Code = 82,

	enSmsWordWrite_DuplicateNG_Cell_ID = 85,

	enSmsWordWriteMaxSize = 149,

};

CSiemensPlcIo::CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort)
	: m_strIPAddress(strIPAddress)
	, m_nPort(nPort)
	, m_nReConnetTimeOut(nReConnetTimeOut)
	, m_pReceiveMsgWnd(pReceiveMsgWnd)
	, m_pLGIS_Plc(NULL)
{
	//Alive 값 연산 변수
	m_bSmsAlive = FALSE;

	m_pThread_SiemensPlc = NULL;
	pEvent_SiemensPlc = NULL;
	//Read Data 버퍼 초기화
	memset(m_ReadBitData, 0, sizeof(short) * SIENENS_READBIT);
	memset(m_ReadWordData, 0, sizeof(short) * SIENENS_READWORD_MAX);

	//Write Data 버퍼 초기화
	memset(m_WriteData, 0, sizeof(short) * SIENENS_WRITEBITWORD_MAX);

	//지멘스 Plc 연결 및 데이터 처리 스래드 객체 생성
	OpenPlcIo();
}


CSiemensPlcIo::~CSiemensPlcIo()
{
	ClosePlcIo();
	if (m_pThread_SiemensPlc)
	{
		setEvent_SiemensPlc();
		CGlobalFunc::ThreadExit(&m_pThread_SiemensPlc->m_hThread, 5000);
		m_pThread_SiemensPlc->m_hThread = NULL;
		m_pThread_SiemensPlc = NULL;
	}

	if (pEvent_SiemensPlc)
	{
		CloseHandle(pEvent_SiemensPlc);
		pEvent_SiemensPlc = NULL;
	}
}

//스래드 함수
#define SIEMENSPLC_TIMEOUT 500
UINT CSiemensPlcIo::SiemensPlc_ThreadProc(LPVOID param)
{
	CSiemensPlcIo* pMain = (CSiemensPlcIo*)param;

	int ret = WAIT_OBJECT_0;
	while (pMain)
	{
		//타임 주기 이벤트
		ret = WaitForSingleObject(pMain->getEvent_SiemensPlc(), SIEMENSPLC_TIMEOUT);

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			break;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			pMain->SiemensPlcProc();
		}
		else
		{
			break;
		}

	}
	AfxEndThread(0);
	return 0;
}

//스래드에서 호출하는 함수
void CSiemensPlcIo::SiemensPlcProc()
{
	if (IsOpened())
	{
		//bit Word 전체 버퍼
		short	ReadBitWordData[SIENENS_READBITWORD_MAX];
		memset(ReadBitWordData, 0, SIENENS_READBITWORD_MAX);

		//Read bit 영역 읽기
		short	ReadBitData[SIENENS_READBIT];
		//Read word 영역 읽기
		short	ReadWordData[SIENENS_READWORD_MAX];
		//bit 읽기 영역 읽기
		ReadDataReg(AprData.m_System.m_nBitIn, ReadBitWordData, SIENENS_READBITWORD_MAX);

		//읽은 Bit 데이터 파싱
		memcpy(ReadBitData, &ReadBitWordData[0], sizeof(short) * SIENENS_READBIT);
		if (std::equal(std::begin(ReadBitData), std::end(ReadBitData), std::begin(m_ReadBitData)) == false)
		{
			ReadPlcBitDataParser(ReadBitData);
			LOGDISPLAY_SPEC(2)(_T("In bit data :	%s"), CStrSuport::ChangshorttohexTab(ReadBitData, SIENENS_READBIT));
		}

		//word 읽기 영역 읽기
		memcpy(ReadWordData, &ReadBitWordData[SIENENS_READBIT], sizeof(short) * SIENENS_READWORD_MAX);
		//읽은 Word 데이터 파싱
		if (std::equal(std::begin(ReadWordData), std::end(ReadWordData), std::begin(m_ReadWordData)) == false)
		{
			ReadPlcWordDataParser(ReadWordData);
			LOGDISPLAY_SPEC(2)(_T("In word data :	%s"), CStrSuport::ChangshorttohexTab(ReadWordData, SIENENS_READWORD_MAX));
		}

		//쓰기 데이터 만들기
		//ret : 쓰기 영역에 어디까지 데이터가 바뀌었는가 값 
		//m_WriteData에 쓰여진 데이터에서 바뀐 영역까지 만 쓰기 : Bit 영역에서 한번에 연속 되는 부분까지 
		int ret = WritePlcDataMake();
		//쓰기
		WriteDataReg(AprData.m_System.m_nBitOut, m_WriteData, ret);
		LOGDISPLAY_SPEC(2)(_T("Out data :	%s"), CStrSuport::ChangshorttohexTab(m_WriteData, ret));
	}
}

//PLC read Data Parser 함수
void CSiemensPlcIo::ReadPlcBitDataParser(short* data)
{
	if (m_ReadBitData[enSmsBitIn_Alive] ^ data[enSmsBitIn_Alive]) setBitIn_Alive(data[enSmsBitIn_Alive] & 0x1);
	if (m_ReadBitData[enSmsBitIn_Ready] ^ data[enSmsBitIn_Ready]) setBitIn_Ready(data[enSmsBitIn_Ready] & 0x1);
	if (m_ReadBitData[enSmsBitIn_Run] ^ data[enSmsBitIn_Run]) setBitIn_Run(data[enSmsBitIn_Run] & 0x1);
	if (m_ReadBitData[enSmsBitIn_EncoderReset] ^ data[enSmsBitIn_EncoderReset]) setBitIn_EncoderReset(data[enSmsBitIn_EncoderReset] & 0x1);
	if (m_ReadBitData[enSmsBitIn_TabZeroReset] ^ data[enSmsBitIn_TabZeroReset]) setBitIn_TabZeroReset(data[enSmsBitIn_TabZeroReset] & 0x1);
	if (m_ReadBitData[enSmsBitIn_InkMarkingActive] ^ data[enSmsBitIn_InkMarkingActive]) setBitIn_InkMarkingActive(data[enSmsBitIn_InkMarkingActive] & 0x1);
	if (m_ReadBitData[enSmsBitIn_ConnectZone] ^ data[enSmsBitIn_ConnectZone]) setBitIn_ConnectZone(data[enSmsBitIn_ConnectZone] & 0x1);
	if (m_ReadBitData[enSmsBitIn_RecipeChange] ^ data[enSmsBitIn_RecipeChange]) setBitIn_RecipeChange(data[enSmsBitIn_RecipeChange] & 0x1);
	if (m_ReadBitData[enSmsBitIn_LotStartReq] ^ data[enSmsBitIn_LotStartReq]) setBitIn_LotStartReq(data[enSmsBitIn_LotStartReq] & 0x1);
	if (m_ReadBitData[enSmsBitIn_LotEndReq] ^ data[enSmsBitIn_LotEndReq]) setBitIn_LotEndReq(data[enSmsBitIn_LotEndReq] & 0x1);
	if (m_ReadBitData[enSmsBitIn_AlarmResetReq] ^ data[enSmsBitIn_AlarmResetReq]) setBitIn_AlarmResetReq(data[enSmsBitIn_AlarmResetReq] & 0x1);
	if (m_ReadBitData[enSmsBitIn_AlarmNgAck] ^ data[enSmsBitIn_AlarmNgAck]) setBitIn_AlarmNgAck(data[enSmsBitIn_AlarmNgAck] & 0x1);

	memcpy(m_ReadBitData, data, sizeof(short) * SIENENS_READBIT);
}

CString CSiemensPlcIo::MakeRecipeName(short* data)
{
	int i;
	int Cnt = 0;
	CString strBuffer;
	int nMax = SIEMENS_READRECIPENAME;

	for (i = 0, Cnt = 0; i < nMax; i++, Cnt += 2)
	{
		BYTE btTemp = (BYTE)(*data & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt(Cnt, (char)btTemp);

		btTemp = (BYTE)((*data >> 8) & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt((Cnt + 1), (char)btTemp);

		data++;
	}
	strBuffer.TrimRight();
	strBuffer.TrimLeft();

	LOGDISPLAY_SPEC(2)(_T("RecipeName :	%s"), strBuffer);

	return strBuffer;
}

//CELL ID를 만든다.
CString CSiemensPlcIo::MakeCellId(short* data)
{
	int i;
	int Cnt = 0;
	CString strBuffer;
	int nMax = SIEMENS_READCELLID;

	for (i = 0, Cnt = 0; i < nMax; i++, Cnt += 2)
	{
		BYTE btTemp = (BYTE)(*data & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt(Cnt, (char)btTemp);

		btTemp = (BYTE)((*data >> 8) & 0xff);
		strBuffer += _T(" ");
		strBuffer.SetAt((Cnt + 1), (char)btTemp);

		data++;
	}

	strBuffer.TrimRight();
	strBuffer.TrimLeft();

	LOGDISPLAY_SPEC(2)(_T("Cell Id :	%s"), strBuffer);

	return strBuffer;
}

void CSiemensPlcIo::ReadPlcWordDataParser(short* data)
{
	if (m_ReadWordData[enSmsWordRead_RecipeNo] ^ data[enSmsWordRead_RecipeNo]) setWordIn_RecipeNo(data[enSmsWordRead_RecipeNo]);
	if ((m_ReadWordData[enSmsWordRead_RecipeName] ^ data[enSmsWordRead_RecipeName]) |
		(m_ReadWordData[enSmsWordRead_RecipeName + 1] ^ data[enSmsWordRead_RecipeName + 1]) |
		(m_ReadWordData[enSmsWordRead_RecipeName + 2] ^ data[enSmsWordRead_RecipeName + 2]) |
		(m_ReadWordData[enSmsWordRead_RecipeName + 3] ^ data[enSmsWordRead_RecipeName + 3]))
		setWordIn_RecipeName(MakeRecipeName(&data[enSmsWordRead_RecipeName]));

	if ((m_ReadWordData[enSmsWordRead_CELL_ID] ^ data[enSmsWordRead_CELL_ID]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 1] ^ data[enSmsWordRead_CELL_ID + 1]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 2] ^ data[enSmsWordRead_CELL_ID + 2]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 3] ^ data[enSmsWordRead_CELL_ID + 3]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 4] ^ data[enSmsWordRead_CELL_ID + 4]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 5] ^ data[enSmsWordRead_CELL_ID + 5]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 6] ^ data[enSmsWordRead_CELL_ID + 6]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 7] ^ data[enSmsWordRead_CELL_ID + 7]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 8] ^ data[enSmsWordRead_CELL_ID + 8]) |
		(m_ReadWordData[enSmsWordRead_CELL_ID + 9] ^ data[enSmsWordRead_CELL_ID + 9]))
		setWordIn_CELL_ID(MakeCellId(&data[enSmsWordRead_CELL_ID]));
	if (m_ReadWordData[enSmsWordRead_FoilExpInTopTarget] ^ data[enSmsWordRead_FoilExpInTopTarget]) setWordIn_FoilExpInTopTarget(data[enSmsWordRead_FoilExpInTopTarget]);
	if (m_ReadWordData[enSmsWordRead_FoilExpInBtmTarget] ^ data[enSmsWordRead_FoilExpInBtmTarget]) setWordIn_FoilExpInBtmTarget(data[enSmsWordRead_FoilExpInBtmTarget]);

	if (m_ReadWordData[enSmsWordRead_FoilExpOutTopTarget] ^ data[enSmsWordRead_FoilExpOutTopTarget]) setWordIn_FoilExpOutTopTarget(data[enSmsWordRead_FoilExpOutTopTarget]);
	if (m_ReadWordData[enSmsWordRead_FoilExpOutBtmTarget] ^ data[enSmsWordRead_FoilExpOutBtmTarget]) setWordIn_FoilExpOutBtmTarget(data[enSmsWordRead_FoilExpOutBtmTarget]);

	if (m_ReadWordData[enSmsWordRead_FoilExpBothTopTarget] ^ data[enSmsWordRead_FoilExpBothTopTarget]) setWordIn_FoilExpBothTopTarget(data[enSmsWordRead_FoilExpBothTopTarget]);
	if (m_ReadWordData[enSmsWordRead_FoilExpBothBtmTarget] ^ data[enSmsWordRead_FoilExpBothBtmTarget]) setWordIn_FoilExpBothBtmTarget(data[enSmsWordRead_FoilExpBothBtmTarget]);

	if (m_ReadWordData[enSmsWordRead_SpeterTopTarget] ^ data[enSmsWordRead_SpeterTopTarget]) setWordIn_SpeterTopTarget(data[enSmsWordRead_SpeterTopTarget]);
	if (m_ReadWordData[enSmsWordRead_SpeterBtmTarget] ^ data[enSmsWordRead_SpeterBtmTarget]) setWordIn_SpeterBtmTarget(data[enSmsWordRead_SpeterBtmTarget]);

	if (m_ReadWordData[enSmsWordRead_PrmContinuousCnt] ^ data[enSmsWordRead_PrmContinuousCnt]) setWordIn_PrmContinuousCnt(data[enSmsWordRead_PrmContinuousCnt]);
	if (m_ReadWordData[enSmsWordRead_PrmSectorNgTabCnt] ^ data[enSmsWordRead_PrmSectorNgTabCnt]) setWordIn_PrmSectorNgTabCnt(data[enSmsWordRead_PrmSectorNgTabCnt]);
	if (m_ReadWordData[enSmsWordRead_PrmSectorBaseCnt] ^ data[enSmsWordRead_PrmSectorBaseCnt]) setWordIn_PrmSectorBaseCnt(data[enSmsWordRead_PrmSectorBaseCnt]);

	for (int i = 0; i < COUNT_CELLKEY; i++)
	{
		int idx = enSmsWordRead_CellKey + i;
		if (m_ReadWordData[idx] ^ data[idx]) setWordIn_CellKey(i, data[idx]);
	}

	memcpy(m_ReadWordData, data, sizeof(short) * SIENENS_READWORD_MAX);
}

//PLC write Data Make 함수
int CSiemensPlcIo::WritePlcDataMake()
{
	//Write Data Block Num
	int ret = 0;
	if (isBitOut_Alive())
	{ret = enSmsBitOut_Alive; m_WriteData[ret] = getBitOut_Alive(); }

	if (isBitOut_Ready())
	{ret = enSmsBitOut_Ready; m_WriteData[ret] = getBitOut_Ready(); }

	if (isBitOut_EncoderSet())
	{ret = enSmsBitOut_EncoderSet; m_WriteData[ret] = getBitOut_EncoderSet(); }

	if (isBitOut_RecipeChangeAck())
	{ret = enSmsBitOut_RecipeChangeAck; m_WriteData[ret] = getBitOut_RecipeChangeAck(); }

	if (isBitOut_LotStartReqAck())
	{ret = enSmsBitOut_LotStartReqAck; m_WriteData[ret] = getBitOut_LotStartReqAck(); }

	if (isBitOut_LotEndReqAck())
	{ret = enSmsBitOut_LotEndReqAck; m_WriteData[ret] = getBitOut_LotEndReqAck(); }

	if (isBitOut_TabZeroReset())
	{ret = enSmsBitOut_TabZeroReset; m_WriteData[ret] = getBitOut_TabZeroReset(); }

	if (isBitOut_AlarmResetAck())
	{ret = enSmsBitOut_AlarmResetAck; m_WriteData[ret] = getBitOut_AlarmResetAck(); }

	if (isBitOut_AlarmNgResetAck())
	{ret = enSmsBitOut_AlarmNgResetAck; m_WriteData[ret] = getBitOut_AlarmNgResetAck(); }

	if (isBitOut_DiskSpaceWarning())
	{ret = enSmsBitOut_DiskSpaceWarning; m_WriteData[ret] = getBitOut_DiskSpaceWarning(); }

	if (isBitOut_DiskSpaceAlarm())
	{ret = enSmsBitOut_DiskSpaceAlarm; m_WriteData[ret] = getBitOut_DiskSpaceAlarm(); }

	if (isWordOut_DataReportV1_Ea())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_DataReportV1_Ea;  m_WriteData[ret] = getWordOut_DataReportV1_Ea(); }

	if (isWordOut_DataReportV2_OK())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_DataReportV2_OK; m_WriteData[ret] = getWordOut_DataReportV2_OK(); }

	if (isWordOut_DataReportV3_NG())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_DataReportV3_NG; m_WriteData[ret] = getWordOut_DataReportV3_NG(); }

	if (isWordOut_DataReportV4_OkRate())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_DataReportV4_OkRate; m_WriteData[ret] = getWordOut_DataReportV4_OkRate(); }

	if (isWordOut_DataReportV5_NgRate())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_DataReportV5_NgRate; m_WriteData[ret] = getWordOut_DataReportV5_NgRate(); }

	if (isWordOut_DataReportV6_RunRate())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_DataReportV6_RunRate; m_WriteData[ret] = getWordOut_DataReportV6_RunRate(); }

	if (isWordOut_Continue_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Continue_Alarm_Cnt; m_WriteData[ret] = getWordOut_Continue_Alarm_Cnt(); }

	if (isWordOut_Heavy_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Heavy_Alarm_Cnt; m_WriteData[ret] = getWordOut_Heavy_Alarm_Cnt(); }

	if (isWordOut_FoilExpInTop_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpInTop_Alarm_Cnt;m_WriteData[ret] = getWordOut_FoilExpInTop_Alarm_Cnt();  }

	if (isWordOut_FoilExpInBtm_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpInBtm_Alarm_Cnt; m_WriteData[ret] = getWordOut_FoilExpInBtm_Alarm_Cnt(); }

	if (isWordOut_FoilExpOutTop_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpOutTop_Alarm_Cnt; m_WriteData[ret] = getWordOut_FoilExpOutTop_Alarm_Cnt(); }

	if (isWordOut_FoilExpOutBtm_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt; m_WriteData[ret] = getWordOut_FoilExpOutBtm_Alarm_Cnt(); }

	if (isWordOut_FoilExpBothTop_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpBothTop_Alarm_Cnt; m_WriteData[ret] = getWordOut_FoilExpBothTop_Alarm_Cnt(); }

	if (isWordOut_FoilExpBothBtm_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt; m_WriteData[ret] = getWordOut_FoilExpBothBtm_Alarm_Cnt(); }

	if (isWordOut_SpeterTop_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_SpeterTop_Alarm_Cnt; m_WriteData[ret] = getWordOut_SpeterTop_Alarm_Cnt(); }

	if (isWordOut_SpeterBtm_Alarm_Cnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_SpeterBtm_Alarm_Cnt; m_WriteData[ret] = getWordOut_SpeterBtm_Alarm_Cnt(); }

	if (isWordOut_Top_Defect_Count_Real())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Top_Defect_Count_Real; m_WriteData[ret] = getWordOut_Top_Defect_Count_Real(); }

	if (isWordOut_Btm_Defect_Count_Real())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Btm_Defect_Count_Real; m_WriteData[ret] = getWordOut_Btm_Defect_Count_Real(); }

	if (isWordOut_Top_Defect_Count_LotEnd())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Top_Defect_Count_LotEnd; m_WriteData[ret] = getWordOut_Top_Defect_Count_LotEnd(); }

	if (isWordOut_Btm_Defect_Count_LotEnd())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Btm_Defect_Count_LotEnd; m_WriteData[ret] = getWordOut_Btm_Defect_Count_LotEnd(); }

	if (isWordOut_FoilExpInTopTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpInTopTarget; m_WriteData[ret] = getWordOut_FoilExpInTopTarget(); }

	if (isWordOut_FoilExpInBtmTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpInBtmTarget; m_WriteData[ret] = getWordOut_FoilExpInBtmTarget(); }

	if (isWordOut_FoilExpOutTopTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpOutTopTarget; m_WriteData[ret] = getWordOut_FoilExpOutTopTarget(); }

	if (isWordOut_FoilExpOutBtmTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpOutBtmTarget; m_WriteData[ret] = getWordOut_FoilExpOutBtmTarget(); }

	if (isWordOut_FoilExpBothTopTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpBothTopTarget; m_WriteData[ret] = getWordOut_FoilExpBothTopTarget(); }

	if (isWordOut_FoilExpBothBtmTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_FoilExpBothBtmTarget; m_WriteData[ret] = getWordOut_FoilExpBothBtmTarget(); }

	if (isWordOut_SpeterTopTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_SpeterTopTarget; m_WriteData[ret] = getWordOut_SpeterTopTarget(); }

	if (isWordOut_SpeterBtmTarget())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_SpeterBtmTarget; m_WriteData[ret] = getWordOut_SpeterBtmTarget(); }

	if (isWordOut_PrmContinuousCnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_PrmContinuousCnt; m_WriteData[ret] = getWordOut_PrmContinuousCnt(); }

	if (isWordOut_PrmSectorNgTabCnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_PrmSectorNgTabCnt; m_WriteData[ret] = getWordOut_PrmSectorNgTabCnt(); }

	if (isWordOut_PrmSectorBaseCnt())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_PrmSectorBaseCnt; m_WriteData[ret] = getWordOut_PrmSectorBaseCnt(); }

	if (isWordOut_AlarmExist())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_AlarmExist; m_WriteData[ret] = getWordOut_AlarmExist(); }

	if (isWordOut_AlarmCode_Buffer())
	{
		ret = SIENENS_WRITEBIT + enSmsWordWrite_AlarmCode_Buffer;
		for (int i = 0; i < COUNT_ALRAMBUFF; i++)
		{
			ret += i;
			m_WriteData[ret] = getWordOut_AlarmCode_Buffer(i);
		}
	}

	if (isWordOut_Cell_Trigger_ID())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Cell_Trigger_ID; m_WriteData[ret] = getWordOut_Cell_Trigger_ID(); }

	if (isWordOut_Judge())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_Judge; m_WriteData[ret] = getWordOut_Judge(); }

	if (isWordOut_NG_Code())
	{ret = SIENENS_WRITEBIT + enSmsWordWrite_NG_Code; m_WriteData[ret] = getWordOut_Judge(); }

	if (isWordOut_DuplicateNG_Cell_ID())
	{
		ret = SIENENS_WRITEBIT + enSmsWordWrite_DuplicateNG_Cell_ID;
		for (int i = 0; i < COUNT_DUPLICATENGCELLID; i++)
		{
			ret += i;
			m_WriteData[ret] = getWordOut_DuplicateNG_Cell_ID(i);
		}
	}

	//버퍼 block 위치에 + 1 = 크기(size)
	//쓰기 영역에 쓸 데이터 크기
	return ret+1;
}

void CSiemensPlcIo::SetSlaveId(int nId)
{
	if (m_pLGIS_Plc)
	{
		m_pLGIS_Plc->SetSlaveId(nId);
	}
}

void CSiemensPlcIo::SetReceiveMsgWnd(CWnd* pWnd)
{
	if (m_pLGIS_Plc)
	{
		m_pReceiveMsgWnd = pWnd;
		m_pLGIS_Plc->SetReceiveMsgWnd(m_pReceiveMsgWnd);
	}

}

void CSiemensPlcIo::SetReConnetTimeOut(int nTimeOut)
{
	if (m_pLGIS_Plc)
	{
		m_nReConnetTimeOut = nTimeOut;
		m_pLGIS_Plc->SetReConnetTimeOut(m_nReConnetTimeOut);
	}
}


int CSiemensPlcIo::WriteDataReg(int offset, short data[], int num)
{
	CString strMsg;

	int nRet = 0;
	int nAdd = offset;// +m_wOffset_Out; //+ m_wOffsetWord_Out;
	{	
		if (m_pLGIS_Plc)
		{
			nRet = m_pLGIS_Plc->WriteMultipleRegisters(nAdd, num, (uint16_t*)data);

			if (nRet < 0)
			{
				//strMsg.Format(_T("Siemens Write Error"));

			}

		}
		else
		{
			//strMsg.Format(_T("Siemens Open Error"));

		}

	}

	return nRet ;
}

int CSiemensPlcIo::ReadDataReg(int offset, short data[], int num)
{
	int nRet = 0;
	int nAdd = offset;// +m_wOffsetWord_In;
	{

 		CString strMsg;
		if (m_pLGIS_Plc)
		{
			nRet = m_pLGIS_Plc->ReadHoldingRegisters(nAdd, num, (uint16_t*)data);

			if( nRet < 0)
			{
				//strMsg.Format(_T("Siemens Read Error"));
			}
		}
		else
		{
			//strMsg.Format(_T("Siemens Open Error"));
		}

	}
	return nRet;
}

BOOL CSiemensPlcIo::IsOpened()
{
	if (m_pLGIS_Plc)
		return m_pLGIS_Plc->CheckConnection();

	return false;
}

CString CSiemensPlcIo::GetErrorMsg()
{
	if (IsOpened())
	{
		if (m_pLGIS_Plc->m_bErr)
			return m_pLGIS_Plc->m_strErrorMsg;
		else
			return CString();
	}
	else
	{
		if (m_pLGIS_Plc)
			return m_pLGIS_Plc->m_strErrorMsg;
	}

	return _T("Not created PLC handle");
}

int CSiemensPlcIo::GetErrorNo()
{
	if (IsOpened())
	{
		if (m_pLGIS_Plc->m_bErr)
			return m_pLGIS_Plc->m_nErrNo;
		else
			PLC_OK;
	}
	else
	{
		if (m_pLGIS_Plc)
			return m_pLGIS_Plc->m_nErrNo;
	}

	return CONNECTION_ERROR;
}

int CSiemensPlcIo::OpenPlcIo(void)
{
	int ret = 0;

	m_pLGIS_Plc = new CLGIS_Plc(m_strIPAddress, m_nPort, m_nReConnetTimeOut, m_pReceiveMsgWnd );
	//로그출력
	LOGDISPLAY_SPEC(2)("PLC Info	Address : %s	Port : %d", m_strIPAddress, m_nPort);

	if (!m_pLGIS_Plc->CheckConnection())
	{
		ClosePlcIo();
		//로그출력
		LOGDISPLAY_SPEC(2)("PLC Siemens Open failed");
		ret = -1;
	}
	else
	{
		//로그출력
		LOGDISPLAY_SPEC(2)("PLC Siemens Open success Slave Id : %s", AprData.m_System.m_nSlaveID);

		//슬레이브 아이디 
		SetSlaveId(AprData.m_System.m_nSlaveID);
		//이벤트 객체 생성
		pEvent_SiemensPlc = CreateEvent(NULL, FALSE, FALSE, NULL);
		//스래드 생성
		m_pThread_SiemensPlc = AfxBeginThread(SiemensPlc_ThreadProc, this);
	}

	return ret;
}

void CSiemensPlcIo::ClosePlcIo(void)
{
	if (m_pLGIS_Plc)
	{
		delete m_pLGIS_Plc;
		m_pLGIS_Plc = NULL;
	}
}

//Out
int CSiemensPlcIo::SigOutEncoderZeroSet(int nMode)
{ 
	setBitOut_EncoderSet(nMode);
	return 0;
}
int CSiemensPlcIo::SigOutRecipeChangeAck(int nMode)
{
	setBitOut_RecipeChangeAck(nMode);
	return 0; 
}
int CSiemensPlcIo::SigOutLotEndAck(int nMode)
{
	setBitOut_LotEndReqAck(nMode);
	return 0; 
}
int CSiemensPlcIo::SigOutLotStartAck(int nMode)
{ 
	setBitOut_LotStartReqAck(nMode);
	return 0; 
}
int CSiemensPlcIo::WriteAlarmCode(WORD nAlarmCode)
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
int CSiemensPlcIo::SigOutReady(int nMode)
{ 
	setBitOut_Ready(nMode);
	return 0;
}
int CSiemensPlcIo::SigOutAlivePulse(int nInMode)
{ 
	m_bSmsAlive ^= 0x1;
	setBitOut_Alive(m_bSmsAlive);
	return 0;
}
int CSiemensPlcIo::SigOutTabZeroReset(int nMode)
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
int CSiemensPlcIo::SigOutAlarmResetAck(int nMode)
{ 
	setBitOut_AlarmResetAck(nMode);
	return 0; 
}
int CSiemensPlcIo::WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode)
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
int CSiemensPlcIo::GetCellKey(int num)
{
	return getWordIn_CellKey(num);
}

//마킹 설정 값 변수
BOOL CSiemensPlcIo::GetInkMarkActive()
{
	return getBitIn_InkMarkingActive();
}
BOOL CSiemensPlcIo::GetConnectZone()
{
	return getBitIn_ConnectZone();
}
int CSiemensPlcIo::SigOutAlivePulseReady(int nInMode, BOOL bIsReady)
{
	m_bSmsAlive ^= 0x1;
	setBitOut_Alive(m_bSmsAlive);
	setBitOut_Ready(bIsReady);
	return 0; 
}
int CSiemensPlcIo::SigOutAlarmExist(int nMode)
{
	setWordOut_AlarmExist(nMode);
	return 0; 
}
void CSiemensPlcIo::EnableWorkSet(BOOL bMode)
{
}
int CSiemensPlcIo::SigOutDiskCapacityAlarm(int nMode)
{ 
	setBitOut_DiskSpaceAlarm(nMode);
	return 0; 
}
int CSiemensPlcIo::SigOutDiskCapacityWarning(int nMode)
{ 
	setBitOut_DiskSpaceWarning(nMode);
	return 0;
}

int CSiemensPlcIo::SignalBitOut(int nIntegration, int nMode, BOOL bLocal)
{
	return 0;
}

//Lot End 처리 함수
void CSiemensPlcIo::SigOutLotEnd(int TopDefectCnt, int BtmDefectCnt)
{

}

//In
int CSiemensPlcIo::SigInReady()
{ 
	return getBitIn_Ready();
}
int CSiemensPlcIo::SigInRun()
{
	return getBitIn_Run();
}
int CSiemensPlcIo::SigInLotEnd()
{ 
	return getBitIn_LotEndReq();
}
int CSiemensPlcIo::SigInTabZeroReset()
{ 
	return getBitIn_TabZeroReset();
}
int CSiemensPlcIo::SigInLotStart()
{
	return getBitIn_LotStartReq();
}
int CSiemensPlcIo::SigInAlarmReset()
{ 
	return getBitIn_AlarmResetReq();
}
int CSiemensPlcIo::SigInAlarmNgAck()
{
	return getBitIn_AlarmNgAck();
}
int CSiemensPlcIo::SigInRecipeChange()
{
	return getBitIn_RecipeChange();
}
int CSiemensPlcIo::SigInInkMarkActive()
{ 
	return getBitIn_InkMarkingActive();
}
int CSiemensPlcIo::SigInConnectZone()
{
	return getBitIn_ConnectZone();
}

//임시
int CSiemensPlcIo::ReadBlockAllData(CSequenceData* pSeqData)
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

int CSiemensPlcIo::WriteBlockData(void* pGlobalData)
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

int CSiemensPlcIo::ReadAllPort_BitIn(BOOL* pSigBitIn)
{ 
	for (int i = 0; i < MAX_SMS_BITIO_IN; i++)
	{

		if (m_ReadBitData[i] == 0x01)
		{
			pSigBitIn[i] = TRUE;
		}
	}
	return 0; 
}
int CSiemensPlcIo::ReadAllPort_BitOut(BOOL* pSigBitOut)
{ 
	for (int i = 0; i < MAX_SMS_BITIO_OUT; i++)
	{
		if (m_WriteData[i] == 0x01)
		{
			pSigBitOut[i] = TRUE;
		}

	}
	return 0; 
}

CString CSiemensPlcIo::GetInWordData(int idx)
{
	CString str = _T("");
	str.Format(_T("%d"), (int)m_ReadWordData[idx]);
	return str;
}
CString CSiemensPlcIo::GetOutWordData(int idx)
{
	CString str = _T("");
	str.Format(_T("%d"), (int)m_WriteData[SIENENS_WRITEBIT+idx]);
	return str;
}