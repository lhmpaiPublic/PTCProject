// SpcInDataDefectInfo.h : CSpcInDataDefectInfo 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCINDATADEFECTINFO_H
#define SPCINDATADEFECTINFO_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcInspManager;
class CSpcInDataDefectInfo : public CSpcInData
{
public:
	CSpcInDataDefectInfo(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcInDataDefectInfo() ;

private:
	//관리 클래스 객체 포인터
	CSpcInspManager* manager;

	//결함 중 해당 결함의 순서	
	CString	m_DefectIndex;
	//Rule Base Defect Type(불량명 기입) → 통일화된 외관 불량명 기입	
	CString	m_DefectTypeRuleBaseName;
	//해당 불량의 실제 명칭 기입	
	CString	m_DefectTypeRuleBaseNameReal;
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	CString	m_DefectCameraType;
	//카메라 위치_Top, Bottom, Side
	//디폴트 값 : "TOP"	
	CString	m_DefectCameraLocation;
	//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고)
	//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
	CString	m_DefectCameraNumber;
	//1개 Camera에서 2장 Image 생성시 구분 인자
	//디폴트 값 : "1"
	CString	m_DefectScreenNumber;
	//외관불량 측정위치	
	CString	m_DefectInspPosition;
	//외관검사 기준 좌표의 X [pxl]_절대 위치 Image좌,상단(0,0)	
	CString	m_DefectCriteriaPointX;
	//외관검사 기준 좌표의 Y [pxl]_절대 위치 Image좌,상단(0,0)	
	CString	m_DefectCriteriaPointY;
	//불량 발생위치와 기준좌표간 거리 R [pxl]_검사별 지정된 기준좌표로 부터	
	CString	m_DefectRelativePointRPx;
	//불량 발생 위치와 기준좌표간 각도 [°]_검사별 지정된 기준좌표 및 기준선으로 부터	
	CString	m_DefectRelativePointAngle;
	//Image 기준 불량 발생 위치 X [pxl]_절대 위치 Image좌,상단(0,0)	
	CString	m_DefectAbsoluteImgX;
	//Image 기준 불량 발생 위치 Y [pxl]_절대 위치 Image좌,상단(0,0)	
	CString	m_DefectAbsoluteImgY;
	//Image 기준 Defect의 Width [pxl]	
	CString	m_DefectImgWidth;
	//Image 기준 Defect의 Length [pxl]	
	CString	m_DefectImgLength;
	//Image 기준 불량 발생 면적 [pxl]	
	CString	m_DefectAreaPixels;
	//불량 검출 영역 평균 Gray Level 값	
	CString	m_DefectGrayLvAvg;
	//불량 검출 영역 최고 Gray Level 값	
	CString	m_DefectGrayLvMax;
	//불량 검출 영역 최저 Gray Level 값	
	CString	m_DefectGrayLvMin;
	//불량 검출 영역 내 Gray Level 범위(DEFECT_GRAY_LV_MAX, DEFECT_GRAY_LV_MIN의 차이값)	
	CString	m_DefectGrayLvRange;
	//불량 검출 영역 표준편차	
	CString	m_DefectGrayLvStddev;
	//불량 장축의 기울기	
	CString	m_DefectRatioWidthLength;
	//불량 각도	
	CString	m_DefectAngle;
	//불량의 휨 정도	
	CString	m_DefectElongation;
	//Defect 윤곽 둘레 길이	
	CString	m_DefectBoundaryPixelLength;
	//ROI 내 최대 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString	m_DefectRoiGrayLvMax;
	//ROI 내 평균 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString	m_DefectRoiGrayLvAvg;
	//ROI 내 최소 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString	m_DefectRoiGrayLvMin;
	//ROI 내 중앙값 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString	m_DefectRoiGrayLvMedian;
	//ROI 내 표준편차 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString	m_DefectRoiGrayLvStddev;
	//외관 판정 유무(OUT or IN) IN=Bypass, OUT=배출/Marking	
	CString	m_DefectInOut;
	//외관 불량 Crop 이미지 파일명
	CString	m_DefectCropImageFileName;

public:
	static char* MakeInDataDefectInfoText;

	//결함 중 해당 결함의 순서	
	void setDefectIndex(CString	DefectIndex) { m_DefectIndex = DefectIndex; }
	//Rule Base Defect Type(불량명 기입) → 통일화된 외관 불량명 기입	
	void setDefectTypeRuleBaseName(CString	DefectTypeRuleBaseName) { m_DefectTypeRuleBaseName = DefectTypeRuleBaseName; }
	//해당 불량의 실제 명칭 기입	
	void setDefectTypeRuleBaseNameReal(CString	DefectTypeRuleBaseNameReal) { m_DefectTypeRuleBaseNameReal = DefectTypeRuleBaseNameReal; }
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	void setDefectCameraType(CString	DefectCameraType) { m_DefectCameraType = DefectCameraType; }
	//카메라 위치_Top, Bottom, Side
	//디폴트 값 : "TOP"	
	void setDefectCameraLocation(CString	DefectCameraLocation) { m_DefectCameraLocation = DefectCameraLocation; }
	//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고)
	//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
	void setDefectCameraNumber(CString	 DefectCameraNumber) { m_DefectCameraNumber = DefectCameraNumber; }
	//1개 Camera에서 2장 Image 생성시 구분 인자
	//디폴트 값 : "1"
	void setDefectScreenNumber(CString	DefectScreenNumber) { m_DefectScreenNumber = DefectScreenNumber; }
	//외관불량 측정위치	
	void setDefectInspPosition(CString	DefectInspPosition) { m_DefectInspPosition = DefectInspPosition; }
	//외관검사 기준 좌표의 X [pxl]_절대 위치 Image좌,상단(0,0)	
	void setDefectCriteriaPointX(CString	DefectCriteriaPointX) { m_DefectCriteriaPointX = DefectCriteriaPointX; }
	//외관검사 기준 좌표의 Y [pxl]_절대 위치 Image좌,상단(0,0)	
	void setDefectCriteriaPointY(CString	DefectCriteriaPointY) { m_DefectCriteriaPointY = DefectCriteriaPointY; }
	//불량 발생위치와 기준좌표간 거리 R [pxl]_검사별 지정된 기준좌표로 부터	
	void setDefectRelativePointRPx(CString	DefectRelativePointRPx) { m_DefectRelativePointRPx = DefectRelativePointRPx; }
	//불량 발생 위치와 기준좌표간 각도 [°]_검사별 지정된 기준좌표 및 기준선으로 부터	
	void setDefectRelativePointAngle(CString	DefectRelativePointAngle) { m_DefectRelativePointAngle = DefectRelativePointAngle; }
	//Image 기준 불량 발생 위치 X [pxl]_절대 위치 Image좌,상단(0,0)	
	void setDefectAbsoluteImgX(CString	DefectAbsoluteImgX) { m_DefectAbsoluteImgX = DefectAbsoluteImgX; }
	//Image 기준 불량 발생 위치 Y [pxl]_절대 위치 Image좌,상단(0,0)	
	void setDefectAbsoluteImgY(CString	DefectAbsoluteImgY) { m_DefectAbsoluteImgY = DefectAbsoluteImgY; }
	//Image 기준 Defect의 Width [pxl]	
	void setDefectImgWidth(CString	DefectImgWidth) { m_DefectImgWidth = DefectImgWidth; }
	//Image 기준 Defect의 Length [pxl]	
	void setDefectImgLength(CString	DefectImgLength) { m_DefectImgLength = DefectImgLength; }
	//Image 기준 불량 발생 면적 [pxl]	
	void setDefectAreaPixels(CString	DefectAreaPixels) { m_DefectAreaPixels = DefectAreaPixels; }
	//불량 검출 영역 평균 Gray Level 값	
	void setDefectGrayLvAvg(CString	DefectGrayLvAvg) { m_DefectGrayLvAvg = DefectGrayLvAvg; }
	//불량 검출 영역 최고 Gray Level 값	
	void setDefectGrayLvMax(CString	DefectGrayLvMax) { m_DefectGrayLvMax = DefectGrayLvMax; }
	//불량 검출 영역 최저 Gray Level 값	
	void setDefectGrayLvMin(CString	DefectGrayLvMin) { m_DefectGrayLvMin = DefectGrayLvMin; }
	//불량 검출 영역 내 Gray Level 범위(DEFECT_GRAY_LV_MAX, DEFECT_GRAY_LV_MIN의 차이값)	
	void setDefectGrayLvRange(CString	DefectGrayLvRange) { m_DefectGrayLvRange = DefectGrayLvRange; }
	//불량 검출 영역 표준편차	
	void setDefectGrayLvStddev(CString	DefectGrayLvStddev) { m_DefectGrayLvStddev = DefectGrayLvStddev; }
	//불량 장축의 기울기	
	void setDefectRatioWidthLength(CString	DefectRatioWidthLength) { m_DefectRatioWidthLength = DefectRatioWidthLength; }
	//불량 각도	
	void setDefectAngle(CString	 DefectAngle) { m_DefectAngle = DefectAngle; }
	//불량의 휨 정도	
	void setDefectElongation(CString	DefectElongation) { m_DefectElongation = DefectElongation; }
	//Defect 윤곽 둘레 길이	
	void setDefectBoundaryPixelLength(CString	DefectBoundaryPixelLength) { m_DefectBoundaryPixelLength = DefectBoundaryPixelLength; }
	//ROI 내 최대 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	void setDefectRoiGrayLvMax(CString	DefectRoiGrayLvMax) { m_DefectRoiGrayLvMax = DefectRoiGrayLvMax; }
	//ROI 내 평균 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	void setDefectRoiGrayLvAvg(CString	DefectRoiGrayLvAvg) { m_DefectRoiGrayLvAvg = DefectRoiGrayLvAvg; }
	//ROI 내 최소 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	void setDefectRoiGrayLvMin(CString	DefectRoiGrayLvMin) { m_DefectRoiGrayLvMin = DefectRoiGrayLvMin; }
	//ROI 내 중앙값 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	void setDefectRoiGrayLvMedian(CString	DefectRoiGrayLvMedian) { m_DefectRoiGrayLvMedian = DefectRoiGrayLvMedian; }
	//ROI 내 표준편차 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	void setDefectRoiGrayLvStddev(CString	DefectRoiGrayLvStddev) { m_DefectRoiGrayLvStddev = DefectRoiGrayLvStddev; }
	//외관 판정 유무(OUT or IN) IN=Bypass, OUT=배출/Marking	
	void setDefectInOut(CString	 DefectInOut) { m_DefectInOut = DefectInOut; }
	//외관 불량 Crop 이미지 파일명
	void setDefectCropImageFileName(CString	DefectCropImageFileName) { m_DefectCropImageFileName = DefectCropImageFileName; }

	
	//결함 중 해당 결함의 순서	
	CString getDefectIndex() { return m_DefectIndex;}
	//Rule Base Defect Type(불량명 기입) → 통일화된 외관 불량명 기입	
	CString getDefectTypeRuleBaseName() { return m_DefectTypeRuleBaseName;}
	//해당 불량의 실제 명칭 기입	
	CString getDefectTypeRuleBaseNameReal() { return m_DefectTypeRuleBaseNameReal;}
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	CString getDefectCameraType() { return m_DefectCameraType;}
	//카메라 위치_Top, Bottom, Side
	//디폴트 값 : "TOP"	
	CString getDefectCameraLocation() { return m_DefectCameraLocation;}
	//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고)
	//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
	CString getDefectCameraNumber() { return m_DefectCameraNumber;}
	//1개 Camera에서 2장 Image 생성시 구분 인자
	//디폴트 값 : "1"
	CString getDefectScreenNumber() { return m_DefectScreenNumber;}
	//외관불량 측정위치	
	CString getDefectInspPosition() { return m_DefectInspPosition;}
	//외관검사 기준 좌표의 X [pxl]_절대 위치 Image좌,상단(0,0)	
	CString getDefectCriteriaPointX() { return m_DefectCriteriaPointX;}
	//외관검사 기준 좌표의 Y [pxl]_절대 위치 Image좌,상단(0,0)	
	CString getDefectCriteriaPointY() { return m_DefectCriteriaPointY;}
	//불량 발생위치와 기준좌표간 거리 R [pxl]_검사별 지정된 기준좌표로 부터	
	CString getDefectRelativePointRPx() { return m_DefectRelativePointRPx;}
	//불량 발생 위치와 기준좌표간 각도 [°]_검사별 지정된 기준좌표 및 기준선으로 부터	
	CString getDefectRelativePointAngle() { return m_DefectRelativePointAngle;}
	//Image 기준 불량 발생 위치 X [pxl]_절대 위치 Image좌,상단(0,0)	
	CString getDefectAbsoluteImgX() { return m_DefectAbsoluteImgX;}
	//Image 기준 불량 발생 위치 Y [pxl]_절대 위치 Image좌,상단(0,0)	
	CString getDefectAbsoluteImgY() { return m_DefectAbsoluteImgY;}
	//Image 기준 Defect의 Width [pxl]	
	CString getDefectImgWidth() { return m_DefectImgWidth;}
	//Image 기준 Defect의 Length [pxl]	
	CString getDefectImgLength() { return m_DefectImgLength;}
	//Image 기준 불량 발생 면적 [pxl]	
	CString getDefectAreaPixels() { return m_DefectAreaPixels;}
	//불량 검출 영역 평균 Gray Level 값	
	CString getDefectGrayLvAvg() { return m_DefectGrayLvAvg;}
	//불량 검출 영역 최고 Gray Level 값	
	CString getDefectGrayLvMax() { return m_DefectGrayLvMax;}
	//불량 검출 영역 최저 Gray Level 값	
	CString getDefectGrayLvMin() { return m_DefectGrayLvMin; }
	//불량 검출 영역 내 Gray Level 범위(DEFECT_GRAY_LV_MAX, DEFECT_GRAY_LV_MIN의 차이값)	
	CString getDefectGrayLvRange() { return m_DefectGrayLvRange;}
	//불량 검출 영역 표준편차	
	CString getDDefectGrayLvStddev() { return m_DefectGrayLvStddev;}
	//불량 장축의 기울기	
	CString getDefectRatioWidthLength() { return m_DefectRatioWidthLength;}
	//불량 각도	
	CString getDefectAngle() { return m_DefectAngle;}
	//불량의 휨 정도	
	CString getDefectElongation() { return m_DefectElongation;}
	//Defect 윤곽 둘레 길이	
	CString getDefectBoundaryPixelLength() { return m_DefectBoundaryPixelLength;}
	//ROI 내 최대 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString getDefectRoiGrayLvMax() { return m_DefectRoiGrayLvMax;}
	//ROI 내 평균 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString getDefectRoiGrayLvAvg() { return m_DefectRoiGrayLvAvg;}
	//ROI 내 최소 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString getDefectRoiGrayLvMin() { return m_DefectRoiGrayLvMin;}
	//ROI 내 중앙값 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString getDefectRoiGrayLvMedian() { return m_DefectRoiGrayLvMedian;}
	//ROI 내 표준편차 Gray Level 값(코팅부, 미코팅부(Tab), 절연부)	
	CString getDefectRoiGrayLvStddev() { return m_DefectRoiGrayLvStddev;}
	//외관 판정 유무(OUT or IN) IN=Bypass, OUT=배출/Marking	
	CString getDefectInOut() { return m_DefectInOut;}
	//외관 불량 Crop 이미지 파일명
	CString getDefectCropImageFileName() { return m_DefectCropImageFileName;}

	//Camera Number(상부 또는 하부 카메라 n개인 경우) (""5. CAM NUM 규칙"" Sheet 참고)
	//Tab이 보이는 카메라는 1번, Tab이 없으면 2번
	int getDefectCameraNumber_int() { return CGlobalFunc::StringToint(m_DefectCameraNumber); }

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_DefectInfo();

	//INSP 이미지 파일명을 넘긴다.
	CString ImagCropFileName();

};


#endif // SPCINDATADEFECTINFO_H
