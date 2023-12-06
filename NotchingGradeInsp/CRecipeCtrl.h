#pragma once


/*				---------------------
*				|					|
*				|					|
*				|					|
*				|					|
*				|					|
**				---------------------
*				|					|		nTabCeramicHeight
*				|					|
 * --------------					----------------
 *														nCeramicHeight
 * -------------------------------------------------
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *						생략
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * -------------------------------------------------------
 *														    nCeramicTailHeight
 * -------------------------------------------------------
 *
*/
class CTabCondition {
public:

	int nTabWidth;			// Tab Head의 폭
	int nTabPitch;			// Tab간 Pitch
	int nRadiusH;			// Tab 양쪽 라운드의 반지름. 세로 방향 Pixel수
	int nRadiusW;			// Tab 라운드 가로 방향 Pixel수 
	int nCeramicHeight;		// Top 부 어깨쪽 코팅 두께.
	int nTabCeramicHeight;	// Tab Head 쪽 Ceramic 코팅 두께
	int nCeramicTailHeight; // Tab이 없는 꼬리부분의 Ceramic 코팅 두께

	// 22.06.08 Ahn Modify Start
	double dTabWidth;			// Tab Head의 폭
	double dTabPitch;			// Tab간 Pitch
	double dRadius;			// Tab 양쪽 라운드의 반지름. 세로 방향 Pixel수
	double dCeramicHeight;		// Tab Head 쪽 Ceramic 코팅 두께
	double dTabCeramicHeight;
	double dCeramicTailHeight; // Tab이 없는 꼬리부분의 Ceramic 코팅 두께
	// 22.06.08 Ahn Modify End
	//int nMeanWidth;			// Mean 처리 폭

	// 23.02.14 Ahn Add Start
	int nTabMinBright;		// Tab 검색용 최저휘도
	int nRollBrightMode[MAX_CAMERA_NO];
	// 23.02.14 Ahn Add End

	int nCeramicBrightLow[MAX_CAMERA_NO];
	int nCeramicBrightHigh[MAX_CAMERA_NO];
	int nRollBrightLow[MAX_CAMERA_NO];
	int nRollBrightHigh[MAX_CAMERA_NO];
	int nEdgeFindMode[MAX_CAMERA_NO];		// en_FindThresMode(Min~Max) Threshold 사이의 값을 기준으로 Edge 인식  // en_FineMaxDiffMode 좌우 휘도차가 최대인 곳의 위치를 Edge로 인식.
	// Test용 
	//int nCompWidth;			// Tab 찾기 모드가 설정값 만큼의 좌우 차를 사용하는 Mode인 경우 사용.

	// 22.01.05 Ahn Add Start
	int nNegCoatHeight;		// 음극 Tab부의 흑연 코팅높이
	int nNegVGrooveHeight;	// 음극 V 홈의 깊이. 
	// 22.01.05 Ahn Add End
};

class CRecipeInfo {
	// Tab Condition
public:
	// Tab 분할 및 Tab위치 확인용 Recipe
	CString m_strRecipeName;
	CTabCondition TabCond;

	// Detect Param
	// Threshold
	int nDrossThresLower[MAX_CAMERA_NO];
	int nDrossThresOrigin[MAX_CAMERA_NO];
	int nFoilExpThresOrigin[MAX_CAMERA_NO];

	int nFoilExpThresLower[MAX_CAMERA_NO];
	// 최소 검출 사이즈
	// 22.07.19 Ahn Modify Start
	int nFoilExpInMinSize[MAX_CAMERA_NO];
	int nFoilExpOutMinSize[MAX_CAMERA_NO];
	// 22.07.19 Ahn Modify Start

	// 검사 폭(높이) 
	double dFoilOutInspWidth[MAX_CAMERA_NO];
	double dFoilExpInspWidth[MAX_CAMERA_NO];
	int nFoilOutInspWidth[MAX_CAMERA_NO];			// Dross 검출 범위 - Threshold 처리 영역 지정용
	int nFoilExpInspWidth[MAX_CAMERA_NO];		// Foil 노출 검출 범위 - Threshold 처리 영역 지정용
	int nFoilOutMaskOffset[MAX_CAMERA_NO];		// 검출 Line 기준으로 비검사 영역 설정. 
	int nFoilExpMaskOffset[MAX_CAMERA_NO];		// 검출 Line을 기준으로 비검사 영역 설정.
	double dMagnification[MAX_CAMERA_NO];		// 검출 감도
	CPoint pntLinkRange[MAX_CAMERA_NO][MAX_INSP_TYPE];

	//Light 광량
	int nLightLevel[MAX_LIGHT_UNIT][MAX_LIGHT_CHANEL];

	// 22.07.15 Ahn Add Start
	int nMarkingUse[MAX_INSP_TYPE];
	int nMarkingType[MAX_INSP_TYPE];
	// 22.07.15 Ahn Add End

	// 판정 정보
	int nJudgeMode;		// 0 : AND , 1 : OR
	// 22.07.19 Ahn Modify Start
	//double dDefJudgeWidth;
	double dDefJudgeHeight; // 삭제요...

	double dFoilExpInNgSize[MAX_CAMERA_NO];
	double dFoilExpOutNgSize[MAX_CAMERA_NO];
	double dFoilExpBothNgSize[MAX_CAMERA_NO];
	double dSurfaceNgSize[MAX_CAMERA_NO];

	double dDefectYSize[MAX_CAMERA_NO];


	double dIgnoreDistance;
	double dIgnoreSize;
	int nThresSurface[MAX_CAMERA_NO];
	int nSurfaceMaskOffset[MAX_CAMERA_NO];
	BOOL bDisableSurface;

	int m_nSurfaceMinSize[MAX_CAMERA_NO];
	// 22.11.21 Ahn Add Start - JUDGE_GRAY
	double dSurfaceGraySize;
	double dFoileGraySize;
	// 22.11.21 Ahn Add End

	double dSaveDefSize;
	BOOL bSaveDefInTab;
	int nBmpSaveInterval;
	BOOL bSaveOnlyNgTab;
	BOOL bDisableProcDiff;
	BOOL bEnableDefectLink[MAX_CAMERA_NO];
	BOOL bNgStop;
	BOOL bDarkEmpMode;

	// 22.08.09 Ahn Add Start
	int	nContinousNgCount;
	int nAlarmCount;
	int nSectorCount;
	// 22.08.09 Ahn Add End


	BOOL bDisablePET;
	int nPetMatrixX[MAX_CAMERA_NO];
	int nPetMatrixY[MAX_CAMERA_NO];
	int nPetThreshold[MAX_CAMERA_NO];
	int nPetCheckCnt[MAX_CAMERA_NO];



	enum {
		en_RankCond_Size = 0,
		en_RankCond_Width,
		en_RankCond_Height,
		en_RankCond_Length,
		en_RankCond_Bright,
		en_RankCond_Max,
	};
};


class CRecipeCtrl
{
public :
	CRecipeCtrl();
	~CRecipeCtrl();

	int SearchRecipeFile(CStringList& strRecipeList);

	int LoadRecipe(CString strRecipeName);
	int SaveRecipe(CString strRecipeName);
	int LoadRecipe(CRecipeInfo* pRecipe, CString strRecipeName);
	int SaveRecipe(CRecipeInfo* pRecipe, CString strRecipeName);
	int GetRecipeList( CStringList &strRecipeNameList );

	BOOL IsEnableRecipe(CString strRcpName);

	void SetRecipeInfo(CRecipeInfo *pRcpInfo);
	CRecipeInfo* GetRecipeInfoPtr() { return &m_RcpInfo; };

	BOOL IsAvailableRecipe(CRecipeInfo* pRcpInfo);

private :

	// 22.06.08 Ahn Add Start
	BOOL SaveRecipePrameter( CString strRecipeName, CString strSection, CString strKey, CString strData, CString strFileName ) ;
	BOOL SaveModifyedRecipePramLog(CString strRecipeName, CString strPramName, CString strOldParam, CString strNewParam);
	// 22.06.08 Ahn Add End

	int FileCtrl(CString strRecipeName, int nMode, CRecipeInfo* pRecipeInfo = NULL);

	CStringList m_strList;
	CRecipeInfo m_RcpInfo;

	//CRecipeTableInfo m_RcpTable;


};

