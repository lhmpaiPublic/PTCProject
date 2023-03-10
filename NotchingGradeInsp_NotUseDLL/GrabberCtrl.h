#pragma once
class CQueueCtrl;

class CGrabberCtrl
{
public:
	CGrabberCtrl(void);
	~CGrabberCtrl(void);

	virtual int Open() { return 0; };
	virtual int Open(HWND hWnd, CQueueCtrl* pQueueFrmPtr, int nGrabberIndex) { return 0; };
	virtual int Close() = 0 ;
	virtual int GrabberInit() = 0 ;

	virtual int GrabStart(BOOL bMode=FALSE) = 0;
	virtual int GrabPrepare(BOOL bMode = FALSE) = 0;
	virtual int GrabStop() = 0 ;
	virtual BOOL IsRun() = 0;

	virtual int GetImageSize(int *pnWidth, int *pnHeight ) = 0 ;
//	virtual BYTE* GetImageBuffer() = 0 ;
//	virtual void SetDispWindow(HWND hWnd) { return TRUE; }; // 22.04.01 Ahn Test
//	virtual void SetZoomRate(float fZoomRateHor, float fZoomRateVer) = 0;
//	virtual int GetZoomRate(float &fZoomRateHor, float &fZoomRateVer) = 0;
//	virtual void SetScrollPos(POINT point)=0;
	virtual BOOL GetImagePtr(BYTE** pImgPtr, int* pWidth, int* pHeight) { return FALSE; };

	virtual BOOL GetBoardReady( void ) = 0 ;

	virtual int SetFrameWidth(int nWidth) {	return 0; };
	virtual int SetFrameHeight(int nLength) { return 0; };
	virtual int SetFrameOffset(int nOffset) { return 0; };
	virtual int SetLineRate(double dLineRate) { return 0; };
	virtual int SetGain(double dGain) { return 0; };
	virtual int SetExposureTime(double dTime) { return 0; };

	virtual double dGetLineRate() { return 0.0; } ;
	virtual	double dGetGain() { return 0.0; };
	virtual double dGetExposureTime() { return 0.0; };
	//virtual void SetDispWindow(HWND hWnd) { return; };

	virtual void SetTriggerMode(int nMode) { return; };
	virtual int GetTriggerMode(){ return 0; };
	virtual BOOL GetCamWaveData(BYTE* btWaveArr) { return TRUE; };
	virtual void SetWaveMode(BOOL bMode) { return; };
	virtual void FrameCounterReset() { return; };
	virtual BOOL IsConnected() { return TRUE ; };

	virtual BOOL GetCameraLinkStatus() { return FALSE; };
	virtual BOOL GetGrabberInitStatus() { return FALSE; };

	virtual double GetMeasuredLineRate() { return 0.0; };
	virtual double GetTemperature() { return 0.0; };

	// 22.12.01 Ahn Add Start
	virtual void SetFrameCount(long lFrameCnt) = 0 ;
	// 22.12.01 Ahn Add End

};
