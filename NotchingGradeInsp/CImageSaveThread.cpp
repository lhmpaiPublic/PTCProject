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
	if (m_pThread == NULL) {
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
	DWORD	dwCode;
	LONG	ret;

	if (m_pThread != NULL) {
		ret = ::GetExitCodeThread(m_pThread->m_hThread, &dwCode);
		if (ret && dwCode == STILL_ACTIVE) {
			m_bKill = TRUE;
			WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		}
		delete m_pThread;
		m_pThread = NULL;
	}
}

UINT CImageSaveThread::CtrlThreadImgSave(LPVOID pParam)
{
	CImageSaveThread* pThis = (CImageSaveThread*)pParam;
	CImageProcessCtrl* pParent = pThis->m_pParent ;
	CImageSaveQueueCtrl* pQueuePtr = pParent->GetImageSaveQueuePtr() ;

	while (1)
	{
		if (pThis == NULL)
		{
			break;
		}

		if (pThis->m_bKill == TRUE)
		{
			break;
		}

		if (pQueuePtr == NULL)
		{
			break;
		}

		if (pQueuePtr->IsEmpty() == FALSE )
		{
			// Image 하나 가지고 오고 삭제함.
			CImgSaveInfo* pSaveInfo = pQueuePtr->Pop() ;
			if( pSaveInfo == NULL ) continue ;

			if ((pSaveInfo->m_nWidth <= 0) || (pSaveInfo->m_nHeight <= 0))
			{
				BYTE* pImgPtr = pSaveInfo->m_pImagePtr;
				delete[]pImgPtr;
				pImgPtr = NULL;

				delete pSaveInfo;
				pSaveInfo = NULL;
				continue ;
			}

			if (pSaveInfo->m_strSavePath.GetLength() > 0)
			{
				BYTE* pImgPtr = pSaveInfo->m_pImagePtr;

				CBitmapStd bmp(pSaveInfo->m_nWidth, pSaveInfo->m_nHeight, 8);
				bmp.SetImage(pSaveInfo->m_nWidth, pSaveInfo->m_nHeight, pSaveInfo->GetImgPtr() );
				// Debug시에 이미지 퀄리티가 계속 저하 되는 것을 방지.

				int nJpgQuality; 
				if (pQueuePtr->GetSize() > 2)
				{
					nJpgQuality = AprData.m_System.m_nJpegSaveQuality - 10 ;
				}
				else
				{
					nJpgQuality = AprData.m_System.m_nJpegSaveQuality;
				}

				bmp.SetJpegQuality(nJpgQuality);

				bmp.SaveBitmap(pSaveInfo->m_strSavePath);

				LOGDISPLAY_SPEC(0)(_T("<<CtrlThreadImgSave>> : Save BMP Name = %s"),
					pSaveInfo->m_strSavePath);

				delete []pImgPtr;
				pImgPtr = NULL;
			}

			delete pSaveInfo;
			pSaveInfo = NULL;
		}


		Sleep(10);
	}
	

	AfxEndThread(0);
	pThis->m_bKill = FALSE;

	return 0;
}