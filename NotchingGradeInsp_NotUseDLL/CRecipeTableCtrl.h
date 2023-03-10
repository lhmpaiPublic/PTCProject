#pragma once


class CRecipeTableInfo {
public:
	int nNo;
	BOOL bUse;
	CString strRecipeName;
	CString strMemo;
};

class CRecipeTableCtrl
{

public :
	int FileCtrl(int nMode);

	CRecipeTableInfo* GetRecipeTableInfo(int nNo);
	int GetRecipeNo(CString strRecipeName);
	

private :
	CRecipeTableInfo m_RecipeTable[MAX_RECIPE_TABLE] ;

};

