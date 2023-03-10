#pragma once

// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

// �����Դϴ�.
public:
	CColorStatic();

// Ư���Դϴ�.
protected:
	COLORREF		m_rgbText;				//�ؽ�Ʈ����(COLORREF)x1
	COLORREF		m_rgbBackground;		//������(COLORREF)x1
	CBrush			m_Brush;				//���� �귯��(CBrush*)x1
	CFont			m_font;
	BOOL			m_bBold;

public:
	int SetTextColor(COLORREF color);
	void SetBackgroundColor(COLORREF color);
	COLORREF GetTextColor() { return (m_rgbText); }
	COLORREF GetBackGroundColor() { return (m_rgbBackground); }

// �����Դϴ�.
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

	// �����Դϴ�.
public:
	CBmpStatic();

	// Ư���Դϴ�.
protected:
	BOOL			m_bBitmap;
	HBITMAP			hBmp;
	UINT			m_nBmpID; // 22.08.03 Ahn Add 
public:
	int SetBitmap(UINT nID);
	UINT GetBitmapID(); // 22.08.03 Ahn Add 

	// �����Դϴ�.
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

	// �����Դϴ�.
public:
	CColorEdit();

	// Ư���Դϴ�.
protected:
	COLORREF		m_rgbText;				//�ؽ�Ʈ����(COLORREF)x1
	COLORREF		m_rgbBackground;		//������(COLORREF)x1
	CBrush			m_Brush;				//���� �귯��(CBrush*)x1

public:
	int SetTextColor(COLORREF color);
	void SetBackgroundColor(COLORREF color);
	COLORREF GetTextColor() { return (m_rgbText); }
	COLORREF GetBackGroundColor() { return (m_rgbBackground); }

	// �����Դϴ�.
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

	// �����Դϴ�.
public:
	CBmpButton();

	// �����Դϴ�.
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


