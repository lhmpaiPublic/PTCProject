#pragma once

#include <queue>
class CFrameInfo ;
class CQueueCtrl
{
public:
	CQueueCtrl();
	~CQueueCtrl(void);

	bool m_bQueuePushPop;

	void ResetQueue() ;
	int PushBack( CFrameInfo *pFrmInfo ) ;
	int GetSize() ;
	BOOL IsEmpty() ;
	CFrameInfo * Pop() ;

	long m_lCount ;

protected :
	CRITICAL_SECTION m_csQueue ;

private :
	std::queue< CFrameInfo * > m_queFrameInfo ;

};
