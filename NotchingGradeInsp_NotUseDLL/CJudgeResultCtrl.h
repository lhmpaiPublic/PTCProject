#pragma once

#include <vector>

class CJuegeResultInfo{
public :
	int nTabNo;
	BOOL nJudgeNG[MAX_CAMERA_NO];
};

class CJudgeResultCtrl
{
public :
	CJudgeResultCtrl();
	~CJudgeResultCtrl();

private :
	std::vector< CJuegeResultInfo > m_vecJudgeRslt;
	BOOL IsNewJudgeNG(int nTabNo, int nCamPos);

public :
	// 판정 종합.
	int GetNgTabCount();
	int SetJudgeNG(int nTabNo, int nCamPos);
	void ResetAll();


protected:
	CRITICAL_SECTION m_csJudge;

};

