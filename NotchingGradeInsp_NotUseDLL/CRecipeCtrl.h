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
 *						����
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * -------------------------------------------------------
 *														    nCeramicTailHeight
 * -------------------------------------------------------
 *
*/
class CTabCondition {
public:

	int nTabWidth;			// Tab Head�� ��
	int nTabPitch;			// Tab�� Pitch
	int nRadiusH;			// Tab ���� ������ ������. ���� ���� Pixel��
	int nRadiusW;			// Tab ���� ���� ���� Pixel�� 
	int nCeramicHeight;		// Top �� ����� ���� �β�.
	int nTabCeramicHeight;	// Tab Head �� Ceramic ���� �β�
	int nCeramicTailHeight; // Tab�� ���� �����κ��� Ceramic ���� �β�

	// 22.06.08 Ahn Modify Start
	double dTabWidth;			// Tab Head�� ��
	double dTabPitch;			// Tab�� Pitch
	double dRadius;			// Tab ���� ������ ������. ���� ���� Pixel��
	double dCeramicHeight;		// Tab Head �� Ceramic ���� �β�
	double dTabCeramicHeight;
	double dCeramicTailHeight; // Tab�� ���� �����κ��� Ceramic ���� �β�
	// 22.06.08 Ahn Modify End
	//int nMeanWidth;			// Mean ó�� ��

	// 23.02.14 Ahn Add Start
	int nTabMinBright;		// Tab �˻��� �����ֵ�
	int nRollBrightMode[MAX_CAMERA_NO];
	// 23.02.14 Ahn Add End

	int nCeramicBrightLow[MAX_CAMERA_NO];
	int nCeramicBrightHigh[MAX_CAMERA_NO];
	int nRollBrightLow[MAX_CAMERA_NO];
	int nRollBrightHigh[MAX_CAMERA_NO];
	int nEdgeFindMode[MAX_CAMERA_NO];		// en_FindThresMode(Min~Max) Threshold ������ ���� �������� Edge �ν�  // en_FineMaxDiffMode �¿� �ֵ����� �ִ��� ���� ��ġ�� Edge�� �ν�.
	// Test�� 
	//int nCompWidth;			// Tab ã�� ��尡 ������ ��ŭ�� �¿� ���� ����ϴ� Mode�� ��� ���.

	// 22.01.05 Ahn Add Start
	int nNegCoatHeight;		// ���� Tab���� �濬 ���ó���
	int nNegVGrooveHeight;	// ���� V Ȩ�� ����. 
	// 22.01.05 Ahn Add End
};

class CRecipeInfo {
	// Tab Condition
public:
	// Tab ���� �� Tab��ġ Ȯ�ο� Recipe
	CString m_strRecipeName;
	CTabCondition TabCond;

	// Detect Param
	// Threshold
	int nDrossThresLower[MAX_CAMERA_NO];
	int nDrossThresOrigin[MAX_CAMERA_NO];
	int nFoilExpThresOrigin[MAX_CAMERA_NO];

	int nFoilExpThresLower[MAX_CAMERA_NO];
	// �ּ� ���� ������
	// 22.07.19 Ahn Modify Start
	int nFoilExpInMinSize[MAX_CAMERA_NO];
	int nFoilExpOutMinSize[MAX_CAMERA_NO];
	// 22.07.19 Ahn Modify Start

	// �˻� ��(����) 
	double dFoilOutInspWidth[MAX_CAMERA_NO];
	double dFoilExpInspWidth[MAX_CAMERA_NO];
	int nFoilOutInspWidth[MAX_CAMERA_NO];			// Dross ���� ���� - Threshold ó�� ���� ������
	int nFoilExpInspWidth[MAX_CAMERA_NO];		// Foil ���� ���� ���� - Threshold ó�� ���� ������
	int nFoilOutMaskOffset[MAX_CAMERA_NO];		// ���� Line �������� ��˻� ���� ����. 
	int nFoilExpMaskOffset[MAX_CAMERA_NO];		// ���� Line�� �������� ��˻� ���� ����.
	double dMagnification[MAX_CAMERA_NO];		// ���� ����
	CPoint pntLinkRange[MAX_CAMERA_NO][MAX_INSP_TYPE];

	//Light ����
	int nLightLevel[MAX_LIGHT_UNIT][MAX_LIGHT_CHANEL];

	// 22.07.15 Ahn Add Start
	int nMarkingUse[MAX_INSP_TYPE];
	int nMarkingType[MAX_INSP_TYPE];
	// 22.07.15 Ahn Add End

	// ���� ����
	int nJudgeMode;		// 0 : AND , 1 : OR
	// 22.07.19 Ahn Modify Start
	//double dDefJudgeWidth;
	double dDefJudgeHeight; // ������...

	double dFoilExpInNgSize;
	double dFoilExpOutNgSize;
	double dFoilExpBothNgSize;
	// 22.07.19 Ahn Modify End

	double dIgnoreDistance;
	double dIgnoreSize;
	int nThresSurface[MAX_CAMERA_NO];
	int nSurfaceMaskOffset[MAX_CAMERA_NO];
	BOOL bDisableSurface;

	double dSurfaceNgSize ;
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

