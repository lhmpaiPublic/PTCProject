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
	//Camera 위치(Top, Bottom) 	
	CString	m_CameraLocation;
	//카메라 Gain value(카메라 조정 값)	
	CString	m_CameraGain;
	//Exposure Time value [ms](카메라 노출 시간)	
	CString	m_CameraExposureTime;
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	CString	m_CameraType;
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

	//Spc Plus 정보 파일을 읽어서 저장한다.
	void LoadSpcPlusInfo();
public:
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
	//Camera 위치(Top, Bottom) 	
	CString getCameraLocation() { return m_CameraLocation; }
	//카메라 Gain value(카메라 조정 값)	
	CString getCameraGain() { return m_CameraGain; }
	//Exposure Time value [ms](카메라 노출 시간)	
	CString getCameraExposureTime() { return m_CameraExposureTime; }
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	CString getCameraType() { return m_CameraType; }
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
	//Camera 위치(Top, Bottom) 	
	void setCameraLocation(CString CameraLocation) { m_CameraLocation = CameraLocation; }
	//카메라 Gain value(카메라 조정 값)	
	void setCameraGain(CString CameraGain) { m_CameraGain = CameraGain; }
	//Exposure Time value [ms](카메라 노출 시간)	
	void setCameraExposureTime(CString CameraExposureTime) { m_CameraExposureTime = CameraExposureTime; }
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	void setCameraType(CString CameraType) { m_CameraType = CameraType; }
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

};

#define  SPCINFO CSpcInfo::Inst()

#endif // SPCINFO_H
