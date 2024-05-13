
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include "TopPanel.h"
#include "BottomPanel.h"
#include "CCropImageViewDlg.h"
#include "CHistoryLotDlg.h"
#include "InspInfoDlg.h"
#include "ResultViewDlg.h"

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
	int OnRefresh_HistoryLotView();
	int OnRefresh_CropImageView();
	int ShowResultPanel(BOOL bShow);
	void ResetResultViewDlg();

	//마킹 BCD ID를 갱신한다.
	void UpdateBCDID();

	//마킹 BCD ID를 출럭한다.
	void UpdateBCDIDData(int nowBCDID, int lastBCDID, int grabBCDID);

	void LoadHistoryData();

	void ChangeStatus(int nStatus) { m_pWndTopPanel->ChangeState(nStatus) ; };

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 구현입니다.
private:
	CPoint m_cpInspDispPos;
	CPoint m_cpMapSize ; 
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CPoint GetInspDIspPosition() { return m_cpInspDispPos; };
	CPoint GetInspMapSize() { return m_cpMapSize; };

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCStatusBar	m_wndStatusBar;
	CTopPanel*		m_pWndTopPanel;
	CCropImageViewDlg *m_pCropDlg;
	CHistoryLotDlg* m_pHistoryDlg;
	CInspInfoDlg* m_pInspInfoDlg;
	CResultViewDlg* m_pResultDlg;
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};


