// CHistogramDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CHistogramDlg.h"
#include "afxdialogex.h"


// CHistogramDlg 대화 상자

IMPLEMENT_DYNAMIC(CHistogramDlg, CDialogEx)

CHistogramDlg::CHistogramDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_HISTOGRAM, pParent)
{
	m_nMax = 100;
	m_nMin = 0;
}

CHistogramDlg::~CHistogramDlg()
{
}

void CHistogramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUST_HISTOGRAM, m_cGrphHist);

}


BEGIN_MESSAGE_MAP(CHistogramDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CHistogramDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CHistogramDlg 메시지 처리기


BOOL CHistogramDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_cGrphHist.DrawFrameRect(TRUE);
	m_cGrphHist.SetDataColor(0, RGB(128, 255, 128));
	m_cGrphHist.SetMaxVerScaleSize(250);


	m_cGrphHist.SetDrawIndividual(TRUE);
	m_cGrphHist.SetDrawData(0, TRUE);
	m_cGrphHist.SetHorScaleInterval(10);
	m_cGrphHist.SetGraphSize(256);
//	m_cGrphHist.AdjustSize();
	m_cGrphHist.OnRefresh(256);
	m_cGrphHist.Invalidate(FALSE);

//	m_cGrphHist.EnableWindow(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CHistogramDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_cGrphHist.GetSafeHwnd() != nullptr) {
		m_cGrphHist.MoveWindow(5, 5, (cx - 5), (cy / 2) - 50);
		m_cGrphHist.ScaleChange(10);
	}
}


void CHistogramDlg::SetHistoData(int* nHistoArray, int nSize )
{

	ASSERT(nHistoArray);

	if (m_cGrphHist.m_hWnd == nullptr) return;
	m_cGrphHist.SetFixHScroll(FALSE);

	m_nSum = 0;
	m_nMax = 0;
	m_nMin = 9999;
	for (int i = 0; i < 256; i++) {
		if (m_nMax < nHistoArray[i]) {
			m_nMax = nHistoArray[i];
		}
		if (m_nMin > nHistoArray[i]) {
			m_nMin = nHistoArray[i];
		}
		m_nSum += nHistoArray[i];
	}

	double dRate = (double)(255.0 / (double)m_nMax);
	for (int i = 0; i < 256; i++) {
		int nValue = (int)((double)nHistoArray[i] * dRate);
		m_cGrphHist.SetData(0, i, nValue);
	}
	m_cGrphHist.SetMaxVerScaleSize( 256 );
	int nScaleIInterval = (m_nMax / 20) ;
	m_cGrphHist.SetVerMajorScaleInterval(nScaleIInterval);
	m_cGrphHist.AdjustSize();
	m_cGrphHist.OnRefresh(0);
	m_cGrphHist.Invalidate(FALSE);

}

void CHistogramDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
