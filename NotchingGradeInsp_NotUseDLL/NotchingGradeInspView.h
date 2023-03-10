
// NotchingGradeInspView.h: CNotchingGradeInspView 클래스의 인터페이스
//

#pragma once

#include "InspDlg.h"
#include "HistoryDlg.h"
#include "CondDlg.h"
#include "CDefectMapDlg.h" // 22.11.09 Ahn Add 

//class CCameraViewDlg;
class CRecipeSettingDlg;// 22.07.21 Ahn Add
class CCameraWaveDlg;
class CDebugImageAcqDlg;
class CResultFileManager;// 22.04.21 Ahn Add
class CDeleteResultFileThread; // 22.07.04 Ahn Add

class CNotchingGradeInspView : public CView
{
protected: // serialization에서만 만들어집니다.
	CNotchingGradeInspView() noexcept;
	DECLARE_DYNCREATE(CNotchingGradeInspView)

// 특성입니다.
public:
	CNotchingGradeInspDoc* GetDocument() const;

protected:
	CInspDlg*		m_pInspDlg;
	// 22.11.09 Ahn Add Start
	CDefectMapDlg* m_pDefMapDlg;
	// 22.11.09 Ahn Add End

	CHistoryDlg*	m_pHistoryDlg;
	// 22.07.21 Ahn Modify Start
	//CCondDlg*		m_pCondDlg;
	CRecipeSettingDlg* m_pCondDlg;
	// 22.07.21 Ahn Modify End

	//// 22.04.21 Ahn Add Start
	//CResultFileManager* m_pFileManager;
	//// 22.04.21 Ahn Add End

// 작업입니다.
public:
	int ChangeViewMode(int nMode);
	int OnRefresh();
//KANG 22.01.07 Add Start
	CInspDlg* GetInspDlgPtr() { return m_pInspDlg; };
	CHistoryDlg* GetHistoryDlgPtr() { return m_pHistoryDlg; };
	// 22.07.21 Ahn Modify Start
	//CCondDlg* GetCondDlgPtr() { return m_pCondDlg; };
	CRecipeSettingDlg* GetCondDlgPtr() { return m_pCondDlg; };
	// 22.07.21 Ahn Modify End
//KANG 22.01.07 Add End

	// 22.11.15 Ahn Add Start
	void SetMapDlgSize(CRect rcMapSize);
	// 22.11.15 Ahn Add End

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

	//int SaveStatusInfo();

	// 22.03.23 Ahn Add Start
	UINT_PTR m_TID_IO;
	BOOL Set_I0Timer();
	BOOL Kill_IoTimer();

	void CheckDiskSpace();	// 22.07.01 Ahn Add Start

	UINT_PTR m_TID_IO_Check;
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

	// 22.03.24 Ahn Add Start
	BOOL m_bLoadGlsInfoReq;
	WORD m_bLastAlarmCode;
	// 22.03.24 Ahn Add End
	// 22.03.29 Ahn Add Start
	BOOL m_bLotEndProc;
	// 22.03.29 Ahn Add End
	//// 22.03.25 Ahn Add Start
	BOOL m_bLotStartFlag ;
	//// 22.03.25 Ahn Add End

	// 22.07.04 Ahn Add Start
	CDeleteResultFileThread* m_pDeleteThread;
	// 22.07.04 Ahn Add End
	// 22.11.09 Ahn Add Start
	BOOL m_bDispMap;
	// 22.11.09 Ahn Add End
// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
//KANG 22.01.07 Delete Start
//	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
//KANG 22.01.07 Delete End
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
//	afx_msg void OnCameraSetting();
	afx_msg void OnCameraView();

private :
//	CCameraViewDlg* m_pCamViewDlg;
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
	// 22.07.06 Ahn Add Start
	int CameraGrabStart();
	int CameraGrabStop();
	// 22.07.06 Ahn Add End
	// 22.05.19 Ahn Add Start
	int GrabberResetReqest();
	// 22.05.19 Ahn Add End
	// 22.07.26 Ahn Add Start
	int CheckLotEndProcess();
	// 22.07.26 Ahn Add End

// 22.06.27 Ahn Add Start
public :
	BOOL m_bDebugLotStartReq ;
	BOOL m_bDebugLotEndReq ;
	BOOL m_bDebug_counterResetReq;
// 22.06.27 Ahn Add End

public:
	BOOL IsInspReady();
	BOOL SetInspReady(BOOL bMode);
	BOOL IsRun();
	BOOL GetLotChangeReq();
	void SetLotChangeReq(BOOL bMode);
	void ChangeStatus(int nStatus);

	// 22.12.01 Ahn Add Start
	void ReDrawMap( BOOL bModeRect, CRect rcRange );
	int m_nRedrawCnt ;
	// 22.12.01 Ahn Add End

	// 23.02.09 Ahn Add Start
	void SwitchDisplay(BOOL bMode);
	// 23.02.09 Ahn Add End

	afx_msg void OnCameraWave();
	afx_msg void OnDebugImageAcqStart();

	CDebugImageAcqDlg* pImgAcqDlg;
	void CloseDebugImgAcqDlg();
	afx_msg void OnRecipeSetting();
	afx_msg void OnSystemSetting();
	afx_msg void OnBackupDeleteSetting();
	afx_msg void OnDebugControl();
};

#ifndef _DEBUG  // NotchingGradeInspView.cpp의 디버그 버전
inline CNotchingGradeInspDoc* CNotchingGradeInspView::GetDocument() const
   { return reinterpret_cast<CNotchingGradeInspDoc*>(m_pDocument); }
#endif

