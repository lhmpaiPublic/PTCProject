#pragma once

// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

// 생성입니다.
public:
	CColorStatic();

// 특성입니다.
protected:
	COLORREF		m_rgbText;				//텍스트색상(COLORREF)x1
	COLORREF		m_rgbBackground;		//배경색상(COLORREF)x1
	CBrush			m_Brush;				//배경색 브러쉬(CBrush*)x1
	CFont			m_font;
	BOOL			m_bBold;

public:
	int SetTextColor(COLORREF color);
	void SetBackgroundColor(COLORREF color);
	COLORREF GetTextColor() { return (m_rgbText); }
	COLORREF GetBackGroundColor() { return (m_rgbBackground); }

// 구현입니다.
public:
	virtual ~CColorStatic();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};


// CBmpStatic

class CBmpStatic : public CStatic
{
	DECLARE_DYNAMIC(CBmpStatic)

	// 생성입니다.
public:
	CBmpStatic();

	// 특성입니다.
protected:
	BOOL			m_bBitmap;
	HBITMAP			hBmp;
	UINT			m_nBmpID; // 22.08.03 Ahn Add 
public:
	int SetBitmap(UINT nID);
	UINT GetBitmapID(); // 22.08.03 Ahn Add 

	// 구현입니다.
public:
	virtual ~CBmpStatic();

public:
	afx_msg void OnPaint();

protected:
	DECLARE_MESSAGE_MAP()

};


// CColorEdit

class CColorEdit : public CEdit
{
	DECLARE_DYNAMIC(CColorEdit)

	// 생성입니다.
public:
	CColorEdit();

	// 특성입니다.
protected:
	COLORREF		m_rgbText;				//텍스트색상(COLORREF)x1
	COLORREF		m_rgbBackground;		//배경색상(COLORREF)x1
	CBrush			m_Brush;				//배경색 브러쉬(CBrush*)x1

public:
	int SetTextColor(COLORREF color);
	void SetBackgroundColor(COLORREF color);
	COLORREF GetTextColor() { return (m_rgbText); }
	COLORREF GetBackGroundColor() { return (m_rgbBackground); }

	// 구현입니다.
public:
	virtual ~CColorEdit();

public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:
	DECLARE_MESSAGE_MAP()

};

// CBmpButton

class CBmpButton : public CBitmapButton
{
	DECLARE_DYNAMIC(CBmpButton)

	// 생성입니다.
public:
	CBmpButton();

	// 구현입니다.
public:
	virtual ~CBmpButton();

private:
	int  m_bRadioButton;
	int  m_bCheckButton;
	BOOL  m_bPushed;

public:
	int SetPushed(BOOL bPush);

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void PreSubclassWindow();

};


