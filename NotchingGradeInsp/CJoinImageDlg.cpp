// CJoinImageDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CJoinImageDlg.h"
#include "afxdialogex.h"
#include "BitmapStd.h"
#include "Win32File.h"



// CJoinImageDlg 대화 상자

IMPLEMENT_DYNAMIC(CJoinImageDlg, CDialogEx)

CJoinImageDlg::CJoinImageDlg(CWnd* pParent /*=nullptr*/, CString strFilePath )
	: CDialogEx(IDD_DLG_ATTACH_IMAGE, pParent)
	, m_strImageName1(_T(""))
	, m_strImageName2(_T(""))
	, m_strImagePath(_T(""))
{
}

CJoinImageDlg::~CJoinImageDlg()
{
}

void CJoinImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_IMAGE_1, m_strImageName1);
	DDX_Text(pDX, IDC_ED_IMAGE_2, m_strImageName2);
}


BEGIN_MESSAGE_MAP(CJoinImageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOAD_IMG_1, &CJoinImageDlg::OnBnClickedBtnLoadImg1)
	ON_BN_CLICKED(IDC_BTN_LOAD_IMG_2, &CJoinImageDlg::OnBnClickedBtnLoadImg2)
	ON_BN_CLICKED(IDOK, &CJoinImageDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CJoinImageDlg 메시지 처리기


void CJoinImageDlg::OnBnClickedBtnLoadImg1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strRet;
	strRet = GetSelectFilePath();
	if (strRet.GetLength() > 0) {
		m_strImageName1 = strRet;
	}
	UpdateData(FALSE);
}

void CJoinImageDlg::OnBnClickedBtnLoadImg2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strRet;
	strRet = GetSelectFilePath();
	if (strRet.GetLength() > 0) {
		m_strImageName2 = strRet;
	}
	UpdateData(FALSE);
}

CString CJoinImageDlg::GetSelectFilePath()
{
	CString stRet = _T("");
	TCHAR szFilter[256] = _T("Image Files (*.bmp;*.JPG;*.JPEG)|*.bmp;*.JPG;*.JPEG|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("bmp"), m_strImagePath, OFN_READONLY, szFilter, this);
	if (dlg.DoModal() == IDOK) {
		stRet = dlg.GetPathName();
	}	
	return stRet;
}

void CJoinImageDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nLeng1 = m_strImageName1.GetAllocLength();
	int nLeng2 = m_strImageName2.GetAllocLength();
	if ((nLeng1 < 7) || (nLeng1 < 7)) {
		MessageBox(_T("경로 지정이 정상적인지 확인하시기 바랍니다."));
	}

	CBitmapStd bmp1;
	CBitmapStd bmp2;

	bmp1.ReadBitmap(m_strImageName1);
	bmp2.ReadBitmap(m_strImageName2);

	BYTE* pImg1 = bmp1.GetImgPtr();
	BYTE* pImg2 = bmp2.GetImgPtr();

	CSize size1, size2;
	size1 = bmp1.GetImgSize();
	size2 = bmp2.GetImgSize();

	if (size1.cx != size2.cx) {
		MessageBox(_T("두 이미지의 폭이 동일하지 않습니다."));
		return ;
	}

	int nWidth = (int)size1.cx;
	int nHeight = (int)size1.cy + (int)size2.cy;

	BYTE* pImgJoin;
	CBitmapStd bmpJoin( nWidth, nHeight ) ;
	pImgJoin = bmpJoin.GetImgPtr();
	memset(pImgJoin, 0x00, sizeof(BYTE) * (nWidth * nHeight));

	int x, y;
	// 1번이미지 복사

	BYTE* pLinePtr;
	BYTE* pSrcLinePtr;

	for (y = 0; y < size2.cy; y++) {
		pLinePtr = pImgJoin + (size2.cx * y);
		pSrcLinePtr = pImg2 + (nWidth * y);
		for (x = 0; x < size2.cx; x++) {
			CopyMemory(pLinePtr, pSrcLinePtr, (sizeof(BYTE) * size2.cx));
		}
	}

	for (y = 0; y < size1.cy; y++) {
		pLinePtr = pImgJoin + (nWidth * (y + size2.cy));
		pSrcLinePtr = pImg1 + (size1.cx * y );
		for (x = 0; x < size1.cx; x++) {
			CopyMemory(pLinePtr, pSrcLinePtr, ( sizeof(BYTE) *size1.cx ) );
		}
	}

	CString strJoinImgName  ;
	//strJoinImgName.Replace(_T("."), _T("_Join."));

	int nPos = m_strImageName1.ReverseFind('\\');
	strJoinImgName.Format( _T("%s\\JoinImage.jpg" ), m_strImageName1.Left(nPos) );



	bmpJoin.SetJpegQuality(100);
	bmpJoin.SaveBitmap(strJoinImgName);

}
