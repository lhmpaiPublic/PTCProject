// SpcAlarmManager.cpp: CSpcAlarmManager 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcAlarmManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcAlarmManager::CSpcAlarmManager()
{
	//SPC Plus Header 객체 포인터	
	m_SpcHeader = new CSpcHeader(this);
	//Act Id 세팅
	m_SpcHeader->setActId("ALARM");

	//SPC Reference Data Set 객체 포인터	
	m_SpcRefDs = new CSpcRefDs(this);
	///In Data(송신 데이터) 객체 포인터	
	m_SpcAlarmInData = new CSpcAlarmInData(this);
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcAlarmManager::~CSpcAlarmManager()
{
	//SPC Plus Header 객체 포인터	
	if (m_SpcHeader)
		delete m_SpcHeader;
	//SPC Reference Data Set 객체 포인터	
	if (m_SpcRefDs)
		delete m_SpcRefDs;
	///In Data(송신 데이터) 객체 포인터	
	if (m_SpcAlarmInData)
		delete m_SpcAlarmInData;
}

// JSON 형식의 파일 생성
void CSpcAlarmManager::makeJSONFile()
{
	if (CSpcInfo::Inst()->getSPCStartFlag())
	{
		//파일 저장 경로
		CString strPath = m_SpcAlarmInData->JsonFilePath();
		//파일명을 가져온다.
		CString strJsonFileName = m_SpcAlarmInData->JsonFileName();

		CString strMakeJsonData = "";

		//CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcHeader->makeJSONText_Header());
		strMakeJsonData += m_SpcHeader->makeJSONText_Header();
		//CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcRefDs->getJSONText_RefDsFront());
		strMakeJsonData += m_SpcRefDs->getJSONText_RefDsFront();

		//CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcAlarmInData->makeJSONText_Alarm());
		strMakeJsonData += m_SpcAlarmInData->makeJSONText_Alarm();

		//CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcRefDs->getJSONText_RefDsTail());
		strMakeJsonData += m_SpcRefDs->getJSONText_RefDsTail();
		//CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcHeader->getJSONText_HeaderTail());
		strMakeJsonData += m_SpcHeader->getJSONText_HeaderTail();
		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, strMakeJsonData);
	}

}