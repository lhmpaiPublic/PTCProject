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

	//Read Word 영역 최대 크기
	enSmsWordReadMaxSize = SIENENS_READWORD_MAX,
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

	//Write Word 영역 최대 크기
	enSmsWordWriteMaxSize = SIENENS_WRITEWORD_MAX

};

CSiemensPlcIo::CSiemensPlcIo(CString strIPAddress, int nReConnetTimeOut, CWnd* pReceiveMsgWnd, int nPort, int nSlaveID, int nBitIn, int nBitOut, int nWordIn, int nWordOut)
	: m_strIPAddress(strIPAddress)
	, m_nPort(nPort)
	, m_nReConnetTimeOut(nReConnetTimeOut)
	, m_pReceiveMsgWnd(pReceiveMsgWnd)
	, m_pLGIS_Plc(NULL)
{
	m_nSlaveID = nSlaveID;
	m_nBitIn = nBitIn;
	m_nBitOut = nBitOut;
	m_nWordIn = nWordIn;
	m_nWordOut = nWordOut;
	//Alive 값 연산 변수
	m_bSmsAlive = FALSE;

	//Read Data 버퍼 초기화
	memset(m_ReadBitData, 0xff, sizeof(short) * SIENENS_READBIT);
	memset(m_ReadWordData, 0xff, sizeof(short) * SIENENS_READWORD_MAX);

	//Write Bit Data 버퍼 초기화
	memset(m_WriteBitData, 0x00, sizeof(short) * SIENENS_WRITEBIT);

	//Write Word Data 버퍼 초기화
	memset(m_WriteWordData, 0x00, sizeof(short) * SIENENS_WRITEWORD_MAX);

	//PLC 쓰기 Bit Data 읽기
	memset(m_WriteBitDataRead, 0xff, sizeof(short) * SIENENS_WRITEBIT);
	//PLC 쓰기 Word Data 읽기
	memset(m_WriteWordDataRead, 0xff, sizeof(short) * SIENENS_WRITEWORD_MAX);

	//지멘스 Plc 연결 및 데이터 처리 스래드 객체 생성
	OpenPlcIo();
}


CSiemensPlcIo::~CSiemensPlcIo()
{
	ClosePlcIo();	
}

//스래드에서 호출하는 함수
void CSiemensPlcIo::SiemensPlcProc()
{
	if (IsOpened())
	{
		//Read bit 영역 읽기
		short	ReadBitData[SIENENS_READBIT];
		//bit 읽기 영역 읽기
		ReadDataReg(m_nBitIn, ReadBitData, SIENENS_READBIT);
		//읽은 Bit 데이터 파싱
		if (std::equal(std::begin(ReadBitData), std::end(ReadBitData), std::begin(m_ReadBitData)) == false)
		{
			ReadPlcBitDataParser(ReadBitData);
			LOGDISPLAY_SPEC(2)(_T("In bit data :	%s"), CStrSuport::ChangshorttohexTab(ReadBitData, SIENENS_READBIT, m_nBitIn));
		}

		//Read word 영역 읽기
		short	ReadWordData[SIENENS_READWORD_MAX];
		//word 읽기 영역 읽기
		ReadDataReg(m_nWordIn, ReadWordData, SIENENS_READWORD_MAX);
		//읽은 Word 데이터 파싱
		if (std::equal(std::begin(ReadWordData), std::end(ReadWordData), std::begin(m_ReadWordData)) == false)
		{
			ReadPlcWordDataParser(ReadWordData);
			LOGDISPLAY_SPEC(2)(_T("In word data :	%s"), CStrSuport::ChangshorttohexTab(ReadWordData, SIENENS_READWORD_MAX, m_nWordIn));
		}

		//Bit 쓰기 데이터 만들기
		int ret = WritePlcBitDataMake();
		//쓰기
		if (ret > 0)
		{
			WriteDataReg(m_nBitOut, m_WriteBitData, ret);
			LOGDISPLAY_SPEC(2)(_T("Out Bit data :	%s"), CStrSuport::ChangshorttohexTab(m_WriteBitData, ret, m_nBitOut));
		}

		//Word 쓰기 데이터 만들기
		ret = WritePlcWordDataMake();
		//쓰기
		if (ret > 0)
		{
			WriteDataReg(m_nWordOut, m_WriteWordData, ret);
			LOGDISPLAY_SPEC(2)(_T("Out Word data :	%s"), CStrSuport::ChangshorttohexTab(m_WriteWordData, ret, m_nWordOut));
		}

		//Bit 쓰기 데이터 영역 읽기
		short	WriteBitDataRead[SIENENS_WRITEBIT];
		//bit 쓰기 영역 읽기
		ReadDataReg(m_nBitOut, WriteBitDataRead, SIENENS_WRITEBIT);
		//읽은 Bit 데이터 출력
		if (std::equal(std::begin(WriteBitDataRead), std::end(WriteBitDataRead), std::begin(m_WriteBitDataRead)) == false)
		{
			memcpy(m_WriteBitDataRead, WriteBitDataRead, SIENENS_WRITEBIT);
			LOGDISPLAY_SPEC(2)(_T("Out Bit data read :	%s"), CStrSuport::ChangshorttohexTab(m_WriteBitDataRead, SIENENS_WRITEBIT, m_nBitOut));
		}

		//Word 쓰기 데이터 영역 읽기
		short	WriteWordDataRead[SIENENS_WRITEWORD_MAX];
		//Word 쓰기 영역 읽기
		ReadDataReg(m_nWordOut, WriteWordDataRead, SIENENS_WRITEWORD_MAX);
		//읽은 Word 데이터 출력
		if (std::equal(std::begin(WriteWordDataRead), std::end(WriteWordDataRead), std::begin(m_WriteWordDataRead)) == false)
		{
			memcpy(m_WriteWordDataRead, WriteWordDataRead, SIENENS_WRITEWORD_MAX);
			LOGDISPLAY_SPEC(2)(_T("In bit data :	%s"), CStrSuport::ChangshorttohexTab(m_WriteWordDataRead, SIENENS_WRITEWORD_MAX, m_nWordOut));
		}
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
	CString str = _T("");

	BYTE byteData[SIEMENS_READRECIPENAME + 1] = { 0, };
	memcpy(byteData, data, SIEMENS_READRECIPENAME);
	byteData[SIEMENS_READRECIPENAME] = '\0';
	str = (char*)byteData;

	str.TrimRight();
	str.TrimLeft();
	AprData.m_SeqDataIN.strRecipeName = str;

	return str;
}

//CELL ID를 만든다.
CString CSiemensPlcIo::MakeCellId(short* data)
{
	CString str = _T("");

	BYTE byteData[SIEMENS_READCELLID + 1] = { 0, };
	memcpy(byteData, data, SIEMENS_READCELLID);
	byteData[SIEMENS_READCELLID] = '\0';
	str = (char*)byteData;

	str.TrimRight();
	str.TrimLeft();
	AprData.m_SeqDataIN.strCell_ID = str;

	return str;
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

	for (int count = 0; count < COUNT_CELLKEY; count++)
	{
		int idx = enSmsWordRead_CellKey + count;
		if (m_ReadWordData[idx] ^ data[idx]) setWordIn_CellKey(count, data[idx]);
	}

	memcpy(m_ReadWordData, data, sizeof(short) * SIENENS_READWORD_MAX);
}

//PLC write Bit Data Make 함수
int CSiemensPlcIo::WritePlcBitDataMake()
{
	//Write Data Block Num
	int ret = -1;
	int idx = 0;

	idx = enSmsBitOut_Alive;
	if (isBitOut_Alive()) ret = idx;
	m_WriteBitData[idx] = getBitOut_Alive();

	idx = enSmsBitOut_Ready;
	if (isBitOut_Ready()) ret = idx;
	m_WriteBitData[idx] = getBitOut_Ready();

	idx = enSmsBitOut_EncoderSet;
	if (isBitOut_EncoderSet()) ret = idx;
	m_WriteBitData[idx] = getBitOut_EncoderSet();

	idx = enSmsBitOut_RecipeChangeAck;
	if (isBitOut_RecipeChangeAck()) ret = idx;
	m_WriteBitData[idx] = getBitOut_RecipeChangeAck();

	idx = enSmsBitOut_LotStartReqAck;
	if (isBitOut_LotStartReqAck())ret = idx;
	m_WriteBitData[idx] = getBitOut_LotStartReqAck();

	idx = enSmsBitOut_LotEndReqAck;
	if (isBitOut_LotEndReqAck())ret = idx;
	m_WriteBitData[idx] = getBitOut_LotEndReqAck();

	idx = enSmsBitOut_TabZeroReset;
	if (isBitOut_TabZeroReset())ret = idx;
	m_WriteBitData[idx] = getBitOut_TabZeroReset();

	idx = enSmsBitOut_AlarmResetAck;
	if (isBitOut_AlarmResetAck())ret = idx;
	m_WriteBitData[idx] = getBitOut_AlarmResetAck();

	idx = enSmsBitOut_AlarmNgResetAck;
	if (isBitOut_AlarmNgResetAck()) ret = idx;
	m_WriteBitData[idx] = getBitOut_AlarmNgResetAck();

	idx = enSmsBitOut_DiskSpaceWarning;
	if (isBitOut_DiskSpaceWarning()) ret = idx;
	m_WriteBitData[idx] = getBitOut_DiskSpaceWarning();

	idx = enSmsBitOut_DiskSpaceAlarm;
	if (getBitOut_DiskSpaceAlarm()) ret = idx;
	m_WriteBitData[idx] = getBitOut_DiskSpaceAlarm();

	//버퍼 block 위치에 + 1 = 크기(size)
	//쓰기 영역에 쓸 데이터 크기
	return ret + 1;
}

//PLC write Word Data Make 함수
int CSiemensPlcIo::WritePlcWordDataMake()
{
	//Write Data Block Num
	int ret = -1;
	int idx = 0;

	idx = enSmsWordWrite_DataReportV1_Ea;
	if (isWordOut_DataReportV1_Ea()) ret = idx;
	m_WriteWordData[idx] = getWordOut_DataReportV1_Ea();

	idx = enSmsWordWrite_DataReportV2_OK;
	if (isWordOut_DataReportV2_OK()) ret = idx;
	m_WriteWordData[idx] = getWordOut_DataReportV2_OK();

	idx = enSmsWordWrite_DataReportV3_NG;
	if (isWordOut_DataReportV3_NG()) ret = idx;
	m_WriteWordData[idx] = getWordOut_DataReportV3_NG();

	idx = enSmsWordWrite_DataReportV4_OkRate;
	if (isWordOut_DataReportV4_OkRate()) ret = idx;
	m_WriteWordData[idx] = getWordOut_DataReportV4_OkRate();

	idx = enSmsWordWrite_DataReportV5_NgRate;
	if (isWordOut_DataReportV5_NgRate()) ret = idx;
	m_WriteWordData[idx] = getWordOut_DataReportV5_NgRate();

	idx = enSmsWordWrite_DataReportV6_RunRate;
	if (isWordOut_DataReportV6_RunRate()) ret = idx;
	m_WriteWordData[idx] = getWordOut_DataReportV6_RunRate();

	idx = enSmsWordWrite_Continue_Alarm_Cnt;
	if (isWordOut_Continue_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Continue_Alarm_Cnt();

	idx = enSmsWordWrite_Heavy_Alarm_Cnt;
	if (isWordOut_Heavy_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Heavy_Alarm_Cnt();

	idx = enSmsWordWrite_FoilExpInTop_Alarm_Cnt;
	if (isWordOut_FoilExpInTop_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpInTop_Alarm_Cnt();

	idx = enSmsWordWrite_FoilExpInBtm_Alarm_Cnt;
	if (isWordOut_FoilExpInBtm_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpInBtm_Alarm_Cnt();

	idx = enSmsWordWrite_FoilExpOutTop_Alarm_Cnt;
	if (isWordOut_FoilExpOutTop_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpOutTop_Alarm_Cnt();

	idx = enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt;
	if (isWordOut_FoilExpOutBtm_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpOutBtm_Alarm_Cnt();

	idx = enSmsWordWrite_FoilExpBothTop_Alarm_Cnt;
	if (isWordOut_FoilExpBothTop_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpBothTop_Alarm_Cnt();

	idx = enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt;
	if (isWordOut_FoilExpBothBtm_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpBothBtm_Alarm_Cnt();

	idx = enSmsWordWrite_SpeterTop_Alarm_Cnt;
	if (isWordOut_SpeterTop_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_SpeterTop_Alarm_Cnt();

	idx = enSmsWordWrite_SpeterBtm_Alarm_Cnt;
	if (isWordOut_SpeterBtm_Alarm_Cnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_SpeterBtm_Alarm_Cnt();

	idx = enSmsWordWrite_Top_Defect_Count_Real;
	if (isWordOut_Top_Defect_Count_Real()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Top_Defect_Count_Real();

	idx = enSmsWordWrite_Btm_Defect_Count_Real;
	if (isWordOut_Btm_Defect_Count_Real()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Btm_Defect_Count_Real();

	idx = enSmsWordWrite_Top_Defect_Count_LotEnd;
	if (isWordOut_Top_Defect_Count_LotEnd()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Top_Defect_Count_LotEnd();

	idx = enSmsWordWrite_Btm_Defect_Count_LotEnd;
	if (isWordOut_Btm_Defect_Count_LotEnd()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Btm_Defect_Count_LotEnd();

	idx = enSmsWordWrite_FoilExpInTopTarget;
	if (isWordOut_FoilExpInTopTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpInTopTarget();

	idx = enSmsWordWrite_FoilExpInBtmTarget;
	if (isWordOut_FoilExpInBtmTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpInBtmTarget();

	idx = enSmsWordWrite_FoilExpOutTopTarget;
	if (isWordOut_FoilExpOutTopTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpOutTopTarget();

	idx = enSmsWordWrite_FoilExpOutBtmTarget;
	if (isWordOut_FoilExpOutBtmTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpOutBtmTarget();

	idx = enSmsWordWrite_FoilExpBothTopTarget;
	if (isWordOut_FoilExpBothTopTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpBothTopTarget();

	idx = enSmsWordWrite_FoilExpBothBtmTarget;
	if (isWordOut_FoilExpBothBtmTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_FoilExpBothBtmTarget();

	idx = enSmsWordWrite_SpeterTopTarget;
	if (isWordOut_SpeterTopTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_SpeterTopTarget();

	idx = enSmsWordWrite_SpeterBtmTarget;
	if (isWordOut_SpeterBtmTarget()) ret = idx;
	m_WriteWordData[idx] = getWordOut_SpeterBtmTarget();

	idx = enSmsWordWrite_PrmContinuousCnt;
	if (isWordOut_PrmContinuousCnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_PrmContinuousCnt();

	idx = enSmsWordWrite_PrmSectorNgTabCnt;
	if (isWordOut_PrmSectorNgTabCnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_PrmSectorNgTabCnt();

	idx = enSmsWordWrite_PrmSectorBaseCnt;
	if (isWordOut_PrmSectorBaseCnt()) ret = idx;
	m_WriteWordData[idx] = getWordOut_PrmSectorBaseCnt();

	idx = enSmsWordWrite_AlarmExist;
	if (isWordOut_AlarmExist()) ret = idx;
	m_WriteWordData[idx] = getWordOut_AlarmExist();

	idx = enSmsWordWrite_AlarmCode_Buffer;
	if (isWordOut_AlarmCode_Buffer()) ret = idx + COUNT_ALRAMBUFF;
	for (int count = 0; count < COUNT_ALRAMBUFF; count++)
	{
		m_WriteWordData[count + idx] = getWordOut_AlarmCode_Buffer(count);
	}


	idx = enSmsWordWrite_Cell_Trigger_ID;
	if (isWordOut_Cell_Trigger_ID()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Cell_Trigger_ID();

	idx = enSmsWordWrite_Judge;
	if (isWordOut_Judge()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Judge();

	idx = enSmsWordWrite_NG_Code;
	if (isWordOut_NG_Code()) ret = idx;
	m_WriteWordData[idx] = getWordOut_Judge();

	idx = enSmsWordWrite_DuplicateNG_Cell_ID;
	if (isWordOut_DuplicateNG_Cell_ID()) ret = idx + COUNT_DUPLICATENGCELLID;
	for (int count = 0; count < COUNT_DUPLICATENGCELLID; count++)
	{
		m_WriteWordData[count + idx] = getWordOut_DuplicateNG_Cell_ID(count);
	}


	//버퍼 block 위치에 + 1 = 크기(size)
	//쓰기 영역에 쓸 데이터 크기
	return ret + 1;
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
	int nRet = CONNECTION_ERROR;
	if (IsOpened())
	{
		nRet = m_pLGIS_Plc->WriteMultipleRegisters(offset, num, (uint16_t*)data);

		if (nRet < 0)
		{
			//로그출력
			LOGDISPLAY_SPEC(2)(" Siemens WriteDataReg Error	code : %d	offset : %d", nRet, offset);
		}

	}

	return nRet;
}

int CSiemensPlcIo::ReadDataReg(int offset, short data[], int num)
{
	int nRet = CONNECTION_ERROR;

	if (IsOpened())
	{
		nRet = m_pLGIS_Plc->ReadHoldingRegisters(offset, num, (uint16_t*)data);

		if (nRet < 0)
		{
			//로그출력
			LOGDISPLAY_SPEC(2)(" Siemens ReadDataReg Error	code : %d	offset : %d", nRet, offset);
		}
	}

	return nRet;
}

BOOL CSiemensPlcIo::IsOpened()
{
	if (m_pLGIS_Plc)
	{
		return m_pLGIS_Plc->CheckConnection();
	}

	return false;
}

CString CSiemensPlcIo::GetErrorMsg()
{
	if (IsOpened())
	{
		if (m_pLGIS_Plc->m_bErr)
			return m_pLGIS_Plc->m_strErrorMsg;
		else
			return CString(_T(""));
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
	//PLC 데이터 변수 값 초기화
	initDataPlcImp();

	m_pLGIS_Plc = new CLGIS_Plc(m_strIPAddress, m_nPort, m_nReConnetTimeOut, m_pReceiveMsgWnd );
	//로그출력
	LOGDISPLAY_SPEC(2)(_T("PLC Info	Address : %s	Port : %d	SlaveID : %d	BitIn : %d	BitOut : %d	WordIn : %d	WordOut : %d"),
		m_strIPAddress,
		m_nPort,
		m_nSlaveID,
		m_nBitIn,
		m_nBitOut,
		m_nWordIn,
		m_nWordOut);

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
		LOGDISPLAY_SPEC(2)("PLC Siemens Open success Slave Id : %d", m_nSlaveID);

		//슬레이브 아이디 
		SetSlaveId(m_nSlaveID);
	}

	return ret;
}

void CSiemensPlcIo::ClosePlcIo(void)
{
	if (m_pLGIS_Plc)
	{
		//로그출력
		LOGDISPLAY_SPEC(2)("PLC Siemens ClosePlcIo");
		delete m_pLGIS_Plc;
		m_pLGIS_Plc = NULL;
	}
}

int CSiemensPlcIo::PlcDataReadWritePorc()
{
	//PLC 데이터 Read / Write 처리 함수
	if (IsOpened())
	{
		SiemensPlcProc();
	}
	return 0;
}

BOOL CSiemensPlcIo::isPlcOpen()
{
	return IsOpened();
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
int CSiemensPlcIo::SignalBitOut(int nIntegration, int nMode, BOOL bLocal)
{
	return 0;
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

//Lot End 처리 함수
void CSiemensPlcIo::SigOutLotEnd(int TopDefectCnt, int BtmDefectCnt)
{
	setWordOut_Top_Defect_Count_LotEnd(TopDefectCnt);
	setWordOut_Btm_Defect_Count_LotEnd(BtmDefectCnt);
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
		if (m_WriteBitData[i] == 0x01)
		{
			pSigBitOut[i] = TRUE;
		}

	}
	return 0; 
}

CString CSiemensPlcIo::GetInWordData(int idx)
{
	CString str = _T("");
	if (idx < enSmsWordReadMaxSize)
	{
		str.Format(_T("%d"), (int)m_ReadWordData[idx]);
	}
	return str;
}
CString CSiemensPlcIo::GetOutWordData(int idx)
{
	CString str = _T("");
	if (idx < enSmsWordWriteMaxSize)
	{
		str.Format(_T("%d"), (int)m_WriteWordData[idx]);
	}
	return str;
}