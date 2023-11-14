// SpcInfo.cpp: CSpcInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcInfo.h"

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE	
#include "Win32File.h"
#endif //SPCPLUS_CREATE

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define		SPCPLUSINFO_PROFILE_NAME	_T( "\\Data\\SpcPlusInfo.ini" )

CSpcInfo* CSpcInfo::instance=NULL;

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcInfo::CSpcInfo()
{
	instance = this;
	LoadSpcPlusInfo();
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcInfo::~CSpcInfo()
{

}

//Spc Plus 정보 파일을 읽어서 저장한다.
void CSpcInfo::LoadSpcPlusInfo()
{
	m_bSPCStartFlag = FALSE;

	TCHAR szProfilePath[_MAX_PATH];
	memset(szProfilePath, 0x00, sizeof(szProfilePath));
	::GetCurrentDirectory(_MAX_PATH, szProfilePath);
	_tcsncat_s(szProfilePath, sizeof(szProfilePath), SPCPLUSINFO_PROFILE_NAME, _MAX_PATH);

//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	BOOL bExist = CWin32File::FolderFileExists(szProfilePath);
	LOGDISPLAY_SPEC(3)("SPC+=====정보파일 읽기<%s><%s>", szProfilePath, (bExist) ? "성공" : "실패");
#endif //SPCPLUS_CREATE

	//설비ID	
	m_EqpId = CGlobalFunc::ReadIniFile("EQP", "EQP_ID", szProfilePath);
	//검사기ID
	m_EqpInspId = CGlobalFunc::ReadIniFile("EQP", "EQP_INSP_ID", szProfilePath);
	//검사 Lot ID	
	m_LotId = CGlobalFunc::ReadIniFile("EQP", "LOT_ID", szProfilePath);
	//검사 호기 Line	
	m_LineNumber = CGlobalFunc::ReadIniFile("INSP_TYPE", "LINE_NUMBER", szProfilePath);
	//검사 호기 정보	
	m_MachineNumber = CGlobalFunc::ReadIniFile("INSP_TYPE", "MACHINE_NUMBER", szProfilePath);
	//검사 Lane 정보 (""5. CAM NUM 규칙"" Sheet 참고)
	m_LaneNumber = CGlobalFunc::ReadIniFile("INSP_TYPE", "LANE_NUMBER", szProfilePath);
	//비전 타입
	m_VisionType = CGlobalFunc::ReadIniFile("INSP_TYPE", "VISION_TYPE", szProfilePath);
	//해당 검사기의 공정 구분	
	m_ProcessGroup = CGlobalFunc::ReadIniFile("PROCESS", "PROCESS_GROUP", szProfilePath);
	//조립공정(노칭, 라미, 스택, 폴딩, 패키지)	
	m_ProcessName = CGlobalFunc::ReadIniFile("PROCESS", "PROCESS_NAME", szProfilePath);
	//물류 진행방향	
	m_ProcessDirection = CGlobalFunc::ReadIniFile("PROCESS", "PROCESS_DIRECTION", szProfilePath);
	//물류 진행방향	
	m_RecipeId = CGlobalFunc::ReadIniFile("PROCESS", "RECIPE_ID", szProfilePath);
	//검사 위치 TOP
	m_InspPositionTop = CGlobalFunc::ReadIniFile("PROCESS", "INSP_POSITION_TOP", szProfilePath);
	//검사위치 BOTTOM
	m_InspPositionBottom = CGlobalFunc::ReadIniFile("PROCESS", "INSP_POSITION_BOTTOM", szProfilePath);
	//Camera 위치(Top, Bottom) 	
	m_CameraLocation = CGlobalFunc::ReadIniFile("CAMERA", "CAMERA_LOCATION", szProfilePath);
	//카메라 Gain value(카메라 조정 값)	
	m_CameraGain = CGlobalFunc::ReadIniFile("CAMERA", "CAMERA_GAIN", szProfilePath);
	//Exposure Time value [ms](카메라 노출 시간)	
	m_CameraExposureTime = CGlobalFunc::ReadIniFile("CAMERA", "CAMERA_EXPOSURE_TIME", szProfilePath);
	//카메라 종류_BarType, LineScan, Mono Area, Color Area	
	m_CameraType = CGlobalFunc::ReadIniFile("CAMERA", "CAMERA_TYPE", szProfilePath);

	//Image Focus Value	
	m_IqFocusValue = CGlobalFunc::ReadIniFile("IQIMAGEINFO", "IQ_FOCUS_VALUE", szProfilePath);
	//Image Bright Value	
	m_IqBrightValue = CGlobalFunc::ReadIniFile("IQIMAGEINFO", "IQ_BRIGHT_VALUE", szProfilePath);
	//Image 해상도 x축 Value[um]	
	m_IqResolXValue = CGlobalFunc::ReadIniFile("IQIMAGEINFO", "IQ_RESOL_X_VALUE", szProfilePath);
	//Image 해상도 y축 Value	
	m_IqResolYValue = CGlobalFunc::ReadIniFile("IQIMAGEINFO", "IQ_RESOL_Y_VALUE", szProfilePath);
	//Image JPG 퀄리티 정보	
	m_IqJpgQuality = CGlobalFunc::ReadIniFile("IQIMAGEINFO", "IQ_JPG_QUALITY", szProfilePath);
	//외관 불량명	
	m_DefectTypeRuleBaseName = CGlobalFunc::ReadIniFile("DEFECTINFO", "DEFECT_TYPE_RULE_BASE_NAME", szProfilePath);
	//불량의 실제 명칭	
	m_DefectTypeRuleBaseNameReal = CGlobalFunc::ReadIniFile("DEFECTINFO", "DEFECT_TYPE_RULE_BASE_NAME_REAL", szProfilePath);

	//Agent Sw Version	
	m_AgentSwVersion = CGlobalFunc::ReadIniFile("SOFTWARE", "AGENT_SW_VERSION", szProfilePath);

	//외관불량 JSON 파일 경로	
	m_InspPath = CGlobalFunc::ReadIniFile("FILEPATH", "INSP_PATH", szProfilePath);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====외관불량 JSON 파일 경로<%s>", m_InspPath);
#endif //SPCPLUS_CREATE
	//장비 Spec, Para 변경 JSON 파일 경로	
	m_SpecParaPath = CGlobalFunc::ReadIniFile("FILEPATH", "SPEC_PARA_PATH", szProfilePath);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====장비 Spec, Para 변경 JSON 파일 경로<%s>", m_SpecParaPath);
#endif //SPCPLUS_CREATE
	//Alarm JSON 파일 경로	
	m_AlarmPath = CGlobalFunc::ReadIniFile("FILEPATH", "ALARM_PATH", szProfilePath);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====Alarm JSON 파일 경로<%s>", m_AlarmPath);
#endif //SPCPLUS_CREATE
	//Status JSON 파일 경로	
	m_StatusPath = CGlobalFunc::ReadIniFile("FILEPATH", "STATUS_PATH", szProfilePath);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====Status JSON 파일 경로<%s>", m_StatusPath);
#endif //SPCPLUS_CREATE
	//INSP Ok Image 파일 경로	
	m_InspOkImagePath = CGlobalFunc::ReadIniFile("FILEPATH", "INSP_OK_IMGPATH", szProfilePath);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====INSP Ok Image 파일 경로<%s>", m_InspOkImagePath);
#endif //SPCPLUS_CREATE
	//INSP Ng Image 파일 경로	
	m_InspNgImagePath = CGlobalFunc::ReadIniFile("FILEPATH", "INSP_NG_IMGPATH", szProfilePath);
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====INSP Ng Image 파일 경로<%s>", m_InspNgImagePath);
#endif //SPCPLUS_CREATE
}

//SPC+ 작동 플래그
void CSpcInfo::setSPCStartFlag(BOOL bSPCStartFlag)
{
	m_bSPCStartFlag = bSPCStartFlag; 
//SPC 객체 소스에서 컴파일 여부 결정
#ifdef SPCPLUS_CREATE			
	//SPC+ 정보 출력 로그
	LOGDISPLAY_SPEC(3)("SPC+=====출력여부<%s>", (m_bSPCStartFlag) ? "Yes" : "No");
#endif //SPCPLUS_CREATE
}