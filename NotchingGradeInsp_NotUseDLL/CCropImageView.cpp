#include "pch.h"
#include "NotchingGradeInsp.h"
#include "MainFrm.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "CCropImageView.h"

// CCropImageView

IMPLEMENT_DYNAMIC(CCropImageView, CDockablePane)

CCropImageView::CCropImageView(CWnd* pParent /*= NULL*/, CNotchingGradeInspView* pView /*= NULL*/)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = m_pView->GetDocument();
	}
	m_pCropImgViewDlg = NULL;
}

CCropImageView::~CCropImageView()
{
	if (m_pCropImgViewDlg != nullptr) {
		if (m_pCropImgViewDlg->m_hWnd != nullptr) {
			m_pCropImgViewDlg->DestroyWindow();
		}
		delete m_pCropImgViewDlg;
		m_pCropImgViewDlg = NULL;
	}
}


BEGIN_MESSAGE_MAP(CCropImageView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



int CCropImageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	if (m_pCropImgViewDlg == nullptr) {
		m_pCropImgViewDlg = new CCropImageViewDlg(this, m_pView);
		if (m_pCropImgViewDlg != nullptr) {
			if (m_pCropImgViewDlg->Create(IDD_DLG_CROP_IMAGE_VIEW, this) == 0) {
				delete m_pCropImgViewDlg;
				m_pCropImgViewDlg = NULL;
				return -1;
			}
			else {
				m_pCropImgViewDlg->ShowWindow(SW_SHOW);
			}
		}
	}

	return 0;
}


void CCropImageView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	RecalcLayout();
	if (m_pCropImgViewDlg->GetSafeHwnd()) {
		m_pCropImgViewDlg->SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, SWP_NOZORDER);
		CRect rc;
		m_pCropImgViewDlg->GetClientRect(&rc);
		m_pCropImgViewDlg->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
	}
}


void CCropImageView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDockablePane::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CRect rect;
	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(pFrame->m_clrBkColor));
}

int CCropImageView::OnRefresh()
{
	// TODO: ���⿡ ���� �ڵ� �߰�.
	int nRet = 0;
	//nRet = m_pCropImgViewDlg->ImageReflesh;
	return nRet;
}

void CCropImageView::ClearAllData()
{
	ASSERT(m_pCropImgViewDlg);
	m_pCropImgViewDlg->ClearAllCropImage();
}


