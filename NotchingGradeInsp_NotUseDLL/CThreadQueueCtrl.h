#pragma once

#include <queue>

class CImageProcThreadUnit;

//#define	MAX_CAM_NO 2


//class CFrameInfo {
//public:
//	int nTabNo;
//	BYTE* pImagePtr;
//	int nWidth;
//	int nHeight;
//	int nCamNo;
//};
class CFrameInfo;
class CImageProcessCtrl;

class CThreadQueueCtrl
{
public :
	CThreadQueueCtrl(CImageProcessCtrl *pParent );
	~CThreadQueueCtrl();
	int push(CFrameInfo* pFrmInfo) ;
	CImageProcThreadUnit* pop();
	BOOL IsEmpty();
	void ResetQueue();
	int GetSize();


protected :

	CImageProcessCtrl* m_pParent;
	CRITICAL_SECTION m_csQueue;

	typedef std::queue< CImageProcThreadUnit*> THREAD_QUEUE;
	THREAD_QUEUE m_pThradQue;


};

