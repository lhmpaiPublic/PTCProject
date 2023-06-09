// SpcAlarmInData.h : CSpcAlarmInData 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCALARMINDATA_H
#define SPCALARMINDATA_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcAlarmInData : public CSpcInData
{
public:
	CSpcAlarmInData(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcAlarmInData() ;

private:
	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;

	//Message 종류	
	CString	m_Category;
	//검사기 EM NO. 
	CString m_EqpInspId;
	//해당 검사기의 공정 구분
	CString	m_ProcessGroup;
	//공정 명	
	CString	m_ProcessName;
	//검사 호기 정보 	
	CString	m_MachineNumber;
	//검사 호기 Line	
	CString	m_LineNumber;
	//Lane 정보 (1 lane, 2 lane인지) 
	//디폴트 값 : "1"
	CString	m_LaneNumber;
	//NTC : INTEGRATION
	//LAMI : LAMI-FINAL/ HABCHI/ HALF-CELL/ THICKNESS
	//FOL : GAP/ TAB/ SEPA/ SEALING-TOP, SEALING-BACK
	//STK : MONO-ALIGN/ MONO-SEPA/ MONO-STACK/ HALF-ALIGN/ HALF-SEPA/HALF-STACK/TOP-BACK-APPEARANCE/ 4SIDE-APPEARANCE	
	CString	m_VisionType;
	//MMD기준 설비ID 기입 
	//10-4 참조	
	CString	m_EqpId;
	//알람 발생 or 해제 MSG Flag
	//TRUE(알람 발생)	
	CString	m_AlarmFlag;
	//알람 발생 시간
	//10-5 참조
	//YYYY(연) + MM(월) + DD일 + HH(시) + MM(분) + SS(초) + MMM(밀리초)	
	CString	m_AlarmInfoDate;
	//알람 발생 주최 (PLC, VISION)	
	CString	m_AlarmHost;
	//알람 구분 
	//LIGHT(경알람), HEAVY(중알람)	
	CString	m_AlarmLevel;
	//알람 구분자
	//0 ~ 9999	
	CString	m_AlarmCode;
	//알람 명-알람 정의 필요	
	CString	m_AlarmName;
	//모든 알람 해제 신호(검사기 Off→On)
	//TRUE, FALSE	
	CString	m_AlarmAllResetFlag;
	//알람 해제 시간
	//YYYY(연) + MM(월) + DD일 + HH(시) + MM(분) + SS(초) + MMM(밀리초)	
	CString	m_AlarmOutToDate;

public:
	static char* MakeAlarmInDataText;
	//객체 생성시간
	CString m_CreateTime;

	//Message 종류	
	void setCategory(CString	Category) { m_Category = Category; }
	//검사기 EM NO. 
	void setEqpInspId(CString EqpInspId) { m_EqpInspId = EqpInspId; }
	//해당 검사기의 공정 구분
	void setProcessGroup(CString ProcessGroup) { m_ProcessGroup = ProcessGroup; }
	//공정 명	
	void setProcessName(CString	 ProcessName) { m_ProcessName = ProcessName; }
	//검사 호기 정보 	
	void setMachineNumber(CString MachineNumber) { m_MachineNumber = MachineNumber; }
	//검사 호기 Line	
	void setLineNumber(CString	LineNumber) { m_LineNumber = LineNumber; }
	//Lane 정보 (1 lane, 2 lane인지) 
	//디폴트 값 : "1"
	void setLaneNumber(CString	LaneNumber) { m_LaneNumber = LaneNumber; }
	//NTC : INTEGRATION
	//LAMI : LAMI-FINAL/ HABCHI/ HALF-CELL/ THICKNESS
	//FOL : GAP/ TAB/ SEPA/ SEALING-TOP, SEALING-BACK
	//STK : MONO-ALIGN/ MONO-SEPA/ MONO-STACK/ HALF-ALIGN/ HALF-SEPA/HALF-STACK/TOP-BACK-APPEARANCE/ 4SIDE-APPEARANCE	
	void setVisionType(CString	VisionType) { m_VisionType = VisionType; }
	//MMD기준 설비ID 기입 
	//10-4 참조	
	void setEqpId(CString	EqpId) { m_EqpId = EqpId; }
	//알람 발생 or 해제 MSG Flag
	//TRUE(알람 발생)	
	void setAlarmFlag(CString	AlarmFlag) { m_AlarmFlag = AlarmFlag; }
	//알람 발생 시간
	//10-5 참조
	//YYYY(연) + MM(월) + DD일 + HH(시) + MM(분) + SS(초) + MMM(밀리초)	
	void setAlarmInfoDate(CString	AlarmInfoDate) { m_AlarmInfoDate = AlarmInfoDate; }
	//알람 발생 주최 (PLC, VISION)	
	void setAlarmHost(CString	AlarmHost) { m_AlarmHost = AlarmHost; }
	//알람 구분 
	//LIGHT(경알람), HEAVY(중알람)	
	void setAlarmLevel(CString	AlarmLevel) { m_AlarmLevel = AlarmLevel; }
	//알람 구분자
	//0 ~ 9999	
	void setAlarmCode(CString	AlarmCode) { m_AlarmCode = AlarmCode; }
	//알람 명-알람 정의 필요	
	void setAlarmName(CString	AlarmName) { m_AlarmName = AlarmName; }
	//모든 알람 해제 신호(검사기 Off→On)
	//TRUE, FALSE	
	void setAlarmAllResetFlag(CString	AlarmAllResetFlag) { m_AlarmAllResetFlag = AlarmAllResetFlag; }
	//알람 해제 시간
	//YYYY(연) + MM(월) + DD일 + HH(시) + MM(분) + SS(초) + MMM(밀리초)	
	void setAlarmOutToDate(CString	AlarmOutToDate) { m_AlarmOutToDate = AlarmOutToDate; }

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_Alarm();

	//파일저장경로를 넘긴다.
	CString JsonFilePath();

	//파일명을 넘긴다.
	CString JsonFileName();
	
};


#endif // SPCALARMINDATA_H
