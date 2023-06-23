// SpcInspInData.h : CSpcInspInData 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCINSPINDATA_H
#define SPCINSPINDATA_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcInDataIqInfo;
class CSpcInDataDefectInfo;
class CSpcInspInData : public CSpcInData
{
public:
	CSpcInspInData(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcInspInData() ;

private:
	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;

	//Message 종류	
	CString	m_Category;
	//Trigger 받은 시간
	//YYYMMDDhhmmssmmm"	
	CString	m_VisionInputTime;
	//판정결과 PLC로 전달하는 시간
	//YYYMMDDhhmmssmmm"	
	CString	m_VisionOutputTime;
	//Trigger 받은 시간부터 판정결과 PLC로 전달하는 시간까지의 시간(2번항목 - 1번항목)
	//소수점3자리	
	CString	m_InspTactTime;
	//검사 시간(Grab + 처리) - (상, 하, Area) 
	//소수점3자리"	
	CString	m_InspProcessingTime;
	//검사 Recipe Id	
	CString	m_RecipeId;
	//NG배출 Cell 대상 여부(PLC에서 받아서 기입)
	//디폴트 : "N"	
	CString	m_NgOut;
	//MMD기준 설비ID 기입 	
	CString	m_EqpId;
	//검사기ID
	//디폴트 : "NTC"
	CString	m_EqpInspId;
	//해당 검사기의 공정 구분	
	CString	m_ProcessGroup;
	//조립공정(노칭, 라미, 스택, 폴딩, 패키지)	
	CString	m_ProcessName;
	//검사 호기 Line	
	CString	m_LineNumber;
	//검사 호기 정보	
	CString	m_MachineNumber;
	//검사 Lane 정보 (""5. CAM NUM 규칙"" Sheet 참고)
	//디폴트 값 : "1"
	CString	m_LaneNumber;
	//버전
	//NTC : INTEGRATION
	//LAMI : LAMI-FINAL/ HABCHI/ HALF-CELL/ THICKNESS
	//FOL : GAP/ TAB/ SEPA/ SEALING-TOP, SEALING-BACK
	//STK : MONO-ALIGN/ MONO-SEPA/ MONO-STACK/ HALF-ALIGN/ HALF-SEPA/HALF-STACK/TOP-BACK-APPEARANCE/ 4SIDE-APPEARANCE	
	CString	m_VisionType;
	//물류 진행방향	
	CString	m_ProcessDirection;
	//검사 Lot ID	
	CString	m_LotId;
	//Cell 추적 PLC에서 받은 ID (VIRTUAL_CELL_ID와 동일값 작성)	
	CString	m_CellId;
	//검사기 생성 셀 카운트 번호(자정을 기준으로 초기화한다)	
	CString	m_CellCountNo;
	//Cell 추적 PLC에서 받은 ID (CELL_ID와 동일값 작성)	
	CString	m_VirtualCellId;
	//Cell 판정결과	
	CString	m_CellFinalJudge;
	//외관 판정 결과 OK/NG/DLNG여부 	
	CString	m_AppearanceJudgeResult;
	//해당 Cell의 외관 NG 개수	
	CString	m_TotalAppearanceNgCount;
	//외관 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분) (개수제한X, 중복항목은 1회만 기입) 	
	CString	m_AppearanceReasonAll;
	//현업에서 통용된 되는 외관 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분) (개수제한X, 중복항목은 1회만 기입)	
	CString	m_AppearanceReasonAllReal;
	//외관 대표 불량명(우선순위 대표 불량명 없을시 첫번째 검출 항목)	
	CString	m_AppearanceReasonMain;
	//해당 셀의 대표불량명을 실제 통용되는 명칭으로 기입	
	CString	m_AppearanceReasonMainReal;

	//============파일을 생성하기 위한 정보 데이터 키 없는 데이터 정보 ======
	//객체 생성시간
	CString m_CreateTime;
	//검사 위치 TOP
	CString m_InspPositionTop;
	//검사위치 BOTTOM
	CString m_InspPositionBottom;

public:
	static char* MakeInspInDataText_1;
	static char* MakeInspInDataText_2;
	static char* MakeInspInDataText_3;	

	//Message 종류	
	void setCategory(CString	Category) { m_Category = Category; }
	//Trigger 받은 시간
	//YYYMMDDhhmmssmmm"	
	void setVisionInputTime(CString	VisionInputTime) { m_VisionInputTime = VisionInputTime; }
	//판정결과 PLC로 전달하는 시간
	//YYYMMDDhhmmssmmm"	
	void setVisionOutputTime(CString	VisionOutputTime) { m_VisionOutputTime = VisionOutputTime; }
	//Trigger 받은 시간부터 판정결과 PLC로 전달하는 시간까지의 시간(2번항목 - 1번항목)
	//소수점3자리	
	void setInspTactTime(CString	InspTactTime) { m_InspTactTime = InspTactTime; }
	//검사 시간(Grab + 처리) - (상, 하, Area) 
	//소수점3자리"	
	void setInspProcessingTime(CString	InspProcessingTime) { m_InspProcessingTime = InspProcessingTime; }
	//검사 Recipe Id	
	void setRecipeId(CString	RecipeId) { m_RecipeId = RecipeId; }
	//NG배출 Cell 대상 여부(PLC에서 받아서 기입)
	//디폴트 : "N"	
	void setNgOut(CString	NgOut) { m_NgOut = NgOut; }
	//MMD기준 설비ID 기입 	
	void setEqpId(CString	EqpId) { m_EqpId = EqpId; }
	//검사기ID
	//디폴트 : "NTC"
	void setEqpInspId(CString	EqpInspId) { m_EqpInspId = EqpInspId; }
	//해당 검사기의 공정 구분	
	void setProcessGroup(CString	ProcessGroup) { m_ProcessGroup = ProcessGroup; }
	//조립공정(노칭, 라미, 스택, 폴딩, 패키지)	
	void setProcessName(CString	ProcessName) { m_ProcessName = ProcessName; }
	//검사 호기 Line	
	void setLineNumber(CString	 LineNumber) { m_LineNumber = LineNumber; }
	//검사 호기 정보	
	void setMachineNumber(CString	MachineNumber) { m_MachineNumber = MachineNumber; }
	//검사 Lane 정보 (""5. CAM NUM 규칙"" Sheet 참고)
	//디폴트 값 : "1"
	void setLaneNumber(CString	LaneNumber) { m_LaneNumber = LaneNumber; }
	//버전
	//NTC : INTEGRATION
	//LAMI : LAMI-FINAL/ HABCHI/ HALF-CELL/ THICKNESS
	//FOL : GAP/ TAB/ SEPA/ SEALING-TOP, SEALING-BACK
	//STK : MONO-ALIGN/ MONO-SEPA/ MONO-STACK/ HALF-ALIGN/ HALF-SEPA/HALF-STACK/TOP-BACK-APPEARANCE/ 4SIDE-APPEARANCE	
	void setVisionType(CString	VisionType) { m_VisionType = VisionType; }
	//물류 진행방향	
	void setProcessDirection(CString	ProcessDirection) { m_ProcessDirection = ProcessDirection; }
	//검사 Lot ID	
	void setLotId(CString	LotId) { m_LotId = LotId; }
	//Cell 추적 PLC에서 받은 ID (VIRTUAL_CELL_ID와 동일값 작성)	
	void setCellId(CString	CellId) { m_CellId = CellId; }
	//검사기 생성 셀 카운트 번호(자정을 기준으로 초기화한다)	
	void setCellCountNo(CString	 CellCountNo) { m_CellCountNo = CellCountNo; }
	//Cell 추적 PLC에서 받은 ID (CELL_ID와 동일값 작성)	
	void setVirtualCellId(CString	 VirtualCellId) { m_VirtualCellId = VirtualCellId; }
	//Cell 판정결과	
	void setCellFinalJudge(CString	CellFinalJudge) { m_CellFinalJudge = CellFinalJudge; }
	//외관 판정 결과 OK/NG/DLNG여부 	
	void setAppearanceJudgeResult(CString	AppearanceJudgeResult) { m_AppearanceJudgeResult = AppearanceJudgeResult; }
	//해당 Cell의 외관 NG 개수	
	void setTotalAppearanceNgCount(CString	TotalAppearanceNgCount) { m_TotalAppearanceNgCount = TotalAppearanceNgCount; }
	//외관 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분) (개수제한X, 중복항목은 1회만 기입) 	
	void appendAppearanceReasonAll(CString	AppearanceReasonAll);
	//현업에서 통용된 되는 외관 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분) (개수제한X, 중복항목은 1회만 기입)	
	void appendAppearanceReasonAllReal(CString  AppearanceReasonAllReal);
	//외관 대표 불량명(우선순위 대표 불량명 없을시 첫번째 검출 항목)	
	void setAppearanceReasonMain(CString	AppearanceReasonMain) { m_AppearanceReasonMain = AppearanceReasonMain; }
	//해당 셀의 대표불량명을 실제 통용되는 명칭으로 기입	
	void setAppearanceReasonMainReal(CString	AppearanceReasonMainReal) { m_AppearanceReasonMainReal = AppearanceReasonMainReal; }

	//Message 종류	
	CString getCategory() { return m_Category; }
	//Trigger 받은 시간
	//YYYMMDDhhmmssmmm"	
	CString getVisionInputTime() { return m_VisionInputTime; }
	//판정결과 PLC로 전달하는 시간
	//YYYMMDDhhmmssmmm"	
	CString getVisionOutputTime() { return m_VisionOutputTime; }
	//Trigger 받은 시간부터 판정결과 PLC로 전달하는 시간까지의 시간(2번항목 - 1번항목)
	//소수점3자리	
	CString getInspTactTime() { return m_InspTactTime; }
	//검사 시간(Grab + 처리) - (상, 하, Area) 
	//소수점3자리"	
	CString getInspProcessingTime() { return m_InspProcessingTime; }
	//검사 Recipe Id	
	CString getRecipeId() { return m_RecipeId; }
	//NG배출 Cell 대상 여부(PLC에서 받아서 기입)
	//디폴트 : "N"	
	CString getNgOut() { return m_NgOut; }
	//MMD기준 설비ID 기입 	
	CString getEqpId() { return m_EqpId; }
	//검사기ID
	//디폴트 : "NTC"
	CString getEqpInspId() { return m_EqpInspId; }
	//해당 검사기의 공정 구분	
	CString getProcessGroup() { return m_ProcessGroup; }
	//조립공정(노칭, 라미, 스택, 폴딩, 패키지)	
	CString getProcessName() { return m_ProcessName; }
	//검사 호기 Line	
	CString getLineNumber() { return m_LineNumber; }
	//검사 호기 정보	
	CString getMachineNumber() { return m_MachineNumber; }
	//검사 Lane 정보 (""5. CAM NUM 규칙"" Sheet 참고)
	//디폴트 값 : "1"
	CString getLaneNumber() { return m_LaneNumber; }
	//버전
	//NTC : INTEGRATION
	//LAMI : LAMI-FINAL/ HABCHI/ HALF-CELL/ THICKNESS
	//FOL : GAP/ TAB/ SEPA/ SEALING-TOP, SEALING-BACK
	//STK : MONO-ALIGN/ MONO-SEPA/ MONO-STACK/ HALF-ALIGN/ HALF-SEPA/HALF-STACK/TOP-BACK-APPEARANCE/ 4SIDE-APPEARANCE	
	CString getVisionType() { return m_VisionType; }
	//물류 진행방향	
	CString getProcessDirection() { return m_ProcessDirection; }
	//검사 Lot ID	
	CString getLotId() { return m_LotId; }
	//Cell 추적 PLC에서 받은 ID (VIRTUAL_CELL_ID와 동일값 작성)	
	CString getCellId() { return m_CellId; }
	//검사기 생성 셀 카운트 번호(자정을 기준으로 초기화한다)	
	CString getCellCountNo() { return m_CellCountNo; }
	//Cell 추적 PLC에서 받은 ID (CELL_ID와 동일값 작성)	
	CString getVirtualCellId() { return m_VirtualCellId; }
	//Cell 판정결과	
	CString getCellFinalJudge() { return m_CellFinalJudge; }
	//외관 판정 결과 OK/NG/DLNG여부 	
	CString getAppearanceJudgeResult() { return m_AppearanceJudgeResult; }
	//해당 Cell의 외관 NG 개수	
	CString getTotalAppearanceNgCount() { return m_TotalAppearanceNgCount; }
	//외관 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분) (개수제한X, 중복항목은 1회만 기입) 	
	CString getAppearanceReasonAll() { return m_AppearanceReasonAll; }
	//현업에서 통용된 되는 외관 불량명 전체 기입(csv파일 저장시 세미콜론 ' ; ' 으로 불량명 구분) (개수제한X, 중복항목은 1회만 기입)	
	CString getAppearanceReasonAllReal() { return m_AppearanceReasonAllReal; }
	//외관 대표 불량명(우선순위 대표 불량명 없을시 첫번째 검출 항목)	
	CString getAppearanceReasonMain() { return m_AppearanceReasonMain; }
	//해당 셀의 대표불량명을 실제 통용되는 명칭으로 기입	
	CString getAppearanceReasonMainReal() { return m_AppearanceReasonMainReal; }

	//============파일을 생성하기 위한 정보 데이터 키 없는 데이터 정보 ======
	//객체 생성시간
	CString getCreateTime() { return m_CreateTime; }
	//검사 위치 TOP
	CString getInspPositionTop() { return m_InspPositionTop; }
	//검사위치 BOTTOM
	CString getInspPositionBottom() { return m_InspPositionBottom; }

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_Insp1();
	CString makeJSONText_Insp2();
	CString getSONText_InspTail();

	//파일저장경로를 넘긴다.
	CString JsonFilePath();

	//파일명을 넘긴다.
	CString JsonFileName();

	//INSP 이미지 저장경로를 넘긴다.
	CString ImageFilePath();

};


#endif // SPCINSPINDATA_H
