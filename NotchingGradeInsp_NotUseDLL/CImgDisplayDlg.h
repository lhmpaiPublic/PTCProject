#pragma once

#include "ThumbnailCtrl.h"
#include "GridCtrl.h"
#include "CDefectDataCtrl.h"
// CImgDisplayDlg 대화 상자

#include "BitmapStd.h"

class CDefectMapDlg;
class CImgDisplayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CImgDisplayDlg)

public:
	CImgDisplayDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CImgDisplayDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DISP_IMAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private :

	// Thumbnail
	CImageList		m_ImgList;
	CThumbnailCtrl	m_ThumbCtrl;

	// Defect Information Grid
	CGridCtrl m_GridDefInfo;
	CFont* m_pFontGrid;

	CDefectDataCtrl::_VEC_DEFECT_LIST m_DefList ;
	CDefectMapDlg* m_pParent;
	CDefectInfo* m_pDefInfo;

	int m_nSelDefNo;	
public:
	virtual BOOL OnInitDialog();

	void SetDefectList(CDefectDataCtrl::_VEC_DEFECT_LIST* pList);

	CString GetTabPathFormDefPath(CString strDefImagePath);

	void MakeDefInfoGrid();
	void UpdateDefInfoGrid(CDefectInfo* pInfo);
	void DrawDefectRect(CDC* pDC, CRect rcDef, COLORREF color);

	enum {
		en_Def_Title = 0,
		en_Def_TabNo ,
		en_Def_No,
		en_Def_Judge,
		en_Def_Size, 
		en_Def_Bright_Max,
		en_Def_Bright_Min,
		en_Def_Bright_Ave,
		en_Def_MaxRows,

		en_Def_MaxCols = 2 ,
	};

private :
	int ImageLoad(CString strImgPath);
	void DrawImageCrop(HWND HWnd, BYTE *pImgPtr, CSize imgSize);
	BYTE* m_pImgPtr;
	CSize m_ImgSize;
	CBitmapStd m_BmpStd;
	CBitmapStd m_CropBmpStd; // 23.01.05 Ahn Add

	void DrawImage(HWND HWnd, BYTE* pImgPtr, int nWidth, int nHeight, int nMagnif, CDefectInfo *pDefInfo);

public:
	afx_msg void OnPaint();
	afx_msg void OnSelectImage(NMHDR* pNMHDR, LRESULT* pResult);

	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	// 23.01.10 Ahn Add Start
	int SelectDefectInfo(CDefectInfo* pDefInfo);
	// 23.01.10 Ahn Add End

};
