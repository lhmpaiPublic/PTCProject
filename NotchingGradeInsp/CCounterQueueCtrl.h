#pragma once

#include <queue>
#define MAX_COUNTER_QUEUE_SIZE 4 // 22.01.11 Ahn Modify 2->3
class CCounterInfo {
public :
	CCounterInfo() { nTabNo = 0; nTabID = 0;  nJudgement = 0; nMarkSel = 0; nMarkType = 0; };
	int nTabNo; // 검사기의 TabNo
	int nTabID;	// Counter의 TabId
	int nJudgement;
	int nMarkSel;
	int nMarkType;
};
class CCounterQueueCtrl
{
public :
	CCounterQueueCtrl();
	~CCounterQueueCtrl(void);
	void ResetQueue();
	int PushBack(CCounterInfo CntInfo);
	int GetSize();
	BOOL IsEmpty();
	CCounterInfo Pop();
	int CalcMaxQueueSize(int nAcqBufSize, int nOneTabSize);
	void SetMaxQueueSize( int nSize );

protected :
	long m_lCount;
	CRITICAL_SECTION m_csQueue;
	int m_nMaxQueueSize;

private:
	std::queue< CCounterInfo > m_CntInfo;

};

