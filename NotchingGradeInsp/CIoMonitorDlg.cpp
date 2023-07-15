// CIoMonitorDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CIoMonitorDlg.h"
#include "afxdialogex.h"
#include "SigProc.h"
#include "ModeDlg.h"

// CIoMonitorDlg 대화 상자
#define T_ID_CHECK_IO	1005

IMPLEMENT_DYNAMIC(CIoMonitorDlg, CDialogEx)

CIoMonitorDlg::CIoMonitorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_IO_MONITOR, pParent)
	, m_strLotID(_T(""))
	, m_strRecipeName(_T(""))
	, m_strAlarm(_T(""))
	, m_strAlarmBit(_T(""))
{
	m_TID_CheckIO = 0;
	
	memset( m_bSigBitIn, 0x0000, sizeof( BOOL) *MAX_ADR_BIT_IN ) ;
	memset( m_bSigBitOut, 0x0000, sizeof(BOOL) * MAX_ADR_BIT_IN) ;

	m_pParent = (CModeDlg*)pParent;
}

CIoMonitorDlg::~CIoMonitorDlg()
{
}

void CIoMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_ALIVE_PULSE_IN, m_bmpSigIn[en_In_Alive][en_mode_on]);
	DDX_Control(pDX, IDC_ST_READY_IN, m_bmpSigIn[en_In_Ready][en_mode_on]);
	DDX_Control(pDX, IDC_ST_RUN_IN, m_bmpSigIn[en_In_Run][en_mode_on]);
	DDX_Control(pDX, IDC_ST_CNT_RESET_IN, m_bmpSigIn[en_In_CntReset][en_mode_on]);
	DDX_Control(pDX, IDC_ST_INK_MARK_IN, m_bmpSigIn[en_In_InkMarkActive][en_mode_on]);
	DDX_Control(pDX, IDC_ST_RECIPE_CHANGE_IN, m_bmpSigIn[en_In_RcpChange][en_mode_on]);
	DDX_Control(pDX, IDC_ST_LOT_START_IN, m_bmpSigIn[en_In_LotStart][en_mode_on]);
	DDX_Control(pDX, IDC_ST_LOT_END_IN, m_bmpSigIn[en_In_LotEnd][en_mode_on]);

	DDX_Control(pDX, IDC_ST_ALIVE_PULSE_OUT, m_bmpSigOut[en_Out_Alive][en_mode_on]);
	DDX_Control(pDX, IDC_ST_READY_OUT, m_bmpSigOut[en_Out_Ready][en_mode_on]);
	DDX_Control(pDX, IDC_ST_CNT_RESET_ACK, m_bmpSigOut[en_Out_CntResetAck][en_mode_on]);
	DDX_Control(pDX, IDC_ST_DISK_SPACE_ALARM, m_bmpSigOut[en_Out_RcpChangeAck][en_mode_on]);
	DDX_Control(pDX, IDC_ST_DISK_SPACE_ERROR, m_bmpSigOut[en_Out_DiskSpaceAlarm][en_mode_on]);
	DDX_Control(pDX, IDC_ST_RAD_RECIPE_CHANGE_ACK, m_bmpSigOut[en_Out_DiskSpaceWarning][en_mode_on]);
	DDX_Control(pDX, IDC_ST_LOT_START_ACK, m_bmpSigOut[en_Out_LotStartAck][en_mode_on]);
	DDX_Control(pDX, IDC_ST_LOT_END_ACK, m_bmpSigOut[en_Out_LotEndAck][en_mode_on]);

	DDX_Control(pDX, IDC_ST_ALIVE_PULSE_IN_OFF, m_bmpSigIn[en_In_Alive][en_mode_off]);
	DDX_Control(pDX, IDC_ST_READY_IN_OFF, m_bmpSigIn[en_In_Ready][en_mode_off]);
	DDX_Control(pDX, IDC_ST_RUN_IN_OFF, m_bmpSigIn[en_In_Run][en_mode_off]);
	DDX_Control(pDX, IDC_ST_CNT_RESET_IN_OFF, m_bmpSigIn[en_In_CntReset][en_mode_off]);
	DDX_Control(pDX, IDC_ST_INK_MARK_IN_OFF, m_bmpSigIn[en_In_InkMarkActive][en_mode_off]);
	DDX_Control(pDX, IDC_ST_RECIPE_CHANGE_IN_OFF, m_bmpSigIn[en_In_RcpChange][en_mode_off]);
	DDX_Control(pDX, IDC_ST_LOT_START_IN_OFF, m_bmpSigIn[en_In_LotStart][en_mode_off]);
	DDX_Control(pDX, IDC_ST_LOT_END_IN_OFF, m_bmpSigIn[en_In_LotEnd][en_mode_off]);

	DDX_Control(pDX, IDC_ST_ALIVE_PULSE_OUT_OFF, m_bmpSigOut[en_Out_Alive][en_mode_off]);
	DDX_Control(pDX, IDC_ST_READY_OUT_OFF, m_bmpSigOut[en_Out_Ready][en_mode_off]);
	DDX_Control(pDX, IDC_ST_CNT_RESET_ACK_OFF, m_bmpSigOut[en_Out_CntResetAck][en_mode_off]);
	DDX_Control(pDX, IDC_ST_DISK_SPACE_ALARM_OFF, m_bmpSigOut[en_Out_RcpChangeAck][en_mode_off]);
	DDX_Control(pDX, IDC_ST_DISK_SPACE_ERROR_OFF, m_bmpSigOut[en_Out_DiskSpaceAlarm][en_mode_off]);
	DDX_Control(pDX, IDC_ST_RAD_RECIPE_CHANGE_ACK_OFF, m_bmpSigOut[en_Out_DiskSpaceWarning][en_mode_off]);
	DDX_Control(pDX, IDC_ST_LOT_START_ACK_OFF, m_bmpSigOut[en_Out_LotStartAck][en_mode_off]);
	DDX_Control(pDX, IDC_ST_LOT_END_ACK_OFF, m_bmpSigOut[en_Out_LotEndAck][en_mode_off]);
	//DDX_Text(pDX,	IDC_ED_LOT_ID, m_strLotID);
	//DDX_Text(pDX,	IDC_ED_ALARM_OUT, m_strAlarm);
	//DDX_Text(pDX,	IDC_ED_ALARM_BIT, m_strAlarmBit);
	// 22.08.12 Ahn Add Start
	DDX_Control(pDX, IDC_GRID_WORD_IN, m_GridWordIn);
	DDX_Control(pDX, IDC_GRID_WORD_OUT, m_GridWordOut);

	DDX_Control(pDX, IDC_GRID_BIT_IN, m_GridBitIn);
	DDX_Control(pDX, IDC_GRID_BIT_OUT, m_GridBitOut);

	// 22.08.12 Ahn Add End
}


BEGIN_MESSAGE_MAP(CIoMonitorDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CIoMonitorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_DATA_LOAD, &CIoMonitorDlg::OnBnClickedBtnDataLoad)
	ON_BN_CLICKED(IDC_BTN_DUMMY_ERROR, &CIoMonitorDlg::OnBnClickedBtnDummyError)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_DUMMY_ERROR_CLEAR, &CIoMonitorDlg::OnBnClickedBtnDummyErrorClear)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_BIT_OUT, &CIoMonitorDlg::OnMouseDblClickGridBitOut)	// 22.07.15 Ahn Add 

END_MESSAGE_MAP()


// CIoMonitorDlg 메시지 처리기


void CIoMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_TID_CheckIO == nIDEvent)
	{
		KillCheckIoTimer();

		// 신호 Display 갱신
		RefreshAll();

		SetCheckIoTimer();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CIoMonitorDlg::SetCheckIoTimer()
{
	m_TID_CheckIO = SetTimer(T_ID_CHECK_IO, 200, NULL);
}
BOOL CIoMonitorDlg::KillCheckIoTimer()
{
	return KillTimer(m_TID_CheckIO);
}

void CIoMonitorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}

BOOL CIoMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();	

	MakeGridCtrl();
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 23.03.03 Ahn Add Start
int CIoMonitorDlg::GetAddressAndBitPos( int nMode, int nSigNo, int &nBitPos  )
{
	int nAddress = 0;

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec)
	{
		if (nMode == MODE_READ) { // Signal In
			switch (nSigNo) {
			case	en_In_Alive:
				nAddress = CSigProc::enBitIn_Alive;
				nBitPos = 0;
				break;
			case	en_In_Ready:
				nAddress = CSigProc::enBitIn_Ready;
				nBitPos = 1;
				break;
			case	en_In_Run:
				nAddress = CSigProc::enBitIn_Run;
				nBitPos = 2;
				break;
			case	en_In_CntReset:
				nAddress = CSigProc::enBitIn_TabZeroReset;
				nBitPos = 6;
				break;
			case	en_In_InkMarkActive:
				nAddress = CSigProc::enBitIn_InkMarkingActive;
				nBitPos = 9;
				break;
			case	en_In_RcpChange:
				nAddress = CSigProc::enBitIn_RecipeChange;
				nBitPos = 18;
				break;
			case	en_In_LotStart:
				nAddress = CSigProc::enBitIn_LotStartReq;
				nBitPos = 19;
				break;
			case	en_In_LotEnd:
				nAddress = CSigProc::enBitIn_LotEndReq;
				nBitPos = 20;
				break;
			default:
				break;
			}
		}
		else { // Sig Out
			switch (nSigNo) {
			case	en_Out_Alive:
				nAddress = CSigProc::enBitOut_Alive;
				nBitPos = 0;
				break;
			case	en_Out_Ready:
				nAddress = CSigProc::enBitOut_Ready;
				nBitPos = 1;
				break;
			case	en_Out_CntResetAck:
				nAddress = CSigProc::enBitOut_TabZeroReset;
				nBitPos = 5;
				break;
			case	en_Out_DiskSpaceWarning:
				nAddress = CSigProc::enBitOut_DiskSpaceWarning;
				nBitPos = 8;
				break;
			case	en_Out_DiskSpaceAlarm:
				nAddress = CSigProc::enBitOut_DiskSpaceAlarm;
				nBitPos = 9;
				break;
			case	en_Out_RcpChangeAck:
				nAddress = CSigProc::enBitOut_RecipeChangeAck;
				nBitPos = 18;
				break;
			case	en_Out_LotStartAck:
				nAddress = CSigProc::enBitOut_LotStartReqAck;
				nBitPos = 19;
				break;
			case	en_Out_LotEndAck:
				nAddress = CSigProc::enBitOut_LotEndReqAck;
				nBitPos = 20;
				break;
			}
			nAddress += AprData.m_System.m_nBitOut;
		}
	}
	else {	// siemens type 
		if (nMode == MODE_READ) { // Signal In
			switch (nSigNo) {
			case	en_In_Alive:
				nBitPos = nAddress = CSigProc::enSmsBitIn_Alive ;
				break;
			case	en_In_Ready:
				nBitPos = nAddress = CSigProc::enSmsBitIn_Ready ;
				break;
			case	en_In_Run:
				nBitPos = nAddress = CSigProc::enSmsBitIn_Run ;
				break;
			case	en_In_CntReset:
				nBitPos = nAddress = CSigProc::enSmsBitIn_TabZeroReset ;
				break;
			case	en_In_InkMarkActive:
				nBitPos = nAddress = CSigProc::enSmsBitIn_InkMarkingActive ;
				break;
			case	en_In_RcpChange:
				nBitPos = nAddress = CSigProc::enSmsBitIn_RecipeChange ;
				break;
			case	en_In_LotStart:
				nBitPos = nAddress = CSigProc::enSmsBitIn_LotStartReq ;
				break;
			case	en_In_LotEnd:
				nBitPos = nAddress = CSigProc::enSmsBitIn_LotEndReq ;
				break;
			default:

				break;
			}
		} else { // Sig Out
			switch (nSigNo) {
			case	en_Out_Alive:
				nAddress = CSigProc::enSmsBitOut_Alive ;
				break;
			case	en_Out_Ready:
				nAddress = CSigProc::enSmsBitOut_Ready ;
				break;
			case	en_Out_CntResetAck:
				nAddress = CSigProc::enSmsBitOut_TabZeroReset ;
				break;
			case	en_Out_RcpChangeAck:
				nAddress = CSigProc::enSmsBitOut_RecipeChangeAck;
				break;
			case	en_Out_DiskSpaceAlarm:
				nAddress = CSigProc::enSmsBitOut_DiskSpaceAlarm ;
				break;
			case	en_Out_DiskSpaceWarning:
				nAddress = CSigProc::enSmsBitOut_DiskSpaceWarning ;
				break;
			case	en_Out_LotStartAck:
				nAddress = CSigProc::enSmsBitOut_LotStartReqAck ; 
				break;
			case	en_Out_LotEndAck:
				nAddress = CSigProc::enSmsBitOut_LotEndReqAck ;
				break;
			}
			nBitPos = nAddress;
			nAddress += AprData.m_System.m_nBitOut ;
		}
	}

	return nAddress ;
}
// 23.03.03 Ahn Add End

int CIoMonitorDlg::RefreshAll()
{
	int nRet = 0;

	CSigProc* pSigProc = theApp.m_pSigProc;
	memset(m_bSigBitIn, 0x0000, sizeof(BOOL) * MAX_ADR_BIT_IN);
	memset(m_bSigBitOut, 0x0000, sizeof(BOOL) * MAX_ADR_BIT_OUT);
	pSigProc->ReadAllPort_BitIn(m_bSigBitIn);
	pSigProc->ReadAllPort_BitOut(m_bSigBitOut);
	pSigProc->ReadBlockAllData(&m_localSeqDataIn);

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec)
	{
		pSigProc->ReadBlockWriteDataAll_Melsec(&m_localSeqDataOut);
	}
	else
	{
		pSigProc->ReadBlockWriteDataAll_Siemens(&m_localSeqDataOutSms);
	}

 	UpdateGridCtrl();



	return nRet;
}

#if defined(_DEBUG)
void CIoMonitorDlg::DebugLedChange()
{
	if ((m_bSigBitIn[0] & 0x01) == 0x00 ) {
		m_bSigBitIn[0] |= 0x01;
	}
	else {
		m_bSigBitIn[0] &= 0xFE;
	}
	if ((m_bSigBitOut[0] & 0x01) == 0x00) {
		m_bSigBitOut[0] |= 0x01;
	}
	else {
		m_bSigBitOut[0] &= 0xFE;
	}
}
#endif


void CIoMonitorDlg::OnBnClickedOk()
{
	KillCheckIoTimer();


	CDialogEx::OnOK();
}


void CIoMonitorDlg::OnBnClickedBtnDataLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSigProc* pSigProc = theApp.m_pSigProc;;
	pSigProc->ReadBlockAllData(&m_SeqData);

	m_strLotID = m_SeqData.strCell_ID ;
	m_strRecipeName = m_SeqData.strRecipeName;

	
	//m_strAlarm.Format( _T("%x"), AprData.m_NowLotData.m_s)

	//m_SeqData.
}


void CIoMonitorDlg::MakeGridCtrl()
{
	if (m_pFontGrid == nullptr)
	{
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFontA( 12, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
			, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
			, FIXED_PITCH, "Arial");
	}

	//////////////////////////////////////////////////////////////////////////
	// Grid Control 생성
	CGridCtrl* pGridCtrl = NULL;
	for( int i = 0 ; i < 4 ; i++ )
	{
		int m_nFixCols = 1;
		int m_nFixRows = 1;
		int m_nCols = en_col_Max;
		int m_nRows;
		int nRow = 0;
		int nCol = 0;
		int nHeight = 25;

		switch (i)
		{
		case 0:
			pGridCtrl = &m_GridBitIn;
			m_nRows = MAX_ADR_BIT_IN + 1;
			break;

		case 1:
			pGridCtrl = &m_GridBitOut;
			m_nRows = MAX_ADR_BIT_OUT + 1;
			break;

		case 2:
			pGridCtrl = &m_GridWordIn;
			m_nRows = en_InWord_Max + 1;
			break;

		case 3:
			pGridCtrl = &m_GridWordOut;
			m_nRows = en_OutWord_Max + 1;
			break;

		default:
			break;
		}

		pGridCtrl->SetFont(m_pFontGrid, TRUE);

		pGridCtrl->SetAutoSizeStyle();
		pGridCtrl->SetFont(m_pFontGrid, TRUE);

		TRY
		{
			pGridCtrl->SetRowCount(m_nRows);
			pGridCtrl->SetColumnCount(m_nCols);
			pGridCtrl->SetFixedRowCount(m_nFixRows);
			pGridCtrl->SetFixedColumnCount(m_nFixCols);
		}
		CATCH(CMemoryException, e)
		{
			e->ReportError();
			return ;
		}
		END_CATCH

		CString strTitle[en_col_Max] = { _T("Category"),_T("Address"),_T("Content") };
		for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;
			Item.col = nCol;
			Item.strText = strTitle[nCol];

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strTitle[nCol], 2);

			pGridCtrl->SetItem(&Item);

			pGridCtrl->SetColAlignment(nCol, flexAlignLeftCenter /*flexAlignCenterCenter*/);
		}
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
	//크기 조정
	pGridCtrl->setGridFullColumnWidthVec();




	//////////////////////////////////////////////////////////////////////////
	// Bit IN
	pGridCtrl = &m_GridBitIn;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount() - 1; nCol++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;
			CString strTemp;
			switch (nCol)
			{
			case	0:
				strTemp = GetInBitName(row);
				break;
			case	1:
				strTemp = GetInBitAddress(row);
				break;
			}

			Item.strText = strTemp;

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strTemp, 2);

			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
	//크기조정
	pGridCtrl->setGridFullColumnWidthVec();

	// Bit OUT
	pGridCtrl = &m_GridBitOut;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount() - 1; nCol++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;
			CString strTemp;
			switch (nCol)
			{
			case	0:
				strTemp = GetOutBitName(row);
				break;
			case	1:
				strTemp = GetOutBitAddress(row);
				break;
			}

			Item.strText = strTemp;

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strTemp, 2);


			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
	//크기조정
	pGridCtrl->setGridFullColumnWidthVec();

	//////////////////////////////////////////////////////////////////////////
	// Word 입력 Data
	pGridCtrl = &m_GridWordIn;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount() - 1; nCol++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;
			CString strTemp;
			switch (nCol)
			{
			case	0 :
				strTemp = GetInWordName(row);
				break;
			case	1 :
				strTemp = GetInWordAddress(row);
				break;
			}

			Item.strText = strTemp;

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strTemp, 2);

			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
	//크기조정
	pGridCtrl->setGridFullColumnWidthVec();

	// Word 출력 Data
	pGridCtrl = &m_GridWordOut;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount()-1; nCol++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;
			CString strTemp;

			switch (nCol)
			{
			case	0:
				strTemp = GetOutWordName(row);
				break;
			case	1:
				strTemp = GetOutWordAddress(row);
				break;
			}

			Item.strText = strTemp;

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strTemp, 2);

			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 20 ;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
	//크기조정
	pGridCtrl->setGridFullColumnWidthVec();
}

void CIoMonitorDlg::UpdateGridCtrl()
{
	CGridCtrl* pGridCtrl;

	// Bit IN Data
	pGridCtrl = &m_GridBitIn;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = en_col_Data;

			CString strTemp;
			strTemp.Format(_T("%d"), m_bSigBitIn[row]);

			Item.strText = strTemp;
			pGridCtrl->SetItem(&Item);
		}
	}

	pGridCtrl->Refresh();


	// Bit IN Data
	pGridCtrl = &m_GridBitOut;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = en_col_Data;

			CString strTemp;
			strTemp.Format(_T("%d"), m_bSigBitOut[row]);

			Item.strText = strTemp;
			pGridCtrl->SetItem(&Item);
		}
	}

	pGridCtrl->Refresh();


	// Word 입력 Data
	pGridCtrl = &m_GridWordIn;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = en_col_Data ;

			CString strTemp;
			strTemp = GetInWordData(row);

			Item.strText = strTemp;
			pGridCtrl->SetItem(&Item);
		}
	}

	pGridCtrl->Refresh();


	// Word 출력 Data
	pGridCtrl = &m_GridWordOut;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		int row = nRow - 1;
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = en_col_Data;

			CString strTemp;
			strTemp = GetOutWordData(row);

			Item.strText = strTemp;
			pGridCtrl->SetItem(&Item);
		}
	}

	pGridCtrl->Refresh();

}



CString CIoMonitorDlg::GetInBitName(int nRow)
{
	CSigProc* pSigProc = theApp.m_pSigProc;

	CString strRet;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		switch (nRow)
		{
		case	CSigProc::enSmsBitIn_Alive:
			strRet = _T("Alive");
			break;
		case	CSigProc::enSmsBitIn_Ready:
			strRet = _T("Ready");
			break;
		case	CSigProc::enSmsBitIn_Run:
			strRet = _T("Run");
			break;
			//	case	CSigProc::enSmsBitIn_EncoderReset:		//사용 안함
			//		strRet = _T("Encoder Zero Set");
			//		break;
		case	CSigProc::enSmsBitIn_TabZeroReset:
			strRet = _T("Tab Zero Set");
			break;
		case	CSigProc::enSmsBitIn_InkMarkingActive:
			strRet = _T("Ink Marking Active");
			break;
		case	CSigProc::enSmsBitIn_ConnectZone:
			strRet = _T("Connect Zone Pass");
			break;
		case	CSigProc::enSmsBitIn_RecipeChange:
			strRet = _T("Vision Recipe Change");
			break;
		case	CSigProc::enSmsBitIn_LotStartReq:
			strRet = _T("Lot Start Request");
			break;
		case	CSigProc::enSmsBitIn_LotEndReq:
			strRet = _T("Lot End Request");
			break;
		case	CSigProc::enSmsBitIn_AlarmResetReq:
			strRet = _T("Alarm Reset");
			break;
		case	CSigProc::enSmsBitIn_AlarmNgAck:
			strRet = _T("Alarm NG Ack");
			break;
		default:
			strRet = _T("");
			break;
		}

	}
	else
	{
		int nPort = (nRow / 8);
		unsigned short nBitPos = 0x00000001 << (nRow - (nPort * 8));
		if (nPort != 0)
		{
			nBitPos += 0x00000001 << (8 + (nPort - 1));
		}

		switch (nBitPos)
		{
		case	CSigProc::enBitIn_Alive:
			strRet = _T("Alive");
			break;
		case	CSigProc::enBitIn_Ready:
			strRet = _T("Ready");
			break;
		case	CSigProc::enBitIn_Run:
			strRet = _T("Run");
			break;
			//	case	CSigProc::enBitIn_EncoderReset:		//사용 안함
			//		strRet = _T("Encoder Zero Set");
			//		break;
		case	CSigProc::enBitIn_TabZeroReset:
			strRet = _T("Tab Zero Set");
			break;
		case	CSigProc::enBitIn_InkMarkingActive:
			strRet = _T("Ink Marking Active");
			break;
		case	CSigProc::enBitIn_ConnectZone:
			strRet = _T("Connect Zone Pass");
			break;
		case	CSigProc::enBitIn_RecipeChange:
			strRet = _T("Vision Recipe Change");
			break;
		case	CSigProc::enBitIn_LotStartReq:
			strRet = _T("Lot Start Request");
			break;
		case	CSigProc::enBitIn_LotEndReq:
			strRet = _T("Lot End Request");
			break;
		case	CSigProc::enBitIn_AlarmResetReq:
			strRet = _T("Alarm Reset");
			break;
		case	CSigProc::enBitIn_AlarmNgAck:
			strRet = _T("Alarm NG Ack");
			break;
		default:
			strRet = _T("");
			break;
		}
	}






	return strRet;
}

CString CIoMonitorDlg::GetOutBitName(int nRow)
{
	CSigProc* pSigProc = theApp.m_pSigProc;

	CString strRet;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		switch (nRow)
		{
		case	CSigProc::enSmsBitOut_Alive:
			strRet = _T("Alive");
			break;
		case	CSigProc::enSmsBitOut_Ready:
			strRet = _T("Ready");
			break;
			//	case	CSigProc::enSmsBitOut_EncoderSet:		//사용 안함
			//		strRet = _T("Encoder Zero Set");
			//		break;
		case	CSigProc::enSmsBitOut_RecipeChangeAck:
			strRet = _T("Recipe Change Ack");
			break;
		case	CSigProc::enSmsBitOut_LotStartReqAck:
			strRet = _T("Lot Start Ack");
			break;
		case	CSigProc::enSmsBitOut_LotEndReqAck:
			strRet = _T("Lot End Ack");
			break;
		case	CSigProc::enSmsBitOut_TabZeroReset:
			strRet = _T("Tab Zero Set Ack");
			break;
		case	CSigProc::enSmsBitOut_AlarmResetAck:
			strRet = _T("Alarm Reset Ack");
			break;
			//	case	CSigProc::enSmsBitOut_DiskSpaceWarning:		//사용 안함
			//		strRet = _T("");
			//		break;
			//	case	CSigProc::enSmsBitOut_DiskSpaceAlarm:		//사용 안함
			//		strRet = _T("");
			//		break;

		default:
			strRet = _T("");
			break;
		}

	}
	else
	{
		int nPort = (nRow / 8);
		unsigned short nBitPos = 0x00000001 << (nRow - (nPort * 8));
		if (nPort != 0)
		{
			nBitPos += 0x00000001 << (8 + (nPort - 1));
		}

		switch (nBitPos)
		{
		case	CSigProc::enBitOut_Alive:
			strRet = _T("Alive");
			break;
		case	CSigProc::enBitOut_Ready:
			strRet = _T("Ready");
			break;
			//	case	CSigProc::enBitOut_EncoderSet:		//사용 안함
			//		strRet = _T("Encoder Zero Set");
			//		break;
		case	CSigProc::enBitOut_TabZeroReset:
			strRet = _T("Tab Zero Set Ack");
			break;
			//	case	CSigProc::enBitOut_DiskSpaceWarning:		//사용 안함
			//		strRet = _T("");
			//		break;
			//	case	CSigProc::enBitOut_DiskSpaceAlarm:		//사용 안함
			//		strRet = _T("");
			//		break;
		case	CSigProc::enBitOut_RecipeChangeAck:
			strRet = _T("Recipe Change Ack");
			break;
		case	CSigProc::enBitOut_LotStartReqAck:
			strRet = _T("Lot Start Ack");
			break;
		case	CSigProc::enBitOut_LotEndReqAck:
			strRet = _T("Lot End Ack");
			break;
		case	CSigProc::enBitOut_AlramResetAck:
			strRet = _T("Alarm Reset Ack");
			break;

		default:
			strRet = _T("");
			break;
		}

	}


	return strRet;
}


CString CIoMonitorDlg::GetInBitAddress(int nRow)
{
	CString strRet;

	int nAddress = AprData.m_System.m_nBitIn + nRow;
	
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec)
	{
		strRet.Format(_T("0x%04X"), nAddress);
	}
	else
	{
		strRet.Format(_T("%04d"), nAddress);
	}

	return strRet;
}

CString CIoMonitorDlg::GetOutBitAddress(int nRow)
{
	CString strRet;

	int nAddress = AprData.m_System.m_nBitOut + nRow;

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec)
	{
		strRet.Format(_T("0x%04X"), nAddress);
	}
	else
	{
		strRet.Format(_T("%04d"), nAddress);
	}

	return strRet;
}

CString CIoMonitorDlg::GetInWordName(int nRow)
{
	CString strRet ;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		switch (nRow)
		{
		case	CSigProc::enSmsWordRead_RecipeNo:
			strRet = _T("Recipe No");
			break;

		case	CSigProc::enSmsWordRead_RecipeName:
			strRet = _T("Recipe Name 1");
			break;
		case	CSigProc::enSmsWordRead_RecipeName + 1:
			strRet = _T("Recipe Name 2");
			break;
		case	CSigProc::enSmsWordRead_RecipeName + 2:
			strRet = _T("Recipe Name 3");
			break;
		case	CSigProc::enSmsWordRead_RecipeName + 3:
			strRet = _T("Recipe Name 4");
			break;

		case	CSigProc::enSmsWordRead_CELL_ID:
			strRet = _T("Lot ID 1");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 1:
			strRet = _T("Lot ID 2");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 2:
			strRet = _T("Lot ID 3");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 3:
			strRet = _T("Lot ID 4");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 4:
			strRet = _T("Lot ID 5");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 5:
			strRet = _T("Lot ID 6");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 6:
			strRet = _T("Lot ID 7");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 7:
			strRet = _T("Lot ID 8");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 8:
			strRet = _T("Lot ID 9");
			break;
		case	CSigProc::enSmsWordRead_CELL_ID + 9:
			strRet = _T("Lot ID 10");
			break;

		case	CSigProc::enSmsWordRead_FoilExpInTopTarget :
			strRet = _T("Foil Exp In Tab Target");
			break;
		case	CSigProc::enSmsWordRead_FoilExpInBtmTarget:
			strRet = _T("Foil Exp In Bottom Target");
			break;
		case	CSigProc::enSmsWordRead_FoilExpOutTopTarget:
			strRet = _T("Foil Exp Out Tab Target");
			break;
		case	CSigProc::enSmsWordRead_FoilExpOutBtmTarget:
			strRet = _T("Foil Exp Out Bottom Target");
			break;
		case	CSigProc::enSmsWordRead_FoilExpBothTopTarget:
			strRet = _T("Foil Exp Both Tab Target");
			break;
		case	CSigProc::enSmsWordRead_FoilExpBothBtmTarget:
			strRet = _T("Foil Exp Both Bottom Target");
			break;
		case	CSigProc::enSmsWordRead_SpeterTopTarget:
			strRet = _T("Spatter Tab Target");
			break;
		case	CSigProc::enSmsWordRead_SpeterBtmTarget:
			strRet = _T("Spatter Bottom Target");
			break;
		case	CSigProc::enSmsWordRead_PrmContinuousCnt:
			strRet = _T("Continuous NG Count");
			break;
		case	CSigProc::enSmsWordRead_PrmSectorNgTabCnt:
			strRet = _T("Sector NG Count");
			break;
		case	CSigProc::enSmsWordRead_PrmSectorBaseCnt:
			strRet = _T("Sector Range Tab Count");
			break;



		default:
			strRet = _T("");
			break;
		}

	}
	else
	{
		nRow *= 2;
		switch (nRow)
		{
		case	CSigProc::enWordRead_RecipeNo:
			strRet = _T("Recipe No");
			break;

		case	CSigProc::enWordRead_RecipeName:
			strRet = _T("Recipe Name 1");
			break;
		case	CSigProc::enWordRead_RecipeName + 2:
			strRet = _T("Recipe Name 2");
			break;
		case	CSigProc::enWordRead_RecipeName + 4:
			strRet = _T("Recipe Name 3");
			break;
		case	CSigProc::enWordRead_RecipeName + 6:
			strRet = _T("Recipe Name 4");
			break;

		case	CSigProc::enWordRead_CELL_ID:
			strRet = _T("Lot ID 1");
			break;
		case	CSigProc::enWordRead_CELL_ID + 2:
			strRet = _T("Lot ID 2");
			break;
		case	CSigProc::enWordRead_CELL_ID + 4:
			strRet = _T("Lot ID 3");
			break;
		case	CSigProc::enWordRead_CELL_ID + 6:
			strRet = _T("Lot ID 4");
			break;
		case	CSigProc::enWordRead_CELL_ID + 8:
			strRet = _T("Lot ID 5");
			break;
		case	CSigProc::enWordRead_CELL_ID + 10:
			strRet = _T("Lot ID 6");
			break;
		case	CSigProc::enWordRead_CELL_ID + 12:
			strRet = _T("Lot ID 7");
			break;
		case	CSigProc::enWordRead_CELL_ID + 14:
			strRet = _T("Lot ID 8");
			break;
		case	CSigProc::enWordRead_CELL_ID + 16:
			strRet = _T("Lot ID 9");
			break;
		case	CSigProc::enWordRead_CELL_ID + 18:
			strRet = _T("Lot ID 10");
			break;

		case	CSigProc::enWordRead_FoilExpInTopTarget:
			strRet = _T("Foil Exp In Tab Target");
			break;
		case	CSigProc::enWordRead_FoilExpInBtmTarget:
			strRet = _T("Foil Exp In Bottom Target");
			break;
		case	CSigProc::enWordRead_FoilExpOutTopTarget:
			strRet = _T("Foil Exp Out Tab Target");
			break;
		case	CSigProc::enWordRead_FoilExpOutBtmTarget:
			strRet = _T("Foil Exp Out Bottom Target");
			break;
		case	CSigProc::enWordRead_FoilExpBothTopTarget:
			strRet = _T("Foil Exp Both Tab Target");
			break;
		case	CSigProc::enWordRead_FoilExpBothBtmTarget:
			strRet = _T("Foil Exp Both Bottom Target");
			break;
		case	CSigProc::enWordRead_SpeterTopTarget:
			strRet = _T("Spatter Tab Target");
			break;
		case	CSigProc::enWordRead_SpeterBtmTarget:
			strRet = _T("Spatter Bottom Target");
			break;

		case	CSigProc::enWordRead_PrmContinuousCnt:
			strRet = _T("Continuous NG Count");
			break;
		case	CSigProc::enWordRead_PrmSectorNgTabCnt:
			strRet = _T("Sector NG Count");
			break;
		case	CSigProc::enWordRead_PrmSectorBaseCnt:
			strRet = _T("Sector Range Tab Count");
			break;



		default:
			strRet = _T("");
			break;
		}

	}

	return strRet;
}

CString CIoMonitorDlg::GetOutWordName(int nRow)
{
	CString strRet;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		switch (nRow)
		{
		case	CSigProc::enSmsWordWrite_DataReportV1_Ea:
			strRet = _T("Total Count");
			break;
		case	CSigProc::enSmsWordWrite_DataReportV2_OK:
			strRet = _T("OK Count");
			break;
		case	CSigProc::enSmsWordWrite_DataReportV3_NG:
			strRet = _T("NG Count");
			break;
		case	CSigProc::enSmsWordWrite_DataReportV4_OkRate:
			strRet = _T("OK Rate");
			break;
		case	CSigProc::enSmsWordWrite_DataReportV5_NgRate:
			strRet = _T("NG Rate");
			break;
		case	CSigProc::enSmsWordWrite_DataReportV6_RunRate:
			strRet = _T("Run Rate");
			break;
		case	CSigProc::enSmsWordWrite_Continue_Alarm_Cnt:
			strRet = _T("Continuous Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_Heavy_Alarm_Cnt:
			strRet = _T("Sector Alarm Count");
			break;


		case	CSigProc::enSmsWordWrite_FoilExpInTop_Alarm_Cnt:
			strRet = _T("Foil Exp In Tab Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpInBtm_Alarm_Cnt:
			strRet = _T("Foil Exp In Bottom Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpOutTop_Alarm_Cnt:
			strRet = _T("Foil Exp Out Tab Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpOutBtm_Alarm_Cnt:
			strRet = _T("Foil Exp Out Bottom Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpBothTop_Alarm_Cnt:
			strRet = _T("Foil Exp Both Tab Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpBothBtm_Alarm_Cnt:
			strRet = _T("Foil Exp Both Bottom Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_SpeterTop_Alarm_Cnt:
			strRet = _T("Spatter Tab Alarm Count");
			break;
		case	CSigProc::enSmsWordWrite_SpeterBtm_Alarm_Cnt:
			strRet = _T("Spatter Bottom Alarm Count");
			break;

		case	CSigProc::enSmsWordWrite_Top_Defect_Count_Real:
			strRet = _T("RealTime Tab NG Count");
			break;
		case	CSigProc::enSmsWordWrite_Btm_Defect_Count_Real:
			strRet = _T("RealTime Bottom NG Count");
			break;
		case	CSigProc::enSmsWordWrite_Top_Defect_Count_LotEnd:
			strRet = _T("LotEnd Time Tab NG Count");
			break;
		case	CSigProc::enSmsWordWrite_Btm_Defect_Count_LotEnd:
			strRet = _T("LotEnd Time Bottom NG Count");
			break;




		case	CSigProc::enSmsWordWrite_FoilExpInTopTarget:
			strRet = _T("Foil Exp In Tab Target");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpInBtmTarget:
			strRet = _T("Foil Exp In Bottom Target");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpOutTopTarget:
			strRet = _T("Foil Exp Out Tab Target");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpOutBtmTarget:
			strRet = _T("Foil Exp Out Bottom Target");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpBothTopTarget:
			strRet = _T("Foil Exp Both Tab Target");
			break;
		case	CSigProc::enSmsWordWrite_FoilExpBothBtmTarget:
			strRet = _T("Foil Exp Both Bottom Target");
			break;
		case	CSigProc::enSmsWordWrite_SpeterTopTarget:
			strRet = _T("Spatter Tab Target");
			break;
		case	CSigProc::enSmsWordWrite_SpeterBtmTarget:
			strRet = _T("Spatter Bottom Target");
			break;

		case	CSigProc::enSmsWordWrite_PrmContinuousCnt:
			strRet = _T("Continuous NG Count");
			break;
		case	CSigProc::enSmsWordWrite_PrmSectorNgTabCnt:
			strRet = _T("Sector NG Count");
			break;
		case	CSigProc::enSmsWordWrite_PrmSectorBaseCnt:
			strRet = _T("Sector Range Tab Count");
			break;


			break;
		case	CSigProc::enSmsWordWrite_AlarmExist:
			strRet = _T("Alarm Exist");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer1:
			strRet = _T("Alarm 01: Continuous NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer2:
			strRet = _T("Alarm 02: Not Use");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer3:
			strRet = _T("Alarm 03: Sector NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer4:
			strRet = _T("Alarm 04: Foil Exp In Tab NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer5:
			strRet = _T("Alarm 05: Foil Exp In Bottom NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer6:
			strRet = _T("Alarm 06: Foil Exp Out Tab NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer7:
			strRet = _T("Alarm 07: Foil Exp Out Bottom NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer8:
			strRet = _T("Alarm 08: Foil Exp Both Tab NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer9:
			strRet = _T("Alarm 09: Foil Exp Both Bottom NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer10:
			strRet = _T("Alarm 10: Spatter Tab NG");
			break;
		case	CSigProc::enSmsWordWrite_AlarmCode_Buffer11:
			strRet = _T("Alarm 11: Spatter Bottom NG");
			break;




		case	CSigProc::en_SmsWordWrite_Cell_Trigger_ID:
			strRet = _T("Cell ID");
			break;
		case	CSigProc::en_SmsWordWrite_Judge:
			strRet = _T("OK/NG");
			break;
		case	CSigProc::en_SmsWordWrite_NG_Code:
			strRet = _T("NG Code");
			break;



		default:
			strRet = _T("");
			break;
		}
	}
	else
	{
		nRow *= 2;
		switch (nRow)
		{
		case	CSigProc::enWordWrite_DataReportV1_Ea:
			strRet = _T("Total Count");
			break;
		case	CSigProc::enWordWrite_DataReportV2_OK:
			strRet = _T("OK Count");
			break;
		case	CSigProc::enWordWrite_DataReportV3_NG:
			strRet = _T("NG Count");
			break;
		case	CSigProc::enWordWrite_DataReportV4_OkRate:
			strRet = _T("OK Rate");
			break;
		case	CSigProc::enWordWrite_DataReportV5_NgRate:
			strRet = _T("NG Rate");
			break;
		case	CSigProc::enWordWrite_DataReportV6_RunRate:
			strRet = _T("Run Rate");
			break;

		case	CSigProc::enWordWrite_Continue_Alarm_Cnt:
			strRet = _T("Continuous Alarm Count");
			break;
		case	CSigProc::enWordWrite_Heavy_Alarm_Cnt:
			strRet = _T("Sector Alarm Count");
			break;
		case	CSigProc::enWordWrite_FoilExpInTop_Alarm_Cnt:
			strRet = _T("Foil Exp In Tab Alarm Count");
			break;
		case	CSigProc::enWordWrite_FoilExpInBottom_Alarm_Cnt:
			strRet = _T("Foil Exp In Bottom Alarm Count");
			break;
		case	CSigProc::enWordWrite_FoilExpOutTop_Alarm_Cnt:
			strRet = _T("Foil Exp Out Tab Alarm Count");
			break;
		case	CSigProc::enWordWrite_FoilExpOutBottom_Alarm_Cnt:
			strRet = _T("Foil Exp Out Bottom Alarm Count");
			break;
		case	CSigProc::enWordWrite_FoilExpBothTop_Alarm_Cnt:
			strRet = _T("Foil Exp Both Tab Alarm Count");
			break;
		case	CSigProc::enWordWrite_FoilExpBothBottom_Alarm_Cnt:
			strRet = _T("Foil Exp Both Bottom Alarm Count");
			break;
		case	CSigProc::enWordWrite_SpeterTop_Alarm_Cnt:
			strRet = _T("Spatter Tab Alarm Count");
			break;
		case	CSigProc::enWordWrite_SpeterBtm_Alarm_Cnt:
			strRet = _T("Spatter Bottom Alarm Count");
			break;
		case	CSigProc::enWordWrite_Top_Defect_Count_Real:
			strRet = _T("RealTime Tab NG Count");
			break;
		case	CSigProc::enWordWrite_Btm_Defect_Count_Real:
			strRet = _T("RealTime Bottom NG Count");
			break;
		case	CSigProc::enWordWrite_Top_Defect_Count_LotEnd:
			strRet = _T("LotEnd Time Tab NG Count");
			break;
		case	CSigProc::enWordWrite_Btm_Defect_Count_LotEnd:
			strRet = _T("LotEnd Time Bottom NG Count");
			break;

		case	CSigProc::enWordWrite_FoilExpInTopTarget:
			strRet = _T("Foil Exp In Tab Target");
			break;
		case	CSigProc::enWordWrite_FoilExpInBottomTarget:
			strRet = _T("Foil Exp In Bottom Target");
			break;
		case	CSigProc::enWordWrite_FoilExpOutTopTarget:
			strRet = _T("Foil Exp Out Tab Target");
			break;
		case	CSigProc::enWordWrite_FoilExpOutBottomTarget:
			strRet = _T("Foil Exp Out Bottom Target");
			break;
		case	CSigProc::enWordWrite_FoilExpBothTopTarget:
			strRet = _T("Foil Exp Both Tab Target");
			break;
		case	CSigProc::enWordWrite_FoilExpBothBottomTarget:
			strRet = _T("Foil Exp Both Bottom Target");
			break;
		case	CSigProc::enWordWrite_SpeterTopTarget:
			strRet = _T("Spatter Tab Target");
			break;
		case	CSigProc::enWordWrite_SpeterBtmTarget:
			strRet = _T("Spatter Bottom Target");
			break;

		case	CSigProc::enWordWrite_PrmContinuousCnt:
			strRet = _T("Continuous NG Count");
			break;
		case	CSigProc::enWordWrite_PrmSectorNgTabCnt:
			strRet = _T("Sector NG Count");
			break;
		case	CSigProc::enWordWrite_PrmSectorBaseCnt:
			strRet = _T("Sector Range Tab Count");
			break;

		case	CSigProc::enWordWrite_AlarmExist:
			strRet = _T("Alarm Exist");
			break;
		case	CSigProc::enWordWrite_AlarmCode_Buffer1:
			strRet = _T("Alarm Code Buffer 1");
			break;
		case	CSigProc::enWordWrite_AlarmCode_Buffer2:
			strRet = _T("Alarm Code Buffer 2");
			break;
		case	CSigProc::enWordWrite_AlarmCode_Buffer3:
			strRet = _T("Alarm Code Buffer 3");
			break;
		case	CSigProc::enWordWrite_AlarmCode_Buffer4:
			strRet = _T("Alarm Code Buffer 4");
			break;
		case	CSigProc::enWordWrite_AlarmCode_Buffer5:
			strRet = _T("Alarm Code Buffer 5");
			break;

		case	CSigProc::en_WordWrite_Cell_Trigger_ID:
			strRet = _T("Cell ID");
			break;
		case	CSigProc::en_WordWrite_Judge:
			strRet = _T("OK/NG");
			break;
		case	CSigProc::en_WordWrite_NG_Code:
			strRet = _T("NG Code");
			break;

		default:
			strRet = _T("");
			break;
		}
	}
	
	return strRet;
}

// 22.08.16 Ahn Add Start
CString CIoMonitorDlg::GetInWordAddress(int nRow)
{
	CString strRet;
	
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec)
	{
		strRet.Format(_T("0x%04X"), AprData.m_System.m_nWordIn + (nRow*2));
	}
	else
	{
		strRet.Format(_T("%04d"), AprData.m_System.m_nWordIn + nRow);
	}
	// 23.03.03 Son Modify End
	return strRet;
}


CString CIoMonitorDlg::GetOutWordAddress(int nRow)
{
	CString strRet;

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec)
	{
		strRet.Format(_T("0x%04X"), AprData.m_System.m_nWordOut + (nRow * 2));
	}
	else
	{
		strRet.Format(_T("%04d"), AprData.m_System.m_nWordOut + nRow);
	}

	return strRet;
}

CString CIoMonitorDlg::GetInWordData(int nRow)
{
	CString strRet;

	CSigProc* pSigProc = theApp.m_pSigProc;

	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		strRet.Format(_T("%d"), (int)pSigProc->GetMonitoringReadData_Siemens(nRow));
	}
	else
	{
		strRet.Format(_T("%d"), (int)pSigProc->GetMonitoringReadData_Melsec(nRow * 2));
	}

	return strRet;
}
CString CIoMonitorDlg::GetOutWordData(int nRow)
{
	CString strRet;

	CSigProc* pSigProc = theApp.m_pSigProc;
	if (AprData.m_System.m_nPlcMode == en_Plc_Siemens)
	{
		strRet.Format(_T("%d"), (int)pSigProc->GetMonitoringWriteData_Siemens(nRow));
	}
	else
	{
		strRet.Format(_T("%d"), (int)pSigProc->GetMonitoringWriteData_Melsec(nRow * 2));
	}


	return strRet;
}
// 22.08.16 Ahn Add End

// 22.10.05 Ahn Add Start
void CIoMonitorDlg::OnBnClickedBtnDummyError()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (MessageBox(_T("Dummy NG 발생시 장비가 정지 할 수 있습니다. 정말 NG를 발생시키 시겠습니까?"), _T("Dummy NG"), MB_OKCANCEL) == IDOK) {
		// 22.10.05 Ahn Add Start
		WORD wAlarmCode = 0x0000 ;
		wAlarmCode |= CSigProc::en_Alarm_SectorNg;

		CString strMessage;
		strMessage.Format(_T("Dummy 구간 NG Alarm 발생. Dummy NG 발생"));
		AprData.m_ErrStatus.SetError(CErrorStatus::en_SectorNg, strMessage);

		CSigProc* pSigProc = theApp.m_pSigProc;	
		pSigProc->WriteAlarmCode(wAlarmCode);
		pSigProc->SigOutAlarmExist(TRUE);

//   		Sleep(200);
//  		pSigProc->WriteAlarmCode(0x0000);
// 		pSigProc->SigOutAlarmExist(FALSE);


// 22.10.05 Ahn Add End
	}
}
// 22.10.05 Ahn Add End


void CIoMonitorDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		SetCheckIoTimer();

	}
}
void CIoMonitorDlg::OnBnClickedBtnDummyErrorClear()
{
	CSigProc* pSigProc = theApp.m_pSigProc;

	pSigProc->WriteAlarmCode(0x0000);
	pSigProc->SigOutAlarmExist(FALSE);
}


void CIoMonitorDlg::OnMouseDblClickGridBitOut(NMHDR* pNMHDR, LRESULT* pResult)
{
	CSigProc* pSigProc = theApp.m_pSigProc;
	CGridCtrl* pGridCtrl = &m_GridBitOut;

	if (pGridCtrl != NULL)
	{
		int	iMaxColNum = pGridCtrl->GetCols();
		int	iMaxRowNum = pGridCtrl->GetRows();
		int iRow = pGridCtrl->GetRow();
		int iCol = pGridCtrl->GetCol();

		if (iCol == 2 && iRow < MAX_ADR_BIT_OUT)
		{
			CString	szTemp = pGridCtrl->GetTextMatrix(iRow, iCol);

			int nCol = iCol - 1;
			int nRow = iRow - 1;
			BOOL bSignalOut = (_ttoi(szTemp)==0) ? TRUE : FALSE;

			int nPort = (nRow / 8);
			unsigned short nBitPos = 0x00000001 << (nRow - (nPort * 8));
			if (nPort != 0)
			{
				nBitPos += 0x00000001 << (8 + (nPort - 1));
			}

			CString str;
			str.Format(_T("[ Send Data ]\r\n\r\n  * Address:	%s\r\n  * data:		%d		"), GetOutBitAddress(nRow), bSignalOut);
			if (AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				if (pSigProc->SignalBitOut(nBitPos, (int)bSignalOut) < 0)
				{
					AfxMessageBox(_T("Failed to send data."), MB_ICONERROR);
					return;
				}
			}


			pGridCtrl->Refresh();

		}


	}
}