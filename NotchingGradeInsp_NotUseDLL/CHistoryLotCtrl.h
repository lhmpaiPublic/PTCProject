#pragma once

#include <queue>

class CHistoryLotCtrl
{
public :
	typedef struct stLotInfo {
		char chLotNo[32];
		char chRecipeName[32];
		int	nTabCount;
		int	nTabCountNG;
		int	nTabCountOK;
		int nTopNgCount;
		int nBottomNgCount;
		int nMarkingCount;

		SYSTEMTIME StartTime;
		SYSTEMTIME EndTime;

	} _LOT_INFO;


private:
	typedef std::vector< _LOT_INFO > VEC_LOT_INFO ;
	VEC_LOT_INFO m_vecLotInfo;

	CString m_strPath;
	CString m_strFileName;
	CString m_strFullPath;
//	int m_nMaxQueueSize ;
public :

	CHistoryLotCtrl( CString strPath, CString strFileName );
	~CHistoryLotCtrl();

	BOOL IsFileSizeNormal(CString strFileName);
	int AddLotFileList(_LOT_INFO& LotInfo);
	int ReadLotFileList(VEC_LOT_INFO* pVecLotList);
	int SaveLotFileList(VEC_LOT_INFO* pVecLotList);
	int PushLotInfo(_LOT_INFO& LotInfo);
	int PopLotInfo(_LOT_INFO* pLotInfo);
	int GetAtLotInfo( int nIndex, _LOT_INFO* pLotInfo);
	void ClearQueue();

	int size();


};

