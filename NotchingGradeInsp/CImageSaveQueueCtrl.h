#pragma once

#include <queue>
class CImgSaveInfo {
public :
	CString m_strSavePath;
	BYTE* m_pImagePtr;
	int m_nWidth; 
	int m_nHeight;
	int m_nBitCnt;

	BYTE* GetImgPtr() { return m_pImagePtr; };
	void SetImgPtr(BYTE* pImgPtr, int nWidth, int nHeight) { m_pImagePtr = pImgPtr; m_nWidth = nWidth; m_nHeight = nHeight; };
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

