#pragma once


#include <vector>

//class CImageProcess;
#include "CImageProcess.h"
class CBitmapStd;
class CImgClipedBmp;
class CImageProcSimDlg;

// CImageDispDlg 대화 상자

class CImageDispDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CImageDispDlg)

public:
	CImageDispDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CImageDispDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISP_IMAGE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CString	m_stDispPos;
	CString	m_EdRate;

private:
	CBitmapStd* m_pBitmap;
	CImgClipedBmp* m_pBmpDraw;
	CBitmapStd* m_pBmpMeas;						// 길이 측정용 
	CSize	m_sizeImage;
	CRect	m_rcCur;
	CRect	m_rcWnd;
	CRect	m_rcDisp;
	CPoint	m_ptPrev;			// 마지막 마우스 클릭 좌표

	CRect	m_rcMask;
	CRect	m_rcDragPos;
	BOOL	m_bMaskDrawFlag;
	int		m_nDrawRate;
	int		m_nScopeRate;
	SCROLLINFO	m_ScrollInfoH;
	SCROLLINFO	m_ScrollInfoV;
	BYTE m_btColorTable[256];

	int		m_nZoomOutV;
	int		m_nZoomOutH;
	int		m_nThreshold;
	CFont* m_pFontDispMeasRslt;
	BOOL	m_bDispDefectFlag;
	BOOL	m_bDispMaskFlag;
	BOOL	m_bDispRound;
	int		m_nDrawColorMode;

	int GetPickBitmap(CImgClipedBmp& clipbmp);
	int MakeDrawBitmap(void);
	void DrawDefect(CDC* pDC);
	void DrawBlobArea(CDC* pDC, CRect* rcArea);
	void DrawMask(CDC* pDC);
	void DrawMeasureLine(CDC* pDC);
	void DrawPetArea(CDC* pDC);
	void DrawBrightCheckArea(CDC* pDC);

	CPoint GetImagePoint(int x, int y);
	int DrawSelectRect(CDC* pDC);

	CRect	m_rectTabHead;
	CRect	m_rectLeftLine;
	CRect	m_rectRightLine;
	int		m_nCircleRadius;
	CPoint	m_cpCircleCenterLeft;
	CPoint	m_cpCircleCenterRight;

	BOOL m_bDispBoundary;
	BOOL m_bDispBlob;
	void DrawBoundaryLine(CDC* pDC);
	CImageProcess::VEC_ROUND_INFO m_VecLeftRound;
	CImageProcess::VEC_ROUND_INFO m_VecRightRound;

	BOOL	m_bDispSelectedRect;
	int		m_nSelectDefNo;
	CRect	m_rectSelected;

	CImageProcSimDlg* m_pParent;

	BOOL m_bMeasureFlag;
	BOOL m_bDispMeasureLine;
	enum {
		en_First_Point = 0,
		en_Second_Point = 1, 
		en_Measure_Point = 2,
		en_Reset_Point = 3,
	};
	int		m_nMeasureState;
	CPoint	m_cpLinePos[ 2 ] ;
	double	m_dMeasureLength;
	int		m_nMeasurePixLen;



public:
	void SetBoundary(CImageProcess::VEC_ROUND_INFO* pVecLeft, CImageProcess::VEC_ROUND_INFO* pVecRight);
	void SetDrawBoundaryFlag(BOOL bFlag);
	void PointMove(int nPosX, int nPosY);
	void SetSelectDefRect(CRect rcDefect);
	void SetDispDefect(BOOL bDispFlag);
	void SetSelDefectNo(int nSelDefNo);
	void SetMeasureMode(BOOL bFlag);
	CRect GetMaskRect();
	void SetDrawBlobFlag(BOOL bFlag);


public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnZoomIn();
	afx_msg void OnBnClickedBtnZoomOut();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	int SetBitmapStd(CBitmapStd* pBmp, BOOL bReverse = FALSE);
//	int SetImage(CString& strFileName, int repeat);
//	void SetImageRect(void);

};
