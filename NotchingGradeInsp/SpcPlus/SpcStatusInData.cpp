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
"            {"
"                \"CATEGORY\": \"%s\","
"                \"TRANSFER_TIME\": \"%s\","
"                \"EQP_INSP_ID\": \"\","
"                \"PROCESS_GROUP\": \"%s\","
"                \"PROCESS_NAME\": \"%s\","
"                \"EQP_ID\": \"%s\","
"                \"AGENT_SW_VERSION\": \"%s\","
"                \"VISION_TYPE\": \"%s\","
"                \"INSP_STATUS\": \"%s\""
"            }";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcStatusInData::CSpcStatusInData(CSpcPlusManager* sMgr)
{
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
