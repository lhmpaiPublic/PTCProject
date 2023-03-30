#pragma once


// CRecipeSelectDlg 대화 상자
#include "CRecipeTableCtrl.h"

class CRecipeSelectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRecipeSelectDlg)

public:
	CRecipeSelectDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRecipeSelectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RECIPE_SELECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClickGridCtrlRecipeTable(NMHDR* pNMHDR, LRESULT* pResult);

	CListBox m_ListRecipeName;
	virtual BOOL OnInitDialog();

private :
	CString m_strSelectedRcpName;
	CGridCtrl m_GridRecipeTable;
	CFont* m_pFontGrid;
	CRecipeTableCtrl m_RcpTableCtrl;
	
	BOOL m_bAvailableRecipe;

	int MakeGridCtrl_RecipeTable();
	int UpdateGrid_RecipeTable();

public :
	CString GetSeletedName();
	CString m_strRecipeName;
};
