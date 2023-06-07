// SpcStatusInData.h : CSpcStatusInData 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCSTATUSINDATA_H
#define SPCSTATUSINDATA_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcStatusInData : public CSpcInData
{
public:
	CSpcStatusInData(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcStatusInData() ;

private:
	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;

	//Message 종류	
	CString	m_Category;
	//발생시간	
	CString	m_TransferTimer;
	//검사기ID	
	CString	m_EqpInspId;
	//대공정명	
	CString	m_ProcessGroup;
	//세부공정명	
	CString	m_ProcessName;
	//MMD기준 설비ID 기입	
	CString	m_EqpId;
	//Agent Sw Version	
	CString	m_AgentSwVersion;
	//검사기 종류(유첨2. 조립 Vision Type Code 기준)	
	CString	m_VisonType;
	//검사 Live Signal (1), 가동 불가 (0)
	//가동 불가 사유로는 SW 외적인 요인에 의한 가동 불가 상태	
	CString	m_InspStatus;

public:
	static CString MakeStatusInDataText;

	//Message 종류	
	void setCategory(CString	Category) { m_Category = Category; }
	//발생시간	
	void setTransferTimer(CString	TransferTimer) { m_TransferTimer = TransferTimer; }
	//검사기ID	
	void setEqpInspId(CString	EqpInspId) { m_EqpInspId = EqpInspId; }
	//대공정명	
	void setProcessGroup(CString	ProcessGroup) { m_ProcessGroup = ProcessGroup; }
	//세부공정명	
	void setProcessName(CString	ProcessName) { m_ProcessName = ProcessName; }
	//MMD기준 설비ID 기입	
	void setEqpId(CString	EqpId) { m_EqpId = EqpId; }
	//Agent Sw Version	
	void setAgentSwVersion(CString	AgentSwVersion) { m_AgentSwVersion = AgentSwVersion; }
	//검사기 종류(유첨2. 조립 Vision Type Code 기준)	
	void setVisonType(CString	VisonType) { m_VisonType = VisonType; }
	//검사 Live Signal (1), 가동 불가 (0)
	//가동 불가 사유로는 SW 외적인 요인에 의한 가동 불가 상태	
	void setInspStatus(CString	InspStatus) { m_InspStatus = InspStatus; }

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_Status();
};


#endif // SPCSTATUSINDATA_H
