#pragma once

#include <queue>

//#define MAX_CROP_IMAGE_QUEUE_SIZE 9

class CCropImgData {
public:
	CCropImgData() {
		m_bEnable = FALSE;
		m_strFileName = _T("");
		m_strDispName = _T("");
	//	pImgPtr = NULL;
	}
	BOOL	m_bEnable;
	CString m_strFileName;
	CString m_strDispName;
	//BYTE* pImgPtr;
};
class CCropImgQueueCtrl
{
public:
	CCropImgQueueCtrl();
	~CCropImgQueueCtrl(void);

	void ResetQueue();
	int PushBack(CCropImgData data);
	int GetSize();
	BOOL IsEmpty();
	CCropImgData Pop();

	long m_lCount;

protected:
	CRITICAL_SECTION m_csQueue;

private:
	std::queue< CCropImgData > m_queCropImg ;
};

