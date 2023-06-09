// SpcSpecParaInData.h : CSpcSpecParaInData 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCSPECPARAINDATA_H
#define SPCSPECPARAINDATA_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcParaInfo;
class CSpcSpecInfo;
class CSpcSpecParaInData : public CSpcInData
{
public:
	CSpcSpecParaInData(CSpcPlusManager* sMgr = NULL);
	virtual ~CSpcSpecParaInData();

private:
	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;

	//Message 종류	
	CString	m_Category;
	//MMD기준 설비ID 기입 
	//10-4 참조	
	CString	m_EqpId;
	//검사기ID	
	CString	m_EqpInspId;
	//검사PC에 저장된 모델 정보	
	CString	m_RecipeId;
	//검사 레시피	
	CString	m_InspRecipe;
	//검사기 S/W Version	
	CString	m_InspSwVersion;
	//검사Para FILE 변경시간	
	CString	m_ModifyTime;
	//변경자 이름
	//프로그램 Off/On 보고시 "NA"	
	CString	m_Modifier;
	//변경 사유
	//프로그램 Off/On 보고시 "RESTART"
	CString	m_ModifyReason;

public:
	static char* MakeSpecParaInDataText_1;
	static char* MakeSpecParaInDataText_2;
	static char* MakeSpecParaInDataText_3;

	//Message 종류	
	void setCategory(CString	Category) { m_Category = Category; }
	//MMD기준 설비ID 기입 
	//10-4 참조	
	void setEqpId(CString	EqpId) { m_EqpId = EqpId; }
	//검사기ID	
	void setEqpInspId(CString	EqpInspId) { m_EqpInspId = EqpInspId; }
	//검사PC에 저장된 모델 정보	
	void setRecipeId(CString	RecipeId) { m_RecipeId = RecipeId; }
	//검사 레시피	
	void setInspRecipe(CString	InspRecipe) { m_InspRecipe = InspRecipe; }
	//검사기 S/W Version	
	void setInspSwVersion(CString	InspSwVersion) { m_InspSwVersion = InspSwVersion; }
	//검사Para FILE 변경시간	
	void setModifyTime(CString	ModifyTime) { m_ModifyTime = ModifyTime; }
	//변경자 이름
	//프로그램 Off/On 보고시 "NA"	
	void setModifier(CString	Modifier) { m_Modifier = Modifier; }
	//변경 사유
	//프로그램 Off/On 보고시 "RESTART"
	void setModifyReason(CString	ModifyReason) { m_ModifyReason = ModifyReason; }

	//검사Para FILE 변경시간	
	CString	getModifyTime() {return m_Modifier;}

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_SpecPara1();
	CString getJSONText_SpecPara2();
	CString getJSONText_SpecParaTail();

	//파일저장경로를 넘긴다.
	CString JsonFilePath();

	//파일명을 넘긴다.
	CString JsonFileName();

};


#endif // SPCSPECPARAINDATA_H
