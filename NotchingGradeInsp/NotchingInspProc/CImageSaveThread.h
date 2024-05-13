#pragma once

class CImageProcessCtrl;

//class CImgSaveInfo {
//public :
//	CString strSavePath;
//	BYTE* pImagePtr;
//	int nWidth; 
//	int nHeight;
//	int nBitCnt;
//};

class CImageSaveThread
{
public :
	CImageSaveThread(int nCamNo, CImageProcessCtrl* pParent);
	~CImageSaveThread(void);

	void Begin(void);
	void Kill(void);

	//typedef std::queue< CImgSaveInfo* > QUEUE_IMG_SAVE ;

	// 동기화 이벤트 객체
	HANDLE getEvent_ImageSaveThread() { return pEvent_ImageSaveThread; }
	void setEvent_ImageSaveThread() { SetEvent(pEvent_ImageSaveThread); }
protected:
	BOOL	m_bKill;
	int		m_nThreadId;
	CWinThread* m_pThread;				
	CImageProcessCtrl* m_pParent;
	int		m_nCameraNo;

	//동기화 이벤트 객체
	HANDLE pEvent_ImageSaveThread;
protected:
	static UINT CtrlThreadImgSave(LPVOID pParam);
};

