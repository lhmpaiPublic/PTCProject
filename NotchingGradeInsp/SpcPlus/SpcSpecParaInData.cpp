// SpcSpecParaInData.cpp: CSpcSpecParaInData 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcSpecParaInData.h"
#include "Win32File.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char* CSpcSpecParaInData::MakeSpecParaInDataText_1 =
"      {\r\n"
"        \"CATEGORY\": \"%s\",\r\n"
"        \"EQP_ID\": \"%s\",\r\n"
"        \"EQP_INSP_ID\": \"%s\",\r\n"
"        \"RECIPE_ID\" : \"%s\",\r\n"
"        \"INSP_SW_VERSION\": \"%s\",\r\n"
"        \"MODIFY_TIME\": \"%s\",\r\n"
"        \"MODIFIER\": \"%s\",\r\n"
"        \"MODIFIER_REASON\": \"%s\",\r\n"
"        \"PARA_INFO\": [\r\n";
char* CSpcSpecParaInData::MakeSpecParaInDataText_2 =
"        ],\r\n"
"        \"SPEC_INFO\": [\r\n";
char* CSpcSpecParaInData::MakeSpecParaInDataText_3 =
"        ]\r\n"
"      }\r\n";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcSpecParaInData::CSpcSpecParaInData(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

	//객체 생성시간
	m_CreateTime = CGlobalFunc::strLocalTime();

	m_Category = "SPEC_PARA";
	m_EqpId = SPCINFO->getEqpId();
	m_EqpInspId = SPCINFO->getEqpInspId();
	m_RecipeId = SPCINFO->getRecipeId();
	m_InspRecipe = SPCINFO->getRecipeId();
	m_InspSwVersion = SPCINFO->getAgentSwVersion();
	m_ModifyTime = CGlobalFunc::strLocalTime();
	m_Modifier = "NA";
	m_ModifyReason = "RESTART";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcSpecParaInData::~CSpcSpecParaInData()
{

}

//JSON 형식의 텍스트를 만든다.
CString CSpcSpecParaInData::makeJSONText_SpecPara1()
{
	CString makeJSONText;
	makeJSONText.Format(MakeSpecParaInDataText_1,
		m_Category,
		m_EqpId,
		m_EqpInspId,
		m_RecipeId,
		m_InspRecipe,
		m_InspSwVersion,
		m_ModifyTime,
		m_Modifier,
		m_ModifyReason
	);
	return makeJSONText;
}
CString CSpcSpecParaInData::getJSONText_SpecPara2()
{
	return MakeSpecParaInDataText_2;
}
CString CSpcSpecParaInData::getJSONText_SpecParaTail() 
{
	return MakeSpecParaInDataText_3;
}

//파일저장경로를 넘긴다.
CString CSpcSpecParaInData::JsonFilePath()
{
	CString path = SPCINFO->getSpecParaPath();
	if (CWin32File::Exists(path) == FALSE)
	{
		CWin32File::CreateDirectory(path);
	}
	return path;
}

//파일명을 넘긴다.
CString CSpcSpecParaInData::JsonFileName()
{
	return m_CreateTime + CString("_") + CString("PARAM.JSON");
}