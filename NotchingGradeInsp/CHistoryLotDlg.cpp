// CHistoryLotDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CHistoryLotDlg.h"
#include "afxdialogex.h"

#include "NotchingGradeInspDoc.h"
#include "NotchingGradeInspView.h"
#include "Win32File.h"
#include "GlobalData.h"

// CHistoryLotDlg 대화 상자

#define IDC_LOT_HISTORY_LIST	1002


IMPLEMENT_DYNAMIC(CHistoryLotDlg, CDialogEx)

CHistoryLotDlg::CHistoryLotDlg(CWnd* pParent /*=nullptr*/, CNotchingGradeInspView *pView )
	: CDialogEx(IDD_DLG_HISTORY_LOT, pParent)
{
	m_pParent = pParent;
	m_pView = pView;
	m_pDoc = NULL;

	m_pHistoryList = NULL ;
	m_pFontGrid = NULL; // 22.07.22 Ahn Add 
}

CHistoryLotDlg::~CHistoryLotDlg()
{
	if (m_pHistoryList != NULL) {
		delete m_pHistoryList ;
		m_pHistoryList = NULL;
	}

	if (m_pFontGrid != nullptr) {
		m_pFontGrid->DeleteObject();
		delete m_pFontGrid;
		m_pFontGrid = NULL;
	}
}

void CHistoryLotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHistoryLotDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LOT_HISTORY_LIST, &CHistoryLotDlg::OnMouseLDownHistoryList)	// 22.07.01 Ahn Add 
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BCDID_DIFF, &CHistoryLotDlg::OnDeltaposSpinBcdidDiff)
END_MESSAGE_MAP()


// CHistoryLotDlg 메시지 처리기
BOOL CHistoryLotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (m_pHistoryList == NULL) {
		m_pHistoryList = new CGridCtrl();
	}
	
	MakeHitoryGrid();

	GetDlgItem(IDC_EDIT_BCDID_DIFF)->SetWindowTextA(std::to_string(AprData.m_NowLotData.m_BCDIDVal).c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CHistoryLotDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect	btnRect;

	if (m_pHistoryList->GetSafeHwnd()) {
		//KANG 22.01.07 Add Start
		{
			//KANG 22.01.07 Add End
			m_pHistoryList->SetWindowPos(NULL, 5, 5, cx - 10, cy - 40, SWP_NOZORDER);
			//KANG 22.01.07 Add Start
			m_pHistoryList->ShowWindow(SW_SHOW);
		}
		//KANG 22.01.07 Add End
	}
}

int CHistoryLotDlg::MakeHitoryGrid()
{
	// m_pHistoryList->

	if (m_pHistoryList != nullptr) {
		CRect rect;
		GetClientRect(rect);
		if (m_pHistoryList->Create(rect, this, IDC_LOT_HISTORY_LIST) == FALSE) {
			delete m_pHistoryList;
			m_pHistoryList = NULL;
			return -1;
		}
		else {
			if (m_pFontGrid == NULL) {
				//CFont* pFont = NULL;
				//LOGFONT lf;
				//pFont = m_pHistoryList->GetFont();
				//pFont->GetLogFont(&lf);
				//CString strFontFace = _T("굴림");
				//lf.lfHeight = (long)(m_pHistoryList->GetRowHeight(0) * 1);
				//lf.lfWeight = FW_NORMAL;
				//::_tcsnccpy_s(lf.lfFaceName, _countof(lf.lfFaceName), strFontFace.GetBuffer(), _TRUNCATE);
				//m_pFontGrid = new CFont();
				//m_pFontGrid->CreateFontIndirect(&lf);
				//m_pHistoryList->SetFont(m_pFontGrid, TRUE);
				m_pFontGrid = new CFont();
				m_pFontGrid->CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
					, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
					, FIXED_PITCH, "Arial");
				m_pHistoryList->SetFont(m_pFontGrid, FALSE);
			}

			int m_nFixCols = 1;
			int m_nFixRows = 1;
			int m_nCols = en_Max_GridCols;
			int m_nRows = 1;
			int nRow = 0;
			int nCol = 0;
			int nHeight = 25;

			m_pHistoryList->SetAutoSizeStyle();
			m_pHistoryList->SetFixedColumnSelection(TRUE);
			m_pHistoryList->SetFixedRowSelection(TRUE);

			TRY{
				m_pHistoryList->SetRowCount(m_nRows);
				m_pHistoryList->SetColumnCount(m_nCols);
				m_pHistoryList->SetFixedRowCount(m_nFixRows);
				m_pHistoryList->SetFixedColumnCount(m_nFixCols);
			}
				CATCH(CMemoryException, e) {
				e->ReportError();
				return -1;
			}
			END_CATCH

			// 
			CString strTitle[en_Max_GridCols] = { _T("No."),_T("      Date     "),_T("     LotID     "),_T("Lot Start"),_T("LotEnd"),_T("Total"),_T("OK Count")
													,_T("OK(%)"), _T("NG Count"), _T("NG(%)"), _T("Tab FoilExp"), _T("BTM FoilExp"), _T("Ink Marking"), _T("Marking(%)")};

			for (nCol = 0; nCol < m_pHistoryList->GetColumnCount(); nCol++) {
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = 0;
				Item.col = nCol;
				Item.strText = strTitle[nCol];

				//폰트 픽셀 넓이 저정
				CDC* dc = GetDC();
				m_pHistoryList->setGrideFontWidth(nCol, dc, strTitle[nCol], 20);
				//cd 메모리 해제
				ReleaseDC(dc);

				m_pHistoryList->SetItem(&Item);
				m_pHistoryList->SetColAlignment(nCol, flexAlignCenterCenter);
			}

			//크기 조정
			m_pHistoryList->setGridFullColumnWidthVec();

			m_pHistoryList->SetRowHeight(0, nHeight);
			//m_pHistoryList->AutoSize();
			m_pHistoryList->ShowWindow(SW_SHOW);
		}

		UpdateHistoryGrid();
	}

	return 0;
}

void CHistoryLotDlg::RefreshAll()
{
	UpdateHistoryGrid();
}

//마킹 BCD ID를 갱신한다.
void CHistoryLotDlg::UpdateBCDID()
{
	AprData.m_NowLotData.m_BCDIDVal = 0;
	GetDlgItem(IDC_EDIT_BCDID_DIFF)->SetWindowTextA(std::to_string(AprData.m_NowLotData.m_BCDIDVal).c_str());
}


int CHistoryLotDlg::UpdateHistoryGrid()
{

	int m_nRows = 0;

	if (!::IsWindow(m_pHistoryList->m_hWnd)) {
		return -1;
	}

	UpdateData();

	// 22.06.27 Ahn Modify Start
	//int nOldNumRows = m_pHistoryList->GetRowCount();
	//m_nRows = nOldNumRows + nSize;
	int nSize = (int)AprData.m_pHistoryLotCtrl->size();
	m_nRows = nSize + 1 ;
	//AprData.m_pHistoryLotCtrl->
	// 22.06.27 Ahn Modify End

	TRY{
		m_pHistoryList->SetRowCount(m_nRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return -1;
	}
	END_CATCH

	UpdateData(FALSE);

	int nHeight = 20;

	for (int nRow = 1; nRow < m_nRows; nRow++) {
		CHistoryLotCtrl::stLotInfo LotInfo;
		int nLocalRet = AprData.m_pHistoryLotCtrl->GetAtLotInfo(nRow - 1, &LotInfo);
		if (nLocalRet < 0) return -2;
		CString strDate;
		strDate.Format(_T("%04d-%02d-%02d"), LotInfo.StartTime.wYear, LotInfo.StartTime.wMonth, LotInfo.StartTime.wDay);

		for (int nCol = 0; nCol < m_pHistoryList->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			if (nRow < m_pHistoryList->GetFixedRowCount())
				strText.Format(_T("Column %d"), nCol);
			else if (nCol < m_pHistoryList->GetFixedColumnCount())
				strText.Format(_T("%d"), nRow);
			else {
				// 22.07.05 Ahn Modify Start
				switch (nCol) {
				case	en_col_no:
					strText.Format(_T("%d"), nCol);
					break;
				case	en_col_Date:
					strText.Format(_T("%s"), strDate);
					break;
				case	en_col_LotId:
					strText.Format(_T("%s"), LotInfo.chLotNo );
					break;
				case	en_col_StartTime:
					strText.Format(_T("%02d:%02d:%02d"), LotInfo.StartTime.wHour, LotInfo.StartTime.wMinute, LotInfo.StartTime.wSecond);
					break;
				case	en_col_EndTime:
					strText.Format(_T("%02d:%02d:%02d"), LotInfo.EndTime.wHour, LotInfo.EndTime.wMinute, LotInfo.EndTime.wSecond);
					break;
				case	en_col_TotalCount:
					strText.Format(_T("%d"), LotInfo.nTabCount);
					break;
				case	en_col_OK_Count:
					strText.Format(_T("%d"), LotInfo.nTabCountOK);
					break;
				case	en_col_OK_Rate:
					if (LotInfo.nTabCount > 0) {
						// 23.02.08 Ahn Modify Start
						//strText.Format(_T("%.1lf"), ((double)((double)LotInfo.nTabCountOK / (double)LotInfo.nTabCount) * 100.0));
						strText.Format(_T("%.2lf"), ((double)((double)LotInfo.nTabCountOK / (double)LotInfo.nTabCount) * 100.0));
						// 23.02.08 Ahn Modify End
					}
					else {
						strText = _T("0.0");
					}
					break;
				case	en_col_NG_Count :
					strText.Format(_T("%d"), LotInfo.nTabCountNG);
					break;
				case	en_col_NG_Rate :
					// 23.02.08 Ahn Modify End
					//strText.Format(_T("%.1lf"), ((double)((double)LotInfo.nTabCountNG / (double)LotInfo.nTabCount) * 100.0));
					strText.Format(_T("%.2lf"), ((double)((double)LotInfo.nTabCountNG / (double)LotInfo.nTabCount) * 100.0));
					// 23.02.08 Ahn Modify End
					break;
				case	en_col_Top_NG_Count:
					strText.Format(_T("%d"), LotInfo.nTabCountNG);
					break;
				case	en_col_Btm_NG_Count:
					strText.Format(_T("%d"), LotInfo.nBottomNgCount );
					break;
				case	en_col_Marking_Count:
					strText.Format(_T("%d"), LotInfo.nMarkingCount);
					break;
				case	en_col_Marking_Rage:
					if (LotInfo.nTabCount > 0) {
						strText.Format(_T("%lf"), (double)((double)LotInfo.nMarkingCount / (double)LotInfo.nTabCount) * 100.0);
					}
					else {
						strText = _T("0.0");
					}
					break;
				}
				// 22.07.05 Ahn Modify End
			}
			Item.strText = strText;
			m_pHistoryList->SetItem(&Item);
			m_pHistoryList->SetColAlignment(nCol, flexAlignCenterCenter);
		}
		m_pHistoryList->SetRowHeight(nRow, nHeight);
		//delete pInfo;
		//pInfo = NULL;
		//nOldNumRows = nRow;
	}

	//{
	//	m_pHistoryList->EnsureVisible(m_nRows - 1, 0);
	//}

	return 0;
}


void CHistoryLotDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	CDialogEx::OnClose();
}


void CHistoryLotDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialogEx::OnOK();
}

// 22.07.01 Ahn Add Start
void CHistoryLotDlg::OnMouseLDownHistoryList(NMHDR* pNMHDR, LRESULT* pResult)
{
	//*pResult = 0;
	//NM_GRIDVIEW* pNmgv = (NM_GRIDVIEW*)pNMHDR;
	//CGridCtrl* pGrid = m_pHistoryList ;
	//CDefectSearchList* pList = NULL;

	//int row = pNmgv->iRow;
	//int col = pNmgv->iColumn;
	//int nMaxCol = pGrid->GetCols() - 1;

	//COleDateTime OleDateTime;
	//OleDateTime = m_CalCtrl.GetCurrentDate();
	//SetSelectCell(OleDateTime);
}
// 22.07.01 Ahn Add End

void CHistoryLotDlg::OnDeltaposSpinBcdidDiff(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMUpDown->iDelta < 0)
	{
		AprData.m_NowLotData.m_BCDIDVal++;
		AprData.m_NowLotData.m_nUseBCDID++;
		if (AprData.m_NowLotData.m_nUseBCDID >= 64)
			AprData.m_NowLotData.m_nUseBCDID = 0;
	}
	else
	{
		AprData.m_NowLotData.m_BCDIDVal--;
		AprData.m_NowLotData.m_nUseBCDID--;
		if (AprData.m_NowLotData.m_nUseBCDID < 0)
			AprData.m_NowLotData.m_nUseBCDID = 63;
	}
	GetDlgItem(IDC_EDIT_BCDID_DIFF)->SetWindowTextA(std::to_string(AprData.m_NowLotData.m_BCDIDVal).c_str());
	*pResult = 0;
}
