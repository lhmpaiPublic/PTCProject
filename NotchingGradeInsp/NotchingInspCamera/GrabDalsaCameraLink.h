#pragma once

#include "sapclassbasic.h"
#include "GrabberCtrl.h"

class CQueueCtrl;

class CCallBackInfo {
public :
	SapView *m_SapViewPtr;
	CQueueCtrl* m_pQueuePtr;
	BYTE *m_pWave ;
	int	  m_nWaveWidth;
	BOOL m_bWaveFlag ;
	long m_lFrameCnt;
	int m_nHeadNo;
	BOOL m_bMonitoring;
	// 22.01.12 Ahn Add Start
	BOOL m_bRunFlag;
	// 22.01.12 Ahn Add End
	CCallBackInfo(SapView* pView, CQueueCtrl* pQuePtr, BYTE *pWave, int nHeadNo, int nWidth ) {
		m_SapViewPtr = pView;
		m_pQueuePtr = pQuePtr;
		m_nHeadNo = nHeadNo;
		m_bMonitoring = FALSE;
		m_pWave = pWave;
		m_nWaveWidth = nWidth;
		m_bRunFlag = FALSE; // 22.01.12 Ahn Add
	}
	void SetMonitoringMode(BOOL bMode) { m_bMonitoring = bMode; } ;
	int GetMonitoringMode() { return m_bMonitoring ; } ;
	// 22.12.01 Ahn Add Start
	void SetFrameCount(long lFrameCnt) { m_lFrameCnt = lFrameCnt; };
	// 22.12.01 Ahn Add End
	void ResetFrameCnt() { m_lFrameCnt = 0; } ;
	long GetFrameCnt() { return m_lFrameCnt; } ;
	long AddFrameCnt() { return m_lFrameCnt++; } ;
	void SetWaveMode(BOOL bMode) { m_bWaveFlag = TRUE; } ;
	BOOL IsRun() { return m_bRunFlag; } ;
	void SetRun(BOOL bRun) { m_bRunFlag = bRun; } ;
};

//Grab Call 정보
class CGrabFrameInfo
{
public:
	CGrabFrameInfo()
	{
		//Grab Frame Count
		m_nGrabFrmCount = -1;
		//Grab 시 제일 마지막에 받은 BCD ID
		//이전 BCD ID와 같다면 -1
		m_nGrabFrmBCDID = -1;
		//Grab 시 총 Pixel 수
		m_nGrabFrmPixelCount = 0;
	}
	//Grab Frame Count
	int m_nGrabFrmCount;
	//Grab 시 제일 마지막에 받은 BCD ID
	//이전 BCD ID와 같다면 -1
	int m_nGrabFrmBCDID;
	//Grab 시 총 Pixel 수
	UINT64 m_nGrabFrmPixelCount;
};
class CImageProcessCtrl;
class CGrabDalsaCameraLink :public CGrabberCtrl
{
public :
	CGrabDalsaCameraLink(CImageProcessCtrl* pImageProcessCtrl);
	~CGrabDalsaCameraLink();

	//콜백에서 사용할 static 전역 객체
	static CImageProcessCtrl* m_pImageProcessCtrl;
private :
	HWND m_DispHwnd;

	SapAcquisition* m_pAcq ;
	SapAcqDevice*	m_pAcqDevice ;
	SapFeature*		m_pFeature ;
	SapBuffer*		m_pBuffers ;
	SapTransfer*	m_pXfer ;
	SapView*		m_pView ;
	CCallBackInfo *	m_pCallbackInfo ;
	CQueueCtrl*		m_pQueueFrmPtr;
	BYTE*			m_pbtWave;
	int				m_nWaveWidth;
	UINT32			m_acqDeviceNumber;
	BOOL			m_bInitComplet;
	BOOL			m_bOpen;
	BOOL			m_bRun;
	BOOL			m_bInit;
	int				m_nServerIndex ;
	BOOL			m_bCameraLInkComplet;

	int				m_nImgWidth;
	int				m_nImgHeight;
	long			m_lStartFrameNo;

	//Grab 시 정보
public:
	//Grab 이미지 취득 시 BCD ID 와 Frame 에 대한 얻은 픽셀의 수를 누적한다.
	static std::vector<CGrabFrameInfo*> m_GrabFrameInfo;

	//Grab Frame Info 동기화
	static CRITICAL_SECTION m_csGrabFrameInfo;
	//Grab 정보 저장
	static void pushGrabFrameInfo(CGrabFrameInfo* info);
	//Grab 정보 가져오기
	static CGrabFrameInfo* popGrabFrameInfo(int nFrameCount);
	//Grab Height Pixel Total Count
	static UINT64 m_GrabPixelHeightTotalCount;
	//Cell Make Height Pixel Total Count
	static UINT64 m_CellMakePixelHeightTotalCount;
public:
	//TacTime 출력 변수
	static LARGE_INTEGER stTime;
	static double dFrecuency;

	int Open( HWND hWnd, CQueueCtrl* pQueueFrmPtr, int nServerIdx );
	int Close();
	// 22.01.04 Ahn Add Start
	int GrabPrepare(BOOL bMode);
	// 22.01.04 Ahn Add End
	// 21.08.25 Ahn Modify Start
	int GrabStart( BOOL bMode = FALSE );
	// 21.08.25 Ahn Modify Start
	int GrabStop();
	int FreeHandle();
	void SetWaveMode(BOOL bMode);
	// 22.07.06 Ahn Add Start
	BOOL IsRun();
	// 22.07.06 Ahn Add End

	int GrabberInit();
	void FrameCounterReset();
	int GetImageSize(int* pnWidth, int* pnHeight);
	//BOOL SetDispWindow(HWND hWnd, int nDispNo);
	//BYTE* GetImageBuffer();
	//BOOL CopyImageFromBuffer(BYTE* pImage);

	// 22.12.01 Ahn Add Start
	void SetFrameCount(long lFrameCnt) ;
	// 22.12.01 Ahn Add End

	BOOL GetBoardReady(void);
	BOOL GetCamWaveData( BYTE *btWaveArr);

	float m_fZoomRateVer;
	float m_fZoomRateHor;

	void SetZoomRate(float fZoomRateHor, float fZoomRateVer);
	void SetZoomRateHor(float fZoomRate);
	void SetZoomRateVer(float fZoomRate);

	int GetZoomRate(float& fZoomRateHor, float& fZoomRateVer);

	void SetScrollPos(POINT point);
	BOOL GetImagePtr(BYTE** pImgPtr, int* pWidth, int* pHeight);

	int GetImageWidth();
	int GetImageHeight();
	int SetFrameHeight(int nLength);
	int SetFrameWidth(int nWidth);
	int SetFrameOffset(int nOffset);

	int SetLineRate(double dLineRate) ;
	int SetGain(double dGain) ;
	int SetExposureTime(double dTime) ;

	double dGetLineRate();
	double dGetGain();
	double dGetExposureTime();

	//void SetDispWindow(HWND hWnd) {	m_DispHwnd = hWnd; }; // 22.04.01 Ahn Test

	void SetTriggerMode(int nMode);
	int GetTriggerMode();

	BOOL IsConnected();
	// 22.05.20 Ahn Add Start
	BOOL GetCameraLinkStatus(); 
	BOOL GetGrabberInitStatus(); 
	// 22.05.20 Ahn Add End
	// 22.06.03 Ahn Add Start
	double GetMeasuredLineRate();
	double GetTemperature();
	// 22.06.03 Ahn Add End
};

