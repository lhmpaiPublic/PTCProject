#pragma once

#include "CDefectDataCtrl.h"
//KANG 22.01.07 Add Start
#include "CalenderCtrl.h"
#include "ThumbnailCtrl.h"
//KANG 22.01.07 Add End
//KANG 22.05.24 Add Start
#include "GridCtrl.h"
#include "CImageViewerDlg.h"
#include "CImageProcess.h"
//KANG 22.05.24 Add End
// 22.05.27 Ahn Add Start
#include "ColorControl.h"
#include "CExcelResultFileCtrl.h"
// 22.05.27 Ahn Add End
//KANG 22.06.17 Add Start
#include "GraphCtrl.h"
//KANG 22.06.17 Add End


#define	MAX_DISP_IMAGE_NUM	128
//KANG 22.05.24 Add Start
#define IDC_SEARCH_LIST 1002
//KANG 22.05.24 Add End
//KANG 22.06.17 Add Start
#define MAX_DEFECT_IN_TAB_NUM 5
//KANG 22.06.17 Add End

//KANG 22.01.07 Add Start
enum {
	enHistoryCalView = 0,
	enHistoryImgView = 1,
	enMaxHistoryView,
};
//KANG 22.01.07 Add End

// CHistoryDlg 대화 상자
class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CImageProcSimDlg;
class CExcelFileSaveDlg;

class CHistoryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHistoryDlg)

public:
	CHistoryDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다.
	virtual ~CHistoryDlg();

protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;
	CDefectDataCtrl *m_pDefData;
//KANG 22.01.07 Add Start
	CFont			m_fontHeader;
	CColorStatic	m_stHeader;
	BOOL			m_nImageType;	//0: NG Crop    1: NG      2: OK Crop     3: OK
//KANG 22.01.07 Add Start
//KANG 22.05.24 Add Start
	CGridCtrl* m_pSearchList;
	CFont* m_pFontSearchList;
//KANG 22.05.24 Add End

	CExcelFileSaveDlg *m_pExcelDlg;

	int m_startRow;
private:
	CImageProcSimDlg* m_pImageSimDlg;
	void OpenSimDlg();
//KANG 22.01.07 Add Start
	void SetTextHeader(COleDateTime dtDate);
	void SetupItems();
	long nDataItemID;
	CImageList m_ImgList;
	CCalenderCtrl m_CalCtrl;
	CThumbnailCtrl m_ThumbnailCtrl;
	void SetSelectCell(COleDateTime dtDate);
	int m_nDispMode;				//0: Calc		1: Thumbnail
	CString m_strImageFilePath[4];	//0: NG Crop    1: NG      2: OK Crop     3: OK
	int m_nImageFileCount[4];
//KANG 22.01.07 Add End
//KANG 22.05.24 Add Start
	CDateTimeCtrl m_dtPickStart;
	CDateTimeCtrl m_dtPickEnd;
	CComboBox m_cmbTimeStart;
	CComboBox m_cmbTimeEnd;

//	typedef std::vector<CDefectSearchList*> VEC_DEF_SEARCH_LIST;
	CExcelResultFileCtrl::VEC_DEF_SEARCH_LIST * m_pVtList;

	int m_nSearchMethod;
	int m_nSelectRow;
	CImageViewerDlg* m_pImageViewerDlg;

	void FreeSearchList(std::vector<CDefectSearchList*>* pVector);
	static UINT FreeAsyncThread(LPVOID param);
	void CreateExpandView(LPCTSTR lpszImageFileName);
	void DestroyExpandView();
//KANG 22.05.24 Add End
 
	// 22.02.11 Ahn Add Start
	enum {
		en_NgCrop = 0,
		en_NgImage,
		en_OkCrop,
		en_OkImage,
		en_ImageAllType,
	};
	// 22.02.11 Ahn Add End

//KANG 22.05.24 Add Start
	enum {
		en_ListCol_No = 0,
		en_ListCol_Time,
		en_ListCol_LotID,
		en_ListCol_Model,
		en_ListCol_Lane,
		en_ListCol_Cell,
		en_ListCol_TopBottom,
		en_ListCol_Type,
		en_ListCol_Rank,
		en_ListCol_XSize,
		en_ListCol_YSize,
		en_ListCol_Marking,
		en_ListCol_Link,
		en_MaxListCol
	};
//KANG 22.05.24 Add End

	// 22.02.11 Ahn Add Start
	CStringList m_strImageList[en_ImageAllType];
	// 22.02.11 Ahn Add End

//KANG 22.06.17 Add Start
	CGraphCtrl* m_pGraphHist;
	BOOL		m_bShowGraphHist;
	int			m_nGraphHistHorSize;
	int			m_nGraphHistVerSize;
//KANG 22.06.17 Add End

//KANG 22.05.24 Add Start
protected:
	int AddDefectSearchList(LPCTSTR lpszSerchFileName);
//KANG 22.05.24 Add End
//KANG 22.05.27 Add Start
	CString m_strSearchLotID;
	int	GetFileList(LPCTSTR lpszPath, CStringList& rstrFileNameList, LPCTSTR lpszExt = _T("*"), int nMaxFiles = 0, BOOL bSubFolder = FALSE, BOOL bIncludePath = FALSE, BOOL bListClear = TRUE);
//KANG 22.05.27 Add End

public:
//KANG 22.01.07 Add Start
	int OnRefresh(int nKind = 0);
	int ReadThumbnailfileInFolder(LPCTSTR lpszPath = _T(""));

	int m_nCalcViewType;
	int m_nCurMonth;
//KANG 22.01.07 Add End
//KANG 22.05.24 Add Start
	void SearchListClear();
	void AddSearchListData(CDefectSearchList* pList);
	int GetSearchListCount();
	int MakeGridSearchList();
	CDefectSearchList* GetSearchListData(int nIndex);
//KANG 22.05.24 Add End

	// 22.02.11 Ahn Add Start
	//int ReadThumbnailFromErrorData( CString strPath, CString strMonth, CString strDay, CString strLotID );
	int ReadThumbnailFromErrorData(CString strPath, CString strYear, CString strMonth, CString strDay, CString strLotID);
	// 22.02.11 Ahn Add End

//KANG 22.06.17 Add Start
	void AddGraphHistData(int nTab, int nType, double dData);
	int GetGraphIntervalSize(int nNum);
//KANG 22.06.17 Add End

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_HISTORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
//KANG 22.01.07 Add Start
	afx_msg void OnCellClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCellDblClick(NMHDR* pNotifyStruct, LRESULT* pResult);
//KANG 22.01.07 Add End

public:
	void SetDefectCtrlPtr(CDefectDataCtrl* pCtrl);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
//KANG 22.01.07 Add Start
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnToday();
	afx_msg void OnBnClickedRdMonth();
	afx_msg void OnBnClickedRdWeek();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedRdNgCrop();
	afx_msg void OnBnClickedRdNg();
	afx_msg void OnBnClickedRdOkCrop();
	afx_msg void OnBnClickedRdOk();
	afx_msg void OnNotifySimulation(NMHDR* pNMHDR, LRESULT* pResult);
//KANG 22.01.07 Add End
//KANG 22.05.24 Add Start
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnBnClickedRdLotid();
	afx_msg void OnBnClickedRdTime();
	afx_msg void OnBnClickedBtnExport();
	afx_msg void OnMouseLDownSearchList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseDblClickSearchList(NMHDR* pNMHDR, LRESULT* pResult);
//KANG 22.05.24 Add End
	afx_msg void OnCbnSelchangeCmbTimeStart();
//KANG 22.06.17 Add Start
	afx_msg void OnCellChangingSearchList(NMHDR* pNMHDR, LRESULT* pResult);
//KANG 22.06.17 Add End
	afx_msg void OnBnClickedBtnClear();
};
