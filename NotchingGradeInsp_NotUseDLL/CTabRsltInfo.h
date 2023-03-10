#pragma once

#include <vector>
#include "CDefectInfo.h"

class CRecipeInfo;

class CTabRsltBase
{
public:
	CTabRsltBase();
	~CTabRsltBase();
	int m_nTabNo;
	int m_nHeadNo;
	char m_chImagePath[ 256 ];
	char m_chImageFile[ 256 ];
	char m_chLotID[16] ;
	SYSTEMTIME sysTime;

	int m_nJudge;
	int m_nCount[MAX_INSP_TYPE][MAX_RANK];
	int m_nDefectCount;

	double m_dMaxSizeDef;
	BOOL bDummy1[2];
	BOOL m_bMarkingFlag;

	// 22.07.08 Ahn Add Start
	enum {
		// 22.07.25 Ahn Modify Start
		en_Reason_NA					= 0x0000,
		en_Reason_Continus				= 0x0001,
		en_Reason_Heavy					= 0x0002,
		en_Reason_HV_Continus			= 0x0004, 
		en_Reason_FoilExp_Top			= 0x0008,
		en_Reason_FoilExp_Btm			= 0x0010,
		en_Reason_FoilExpOut_Top		= 0x0020,
		en_Reason_FoilExpOut_Btm		= 0x0040,
		en_Reason_Surface_Top			= 0x0080,
		en_Reason_Surface_Btm			= 0x0100,
		// 22.07.25 Ahn Modify End
	};
	// 22.07.08 Ahn Add End
	// 22.07.08 Ahn Modify Start
	// int nDummy[10];
	WORD m_wNgReason; // 
	// 22.11.18 Ahn Modify Start
	//int nDummy[9];
	// 23.02.13 Ahn Modify Start
	//int nDummy[7];
	int nDummy[6];
	int nTargetBright;
	// 23.02.13 Ahn Modify End
	int nFrameCount;
	int nTabStartPosInFrame;
	// 22.11.18 Ahn Modify End
	// 22.07.08 Ahn Modify End
	BOOL bDummy[8];
	BOOL m_bImageFlag;
	BOOL m_bCropImgFlag;

	// 22.06.15 Ahn Modify Start
	// 23.02.13 Ahn Modify Start
	//double dDummy[10];
	double dDummy[9];
	double dFocusValue;
	// 23.02.13 Ahn Modify End
	//double dDummy[4];
	//int m_nDefectMaxSize[MAX_CAMERA_NO][MAX_INSP_TYPE];
	// 22.06.15 Ahn Modify End

	void SetTabInfo(int nHeadNo, int nTabNo, char *strPath);

};
class CTabRsltInfo:public CTabRsltBase
{
	
	public :
		CTabRsltInfo();
		~CTabRsltInfo();

		typedef std::vector< CDefectInfo *> VEC_DEF_INFO;
	
		VEC_DEF_INFO m_vecDefInfo;

		int GetDefectCount();
		double SortingDefect(int nMode);
		int GetJudgement(CRecipeInfo *pRecipeInfo);
		int GetJudgement(double dSizeX, double dSizeY, double &dMaxSize, BOOL *pbJudgeType, int &bDefNo );
		CPoint GetDefectCenterPos(int nDefIndex);
		static int GetDefJudge(double dSizeX, double dSizeY, double dDefSizeX, double dDefSizeY );

		void AddDefectInfo(CDefectInfo *pDefInfo);
		void RemoveAll();
		int GetSize();

		// 22.05.25 Ahn Add Start
		CDefectInfo* GetDefectInfo(int nIndex);
		// 22.05.25 Ahn Add End

};

