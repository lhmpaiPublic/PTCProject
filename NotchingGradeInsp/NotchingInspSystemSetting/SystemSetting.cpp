#include "pch.h"
#include "SystemSetting.h"
#include "Win32File.h"
#include "GlobalData.h"


CSystemSetting::CSystemSetting()
{
	//SPC+ 출력 여부 플래그 초기화
	m_bDisableSpcPlus = FALSE;

	m_dResolY1000P = 0.0;

}


int CSystemSetting::FileCtrl(int nMode)
{
    // TODO: 여기에 구현 코드 추가.

	//CWin32File winFile;
	CString strFileName;
	strFileName.Format(_T("%s\\SystemSetting.ini"), AprData.m_strDataPath);
	CString strSection;
	CString strKey;
	CString strData;
	int i;

    if (nMode == MODE_READ) {
		char buff[256];
		
		{
			strSection = _T("EQP_SETTING");
			strKey = _T("MACHINE_MODE"); // 0 : 양극 , 1 : 음극
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nMachineMode = atoi(buff);

			// 23.02.27 Son Add Start
			strKey = _T("LOGO");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nLogo = atoi(buff);
			// 23.02.27 Son Add End

			// 23.02.28 Son Add Start
			strKey = _T("PLC_MODE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nPlcMode = atoi(buff);

			strKey = _T("PLC_IP_ADDRESS");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_strPLCIPAddress.Format(_T("%s"), buff);

			strKey = _T("PLC_PORT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nPLCPort = atoi(buff);
			// 23.02.28 Son Add End

			strKey = _T("EDGE_DET_MEAN_WIDTH");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nEdgeSearchMean = atoi(buff);

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("RESOLUTION_HOR_CAM_NO_%02d"), i);
				::GetPrivateProfileString(strSection, strKey, "6.0", buff, 256, strFileName);
				m_dResolX[i] = atof(buff);
			}
			;
			strKey = _T("RESOLUTION_VER") ;
			::GetPrivateProfileString(strSection, strKey, "10.85", buff, 256, strFileName);
			m_dResolY = atof(buff);
			m_dResolY1000P = m_dResolY;

			// 22.05.27 Ahn Add Start
			strKey = _T("MACHINE_ID");
			::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
			m_strMachineID.Format(_T("%s"), buff);
			// 22.05.27 Ahn Add End

			//// 22.06.20 Ahn Add Start
			//strKey = _T("GRAPH_SIZE_FOILEXP");
			//::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
			//m_nGraphMaxSize_FoilExp = atoi(buff);

			//strKey = _T("GRAPH_SIZE_SURFACE");
			//::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
			//m_nGraphMaxSize_Surface = atoi(buff);
			//// 22.06.20 Ahn Add End
			// 22.09.01 Ahn Add Start			
			strKey = _T("DISPLAY_LANGUAGE");
			::GetPrivateProfileString(strSection, strKey, "kor", buff, 256, strFileName);
			m_strLangCode.Format( _T("%s"),buff) ;
			// 22.09.01 Ahn Add End

			// 22.07.04 Ahn Add Start
			strSection = _T("RESULT_DELETE");
			strKey = _T("RESULT_SAVE_PERIOD");
			::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
			m_nResultSavePeriod = atoi(buff);

			strKey = _T("DISK_CAPACITY_ALARM");
			::GetPrivateProfileString(strSection, strKey, "", buff, 256, strFileName);
			m_nDiskCapacity = atoi(buff);
			// 22.07.04 Ahn Add End
		}

		{
			strSection = _T("CAMERA_SETTING");
			m_bTriggerMode;					// TRUE : External Trigger , FALSE : Internal Trigger

			strKey = _T("TRIGGER_MODE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bTriggerMode = atoi(buff);

			for (i = 0; i < MAX_CAMERA_NO; i++){
				strKey.Format( _T("ACQISITION_RATE_CAM_NO_%02d"), i  );
				::GetPrivateProfileString(strSection, strKey, "100000.0", buff, 256, strFileName);
				m_dCamLineRate[ i ] = atof(buff);
			}

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("GAIN_CAM_NO_%02d"), i);
				::GetPrivateProfileString(strSection, strKey, "1.0", buff, 256, strFileName);
				m_dCamGain[i] = atof(buff);
			}

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("EXPOSURE_TIME_CAM_NO_%02d"), i);
				::GetPrivateProfileString(strSection, strKey, "9.16667", buff, 256, strFileName);
				m_dExposureTime[i] = atof(buff);
			}

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("VIEW_OFFSET_HOR_CAM_NO_%02d"), i);
				::GetPrivateProfileString(strSection, strKey, "7000", buff, 256, strFileName);
				m_nCamViewOffset[i] = atoi(buff);
			}

			//for (i = 0; i < MAX_CAMERA_NO; i++) 
			{
				strKey.Format(_T("VIEW_WIDTH_CAM_NO") );
				::GetPrivateProfileString(strSection, strKey, "2000", buff, 256, strFileName);
				m_nCamViewWidth = atoi(buff);
			}

			//for (i = 0; i < MAX_CAMERA_NO; i++) 
			{
				strKey.Format(_T("VIEW_HEIGHT_CAM_NO"));
				::GetPrivateProfileString(strSection, strKey, "16000", buff, 256, strFileName);
				m_nCamViewHeight = atoi(buff);
			}

			// 22.07.27 Ahn Add Start
			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("CAMERA_IMAGE_FLIP_%02d"), i);
				::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
				m_nCamImageFlip[i] = atoi(buff);
			}
			// 22.07.27 Ahn Add End
		}
		{
			strSection = _T("DISPLAY_SETTING");
			strKey = _T("GRID_FONT_SIZE");
			::GetPrivateProfileString(strSection, strKey, "14", buff, 256, strFileName);
			m_nGridFontSize = atoi(buff);
		}
		{
			strSection = _T("OTHER_SETTING");
			strKey = _T("ENABLE_MARKER");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bChkEnableMarker = atoi(buff);

			// 22.01.11 Ahn Add Start
			strKey = _T("ENABLE_MARKING_ALL_TAB");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bMarkingAllTab = atoi(buff);
			// 22.01.11 Ahn Add End

			strKey = _T("ENABLE_NG_STOP");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bEnableNgStop = atoi(buff);

			strKey = _T("JPEG_SAVE_QUALITY");
			::GetPrivateProfileString(strSection, strKey, "90", buff, 256, strFileName);
			m_nJpegSaveQuality = atoi(buff);
			if ((m_nJpegSaveQuality > 100) || (m_nJpegSaveQuality < 10)) {
				m_nJpegSaveQuality = 90;
			}

			strKey = _T("OVERFLOW_COUNT_MAX");
			::GetPrivateProfileString(strSection, strKey, "5", buff, 256, strFileName);
			m_nOverflowCountMax = atoi(buff);
			if ((m_nOverflowCountMax > 100) || (m_nOverflowCountMax < 2)) {
				m_nJpegSaveQuality = 5;
			}

			strKey = _T("MISS_TAB_ID_MAX");
			::GetPrivateProfileString(strSection, strKey, "3", buff, 256, strFileName);
			m_nMissTabIdMax = atoi(buff);
			if ((m_nMissTabIdMax > 100) || (m_nMissTabIdMax < 0)) {
				m_nJpegSaveQuality = 5;
			}

			strKey = _T("FIRST_TAB_DONOT_PROCESS");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bFirstTabDoNotProc = atoi(buff);

			strKey = _T("NON_NG_SAVE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bNonNgSave = atoi(buff);

			strKey = _T("NON_NG_STOP");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bNonNgStop = atoi(buff);
			
			strKey = _T("COUNTERBOARD_ERRCOUNT");
			::GetPrivateProfileString(strSection, strKey, "5", buff, 256, strFileName);
			m_nCounterBoard_ErrCount = atoi(buff);

			strKey = _T("MEMORYERROR");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 90, strFileName);
			m_MemorydError = atoi(buff);
			
			strKey = _T("LED_CHECK_DELAY");
			::GetPrivateProfileString(strSection, strKey, "2000", buff, 256, strFileName);
			m_nLED_Check_Delay = atoi(buff);

			strSection = _T("SPCPLUS_INFO");
			strKey = _T("NON_EXEC_SPCPLUS");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			//SPC+ 출력여부 플래그를 SystemSetting 파일에서 읽어서 가져온다.
			m_bDisableSpcPlus = atoi(buff);


			strKey = _T("NO_TAB_NO_MARKING");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bNoTabMarkingSkip = atoi(buff);

		}
		// 23.02.17 Son Add Start
		{
			strSection = _T("ADDRESS_SETTING");
			strKey = _T("BIT_IN");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nBitIn = atoi(buff);

			strKey = _T("BIT_OUT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nBitOut = atoi(buff);

			strKey = _T("WORD_IN");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nWordIn = atoi(buff);

			strKey = _T("WORD_OUT");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nWordOut = atoi(buff);

			strKey = _T("SLAVE_ID");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nSlaveID = atoi(buff);


		}
		// 23.02.17 Son Add End
		{
			// 230922 kjk
			strSection = _T("RS232_SETTING");
			strKey = _T("RS232_MODE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nRS232_Mode = atoi(buff);

			strKey = _T("RS232_BAUDRATE");
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_nRS232_Baudrate = atoi(buff);

			strKey = _T("RS232_COMPORT_TOP");
			::GetPrivateProfileString(strSection, strKey, "COM1", buff, 256, strFileName);
			m_strRS232_COMPORT_TOP.Format(_T("%s"), buff);

			strKey = _T("RS232_COMPORT_BOTTOM");
			::GetPrivateProfileString(strSection, strKey, "COM2", buff, 256, strFileName);
			m_strRS232_COMPORT_BOTTOM.Format(_T("%s"), buff);
		}

    }
    else {
		{
			strSection = _T("EQP_SETTING");
			strKey = _T("MACHINE_MODE");
			strData.Format(_T("%d"), m_nMachineMode);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			// 23.02.27 Son Add Start
			strKey = _T("LOGO");
			strData.Format(_T("%d"), m_nLogo);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			// 23.02.27 Son Add End

			// 23.02.28 Son Add Start
			strKey = _T("PLC_MODE");
			strData.Format(_T("%d"), m_nPlcMode);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("PLC_IP_ADDRESS");
			::WritePrivateProfileString(strSection, strKey, m_strPLCIPAddress, strFileName);

			strKey = _T("PLC_PORT");
			strData.Format(_T("%d"), m_nPLCPort);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			// 23.02.28 Son Add End

			strKey = _T("EDGE_DET_MEAN_WIDTH");
			strData.Format(_T("%d"), m_nEdgeSearchMean);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("RESOLUTION_HOR_CAM_NO_%02d"), i);
				strData.Format(_T("%.6lf"), m_dResolX[i]);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}
			;
			strKey = _T("RESOLUTION_VER");
			strData.Format(_T("%.2lf"), m_dResolY);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			// 22.05.27 Ahn Add Start
			strKey = _T("MACHINE_ID");
			::WritePrivateProfileString(strSection, strKey, m_strMachineID, strFileName);
			// 22.05.27 Ahn Add End

			//// 22.06.20 Ahn Add Start
			//strKey = _T("GRAPH_SIZE_FOILEXP");
			//strData.Format("%d", m_nGraphMaxSize_FoilExp);
			//::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			//strKey = _T("GRAPH_SIZE_SURFACE");
			//strData.Format("%d", m_nGraphMaxSize_Surface);
			//::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			//// 22.06.20 Ahn Add End
			// 22.09.01 Ahn Add Start
			strKey = _T("DISPLAY_LANGUAGE");
			::WritePrivateProfileString(strSection, strKey, m_strLangCode, strFileName);
			// 22.09.01 Ahn Add End
			// 
			// 22.07.04 Ahn Add Start
			strSection = _T("RESULT_DELETE");
			strKey = _T("RESULT_SAVE_PERIOD");
			strData.Format(_T("%d"), m_nResultSavePeriod);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("DISK_CAPACITY_ALARM");
			strData.Format(_T("%d"), m_nDiskCapacity);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			// 22.07.04 Ahn Add End
		}
		// 카메라 세팅
		{
			strSection = _T("CAMERA_SETTING");
			strKey.Format(_T("TRIGGER_MODE"));
			strData.Format(_T("%d"), m_bTriggerMode);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("ACQISITION_RATE_CAM_NO_%02d"), i);
				strData.Format(_T("%.6lf"), m_dCamLineRate[i]);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("GAIN_CAM_NO_%02d"), i);
				strData.Format(_T("%.1lf"), m_dCamGain[i]);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("EXPOSURE_TIME_CAM_NO_%02d"), i);
				strData.Format(_T("%.6lf"), m_dExposureTime[i]);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}

			//for (i = 0; i < MAX_CAMERA_NO; i++) 
			{
				strKey.Format(_T("VIEW_WIDTH_CAM_NO"));
				strData.Format(_T("%d"), m_nCamViewWidth);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}

			//for (i = 0; i < MAX_CAMERA_NO; i++) 
			{
				strKey.Format(_T("VIEW_HEIGHT_CAM_NO"));
				strData.Format(_T("%d"), m_nCamViewHeight);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}

			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("VIEW_OFFSET_HOR_CAM_NO_%02d"), i);
				strData.Format(_T("%d"), m_nCamViewOffset[i]);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}

			// 22.07.27 Ahn Add Start
			for (i = 0; i < MAX_CAMERA_NO; i++) {
				strKey.Format(_T("CAMERA_IMAGE_FLIP_%02d"), i);
				strData.Format(_T("%d"), m_nCamImageFlip[i]);
				::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			}
			// 22.07.27 Ahn Add End
		}
		{
			strSection = _T("DISPLAY_SETTING");
			strKey = _T("GRID_FONT_SIZE") ;
			strData.Format(_T("%d"), m_nGridFontSize);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
		}
		{
			strSection = _T("OTHER_SETTING");
			strKey = _T("ENABLE_MARKER");
			strData.Format(_T("%d"), m_bChkEnableMarker);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			// 22.01.11 Ahn Add Start
			strKey = _T("ENABLE_MARKING_ALL_TAB");
			strData.Format( _T("%d"), m_bMarkingAllTab );
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
			// 22.01.11 Ahn Add End

			strKey = _T("ENABLE_NG_STOP");
			strData.Format(_T("%d"), m_bEnableNgStop);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);			

			strKey = _T("JPEG_SAVE_QUALITY");
			strData.Format(_T("%d"), m_nJpegSaveQuality);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("OVERFLOW_COUNT_MAX");
			strData.Format(_T("%d"), m_nOverflowCountMax);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("MISS_TAB_ID_MAX");
			strData.Format(_T("%d"), m_nMissTabIdMax);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("FIRST_TAB_DONOT_PROCESS");
			strData.Format(_T("%d"), m_bFirstTabDoNotProc);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);	

			strKey = _T("NON_NG_SAVE");
			strData.Format(_T("%d"), m_bNonNgSave);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("NON_NG_STOP");
			strData.Format(_T("%d"), m_bNonNgStop);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("COUNTERBOARD_ERRCOUNT");
			strData.Format(_T("%d"), m_nCounterBoard_ErrCount);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("MEMORYERROR");
			strData.Format(_T("%d"), m_MemorydError);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strSection = _T("SPCPLUS_INFO");
			strKey = _T("NON_EXEC_SPCPLUS");
			//SPC+ 출력여브 체크박스 이벤트 값을 SystemSetting 파일에 저장한다.
			strData.Format(_T("%d"), m_bDisableSpcPlus);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("NO_TAB_NO_MARKING");
			strData.Format(_T("%d"), m_bNoTabMarkingSkip);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
		}
		// 23.02.17 Son Add Start
		{
			strSection = _T("ADDRESS_SETTING");
			strKey = _T("BIT_IN");
			strData.Format(_T("%d"), m_nBitIn);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("BIT_OUT");
			strData.Format(_T("%d"), m_nBitOut);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("WORD_IN");
			strData.Format(_T("%d"), m_nWordIn);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

			strKey = _T("WORD_OUT");
			strData.Format(_T("%d"), m_nWordOut);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

//			strKey = _T("SLAVE_ID");
// 			strData.Format(_T("%d"), );
// 			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

		}
		// 23.02.17 Son Add End

		{
			//			strKey = _T("SLAVE_ID");
			// 			strData.Format(_T("%d"), );
			// 			::WritePrivateProfileString(strSection, strKey, strData, strFileName);

		}

    }
    return 0;
}

//CSystemSetting CSystemSetting::Copy(CSystemSetting& Info)
//{
//	
//	return 
//}
//CSystemSetting CSystemSetting::operator=(CSystemSetting& Info)
//{
//	Copy(Info);
//}