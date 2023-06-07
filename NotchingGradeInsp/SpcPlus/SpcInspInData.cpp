// SpcInspInData.cpp: CSpcInspInData 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcInspInData.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcInspInData::MakeInspInDataText_1 =
"	                       {"
"			                       \"CATEGORY\": \"%s\","
"			                       \"VISION_INPUT_TIME\" : \"%s\","
"			                       \"VISION_OUTPUT_TIME\" : \"%s\","
"			                       \"INSP_TACTTIME\" : \"%s\","
"			                       \"INSP_PROCESSING_TIME\" : \"%s\","
"			                       \"RECIPE_ID\" : \"%s\","
"			                       \"NG_OUT\" : \"%s\","
"			                       \"EQP_ID\" : \"%s\","
"			                       \"EQP_INSP_ID\" : \"%s\","
"			                       \"PROCESS_GROUP\" : \"%s\","
"			                       \"PROCESS_NAME\" : \"%s\","
"			                       \"LINE_NUMBER\" : \"%s\","
"			                       \"MACHINE_NUMBER\" : \"%s\","
"			                       \"LANE_NUMBER\" : \"%s\","
"			                       \"VISION_TYPE\" : \"%s\","
"			                       \"PROCESS_DIRECTION\" : \"%s\","
"			                       \"LOT_ID\" : \"%s\","
"			                       \"CELL_ID\" : \"%s\","
"			                       \"CELL_COUNT_NO\" : \"%s\","
"			                       \"VIRTUAL_CELL_ID\" : \"%s\","
"			                       \"CELL_FINAL_JUDGE\" : \"%s\","
"			                       \"IQ_INFO\": [";
//+"%s"	 IQ_INFO
CString CSpcInspInData::MakeInspInDataText_2 =
"											]"
"			                       \"APPEARANCE_JUDGE_RESULT\": \"%s\","
"			                       \"TOTAL_APPEARANCE_NG_COUNT\" : \"%s\","
"			                       \"APPEARANCE_REASON_ALL\" : ["
"			                       \"%s\""
"			                       ] ,"
"			                       \"APPEARANCE_REASON_ALL_REAL\" : ["
"			                       \"%s\""
"			                       ] ,"
"			                       \"APPEARANCE_REASON_MAIN\" : \"%s\","
"			                       \"APPEARANCE_REASON_MAIN_REAL\" : \"%s\","
"			                       \"DEFECT_INFO\": [";
//+"%s"	DEFECT_INFO
CString CSpcInspInData::MakeInspInDataText_3 =
"											]"
"                      }";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInspInData::CSpcInspInData(CSpcPlusManager* sMgr)
{
	m_Category = "INSP";
	m_VisionInputTime = "20230403090002757";
	m_VisionOutputTime = "20230403090002974";
	m_InspTactTime = "100.217";
	m_InspProcessingTime = "4.776";
	m_RecipeId = "default";
	m_NgOut = "N";
	m_EqpId = "W1ASTKM01";
	m_EqpInspId = "W1ASTKM01-02";
	m_ProcessGroup = "ASSEMBLY";
	m_ProcessName = "NTC";
	m_LineNumber = "10";
	m_MachineNumber = "1";
	m_LaneNumber = "1";
	m_VisionType = "FOIL-EXP";
	m_ProcessDirection = "RIGHT";
	m_LotId = "NA";
	m_CellId = "NA";
	m_CellCountNo = "10";
	m_VirtualCellId = "NA";
	m_CellFinalJudge = "NG";
	m_AppearanceJudgeResult = "NG";
	m_TotalAppearanceNgCount = "2";
	m_AppearanceReasonAll.push_back("PINH");
	m_AppearanceReasonAll.push_back("INPH");
	m_AppearanceReasonAllReal.push_back("Pinhole");
	m_AppearanceReasonAllReal.push_back("Insulation Pinhole");
	m_AppearanceReasonMain = "PINH";
	m_AppearanceReasonMainReal = "Pinhole";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcInspInData::~CSpcInspInData()
{

}
