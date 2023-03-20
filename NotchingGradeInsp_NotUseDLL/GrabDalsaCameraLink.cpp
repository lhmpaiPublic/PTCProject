#include "pch.h"
#include "GrabDalsaCameraLink.h"
#include "QueueCtrl.h"
#include "FrameInfo.h"
#include "GlobalData.h"
// Static Functions
static void AcqCallback(SapXferCallbackInfo* pInfo)
{
	//	SapView* pView = (SapView*)pInfo->GetContext();
	CCallBackInfo* pCbInfo = (CCallBackInfo*)pInfo->GetContext();
	SapView* pView = pCbInfo->m_SapViewPtr;
	CQueueCtrl* pQueueCtrl = pCbInfo->m_pQueuePtr ;
	BYTE* pWave = pCbInfo->m_pWave;
	
	// 21.05.27 Ahn Add Start
	if (pQueueCtrl != NULL){
		int nEventCnt = pInfo->GetEventCount();		
		int nEventType = pInfo->GetEventType();
		
		long nFrameCnt = pCbInfo->AddFrameCnt();
		SapBuffer* pBuffer = pView->GetBuffer();

		int nWidth = pBuffer->GetWidth();
		int nHeight = pBuffer->GetHeight();
		int nCount = pBuffer->GetCount();
		int nIndex = pBuffer->GetIndex();
		SapBuffer::State State1;
		pBuffer->GetState( nIndex, &State1);
		
		BYTE* pData;
		pBuffer->GetAddress(nIndex, (void**)&pData);

		if (pCbInfo->m_bWaveFlag == TRUE) {
			int nCopyW = pCbInfo->m_nWaveWidth  ;
			if (nWidth < pCbInfo->m_nWaveWidth) {
				nCopyW = nWidth;
			}
			memcpy( pWave, pData, sizeof(BYTE) * nCopyW) ;
		}

		if (pCbInfo->GetMonitoringMode() == TRUE) {
			return;
		}

		if (pCbInfo->IsRun() == FALSE) {
			return;
		}

		if (pQueueCtrl != NULL) {
			BYTE* pImg = new BYTE[nWidth * nHeight];
			memcpy(pImg, pData, sizeof(BYTE) * nWidth * nHeight);

			BOOL bSend = FALSE;
			CFrameInfo* pFrmInfo; // 22.01.11 Ahn Modify ( pInfo -> pFrmInfo )
			pFrmInfo = new CFrameInfo;
			pFrmInfo->SetImgPtr(pImg);
			pFrmInfo->m_nHeight = nHeight;
			pFrmInfo->m_nWidth = nWidth;
			pFrmInfo->m_nFrameCount = nFrameCnt;
			pFrmInfo->m_nBand = 1;
			pFrmInfo->m_nHeadNo = pCbInfo->m_nHeadNo - 1;
			pQueueCtrl->PushBack(pFrmInfo);
			bSend = TRUE;

			CString strMsg;
			strMsg.Format(_T("FrameLog Head[%d], Width[%d], Height[%d], FrmCount[%d]"), pFrmInfo->m_nHeadNo, pFrmInfo->m_nWidth, pFrmInfo->m_nHeight, pFrmInfo->m_nFrameCount);
			AprData.SaveFrameLog(strMsg, pFrmInfo->m_nHeadNo );

			if (bSend == FALSE) {
				delete[]pImg;
			}
		}
		//pBuffer->SetState(nIndex, SapBuffer::StateEmpty);
	}		
}

CGrabDalsaCameraLink::CGrabDalsaCameraLink()
{
	m_DispHwnd = NULL ;
	m_pAcq = NULL ;
	m_pAcqDevice = NULL ;
	m_pFeature = NULL ;
	m_pBuffers = NULL ;
	m_pXfer = NULL ;
	m_pView = NULL ;

	m_bInitComplet = FALSE;
	m_bOpen = FALSE;
	m_acqDeviceNumber = -1;

	m_fZoomRateHor = (float)0.1 ;
	m_fZoomRateVer = (float)0.1 ;
	m_bRun = FALSE;
	m_bInit = FALSE;
	m_bCameraLInkComplet = FALSE; // 22.05.20 Ahn Add
	// 22.05.26 Ahn Add Start
	m_nImgWidth = AprData.m_System.m_nCamViewWidth ; 
	m_nImgHeight = AprData.m_System.m_nCamViewHeight ;
	// 22.05.26 Ahn Add End
}

CGrabDalsaCameraLink::~CGrabDalsaCameraLink(void)
{

}
int CGrabDalsaCameraLink::Close()
{
	if (m_pbtWave != nullptr) {
		delete m_pbtWave; 
		m_pbtWave = FALSE;
	}
	FreeHandle();
	return 0;
}

int CGrabDalsaCameraLink::Open( HWND hWnd, CQueueCtrl *pQueueFrmPtr, int nServerIdx )
{
	m_nServerIndex = nServerIdx + 1;
	m_DispHwnd = hWnd;
	m_pQueueFrmPtr = pQueueFrmPtr;
	char acqServerName[CORSERVER_MAX_STRLEN];
	char configFilename[MAX_PATH];
	BOOL isNotSupported = FALSE, status = FALSE, acquisitionCreated = TRUE, acqDeviceCreated = TRUE;

	BOOL serverFound = FALSE;
	CString strMsg;

	int serverCount = SapManager::GetServerCount();
	if (serverCount == 0)
	{
	//	printf("No device found!\n");
		AfxMessageBox("No device found!\n");
		return -1 ;
	}

//	for (int serverIndex = 0; serverIndex < serverCount; serverIndex++){
	if( m_nServerIndex > -1 ){
		if (SapManager::GetResourceCount(m_nServerIndex, SapManager::ResourceAcq) != 0)
		{
			char serverName[CORSERVER_MAX_STRLEN];
			SapManager::GetServerName(m_nServerIndex, serverName, sizeof(serverName));
		//	printf("%d: %s\n", serverIndex, serverName);
			strMsg.Format("%d: %s\n", m_nServerIndex, serverName);
			CorStrncpy(acqServerName, serverName, MAX_PATH);
			TRACE(strMsg);
			serverFound = TRUE;
		}
	}

	if (!serverFound)
	{
	//	printf("No acquisition server found!\n");
		CString strMsg = _T("No acquisition server found!");
	//	AfxMessageBox(strMsg);
		AprData.m_ErrStatus.SetError(CErrorStatus::en_GrabberError, strMsg);
		return -2;
	}

	int deviceCount = SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
	int cameraCount = 0;

	for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
	{
		char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
		SapManager::GetResourceName(acqServerName, SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));
		strMsg.Format("%d: %s\n", deviceIndex + 1, deviceName);
		TRACE(strMsg);
	}

	// List all files in the config directory
	char configPath[MAX_PATH];
	configPath[0] = '\0';
	GetEnvironmentVariable("SAPERADIR", configPath, sizeof(configPath));
	configPath[sizeof(configPath) - 1] = '\0';

	CorStrncat(configPath, "\\CamFiles\\User\\", sizeof(configPath));

	char findPath[MAX_PATH];
	CorStrncpy(findPath, configPath, MAX_PATH);
	CorStrncat(findPath, "CameraSetting.ccf", sizeof(findPath));
	CorStrncpy(configFilename, findPath, sizeof(findPath));

	m_acqDeviceNumber = 0;
	SapLocation loc(acqServerName, m_acqDeviceNumber);

	if (SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq) > 0)
	{
		m_pAcq = new SapAcquisition(loc, configFilename);
		m_pAcqDevice = new SapAcqDevice(acqServerName, 0);

		m_pBuffers = new SapBuffer(2, m_pAcq); 
		m_pView = new SapView(m_pBuffers, m_DispHwnd);

		GrabberInit();

		m_pCallbackInfo = new CCallBackInfo(m_pView, m_pQueueFrmPtr, m_pbtWave, m_nServerIndex, m_nWaveWidth );
		m_pXfer = new SapAcqToBuf(m_pAcq, m_pBuffers, AcqCallback, m_pCallbackInfo);
		// Create acquisition object

		if (m_pAcq && !*m_pAcq && !m_pAcq->Create())
			acquisitionCreated = FALSE;
	}

	// 22.05.20 Ahn Add Start
	CString strLabel;
	strLabel.Format( _T("%s"), m_pAcq->GetLabel() );
	if (strLabel.Find(_T("CameraLink HS Mono")) >= 0){
		// Camera 연결 완료.
		m_bCameraLInkComplet = TRUE;
	}
	else {
		m_bCameraLInkComplet = FALSE;
	}
	// 22.05.20 Ahn Add End

	//// 보드 시리얼 넘버 취득
	//char szSerialNo[MAX_PATH];
	//SapManager::GetServerSerialNumber(nServerIdx+1, szSerialNo);

	// 22.05.17 Ahn Add Start
	int nDrop = 1;
	//m_pAcq->SetParameter(CORACQ_PRM_EXT_LINE_TRIGGER_ENABLE, TRUE);
	m_pAcq->SetParameter(CORACQ_PRM_SHAFT_ENCODER_ENABLE, TRUE);
	m_pAcq->SetParameter(CORVIC_PRM_SHAFT_ENCODER_DROP, nDrop);
	// 22.05.17 Ahn Add End

	// 21.09.29 Ahn Add Start
	m_pAcq->SetParameter(CORACQ_PRM_CROP_WIDTH, m_nImgWidth, TRUE);
	m_pAcq->SetParameter(CORACQ_PRM_CROP_HEIGHT, m_nImgHeight, TRUE);
	m_pAcq->SetParameter(CORACQ_PRM_CROP_LEFT, AprData.m_System.m_nCamViewOffset[m_nServerIndex-1], TRUE);
	//m_pAcq->SetParameter(CORACQ_PRM_SHAFT_ENCODER_SOURCE, 3, TRUE);
	// 21.09.29 Ahn Add End

	// 22.07.25 Ahn Add Start
	m_pAcq->SetParameter(CORACQ_PRM_FLIP, AprData.m_System.m_nCamImageFlip[m_nServerIndex - 1], TRUE);
	// 22.07.25 Ahn Add End

	m_bOpen = TRUE;
	m_bInitComplet = TRUE;
	return 0;
}

int CGrabDalsaCameraLink::GrabberInit()
{
	m_pAcqDevice->Create();
	// 카메라 초기화 시점에 비닝이나 기타 카메라 파라메터를 변경할 경우 풀어서 사용.
	double dLineRate = AprData.m_System.m_dCamLineRate[0];
	double dExposureTime = AprData.m_System.m_dExposureTime[0];

	// m_nServerIndex

	m_pAcqDevice->SetFeatureValue("TriggerMode", AprData.m_System.m_bTriggerMode); // 0 : Internal, 1 : External 
	if (AprData.m_System.m_bTriggerMode == FALSE) {
		m_pAcqDevice->SetFeatureValue("AcquisitionLineRate", dLineRate);
	}

	m_nWaveWidth = AprData.m_System.m_nCamViewWidth;
	m_pbtWave = new BYTE[m_nWaveWidth];
	memset(m_pbtWave, 0, sizeof(BYTE)* m_nWaveWidth);

	m_pAcqDevice->SetFeatureValue("ExposureTime", dExposureTime);
#if defined( CAMERA_TDI_MODE )
	m_pAcqDevice->SetFeatureValue("Gain", AprData.m_System.m_dCamGain[m_nServerIndex - 1]);
#else
	m_pAcqDevice->SetFeatureValue("Gain", 5.0);
#endif
	m_pAcqDevice->SetFeatureValue("BlackLevel", 0);
//#if defined( DBUG_MODE )
//	m_pAcqDevice->SetFeatureValue("TriggerMode", 0); // 0 : Internal, 1 : External 
//#else
//#endif

	return 0;
}
void CGrabDalsaCameraLink::SetTriggerMode(int nMode)
{
	if ((nMode > 1) || (nMode < 0)) {
		AfxMessageBox(_T("입력범위 초과 0~1"));
		return;
	}
	m_pAcqDevice->SetFeatureValue("TriggerMode", nMode); // 0 : Internal, 1 : External 

}
int CGrabDalsaCameraLink::GetTriggerMode()
{
	int nMode = 0;
	if (m_pAcqDevice != NULL) {
		m_pAcqDevice->GetFeatureValue("TriggerMode", &nMode); // 0 : Internal, 1 : External 
	}

	return nMode;
}

void CGrabDalsaCameraLink::FrameCounterReset()
{
	m_pCallbackInfo->ResetFrameCnt();
	return;
}

// 22.12.01 Ahn Add Start
void CGrabDalsaCameraLink::SetFrameCount(long lFrameCnt) 
{ 
	if (m_pCallbackInfo != nullptr) {
		m_pCallbackInfo->SetFrameCount(lFrameCnt);
	}
};
// 22.12.01 Ahn Add End


// 22.01.04 Ahn Add Start
int CGrabDalsaCameraLink::GrabPrepare(BOOL bMode)
{
	if (m_bInit == TRUE) return -1;
	if (m_bInitComplet == FALSE) return -1;
	// Create buffer object

	if (m_pBuffers && !*m_pBuffers && !m_pBuffers->Create())
	{
		FreeHandle();
		return -1;
	}

	// Create transfer object
	if (m_pXfer && !*m_pXfer && !m_pXfer->Create())
	{
		FreeHandle();
		return -2;
	}

	// Create view object
	if (m_DispHwnd != NULL) {
		if (m_pView && !*m_pView && !m_pView->Create())
		{
			FreeHandle();
			return -3;
		}
	}
	// 21.08.25 Ahn Add Start
	m_pCallbackInfo->SetMonitoringMode(bMode);
	m_pView->SetWindow(m_DispHwnd);
	// 21.08.25 Ahn Add Start

	m_pBuffers->ResetIndex(); // 21.12.15 Ahn Add 

	m_bInit = TRUE;
	return 0;
}
// 22.01.04 Ahn Add Start

// 22.07.06 Ahn Add Start
BOOL CGrabDalsaCameraLink::IsRun()
{
	return m_bRun;
}
// 22.07.06 Ahn Add End

// 21.08.25 Ahn Add Start
//int CGrabDalsaCameraLink::GrabStart()
int CGrabDalsaCameraLink::GrabStart(BOOL bMode /*bMonitoringMode=FALSE*/)
// 21.08.25 Ahn Add Start
{

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Image) == TRUE) {
		return 2;
	}

	if (m_bRun == TRUE) {
		return -1;
	}

	// Start continous grab
	m_bRun = TRUE;
	m_pCallbackInfo->SetRun( TRUE );	// 22.01.12 Ahn Add

	m_pXfer->Grab();

	return 0;
}

int CGrabDalsaCameraLink::GrabStop()
{
	if (m_bInitComplet == FALSE) return 0;

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Image) == TRUE) {
		return 2;
	}

	m_bInit = FALSE;
	m_bRun = FALSE;
	m_pCallbackInfo->SetRun( FALSE ) ;	// 22.01.12 Ahn Add

	BOOL bConntected = m_pXfer->IsConnected();
	if (bConntected == TRUE) {
		m_pXfer->Freeze();
		//m_pBuffers->Clear(); // 21.12.15 Ahn Add
		if (!m_pXfer->Wait(5000))
		{
			TRACE("Grab could not stop properly.\n");
			m_pXfer->Abort();
		}
	}

	return 0;
}

int CGrabDalsaCameraLink::FreeHandle()
{
	//end program
//	printf("Press any key to terminate\n");

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Image) == TRUE) {
		return 2;
	}

	m_bInitComplet = FALSE;

	// Destroy view object
	if (m_pView && *m_pView && !m_pView->Destroy()) return FALSE;

	// Destroy transfer object
	if (m_pXfer && *m_pXfer && !m_pXfer->Destroy()) return FALSE;

	// Destroy buffer object
	if (m_pBuffers && *m_pBuffers && !m_pBuffers->Destroy()) return FALSE;

	// Destroy acquisition object
	if (m_pAcq && *m_pAcq && !m_pAcq->Destroy()) return FALSE;

	// Destroy feature object
	if (m_pFeature && *m_pFeature && !m_pFeature->Destroy()) return FALSE;

	// Destroy acquisition object
	if (m_pAcqDevice && *m_pAcqDevice && !m_pAcqDevice->Destroy()) return FALSE;

	// Delete all objects
	if (m_pView)		delete m_pView;
	if (m_pXfer)		delete m_pXfer;
	if (m_pBuffers)		delete m_pBuffers;
	if (m_pAcq)			delete m_pAcq;
	if (m_pFeature)		delete m_pFeature;
	if (m_pAcqDevice)	delete m_pAcqDevice;
	if (m_pCallbackInfo) delete m_pCallbackInfo;

	return 0;
}

int CGrabDalsaCameraLink::GetImageSize(int* pnWidth, int* pnHeight)
{
//	*pnWidth = 
	return 0;
}


BOOL CGrabDalsaCameraLink::GetBoardReady(void)
{
	return m_bInitComplet;
}

void CGrabDalsaCameraLink::SetZoomRateHor(float fZoomRate)
{
	m_fZoomRateHor = fZoomRate;
	m_pView->SetScalingMode(m_fZoomRateHor, m_fZoomRateVer);

}
void CGrabDalsaCameraLink::SetZoomRateVer(float fZoomRate)
{
	m_fZoomRateVer = fZoomRate;
	m_pView->SetScalingMode(m_fZoomRateHor, m_fZoomRateVer);

}

int CGrabDalsaCameraLink::GetZoomRate(float& fZoomRateHor, float& fZoomRateVer)
{
	fZoomRateHor = m_fZoomRateHor;
	fZoomRateVer = m_fZoomRateVer;

	return 0;
}


void CGrabDalsaCameraLink::SetZoomRate(float fZoomRateHor, float fZoomRateVer)
{
	m_fZoomRateHor = fZoomRateHor;
	m_fZoomRateVer = fZoomRateVer;
	
	m_pView->SetScalingMode(m_fZoomRateHor, m_fZoomRateVer);
}

int CGrabDalsaCameraLink::GetImageWidth()
{
	return m_nImgWidth;
}
int CGrabDalsaCameraLink::GetImageHeight()
{
	return m_nImgHeight;
}

void CGrabDalsaCameraLink::SetScrollPos(POINT point)
{
	SIZE range = m_pView->GetScrollRange();
	if (point.x < (int)range.cx)
	{
		POINT pnt = m_pView->GetScrollPos();
		m_pView->OnHScroll(pnt.x + point.x );
	}
	if (point.y < (int)range.cy)
	{
		POINT pnt = m_pView->GetScrollPos();
		m_pView->OnVScroll(pnt.y + point.y);
	}
}

int CGrabDalsaCameraLink::GetImagePtr(BYTE** pImgPtr, int* pnWidth, int* pnHeight)
{
	BOOL bRet = 0;

	if (m_pView == NULL) return -1;

	SapBuffer* pBuffer = m_pView->GetBuffer();

	if (pBuffer == NULL) return -2;
	int nWidth = pBuffer->GetWidth();
	int nHeight = pBuffer->GetHeight();
	int nCount = pBuffer->GetCount();
	int nIndex = pBuffer->GetIndex();
	SapBuffer::State State1;

	if ((pnWidth == NULL) || (pnHeight == NULL)) {
		return -3;
	}

	*pnWidth = nWidth;
	*pnHeight = nHeight;

	BYTE* pData;

	pBuffer->GetState(nIndex, &State1);
	pBuffer->GetAddress(nIndex, (void**)&pData);

	*pImgPtr = new BYTE[nWidth * nHeight];
	memcpy(*pImgPtr, pData, sizeof(BYTE) * nWidth * nHeight);

	return 0;
}


int CGrabDalsaCameraLink::SetFrameHeight( int nLength )
{
	int nRet = 0;

	nRet = (int)m_pAcq->SetParameter(CORACQ_PRM_CROP_HEIGHT, nLength, TRUE);

	if (nRet == 0) {
		m_nImgHeight = nLength;
	}
	return nRet;
}
int CGrabDalsaCameraLink::SetFrameWidth(int nWidth)
{
	int nRet = 0;

	nRet = (int)m_pAcq->SetParameter(CORACQ_PRM_CROP_WIDTH, nWidth, TRUE);
	
	if (nRet == 0) {
		m_nImgWidth = nWidth;
	}
	return nRet;
}

int CGrabDalsaCameraLink::SetFrameOffset(int nOffset)
{
	int nRet = 0;

	m_pAcq->SetParameter(CORACQ_PRM_CROP_LEFT, nOffset, TRUE);

	return nRet;
}

int CGrabDalsaCameraLink::SetLineRate(double dLineRate)
{
	int nRet = 0;
	nRet = m_pAcqDevice->SetFeatureValue("AcquisitionLineRate", dLineRate) ;
	return nRet;
}
int CGrabDalsaCameraLink::SetGain(double dGain)
{
	int nRet = 0;
	nRet = m_pAcqDevice->SetFeatureValue("Gain", dGain );
	return nRet;
}
int CGrabDalsaCameraLink::SetExposureTime(double dTime)
{
	int nRet = 0;
	nRet = m_pAcqDevice->SetFeatureValue("ExposureTime", dTime);
	return nRet;
}

double CGrabDalsaCameraLink::dGetLineRate()
{
	double dLineRate = 0.0;
	m_pAcqDevice->GetFeatureValue("AcquisitionLineRate", &dLineRate);
	return dLineRate;
}
double CGrabDalsaCameraLink::dGetGain()
{
	double dGain = 0.0;
	m_pAcqDevice->GetFeatureValue("Gain", &dGain);
	return dGain;
}
double CGrabDalsaCameraLink::dGetExposureTime()
{
	double dTime = 0.0;
	m_pAcqDevice->GetFeatureValue("ExposureTime", &dTime);

	return dTime;
}

BOOL CGrabDalsaCameraLink::GetCamWaveData(BYTE* btWaveArr)
{
	memcpy(btWaveArr, m_pbtWave, sizeof(BYTE) * m_nWaveWidth );

	return TRUE;
}

void CGrabDalsaCameraLink::SetWaveMode(BOOL bMode) 
{ 
	if (m_pCallbackInfo != NULL) {
		m_pCallbackInfo->m_bWaveFlag = bMode;
	}
};

BOOL CGrabDalsaCameraLink::IsConnected()
{

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Image) == TRUE) {
		return 2;
	}

	if (m_pXfer != NULL) {
		return m_pXfer->IsConnected();
	}

	return FALSE;
}

BOOL CGrabDalsaCameraLink::GetCameraLinkStatus()
{
	return m_bCameraLInkComplet;
}

BOOL CGrabDalsaCameraLink::GetGrabberInitStatus()
{
	return m_bInitComplet;
}


// 22.06.03 Ahn Add Start
double CGrabDalsaCameraLink::GetMeasuredLineRate()
{
	double dRet = 0.0;
	//this->m_pAcqDevice
	return dRet;
}
double CGrabDalsaCameraLink::GetTemperature()
{
	double dRet = 0.0;

	return dRet;
}
// 22.06.03 Ahn Add End
