#pragma once
#include "GridCtrl.h"
#include "SystemSetting.h"
// CSystemSettingDlg 대화 상자

class CSystemSetting;
class CNotchingGradeInspView;

class CSystemSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemSettingDlg)

public:
	CSystemSettingDlg(CWnd* pParent = nullptr, BOOL m_bMode = TRUE );   // 표준 생성자입니다.

public:

	enum {
		//en_CamSerialNo = 0,
		en_CamResolutionHor = 0,//1
		en_CamAquisitionRate,	//2
		en_CamExposureTime,		//3
		en_CamGain,				//4
		en_CamViewWidth,		//5
		en_CamViewHeight,		//6
		en_CamViewOffset,		//7
		en_CamImgFlipMode,		//8
		en_CamParamMax,			//9
		en_CamPramRows, 
	};

	int MakeGridCtrl();
	int UpdateGrid();
	BOOL CheckExposureTime(double dAcqRate, double *dExpTime );
	double CalcExposureTime(double dAcqRate, double dSpeed );
	double CalcAquisitionRate(double dSpeed, double dResolutionY);
	BOOL CheckViewWidthAndOffset(int nWidth, int nOffset);

private :
	CSystemSetting m_SysSetting;

	CGridCtrl m_GridCameraParam;
	CFont* m_pFontGrid;
	CNotchingGradeInspView* m_pParent;

	BOOL m_bStartMode; // 22.08.05 Ahn Add 

	CComboBox m_ImageFormat;
// 구현입니다.
public:
	virtual ~CSystemSettingDlg();


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SYSTEM_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public :
	int m_nEdMeanWidth;
	double m_dEdResolY;
	int m_nRadMachineMode;
	CEdit m_EdDummy;
	BOOL m_bChkTriggerMode;
	BOOL m_bChkEnableMarker;
	BOOL m_bChkMarkingAllTab;// 22.01.11 Ahn Add

	BOOL CheckPrmWidth( int nWidth );
	BOOL CheckPrmOffset( int nOffset );

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedRadPositiveMode();
	afx_msg void OnClickGridCtrlCamParam(NMHDR* pNMHDR, LRESULT* pResult);
	// 22.07.25 Ahn Add Start
	afx_msg void OnDBClickGridCtrlCamParam(NMHDR* pNMHDR, LRESULT* pResult);
	// 22.07.25 Ahn Add End

	afx_msg void OnEnSetfocusEdEdgeDetMeanWidth();
	afx_msg void OnEnSetfocusEdResolutionVer();

	int DataControl(int nMode);
	afx_msg void OnBnClickedChkExternalTrigger();
	afx_msg void OnBnClickedBtnCamParamSetAll();
	afx_msg void OnBnClickedChkEnableMarker();
	afx_msg void OnEnClickChkMarkingAllTab(); // 22.01.11 Ahn Add
	int m_nJpgSaveQulaity;
	int m_nEdOverflowCountMax;
	afx_msg void OnEnSetfocusEdJpegSaveQuality();
	afx_msg void OnBnClickedChkFirstTabDonotProc();
	BOOL m_bChkFirstTabDoNotProc;
	BOOL m_bChkEnableNgStop;
	afx_msg void OnBnClickedChkEnableNgStop();
	CString m_EdMachineID;
	afx_msg void OnEnSetfocusEdMachineId();
	afx_msg void OnDestroy();
	// 22.08.31 Ahn Delete Start
	//int	m_nGridFondSize;
	//int m_nGraphSizeFoilExp;
	//int m_nGraphSizeSurface;
	// 22.08.31 Ahn Delete End
	 
	// 22.08.31 Ahn Delete Start
	//afx_msg void OnEnSetfocusEdGridFontSize();
	//afx_msg void OnEnSetfocusEdGraphSizeFoilexp();
	//afx_msg void OnEnSetfocusEdGraphSizeSurface();
	// 22.08.31 Ahn Delete End
	int m_nEdSavePeriod;
	afx_msg void OnEnSetfocusEdResultSavePeriod();
	int m_nDiskCapacityAlarm;
	afx_msg void OnEnSetfocusEdDiskCapacityAlarm();
	// 22.08.31 Ahn Add Start
	int m_nLanguage;
	// 22.08.31 Ahn Add End
	afx_msg void OnBnClickedRadKorean();
	afx_msg void OnBnClickedRadEnglish();


	// 22.09.01 Ahn Add Start
	void DisplayLanguage();
	// 22.09.01 Ahn Add End
	
	// 23.02.17 Son Add Start
	CString m_EdBitIn;
	CString m_EdBitOut;
	CString m_EdWordIn;
	CString m_EdWordOut;

	afx_msg void OnEnSetfocusEdBitIn();
	afx_msg void OnEnSetfocusEdBitOut();
	afx_msg void OnEnSetfocusEdWordIn();
	afx_msg void OnEnSetfocusEdWordOut();
	// 23.02.17 Son Add Ebd
	// 23.02.27 Son Add Start
	afx_msg void LogoCtrl(UINT id);
	UINT m_RadLogo;
	// 23.02.27 Son Add End
	// 23.02.28 Son Add Start
	afx_msg void PLCCtrl(UINT id);
	UINT m_RadPlcMode;
	afx_msg void OnEnSetfocusEdIP();
	afx_msg void OnEnSetfocusEdPort();
	CString m_EdIPAddress;
	int	m_nEdPort;
	// 23.02.28 Son Add End
	// 23.03.03 Son Add Start
	CString m_EdBitIn_16;
	CString m_EdBitOut_16;
	CString m_EdWordIn_16;
	CString m_EdWordOut_16;
	afx_msg void OnCbnSelchangeComImageoutformat();
	afx_msg void OnBnClickedChkEnableNonNgSave();
	afx_msg void OnBnClickedChkEnableNonNgStop();

	BOOL m_bChkEnableNonNgSave;
	BOOL m_bChkEnableNonNgStop;

	afx_msg void OnSetfocusEdOverflowMax();
	int m_nEdMissTabIdMax;
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSetfocusEdMissTabIdMax();
	afx_msg void OnBnClickedChkDisableSpcplus();
	//SPC+ 출력여부 플래그
	BOOL m_bChkDisableSpcPlus;
};
