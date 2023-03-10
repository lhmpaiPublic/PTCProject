#pragma once

class CSystemSetting {

public :
	CSystemSetting();
public:
	int m_nMachineMode;// 0 : ¾ç±Ø, 1 : À½±Ø

	CString m_strFeederPath;
	CString	m_strSamplePath;

	double	m_dResolX[MAX_CAMERA_NO];
	double	m_dResolY;

	CString m_strSerialNo[MAX_CAMERA_NO]; 

	// Camera Parameter 
	double	m_dCamLineRate[MAX_CAMERA_NO]; // Acquisition Rate 
	double	m_dCamGain[MAX_CAMERA_NO];
	double	m_dExposureTime[MAX_CAMERA_NO];	// Line Rat¿¡ ¼³Á¤°ª¿¡ µû¶ó º¯µ¿.
	BOOL	m_bTriggerMode;					// TRUE : External Trigger , FALSE : Internal Trigger

	int		m_nCamViewWidth;
	int		m_nCamViewOffset[MAX_CAMERA_NO];
	int		m_nCamViewHeight;
	// 22.07.25 Ahn Add Start
	int		m_nCamImageFlip[MAX_CAMERA_NO];
	// 22.07.25 Ahn Add End

	int		m_nEdgeSearchMean;		// ¿§Áö °ËÃâ¿ë Mean Æø

	int		m_nGridFontSize;
	BOOL	m_bChkEnableMarker;
	// 22.01.11 Ahn Add Start
	BOOL	m_bMarkingAllTab;
	// 22.01.11 Ahn Add End
	BOOL	m_bEnableNgStop;
	int		m_nJpegSaveQuality;

	BOOL	m_bFirstTabDoNotProc;

	CString m_strMachineID;

	// 22.06.20 Ahn Add Start
	int		m_nGraphMaxSize_FoilExp;
	int		m_nGraphMaxSize_Surface;
	// 22.06.20 Ahn Add End
	// 22.07.04 Ahn Add Start
	int		m_nResultSavePeriod;
	int		m_nDiskCapacity;
	// 22.07.04 Ahn Add End
	// 22.09.01 Ahn Add Start
	//int		m_nLanguage;
	CString m_strLangCode;
	// 22.09.01 Ahn Add End

	int FileCtrl(int nMode);
	//CSystemSetting Copy(CSystemSetting &Info);
	//CSystemSetting operator=(CSystemSetting& Info);

	// 23.02.17 Son Add Start
	int m_nBitIn;
	int m_nBitOut;
	int m_nWordIn;
	int m_nWordOut;
	int m_nSlaveID;
	// 23.02.17 Son Add End
	// 23.02.27 Son Add Start
	int m_nLogo;
	// 23.02.27 Son Add End
	// 23.02.28 Son Add Start
	int m_nPlcMode;
	CString m_strPLCIPAddress;
	int m_nPLCPort;
	// 23.02.28 Son Add End
};

