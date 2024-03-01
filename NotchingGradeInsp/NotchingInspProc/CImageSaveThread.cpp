#include "pch.h"
#include "CImageSaveThread.h"
#include "ImageProcessCtrl.h"
#include "CImageSaveQueueCtrl.h"
#include "BitmapStd.h"
#include "GlobalData.h"


CImageSaveThread::CImageSaveThread( int nCamNo, CImageProcessCtrl* pParent)
{
	m_nCameraNo = nCamNo;
	m_pParent = pParent;
}
CImageSaveThread::~CImageSaveThread(void)
{

}

void CImageSaveThread::Begin(void)
{
	m_bKill = FALSE;

	//	m_DisphWnd = NULL ;
	if (m_pThread == NULL)
	{
		//이벤트 객체 생성
		pEvent_ImageSaveThread = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadImgSave,
			(LPVOID)this,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);
		if (m_pThread != NULL) {
			m_pThread->m_bAutoDelete = FALSE;
			m_pThread->ResumeThread();
		}
	}
}

void CImageSaveThread::Kill(void)
{
	if (m_pThread)
	{
		setEvent_ImageSaveThread();
		CGlobalFunc::ThreadExit(&m_pThread->m_hThread, 5000);
		m_pThread->m_hThread = NULL;
	}
}

//스래드 타임아웃 시간
#define IMAGESAVETHREAD_TIMEOUT 5
UINT CImageSaveThread::CtrlThreadImgSave(LPVOID pParam)
{
	CImageSaveThread* pThis = (CImageSaveThread*)pParam;
	CImageProcessCtrl* pParent = pThis->m_pParent;
	CImageSaveQueueCtrl* pQueuePtr = pParent->GetImageSaveQueuePtr();

	UINT ret = 0;
	//스래드 대기 여부
	BOOL bThreadWait = TRUE;
	while (1)
	{
		//타임 주기 이벤트
		//대기 상태일 때
		if (bThreadWait)
			ret = WaitForSingleObject(pThis->getEvent_ImageSaveThread(), IMAGESAVETHREAD_TIMEOUT);
		//대기하지 않고 바로 처리한다.
		else
			ret = WAIT_TIMEOUT;

		if (ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			break;
		}
		else if (ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{

			if (pThis == NULL)
			{
				LOGDISPLAY_SPECTXT(0)(_T("CtrlThreadImgSave-1 Thread 종료"));
				break;
			}

			if (pThis->m_bKill == TRUE)
			{
				LOGDISPLAY_SPECTXT(0)(_T("CtrlThreadImgSave-2 Thread 종료"));
				break;
			}

			if (pQueuePtr == NULL)
			{
				LOGDISPLAY_SPECTXT(0)(_T("CtrlThreadImgSave-3 Thread 종료"));
				break;
			}

			while (pQueuePtr->GetSize())
			{

				// Image 하나 가지고 오고 삭제함.
				CImgSaveInfo* pSaveInfo = pQueuePtr->Pop();
				if (pSaveInfo == NULL)
				{
					break;
				}

				if ((pSaveInfo->m_nWidth <= 0) || (pSaveInfo->m_nHeight <= 0))
				{
					//Image Save Log
					LOGDISPLAY_SPECTXT(8)(_T("CtrlThreadImgSave 저장 이미지 정보 : 넓이가 0 또는 높이가 0이다"));
					BYTE* pImgPtr = pSaveInfo->m_pImagePtr;
					if (pImgPtr)
					{
						delete[]pImgPtr;
						pImgPtr = NULL;
					}
					else
					{
						LOGDISPLAY_SPECTXT(8)(_T("CtrlThreadImgSave pImgPtr NULL"));
					}

					if (pSaveInfo)
					{
						delete pSaveInfo;
						pSaveInfo = NULL;
					}
					break;
				}

				if (pSaveInfo->m_strSavePath.GetLength() > 0)
				{

					BYTE* pImgPtr = pSaveInfo->m_pImagePtr;

					if (pImgPtr)
					{
						CBitmapStd bmp(pSaveInfo->m_nWidth, pSaveInfo->m_nHeight, 8);
						bmp.SetImage(pSaveInfo->m_nWidth, pSaveInfo->m_nHeight, pSaveInfo->GetImgPtr());
						// Debug시에 이미지 퀄리티가 계속 저하 되는 것을 방지.

						int nJpgQuality;

						//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
						nJpgQuality = pSaveInfo->m_nJpgQuality;
#else
						if (pQueuePtr->GetSize() > 2)
						{
							nJpgQuality = AprData.m_System.m_nJpegSaveQuality - 10;
						}
						else
						{
							nJpgQuality = AprData.m_System.m_nJpegSaveQuality;
						}
#endif //SPCPLUS_CREATE

						AprData.SaveDebugLog_Format(_T("Image Save Path : %s"),
							pSaveInfo->m_strSavePath);

						bmp.SetJpegQuality(nJpgQuality);

						bmp.SaveBitmap(pSaveInfo->m_strSavePath);

						delete[]pImgPtr;
						pImgPtr = NULL;
					}
					else
					{
						LOGDISPLAY_SPECTXT(8)(_T("CtrlThreadImgSave pImgPtr NULL"));
					}

				}
				if (pSaveInfo)
				{
					delete pSaveInfo;
					pSaveInfo = NULL;
				}
				break;
			}
			//큐에 데이터가 있으면 기다리지 않고 실행하도록 설정
			if (pQueuePtr->GetSize())
				bThreadWait = FALSE;
			//없으면 대기
			else
				bThreadWait = TRUE;
		}
		else
		{
			break;
		}
	}


	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	return 0;
}