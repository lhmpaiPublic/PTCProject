// SpcInfo.h : CSpcInfo 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCINFO_H
#define SPCINFO_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcInfo
{
	static CSpcInfo* instance;
public:
	CSpcInfo() ;
	virtual ~CSpcInfo() ;

	static CSpcInfo* Inst() { return instance; }

private:
	//SPC+ 작동 플래그
	BOOL m_bSPCStartFlag;

	//설비ID	
	CString	m_EqpId;
	//검사기ID
	CString	m_EqpInspId;
	//검사 Lot ID	
	CString	m_LotId;
	//검사 호기 Line	
	CString	m_LineNumber;
	//검사 호기 정보	
	CString	m_MachineNumber;
	//검사 Lane 정보 (""5. CAM NUM 규칙"" Sheet 참고)
	CString	m_LaneNumber;
	//비전 타입
	CString	m_VisionType;
	//해당 검사기의 공정 구분	
	CString	m_ProcessGroup;
	//조립공정(노칭, 라미, 스택, 폴딩, 패키지)	
	CString	m_ProcessName;
	//물류 진행방향	
	CString	m_ProcessDirection;
	//검사 Recipe Id
	CString m_RecipeId;
	//검사 위치 TOP
	CString m_InspPositionTop;
	//검사위치 BOTTOM
	CString m_InspPositionBottom;
	//Camera 위치(Top, Bottom) 	
	CString	m_CameraLocation;
	//카메라 Gain value(카메라 조정 값)	
	CString	m_CameraGain;
	//Exposure Time value [ms](카메라 노출 시간)	
	CString	m_CameraExposureTime;
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	CString	m_CameraType;
	//Image 직진도 Value(카메라 엥글)
	CString	m_CamAngleValue;
	//Image Focus Value	
	CString	m_IqFocusValue;
	//Image Bright Value	
	CString	m_IqBrightValue;
	//Image 해상도 x축 Value[um]	
	CString	m_IqResolXValue;
	//Image 해상도 y축 Value	
	CString	m_IqResolYValue;
	//Image JPG 퀄리티 정보	
	CString	m_IqJpgQuality;
	//외관 불량명	
	CString	m_DefectTypeRuleBaseName;
	//불량의 실제 명칭	
	CString	m_DefectTypeRuleBaseNameReal;
	//Agent Sw Version	
	CString	m_AgentSwVersion;
	//외관불량 JSON 파일 경로	
	CString	m_InspPath;
	//장비 Spec, Para 변경 JSON 파일 경로	
	CString	m_SpecParaPath;
	//Alarm JSON 파일 경로	
	CString	m_AlarmPath;
	//Status JSON 파일 경로	
	CString	m_StatusPath;

	//INSP Ok Image 파일 경로	
	CString	m_InspOkImagePath;
	//INSP Ng Image 파일 경로	
	CString	m_InspNgImagePath;

	//Spc Plus 정보 파일을 읽어서 저장한다.
	void LoadSpcPlusInfo();
public:
	//SPC+ 작동 플래그
	BOOL getSPCStartFlag() { return m_bSPCStartFlag; }

	//설비ID	
	CString getEqpId() { return m_EqpId; }
	//검사기ID
	CString getEqpInspId() { return m_EqpInspId; }
	//검사 Lot ID	
	CString getLotId() { return m_LotId; }
	//검사 호기 Line	
	CString getLineNumber() { return m_LineNumber; }
	//검사 호기 정보	
	CString getMachineNumber() { return m_MachineNumber; }
	//검사 Lane 정보 (""5. CAM NUM 규칙"" Sheet 참고)
	CString getLaneNumber() { return m_LaneNumber; }
	//비전 타입
	CString getVisionType() { return m_VisionType; }
	//해당 검사기의 공정 구분	
	CString getProcessGroup() { return m_ProcessGroup; }
	//조립공정(노칭, 라미, 스택, 폴딩, 패키지)	
	CString getProcessName() { return m_ProcessName; }
	//물류 진행방향	
	CString getProcessDirection() { return m_ProcessDirection; }
	//검사 Recipe Id
	CString getRecipeId() { return m_RecipeId; }
	//검사 위치 TOP
	CString getInspPositionTop() { return m_InspPositionTop; }
	//검사위치 BOTTOM
	CString getInspPositionBottom() { return m_InspPositionBottom; }
	//Camera 위치(Top, Bottom) 	
	CString getCameraLocation() { return m_CameraLocation; }
	//카메라 Gain value(카메라 조정 값)	
	CString getCameraGain() { return m_CameraGain; }
	//Exposure Time value [ms](카메라 노출 시간)	
	CString getCameraExposureTime() { return m_CameraExposureTime; }
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	CString getCameraType() { return m_CameraType; }
	//Image 직진도 Value(카메라 엥글)
	CString getCamAngleValue() { return m_CamAngleValue; }
	//Image Focus Value	
	CString getIqFocusValue() { return m_IqFocusValue; }
	//Image Bright Value	
	CString getIqBrightValue () { return m_IqBrightValue; }
	//Image 해상도 x축 Value[um]	
	CString getIqResolXValue() { return m_IqResolXValue; }
	//Image 해상도 y축 Value	
	CString getIqResolYValue() { return m_IqResolYValue; }
	//Image JPG 퀄리티 정보	
	CString getIqJpgQuality() { return m_IqJpgQuality; }
	//외관 불량명	
	CString getDefectTypeRuleBaseName() { return m_DefectTypeRuleBaseName; }
	//불량의 실제 명칭	
	CString getDefectTypeRuleBaseNameReal() { return m_DefectTypeRuleBaseNameReal; }
	//Agent Sw Version	
	CString getAgentSwVersion() { return m_AgentSwVersion; }

	//외관불량 JSON 파일 경로	
	CString getInspPath() { return m_InspPath; }
	//장비 Spec, Para 변경 JSON 파일 경로	
	CString getSpecParaPath() { return m_SpecParaPath; }
	//Alarm JSON 파일 경로	
	CString getAlarmPath() { return m_AlarmPath; }
	//Status JSON 파일 경로	
	CString getStatusPath() { return m_StatusPath; }

	//INSP Ok Image 파일 경로	
	CString	getInspOkImagePath() { return m_InspOkImagePath; }
	//INSP Ng Image 파일 경로	
	CString	getInspNgImagePath() { return m_InspNgImagePath; }

	//SPC+ 작동 플래그
	void setSPCStartFlag(BOOL bSPCStartFlag);

	//설비ID	
	void setEqpId(CString EqpId) { m_EqpId = EqpId; }
	//검사기ID
	void setEqpInspId(CString EqpInspId) { m_EqpInspId = EqpInspId; }
	//검사 Lot ID	
	void setLotId(CString LotId) { m_LotId = LotId; }
	//검사 호기 Line	
	void setLineNumber(CString LineNumber) { m_LineNumber = LineNumber; }
	//검사 호기 정보	
	void setMachineNumber(CString MachineNumber) { m_MachineNumber = MachineNumber; }
	//검사 Lane 정보 (""5. CAM NUM 규칙"" Sheet 참고)
	void setLaneNumber(CString LaneNumber) { m_LaneNumber = LaneNumber; }
	//비전 타입
	void setVisionType(CString VisionType) { m_VisionType = VisionType; }
	//해당 검사기의 공정 구분	
	void setProcessGroup(CString ProcessGroup) { m_ProcessGroup = ProcessGroup; }
	//조립공정(노칭, 라미, 스택, 폴딩, 패키지)	
	void setProcessName(CString ProcessName) { m_ProcessName = ProcessName; }
	//물류 진행방향	
	void setProcessDirection(CString ProcessDirection) { m_ProcessDirection = ProcessDirection; }
	//검사 Recipe Id
	void setRecipeId(CString RecipeId) { m_RecipeId = RecipeId; }
	//검사 위치 TOP
	void setInspPositionTop(CString InspPositionTop) {m_InspPositionTop = InspPositionTop; }
	//검사위치 BOTTOM
	void setInspPositionBottom(CString InspPositionBottom) {m_InspPositionBottom = InspPositionBottom; }
	//Camera 위치(Top, Bottom) 	
	void setCameraLocation(CString CameraLocation) { m_CameraLocation = CameraLocation; }
	//카메라 Gain value(카메라 조정 값)	
	void setCameraGain(CString CameraGain) { m_CameraGain = CameraGain; }
	//Exposure Time value [ms](카메라 노출 시간)	
	void setCameraExposureTime(CString CameraExposureTime) { m_CameraExposureTime = CameraExposureTime; }
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	void setCameraType(CString CameraType) { m_CameraType = CameraType; }
	//Image 직진도 Value(카메라 엥글)
	void setCamAngleValue(CString CamAngleValue) { m_CamAngleValue = CamAngleValue; }
	//Image Focus Value	
	void setIqFocusValue(CString IqFocusValue) { m_IqFocusValue = IqFocusValue; }
	//Image Bright Value	
	void setIqBrightValue(CString IqBrightValue) { m_IqBrightValue = IqBrightValue; }
	//Image 해상도 x축 Value[um]	
	void setIqResolXValue(CString IqResolXValue) { m_IqResolXValue = IqResolXValue; }
	//Image 해상도 y축 Value	
	void setIqResolYValue(CString IqResolYValue) { m_IqResolYValue = IqResolYValue; }
	//Image JPG 퀄리티 정보	
	void setIqJpgQuality(CString IqJpgQuality) { m_IqJpgQuality = IqJpgQuality; }
	//외관 불량명	
	void setDefectTypeRuleBaseName(CString DefectTypeRuleBaseName) { m_DefectTypeRuleBaseName = DefectTypeRuleBaseName; }
	//불량의 실제 명칭	
	void setDefectTypeRuleBaseNameReal(CString DefectTypeRuleBaseNameReal) { m_DefectTypeRuleBaseNameReal = DefectTypeRuleBaseNameReal; }
	//Agent Sw Version	
	void setAgentSwVersion(CString AgentSwVersion) { m_AgentSwVersion = AgentSwVersion; }
	//외관불량 JSON 파일 경로	
	void setInspPath(CString InspPath) { m_InspPath = InspPath; }
	//장비 Spec, Para 변경 JSON 파일 경로	
	void setSpecParaPath(CString SpecParaPath) { m_SpecParaPath = SpecParaPath; }
	//Alarm JSON 파일 경로	
	void setAlarmPath(CString AlarmPath) { m_AlarmPath = AlarmPath; }
	//Status JSON 파일 경로	
	void setStatusPath(CString StatusPath) {m_StatusPath = StatusPath; }
	//INSP Ok Image 파일 경로	
	void setInspOkImagePath(CString InspOkImagePath) { m_InspOkImagePath = InspOkImagePath; }
	//INSP Ng Image 파일 경로	
	void setInspNgImagePath(CString InspNgImagePath) { m_InspNgImagePath = InspNgImagePath; }

};

#define  SPCINFO CSpcInfo::Inst()

#endif // SPCINFO_H
