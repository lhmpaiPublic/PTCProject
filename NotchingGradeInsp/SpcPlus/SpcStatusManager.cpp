// SpcStatusManager.cpp: CSpcStatusManager 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcStatusManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcStatusManager::CSpcStatusManager()
{
	//SPC Plus Header 객체 포인터	
	m_SpcHeader = new CSpcHeader(this);
	//SPC Reference Data Set 객체 포인터	
	m_SpcRefDs = new CSpcRefDs(this);
	//In Data(송신 데이터) 객체 포인터	
	m_SpcStatusInData = new CSpcStatusInData(this);
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcStatusManager::~CSpcStatusManager()
{
	//SPC Plus Header 객체 포인터	
	if(m_SpcHeader)
		delete m_SpcHeader;
	//SPC Reference Data Set 객체 포인터	
	if(m_SpcRefDs)
		delete m_SpcRefDs;
	//In Data(송신 데이터) 객체 포인터	
	if(m_SpcStatusInData)
		m_SpcStatusInData;
}

// JSON 형식의 파일 생성
void CSpcStatusManager::makeJSONFile()
{
	CGlobalFunc::makeJSONFile("D:\\JSON", "Status.txt", m_SpcHeader->makeJSONText_Header());
	CGlobalFunc::makeJSONFile("D:\\JSON", "Status.txt", m_SpcRefDs->getJSONText_RefDsFront());

	CGlobalFunc::makeJSONFile("D:\\JSON", "Status.txt", m_SpcStatusInData->makeJSONText_Status());

	CGlobalFunc::makeJSONFile("D:\\JSON", "Status.txt", m_SpcRefDs->getJSONText_RefDsTail());
	CGlobalFunc::makeJSONFile("D:\\JSON", "Status.txt", m_SpcHeader->getJSONText_HeaderTail());

}