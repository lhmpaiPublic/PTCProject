// UiManager.cpp: CUiManager 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "UiManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//자신의 전역 객체 생성
CUiManager* CUiManager::gInstObject=NULL;

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CUiManager::CUiManager()
{
	//class 인스턴스 객체
	m_pNotchingGradeInspView = NULL;
	m_pMainFrame = NULL;
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CUiManager::~CUiManager()
{

}
//전역 인스턴스 객체 생성 및 접근
CUiManager* CUiManager::gInst()
{
	return gInstObject;
}

//UiManager 메모리 해제
void CUiManager::CreateUiManager()
{
	if (gInstObject == NULL)
	{
		gInstObject = new CUiManager();
	}
}

//UiManager 메모리 해제
void CUiManager::ExitUiManager()
{
	if (gInstObject != NULL)
	{
		delete gInstObject;
		gInstObject = NULL;
	}
}