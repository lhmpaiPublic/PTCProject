
// NotchingGradeInsp.h: NotchingGradeInsp 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
//KANG 22.01.07 Add Start
#include "ResultView.h"
//KANG 22.01.07 Add End


//#import "PLCServer.tlb" no_namespace 

// CNotchingGradeInspApp:
// 이 클래스의 구현에 대해서는 NotchingGradeInsp.cpp을(를) 참조하세요.
//
class CImageProcSimDlg;
class CImageProcessCtrl;
class CPioCtrl;
class CStdIoCtrl;
class CLightControl;
class CSigProc;
class CDispErrorDlg;	// 22.06.29 Son Add

class CNotchingGradeInspApp : public CWinAppEx
{
public:
	CNotchingGradeInspApp() noexcept;
	// 22.02.15 Ahn Add Start
	//CComPtr<IPLCManager> m_pPlcManager;
	// 22.02.15 Ahn Add End


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	ULONG_PTR		m_gdiplusToken;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	int		DeviceOpen(void);
	int		DeviceClose(void);

public :
	CImageProcessCtrl *m_pImgProcCtrl ;

	CPioCtrl* m_pPioCtrl;
	// 22.04.01 Ahn Add Start
	CSigProc* m_pSigProc; 
	// 22.04.01 Ahn Add End
	CStdIoCtrl* m_pIoCtrl;
	CImageProcSimDlg * m_pImageSimDlg ;
	CLightControl* m_pLightCtrl;

	// 22.06.29 Son Add Start
	CDispErrorDlg* m_pDispErrorDlg;
	// 22.06.29 Son Add End

	int GetFileVersion(CString& strVersion);
	CString& GetAppBuildInfo(void);	
////KANG 22.01.07 Add Start
//	CResultView* GetResultViewPrt();
////KANG 22.01.07 Add End
	// 22.06.29 Son Add Start
	int ErrOutput(LPCTSTR ptstr, int nErrorType );
	int DispCapacityAlarm(int nMode);
	CCriticalSection m_csError;
	// 22.06.29 Son Add End
	// 22.11.25 Ahn Add Start
	int LoadLastDefList();
	// 22.11.25 Ahn Add End
};

extern CNotchingGradeInspApp theApp;
