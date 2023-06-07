// SpcInDataIqInfo.h : CSpcInDataIqInfo 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCINDATAIQINFO_H
#define SPCINDATAIQINFO_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcInDataIqInfo
{
public:
	CSpcInDataIqInfo(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcInDataIqInfo() ;

private:
	//Camera 위치(Top, Bottom) 
	//디폴트 값 : "TOP"	
	CString	m_IqCameraLocation;
	//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고
	//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
	CString	m_IqCameraNumber;
	//1개 Camera에서 2장 Image 생성시 구분 인자.
	//디폴트 값 : "1"
	CString	m_IqScreenNumber;
	//카메라 취득영상의 X Size [pxl]	
	CString	m_IqScreenImageSizeX;
	//카메라 취득영상의  Y Size [pxl]	
	CString	m_IqScreenImageSizeY;
	//해당 Cell의 Image Focus Value	
	CString	m_IqFocusValue;
	//해당 Cell의 기준 Image Bright Value	
	CString	m_IqBrightValue;
	//해당 Cell의 Image 해상도 x축 Value [um]	
	CString	m_IqResolXValue;
	//해당 Cell의 Image 해상도 y축 Value [um]	
	CString	m_IqResolYValue;
	//해당 Cell의 Image 직진도 Value	
	CString	m_IqCamAngleValue;
	//카메라 Gain value(카메라 조정 값)	
	CString	m_IqCameraGain;
	//Exposure Time value [ms](카메라 노출 시간)	
	CString	m_IqExposureTime;
	//Image 판정결과	
	CString	m_ImageJudge;
	//Original Image 파일명 	
	CString	m_ImageFileName;

public:
	static CString MakeInDataIqInfoText;

	//Camera 위치(Top, Bottom) 
	//디폴트 값 : "TOP"		
	void setIqCameraLocation(CString	IqCameraLocation) { m_IqCameraLocation = IqCameraLocation; }
	//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고
	//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
	void setIqCameraNumber(CString	IqCameraNumber) { m_IqCameraNumber = IqCameraNumber; }
	//1개 Camera에서 2장 Image 생성시 구분 인자.
	//디폴트 값 : "1"
	void setIqScreenNumber(CString	IqScreenNumber) { m_IqScreenNumber = IqScreenNumber; }
	//카메라 취득영상의 X Size [pxl]	
	void setIqScreenImageSizeX(CString	IqScreenImageSizeX) { m_IqScreenImageSizeX = IqScreenImageSizeX; }
	//카메라 취득영상의  Y Size [pxl]	
	void setIqScreenImageSizeY(CString	IqScreenImageSizeY) { m_IqScreenImageSizeY = IqScreenImageSizeY; }
	//해당 Cell의 Image Focus Value	
	void setIqFocusValue(CString	IqFocusValue) { m_IqFocusValue = IqFocusValue; }
	//해당 Cell의 기준 Image Bright Value	
	void setIqBrightValue(CString	IqBrightValue) { m_IqBrightValue = IqBrightValue; }
	//해당 Cell의 Image 해상도 x축 Value [um]	
	void setIqResolXValue(CString	IqResolXValue) { m_IqResolXValue = IqResolXValue; }
	//해당 Cell의 Image 해상도 y축 Value [um]	
	void setIqResolYValue(CString	IqResolYValue) { m_IqResolYValue = IqResolYValue; }
	//해당 Cell의 Image 직진도 Value	
	void setIqCamAngleValue(CString	IqCamAngleValue) { m_IqCamAngleValue = IqCamAngleValue; }
	//카메라 Gain value(카메라 조정 값)	
	void setIqCameraGain(CString	IqCameraGain) { m_IqCameraGain = IqCameraGain; }
	//Exposure Time value [ms](카메라 노출 시간)	
	void setIqExposureTime(CString	IqExposureTime) { m_IqExposureTime = IqExposureTime; }
	//Image 판정결과	
	void setImageJudge(CString	 ImageJudge) { m_ImageJudge = ImageJudge; }
	//Original Image 파일명 	
	void setImageFileName(CString	ImageFileName) { m_ImageFileName = ImageFileName; }

};


#endif // SPCINDATAIQINFO_H
