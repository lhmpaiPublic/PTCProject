#pragma once

#include "ColorControl.h"

// CModeDlg 대화 상자

#define AUTO_START_TIMER 1111
#define AUTO_START_DELAY 1000

#define T_CHECK_DEVICE 8858 //240108

class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CIoMonitorDlg;
// 22.12.12 Ahn Add Start
class CTactTimeGraphDlg;
// 22.12.12 Ahn Add End
class CModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModeDlg)

public:
	CModeDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다.
	virtual ~CModeDlg();

protected:
	CWnd*					m_pParent;
	CNotchingGradeInspDoc*	m_pDoc;
	CNotchingGradeInspView* m_pView;

	int						m_nViewMode;
	CFont					m_fontTitle;
	CColorStatic			m_stTitle;
	CColorStatic			m_stState;
	CBmpStatic				m_cLogo;
	CBmpButton				m_cBtnExit;
	CMFCButton				m_cbtnLog;
	// 22.06.30 Ahn Add Start
	CColorStatic			m_stCameraState;
	CColorStatic			m_stPlcState;
	CColorStatic			m_stIoState;
	CColorStatic			m_stLightState;
	CColorStatic			m_stDiskSpace;
	CFont					m_FontDiskCapa;
	// 22.06.30 Ahn Add End

	// 22.07.01 Ahn Add Start
	BOOL m_bPlcLastFlag;
	BOOL m_bIoLastFlag;
	BOOL m_bLampLastFlag;
	BOOL m_bCamLastFlag;
	// 22.07.01 Ahn Add End
	// 22.07.07 Ahn Add Start
	BOOL m_bRunLastFlag;
	// 22.07.07 Ahn Add End

	// 22.08.03 Ahn Add Start
	CIoMonitorDlg* m_pIoMonitDlg;
	// 22.08.03 Ahn Add End

	// 22.12.12 Ahn Add start
	CTactTimeGraphDlg* m_pTactGraph;
	// 22.12.12 Ahn Add End

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MODE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedRadiInspMode();
	afx_msg void OnBnClickedRadHistoryMode();
	afx_msg void OnBnClickedRadCondMode();
	afx_msg void OnBnClickedBtnInspSim();
	afx_msg void OnBnClickedBtnLog();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnCancel();
	void Refresh();										// 22.05.25 Son Add
	virtual BOOL PreTranslateMessage(MSG* pMsg);		//KANG 22.05.24 Add
	afx_msg void OnBnClickedBtnDirectory();
	CMFCButton m_radInspRun;
	CMFCButton m_radInspStop;
	afx_msg void OnBnClickedRadRun();
	afx_msg void OnBnClickedRadStop();

public :
	void ChangeState(int nStatus);
	void EnableChild(BOOL bRunMode );

	void DisplayLanguage();
	afx_msg void OnBnClickedBtnTactTime();
	afx_msg void OnBnClickedChkSwitchDisp();
	BOOL m_bDispSwitch;

	// 23.02.16 Ahn Add Start
	void EnableControl(BOOL bModeRun);
	// 23.02.16 Ahn Add End

	afx_msg void OnTimer(UINT_PTR nIDEvent);

#if 1 // 240108
//	INT_PTR     m_nTimer_DeviceCheck;
	int         m_nTimerInterval_DeviceCheck;

	// LED 장비 확인 함수.
	afx_msg BOOL CheckDevice();
#endif

};
