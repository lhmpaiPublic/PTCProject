#pragma once

// CGraphCtrl
#define MAX_GRAPH_DATA_NUM		30000
//KANG 22.06.17 Modify Start
//#define MAX_DATA_NUM			10
#define MAX_DATA_NUM			20
//KANG 22.06.17 Modify End
#define GRAPHCTRL_CLASSNAME		_T("GraphCtrl")
#define FONT_HEIGHT				16

class CGraphCtrl : public CWnd
{
	DECLARE_DYNAMIC(CGraphCtrl)

public:
	CGraphCtrl(int nDataNum = 1);
	virtual ~CGraphCtrl();

protected:
	BOOL RegisterWindowClass();

public:
	BOOL Create(const RECT& rect, CWnd* parent, UINT nID, DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);
	void DrawFtcGraphBack(CDC* cdc, BOOL bHorizAxis = TRUE, BOOL bVerticAxis = TRUE);
	void DrawFtcGraphData(CDC* cdc );
	void AdjustSize(void);
	void ScaleChange(int nScale);
	int	 OnRefresh(int nScrollPos);
	void DrawFrameRect(BOOL bDraw);

	int SetData(int nDataIndex, int nCount, int nData);
	int DataClear();
	
	void SetGraphSize(int nSize);
	void SetThresHold1(int nThreshold);
	void SetThresHold2(int nThreshold);
	void SetImageSize(int nSize);
	void SetMaxVerScaleSize(int nSize);
	void SetVerMajorScaleInterval(int nInterval);
	void SetVerMinorScaleInterval(int nInterval);
	void SetHorScaleInterval(int nInterval);
	void SetHorScaleStartNum(int nNum);
	void SetFixHScroll(BOOL bFix);
	void DrawVerAxis(BOOL bDraw);
	void DrawHorAxis(BOOL bDraw);
	void DrawVerSubLine(BOOL bDraw);
	void DrawHorSubLine(BOOL bDraw);
	void SetVerScaleTitle(LPCTSTR strTitle);
	void SetHorScaleTitle(LPCTSTR strTitle);
	void SetDrawData(int nDataIndex, BOOL bDraw);
	void SetDrawDataTotal(BOOL bDraw);
	BOOL IsDrawData(int nDataIndex);
	BOOL IsDrawDataTotal();
	BOOL IsDrawDataIndividual();
	void SetDataColor(int nDataIndex, COLORREF color);
	void SetDrawIndividual(BOOL bDraw);
//KANG 22.06.17 Add Start
	void SetVisibleHScollbar(BOOL bShow);
	void SetBkColor(COLORREF bkColor);
	void DrawShape(CDC* cdc, const POINT* lpPoints, int nCount, int nSize, COLORREF color = RGB(0, 0, 0), int nShape = 0);
	void DrawDataLine(BOOL bDraw);
	void DrawDataPoint(BOOL bDraw);
	int	 GetData(int nDataIndex, int nCount);
	void DrawPointZeroData(BOOL bDraw);
	void UseVerScaleLabel(BOOL bUse);
	void SetVerScaleLableSpace(int nSpace);
	void SetDataLineColor(int nDataIndex, COLORREF color);
	void SetDataPointColor(int nDataIndex, COLORREF color);
	void SetDataPointShape(int nDataIndex, int nShape);
	// 22.06.20 Ahn Add Start
	void SetDrawThresholdLine(BOOL bDraw);
	int SetThresholdLine(int nLine);
	// 22.06.20 Ahn Add End
	// 22.12.14 Ahn Add Start
	void SetScrollV();
	// 22.12.14 Ahn Add End
public:
	enum {
		en_Shape_Rectangle = 0,
		en_Shape_Ellipse,
		en_Shape_Triangle,
		en_RankCond_Max,
	};
//KANG 22.06.17 Add End

private:
	int		m_nDataNum;

protected:
	UINT	grp_start, grp_size;
	RECT	m_rcDrawRect;
	RECT	m_rcGraphRect;
	RECT	m_rcSubGraphRect[MAX_DATA_NUM] = { 0, };
	BOOL	m_bIndividualDraw;
	int		m_nImageSIze;
	BOOL	m_bVerAxis;
	BOOL	m_bHorAxis;
	int		m_nMaxVerSIze;
	int		m_nVerMajorScaleInterval;
	int		m_nVerMinorScaleInterval;
	int		m_nHorStartNum;
	int		m_nHorScaleInterval;
	BOOL	m_bVerSubLine;
	BOOL	m_bHorSubLine;
	BOOL	m_bDrawFrameRect;
	int		m_nThreshold1;
	int		m_nThreshold2;
	BOOL*	m_bDrawData;
	BOOL	m_bDrawDataTotal;
	BOOL	m_bFixHScroll;

	CString		m_strVerScaleTitle;
	CString		m_strHorScaleTitle;

	COLORREF*	m_colorDataLine;
//KANG 22.06.17 Add Start
	BOOL		m_bShowHScrollbar;
	COLORREF	m_ColorBk;
	BOOL		m_bDrawDataLine;
	BOOL		m_bDrawDataPoint;
	BOOL		m_bDrawPointZeroData;
	BOOL		m_bVerScaleLabel;
	int			m_nVerScaleLabelSpace;
	COLORREF* m_colorDataPoint;
	int* m_nDataPointShape;
//KANG 22.06.17 Add End

public:
	RECT mGraphRect;
//	LPBYTE	m_pMemo1;
//	LPBYTE	m_pMemo2;
//	LPBYTE	m_pMemo3;
//KANG 22.06.17 Modify Start
//	BYTE**	m_pMemo;
	WORD** m_pMemo;
//KANG 22.06.17 Modify End

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void PreSubclassWindow();
};


