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
	//DDX_Text(pDX,	IDC_ED_RCP_NAME, m_strRecipeName);
	//DDX_Text(pDX,	IDC_ED_ALARM_OUT, m_strAlarm);
	//DDX_Text(pDX,	IDC_ED_ALARM_BIT, m_strAlarmBit);
	// 22.08.12 Ahn Add Start
	DDX_Control(pDX, IDC_GRID_WORD_IN, m_GridWordIn) ;
	DDX_Control(pDX, IDC_GRID_WORD_OUT, m_GridWordOut) ;
	// 22.08.12 Ahn Add End
}


BEGIN_MESSAGE_MAP(CIoMonitorDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CIoMonitorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_DATA_LOAD, &CIoMonitorDlg::OnBnClickedBtnDataLoad)
	ON_BN_CLICKED(IDC_BTN_DUMMY_ERROR, &CIoMonitorDlg::OnBnClickedBtnDummyError)
END_MESSAGE_MAP()


// CIoMonitorDlg 메시지 처리기


void CIoMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_TID_CheckIO == nIDEvent) {
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


	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	int nMaxBit = en_In_Max;
	int i;

	for ( i = 0; i < nMaxBit; i++) {
		m_bmpSigIn[i][en_mode_on].SetBitmap(IDB_GREEN_LED_ON);
		m_bmpSigIn[i][en_mode_on].ShowWindow(SW_HIDE);
		m_bmpSigIn[i][en_mode_off].SetBitmap(IDB_GREEN_LED_OFF);
		m_bmpSigIn[i][en_mode_off].ShowWindow(SW_SHOW);
	}

	nMaxBit = en_Out_Max;
	for (i = 0; i < nMaxBit; i++) {
		m_bmpSigOut[i][en_mode_on].SetBitmap(IDB_GREEN_LED_ON);
		m_bmpSigOut[i][en_mode_on].ShowWindow(SW_HIDE);
		m_bmpSigOut[i][en_mode_off].SetBitmap(IDB_GREEN_LED_OFF);
		m_bmpSigOut[i][en_mode_off].ShowWindow(SW_SHOW);
	}

	SetCheckIoTimer();

	CSigProc* pSigProc = theApp.m_pSigProc;;
	pSigProc->ReadBlockAllData( &m_localSeqDataIn );
	pSigProc->ReadBlockWriteDataAll(&m_localSeqDataOut);

	MakeGridCtrl();
	UpdateGridCtrl();

//	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 23.03.03 Ahn Add Start
int CIoMonitorDlg::GetAddressAndBitPos( int nMode, int nSigNo, int &nBitPos  )
{
	int nAddress = 0;

	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
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
			case	en_Out_RcpChangeAck:
				nAddress = CSigProc::enBitOut_DiskSpaceWarning;
				nBitPos = 8;
				break;
			case	en_Out_DiskSpaceAlarm:
				nAddress = CSigProc::enBitOut_DiskSpaceAlarm;
				nBitPos = 9;
				break;
			case	en_Out_DiskSpaceWarning:
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
				nAddress = CSigProc::enSmsBitOut_DiskSpaceWarning ;
				break;
			case	en_Out_DiskSpaceAlarm:
				nAddress = CSigProc::enSmsBitOut_DiskSpaceAlarm ;
				break;
			case	en_Out_DiskSpaceWarning:
				nAddress = CSigProc::enSmsBitOut_RecipeChangeAck ;
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
// 	AprData.SaveDebugLog(_T("RefreshAll")); //pyjtest


	int nRet = 0;

	CSigProc* pSigProc = theApp.m_pSigProc;;
	// 23.03.03 Ahn Modify Start
	//memset(m_bSigBitIn, 0x0000, sizeof(BOOL) * en_In_Max);
	//memset(m_bSigBitOut, 0x0000, sizeof(BOOL) * en_Out_Max);
	memset(m_bSigBitIn, 0x0000, sizeof(BOOL) * MAX_ADR_BIT_IN);
	memset(m_bSigBitOut, 0x0000, sizeof(BOOL) * MAX_ADR_BIT_OUT);
	// 23.03.03 Ahn Modify End
	pSigProc->ReadAllPort_BitIn(m_bSigBitIn);
	pSigProc->ReadAllPort_BitOut(m_bSigBitOut); //pyjtest


// 	CString strMsg;
// 	strMsg.Format(_T("SigIn = %d"), m_bSigBitIn[0] );
// 	AprData.SaveDebugLog(strMsg); //pyjtest


	//int nIdOnOff[2] = { IDB_GREEN_LED_OFF , IDB_GREEN_LED_ON };

	int bit = 0 ;
	int i = 0;

	int nMaxBit = en_In_Max ;

	int nPort = 0 ;
	BYTE bBit = 0 ;
	int nAddress = 0;
	// 22.09.22 Ahn Add Start
	int nBitPos = 0;
	// 22.09.22 Ahn Add End
	for (i = 0; i < nMaxBit; i++) {
		int nShift = 0;
		// 23.03.02 Ahn Modify Start
		// ..... Move All Switch() ....
		nAddress = GetAddressAndBitPos(MODE_READ, i, nBitPos);
		//CSigProc::GetPortBit(nAddress, &nPort, &bBit);
		// 23.03.02 Ahn Modify End

		int nOn = 0;
		// 22.09.22 Ahn Modify Start
		if ( m_bSigBitIn[nBitPos] == TRUE ) { // 22.08.11 Ahn Modify
		// 22.09.22 Ahn Modify End
			nOn = 1;
		}
		if( (m_bmpSigIn[i][en_mode_on].m_hWnd != nullptr) && (m_bmpSigIn[i][en_mode_off].m_hWnd != nullptr)){
			if (nOn == 1) {
				m_bmpSigIn[i][en_mode_off].ShowWindow(SW_HIDE);
				m_bmpSigIn[i][en_mode_on].ShowWindow(SW_SHOW);
			}
			else {
				m_bmpSigIn[i][en_mode_on].ShowWindow(SW_HIDE);
				m_bmpSigIn[i][en_mode_off].ShowWindow(SW_SHOW);
			}
		}
	}

	nMaxBit = en_Out_Max;
	for (i = 0; i < nMaxBit; i++) {

		// 23.03.02 Ahn Modify Start
		// ..... Move All Switch() ....
		nAddress = GetAddressAndBitPos(MODE_WRITE, i, nBitPos) ;
		// 23.03.02 Ahn Modify End

		//CSigProc::GetPortBit(nAddress, &nPort, &bBit);

		int nOn = 0;
		if ( m_bSigBitOut[nBitPos] == TRUE ) { // 22.08.11 Ahn Modify
			nOn = 1;
		}
		if ((m_bmpSigOut[i][en_mode_on].m_hWnd != nullptr) && (m_bmpSigOut[i][en_mode_off].m_hWnd != nullptr)) {
			if (nOn == 1) {
				m_bmpSigOut[i][en_mode_off].ShowWindow(SW_HIDE);
				m_bmpSigOut[i][en_mode_on].ShowWindow(SW_SHOW);
			} else {
				m_bmpSigOut[i][en_mode_on].ShowWindow(SW_HIDE);
				m_bmpSigOut[i][en_mode_off].ShowWindow(SW_SHOW);
			}
		}
	}

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//m_pParent->
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
	if (m_pFontGrid == nullptr) {
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFontA( 12, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
			, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
			, FIXED_PITCH, "Gulim");
	}

	CGridCtrl* pGridCtrl;
	for( int i = 0 ; i < 2 ; i++ )
	{
		int m_nFixCols = 1;
		int m_nFixRows = 1;
		int m_nCols = en_col_Max;
		int m_nRows  ;
		int nRow = 0;
		int nCol = 0;
		int nHeight = 25;

		if (i == 0) {
			pGridCtrl = &m_GridWordIn;
			m_nRows = en_InWord_Max + 1 ;
		} else {
			pGridCtrl = &m_GridWordOut;
			m_nRows = en_OutWord_Max + 1 ;
		}
		pGridCtrl->SetFont(m_pFontGrid, TRUE);

		pGridCtrl->SetAutoSizeStyle();
		pGridCtrl->SetFont(m_pFontGrid, TRUE);

		TRY{
			pGridCtrl->SetRowCount(m_nRows);
			pGridCtrl->SetColumnCount(m_nCols);
			pGridCtrl->SetFixedRowCount(m_nFixRows);
			pGridCtrl->SetFixedColumnCount(m_nFixCols);
		}
			CATCH(CMemoryException, e) {
			e->ReportError();
			return ;
		}
		END_CATCH

		CString strTitle[en_col_Max] = { _T("항목명"),_T("선두주소"),_T("내용") };
		for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;
			Item.col = nCol;
			Item.strText = strTitle[nCol];
			pGridCtrl->SetItem(&Item);
			int nWidth = 70;
			switch (nCol) {
			case	en_col_Name :
				nWidth = 140;
				break;
			case	en_col_Address :
				nWidth = 70;
				break;
			case	en_col_Data :
				nWidth = 100;
				break;
			default:
				nWidth = 50;
				break;
			}
			pGridCtrl->SetColumnWidth(nCol, nWidth);
			pGridCtrl->SetColAlignment(nCol, flexAlignCenterCenter);
		}
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}

	// Word 입력 Data
	pGridCtrl = &m_GridWordIn;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
		int row = nRow - 1;
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount() - 1; nCol++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;
			CString strTemp;
			switch (nCol) {
			case	0 :
				strTemp = GetInWordName(row);
				break;
			case	1 :
				strTemp = GetInWordAddress(row);
				break;
			}

			Item.strText = strTemp;
			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}

	// Word 출력 Data
	pGridCtrl = &m_GridWordOut;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
		int row = nRow - 1;
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount()-1; nCol++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;
			CString strTemp;

			switch (nCol) {
			case	0:
				strTemp = GetOutWordName(row);
				break;
			case	1:
				strTemp = GetOutWordAddress(row);
				break;
			}

			Item.strText = strTemp;
			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 20 ;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
}

void CIoMonitorDlg::UpdateGridCtrl()
{
	CGridCtrl* pGridCtrl;

	// Word 입력 Data
	pGridCtrl = &m_GridWordIn;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
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
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}

	// Word 출력 Data
	pGridCtrl = &m_GridWordOut;
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
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
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
}

CString CIoMonitorDlg::GetInWordName(int nRow)
{
	CString strRet ;

	switch (nRow) {
	case	en_InWord_RecipeNo :
		strRet = _T("Recipe No");
		break;
	case	en_InWord_RecipeName :
		strRet = _T("Recipe Name");
		break;
	case	en_InWord_LotId :
		strRet = _T("Lot ID");
		break;
	case	en_InWord_ContinuosCnt :
		strRet = _T("연속결함수");
		break;
	case	en_InWord_SectorNgCnt :
		strRet = _T("구간결함수");
		break;
	case	en_InWord_SectorBaseCnt :
		strRet = _T("구간크기");
		break;
	default :
		strRet = _T("");
		break;
	}

	return strRet;
}

CString CIoMonitorDlg::GetOutWordName(int nRow)
{
	CString strRet;

	switch (nRow) {
	case	en_OutWord_TabCount :
		strRet = _T("Total");
		break;
	case	en_OutWord_OkCount :
		strRet = _T("OK");
		break;
	case	en_OutWord_NgCount :
		strRet = _T("NG");
		break;
	case	en_OutWord_OkRate :
		strRet = _T("OK(%)");
		break;
	case	en_OutWord_NgRate :
		strRet = _T("NG(%)");
		break;
	case	en_OutWord_TopDefCount :
		strRet = _T("Top Count");
		break;
	case	en_OutWord_BtmDefCount :
		strRet = _T("Btm Count");
		break;
	case	en_OutWord_TopDefCount_LotEnd :
		strRet = _T("Top Count(LotEnd)");
		break;
	case	en_OutWord_BtmDefCount_LotEnd :
		strRet = _T("Top Count(LotEnd)");
		break;
	case	en_OutWord_Alarm :
		strRet = _T("Alarm");
		break;
	case	en_OutWord_Code :
		strRet = _T("Alarm Code");
		break;
	default :
		strRet = _T("");
		break;
	}
	
	return strRet;
}

// 22.08.16 Ahn Add Start
CString CIoMonitorDlg::GetInWordAddress(int nRow)
{
	CString strRet;
	// 23.03.03 Son Modify Start
	//switch (nRow) {
	//case	en_InWord_RecipeNo:
	//	strRet = _T("0x9680");
	//	break;
	//case	en_InWord_RecipeName:
	//	strRet = _T("0x9682");
	//	break;
	//case	en_InWord_LotId:
	//	strRet = _T("0x9690");
	//	break;
	//case	en_InWord_ContinuosCnt:
	//	strRet = _T("0x96C0");
	//	break;
	//case	en_InWord_SectorNgCnt:
	//	strRet = _T("0x96C2");
	//	break;
	//case	en_InWord_SectorBaseCnt:
	//	strRet = _T("0x96C4");
	//	break;
	//default:
	//	strRet = _T("");
	//	break;
	//}
	int nAddress = 0;// AprData.m_System.m_nWordIn;
	switch (nRow) {
	case	en_InWord_RecipeNo:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordRead_RecipeNo, MODE_READ);
		break;
	case	en_InWord_RecipeName:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordRead_RecipeName, MODE_READ);
		break;
	case	en_InWord_LotId:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordRead_CELL_ID, MODE_READ);
		break;
	case	en_InWord_ContinuosCnt:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordRead_PrmContinuousCnt, MODE_READ);
		break;
	case	en_InWord_SectorNgCnt:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordRead_PrmSectorNgTabCnt, MODE_READ);
		break;
	case	en_InWord_SectorBaseCnt:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordRead_PrmSectorBaseCnt, MODE_READ);
		break;
	default:
		nAddress = 0;
		break;
	}
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		strRet.Format(_T("0x%X"), nAddress);
	}
	else {
		strRet.Format(_T("%d"), nAddress);
	}
	// 23.03.03 Son Modify End
	return strRet;
}
CString CIoMonitorDlg::GetOutWordAddress(int nRow)
{
	CString strRet;
	// 23.03.03 Son Modify Start
	//switch (nRow) {
	//case	en_OutWord_TabCount:
	//	strRet = _T("0xC380");
	//	break;
	//case	en_OutWord_OkCount:
	//	strRet = _T("0xC382");
	//	break;
	//case	en_OutWord_NgCount:
	//	strRet = _T("0xC384");
	//	break;
	//case	en_OutWord_OkRate:
	//	strRet = _T("0xC386");
	//	break;
	//case	en_OutWord_NgRate:
	//	strRet = _T("0xC388");
	//	break;
	//case	en_OutWord_TopDefCount:
	//	strRet = _T("0xC39C");
	//	break;
	//case	en_OutWord_BtmDefCount:
	//	strRet = _T("0xC39E");
	//	break;
	//case	en_OutWord_TopDefCount_LotEnd:
	//	strRet = _T("0xC3A0");
	//	break;
	//case	en_OutWord_BtmDefCount_LotEnd:
	//	strRet = _T("0xC3A2");
	//	break;
	//case	en_OutWord_Alarm:
	//	strRet = _T("0xC3D0");
	//	break;
	//case	en_OutWord_Code:
	//	strRet = _T("0xC3D2");
	//	break;
	//default:
	//	strRet = _T("");
	//	break;
	//}
	int nAddress = 0; // AprData.m_System.m_nWordOut;
	switch (nRow) {
	case	en_OutWord_TabCount:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_DataReportV1_Ea, MODE_WRITE);
		break;
	case	en_OutWord_OkCount:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_DataReportV2_OK, MODE_WRITE);
		break;
	case	en_OutWord_NgCount:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_DataReportV3_NG, MODE_WRITE);
		break;
	case	en_OutWord_OkRate:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_DataReportV4_OkRate, MODE_WRITE);
		break;
	case	en_OutWord_NgRate:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_DataReportV5_NgRate, MODE_WRITE);
		break;
	case	en_OutWord_TopDefCount:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_Top_Defect_Count_Real, MODE_WRITE);
		break;
	case	en_OutWord_BtmDefCount:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_Btm_Defect_Count_Real, MODE_WRITE);
		break;
	case	en_OutWord_TopDefCount_LotEnd:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_Top_Defect_Count_LotEnd, MODE_WRITE);
		break;
	case	en_OutWord_BtmDefCount_LotEnd:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_Btm_Defect_Count_LotEnd, MODE_WRITE);
		break;
	case	en_OutWord_Alarm:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_AlarmExist, MODE_WRITE);
		break;
	case	en_OutWord_Code:
		nAddress = nAddress + CSigProc::GetWordAddress(CSigProc::enWordWrite_AlarmCode_Buffer1, MODE_WRITE);
		break;
	default:
		nAddress = 0;
		break;
	}
	if (AprData.m_System.m_nPlcMode == en_Plc_Melsec) {
		strRet.Format(_T("0x%X"), nAddress);
	}
	else {
		strRet.Format(_T("%d"), nAddress);
	}
	// 23.03.03 Son Modify End
	return strRet;
}

CString CIoMonitorDlg::GetInWordData(int nRow)
{
	CString strRet;
	switch (nRow) {
	case	en_InWord_RecipeNo:
		strRet.Format( _T("%d"), (int)m_localSeqDataIn.wRecipeNo );
		break;
	case	en_InWord_RecipeName:
		strRet.Format(_T("%s"), m_localSeqDataIn.strRecipeName );
		break;
	case	en_InWord_LotId:
		strRet.Format(_T("%s"), m_localSeqDataIn.strCell_ID);
		break;
	case	en_InWord_ContinuosCnt:
		strRet.Format(_T("%d"), (int)m_localSeqDataIn.wContinousCount );
		break;
	case	en_InWord_SectorNgCnt:
		strRet.Format(_T("%d"), (int)m_localSeqDataIn.wSectorNgCount);
		break;
	case	en_InWord_SectorBaseCnt:
		strRet.Format(_T("%d"), (int)m_localSeqDataIn.wSectorBaseCount);
		break;
	default:
		strRet = _T("");
		break;
	}
	return strRet;
}
CString CIoMonitorDlg::GetOutWordData(int nRow)
{
	CString strRet;
	_SEQ_OUT_DATA* pSeqOutData = &m_localSeqDataOut.m_SeqOutData;
	switch (nRow) {
	case	en_OutWord_TabCount:
		strRet.Format(_T("%d"), (int)pSeqOutData->dwDataReportV1 );
		break;
	case	en_OutWord_OkCount:
		strRet.Format(_T("%d"), (int)pSeqOutData->dwDataReportV2);
		break;
	case	en_OutWord_NgCount:
		strRet.Format(_T("%d"), (int)pSeqOutData->dwDataReportV3);
		break;
	case	en_OutWord_OkRate:
		strRet.Format(_T("%d"), (int)pSeqOutData->dwDataReportV4);
		break;
	case	en_OutWord_NgRate:
		strRet.Format(_T("%d"), (int)pSeqOutData->dwDataReportV5);
		break;
	case	en_OutWord_TopDefCount:
		strRet.Format(_T("%d"), (int)pSeqOutData->dwTopNgRealTimeCount);
		break;
	case	en_OutWord_BtmDefCount:
		strRet.Format(_T("%d"), (int)pSeqOutData->dwBottomNgRealTimeCount);
		break;
	case	en_OutWord_TopDefCount_LotEnd:
		strRet.Format(_T("%d"), (int)m_localSeqDataOut.dwTopNgLotEndCount);
		break;
	case	en_OutWord_BtmDefCount_LotEnd:
		strRet.Format(_T("%d"), (int)m_localSeqDataOut.dwBottomNgLotEndCount);
		break;
	case	en_OutWord_Alarm:
		strRet.Format(_T("%d"), (int)m_localSeqDataOut.dwAlarm);
		break;
	case	en_OutWord_Code:
		strRet.Format(_T("0x%x"), (int)m_localSeqDataOut.dwAlarmCode);
		break;
	default :
		break;
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
		AprData.m_ErrStatus.SetError(CErrorStatus::en_ContinuousNg, strMessage);
		CSigProc* pSigProc = theApp.m_pSigProc;
		pSigProc->WriteAlarmCode(wAlarmCode);
		pSigProc->SigOutAlarmExist(TRUE);

  		Sleep(200);
 		pSigProc->WriteAlarmCode(0x0000);
		pSigProc->SigOutAlarmExist(FALSE);


// 22.10.05 Ahn Add End
	}
}
// 22.10.05 Ahn Add End
