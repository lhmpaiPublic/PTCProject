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

protected:
	BOOL	m_bKill;
	int		m_nThreadId;
	CWinThread* m_pThread;				
	CImageProcessCtrl* m_pParent;
	int		m_nCameraNo;

protected:
	static UINT CtrlThreadImgSave(LPVOID pParam);
};

