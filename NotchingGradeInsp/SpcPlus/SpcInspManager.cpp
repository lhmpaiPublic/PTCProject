// SpcInspManager.cpp: CSpcInspManager 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcInspManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInspManager::CSpcInspManager()
{
	//SPC Plus Header 객체 포인터	
	m_SpcHeader = new CSpcHeader(this);
	//SPC Reference Data Set 객체 포인터	
	m_SpcRefDs = new CSpcRefDs(this);
	///In Data(송신 데이터) 객체 포인터	
	m_SpcInspInData = new CSpcInspInData(this);

	CSpcInDataIqInfo* tempIq = new CSpcInDataIqInfo(this);
	m_SpcInDataIqInfo.push_back(tempIq);

	CSpcInDataDefectInfo* tempdefect = new CSpcInDataDefectInfo(this);
	m_SpcInDataDefectInfo.push_back(tempdefect);
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcInspManager::~CSpcInspManager()
{
	//SPC Plus Header 객체 포인터	
	if (m_SpcHeader)
		delete m_SpcHeader;
	//SPC Reference Data Set 객체 포인터	
	if (m_SpcRefDs)
		delete m_SpcRefDs;
	///In Data(송신 데이터) 객체 포인터	
	if (m_SpcInspInData)
		delete m_SpcInspInData;
	//이미지 퀄리티 정보 객체 포인터
	for (int idx = 0; idx < (int)m_SpcInDataIqInfo.size(); idx++)
		delete m_SpcInDataIqInfo[idx];
	//결함정보 객체 포인터
	for (int idx = 0; idx < (int)m_SpcInDataDefectInfo.size(); idx++)
		delete m_SpcInDataDefectInfo[idx];

}

// JSON 형식의 파일 생성
void CSpcInspManager::makeJSONFile()
{
	CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcHeader->makeJSONText_Header());
	CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcRefDs->getJSONText_RefDsFront());

	CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcInspInData->makeJSONText_Insp1());

	for (int idx = 0; idx < (int)m_SpcInDataIqInfo.size(); idx++)
		CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcInDataIqInfo[idx]->makeJSONText_IqInfo());

	CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcInspInData->makeJSONText_Insp2());

	for (int idx = 0; idx < (int)m_SpcInDataDefectInfo.size(); idx++)
		CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcInDataDefectInfo[idx]->makeJSONText_DefectInfo());

	CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcInspInData->getSONText_InspTail());

	CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcRefDs->getJSONText_RefDsTail());
	CGlobalFunc::makeJSONFile("D:\\JSON", "Insp.txt", m_SpcHeader->getJSONText_HeaderTail());

}