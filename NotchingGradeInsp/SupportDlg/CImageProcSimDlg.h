#pragma once


// CImageProcSimDlg 대화 상자
#include "GridCtrl.h"
#include "ColorControl.h"

#include "CImageProcess.h"
//class CimageProcess;
class CBitmapStd;
class CImageDispDlg;
class CDefectDataCtrl;
class CRecipeInfo;
class CRecipeSettingDlg;
class CHistogramDlg;


class CImageProcSimDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CImageProcSimDlg)


public:
	CImageProcSimDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CImageProcSimDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INSP_SIM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnFileOpen();
	afx_msg void OnBnClickedBtnProcAll();
	afx_msg void OnClickGridCtrlDef(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickGridCtrlRecipeSet(NMHDR* pNMHDR, LRESULT* pResult);

	CGridCtrl m_GridCtrlDef;
	CGridCtrl m_GridCtrlRecipe;
	BOOL m_bModeTop;

protected :
	CFont* m_pFontGrid;

	int MakeGridData();
	int UpdateGrid( int nHeadNo = 0 );
	// 22.09.16 Ahn Add Start
	int UpdateRecipeGrid();
	// 22.09.16 Ahn Add End

	int RecipeListLoad(); // 22.07.26 Ahn Add Start

	int ProceTopAll_AreaDiff();
	int ProceBottomAll_AreaDiff();

	// 22.01.17 Ahn Add Start
	int ProceTopAll_Negative();
	int ProceBottomAll_Negative();
	// 22.01.17 Ahn Add End
	// 23.02.15 Ahn Add Start
	int ProcTopAll_BrightRoll();
	int ProcBottomAll_BrightRoll();
	// 23.02.15 Ahn Add End

	// 22.07.14 Ahn Add Start
	enum {
		en_Def_Wave			= 0,
		en_Def_Crack		= 1,
		en_Def_Bur			= 2,
		en_HalfSlit_Def_Max = 3,
	};
	int ProcessHalfSlittingInsp();
	int ProcHalfSlittingInsp(BYTE* pImgPtr, int nWidth, int nHeight);
	// 22.07.14 Ahn Add End
	// 22.09.15 Ahn Add Start
	int UserChange();
	// 22.09.15 Ahn Add End

	// 23.01.06 Ahn Add Start
	int ProcessFoilExpInRect();
	// 23.01.06 Ahn Add End
	// 23.01.12 Ahn Add Start
	int ProcFoilExpInRect_Anode();// 양극
	int ProcFoilExpInRect_Cathode();// 음극
	int CheckProcRange(CRect& rcRect, int nWidth, int nHeight);
	int FindTabFromRect(CRect rcRect, CImageProcess::VEC_SECTOR& vecSector);
	// 23.01.12 Ahn Add End
public :
	// 22.04.20 Ahn Add Start
	int FileNameFromTabNo(CString strFileName);
	int FileNameFromHeadNo(CString strFileName);
	// 22.04.20 Ahn Add End
	void SetBitmapStd(CBitmapStd bmpStd);
	void OpenImage();

	int  AddDefectInfo(CImageProcess::_VEC_BLOCK *vecBlockFoilExp, CImageProcess::_VEC_BLOCK *vecBlockDross, int nCamPos, BOOL bModeClear = TRUE );
	typedef CImageProcess::_VEC_BLOCK VEC_BLOCK_ALL;
	VEC_BLOCK_ALL* m_pVecBlockAll ;
	VEC_BLOCK_ALL* GetDefectData() { return m_pVecBlockAll ; };

	CImageProcess::VEC_PET_INFO* m_pvstPetInfo;
	CImageProcess::VEC_PET_INFO* GetPetInfo() { return m_pvstPetInfo; };

	CImageProcess::_BRIGHT_INFO* m_pstBrightInfo;
	CImageProcess::_BRIGHT_INFO* GetBrightInfo() { return m_pstBrightInfo; };


	CRect m_rcLineInspEdge[2];
	CRect m_rcInspArea[2];

//KANG 22.01.07 Add Start
	void LoadImageFile(LPCTSTR lpszFileName);
//KANG 22.01.07 Add End

private :
	CImageDispDlg* m_pImageDispDlg;

	enum {
		en_OrgImage = 0,
		en_ProcImage1,
		en_ProcImage2,
		en_ProcImage3,
		en_ProcImage4,
		en_ProcImage5,
		en_ProcImage6,
		en_ProcImage7,
		en_MaxProcImage,
	};

	enum {
		en_CimDef_Index = 0,
		en_CimDef_Type,
		en_CimDef_Judge,
		en_CimDef_Count,
		en_CimDef_PosX,
		en_CimDef_PosY,
		en_CimDef_JuegeSize,
		en_CimDef_Distance, //	22.04.15 Ahn Add
		en_CimDef_Width,
		en_CimDef_Height,
		en_CimDef_BriAve,
		en_CimDef_BriMax,
		en_CimDef_BriMin,
		en_CimDef_OrgAve,
		en_CimDef_OrgMax,
		en_CimDef_OrgMin,
		en_CimDef_Max,
	};

	enum {
		// 22.08.08 Ahn Modify Start
		en_RcpPrm_ThresOrg		= 1,
		en_RcpPrm_ThresMin		= 2,
		en_RcpPrm_MinSize		= 3, 
		en_RcpPrm_InspRange		= 4, 
		en_RcpPrm_JudgeSize		= 5,
		en_RcpPrm_Magnification = 6,
		en_RcpPrm_UseDefLink	= 7,
		en_RcpPrm_LinkRangeX	= 8,
		en_RcpPrm_LinkRangeY	= 9,
		en_RcpPrm_Max			= 10,
		// 22.08.08 Ahn Modify End
	};
	CBitmapStd* m_pBmpStd[en_MaxProcImage];

	CRect m_rWindowExtents;
	CRect m_rcImgDisp;
	int m_WindowWidth;
	int m_WindowHeight;
	CString m_strSrcFileName;
	CString m_strCropImagePath;
	BOOL m_bLoadImage;

	CRecipeInfo* m_pRecipeInfo;
	CRecipeSettingDlg* m_pRecipeSetDlg;

	CStringList m_strListFiles;
	POSITION	m_nNowPos;

	int SaveResultFile(CImageProcess::_VEC_BLOCK *vecDef);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnImgOrg();
	afx_msg void OnBnClickedBtnImgProc1();
	afx_msg void OnBnClickedBtnImgProc2();
	afx_msg void OnBnClickedBtnImgProc3();
	afx_msg void OnBnClickedBtnImgProc4();
	afx_msg void OnBnClickedBtnImgProc5();
	afx_msg void OnBnClickedBtnImgProc6();
	afx_msg void OnBnClickedBtnImgProc7();

public :
	void ResizeDlg(int cx, int cy); 
	void SelectImage(int nImgNo);
	CString m_strProcTime;
	afx_msg void OnBnClickedBtnDetectLine();

	//
	int GetTabHeadPos(CSize* size, int *nLevel );
	int GetLineLevel(int* nLevel);
	BOOL m_bChkDIspBoundary;
	afx_msg void OnBnClickedChkDispLine();
	afx_msg void OnBnClickedBtnDivisionTab();
	afx_msg void OnBnClickedBtnRecipeSet();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedBtnTest();

	BOOL m_bChkDefLink;
	BOOL m_bUsePreFilter;
	BOOL m_bDispDefect;
	CString m_strEdFileName;
	int m_nSelectDefNo;

	int m_nSelImageNo; // 22.01.17 Ahn Add 

	static CString GetTokenFromString( CString strLine, char chTokinizer, int nPos); // 22.06.04 Ahn Add

private:
	void Initial();
	CHistogramDlg* m_pHistoDlg;
public:
	BOOL m_chkMeasure;
	afx_msg void OnBnClickedChkMeasure();
	afx_msg void OnBnClickedChkAreaSet();
	afx_msg void OnBnClickedChkDispDefect();

	afx_msg void OnBnClickedBtnHistogram();

private :
	void InspectionAuto();
	void DrawLine(); //230109
	void DrawLine_Top(); //230109
	void DrawLine_Bottom(); //230109
	void DrawLine_Top_Negative(); //230109
	void DrawLine_Bottom_Negative(); //230109
	void LoopLabelingTest();
	// 22.06.04 Ahn Add Start
	int SaveCropAndResultSave();
	int SaveCropImage(int nPosX, int nPosY, CString strFileName);
	// 22.06.04 Ahn Add End

	// 23.02.06 Ahn Test Start
	void DrawImage(CDC *pDC, int nWidth, int nHeight, int nMagnif);
	// 23.02.06 Ahn Test End
public:
	afx_msg void OnBnClickedBtnInspSpeter();
	afx_msg void OnBnClickedBtnCropSave();
	afx_msg void OnBnClickedBtnAttachImg();
	CComboBox m_cmbRecipeSelect;
	afx_msg void OnCbnSelchangeCmbRecipeSelect();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnDropdownCmbRecipeSelect();
	afx_msg void OnBnClickedBtnResetCount();
};
