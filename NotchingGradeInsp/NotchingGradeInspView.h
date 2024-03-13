
// NotchingGradeInspView.h: CNotchingGradeInspView 클래스의 인터페이스
//

#pragma once

#include "InspDlg.h"
#include "HistoryDlg.h"
#include "CondDlg.h"
#include "CDefectMapDlg.h" 

class CRecipeSettingDlg;
class CCameraWaveDlg;
class CDebugImageAcqDlg;
class CResultFileManager;
class CDeleteResultFileThread;

class CNotchingGradeInspView : public CView
{
protected: // serialization에서만 만들어집니다.
	CNotchingGradeInspView() noexcept;
	DECLARE_DYNCREATE(CNotchingGradeInspView)

	//동기화 이벤트 객체
	HANDLE pEvent_NotchingGradeInspView;
	CWinThread* pThread;
// 특성입니다.
public:
	CNotchingGradeInspDoc* GetDocument() const;

	// 동기화 이벤트 객체
	HANDLE getEvent_NotchingGradeInspView() { return pEvent_NotchingGradeInspView; }
	void setEvent_NotchingGradeInspView() { SetEvent(pEvent_NotchingGradeInspView); }
protected:
	bool logControlKeyDown;
	CInspDlg*		m_pInspDlg;
	CDefectMapDlg* m_pDefMapDlg;

	CHistoryDlg*	m_pHistoryDlg;
	CRecipeSettingDlg* m_pCondDlg;

// 작업입니다.
public:
	int ChangeViewMode(int nMode);
	int OnRefresh();

	CInspDlg* GetInspDlgPtr() { return m_pInspDlg; };
	CHistoryDlg* GetHistoryDlgPtr() { return m_pHistoryDlg; };
	CRecipeSettingDlg* GetCondDlgPtr() { return m_pCondDlg; };

	void SetMapDlgSize(CRect rcMapSize);

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CNotchingGradeInspView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void OnInitialUpdate();

protected:

	UINT_PTR m_TID_IO;
	BOOL Set_I0Timer();
	BOOL Kill_IoTimer();

	void CheckDiskSpace();	// 22.07.01 Ahn Add Start

	UINT_PTR m_TID_IO_Check;
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	UINT_PTR m_SpcStatus;
#endif //SPCPLUS_CREATE
	BOOL SetSignalCheckTimer();
	BOOL KillSignalCheckTimer();

	UINT_PTR m_TID_Alive_Pulse;
	BOOL SetAlivePulseTimer();
	BOOL KillAlivePulseTimer();

	UINT_PTR m_TID_Long_Term;
	BOOL SetLogTermTimer();
	BOOL KillLongTermTimer();

	void ResetSignal();
	void SigInitialize();

	BOOL m_bLoadGlsInfoReq;
	WORD m_bLastAlarmCode;
	BOOL m_bLotEndProc;
	BOOL m_bLotStartFlag;
	BOOL m_bLotStartInitFlag;
	BOOL m_bLotEndFlag;
	BOOL m_bTabCountResetFlag;
	BOOL m_bAlarmResetFlag;
	BOOL m_bRecipeChagneFlag;
	BOOL m_bInkMarkActiveFlag;
	BOOL m_bConnectZoneFlag;
	BOOL m_bAlarmNgAck;

	CDeleteResultFileThread* m_pDeleteThread;

	BOOL m_bDispMap;
// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnCameraView();

private :
	CCameraWaveDlg* m_pCamWaveDlg;

	int		m_nCamErrorResetCnt ;
	int		m_nStatus;
	BOOL	m_bEncoderReset;
	enum {
		en_Initialize	= 1,
		en_InspStop		= 2,
		en_CameraReset	= 3,	// 22.05.19 Ahn Add
		en_WaitReady	= 10,
		en_Ready		= 11,
		en_PrepareRun	= 15,
		en_Run			= 20,
		en_LotChange	= 30,
		en_ErrorStop	= 100,
	};
	BOOL m_bReady;
	BOOL m_bLotChangeReq;
	int	 m_nRunCheckCounter;

	int InspectionStart();
	int InspectionEnd();
	int CameraGrabStart();
	int CameraGrabStop();
	int GrabberResetReqest();
	int CheckLotEndProcess();

	int CheckLotEndProcess2(); //조건 없이 Lot End Check
	int CheckTabZeroReset();
	int CheckLotStartProcess();
	int CheckAlarmReset();
	int CheckRecipeChange();
	int CheckInkMarkActive();
	int CheckConnectZone();
	int CheckAlarmNgAck();

	void StartThreadAliveSiginal();
	static UINT AliveThread(LPVOID lpParm);


private:
	static CCriticalSection	m_csMelsecThread;
	CWinThread* m_pThread;
public :
	BOOL m_bDebugLotStartReq ;
	BOOL m_bDebugLotEndReq ;
	BOOL m_bDebug_counterResetReq;

public:
	BOOL IsInspReady();
	BOOL SetInspReady(BOOL bMode);
	BOOL IsRun();
	BOOL GetLotChangeReq();
	void SetLotChangeReq(BOOL bMode);
	void ChangeStatus(int nStatus);

	void ReDrawMap( BOOL bModeRect, CRect rcRange );
	int m_nRedrawCnt ;

	void SwitchDisplay(BOOL bMode);

	afx_msg void OnCameraWave();
	afx_msg void OnDebugImageAcqStart();

	CDebugImageAcqDlg* pImgAcqDlg;
	void CloseDebugImgAcqDlg();
	afx_msg void OnRecipeSetting();
	afx_msg void OnSystemSetting();
	afx_msg void OnBackupDeleteSetting();
	afx_msg void OnDebugControl();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // NotchingGradeInspView.cpp의 디버그 버전
inline CNotchingGradeInspDoc* CNotchingGradeInspView::GetDocument() const
   { return reinterpret_cast<CNotchingGradeInspDoc*>(m_pDocument); }
#endif

