// UiManager.h : UiManager 관리 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined UIMANAGER_H
#define UIMANAGER_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNotchingGradeInspView;
class CMainFrame;
class CModeDlg;
class CUiManager
{
	//자신의 전역 객체 생성
	static CUiManager* gInstObject;

	//class 인스턴스 객체
	CNotchingGradeInspView* m_pNotchingGradeInspView;
	CMainFrame* m_pMainFrame;
	CModeDlg* m_pModeDlg;
public:
	CUiManager();
	virtual ~CUiManager() ;

	//전역 인스턴스 객체 생성 및 접근
	static CUiManager* gInst();

	//UiManager 메모리 해제
	static void CreateUiManager();

	//UiManager 메모리 해제
	static void ExitUiManager();

	//UI Instance 객체
public:
	//get
	//class 인스턴스 객체
	CNotchingGradeInspView* getNotchingGradeInspView() { return m_pNotchingGradeInspView; }
	CMainFrame* getMainFrame() { return m_pMainFrame; }
	CModeDlg* getModeDlg() { return m_pModeDlg; }

	//set
	//class 인스턴스 객체
	void setNotchingGradeInspView(CNotchingGradeInspView* pNotchingGradeInspView) { m_pNotchingGradeInspView = pNotchingGradeInspView; }
	void setMainFrame(CMainFrame* pMainFrame) { m_pMainFrame = pMainFrame; }
	void setModeDlg(CModeDlg* pModeDlg) { m_pModeDlg = pModeDlg; }

	//del
	void delNotchingGradeInspView() { m_pNotchingGradeInspView = NULL; }
	void delMainFrame() { m_pMainFrame = NULL; }
	void delModeDlg() { m_pModeDlg = NULL; }

};

#define  UIMGR CUiManager::gInst()

#endif // UIMANAGER_H
