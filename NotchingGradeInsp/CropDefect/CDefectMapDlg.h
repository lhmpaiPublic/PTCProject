#pragma once


// CDefectMapDlg 대화 상자

class CNotchingGradeInspView;
class CImgDisplayDlg;

class CDefectMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDefectMapDlg)

public:
	CDefectMapDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr );   // 표준 생성자입니다.
	virtual ~CDefectMapDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEFECT_MAP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	// 22.12.27 Ahn Add Start
	CNotchingGradeInspView* m_pView;
	BOOL m_bShowImgDlg ;
	// 22.12.27 Ahn Add End
protected :

	CString		m_strFontFace;
	CFont*		m_pFontGrid;
	CFont*		m_pFontBtn;

	//CRect		m_rcMap;	
	CRect		m_rcDraw;			// Map 전체
	//int			m_nMapLength;		//Map 표시길이 [mm]
	double		m_dMapLength ;		//Map 전체길이 [mm]
	double		m_dMapWidth ;		//Map 전체폭   [mm]

	double		m_dZoomRate;		// Zoom 비율.

	int			m_nDispLength;		// Map 표시 길이
	int			m_nMapScrollMax ;	// Scroll 최대 길이
	int			m_nDispStartY;		// Map 표시 시작위치

	double		m_dMapResoX;		// Map 분해능 X
	double		m_dMapResoY;		// Map 분해능 Y

	int			m_nMargin;			// 마진
	int			m_nPosSizeX ;		// Map 위치 
	int			m_nMapSizeX ;		// Map 크기 X
	int			m_nMapSizeY ;		// Map 크기 Y
	int			m_nMapStartPosY;					// Map 시작외치 Y
	int			m_nMapStartPosX[MAX_CAMERA_NO] ;	// Map 시작위치 X
	int			m_nScrollSizeX;						// Scroll 가로크기

	int			m_nInspPosY;		// 현재	 표시선.

	//int			m_nDispStartY;		// Map 표시 Y 방향 시작점
	//int			m_nDispEndY;		// Map 표시 Y 방향 종료점.
	//double		m_dAreaOffset;		//개시좌표가 0이 아닌 경우(시프트 단이 0mm 이지만, 표기하는 5mm 마스크한 검사위치로 부터 検査位置からのときなど）

	CPoint	m_cpMouse ;
	void	DrawMap(CDC* pDC, CRect &rcRect);
	void	DrawDefect(CDC* pDC, CRect &rcRect );
	void	DrawAxis(CDC* pDC, CRect &rcRect);
	// 23.01.16 Ahn Add Start
	void	DrawTabAndRemark(CDC* pDC, CRect& rcRect);
	// 23.01.16 Ahn Add End

	void	DrawShapeCircle(Graphics& graphics, Pen* penRank, SolidBrush* brs, RectF rect);
	void	DrawShapeRectangle(Graphics& graphics, Pen* penRank, CRect rcRect);
	void	DrawShapeTriangle(Graphics& graphics, Pen* penRank, SolidBrush* brs, RectF rect);

	int		GetXGraphPos(double dPosX, double dBasePos);
	int		GetYGraphPos(double dPosY, double dBasePos);
	void	DrawInspLine(Graphics& graphics, double dBgnLength, double dLinePos, DWORD color, REAL fWidth = 2.0);

	void	CalcMapRangeAndZoomRate();
	CPoint CalcMapPosByRealPos(double dPosX, double dPosY, int nCamPos);
	int    CalcRealPosByMapPos(double dMapPosX, double dMapPosY, double& dPosX, double& dPosY, int &nCamPos );

	BOOL	IsInMapRange(double dPosX, double dPosY);

	// Scroll
	void ChangeVScroll(UINT nSBCode, BOOL bSetScrollBar) ;
	void SetScrollV();

	enum {
		enMinMapLen = 1,		// Mqp 최소 표시 길이[m]
		enMaxMapLen = 2000,		// Map 최대 길이 2000m

		// 22.12.07 Ahn Add Start
		en_MapOffsetY = 25,
		// 22.12.07 Ahn Add End
	};

	// 22.12.16 Ahn Add Start
	static CString GetTabPathFormDefPath(CString strDefImagePath);

	// 23.02.08 Ahn Modify Start
	//void SetDefectList(double dPosY);
	void SetDefectList( double dPosY, int nCamPos );
	// 23.02.08 Ahn Modify Start
	// 22.12.16 Ahn Add End


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CComboBox m_cmbMapDispLen;
	afx_msg void OnCbnSelchangeCmbMapDispLength();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnBnClickedBtnDispImage();

	// 22.12.27 Ahn Add Start
	void RequestCloseImgDispDlg();
	void CloseImgDispDlg();
	// 22.12.27 Ahn Add End
private :
	// 22.12.15 Ahn Add Start
	CImgDisplayDlg* m_pImgDispDlg; 
	// 22.12.15 Ahn Add End

};
