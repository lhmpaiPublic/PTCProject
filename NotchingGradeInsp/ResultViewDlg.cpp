// ResultViewDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"

#include "ResultViewDlg.h"
#include "afxdialogex.h"
#include "CDefectDataCtrl.h"
//KANG 22.01.07 Add Start
#include "GlobalData.h"
#include "Win32File.h"
//KANG 22.01.07 Add End
// 22.06.09 Ahn Add Start
#include "FrameRsltInfo.h"
#include "QueueCtrl.h"
#include "ImageProcessCtrl.h"
// 22.06.09 Ahn Add End
// 22.07.07 Ahn Add Start
#include "CCropImgQueueCtrl.h"
// 22.07.07 Ahn Add End

// CResultViewDlg 대화 상자

#define IDC_RESULT_LIST	1001

IMPLEMENT_DYNAMIC(CResultViewDlg, CDialogEx)

CResultViewDlg::CResultViewDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView* pView /*=nullptr*/)
	: CDialogEx(IDD_DLG_RESULT, pParent)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;
	if (m_pView != NULL) {
		m_pDoc = pView->GetDocument();
	}
	m_pResultList = NULL;
	m_pFontGrid = NULL;
	m_bAutoRefresh = TRUE;
	m_pImageViewerDlg = NULL;
//	m_pDefDataCtrl = NULL;
////KANG 22.01.07 Add Start
//	m_pHistList = NULL;
//	m_pFontHist = NULL;
////KANG 22.01.07 Add End
	m_nSelectRow = -1; // 22.07.01 Ahn Add
}

CResultViewDlg::~CResultViewDlg()
{
	if (m_pResultList != nullptr) {
		delete m_pResultList;
		m_pResultList = NULL;
	}
	if (m_pFontGrid != nullptr) {
		m_pFontGrid->DeleteObject();
		delete m_pFontGrid;
		m_pFontGrid = NULL;
	}

	// 22.06.23 Ahn Add Start
	if (m_pImageViewerDlg != NULL) {
		m_pImageViewerDlg->DestroyWindow();
		delete m_pImageViewerDlg;
		m_pImageViewerDlg = NULL;
	}
	// 22.06.23 Ahn Add End

	//if (m_pDefDataCtrl != NULL) {
	//	m_pDefDataCtrl->RemoveAll();
	//	delete m_pDefDataCtrl;
	//	m_pDefDataCtrl = NULL;
	//}
}

void CResultViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_REFRESH, m_bAutoRefresh);
}


BEGIN_MESSAGE_MAP(CResultViewDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHK_REFRESH, &CResultViewDlg::OnBnClickedChkRefresh)
	ON_WM_TIMER()
//	ON_NOTIFY(GVN_SELCHANGING, IDC_HIST_LIST, OnMouseLDownHistList)	//KANG 22.01.07 Add
//	ON_NOTIFY(NM_DBLCLK, IDC_HIST_LIST, OnMouseDblClickHistList)	//KANG 22.01.07 Add
	ON_NOTIFY(NM_DBLCLK, IDC_RESULT_LIST, OnMouseDblClickReltList)
	ON_NOTIFY(NM_CLICK, IDC_RESULT_LIST, OnRBottonDownReltList)		// 22.07.01 Ahn Add 
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CResultViewDlg 메시지 처리기


BOOL CResultViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (m_pResultList == NULL) {
		m_pResultList = new CGridCtrl();
	}
	//if (m_pDefDataCtrl == NULL) {
	//	m_pDefDataCtrl = new CDefectDataCtrl();
	//	m_pDefDataCtrl->RemoveAll();
	//}
	
	MakeGrid();


	// 22.11.25 Ahn Delete Start
	//LoadLastLotData();
	// 22.11.25 Ahn Delete End

	SetTimer(T_GRID_REFRESH, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CResultViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect	btnRect;
	int		nBtnHeight = 0;
	int		nBtnWidth = 0;
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHK_REFRESH);
	if (pButton != nullptr) {
		pButton->GetClientRect(&btnRect);
		nBtnHeight = btnRect.Height();
		nBtnWidth = btnRect.Width();
		pButton->MoveWindow(cx - nBtnWidth-5, cy-nBtnHeight-5, nBtnWidth, nBtnHeight);
	}

	//컴파일 정보를 출력을 위한 객체
	CButton* pMakeText = (CButton*)GetDlgItem(IDC_EXEC_MAKE_TEXT);
	CButton* pMakeDay = (CButton*)GetDlgItem(IDC_EXEC_MAKEDAY);
	//객체 가 있다면
	if ((pMakeText != nullptr) && (pMakeDay != nullptr))
	{
		//객체의 크기를 가져오기 위한 변수
		CRect	MakeTextRect;
		CRect	MakeDayRect;
		//객체의 크기 정보를 가져온다.
		pMakeText->GetClientRect(&MakeTextRect);
		pMakeDay->GetClientRect(&MakeDayRect);
		//객체의 크기정보를 사용하여 위치 및 크기를 재 설정한다.
		pMakeText->MoveWindow(5, cy - nBtnHeight - 5, MakeTextRect.Width(), MakeTextRect.Height());
		pMakeDay->MoveWindow(5 + MakeTextRect.Width() + 5, cy - nBtnHeight - 5, MakeDayRect.Width(), MakeDayRect.Height());
		//버튼의 기능을 사용하지 못하도록 세팅한다.
		pMakeText->EnableWindow(false);
		pMakeDay->EnableWindow(false);
		
		//실행파일 컴파일 정보 출력
		CString strCompileInfo;
		CString strVersion;
		//정보를 만들기 위한 함수
		CNotchingGradeInspApp::GetFileVersion(strVersion);
		//출력을 텍스트를 만든다.
		strCompileInfo.Format(_T("NotchingGradeInsp %s"), strVersion);
		//정보를 윈도우에 출력한다.
		pMakeDay->SetWindowText(strCompileInfo);
		
	}

	if (m_pResultList->GetSafeHwnd()) {
	//KANG 22.01.07 Add Start
		if (m_pDoc != NULL) {
	//KANG 22.01.07 Add End
			m_pResultList->SetWindowPos(NULL, 5, 5, cx - 10, cy - 10 - nBtnHeight-5, SWP_NOZORDER);
	//KANG 22.01.07 Add Start
			if (m_pDoc->GetViewMode() != enHistoryMode) {
				m_pResultList->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_CHK_REFRESH)->ShowWindow(SW_SHOW);
			}
			else {
				m_pResultList->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_CHK_REFRESH)->ShowWindow(SW_HIDE);
			}
		}
	//KANG 22.01.07 Add End
	}
}

int CResultViewDlg::MakeGrid()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_pResultList != nullptr) {
		CRect rect;
		GetClientRect(rect);
		if (m_pResultList->Create(rect, this, IDC_RESULT_LIST) == FALSE) {
			delete m_pResultList;
			m_pResultList = NULL;
			return -1;
		}
		else {
			if (m_pFontGrid == NULL) {
				CFont* pFont = NULL;
				LOGFONT lf;
				pFont = m_pResultList->GetFont();
				pFont->GetLogFont(&lf);
				CString strFontFace = _T("Arial");
				lf.lfHeight = (long)(m_pResultList->GetRowHeight(0) * 1);
				lf.lfWeight = FW_NORMAL;
				::_tcsnccpy_s(lf.lfFaceName, _countof(lf.lfFaceName), strFontFace.GetBuffer(), _TRUNCATE);
				m_pFontGrid = new CFont();
				m_pFontGrid->CreateFontIndirect(&lf);
				m_pResultList->SetFont(m_pFontGrid, TRUE);
			}

			int m_nFixCols = 1;
			int m_nFixRows = 1;
			int m_nCols = en_Max_GridCols;
			int m_nRows = 1;
			int nRow = 0; 
			int nCol = 0;

			m_pResultList->SetAutoSizeStyle();
			m_pResultList->SetFixedColumnSelection(TRUE);
			m_pResultList->SetFixedRowSelection(TRUE);

			//m_pResultList->SetItemBkColour();

			TRY{
				m_pResultList->SetRowCount(m_nRows);
				m_pResultList->SetColumnCount(m_nCols);
				m_pResultList->SetFixedRowCount(m_nFixRows);
				m_pResultList->SetFixedColumnCount(m_nFixCols);
			}
				CATCH(CMemoryException, e) {
				e->ReportError();
				return -1;
			}
			END_CATCH
			// 22.06.23 Ahn Modify Start
			//CString strTitle[en_Max_GridCols] = { _T("No."),_T("검사시각"),_T("LOT_ID"),_T("CELL_ID"),_T("불량위치"),_T("불량종류"),_T("불량 사이즈"), _T("Ink Marking"), _T("Image Path") };
			CString strTitle[en_Max_GridCols];
			strTitle[en_col_No]			= _T("No.") ;
			strTitle[en_col_InspTime]	= _LANG(_T("검사시각"), _T("Insp Time")) ;
			strTitle[en_col_LOT_ID]		= _T("LOT_ID");
			strTitle[en_col_CELL_NO]	= _T("CELL_NO");
			strTitle[en_col_CELL_ID] = _T("CELL_ID");
			strTitle[en_col_POSITION]	= _LANG(_T("불량위치"), _T("Position"));
			strTitle[en_col_DEF_TYPE]	= _LANG(_T("불량종류"), _T("NG Type"));
			strTitle[en_col_JUDGE]		= _LANG(_T("결함판정"), _T("Judgement"));
			strTitle[en_col_DEF_SIZE]	= _LANG(_T("불량 사이즈"), _T("Size"));
			strTitle[en_col_InkMarking] = _LANG(_T("마킹"), _T("Marking"));
			strTitle[en_col_DefPosY]	= _LANG(_T("Pos Y"), _T("Pos Y"));
			strTitle[en_col_ImagePath]	= _LANG(_T("이미지"), _T("Img Path"));
			// 22.06.26 Ahn Modify End

			int nColWidth = 50;
			int nHeight = 25;
			for (nCol = 0; nCol < m_pResultList->GetColumnCount(); nCol++) {
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = 0;
				Item.col = nCol;
				Item.strText = strTitle[ nCol ];		

				//폰트 픽셀 넓이 저정
				CDC* dc = GetDC();
				m_pResultList->setGrideFontWidth(nCol, dc, strTitle[nCol], 47);
				//cd 메모리 해제
				ReleaseDC(dc);

				// 22.06.30 Ahn Add End
				m_pResultList->SetItem(&Item);
				m_pResultList->SetColAlignment(nCol, flexAlignCenterCenter);
			}
			//크기에 맞게 조정
			m_pResultList->setGridFullColumnWidthVec();

			m_pResultList->SetRowHeight(0, nHeight);
		//	m_pResultList->AutoSize();
			m_pResultList->ShowWindow(SW_SHOW);
		}
	}

	return 0;
}

#define MAX_GRIDCOUNT 500
int CResultViewDlg::UpdateGrid()
{
	// TODO: 여기에 구현 코드 추가.
	int m_nRows = 0;

	if (!::IsWindow(m_pResultList->m_hWnd)) {
		return -1;
	}

	UpdateData();

	CDefectQueueCtrl* pQueue = theApp.m_pImgProcCtrl->GetDefectQueuePtr();
	int nSize = (int)pQueue->GetSize();
	int nOldNumRows = m_pResultList->GetRowCount();
	m_nRows = nOldNumRows + nSize; 

	TRY
	{
		m_pResultList->SetRowCount(m_nRows);
	}
	CATCH(CMemoryException, e)
	{
		e->ReportError();
		return -1;
	}
	END_CATCH

	UpdateData(FALSE);
	

	for( int nRow = nOldNumRows; nRow < m_nRows; nRow++ )
	{

		//CFrameCeInfo* pInfo = (CFrameCeInfo *)pQueue->Pop();
		CDefectInfo* pInfo = pQueue->Pop();
		if (pInfo == nullptr) continue;

		for (int nCol = 0; nCol < m_pResultList->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			if (nRow < m_pResultList->GetFixedRowCount())
				strText.Format(_T("Column %d"), nCol);
			else if (nCol < m_pResultList->GetFixedColumnCount())
				strText.Format(_T("%d"), m_pResultList->getTotalcount());
			else {
				switch (nCol) {
				case	en_col_No:
					break;
				case	en_col_InspTime:
					// 22.12.07 Ahn Modify Start
					//strText.Format(_T("%02d:%02d:%02d")						
					//	, pInfo->sysTime.wHour, pInfo->sysTime.wMinute, pInfo->sysTime.wSecond ); // 22.07.06 Ahn 시:분:초 외 삭제
					strText.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
						,pInfo->sysTime.wYear , pInfo->sysTime.wMonth, pInfo->sysTime.wDay 
						, pInfo->sysTime.wHour, pInfo->sysTime.wMinute, pInfo->sysTime.wSecond); // 22.07.06 Ahn 시:분:초 외 삭제
					// 22.12.07 Ahn Modify End
					break;
				case	en_col_LOT_ID:
					strText.Format( _T("%s"), pInfo->szLotID ) ;

					break;
				case	en_col_CELL_NO:
					strText.Format( _T("%d"), pInfo->nTabNo + 1 ) ;
					LOGDISPLAY_SPEC(8)("UpdateGrid - defect TabNo<%d> view Enter", pInfo->nTabNo);
					break;
				case	en_col_CELL_ID:
					strText.Format(_T("%d"), pInfo->nCellId);
					break;
				case	en_col_POSITION:
					strText = ( ( pInfo->nHeadNo == 0 ) ? _T("TAB") : _T("BOTTOM") ) ;
					break;
				case	en_col_DEF_TYPE:
					strText.Format(_T("%s"), ( pInfo->nType == TYPE_SURFACE ) ? _T("SURFACE") : _T("FOIL_EXP") );
					break;
				// 22.11.21 Ahn Add Start - JUDGE_GRAY
				case	en_col_JUDGE: 
					strText.Format(_T("%s"), (pInfo->nRank == JUDGE_NG) ? _T("NG") : _T("OK"));
					break;
				// 22.11.21 Ahn Add End
				case	en_col_DEF_SIZE:
					strText.Format(_T("%.2lf"), pInfo->dJudgeSize );
					break;
				case	en_col_InkMarking:
					strText.Format(_T("%s"), ((pInfo->bMarking == TRUE) ? _T("○") : _T("Χ") ) ) ;
					break;
				// 22.12.07 Ahn Add Start
				case	en_col_DefPosY: 
					strText.Format(_T("%.2lf"), pInfo->dDefectPosY / 1000.0 );
					break;
				// 22.12.07 Ahn Add End
				case	en_col_ImagePath:
					strText.Format(_T("%s"), pInfo->szImgFileName);
					break;
				}
			}
			Item.strText = strText;
			m_pResultList->SetItem(&Item);
			m_pResultList->SetColAlignment(nCol, flexAlignCenterCenter);
		}

	
		delete pInfo ;
		pInfo = NULL;
		nOldNumRows = nRow;
		

	}

	for (int count = m_pResultList->GetRowCount(); count >= MAX_GRIDCOUNT; count--)
	{
		m_pResultList->DeleteRow(1);
	}

	//m_pResultList->AutoSize();
	if (m_bAutoRefresh == TRUE) {
		m_pResultList->EnsureVisible(m_nRows - 1, 0);
	}


	return 0;
}

void CResultViewDlg::OnBnClickedChkRefresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bAutoRefresh = !m_bAutoRefresh;
	UpdateData(FALSE);
}


void CResultViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == T_GRID_REFRESH) {
		KillTimer(T_GRID_REFRESH);

		UpdateGrid();
		if (m_pDoc->GetInspState() == enInspRun) {
			SetTimer(T_GRID_REFRESH, 100, NULL);
		}
		else {
			SetTimer(T_GRID_REFRESH, 1000, NULL);
		}

		// 22.06.09 Ahn Add Start
		if (m_pDoc->IsReqCounterReset() == TRUE) {

			CounterReset();
			m_pDoc->SetReqCounterReset(FALSE);
		}
		// 22.06.09 Ahn Add End
	}

	CDialogEx::OnTimer(nIDEvent);
}

// 22.06.23 Ahn Add Start
void CResultViewDlg::OnMouseDblClickReltList(NMHDR* pNMHDR, LRESULT* pResult)
{
		*pResult = 0;
		NM_GRIDVIEW* pNmgv = (NM_GRIDVIEW*)pNMHDR;
		CGridCtrl* pGrid = m_pResultList ;
		int row = pNmgv->iRow;
		int col = pNmgv->iColumn;
		if (row < pGrid->GetFixedRowCount()) {
			return;
		}

		CString strPath = pGrid->GetItemText(row, en_col_ImagePath);

		BOOL bMoveWindow = FALSE;

		if (m_pImageViewerDlg == NULL) {
			m_pImageViewerDlg = new CImageViewerDlg(this);
			m_pImageViewerDlg->Create(IDD_DLG_IMAGE_VIEWER, this);
			bMoveWindow = TRUE;
		}

		if(m_pImageViewerDlg != nullptr ){

			if (bMoveWindow == TRUE) {
				CRect rcTmp, rcPos;
				int nWidth, nHeight;
				m_pResultList->GetWindowRect(rcTmp);
				if (rcTmp.Width() > rcTmp.Height()) {
					nWidth = (int)(rcTmp.Height() * 4 / 5);
					nHeight = nWidth;
				}
				else {
					nHeight = (int)(rcTmp.Width() * 4 / 5);
					nWidth = nHeight;
				}
				rcPos.top = (int)((rcTmp.top + rcTmp.bottom) / 2) - (int)(nHeight / 2);
				rcPos.left = (int)((rcTmp.left + rcTmp.right) / 2) - (int)(nWidth / 2);
				rcPos.bottom = rcPos.top + nHeight;
				rcPos.right = rcPos.left + nWidth;
				m_pImageViewerDlg->MoveWindow(rcPos);
			}

			if (m_pImageViewerDlg != NULL) {
				m_pImageViewerDlg->ShowWindow(SW_SHOW);
				m_pImageViewerDlg->SetImageFile(strPath);
				//KANG 22.07.12 Add Start
				m_nSelectRow = row;
				//KANG 22.07.12 Add End
			}

		}

}
// 22.06.23 Ahn Add End

// 22.06.09 Ahn Delete Start
////KANG 22.01.07 Add Start
//int CResultViewDlg::MakeGridHist(LPCTSTR lpszPath /*= _T("")*/)
//{
//	// TODO: 여기에 구현 코드 추가.
//	if (m_pHistList != nullptr) {
//		if (m_pHistList->m_hWnd != NULL)
//		{
//			delete m_pHistList;
//			m_pHistList = NULL;
//			delete m_pFontHist;
//			m_pFontHist = NULL;
//			m_pHistList = new CGridCtrl();
//		}
//		CRect rect;
//		GetClientRect(rect);
//		if (m_pHistList->Create(rect, this, IDC_HIST_LIST) == FALSE) {
//			delete m_pHistList;
//			m_pHistList = NULL;
//			return -1;
//		}
//		else {
//			if (m_pFontHist == NULL) {
//				m_pFontHist = new CFont();
//				m_pFontHist->CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET
//					, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
//					, FIXED_PITCH, "MS PGothic");
//				m_pHistList->SetFont(m_pFontHist, FALSE);
//
//			}
//
//			int m_nFixCols = 1;
//			int m_nFixRows = 1;
//			int m_nCols = 7;
//			int m_nRows = 1;
//			int nRow = 0;
//			int nCol = 0;
//
//			CString strFolder = lpszPath;
//			CString strRollID = _T("");
//			CStringList strList;
//			CWin32File file;
//
//
//			// 22.03.02 Ahn Modify Start
//			//CString strDate = strFolder + _T("일");
//			CString strDate = _T("") ;
//			CString strTemp = strFolder ;
//			if (strTemp.GetLength() > 0)
//			{
//				strTemp.Replace(AprData.m_strFeederPath + _T("\\"), _T(""));
//				strDate = strTemp.Left(4) + _T("년") + strTemp.Mid(5, 2) + _T("월") + strTemp.Right(2) + _T("일");
//				//	strDate.Replace(_T("\\"), _T("월"));
//				file.GetFolderList(strFolder, strList);
//				m_nRows += (int)strList.GetCount();
//			}
//			// 22.03.02 Ahn Modify Start
//
//
//			m_pHistList->SetAutoSizeStyle();
//			m_pHistList->SetFixedColumnSelection(TRUE);
//			m_pHistList->SetFixedRowSelection(TRUE);
//
//			TRY{
//				m_pHistList->SetRowCount(m_nRows);
//				m_pHistList->SetColumnCount(m_nCols);
//				m_pHistList->SetFixedRowCount(m_nFixRows);
//				m_pHistList->SetFixedColumnCount(m_nFixCols);
//			}
//				CATCH(CMemoryException, e) {
//				e->ReportError();
//				return -1;
//			}
//			END_CATCH
//
//			CString strTitle[7] = { _T("       ROLL_ID       "), _T("         일자         "), _T("     시작 시각     "), _T("     종료시각    "), _T("      검사레시피      "), _T("   NG수   "), _T("  OK수  ") };
//			for (nCol = 0; nCol < m_pHistList->GetColumnCount(); nCol++) {
//				CString strText;
//				GV_ITEM Item;
//				Item.mask = GVIF_TEXT;
//				Item.row = 0;
//				Item.col = nCol;
//				Item.strText = strTitle[nCol];
//				m_pHistList->SetItem(&Item);
//			}
//
//			POSITION pos = strList.GetHeadPosition();
//			for (int nRow = 1; nRow < m_nRows; nRow++)
//			{
//				for (int nCol = 0; nCol < m_pHistList->GetColumnCount(); nCol++)
//				{
//					CString strText;
//					GV_ITEM Item;
//					Item.mask = GVIF_TEXT;
//					Item.row = nRow;
//					Item.col = nCol;
//
//					// 0 : 일자 
//					// 1 : ROLL ID 
//					// 2 : NG 수 
//					// 3 : OK 수 
//					switch (nCol) {
//					case	0:
//					{
//						if (pos != NULL)
//						{
//							strText = strList.GetNext(pos);
//						}
//						break;
//					}
//					case	1:
//						strText = strDate;
//						break;
//					case	2:
//						break;
//					case	3:
//						break;
//					case	4:
//						break;
//					case	5:
//						strText.Format(_T("%d"), 0);
//						break;
//					case	6:
//						strText.Format(_T("%d"), 0);
//						break;
//					default:
//						break;
//					}
//
//					Item.strText = strText;
//					m_pHistList->SetItem(&Item);
//				}
//			}
//
//			m_pHistList->AutoSize();
//			m_pHistList->ShowWindow(SW_SHOW);
//		}
//	}
//
//	return 0;
//}
// 22.06.09 Ahn Delete End

// 22.06.23 AHn Modify Start
//void CResultViewDlg::OnMouseLDownHistList(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	*pResult = 0;
//	NM_GRIDVIEW* pNmgv = (NM_GRIDVIEW*)pNMHDR;
//	CGridCtrl* pGrid = m_pHistList;
//
//	int row = pNmgv->iRow;
//	int col = pNmgv->iColumn;
//
//	int nMaxRow = pGrid->GetRows();
//	int nMaxCol = pGrid->GetCols();
//
//	for (int nRow = pGrid->GetFixedRowCount(); nRow < nMaxRow; nRow++) {
//		for (int nCol = pGrid->GetFixedColumnCount(); nCol < nMaxCol; nCol++) {
//			if (nRow == row) {
//				pGrid->SetCellBackColor(nRow, nCol, ::GetSysColor(COLOR_HIGHLIGHT));
//				pGrid->SetCellTextColor(nRow, nCol, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
//			}else {
//				pGrid->SetCellBackColor(nRow, nCol, ::GetSysColor(COLOR_WINDOW));
//				pGrid->SetCellTextColor(nRow, nCol, ::GetSysColor(COLOR_WINDOWTEXT));
//			}
//		}
//	}
//	Invalidate(FALSE);
//}
//
//void CResultViewDlg::OnMouseDblClickHistList(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	*pResult = 0;
//	NM_GRIDVIEW* pNmgv = (NM_GRIDVIEW*)pNMHDR;
//	CGridCtrl* pGrid = m_pHistList;
//	int row = pNmgv->iRow;
//	int col = pNmgv->iColumn;
//	if (row < pGrid->GetFixedRowCount()) {
//		return;
//	}
//
//	CString strLotID = pGrid->GetItemText(row, 0);
//	CString strDate = pGrid->GetItemText(row, 1);
//	int nMonth = 0, nDay = 0;
//	int nYear = 0;
//	::sscanf_s(strDate, _T("%d년%d월%d일"), &nYear, &nMonth, &nDay ) ;
//	CString strPath;
//	strPath.Format(_T("%s\\%d\\%02d\\%02d\\%s"), AprData.m_strFeederPath, nYear, nMonth, nDay, strLotID);
//	m_pView->GetHistoryDlgPtr()->OnRefresh(1);
//
//	CString strMonth, strDay;
//	strMonth.Format(_T("%02d"), nMonth);
//	strDay.Format(_T("%02d"), nDay);
//	CString strYear;
//	strYear.Format(_T("%d"), nYear);
//	m_pView->GetHistoryDlgPtr()->ReadThumbnailFromErrorData(strPath, strYear, strMonth, strDay, strLotID);
//
//}
////KANG 22.01.07 Add End
// 22.06.23 AHn Modify End

//KANG 22.05.24 Add Start
BOOL CResultViewDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
//KANG 22.05.24 Add End


// 22.06.09 Ahn Add Starrt
int CResultViewDlg::CounterReset()
{
//	m_nRow 
	int m_nRows = 1 ; 
	m_pResultList->resetTotalcount();

	TRY{
		m_pResultList->SetRowCount(m_nRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return -1;
	}
	END_CATCH

	UpdateData(FALSE);
	return 0;
}
// 22.06.09 Ahn Add End


void CResultViewDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	CDialogEx::OnClose();
}


// 22.07.01 Ahn Add Start

void CResultViewDlg::OnRBottonDownReltList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	NM_GRIDVIEW* pNmgv = (NM_GRIDVIEW*)pNMHDR;
	CGridCtrl* pGrid = m_pResultList;
	int row = pNmgv->iRow;
	int col = pNmgv->iColumn;
	int nRowCount = pGrid->GetRowCount() ;
	if (row < pGrid->GetFixedRowCount()) {
		return;
	}

	CString strPath = pGrid->GetItemText(row, en_col_ImagePath);

	BOOL bMoveWindow = FALSE;

	pGrid->SetFocusCell(row, 0);

	SelectResultRow(row);

} 

void CResultViewDlg::SelectResultRow(int nRow) {

	CGridCtrl* pGrid = m_pResultList;
	CDefectSearchList* pList = NULL;

	int nRowCnt = pGrid->GetRowCount();
	if (nRow > nRowCnt) {
		return;
	}

	int nMaxCol = pGrid->GetCols() - 1;
	pGrid->SetFocusCell(nRow, 0);

	if ((nRow != m_nSelectRow) && (nRow > pGrid->GetFixedRowCount() - 1)) {
		pGrid->SetRedraw(FALSE);
		for (int nCol = pGrid->GetFixedColumnCount(); nCol < nMaxCol; nCol++) {
			pGrid->SetCellBackColor(nRow, nCol, ::GetSysColor(COLOR_HIGHLIGHT));
			pGrid->SetCellTextColor(nRow, nCol, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
			if ((m_nSelectRow > 0) && (nRow != m_nSelectRow) && (m_nSelectRow < pGrid->GetRows())) {
				pGrid->SetCellBackColor(m_nSelectRow, nCol, ::GetSysColor(COLOR_WINDOW));
				pGrid->SetCellTextColor(m_nSelectRow, nCol, ::GetSysColor(COLOR_WINDOWTEXT));
			}
		}
		pGrid->SetRedraw(TRUE);
		m_nSelectRow = nRow;

		//KANG 22.07.12 Add Start
		if (m_pImageViewerDlg != NULL) {
			m_pImageViewerDlg->ShowWindow(SW_HIDE);
		}
		//KANG 22.07.12 Add End
	}

	//KANG 22.07.12 Delete Start
	//	if (m_pImageViewerDlg != NULL) {
	//		m_pImageViewerDlg->ShowWindow(SW_HIDE);
	//	}
	//KANG 22.07.12 Delete End

}
// 22.07.01 Ahn Add End
// 22.07.07 Ahn Add Start
void CResultViewDlg::LoadLastLotData()
{
	CString strLotID;
	CString strReadFileName;
	CString strIndexFileName;
	CStringList strSearchFileList;
	CWin32File file;
	strSearchFileList.RemoveAll();

	strLotID = AprData.m_NowLotData.m_strLotNo;
	strIndexFileName = AprData.GetIndexFileName(strLotID);

	if (strLotID.GetLength() <= 0) return;
	if (file.ExistFile(strIndexFileName) == FALSE) {
	//	MessageBox(_T("검색 결과가 없습니다."));
		return;
	}
	else {
		CStdioFile cf;
		CString strMsg;
		if (!cf.Open(strIndexFileName, (CFile::typeText | CFile::modeRead))) {
			strMsg.Format(_T("파일 오픈 에러[%s]"), strIndexFileName);
			MessageBox(strMsg);
			return;
		}
		while (1) {
			try {
				if (cf.ReadString(strReadFileName) == NULL) {
					break;
				}
				strSearchFileList.AddTail(strReadFileName);

			}
			catch (CFileException& e) {
				CHAR	szChar[256];
				e.GetErrorMessage(szChar, 255);
				strMsg.Format(_T("에러 - %s"), szChar);
				MessageBox(strMsg);
				cf.Close();
				return;
			}
		}
		cf.Close();
	}

	CString strLastFileName = strSearchFileList.GetTail() ;

	CDefectDataCtrl::_VEC_DEFECT_LIST defList;
	defList.clear();
	CDefectQueueCtrl* pQueue = theApp.m_pImgProcCtrl->GetDefectQueuePtr();
	CCropImgQueueCtrl* pCropQueue = theApp.m_pImgProcCtrl->GetCropImageQueuePtr();

	if (CWin32File::ExistFile(strLastFileName) == TRUE) {
		if (CDefectDataCtrl::LoadDefectInfo(strLastFileName, defList) > 0) {
			int nSize = (int)defList.size();

			CDefectDataCtrl::_VEC_DEFECT_LIST::iterator iter ;

			int nStartPos = nSize - MAX_CROP_IMAGE_QUEUE_SIZE ;
			if (nStartPos > 0) {
				iter = defList.begin() + nStartPos;
			}
			else {
				iter = defList.begin();
			}
			for (; iter != defList.end(); iter++) {
				CCropImgData CropData;
				CString strPos;
				// 22.11.24 Ahn Modify Start
				//strPos.Format(_T("%s"), ((*iter)->nHeadNo == 0) ? _T("TOP") : _T("BTM"));
				//CropData.m_bEnable = TRUE;
				//CropData.m_strFileName.Format(_T("%s"),(*iter)->szImgFileName);
				//CropData.m_strDispName.Format(_T("%s_%d_%d"), strPos, (*iter)->nTabNo + 1, (*iter)->nDefNo + 1);
				strPos.Format(_T("%s"), (iter->nHeadNo == 0) ? _T("TAB") : _T("BTM"));
				CropData.m_bEnable = TRUE;
				CropData.m_strFileName.Format(_T("%s"), iter->szImgFileName);
				CropData.m_strDispName.Format(_T("%s_%d_%d"), strPos, iter->nTabNo + 1, iter->nDefNo + 1);
				// 22.11.24 Ahn Modify End
				pCropQueue->PushBack(CropData);
			}

			for (iter = defList.begin(); iter != defList.end(); ) {
				// 22.11.24 Ahn Modify Start
				//pQueue->PushBack( *iter );
				CDefectInfo* pInfo = new CDefectInfo;
				*pInfo = *iter ;
				pQueue->PushBack(pInfo);
				// 22.11.24 Ahn Modify End
				iter = defList.erase(defList.begin());
			}

			UpdateGrid();
		}
	}	
}

// 22.07.07 Ahn Add End
