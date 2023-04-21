#include "pch.h"
#include "CRecipeTableCtrl.h"
#include "Win32File.h"
#include "GlobalData.h"
#include "LogDisplayDlg.h"

int CRecipeTableCtrl::FileCtrl(int nMode)
{

	CString strFileName;
	strFileName.Format(_T("%s\\RecipeTable.ini"), AprData.m_strDataPath); 

	//로그출력
	LOGDISPLAY_SPEC(0)("Recipe Table File 경로 : %s", strFileName);

	CString strKey;
	CString strSection;
	CString strData;

	//레시피 테이블(정보관리) 파일에 쓰기(RecipeTable.ini)
	if (nMode == MODE_WRITE) 
	{
		//로그출력
		LOGDISPLAY_SPECTXT(0)("Recipe Table File 쓰기");

		for (int i = 0; i < MAX_RECIPE_TABLE; i++) 
		{
			strSection.Format( _T("RECIPE_NO_%d"), i + 1) ;
			strKey = _T("RECIPE_NAME");
			strData.Format(_T("%s"), m_RecipeTable[i].strRecipeName);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("MEMO");
			strData.Format(_T("%s"), m_RecipeTable[i].strMemo );
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
		}
	}
	//레시피 테이블 파일에서 읽기
	else 
	{
		//로그출력
		LOGDISPLAY_SPECTXT(0)("Recipe Table File 읽기");

		char buff[256];
		for (int i = 0; i < MAX_RECIPE_TABLE; i++) 
		{
			strSection.Format(_T("RECIPE_NO_%d"), i + 1);

			//로그출력
			LOGDISPLAY_SPEC(0)("[%s]", strSection);

			strKey = _T("RECIPE_NAME");
			memset(buff, 0, 256);
			::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);

			//레시피 테이블에서읽은 레시피 명
			m_RecipeTable[i].strRecipeName.Format( _T("%s"), buff  ) ;

			//로그출력
			LOGDISPLAY_SPEC(0)("RECIPE_NAME : %s", buff);

			strKey = _T("MEMO");
			memset(buff, 0, 256);
			::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);

			//레시피 테이블에서 읽은 레이피 메모
			m_RecipeTable[i].strMemo.Format(_T("%s"), buff );

			//로그출력
			LOGDISPLAY_SPEC(0)("MEMO : %s", buff);

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
