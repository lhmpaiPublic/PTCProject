// SpcStatusInData.cpp: CSpcStatusInData 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcStatusInData.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcStatusInData::MakeStatusInDataText =
"               {\r\n"
"                     \"CATEGORY\": \"%s\",\r\n"
"                     \"TRANSFER_TIME\": \"%s\",\r\n"
"                     \"EQP_INSP_ID\": \"\",\r\n"
"                     \"PROCESS_GROUP\": \"%s\",\r\n"
"                     \"PROCESS_NAME\": \"%s\",\r\n"
"                     \"EQP_ID\": \"%s\",\r\n"
"                     \"AGENT_SW_VERSION\": \"%s\",\r\n"
"                     \"VISION_TYPE\": \"%s\",\r\n"
"                     \"INSP_STATUS\": \"%s\"\r\n"
"                 }\r\n";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcStatusInData::CSpcStatusInData(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

	m_Category = "STATUS";
	m_TransferTimer = "20230403090002757";
	m_EqpInspId = "W1ASTKM01-02";
	m_ProcessGroup = "ASSEMBLY";
	m_ProcessName = "NTC";
	m_EqpId = "W1ASTKM01-02";
	m_AgentSwVersion = "1.0";
	m_VisonType = "FOIL-EXP";
	m_InspStatus = "1";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcStatusInData::~CSpcStatusInData()
{

}

//JSON 형식의 텍스트를 만든다.
CString CSpcStatusInData::makeJSONText_Status()
{
	CString makeJSONText;
	makeJSONText.Format(MakeStatusInDataText,
		m_Category,
		m_TransferTimer,
		m_EqpInspId,
		m_ProcessGroup,
		m_ProcessName,
		m_EqpId,
		m_AgentSwVersion,
		m_VisonType,
		m_InspStatus
	);
	return makeJSONText;
}