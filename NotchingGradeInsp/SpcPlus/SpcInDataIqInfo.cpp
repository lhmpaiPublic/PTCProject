// SpcInDataIqInfo.cpp: CSpcInDataIqInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcInDataIqInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcInDataIqInfo::MakeInDataIqInfoText =
"											{"
"			                       				\"IQ_CAMERA_LOCATION\": \"%s\","
"			                       				\"IQ_CAMERA_NUMBER\" : \"%s\","
"			                       				\"IQ_SCREEN_NUMBER\" : \"%s\","
"			                       				\"IQ_SCREEN_IMAGE_SIZE_X\" : \"%s\","
"			                       				\"IQ_SCREEN_IMAGE_SIZE_Y\" : \"%s\","
"			                       				\"IQ_FOCUS_VALUE\" : \"%s\","
"			                       				\"IQ_BRIGHT_VALUE\" : \"%s\","
"			                       				\"IQ_RESOL_X_VALUE\" : \"%s\","
"			                       				\"IQ_RESOL_Y_VALUE\" : \"%s\","
"			                       				\"IQ_CAM_ANGLE_VALUE\" : \"%s\","
"			                       				\"IQ_CAMERA_GAIN\" : \"%s\","
"			                       				\"IQ_EXPOSURE_TIME\" : \"%s\","
"			                       				\"IMAGE_JUDGE\" : \"%s\","
"			                       				\"IMAGE_FILE_NAME\" : \"%s\""
"											]";


//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInDataIqInfo::CSpcInDataIqInfo(CSpcPlusManager* sMgr)
{
	m_IqCameraLocation = "TOP";
	m_IqCameraNumber = "1";
	m_IqScreenNumber = "1";
	m_IqScreenImageSizeX = "1024";
	m_IqScreenImageSizeY = "1024";
	m_IqFocusValue = "0.000";
	m_IqBrightValue = "0.000";
	m_IqResolXValue = "0.000";
	m_IqResolYValue = "0.000";
	m_IqCamAngleValue = "0.000";
	m_IqCameraGain = "0.000";
	m_IqExposureTime = "0.000";
	m_ImageJudge = "NG";
	m_ImageFileName = "FOIL-EXP_20230403090002974_A1ANTC203_ECCCF41122_NA_TOP_1_1_NG_NG.jpg";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcInDataIqInfo::~CSpcInDataIqInfo()
{

}
