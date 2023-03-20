#ifndef	_WINTOOL_H
#define	_WINTOOL_H
//긂귻깛긤긂긚듫쁀궻붍뾭맜궻뛼궋듫릶궳궥
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Windows 긶긞??궔귞뻲궵럊뾭궠귢궶궋긚?긞긲귩룣둖궢귏궥갃

#include <afxwin.h>         // MFC 궻긓귺궓귝귂뷭?긓깛??긨깛긣
#include <afxext.h>         // MFC 궻둮뮗븫빁
#include <afxdisp.h>        // MFC 궻 OLE 긆?긣긽?긘깈깛 긏깋긚



#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 궻 Windows 긓긾깛 긓깛긣깓?깑 긖??긣
#include <ole2.h>
#include <comdef.h>


#endif // _AFX_NO_AFXCMN_SUPPORT




BOOL PumpMessages( LPMSG lpMsg = NULL, HWND hWnd = NULL, UINT wMsgFilterMin = WM_PAINT, UINT wMsgFilterMax = WM_PAINT, UINT wRemoveMsg = PM_REMOVE ) ;
//int PcConnection( HWND hwndOwner, LPTSTR lpstrPcName, LPTSTR lpstrUserName, LPTSTR lpstrPassWord ) ;
CString	FormatErrorMsg( DWORD dwErrNo ) ;
BOOL SetPriorityClassMacro( DWORD dwPriority, DWORD *pdwBackPriority = NULL, int nMode = 0 ) ;
BOOL PickMessages( LPMSG lpMsg = NULL, HWND hWnd = NULL, UINT wMsgFilterMin = WM_PAINT, UINT wMsgFilterMax = WM_PAINT, UINT wRemoveMsg = PM_REMOVE ) ;


#endif