#pragma once

#include "sapclassbasic.h"
#include "GrabberCtrl.h"

class CQueueCtrl;

class CCallBackInfo 
{
public :
	SapView *m_SapViewPtr;
	CQueueCtrl* m_pQueuePtr;
	BYTE *m_pWave ;
	int	  m_nWaveWidth;
	BOOL m_bWaveFlag ;
	long m_lFrameCnt;
	int m_nHeadNo;
	BOOL m_bMonitoring;
	BOOL m_bRunFlag;

	CCallBackInfo(SapView* pView, CQueueCtrl* pQuePtr, BYTE *pWave, int nHeadNo, int nWidth ) 
	{
		m_SapViewPtr = pView;
		m_pQueuePtr = pQuePtr;
		m_nHeadNo = nHeadNo;
		m_bMonitoring = FALSE;
		m_pWave = pWave;
		m_nWaveWidth = nWidth;
		m_bRunFlag = FALSE; 
	}
	void SetMonitoringMode(BOOL bMode) { m_bMonitoring = bMode; } ;
	int GetMonitoringMode() { return m_bMonitoring ; } ;
	void SetFrameCount(long lFrameCnt) { m_lFrameCnt = lFrameCnt; };
	void ResetFrameCnt() { m_lFrameCnt = 0; } ;
	long GetFrameCnt() { return m_lFrameCnt; } ;
	long AddFrameCnt() { return m_lFrameCnt++; } ;
	void SetWaveMode(BOOL bMode) { m_bWaveFlag = TRUE; } ;
	BOOL IsRun() { return m_bRunFlag; } ;
	void SetRun(BOOL bRun) { m_bRunFlag = bRun; } ;
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

public:
	//TacTime 출력 변수
	static LARGE_INTEGER stTime;
	static double dFrecuency;

	int Open( HWND hWnd, CQueueCtrl* pQueueFrmPtr, int nServerIdx );
	int Close();
	int GrabPrepare(BOOL bMode);
	int GrabStart( BOOL bMode = FALSE );
	int GrabStop();
	int FreeHandle();
	void SetWaveMode(BOOL bMode);
	BOOL IsRun();

	int GrabberInit();
	void FrameCounterReset();
	int GetImageSize(int* pnWidth, int* pnHeight);

	void SetFrameCount(long lFrameCnt) ;

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

	void SetTriggerMode(int nMode);
	int GetTriggerMode();

	BOOL IsConnected();
	BOOL GetCameraLinkStatus(); 
	BOOL GetGrabberInitStatus(); 
	double GetMeasuredLineRate();
	double GetTemperature();
};

