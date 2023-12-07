// SpcCreateJSONFileThread.cpp: CSpcCreateJSONFileThread 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcCreateJSONFileThread.h"
#include "SpcInfo.h"


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
			//스래드를 생성한다.
			gInstObject->CreateThread();
		}
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
		else
		{
				LOGDISPLAY_SPEC(3)("SPC+===== CreateSpcCreateJSONFileThread 실패 === ");
		}
#endif //SPCPLUS_CREATE
	}
}

//객체 소멸함수
void CSpcCreateJSONFileThread::ExitSpcCreateJSONFileThread()
{
	// 로그출력 창 생성
	if (gInstObject != NULL)
	{
		delete gInstObject;
		gInstObject = NULL;
		bCreate = false;
	}
}

//로그 ListBox에 출력
#define SPCCREATEJSONFILETHREAD_TIMEOUT 200
UINT CSpcCreateJSONFileThread::ThreadProc(LPVOID param)
{
	CSpcCreateJSONFileThread* pMain = (CSpcCreateJSONFileThread*)param;
	std::queue<CSpcPlusManager*>* SpcList = pMain->getSpcList();
	UINT ret;

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
	LOGDISPLAY_SPEC(3)("SPC+===== ThreadProc 생성");
#endif //SPCPLUS_CREATE

	while (pMain)
	{
		ret = WaitForSingleObject(pMain->getEvent_SpcCreateJSONFileThread(), SPCCREATEJSONFILETHREAD_TIMEOUT);

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			break;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			//Do something...
			while (true)
			{
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
					break;
				}
			}

			//객체 소멸 시 스래드 함수 빠져 나가기
			if (CLogDisplayDlg::bCreate == FALSE)
			{
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
				LOGDISPLAY_SPEC(3)("SPC+===== Make Json File 생성 루프 Create FALSE");
#endif //SPCPLUS_CREATE
				break;
			}
		}
		else
		{
			break;
		}

	}

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
	LOGDISPLAY_SPEC(3)("SPC+===== ThreadProc 소멸");
#endif //SPCPLUS_CREATE
	return 0;
}
//스래드 생성함수
void CSpcCreateJSONFileThread::CreateThread()
{
	::InitializeCriticalSection(&m_csQueue);

	//이벤트 객체 생성
	pEvent_SpcCreateJSONFileThread = CreateEvent(NULL, FALSE, FALSE, NULL);

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
		LOGDISPLAY_SPEC(3)("SPC+===== pEvent_SpcCreateJSONFileThread CreateEvent <%s> === ", (pEvent_SpcCreateJSONFileThread == NULL) ? "실패":"성공");
#endif //SPCPLUS_CREATE

	//스래드 생성
	m_pThread = AfxBeginThread(ThreadProc, this);
}
//스래드 종료함수
void CSpcCreateJSONFileThread::ExitThread()
{
	if (m_pThread)
	{
		setEvent_SpcCreateJSONFileThread();
		CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
		m_pThread->m_hThread = NULL;
	}
	::DeleteCriticalSection(&m_csQueue);
}

//SPC 객체 추가 전역함수
void CSpcCreateJSONFileThread::AddSpcPlusManager(CSpcPlusManager* obj)
{
	if (CSpcInfo::Inst()->getSPCStartFlag())
	{
		CSpcCreateJSONFileThread::gInst()->AddSpcPlus(obj);
	}
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	else
	{
		LOGDISPLAY_SPEC(3)("SPC+===== Add Spc Manager Failed None Run === ");
	}
#endif //SPCPLUS_CREATE
}

//SPC 객체 추가 함수
void CSpcCreateJSONFileThread::AddSpcPlus(CSpcPlusManager* obj)
{
	if (bCreate)
	{
		::EnterCriticalSection(&m_csQueue);
		m_SpcList.push(obj);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
		LOGDISPLAY_SPEC(3)("SPC+===== SPCPlusMamager Size<%d> === ", m_SpcList.size());
#endif //SPCPLUS_CREATE
		::LeaveCriticalSection(&m_csQueue);
	}
}

CSpcPlusManager* CSpcCreateJSONFileThread::popSpcPlus()
{
	// TODO: 여기에 구현 코드 추가.
	CSpcPlusManager* obj = NULL;
	if (bCreate)
	{
		::EnterCriticalSection(&m_csQueue);
		obj = m_SpcList.front();
		m_SpcList.pop();
		::LeaveCriticalSection(&m_csQueue);
	}
	return obj;
}