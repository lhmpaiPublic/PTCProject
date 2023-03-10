#pragma once


// CCameraViewDlg 대화 상자

class CCameraViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraViewDlg)

public:
	CCameraViewDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCameraViewDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CAM_VIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	//afx_msg void OnBnClickedBtnCam01();
	//afx_msg void OnBnClickedBtnCam02();

	HWND m_ImgViewhWnd[ MAX_CAMERA_NO ];
	afx_msg void OnBnClickedBtnStart();

private :
	BOOL m_bRun;
	BOOL m_bDisableRun;

public :
	void SetDisableRun(BOOL bMode);
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedRadCam01();
	afx_msg void OnBnClickedRadCam02();
	int m_radSelectCam;
	afx_msg void OnBnClickedChkTriggerMode();
	BOOL m_bTriggerMode;

	enum {
		en_Int_Trg = 0,
		en_Ext_Trg = 1,
	};
};
