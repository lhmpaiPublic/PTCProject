#pragma once

#include <queue>
//Save 이미지 사이트 픽스 크기
#define MAX_IMAGESAVEINFO_INSPDATA_SIZE (4000 * 14000) + 1
class CImgSaveInfo {
public :
	CString m_strSavePath;
	BYTE m_pImagePtr[MAX_IMAGESAVEINFO_INSPDATA_SIZE];
	int m_nWidth; 
	int m_nHeight;
	int m_nBitCnt;

	//이미지 저장 퀄리티 정보
	int m_nJpgQuality;

	void initImgPtr(int nSize);
	void initImgPtr(int nWidth, int nHeight);
	BYTE* GetImgPtr();
	void SetImgPtr(BYTE* pImgPtr, int nWidth, int nHeight, int nJpgQuality = 100);
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

