// DIOTestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "DIOTestDlg.h"
#include "afxdialogex.h"
#include "AppDIO.h"
#include "CCounterThread.h"

// CDIOTestDlg 대화 상자

#define	TM_SENSOR_ID		1000
#define	TM_SENSOR_INTERVAL	10
#define WM_DIO_INTERRUPT	1011	//Window message method (define to get interrupt message)

IMPLEMENT_DYNAMIC(CDIOTestDlg, CDialogEx)

CDIOTestDlg::CDIOTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DIO_TEST, pParent)
{
	m_hHandle = NULL;
	m_hInterrupt = NULL;
	m_bThread = FALSE;
	m_wLastInfo = 0x00;
	m_bChkTestRun = FALSE;
}

CDIOTestDlg::~CDIOTestDlg()
{
}

void CDIOTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_MESSAGE, m_radioMessage);
	DDX_Control(pDX, IDC_CHECK_RISING, m_chkRising);
	DDX_Control(pDX, IDC_LIST_Message, m_listMessage);
	DDX_Control(pDX, IDC_CHECK_FALLING, m_chkFalling);
	DDX_Control(pDX, IDC_CHECK_INTRRUPT, m_chkIntrrupt);
	DDX_Control(pDX, IDC_STATIC_LOW, m_stcLow);
	DDX_Control(pDX, IDC_STATIC_HIGH, m_stcHigh);
	DDX_Control(pDX, IDC_COMBO, m_cboModuleInfo);
	DDX_Check(pDX, IDC_CHK_FREERUN, m_bChkTestRun);
}


BEGIN_MESSAGE_MAP(CDIOTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	ON_BN_CLICKED(IDC_CHECK_INTRRUPT, OnCheckIntrrupt)
	ON_BN_CLICKED(IDC_CHECK_RISING, OnCheckRising)
	ON_BN_CLICKED(IDC_CHECK_FALLING, OnCheckFalling)
	ON_BN_CLICKED(IDC_LIST_CLEAR, OnListClear)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_MESSAGE, OnRadioMessage)
	ON_BN_CLICKED(IDC_RADIO_CALLBACK, OnRadioCallback)
	ON_BN_CLICKED(IDC_RADIO_EVENT, OnRadioEvent)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_COMMAND_RANGE(IDC_CHECK_HIGH_00, IDC_CHECK_HIGH_15, OnClickHigh)
	ON_COMMAND_RANGE(IDC_CHECK_LOW_00, IDC_CHECK_LOW_15, OnClickLow)
	ON_MESSAGE(WM_AXL_INTERRUPT, OnInterruptMessage)
	ON_BN_CLICKED(IDC_CHK_FREERUN, &CDIOTestDlg::OnBnClickedChkFreerun)
END_MESSAGE_MAP()


// CDIOTestDlg 메시지 처리기


BOOL CDIOTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i = 0; i < 16; i++)
	{
		m_pchkHigh[i] = (CButton*)GetDlgItem(IDC_CHECK_HIGH_00 + i);
		m_pchkLow[i] = (CButton*)GetDlgItem(IDC_CHECK_LOW_00 + i);
	}

//	if (AXTInit())
//	{
//		OnSelchangeCombo();
		SetTimer(TM_SENSOR_ID, TM_SENSOR_INTERVAL, NULL);
//	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDIOTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDIOTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDIOTestDlg::OnSelchangeCombo()
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{
		DWORD	dwStateLow;
		DWORD	dwStateHigh;
		long	IBoardNo;
		long	IModulePos;
		DWORD	dwModuleID;
		BOOL	bFlagHigh;
		BOOL	bFlagLow;
		DWORD	dwInterrupt;

		if (AxdInfoGetModule(m_cboModuleInfo.GetCurSel(), &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
		{

			/*
			1111 1111 1111 1111
			0000 0000 0000 0001
			--------------------
			0000 0000 0000 0001
			Check input area comparing with shift operation bit by bit(16bit)
			*/

			switch (dwModuleID)
			{
			case AXT_SIO_DI32:
			case AXT_SIO_RDI32:
			case AXT_SIO_RDI16MLII:
			case AXT_SIO_RDI32RTEX:
			case AXT_SIO_DI32_P:
				m_stcHigh.SetWindowText("INPUT 1");
				m_stcLow.SetWindowText("INPUT 2");
				if (dwModuleID == AXT_SIO_RDI32) {
					m_chkIntrrupt.EnableWindow(false);
					m_chkRising.EnableWindow(false);
					m_chkFalling.EnableWindow(false);
					GetDlgItem(IDC_RADIO_MESSAGE)->EnableWindow(false);
					GetDlgItem(IDC_RADIO_CALLBACK)->EnableWindow(false);
					GetDlgItem(IDC_RADIO_EVENT)->EnableWindow(false);
				}
				else {
					m_chkIntrrupt.EnableWindow(true);
					m_chkRising.EnableWindow(true);
					m_chkFalling.EnableWindow(true);

					GetDlgItem(IDC_RADIO_MESSAGE)->EnableWindow(true);
					GetDlgItem(IDC_RADIO_CALLBACK)->EnableWindow(true);
					GetDlgItem(IDC_RADIO_EVENT)->EnableWindow(true);

					AxdiInterruptGetModuleEnable(m_cboModuleInfo.GetCurSel(), &dwInterrupt);
					if (dwInterrupt == ENABLE)
					{
						m_chkIntrrupt.SetCheck(1);
					}
					else
					{
						m_chkIntrrupt.SetCheck(0);
					}

					AxdiInterruptEdgeGetWord(m_cboModuleInfo.GetCurSel(), 0, UP_EDGE, &dwStateHigh);
					if (dwStateHigh == 0xFFFF)
						m_chkRising.SetCheck(1);
					else
						m_chkRising.SetCheck(0);

					AxdiInterruptEdgeGetWord(m_cboModuleInfo.GetCurSel(), 0, DOWN_EDGE, &dwStateHigh);
					if (dwStateHigh == 0xFFFF)
						m_chkFalling.SetCheck(1);
					else
						m_chkFalling.SetCheck(0);
				}
				break;
			case AXT_SIO_DO32P:
			case AXT_SIO_DO32T:
			case AXT_SIO_RDO32:
			case AXT_SIO_RDO16AMLII:
			case AXT_SIO_RDO16BMLII:
			case AXT_SIO_RDO32RTEX:
			case AXT_SIO_DO32T_P:

			{
				m_stcHigh.SetWindowText("OUTPUT 1");
				m_stcLow.SetWindowText("OUTPUT 2");

				m_chkIntrrupt.EnableWindow(false);
				m_chkRising.EnableWindow(false);
				m_chkFalling.EnableWindow(false);
				GetDlgItem(IDC_RADIO_MESSAGE)->EnableWindow(false);
				GetDlgItem(IDC_RADIO_CALLBACK)->EnableWindow(false);
				GetDlgItem(IDC_RADIO_EVENT)->EnableWindow(false);

				AxdoReadOutportWord(m_cboModuleInfo.GetCurSel(), 0, &dwStateHigh);
				AxdoReadOutportWord(m_cboModuleInfo.GetCurSel(), 1, &dwStateLow);

				for (int nIndex = 0; nIndex < 16; nIndex++)
				{
					// Verify the last bit value of data read
					bFlagHigh = dwStateHigh & 0x0001;
					bFlagLow = dwStateLow & 0x0001;

					// Shift rightward by bit by bit
					dwStateHigh = dwStateHigh >> 1;
					dwStateLow = dwStateLow >> 1;

					// Update bit value in control
					m_pchkHigh[nIndex]->SetCheck(bFlagHigh);
					m_pchkLow[nIndex]->SetCheck(bFlagLow);
				}
			}
			break;
			case AXT_SIO_DB32P:
			case AXT_SIO_DB32T:
			case AXT_SIO_RDB32RTEX:
			case AXT_SIO_RDB32T:
			case AXT_SIO_RDB96MLII:
			case AXT_SIO_RDB128MLII:
			case AXT_SIO_RSIMPLEIOMLII:
				m_stcHigh.SetWindowText("INPUT 1");
				m_stcLow.SetWindowText("OUTPUT 1");

				m_chkIntrrupt.EnableWindow(true);
				m_chkRising.EnableWindow(true);
				m_chkFalling.EnableWindow(true);

				GetDlgItem(IDC_RADIO_MESSAGE)->EnableWindow(true);
				GetDlgItem(IDC_RADIO_CALLBACK)->EnableWindow(true);
				GetDlgItem(IDC_RADIO_EVENT)->EnableWindow(true);

				AxdiInterruptGetModuleEnable(m_cboModuleInfo.GetCurSel(), &dwInterrupt);
				if (dwInterrupt == ENABLE)
				{
					m_chkIntrrupt.SetCheck(1);
				}
				else
				{
					m_chkIntrrupt.SetCheck(0);
				}

				AxdiInterruptEdgeGetWord(m_cboModuleInfo.GetCurSel(), 0, UP_EDGE, &dwStateHigh);
				if (dwStateHigh == 0xFFFF)
					m_chkRising.SetCheck(1);
				else
					m_chkRising.SetCheck(0);

				AxdiInterruptEdgeGetWord(m_cboModuleInfo.GetCurSel(), 0, DOWN_EDGE, &dwStateHigh);
				if (dwStateHigh == 0xFFFF)
					m_chkFalling.SetCheck(1);
				else
					m_chkFalling.SetCheck(0);

				AxdiReadInportWord(m_cboModuleInfo.GetCurSel(), 0, &dwStateHigh);
				AxdoReadOutportWord(m_cboModuleInfo.GetCurSel(), 0, &dwStateLow);

				for (int i = 0; i < 16; i++)
				{
					bFlagHigh = dwStateHigh & 0x0001;
					bFlagLow = dwStateLow & 0x0001;

					dwStateHigh = dwStateHigh >> 1;
					dwStateLow = dwStateLow >> 1;

					m_pchkHigh[i]->SetCheck(bFlagHigh);
					m_pchkLow[i]->SetCheck(bFlagLow);

				}
				break;
			}
		}
	}
}

BOOL CDIOTestDlg::AXTInit()
{
	// Initialize library 
	DWORD Code = AxlOpenNoReset(7);
	if (Code == AXT_RT_SUCCESS)
	{
		TRACE("Library is initialized .\n");
		DWORD dwStatus;

		// Inspect if DIO module exsits
		Code = AxdInfoIsDIOModule(&dwStatus);
		if (dwStatus == STATUS_EXIST)
		{
			TRACE("DIO module exists.\n");
			long IModuleCounts;
			Code = AxdInfoGetModuleCount(&IModuleCounts);
			if (Code == AXT_RT_SUCCESS)
				TRACE("Number of DIO module: %d\n", IModuleCounts);
			else
				TRACE("AxdInfoGetModuleCount() : ERROR code Ox%x\n", Code);

			long IInputCounts;
			long IOutputCounts;

			long IBoardNo;
			long IModulePos;
			DWORD dwModuleID;

			CString strData;
			for (int ModuleNo = 0; ModuleNo < IModuleCounts; ModuleNo++)
			{
				// Grasp IO channel
				AxdInfoGetInputCount(ModuleNo, &IInputCounts);
				AxdInfoGetOutputCount(ModuleNo, &IOutputCounts);

				if (AxdInfoGetModule(ModuleNo, &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
				{
					switch (dwModuleID)
					{
					case AXT_SIO_DI32:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_DI32"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDI32:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDI32"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_DO32P:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_DO32P"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_DB32P:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_DB32P"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_DO32T:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_DO32T"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDO32:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDO32"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_DB32T:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_DB32T"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDB128MLII:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDB128MLII"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RSIMPLEIOMLII:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RSIMPLEIOMLII"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDI16MLII:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDI16MLII"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDO16AMLII:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDO16AMLII"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDO16BMLII:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDO16BMLII"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDB96MLII:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDB96MLII"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDO32RTEX:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDO32RTEX"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDI32RTEX:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDI32RTEX"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDB32RTEX:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDB32RTEX"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_DI32_P:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_DI32_P"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_DO32T_P:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_DO32T_P"), IBoardNo, ModuleNo);
						break;
					case AXT_SIO_RDB32T:
						strData.Format(_T("[BD No:%d - MD No:%d] SIO_RDB32T"), IBoardNo, ModuleNo);
						break;

					default:
						strData.Format(_T("[BD No:%d - MD No:%d] Unknown"), IBoardNo, ModuleNo);
						break;
					}
					m_cboModuleInfo.AddString(strData);
				}
				m_cboModuleInfo.SetCurSel(0);
			}
		}
		else
		{
			AfxMessageBox(_T("Module not exist."));
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox(_T("Failed initialization."));
		return FALSE;
	}
	return TRUE;
}

void CDIOTestDlg::OnListClear()
{
	m_listMessage.ResetContent();
}

void CDIOTestDlg::OnDestroy()
{
	CDialog::OnDestroy();

	//Exit timer
	KillTimer(0);
	// Exit library and motion
	CloseDevice();
}

void CDIOTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	CAppDIO		dio;
	short		nIndex;
//	DWORD		dwDataHigh;
//	DWORD		dwDataLow;
	BOOL		bFlagHigh=0;
	BOOL		bFlagLow=0;
	long		lBoardNo;
	long		lModulePos;
	DWORD		dwModuleID;

	//OnTimer 로그출력
	LOGDISPLAY_SPEC(6)("CDIOTestDlg::OnTimer => %s", m_bChkTestRun ? "DIO TestRun" : "DIO None");

	if( m_bChkTestRun ) {
		BOOL bTriggerBit;
		dio.InputBit(CAppDIO::eIn_TRIGGER, &bTriggerBit);

		if (bTriggerBit == TRUE) {
			WORD wInSignal = 0x00;
			// 입력 
			dio.InputWord(&wInSignal);

			if (wInSignal != m_wLastInfo) { // 같은 Data 이면 Pass
				WORD wID = CCounterThread::GetTabId_FromSignal(wInSignal);
				//WORD wOutSignal = CCounterThread::GetCounterSignal(wID, 0, 0, 0, 0, 0, 0);

				// 출력
				//dio.OutputWord(wOutSignal);
				m_wLastInfo = wInSignal;
				::Sleep(5);
			}
		}

	}

	switch (nIDEvent)
	{
	case TM_SENSOR_ID:
		AxdInfoGetModule(/*m_cboModuleInfo.GetCurSel()*/0, &lBoardNo, &lModulePos, &dwModuleID);

		switch (dwModuleID)
		{
		case AXT_SIO_DI32:
		case AXT_SIO_RDI32:
		case AXT_SIO_RDI16MLII:
		case AXT_SIO_RDI32RTEX:
		case AXT_SIO_DI32_P:

			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CDIOTestDlg::OnTimer => ModuleID<AXT_SIO_DI32><AXT_SIO_RDI32><AXT_SIO_RDI16MLII><AXT_SIO_RDI32RTEX><AXT_SIO_DI32_P>");

			//++
			// Read inputting signal in WORD
			//AxdiReadInportWord(/*m_cboModuleInfo.GetCurSel()*/0, 0, &dwDataHigh);
			//AxdiReadInportWord(/*m_cboModuleInfo.GetCurSel()*/0, 1, &dwDataLow);
			//
			//for (nIndex = 0; nIndex < 16; nIndex++)
			//{
			//	// Verify the last bit value of data read
			//	bFlagHigh = dwDataHigh & 0x0001;
			//	bFlagLow = dwDataLow & 0x0001;
			//
			//	// Shift rightward by bit by bit
			//	dwDataHigh = dwDataHigh >> 1;
			//	dwDataLow = dwDataLow >> 1;
			//
			//	// Updat bit value in control
			//	m_pchkHigh[nIndex]->SetCheck(bFlagHigh);
			//	m_pchkLow[nIndex]->SetCheck(bFlagLow);
			//}
			for (nIndex = 0; nIndex < 16; nIndex++)
			{
				dio.InputBit(GetBitPat(nIndex), &bFlagHigh);
				m_pchkHigh[nIndex]->SetCheck(bFlagHigh);
			}
			for (nIndex = 16; nIndex < 32; nIndex++)
			{
				dio.InputBit(GetBitPat(nIndex), &bFlagLow);
				m_pchkHigh[nIndex]->SetCheck(bFlagLow);
			}
			break;

		case AXT_SIO_DB32P:
		case AXT_SIO_DB32T:
		case AXT_SIO_RDB32RTEX:
		case AXT_SIO_RDB32T:
		case AXT_SIO_RDB96MLII:
		case AXT_SIO_RDB128MLII:
		case AXT_SIO_RSIMPLEIOMLII:

			//OnTimer 로그출력
			LOGDISPLAY_SPEC(6)("CDIOTestDlg::OnTimer => ModuleID<AXT_SIO_DB32P><AXT_SIO_DB32T><AXT_SIO_RDB32RTEX><AXT_SIO_RDB32T><AXT_SIO_RDB96MLII><AXT_SIO_RDB128MLII><AXT_SIO_RSIMPLEIOMLII>");

			//++
			// Read inputting signal in WORD
			//AxdiReadInportWord(m_cboModuleInfo.GetCurSel(), 0, &dwDataHigh);
			//
			//for (nIndex = 0; nIndex < 16; nIndex++)
			//{
			//	// Verify the last bit value of data read
			//	bFlagHigh = dwDataHigh & 0x0001;
			//
			//	// Shift rightward by bit by bit
			//	dwDataHigh = dwDataHigh >> 1;
			//
			//	// Updat bit value in control
			//	m_pchkHigh[nIndex]->SetCheck(bFlagHigh);
			//}
			for (nIndex = 0; nIndex < 16; nIndex++)
			{
				dio.InputBit(GetBitPat(nIndex), &bFlagHigh);
				m_pchkHigh[nIndex]->SetCheck(bFlagHigh);
			}
			break;
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CDIOTestDlg::OnClickHigh(UINT nID)
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{
		long	IBoardNo;
		long	IModulePos;
		DWORD	dwModuleID;
		CString strMessage;
		short	nIndex;
		BOOL    bState;
		CAppDIO dio;

		if (AxdInfoGetModule(m_cboModuleInfo.GetCurSel(), &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
		{
			switch (dwModuleID)
			{
			case AXT_SIO_DO32P:
			case AXT_SIO_DO32T:
			case AXT_SIO_RDO32:
				//nIndex = nID - IDC_CHECK_HIGH_00;
				//bState = m_pchkHigh[nIndex]->GetCheck();
				//AxdoWriteOutportBit(m_cboModuleInfo.GetCurSel(), nIndex, bState);
				{
					nIndex = nID - IDC_CHECK_LOW_00;
					bState = m_pchkLow[nIndex]->GetCheck();
					dio.OutputBit(GetBitPat(nIndex), bState);
				}
				break;
			}
		}
	}
}

void CDIOTestDlg::OnClickLow(UINT nID)
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{
		long	IBoardNo;
		long	IModulePos;
		DWORD	dwModuleID;
		CString strMessage;
		short	nIndex;
		BOOL    bState;
		CAppDIO dio;

		if (AxdInfoGetModule(/*m_cboModuleInfo.GetCurSel()*/0, &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
		{
			switch (dwModuleID)
			{
			case AXT_SIO_DB32P:
			case AXT_SIO_DB32T:
			case AXT_SIO_RDB128MLII:
				//nIndex = nID - IDC_CHECK_LOW_00;
				//bState = m_pchkLow[nIndex]->GetCheck();
				//AxdoWriteOutportBit(m_cboModuleInfo.GetCurSel(), nIndex, bState);
				{
					nIndex = nID - IDC_CHECK_LOW_00;
					bState = m_pchkLow[nIndex]->GetCheck();
					dio.OutputBit(GetBitPat(nIndex), bState);
				}
				break;
			case AXT_SIO_DO32P:
			case AXT_SIO_DO32T:
			case AXT_SIO_RDO32:
				//nIndex = nID - IDC_CHECK_LOW_00;
				//bState = m_pchkLow[nIndex]->GetCheck();
				//AxdoWriteOutportBit(m_cboModuleInfo.GetCurSel(), nIndex + 16, bState);
				{
					nIndex = nID - IDC_CHECK_LOW_00 + 16;
					bState = m_pchkLow[nIndex]->GetCheck();
					dio.OutputBit(GetBitPat(nIndex), bState);
				}
				break;
			}
		}
	}
}

DWORD CDIOTestDlg::GetBitPat(short nIndex) 
{
	switch (nIndex)
	{
	case 0: return(CAppDIO::eOut_TAB_JUDGE_SURFACE); break;
	case 1: return(CAppDIO::eOut_TAB_JUDGE_SIZE); break;
	case 2: return(CAppDIO::eOut_BCD_ID_01); break;
	case 3: return(CAppDIO::eOut_BCD_ID_02); break;
	case 4: return(CAppDIO::eOut_BCD_ID_04); break;
	case 5: return(CAppDIO::eOut_BCD_ID_08); break;
	case 6: return(CAppDIO::eOut_BCD_ID_16); break;
	case 7: return(CAppDIO::eOut_BCD_ID_32); break;
	case 8: return(CAppDIO::eOut_08); break;
	case 9: return(CAppDIO::eOut_PULSE); break;
	case 10: return(CAppDIO::eOut_MARK_SEL_01); break;
	case 11: return(CAppDIO::eOut_MARK_SEL_02); break;
	case 12: return(CAppDIO::eOut_12); break;
	case 13: return(CAppDIO::eOut_13); break;
	case 14: return(CAppDIO::eOut_14); break;
	case 15: return(CAppDIO::eOut_15); break;
	default: return 0; break;
	}
	return 0;
}

BOOL CDIOTestDlg::OnModuleInfo(long lModuleNo, short* InputCount)
{
	long	IBoardNo;
	long	IModulePos;
	DWORD	dwModuleID;

	if (AxdInfoGetModule(lModuleNo, &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
	{
		switch (dwModuleID)
		{
		case AXT_SIO_DI32:
		case AXT_SIO_RDI32:
		case AXT_SIO_RDI32RTEX:
		case AXT_SIO_DI32_P:
			*InputCount = 32;
			break;
		case AXT_SIO_DB32P:
		case AXT_SIO_DB32T:
		case AXT_SIO_RDB32RTEX:
		case AXT_SIO_RDB32T:
		case AXT_SIO_RDB128MLII:
		case AXT_SIO_RSIMPLEIOMLII:
		case AXT_SIO_RDI16MLII:
			*InputCount = 16;
			break;
		case AXT_SIO_RDB96MLII:
			*InputCount = 96;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDIOTestDlg::CloseDevice()
{
	// Verify if the integrated library is available to use (If it is initialized)  
	if (AxlIsOpened())
	{
		AxlClose();

		return	TRUE;
	}
	else
	{
		return	FALSE;
	}
}

void CDIOTestDlg::OnButtonExit()
{
	CloseDevice();

	OnOK();
}

void CDIOTestDlg::OnCheckIntrrupt()
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{
		long	IBoardNo;
		long	IModulePos;
		DWORD	dwModuleID;
		CString strMessage;

		if (AxdInfoGetModule(m_cboModuleInfo.GetCurSel(), &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
		{
			switch (dwModuleID)
			{
			case AXT_SIO_DI32:
			case AXT_SIO_RDI32:
			case AXT_SIO_DB32P:
			case AXT_SIO_DB32T:
				if (m_chkIntrrupt.GetCheck())
				{
					AxlInterruptEnable();
					strMessage.Format(_T("Enable Interrupt Button Enable"));
					AxdiInterruptSetModuleEnable(m_cboModuleInfo.GetCurSel(), ENABLE);
				}
				else
				{
					//Initialize interrupt part
					strMessage.Format(_T("Enable Interrupt Button Desable"));
					AxdiInterruptEdgeSetDword(m_cboModuleInfo.GetCurSel(), 0, UP_EDGE, 0x00000000);
					AxdiInterruptEdgeSetDword(m_cboModuleInfo.GetCurSel(), 0, DOWN_EDGE, 0x00000000);

					m_chkRising.SetCheck(0);
					m_chkFalling.SetCheck(0);
					m_radioMessage.SetCheck(0);

					AxdiInterruptSetModuleEnable(m_cboModuleInfo.GetCurSel(), DISABLE);
					//Initialize interrupt event (3 ways: callback, window message, event)
					AxdiInterruptSetModule(m_cboModuleInfo.GetCurSel(), m_hWnd, NULL, NULL, NULL);
					AxlInterruptDisable();

				}
				break;
			default:
				strMessage.Format(_T("It is the module not to able to use Enable Interrupt."));
				m_chkIntrrupt.SetCheck(0);
				break;
			}
			m_listMessage.AddString(strMessage);

		}
	}
}

void CDIOTestDlg::OnCheckRising()
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{

		long	IBoardNo;
		long	IModulePos;
		DWORD	dwModuleID;
		CString strMessage;

		if (m_chkIntrrupt.GetCheck())
		{
			if (AxdInfoGetModule(m_cboModuleInfo.GetCurSel(), &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
			{
				switch (dwModuleID)
				{
				case AXT_SIO_DI32:
				case AXT_SIO_RDI32:
					if (m_chkRising.GetCheck())
					{
						strMessage.Format(_T("▲ Rising Edge Button Ensable"));
						AxdiInterruptEdgeSetDword(m_cboModuleInfo.GetCurSel(), 0, UP_EDGE, 0xFFFFFFFF);
					}
					else
					{
						strMessage.Format(_T("▲ Rising Edge Button Desable"));
						AxdiInterruptEdgeSetDword(m_cboModuleInfo.GetCurSel(), 0, UP_EDGE, 0x00000000);
					}
					break;
				case AXT_SIO_DB32P:
				case AXT_SIO_DB32T:
					if (m_chkRising.GetCheck())
					{
						strMessage.Format(_T("▲ Rising Edge Button Ensable"));
						AxdiInterruptEdgeSetWord(m_cboModuleInfo.GetCurSel(), 0, UP_EDGE, 0xFFFF);
					}
					else
					{
						strMessage.Format(_T("▲ Rising Edge Button Desable"));
						AxdiInterruptEdgeSetWord(m_cboModuleInfo.GetCurSel(), 0, UP_EDGE, 0x0000);
					}
					break;
				default:
					strMessage.Format(_T("It is the module not to able to use ▲ Rising Edge."));
					m_chkRising.SetCheck(0);
					break;
				}

			}
		}
		else
		{
			strMessage.Format(_T("Enable Interrupt is not to be selected."));
			m_chkRising.SetCheck(0);
		}
		m_listMessage.AddString(strMessage);
	}
}

void CDIOTestDlg::OnCheckFalling()
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{
		long	IBoardNo;
		long	IModulePos;
		DWORD	dwModuleID;
		CString strMessage;

		if (m_chkIntrrupt.GetCheck())
		{
			if (AxdInfoGetModule(m_cboModuleInfo.GetCurSel(), &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
			{
				switch (dwModuleID)
				{
				case AXT_SIO_DI32:
				case AXT_SIO_RDI32:
					if (m_chkFalling.GetCheck())
					{
						strMessage.Format(_T("▼ Falling Edge Button Ensable"));
						AxdiInterruptEdgeSetDword(m_cboModuleInfo.GetCurSel(), 0, DOWN_EDGE, 0xFFFFFFFF);
					}
					else
					{
						strMessage.Format(_T("▼ Falling Edge Button Desable"));
						AxdiInterruptEdgeSetDword(m_cboModuleInfo.GetCurSel(), 0, DOWN_EDGE, 0x00000000);
					}
					break;
				case AXT_SIO_DB32P:
				case AXT_SIO_DB32T:
					if (m_chkFalling.GetCheck())
					{
						strMessage.Format(_T("▼ Falling Edge Button Ensable"));
						AxdiInterruptEdgeSetWord(m_cboModuleInfo.GetCurSel(), 0, DOWN_EDGE, 0xFFFF);
					}
					else
					{
						strMessage.Format(_T("▼ Falling Edge Button Desable"));
						AxdiInterruptEdgeSetWord(m_cboModuleInfo.GetCurSel(), 0, DOWN_EDGE, 0x0000);
					}
					break;
				default:
					strMessage.Format(_T("It is the module not to able to use ▼ Falling Edge."));
					m_chkFalling.SetCheck(0);
					break;
				}

			}
		}
		else
		{
			strMessage.Format(_T("Enable Interrupt is not to be selected."));
			m_chkFalling.SetCheck(0);
		}
		m_listMessage.AddString(strMessage);
	}
}

void CDIOTestDlg::OnRadioMessage()
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{

		CString strMessage;

		if (m_chkIntrrupt.GetCheck())
		{
			//Initialize interrupt method
			AxdiInterruptSetModule(m_cboModuleInfo.GetCurSel(), m_hWnd, NULL, NULL, NULL);

			//Exit thread
			if (m_hHandle)
			{
				TerminateThread(m_hHandle, 0);
				WaitForSingleObject(m_hHandle, WAIT_ABANDONED);
				CloseHandle(m_hHandle);
				m_bThread = NULL;
				m_hHandle = NULL;
			}

			AxdiInterruptSetModule(m_cboModuleInfo.GetCurSel(), m_hWnd, WM_AXL_INTERRUPT, NULL, NULL);

			strMessage.Format(_T("Window Message Interrupt is selected."));

		}

		else
		{
			strMessage.Format(_T("Enable Interrupt is not to be selected."));
			m_radioMessage.SetCheck(0);

		}

		m_listMessage.AddString(strMessage);
	}
}

LRESULT CDIOTestDlg::OnInterruptMessage(WPARAM wParam, LPARAM lParam)
{
	short	InputCount = 0;
	DWORD	dwValue;

	int		nBoard = HIBYTE(wParam);	// Board number of interrupt occurrence (0,1….)
	int		nModule = LOBYTE(wParam);	// Module number of interrupt occurrence (0,1,2,3)
	int		nIntrFlag = (int)lParam;		// Flag of interrupt occurrence ( verify interrupt occurrence at input port 1,2,4,8,16..)

	CString strMessage;

	if (OnModuleInfo(nModule, &InputCount))
	{
		for (int ChkBit = 0; ChkBit < InputCount; ChkBit++)
		{
			if ((nIntrFlag >> ChkBit) & 0x01)
			{
				AxdiReadInportBit(nModule, ChkBit, &dwValue);
				if (dwValue)
					strMessage.Format(_T("▲ Rising %02Xh"), ChkBit);
				else
					strMessage.Format(_T("▼ Falling %02Xh"), ChkBit);
			}
		}
		m_listMessage.AddString(strMessage);
	}
	return 0;
}

void __stdcall OnDIOInterruptCallback(long lActiveNo, DWORD uFlag)
{
	short	InputCount = 0;
	DWORD	dwValue;
	CString strMessage;

	CDIOTestDlg* pDlg = (CDIOTestDlg*)AfxGetApp()->GetMainWnd();

	if (pDlg->OnModuleInfo(lActiveNo, &InputCount))
	{
		for (int ChkBit = 0; ChkBit < InputCount; ChkBit++)
		{
			if ((uFlag >> ChkBit) & 0x01)
			{
				AxdiReadInportBit(lActiveNo, ChkBit, &dwValue);
				if (dwValue)
					strMessage.Format(_T("▲ Rising %02Xh"), ChkBit);
				else
					strMessage.Format(_T("▼ Falling %02Xh"), ChkBit);
			}
		}
		//pDlg->m_listMessage.AddString(strMessage);
	}
	return;
}


void CDIOTestDlg::OnRadioCallback()
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{
		CString strMessage;

		if (m_chkIntrrupt.GetCheck())
		{
			//Initialize interrupt method
			AxdiInterruptSetModule(m_cboModuleInfo.GetCurSel(), m_hWnd, NULL, NULL, NULL);

			if (m_hHandle)
			{
				TerminateThread(m_hHandle, 0);
				WaitForSingleObject(m_hHandle, WAIT_ABANDONED);
				CloseHandle(m_hHandle);
				m_bThread = NULL;
				m_hHandle = NULL;
			}

			AxdiInterruptSetModule(m_cboModuleInfo.GetCurSel(), NULL, NULL, (AXT_INTERRUPT_PROC)OnDIOInterruptCallback, NULL);

			strMessage.Format(_T("Callback Intrrupt is selected."));
		}

		else
		{
			strMessage.Format(_T("Enable Intrrupt is selected."));
			m_radioMessage.SetCheck(0);
		}

		m_listMessage.AddString(strMessage);
	}
}

BOOL ThreadProc(LPVOID lpData)
{
	CDIOTestDlg* pDlg = (CDIOTestDlg*)lpData;

	pDlg->ThreadData();

	return TRUE;
}

void CDIOTestDlg::OnRadioEvent()
{
	long IModuleCounts;
	AxdInfoGetModuleCount(&IModuleCounts);

	if (IModuleCounts > 0)
	{
		CString strMessage;

		if (m_chkIntrrupt.GetCheck())
		{
			//Initialize interrupt method
			AxdiInterruptSetModule(m_cboModuleInfo.GetCurSel(), m_hWnd, NULL, NULL, NULL);

			AxdiInterruptSetModule(m_cboModuleInfo.GetCurSel(), NULL, NULL, NULL, &m_hInterrupt);

			if (m_hHandle)
			{
				TerminateThread(m_hHandle, 0);
				WaitForSingleObject(m_hHandle, WAIT_ABANDONED);
				CloseHandle(m_hHandle);
				m_bThread = NULL;
				m_hHandle = NULL;
			}

			if (!m_bThread)
			{
				m_bThread = TRUE;
				m_hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, NULL, NULL);
			}

			strMessage.Format(_T("Event Intrrupt is selected."));
		}

		else
		{
			strMessage.Format(_T("Enable Intrrupt is selected."));
			m_radioMessage.SetCheck(0);
		}

		m_listMessage.AddString(strMessage);
	}
}

BOOL CDIOTestDlg::ThreadData()
{
	while (m_bThread)
	{
		if (WaitForSingleObject(m_hInterrupt, INFINITE) == WAIT_OBJECT_0)
		{
			long IModuleCounts;
			AxdInfoGetModuleCount(&IModuleCounts);

			if (IModuleCounts > 0)
			{
				CString strMessage;

				if (m_chkIntrrupt.GetCheck())
				{
					long	lModuleNo;
					DWORD	dwFlag;
					CString	strData;
					CString	strInt;
					DWORD	dwValue;
					short	InputCount = 0;

					//Verify interrupt occurrence position
					AxdiInterruptRead(&lModuleNo, &dwFlag);
					//TRACE("\nInterrupt => %d %08X\n", lModuleNo, dwFlag);

					if (OnModuleInfo(lModuleNo, &InputCount))
					{
						for (int ChkBit = 0; ChkBit < InputCount; ChkBit++)
						{
							if ((dwFlag >> ChkBit) & 0x01)
							{
								AxdiReadInportBit(lModuleNo, ChkBit, &dwValue);
								if (dwValue)
									strMessage.Format(_T("▲ Rising %02Xh"), ChkBit);
								else
									strMessage.Format(_T("▼ Falling %02Xh"), ChkBit);
							}
						}
						m_listMessage.AddString(strMessage);
					}
				}
			}
		}
	}

	return TRUE;
}


void CDIOTestDlg::OnBnClickedChkFreerun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

//	theApp.m_pImgProcCtrl->SetImgDispHWnd();
	UpdateData(TRUE);
}
