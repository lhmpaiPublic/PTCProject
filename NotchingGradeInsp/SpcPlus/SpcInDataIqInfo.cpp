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

char* CSpcInDataIqInfo::MakeInDataIqInfoText =
"                                         {\r\n"
"                                               \"IQ_CAMERA_LOCATION\": \"%s\",\r\n"
"                                               \"IQ_CAMERA_NUMBER\" : \"%s\",\r\n"
"                                               \"IQ_SCREEN_NUMBER\" : \"%s\",\r\n"
"                                               \"IQ_SCREEN_IMAGE_SIZE_X\" : \"%s\",\r\n"
"                                               \"IQ_SCREEN_IMAGE_SIZE_Y\" : \"%s\",\r\n"
"                                               \"IQ_FOCUS_VALUE\" : \"%s\",\r\n"
"                                               \"IQ_BRIGHT_VALUE\" : \"%s\",\r\n"
"                                               \"IQ_RESOL_X_VALUE\" : \"%s\",\r\n"
"                                               \"IQ_RESOL_Y_VALUE\" : \"%s\",\r\n"
"                                               \"IQ_CAM_ANGLE_VALUE\" : \"%s\",\r\n"
"                                               \"IQ_CAMERA_GAIN\" : \"%s\",\r\n"
"                                               \"IQ_EXPOSURE_TIME\" : \"%s\",\r\n"
"                                               \"IMAGE_JUDGE\" : \"%s\",\r\n"
"                                               \"IMAGE_FILE_NAME\" : \"%s\"\r\n"
"                                        }";


//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInDataIqInfo::CSpcInDataIqInfo(CSpcPlusManager* sMgr)
{
	//관리 클래스 객체 포인터
	manager = sMgr;

	m_IqCameraLocation = SPCINFO->getCameraLocation();
	m_IqCameraNumber = "1";
	m_IqScreenNumber = "1";
	m_IqScreenImageSizeX = "1024";
	m_IqScreenImageSizeY = "1024";
	m_IqFocusValue = "0.000";
	m_IqBrightValue = "0.000";
	m_IqResolXValue = "0.000";
	m_IqResolYValue = "0.000";
	m_IqCamAngleValue = "0.000";
	m_IqCameraGain = SPCINFO->getCameraGain();
	m_IqExposureTime = SPCINFO->getCameraExposureTime();
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

//JSON 형식의 텍스트를 만든다.
CString CSpcInDataIqInfo::makeJSONText_IqInfo() 
{
	CString makeJSONText;
	makeJSONText.Format(MakeInDataIqInfoText,
		m_IqCameraLocation,
		m_IqCameraNumber,
		m_IqScreenNumber,
		m_IqScreenImageSizeX,
		m_IqScreenImageSizeY,
		m_IqFocusValue,
		m_IqBrightValue,
		m_IqResolXValue,
		m_IqResolYValue,
		m_IqCamAngleValue,
		m_IqCameraGain,
		m_IqExposureTime,
		m_ImageJudge,
		m_ImageFileName
	);
	return makeJSONText;
}