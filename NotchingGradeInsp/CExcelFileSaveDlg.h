#pragma once

#include "CExcelResultFileCtrl.h"
// CExcelFileSaveDlg 대화 상자

class CExcelFileSaveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExcelFileSaveDlg)

public:
	CExcelFileSaveDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CExcelFileSaveDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_EXCEL_SAVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ExcelPrgCtrl;
	CString m_EdStrSavePath;
	afx_msg void OnBnClickedBtnSetPath();
	afx_msg void OnBnClickedBtnSave();

	CExcelResultFileCtrl *m_pExcelCtrl ;

	void CopyData(CExcelResultFileCtrl::VEC_DEF_SEARCH_LIST vecSearchList);
	CExcelResultFileCtrl::VEC_DEF_SEARCH_LIST m_VecDefSearchList;
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public :
	void SetProgressTimer();
	void KillProgressTimer();
	UINT_PTR m_TID_Progress; // = SetTimer(T_ID_IO_CHECK, 10, NULL);

	// 23.01.09 Ahn Add Start
	void SetSaveFilePath(CString strFilePath);
	// 23.01.09 Ahn Add End

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();

	//23.01.10 Ahn Add Start
	BOOL m_bProcStart; 
	//23.01.10 Ahn Add End
};
