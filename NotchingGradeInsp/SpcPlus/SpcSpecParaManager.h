// SpcSpecParaManager.h : SpcSpecParaManager 관리 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCSPECPARAMANAGER_H
#define SPCSPECPARAMANAGER_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcHeader;
class CSpcRefDs;
class CSpcSpecParaInData;
class CSpcParaInfo;
class CSpcSpecInfo;
class CSpcSpecParaManager : public CSpcPlusManager
{
public:
	CSpcSpecParaManager() ;
	virtual ~CSpcSpecParaManager() ;

	//SPC Plus Header 객체 포인터	
	CSpcHeader* m_SpcHeader;
	//SPC Reference Data Set 객체 포인터	
	CSpcRefDs* m_SpcRefDs;
	///In Data(송신 데이터) 객체 포인터	
	CSpcSpecParaInData* m_SpcSpecParaInData;
	//변경Para 정보	
	std::vector<CSpcParaInfo*> m_SpcParaInfo;
	//변경 Spec 정보	
	std::vector<CSpcSpecInfo*> m_SpcSpecInfo;

	//SPC Plus Header 객체 포인터
	CSpcHeader* getSpcHeader() { return m_SpcHeader; }
	//SPC Reference Data Set 객체 포인터
	CSpcRefDs* getSpcRefDs() { return m_SpcRefDs; }
	///In Data(송신 데이터) 객체 포인터
	CSpcSpecParaInData* getSpcInspInData() { return m_SpcSpecParaInData; }
	//변경Para 정보	
	std::vector<CSpcParaInfo*> getSpcParaInfo() { return m_SpcParaInfo; }
	//변경 Spec 정보	
	std::vector<CSpcSpecInfo*> getSpcSpecInfo() { return m_SpcSpecInfo; }
};


#endif // SPCSPECPARAMANAGER_H
