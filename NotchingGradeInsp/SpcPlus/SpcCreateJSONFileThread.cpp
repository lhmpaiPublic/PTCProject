// SpcCreateJSONFileThread.cpp: CSpcCreateJSONFileThread 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcCreateJSONFileThread.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//자신의 전역 객체 생성
CSpcCreateJSONFileThread* CSpcCreateJSONFileThread::gInstObject = NULL;
//객체 생성 여부 확인용
bool CSpcCreateJSONFileThread::bCreate = false;

//전역 객체 인스턴스
CSpcCreateJSONFileThread* CSpcCreateJSONFileThread::gInst()
{
	return gInstObject;
}

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcCreateJSONFileThread::CSpcCreateJSONFileThread()
{
	//스래드를 생성한다.
	CreateThread();
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcCreateJSONFileThread::~CSpcCreateJSONFileThread()
{
	//스래드를 소멸한다.
	ExitThread();
}

//객체 생성함수 싱글톤
void CSpcCreateJSONFileThread::CreateSpcCreateJSONFileThread()
{
	// 로그출력 창 생성
	if (gInstObject == NULL)
	{
		gInstObject = new CSpcCreateJSONFileThread();
		if (gInstObject)
		{
			bCreate = true;
		}		
	}
}

//객체 소멸함수
void CSpcCreateJSONFileThread::ExitSpcCreateJSONFileThread()
{
	// 로그출력 창 생성
	if (gInstObject != NULL)
	{
		bCreate = false;
		delete gInstObject;
		gInstObject = NULL;
	}
}

//로그 ListBox에 출력
UINT CSpcCreateJSONFileThread::ThreadProc(LPVOID param)
{
	CSpcCreateJSONFileThread* pMain = (CSpcCreateJSONFileThread*)param;
	std::queue<CSpcPlusManager*>* SpcList = pMain->getSpcList();
	while (pMain)
	{
		if (CGlobalFunc::isPeekMessage() == WM_NULL)
		{
		//Do something...
		if (CLogDisplayDlg::bCreate && SpcList->size())
		{
			CSpcPlusManager* obj = pMain->popSpcPlus();
			if (obj)
			{
				obj->makeJSONFile();
				delete obj;
			}
		}
		else
		{
			if(pMain->m_isWorkingThread == false)
				break;
		}
		}
		else if (CGlobalFunc::isPeekMessage() == WM_QUIT)
		{
			break;
		}
	}

	return 0;
}
//스래드 생성함수
void CSpcCreateJSONFileThread::CreateThread()
{
	::InitializeCriticalSection(&m_csQueue);

	//source file
	m_isWorkingThread = true;

	//스래드 생성
	m_pThread = AfxBeginThread(ThreadProc, this);
}
//스래드 종료함수
void CSpcCreateJSONFileThread::ExitThread()
{
	m_isWorkingThread = false;
	CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
	::DeleteCriticalSection(&m_csQueue);
}

//SPC 객체 추가 전역함수
void CSpcCreateJSONFileThread::AddSpcPlusManager(CSpcPlusManager* obj)
{
	CSpcCreateJSONFileThread::gInst()->AddSpcPlus(obj);
}

//SPC 객체 추가 함수
void CSpcCreateJSONFileThread::AddSpcPlus(CSpcPlusManager* obj)
{
	if (bCreate)
	{
		::EnterCriticalSection(&m_csQueue);
		m_SpcList.push(obj);
		::LeaveCriticalSection(&m_csQueue);
	}
}

CSpcPlusManager* CSpcCreateJSONFileThread::popSpcPlus()
{
	// TODO: 여기에 구현 코드 추가.
	CSpcPlusManager* obj = NULL;
	if (bCreate)
	{
		obj = m_SpcList.front();
		::EnterCriticalSection(&m_csQueue);
		m_SpcList.pop();
		::LeaveCriticalSection(&m_csQueue);
	}
	return obj;
}