#pragma once
#include <vector>
#include <queue>
//class CDefectInfo;
#include "CTabRsltInfo.h"
#include "CJudgeResultCtrl.h"

class CDefectDataCtrl
{
private :
	CJudgeResultCtrl m_NgRsltCtrl ;
//	CJudgeResultCtrl m_OkRsltCtrl ;
//	CJudgeResultCtrl* GetJudgeResultCtrlPtr() { return &m_JudgeRsltCtrl; };
	int m_nOkImageCount ;
	int m_nNgImageCount ;
	int m_nOkCropImgCount ;
	int m_nNGCropImgCount ;

public :

	CDefectDataCtrl();
	~CDefectDataCtrl();

	// 기본
	//CTabRsltInfo* GetTabRsltInfoPtr(int nDefIndex) ;
	int GetSize();
	int DeleteDefect(int nIndex);
	CDefectInfo GetDefecInfo(int nIndex) ;
	void RemoveAll();

	int m_nLastSaveIndex;
	int m_nLastSaveTabIndex;
	CString m_strLotNo;
	CString m_strDate;
	// 22.06.30 Ahn Delete Start
	//CString GetLotStartDate(CTime ctLotStartTime);
	//CString GetErrorSavePath(CTime ctLotStTime, CString strLotNo);
	//CString GetErrorSavePath(CString strDate, CString strLotNo);
	// 22.06.30 Ahn Delete End

	// 결과 저장전의 Data 
	typedef std::queue< CTabRsltInfo *>QUE_TAB_RSLT_INFO ;
	QUE_TAB_RSLT_INFO m_queTabRsltInfo ;
	void PushBackTabQueue(CTabRsltInfo *pTabRsltInfo);
	CTabRsltInfo* PopTabQueue();
	void ResetTabQueue();

	CTabRsltInfo* GetTabRsltInfo(int nHeadNo, int nTabNo);

	int SaveErrorData();
	int LoadErrorData( CString strDate, CString strLotNo);

	// 22.11.24 Modify Start
	//// 22.07.07 Ahn Add Start
	//typedef std::vector< CDefectInfo*> _VEC_DEFECT_LIST;
	//static int LoadDefectInfo(CString strFileName, _VEC_DEFECT_LIST& vecDefList);
	//// 22.07.07 Ahn Add Start
	typedef std::vector<CDefectInfo> _VEC_DEFECT_LIST;
	static int LoadDefectInfo(CString strFileName, _VEC_DEFECT_LIST& vecDefList);
	// 22.11.24 Modify End

	// 22.11.24 Ahn Add Start
	_VEC_DEFECT_LIST m_DrawDefList;
	void ClearDefectList();
	_VEC_DEFECT_LIST* GetDrawDefectList() {	return &m_DrawDefList; };
	//void AddDefectList(CDefectInfo pDefData);
	//int GetDrawDefectCount() ;
	// 22.11.24 Ahn Add End

	// 22.06.22 Ahn Delete Start
	//typedef std::vector< CTabRsltInfo* > VEC_DEV_LIST;
	//VEC_DEV_LIST m_VecTabRsltList;
	// 22.06.22 Ahn Delete Start

	//enum {
	//	en_InspType		= 0, // FOIL-EXP
	//	en_InspTime		= 1, // 202212....
	//	en_EqpType		= 2, // 장비명
	//	en_Electrode	= 3, // 
	//	en_LotID		= 4, 
	//	en_TabNo		= 5,
	//	en_CameraPos	= 6,
	//	en_TabJudge		= 7,
	//	en_DefJudge		= 8,
	//	en_DefNo		= 9,
	//	en_DefType		= 10,
	//	en_FilePathMax  = 11,
	//};

	//판정 정보
	int AddJudgeNG(int nTabNo, int nCamNo);
	int GetNgTabCount();

	enum {
		enResultFileSize = 8192,
	};

	// 22.02.11 Ahn Add Start
	int GetOkImgCnt() { return m_nOkImageCount; };
	int GetNgImgCnt() { return m_nNgImageCount; };
	int GetOkCropImgCnt() { return m_nOkCropImgCount; };
	int GetNgCropImgCnt() { return m_nNGCropImgCount; };
	// 22.02.11 Ahn Add End
private :

protected :
	CRITICAL_SECTION m_csDefData;
	CRITICAL_SECTION m_csTabQueue ;

};

