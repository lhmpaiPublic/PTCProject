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

	CListBox m_ListRecipeName;
	virtual BOOL OnInitDialog();

private :
	//레시피 파일 목록을 얻는다.
	void LoadRecipeFileList();
public :
	//선택된 레시피명
	CString m_strRecipeName;
	//선택된 레시피의 메모
	CString m_strRecipeMemo;
	//레시피 명 콤보박스 컨트롤 제어 객체
	CComboBox m_RecipeNameCtrl;
	afx_msg void OnBnClickedButRecipedelete();
	afx_msg void OnCbnSelchangeComboRecipeName();
};
