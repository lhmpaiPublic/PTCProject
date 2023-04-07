#pragma once

#include "ColorControl.h"

// CInspInfoDlg 대화 상자
class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CInspInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInspInfoDlg)

public:
	CInspInfoDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다..
	virtual ~CInspInfoDlg();

protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;

public:
//	CFont			m_fontUnitID;
	CFont			m_fontEngMode;
//	CColorStatic	m_strUnitID;
//	CColorStatic	m_strEngMode;
	CColorStatic	m_stLotID;
	CColorStatic	m_stNumTotal;
	CColorStatic	m_stNumOK;
	CColorStatic	m_stNumNG;
	//CColorStatic	m_stLight1Channel1;
	//CColorStatic	m_stLight1Channel2;
	//CColorStatic	m_stLight2Channel1;
	//CColorStatic	m_stLight2Channel2;
	// 22.12.13 Ahn Modify Start
	//CColorStatic	m_stMemo;
	CColorStatic	m_stNumYield;
	// 22.12.13 Ahn Modify End

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INSP_INFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//CComboBox m_cmbRecipeSelect;
//	afx_msg void OnCbnSelchangeCmbRecipeSelect();

public :

	void ReflashAll();
	int LoadRecipeList();
	//afx_msg void OnBnClickedBtnLightCtrl();
	//afx_msg void OnBnClickedBtnModeChange();
//	afx_msg void OnBnClickedBtnRecipeSelect();
	CString m_strEdRecipeName;
//	CString m_strEdNextRecipeName;
// 22.05.25 Son Add Start
	CColorStatic m_strUserMode;
	afx_msg void OnBnClickedBtnUserChange();
// 22.05.25 Son Add End
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnRecipeSelect();
	CString m_strNextRecipeName;

	void DisplayLanguage();
};
