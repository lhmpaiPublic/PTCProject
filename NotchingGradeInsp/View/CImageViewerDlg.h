#pragma once


// CImageViewerDlg 대화 상자

class CImageViewerDlg : public CDialog
{
	DECLARE_DYNAMIC(CImageViewerDlg)

public:
	CImageViewerDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CImageViewerDlg();

protected:
	CRect m_rcImageRect;
	CImage m_Image;
	CString m_strImageFileName;

public:
	int SetImageFile(LPCTSTR lpszImageFileName);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_IMAGE_VIEWER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};
