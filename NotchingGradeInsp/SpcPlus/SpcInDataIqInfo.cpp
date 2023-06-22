// SpcInDataIqInfo.cpp: CSpcInDataIqInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcInDataIqInfo.h"
#include "SpcInspManager.h"


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
	manager = dynamic_cast<CSpcInspManager*>(sMgr);

	m_IqCameraLocation = SPCINFO->getCameraLocation();
	m_IqCameraNumber = "1";
	m_IqScreenNumber = "1";
	m_IqScreenImageSizeX = "1024";
	m_IqScreenImageSizeY = "1024";
	m_IqFocusValue = SPCINFO->getIqFocusValue();
	m_IqBrightValue = SPCINFO->getIqBrightValue();
	m_IqResolXValue = SPCINFO->getIqResolXValue();
	m_IqResolYValue = SPCINFO->getIqResolYValue();
	m_IqCamAngleValue = SPCINFO->getCamAngleValue();
	m_IqCameraGain = SPCINFO->getCameraGain();
	m_IqExposureTime = SPCINFO->getCameraExposureTime();
	m_ImageJudge = "NG";
	m_ImageFileName = "";

	m_CameraLocation = SPCINFO->getCameraLocation();
	m_CameraNumber = "1";
	m_ScreenNumber = "1";

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
//INSP 이미지 파일명을 넘긴다.
CString CSpcInDataIqInfo::ImagIqFileName()
{
	//①검사기종류_②파일생성시간_③설비ID_④RECIPE_ID_⑤LOT_ID_⑥Cell ID_⑦카메라 위치_⑧카메라 번호_⑨스크린 번호_⑩검사위치_⑮ 이미지판정결과_⑫ 최종판정결과
	CSpcInspInData* InspInData = manager->getSpcInspInData();
	return InspInData->getVisionType() + CString("_") + InspInData->getCreateTime() + CString("_") + InspInData->getEqpId() + CString("_") + InspInData->getLotId() + CString("_") + InspInData->getCellId() + CString("_") + m_CameraLocation + CString("_") + m_CameraNumber + CString("_") + m_ScreenNumber + CString("_") + m_ImageJudge + CString("_") + InspInData->getCellFinalJudge() + CString(".JPG");
}