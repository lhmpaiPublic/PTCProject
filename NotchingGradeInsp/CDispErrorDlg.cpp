// CDispErrorDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CDispErrorDlg.h"
#include "afxdialogex.h"
#include "GlobalData.h" // 22.06.30 Ahn Add
#include "SigProc.h"	// 22.08.05 Ahn Add
enum {
	eCheckMsg_ID = 1,
	eCheckMsg_Time = 500,

	eAlarmReset_ID	= 2,
	eAlarmReset_Time = 200,
};
typedef struct {
	COLORREF on_bg;
	COLORREF on_txt;
	COLORREF off_bg;
	COLORREF off_txt;
} _BLINK_COLOR ;

const _BLINK_COLOR m_color[] = {
	{ 0x0000ff, 0xffffff, 0x00ffff, 0x000000 },
	{ 0x00ff00, 0xffffff, 0xffffff, 0x000000 },
};
// CDispErrorDlg 대화 상자

IMPLEMENT_DYNAMIC(CDispErrorDlg, CDialogEx)

CDispErrorDlg::CDispErrorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DISPERROR, pParent)
{
	m_bReset = FALSE;
}

CDispErrorDlg::~CDispErrorDlg()
{
}

void CDispErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_MSGSTR, m_ErrTextStatic);
	DDX_Control(pDX, IDC_LIST_MSG, m_ListBox);
}


BEGIN_MESSAGE_MAP(CDispErrorDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &CDispErrorDlg::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDispErrorDlg 메시지 처리기


void CDispErrorDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//#ifdef _DEBUG
//	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, (SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE));
//#else
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE));
//#endif

	m_ListMsg.RemoveAll();
	m_bReset = TRUE;

	for (int i = m_ListBox.GetCount() - 1; i >= 0; i--) {
		m_ListBox.DeleteString(i);
	}

	CDialogEx::OnOK();
}


BOOL CDispErrorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_Font.CreateFontA(50, 20, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET
						, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
						, FIXED_PITCH, _T("Gulim"));
	GetDlgItem(IDC_ED_MSGSTR)->SetFont(&m_Font);
	GetDlgItem(IDOK)->SetFont(&m_Font);

	m_FontListBox.CreateFontA(22, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET
							, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
							, FIXED_PITCH, _T("Gulim"));
	GetDlgItem(IDC_LIST_MSG)->SetFont(&m_FontListBox);

	m_Cnt = 0;
	SetWindowText(_T("ERROR"));
	GetDlgItem(IDC_ED_MSGSTR)->SetWindowTextA(_T("시스템 이상"));
	GetDlgItem(IDOK)->SetWindowTextA(_T("에러 리셋"));

	Data2Dlg();

	// 22.10.04 Ahn Add Start
	SetTimer(eCheckMsg_ID, eCheckMsg_Time, NULL);
	SetTimer(eAlarmReset_ID, eAlarmReset_Time, NULL);
	// 22.10.04 Ahn Add End

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDispErrorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent) {
	case eCheckMsg_ID :
		KillTimer(eCheckMsg_ID); // 22.10.04 Ahn Add 
		//OnTimer 로그출력
		LOGDISPLAY_SPEC(6)("CDispErrorDlg::OnTimer => CheckMsg_ID");

		if (m_ListMsg.GetCount() > 0) {
			ShowWindow(SW_NORMAL);
			Data2Dlg();
		}

		// 22.10.04 Ahn Add Start
		SetTimer(eCheckMsg_ID, eCheckMsg_Time, NULL);
		// 22.10.04 Ahn Add Start

		break;
	case eAlarmReset_ID :
		KillTimer(eAlarmReset_ID);

		//OnTimer 로그출력
		LOGDISPLAY_SPEC(6)("CDispErrorDlg::OnTimer => AlarmReset_ID");
		if (IsWindowVisible() == TRUE) {
			if (IsAlarmReset() == TRUE) {
				OnBnClickedOk();
				//	SendMessage(WM_COMMAND, IDOK);
			}
		}
		SetTimer(eAlarmReset_ID, eAlarmReset_Time, NULL);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CDispErrorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ListMsg.RemoveAll();
}


void CDispErrorDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE) {
		m_bReset = FALSE;
//#ifdef _DEBUG
//		::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, (SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE));
//#else
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE));
//#endif
	}
}


bool CDispErrorDlg::AddMessage(CString msg)
{
	// TODO: 여기에 구현 코드 추가.
	CSingleLock lock(&m_cs, TRUE);

	BOOL bAdd = FALSE;

	POSITION pos = m_ListMsg.Find(msg);
	if (pos == NULL) {
		m_ListMsg.AddTail(msg);
		bAdd = TRUE;
	}

	return( bAdd );
}


bool CDispErrorDlg::IsError()
{
	// TODO: 여기에 구현 코드 추가.
	CSingleLock lock(&m_cs, TRUE);

	Data2Dlg();

	int Cnt = m_ListBox.GetCount();

	if (Cnt > 0) {
		return(TRUE);
	}
	return (FALSE);
}


void CDispErrorDlg::Data2Dlg()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_Cnt & 0x0001) {
		m_ErrTextStatic.SetTextColor(m_color[1].on_txt);
		m_ErrTextStatic.SetBackgroundColor(m_color[1].on_bg);
	}
	else {
		m_ErrTextStatic.SetTextColor(m_color[0].on_txt);
		m_ErrTextStatic.SetBackgroundColor(m_color[0].on_bg);
	}
	GetDlgItem(IDC_ED_MSGSTR)->RedrawWindow();

	int longestStrLength = 0;
	CString longestStr;
	if (m_ListBox.GetCount() != m_ListMsg.GetCount()) {
		for (int i = m_ListBox.GetCount() - 1; i >= 0; i--) {
			m_ListBox.DeleteString(i);
		}

		POSITION pos = m_ListMsg.GetHeadPosition();
		while (pos != NULL) {
			CString str = m_ListMsg.GetNext(pos);
			m_ListBox.AddString(str);

			if (longestStrLength < str.GetLength()) {
				longestStrLength = str.GetLength();
				longestStr = str;
			}
		}

		CDC* pDC = m_ListBox.GetDC();
		CFont* pFont = m_ListBox.GetFont();
		CFont* pOldFont = pDC->SelectObject(pFont);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		CSize sz = pDC->GetTextExtent(longestStr);

		m_ListBox.SetHorizontalExtent(sz.cx + tm.tmAveCharWidth);

		pDC->SelectObject(pOldFont);
		m_ListBox.ReleaseDC(pDC);
	}

}


void CDispErrorDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AprData.m_ErrStatus.ErrorClear();
	m_ListMsg.RemoveAll();

	CSigProc* pSigProc = theApp.m_pSigProc;
	pSigProc->SigOutDiskCapacityAlarm(FALSE);
	pSigProc->SigOutDiskCapacityWarning(FALSE);

	// 23.01.03 Ahn Add Start
	m_bReset = TRUE;

	for (int i = m_ListBox.GetCount() - 1; i >= 0; i--) {
		m_ListBox.DeleteString(i);
	}
	// 23.01.03 Ahn Add End

	CDialogEx::OnOK();
}


void CDispErrorDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	CDialogEx::OnClose();
}


int CDispErrorDlg::IsAlarmReset()
{
	CSigProc* pSigProc = theApp.m_pSigProc;
	int nRet = 0;
	if (pSigProc != nullptr) {
		nRet = pSigProc->SigInAlarmReset();
	}

	return nRet;
}
