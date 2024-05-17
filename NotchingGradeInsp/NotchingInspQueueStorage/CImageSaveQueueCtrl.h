#pragma once

#include <queue>
#include <vector>
class CImgSaveInfo {
public :
	BYTE* m_pImagePtr;
	int m_nWidth; 
	int m_nHeight;
	int m_nBitCnt;

	std::vector<CString> m_strSavePath;
	//이미지 저장 퀄리티 정보
	std::vector<int> m_nJpgQuality;

	CImgSaveInfo()
	{
		m_pImagePtr = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBitCnt = 0;
		m_strSavePath.clear();
		m_nJpgQuality.clear();
	}
	CImgSaveInfo(const CImgSaveInfo& info)
	{
		m_pImagePtr = info.m_pImagePtr;
		m_nWidth = info.m_nWidth;
		m_nHeight = info.m_nHeight;
		m_nBitCnt = info.m_nBitCnt;
		m_strSavePath = info.m_strSavePath;
		m_nJpgQuality = info.m_nJpgQuality;
	}

	// 복사 대입 연산자(copy assignment operator)    
	CImgSaveInfo& operator=(const CImgSaveInfo& info)
	{
		m_pImagePtr = info.m_pImagePtr;
		m_nWidth = info.m_nWidth;
		m_nHeight = info.m_nHeight;
		m_nBitCnt = info.m_nBitCnt;
		m_strSavePath = info.m_strSavePath;
		m_nJpgQuality = info.m_nJpgQuality;

		return *this;
	}
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

