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

char* CSpcInspInData::MakeInspInDataText_1 =
"                         {\r\n"
"                                  \"CATEGORY\": \"%s\",\r\n"
"                                  \"VISION_INPUT_TIME\" : \"%s\",\r\n"
"                                  \"VISION_OUTPUT_TIME\" : \"%s\",\r\n"
"                                  \"INSP_TACTTIME\" : \"%s\",\r\n"
"                                  \"INSP_PROCESSING_TIME\" : \"%s\",\r\n"
"                                  \"RECIPE_ID\" : \"%s\",\r\n"
"                                  \"NG_OUT\" : \"%s\",\r\n"
"                                  \"EQP_ID\" : \"%s\",\r\n"
"                                  \"EQP_INSP_ID\" : \"%s\",\r\n"
"                                  \"PROCESS_GROUP\" : \"%s\",\r\n"
"                                  \"PROCESS_NAME\" : \"%s\",\r\n"
"                                  \"LINE_NUMBER\" : \"%s\",\r\n"
"                                  \"MACHINE_NUMBER\" : \"%s\",\r\n"
"                                  \"LANE_NUMBER\" : \"%s\",\r\n"
"                                  \"VISION_TYPE\" : \"%s\",\r\n"
"                                  \"PROCESS_DIRECTION\" : \"%s\",\r\n"
"                                  \"LOT_ID\" : \"%s\",\r\n"
"                                  \"CELL_ID\" : \"%s\",\r\n"
"                                  \"CELL_COUNT_NO\" : \"%s\",\r\n"
"                                  \"VIRTUAL_CELL_ID\" : \"%s\",\r\n"
"                                  \"CELL_FINAL_JUDGE\" : \"%s\",\r\n"
"                                  \"IQ_INFO\": [\r\n";
//+"%s"	 IQ_INFO
char* CSpcInspInData::MakeInspInDataText_2 =
"                                  ]\r\n"
"                                  \"APPEARANCE_JUDGE_RESULT\": \"%s\",\r\n"
"                                  \"TOTAL_APPEARANCE_NG_COUNT\" : \"%s\",\r\n"
"                                  \"APPEARANCE_REASON_ALL\" : [\r\n"
"                                  \"%s\"\r\n"
"                                  ] ,\r\n"
"                                  \"APPEARANCE_REASON_ALL_REAL\" : [\r\n"
"                                  \"%s\"\r\n"
"                                  ] ,\r\n"
"                                  \"APPEARANCE_REASON_MAIN\" : \"%s\",\r\n"
"                                  \"APPEARANCE_REASON_MAIN_REAL\" : \"%s\",\r\n"
"                                  \"DEFECT_INFO\": [\r\n";
//+"%s"	DEFECT_INFO
char* CSpcInspInData::MakeInspInDataText_3 =
"                                ]\r\n"
"                          }\r\n";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInspInData::CSpcInspInData(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

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
	m_AppearanceReasonAll = "";
	m_AppearanceReasonAllReal = "";
	m_AppearanceReasonMain = "PINH";
	m_AppearanceReasonMainReal = "Pinhole";

	appendAppearanceReasonAll("PINH");
	appendAppearanceReasonAll(",");
	appendAppearanceReasonAll("INPH");

	appendAppearanceReasonAllReal("Pinhole");
	appendAppearanceReasonAllReal(",");
	appendAppearanceReasonAllReal("Insulation Pinhole");

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

//JSON 형식의 텍스트를 만든다.
CString CSpcInspInData::makeJSONText_Insp1()
{
	CString makeJSONText;
	makeJSONText.Format(MakeInspInDataText_1,
		m_Category,
		m_VisionInputTime,
		m_VisionOutputTime,
		m_InspTactTime,
		m_InspProcessingTime,
		m_RecipeId,
		m_NgOut,
		m_EqpId,
		m_EqpInspId,
		m_ProcessGroup,
		m_ProcessName,
		m_LineNumber,
		m_MachineNumber,
		m_LaneNumber,
		m_VisionType,
		m_ProcessDirection,
		m_LotId,
		m_CellId,
		m_CellCountNo,
		m_VirtualCellId,
		m_CellFinalJudge
	);
	return makeJSONText;
}
CString CSpcInspInData::makeJSONText_Insp2()
{
	CString makeJSONText;
	makeJSONText.Format(MakeInspInDataText_2,
		m_AppearanceJudgeResult,
		m_TotalAppearanceNgCount,
		m_AppearanceReasonAll,
		m_AppearanceReasonAllReal,
		m_AppearanceReasonMain,
		m_AppearanceReasonMainReal
	);
	return makeJSONText;
}
CString CSpcInspInData::getSONText_InspTail()
{
	return MakeInspInDataText_3;
}