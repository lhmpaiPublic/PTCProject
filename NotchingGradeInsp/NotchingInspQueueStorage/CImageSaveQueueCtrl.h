#pragma once

#include <queue>
#include <vector>
class CImgSaveInfo {
public :
	std::vector<CString> m_strSavePath;
	BYTE* m_pImagePtr;
	int m_nWidth; 
	int m_nHeight;
	int m_nBitCnt;

	//이미지 저장 퀄리티 정보
	std::vector<int> m_nJpgQuality;

	BYTE* GetImgPtr() { return m_pImagePtr; };
	void SetImgPtr(BYTE* pImgPtr, int nWidth, int nHeight, int nJpgQuality = 100) { m_pImagePtr = pImgPtr; m_nWidth = nWidth; m_nHeight = nHeight; };
};

class CImageSaveQueueCtrl
{
public:
	CImageSaveQueueCtrl();
	~CImageSaveQueueCtrl(void);

	void ResetQueue();
	int PushBack(CImgSaveInfo* pSaveInfo);
	int GetSize();
	BOOL IsEmpty();
	CImgSaveInfo* Pop();

	long m_lCount;

protected:
	CRITICAL_SECTION m_csQueue;

private:
	std::queue< CImgSaveInfo* > m_queSaveInfo ;
};

