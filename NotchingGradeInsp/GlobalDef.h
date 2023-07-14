#pragma once

//#if defined( POSITIVE_VRESION )
//#elif defined( NEGATIVE_VERSION )
//#else
//#endif

#include "LanguageCode.h"

enum {
	enInspMode = 0,
	enHistoryMode = 1,
	enCondMode = 2,
	enMaxViewMode,
};

enum {
	en_ModeFoilExp		= 0,
	en_ModeFoilExpOut	= 1,
	en_ModeSurface		= 2,
	en_ModeFoilBoth		= 3, // 22.07.20 Ahn Add
	en_ModeMax			= 4,
};

enum {
	enInspRun = 0,
	enInspStop = 1,
	enInspPause = 2,
	enMaxInspMode,
};

// 22.12.14 Ahn Add Start
enum {
	en_defPosEdge = 0,
	en_defPosRound = 1,
	en_defPosSurface = 2,
	en_defPosMax = 3,
};
// 22.12.14 Ahn Add End

//KANG 22.05.24 Add Start
enum {
	enHistoryDisp_Calender = 0,
	enHistoryDisp_Thumbnail = 1,
	enHistoryDisp_List = 2,
	enMaxHistoryDisp,
};
//KANG 22.05.24 Add End

// 22.05.25 Son Add Start
namespace UserModeDefine {
	typedef enum {
		enOperator = 0	,
		enEngineer		,
		enMaker			,	// 22.09.15 Ahn Add 
		enMax			,
	} eMode;

	// 22.09.15 Ahn Add Start
	static const char* szName[enMax] = { _T("OPERATOR_MODE"),_T("ENGINEER_MODE"), _T("MAKER_MODE") }; 
	// 22.09.15 Ahn Add End
};
// 22.05.25 Son Add End

// 23.02.02 Ahn Add Start
enum {
	en_Plc_Melsec = 0,
	en_Plc_Siemens = 1,
};
// 23.02.02 Ahn Add End

//#define		BOTTOM_CAMERA_DEBUG

//#define		DEBUG_NOISE_COUNTERMEASURE

//#define		MELSEC_ETHERNET 
//#define		DEBUG_MELSEC_ETHERNET // 멜섹 사용시 커맨드 확인용 디버그 디파인

// 22.06.26 Ahn Add Start
#define		MAX_CROP_IMAGE_QUEUE_SIZE	54
#define		MAX_SAVE_LOT_INFO_SIZE	30 
// 22.06.26 Ahn Add End

#define		SAVE_TAB_LOG
#define		SAVE_TAB_DIV_LOG

// 22.11.07 Ahn Test Start 5 -> 20 
#define		MAX_DISP_DEF_COUNT			30	// 22.02.25 Ahn Modify 100 -> 5 
#define		MAX_SAVE_DEFECT_COUNT		30	// 22.05.24 Ahn Add 
// 22.11.07 Ahn Test Start
#define		INSPECTION_TYPE				_T("FOIL-EXP")

#define		MAX_SAVE_IMAGE_QUEUE		20		// 

// 장비 고유 설정
// 22.03.25 Ahn Add Start
#define		MIN_FRAME_SIZE		2000
#define		MAX_FRAME_SIZE		16384
// 22.03.25 Ahn Add End

// 22.05.18 Ahn Add Start
#define		IMAGE_PROC_SKIP_COUNT		5
#define		IMAGE_DISPLAY_SKIP_COUNT	5
#define		FRAME_ACQ_ERROR_CHK_CNT		5	
// 22.05.18 Ahn Add End

// 23.02.10 Ahn Add Start
//#define		MAX_THREAD_QUEUE_SIZE 4
#define		MAX_THREAD_QUEUE_SIZE	5
// 23.02.10 Ahn Add End

#define		CAMERA_TDI_MODE
#define		RESOLUTION_X		6.0
#define		RESOLUTION_Y		10.85

#define		CAMERA_MAX_WIDTH	16384

#define		MEAN_SIZE_HOR		100
#define		MEAN_SIZE_HOR_NEGATIVE	50

// 22.09.15 Ahn Modify Start
//#define		ANODE_MODE			0 // 음극 
//#define		CATHODE_MODE		1 // 양극
#define		CATHODE_MODE		0 // 음극
#define		ANODE_MODE			1 // 양극 
// 22.09.15 Ahn Modify End

#define		TEST_LOG

#define		MODE_READ			0
#define		MODE_WRITE			1

#define		GRABBER_COUNT		2		// Grabber Board 수	
// 22.07.08 Son Add Start
#define		IMAGE_SAVE_THREAD	4
// 22.07.08 Son Add End

#define		CAM_POS_TOP			0
#define		CAM_POS_BOTTOM		1
#define		MAX_CAMERA_NO		2		// 카메라 대수

#define		MAX_LIGHT_CHANEL	2
#define		MAX_LIGHT_UNIT		2

#define		DIR_HOR				0	// 가로방향
#define		DIR_VER				1	// 세로방향

#define		DEFAULT_IMG_SIZE_W		2000
#define		DEFAULT_IMG_SIZE_H		7500

// Overflow 조건
#define		OVERFLOW_OCCURED	4
#define		MAX_BLOCK_PIXEL		32768
#define		MAX_DEFECT_COUNT	128

#define		BUFFER_RANGE_COUNT		50 

#define		MAX_RECIPE_TABLE	50

#define		GRID_FONT_HEIGHT	14
#define		GRID_ROW_HEIGHT		(GRID_FONT_HEIGHT+2)

#define		SWAP( type, x, y) { type t; t = x, x = y ; y = t ; } ;

//{_T("No"),_T("Pixel"),_T("PosX"),_T("PosY"),_T("Col1"),_T("Row1"),_T("Col2"),_T("Row2"),_T("Height"),_T("Width"),_T("LinkCnt"),_T("--")};
enum {
	enDefNo = 0,
	enDefIndex,
	enPixel,
	enPosX,
	enPosY,
	enCol1,
	enRow1,
	enCol2,
	enRow2,
	enHeight,
	enWidth,
	enLinkCnt,
	enBriAvg,
	enBriMax,
	enEtc,
};


// Debug용 Log
#define	SAVE_TACT_LOG 

//#define IMAGE_DRAW_NOTIFY_VERSION 
#if !defined( IMAGE_DRAW_NOTIFY_VERSION )
	#define IMAGE_DRAW_DIRECT_VERSION
#endif

#define		TAB_LEVEL_FIND_POINT	7

#define		TYPE_FOILEXP			0
#define		TYPE_FOILEXP_OUT		1
#define		TYPE_SURFACE			2
#define		MAX_INSP_TYPE			3 // 결함 검출 모드 Dross / Foil 노출

#define		RANK_OK			0
#define     RANK_GRAY		1
#define		RANK_NG			2
#define		MAX_RANK		3

#define		JUDGE_OK		0
#define		JUDGE_GRAY		1 
#define		JUDGE_NG		2
#define		JUDGE_MAX		3

#define		RANK_COND_SIZE		0
#define		RANK_COND_BRIGHT	1
#define		MAX_RANK_COND		2 

// 검출력 관련 Define

#define		THRESHOLD_MIN_VALUE		100

//
////// 22.09.01 Ahn Add Start
//#define		LANGUAGE_CODE_KOR	0
//#define		LANGUAGE_CODE_ENG	1
////
//int GetLanguageCode(void)
//{
//	static int nLangCode = -1 ;
//	if ((nLangCode < 0) || (nLangCode > LANGUAGE_CODE_ENG)) {
//		char szPath[ 256 ] ;
//		char buff[128];
//		::GetCurrentDirectory(_MAX_PATH, szPath);
//		CString strPath;
//		CString strFileName;
//		CString strSection;
//		CString strKey;
//
//		strFileName.Format(_T("%s\\Data\\SystemSetting.ini"), szPath ) ;
//		strPath.Format(_T("%s\\Data"), szPath );
//
//		strKey = _T("DISPLAY_LANGUAGE");
//		::GetPrivateProfileString(strSection, strKey, 0, buff, 1, strFileName);
//		nLangCode = atoi(buff);
//	}
//	return (nLangCode);
//}
////
//#define		_DISP_LANG( kor, eng ) ( ( GetLanguageCode() == LANGUAGE_CODE_KOR ) ? kor : eng )  
////// 22.09.01 Ahn Add End
//


// 22.07.29 Ahn Add Start
#define	MAX_USE_PORT 4 
#define	MAX_PORT_BIT 8 
// 22.07.29 Ahn Add End

#define MAX_SMS_IO_IN	20
#define MAX_SMS_IO_OUT	20
