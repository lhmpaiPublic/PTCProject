// SpcAlarmInData.cpp: CSpcAlarmInData 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcAlarmInData.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcAlarmInData::MakeAlarmInDataText =
"            {"
"                \"CATEGORY\": \"%s\","
"                \"EQP_INSP_ID\": \"%s\","
"                \"PROCESS_GROUP\": \"%s\","
"                \"PROCESS_NAME\": \"%s\","
"                \"LINE_NUMBER\": \"%s\","
"                \"MACHINE_NUMBER\": \"%s\","
"                \"LANE_NUMBER\": \"%s\","
"                \"VISION_TYPE\": \"%s\","
"                \"EQP_ID\": \"%s\","
"                \"ALARM_FLAG\": \"%s\","
"                \"ALARM_INTO_DATE\": \"%s\","
"                \"ALARM_HOST\": \"%s\","
"                \"ALARM_LEVEL\": \"%s\","
"                \"ALARM_CODE\": \"%s\","
"                \"ALARM_NAME\": \"%s\","
"                \"ALARM_ALL_RESET_FLAG\": \"%s\","
"                \"ALARM_OUTTO_DATE\": \"%s\","
"            }";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcAlarmInData::CSpcAlarmInData(CSpcPlusManager* sMgr)
{
	m_Category = "ALARM";
	m_EqpInspId = "W1ASTKM01-02";
	m_ProcessGroup = "ASSEMBLY";
	m_ProcessName = "NTC";
	m_MachineNumber = "1";
	m_LineNumber = "1";
	m_LaneNumber = "1";
	m_VisionType = "FOIL-EXP";
	m_EqpId = "W1ASTKM01-02";
	m_AlarmFlag = "FALSE";
	m_AlarmInfoDate = "20230403090002757";
	m_AlarmHost = "VISION";
	m_AlarmLevel = "LIGHT";
	m_AlarmCode = "9999";
	m_AlarmName = "--알람";
	m_AlarmAllResetFlag = "TRUE";
	m_AlarmOutToDate = "20230403090002757";

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
