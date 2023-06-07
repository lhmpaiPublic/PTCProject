// SpcParaInfo.cpp: CSpcParaInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcParaInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char* CSpcParaInfo::MakeParaInfoText_1 =
"					{"
"						\"PARA_INSP_TYPE\": \"%s\","
"						\"PARA_NO\" : \"%s\","
"						\"PARA_CAM_POSITION\": \"%s\","
"						\"PARA_DETECTION_NAME\": \"%s\","
"%s"
"					},";

char* CSpcParaInfo::MakeParaInfoText_2 =
"						\"%s\": \"%s\"";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcParaInfo::CSpcParaInfo(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

	m_ParaInspType = "DEFECT";
	m_ParaNo = "1";
	m_ParaCamPosition = "TOP";
	m_ParaDetectionName = "PINH";
	m_ParaData = "";

	appendParaData("Key", "Data");

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcParaInfo::~CSpcParaInfo()
{

}

//정의된 Para Key : Value N개	
void CSpcParaInfo::appendParaData(CString	ParaKey, CString	ParaData)
{
	CString makeParaText;
	makeParaText.Format(MakeParaInfoText_2,
		ParaKey,
		ParaData
	);
	if (m_ParaData.GetLength())
	{
		m_ParaData.Append(",\r\n");
		m_ParaData.Append(makeParaText);
	}
	else
	{
		m_ParaData.Append(makeParaText);
	}
}

//JSON 형식의 텍스트를 만든다.
CString CSpcParaInfo::makeJSONText_ParaInfo()
{
	CString makeJSONText;
	makeJSONText.Format(MakeParaInfoText_1,
		m_ParaInspType,
		m_ParaNo,
		m_ParaCamPosition,
		m_ParaDetectionName,
		m_ParaData
	);
	return makeJSONText;
}