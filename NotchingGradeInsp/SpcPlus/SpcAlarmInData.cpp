// SpcAlarmInData.cpp: CSpcAlarmInData 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcAlarmInData.h"
#include "Win32File.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char* CSpcAlarmInData::MakeAlarmInDataText =
"               {\r\n"
"                     \"CATEGORY\": \"%s\",\r\n"
"                     \"EQP_INSP_ID\": \"%s\",\r\n"
"                     \"PROCESS_GROUP\": \"%s\",\r\n"
"                     \"PROCESS_NAME\": \"%s\",\r\n"
"                     \"LINE_NUMBER\": \"%s\",\r\n"
"                     \"MACHINE_NUMBER\": \"%s\",\r\n"
"                     \"LANE_NUMBER\": \"%s\",\r\n"
"                     \"VISION_TYPE\": \"%s\",\r\n"
"                     \"EQP_ID\": \"%s\",\r\n"
"                     \"ALARM_FLAG\": \"%s\",\r\n"
"                     \"ALARM_INTO_DATE\": \"%s\",\r\n"
"                     \"ALARM_HOST\": \"%s\",\r\n"
"                     \"ALARM_LEVEL\": \"%s\",\r\n"
"                     \"ALARM_CODE\": \"%s\",\r\n"
"                     \"ALARM_NAME\": \"%s\",\r\n"
"                     \"ALARM_ALL_RESET_FLAG\": \"%s\",\r\n"
"                     \"ALARM_OUTTO_DATE\": \"%s\",\r\n"
"                  }\r\n";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcAlarmInData::CSpcAlarmInData(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

	//객체 생성시간
	m_CreateTime = CGlobalFunc::strLocalTime();

	m_Category = "ALARM";
	m_EqpInspId = SPCINFO->getEqpInspId();
	m_ProcessGroup = SPCINFO->getProcessGroup();
	m_ProcessName = SPCINFO->getProcessName();
	m_MachineNumber = SPCINFO->getMachineNumber();
	m_LineNumber = SPCINFO->getLineNumber();
	m_LaneNumber = SPCINFO->getLaneNumber();
	m_VisionType = SPCINFO->getVisionType();
	m_EqpId = SPCINFO->getEqpId();
	m_AlarmFlag = "FALSE";
	m_AlarmInfoDate = CGlobalFunc::strLocalTime();
	m_AlarmHost = "VISION";
	m_AlarmLevel = "LIGHT";
	m_AlarmCode = "9999";
	m_AlarmName = "--알람";
	m_AlarmAllResetFlag = "TRUE";
	m_AlarmOutToDate = m_AlarmInfoDate;

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcAlarmInData::~CSpcAlarmInData()
{

}

CString CSpcAlarmInData::makeJSONText_Alarm()
{
	CString makeJSONText;
	makeJSONText.Format(MakeAlarmInDataText,
		m_Category,
		m_EqpInspId,
		m_ProcessGroup,
		m_ProcessName,
		m_MachineNumber,
		m_LineNumber,
		m_LaneNumber,
		m_VisionType,
		m_EqpId,
		m_AlarmFlag,
		m_AlarmInfoDate,
		m_AlarmHost,
		m_AlarmLevel,
		m_AlarmCode,
		m_AlarmName,
		m_AlarmAllResetFlag,
		m_AlarmOutToDate
	);
	return makeJSONText;
}

//파일저장경로를 넘긴다.
CString CSpcAlarmInData::JsonFilePath()
{
	CString path = SPCINFO->getAlarmPath() + m_CreateTime.Mid(0, 6);
	CWin32File::CreateDirectory(path);
	return path;
}

//파일명을 넘긴다.
CString CSpcAlarmInData::JsonFileName()
{
	return m_CreateTime + CString("_") + CString("ALARM.JSON");
}