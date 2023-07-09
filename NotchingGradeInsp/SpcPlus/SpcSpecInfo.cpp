// SpcSpecInfo.cpp: CSpcSpecInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcSpecInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char* CSpcSpecInfo::MakeSpecInfoText_1 =
"          {\r\n"
"            \"SPEC_INSP_TYPE\": \"%s\",\r\n"
"            \"SPEC_CAM_POSITION\": \"%s\",\r\n"
"            \"SPEC_DETECTION_NAME\": \"%s\",\r\n"
"%s"
"          }";

char* CSpcSpecInfo::MakeSpecInfoText_2 =
"            \"%s\": \"%s\"";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcSpecInfo::CSpcSpecInfo(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

	m_SpecInspType = "DETECT";
	m_SpecCamPosition = SPCINFO->getCameraLocation();
	m_SpecDetectonName = "PINH";
	m_SpecData = "";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcSpecInfo::~CSpcSpecInfo()
{

}

void CSpcSpecInfo::appendSpec(CString	SpecKey, CString	SpecData)
{
	CString makeSpecText;
	makeSpecText.Format(MakeSpecInfoText_2,
		SpecKey,
		SpecData
	);
	//추가된 값이 있으면  라인을 내리고 추가
	if (m_SpecData.GetLength())
	{
		m_SpecData.Append(",\r\n");
		m_SpecData.Append(makeSpecText);
	}
	else
	{
		m_SpecData.Append(makeSpecText);
	}
}

//JSON 형식의 텍스트를 만든다.
CString CSpcSpecInfo::makeJSONText_SpecInfo()
{
	CString makeJSONText;
	makeJSONText.Format(MakeSpecInfoText_1,
		m_SpecInspType,
		m_SpecCamPosition,
		m_SpecDetectonName,
		(m_SpecData+CString("\r\n")) //라인을 내리는 기능 추가된다.
	);
	return makeJSONText;
}