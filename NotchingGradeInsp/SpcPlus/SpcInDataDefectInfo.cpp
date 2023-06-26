// SpcInDataDefectInfo.cpp: CSpcInDataDefectInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcInDataDefectInfo.h"
#include "SpcInspManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char* CSpcInDataDefectInfo::MakeInDataDefectInfoText =
"          {\r\n"
"            \"DEFECT_INDEX\": \"%s\",\r\n"
"            \"DEFECT_TYPE_RULE_BASE_NAME\" : \"%s\",\r\n"
"            \"DEFECT_TYPE_RULE_BASE_NAME_REAL\" : \"%s\",\r\n"
"            \"DEFECT_CAMERA_TYPE\" : \"%s\",\r\n"
"            \"DEFECT_CAMERA_LOCATION\" : \"%s\",\r\n"
"            \"DEFECT_CAMERA_NUMBER\" : \"%s\",\r\n"
"            \"DEFECT_SCREEN_NUMBER\" : \"%s\",\r\n"
"            \"DEFECT_INSP_POSITION\" : \"%s\",\r\n"
"            \"DEFECT_CRITERIA_POINT_X\" : \"%s\",\r\n"
"            \"DEFECT_CRITERIA_POINT_Y\" : \"%s\",\r\n"
"            \"DEFECT_RELATIVE_POINT_R_PX\" : \"%s\",\r\n"
"            \"DEFECT_RELATIVE_POINT_ANGLE\" : \"%s\",\r\n"
"            \"DEFECT_ABSOLUTE_IMG_X\" : \"%s\",\r\n"
"            \"DEFECT_ABSOLUTE_IMG_Y\" : \"%s\",\r\n"
"            \"DEFECT_IMG_WIDTH\" : \"%s\",\r\n"
"            \"DEFECT_IMG_LENGTH\" : \"%s\",\r\n"
"            \"DEFECT_AREA_PIXELS\" : \"%s\",\r\n"
"            \"DEFECT_GRAY_LV_AVG\" : \"%s\",\r\n"
"            \"DEFECT_GRAY_LV_MAX\" : \"%s\",\r\n"
"            \"DEFECT_GRAY_LV_MIN\" : \"%s\",\r\n"
"            \"DEFECT_GRAY_LV_RANGE\" : \"%s\",\r\n"
"            \"DEFECT_GRAY_LV_STDDEV\" : \"%s\",\r\n"
"            \"DEFECT_RATIO_WIDTH_LENGTH\" : \"%s\",\r\n"
"            \"DEFECT_ANGLE\" : \"%s\",\r\n"
"            \"DEFECT_ELONGATION\" : \"%s\",\r\n"
"            \"DEFECT_BOUNDARY_PIXEL_LENGTH\" : \"%s\",\r\n"
"            \"DEFECT_ROI_GRAY_LV_MAX\" : \"%s\",\r\n"
"            \"DEFECT_ROI_GRAY_LV_AVG\" : \"%s\",\r\n"
"            \"DEFECT_ROI_GRAY_LV_MIN\" : \"%s\",\r\n"
"            \"DEFECT_ROI_GRAY_LV_MEDIAN\" : \"%s\",\r\n"
"            \"DEFECT_ROI_GRAY_LV_STDDEV\" : \"%s\",\r\n"
"            \"DEFECT_INOUT\" : \"%s\",\r\n"
"            \"DEFECT_CROP_IMAGE_FILE_NAME\" : \"%s\"\r\n"
"          }";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInDataDefectInfo::CSpcInDataDefectInfo(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = dynamic_cast<CSpcInspManager*>(sMgr);

	m_DefectIndex = "0";
	m_DefectTypeRuleBaseName = "";
	m_DefectTypeRuleBaseNameReal = "";
	m_DefectCameraType = SPCINFO->getCameraType();
	m_DefectCameraLocation = SPCINFO->getCameraLocation();
	m_DefectCameraNumber = "1";
	m_DefectScreenNumber = "1";
	m_DefectInspPosition = SPCINFO->getInspPositionTop();
	m_DefectCriteriaPointX = "0.000";
	m_DefectCriteriaPointY = "0.000";
	m_DefectRelativePointRPx = "0.000";
	m_DefectRelativePointAngle = "0.000";
	m_DefectAbsoluteImgX = "0.000";
	m_DefectAbsoluteImgY = "0.000";
	m_DefectImgWidth = "0.000";
	m_DefectImgLength = "0.000";
	m_DefectAreaPixels = "0.000";
	m_DefectGrayLvAvg = "0.000";
	m_DefectGrayLvMax = "0.000";
	m_DefectGrayLvMin = "0.000";
	m_DefectGrayLvRange = "0.000";
	m_DefectGrayLvStddev = "0.000";
	m_DefectRatioWidthLength = "0.000";
	m_DefectAngle = "0.000";
	m_DefectElongation = "0.000";
	m_DefectBoundaryPixelLength = "0.000";
	m_DefectRoiGrayLvMax = "0.000";
	m_DefectRoiGrayLvAvg = "0.000";
	m_DefectRoiGrayLvMin = "0.000";
	m_DefectRoiGrayLvMedian = "0.000";
	m_DefectRoiGrayLvStddev = "0.000";
	m_DefectInOut = "IN";
	m_DefectCropImageFileName = "";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcInDataDefectInfo::~CSpcInDataDefectInfo()
{

}

//JSON 형식의 텍스트를 만든다.
CString CSpcInDataDefectInfo::makeJSONText_DefectInfo()
{
	CString makeJSONText;
	makeJSONText.Format(MakeInDataDefectInfoText,
			m_DefectIndex,
			m_DefectTypeRuleBaseName,
			m_DefectTypeRuleBaseNameReal,
			m_DefectCameraType,
			m_DefectCameraLocation,
			m_DefectCameraNumber,
			m_DefectScreenNumber,
			m_DefectInspPosition,
			m_DefectCriteriaPointX,
			m_DefectCriteriaPointY,
			m_DefectRelativePointRPx,
			m_DefectRelativePointAngle,
			m_DefectAbsoluteImgX,
			m_DefectAbsoluteImgY,
			m_DefectImgWidth,
			m_DefectImgLength,
			m_DefectAreaPixels,
			m_DefectGrayLvAvg,
			m_DefectGrayLvMax,
			m_DefectGrayLvMin,
			m_DefectGrayLvRange,
			m_DefectGrayLvStddev,
			m_DefectRatioWidthLength,
			m_DefectAngle,
			m_DefectElongation,
			m_DefectBoundaryPixelLength,
			m_DefectRoiGrayLvMax,
			m_DefectRoiGrayLvAvg,
			m_DefectRoiGrayLvMin,
			m_DefectRoiGrayLvMedian,
			m_DefectRoiGrayLvStddev,
			m_DefectInOut,
			m_DefectCropImageFileName
		);
	return makeJSONText;
}

//INSP 이미지 파일명을 넘긴다.
CString CSpcInDataDefectInfo::ImagCropFileName()
{
	//①검사기종류_②파일생성시간_③설비ID_④RECIPE_ID_⑤LOT_ID_⑥Cell ID_⑦카메라 위치_⑧카메라 번호_⑨스크린 번호_⑩검사위치_⑪최종판정결과_⑫불량명_⑬불량좌표X_⑭불량좌표Y.JPG
	CSpcInspInData* InspInData = manager->getSpcInspInData();
	return InspInData->getVisionType() + CString("_") + InspInData->getCreateTime() + CString("_") + InspInData->getEqpId() + CString("_") + InspInData->getLotId() + CString("_") + InspInData->getCellId() + CString("_") + m_DefectCameraLocation + CString("_") + m_DefectCameraNumber + CString("_") + m_DefectScreenNumber + CString("_") + m_DefectInspPosition + CString("_") + InspInData->getCellFinalJudge() + CString("_") + m_DefectTypeRuleBaseName + CString("_") + m_DefectAbsoluteImgX + CString("_") + m_DefectAbsoluteImgY + CString(".JPG");
}