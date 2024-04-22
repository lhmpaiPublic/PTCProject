#include "pch.h"
#include "MelsecPlcIo.h"
#include "mdfunc.h"
#include "GlobalData.h"
#include "LogDisplayDlg.h"

#define uint16_t    unsigned short
#define uint32_t    unsigned int
#define SWAP_UINT16(x) ((uint16_t)((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8)))
#define SWAP_UINT32(x) ((uint32_t) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24)))


// [ Melsec Address Start ]
// [ BIT IN ]
enum MelsBitIn
{
	enMelsBitInAlive = 0x00000000,
	enMelsBitInReady = 0x00000001,
	enMelsBitInRun = 0x00000002,
	enMelsBitInDummy4 = 0x00000003,
	enMelsBitInDummy5 = 0x00000004,
	enMelsBitInEncoderReset = 0x00000005,
	enMelsBitInTabZeroReset = 0x00000006,
	enMelsBitInDummy8 = 0x00000007,

	enMelsBitInDummy9 = 0x00000100,
	enMelsBitInInkMarkingActive = 0x00000101,
	enMelsBitInConnectZone = 0x00000102,
	enMelsBitInDummy12 = 0x00000103,
	enMelsBitInDummy13 = 0x00000104,
	enMelsBitInDummy14 = 0x00000105,
	enMelsBitInDummy15 = 0x00000106,
	enMelsBitInDummy16 = 0x00000107,

	enMelsBitInDummy17 = 0x00000200,
	enMelsBitInDummy18 = 0x00000201,
	enMelsBitInRecipeChange = 0x00000202,
	enMelsBitInLotStartReq = 0x00000203,
	enMelsBitInLotEndReq = 0x00000204,
	enMelsBitInAlarmResetReq = 0x00000205,
	enMelsBitInAlarmNgAck = 0x00000206
};

// [ BIT OUT ]
enum MelsBitOut
{
	enMelsBitOut_Alive = 0x00000000,
	enMelsBitOut_Ready = 0x00000001,
	enMelsBitOut_Dummy3 = 0x00000002,
	enMelsBitOut_Dummy4 = 0x00000003,
	enMelsBitOut_Dummy5 = 0x00000004,
	enMelsBitOut_EncoderSet = 0x00000005,
	enMelsBitOut_TabZeroReset = 0x00000006,
	enMelsBitOut_Dummy8 = 0x00000007,

	enMelsBitOut_DiskSpaceWarning = 0x00000100,
	enMelsBitOut_DiskSpaceAlarm = 0x00000101,
	enMelsBitOut_Dummy11 = 0x00000102,
	enMelsBitOut_Dummy12 = 0x00000103,
	enMelsBitOut_Dummy13 = 0x00000104,
	enMelsBitOut_Dummy14 = 0x00000105,
	enMelsBitOut_Dummy15 = 0x00000106,
	enMelsBitOut_Dummy16 = 0x00000107,

	enMelsBitOut_Dummy17 = 0x00000200,
	enMelsBitOut_Dummy18 = 0x00000201,
	enMelsBitOut_RecipeChangeAck = 0x00000202,
	enMelsBitOut_LotStartReqAck = 0x00000203,
	enMelsBitOut_LotEndReqAck = 0x00000204,
	enMelsBitOut_AlramResetAck = 0x00000205,
	enMelsBitOut_AlramNgResetAck = 0x00000206
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

	enMelsWordReadMaxSize = MELSEC_WORDINSIZE_MAX //160,

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

	enWordWriteMaxSize = MELSEC_WORDOUTSIZE_MAX //160,
};

//////////////////////////////////////////////////////////////////////////
// [ DATA READ ]
enum MelsDwordRead
{
	enMelsDwordRead_RecipeNo = (enMelsWordRead_RecipeNo / 2),
	enMelsDwordRead_RecipeName = (enMelsWordRead_RecipeName / 2),
	enMelsDwordRead_CELL_ID = (enMelsWordRead_CELL_ID / 2),
	enMelsDwordRead_FoilExpInTopTarget = (enMelsWordRead_FoilExpInTopTarget / 2),
	enMelsDwordRead_FoilExpInBtmTarget = (enMelsWordRead_FoilExpInBtmTarget / 2),
	enMelsDwordRead_FoilExpOutTopTarget = (enMelsWordRead_FoilExpOutTopTarget / 2),
	enMelsDwordRead_FoilExpOutBtmTarget = (enMelsWordRead_FoilExpOutBtmTarget / 2),
	enMelsDwordRead_FoilExpBothTopTarget = (enMelsWordRead_FoilExpBothTopTarget / 2),
	enMelsDwordRead_FoilExpBothBtmTarget = (enMelsWordRead_FoilExpBothBtmTarget / 2),
	enMelsDwordRead_SpeterTopTarget = (enMelsWordRead_SpeterTopTarget / 2),
	enMelsDwordRead_SpeterBtmTarget = (enMelsWordRead_SpeterBtmTarget / 2),
	enMelsDwordRead_PrmContinuousCnt = (enMelsWordRead_PrmContinuousCnt / 2),
	enMelsDwordRead_PrmSectorNgTabCnt = (enMelsWordRead_PrmSectorNgTabCnt / 2),
	enMelsDwordRead_PrmSectorBaseCnt = (enMelsWordRead_PrmSectorBaseCnt / 2),
	enMelsDwordRead_AlarmExistAck = (enMelsWordRead_AlarmExistAck / 2),

	enMelsDwordReadMaxSize = (enMelsWordReadMaxSize / 2),

};

// [ DATA WRITE ]
enum MelsDwordWrite
{
	enMelsDwordWrite_DataReportV1_Ea = (enMelsWordWrite_DataReportV1_Ea / 2),
	enMelsDwordWrite_DataReportV2_OK = (enMelsWordWrite_DataReportV2_OK / 2),
	enMelsDwordWrite_DataReportV3_NG = (enMelsWordWrite_DataReportV3_NG / 2),
	enMelsDwordWrite_DataReportV4_OkRate = (enMelsWordWrite_DataReportV4_OkRate / 2),
	enMelsDwordWrite_DataReportV5_NgRate = (enMelsWordWrite_DataReportV5_NgRate / 2),
	enMelsDwordWrite_DataReportV6_RunRate = (enMelsWordWrite_DataReportV6_RunRate / 2),
	enMelsDwordWrite_Continue_Alarm_Cnt = (enMelsWordWrite_Continue_Alarm_Cnt / 2),
	enMelsDwordWrite_Heavy_Alarm_Cnt = (enMelsWordWrite_Heavy_Alarm_Cnt / 2),

	enMelsDwordWrite_FoilExpInTop_Alarm_Cnt = (enMelsWordWrite_FoilExpInTop_Alarm_Cnt / 2),
	enMelsDwordWrite_FoilExpInBottom_Alarm_Cnt = (enMelsWordWrite_FoilExpInBottom_Alarm_Cnt / 2),
	enMelsDwordWrite_FoilExpOutTop_Alarm_Cnt = (enMelsWordWrite_FoilExpOutTop_Alarm_Cnt / 2),
	enMelsDwordWrite_FoilExpOutBottom_Alarm_Cnt = (enMelsWordWrite_FoilExpOutBottom_Alarm_Cnt / 2),
	enMelsDwordWrite_FoilExpBothTop_Alarm_Cnt = (enMelsWordWrite_FoilExpBothTop_Alarm_Cnt / 2),
	enMelsDwordWrite_FoilExpBothBottom_Alarm_Cnt = (enMelsWordWrite_FoilExpBothBottom_Alarm_Cnt / 2),
	enMelsDwordWrite_SpeterTop_Alarm_Cnt = (enMelsWordWrite_SpeterTop_Alarm_Cnt / 2),
	enMelsDwordWrite_SpeterBtm_Alarm_Cnt = (enMelsWordWrite_SpeterBtm_Alarm_Cnt / 2),
	enMelsDwordWrite_Top_Defect_Count_Real = (enMelsWordWrite_Top_Defect_Count_Real / 2),
	enMelsDwordWrite_Btm_Defect_Count_Real = (enMelsWordWrite_Btm_Defect_Count_Real / 2),
	enMelsDwordWrite_Top_Defect_Count_LotEnd = (enMelsWordWrite_Top_Defect_Count_LotEnd / 2),
	enMelsDwordWrite_Btm_Defect_Count_LotEnd = (enMelsWordWrite_Btm_Defect_Count_LotEnd / 2),

	enMelsDwordWrite_FoilExpInTopTarget = (enMelsWordWrite_FoilExpInTopTarget / 2),
	enMelsDwordWrite_FoilExpInBottomTarget = (enMelsWordWrite_FoilExpInBottomTarget / 2),
	enMelsDworddWrite_FoilExpOutTopTarget = (enMelsWordWrite_FoilExpOutTopTarget / 2),
	enMelsDwordWrite_FoilExpOutBottomTarget = (enMelsWordWrite_FoilExpOutBottomTarget / 2),
	enMelsDwordWrite_FoilExpBothTopTarget = (enMelsWordWrite_FoilExpBothTopTarget / 2),
	enMelsDwordWrite_FoilExpBothBottomTarget = (enMelsWordWrite_FoilExpBothBottomTarget / 2),
	enMelsDwordWrite_SpeterTopTarget = (enMelsWordWrite_SpeterTopTarget / 2),
	enMelsDwordWrite_SpeterBtmTarget = (enMelsWordWrite_SpeterBtmTarget / 2),

	enMelsDwordWrite_PrmContinuousCnt = (enMelsWordWrite_PrmContinuousCnt / 2),
	enMelsDwordWrite_PrmSectorNgTabCnt = (enMelsWordWrite_PrmSectorNgTabCnt / 2),
	enMelsDwordWrite_PrmSectorBaseCnt = (enMelsWordWrite_PrmSectorBaseCnt / 2),


	enMelsDwordWrite_AlarmExist = (enMelsWordWrite_AlarmExist / 2),
	enMelsDwordWrite_AlarmCode_Buffer1 = (enMelsWordWrite_AlarmCode_Buffer1 / 2),
	enMelsDwordWrite_AlarmCode_Buffer2 = (enMelsWordWrite_AlarmCode_Buffer2 / 2),
	enMelsDwordWrite_AlarmCode_Buffer3 = (enMelsWordWrite_AlarmCode_Buffer3 / 2),
	enMelsDwordWrite_AlarmCode_Buffer4 = (enMelsWordWrite_AlarmCode_Buffer4 / 2),
	enMelsDwordWrite_AlarmCode_Buffer5 = (enMelsWordWrite_AlarmCode_Buffer5 / 2),

	enMelsDwordWrite_Cell_Trigger_ID = (enMelsWordWrite_Cell_Trigger_ID / 2),
	enMelsDwordWrite_Judge = (enMelsWordWrite_Judge / 2),
	enMelsDwordWrite_NG_Code = (enMelsWordWrite_NG_Code / 2),

	enDwordWriteMaxSize = (enWordWriteMaxSize / 2),
};

CMelsecPlcIo::CMelsecPlcIo(WORD wOffset_BitIn, WORD wOffset_BitOut, WORD wOffset_WordIn, WORD wOffset_WordOut, WORD wChannelNo, WORD wMaxPort, WORD wMyStNo, WORD wExtStNo, WORD wSeqStNo)
{
	m_wChannelNo = wChannelNo;
	m_wMaxPort = wMaxPort;
	m_wMyStNo = wMyStNo;
	m_wExtStNo = wExtStNo;
	m_wSeqStNo = wSeqStNo;

	m_wOffset_BitIn = wOffset_BitIn;
	m_wOffset_BitOut = wOffset_BitOut;
	m_wOffset_WordIn = wOffset_WordIn;
	m_wOffset_WordOut = wOffset_WordOut;

	m_pPath = 0;

	m_bOpened = FALSE;

	m_bEnableWorkSet = TRUE;

	m_bMelsecAlive = 0x1;

	//PLC 읽기 Data
	memset(m_ReadBitData, 0, MELSEC_BITINSIZE_MAX);
	memset(m_ReadWordData, 0, MELSEC_WORDINSIZE_MAX*sizeof(DWORD));

	//PLC 쓰기 Data
	memset(m_WriteBitData, 0, MELSEC_BITOUTSIZE_MAX);
	memset(m_WriteWordData, 0, MELSEC_WORDOUTSIZE_MAX * sizeof(DWORD));

	//PLC Open
	OpenPlcIo();
}


CMelsecPlcIo::~CMelsecPlcIo()
{
	
}

//PLC 데이터 Read / Write 처리 함수
void CMelsecPlcIo::MelsecPlcProc()
{
	//Bit Read 영역 읽기
	//4포트
	//시작번지 : 0, 읽을 갯수 4
	byte buffBitIn[MELSEC_BITINSIZE_MAX];
	memset(buffBitIn, 0, MELSEC_BITINSIZE_MAX);
	ReadBitData(0xff, MELSEC_DEVICE_B, 0, buffBitIn, MELSEC_BITINSIZE_MAX, m_wOffset_BitIn);
	//읽은 Bit 데이터 파싱
	if (std::equal(std::begin(buffBitIn), std::end(buffBitIn), std::begin(m_ReadBitData)) == false)
	{
		ReadPlcBitDataParser(buffBitIn);
		LOGDISPLAY_SPEC(2)(_T("In bit data :	%s"), CStrSuport::ChangbytetohexTab(buffBitIn, MELSEC_BITINSIZE_MAX, m_wOffset_BitIn));
	}

	//word 읽기 영역 읽기(멜섹은 DWORD 를 사용한다.)
	DWORD buffDwordIn[MELSEC_WORDINSIZE_MAX];
	memset(buffDwordIn, 0, MELSEC_WORDINSIZE_MAX *sizeof(int));
	ReadWordData(0xff, MELSEC_DEVICE_W, 0, (int*)buffDwordIn, MELSEC_WORDINSIZE_MAX, m_wOffset_WordIn);
	//읽은 Word 데이터 파싱
	if (std::equal(std::begin(buffDwordIn), std::end(buffDwordIn), std::begin(m_ReadWordData)) == false)
	{
		ReadPlcWordDataParser((DWORD*)buffDwordIn);
		LOGDISPLAY_SPEC(2)(_T("In Word data int :	%s"), CStrSuport::ChanginttohexTab((int*)buffDwordIn, MELSEC_WORDINSIZE_MAX, m_wOffset_WordIn));
	}


	byte* buffBitOut = new byte[MELSEC_BITOUTSIZE_MAX];
	memset(buffBitOut, 0, MELSEC_BITOUTSIZE_MAX);
	int portsize = WritePlcBitDataMake(&buffBitOut);
	if (portsize > 0)
	{
		LOGDISPLAY_SPEC(2)(_T("Out bit data :	%s"), CStrSuport::ChangbytetohexTab(buffBitOut, portsize, m_wOffset_BitOut));
		WriteBitData(0xff, MELSEC_DEVICE_B, 0, buffBitOut, portsize, m_wOffset_BitOut);
	}
	delete[] buffBitOut;

	DWORD* buffWordOut = new DWORD[MELSEC_WORDOUTSIZE_MAX];
	memset(buffWordOut, 0, MELSEC_WORDOUTSIZE_MAX*sizeof(DWORD));
	int wordwritesize = WritePlcWordDataMake(&buffWordOut);
	if (wordwritesize > 0)
	{
		LOGDISPLAY_SPEC(2)(_T("Out word data int :	%s"), CStrSuport::ChanginttohexTab((int*)buffWordOut, wordwritesize, m_wOffset_WordOut));
		WriteWordData(0xff, MELSEC_DEVICE_W, 0, (int*)buffWordOut, wordwritesize, m_wOffset_WordOut);
	}
	delete[] buffWordOut;

	//비트영역 쓰기 데이터 읽기
	byte buffBitOutRead[MELSEC_BITOUTSIZE_MAX];
	memset(buffBitOutRead, 0, MELSEC_BITOUTSIZE_MAX);
	ReadBitData(0xff, MELSEC_DEVICE_B, 0, buffBitOutRead, MELSEC_BITOUTSIZE_MAX, m_wOffset_BitOut);
	//읽은 Bit 데이터 비교
	if (std::equal(std::begin(buffBitOutRead), std::end(buffBitOutRead), std::begin(m_WriteBitData)) == false)
	{
		memcpy(m_WriteBitData, buffBitOutRead, MELSEC_BITOUTSIZE_MAX);
		LOGDISPLAY_SPEC(2)(_T("Out bit data Read :	%s"), CStrSuport::ChangbytetohexTab(buffBitOutRead, MELSEC_BITOUTSIZE_MAX, m_wOffset_BitOut));
	}

	//word 읽기 영역 읽기(멜섹은 DWORD 를 사용한다.)
	DWORD buffWordOutRead[MELSEC_WORDOUTSIZE_MAX];
	memset(buffWordOutRead, 0, MELSEC_WORDOUTSIZE_MAX * sizeof(int));
	ReadWordData(0xff, MELSEC_DEVICE_W, 0, (int*)buffWordOutRead, MELSEC_WORDOUTSIZE_MAX, m_wOffset_WordOut);
	//읽은 Word 데이터 비교
	if (std::equal(std::begin(buffWordOutRead), std::end(buffWordOutRead), std::begin(m_WriteWordData)) == false)
	{
		memcpy(m_WriteWordData, buffWordOutRead, MELSEC_WORDOUTSIZE_MAX);
		LOGDISPLAY_SPEC(2)(_T("Out Word data int Read :	%s"), CStrSuport::ChanginttohexTab((int*)buffWordOutRead, MELSEC_WORDINSIZE_MAX, m_wOffset_WordOut));
	}
}

int CMelsecPlcIo::OpenPlcIo(void)
{
	int ret = 0;

	//로그출력
	LOGDISPLAY_SPEC(2)(_T("PLC Info	ChannelNo : %d	BitIn : %d	BitOut : %d	WordIn : %d	WordOut : %d"),
		m_wChannelNo,
		m_wOffset_BitIn,
		m_wOffset_BitOut,
		m_wOffset_WordIn,
		m_wOffset_WordOut);

	if ((m_wOffset_BitIn % 8) || (m_wOffset_BitOut % 8))
	{
		AfxMessageBox("Melsec Bit Offset 값이 잘못 입력되었습니다.");
	}

	ret = LocalPioOpen();

	if (ret != 0)
	{
		//에러로그
	}
	else
	{
		
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
		//로그출력
		LOGDISPLAY_SPEC(2)("PLC Melsec Open Success");

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

int CMelsecPlcIo::ReadBitData(short stno, int devtype, int startport, byte buff[], int num, int offset)
{
	int iRet = 0;

	short	devno, size;

	short* localbuff;
	int buffsize = (num / 2) + 1;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	//읽을 번지 세팅
	//startport : 8비트만큼 이동 번지, 
	devno = (short)(startport * 8 + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num;

	iRet = mdReceive(m_pPath
		, stno
		, devtype
		, devno
		, &size
		, localbuff
	);

	memcpy(buff, localbuff, size);

	delete[] localbuff;

	return iRet;

}

int CMelsecPlcIo::ReadWordData(short stno, int devtype, int startport, short buff[], int num, int offset)
{
	int iRet = 0;

	short	devno, size;

	short* localbuff;
	int buffsize = num;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(short);

	iRet = mdReceive(m_pPath
		, stno
		, devtype
		, devno
		, &size
		, localbuff
	);

	memcpy(buff, localbuff, size);

	delete[] localbuff;

	return iRet;

}

int CMelsecPlcIo::ReadWordDataEx(short netNo, int devtype, int startport, short buff[], int num, int offset)
{
	int iRet = 0;

	long	devno, size;

	short* localbuff;
	int buffsize = num;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(short);

	iRet = mdReceiveEx(m_pPath
		, netNo
		, m_wSeqStNo
		, devtype
		, devno
		, &size
		, localbuff
	);

	memcpy(buff, localbuff, size);

	delete[] localbuff;

	return iRet;
}

int CMelsecPlcIo::ReadWordData(short stno, int devtype, int startport, int buff[], int num, int offset)
{
	int iRet = 0;

	short	devno, size;

	short* localbuff;
	int buffsize = num*2;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(int);

	iRet = mdReceive(m_pPath
		, stno
		, devtype
		, devno
		, &size
		, localbuff
	);

	memcpy(buff, localbuff, size);

	delete[] localbuff;

	return iRet;

}

int CMelsecPlcIo::ReadWordDataEx(short netNo, int devtype, int startport, int buff[], int num, int offset)
{
	int iRet = 0;

	long	devno, size;

	short* localbuff;
	int buffsize = num*2;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(int);

	iRet = mdReceiveEx(m_pPath
		, netNo
		, m_wSeqStNo
		, devtype
		, devno
		, &size
		, localbuff
	);

	memcpy(buff, localbuff, size);

	delete[] localbuff;

	return iRet;
}

int CMelsecPlcIo::WriteBitData(short stno, int devtype, int startport, byte buff[], int num, int offset)
{
	int iRet = 0;

	short	devno, size;

	short* localbuff;
	int buffsize = (num / 2) + 1;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	//읽을 번지 세팅
	//startport : 8비트만큼 이동 번지, 
	devno = (short)(startport * 8 + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num;

	memcpy(localbuff, buff, size);

	iRet = mdSend(m_pPath
		, stno
		, devtype
		, devno
		, &size
		, localbuff
	);

	delete[] localbuff;

	return iRet;

}

int CMelsecPlcIo::WriteWordData(short stno, int devtype, int startport, short buff[], int num, int offset)
{
	int iRet = 0;

	short	devno, size;

	short* localbuff;
	int buffsize = num;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(short);

	memcpy(localbuff, buff, size);

	iRet = mdSend(m_pPath
		, stno
		, devtype
		, devno
		, &size
		, localbuff
	);

	delete[] localbuff;

	return iRet;

}

int CMelsecPlcIo::WriteWordDataEx(short netNo, int devtype, int startport, short buff[], int num, int offset)
{
	int iRet = 0;

	long	devno, size;

	short* localbuff;
	int buffsize = num;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(short);

	memcpy(localbuff, buff, size);

	iRet = mdSendEx(m_pPath
		, netNo
		, m_wSeqStNo
		, devtype
		, devno
		, &size
		, localbuff
	);

	delete[] localbuff;

	return iRet;
}

int CMelsecPlcIo::WriteWordData(short stno, int devtype, int startport, int buff[], int num, int offset)
{
	int iRet = 0;

	short	devno, size;

	short* localbuff;
	int buffsize = num * 2;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(int);

	memcpy(localbuff, buff, size);

	iRet = mdSend(m_pPath
		, stno
		, devtype
		, devno
		, &size
		, localbuff
	);

	delete[] localbuff;

	return iRet;

}

int CMelsecPlcIo::WriteWordDataEx(short netNo, int devtype, int startport, int buff[], int num, int offset)
{
	int iRet = 0;

	long	devno, size;

	short* localbuff;
	int buffsize = num * 2;
	localbuff = new short[buffsize];
	memset(localbuff, 0, buffsize * sizeof(short));

	devno = (short)(startport + (m_wMyStNo - 1) * 4 * 8 + offset);
	size = (short)num * sizeof(int);

	memcpy(localbuff, buff, size);

	iRet = mdSendEx(m_pPath
		, netNo
		, m_wSeqStNo
		, devtype
		, devno
		, &size
		, localbuff
	);

	delete[] localbuff;

	return iRet;
}

//PLC read Data Parser 함수
void CMelsecPlcIo::ReadPlcBitDataParser(BYTE* data)
{
	if (m_ReadBitData[(enMelsBitInAlive >> 8)&0xff] ^ data[(enMelsBitInAlive >> 8) & 0xff]) setBitIn_Alive((data[(enMelsBitInAlive >> 8) & 0xff]>>(enMelsBitInAlive&0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInReady >> 8) & 0xff] ^ data[(enMelsBitInReady >> 8) & 0xff]) setBitIn_Ready((data[(enMelsBitInReady >> 8) & 0xff] >> (enMelsBitInReady & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInRun >> 8) & 0xff] ^ data[(enMelsBitInRun >> 8) & 0xff]) setBitIn_Run((data[(enMelsBitInRun >> 8) & 0xff] >> (enMelsBitInRun & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInEncoderReset >> 8) & 0xff] ^ data[(enMelsBitInEncoderReset >> 8) & 0xff]) setBitIn_EncoderReset((data[(enMelsBitInEncoderReset >> 8) & 0xff] >> (enMelsBitInEncoderReset & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInTabZeroReset >> 8) & 0xff] ^ data[(enMelsBitInTabZeroReset >> 8) & 0xff]) setBitIn_TabZeroReset((data[(enMelsBitInTabZeroReset >> 8) & 0xff] >> (enMelsBitInTabZeroReset & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInInkMarkingActive >> 8) & 0xff] ^ data[(enMelsBitInInkMarkingActive >> 8) & 0xff]) setBitIn_InkMarkingActive((data[(enMelsBitInInkMarkingActive >> 8) & 0xff] >> (enMelsBitInInkMarkingActive & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInConnectZone >> 8) & 0xff] ^ data[(enMelsBitInConnectZone >> 8) & 0xff]) setBitIn_ConnectZone((data[(enMelsBitInConnectZone >> 8) & 0xff] >> (enMelsBitInConnectZone & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInRecipeChange >> 8) & 0xff] ^ data[(enMelsBitInRecipeChange >> 8) & 0xff]) setBitIn_RecipeChange((data[(enMelsBitInRecipeChange >> 8) & 0xff] >> (enMelsBitInRecipeChange & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInLotStartReq >> 8) & 0xff] ^ data[(enMelsBitInLotStartReq >> 8) & 0xff]) setBitIn_LotStartReq((data[(enMelsBitInLotStartReq >> 8) & 0xff] >> (enMelsBitInLotStartReq & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInLotEndReq >> 8) & 0xff] ^ data[(enMelsBitInLotEndReq >> 8) & 0xff]) setBitIn_LotEndReq((data[(enMelsBitInLotEndReq >> 8) & 0xff] >> (enMelsBitInLotEndReq & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInAlarmResetReq >> 8) & 0xff] ^ data[(enMelsBitInAlarmResetReq >> 8) & 0xff]) setBitIn_AlarmResetReq((data[(enMelsBitInAlarmResetReq >> 8) & 0xff] >> (enMelsBitInAlarmResetReq & 0xff)) & 0x1);
	if (m_ReadBitData[(enMelsBitInAlarmNgAck >> 8) & 0xff] ^ data[(enMelsBitInAlarmNgAck >> 8) & 0xff]) setBitIn_AlarmNgAck((data[(enMelsBitInAlarmNgAck >> 8) & 0xff] >> (enMelsBitInAlarmNgAck & 0xff)) & 0x1);
	memcpy(m_ReadBitData, data, MELSEC_BITINSIZE_MAX);
}
void CMelsecPlcIo::ReadPlcWordDataParser(DWORD* data)
{
	if (m_ReadWordData[enMelsDwordRead_RecipeNo] ^ data[enMelsDwordRead_RecipeNo]) setWordIn_RecipeNo(data[enMelsDwordRead_RecipeNo]);
	if ((m_ReadWordData[enMelsDwordRead_RecipeName] ^ data[enMelsDwordRead_RecipeName]) |
		(m_ReadWordData[enMelsDwordRead_RecipeName + 1] ^ data[enMelsDwordRead_RecipeName + 1]) |
		(m_ReadWordData[enMelsDwordRead_RecipeName + 2] ^ data[enMelsDwordRead_RecipeName + 2]) |
		(m_ReadWordData[enMelsDwordRead_RecipeName + 3] ^ data[enMelsDwordRead_RecipeName + 3]))
		setWordIn_RecipeName(MakeRecipeName(&data[enMelsDwordRead_RecipeName]));

	if ((m_ReadWordData[enMelsDwordRead_CELL_ID] ^ data[enMelsDwordRead_CELL_ID]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 1] ^ data[enMelsDwordRead_CELL_ID + 1]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 2] ^ data[enMelsDwordRead_CELL_ID + 2]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 3] ^ data[enMelsDwordRead_CELL_ID + 3]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 4] ^ data[enMelsDwordRead_CELL_ID + 4]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 5] ^ data[enMelsDwordRead_CELL_ID + 5]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 6] ^ data[enMelsDwordRead_CELL_ID + 6]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 7] ^ data[enMelsDwordRead_CELL_ID + 7]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 8] ^ data[enMelsDwordRead_CELL_ID + 8]) |
		(m_ReadWordData[enMelsDwordRead_CELL_ID + 9] ^ data[enMelsDwordRead_CELL_ID + 9]))
		setWordIn_CELL_ID(MakeCellId(&data[enMelsDwordRead_CELL_ID]));
	if (m_ReadWordData[enMelsDwordRead_FoilExpInTopTarget] ^ data[enMelsDwordRead_FoilExpInTopTarget]) setWordIn_FoilExpInTopTarget(data[enMelsDwordRead_FoilExpInTopTarget]);
	if (m_ReadWordData[enMelsDwordRead_FoilExpInBtmTarget] ^ data[enMelsDwordRead_FoilExpInBtmTarget]) setWordIn_FoilExpInBtmTarget(data[enMelsDwordRead_FoilExpInBtmTarget]);

	if (m_ReadWordData[enMelsDwordRead_FoilExpOutTopTarget] ^ data[enMelsDwordRead_FoilExpOutTopTarget]) setWordIn_FoilExpOutTopTarget(data[enMelsDwordRead_FoilExpOutTopTarget]);
	if (m_ReadWordData[enMelsDwordRead_FoilExpOutBtmTarget] ^ data[enMelsDwordRead_FoilExpOutBtmTarget]) setWordIn_FoilExpOutBtmTarget(data[enMelsDwordRead_FoilExpOutBtmTarget]);

	if (m_ReadWordData[enMelsDwordRead_FoilExpBothTopTarget] ^ data[enMelsDwordRead_FoilExpBothTopTarget]) setWordIn_FoilExpBothTopTarget(data[enMelsDwordRead_FoilExpBothTopTarget]);
	if (m_ReadWordData[enMelsDwordRead_FoilExpBothBtmTarget] ^ data[enMelsDwordRead_FoilExpBothBtmTarget]) setWordIn_FoilExpBothBtmTarget(data[enMelsDwordRead_FoilExpBothBtmTarget]);

	if (m_ReadWordData[enMelsDwordRead_SpeterTopTarget] ^ data[enMelsDwordRead_SpeterTopTarget]) setWordIn_SpeterTopTarget(data[enMelsDwordRead_SpeterTopTarget]);
	if (m_ReadWordData[enMelsDwordRead_SpeterBtmTarget] ^ data[enMelsDwordRead_SpeterBtmTarget]) setWordIn_SpeterBtmTarget(data[enMelsDwordRead_SpeterBtmTarget]);

	if (m_ReadWordData[enMelsDwordRead_PrmContinuousCnt] ^ data[enMelsDwordRead_PrmContinuousCnt]) setWordIn_PrmContinuousCnt(data[enMelsDwordRead_PrmContinuousCnt]);
	if (m_ReadWordData[enMelsDwordRead_PrmSectorNgTabCnt] ^ data[enMelsDwordRead_PrmSectorNgTabCnt]) setWordIn_PrmSectorNgTabCnt(data[enMelsDwordRead_PrmSectorNgTabCnt]);
	if (m_ReadWordData[enMelsDwordRead_PrmSectorBaseCnt] ^ data[enMelsDwordRead_PrmSectorBaseCnt]) setWordIn_PrmSectorBaseCnt(data[enMelsDwordRead_PrmSectorBaseCnt]);

	if (m_ReadWordData[enMelsDwordRead_AlarmExistAck] ^ data[enMelsDwordRead_AlarmExistAck]) setWordIn_AlarmExistAck(data[enMelsDwordRead_AlarmExistAck]);

	memcpy(m_ReadWordData, data, sizeof(DWORD) * MELSEC_WORDINSIZE_MAX);
}
//레시피 명을 만든다.
CString CMelsecPlcIo::MakeRecipeName(DWORD* data)
{
	CString str = _T("");

	BYTE byteData[MELSEC_READRECIPENAME +1] = { 0, };
	memcpy(byteData, data, MELSEC_READRECIPENAME);
	byteData[MELSEC_READRECIPENAME] = '\0';
	str = (char*)byteData;

	LOGDISPLAY_SPEC(2)(_T("RecipeName :	%s"), str);

	return str;
}
//CELL ID를 만든다.
CString CMelsecPlcIo::MakeCellId(DWORD* data)
{
	CString str = _T("");

	BYTE byteData[MELSEC_READCELLID + 1] = { 0, };
	memcpy(byteData, data, MELSEC_READCELLID);
	byteData[MELSEC_READCELLID] = '\0';
	str = (char*)byteData;

	LOGDISPLAY_SPEC(2)(_T("Cell Id :	%s"), str);

	return str;
}


//PLC write Bit Make 함수
int CMelsecPlcIo::WritePlcBitDataMake(BYTE** data)
{
	//Write Data Block Num
	int ret = -1;

	int port = (enMelsBitOut_Alive >> 8) & 0xff;
	int shift = enMelsBitOut_Alive & 0xff;
	if (isBitOut_Alive()) ret = port;
	*(*data + port) |= ((getBitOut_Alive() & 0x1) << shift);	


	port = (enMelsBitOut_Ready >> 8) & 0xff;
	shift = enMelsBitOut_Ready & 0xff;
	if (isBitOut_Ready()) ret = port;
	*(*data + port) |= ((getBitOut_Ready() & 0x1) << shift);


	port = (enMelsBitOut_EncoderSet >> 8) & 0xff;
	shift = enMelsBitOut_EncoderSet & 0xff;
	if (isBitOut_EncoderSet()) ret = port;
	*(*data + port) |= ((getBitOut_EncoderSet() & 0x1) << shift);

	port = (enMelsBitOut_TabZeroReset >> 8) & 0xff;
	shift = enMelsBitOut_TabZeroReset & 0xff;
	if (isBitOut_TabZeroReset()) ret = port;
	*(*data + port) |= ((getBitOut_TabZeroReset() & 0x1) << shift);

	port = (enMelsBitOut_DiskSpaceWarning >> 8) & 0xff;
	shift = enMelsBitOut_DiskSpaceWarning & 0xff;
	if (isBitOut_DiskSpaceWarning()) ret = port;
	*(*data + port) |= ((getBitOut_DiskSpaceWarning() & 0x1) << shift);

	static char b = 0;
	b ^= 0x1;
	port = (enMelsBitOut_DiskSpaceAlarm >> 8) & 0xff;
	shift = enMelsBitOut_DiskSpaceAlarm & 0xff;
	if (isBitOut_DiskSpaceAlarm()) ret = port;
	*(*data + port) |= ((getBitOut_DiskSpaceAlarm() & 0x1) << shift);

	port = (enMelsBitOut_RecipeChangeAck >> 8) & 0xff;
	shift = enMelsBitOut_RecipeChangeAck & 0xff;
	if (isBitOut_RecipeChangeAck()) ret = port;
	*(*data + port) |= ((getBitOut_RecipeChangeAck() & 0x1) << shift);


	port = (enMelsBitOut_LotStartReqAck >> 8) & 0xff;
	shift = enMelsBitOut_LotStartReqAck & 0xff;
	if (isBitOut_LotStartReqAck()) ret = port;
	*(*data + port) |= ((getBitOut_LotStartReqAck() & 0x1) << shift);


	port = (enMelsBitOut_LotEndReqAck >> 8) & 0xff;
	shift = enMelsBitOut_LotEndReqAck & 0xff;
	if (isBitOut_LotEndReqAck()) ret = port;
	*(*data + port) |= ((getBitOut_LotEndReqAck() & 0x1) << shift);

	port = (enMelsBitOut_AlramResetAck >> 8) & 0xff;
	shift = enMelsBitOut_AlramResetAck & 0xff;
	if (isBitOut_AlarmResetAck()) ret = port;
	*(*data + port) |= ((getBitOut_AlarmResetAck() & 0x1) << shift);

	port = (enMelsBitOut_AlramNgResetAck >> 8) & 0xff;
	shift = enMelsBitOut_AlramNgResetAck & 0xff;
	if (isBitOut_AlarmNgResetAck()) ret = port;
	*(*data + port) |= ((getBitOut_AlarmNgResetAck() & 0x1) << shift);


//버퍼 block 위치에 + 1 = 크기(size)
//쓰기 영역에 쓸 데이터 크기
	return ret + 1;
}

//PLC write Bit Make 함수
int CMelsecPlcIo::WritePlcWordDataMake(DWORD** data)
{
	int ret = -1;
	setWordOut_DataReportV1_Ea(getWordOut_DataReportV1_Ea() + 1);
	int idx = enMelsDwordWrite_DataReportV1_Ea;;
	if (isWordOut_DataReportV1_Ea()) ret = idx;
	*(*data + idx) = getWordOut_DataReportV1_Ea();

	idx = enMelsDwordWrite_DataReportV2_OK;;
	if (isWordOut_DataReportV2_OK()) ret = idx;
	*(*data + idx) = getWordOut_DataReportV2_OK();

	idx = enMelsDwordWrite_DataReportV3_NG;;
	if (isWordOut_DataReportV3_NG()) ret = idx;
	*(*data + idx) = getWordOut_DataReportV3_NG();

	idx = enMelsDwordWrite_DataReportV4_OkRate;;
	if (isWordOut_DataReportV4_OkRate()) ret = idx;
	*(*data + idx) = getWordOut_DataReportV4_OkRate();

	idx = enMelsDwordWrite_DataReportV5_NgRate;;
	if (isWordOut_DataReportV5_NgRate()) ret = idx;
	*(*data + idx) = getWordOut_DataReportV5_NgRate();

	idx = enMelsDwordWrite_DataReportV6_RunRate;;
	if (isWordOut_DataReportV6_RunRate()) ret = idx;
	*(*data + idx) = getWordOut_DataReportV6_RunRate();

	idx = enMelsDwordWrite_Continue_Alarm_Cnt;;
	if (isWordOut_Continue_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_Continue_Alarm_Cnt();

	idx = enMelsDwordWrite_Heavy_Alarm_Cnt;;
	if (isWordOut_Heavy_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_Heavy_Alarm_Cnt();

	idx = enMelsDwordWrite_FoilExpInTop_Alarm_Cnt;;
	if (isWordOut_FoilExpInTop_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpInTop_Alarm_Cnt();

	idx = enMelsDwordWrite_FoilExpInBottom_Alarm_Cnt;;
	if (isWordOut_FoilExpInBtm_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpInBtm_Alarm_Cnt();

	idx = enMelsDwordWrite_FoilExpOutTop_Alarm_Cnt;
	if (isWordOut_FoilExpOutTop_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpOutTop_Alarm_Cnt();

	idx = enMelsDwordWrite_FoilExpOutBottom_Alarm_Cnt;;
	if (isWordOut_FoilExpOutBtm_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpOutBtm_Alarm_Cnt();

	idx = enMelsDwordWrite_FoilExpBothTop_Alarm_Cnt;;
	if (isWordOut_FoilExpBothTop_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpBothTop_Alarm_Cnt();

	idx = enMelsDwordWrite_FoilExpBothBottom_Alarm_Cnt;;
	if (isWordOut_FoilExpBothBtm_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpBothBtm_Alarm_Cnt();

	idx = enMelsDwordWrite_SpeterTop_Alarm_Cnt;;
	if (isWordOut_SpeterTop_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_SpeterTop_Alarm_Cnt();

	idx = enMelsDwordWrite_SpeterBtm_Alarm_Cnt;;
	if (isWordOut_SpeterBtm_Alarm_Cnt()) ret = idx;
	*(*data + idx) = getWordOut_SpeterBtm_Alarm_Cnt();

	idx = enMelsDwordWrite_Top_Defect_Count_Real;;
	if (isWordOut_Top_Defect_Count_Real()) ret = idx;
	*(*data + idx) = getWordOut_Top_Defect_Count_Real();

	idx = enMelsDwordWrite_Btm_Defect_Count_Real;;
	if (isWordOut_Btm_Defect_Count_Real()) ret = idx;
	*(*data + idx) = getWordOut_Btm_Defect_Count_Real();

	idx = enMelsDwordWrite_Top_Defect_Count_LotEnd;;
	if (isWordOut_Top_Defect_Count_LotEnd()) ret = idx;
	*(*data + idx) = getWordOut_Top_Defect_Count_LotEnd();

	idx = enMelsDwordWrite_Btm_Defect_Count_LotEnd;;
	if (isWordOut_Btm_Defect_Count_LotEnd()) ret = idx;
	*(*data + idx) = getWordOut_Btm_Defect_Count_LotEnd();

	idx = enMelsDwordWrite_FoilExpInTopTarget;;
	if (isWordOut_FoilExpInTopTarget()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpInTopTarget();

	idx = enMelsDwordWrite_FoilExpInBottomTarget;;
	if (isWordOut_FoilExpInBtmTarget()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpInBtmTarget();

	idx = enMelsDworddWrite_FoilExpOutTopTarget;;
	if (isWordOut_FoilExpOutTopTarget()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpOutTopTarget();

	idx = enMelsDwordWrite_FoilExpOutBottomTarget;;
	if (isWordOut_FoilExpOutBtmTarget()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpOutBtmTarget();

	idx = enMelsDwordWrite_FoilExpBothTopTarget;;
	if (isWordOut_FoilExpBothTopTarget()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpBothTopTarget();

	idx = enMelsDwordWrite_FoilExpBothBottomTarget;;
	if (isWordOut_FoilExpBothBtmTarget()) ret = idx;
	*(*data + idx) = getWordOut_FoilExpBothBtmTarget();

	idx = enMelsDwordWrite_SpeterTopTarget;;
	if (isWordOut_SpeterTopTarget()) ret = idx;
	*(*data + idx) = getWordOut_SpeterTopTarget();

	idx = enMelsDwordWrite_SpeterBtmTarget;;
	if (isWordOut_SpeterBtmTarget()) ret = idx;
	*(*data + idx) = getWordOut_SpeterBtmTarget();

	idx = enMelsDwordWrite_PrmContinuousCnt;;
	if (isWordOut_PrmContinuousCnt()) ret = idx;
	*(*data + idx) = getWordOut_PrmContinuousCnt();

	idx = enMelsDwordWrite_PrmSectorNgTabCnt;;
	if (isWordOut_PrmSectorNgTabCnt()) ret = idx;
	*(*data + idx) = getWordOut_PrmSectorNgTabCnt();

	idx = enMelsDwordWrite_PrmSectorBaseCnt;;
	if (isWordOut_PrmSectorBaseCnt()) ret = idx;
	*(*data + idx) = getWordOut_PrmSectorBaseCnt();

	idx = enMelsDwordWrite_AlarmExist;;
	if (isWordOut_AlarmExist()) ret = idx;
	*(*data + idx) = getWordOut_AlarmExist();

	idx = enMelsDwordWrite_AlarmCode_Buffer1 + (MELSEC_COUNT_ALRAMBUFF - 1);
	if (isWordOut_AlarmCode_Buffer()) ret = idx;
	for (int num = 0; num < MELSEC_COUNT_ALRAMBUFF; num++)
	{
		*(*data+enMelsDwordWrite_AlarmCode_Buffer1 + num) = getWordOut_AlarmCode_Buffer(num);
	}

	idx = enMelsDwordWrite_Cell_Trigger_ID;;
	if (isWordOut_Cell_Trigger_ID()) ret = idx;
	*(*data + idx) = getWordOut_Cell_Trigger_ID();

	idx = enMelsDwordWrite_Judge;;
	if (isWordOut_Judge()) ret = idx;
	*(*data + idx) = getWordOut_Judge();

	idx = enMelsDwordWrite_NG_Code;;
	if (isWordOut_NG_Code()) ret = idx;
	*(*data + idx) = getWordOut_NG_Code();

	//버퍼 block 위치에 + 1 = 크기(size)
	//쓰기 영역에 쓸 데이터 크기
	return ret + 1;
}

void CMelsecPlcIo::chnageEndian(short data[], int size)
{
	for (int idx = 0; idx < size; idx++)
	{
		data[idx] = SWAP_UINT16(data[idx]);
	}
}

void CMelsecPlcIo::ClosePlcIo(void)
{
	mdClose(m_pPath);
}

BOOL CMelsecPlcIo::IsOpened()
{
	if (m_pPath != 0 && m_bOpened)
	{
		return true;
	}

	return false;
}

int CMelsecPlcIo::PlcDataReadWritePorc()
{
	//PLC 데이터 Read / Write 처리 함수
	if (IsOpened())
	{
		MelsecPlcProc();
	}
	return 0;
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
	m_bMelsecAlive ^= 0x1;
	setBitOut_Alive(m_bMelsecAlive);
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
	m_bMelsecAlive ^= 0x1;
	setBitOut_Alive(m_bMelsecAlive);
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