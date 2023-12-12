#pragma once

#include "GridCtrl.h"
// CRecipeSettingDlg 대화 상자
#include "CRecipeTableCtrl.h"

class CRecipeInfo;
class CRecipeCtrl;
class CRecipeSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRecipeSettingDlg)

public:
	CRecipeSettingDlg( BOOL bRcpSelMode, CRecipeInfo* pRecipeInfo, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRecipeSettingDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RECIPE_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	//CGridCtrl m_GridCtrlTabInfo;
	//CGridCtrl m_GridCtrlInspCond;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnSetfocusEdTabWidthMm();
	afx_msg void OnEnSetfocusEdTabPitchMm();
	afx_msg void OnEnSetfocusEdTabRadiusMm();
	afx_msg void OnEnSetfocusEdTopCoatHeightMm();
	afx_msg void OnEnSetfocusEdBtmCoatHeightMm();
	afx_msg void OnEnSetfocusEdTabCoatHeightMm();
	afx_msg void OnEnSetfocusEdCoatBright();
	afx_msg void OnEnSetfocusEdRollBright();
	afx_msg void OnEnSetfocusEdCoatBrightMax();
	afx_msg void OnEnSetfocusEdRollBrightMax();
	afx_msg void OnEnSetfocusEdFoilThresUpper();
	afx_msg void OnEnSetfocusEdDrossThresUpper();
	afx_msg void OnEnSetfocusEdFoilThresLow();
	afx_msg void OnEnSetfocusEdDrossThresLow();
	afx_msg void OnEnSetfocusEdMinSizeFoil();
	afx_msg void OnEnSetfocusEdMinSizeDross();
	afx_msg void OnEnSetfocusEdMaskOffsetFoil();
	afx_msg void OnEnSetfocusEdMaskOffsetDross();
	afx_msg void OnEnSetfocusEdInspRangeIn();
	afx_msg void OnEnSetfocusEdInspRangeOut();

private:

	BOOL m_bRcpSelMode;
	CEdit m_EdTemp;
	double m_dTabWidth;
	double m_dTabPitch;
	double m_dRadius;
	double m_dTopCoatH;
	double m_dBtmCoatH;
	double m_dTabCoatH;

	int m_nCoatBrightMin;
	int m_nRollBrightMin;
	int m_nCoatBrightMax;
	int m_nRollBrightMax;


	int m_nTabWidthPix;
	int m_nTabPitchPix;
	int m_nRadiusPixW;
	int m_nRadiusPixH;
	int m_nTopCoatPixH;
	int m_nBtmCoatPixH;
	int m_nTabCoatPixH;
	// 22.01.11 Ahn Modify Strat
	//int m_nThresFoilUpper;
	//int m_nThresDrossUpper;
	int m_nThresFoilOrigin;
	int m_nThresDrossOrigin;
	// 22.01.11 Ahn Modify End
	int m_nThresFoilLower;
	int m_nThresDrossLower;

	int m_nMinSizeFoil;
	int m_nMinSizeDross;
	int m_nMinSizeBoth ;// 22.07.19 Ahn Add

	int m_nMaskOffsetFoil;
	int m_nMaskOffsetDross;
	double m_dInspWidthFoil;
	double m_dInspWidthDross;

	int m_nInspWidthFoil;
	int m_nInspWidthDross;

	int m_nFoilLinkX;
	int m_nFoilLinkY;
	int m_nDrossLinkX;
	int m_nDrossLinkY;
	CPoint m_pntDefLink[MAX_INSP_TYPE];

	CGridCtrl m_GridCtrlLight;
	CGridCtrl m_GridRecipeTable;
	CGridCtrl m_GridMarking; // 22.07.15 Ahn Add

	int SetValue(int nValue, CString strMsg, int nMax, int nMin);
	double SetValue(double nValue, CString strMsg, double nMax, double nMin);

	void DataControl(int nMode, CRecipeInfo *pRecipeInfo );

	CRecipeInfo* m_pRecipeInfo;
	CRecipeCtrl* m_pRecipeCtrl;

	CFont* m_pFontGrid;

	CString m_strSelectRcpName;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnClickGridCtrlLight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickGridCtrlRecipeTable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeCmbRecipeName();
//	afx_msg void OnClickGridCtrlMarking(NMHDR* pNMHDR, LRESULT* pResult); // 22.07.15 Ahn Add 
	afx_msg void OnMouseDblClickMarkingList(NMHDR* pNMHDR, LRESULT* pResult);	 // 22.06.23 Ahn Add 

	enum {
		en_Judge_Cols = 3,
		en_Light_Rows = MAX_LIGHT_UNIT + 1,
		en_Light_Cols = 3,

	};

	enum {
		en_Rank_FoilExp_OK = 0,
		en_Rank_FoilExp_NG = 1,
		en_Rank_Dross_OK = 2,
		en_Rank_Dross_NG = 3,
		en_Rank_Max_Type = 4,
		en_Rank_Rows = 5,
	};

	enum {
		en_RankCond_Size = 0,
		en_RankCond_Width = 1,
		en_RankCond_Length = 2,
		en_RankCond_LongLen = 3,
		en_RankCond_MaxBri = 4,
		en_RankCond_MAX = 5,
		en_RankCond_MaxCols = 6,
	};

	enum {
		en_RcpTable_No = 0,
		en_RcpTable_Name,
		en_RcpName_Memo,
		en_RcpName_MaxCols,
	};

	// 22.07.15 Ahn Add Start
	enum {
		en_MarkingTitle = 0,
		en_MarkingUse = 1,
		en_MarkingType = 2,
		en_MarkingGrid_Cols = 3,

		en_MarkingFoilExp = 1,
		en_MarkingFoilExpOut = 2, 
		en_MarkingSurface = 3,
		en_MarkingGrid_Rows = 4,
	};

	int MakeGridCtrl_Marking();
	int UpdateGrid_Marking();
	// 22.07.15 Ahn Add End
	int MakeGridCtrl();
	int UpdateGrid();
	int MakeGridCtrl_RecipeTable();
	int UpdateGrid_RecipeTable();

	CTabCtrl m_TabDetectCond;

	void LoadRecipeTable();
	void SaveRecipeTable();

	CString GetSelectName() { return m_strSelectRcpName; };



private :
	int m_nLastSelTab;
	CRecipeTableCtrl m_RcpTableCtrl;
	BOOL m_bRecipeTableChanged;

	void ShowControl();

	// 22.09.05 Ahn Add Start
	void DisplayLanguage();
	// 22.09.05 Ahn Add End

public:
	// 22.07.15 Ahn Delete Start
	//afx_msg void OnBnClickedRadMaxDiff();
	//afx_msg void OnBnClickedRadSetBright();
	// 22.07.15 Ahn Delete End
	//int m_nRadEdgeDetectMode;
	int m_nBmpSaveInterval;
	afx_msg void OnEnSetfocusEdBmpSaveInterval();
	int m_nLastSelTabInfo;
	CTabCtrl m_TabInfo;
	afx_msg void OnTcnSelchangeTabInfo(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL m_bChkSaveOnlyNgTab;
	afx_msg void OnBnClickedChkSaveOnlyNgTab();
	BOOL m_bChkDisableProcDiff;
	afx_msg void OnBnClickedChkDisableProcDiff();
	double m_dEdMagnification;
	afx_msg void OnEnSetfocusEdMagnification();
	BOOL m_bChkEnableDefLink;
	afx_msg void OnBnClickedChkEnableDefLink();

	BOOL m_bChkEnableVGroove;


	void OnRefresh();
	double m_dEdNgSizeHeight;
	afx_msg void OnEnSetfocusEdNgXSize();
	afx_msg void OnEnSetfocusEdNgYSize();
	BOOL m_bChkNgStop;
	afx_msg void OnBnClickedChkNgStop();
	BOOL m_bChkDarkEmpMode;
	afx_msg void OnBnClickedChkDarkEmpMode();
	afx_msg void OnBnClickedBtnRegist();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTabDetectCond(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnSetfocusFoilLinkRangeX();
	afx_msg void OnEnSetfocusDrossLinkRangeX();
	afx_msg void OnEnSetfocusFoilLinkRangeY();
	afx_msg void OnEnSetfocusDrossLinkRangeY();


	int m_nEdRecipeNo;
	CString m_strEdRecipeName;
	afx_msg void OnBnClickedBtnLoad();
	BOOL m_bChkSaveDefInTab;
	double m_dEdSaveDefSize;
	afx_msg void OnBnClickedChkSaveDefInTab();
	afx_msg void OnEnSetfocusEdSaveDefSize();

	// 22.01.05 Ahn Add Start
	double	m_dEdNegTabCoatHeight;
	int		m_nEdNegTabCoatHeightPix;
	double	m_dEdNegVGrooveHeight;
	int		m_nEdNegVGrooveHeightPix;
	afx_msg void OnEnSetfocusEdNegaTabCoatHeight();
	afx_msg void OnEnSetfocusEdVGrooveHeight();
	// 22.01.05 Ahn Add Start
	double m_dEdIgnoreDistance;
	afx_msg void OnEnSetfocusEdIgnoreDistance();
	// 22.03.07 Ahn Add Start
	double m_dEdIgnoreSize;
	afx_msg void OnEnSetfocusEdIgnoreSize();
	// 22.03.07 Ahn Add End
	int m_nEdThresSurface;
	afx_msg void OnEnSetfocusEdThresSurface();
	afx_msg void OnEnSetfocusEdSptMask();
	int m_nEdSurfaceMaskOffset;
	afx_msg void OnBnClickedChkDisableSurface();
	BOOL m_bChkDisableSurface;
	afx_msg void OnEnSetfocusEdSurfaceNgSize();
	int m_nEdSurfaceMinSize;
	afx_msg void OnEnSetfocusEdSurfaceMinsize();

	
	afx_msg void OnEnSetfocusEdFoilOutNgXSize();
	afx_msg void OnEnSetfocusEdFoilBothNgXSize();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnSystemSet();
	// 22.08.09 Ahn Add Start
	int m_nContinuousNgAlarmCnt;
	int m_nAlarmCnt;
	int m_nSectorCnt;
	// 22.08.09 Ahn Add End
	afx_msg void OnEnSetfocusEdContinuousAlarm();
	afx_msg void OnEnSetfocusEdAlarmCount();
	afx_msg void OnEnSetfocusEdSectorCount();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	// 22.11.21 Ahn Add Start - JUDGE_GRAY
	double m_dEdFoilGraySize;
	double m_dEdSurfaceGraySize;
	afx_msg void OnEnSetfocusEdFoilexpGraySize();
	afx_msg void OnEnSetfocusEdSurfaceGraySize();
	// 22.11.21 Ahn Add End
	int m_nTabMinBright;
	afx_msg void OnEnSetfocusEdTabMinBright();
	int m_nRollBrightMode;
	//afx_msg void OnBnClickedRadDarkRoll();
	//afx_msg void OnBnClickedRadBrightRoll();
	afx_msg void OnBnClickedBtnSystemList();
	afx_msg void OnBnClickedBtnPrograminfo();
	CString m_strRecipeMemo;



	double m_dEdNgSizeWidth[MAX_CAMERA_NO];
	double m_dEdFoilExpOutNgSize[MAX_CAMERA_NO];
	double m_dEdFoilExpBothNgSize[MAX_CAMERA_NO];
	double m_dSurfaceNgSize[MAX_CAMERA_NO];

	double m_dEdDefectYSize[MAX_CAMERA_NO];



	afx_msg void OnSetfocusEdNgXSizeBtm();
	afx_msg void OnSetfocusEdFoilOutNgXSizeBtm();
	afx_msg void OnSetfocusEdFoilBothNgXSizeBtm();
	afx_msg void OnSetfocusEdSurfaceNgSizeBtm();
	afx_msg void OnBnClickedChkDisablePet();

	BOOL m_bChkDisablePET;
	int m_nEdPetMatrixX;
	int m_nEdPetMatrixY;
	int m_nEdPetThreshold;
	int m_nEdPetCheckCnt;

	afx_msg void OnSetfocusEdPetMatrixX();
	afx_msg void OnSetfocusEdPetMatrixY();
	afx_msg void OnSetfocusEdPetThreshold();
	afx_msg void OnSetfocusEdPetCheckCnt();
	afx_msg void OnSetfocusEdFoilDefectYSizeTop();
	afx_msg void OnSetfocusEdFoilDefectYSizeBtm();
	afx_msg void OnBnClickedChkEnableVGroove();
};


