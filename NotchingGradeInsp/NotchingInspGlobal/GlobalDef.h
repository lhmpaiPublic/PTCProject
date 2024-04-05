#pragma once

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

enum {
	en_defPosEdge = 0,
	en_defPosRound = 1,
	en_defPosSurface = 2,
	en_defPosMax = 3,
};

enum {
	enHistoryDisp_Calender = 0,
	enHistoryDisp_Thumbnail = 1,
	enHistoryDisp_List = 2,
	enMaxHistoryDisp,
};

namespace UserModeDefine {
	typedef enum {
		enOperator = 0	,
		enEngineer		,
		enMaker			,	
		enMax			,
	} eMode;

	static const CString szName[enMax] = { _T("OPERATOR_MODE"),_T("ENGINEER_MODE"), _T("MAKER_MODE") }; 
};

enum {
	en_Plc_Melsec = 0,
	en_Plc_Siemens = 1,
};

#define		MAX_CROP_IMAGE_QUEUE_SIZE	540
#define		MAX_SAVE_LOT_INFO_SIZE	30 

#define		SAVE_TAB_LOG
#define		SAVE_TAB_DIV_LOG

#define		MAX_DISP_DEF_COUNT			30	
#define		MAX_SAVE_DEFECT_COUNT		30	
#define		INSPECTION_TYPE				_T("FOIL-EXP")


// 장비 고유 설정
#define		MIN_FRAME_SIZE		1000
#define		MAX_FRAME_SIZE		16384

#define		IMAGE_PROC_SKIP_COUNT		5
#define		IMAGE_DISPLAY_SKIP_COUNT	5
#define		FRAME_ACQ_ERROR_CHK_CNT		5	

#define		MAX_THREAD_QUEUE_SIZE	5

#define		CAMERA_TDI_MODE
#define		RESOLUTION_X		6.0
#define		RESOLUTION_Y		10.85

#define		CAMERA_MAX_WIDTH	16384

#define		MEAN_SIZE_HOR		100
#define		MEAN_SIZE_HOR_NEGATIVE	50

#define		CATHODE_MODE		0 // 음극
#define		ANODE_MODE			1 // 양극 

#define		TEST_LOG

#define		MODE_READ			0
#define		MODE_WRITE			1

#define		GRABBER_COUNT		2		// Grabber Board 수	
#define		IMAGE_SAVE_THREAD	4

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

#define		DISK_SPACE_ALARM_LIGHT	80
#define		DISK_SPACE_ALARM_HEAVY	90



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
//#define	SAVE_TACT_LOG 

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


#define	MAX_USE_PORT 4 
#define	MAX_PORT_BIT 8 

//Seimens Bit 영역 갯수
#define MAX_SMS_BITIO_IN	20
#define MAX_SMS_BITIO_OUT	20

//Seimens Word 영역 갯수
#define MAX_SMS_WORDIO_IN	160
#define MAX_SMS_WORDIO_OUT	160
