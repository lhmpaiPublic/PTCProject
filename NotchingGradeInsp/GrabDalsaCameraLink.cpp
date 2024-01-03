#include "pch.h"
#include "GrabDalsaCameraLink.h"
#include "QueueCtrl.h"
#include "FrameInfo.h"
#include "GlobalData.h"
#include "SigProc.h"
#include "ImageProcessCtrl.h"
#include "GlobalFunc.h"

CImageProcessCtrl* CGrabDalsaCameraLink::m_pImageProcessCtrl = NULL;

//프레임 처리시간 객체를 세팅한다.
#include "ImageProcThread.h"

int nImageNoneExit = 0;
static void AcqCallback(SapXferCallbackInfo* pInfo)
{
	//	SapView* pView = (SapView*)pInfo->GetContext();
	CCallBackInfo* pCbInfo = (CCallBackInfo*)pInfo->GetContext();
	SapView* pView = pCbInfo->m_SapViewPtr;
	CQueueCtrl* pQueueCtrl = pCbInfo->m_pQueuePtr ;
	BYTE* pWave = pCbInfo->m_pWave;

	bool bImageExit = false;

	if ((pCbInfo == NULL) || (pView == NULL) || (pQueueCtrl == NULL) || (pWave == NULL))
	{
		//Log Camera Setting
		LOGDISPLAY_SPECTXT(8)(_T("@@@ Cam Error** CGrabDalsaCameraLink AcqCallback함수 Object Pointer Receive 오류"));
		return;
	}

	// Cam Image Queue 제어 객체
	if (pQueueCtrl != NULL)
	{
		int nEventCnt = pInfo->GetEventCount();		
		int nEventType = pInfo->GetEventType();
		
		long nFrameCnt = pCbInfo->AddFrameCnt();

		//SapXferNode 상속 객체
		//Image 정보 객체
		SapBuffer* pBuffer = pView->GetBuffer();

		if (pBuffer == NULL)
		{
			//Log Camera Setting
			LOGDISPLAY_SPEC(8)(_T("@@@ Cam Error** CGrabDalsaCameraLink Buffer 오류 "));
			return;
		}

		int nWidth = pBuffer->GetWidth();
		int nHeight = pBuffer->GetHeight();
		int nCount = pBuffer->GetCount();
		int nIndex = pBuffer->GetIndex();
		SapBuffer::State State1;
		pBuffer->GetState( nIndex, &State1);
		
		BYTE* pData;
		pBuffer->GetAddress(nIndex, (void**)&pData);

		if (pCbInfo->m_bWaveFlag == TRUE) 
		{
			int nCopyW = pCbInfo->m_nWaveWidth  ;
			if (nWidth < pCbInfo->m_nWaveWidth) {
				nCopyW = nWidth;
			}
			CopyMemory( pWave, pData, sizeof(BYTE) * nCopyW) ;
		}

		if (pCbInfo->GetMonitoringMode() == TRUE) 
		{
			//Log Camera Setting
			LOGDISPLAY_SPECTXT(8)(_T("@@@vCam Error** CGrabDalsaCameraLink AcqCallback함수 Monitering TRUE 오류"));

			//return;
		}

		if (pCbInfo->IsRun() == FALSE) 
		{
			//Log Camera Setting
			LOGDISPLAY_SPECTXT(8)(_T("@@@vCam Error** CGrabDalsaCameraLink AcqCallback함수 Not Run Image Get"));
		}

#ifdef USE_PLCCONNECTZONE

		CSigProc* pSigProc = theApp.m_pSigProc;
		if ((pSigProc != NULL) && (pSigProc->GetConnectZone() == TRUE))
		{
			//Log Camera Setting
			LOGDISPLAY_SPECTXT(0)(_T("**Cam Error** CGrabDalsaCameraLink AcqCallback함수 PLC Connect Zone 상태 - 이미지 무시한다."));
			AprData.SaveDebugLog_Format(_T("<AcqCallback> Connect Zone TURE") );

			return;
		}

#endif //USE_PLCCONNECTZONE

		if ( pQueueCtrl != NULL )
		{
			//이미지 저장 버퍼 생성
			FrameImagePtr* pImg = new FrameImagePtr();
			memset(pImg->m_pImagePtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
			CopyMemory(pImg->m_pImagePtr, pData, sizeof(BYTE) * nWidth * nHeight);

			//프레임 정보 객체 생성
			CFrameInfo* pFrmInfo; // 22.01.11 Ahn Modify ( pInfo -> pFrmInfo )
			pFrmInfo = new CFrameInfo;

			if (pImg && pFrmInfo)
			{
				pFrmInfo->SetImgPtr(pImg);
				pFrmInfo->m_nHeight = nHeight;
				pFrmInfo->m_nWidth = nWidth;
				pFrmInfo->m_nFrameCount = nFrameCnt;
				pFrmInfo->m_nBand = 1;
				pFrmInfo->m_nHeadNo = pCbInfo->m_nHeadNo - 1;

				CString strMsg = "";
				strMsg.Format(_T("FrameLog Head[%d], Width[%d], Height[%d], FrmCount[%d]"), pFrmInfo->m_nHeadNo, pFrmInfo->m_nWidth, pFrmInfo->m_nHeight, pFrmInfo->m_nFrameCount);
				AprData.SaveFrameLog(strMsg, pFrmInfo->m_nHeadNo);

				//메모리 로그 기록
				strMsg = "";
				strMsg.Format(_T("FrameLog Head[%s], FrmNum[%d]"), (pFrmInfo->m_nHeadNo == CAM_POS_TOP) ? "TOP":"BOTTOM", pFrmInfo->m_nFrameCount);
				AprData.SaveMemoryLog(strMsg);

				//Image Capture 정보 출력 로그
				LOGDISPLAY_SPEC(8)(_T("@@@ Cam Pos[%s], Width[%d], Height[%d], FrmCount[%d]"), (pFrmInfo->m_nHeadNo == 0) ? "TOP" : "BOTTOM", pFrmInfo->m_nWidth, pFrmInfo->m_nHeight, pFrmInfo->m_nFrameCount);

				//얻은 이미지 정보를 TabFind 스래드로 전달하기 위해 queue 에 넣는다.
				pQueueCtrl->PushBack(pFrmInfo);

				theApp.m_nImageProcGrabberImageGet = GetTickCount();

				//이미지 Top Bottom 모두 받으면 TabFind 스래드가 실행하도록 이벤트를 발생한다.
				CGrabDalsaCameraLink::m_pImageProcessCtrl->EventGrabDalsaCameraLink(pFrmInfo->m_nHeadNo, pFrmInfo->m_nFrameCount);


				bImageExit = true;
			}
		}
		
	}	

	if (bImageExit == false)
	{
		++nImageNoneExit;
		//Image Capture 정보 출력 로그
		LOGDISPLAY_SPEC(1)(_T("Grab == Error : Image 정보가 없다. <%d>"), nImageNoneExit);
	}
}

CGrabDalsaCameraLink::CGrabDalsaCameraLink(CImageProcessCtrl* pImageProcessCtrl)
{
	//부모객체 생성 포인터
	m_pImageProcessCtrl = pImageProcessCtrl;

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
	//Dalsa index 는 저장객체 index +1 1부터 시작
	m_nServerIndex = nServerIdx + 1;
	m_DispHwnd = hWnd;

	//메모리가 할당된 저장개체의 포인터
	m_pQueueFrmPtr = pQueueFrmPtr;

	char acqServerName[CORSERVER_MAX_STRLEN];
	char configFilename[MAX_PATH];
	BOOL isNotSupported = FALSE, status = FALSE, acquisitionCreated = TRUE, acqDeviceCreated = TRUE;

	BOOL serverFound = FALSE;
	CString strMsg;

	//Dalsa 서버 갯수를 가져온다.
	int serverCount = SapManager::GetServerCount();

	//서버의 갯수가 없으면 에러 
	if (serverCount == 0)
	{
	//	printf("No device found!\n");
		AfxMessageBox("No device found!\n");
		return -1 ;
	}

//	for (int serverIndex = 0; serverIndex < serverCount; serverIndex++){
	if( m_nServerIndex > -1 ){
		//Server index 1 : Top, 2 : Bottom
		//해당 인덱스 리소스  가져온다.
		if (SapManager::GetResourceCount(m_nServerIndex, SapManager::ResourceAcq) != 0)
		{
			//인덱스 리소스 서버 명을 가져온다.
			char serverName[CORSERVER_MAX_STRLEN];
			SapManager::GetServerName(m_nServerIndex, serverName, sizeof(serverName));

		//	printf("%d: %s\n", serverIndex, serverName);
			strMsg.Format(_T("%d: %s\n"), m_nServerIndex, serverName);

			//서버명 복사
			CorStrncpy(acqServerName, serverName, MAX_PATH);

			//디버그 출력
			TRACE(strMsg);
			serverFound = TRUE;

			//Log Camera Setting
			LOGDISPLAY_SPEC(0)(_T("**Cam Server ** serverIndex = %d, serverName = %s "), m_nServerIndex, serverName);
		}
	}

	//서버를 못찾았으면 실패 에러출력
	if (!serverFound)
	{
	//	printf("No acquisition server found!\n");
		CString strMsg = _T("No acquisition server found!");
	//	AfxMessageBox(strMsg);
		AprData.m_ErrStatus.SetError(CErrorStatus::en_GrabberError, strMsg);

		//Log Camera Setting
		LOGDISPLAY_SPECTXT(0)(_T("**Cam Error** CGrabDalsaCameraLink Server Open 실패 "));

		return -2;
	}

	//찾은 서버의 디바이스 리소스 갯수 가져온다.
	int deviceCount = SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);

	//device 의 이름을 가져와서 출력한다.
	for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
	{
		//디바이스 명을 받을 객체
		char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
		//디버이스명을 가져온다.
		SapManager::GetResourceName(acqServerName, SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));
		strMsg.Format(_T("%d: %s\n"), deviceIndex + 1, deviceName);
		TRACE(strMsg);

		//Log Camera Setting
		LOGDISPLAY_SPEC(0)(_T("** %s Server Cam Device ** deviceIndex = %d, deviceName = %s "), acqServerName, deviceIndex, deviceName);

	}

	// List all files in the config directory
	char configPath[MAX_PATH];
	configPath[0] = '\0';

	//환경변수의 SAPERADIR 경로를 가져온다.
	GetEnvironmentVariable("SAPERADIR", configPath, sizeof(configPath));
	configPath[sizeof(configPath) - 1] = '\0';

	//Log Camera Setting
	LOGDISPLAY_SPEC(0)(_T("CGrabDalsaCameraLink <SAPERADIR 환경변수 Path> = <%s>"),
		configPath);

	//환경변수의 경로에 \\CamFiles\\User\\ 붙인다. configPath 버퍼크기 오버되지 않게 한다.
	CorStrncat(configPath, "\\CamFiles\\User\\", sizeof(configPath));

	//찾은 경로에 CameraSetting.ccf 을 붙인다.
	//C:\Program Files\Teledyne DALSA\Sapera\CamFiles\user\CameraSetting.ccf
	char findPath[MAX_PATH];
	CorStrncpy(findPath, configPath, MAX_PATH);
	CorStrncat(findPath, "CameraSetting.ccf", sizeof(findPath));

	//CameraSetting.ccf 명을 붙인 경로 복사
	CorStrncpy(configFilename, findPath, sizeof(findPath));

	//Device Numver
	m_acqDeviceNumber = 0;
	//Sap 위치, 버서명과 디버이스 번호 0 세팅 SapLocation 임시변수
	SapLocation loc(acqServerName, m_acqDeviceNumber);

	//서버명에 해당하는 리소스가 있으면 ?
	if (SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq) > 0)
	{
		//위치에 해당하는 객체 생성
		//Camera Settting 파일 경로로 객체 생성 CameraSetting.ccf 파일을 읽어서 생성
		m_pAcq = new SapAcquisition(loc, configFilename);

		//디바이스 저장 객체 생성
		m_pAcqDevice = new SapAcqDevice(acqServerName, 0);

		//버퍼 객체 생성
		m_pBuffers = new SapBuffer(10/*2*/, m_pAcq); // double buffering 

		//View 객체 생성
		m_pView = new SapView(m_pBuffers, m_DispHwnd);

		//찾은 디버이스에 해당하는 객체를 초기화
		GrabberInit();

		//콜백을 위한 객채생성
		//View 객체, 큐 저장 버퍼, pbt Wave, 서버인덱스, Wave 넓이
		//콜백함수 AcqCallback 의 SapXferCallbackInfo 포인터 pContext을 받는다.
		m_pCallbackInfo = new CCallBackInfo(m_pView, m_pQueueFrmPtr, m_pbtWave, m_nServerIndex, m_nWaveWidth );

		//콜백함수 : AcqCallback
		//m_pCallbackInfo 콜백 정보
		m_pXfer = new SapAcqToBuf(m_pAcq, m_pBuffers, AcqCallback, m_pCallbackInfo);
		// Create acquisition object

		//SapAcquisition 객체 포인터와 값 Create(생성) 중 하나라도 실했으면 생성 실패
		if (m_pAcq && !*m_pAcq && !m_pAcq->Create())
			acquisitionCreated = FALSE;
	}

	// 22.05.20 Ahn Add Start
	//SapAcquisition Label 이름을 비교해서 성공여부 확인
	CString strLabel;
	strLabel.Format( _T("%s"), m_pAcq->GetLabel() );
	//CameraLink HS Mono 텍스트을 가지고 왔으면 성공
	if (strLabel.Find(_T("CameraLink HS Mono")) >= 0){
		// Camera 연결 완료.
		m_bCameraLInkComplet = TRUE;

		//Log Camera Setting
		LOGDISPLAY_SPECTXT(0)(_T("CGrabDalsaCameraLink CameraLink 성공"));
	}
	//아니면 실패
	else {
		m_bCameraLInkComplet = FALSE;

		//Log Camera Setting
		LOGDISPLAY_SPECTXT(0)(_T("CGrabDalsaCameraLink CameraLink 실패"));
	}
	// 22.05.20 Ahn Add End

	//// 보드 시리얼 넘버 취득
	//char szSerialNo[MAX_PATH];
	//SapManager::GetServerSerialNumber(nServerIdx+1, szSerialNo);

	// 22.05.17 Ahn Add Start
	int nDrop = 1;
	//m_pAcq->SetParameter(CORACQ_PRM_EXT_LINE_TRIGGER_ENABLE, TRUE);
	
	//파라메타 설정 CORACQ_PRM_SHAFT_ENCODER_ENABLE TRUE
	m_pAcq->SetParameter(CORACQ_PRM_SHAFT_ENCODER_ENABLE, TRUE);
	//파라메타 CORVIC_PRM_SHAFT_ENCODER_DROP Drop 1
	m_pAcq->SetParameter(CORVIC_PRM_SHAFT_ENCODER_DROP, nDrop);
	// 22.05.17 Ahn Add End

	// 21.09.29 Ahn Add Start
	// COR ACQ 번호에 해당하는 파라미터 정의 B.2 VIC Related Parameters
	//파라메터 CORACQ_PRM_CROP_WIDTH 넓이 TRUE
	m_pAcq->SetParameter(CORACQ_PRM_CROP_WIDTH, m_nImgWidth, TRUE);
	//파라메터 CORACQ_PRM_CROP_WIDTH 높이 TRUE
	m_pAcq->SetParameter(CORACQ_PRM_CROP_HEIGHT, m_nImgHeight, TRUE);

	//파라메타 LEFT 카메라 OFFSET 설정
	m_pAcq->SetParameter(CORACQ_PRM_CROP_LEFT, AprData.m_System.m_nCamViewOffset[m_nServerIndex-1], TRUE);
	//m_pAcq->SetParameter(CORACQ_PRM_SHAFT_ENCODER_SOURCE, 3, TRUE);
	// 21.09.29 Ahn Add End

	// 22.07.25 Ahn Add Start
	//FLIP  파라메터 설정
	m_pAcq->SetParameter(CORACQ_PRM_FLIP, AprData.m_System.m_nCamImageFlip[m_nServerIndex - 1], TRUE);
	// 22.07.25 Ahn Add End

	//파라메타를 모두 설정하고 OPEN 성공
	m_bOpen = TRUE;

	//초기화 완료 여부 확인
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
	m_pbtWave = new BYTE[m_nWaveWidth + 1];
	memset(m_pbtWave, 0, sizeof(BYTE)* m_nWaveWidth + 1);

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

	//Log Camera Setting
	LOGDISPLAY_SPECTXT(0)(_T("GrabDalsaCameraLink_FreeHandle(메모리 해제)"));

	if (AprData.m_DebugSet.GetDebug(CDebugSet::en_Debug_Image) == TRUE) {

		return 2;
	}

	//객체 메모리 해제 시 초기화 초기화
	m_bInitComplet = FALSE;

	// Destroy view object
	if (m_pView && *m_pView && !m_pView->Destroy())
	{
		return FALSE;
	}

	// Destroy transfer object
	if (m_pXfer && *m_pXfer && !m_pXfer->Destroy())
	{
		return FALSE;
	}

	// Destroy buffer object
	if (m_pBuffers && *m_pBuffers && !m_pBuffers->Destroy()) 
	{
		return FALSE;
	}

	// Destroy acquisition object
	if (m_pAcq && *m_pAcq && !m_pAcq->Destroy())
	{
		return FALSE;
	}

	// Destroy feature object
	if (m_pFeature && *m_pFeature && !m_pFeature->Destroy())
	{
		return FALSE;
	}

	// Destroy acquisition object
	if (m_pAcqDevice && *m_pAcqDevice && !m_pAcqDevice->Destroy())
	{
		return FALSE;
	}

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

	if (m_pView == NULL)
	{
		return -1;
	}

	SapBuffer* pBuffer = m_pView->GetBuffer();

	if (pBuffer == NULL)
	{
		//Log Camera Setting
		LOGDISPLAY_SPECTXT(0)(_T("GrabDalsaCameraLink_GetImage_Error : SapBuffer NULL"));
		return -2;
	}
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

	*pImgPtr = new BYTE[nWidth * nHeight + 1];
	memset(*pImgPtr, 0x00, sizeof(BYTE) * nWidth * nHeight + 1);
	CopyMemory(*pImgPtr, pData, sizeof(BYTE) * nWidth * nHeight);

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
	CopyMemory(btWaveArr, m_pbtWave, sizeof(BYTE) * m_nWaveWidth );

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
