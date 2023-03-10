#include "pch.h"
#include "CRecipeCtrl.h"
#include "Win32File.h"
#include "GlobalData.h"


CRecipeCtrl::CRecipeCtrl()
{
}
CRecipeCtrl::~CRecipeCtrl()
{

}
// 22.06.08 Ahn Add Start
BOOL CRecipeCtrl::SaveModifyedRecipePramLog(CString strRecipeName, CString strParamName, CString strOldParam, CString strNewParam)
{
	BOOL bRet = FALSE;
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	CString strLogPath; // D:/DAT/RECIPELOG/³â¿ù/ÀÏ/
	CString strLogFileName = _T("RecipeParamLog.txt");
	CString strLogMsg;

	CString strTime;
	::GetLocalTime(&sysTime);

	strTime.Format(_T("%04d/%02d/%02d ,%02d:%02d:%02d.%03d, ")
		, sysTime.wYear
		, sysTime.wMonth
		, sysTime.wDay
		, sysTime.wHour
		, sysTime.wMinute
		, sysTime.wSecond
		, sysTime.wMilliseconds
	) ;

	strLogPath.Format(_T("%s\\%04d%02d\\%02d"), AprData.m_strRecipeLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay); // 22.07.28 Ahn Modify %02
	CWin32File winFile;
	winFile.CreateDirectory(strLogPath);

	strLogMsg.Format(_T("%s RecipeName[%s], %s Old[%s]->New[%s]\r\n"), strTime, strRecipeName, strParamName, strOldParam, strNewParam ) ;

	CWin32File file;
	file.TextSave1Line(strLogPath, strLogFileName, strLogMsg, _T("at"), FALSE, 999999999);

	return bRet;
}

BOOL CRecipeCtrl::SaveRecipePrameter(CString strRecipeName, CString strSection, CString strKey, CString strData, CString strFileName)
{
	BOOL bRet = FALSE ;
	char buff[256];
	CString strOldData;
	::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
	strOldData.Format(_T("%s"), buff);

	if (strData.Compare(strOldData) != 0 ){
		SaveModifyedRecipePramLog(strRecipeName, strKey, strOldData, strData);
		::WritePrivateProfileString(strSection, strKey, strData, strFileName);
		bRet = TRUE ;
	}

	return bRet;

}
// 22.06.08 Ahn Add End

int CRecipeCtrl::FileCtrl(CString strRecipeName, int nMode, CRecipeInfo* pRecipeInfo)
{
	CWin32File winFile;
	CString strFileName;
	strFileName.Format( _T("%s\\Recipe\\%s.ini"),AprData.m_strDataPath, strRecipeName ) ;

	char chType[MAX_INSP_TYPE][16] = { "FOILEXP","DROSS" };
	char chRank[MAX_RANK][4] = { "OK","NG" };
	char chRankCond[CRecipeInfo::en_RankCond_Max][16] = { "SIZE","WIDTH","HEIGHT","LENGTH","BRIGHT" };

	CRecipeInfo* pRcpInfo;
	if (pRecipeInfo != NULL) {
		pRcpInfo = pRecipeInfo;
	}
	else {
		pRcpInfo = &m_RcpInfo;
	}

	CString strSection;
	CString strKey;
	CString strData;
	if (nMode == MODE_WRITE) {
		{
			strSection = _T("TAB_INFO");
			strKey = _T("Tab_Width");
			strData.Format("%.2lf", pRcpInfo->TabCond.dTabWidth);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("Tab_Pitch");
			strData.Format("%.2lf", pRcpInfo->TabCond.dTabPitch );
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("Round_Radius");
			strData.Format("%.2lf", pRcpInfo->TabCond.dRadius);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			// 22.06.08 Ahn Delete Start
			//strKey = _T("Round_Radius_H");
			//strData.Format("%.2lf", pRcpInfo->TabCond.dRadiusH);
			//SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
			// 22.06.08 Ahn Delete End

			strKey = _T("TOP_COAT_HEIGHT");
			strData.Format("%.2lf", pRcpInfo->TabCond.dCeramicHeight);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("BOTTOM_COAT_HEIGHT");
			strData.Format("%.2lf", pRcpInfo->TabCond.dCeramicTailHeight);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("TAB_COAT_HEIGHT");
			strData.Format("%.2lf", pRcpInfo->TabCond.dTabCeramicHeight);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			// 23.02.14 Ahn Add Start
			strKey.Format(_T("TAB_MIN_BRIGHT"));
			strData.Format(_T("%d"), pRcpInfo->TabCond.nTabMinBright);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);
			// 23.02.14 Ahn Add End

			for (int i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format( _T("COAT_BRIGHT_HIGH_%d"),i);
				strData.Format( _T("%d"), pRcpInfo->TabCond.nCeramicBrightHigh[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format(_T("COAT_BRIGHT_LOW_%d"), i);
				strData.Format(_T("%d"), pRcpInfo->TabCond.nCeramicBrightLow[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format(_T("ROLL_BRIGHT_HIGH_%d"), i);
				strData.Format(_T("%d"), pRcpInfo->TabCond.nRollBrightHigh[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format(_T("ROLL_BRIGHT_LOW_%d"), i);
				strData.Format(_T("%d"), pRcpInfo->TabCond.nRollBrightLow[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format(_T("EDGE_SEARCH_MODE_%d"), i);
				strData.Format(_T("%d"), pRcpInfo->TabCond.nEdgeFindMode[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
				// 23.02.14 Ahn Add Start
				strKey.Format(_T("ROLL_BRIGHT_MODE_%d"), i);
				strData.Format(_T("%d"), pRcpInfo->TabCond.nRollBrightMode[i]);
				SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);
				// 23.02.14 Ahn Add End
			}

			// 22.01.05 Ahn Add Start
			strKey = _T("NEGATIVE_TAB_COAT_HEIGHT");
			strData.Format("%d", pRcpInfo->TabCond.nNegCoatHeight);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("NEGATIVE_V_GROOVE_HEIGHT");
			strData.Format("%d", pRcpInfo->TabCond.nNegVGrooveHeight);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
			// 22.01.05 Ahn Add End
		}
		{
			strSection = _T("INSP_CONDITION");

			for (int i = 0; i < MAX_LIGHT_UNIT; i++) {
				for (int j = 0; j < MAX_LIGHT_CHANEL; j++) {
					strKey.Format(_T("LIGHT_UNIT_%02d_CH_%02d"), i, j);
					strData.Format("%d", pRcpInfo->nLightLevel[i][j]);
					SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
				}
			}

			for (int i = 0; i < MAX_CAMERA_NO; i++) {

				strKey.Format( _T("DROSS_THRES_LOWER_%01d"), i );
				strData.Format("%d", pRcpInfo->nDrossThresLower[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("DROSS_THRES_UPPER_%01d"), i);
				strData.Format("%d", pRcpInfo->nDrossThresOrigin[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOIL_THRES_LOWER_%01d"), i);
				strData.Format("%d", pRcpInfo->nFoilExpThresLower[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOIL_THRES_UPPER_%01d"), i);
				strData.Format("%d", pRcpInfo->nFoilExpThresOrigin[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("DROSS_INSP_WIDTH_%01d"), i);
				strData.Format("%.3lf", pRcpInfo->dFoilOutInspWidth[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOIL_INSP_WIDTH_%01d"), i);
				strData.Format("%.3lf", pRcpInfo->dFoilExpInspWidth[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("DROSS_MASK_OFFSET_%01d"), i);
				strData.Format("%d", pRcpInfo->nFoilOutMaskOffset[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOIL_MASK_OFFSET_%01d"), i);
				strData.Format("%d", pRcpInfo->nFoilExpMaskOffset[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format(_T("MAGNIFICATION_CAM_%01d"), i);
				strData.Format("%.2lf", pRcpInfo->dMagnification[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOIL_OUT_MIN_SIZE_%01d"), i);
				strData.Format("%d", pRcpInfo->nFoilExpOutMinSize[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOIL_IN_MIN_SIZE_%01d"), i);
				strData.Format("%d", pRcpInfo->nFoilExpInMinSize[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("DROSS_LINK_X_%01d"), i);
				strData.Format("%d", pRcpInfo->pntLinkRange[i][TYPE_FOILEXP_OUT].x);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("DROSS_LINK_Y_%01d"), i);
				strData.Format("%d", pRcpInfo->pntLinkRange[i][TYPE_FOILEXP_OUT].y);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOILEXP_LINK_X_%01d"), i);
				strData.Format("%d", pRcpInfo->pntLinkRange[i][TYPE_FOILEXP].x);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("FOILEXP_LINK_Y_%01d"), i);
				strData.Format("%d", pRcpInfo->pntLinkRange[i][TYPE_FOILEXP].y);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format( _T("ENABLE_DEFECLINK_CAM_%01d"), i ) ;
				strData.Format( _T("%d"), pRcpInfo->bEnableDefectLink[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				// 22.05.10 Ahn Add Start
				strKey = _T("DISABLE_SPETTER_INSPECTION");
				strData.Format(_T("%d"), pRcpInfo->bDisableSurface);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format(_T("SPETTER_THRESHOLD_CAM_%01d"), i);
				strData.Format(_T("%d"), pRcpInfo->nThresSurface[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

				strKey.Format(_T("SPETTER_MASK_OFFSET_CAM_%01d"), i);
				strData.Format(_T("%d"), pRcpInfo->nSurfaceMaskOffset[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
				// 22.05.10 Ahn Add End

				// 22.06.08 Ahn Add Start
				strKey.Format(_T("SURFACE_MIN_SIZE_%01d"), i);
				strData.Format(_T("%d"), pRcpInfo->m_nSurfaceMinSize[i]);
				SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
				// 22.06.08 Ahn Add End
			}

			strKey = _T("BMP_SAVE_INTERVAL");
			strData.Format("%d", pRcpInfo->nBmpSaveInterval );
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("SAVE_IMAGE_ONLY_NG_TAB");
			strData.Format("%d", pRcpInfo->bSaveOnlyNgTab);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("ENABLE_DISABLE_DIFF_PROCESS");
			strData.Format("%d", pRcpInfo->bDisableProcDiff);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
						
			strKey = _T("NG_TAB_STOP");
			strData.Format("%d", pRcpInfo->bNgStop);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("DARK_EMP_MODE");
			strData.Format("%d", pRcpInfo->bDarkEmpMode);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
		}

		{
			strSection = _T("JUDGEMENT");

			strKey = _T("JUDGE_MODE");
			strData.Format("%d", pRcpInfo->nJudgeMode );
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("DEFECT_NG_SIZE_WIDTH");
			strData.Format("%.1lf", pRcpInfo->dFoilExpInNgSize);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("DEFECT_NG_SIZE_Height");
			strData.Format("%.1lf", pRcpInfo->dDefJudgeHeight);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			// 22.07.20 Ahn Add Start
			strKey = _T("DEFECT_FOIL_OUT_NG_SIZE");
			strData.Format("%.1lf", pRcpInfo->dFoilExpOutNgSize);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("DEFECT_FOIL_BOTH_NG_SIZE");
			strData.Format("%.1lf", pRcpInfo->dFoilExpBothNgSize);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);
			// 22.07.20 Ahn Add End

			// 22.11.21 Ahn Add Start - JUDGE_GRAY
			strKey = _T("DEFECT_FOIL_GRAY_SIZE");
			strData.Format("%.1lf", pRcpInfo->dFoileGraySize);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("DEFECT_SURFACE_GRAY_SIZE");
			strData.Format("%.1lf", pRcpInfo->dSurfaceGraySize);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);
			// 22.11.21 Ahn Add End

			strKey = _T("ENABLE_SAVE_TAB_IMAGE_BY_DEFECT_SIZE");
			strData.Format( _T("%d"), pRcpInfo->bSaveDefInTab);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("SAVE_TAB_IMAGE_BY_DEFECT_SIZE");
			strData.Format(_T("%.1lf"), pRcpInfo->dSaveDefSize );
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);

			// 22.01.20 Ahn Add Start
			strKey = _T("IGNORE_DETECT_DISTANCE_FROM_EDGE");
			strData.Format(_T("%.1lf"), pRcpInfo->dIgnoreDistance);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
			// 22.01.20 Ahn Add End

			// 22.03.07 Ahn Add Start
			strKey = _T("IGNORE_DETECT_SIZE");
			strData.Format(_T("%.1lf"), pRcpInfo->dIgnoreSize);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
			// 22.03.07 Ahn Add End

			// 22.05.24 Ahn Add Start
			strKey = _T("SURFACE_NG_SIZE");
			strData.Format(_T("%.1lf"), pRcpInfo->dSurfaceNgSize);
			SaveRecipePrameter( strRecipeName, strSection, strKey, strData, strFileName);
			// 22.05.24 Ahn Add End	
		}

		{
			// 22.07.15 Ahn Add Start
			strKey = _T("MARKING_USE_DEF_FOIL");
			strData.Format(_T("%d"), pRcpInfo->nMarkingUse[en_ModeFoilExp]);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("MARKING_USE_DEF_FOIL_OUT");
			strData.Format(_T("%d"), pRcpInfo->nMarkingUse[en_ModeFoilExpOut]);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("MARKING_USE_DEF_SURFACE");
			strData.Format(_T("%d"), pRcpInfo->nMarkingUse[en_ModeSurface]);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("MARKING_TYPE_DEF_FOIL");
			strData.Format(_T("%d"), pRcpInfo->nMarkingType[en_ModeFoilExp]);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("MARKING_TYPE_DEF_FOIL_OUT");
			strData.Format(_T("%d"), pRcpInfo->nMarkingType[en_ModeFoilExpOut]);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("MARKING_TYPE_DEF_SURFACE");
			strData.Format(_T("%d"), pRcpInfo->nMarkingType[en_ModeSurface]);
			// 22.07.15 Ahn Add End
		}
		// 22.08.09 Ahn Add Start
		{
			strKey = _T("CONTINUOUS_NG_COUNT");
			strData.Format(_T("%d"), pRcpInfo->nContinousNgCount);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("SECTOR_NG_ALARM_COUNT");
			strData.Format(_T("%d"), pRcpInfo->nAlarmCount);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName);

			strKey = _T("SECTOR_NG_STD_COUNT");
			strData.Format(_T("%d"), pRcpInfo->nSectorCount);
			SaveRecipePrameter(strRecipeName, strSection, strKey, strData, strFileName) ;
		}
		// 22.08.09 Ahn Add End
	}
	else {
		char buff[256];
		if (winFile.ExistFile(strFileName) == FALSE) {
			return -1;
		}
		else 
		{
			pRcpInfo->m_strRecipeName = strRecipeName;
			strSection = _T("TAB_INFO");
			strKey = _T("Tab_Width");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->TabCond.dTabWidth = atof(buff);

			strKey = _T("Tab_Pitch");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->TabCond.dTabPitch = atof(buff);

			strKey = _T("Round_Radius");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->TabCond.dRadius = atof(buff);

			// 22.06.08 Ahn Delete Start
			//strKey = _T("Round_Radius_H");
			//::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			//pRcpInfo->TabCond.dRadiusH = atof(buff);
			// 22.06.08 Ahn Delete Start

			strKey = _T("TOP_COAT_HEIGHT");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->TabCond.dCeramicHeight = atof(buff);

			strKey = _T("BOTTOM_COAT_HEIGHT");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->TabCond.dCeramicTailHeight = atof(buff);

			strKey = _T("TAB_COAT_HEIGHT");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->TabCond.dTabCeramicHeight = atof(buff);

			pRcpInfo->TabCond.nTabWidth = (int)( ( ( pRcpInfo->TabCond.dTabWidth / AprData.m_System.m_dResolY ) * 1000.0 ) + 0.5 ) ;
			pRcpInfo->TabCond.nTabPitch = (int)(((pRcpInfo->TabCond.dTabPitch / AprData.m_System.m_dResolY) * 1000.0) + 0.5);
			pRcpInfo->TabCond.nRadiusH = (int)(((pRcpInfo->TabCond.dRadius / AprData.m_System.m_dResolY) * 1000.0) + 0.5);
			pRcpInfo->TabCond.nRadiusW = (int)(((pRcpInfo->TabCond.dRadius / AprData.m_System.m_dResolX[ CAM_POS_TOP ]) * 1000.0) + 0.5);
			pRcpInfo->TabCond.nCeramicHeight = (int)(((pRcpInfo->TabCond.dCeramicHeight / AprData.m_System.m_dResolX[CAM_POS_TOP]) * 1000.0) + 0.5);
			pRcpInfo->TabCond.nTabCeramicHeight = (int)(((pRcpInfo->TabCond.dTabCeramicHeight / AprData.m_System.m_dResolX[CAM_POS_TOP]) * 1000.0) + 0.5);
			pRcpInfo->TabCond.nCeramicTailHeight = (int)(((pRcpInfo->TabCond.dCeramicTailHeight / AprData.m_System.m_dResolX[CAM_POS_BOTTOM]) * 1000.0) + 0.5);

			// 23.02.14 Ahn Add Start
			strKey.Format(_T("TAB_MIN_BRIGHT"));
			::GetPrivateProfileString(strSection, strKey, "100", buff, 256, strFileName);
			pRcpInfo->TabCond.nTabMinBright = atoi(buff);
			// 23.02.14 Ahn Add End

			for (int i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format( _T("COAT_BRIGHT_HIGH_%d"), i ) ;
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->TabCond.nCeramicBrightHigh[i] = atoi(buff);

				strKey.Format( _T("COAT_BRIGHT_LOW_%d"), i ) ;
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->TabCond.nCeramicBrightLow[i] = atoi(buff);

				strKey.Format(_T("ROLL_BRIGHT_HIGH_%d"), i);
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->TabCond.nRollBrightHigh[i] = atoi(buff);

				strKey.Format(_T("ROLL_BRIGHT_LOW_%d"), i);
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->TabCond.nRollBrightLow[i] = atoi(buff);

				strKey.Format( _T("EDGE_SEARCH_MODE_%d"), i ) ;
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->TabCond.nEdgeFindMode[i] = atoi(buff);

				// 23.02.14 Ahn Add Start
				strKey.Format(_T("ROLL_BRIGHT_MODE_%d"), i);
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->TabCond.nRollBrightMode[i] = atoi(buff);;
				// 23.02.14 Ahn Add End

			}

			// 22.01.05 Ahn Add Start
			strKey = _T("NEGATIVE_TAB_COAT_HEIGHT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->TabCond.nNegCoatHeight = atoi( buff );

			strKey = _T("NEGATIVE_V_GROOVE_HEIGHT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->TabCond.nNegVGrooveHeight = atoi( buff ) ;
			// 22.01.05 Ahn Add End
		}
		{
			strSection = _T("INSP_CONDITION");

			for (int i = 0; i < MAX_LIGHT_UNIT; i++) {
				for (int j = 0; j < MAX_LIGHT_CHANEL; j++) {
					strKey.Format(_T("LIGHT_UNIT_%02d_CH_%02d"), i, j);
					::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
					pRcpInfo->nLightLevel[i][j] = atoi(buff);
				}
			}
			for (int i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format( _T("DROSS_THRES_LOWER_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nDrossThresLower[i] = atoi(buff);

				strKey.Format( _T("DROSS_THRES_UPPER_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nDrossThresOrigin[i] = atoi(buff);

				strKey.Format( _T("FOIL_THRES_LOWER_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nFoilExpThresLower[i] = atoi(buff);

				strKey.Format( _T("FOIL_THRES_UPPER_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nFoilExpThresOrigin[i] = atoi(buff);

				strKey.Format( _T("DROSS_INSP_WIDTH_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
				pRcpInfo->dFoilOutInspWidth[i] = atof(buff);
				pRcpInfo->nFoilOutInspWidth[i] = (int)((pRcpInfo->dFoilOutInspWidth[i] / AprData.m_System.m_dResolX[i]) * 1000.0);

				strKey.Format( _T("FOIL_INSP_WIDTH_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
				pRcpInfo->dFoilExpInspWidth[i] = atof(buff);
				pRcpInfo->nFoilExpInspWidth[i] = (int)((pRcpInfo->dFoilExpInspWidth[i] / AprData.m_System.m_dResolX[i]) * 1000.0);

				strKey.Format( _T("DROSS_MASK_OFFSET_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nFoilOutMaskOffset[i] = atoi(buff);

				strKey.Format( _T("FOIL_MASK_OFFSET_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nFoilExpMaskOffset[i] = atoi(buff);

				strKey.Format(_T("MAGNIFICATION_CAM_%01d"), i);
				::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
				pRcpInfo->dMagnification[i] = atof( buff) ;

				strKey.Format( _T("FOIL_OUT_MIN_SIZE_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nFoilExpOutMinSize[i] = atoi(buff);

				strKey.Format( _T("FOIL_IN_MIN_SIZE_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nFoilExpInMinSize[i] = atoi(buff);

				strKey.Format( _T("DROSS_LINK_X_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->pntLinkRange[i][TYPE_FOILEXP_OUT].x = atoi(buff);

				strKey.Format( _T("DROSS_LINK_Y_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->pntLinkRange[i][TYPE_FOILEXP_OUT].y = atoi(buff);

				strKey.Format( _T("FOILEXP_LINK_X_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->pntLinkRange[i][TYPE_FOILEXP].x = atoi(buff);

				strKey.Format( _T("FOILEXP_LINK_Y_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->pntLinkRange[i][TYPE_FOILEXP].y = atoi(buff);

				strKey.Format( _T("ENABLE_DEFECLINK_CAM_%01d"), i );
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->bEnableDefectLink[i] = atoi(buff);

				// 22.05.10 Ahn Add Start
				strKey.Format( _T("DISABLE_SPETTER_INSPECTION"), i ) ;
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->bDisableSurface = atoi(buff) ;

				strKey.Format(_T("SPETTER_THRESHOLD_CAM_%01d"), i);
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nThresSurface[i] = atoi(buff) ;

				strKey.Format(_T("SPETTER_MASK_OFFSET_CAM_%01d"), i);
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->nSurfaceMaskOffset[i] = atoi(buff);
				// 22.05.10 Ahn Add End
				// 
				// 22.06.08 Ahn Add Start
				strKey.Format(_T("SURFACE_MIN_SIZE_%01d"), i);
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				pRcpInfo->m_nSurfaceMinSize[i] = atoi(buff) ;
				// 22.06.08 Ahn Add End

			}

			strKey = _T("BMP_SAVE_INTERVAL");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nBmpSaveInterval = atoi(buff);

			strKey = _T("SAVE_IMAGE_ONLY_NG_TAB");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->bSaveOnlyNgTab = atoi(buff);

			strKey = _T("ENABLE_DISABLE_DIFF_PROCESS");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->bDisableProcDiff = atoi(buff);

			strKey = _T("NG_TAB_STOP");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->bNgStop = atoi(buff) ;

			strKey = _T("DARK_EMP_MODE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->bDarkEmpMode = atoi(buff);
		}


		{
			strSection = _T("JUDGEMENT");

			strKey = _T("JUDGE_MODE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nJudgeMode  = atoi(buff);

			//for (int nType = 0; nType < MAX_INSP_TYPE; nType++) {
			//	for (int nRank = 0; nRank < MAX_RANK; nRank++) {
			//		for (int nCond = 0; nCond < CRecipeInfo::en_RankCond_Max; nCond++) {
			//			strKey.Format("%s_%s_%s", chType[nType], chRank[nRank], chRankCond[nCond]);
			//			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			//			pRcpInfo->nRank[ nType ][nRank][nCond] = atoi(buff);

			//		}
			//	}
			//}

			strKey = _T("DEFECT_NG_SIZE_WIDTH");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->dFoilExpInNgSize = atof( buff) ;

			strKey = _T("DEFECT_NG_SIZE_Height");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->dDefJudgeHeight = atof(buff);

			// 22.07.20 Ahn Add Start
			strKey = _T("DEFECT_FOIL_OUT_NG_SIZE");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->dFoilExpOutNgSize = atof(buff);

			strKey = _T("DEFECT_FOIL_BOTH_NG_SIZE");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->dFoilExpBothNgSize = atof(buff);
			// 22.07.20 Ahn Add End

			// 22.11.21 Ahn Add Start - JUDGE_GRAY
			strKey = _T("DEFECT_FOIL_GRAY_SIZE");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->dFoileGraySize = atof(buff);

			strKey = _T("DEFECT_SURFACE_GRAY_SIZE");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->dSurfaceGraySize = atof(buff);
			// 22.11.21 Ahn Add End

			strKey = _T("ENABLE_SAVE_TAB_IMAGE_BY_DEFECT_SIZE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->bSaveDefInTab = atoi(buff);

			strKey = _T("SAVE_TAB_IMAGE_BY_DEFECT_SIZE");
			::GetPrivateProfileString(strSection, strKey, "0.0", buff, 256, strFileName);
			pRcpInfo->dSaveDefSize = atof(buff);

			// 22.01.20 Ahn Add Start
			strKey = _T("IGNORE_DETECT_DISTANCE_FROM_EDGE");
			::GetPrivateProfileString(strSection, strKey, "100.0", buff, 256, strFileName);
			pRcpInfo->dIgnoreDistance = atof(buff);
			// 22.01.20 Ahn Add End

			// 22.03.07 Ahn Add Start
			strKey = _T("IGNORE_DETECT_SIZE");
			::GetPrivateProfileString(strSection, strKey, "61.0", buff, 256, strFileName);
			pRcpInfo->dIgnoreSize = atof(buff);
			// 22.03.07 Ahn Add End

			// 22.05.24 Ahn Add Start
			strKey = _T("SURFACE_NG_SIZE");
			::GetPrivateProfileString(strSection, strKey, "61.0", buff, 256, strFileName);
			pRcpInfo->dSurfaceNgSize = atof(buff);
			// 22.05.24 Ahn Add End			
		}

		{
			// 22.07.15 Ahn Add Start
			strKey = _T("MARKING_USE_DEF_FOIL");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nMarkingUse[en_ModeFoilExp] = atoi(buff);

			strKey = _T("MARKING_USE_DEF_FOIL_OUT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			 pRcpInfo->nMarkingUse[en_ModeFoilExpOut] = atoi(buff);

			strKey = _T("MARKING_USE_DEF_SURFACE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			 pRcpInfo->nMarkingUse[en_ModeSurface] = atoi(buff);

			strKey = _T("MARKING_TYPE_DEF_FOIL");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nMarkingType[en_ModeFoilExp] = atoi(buff);

			strKey = _T("MARKING_TYPE_DEF_FOIL_OUT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nMarkingType[en_ModeFoilExpOut] = atoi(buff);

			strKey = _T("MARKING_TYPE_DEF_SURFACE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nMarkingType[en_ModeSurface] = atoi(buff);
			// 22.07.15 Ahn Add End
		}

		// 22.08.09 Ahn Add Start
		{
			strKey = _T("CONTINUOUS_NG_COUNT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nContinousNgCount = atoi(buff);

			strKey = _T("SECTOR_NG_ALARM_COUNT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			pRcpInfo->nAlarmCount = atoi(buff);

			strKey = _T("SECTOR_NG_STD_COUNT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 10000, strFileName);
			pRcpInfo->nSectorCount = atoi(buff);
		}
		// 22.08.09 Ahn Add End
	}

	return 0;
}
int CRecipeCtrl::LoadRecipe(CString strRecipeName)
{
	FileCtrl(strRecipeName, MODE_READ, &m_RcpInfo);
	return 0;
}
int CRecipeCtrl::SaveRecipe(CString strRecipeName)
{
	FileCtrl(strRecipeName, MODE_WRITE);
	return 0;
}
int CRecipeCtrl::LoadRecipe(CRecipeInfo* pRecipe, CString strRecipeName)
{
	FileCtrl(strRecipeName, MODE_READ, pRecipe);
	return 0;
}
int CRecipeCtrl::SaveRecipe(CRecipeInfo* pRecipe, CString strRecipeName)
{
	FileCtrl(strRecipeName, MODE_WRITE, pRecipe);
	return 0;
}
int CRecipeCtrl::GetRecipeList(CStringList& strRecipeNameList)
{
	return 0;
}

void CRecipeCtrl::SetRecipeInfo(CRecipeInfo *pRcpInfo)
{
	m_RcpInfo = *pRcpInfo;
}

int CRecipeCtrl::SearchRecipeFile(CStringList& strRecipeList)
{
	strRecipeList.RemoveAll();

	CString strRecipeDir;
	strRecipeDir.Format("%s\\Recipe\\", AprData.m_strDataPath );

	CWin32File winFile;

	winFile.GetFileList(strRecipeDir, strRecipeList );
	return 0;
}

// 22.03.25 Ahn Add Start
BOOL CRecipeCtrl::IsEnableRecipe(CString strRcpName)
{
	CStringList strRcpList;
	SearchRecipeFile(strRcpList);

	BOOL bRet = FALSE ;

	POSITION pos;
	CString strTemp;
	for (pos = strRcpList.GetHeadPosition(); pos != NULL; ) {
		strTemp = strRcpList.GetNext(pos);
		strTemp.Replace(_T(".ini"), _T(""));
		if (strTemp.Compare(strRcpName) == 0) {
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

// 22.03.25 Ahn Add End

// 22.06.30 Ahn Add Start
BOOL CRecipeCtrl::IsAvailableRecipe(CRecipeInfo* pRcpInfo)
{
	ASSERT(pRcpInfo);

	BOOL bRet = TRUE;

	int nCamNo = 0;
	CString strLog = _T("");

	// 22.09.15 Ahn Modify Start
	//if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	// 22.09.15 Ahn Modify End
		if (pRcpInfo->TabCond.nNegVGrooveHeight < 0) {
			bRet = FALSE;
		}

		if (pRcpInfo->TabCond.nNegCoatHeight < 0) {
			bRet = FALSE;
		}
	}

	if (pRcpInfo->TabCond.nTabPitch <= 0) {
		bRet = FALSE;
	}
	if (pRcpInfo->TabCond.nTabWidth <= 0) {
		bRet = FALSE;
	}

	for (nCamNo = 0; nCamNo < MAX_CAMERA_NO; nCamNo++) {
		if (pRcpInfo->dMagnification[nCamNo] <= 0.0) {
			bRet = FALSE;
			break;
		}

		if (pRcpInfo->dFoilExpInspWidth[nCamNo] <= 0.0) {
			bRet = FALSE;
			break;
		}

		if (pRcpInfo->nFoilExpThresOrigin[nCamNo] <= 0.0) {
			bRet = FALSE;
			break;
		}


	}


	return bRet;
}
// 22.06.30 Ahn Add End
