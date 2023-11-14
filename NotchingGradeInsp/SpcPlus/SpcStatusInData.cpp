// SpcStatusInData.cpp: CSpcStatusInData 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcStatusInData.h"
#include "Win32File.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcStatusInData::MakeStatusInDataText =
"      {\r\n"
"        \"CATEGORY\": \"%s\",\r\n"
"        \"TRANSFER_TIME\": \"%s\",\r\n"
"        \"EQP_INSP_ID\": \"%s\",\r\n"
"        \"PROCESS_GROUP\": \"%s\",\r\n"
"        \"PROCESS_NAME\": \"%s\",\r\n"
"        \"EQP_ID\": \"%s\",\r\n"
"        \"AGENT_SW_VERSION\": \"%s\",\r\n"
"        \"VISION_TYPE\": \"%s\",\r\n"
"        \"INSP_STATUS\": \"%s\"\r\n"
"      }\r\n";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcStatusInData::CSpcStatusInData(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

	//객체 생성시간
	m_CreateTime = CGlobalFunc::strLocalTime();

	m_Category = "STATUS";
	m_TransferTimer = m_CreateTime;
	m_EqpInspId = SPCINFO->getEqpInspId();
	m_ProcessGroup = SPCINFO->getProcessGroup();
	m_ProcessName = SPCINFO->getProcessName();
	m_EqpId = SPCINFO->getEqpId();
	m_AgentSwVersion = SPCINFO->getAgentSwVersion();
	m_VisonType = SPCINFO->getVisionType();
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

//파일저장경로를 넘긴다.
CString CSpcStatusInData::JsonFilePath()
{
	CString path = SPCINFO->getStatusPath()+ m_CreateTime.Mid(0, 6);
	if (CWin32File::FolderFileExists(path) == FALSE)
	{
		CWin32File::CreateDirectory(path);
	}
	return path;
}

//파일명을 넘긴다.
CString CSpcStatusInData::JsonFileName()
{
	return m_CreateTime + CString("_") + CString("STATUS.JSON");
}