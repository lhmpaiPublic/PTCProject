// SpcInspManager.h : CSpcInspManager 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCINSPMANAGER_H
#define SPCINSPMANAGER_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcHeader;
class CSpcRefDs;
class CSpcInspInData;
class CSpcInDataIqInfo;
class CSpcInDataDefectInfo;
class CSpcInspManager : public CSpcPlusManager
{
public:
	CSpcInspManager();
	virtual ~CSpcInspManager();

	//SPC Plus Header 객체 포인터
	CSpcHeader* m_SpcHeader;
	//SPC Reference Data Set 객체 포인터
	CSpcRefDs* m_SpcRefDs;
	///In Data(송신 데이터) 객체 포인터
	CSpcInspInData* m_SpcInspInData;
	//이미지 퀄리티 정보 객체 포인터
	std::vector<CSpcInDataIqInfo*> m_SpcInDataIqInfo;
	//결함정보 객체 포인터
	std::vector<CSpcInDataDefectInfo*> m_SpcInDataDefectInfo;

	//SPC Plus Header 객체 포인터
	CSpcHeader* getSpcHeader() { return m_SpcHeader; }
	//SPC Reference Data Set 객체 포인터
	CSpcRefDs* getSpcRefDs() { return m_SpcRefDs; }
	///In Data(송신 데이터) 객체 포인터
	CSpcInspInData* getSpcInspInData() { return m_SpcInspInData; }
	//이미지 퀄리티 정보 객체 포인터
	std::vector<CSpcInDataIqInfo*> getSpcInDataIqInfo() { return m_SpcInDataIqInfo; }
	//결함정보 객체 포인터
	std::vector<CSpcInDataDefectInfo*> getSpcInDataDefectInfo() { return m_SpcInDataDefectInfo; }

	// JSON 형식의 파일 생성
	virtual void makeJSONFile();

};


#endif // SPCINSPMANAGER_H
