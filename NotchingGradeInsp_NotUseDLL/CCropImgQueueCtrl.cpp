#include "pch.h"
#include "CCropImgQueueCtrl.h"


CCropImgQueueCtrl::CCropImgQueueCtrl()
{
	::InitializeCriticalSection(&m_csQueue);
}

CCropImgQueueCtrl::~CCropImgQueueCtrl(void)
{
	::DeleteCriticalSection(&m_csQueue);
}

void CCropImgQueueCtrl::ResetQueue()
{
	::EnterCriticalSection(&m_csQueue);
	int size = (int)m_queCropImg.size();
	int i = 0;
	m_lCount = 0;
	CCropImgData data ;
	for (i = 0; i < size; i++) {
		data = m_queCropImg.front();
		m_queCropImg.pop();
	}
	::LeaveCriticalSection(&m_csQueue);
}

int CCropImgQueueCtrl::PushBack(CCropImgData data)
{
	int nFrmCnt = 0;
	::EnterCriticalSection(&m_csQueue);
	int nSize = (int)m_queCropImg.size() ;

	if (MAX_CROP_IMAGE_QUEUE_SIZE <= nSize) {
		if (m_queCropImg.empty() == false) {
			m_queCropImg.pop();
		}
	}

	m_queCropImg.push(data);

	::LeaveCriticalSection(&m_csQueue);
	return nFrmCnt;
}

int CCropImgQueueCtrl::GetSize()
{
	::EnterCriticalSection(&m_csQueue);
	int nSize = (int)m_queCropImg.size();
	::LeaveCriticalSection(&m_csQueue);
	return nSize;
}

BOOL CCropImgQueueCtrl::IsEmpty()
{
	::EnterCriticalSection(&m_csQueue);
	BOOL bRet = TRUE;
	if (!m_queCropImg.empty()) {
		bRet = FALSE;
	}
	::LeaveCriticalSection(&m_csQueue);
	return bRet;
}

CCropImgData CCropImgQueueCtrl::Pop()
{
	::EnterCriticalSection(&m_csQueue);
	CCropImgData data;
	if (m_queCropImg.empty() == false) {
		data = m_queCropImg.front();
		m_queCropImg.pop();
	}
	::LeaveCriticalSection(&m_csQueue);
	return data;
}