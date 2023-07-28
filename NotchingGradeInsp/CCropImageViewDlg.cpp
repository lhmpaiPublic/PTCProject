// CCropImageViewDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CCropImageViewDlg.h"
#include "afxdialogex.h"
// 22.06.10 Ahn Add Start
#include "ImageProcessCtrl.h"
#include "CCropImgQueueCtrl.h"
// 22.06.10 Ahn Add End
// 22.12.23 Ahn Test Start
#include "Win32File.h"
// 22.12.23 Ahn Test End
// CCropImageViewDlg 대화 상자

#include "BitmapStd.h"
//유저메시지 - Crop Image 화면 출력 메시지
#define  WM_CROPIMAGEDRAW WM_USER + 14

IMPLEMENT_DYNAMIC(CCropImageViewDlg, CDialogEx)

#define T_ID_CROP_VIEW_TIMER	112
//KANG 22.07.08 Add Start
#define IDC_EXPAND_VIEW	113
//KANG 22.07.08 Add End


CCropImageViewDlg::CCropImageViewDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_CROP_IMAGE_VIEW, pParent)
{
	m_pView = pView ;

	m_CropThumbCtrl.SetThumbCols(6);
	//KANG 22.07.08 Add Start
	m_pExpandView = NULL;
	//KANG 22.07.08 Add End
}

CCropImageViewDlg::~CCropImageViewDlg()
{
	//KANG 22.07.08 Add Start
	if (m_pExpandView != NULL) {
		if (m_pExpandView->m_hWnd != NULL) {
			m_pExpandView->DestroyWindow();
		}
		delete m_pExpandView;
		m_pExpandView = NULL;
	}
	//KANG 22.07.08 Add End
}

void CCropImageViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CROP_THUMBNAIL, m_CropThumbCtrl);
}


BEGIN_MESSAGE_MAP(CCropImageViewDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCropImageViewDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCropImageViewDlg::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_CROP_THUMBNAIL, OnSelectImage)		// 22.07.01 Ahn Add 
	ON_NOTIFY(NM_DBLCLK, IDC_CROP_THUMBNAIL, OnChangeDispExpand)		//KANG 22.07.08 Add
	ON_NOTIFY(NM_DBLCLK, IDC_EXPAND_VIEW, OnChangeDispThumbnail)		//KANG 22.07.08 Add
	ON_MESSAGE(WM_CROPIMAGEDRAW, &CCropImageViewDlg::OnCropImageDraw) //Crop Image를 그리기 위한 메시지
END_MESSAGE_MAP()


// CCropImageViewDlg 메시지 처리기


void CCropImageViewDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CCropImageViewDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CCropImageViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//	strFileName.Format(_T("%s\\%s"), strPath, strTemp ) ;
	//	m_CropThumbCtrl.AddImageBufferMode(strFileName, strTempName) ;
	//}

	//SetCropViewTimer();

	//m_CropThumbCtrl.SetImageList(m_strImageList);
	m_CropThumbCtrl.OnRefresh();
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CCropImageViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rectControl, rectParent;
	CButton* pButton = NULL;
	int nWidth = 0, nHeight = 0;
	int nXPos = 0, nYPos = 0;
	int nLeftMargin = 5, nRightMargin = 5, nTopMargin = 5, nBottomMargin = 5;
	this->GetClientRect(rectParent);

	if ( m_CropThumbCtrl.GetSafeHwnd() ) {
		nYPos += 0 ;
		m_CropThumbCtrl.SetWindowPos(NULL, nLeftMargin, nYPos, (cx - nLeftMargin - nRightMargin), (cy - nYPos - nBottomMargin), SWP_NOZORDER);
	}
	//KANG 22.07.12 Add Start
	if ((m_pExpandView != NULL) && (m_pExpandView->m_hWnd != NULL)) {
		m_pExpandView->MoveWindow(0, 0, cx, cy);
		m_pExpandView->Invalidate();
	}
	//KANG 22.07.12 Add End
}


void CCropImageViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == T_ID_CROP_VIEW_TIMER) {
		KillCropViewTimer();

		CCropImgQueueCtrl* pQueue = theApp.m_pImgProcCtrl->GetCropImageQueuePtr();
		if (pQueue != NULL)
		{

			while (pQueue->IsEmpty() == FALSE)
			{
				CCropImgData data = pQueue->Pop();
				if (data.m_bEnable == TRUE)
				{
					m_CropThumbCtrl.AddImageBufferMode(data.m_strFileName, data.m_strDispName);
				}
			}
			SendMessage(WM_CROPIMAGEDRAW);
		}
		//UpdateData(FALSE);
		SetCropViewTimer();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CCropImageViewDlg::SetCropViewTimer()
{
	SetTimer(T_ID_CROP_VIEW_TIMER, 10, NULL);
}
void CCropImageViewDlg::KillCropViewTimer()
{
	KillTimer(T_ID_CROP_VIEW_TIMER);
}


// 22.07.01 Ahn Add Start
void CCropImageViewDlg::OnSelectImage(NMHDR* pNMHDR, LRESULT* pResult)
{
//	int nIndex = (int*)pNMHDR;
	NM_THUMBDATA *pNmgv = (NM_THUMBDATA*)pNMHDR;

	int nTotalNo = m_CropThumbCtrl.GetImageCount();
	int nSelectNo = pNmgv->nIndex ;

	// m_pParent->SelectGridDefNo( nSelectNo );

}
// 22.07.01 Ahn Add End

// 22.07.06 Ahn Add Start
void CCropImageViewDlg::ClearAllCropImage()
{
	m_CropThumbCtrl.ClearAll();
	m_CropThumbCtrl.OnRefresh();
	UpdateData(FALSE);
}
// 22.07.06 Ahn Add End

//KANG 22.07.08 Add Start
void CCropImageViewDlg::OnChangeDispExpand(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_THUMBDATA* pNmgv = (NM_THUMBDATA*)pNMHDR;

	int nTotalNo = m_CropThumbCtrl.GetImageCount();
	int nSelectNo = pNmgv->nIndex;
	CString strFileName = m_CropThumbCtrl.GetImageFileNameFromList(nSelectNo);

	if (strFileName.GetLength() < 7) return;
	
	if (m_pExpandView == NULL) {
		CRect rectClient;
		this->GetClientRect(&rectClient);
		m_pExpandView = new CExpandViewImage();
		if (m_pExpandView->Create(_T(""), WS_CHILD | SS_NOTIFY, CRect(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height()), this, IDC_EXPAND_VIEW) == FALSE) {
			return;
		}
		m_pExpandView->SetLClickClose(FALSE);
		m_pExpandView->SetRClickClose(FALSE);
	}

	KillCropViewTimer();
	m_CropThumbCtrl.ShowWindow(SW_HIDE);
	if (m_pExpandView != NULL) {
		m_pExpandView->SetImageFile(strFileName);
		m_pExpandView->ShowWindow(SW_SHOW);
	}
}

void CCropImageViewDlg::OnChangeDispThumbnail(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_THUMBDATA* pNmgv = (NM_THUMBDATA*)pNMHDR;

	if (m_pExpandView != NULL) {
		m_pExpandView->ShowWindow(SW_HIDE);
	}
	m_CropThumbCtrl.ShowWindow(SW_SHOW);
	SetCropViewTimer();
}

LRESULT CCropImageViewDlg::OnCropImageDraw(WPARAM wParam, LPARAM lParam)
{
	m_CropThumbCtrl.OnRefresh();
	m_CropThumbCtrl.Invalidate(FALSE);
	UpdateWindow();
		
	return 0;
}
