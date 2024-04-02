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
	enSmsBitOut_Alive = 0,	// 
	enSmsBitOut_Ready = 1,	// 
	enSmsBitOut_EncoderSet = 2,	// 
	enSmsBitOut_RecipeChangeAck = 3,	// 
	enSmsBitOut_LotStartReqAck = 4,	// 
	enSmsBitOut_LotEndReqAck = 5,	// 
	enSmsBitOut_TabZeroReset = 6,	// 
	enSmsBitOut_AlarmResetAck = 7,	// 
	enSmsBitOut_AlarmNgResetAck = 8,	// 

	enSmsBitOut_DiskSpaceWarning = 9,	// Address Map에 없음
	enSmsBitOut_DiskSpaceAlarm = 10,	// Address Map에 없음
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

	enSmsWordReadMaxSize = 33,
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
	enSmsWordWrite_AlarmCode_Buffer1 = 41, 
	enSmsWordWrite_AlarmCode_Buffer2 = 42, 
	enSmsWordWrite_AlarmCode_Buffer3 = 43,
	enSmsWordWrite_AlarmCode_Buffer4 = 44, 
	enSmsWordWrite_AlarmCode_Buffer5 = 45,
	enSmsWordWrite_AlarmCode_Buffer6 = 46, 
	enSmsWordWrite_AlarmCode_Buffer7 = 47, 
	enSmsWordWrite_AlarmCode_Buffer8 = 48, 
	enSmsWordWrite_AlarmCode_Buffer9 = 49,
	enSmsWordWrite_AlarmCode_Buffer10 = 50, 
	enSmsWordWrite_AlarmCode_Buffer11 = 51, 
	enSmsWordWrite_AlarmCode_Buffer12 = 52, 
	enSmsWordWrite_AlarmCode_Buffer13 = 53, 
	enSmsWordWrite_AlarmCode_Buffer14 = 54, 
	enSmsWordWrite_AlarmCode_Buffer15 = 55, 
	enSmsWordWrite_AlarmCode_Buffer16 = 56, 
	enSmsWordWrite_AlarmCode_Buffer17 = 57,
	enSmsWordWrite_AlarmCode_Buffer18 = 58, 
	enSmsWordWrite_AlarmCode_Buffer19 = 59, 
	enSmsWordWrite_AlarmCode_Buffer20 = 60, 
	enSmsWordWrite_AlarmCode_Buffer21 = 61, 
	enSmsWordWrite_AlarmCode_Buffer22 = 62, 
	enSmsWordWrite_AlarmCode_Buffer23 = 63,
	enSmsWordWrite_AlarmCode_Buffer24 = 64,


	en_SmsWordWrite_Cell_Trigger_ID = 80,
	en_SmsWordWrite_Judge = 81,
	en_SmsWordWrite_NG_Code = 82,


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
	m_pThread_SiemensPlc = NULL;
	pEvent_SiemensPlc = NULL;
	if (OpenPlcIo() == 0)
	{
		//슬레이브 아이디 
		SetSlaveId(AprData.m_System.m_nSlaveID);
		//이벤트 객체 생성
		pEvent_SiemensPlc = CreateEvent(NULL, FALSE, FALSE, NULL);
		//스래드 생성
		m_pThread_SiemensPlc = AfxBeginThread(SiemensPlc_ThreadProc, this);
	}
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
#define SIEMENSPLC_TIMEOUT 100
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
		//Read bit 영역 읽기
		short	ReadBitData[SIENENS_READBITDATA];
		//bit 읽기 영역 읽기
		ReadDataReg(AprData.m_System.m_nBitIn, ReadBitData, SIENENS_READBITDATA);
		//읽은 Bit 데이터 파싱
		if (std::equal(std::begin(ReadBitData), std::end(ReadBitData), std::begin(m_ReadBitData)) == false)
		{
			ReadPlcBitDataParser(ReadBitData);
		}

		//Read word 영역 읽기
		short	ReadWordData[SIENENS_READWORDDATA];
		//word 읽기 영역 읽기
		ReadDataReg(AprData.m_System.m_nWordIn, ReadWordData, SIENENS_READWORDDATA);
		//읽은 Word 데이터 파싱
		if (std::equal(std::begin(ReadWordData), std::end(ReadWordData), std::begin(m_ReadWordData)) == false)
		{
			ReadPlcWordDataParser(ReadWordData);
		}

		//쓰기 데이터 만들기
		WritePlcDataMake();
		//쓰기
		WriteDataReg(AprData.m_System.m_nBitOut, m_WriteBitData, SIENENS_WRITEBITDATA);
	}
}

//PLC read Data Parser 함수
void CSiemensPlcIo::ReadPlcBitDataParser(short* data)
{
#ifdef NEW_PLCTYPE
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
#endif //NEW_PLCTYPE

	memcpy(m_ReadBitData, data, sizeof(short) * SIENENS_READBITDATA);
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
	return strBuffer;
}

void CSiemensPlcIo::ReadPlcWordDataParser(short* data)
{
#ifdef NEW_PLCTYPE
	if (m_ReadWordData[enSmsWordRead_RecipeNo] ^ data[enSmsWordRead_RecipeNo]) setWordIn_RecipeNo(data[enSmsWordRead_RecipeNo] & 0xffff);
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

#endif //NEW_PLCTYPE

	memcpy(m_ReadWordData, data, sizeof(short) * SIENENS_READWORDDATA);
}

//PLC write Data Make 함수
void CSiemensPlcIo::WritePlcDataMake()
{

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
	
	if (!m_pLGIS_Plc->CheckConnection())
	{
		ClosePlcIo();
		//로그출력
		LOGDISPLAY_SPECTXT(0)("PLC Siemens Open failed");
		ret = -1;
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
