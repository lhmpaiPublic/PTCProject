// SpcCreateJSONFileThread.h : CSpcCreateJSONFileThread 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCCREATEJSONFILETHREAD_H
#define SPCCREATEJSONFILETHREAD_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcCreateJSONFileThread
{
	//자신의 전역 객체 생성
	static CSpcCreateJSONFileThread* gInstObject;
	//객체 생성 여부 확인용
	static bool bCreate;
	//SPC Plus 저장용
	std::queue<CSpcPlusManager*> m_SpcList;

	//스래드 생성 인스턴스 객체
	CWinThread* m_pThread;
	//스래드를 계속 유지 여부 설정 변수
	bool m_isWorkingThread;

	//동기화 객체
	CRITICAL_SECTION m_csQueue;
public:
	CSpcCreateJSONFileThread() ;
	virtual ~CSpcCreateJSONFileThread() ;

	//전역 인스턴스 객체 생성 및 접근
	static CSpcCreateJSONFileThread* gInst();

	//SpcCreateJSONFileThread 메모리 해제
	static void CreateSpcCreateJSONFileThread();

	//SpcCreateJSONFileThread 메모리 해제
	static void ExitSpcCreateJSONFileThread();

	//SPC 객체 추가 전역함수
	static void AddSpcPlusManager(CSpcPlusManager* obj);

	//SPC 객체 추가 함수
	void AddSpcPlus(CSpcPlusManager* obj);
	//SPC 객체를 가져온다.
	CSpcPlusManager* popSpcPlus();

	//SPC Plus 객체를 스래드에 접근하기 위한 포인터 객체를 요구한다.
	std::queue<CSpcPlusManager*>* getSpcList() { return &m_SpcList; }

	//로그 ListBox에 출력
	static UINT ThreadProc(LPVOID param);
	//스래드 생성함수
	void CreateThread();
	//스래드 종료함수
	void ExitThread();

};


#endif // SPCCREATEJSONFILETHREAD_H
