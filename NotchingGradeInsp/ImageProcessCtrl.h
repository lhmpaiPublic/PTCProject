#pragma once

#include <vector>
#include <queue>
#include "CDefectDataCtrl.h"

class CFrameInfo;
class CQueueCtrl;
class CImageProcThread;
class CGrabberCtrl;
class CResultThread;
class CBitmapStd;
class CCounterThread;
class CCounterQueueCtrl;
class CThreadQueueCtrl;
// 22.05.31 Ahn Add Start
class CImageSaveQueueCtrl; 
class CImageSaveThread;
// 22.05.31 Ahn Add End
// 22.06.10 Ahn Add Start
class CCropImgQueueCtrl;
// 22.06.10 Ahn Add End
// 22.06.23 Ahn Add Start
class CDefectQueueCtrl ;
// 22.06.23 Ahn Add End

// 22.12.09 Ahn Add Start
class CTacTimeDataCtrl;
// 22.12.09 Ahn Add End

#define MAX_WAITETHREAD_MAKE 4
class CImageProcessCtrl
{

public:
	CImageProcessCtrl(void);
	~CImageProcessCtrl(void);

	int Top_FrameCtn;
	int Bottom_FrameCtn;

public:
	//이미지 처리 스래드 (대기 스래드)
	//출력 대기 이벤트 객체 push
	void ImgProcWaitThread_Event_push(HANDLE hEvent);
	//출력 대기 이벤트 객체 pop
	void ImgProcWaitThread_Event_pop();

private :
	CGrabberCtrl* m_pGrabCtrl[GRABBER_COUNT];
	int m_nImgProcIdx[GRABBER_COUNT];

	//카메라 프레임작동에 의한 데이터 저장소
	CQueueCtrl* m_pQueueFrmCtrl[GRABBER_COUNT];

	//카메라 프레임 정보의 이미지를 분석한 Tab 정보 저장소
	CQueueCtrl* m_pQueueCtrl;

	//이미지 프로세싱 결과 데이터 저장소
	CQueueCtrl* m_pQueueResult[GRABBER_COUNT] ;

	// 22.06.23 Ahn Add Start
	CDefectQueueCtrl *m_pDefectQueueCtrl ;
	// 22.06.23 Ahn Add End

	//카운터 큐 제어 객체
	CCounterQueueCtrl* m_pQueueCounterIn;

	//이미지 처리 스래드 (대기 스래드)
	CImageProcThread* m_pImgProcWaitThread[MAX_WAITETHREAD_MAKE];
	//출력 대기 이벤트 객체
	std::queue< HANDLE > m_pImgProcWaitThread_Event;
	//출력 대기 이벤트 객체 동기화 객체
	CRITICAL_SECTION m_csImgProcWaitThread_Event;
	//스래드 활성화 갯수
	int m_nImgProcWaitThread_ActiveCount;
	

	//이미지 Cut Tab 스래드
	CImageProcThread* m_pImgCutTabThread;

	//결과 스래드 카메라 댓수 만큼
	CResultThread* m_pResultThread[GRABBER_COUNT];

	//카운터 스래드 객체
	CCounterThread* m_pCounterThread;

	//스래드 큐를 제어할 객체
	//Grabber 객수

	//이미지 처리 결과 마킹 - 최종결과 생성을 위한 스래드 객체 저장소
	CThreadQueueCtrl* m_pThreadQueueCtrl[GRABBER_COUNT];

	//카메라 번호에서 부터 장비 세팅 정보가 저장된 객체
	//최종 프레임의 저리 결과를 파일로 저장하기 위해서 사용되는 객체
	CDefectDataCtrl m_DefDataCtrl;

	BOOL m_bReserveClear;

	// 22.05.31 Ahn Add Start
	CImageSaveQueueCtrl* m_pImgSaveQueueCtrl;
	// 22.07.08 Son Mod Start
	//CImageSaveThread* m_pImgSaveThread[GRABBER_COUNT];
	CImageSaveThread* m_pImgSaveThread[IMAGE_SAVE_THREAD];
	// 22.07.08 Son Mod End
	// 22.05.31 Ahn Add End
	// 22.06.10 Ahn Add Start
	CCropImgQueueCtrl *m_pCropImgQueueCtrl;
	// 22.06.10 Ahn Add Start
	// 22.12.09 Ahn Add Start
	CTacTimeDataCtrl* m_pTacTimeDataCtrl;
	// 22.12.09 Ahn Add End

#if defined( IMAGE_DRAW_NOTIFY_VERSION )
	CBitmapStd* m_pLastBmpStd[GRABBER_COUNT];
	CRITICAL_SECTION m_csLastImg[GRABBER_COUNT];
#endif

public :
	int Initialize( HWND hWnd , int nImgProcIdx );
	int Destroy();
	// 22.05.09 Ahn Add Start
	int ResetCamera();
	// 22.05.09 Ahn Add End
	// 22.12.01 Ahn Modify Start
	void SetFrameCount(long lFrameCount);
	// 22.12.01 Ahn Modify End
	// 22.05.25 Ahn Add Start
	int CamResetForMonitorDlg();
	// 22.05.25 Ahn Add End
	BOOL IsInspection();
	BOOL m_bInspFlag;
	int GrabStart(BOOL bMode = FALSE);
	int GrabStart_Monitoring();
	int GrabStop();
	int FrameQueueReset();
	int InspectionStart();
	int InspectionEnd();
	// 22.05.31 Ahn Add Start
	CImageSaveQueueCtrl* GetImageSaveQueuePtr() ;
	// 22.05.31 Ahn Add End
	// 22.06.10 Ahn Add Start
	CCropImgQueueCtrl* GetCropImageQueuePtr() ;
	// 22.06.10 Ahn Add End

	// 22.12.09 Ahn Add Start
	CTacTimeDataCtrl* GetTactDataCtrlPtr();
	// 22.12.09 Ahn Add End

	int LightON();
	int LightOFF();
	void TabCountReset();
	void SetWaveMode(BOOL bMode);
	int SetFrameHeight(int nLength);
	int SetFrameOffset(int nCamNo, int nOffset);
	// 22.05.09 Ahn Add Start
	int SetFrameWidth(int nWidth);
	// 22.05.09 Ahn Add End
	CDefectDataCtrl* GetDefectDataCtrlPtr() { return &m_DefDataCtrl; };

	int GetImagePtr(BYTE** pImagePtr, int* pnWidth, int* pnHeight, int nIndex );
	void SetLastBmpStd(BYTE* pImgPtr, int nWidth, int nHeight, int nHeadNo );
	int GetLastBmpStd(CBitmapStd* pBmpStd, int nHeadNo);

	int SetLineRate(double dLineRate );
	int SetGain(int nCamNo, double dGain);
	int SetExposureTime(double dTime);

	void SetTriggerMode(int nMode);
	int GetTriggerMode();

	double GetLineRate();
	double GetGain();
	double GetExposureTime();

	// 22.07.06 Ahn Add Start
	BOOL IsGrabberRun();
	// 22.07.06 Ahn Add End
	BOOL IsConnected(int nCamNo );
	BOOL IsConnectedAll();
	// 22.05.20 Ahn Add Start
	BOOL GetCameraLinkStatus(int nCamNo);
	// 22.05.20 Ahn Add End
	void SaveErrorData();
	// 22.04.06 Ahn Add Start
	void CounterThreadRun(BOOL bRunFlag);
	// 22.04.06 Ahn Add End

	BOOL GetCameraWaveData(int nCamNo, BYTE* btWaveData);

	CQueueCtrl* GetQueueFrmPtr(int nIndex);
	//CQueueCtrl* GetQueuePtr();
	CQueueCtrl* GetResultPtr(int nHeadNo);

	CCounterQueueCtrl* GetCounterQueInPtr();

	// 21.11.11 Ahn Add Start
	CThreadQueueCtrl* GetThreadQueuePtr(int nHeadNo);
	// 21.11.11 Ahn Add End

	// 22.06.23 Ahn Add Start
	CDefectQueueCtrl* GetDefectQueuePtr();
	// 22.06.23 Ahn Add End

	//CamType : Top 0, Bottom 1 카메라 이미지를 받았을 때
	//FrameCtn : Camera에서 이벤트로 받은 이미지 번호 : Top, Bottom 각각
	void GrabDalsaCameraLink(int CamType, int FrameCnt);

#if defined( IMAGE_DRAW_DIRECT_VERSION )
	void SetImgDispHWnd(HWND pHWnd, int nHeadNo);
	HWND GetImgDispHWnd(int nHeadNo);
	HWND m_pDispHWnd[GRABBER_COUNT];
#endif
	HWND m_MonitorHwnd[GRABBER_COUNT];

};

