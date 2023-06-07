// SpcInDataDefectInfo.cpp: CSpcInDataDefectInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcInDataDefectInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcInDataDefectInfo::MakeInDataDefectInfoText =
"											{"
"			                       				\"DEFECT_INDEX\": \"%s\","
"			                       				\"DEFECT_TYPE_RULE_BASE_NAME\" : \"%s\","
"			                       				\"DEFECT_TYPE_RULE_BASE_NAME_REAL\" : \"%s\","
"			                       				\"DEFECT_CAMERA_TYPE\" : \"%s\","
"			                       				\"DEFECT_CAMERA_LOCATION\" : \"%s\","
"			                       				\"DEFECT_CAMERA_NUMBER\" : \"%s\","
"			                       				\"DEFECT_SCREEN_NUMBER\" : \"%s\","
"			                       				\"DEFECT_INSP_POSITION\" : \"%s\","
"			                       				\"DEFECT_CRITERIA_POINT_X\" : \"%s\","
"			                       				\"DEFECT_CRITERIA_POINT_Y\" : \"%s\","
"			                       				\"DEFECT_RELATIVE_POINT_R_PX\" : \"%s\","
"			                       				\"DEFECT_RELATIVE_POINT_ANGLE\" : \"%s\","
"			                       				\"DEFECT_ABSOLUTE_IMG_X\" : \"%s\","
"			                       				\"DEFECT_ABSOLUTE_IMG_Y\" : \"%s\","
"			                       				\"DEFECT_IMG_WIDTH\" : \"%s\","
"			                       				\"DEFECT_IMG_LENGTH\" : \"%s\","
"			                       				\"DEFECT_AREA_PIXELS\" : \"%s\","
"			                       				\"DEFECT_GRAY_LV_AVG\" : \"%s\","
"			                       				\"DEFECT_GRAY_LV_MAX\" : \"%s\","
"			                       				\"DEFECT_GRAY_LV_MIN\" : \"%s\","
"			                       				\"DEFECT_GRAY_LV_RANGE\" : \"%s\","
"			                       				\"DEFECT_GRAY_LV_STDDEV\" : \"%s\","
"			                       				\"DEFECT_RATIO_WIDTH_LENGTH\" : \"%s\","
"			                       				\"DEFECT_ANGLE\" : \"%s\","
"			                       				\"DEFECT_ELONGATION\" : \"%s\","
"			                       				\"DEFECT_BOUNDARY_PIXEL_LENGTH\" : \"%s\","
"			                       				\"DEFECT_ROI_GRAY_LV_MAX\" : \"%s\","
"			                       				\"DEFECT_ROI_GRAY_LV_AVG\" : \"%s\","
"			                       				\"DEFECT_ROI_GRAY_LV_MIN\" : \"%s\","
"			                       				\"DEFECT_ROI_GRAY_LV_MEDIAN\" : \"%s\","
"			                       				\"DEFECT_ROI_GRAY_LV_STDDEV\" : \"%s\","
"			                       				\"DEFECT_INOUT\" : \"%s\","
"			                       				\"DEFECT_CROP_IMAGE_FILE_NAME\" : \"%s\""
"											]";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInDataDefectInfo::CSpcInDataDefectInfo(CSpcPlusManager* sMgr)
{
	m_DefectIndex = "1";
	m_DefectTypeRuleBaseName = "PINH";
	m_DefectTypeRuleBaseNameReal = "Pinhole";
	m_DefectCameraType = "LINE";
	m_DefectCameraLocation = "TOP";
	m_DefectCameraNumber = "1";
	m_DefectScreenNumber = "1";
	m_DefectInspPosition = "TAB-A";
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
	m_DefectCropImageFileName = "FOIL-EXP_20230403090002974_A1ANTC203_ECCCF41122_NA_TOP_1_1_NG_NG.jpg";

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
