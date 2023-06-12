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
	//Act Id 세팅
	m_SpcHeader->setActId("INSPECTION");

	//SPC Reference Data Set 객체 포인터	
	m_SpcRefDs = new CSpcRefDs(this);
	///In Data(송신 데이터) 객체 포인터	
	m_SpcInspInData = new CSpcInspInData(this);

	//카메라 1번
	CSpcInDataIqInfo* tempIq = new CSpcInDataIqInfo(this);
	tempIq->setIqCameraNumber("1");
	m_SpcInDataIqInfo.push_back(tempIq);
	//카메라 2번
	tempIq = new CSpcInDataIqInfo(this);
	tempIq->setIqCameraNumber("2");
	m_SpcInDataIqInfo.push_back(tempIq);

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
//이미지 퀄리티 정보 객체 포인터
CSpcInDataIqInfo* CSpcInspManager::getSpcInDataIqInfo(TopBottomSelect select) 
{ 
	return m_SpcInDataIqInfo[select]; 
}
//결함정보 객체 포인터



// JSON 형식의 파일 생성
void CSpcInspManager::makeJSONFile()
{
	//파일 저장 경로
	CString strPath = m_SpcInspInData->JsonFilePath();
	//파일명을 가져온다.
	CString strJsonFileName = m_SpcInspInData->JsonFileName();

	CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcHeader->makeJSONText_Header());
	CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcRefDs->getJSONText_RefDsFront());

	CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcInspInData->makeJSONText_Insp1());

	for (int idx = 0; idx < (int)m_SpcInDataIqInfo.size(); idx++)
	{
		CString rn = "";
		if (idx < (m_SpcInDataIqInfo.size()-1))
		{
			rn = ",\r\n";
		}
		else
		{
			rn = "\r\n";
		}
		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcInDataIqInfo[idx]->makeJSONText_IqInfo()+rn);
	}

	CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcInspInData->makeJSONText_Insp2());

	for (int idx = 0; idx < (int)m_SpcInDataDefectInfo.size(); idx++)
	{
		CString rn = "";
		if (idx < (m_SpcInDataDefectInfo.size() - 1))
		{
			rn = ",\r\n";
		}
		else
		{
			rn = "\r\n";
		}
		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcInDataDefectInfo[idx]->makeJSONText_DefectInfo()+rn);
	}

	CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcInspInData->getSONText_InspTail());

	CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcRefDs->getJSONText_RefDsTail());
	CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcHeader->getJSONText_HeaderTail());

}