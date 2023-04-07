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

protected :
	BOOL	m_bKill ;
	int		m_nThreadId ;
	CWinThread* m_pThread ;				//!< 긚깒긞긤(CWinThread *)x1

protected:
	static UINT CtrlThreadImgProc( LPVOID pParam ) ;
	static UINT CtrlThreadImgCuttingTab(LPVOID Param) ;
	// 22.07.19 Ahn Modify Start
	static int GetMarkingFlag(CRecipeInfo* pRecipeInfo, int nTopJudge, int nBtmJudge, WORD wTopReson, WORD wBtmReson, int& nMarkSel1, int& nMarkSel2 );
	// 22.07.19 Ahn Modify End
	// 22.12.09 Ahn Add Start
	static double GetDiffTime(LARGE_INTEGER stTime, double dFrequency);
	// 22.12.09 Ahn Add End
};
