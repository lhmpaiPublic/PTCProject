
// NotchingGradeInsp.h: NotchingGradeInsp 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "ResultView.h"


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
class CDispErrorDlg;

class CSpcInfo;
class CNotchingGradeInspApp : public CWinAppEx
{
	//전역 객체 생성
	CSpcInfo* objSpcInfo;
public:
	CNotchingGradeInspApp() noexcept;


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

#ifndef NEW_PLCTYPE
	CPioCtrl* m_pPioCtrl;
#endif //NEW_PLCTYPE

	CPlcImp* m_pSigProc; 

	CStdIoCtrl* m_pIoCtrl;
	CImageProcSimDlg * m_pImageSimDlg ;
	CLightControl* m_pLightCtrl;

	CDispErrorDlg* m_pDispErrorDlg;

	int ErrOutput(LPCTSTR ptstr, int nErrorType );
	int DispCapacityAlarm(int nMode);
	CCriticalSection m_csError;
	int LoadLastDefList();

};

extern CNotchingGradeInspApp theApp;
