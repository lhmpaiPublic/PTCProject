// SpcSpecParaInData.cpp: CSpcSpecParaInData 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcSpecParaInData.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString MakeSpecParaInDataText_1 =
"		\"IN_DATA\": ["
"			{"
"				\"CATEGORY\": \"%s\","
"				\"EQP_ID\": \"%s\","
"				\"EQP_INSP_ID\": \"%s\","
"				\"RECIPE_ID\" : \"%s\","
"				\"INSP_SW_VERSION\": \"%s\","
"				\"MODIFY_TIME\": \"%s\","
"				\"MODIFIER\": \"%s\","
"				\"MODIFIER_REASON\": \"%s\","
"				\"PARA_INFO\": [";
CString MakeSpecParaInDataText_2 =
"				],"
"				\"SPEC_INFO\": [";
CString MakeSpecParaInDataText_3 =
"				]"
"			}"
"		]";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcSpecParaInData::CSpcSpecParaInData(CSpcPlusManager* sMgr)
{
	m_Category = "SPEC_PARA";
	m_EqpId = "W1ASTKM01-02";
	m_EqpInspId = "W1ASTKM01-02";
	m_RecipeId = "default";
	m_InspRecipe = "default";
	m_InspSwVersion = "1.0";
	m_ModifyTime = "20230403090002757";
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
