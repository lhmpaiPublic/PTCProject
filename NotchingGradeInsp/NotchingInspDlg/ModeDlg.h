#pragma once

#include "ColorControl.h"
#include "CUserLoginDlg.h"

#include "Celltrack/LoadVTECSDll.h"
using namespace LOADVTECSDll_GROUP;

// CModeDlg 대화 상자

#define AUTO_START_TIMER 1111
#define AUTO_START_DELAY 1000

#define T_CHECK_DEVICE 8858 //240108

//카운터 보드 네트워크 에러 메시지 정의
#define WM_COUNTBORDERROR WM_USER + 1 

//CellTrack
#define WM_TOGGLEIPCONFIG WM_USER + 2 

class CNotchingGradeInspDoc;
class CNotchingGradeInspView;
class CIoMonitorDlg;
class CTactTimeGraphDlg;
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
	CColorStatic			m_stCameraState;
	CColorStatic			m_stPlcState;
	CColorStatic			m_stIoState;
	CColorStatic			m_stLightState;
	CColorStatic			m_stDiskSpace;
	CFont					m_FontDiskCapa;

	BOOL m_bPlcLastFlag;
	BOOL m_bIoLastFlag;
	BOOL m_bLampLastFlag;
	BOOL m_bCamLastFlag;
	BOOL m_bRunLastFlag;

	CIoMonitorDlg* m_pIoMonitDlg;

	CTactTimeGraphDlg* m_pTactGraph;

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
	void Refresh();										
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	afx_msg void OnBnClickedBtnDirectory();
	CMFCButton m_radInspRun;
	CMFCButton m_radInspStop;
	afx_msg void OnBnClickedRadRun();
	afx_msg void OnBnClickedRadStop();

	//카운터 보드 네트워크 에러 메시지 함수
	afx_msg LRESULT OnCountBordError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToggleIPConfig(WPARAM wParam, LPARAM lParam);

public :
	//시작/정지 버튼 상태 변경
	void ChangeState(int nStatus);
	void EnableChild(BOOL bRunMode );

	//카운터보드에서 Stop 클릭 상태를 알기 위해서 정의 됨
	BOOL GetStopCheckFALSE();

	void DisplayLanguage();
	afx_msg void OnBnClickedBtnTactTime();
	afx_msg void OnBnClickedChkSwitchDisp();
	BOOL m_bDispSwitch;

	void EnableControl(BOOL bModeRun);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

#if 1 // 240108
//	INT_PTR     m_nTimer_DeviceCheck;
	int         m_nTimerInterval_DeviceCheck;

	// LED 장비 확인 함수.
	afx_msg BOOL CheckDevice();
#endif

	virtual BOOL DestroyWindow();

	BOOL initCelltrack();
	void ToggleIPConfig();

	static CString parse_CellID(CString SourceString, int trigCount);

private : 

	LoadVTECSDll		m_LoadVTECSDll;
	CCriticalSection	m_CriticalDLL;
	stDllStatus			m_CurrentDllStatus, m_OldDllStatus;
	bool				m_bIPConfigShow;
	bool				m_bToggleDataSendQueue;
	typeTest			m_stBinaryTest;
	bool				m_blastOldConnectStatus;
	CString				m_strSocketEvent;

	static UINT WINAPI DllSocket_ConnectStatus(char* strEventContents, int nStatus, int nErrorCode);
	void PostConnectStatus(char* strEventContents, int nStatus, int nErrorCode);
	static UINT WINAPI DllSocket_ReceiveDataBuffer(unsigned char* pReceiveBuffer, int nReceiveLengthByByte);

	void SubDisplayUpdate(void);
};
