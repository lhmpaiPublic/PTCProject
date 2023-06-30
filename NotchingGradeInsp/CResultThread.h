#pragma once

#include <vector>

class CImageProcessCtrl;
class CFrameRsltInfo;
class CDefectDataCtrl;
// 22.06.10 Ahn Add Start
class CCropImgQueueCtrl;
// 22.06.10 Ahn Add End
// 22.06.23 Ahn Add Start
class CDefectQueueCtrl;
// 22.06.23 Ahn Add End
// 23.02.06 Ahn Add Start
class CBitmapStd;
// 23.02.06 Ahn Add End

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
class CSpcInspManager;
#endif //SPCPLUS_CREATE

class CResultThread
{
public:
	CResultThread( CImageProcessCtrl* pParent, int nHeadNo );
	~CResultThread();

	void Begin();
	void Kill(void);
	void SetDispWnd(HWND hwnd);
	void SetDefectDataCtrl(CDefectDataCtrl *pDefDataCtrl);
	CImageProcessCtrl* m_pParent;
	HWND m_DisphWnd;//[GRABBER_COUNT];
	typedef std::vector< CFrameRsltInfo* > _VEC_RSLT_INFO;

	// 동기화 이벤트 객체
	HANDLE getEvent_ResultThread() { return pEvent_ResultThread; }
	void setEvent_ResultThread() { SetEvent(pEvent_ResultThread); }
protected:
	BOOL	m_bKill;
	int		m_nThreadId;
	CWinThread* m_pThread;				//!< 긚깒긞긤(CWinThread *)x1
	int		m_nHeadNo;
	CDefectDataCtrl* m_pDefDataCtrl;

	//동기화 이벤트 객체
	HANDLE pEvent_ResultThread;
protected:
	static UINT CtrlThreadResultProc(LPVOID pParam);

public :
	static void DrawImage(HWND hwnd, CFrameRsltInfo* pFrameInfo, BYTE* pImgPtr, int nWidth, int nHeight, int nMagnif);
	static void DrawDefectRect(CDC* pDC, CRect rcDef, COLORREF color);
	// 22.02.25 Ahn Add Start
	static void DrawString(CDC* pDC, int x, int y, COLORREF color, CString strLine);
	static void CaptureImage(HWND hWnd, CString strPath);
	// 22.02.25 Ahn Add End 
	
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
	//SPC+ DefectInfo 객체 추가
	static void SaveCropImage(BYTE* pImgPtr, int nWidth, int nHeight, CFrameRsltInfo* pFrmInfo, CCropImgQueueCtrl *pQueueCtrl, CDefectQueueCtrl* pDefectQueue, CSpcInspManager* insp);
#else
	static void SaveCropImage(BYTE* pImgPtr, int nWidth, int nHeight, CFrameRsltInfo* pFrmInfo, CCropImgQueueCtrl* pQueueCtrl, CDefectQueueCtrl* pDefectQueue);
#endif //SPCPLUS_CREATE
	// 22.06.10 Ahn Add End

	// 23.02.03 Ahn Add Start
	static void SaveResultImage(HWND hwnd, CFrameRsltInfo* pFrameInfo, BYTE* pImgPtr, int nWidth, int nHeight, int nMagnif, CBitmapStd *pBmpStd);
	static void DrawImage_Test(CDC* pDC, CFrameRsltInfo* pRsltInfo, int nWidth, int nHeight, int nMagnif); 
	// 23.02.03 Ahn Add End
};

