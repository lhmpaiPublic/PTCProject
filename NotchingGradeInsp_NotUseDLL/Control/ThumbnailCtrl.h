#pragma once

#include <vector>

// CExpandViewImage

class CExpandViewImage : public CStatic
{
	DECLARE_DYNAMIC(CExpandViewImage)

	// 생성입니다.
public:
	CExpandViewImage(LPCTSTR lpszImageFileName = _T(""));

	// 특성입니다.
protected:
	CImage m_Image;
	CString m_strImageFileName;
//KANG 22.07.08 Add Start
	BOOL m_bLClickClose;
	BOOL m_bRClickClose;
//KANG 22.07.08 Add End

public:
	int SetImageFile(LPCTSTR lpszImageFileName);
//KANG 22.07.08 Add Start
	void SetLClickClose(BOOL bSet);
	void SetRClickClose(BOOL bSet);
//KANG 22.07.08 Add End

	// 구현입니다.
public:
	virtual ~CExpandViewImage();

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);	//KANG 22.07.08 Add
};

// CThumbnailCtrl

typedef struct tagNM_THUMBDATA {
	NMHDR	hdr;
	int		nIndex;
	long	nItem;
} NM_THUMBDATA;

// 22.06.15 Ahn Add Start
class CThumbData {
public :
	CString strFileName;
	CString strDispName;
};
// 22.06.15 Ahn Add End

#define WM_USER_DEF1 WM_USER + 11
#define WM_USER_DEF2 WM_USER + 12
#define WM_USER_DEF3 WM_USER + 13
#define WM_USER_DEF4 WM_USER + 14
#define WM_USER_DEF5 WM_USER + 15

#define	MAX_DISP_IMAGE_NUM	128

class CThumbnailCtrl : public CWnd
{
	DECLARE_DYNAMIC(CThumbnailCtrl)

public:
	CThumbnailCtrl();
	virtual ~CThumbnailCtrl();

	enum {
		IDR_MENU_BASE = 20000,
		IDR_MENU_USER1 = IDR_MENU_BASE,
		IDR_MENU_USER2,
		IDR_MENU_USER3,
		IDR_MENU_USER4,
		IDR_MENU_USER5,
	};

private:
	BOOL	RegisterWindowClass();
	int		AdjustDispSize(int cx, int cy);
	void	DrawImage(CDC* pDC, CRect& WinRect);
	void	CreateExpandView(LPCTSTR lpszImageFileName);
	void	DestroyExpandView();

	CScrollBar	m_ctlVScroll;
	CSize		m_DispSize;
	CRect		m_rectImage[MAX_DISP_IMAGE_NUM];
	int			m_nMaxImageDispNum;
	CBitmap		m_bmpBuffer;
	BOOL		m_bReDraw;
	int			m_nSelectIndex;

protected:
	int			PagePrev();
	int			PageNext();
	int			GetFileList(LPCTSTR lpszPath, CStringList& rstrFileNameList, LPCTSTR lpszExt = _T("*"), int nMaxFiles = 0, BOOL bSubFolder = FALSE, BOOL bListClear = TRUE);
	LRESULT		SendMessageToParent(int nIndex, int nMessage) const;

	COLORREF	m_clrBackGrnd;
	COLORREF	m_clrImgFrame;
	int			m_nImgFrameWidth;
	int			m_nThumCols;
	int			m_nThumRows;
	CString		m_strContextMenu[5];
	int			m_nContextMenuNum;

	// 22.06.15 Ahn Delete Start
	//CStringList	m_strFileList;
	// 22.06.15 Ahn Delete End
	// 22.06.15 Ahn Add Start
	typedef std::vector< CThumbData > VEC_THUMB_DATA;
	VEC_THUMB_DATA m_vecThumbData ;
	int			GetFileList(LPCTSTR lpszPath, VEC_THUMB_DATA& vecThumbData, LPCTSTR lpszExt = _T("*"), int nMaxFiles = 0, BOOL bSubFolder = FALSE, BOOL bListClear = TRUE);
	// 22.06.15 Ahn Add End
	CExpandViewImage* m_pExpandView;

public:
	void	OnRefresh();
	int		ReadImageFileInFolder(LPCTSTR lpszFilePath, LPCTSTR lpszExt = _T("JPG|BMP"), BOOL bSubFolder = FALSE, BOOL bReDraw = TRUE);
	int		AddImageList(LPCTSTR lpszFilePath) ;
	int		SetImageList(CStringList &strList);
	void	SetBackGroundColor(COLORREF bkColor);
	void	SetImageFrameColor(COLORREF frmColor);
	void	SetImageFrameWidth(int width);
	void	SetThumbCols(int cols);
	void	SetThumbRows(int rows);
	int		GetImageCount();
	CString GetImageFileNameFromList( int nIndex );
	void	SetContextMenuString(LPCTSTR lpszMenu, int nIndex);
	void	SetContextMenuNum(int nNum);

	// 22.06.10 Ahn Add Start
	int AddImageBufferMode(CString strFileName, CString strDispName);
	// 22.06.10 Ahn Add End
	// 22.06.14 Ahn Add Start
	BOOL CheckScrollDIsplay();
	// 22.06.14 Ahn Add End
	BOOL m_bDispPopupImage;
	void SetPopupImageMode(BOOL bMode);
	void ClearAll(); // 22.07.06 Ahn Add 
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMenuUser1();
	afx_msg void OnMenuUser2();
	afx_msg void OnMenuUser3();
	afx_msg void OnMenuUser4();
	afx_msg void OnMenuUser5();
};



