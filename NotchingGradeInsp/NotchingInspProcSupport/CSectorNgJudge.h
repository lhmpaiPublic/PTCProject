#pragma once

//#include <queue>
#include <vector>

class CTabJudge {
public :
	int nTabNo;
	int nJudge;
	int nReason;
};

class CSectorNgJudge
{
private :
	
	typedef std::vector< CTabJudge > VEC_TAB_JUDGE ;
	VEC_TAB_JUDGE m_vecSector;

public :
	int ResetAll();
	int AddNgTab(CTabJudge tab, int nSectSize );
	int AddOkTab(int nTabNo, int nSectSize );
	int m_nNgCount;

};

