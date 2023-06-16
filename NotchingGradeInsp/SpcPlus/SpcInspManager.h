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
	//동기화 객체
	CRITICAL_SECTION m_csQueue;
	//JSON Data 추가 완료 여부 확인
	//Tab, Tab없는 스래드에서 동시에 접근해서 값을 넣을 때 변수값을 증가 시킨다. 2 초기화하고 
	//get 할 때 1씩 감소한다.
	int m_CreateJSONFile;

	//Defect 명 종료 중복확인용
	std::vector<CString> m_DefectKindName;
public:
	CSpcInspManager();
	virtual ~CSpcInspManager();

	//JSON Data 추가 완료 여부 확인
	void setCreateJSONFile(int  CreateJSONFile) { m_CreateJSONFile = CreateJSONFile; }

	//JSON Data 추가 완료 여부 확인
	bool getCreateJSONFile();

	//Defect 명 종료 중복확인용
	std::vector<CString>* getDefectKindName(){ return &m_DefectKindName; }
	//Defect 명 중복을 제거하고 넘긴다.
	std::vector<CString> getDefectKindNameUnique();

	enum IqTopBottomSelect
	{
		IQINFO_TOP,
		IQINFO_BOTTOM
	};

	enum DefectTopBottomSelect
	{
		DEFECTINFO_CAMENO_1 = 1,
		DEFECTINFO_CAMENO_2 = 2
	};

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
	CSpcInDataIqInfo* getSpcInDataIqInfo(IqTopBottomSelect select);
	//결함정보 객체 포인터
	CSpcInDataDefectInfo* getSpcInDataDefectInfo(DefectTopBottomSelect camNo);
	//결함정보 객체를 추가한다.
	//동기화 객체
	CRITICAL_SECTION m_csDefectInfoQueue;
	void addSpcInDataDefectInfo(CSpcInDataDefectInfo* SpcInDataDefectInfo);
	//SpcInDataDefectInfo 크기
	int getSpcInDataDefectInfoSize();

	// JSON 형식의 파일 생성
	virtual void makeJSONFile();

};


#endif // SPCINSPMANAGER_H
