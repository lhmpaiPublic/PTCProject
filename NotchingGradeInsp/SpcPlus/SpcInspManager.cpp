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
	//동기화 객체 초기화
	::InitializeCriticalSection(&m_csQueue);
	//JSON Data 추가 완료 여부 확인
	m_CreateJSONFile = 2;

	//결함 종류 저장소 동기화 객체 초기화
	::InitializeCriticalSection(&m_csDefectKindNameVec);

	//결함정보 저장소 동기화 객체 초기화
	::InitializeCriticalSection(&m_csDefectInfoQueue);

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
	//동기화 객체 메모리 삭제
	::DeleteCriticalSection(&m_csQueue);
	//동기화 객체 메모리 삭제
	::DeleteCriticalSection(&m_csDefectInfoQueue);

	//결함 종류 저장소 동기화 객체 삭제
	::DeleteCriticalSection(&m_csDefectKindNameVec);

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

//Defect 명 중복을 제거하고 넘긴다.
std::vector<CString> CSpcInspManager::getDefectKindNameUnique()
{
	//요소를 정렬한다.
	std::sort(m_DefectKindName.begin(), m_DefectKindName.end());
	//중복을 제거하면서 쓰래기도 제거한다.
	m_DefectKindName.erase(std::unique(m_DefectKindName.begin(), m_DefectKindName.end()), m_DefectKindName.end());
	return m_DefectKindName;
}

//JSON Data 추가 완료 여부 확인
bool CSpcInspManager::getCreateJSONFile()
{
	bool b = false;
	::EnterCriticalSection(&m_csQueue);
	m_CreateJSONFile--;
	if (m_CreateJSONFile == 0)
		b = true;
	::LeaveCriticalSection(&m_csQueue);
	return b;
}

//Defect 명 종료 중복확인용
void CSpcInspManager::addDefectKindName(CString DefectKindName)
{
	::EnterCriticalSection(&m_csDefectKindNameVec);
	m_DefectKindName.push_back(DefectKindName);
	::LeaveCriticalSection(&m_csDefectKindNameVec);
}

//이미지 퀄리티 정보 객체 포인터
CSpcInDataIqInfo* CSpcInspManager::getSpcInDataIqInfo(IqTopBottomSelect select)
{ 
	return m_SpcInDataIqInfo[select]; 
}


//결함정보 객체 포인터
CSpcInDataDefectInfo* CSpcInspManager::getSpcInDataDefectInfo(DefectTopBottomSelect camNo)
{
	CSpcInDataDefectInfo* SpcInDataDefectInfo = NULL;

	for (int idx = 0; idx < m_SpcInDataDefectInfo.size(); idx++)
	{
		if (m_SpcInDataDefectInfo[idx]->getDefectCameraNumber_int() == camNo)
		{
			SpcInDataDefectInfo = m_SpcInDataDefectInfo[idx];
		}
	}

	return SpcInDataDefectInfo;
}



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
		//인덱스가 데이터 사이즈보다 작을 때는 ,(ah
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

//결함정보 객체를 추가한다.
void CSpcInspManager::addSpcInDataDefectInfo(CSpcInDataDefectInfo* SpcInDataDefectInfo) 
{ 
	::EnterCriticalSection(&m_csDefectInfoQueue);
	m_SpcInDataDefectInfo.push_back(SpcInDataDefectInfo); 
	::LeaveCriticalSection(&m_csDefectInfoQueue);
}

//SpcInDataDefectInfo 크기
int CSpcInspManager::getSpcInDataDefectInfoSize()
{ 
	return (int)m_SpcInDataDefectInfo.size(); 
}

//결함정보 객체에 index를 부여한다.
//카메라0, 1 번의 모든 Defect 정보 갯수 vector에 담고 JSON파일 생성전에 부여한다.
//중간에 부여시 동기화가 필요해 시스템에 부하발생
void CSpcInspManager::setIndexSpcInDataDefectInfo()
{
	for (int idx = 0; idx < (int)m_SpcInDataDefectInfo.size(); idx++)
	{
		//결함 중 해당 결함의 순서	
		m_SpcInDataDefectInfo[idx]->setDefectIndex(CGlobalFunc::intToString(idx+1));
	}
}