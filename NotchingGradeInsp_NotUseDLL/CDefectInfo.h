#pragma once
#include <queue>

class CDefectInfo
{
public:
	int nType;				// ���� ���� 0 : Foil Exposure, 1 : Foil ExposureOut, 2 : Surface
	int nHeadNo;			// Camera ��ȣ
	int nTabNo;				// Cell ��ȣ
	int nDefNo;				// ���� ��ȣ.
	int nDefectMode;		// ����
	int nSize;				// 
	int nSizeX;
	int nSizeY;
	int nMaxBright;
	int nMinBright;
	int nAvgBright;
	int nAveOrgBir;
	int nMinOrgBir;
	int nMaxOrgBir;
	int nRank;				// ���� ����. JUDGE_OK / JUDGE_NG
	double dSizeX;
	double dSizeY;
	double dJudgeSize;

	BOOL bDeleteFlag;
	double dDistance;
	CRect rcPos;
	BOOL bImageFlag;
	char szLotID[16];
	SYSTEMTIME sysTime;
	char szImgFileName[256]; // LotID_CellNo_Position_DefNo_Judge_TIme.XXX	
	char szRecipeName[64];

	// 22.12.14 Ahn Modify Start
	//int nDummy[9];
	int	nDispPos ;	// 0 : Electrod Edge 1 : Round Cut 2 : On Surface
	int nDummy[8];
	// 22.12.14 Ahn Modify End
	// 22.11.17 Ahn Modify Start
	//double dDummy[9];
	double dDefectPosY ; // Meter [m]
	double dDummy[8];
	// 22.11.17 Ahn Modify End
	BOOL bMarking;
	BOOL bDummy[3];
};

class CDefectQueueCtrl {
public:
	CDefectQueueCtrl();
	~CDefectQueueCtrl(void);

	void ResetQueue();
	int PushBack(CDefectInfo* pSaveInfo);
	int GetSize();
	BOOL IsEmpty();
	CDefectInfo* Pop();

	long m_lCount;

protected:
	CRITICAL_SECTION m_csQueue;

private:
	std::queue< CDefectInfo* > m_queDefectInfo ;
};