#include "pch.h"
#include "CRecipeTableCtrl.h"
#include "Win32File.h"
#include "GlobalData.h"

int CRecipeTableCtrl::FileCtrl(int nMode)
{

	CString strFileName;
	strFileName.Format(_T("%s\\RecipeTable.ini"), AprData.m_strDataPath); 
	CString strKey;
	CString strSection;
	CString strData;

	if (nMode == MODE_WRITE) {

		for (int i = 0; i < MAX_RECIPE_TABLE; i++) {
			strSection.Format( _T("RECIPE_NO_%d"), i + 1) ;
			strKey = _T("RECIPE_NAME");
			strData.Format(_T("%s"), m_RecipeTable[i].strRecipeName);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("MEMO");
			strData.Format(_T("%s"), m_RecipeTable[i].strMemo );
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
		}
	}
	else {
		char buff[256];
		for (int i = 0; i < MAX_RECIPE_TABLE; i++) {
			strSection.Format(_T("RECIPE_NO_%d"), i + 1);
			strKey = _T("RECIPE_NAME");
			::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
			m_RecipeTable[i].strRecipeName.Format( _T("%s"), buff  ) ;

			strKey = _T("MEMO");
			::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
			m_RecipeTable[i].strMemo.Format(_T("%s"), buff );

		}
	}

	return 0;
}

CRecipeTableInfo *CRecipeTableCtrl::GetRecipeTableInfo(int nNo)
{
	if ((nNo < 0) || (nNo >= MAX_RECIPE_TABLE)) {
		return NULL;
	}

	return &m_RecipeTable[nNo];
}

int CRecipeTableCtrl::GetRecipeNo(CString strRecipeName)
{
	int nRet = -1;

	if (strRecipeName.GetLength() <= 0) return -2;

	for (int i = 0; i < MAX_RECIPE_TABLE; i++) {
		if (strRecipeName.Compare(m_RecipeTable[i].strRecipeName) == 0) {
			nRet = i + 1 ;
			break;
		}
	}


	return nRet;

}
