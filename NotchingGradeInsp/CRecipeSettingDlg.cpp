// CRecipeSettingDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CRecipeSettingDlg.h"
#include "afxdialogex.h"
#include "SetValue.h"
#include "GlobalData.h"
#include "CRecipeCtrl.h"
#include "Win32File.h"
#include "RecipImportDlg.h"
#include "GridCtrl.h"
#include "BitmapStd.h"
#include "SystemSettingDlg.h" // 22.08.05 Ahn Add
#include "CInitSystemSetting.h"
#include "LogDisplayDlg.h"

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
//SPC+ 사용 해더파일
#include "SpcSpecParaManager.h"
#include "SpcSpecParaInData.h"
#include "SpcSpecInfo.h"
#include "SpcParaInfo.h"
#endif //SPCPLUS_CREATE

// CRecipeSettingDlg 대화 상자

static CString UiText1[][3] =
{
	{_T("레시피 설정"), _T("Recipe Setting"), _T("Recipe Setting")},
	{_T("레시피명"), _T("Recipe Name"), _T("Recipe名")},
	{_T("메모"), _T("Memo"), _T("备注")},
	{_T("레시피 번호"), _T("Recipe No."), _T("Recipe编号")},
	{_T("등록"), _T("Regist"), _T("登录")},
	{_T("삭제"), _T("Delete"), _T("删除")},
	{_T("프로그램정보"), _T("Program Info"), _T("程序信息")},
	{_T("가져오기"), _T("Load"), _T("导入")},
	{_T("기타 설정"), _T("Other Setting"), _T("其他设置")},
	{_T("IMAGE 저장 간격"), _T("IMAGE Save Interval"), _T("图片保存间隔")},
	{_T("NG만 저장"), _T("Save only NG Image"), _T("只保存NG")},
	{_T("비교처리 미사용"), _T("Diable Diff"), _T("TAB设置")},
	{_T("Tab 정보"), _T("Tab Setting"), _T("TAB设置")},
	{_T("입력단위[mm]"), _T("Input[mm]"), _T("输入单位[]")},
	{_T("단위[Pix]"), _T("Unit[Pix]"), _T("单位{pix]")},
	{_T("음극"), _T("CATHODE"), _T("阴极")},
	{_T("탭 코팅 높이"), _T("Tab coat Height"), _T("Tab涂层高度")},
	{_T("V홈 높이"), _T("V Hight"), _T("V 沟槽高度")},
	{_T("탭폭"), _T("Tab Width"), _T("Tab宽")},
	{_T("탭 피치"), _T("Tab Pitch"), _T("Tab Pitch")},
	{_T("라운드 반지름"), _T("Round radius"), _T("倒角半径")},
	{_T("양극"), _T("Anode"), _T("阳极")},
	{_T("Tab코팅높이"), _T("Tab shoulder coat size"), _T("Tab 涂层高度")},
	{_T("Bottom코팅높이"), _T("Bottom coat size"), _T("Bottom涂层高度")},
	{_T("Tab코팅높이"), _T("Tab coat size"), _T("Tab 涂层高度")},
	{_T("엣지 검출 조건"), _T("Edge Detect Condition"), _T("Edge识别条件")},
	{_T("코팅부 휘도(Min/Max)"), _T("Coat Bright(Min/Max)"), _T("电极辉度(MimMax)")},
	{_T("롤부 휘도(Min/Max)"), _T("Roll Bright(Min/Max)"), _T("滚轮辉度(MimMax)")},
	{_T("조명설정"), _T("Light Setting"), _T("照明设置")},
	{_T("마킹설정"), _T("Marking Setting"), _T("喷码设置")},
	{_T("검출력 설정"), _T("Detect Info"), _T("检出力设定")},
	{_T("검출레벨(원화상)"), _T("Threshold(Org)"), _T("Threshold(Org)")},
	{_T("검출레벨(하한)"), _T("Threshold(Lower)"), _T("Threshold(Lower)")},
	{_T("검사범위[mm]"), _T("Insp Range[mm]"), _T("检查范围[mm]")},
	{_T("최소검출 사이즈[Pix]"), _T("Min Size[Pix]"), _T("最小检出尺寸[pix]")},
	{_T("마스크오프셋"), _T("Mask offset"), _T("Mask Offset[Pix]")},
	{_T("전극표면 검출 설정"), _T("Surface Defect Setting"), _T("表面缺陷设置")},
	{_T("표면검사 미사용"), _T("Disable surface detection"), _T("禁用表面检查")},
	{_T("스레스홀드"), _T("Threshold"), _T("Threshold")},
	{_T("마스크오프셋[Pix]"), _T("Mask Offset[Pix]"), _T("Mask Offset[Pix]")},
	{_T("최소검출 사이즈[Pix]"), _T("Min size[Pix]"), _T("最小检出尺寸[pix]")},
	{_T("결함 링크 사용"), _T("Enable Defect Link"), _T("缺陷连接功能")},
	{_T("연결거리 X[Pix]"), _T("Distance X[Pix]"), _T("连接距离 X[Pix]")},
	{_T("연결거리 Y[Pix]"), _T("Disatance Y[Pix]"), _T("连接记录 Y[Pix]")},
	{_T("검출배율"), _T("Magnification"), _T("检出倍率")},
	{_T("결함 판정"), _T("Defect Judgement"), _T("缺陷判定")},
	{_T("가로(화면기준)"), _T("Horizon"), _T("地平线")},
	{_T("표면결함 NG 사이즈"), _T("Surface NG Size"), _T("Surface NG Size")},
	{_T("NG 알람 설정"), _T("NG Alarm"), _T("NG报警器")},
	{_T("연속결함 알람"), _T("Continuous detect alarm"), _T("连续报警")},
	{_T("구간결함 알람"), _T("Section detect alarm"), _T("断面报警")},
	{_T("Foil exp 결함 제거"), _T("Foil_Exp_In Defect Remove"), _T("缺陷屏蔽功能")},
	{_T("결함 제거 거리[um]"), _T("Remove distance[um]"), _T("缺陷屏蔽距离[um]")},
	{_T("제거 결함 크기[um]"), _T("Remove size[um]"), _T("缺陷屏蔽大小[um]")},
	{_T("결함 그레이 판정"), _T("Defect Judgement Gray"), _T("Defect Judgement Gray")},
	{_T("NG 멈춤"), _T("NG Stop"), _T("NG停机")},
	{_T("Disable Pet Flim Detection"), _T("Disable Pet Flim Detection"), _T("Disable Pet Flim Detection")},
	{_T("Matrix(X/Y,ea)"), _T("Matrix(X/Y,ea)"), _T("Matrix(X/Y,ea)")},
	{_T("Threshold (over)"), _T("Threshold (over)"), _T("Threshold (over)")},
	{_T("Check Count (over)"), _T("Check Count (over)"), _T("Check Count (over)")},
	{_T("Dark Roll"), _T("Dark Roll"), _T("暗色Roll")},
	{_T("Bright Roll"), _T("Bright Roll"), _T("亮色Roll")},
	{_T("Roll Mode"), _T("Roll Mode"), _T("Roll 模式")},
	{_T("Tab Bright"), _T("Tab Bright"), _T("Tab 辉度")},
	{_T("Tab Foil Lowest"), _T("Tab Foil Lowest"), _T("Tab 金属箔 最低亮度")},

};

enum UiText1Name
{
	ST_GROUP_RCP_SET,
	ST_RECIPE_NAME,
	ST_RECIPE_NAME2,
	ST_RECIPE_NO,
	BTN_REGIST,
	BTN_DELETE,
	BTN_PROGRAMINFO,
	BTN_LOAD,
	ST_OTHER_SETTING,
	STATIC_IMAGESAVEINTERVAL,
	CHK_SAVE_ONLY_NG_TAB,
	CHK_DISABLE_PROC_DIFF,
	ST_GROUP_TAB_INFO,
	ST_INPUT_MM,
	ST_INPUT_PIX,
	GROUP_CATHODE,
	ST_COAT_HEIGHT,
	STATIC_GROOVE_HEIGHT,
	ST_TAB_WIDTH,
	ST_TAB_PITCH,
	ST_ROUND_RADIUS,
	GROUP_ANODE,
	ST_TOP_COAT_HEIGHT,
	ST_BTM_COAT_HEIGHT,
	ST_TAB_COAT_HEIGHT,
	ST_EDGE_DETECT_COND,
	ST_COAT_BRIGHT,
	ST_ROLL_BRIGHT,
	ST_LIGHT_SETTING,
	ST_MARKING_SETTING,
	ST_DETECT_INFO,
	ST_THRES_ORG,
	ST_THRES_LOWER,
	ST_INSP_WIDTH,
	ST_DET_MIN_SIZE,
	ST_MASK_OFFSET,
	ST_GROUP_SPETTER,
	CHK_DISABLE_SPTTER,
	ST_THRES_SURFACE,
	ST_MASK_SURFACE,
	ST_MIN_SIZE_SURFACE,
	CHK_ENABLE_DEF_LINK,
	ST_LINK_RANGE_X,
	ST_LINK_RANGE_Y,
	ST_MAGNIFICATION,
	ST_GROUP_JUDGE,
	ST_HORIZON,
	ST_SURFACE_NG_SIZE,
	ST_NG_ALARM_SETTING,
	ST_CONTINUE_ALRAM,
	ST_SECTION_ALARM,
	ST_FOIL_DEF_REMOVE,
	ST_FOIL_DEF_REMOVE_DISTANCE,
	ST_FOIL_DEF_REMOVE_SIZE,
	ST_GRAY_SIZE,
	CHK_NG_STOP,
	CHK_DISABLE_PET,
	ST_PET_MATRIX,
	ST_PET_THRESHOLD,
	PET_CHECK_CNT,
	RAD_DARK_ROLL,
	RAD_BRIGHT_ROLL,
	STATIC_ROOL_MODE,
	STATIC_TAB_BRIGHT,
	STATIC_LOWEST,
	MAX_COUNT
};

static int UiText1NameText[] =
{
	IDC_ST_GROUP_RCP_SET,
	IDC_ST_RECIPE_NAME,
	IDC_ST_RECIPE_NAME2,
	IDC_ST_RECIPE_NO,
	IDC_BTN_REGIST,
	IDC_BTN_DELETE,
	IDC_BTN_PROGRAMINFO,
	IDC_BTN_LOAD,
	IDC_ST_OTHER_SETTING,
	IDC_STATIC_IMAGESAVEINTERVAL,
	IDC_CHK_SAVE_ONLY_NG_TAB,
	IDC_CHK_DISABLE_PROC_DIFF,
	IDC_ST_GROUP_TAB_INFO,
	IDC_ST_INPUT_MM,
	IDC_ST_INPUT_PIX,
	IDC_GROUP_CATHODE,
	IDC_ST_COAT_HEIGHT,
	IDC_STATIC_GROOVE_HEIGHT,
	IDC_ST_TAB_WIDTH,
	IDC_ST_TAB_PITCH,
	IDC_ST_ROUND_RADIUS,
	IDC_GROUP_ANODE,
	IDC_ST_TOP_COAT_HEIGHT,
	IDC_ST_BTM_COAT_HEIGHT,
	IDC_ST_TAB_COAT_HEIGHT,
	IDC_ST_EDGE_DETECT_COND,
	IDC_ST_COAT_BRIGHT,
	IDC_ST_ROLL_BRIGHT,
	IDC_ST_LIGHT_SETTING,
	IDC_ST_MARKING_SETTING,
	IDC_ST_DETECT_INFO,
	IDC_ST_THRES_ORG,
	IDC_ST_THRES_LOWER,
	IDC_ST_INSP_WIDTH,
	IDC_ST_DET_MIN_SIZE,
	IDC_ST_MASK_OFFSET,
	IDC_ST_GROUP_SPETTER,
	IDC_CHK_DISABLE_SPTTER,
	IDC_ST_THRES_SURFACE,
	IDC_ST_MASK_SURFACE,
	IDC_ST_MIN_SIZE_SURFACE,
	IDC_CHK_ENABLE_DEF_LINK,
	IDC_ST_LINK_RANGE_X,
	IDC_ST_LINK_RANGE_Y,
	IDC_ST_MAGNIFICATION,
	IDC_ST_GROUP_JUDGE,
	IDC_ST_HORIZON,
	IDC_ST_SURFACE_NG_SIZE,
	IDC_ST_NG_ALARM_SETTING,
	IDC_ST_CONTINUE_ALRAM,
	IDC_ST_SECTION_ALARM,
	IDC_ST_FOIL_DEF_REMOVE,
	IDC_ST_FOIL_DEF_REMOVE_DISTANCE,
	IDC_ST_FOIL_DEF_REMOVE_SIZE,
	IDC_ST_GRAY_SIZE,
	IDC_CHK_NG_STOP,
	IDC_CHK_DISABLE_PET,
	IDC_ST_PET_MATRIX,
	IDC_ST_PET_THRESHOLD,
	IDC_ST_PET_CHECK_CNT,
	IDC_RAD_DARK_ROLL,
	IDC_RAD_BRIGHT_ROLL,
	IDC_STATIC_ROOL_MODE,
	IDC_STATIC_TAB_BRIGHT,
	IDC_STATIC_LOWEST,
};

static CString UiText2[][3] =
{
	{_T("1탭분사"), _T("1 Tab"), _T("单喷1片")},
	{_T("3탭분사"), _T("3 Tab"), _T("单喷3片")},
};


static CString UiText3[][3] =
{
	{_T("번호"), _T("No."), _T("编号")},
	{_T("레시피 명"), _T("Recipe Name"), _T("Recipe名")},
	{_T("메모"), _T("Memo"), _T("备注")},
};


IMPLEMENT_DYNAMIC(CRecipeSettingDlg, CDialogEx)

CRecipeSettingDlg::CRecipeSettingDlg(BOOL bRcpSelMode, CRecipeInfo* pRecipeInfo, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RECIPE_SETTING, pParent)
	, m_nCoatBrightMax(0)
	, m_nRollBrightMax(0)
	, m_nFoilLinkX(0)
	, m_nFoilLinkY(0)
	, m_nDrossLinkX(0)
	, m_nDrossLinkY(0)
	//, m_nRadEdgeDetectMode(0)
	, m_nBmpSaveInterval(0)
	, m_bChkSaveOnlyNgTab(FALSE)
	, m_dEdMagnification(0)
	, m_bChkEnableDefLink(FALSE)
	, m_dEdNgSizeHeight(0)
	, m_bChkNgStop(FALSE)
	, m_bChkDarkEmpMode(FALSE)
	, m_nEdRecipeNo(-1)
	, m_strEdRecipeName(_T(""))
	, m_dEdSaveDefSize(0.0)
	, m_dEdNegTabCoatHeight(0.0)
	, m_nEdNegTabCoatHeightPix(0)
	, m_dEdNegVGrooveHeight(0.0)
	, m_nEdNegVGrooveHeightPix(0)
	, m_dEdIgnoreDistance(0)
	, m_dEdIgnoreSize(0)
	, m_nEdThresSurface(0)
	, m_nEdSurfaceMaskOffset(0)
	, m_bChkDisableSurface(FALSE)
	, m_nEdSurfaceMinSize(0)
	, m_nContinuousNgAlarmCnt(0)
	, m_nAlarmCnt(0)
	, m_nSectorCnt(0)
	, m_dEdFoilGraySize(0)
	, m_dEdSurfaceGraySize(0)
	, m_nTabMinBright(0)
	, m_nRollBrightMode(0)
	, m_strRecipeMemo(_T(""))
	, m_bChkDisablePET(FALSE)
	, m_nEdPetMatrixX(0)
	, m_nEdPetMatrixY(0)
	, m_nEdPetThreshold(0)
	, m_nEdPetCheckCnt(0)
	, m_bChkEnableVGroove(FALSE)

{
	m_bRcpSelMode = bRcpSelMode ;

	m_pRecipeInfo = pRecipeInfo;
	m_dTabWidth = 0.0;
	m_dTabPitch = 0.0;
	m_dRadius = 0.0;
	m_dTopCoatH = 0.0;
	m_dBtmCoatH = 0.0;
	m_dTabCoatH = 0.0;
	m_nCoatBrightMin = 0;
	m_nRollBrightMin = 0;
	m_nThresFoilLower = 0;
	m_nThresDrossLower = 0;
	m_pRecipeCtrl = NULL;
	m_nLastSelTab = 0; 
	m_nLastSelTabInfo = 0;
	m_bRecipeTableChanged = FALSE;
	m_strSelectRcpName = _T("");
	m_pFontGrid = NULL;
	// 23.02.14 Ahn Add Start
	m_nTabMinBright = 50 ;
	// 23.02.14 Ahn Add End

	for( int i=0; i<MAX_CAMERA_NO; i++ )
	{
		m_dEdNgSizeWidth[i] = 0.f;
		m_dEdFoilExpOutNgSize[i] = 0.f;
		m_dEdFoilExpBothNgSize[i] = 0.f;
		m_dSurfaceNgSize[i] = 0.f;	
		m_dEdDefectYSize[i] = 0.f;
	}
}

CRecipeSettingDlg::~CRecipeSettingDlg()
{
	if(m_pRecipeCtrl != NULL ){
		delete m_pRecipeCtrl;
		m_pRecipeCtrl = NULL;
	}

	if (m_pFontGrid != NULL) {
		delete m_pFontGrid;
		m_pFontGrid = NULL;
	}
}

void CRecipeSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_TAB_WIDTH_MM, m_dTabWidth);
	DDX_Control(pDX, IDC_ED_TEMP, m_EdTemp);
	DDX_Text(pDX, IDC_ED_TAB_PITCH_MM, m_dTabPitch);
	DDX_Text(pDX, IDC_ED_TAB_RADIUS_MM, m_dRadius);
	DDX_Text(pDX, IDC_ED_TOP_COAT_HEIGHT_MM, m_dTopCoatH);
	DDX_Text(pDX, IDC_ED_BTM_COAT_HEIGHT_MM, m_dBtmCoatH);
	DDX_Text(pDX, IDC_ED_TAB_COAT_HEIGHT_MM, m_dTabCoatH);
	DDX_Text(pDX, IDC_ED_COAT_BRIGHT, m_nCoatBrightMin);
	DDX_Text(pDX, IDC_ED_ROLL_BRIGHT, m_nRollBrightMin);
	DDX_Text(pDX, IDC_ED_COAT_BRIGHT_HIGH, m_nCoatBrightMax);
	DDX_Text(pDX, IDC_ED_ROLL_BRIGHT_HIGH, m_nRollBrightMax);

	DDX_Text(pDX, IDC_ED_TAB_WIDTH_PIX, m_nTabWidthPix);
	DDX_Text(pDX, IDC_ED_TAB_PITCH_PIX, m_nTabPitchPix);
	DDX_Text(pDX, IDC_ED_TAB_RADIUS_PIX, m_nRadiusPixW);
	DDX_Text(pDX, IDC_ED_TAB_RADIUS_PIX_H, m_nRadiusPixH);
	DDX_Text(pDX, IDC_ED_TOP_COAT_HEIGHT_PIX, m_nTopCoatPixH);
	DDX_Text(pDX, IDC_ED_BTM_COAT_HEIGHT_PIX, m_nBtmCoatPixH);
	DDX_Text(pDX, IDC_ED_TAB_COAT_HEIGHT_PIX, m_nTabCoatPixH);

	DDX_Text(pDX, IDC_ED_FOIL_THRES_LOW, m_nThresFoilLower);
	DDX_Text(pDX, IDC_ED_DROSS_THRES_LOW, m_nThresDrossLower);
	DDX_Text(pDX, IDC_ED_MIN_SIZE_FOIL, m_nMinSizeFoil);
	DDX_Text(pDX, IDC_ED_MIN_SIZE_DROSS, m_nMinSizeDross);
	DDX_Text(pDX, IDC_ED_MASK_OFFSET_FOIL, m_nMaskOffsetFoil);
	DDX_Text(pDX, IDC_ED_MASK_OFFSET_DROSS, m_nMaskOffsetDross);
	DDX_Text(pDX, IDC_ED_INSP_RANGE_IN, m_dInspWidthFoil);
	DDX_Text(pDX, IDC_ED_INSP_RANGE_OUT, m_dInspWidthDross);
	DDX_Text(pDX, IDC_ED_FOIL_THRES_UPPER, m_nThresFoilOrigin);
	DDX_Text(pDX, IDC_ED_DROSS_THRES_UPPER, m_nThresDrossOrigin);

	//DDX_Control(pDX, IDC_GRID_CTRL_JUDGE, m_GridCtrlJudge);
	// 22.07.15 Ahn Add Start
	DDX_Control(pDX, IDC_GRID_CTRL_MARKING, m_GridMarking);
	// 22.07.15 Ahn Add End
	DDX_Control(pDX, IDC_GRID_CTRL_LIGHT, m_GridCtrlLight);
	DDX_Control(pDX, IDC_GRID_RECIPE_TABLE, m_GridRecipeTable);
	DDX_Text(pDX, IDC_FOIL_LINK_RANGE_X, m_nFoilLinkX);
	DDX_Text(pDX, IDC_FOIL_LINK_RANGE_Y, m_nFoilLinkY);
	DDX_Text(pDX, IDC_DROSS_LINK_RANGE_X, m_nDrossLinkX);
	DDX_Text(pDX, IDC_DROSS_LINK_RANGE_Y, m_nDrossLinkY);
	DDX_Control(pDX, IDC_TAB_DETECT_COND, m_TabDetectCond);
	DDX_Text(pDX, IDC_ED_BMP_SAVE_INTERVAL, m_nBmpSaveInterval);
	DDX_Control(pDX, IDC_TAB_INFO, m_TabInfo);
	DDX_Check(pDX, IDC_CHK_SAVE_ONLY_NG_TAB, m_bChkSaveOnlyNgTab);
	DDX_Check(pDX, IDC_CHK_DISABLE_PROC_DIFF, m_bChkDisableProcDiff);
	DDX_Text(pDX, IDC_ED_MAGNIFICATION, m_dEdMagnification);
	DDX_Check(pDX, IDC_CHK_ENABLE_DEF_LINK, m_bChkEnableDefLink);
	DDX_Check(pDX, IDC_CHK_NG_STOP, m_bChkNgStop);
	DDX_Check(pDX, IDC_CHK_DARK_EMP_MODE, m_bChkDarkEmpMode);
	DDX_Text(pDX, IDC_ED_RECIPE_NO, m_nEdRecipeNo);
	DDX_Text(pDX, IDC_EDIT_RECIPE_NAME, m_strEdRecipeName);
	DDX_Check(pDX, IDC_CHK_SAVE_DEF_IN_TAB, m_bChkSaveDefInTab);
	DDX_Text(pDX, IDC_ED_SAVE_DEF_SIZE, m_dEdSaveDefSize);
	DDX_Text(pDX, IDC_ED_NEGA_TAB_COAT_HEIGHT, m_dEdNegTabCoatHeight);
	DDX_Text(pDX, IDC_ED_NEGA_TAB_COAT_HEIGHT_PIX, m_nEdNegTabCoatHeightPix);
	DDX_Text(pDX, IDC_ED_V_GROOVE_HEIGHT, m_dEdNegVGrooveHeight);
	DDX_Text(pDX, IDC_ED_V_GROOVE_HEIGHT_PIX, m_nEdNegVGrooveHeightPix);
	DDX_Text(pDX, IDC_ED_IGNORE_DISTANCE, m_dEdIgnoreDistance);
	// 22.03.07 Ahn Add Start
	DDX_Text(pDX, IDC_ED_IGNORE_SIZE, m_dEdIgnoreSize);
	// 22.03.07 Ahn Add End
	DDX_Text(pDX, IDC_ED_THRES_SPETTER, m_nEdThresSurface);
	DDX_Text(pDX, IDC_ED_SPT_MASK, m_nEdSurfaceMaskOffset);
	DDX_Check(pDX, IDC_CHK_DISABLE_SPTTER, m_bChkDisableSurface);

	DDX_Text(pDX, IDC_ED_SURFACE_MINSIZE, m_nEdSurfaceMinSize);
	DDX_Text(pDX, IDC_ED_NG_Y_SIZE, m_dEdNgSizeHeight);
	// 22.08.09 Ahn Add Start
	DDX_Text(pDX, IDC_ED_CONTINUOUS_ALARM, m_nContinuousNgAlarmCnt);
	DDX_Text(pDX, IDC_ED_ALARM_COUNT, m_nAlarmCnt);
	DDX_Text(pDX, IDC_ED_SECTOR_COUNT, m_nSectorCnt);
	// 22.08.09 Ahn Add End
	DDX_Text(pDX, IDC_ED_FOILEXP_GRAY_SIZE, m_dEdFoilGraySize);
	DDX_Text(pDX, IDC_ED_SURFACE_GRAY_SIZE, m_dEdSurfaceGraySize);
	DDX_Text(pDX, IDC_ED_TAB_MIN_BRIGHT, m_nTabMinBright);
	DDX_Radio(pDX, IDC_RAD_DARK_ROLL, m_nRollBrightMode);
	DDX_Text(pDX, IDC_EDIT_RECIPE_MEMO, m_strRecipeMemo);

	DDX_Text(pDX, IDC_ED_NG_X_SIZE, m_dEdNgSizeWidth[CAM_POS_TOP]);
	DDX_Text(pDX, IDC_ED_FOIL_OUT_NG_X_SIZE, m_dEdFoilExpOutNgSize[CAM_POS_TOP]);
	DDX_Text(pDX, IDC_ED_FOIL_BOTH_NG_X_SIZE, m_dEdFoilExpBothNgSize[CAM_POS_TOP]);
	DDX_Text(pDX, IDC_ED_SURFACE_NG_SIZE, m_dSurfaceNgSize[CAM_POS_TOP]);

	DDX_Text(pDX, IDC_ED_NG_X_SIZE_BTM, m_dEdNgSizeWidth[CAM_POS_BOTTOM]);
	DDX_Text(pDX, IDC_ED_FOIL_OUT_NG_X_SIZE_BTM, m_dEdFoilExpOutNgSize[CAM_POS_BOTTOM]);
	DDX_Text(pDX, IDC_ED_FOIL_BOTH_NG_X_SIZE_BTM, m_dEdFoilExpBothNgSize[CAM_POS_BOTTOM]);
	DDX_Text(pDX, IDC_ED_SURFACE_NG_SIZE_BTM, m_dSurfaceNgSize[CAM_POS_BOTTOM]);

	DDX_Check(pDX, IDC_CHK_DISABLE_PET, m_bChkDisablePET);
	DDX_Text(pDX, IDC_ED_PET_MATRIX_X, m_nEdPetMatrixX);
	DDX_Text(pDX, IDC_ED_PET_MATRIX_Y, m_nEdPetMatrixY);
	DDX_Text(pDX, IDC_ED_PET_THRESHOLD, m_nEdPetThreshold);
	DDX_Text(pDX, IDC_ED_PET_CHECK_CNT, m_nEdPetCheckCnt);

	DDX_Text(pDX, IDC_ED_FOIL_DEFECT_Y_SIZE_TOP, m_dEdDefectYSize[CAM_POS_TOP]);
	DDX_Text(pDX, IDC_ED_FOIL_DEFECT_Y_SIZE_BTM, m_dEdDefectYSize[CAM_POS_BOTTOM]);

	DDX_Check(pDX, IDC_CHK_ENABLE_V_GROOVE, m_bChkEnableVGroove);


}


BEGIN_MESSAGE_MAP(CRecipeSettingDlg, CDialogEx)
	ON_EN_SETFOCUS(IDC_ED_TAB_WIDTH_MM, &CRecipeSettingDlg::OnEnSetfocusEdTabWidthMm)
	ON_EN_SETFOCUS(IDC_ED_TAB_PITCH_MM, &CRecipeSettingDlg::OnEnSetfocusEdTabPitchMm)
	ON_EN_SETFOCUS(IDC_ED_TAB_RADIUS_MM, &CRecipeSettingDlg::OnEnSetfocusEdTabRadiusMm)
	ON_EN_SETFOCUS(IDC_ED_TOP_COAT_HEIGHT_MM, &CRecipeSettingDlg::OnEnSetfocusEdTopCoatHeightMm)
	ON_EN_SETFOCUS(IDC_ED_BTM_COAT_HEIGHT_MM, &CRecipeSettingDlg::OnEnSetfocusEdBtmCoatHeightMm)
	ON_EN_SETFOCUS(IDC_ED_TAB_COAT_HEIGHT_MM, &CRecipeSettingDlg::OnEnSetfocusEdTabCoatHeightMm)
	ON_EN_SETFOCUS(IDC_ED_COAT_BRIGHT, &CRecipeSettingDlg::OnEnSetfocusEdCoatBright)
	ON_EN_SETFOCUS(IDC_ED_ROLL_BRIGHT, &CRecipeSettingDlg::OnEnSetfocusEdRollBright)
	ON_EN_SETFOCUS(IDC_ED_COAT_BRIGHT_HIGH, &CRecipeSettingDlg::OnEnSetfocusEdCoatBrightMax)
	ON_EN_SETFOCUS(IDC_ED_ROLL_BRIGHT_HIGH, &CRecipeSettingDlg::OnEnSetfocusEdRollBrightMax)
	ON_EN_SETFOCUS(IDC_ED_FOIL_THRES_LOW, &CRecipeSettingDlg::OnEnSetfocusEdFoilThresLow)
	ON_EN_SETFOCUS(IDC_ED_DROSS_THRES_LOW, &CRecipeSettingDlg::OnEnSetfocusEdDrossThresLow)
	ON_EN_SETFOCUS(IDC_ED_FOIL_THRES_UPPER, &CRecipeSettingDlg::OnEnSetfocusEdFoilThresUpper)
	ON_EN_SETFOCUS(IDC_ED_DROSS_THRES_UPPER, &CRecipeSettingDlg::OnEnSetfocusEdDrossThresUpper)
	ON_EN_SETFOCUS(IDC_ED_MIN_SIZE_FOIL, &CRecipeSettingDlg::OnEnSetfocusEdMinSizeFoil)
	ON_EN_SETFOCUS(IDC_ED_MIN_SIZE_DROSS, &CRecipeSettingDlg::OnEnSetfocusEdMinSizeDross)
	ON_EN_SETFOCUS(IDC_ED_MASK_OFFSET_FOIL, &CRecipeSettingDlg::OnEnSetfocusEdMaskOffsetFoil)
	ON_EN_SETFOCUS(IDC_ED_MASK_OFFSET_DROSS, &CRecipeSettingDlg::OnEnSetfocusEdMaskOffsetDross)
	ON_EN_SETFOCUS(IDC_ED_INSP_RANGE_IN, &CRecipeSettingDlg::OnEnSetfocusEdInspRangeIn)
	ON_EN_SETFOCUS(IDC_ED_INSP_RANGE_OUT, &CRecipeSettingDlg::OnEnSetfocusEdInspRangeOut)
	ON_BN_CLICKED(IDOK, &CRecipeSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CRecipeSettingDlg::OnBnClickedBtnSave)
	ON_NOTIFY(NM_CLICK, IDC_GRID_CTRL_LIGHT, &CRecipeSettingDlg::OnClickGridCtrlLight)			// 22.07.15 Ahn Add
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_CTRL_MARKING, &CRecipeSettingDlg::OnMouseDblClickMarkingList)	// 22.07.15 Ahn Add 
	ON_NOTIFY(NM_CLICK, IDC_GRID_RECIPE_TABLE, &CRecipeSettingDlg::OnClickGridCtrlRecipeTable)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DETECT_COND, &CRecipeSettingDlg::OnTcnSelchangeTabDetectCond)
	ON_EN_SETFOCUS(IDC_FOIL_LINK_RANGE_X, &CRecipeSettingDlg::OnEnSetfocusFoilLinkRangeX)
	ON_EN_SETFOCUS(IDC_DROSS_LINK_RANGE_X, &CRecipeSettingDlg::OnEnSetfocusDrossLinkRangeX)
	ON_EN_SETFOCUS(IDC_FOIL_LINK_RANGE_Y, &CRecipeSettingDlg::OnEnSetfocusFoilLinkRangeY)
	ON_EN_SETFOCUS(IDC_DROSS_LINK_RANGE_Y, &CRecipeSettingDlg::OnEnSetfocusDrossLinkRangeY)
	ON_EN_SETFOCUS(IDC_ED_BMP_SAVE_INTERVAL, &CRecipeSettingDlg::OnEnSetfocusEdBmpSaveInterval)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_INFO, &CRecipeSettingDlg::OnTcnSelchangeTabInfo)
	ON_BN_CLICKED(IDC_CHK_SAVE_ONLY_NG_TAB, &CRecipeSettingDlg::OnBnClickedChkSaveOnlyNgTab)
	ON_BN_CLICKED(IDC_CHK_DISABLE_PROC_DIFF, &CRecipeSettingDlg::OnBnClickedChkDisableProcDiff)
	ON_EN_SETFOCUS(IDC_ED_MAGNIFICATION, &CRecipeSettingDlg::OnEnSetfocusEdMagnification)
	ON_BN_CLICKED(IDC_CHK_ENABLE_DEF_LINK, &CRecipeSettingDlg::OnBnClickedChkEnableDefLink)
	ON_EN_SETFOCUS(IDC_ED_NG_Y_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdNgYSize)
	ON_BN_CLICKED(IDC_CHK_NG_STOP, &CRecipeSettingDlg::OnBnClickedChkNgStop)
	ON_BN_CLICKED(IDC_CHK_DARK_EMP_MODE, &CRecipeSettingDlg::OnBnClickedChkDarkEmpMode)
	ON_BN_CLICKED(IDC_BTN_REGIST, &CRecipeSettingDlg::OnBnClickedBtnRegist)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CRecipeSettingDlg::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CRecipeSettingDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_CHK_SAVE_DEF_IN_TAB, &CRecipeSettingDlg::OnBnClickedChkSaveDefInTab)
	ON_EN_SETFOCUS(IDC_ED_SAVE_DEF_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdSaveDefSize)
	ON_EN_SETFOCUS(IDC_ED_NEGA_TAB_COAT_HEIGHT, &CRecipeSettingDlg::OnEnSetfocusEdNegaTabCoatHeight)
	ON_EN_SETFOCUS(IDC_ED_V_GROOVE_HEIGHT, &CRecipeSettingDlg::OnEnSetfocusEdVGrooveHeight)
	ON_EN_SETFOCUS(IDC_ED_IGNORE_DISTANCE, &CRecipeSettingDlg::OnEnSetfocusEdIgnoreDistance)
	ON_EN_SETFOCUS(IDC_ED_IGNORE_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdIgnoreSize)
	ON_EN_SETFOCUS(IDC_ED_THRES_SPETTER, &CRecipeSettingDlg::OnEnSetfocusEdThresSurface)
	ON_EN_SETFOCUS(IDC_ED_SPT_MASK, &CRecipeSettingDlg::OnEnSetfocusEdSptMask)
	ON_BN_CLICKED(IDC_CHK_DISABLE_SPTTER, &CRecipeSettingDlg::OnBnClickedChkDisableSurface)
	ON_EN_SETFOCUS(IDC_ED_SURFACE_MINSIZE, &CRecipeSettingDlg::OnEnSetfocusEdSurfaceMinsize)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, &CRecipeSettingDlg::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_SYSTEM_SET, &CRecipeSettingDlg::OnBnClickedBtnSystemSet)
	ON_EN_SETFOCUS(IDC_ED_CONTINUOUS_ALARM, &CRecipeSettingDlg::OnEnSetfocusEdContinuousAlarm)
	ON_EN_SETFOCUS(IDC_ED_ALARM_COUNT, &CRecipeSettingDlg::OnEnSetfocusEdAlarmCount)
	ON_EN_SETFOCUS(IDC_ED_SECTOR_COUNT, &CRecipeSettingDlg::OnEnSetfocusEdSectorCount)
	ON_WM_SHOWWINDOW()
	ON_EN_SETFOCUS(IDC_ED_FOILEXP_GRAY_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdFoilexpGraySize)
	ON_EN_SETFOCUS(IDC_ED_SURFACE_GRAY_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdSurfaceGraySize)
	ON_EN_SETFOCUS(IDC_ED_TAB_MIN_BRIGHT, &CRecipeSettingDlg::OnEnSetfocusEdTabMinBright)
	ON_BN_CLICKED(IDC_BTN_PROGRAMINFO, &CRecipeSettingDlg::OnBnClickedBtnPrograminfo)

	ON_EN_SETFOCUS(IDC_ED_NG_X_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdNgXSize)
	ON_EN_SETFOCUS(IDC_ED_FOIL_OUT_NG_X_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdFoilOutNgXSize)
	ON_EN_SETFOCUS(IDC_ED_FOIL_BOTH_NG_X_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdFoilBothNgXSize)
	ON_EN_SETFOCUS(IDC_ED_SURFACE_NG_SIZE, &CRecipeSettingDlg::OnEnSetfocusEdSurfaceNgSize)

	ON_EN_SETFOCUS(IDC_ED_NG_X_SIZE_BTM, &CRecipeSettingDlg::OnSetfocusEdNgXSizeBtm)
	ON_EN_SETFOCUS(IDC_ED_FOIL_OUT_NG_X_SIZE_BTM, &CRecipeSettingDlg::OnSetfocusEdFoilOutNgXSizeBtm)
	ON_EN_SETFOCUS(IDC_ED_FOIL_BOTH_NG_X_SIZE_BTM, &CRecipeSettingDlg::OnSetfocusEdFoilBothNgXSizeBtm)
	ON_EN_SETFOCUS(IDC_ED_SURFACE_NG_SIZE_BTM, &CRecipeSettingDlg::OnSetfocusEdSurfaceNgSizeBtm)
	ON_BN_CLICKED(IDC_CHK_DISABLE_PET, &CRecipeSettingDlg::OnBnClickedChkDisablePet)
	ON_EN_SETFOCUS(IDC_ED_PET_MATRIX_X, &CRecipeSettingDlg::OnSetfocusEdPetMatrixX)
	ON_EN_SETFOCUS(IDC_ED_PET_MATRIX_Y, &CRecipeSettingDlg::OnSetfocusEdPetMatrixY)
	ON_EN_SETFOCUS(IDC_ED_PET_THRESHOLD, &CRecipeSettingDlg::OnSetfocusEdPetThreshold)
	ON_EN_SETFOCUS(IDC_ED_PET_CHECK_CNT, &CRecipeSettingDlg::OnSetfocusEdPetCheckCnt)
	ON_EN_SETFOCUS(IDC_ED_FOIL_DEFECT_Y_SIZE_TOP, &CRecipeSettingDlg::OnSetfocusEdFoilDefectYSizeTop)
	ON_EN_SETFOCUS(IDC_ED_FOIL_DEFECT_Y_SIZE_BTM, &CRecipeSettingDlg::OnSetfocusEdFoilDefectYSizeBtm)
	ON_BN_CLICKED(IDC_CHK_ENABLE_V_GROOVE, &CRecipeSettingDlg::OnBnClickedChkEnableVGroove)
END_MESSAGE_MAP()


// CRecipeSettingDlg 메시지 처리기

BOOL CRecipeSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CRecipeCtrl rcpCtrl;
	if (m_pRecipeCtrl == NULL) {
		m_pRecipeCtrl = new CRecipeCtrl();
		if (m_pRecipeInfo == NULL) {
			m_pRecipeInfo = m_pRecipeCtrl->GetRecipeInfoPtr();
		}
	}

//	CRecipeCtrl::LoadRecipe()
	//CStringList strList;
	//m_pRecipeCtrl->SearchRecipeFile(strList);
	//int i = 0;
	//int nCount = (int)strList.GetSize();
	//m_cmbRecipeName.Clear();
	//POSITION pos;
	//pos = strList.GetHeadPosition();
	//CString strRecipeName;
	//int nSelNo = -1;
	//for (i = 0; i < nCount; i++) {
	//	strRecipeName = strList.GetNext(pos);
	//	strRecipeName.Replace(_T(".ini"), _T(""));
	//	m_cmbRecipeName.AddString(strRecipeName);
	//	if (strRecipeName.Compare(m_pRecipeInfo->m_strRecipeName) == 0) {
	//		nSelNo = i;
	//	}
	//}

	m_TabDetectCond.InsertItem(1, _T("TAB Cond"));
	m_TabDetectCond.InsertItem(2, _T("BTM Cond"));

	m_TabInfo.InsertItem(1, _T("TAB Cond"));
	m_TabInfo.InsertItem(2, _T("BTM Cond"));

	DisplayLanguage(); // 22.09.16 Ahn Add 

	//m_GridCtrlRank.SetRowHeight(0, 20);
	//m_GridCtrlJudge.SetRowHeight(0, 20);
	m_GridCtrlLight.SetRowHeight(0, 20);
	m_GridRecipeTable.SetRowHeight(0, 20);

	DataControl( MODE_READ, m_pRecipeInfo);
	//m_cmbRecipeName.SetWindowTextA(m_pRecipeInfo->m_strRecipeName);
	m_strEdRecipeName = m_pRecipeInfo->m_strRecipeName;
	MakeGridCtrl();

	LoadRecipeTable();
	m_nEdRecipeNo = m_RcpTableCtrl.GetRecipeNo(m_strEdRecipeName);
	MakeGridCtrl_RecipeTable();

	MakeGridCtrl_Marking();	// 22.07.19 Ahn Add 
	UpdateGrid_Marking();	// 22.07.19 Ahn Add 

	ShowControl();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


int CRecipeSettingDlg::SetValue(int nValue, CString strMsg, int nMax, int nMin)
{
	CSetValue setValue(this);
	m_EdTemp.SetFocus();
	setValue.SetValue(strMsg, nValue, nMax, nMin);
	int nRetV = nValue;

	return nRetV;
}


double CRecipeSettingDlg::SetValue(double dValue, CString strMsg, double dMax, double dMin)
{
	CSetValue setValue(this);
	m_EdTemp.SetFocus();
	setValue.SetValue(strMsg, dValue, dMax, dMin);
	double dRetV = dValue;

	return dRetV;
}


void CRecipeSettingDlg::OnEnSetfocusEdTabWidthMm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 100.0;
	dMin = 9.9;
	dValue = m_dTabWidth;
	CString strMsg;
//	strMsg.Format(_T("탭 폭을 입력해 주세요. 범위( %.3lf ~ %.3lf )"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf ~ %.3lf )"), dMin, dMax);
	m_dTabWidth = SetValue(dValue, strMsg, dMax, dMin);

	m_nTabWidthPix = (int)( (m_dTabWidth / AprData.m_System.m_dResolY * 1000 ) + 0.5 ) ;
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdTabPitchMm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 200.0;
	dMin = 9.9;
	dValue = m_dTabPitch;
	CString strMsg;
//	strMsg.Format(_T("탭 피치를 입력해 주세요. 범위( %.3lf ~ %.3lf )"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf ~ %.3lf )"), dMin, dMax);
	m_dTabPitch = SetValue(dValue, strMsg, dMax, dMin);

	m_nTabPitchPix = (int)(((m_dTabPitch / AprData.m_System.m_dResolY) * 1000.0) + 0.5 );
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdTabRadiusMm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 9.9;
	dMin = 0.9;
	dValue = m_dRadius;
	CString strMsg;
//	strMsg.Format(_T("라운드의 반지름 입력해 주세요. 범위( %.3lf ~ %.3lf )"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf ~ %.3lf )"), dMin, dMax);
	m_dRadius = SetValue(dValue, strMsg, dMax, dMin);

	m_nRadiusPixH = (int)(((m_dRadius / AprData.m_System.m_dResolY) * 1000.0) + 0.5 );
	m_nRadiusPixW = (int)(((m_dRadius / AprData.m_System.m_dResolX[CAM_POS_TOP]) * 1000.0) + 0.5 );
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdTopCoatHeightMm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 20.0;
	dMin = 0.0;//0.9;
	dValue = m_dTopCoatH;
	CString strMsg;
//	strMsg.Format(_T("탑 검사부의 코팅 높이 입력해 주세요. 범위( %.3lf ~ %.3lf )"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf ~ %.3lf )"), dMin, dMax);
	m_dTopCoatH = SetValue(dValue, strMsg, dMax, dMin);

	m_nTopCoatPixH = (int)(((m_dTopCoatH / AprData.m_System.m_dResolX[ CAM_POS_TOP ]) * 1000.0) + 0.5 );
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdBtmCoatHeightMm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 9.9;
	dMin = 0.0;//0.9;
	dValue = m_dBtmCoatH;
	CString strMsg;
//	strMsg.Format(_T("버틈 검사부의 코팅 높이 입력해 주세요. 범위( %.3lf ~ %.3lf )"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf ~ %.3lf )"), dMin, dMax);
	m_dBtmCoatH = SetValue(dValue, strMsg, dMax, dMin);

	m_nBtmCoatPixH = (int)(((m_dBtmCoatH / AprData.m_System.m_dResolX[ CAM_POS_TOP ]) * 1000.0) + 0.5 );
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdTabCoatHeightMm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 50.0;
	dMin = 0.0;//0.9;
	dValue = this->m_dTabCoatH;
	CString strMsg;
//	strMsg.Format(_T("버틈 검사부의 코팅 높이 입력해 주세요. 범위( %.3lf ~ %.3lf )"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf ~ %.3lf )"), dMin, dMax);
	m_dTabCoatH = SetValue(dValue, strMsg, dMax, dMin);

	m_nTabCoatPixH = (int)(((m_dTabCoatH / AprData.m_System.m_dResolX[ CAM_POS_BOTTOM ]) * 1000.0) + 0.5);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdCoatBright()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nCoatBrightMin;
	CString strMsg;
//	strMsg.Format(_T("코팅부의 휘도값을 입력해 주세요. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nCoatBrightMin = SetValue(nValue, strMsg, nMax, nMin);

	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdRollBright()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nRollBrightMin;
	CString strMsg;
//	strMsg.Format(_T("롤 부위의 휘도값을 입력해 주세요. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nRollBrightMin = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}

void CRecipeSettingDlg::OnEnSetfocusEdCoatBrightMax()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nCoatBrightMax;
	CString strMsg;
//	strMsg.Format(_T("코팅부의 휘도값을 입력해 주세요. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nCoatBrightMax = SetValue(nValue, strMsg, nMax, nMin);

	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdRollBrightMax()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nRollBrightMax;
	CString strMsg;
//	strMsg.Format(_T("롤 부위의 휘도값을 입력해 주세요. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nRollBrightMax = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}

void CRecipeSettingDlg::OnEnSetfocusEdFoilThresUpper()
{
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = THRESHOLD_MIN_VALUE; // 22.07.07 Ahn Modify 
	nValue = m_nThresFoilOrigin;
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 검출 Origin 쓰레스홀드 레벨 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nThresFoilOrigin = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}
void CRecipeSettingDlg::OnEnSetfocusEdDrossThresUpper()
{
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = THRESHOLD_MIN_VALUE; // 22.07.07 Ahn Modify 
	nValue = m_nThresDrossOrigin;
	CString strMsg;
//	strMsg.Format(_T("Dross 검출 Origin 쓰레스홀드 레벨을 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nThresDrossOrigin = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}

void CRecipeSettingDlg::OnEnSetfocusEdFoilThresLow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = THRESHOLD_MIN_VALUE; // 22.07.07 Ahn Modify 
	nValue = m_nThresFoilLower;
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 검출 쓰레스 홀드 레벨을 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nThresFoilLower = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdDrossThresLow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = THRESHOLD_MIN_VALUE; // 22.07.07 Ahn Modify 
	nValue = m_nThresDrossLower;
	CString strMsg;
//	strMsg.Format(_T("Dross 검출 쓰레스 홀드 레벨을 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nThresDrossLower = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);

}


void CRecipeSettingDlg::OnEnSetfocusEdMinSizeFoil()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nMinSizeFoil;
	CString strMsg;
//	strMsg.Format(_T("Foil 검출 최소 검출 사이즈를 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nMinSizeFoil = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdMinSizeDross()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nMinSizeDross;
	CString strMsg;
//	strMsg.Format(_T("Dross 검출 최소 검출 사이즈를 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nMinSizeDross = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdMaskOffsetFoil()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nMaskOffsetFoil;
	CString strMsg;
//	strMsg.Format(_T("Foil 마스크 오프셋 사이즈를 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nMaskOffsetFoil = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdMaskOffsetDross()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nMaskOffsetDross;
	CString strMsg;
//	strMsg.Format(_T("Dross 마스크 오프셋 사이즈를 설정해 주세요.. 범위( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nMaskOffsetDross = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdInspRangeIn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCamNo = m_TabDetectCond.GetCurSel();
	double dValue, dMax, dMin;
	dMax = 10.0;
	dMin = 0.0;
	dValue = m_dInspWidthFoil;
	CString strMsg;
//	strMsg.Format(_T("Foil부 검사 영역을 설정해 주세요.. 범위( %.3lf mm ~ %.3lf mm)"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf mm ~ %.3lf mm)"), dMin, dMax);
	m_dInspWidthFoil = SetValue(dValue, strMsg, dMax, dMin);
	m_nInspWidthFoil = (int) ( (m_dInspWidthFoil / AprData.m_System.m_dResolX[nCamNo]) * 1000.0)  ;
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdInspRangeOut()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCamNo = m_TabDetectCond.GetCurSel();
	double dValue, dMax, dMin;
	dMax = 10.0;
	dMin = 0.0;
	dValue = m_dInspWidthDross;
	CString strMsg;
//	strMsg.Format(_T("Dross 검사 영역을 설정해 주세요.. 범위( %.3lf mm ~ %.3lf mm)"), dMin, dMax);
	strMsg.Format(_T("Range( %.3lf mm ~ %.3lf mm)"), dMin, dMax);
	m_dInspWidthDross = SetValue(dValue, strMsg, dMax, dMin);
	m_nInspWidthDross = (int)( (m_dInspWidthDross / AprData.m_System.m_dResolX[nCamNo]) * 1000 ) ;
	UpdateData(FALSE);
}

void CRecipeSettingDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelRecipNo = m_nEdRecipeNo - 1;
	if ((nSelRecipNo < 0) || (nSelRecipNo >= MAX_RECIPE_TABLE)) {

		MessageBox(_T("The recipe number is out of the range. Please check."));
		return;
	}

	CRecipeTableInfo* pRcpInfo = m_RcpTableCtrl.GetRecipeTableInfo(nSelRecipNo);
	CString strRecipeName = pRcpInfo->strRecipeName;
	if (pRcpInfo->strRecipeName.GetLength() == 0) {
		MessageBox(_T("The recipe is not registered. Please check."));
		return;
	}
	// 22.07.26 Ahn Add Start
	if (strRecipeName.Compare(AprData.m_NowLotData.m_strRecipeName) == 0) {
		CRecipeCtrl rcpCtrl;
		rcpCtrl.LoadRecipe(AprData.m_pRecipeInfo, strRecipeName);
	}
	else {
		MessageBox(_T("The recipe name you are trying to apply is not the same as the recipe name you are currently applying."));
	}
	// 22.07.26 Ahn Add End
	// 22.07.21 Ahn Delete Start
	//ASSERT(m_pRecipeCtrl);

	//CString strFileName;
	//strFileName.Format(_T("%s\\Recipe\\%s.ini"), AprData.m_strDataPath, pRcpInfo->strRecipeName);
	//if (CWin32File::ExistFile(strFileName) == FALSE) {
	//	MessageBox(_T("레시피가 레시피 폴더에 존재하지 않습니다. 확인해 주세요."));
	//	return;
	//}

	//m_strSelectRcpName = pRcpInfo->strRecipeName;
	// 22.07.21 Ahn Delete End

	// 22.07.21 Ahn Delete Start
	//CDialogEx::OnOK();
	// 22.07.21 Ahn Delete End
}

void CRecipeSettingDlg::DataControl(int nMode, CRecipeInfo* pRecipeInfo)
{
	ASSERT(pRecipeInfo);
	if (pRecipeInfo == nullptr) {
		return;
	}

	int nCamPos = m_nLastSelTab ;
	int nTabCamPos = m_nLastSelTabInfo;

	if (nMode == MODE_READ)
	{

		// 22.06.08 Ahn Modify Start
		m_dTabWidth = pRecipeInfo->TabCond.dTabWidth;
		m_dTabPitch =pRecipeInfo->TabCond.dTabPitch;
		m_dRadius = pRecipeInfo->TabCond.dRadius;
		m_dTopCoatH = pRecipeInfo->TabCond.dCeramicHeight;
		m_dBtmCoatH = pRecipeInfo->TabCond.dCeramicTailHeight;
		m_dTabCoatH = pRecipeInfo->TabCond.dTabCeramicHeight;

		m_nTabWidthPix = pRecipeInfo->TabCond.nTabWidth;
		m_nTabPitchPix = pRecipeInfo->TabCond.nTabPitch;
		m_nRadiusPixH = pRecipeInfo->TabCond.nRadiusH;
		m_nRadiusPixW = pRecipeInfo->TabCond.nRadiusW;
		m_nTopCoatPixH = pRecipeInfo->TabCond.nCeramicHeight;
		m_nBtmCoatPixH = pRecipeInfo->TabCond.nCeramicTailHeight;
		m_nTabCoatPixH = pRecipeInfo->TabCond.nTabCeramicHeight;
		// 22.06.08 Ahn Modify Start

		// 23.02.14 Ahn Add Start
		m_nTabMinBright = pRecipeInfo->TabCond.nTabMinBright;
		m_nRollBrightMode = pRecipeInfo->TabCond.nRollBrightMode[nTabCamPos] ;

		// 23.02.14 Ahn Add End
		m_nCoatBrightMin = pRecipeInfo->TabCond.nCeramicBrightLow[nTabCamPos];
		m_nCoatBrightMax = pRecipeInfo->TabCond.nCeramicBrightHigh[nTabCamPos];
		m_nRollBrightMin = pRecipeInfo->TabCond.nRollBrightLow[nTabCamPos];
		m_nRollBrightMax = pRecipeInfo->TabCond.nRollBrightHigh[nTabCamPos];
		//m_nRadEdgeDetectMode = pRecipeInfo->TabCond.nEdgeFindMode[nTabCamPos];

		//m_nRollBrightHigh;
		// Ceramic High 와 Roll Low도 추가 필요

		m_nTabWidthPix = pRecipeInfo->TabCond.nTabWidth;
		m_nTabPitchPix = pRecipeInfo->TabCond.nTabPitch;
		m_nRadiusPixW = pRecipeInfo->TabCond.nRadiusW;
		m_nRadiusPixH = pRecipeInfo->TabCond.nRadiusH;
		m_nTopCoatPixH = pRecipeInfo->TabCond.nCeramicHeight;
		m_nBtmCoatPixH = pRecipeInfo->TabCond.nCeramicTailHeight;
		m_nTabCoatPixH = pRecipeInfo->TabCond.nTabCeramicHeight;

		// 22.01.05 Ahn Add Start
		m_nEdNegTabCoatHeightPix = pRecipeInfo->TabCond.nNegCoatHeight;
		m_nEdNegVGrooveHeightPix = pRecipeInfo->TabCond.nNegVGrooveHeight;
		m_dEdNegTabCoatHeight = (double)pRecipeInfo->TabCond.nNegCoatHeight * AprData.m_System.m_dResolX[CAM_POS_TOP] / 1000.0;
		m_dEdNegVGrooveHeight = (double)pRecipeInfo->TabCond.nNegVGrooveHeight * AprData.m_System.m_dResolX[CAM_POS_TOP] / 1000.0;
		// 22.01.05 Ahn Add Start

		m_nThresFoilLower = pRecipeInfo->nFoilExpThresLower[nCamPos];
		m_nThresDrossLower = pRecipeInfo->nDrossThresLower[nCamPos];
		m_nThresFoilOrigin = pRecipeInfo->nFoilExpThresOrigin[nCamPos];
		m_nThresDrossOrigin = pRecipeInfo->nDrossThresOrigin[nCamPos];

		m_nMinSizeFoil = pRecipeInfo->nFoilExpInMinSize[nCamPos];
		m_nMinSizeDross = pRecipeInfo->nFoilExpOutMinSize[nCamPos];
		m_nMaskOffsetFoil = pRecipeInfo->nFoilExpMaskOffset[nCamPos];
		m_nMaskOffsetDross = pRecipeInfo->nFoilOutMaskOffset[nCamPos];

		m_dEdMagnification = pRecipeInfo->dMagnification[nCamPos];

		m_nFoilLinkX = pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP].x;
		m_nFoilLinkY = pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP].y;
		m_nDrossLinkX = pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP_OUT].x;
		m_nDrossLinkY = pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP_OUT].y;
		m_dInspWidthDross = pRecipeInfo->dFoilOutInspWidth[nCamPos];
		m_dInspWidthFoil = pRecipeInfo->dFoilExpInspWidth[nCamPos];

		m_nInspWidthDross = pRecipeInfo->nFoilOutInspWidth[nCamPos];
		m_nInspWidthFoil = pRecipeInfo->nFoilExpInspWidth[nCamPos];
		m_nBmpSaveInterval = pRecipeInfo->nBmpSaveInterval;

		m_bChkSaveOnlyNgTab = pRecipeInfo->bSaveOnlyNgTab;
		m_bChkDisableProcDiff = pRecipeInfo->bDisableProcDiff;
		m_bChkEnableDefLink = pRecipeInfo->bEnableDefectLink[nCamPos];

		m_bChkNgStop = pRecipeInfo->bNgStop;
		m_bChkDarkEmpMode = pRecipeInfo->bDarkEmpMode;
		m_dEdNgSizeHeight = pRecipeInfo->dDefJudgeHeight;

		m_dEdSaveDefSize = pRecipeInfo->dSaveDefSize;
		m_bChkSaveDefInTab = pRecipeInfo->bSaveDefInTab;
		m_dEdIgnoreDistance = pRecipeInfo->dIgnoreDistance;
		m_dEdIgnoreSize = pRecipeInfo->dIgnoreSize;
		m_nEdThresSurface = pRecipeInfo->nThresSurface[nCamPos];
		m_nEdSurfaceMaskOffset = pRecipeInfo->nSurfaceMaskOffset[nCamPos];
		m_bChkDisableSurface = pRecipeInfo->bDisableSurface;
		// 22.11.21 Ahn Add Start - JUDGE_GRAY
		m_dEdFoilGraySize = pRecipeInfo->dFoileGraySize;
		m_dEdSurfaceGraySize = pRecipeInfo->dSurfaceGraySize;
		// 22.11.21 Ahn Add End
		// 22.06.08 Ahn Add Start
		m_nEdSurfaceMinSize = pRecipeInfo->m_nSurfaceMinSize[nCamPos];
		// 22.06.08 Ahn Add End

		// 22.08.09 Ahn Add Start
		m_nContinuousNgAlarmCnt = pRecipeInfo->nContinousNgCount ;
		m_nAlarmCnt = pRecipeInfo->nAlarmCount ;
		m_nSectorCnt = pRecipeInfo->nSectorCount ;
		// 22.08.09 Ahn Add End


		m_bChkDisablePET = pRecipeInfo->bDisablePET;
		m_nEdPetMatrixX = pRecipeInfo->nPetMatrixX[nCamPos];
		m_nEdPetMatrixY = pRecipeInfo->nPetMatrixY[nCamPos];
		m_nEdPetThreshold = pRecipeInfo->nPetThreshold[nCamPos];
		m_nEdPetCheckCnt = pRecipeInfo->nPetCheckCnt[nCamPos];

		m_bChkEnableVGroove = pRecipeInfo->bEnableVGroove;


		for( int i=0; i<MAX_CAMERA_NO; i++ )
		{
			m_dEdNgSizeWidth[i] = pRecipeInfo->dFoilExpInNgSize[i];
			m_dEdFoilExpOutNgSize[i] = pRecipeInfo->dFoilExpOutNgSize[i];
			m_dEdFoilExpBothNgSize[i] = pRecipeInfo->dFoilExpBothNgSize[i];
			m_dSurfaceNgSize[i] = pRecipeInfo->dSurfaceNgSize[i];
			m_dEdDefectYSize[i] = pRecipeInfo->dDefectYSize[i];
		}


		UpdateData(FALSE);
	}
	else 
	{
		UpdateData(TRUE);

		// 23.02.14 Ahn Add Start
		pRecipeInfo->TabCond.nTabMinBright = m_nTabMinBright ;
		pRecipeInfo->TabCond.nRollBrightMode[nTabCamPos] = m_nRollBrightMode;
		// 23.02.14 Ahn Add End

		pRecipeInfo->TabCond.nCeramicBrightLow[nTabCamPos] = m_nCoatBrightMin ;
		pRecipeInfo->TabCond.nCeramicBrightHigh[nTabCamPos] = m_nCoatBrightMax;
		pRecipeInfo->TabCond.nRollBrightLow[nTabCamPos] = m_nRollBrightMin;
		pRecipeInfo->TabCond.nRollBrightHigh[nTabCamPos] = m_nRollBrightMax;
		//pRecipeInfo->TabCond.nEdgeFindMode[nTabCamPos] = m_nRadEdgeDetectMode;
		// Ceramic High 와 Roll Low도 추가 필요

		pRecipeInfo->TabCond.nTabWidth = m_nTabWidthPix ;
		pRecipeInfo->TabCond.nTabPitch = m_nTabPitchPix ;
		pRecipeInfo->TabCond.nRadiusW = m_nRadiusPixW ;
		pRecipeInfo->TabCond.nRadiusH = m_nRadiusPixH;
		pRecipeInfo->TabCond.nCeramicHeight = m_nTopCoatPixH ;
		pRecipeInfo->TabCond.nCeramicTailHeight = m_nBtmCoatPixH ;
		pRecipeInfo->TabCond.nTabCeramicHeight = m_nTabCoatPixH ;

		// 22.06.08 Ahn Add Start
		pRecipeInfo->TabCond.dTabWidth = m_dTabWidth;
		pRecipeInfo->TabCond.dTabPitch = m_dTabPitch;
		pRecipeInfo->TabCond.dRadius = m_dRadius;
		pRecipeInfo->TabCond.dCeramicHeight = m_dTopCoatH;
		pRecipeInfo->TabCond.dCeramicTailHeight = m_dBtmCoatH;
		pRecipeInfo->TabCond.dTabCeramicHeight = m_dTabCoatH;
		// 22.06.08 Ahn Add End


		// 22.01.05 Ahn Add Start
		pRecipeInfo->TabCond.nNegCoatHeight = m_nEdNegTabCoatHeightPix ;
		pRecipeInfo->TabCond.nNegVGrooveHeight = m_nEdNegVGrooveHeightPix ;
		// 22.01.05 Ahn Add Start

		pRecipeInfo->nFoilExpThresOrigin[nCamPos] = m_nThresFoilOrigin ;
		pRecipeInfo->nDrossThresOrigin[nCamPos] = m_nThresDrossOrigin;
		pRecipeInfo->nFoilExpThresLower[nCamPos] = m_nThresFoilLower ;
		pRecipeInfo->nDrossThresLower[nCamPos] = m_nThresDrossLower ;
		pRecipeInfo->nFoilExpInMinSize[nCamPos] = m_nMinSizeFoil;
		pRecipeInfo->nFoilExpOutMinSize[nCamPos] = m_nMinSizeDross ;
		pRecipeInfo->nFoilExpMaskOffset[nCamPos] = m_nMaskOffsetFoil ;
		pRecipeInfo->nFoilOutMaskOffset[nCamPos] = m_nMaskOffsetDross ;

		pRecipeInfo->dMagnification[nCamPos] = m_dEdMagnification ;

		pRecipeInfo->dFoilOutInspWidth[nCamPos] = m_dInspWidthDross ;
		pRecipeInfo->dFoilExpInspWidth[nCamPos] = m_dInspWidthFoil ;

		pRecipeInfo->nFoilOutInspWidth[nCamPos] = (int)(m_dInspWidthDross/AprData.m_System.m_dResolX[nCamPos]*1000.0);
		pRecipeInfo->nFoilExpInspWidth[nCamPos] = (int)(m_dInspWidthFoil / AprData.m_System.m_dResolX[nCamPos] * 1000.0);;
		
		pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP].x = m_nFoilLinkX;
		pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP].y = m_nFoilLinkY;
		pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP_OUT].x = m_nDrossLinkX ;
		pRecipeInfo->pntLinkRange[nCamPos][TYPE_FOILEXP_OUT].y = m_nDrossLinkY;

		pRecipeInfo->nBmpSaveInterval = m_nBmpSaveInterval ;

		pRecipeInfo->bSaveOnlyNgTab = m_bChkSaveOnlyNgTab ;
		pRecipeInfo->bDisableProcDiff = m_bChkDisableProcDiff ;
		pRecipeInfo->bEnableDefectLink[nCamPos] = m_bChkEnableDefLink ;

		pRecipeInfo->bNgStop = m_bChkNgStop ;
		pRecipeInfo->bDarkEmpMode = m_bChkDarkEmpMode ;

		for( int i=0; i<MAX_CAMERA_NO; i++ )
		{
			pRecipeInfo->dFoilExpInNgSize[i] = m_dEdNgSizeWidth[i];
			pRecipeInfo->dFoilExpOutNgSize[i] = m_dEdFoilExpOutNgSize[i];
			pRecipeInfo->dFoilExpBothNgSize[i] = m_dEdFoilExpBothNgSize[i];
			pRecipeInfo->dSurfaceNgSize[i] = m_dSurfaceNgSize[i];
			pRecipeInfo->dDefectYSize[i] = m_dEdDefectYSize[i];
		}


		pRecipeInfo->dFoileGraySize = m_dEdFoilGraySize ;
		pRecipeInfo->dSurfaceGraySize = m_dEdSurfaceGraySize ;

		pRecipeInfo->dSaveDefSize = m_dEdSaveDefSize ;
		pRecipeInfo->bSaveDefInTab = m_bChkSaveDefInTab ;
		pRecipeInfo->dIgnoreDistance = m_dEdIgnoreDistance ;
		pRecipeInfo->dIgnoreSize = m_dEdIgnoreSize ;
		pRecipeInfo->nThresSurface[nCamPos] = m_nEdThresSurface;
		pRecipeInfo->nSurfaceMaskOffset[nCamPos] = m_nEdSurfaceMaskOffset ;
		pRecipeInfo->bDisableSurface = m_bChkDisableSurface;

		pRecipeInfo->m_nSurfaceMinSize[nCamPos] = m_nEdSurfaceMinSize ;

		pRecipeInfo->nContinousNgCount = m_nContinuousNgAlarmCnt ;
		pRecipeInfo->nAlarmCount = m_nAlarmCnt ;
		pRecipeInfo->nSectorCount  = m_nSectorCnt ;



		pRecipeInfo->bDisablePET = m_bChkDisablePET;
		pRecipeInfo->nPetMatrixX[nCamPos] = m_nEdPetMatrixX;
		pRecipeInfo->nPetMatrixY[nCamPos] = m_nEdPetMatrixY;
		pRecipeInfo->nPetThreshold[nCamPos] = m_nEdPetThreshold;
		pRecipeInfo->nPetCheckCnt[nCamPos] = m_nEdPetCheckCnt;

		pRecipeInfo->bEnableVGroove = m_bChkEnableVGroove;


	}
	OnRefresh();
}

void CRecipeSettingDlg::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strRecipeName = m_strEdRecipeName;
	//m_cmbRecipeName.GetWindowTextA(strRecipeName);
	ASSERT(m_pRecipeInfo);
	ASSERT(m_pRecipeCtrl);
	if (( m_pRecipeInfo == NULL) || (m_pRecipeCtrl == NULL) ){
		MessageBox(_T("Failed to save recipe") );
		return;
	}
	if (strRecipeName.GetLength() <= 0) {
		MessageBox(_T("Please specify the name of the recipe."));
		return;
	}
	DataControl( MODE_WRITE, m_pRecipeInfo);
	m_pRecipeCtrl->SetRecipeInfo( m_pRecipeInfo );
	m_pRecipeCtrl->SaveRecipe(strRecipeName);
	SaveRecipeTable();

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE
	//SPC+ SPEC_PARA ======================================================
	CSpcSpecParaManager SpecPara;
	//inData 포인터 객체
	CSpcSpecParaInData* SpecParaInData = SpecPara.getSpcInspInData();
	//Spec 정보 세팅 포인터 객체
	//변경Para 정보	
	std::vector<CSpcParaInfo*>* ParaPtr = SpecPara.getSpcParaInfoPtr();
	CSpcParaInfo* ParaInfo = new CSpcParaInfo(&SpecPara);
	//추가적인 Para key:value
	ParaInfo->appendParaData("TabParam", "10");

	//Para 정보를 저장한다.
	ParaPtr->push_back(ParaInfo);

	//변경 Spec 정보	
	std::vector<CSpcSpecInfo*>* SpecPtr = SpecPara.getSpcSpecInfoPtr();
	CSpcSpecInfo* SpecInfo = new CSpcSpecInfo(&SpecPara);
	//추가적인 Spec key:value
	SpecInfo->appendSpec("CameraSpec", "NA");

	//Spec 정보를 저장한다.
	SpecPtr->push_back(SpecInfo);


	SpecPara.makeJSONFile();
	//======================================================================
#endif //SPCPLUS_CREATE

}

int CRecipeSettingDlg::MakeGridCtrl()
{
	CRect rect;
	GetClientRect(rect);
	CGridCtrl* pGridCtrl;

	int nHeight = 30;
	int m_nFixCols = 1;
	int m_nFixRows = 1;
	int m_nCols = en_RankCond_MaxCols;
	int m_nRows = en_Rank_Rows;
	int nRow = 0;
	int nCol = 0;

	// 22.07.13 Ahn Add Start
	if (m_pFontGrid == NULL) {
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFont(AprData.m_System.m_nGridFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
			, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
			, FIXED_PITCH, "Arial");
	}
	// 22.07.13 Ahn Add End

	pGridCtrl = &m_GridCtrlLight;
	if (pGridCtrl != nullptr) {
		TRY{
			pGridCtrl->SetRowCount(en_Light_Rows);
			pGridCtrl->SetColumnCount(en_Light_Cols);
			pGridCtrl->SetFixedRowCount(m_nFixRows);
			pGridCtrl->SetFixedColumnCount(m_nFixCols);
		}
			CATCH(CMemoryException, e) {
			e->ReportError();
			return -1;
		}
		END_CATCH

		pGridCtrl->SetFont(m_pFontGrid, TRUE);
		pGridCtrl->SetRowHeight( 0, nHeight);

		CString strTitle[en_Light_Cols] = { _T("SORT"), _T("TAB"), _T("BOTTOM") };
		for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;
			Item.col = nCol;

			Item.strText = strTitle[nCol];

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strTitle[nCol], 2);
			//cd 메모리 해제
			ReleaseDC(dc);

			pGridCtrl->SetItem(&Item);

		}

		CString strLightName[MAX_LIGHT_UNIT] = { _T("MAIN"), _T("SUB") };
	//	CString strCameraName[MAX_CAMERA_NO] = { _T("탑"), _T("버틈") };
		for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
			int nUnitNo = nRow - 1;
			int nType = (nRow - 1) / en_Rank_Dross_OK;
			int nRank = (nRow - 1) % en_Rank_Dross_OK;
			for (int nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++)
			{
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = nRow;
				Item.col = nCol;

				int nChanel = nCol - 1;

				if (nRow < pGridCtrl->GetFixedRowCount()) {}
				else if (nCol < pGridCtrl->GetFixedColumnCount()) {
					strText.Format(_T("%s"), strLightName[nUnitNo]);
					Item.strText = strText;
				}
				else {
					strText.Format(_T("%d"), m_pRecipeInfo->nLightLevel[nUnitNo][nChanel]);
					Item.strText = strText;
				}

				pGridCtrl->SetItem(&Item);

				//폰트 픽셀 넓이 저정
				CDC* dc = GetDC();
				pGridCtrl->setGrideFontWidth(nCol, dc, strText, 2);
				//cd 메모리 해제
				ReleaseDC(dc);

				pGridCtrl->SetColAlignment(nCol, flexAlignCenterCenter);
			}
			pGridCtrl->SetRowHeight(nRow, nHeight);
		}
		//크기 조정
		pGridCtrl->setGridFullColumnWidthVec();

		pGridCtrl->ShowWindow(SW_SHOW);
	}

	return 0;

}


int CRecipeSettingDlg::UpdateGrid()
{
	// TODO: 여기에 구현 코드 추가.
	int nRows;

	CGridCtrl* pGrid ;

	CRecipeInfo* pRecipeInfo;
	pRecipeInfo = m_pRecipeInfo;
	if (pRecipeInfo == nullptr) {
		MessageBox("Pleases Load Recipe.");
		return -1;
	}
	nRows = en_Rank_Rows;

	UpdateData(FALSE);

	COLORREF clrBkFoil = RGB(170, 200, 170);
	COLORREF clrBkDross = RGB(200, 200, 200);

	pGrid = &m_GridCtrlLight;
	if (pGrid != nullptr) {
		int nRows = pGrid->GetRowCount();
		int nCols = pGrid->GetColumnCount();

		for (int nRow = 1; nRow < nRows; nRow++) {
			for (int nCol = 1; nCol < nCols; nCol++) {
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = nRow;
				Item.col = nCol;
				int nChNo = nCol - 1;
				int nUnit = nRow - 1;

				strText.Format(_T("%d"), this->m_pRecipeInfo->nLightLevel[nUnit][nChNo]);
				Item.strText = strText;
				pGrid->SetItem(&Item);
				pGrid->SetColAlignment(nCol, flexAlignCenterCenter);
			}
		}
		pGrid->Refresh();
	}

	return 0;
}

void CRecipeSettingDlg::OnCbnSelchangeCmbRecipeName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	ASSERT(m_pRecipeCtrl);
	CString strSelRecipeName = _T("");
	UpdateData(TRUE);

	CString strRecipePath;
	strRecipePath.Format(_T("%s\\Recipe"), AprData.m_strDataPath);

	if (m_pRecipeInfo != nullptr) {
		m_pRecipeCtrl->LoadRecipe(m_pRecipeInfo, strSelRecipeName);
		DataControl(MODE_READ, m_pRecipeInfo);

		UpdateGrid();
	}
}

void CRecipeSettingDlg::OnClickGridCtrlLight(NMHDR* pNMHDR, LRESULT* pResult)
{
	CGridCtrl* pGridCtrl = &m_GridCtrlLight;

	if (pGridCtrl != NULL) {
		int	iMaxColNum = pGridCtrl->GetCols();
		int	iMaxRowNum = pGridCtrl->GetRows();
		int iRow = pGridCtrl->GetMouseRow();
		int iCol = pGridCtrl->GetMouseCol();

		int	iMouseRow = pGridCtrl->GetMouseRow();
		int	iMouseCol = pGridCtrl->GetMouseCol();
		if ((iMouseRow < 0) || (iMouseRow > iMaxRowNum)
			|| (iMouseCol < 1) || (iMouseCol > iMaxColNum)) {
			// -----붝댪둖-----
			return;
		}

		int	iRealNo = iRow - 1;
		int	ret = 0;
		int	iSetValue = 0;
		double	dSetValue = (double)0.0;
		double	dMaxValue = (double)0.0;
		double	dTmpValue = (double)0.0;
		CString	szTemp = "";
		szTemp = pGridCtrl->GetTextMatrix(iRow, iCol);
		CSetValue	setvalue(this);
		CString	guidemsg = _T("");
		int nTemp = 0;

		int nColNo = iCol - 1;

		CString strName = _T("MAIN");
		CString strCamName = _T("TAB");
		if (iRealNo == 1) {
			strName = _T("SUB");
		}
		if (nColNo == 1) {
			strCamName = _T("BOTTOM");
		}

		guidemsg.Format(_T("Enter the% s light value of the% s camera."), strName, strCamName );
	

		nTemp = atoi(szTemp.GetBuffer());

		setvalue.SetValue(guidemsg, nTemp, 255, 0);

		szTemp.Format(_T("%d"), nTemp);
		pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
		m_pRecipeInfo->nLightLevel[iRealNo][nColNo] = nTemp;

		pGridCtrl->Refresh();

	}

}

void CRecipeSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

}


void CRecipeSettingDlg::OnTcnSelchangeTabDetectCond(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DataControl(MODE_WRITE, m_pRecipeInfo);

	m_nLastSelTab = m_TabDetectCond.GetCurSel();

	DataControl(MODE_READ, m_pRecipeInfo);

	*pResult = 0;
}


void CRecipeSettingDlg::OnEnSetfocusFoilLinkRangeX()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 999;
	nMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nFoilLinkX;
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 링크 X 범위를 입력해 주세요. 범위( %d mm ~ %d mm)"), nMin, nMax);
	strMsg.Format(_T("Range( %d mm ~ %d mm)"), nMin, nMax);
	m_nFoilLinkX = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusDrossLinkRangeX()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 999;
	nMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nDrossLinkX;
	CString strMsg;
//	strMsg.Format(_T("Dross 결함 링크 X 범위를 입력해 주세요. 범위( %d mm ~ %d mm)"), nMin, nMax);
	strMsg.Format(_T("Range( %d mm ~ %d mm)"), nMin, nMax);
	m_nDrossLinkX = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusFoilLinkRangeY()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 999;
	nMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nFoilLinkY;
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 링크 Y 범위를 입력해 주세요. 범위( %d mm ~ %d mm)"), nMin, nMax);
	strMsg.Format(_T("Range( %d mm ~ %d mm)"), nMin, nMax);
	m_nFoilLinkY = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusDrossLinkRangeY()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 999;
	nMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nDrossLinkY;
	CString strMsg;
//	strMsg.Format(_T("Dross 결함 링크 Y 범위를 입력해 주세요. 범위( %d mm ~ %d mm)"), nMin, nMax);
	strMsg.Format(_T("Range( %d mm ~ %d mm)"), nMin, nMax);
	m_nDrossLinkY = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}

// 22.07.15 Ahn Delete Start
//// 21.08.31 Ahn Add Start
//void CRecipeSettingDlg::OnBnClickedRadMaxDiff()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	UpdateData(TRUE);
//}
//
//
//void CRecipeSettingDlg::OnBnClickedRadSetBright()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	UpdateData(TRUE);
//}
//// 21.08.31 Ahn Add Start
	// 22.07.15 Ahn Delete End



void CRecipeSettingDlg::OnEnSetfocusEdBmpSaveInterval()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 1000;
	nMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nBmpSaveInterval;
	CString strMsg;
//	strMsg.Format(_T("비트맵 이미지 저장 간격을 설정해 주세요. %d ~ %d"), nMin, nMax);
	strMsg.Format(_T("Range(%d ~ %d)"), nMin, nMax);
	nValue = m_nBmpSaveInterval;
	m_nBmpSaveInterval = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnTcnSelchangeTabInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DataControl(MODE_WRITE, m_pRecipeInfo);

	m_nLastSelTabInfo = m_TabInfo.GetCurSel();

	DataControl(MODE_READ, m_pRecipeInfo);
	*pResult = 0;
}


void CRecipeSettingDlg::OnBnClickedChkSaveOnlyNgTab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DataControl(MODE_WRITE, m_pRecipeInfo);
}


void CRecipeSettingDlg::OnBnClickedChkDisableProcDiff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DataControl(MODE_WRITE, m_pRecipeInfo);
}


void CRecipeSettingDlg::OnEnSetfocusEdMagnification()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 64.0; // 22.06.30 Ahn Modify 16 -> 64 
	dMin = 0.1;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	dValue = m_dEdMagnification;
	CString strMsg;
//	strMsg.Format(_T("검출 배율을 입력해 주세요.( %2.lf mm ~ %.2lf mm)"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf mm ~ %.2lf mm)"), dMin, dMax);
	m_dEdMagnification = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}



void CRecipeSettingDlg::OnBnClickedChkEnableDefLink()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DataControl(MODE_WRITE, m_pRecipeInfo);
}


void CRecipeSettingDlg::OnRefresh()
{
	CWnd* pWnd;
	BOOL bEnable ;
	UpdateData(TRUE);
	if (m_bChkEnableDefLink == TRUE) {
		bEnable = TRUE;
	}
	else {
		bEnable = FALSE;
	}

	for (int i = 0; i < 4; i++) {
		int nID = IDC_FOIL_LINK_RANGE_X + i;
		pWnd = (CWnd*)GetDlgItem(nID);
		if ((pWnd != nullptr) && (pWnd->m_hWnd != nullptr)) {
			pWnd->EnableWindow(bEnable);
		}
	}

	UpdateGrid();


}


void CRecipeSettingDlg::OnEnSetfocusEdNgXSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdNgSizeWidth[CAM_POS_TOP];
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 유지부 NG 판정 가로 길이를 입력해 주세요.( %2.lf um ~ %.2lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um). Not used if 0 or less"), dMin, dMax);
	m_dEdNgSizeWidth[CAM_POS_TOP] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdNgYSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 99999.0;
	dMin = 0.0;
	dValue = m_dEdNgSizeHeight;
	CString strMsg;
//	strMsg.Format(_T("NG 판정 세로 길이를 입력해 주세요. ( %2.lf um ~ %.2lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um). Not used if 0 or less"), dMin, dMax);
	m_dEdNgSizeHeight = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnBnClickedChkNgStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DataControl(MODE_WRITE, m_pRecipeInfo);
}


void CRecipeSettingDlg::OnBnClickedChkDarkEmpMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DataControl(MODE_WRITE, m_pRecipeInfo);
}


void CRecipeSettingDlg::OnClickGridCtrlRecipeTable(NMHDR* pNMHDR, LRESULT* pResult)
{
	CGridCtrl* pGridCtrl = &m_GridRecipeTable;

	if (pGridCtrl != NULL) {
		int	iMaxColNum = pGridCtrl->GetCols();
		int	iMaxRowNum = pGridCtrl->GetRows();
		int iRow = pGridCtrl->GetMouseRow();
		int iCol = pGridCtrl->GetMouseCol();

		int	iMouseRow = pGridCtrl->GetMouseRow();
		int	iMouseCol = pGridCtrl->GetMouseCol();
		if ((iMouseRow < 0) || (iMouseRow > iMaxRowNum)
			|| (iMouseCol < 1) || (iMouseCol > iMaxColNum)) {
			// -----붝댪둖-----
			return;
		}


		int	iRealNo = iRow - 1;
		int	ret = 0;
		int	iSetValue = 0;
		double	dSetValue = (double)0.0;
		double	dMaxValue = (double)0.0;
		double	dTmpValue = (double)0.0;
		CString	strRecipeName = _T("");
		strRecipeName = pGridCtrl->GetTextMatrix(iRow, 1);

		CString	strRecipeMemo = _T("");
		strRecipeMemo = pGridCtrl->GetTextMatrix(iRow, 2);

		int nTemp = 0;

		int nColNo = iCol - 1;

		m_nEdRecipeNo = iRow;

		if (strRecipeName.GetLength() > 0) {
			m_pRecipeCtrl->LoadRecipe( m_pRecipeInfo, strRecipeName);
			DataControl(MODE_READ, m_pRecipeInfo);
			m_strEdRecipeName = strRecipeName;
			m_strRecipeMemo = strRecipeMemo;
		}
		else
		{
			m_strEdRecipeName = "";
			m_strRecipeMemo = "";
		}

		//pGridCtrl->Refresh();
	}

	UpdateGrid();
	UpdateGrid_Marking();


	UpdateData(FALSE);
}

int CRecipeSettingDlg::MakeGridCtrl_RecipeTable()
{
	CRect rect;
	GetClientRect(rect);
	CGridCtrl* pGridCtrl;
	pGridCtrl = &m_GridRecipeTable;

	if (m_pFontGrid == NULL) {
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFont( AprData.m_System.m_nGridFontSize , 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
			, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
			, FIXED_PITCH, "Arial");
	}


	pGridCtrl->SetFont(m_pFontGrid, TRUE);

	int m_nFixCols = 1;
	int m_nFixRows = 1;
	int m_nCols = en_RcpName_MaxCols;
	int m_nRows = MAX_RECIPE_TABLE + 1 ;
	int nRow = 0;
	int nCol = 0;

	//	pGridCtrl->SetAutoSizeStyle();

	TRY{
		pGridCtrl->SetRowCount(m_nRows);
		pGridCtrl->SetColumnCount(m_nCols);
		pGridCtrl->SetFixedRowCount(m_nFixRows);
		pGridCtrl->SetFixedColumnCount(m_nFixCols);
	}
		CATCH(CMemoryException, e) {
		e->ReportError();
		return -1;
	}
	END_CATCH

	// 22.09.19 Ahn Modify Start
	//CString strTitle[en_RcpName_MaxCols] = { _T("번호"), _T("레시피 명"),_T("메모") };
	//CString strTitle[CRecipeSettingDlg::en_RcpName_MaxCols] = { _T("번호"), _T("레시피 명"),_T("메모") };
	CString strTitle[CRecipeSettingDlg::en_RcpName_MaxCols]; //  = { _T("번호"), _T("레시피 명"),_T("메모") };


	for (int i = 0; i < en_RcpName_MaxCols; i++) {
		switch (i) {
		case	en_RcpTable_No:
			strTitle[i] = UiText3[0][__Lang];
			break;
		case	en_RcpTable_Name:
			strTitle[i] = UiText3[1][__Lang];
			break;
		case	en_RcpName_Memo:
			strTitle[i] = UiText3[2][__Lang];
			break;
		}
	}
	// 22.09.19 Ahn Modify End
	for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
		CString strText;
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = 0;
		Item.col = nCol;
		Item.strText = strTitle[nCol];

		//폰트 픽셀 넓이 저정
		CDC* dc = GetDC();
		pGridCtrl->setGrideFontWidth(nCol, dc, strTitle[nCol], 2);
		//cd 메모리 해제
		ReleaseDC(dc);

		pGridCtrl->SetItem(&Item);

	}


	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			int nCondNo = nCol - 1;

			//if (nRow < pGridCtrl->GetFixedRowCount())
			//	strText.Format(_T("%d"), nCol);
			//else 
			CString strTemp;
			CRecipeTableInfo* pInfo = m_RcpTableCtrl.GetRecipeTableInfo(nRow-1);
			if (pInfo == NULL) {
				return -1;
			}

			switch(nCol) {
			case	0 :
				strText.Format(_T("%d"), nRow );
				break;
			case	1:
				strText = pInfo->strRecipeName;
				break;
			case	2:
				strText = pInfo->strMemo;
				break;
			}
			Item.strText = strText;

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strText, 2);
			//cd 메모리 해제
			ReleaseDC(dc);

			pGridCtrl->SetItem(&Item);
		}
		int nHeight = 22;
		pGridCtrl->SetRowHeight(nRow, nHeight);
	}
	//크기조정
	pGridCtrl->setGridFullColumnWidthVec();

	pGridCtrl->ShowWindow(SW_SHOW);

	return 0;
}



int CRecipeSettingDlg::UpdateGrid_RecipeTable()
{
	int nRet = 0;
	// TODO: 여기에 구현 코드 추가.
	int nRows;

	CGridCtrl* pGrid = &m_GridRecipeTable;

	CRecipeInfo* pRecipeInfo;
	pRecipeInfo = m_pRecipeInfo;
	if (pRecipeInfo == nullptr) {
		MessageBox("Pleases Load Recipe.");
		return -1;
	}
	nRows = MAX_RECIPE_TABLE + 1;

	if (!::IsWindow(pGrid->m_hWnd)) {
		return -1;
	}

	UpdateData();

	int nOldNumRows = pGrid->GetRowCount();

	TRY{
		pGrid->SetRowCount(nRows);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return -1;
	}
	END_CATCH

		UpdateData(FALSE);

	COLORREF clrBkFoil = RGB(170, 200, 170);
	COLORREF clrBkDross = RGB(200, 200, 200);

	for (int nRow = 1; nRow < nRows; nRow++) {
		for (int nCol = 1; nCol < pGrid->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;
			int nCondNo = nCol - 1;

			//strText.Format(_T("%d"), pRecipeInfo->nRank[nType][nRank][nCondNo]);
			CRecipeTableInfo* pInfo = m_RcpTableCtrl.GetRecipeTableInfo(nRow-1) ;
			if (pInfo == NULL) {
				return -1;
			}

			switch (nCol) {
			case	1:
				strText = pInfo->strRecipeName;
				break;
			case	2:
				strText = pInfo->strMemo;
				break;
			}

			Item.strText = strText;
			pGrid->SetItem(&Item);
		}
	}
	pGrid->Refresh();

	//pGrid->EnsureVisible(nRows - 1, 0);


	return nRet;
}


void CRecipeSettingDlg::OnBnClickedBtnRegist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	m_bRecipeTableChanged = TRUE;

	if ((m_nEdRecipeNo <= 0) || (m_nEdRecipeNo > MAX_RECIPE_TABLE)) {
		MessageBox(_T("Recipe number NG : Please select the recipe number to register in the table below."));
		return ;
	}

	CRecipeTableInfo *pRcpTableInfo = m_RcpTableCtrl.GetRecipeTableInfo(m_nEdRecipeNo-1);

	if (pRcpTableInfo == NULL) {
		MessageBox(_T("Recipe table info NG."));
		return;
	}

	CString strRecipeName = m_strEdRecipeName ;
	CString strRcpTableName;
	//m_cmbRecipeName.GetWindowTextA(strRecipeName);
	strRcpTableName = pRcpTableInfo->strRecipeName;
	if (strRecipeName.Compare(strRcpTableName) == 0) {
		MessageBox(_T("You are already registering."));
		return;
	}

	pRcpTableInfo->strRecipeName = strRecipeName;
	pRcpTableInfo->strMemo = m_strRecipeMemo;

	UpdateGrid();
	UpdateGrid_RecipeTable();

	m_RcpTableCtrl.FileCtrl(MODE_WRITE);
}


void CRecipeSettingDlg::OnBnClickedBtnDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bRecipeTableChanged = TRUE;

	if ((m_nEdRecipeNo <= 0) || (m_nEdRecipeNo > MAX_RECIPE_TABLE)) {
		MessageBox(_T("Recipe number NG : Please select the recipe number to register in the table below."));
		return;
	}

	CRecipeTableInfo* pRcpTableInfo = m_RcpTableCtrl.GetRecipeTableInfo(m_nEdRecipeNo - 1);

	if (pRcpTableInfo == NULL) {
		MessageBox(_T("Recipe table info NG."));
		return;
	}

	pRcpTableInfo->strRecipeName = _T("") ;
	pRcpTableInfo->strMemo = _T("");

	UpdateGrid_RecipeTable();
	m_RcpTableCtrl.FileCtrl(MODE_WRITE);
}


void CRecipeSettingDlg::LoadRecipeTable()
{
	m_RcpTableCtrl.FileCtrl(MODE_READ);
}
void CRecipeSettingDlg::SaveRecipeTable()
{
	m_bRecipeTableChanged = FALSE;
	m_RcpTableCtrl.FileCtrl(MODE_WRITE);
}


void CRecipeSettingDlg::OnBnClickedBtnLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	CGridCtrl* pGridCtrl = &m_GridRecipeTable;

	if (pGridCtrl != NULL) 
	{
		UpdateData();
		int iRow = m_nEdRecipeNo;
		CString	strRecipeName = _T("");
		strRecipeName = pGridCtrl->GetTextMatrix(iRow, 1);
		if (strRecipeName.GetLength())
		{
			MessageBox(_T("You are already registering."));
		}
		else
		{
			//레시피 선택 창
			CRecipeImportDlg rcpSelDlg;

			if (rcpSelDlg.DoModal() == IDOK) {
				m_strEdRecipeName = rcpSelDlg.m_strRecipeName;
				m_pRecipeCtrl->LoadRecipe(m_pRecipeInfo, m_strEdRecipeName);
				DataControl(MODE_READ, m_pRecipeInfo);
				m_strRecipeMemo = rcpSelDlg.m_strRecipeMemo;
				UpdateData(FALSE);
			}
		}
	}
}


void CRecipeSettingDlg::OnBnClickedChkSaveDefInTab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	ShowControl();
}


void CRecipeSettingDlg::OnEnSetfocusEdSaveDefSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 999;
	dMin = 0;
	dValue = m_dEdSaveDefSize ;
	CString strMsg;
//	strMsg.Format(_T("NG 판정 세로 길이를 입력해 주세요. ( %.1lf um ~ %.1lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %.1lf um ~ %.1lf um). Not used if 0.0 or less"), dMin, dMax);
	m_dEdSaveDefSize = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::ShowControl()
{
	CWnd* pWnd;
	if (m_bChkDisableSurface == FALSE) {
		pWnd = (CWnd*)GetDlgItem(IDC_ED_THRES_SPETTER);
		if (pWnd != nullptr) {
			pWnd->EnableWindow(SW_SHOW);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_SPT_MASK);
		if (pWnd != nullptr) {
			pWnd->EnableWindow(SW_SHOW);
		}		
	}
	else {
		pWnd = (CWnd*)GetDlgItem(IDC_ED_THRES_SPETTER);
		if (pWnd != nullptr) {
			pWnd->EnableWindow(SW_HIDE);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_SPT_MASK);
		if (pWnd != nullptr) {
			pWnd->EnableWindow(SW_HIDE);
		}
	}

	if (m_bChkDisablePET == FALSE)
	{
	}
	else
	{

	}


	// 22.07.22 Ahn Add Start
	BOOL bShowAnode;
	BOOL bShowCathode;
	// 22.09.15 Ahn Modify Start
	//if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
	if (AprData.m_System.m_nMachineMode == ANODE_MODE) {
	// 22.09.15 Ahn Modify End
		bShowAnode = FALSE ;
		bShowCathode = TRUE ;
	}
	else {
		bShowAnode = TRUE ;
		bShowCathode = FALSE ;
	}
	{ // Anode (양극) 
		pWnd = (CWnd*)GetDlgItem(IDC_GROUP_ANODE);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ST_TOP_COAT_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}		
		pWnd = (CWnd*)GetDlgItem(IDC_ST_BTM_COAT_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ST_TAB_COAT_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_TOP_COAT_HEIGHT_PIX);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_TOP_COAT_HEIGHT_MM);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_BTM_COAT_HEIGHT_PIX);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_BTM_COAT_HEIGHT_MM);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_TAB_COAT_HEIGHT_PIX);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_TAB_COAT_HEIGHT_MM);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_CHK_ENABLE_V_GROOVE);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowAnode);
		}
	}
	{ // Cathode(음극) 
		pWnd = (CWnd*)GetDlgItem(IDC_GROUP_CATHODE);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowCathode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ST_COAT_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowCathode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_NEGA_TAB_COAT_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowCathode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_NEGA_TAB_COAT_HEIGHT_PIX);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowCathode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_STATIC_GROOVE_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowCathode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_V_GROOVE_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowCathode);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_V_GROOVE_HEIGHT_PIX);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(bShowCathode);
		}
	}
	// 22.07.22 Ahn Add End

	if (bShowAnode == TRUE)
	{
		pWnd = (CWnd*)GetDlgItem(IDC_STATIC_GROOVE_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(m_bChkEnableVGroove);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_V_GROOVE_HEIGHT);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(m_bChkEnableVGroove);
		}
		pWnd = (CWnd*)GetDlgItem(IDC_ED_V_GROOVE_HEIGHT_PIX);
		if (pWnd != nullptr) {
			pWnd->ShowWindow(m_bChkEnableVGroove);
		}
	}
}


void CRecipeSettingDlg::OnEnSetfocusEdNegaTabCoatHeight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 20.0;
	dMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	dValue = m_dEdNegTabCoatHeight  ;
	CString strMsg;
//	strMsg.Format(_T("음극 TAB부의 코팅 높이를 입력해 주세요. 범위( %d mm ~ %d mm)"), dMin, dMax);
	strMsg.Format(_T("Range ( %d mm ~ %d mm)"), dMin, dMax);
	m_dEdNegTabCoatHeight = SetValue(dValue, strMsg, dMax, dMin);

	m_nEdNegTabCoatHeightPix = (int)(m_dEdNegTabCoatHeight / AprData.m_System.m_dResolX[CAM_POS_TOP] * 1000);

	UpdateData(FALSE);
}

void CRecipeSettingDlg::OnEnSetfocusEdVGrooveHeight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 20.0;
	dMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	dValue = m_dEdNegVGrooveHeight;
	CString strMsg;
//	strMsg.Format(_T("음극 TAB부의 코팅 높이를 입력해 주세요. 범위( %d mm ~ %d mm)"), dMin, dMax);
	strMsg.Format(_T("Range( %d mm ~ %d mm)"), dMin, dMax);
	m_dEdNegVGrooveHeight = SetValue(dValue, strMsg, dMax, dMin);

	m_nEdNegVGrooveHeightPix = (int)(m_dEdNegVGrooveHeight / AprData.m_System.m_dResolX[CAM_POS_TOP] * 1000);
	UpdateData(FALSE);	
}


void CRecipeSettingDlg::OnEnSetfocusEdIgnoreDistance()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 9999.0;
	dMin = 0.0;
	dValue = m_dEdIgnoreDistance ;
	CString strMsg;
//	strMsg.Format(_T("판정대상 제외 거리를 설정해 주세요. ( %.1lf um ~ %.1lf um). 0.0 이하 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %.1lf um ~ %.1lf um). Not used if 0.0 or less"), dMin, dMax);
	m_dEdIgnoreDistance = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdIgnoreSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 9999.0;
	dMin = 0.0;
	dValue = m_dEdIgnoreSize;
	CString strMsg;
//	strMsg.Format(_T("판정대상 제외 결함크기를 설정해 주세요. ( %.1lf um ~ %.1lf um). 0.0 이하 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %.1lf um ~ %.1lf um). Not used if 0.0 or less"), dMin, dMax);
	m_dEdIgnoreSize = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);

}


void CRecipeSettingDlg::OnEnSetfocusEdThresSurface()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nEdThresSurface;
	CString strMsg;
//	strMsg.Format(_T("적극내 스패터 결함의 스레스홀드를 입력해 주세요. ( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nEdThresSurface = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);

}


void CRecipeSettingDlg::OnEnSetfocusEdSptMask()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	int nValue, nMax, nMin;
	nMax = 500;
	nMin = 0;
	nValue = m_nEdSurfaceMaskOffset;
	CString strMsg;
//	strMsg.Format(_T("적극내 스패터 검출 마스크 오프셋을 입력해 주세요. ( %d ~ %d )"), nMin, nMax);
	strMsg.Format(_T("Range ( %d ~ %d )"), nMin, nMax);
	m_nEdSurfaceMaskOffset = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnBnClickedChkDisableSurface()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	ShowControl();
}


void CRecipeSettingDlg::OnEnSetfocusEdSurfaceNgSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 9999.0;
	dMin = 0.0;
	dValue = m_dSurfaceNgSize[CAM_POS_TOP];
	CString strMsg;
//	strMsg.Format(_T("표면결함 NG 사이즈을 입력해 주세요. ( %.1lf um ~ %.1lf um). 0.0 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %.1lf um ~ %.1lf um). Not used if 0.0 or less"), dMin, dMax);
	m_dSurfaceNgSize[CAM_POS_TOP] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}

// 22.06.08 Ahn Add Start
void CRecipeSettingDlg::OnEnSetfocusEdSurfaceMinsize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 999;
	nMin = 2 ;
	nValue = m_nEdSurfaceMinSize;
	CString strMsg;
//	strMsg.Format(_T("표면결함 최소검출 사이즈을 입력해 주세요. (Pixcel)"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d pixel)"), nMin, nMax);
	m_nEdSurfaceMinSize = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}
// 22.06.08 Ahn Add End

// 22.07.19 Ahn Add Start
void CRecipeSettingDlg::OnMouseDblClickMarkingList(NMHDR* pNMHDR, LRESULT* pResult)
{

	CGridCtrl* pGridCtrl = &m_GridMarking;

	if (pGridCtrl != NULL) {
		int	iMaxColNum = pGridCtrl->GetCols();
		int	iMaxRowNum = pGridCtrl->GetRows();
		int iRow = pGridCtrl->GetMouseRow();
		int iCol = pGridCtrl->GetMouseCol();

		int	iMouseRow = pGridCtrl->GetMouseRow();
		int	iMouseCol = pGridCtrl->GetMouseCol();
		if ((iMouseRow < 0) || (iMouseRow > iMaxRowNum)
			|| (iMouseCol < 1) || (iMouseCol > iMaxColNum)) {
			// -----붝댪둖-----
			return;
		}

		int	iRealNo = iRow - 1;
		int	ret = 0;
		int	iSetValue = 0;
		CString	szTemp = "";
		szTemp = pGridCtrl->GetTextMatrix(iRow, iCol);

		CSetValue	setvalue(this);
		CString		guidemsg = _T("");

		int nColNo = iCol - 1;
		int nRowNo = iRow - 1;
		BOOL bUse = FALSE;

		switch (iCol) {
		case	en_MarkingUse :
			//m_pRecipeInfo->nMarkingUse[nRowNo];
			m_pRecipeInfo->nMarkingUse[nRowNo] = !m_pRecipeInfo->nMarkingUse[nRowNo];
			break ;
		case	en_MarkingType :
			if (m_pRecipeInfo->nMarkingType[nRowNo] == 0) {
				m_pRecipeInfo->nMarkingType[nRowNo] = 1;
			}
			else {
				m_pRecipeInfo->nMarkingType[nRowNo] = 0; 
			}
			break ;
		default :
			break ;
		}
		UpdateGrid_Marking();
		pGridCtrl->Refresh();

	}
}
// 22.07.19 Ahn Add End


// 22.07.15 Ahn Add Start
int CRecipeSettingDlg::MakeGridCtrl_Marking()
{
	CRect rect;
	GetClientRect(rect);
	CGridCtrl* pGridCtrl;

	int nHeight = 22;
	int m_nFixCols = 1;
	int m_nFixRows = 1;
	int m_nCols = en_MarkingGrid_Cols;
	int m_nRows = en_MarkingGrid_Rows;
	int nRow = 0;
	int nCol = 0;

	// 22.07.13 Ahn Add Start
	if (m_pFontGrid == NULL) {
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFont(AprData.m_System.m_nGridFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET
			, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DRAFT_QUALITY
			, FIXED_PITCH, "Arial");
	}
	// 22.07.13 Ahn Add End


	// Marking 설정 
	pGridCtrl = &m_GridMarking;
	if (pGridCtrl != nullptr) {
		TRY{
			pGridCtrl->SetRowCount(en_MarkingGrid_Rows);
			pGridCtrl->SetColumnCount(en_MarkingGrid_Cols);
			pGridCtrl->SetFixedRowCount(m_nFixRows);
			pGridCtrl->SetFixedColumnCount(m_nFixCols);
		}
			CATCH(CMemoryException, e) {
			e->ReportError();
			return -1;
		}
		END_CATCH

			pGridCtrl->SetFont(m_pFontGrid, TRUE);
		pGridCtrl->SetRowHeight(0, nHeight);

		CString strTitle[en_MarkingGrid_Cols] = { _T("Defect Sorts"), _T("Use"), _T("Type") };
		for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;
			Item.col = nCol;

			Item.strText = strTitle[nCol];

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strTitle[nCol], 2);
			//cd 메모리 해제
			ReleaseDC(dc);

			pGridCtrl->SetItem(&Item);

		}

		CString strDefectName[en_MarkingGrid_Rows] = { _T("FoilExp"), _T("FoilExpOut"), _T("Surface") };
		for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++) {
			for (int nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++)
			{
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = nRow;
				Item.col = nCol;
				int nIdx = nRow - 1;

				if (nRow < pGridCtrl->GetFixedRowCount()) {}
				else if (nCol < pGridCtrl->GetFixedColumnCount()) {
					strText.Format(_T("%s"), strDefectName[nIdx]);
					Item.strText = strText;
				}
				else {
					strText.Format(_T("%d"), m_pRecipeInfo->nMarkingUse[nIdx]);
					Item.strText = strText;
				}
				//폰트 픽셀 넓이 저정
				CDC* dc = GetDC();
				pGridCtrl->setGrideFontWidth(nCol, dc, strText, 2);
				//cd 메모리 해제
				ReleaseDC(dc);

				pGridCtrl->SetItem(&Item);
				pGridCtrl->SetColAlignment(nCol, flexAlignCenterCenter);
			}
			pGridCtrl->SetRowHeight(nRow, nHeight);
		}
		//크기조정
		pGridCtrl->setGridFullColumnWidthVec();

		pGridCtrl->ShowWindow(SW_SHOW);
	}


	return 0;
}

int CRecipeSettingDlg::UpdateGrid_Marking()
{
	int nRet = 0;
	int nRows;

	CGridCtrl* pGrid;

	CRecipeInfo* pRecipeInfo;
	pRecipeInfo = m_pRecipeInfo;
	if (pRecipeInfo == nullptr) {
		MessageBox("Pleases Load Recipe.");
		return -1;
	}
	nRows = en_Rank_Rows;

	UpdateData(FALSE);

	COLORREF clrBkAble = RGB(150, 250, 150);
	COLORREF clrBkDisable = RGB(255, 255, 255);

	pGrid = &m_GridMarking;
	if (pGrid != nullptr) {
		int nRows = pGrid->GetRowCount();
		int nCols = pGrid->GetColumnCount();

		for (int nRow = 1; nRow < nRows; nRow++) {
			for (int nCol = 1; nCol < nCols; nCol++) {
				CString strText;
				GV_ITEM Item;
				Item.mask = GVIF_TEXT;
				Item.row = nRow;
				Item.col = nCol;
				int nKind = nCol - 1;
				int nDefType = nRow - 1;

				switch (nCol) {
				case	en_MarkingUse: 
				{
				//	strText.Format(_T("%s"), ((m_pRecipeInfo->nMarkingUse[nDefType] == TRUE) ? _T("○") : _T("Ｘ")));
				//	Item.crBkClr =
					if (m_pRecipeInfo->nMarkingUse[nDefType] == TRUE) {
						strText = _T("○");
						Item.crBkClr = clrBkAble;
					}
					else {
						strText = _T("Ｘ");
						Item.crBkClr = clrBkDisable;
					}
					Item.crFgClr = RGB(0, 0, 0);
					Item.mask |= (GVIF_BKCLR | GVIF_FGCLR);
				}
					break;
				case	en_MarkingType:
					strText = UiText2[m_pRecipeInfo->nMarkingType[nDefType]][__Lang];
					/*switch (m_pRecipeInfo->nMarkingType[nDefType]) 
					{
					case	0:
						strText = UiText2[0][__Lang];
						break;
					case	1:
						strText = UiText2[1][__Lang];
						break;
					}*/
					break;
				}

				//strText.Format(_T("%d"), this->m_pRecipeInfo->nLightLevel[nUnit][nChNo]);
				Item.strText = strText;
				pGrid->SetItem(&Item);
				pGrid->SetColAlignment(nCol, flexAlignCenterCenter);
			}
		}
		pGrid->Refresh();
	}

	return nRet;
}
// 22.07.15 Ahn Add End

// 22.07.19 Ahn Add Start
void CRecipeSettingDlg::OnEnSetfocusEdFoilOutNgXSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdFoilExpOutNgSize[CAM_POS_TOP];
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 무지부 NG 판정 가로 길이를 입력해 주세요.( %2.lf um ~ %.2lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um).  Not used if 0.0 or less"), dMin, dMax);
	m_dEdFoilExpOutNgSize[CAM_POS_TOP] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdFoilBothNgXSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdFoilExpBothNgSize[CAM_POS_TOP];
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 유/무지부 NG 판정 가로 길이를 입력해 주세요.( %2.lf um ~ %.2lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um).  Not used if 0.0 or less"), dMin, dMax);
	m_dEdFoilExpBothNgSize[CAM_POS_TOP] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}
// 22.07.19 Ahn Add End


void CRecipeSettingDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 22.07.21 Ahn Delete Start
	//CDialogEx::OnCancel();
	// 22.07.21 Ahn Delete End
}


void CRecipeSettingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_ST_TAB_IMAGE);
	CRect imgRect;
	if ( (pWnd != nullptr) && ( pWnd->m_hWnd != nullptr )){
		pWnd->GetWindowRect(imgRect);
		ScreenToClient(imgRect);

		CString strFileName;
		if (AprData.m_System.m_nMachineMode == CATHODE_MODE) {
			strFileName.Format(_T("%s\\Cathode.bmp"), AprData.m_strDataPath);
		}
		else {
			strFileName.Format(_T("%s\\Anode.bmp"), AprData.m_strDataPath);
		}

		//CRect imgRect;
		//GetClientRect(&imgRect);

		Graphics	graphics(dc.m_hDC);
		WCHAR		wchar[512];
		float contrast = 0.0;
		float scale = (100.0f + contrast) / 100.0f;
		scale *= scale;
		float append = 0.5f * (1.0f - scale);
		ColorMatrix cm =
		{
		scale,0.0f,0.0f,0.0f,0.0f,
		0.0f,scale,0.0f,0.0f,0.0f,
		0.0f,0.0f,scale,0.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,0.0f,
		append,append,append,0.0f,1.0f
		};
		ImageAttributes ia;
		ia.SetColorMatrix(&cm);

		if (strFileName.GetLength() > 0) {
			memset(wchar, 0, sizeof(WCHAR) * 512);
			size_t ReturnValue;
			mbstowcs_s(&ReturnValue, wchar, sizeof(wchar) / sizeof(WCHAR), strFileName.GetBuffer(0), _TRUNCATE);

			Image img(wchar);

			LONG imgWidth = img.GetWidth();
			LONG imgHeight = img.GetHeight();
			LONG rectWidth = imgRect.Width();
			LONG rectHeight = imgRect.Height();
			LONG drawWidth;
			LONG drawHeight;
			double rateW = 1.0;
			double rateH = 1.0;

			rateW = (double)rectWidth / (double)imgWidth;
			rateH = (double)rectHeight / (double)imgHeight;

			if (rateW < rateH) {
				drawWidth = (LONG)(imgWidth * rateW);
				drawHeight = (LONG)(imgHeight * rateW);
			}
			else if (rateW > rateH) {
				drawWidth = (LONG)(imgWidth * rateH);
				drawHeight = (LONG)(imgHeight * rateH);
			}
			else {
				drawWidth = (LONG)(imgWidth * rateW) - 20;
				drawHeight = (LONG)(imgHeight * rateH) - 20;
			}

			DWORD color = RGB(0, 0, 0);
			SolidBrush brush(Color((BYTE)color, (BYTE)(color >> 8), (BYTE)(color >> 16)));
			graphics.FillRectangle(&brush, imgRect.left + imgRect.Width() / 2 - rectWidth / 2 - 1, imgRect.top + imgRect.Height() / 2 - rectHeight / 2 - 1, rectWidth + 1, rectHeight + 1);
			graphics.DrawImage(&img, Rect(imgRect.left + imgRect.Width() / 2 - drawWidth / 2, imgRect.top + imgRect.Height() / 2 - drawHeight / 2, drawWidth, drawHeight), 0, 0, img.GetWidth(), img.GetHeight(), UnitPixel, &ia);

			Pen fpen(Color(0, 0, 0));
			DWORD clrFrm = RGB(0, 0, 0);
			fpen.SetColor(Color((BYTE)clrFrm, (BYTE)(clrFrm >> 8), (BYTE)(clrFrm >> 16)));
			fpen.SetWidth(REAL(2.0));
			graphics.DrawRectangle(&fpen, imgRect.left + imgRect.Width() / 2 - rectWidth / 2 - 1, imgRect.top + imgRect.Height() / 2 - rectHeight / 2 - 1, rectWidth + 1, rectHeight + 1);
		}
	}
	
}

// 22.08.05 Ahn Add Start
void CRecipeSettingDlg::OnBnClickedBtnSystemSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CSystemSettingDlg dlg ;
	if (dlg.DoModal() == IDOK) {

	}
}
// 22.08.05 Ahn Add End

// 22.08.09 Ahn Add Start
void CRecipeSettingDlg::OnEnSetfocusEdContinuousAlarm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 100;
	nMin = 0;
	nValue = m_nContinuousNgAlarmCnt;
	CString strMsg;
//	strMsg.Format(_T("연속 NG 발생 알람 발생 탭수. 0 인경우 미사용"), nMin, nMax); 
	strMsg.Format(_T("Range( %d ~ %d ).  Not used if 0 or less"), nMin, nMax);
	m_nContinuousNgAlarmCnt = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE ) ;
}


void CRecipeSettingDlg::OnEnSetfocusEdAlarmCount()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 100;
	nMin = 0;
	nValue = m_nAlarmCnt;
	CString strMsg;
//	strMsg.Format(_T("구간 NG 발생 알람 발생 NG 탭수( n/m 중 n). 0 인경우 미사용"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d ).  Not used if 0 or less"), nMin, nMax);
	m_nAlarmCnt = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdSectorCount()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 10000;
	nMin = 0;
	nValue = m_nSectorCnt;
	CString strMsg;
//	strMsg.Format(_T("구간 NG 발생 알람 발생 설정의 구간 탭수( n/m 중 m). 0 인경우 미사용"), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d ).  Not used if 0 or less"), nMin, nMax);
	m_nSectorCnt = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}
// 22.08.09 Ahn Add End


// 22.09.05 Ahn Add Start
void CRecipeSettingDlg::DisplayLanguage()
{
	CWnd* pWnd;
	CString strText;
	for (int idx = 0; idx < MAX_COUNT; idx++)
	{
		pWnd = NULL;
		pWnd = GetDlgItem(UiText1NameText[idx]);
		if (pWnd != nullptr)
		{
			strText = UiText1[idx][__Lang];
			pWnd->SetWindowTextA(strText);
		}
	}
}
/* {
	CWnd* pWnd;
	CString strText;

	pWnd = GetDlgItem(IDC_ST_GROUP_RCP_SET);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("레시피 설정"), _T("Recipe Setting")));
	}
	pWnd = GetDlgItem(IDC_ST_RECIPE_NAME);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("레시피명"), _T("Recipe Name")));
	}
	pWnd = GetDlgItem(IDC_ST_RECIPE_NAME2);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("메모"), _T("Memo")));
	}
	pWnd = GetDlgItem(IDC_ST_RECIPE_NO);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("레시피 번호"), _T("Recipe No.")));
	}
	pWnd = GetDlgItem(IDC_BTN_REGIST);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("등록"), _T("Regist")));
	}
	pWnd = GetDlgItem(IDC_BTN_DELETE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("삭제"), _T("Delete")));
	}
	pWnd = GetDlgItem(IDC_BTN_PROGRAMINFO);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("프로그램정보"), _T("Program Info")));
	}
	pWnd = GetDlgItem(IDC_BTN_LOAD);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("가져오기"), _T("Load")));
	}
	pWnd = GetDlgItem(IDC_ST_OTHER_SETTING);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("기타 설정"), _T("Other Setting")));
	}
		pWnd = GetDlgItem(IDC_STATIC_IMAGESAVEINTERVAL);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("IMAGE 저장 간격"), _T("IMAGE Save Interval")));
	}
	pWnd = GetDlgItem(IDC_CHK_SAVE_ONLY_NG_TAB);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("NG만 저장"), _T("Save only NG Image")));
	}
	//pWnd = GetDlgItem(IDC_CHK_DISABLE_PROC_DIFF);
	//if (pWnd != nullptr) {
	//	pWnd->SetWindowTextA(_LANG(_T("비교처리 미사용"), _T("Diable Diff")));
	//}
	pWnd = GetDlgItem(IDC_ST_GROUP_TAB_INFO);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("Tab 정보"), _T("Tab Setting")));
	}
	pWnd = GetDlgItem(IDC_ST_INPUT_MM);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("입력단위[mm]"), _T("Input[pix]")));
	}
	pWnd = GetDlgItem(IDC_ST_INPUT_PIX);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("단위[Pix]"), _T("Unit[Pix]")));
	}
	pWnd = GetDlgItem(IDC_GROUP_CATHODE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("음극"), _T("CATHODE")));
	}
	pWnd = GetDlgItem(IDC_ST_COAT_HEIGHT);
	if (pWnd != nullptr) {
	    pWnd->SetWindowTextA(_LANG(_T("탭 코팅 높이"), _T("Tab coat Height")));
	}
	pWnd = GetDlgItem(IDC_STATIC_GROOVE_HEIGHT);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("V홈 높이"), _T("V Hight")));
	}
	pWnd = GetDlgItem(IDC_ST_TAB_WIDTH);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("탭폭"), _T("Tab Width")));
	}
	pWnd = GetDlgItem(IDC_ST_TAB_PITCH);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("탭 피치"), _T("Tab Pitch")));
	}
	pWnd = GetDlgItem(IDC_ST_ROUND_RADIUS);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("라운드 반지름"), _T("Round radius")));
	}
	pWnd = GetDlgItem(IDC_GROUP_ANODE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("양극"), _T("Anode")));
	}
	pWnd = GetDlgItem(IDC_ST_TOP_COAT_HEIGHT);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("Tab코팅높이"), _T("Tab shoulder coat size")));
	}
	pWnd = GetDlgItem(IDC_ST_BTM_COAT_HEIGHT);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("Bottom코팅높이"), _T("Bottom coat size")));
	}
	pWnd = GetDlgItem(IDC_ST_TAB_COAT_HEIGHT);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("Tab코팅높이"), _T("Tab coat size")));
	}
	pWnd = GetDlgItem(IDC_ST_EDGE_DETECT_COND);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("엣지 검출 조건"), _T("Edge Detect Condition")));
	}
	pWnd = GetDlgItem(IDC_ST_COAT_BRIGHT);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("코팅부 휘도(Min/Max)"), _T("Coat Bright(Min/Max)")));
	}
	pWnd = GetDlgItem(IDC_ST_ROLL_BRIGHT);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("롤부 휘도(Min/Max)"), _T("Roll Bright(Min/Max)")));
	}
	pWnd = GetDlgItem(IDC_ST_LIGHT_SETTING);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("조명설정"), _T("Light Setting")));
	}
	pWnd = GetDlgItem(IDC_ST_MARKING_SETTING);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("마킹설정"), _T("Marking Setting")));
	}
	pWnd = GetDlgItem(IDC_ST_DETECT_INFO);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("검출력 설정"), _T("Detect Info")));
	}
	pWnd = GetDlgItem(IDC_ST_THRES_ORG);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("검출레벨(원화상)"), _T("Threshold(Org)")));
	}
	pWnd = GetDlgItem(IDC_ST_THRES_LOWER);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("검출레벨(하한)"), _T("Threshold(Lower)")));
	}
	pWnd = GetDlgItem(IDC_ST_INSP_WIDTH);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("검사범위[mm]"), _T("Insp Range[mm]")));
	}
	pWnd = GetDlgItem(IDC_ST_DET_MIN_SIZE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("최소검출 사이즈[Pix]"), _T("Min Size[Pix]")));
	}
	//pWnd = GetDlgItem(IDC_ST_MASK_OFFSET);
	//if (pWnd != nullptr) {
	//	pWnd->SetWindowTextA(_LANG(_T("마스크오프셋"), _T("Mask offset")));
	//}
	pWnd = GetDlgItem(IDC_ST_GROUP_SPETTER);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("전극표면 검출 설정"), _T("Surface Defect Setting")));
	}
	pWnd = GetDlgItem(IDC_CHK_DISABLE_SPTTER);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("표면검사 미사용"), _T("Disable surface detection")));
	}
	pWnd = GetDlgItem(IDC_ST_THRES_SURFACE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("스레스홀드"), _T("Threshold")));
	}
	pWnd = GetDlgItem(IDC_ST_MASK_SURFACE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("마스크오프셋[Pix]"), _T("Mask Offset[Pix]")));
	}
	pWnd = GetDlgItem(IDC_ST_MIN_SIZE_SURFACE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("최소검출 사이즈[Pix]"), _T("Min size[Pix]")));
	}
	pWnd = GetDlgItem(IDC_CHK_ENABLE_DEF_LINK);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("결함 링크 사용"), _T("Enable Defect Link")));
	}
	pWnd = GetDlgItem(IDC_ST_LINK_RANGE_X);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("연결거리 X[Pix]"), _T("Distance X[Pix]")));
	}
	pWnd = GetDlgItem(IDC_ST_LINK_RANGE_Y);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("연결거리 Y[Pix]"), _T("Disatance Y[Pix]")));
	}
	pWnd = GetDlgItem(IDC_ST_MAGNIFICATION);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("검출배율"), _T("Magnification")));
	}
	pWnd = GetDlgItem(IDC_ST_GROUP_JUDGE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("결함 판정"), _T("Defect Judgement")));
	}
// 	pWnd = GetDlgItem(IDC_ST_HORIZON);
// 	if (pWnd != nullptr) {
// 		pWnd->SetWindowTextA(_LANG(_T("가로(화면기준)"), _T("Horizon")));
// 	}
	pWnd = GetDlgItem(IDC_ST_SURFACE_NG_SIZE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("표면결함 NG 사이즈"), _T("Surface NG Size")));
	}
	pWnd = GetDlgItem(IDC_ST_NG_ALARM_SETTING);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("NG 알람 설정"), _T("NG Alarm")));
	}
	pWnd = GetDlgItem(IDC_ST_CONTINUE_ALRAM);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("연속결함 알람"), _T("Continuous detect alarm")));
	}
	pWnd = GetDlgItem(IDC_ST_SECTION_ALARM);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("구간결함 알람"), _T("Section detect alarm")));
	}
	pWnd = GetDlgItem(IDC_ST_FOIL_DEF_REMOVE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("Foil exp 결함 제거"), _T("Foil_Exp_In Defect Remove")));
	}
	pWnd = GetDlgItem(IDC_ST_FOIL_DEF_REMOVE_DISTANCE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("결함 제거 거리[um]"), _T("Remove distance[um]")));
	}
	pWnd = GetDlgItem(IDC_ST_FOIL_DEF_REMOVE_SIZE);
	if (pWnd != nullptr) {
		pWnd->SetWindowTextA(_LANG(_T("제거 결함 크기[um]"), _T("Remove size[um]")));
	}
	// 22.11.21 Ahn Modify Start - JUDGE_GRAY
	pWnd = GetDlgItem(IDC_ST_GRAY_SIZE);
	if (pWnd != nullptr) {
		pWnd->SetWindowText(_LANG(_T("결함 그레이 판정"), _T("Defect Judgement Gray")));
	}
	// 22.11.21 Ahn Modify End

}
*/
// 22.09.05 Ahn Add End

// 22.09.19 Ahn Add Start
void CRecipeSettingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE) {
		DisplayLanguage();
	}
}
// 22.09.19 Ahn Add End


void CRecipeSettingDlg::OnEnSetfocusEdFoilexpGraySize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdFoilGraySize;
	CString strMsg;
//	strMsg.Format(_LANG(_T("Foil노출 결함 그레이판정 사이즈를 입력하세요."),_T("")), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um)"), dMin, dMax);
	m_dEdFoilGraySize = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnEnSetfocusEdSurfaceGraySize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdSurfaceGraySize;
	CString strMsg;
//	strMsg.Format(_LANG(_T("Surface 결함 그레이판정 사이즈를 입력하세요."), _T("")), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um)"), dMin, dMax);
	m_dEdSurfaceGraySize = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


// 23.02.14 Ahn Add Start
void CRecipeSettingDlg::OnEnSetfocusEdTabMinBright()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	nValue = m_nTabMinBright;
	CString strMsg;
//	strMsg.Format(_LANG(_T("Tab 부 최저휘도를 입력해주세요."), _T("")), nMin, nMax);
	strMsg.Format(_T("Range( %d ~ %d )"), nMin, nMax);
	m_nTabMinBright = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);

}
// 23.02.14 Ahn Add End


//void CRecipeSettingDlg::OnBnClickedRadDarkRoll()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	UpdateData(TRUE);
//
//}
//
//
//void CRecipeSettingDlg::OnBnClickedRadBrightRoll()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	UpdateData(TRUE);
//
//}


void CRecipeSettingDlg::OnBnClickedBtnSystemList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CInitSystemSetting InitSystemSetting(this);
	InitSystemSetting.DoModal();
}


void CRecipeSettingDlg::OnBnClickedBtnPrograminfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.ProgramVersionInfo();
}


void CRecipeSettingDlg::OnSetfocusEdNgXSizeBtm()
{
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdNgSizeWidth[CAM_POS_BOTTOM];
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 유지부 NG 판정 가로 길이를 입력해 주세요.( %2.lf um ~ %.2lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um)."), dMin, dMax);
	m_dEdNgSizeWidth[CAM_POS_BOTTOM] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdFoilOutNgXSizeBtm()
{
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdFoilExpOutNgSize[CAM_POS_BOTTOM];
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 무지부 NG 판정 가로 길이를 입력해 주세요.( %2.lf um ~ %.2lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um)."), dMin, dMax);
	m_dEdFoilExpOutNgSize[CAM_POS_BOTTOM] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdFoilBothNgXSizeBtm()
{
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdFoilExpBothNgSize[CAM_POS_BOTTOM];
	CString strMsg;
//	strMsg.Format(_T("Foil 노출 결함 유/무지부 NG 판정 가로 길이를 입력해 주세요.( %2.lf um ~ %.2lf um). 0 인경우 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um)."), dMin, dMax);
	m_dEdFoilExpBothNgSize[CAM_POS_BOTTOM] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdSurfaceNgSizeBtm()
{
	double dValue, dMax, dMin;
	dMax = 9999.0;
	dMin = 0.0;
	dValue = m_dSurfaceNgSize[CAM_POS_BOTTOM];
	CString strMsg;
//	strMsg.Format(_T("표면결함 NG 사이즈을 입력해 주세요. ( %.1lf um ~ %.1lf um). 0.0 미사용"), dMin, dMax);
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um)."), dMin, dMax);
	m_dSurfaceNgSize[CAM_POS_BOTTOM] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnBnClickedChkDisablePet()
{
	UpdateData(TRUE);

	ShowControl();
}



void CRecipeSettingDlg::OnSetfocusEdPetMatrixX()
{
	int nValue, nMax, nMin;
	nMax = 10;
	nMin = 1;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nEdPetMatrixX;

	CString strMsg;
	strMsg.Format(_T("Range( %d ~ %d ea)"), nMin, nMax);
	m_nEdPetMatrixX = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdPetMatrixY()
{
	int nValue, nMax, nMin;
	nMax = 10;
	nMin = 1;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nEdPetMatrixY;

	CString strMsg;
	strMsg.Format(_T("Range( %d ~ %d ea)"), nMin, nMax);
	m_nEdPetMatrixY = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdPetThreshold()
{
	int nValue, nMax, nMin;
	nMax = 255;
	nMin = 0;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nEdPetThreshold;

	CString strMsg;
	strMsg.Format(_T("Range( %d ~ %d ea)"), nMin, nMax);
	m_nEdPetThreshold = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdPetCheckCnt()
{
	int nValue, nMax, nMin;
	nMax = 100;
	nMin = 1;
	int m_nSelCamPos = m_TabDetectCond.GetCurSel();
	nValue = m_nEdPetCheckCnt;

	CString strMsg;
	strMsg.Format(_T("Range( %d ~ %d ea)"), nMin, nMax);
	m_nEdPetCheckCnt = SetValue(nValue, strMsg, nMax, nMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdFoilDefectYSizeTop()
{
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdDefectYSize[CAM_POS_TOP];
	CString strMsg;
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um). Not used if 0 or less"), dMin, dMax);
	m_dEdDefectYSize[CAM_POS_TOP] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnSetfocusEdFoilDefectYSizeBtm()
{
	double dValue, dMax, dMin;
	dMax = 999.0;
	dMin = 0.0;
	dValue = m_dEdDefectYSize[CAM_POS_BOTTOM];
	CString strMsg;
	strMsg.Format(_T("Range( %2.lf um ~ %.2lf um). Not used if 0 or less"), dMin, dMax);
	m_dEdDefectYSize[CAM_POS_BOTTOM] = SetValue(dValue, strMsg, dMax, dMin);
	UpdateData(FALSE);
}


void CRecipeSettingDlg::OnBnClickedChkEnableVGroove()
{
	UpdateData(TRUE);

	BOOL bShowAnode = m_bChkEnableVGroove;

	CWnd* pWnd;
	pWnd = (CWnd*)GetDlgItem(IDC_STATIC_GROOVE_HEIGHT);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShowAnode);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_V_GROOVE_HEIGHT);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShowAnode);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_V_GROOVE_HEIGHT_PIX);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShowAnode);
	}

	UpdateData(FALSE);

}
