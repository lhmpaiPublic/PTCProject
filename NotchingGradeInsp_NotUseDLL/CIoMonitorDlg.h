#pragma once


// CIoMonitorDlg 대화 상자

#include "ColorControl.h"
#include "SigProc.h"
#include "GlobalData.h"

class CModeDlg;

class CIoMonitorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIoMonitorDlg)

public:
	CIoMonitorDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CIoMonitorDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_IO_MONITOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected :
	UINT_PTR m_TID_CheckIO ;
	void SetCheckIoTimer() ;
	BOOL KillCheckIoTimer() ;

	enum {
		en_mode_on = 0,
		en_mode_off = 1,
		en_mode_max = 2,
	};

	enum {
		en_In_Alive				= 0,
		en_In_Ready				= 1,
		en_In_Run				= 2,
		en_In_CntReset			= 3,
		en_In_InkMarkActive		= 4, 
		en_In_RcpChange			= 5, 
		en_In_LotStart			= 6,
		en_In_LotEnd			= 7,
		en_In_Max				= 8,

		en_Out_Alive			= 0,
		en_Out_Ready			= 1,
		en_Out_CntResetAck		= 2,
		en_Out_DiskSpaceAlarm	= 3,
		en_Out_DiskSpaceWarning = 4,
		en_Out_RcpChangeAck		= 5,
		en_Out_LotStartAck		= 6,
		en_Out_LotEndAck		= 7,
		en_Out_Max				= 8,
	};

	// 22.08.16 Ahn Add Start
	CString GetInWordName(int nRow);
	CString GetOutWordName(int nRow);

	CString GetInWordAddress(int nRow);
	CString GetOutWordAddress(int nRow);

	CString GetInWordData(int nRow);
	CString GetOutWordData(int nRow);
	// 22.08.16 Ahn Add Start

	CBmpStatic m_bmpSigIn[en_In_Max][en_mode_max];
	CBmpStatic m_bmpSigOut[en_Out_Max][en_mode_max];

	BOOL m_bSigBitIn[MAX_ADR_BIT_IN];
	BOOL m_bSigBitOut[MAX_ADR_BIT_OUT];

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();

	enum {
		en_col_Name = 0,
		en_col_Address = 1,
		en_col_Data = 2,
		en_col_Max  =  3, 
	};
	enum {
		en_InWord_RecipeNo		= 0,
		en_InWord_RecipeName	= 1, 
		en_InWord_LotId			= 2, 
		en_InWord_ContinuosCnt	= 3,
		en_InWord_SectorNgCnt	= 4,
		en_InWord_SectorBaseCnt = 5,
		en_InWord_Max			= 6, 
	};

	enum {
		en_OutWord_TabCount				= 0,
		en_OutWord_OkCount				= 1,
		en_OutWord_NgCount				= 2,
		en_OutWord_OkRate				= 3, 
		en_OutWord_NgRate				= 4, 
		en_OutWord_TopDefCount			= 5,
		en_OutWord_BtmDefCount			= 6,
		en_OutWord_TopDefCount_LotEnd	= 7,
		en_OutWord_BtmDefCount_LotEnd	= 8, 
		en_OutWord_Alarm				= 9,
		en_OutWord_Code					= 10,
		en_OutWord_Max					= 11,
	};

private :
	int RefreshAll();
	void MakeGridCtrl();
	void UpdateGridCtrl();

	// 23.03.03 Ahn Add Start
	int GetAddressAndBitPos( int nMode, int nSignal, int &nBitPos );
	// 23.03.03 Ahn Add End

	CModeDlg* m_pParent;
	CSequenceData m_SeqData;

	CGridCtrl	m_GridWordIn ;
	CGridCtrl	m_GridWordOut;
	CFont*		m_pFontGrid;
	
	// 23.02.03 Ahn Delete Start
	//WORD	m_wInWordData[CSigProc::enWordReadMaxSize];
	//WORD	m_wOutWordData[CSigProc::enWordWroteMaxSize];
	// 23.02.03 Ahn Delete End

	CSequenceData m_localSeqDataIn;
	// 22.08.19 Ahn Add Start
	//_SEQ_OUT_DATA m_localSeqDataOut;
	_SEQ_OUT_DATA_ALL m_localSeqDataOut;
	// 22.08.19 Ahn Add End

	_SEQ_OUT_DATA_ALL_SMS m_localSeqDataOutSms;


#if defined(_DEBUG)
	void DebugLedChange();
#endif
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnDataLoad();
	CString m_strLotID;
	CString m_strRecipeName;
	CString m_strAlarm;
	CString m_strAlarmBit;
	// 22.10.05 Ahn Add Start
	afx_msg void OnBnClickedBtnDummyError();
	// 22.10.05 Ahn Add End
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDblclkStReadyOut();
	afx_msg void OnDblclkStReadyOutOff();
	afx_msg void OnDblclkStAlivePulseOut();
	afx_msg void OnDblclkStAlivePulseOutOff();
	afx_msg void OnDblclkStCntResetAck();
	afx_msg void OnDblclkStCntResetAckOff();
	afx_msg void OnDblclkStDiskSpaceAlarm();
	afx_msg void OnDblclkStDiskSpaceAlarmOff();
	afx_msg void OnDblclkStDiskSpaceError();
	afx_msg void OnDblclkStDiskSpaceErrorOff();
	afx_msg void OnDblclkStRadRecipeChangeAck();
	afx_msg void OnDblclkStRadRecipeChangeAckOff();
	afx_msg void OnDblclkStLotStartAck();
	afx_msg void OnDblclkStLotStartAckOff();
	afx_msg void OnDblclkStLotEndAck();
	afx_msg void OnDblclkStLotEndAckOff();
	afx_msg void OnBnClickedBtnDummyErrorClear();
};
