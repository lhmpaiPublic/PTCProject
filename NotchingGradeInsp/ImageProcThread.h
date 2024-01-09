#pragma once


#include <vector>

//class CTabInfo {
//public:
//	CTabInfo() { nFrameCount = 0; nHeadLeft = 0; nHeadRight = 0; nLeft = 0; nRight = 0; nTabNo = 0; nTabNoInFrame = 0; nCenter = 0; nWidth = 0; qwHeadStPos = 0; nHeadWidth = 0; };
//	int nFrameCount;	//
//	int nTabNoInFrame;	// 
//	int nTabNo;		//
//	int nLeft;			//
//	int nRight;		//
//	int nHeadLeft;		// 탭 머리 왼쪽
//	int nHeadRight;	// 탭 머리 오른쪽
//	int nCenter;
//	int nWidth;
//	int nHeadWidth;
//	QWORD qwHeadStPos;	// 검사 시작 Frame 을 기준으로 Tab Head의 Pixel 위치 
//
//};

class CRecipeInfo;
class CTabInfo;
class CImageProcessCtrl ;

//이미지 처리 스래드 객체
class CImageProcThread
{
public:
	CImageProcThread( CImageProcessCtrl *pParent );
	~CImageProcThread(void);

	void Begin( int nMode ) ;

	void Kill( void ) ;
	static WORD GetCounterSignal(int nTabId, int nJudge1, int nJudge2, int nMarkSel1, int nMarkSel2);

	CImageProcessCtrl *m_pParent ;
	typedef std::vector< CTabInfo > _VEC_TAB_INFO;
	// Tab구분 저장 None Halcon Version

	typedef struct Sector {
		int nStartPos;
		int nEndPos;
		int nMode;// 0 = LOW , 1 = High
		BOOL bDelete;
	} ST_SECTOR;

	enum {
		en_Low = 0,
		en_High,
		en_Max,
	};

	typedef std::vector< ST_SECTOR > VEC_SECTOR;

	// 동기화 이벤트 객체
	HANDLE getEvent_ImageProcThread_TabFind() { return pEvent_ImageProcThread_TabFind; }
	void setEvent_ImageProcThread_TabFind() { SetEvent(pEvent_ImageProcThread_TabFind); }
	void resetEvent_ImageProcThread_TabFind() { ResetEvent(pEvent_ImageProcThread_TabFind); }
	void resume_ImageProcThread_TabFind() { m_pThread->ResumeThread(); }

	HANDLE getEvent_ImageProcThread_Result() { return pEvent_ImageProcThread_Result; }
	void setEvent_ImageProcThread_Result() { SetEvent(pEvent_ImageProcThread_Result); }

	HANDLE getEvent_ImageProcThread_InspComplate() { return pEvent_ImageProcThread_InspComplate; }
	void setEvent_ImageProcThread_InspComplate() { SetEvent(pEvent_ImageProcThread_InspComplate); }

	static double TabPitchCalculate(int bforeImageLengtch, int bforeTabLeft, int nownTabLeft, double dResolY);

protected :
	BOOL	m_bKill ;
	int		m_nThreadId ;
	CWinThread* m_pThread ;				//!< 스래드 객체

	//동기화 이벤트 객체
	//카메라에서 이미지를 받았을 때 이벤트 객체
	HANDLE pEvent_ImageProcThread_TabFind;
	//CImageProcThreadUnit가 이미지 프로세싱하는 첫번째 객체를 가져오기 위한 루프 이벤트 객체
	HANDLE pEvent_ImageProcThread_Result;
	//CImageProceThreadUnit 검사가 완료상태 확인용 루프 이벤트 객체
	HANDLE pEvent_ImageProcThread_InspComplate;

	//생성모드
	int m_CreateMode;
protected:
	//이미지 분석처리를 위한 스래드 함수
	static UINT CtrlThreadImgProc( LPVOID pParam ) ;

	//이미지 분석을 위한 검사영역 Cutting Tab 스래드 함수
	static UINT CtrlThreadImgCuttingTab(LPVOID Param) ;

	//레시피 정보에 설정된 마킹 방식이 저장된 방법을 가져온다.
	//마킹셀 1, 2 어디에 할 것인가를 가져온다.
	//nTopJudge NG 거나 nBtmJudge NG 거나
	//Surface < FoilExp < FoilExpOut 우선순위 선택됨
	static int GetMarkingFlag(CRecipeInfo* pRecipeInfo, int nTopJudge, int nBtmJudge, WORD wTopReson, WORD wBtmReson, int& nMarkSel1, int& nMarkSel2 );

};
