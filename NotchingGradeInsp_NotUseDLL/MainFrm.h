
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include "TopPanel.h"
#include "BottomPanel.h"
// 22.07.21 Ahn Modify Start
//#include "ResultView.h"
//#include "LeftPanel.h"
//#include "RightPanel.h"
//#include "CCropImageView.h" // 22.06.09 Ahn Add 
#include "CCropImageViewDlg.h"
#include "CHistoryLotDlg.h"
#include "InspInfoDlg.h"
#include "ResultViewDlg.h"
// 22.07.21 Ahn Modify End

class CHistoryLotView;
class CNotchingGradeInspView;

class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	COLORREF	m_clrBkColor;
	int			m_nTopPanelHeight;

// 작업입니다.
protected:
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

public:
	void ReflashAll();
	void ResetAndRefreshAll();
	// 22.07.21 Ahn Modify Start
	// 22.06.24 Ahn Modify Start
	////int OnRefresh_ResultView();
	//int OnRefresh_HistoryLotView();
	//// 22.06.24 Ahn Modify End
	//int ShowResultViewPanel(BOOL bShow);
	//int ShowLeftPanel(BOOL bShow);
	//int ShowRightPanel(BOOL bShow);
	//// 22.06.09 Ahn Add Start
	//int OnRefresh_CropImageView();
	//int ShowCropImagePanel(BOOL bShow);
	//// 22.06.09 Ahn Add End
	int OnRefresh_HistoryLotView();
	int OnRefresh_CropImageView();
	int ShowResultPanel(BOOL bShow);
	// 22.07.21 Ahn Modify Start
	void ResetResultViewDlg();



	void LoadHistoryData();
// 22.07.21 Ahn Delete Start
//// 22.06.24 Ahn Modify Start
//////KANG 22.01.07 Add Start
////	CResultView* GetResultViewPtr() { return m_pWndResultView; };
//////KANG 22.01.07 Add End
//	CHistoryLotView* GetHistoryLotViewPtr() { return m_pWndHistoryLotView; };
//// 22.06.24 Ahn Modify End
//// 22.06.09 Ahn Add Start
//	CCropImageView* GetCropImageViewPtr() { return m_pWndCropImgView; };
//// 22.06.09 Ahn Add End
// 22.07.21 Ahn Delete End
	// 22.06.13 Ahn Add Start
	//CModeDlg* GetModDlgPtr() { return m_pWndTopPanel->GetModeDlgPtr(); }
	void ChangeStatus(int nStatus) { m_pWndTopPanel->ChangeState(nStatus) ; };
	// 22.06.13 Ahn Add End

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 구현입니다.
// 22.07.22 Ahn Add Start
private:
	CPoint m_cpInspDispPos;
// 22.07.22 Ahn Add End
// 22.11.09 Ahn Add Start
	CPoint m_cpMapSize ; 
// 22.11.09 Ahn Add End
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// 22.07.22 Ahn Add Start
	CPoint GetInspDIspPosition() { return m_cpInspDispPos; };
	// 22.07.22 Ahn Add End
	// 22.11.09 Ahn Add Start
	CPoint GetInspMapSize() { return m_cpMapSize; };
	// 22.11.09 Ahn Add End

protected:  // 컨트롤 모음이 포함된 멤버입니다.
//	CMFCMenuBar		m_wndMenuBar; 	// 22.07.21 Ahn Delete 
//	CMFCToolBar		m_wndToolBar;
	CMFCStatusBar	m_wndStatusBar;
	CTopPanel*		m_pWndTopPanel;
	// 22.07.21 Delete Start
	// 
	//CHistoryLotView* m_pWndHistoryLotView;
	//CLeftPanel*		m_pWndLeftPanel;
	//CRightPanel*	m_pWndRightPanel;
	//CCropImageView* m_pWndCropImgView;	// 22.06.09 Ahn Add 
	// 22.07.21 Delete End
	// 22.07.21 Ahn Add Start
	CCropImageViewDlg *m_pCropDlg;
	CHistoryLotDlg* m_pHistoryDlg;
	CInspInfoDlg* m_pInspInfoDlg;
	CResultViewDlg* m_pResultDlg;
	// 22.07.21 Ahn Add End
	CBottomPanel*	m_pWndBottomPanel;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView; // 22.11.09 Ahn Add

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	// 22.07.21 Ahn Delete Start
	//// 22.06.24 Ahn Modify Start
	////afx_msg void OnResultView();
	////afx_msg void OnUpdateResultView(CCmdUI* pCmdUI);
	//afx_msg void OnHistoryLotView();
	//afx_msg void OnUpdateHistoryLotView(CCmdUI* pCmdUI);
	//// 22.06.24 Ahn Modify End
	//afx_msg void OnInspInfoView();
	//afx_msg void OnUpdateInspInfoView(CCmdUI* pCmdUI);
	//afx_msg void OnTrendView();
	//afx_msg void OnUpdateTrendView(CCmdUI* pCmdUI);
	// 22.07.21 Ahn Delete End
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	// 22.07.21 Ahn Delete Start
	//	afx_msg void OnCropImageView();
	//	afx_msg void OnUpdateCropImageView(CCmdUI* pCmdUI);
	// 22.07.21 Ahn Delete End
};


