// SpcSpecParaManager.cpp: CSpcSpecParaManager 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcSpecParaManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcSpecParaManager::CSpcSpecParaManager()
{
	//SPC Plus Header 객체 포인터	
	m_SpcHeader = new CSpcHeader(this);
	//Act Id 세팅
	m_SpcHeader->setActId("SPEC_PARA");

	//SPC Reference Data Set 객체 포인터	
	m_SpcRefDs = new CSpcRefDs(this);
	///In Data(송신 데이터) 객체 포인터	
	m_SpcSpecParaInData = new CSpcSpecParaInData(this);

}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcSpecParaManager::~CSpcSpecParaManager()
{
	//SPC Plus Header 객체 포인터	
	if(m_SpcHeader)
		delete m_SpcHeader;
	//SPC Reference Data Set 객체 포인터	
	if(m_SpcRefDs)
		delete m_SpcRefDs;
	///In Data(송신 데이터) 객체 포인터	
	if(m_SpcSpecParaInData)
		delete m_SpcSpecParaInData;
	//변경Para 정보	
	for (int idx = 0; idx < (int)m_SpcParaInfo.size(); idx++)
		delete m_SpcParaInfo[idx];
	//변경 Spec 정보	
	for (int idx = 0; idx < (int)m_SpcSpecInfo.size(); idx++)
		delete m_SpcSpecInfo[idx];
}

// JSON 형식의 파일 생성
void CSpcSpecParaManager::makeJSONFile()
{
	if (CSpcInfo::Inst()->getSPCStartFlag())
	{
		//파일 저장 경로
		CString strPath = m_SpcSpecParaInData->JsonFilePath();
		//파일명을 가져온다.
		CString strJsonFileName = m_SpcSpecParaInData->JsonFileName();

		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcHeader->makeJSONText_Header());
		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcRefDs->getJSONText_RefDsFront());

		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcSpecParaInData->makeJSONText_SpecPara1());

		for (int idx = 0; idx < (int)m_SpcParaInfo.size(); idx++)
		{
			CString rn = "";
			if (idx < (m_SpcParaInfo.size() - 1))
			{
				rn = ",\r\n";
			}
			else
			{
				rn = "\r\n";
			}
			CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcParaInfo[idx]->makeJSONText_ParaInfo() + rn);
		}

		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcSpecParaInData->getJSONText_SpecPara2());

		for (int idx = 0; idx < (int)m_SpcSpecInfo.size(); idx++)
		{
			CString rn = "";
			if (idx < (m_SpcSpecInfo.size() - 1))
			{
				rn = ",\r\n";
			}
			else
			{
				rn = "\r\n";
			}
			CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcSpecInfo[idx]->makeJSONText_SpecInfo() + rn);
		}

		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcSpecParaInData->getJSONText_SpecParaTail());

		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcRefDs->getJSONText_RefDsTail());
		CGlobalFunc::makeJSONFile(strPath, strJsonFileName, m_SpcHeader->getJSONText_HeaderTail());
	}

}