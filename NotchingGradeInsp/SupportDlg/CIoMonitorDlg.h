#pragma once


// CIoMonitorDlg 대화 상자

#include "ColorControl.h"
#include "SigProc.h"
#include "GlobalData.h"


#define MAX_BIT_IN 30
#define MAX_BIT_OUT 30

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

	CString GetInBitName(int nRow);
	CString GetOutBitName(int nRow);

	CString GetInBitAddress(int nRow);
	CString GetOutBitAddress(int nRow);


	CString GetInWordName(int nRow);
	CString GetOutWordName(int nRow);

	CString GetInWordAddress(int nRow);
	CString GetOutWordAddress(int nRow);

	CString GetInWordData(int nRow);
	CString GetOutWordData(int nRow);

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

		en_InWord_Max			= 80, 
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

		en_OutWord_Max					= 100,
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

	CGridCtrl	m_GridBitIn;
	CGridCtrl	m_GridBitOut;

	CGridCtrl	m_GridWordIn ;
	CGridCtrl	m_GridWordOut;

	CFont*		m_pFontGrid;
	
	CSequenceData m_localSeqDataIn;
	_SEQ_OUT_DATA_ALL m_localSeqDataOut;

#ifndef NEW_PLCTYPE
	_SEQ_OUT_DATA_ALL_SMS m_localSeqDataOutSms;
#endif //NEW_PLCTYPE

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
	afx_msg void OnBnClickedBtnDummyErrorClear();
	afx_msg void OnMouseDblClickGridBitOut(NMHDR* pNMHDR, LRESULT* pResult);	 // 22.06.23 Ahn Add 

};
