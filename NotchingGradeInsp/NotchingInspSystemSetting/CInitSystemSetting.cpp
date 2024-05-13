// CInitSystemSetting.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "CInitSystemSetting.h"
#include "afxdialogex.h"
#include "resource.h"

#define		STR_PRIVATE_PROFILE_NAME	_T( "\\Data\\SystemSetting.ini" )

CString EQP_SETTING_KEY = "EQP_SETTING";

CString EQP_SETTING_STR =
"MACHINE_MODE,"
"EDGE_DET_MEAN_WIDTH,"
"RESOLUTION_HOR_CAM_NO_00,"
"RESOLUTION_HOR_CAM_NO_01,"
"RESOLUTION_VER,"
"MACHINE_ID,"
"GRAPH_SIZE_FOILEXP,"
"GRAPH_SIZE_SURFACE,"
"DISPLAY_LANGUAGE,"
"LOGO,"
"PLC_MODE,"
"PLC_IP_ADDRESS,"
"PLC_PORT";

int EQP_SETTING_ID[] =
{ (int)IDC_EDIT_MACHINE_MODE,
(int)IDC_EDIT_EDGE_DET_MEAN_WIDTH,
(int)IDC_EDIT_RESOLUTION_HOR_CAM_NO_00,
(int)IDC_EDIT_RESOLUTION_HOR_CAM_NO_01,
(int)IDC_EDIT_RESOLUTION_VER,
(int)IDC_EDIT_MACHINE_ID,
(int)IDC_EDIT_GRAPH_SIZE_FOILEXP,
(int)IDC_EDIT_GRAPH_SIZE_SURFACE,
(int)IDC_EDIT_DISPLAY_LANGUAGE,
(int)IDC_EDIT_LOGO,
(int)IDC_EDIT_PLC_MODE,
(int)IDC_EDIT_PLC_IP_ADDRESS,
(int)IDC_EDIT_PLC_PORT };

CString CAMERA_SETTING_KEY = "CAMERA_SETTING";

CString CAMERA_SETTING_STR =
"TRIGGER_MODE,"
"ACQISITION_RATE_CAM_NO_00,"
"ACQISITION_RATE_CAM_NO_01,"
"GAIN_CAM_NO_00,"
"GAIN_CAM_NO_01,"
"EXPOSURE_TIME_CAM_NO_00,"
"EXPOSURE_TIME_CAM_NO_01,"
"VIEW_OFFSET_HOR_CAM_NO_00,"
"VIEW_OFFSET_HOR_CAM_NO_01,"
"VIEW_WIDTH_CAM_NO,"
"VIEW_HEIGHT_CAM_NO,"
"CAMERA_IMAGE_FLIP_00,"
"CAMERA_IMAGE_FLIP_01";

int CAMERA_SETTING_ID[] =
{ (int)IDC_EDIT_TRIGGER_MODE,
(int)IDC_EDIT_ACQISITION_RATE_CAM_NO_00,
(int)IDC_EDIT_ACQISITION_RATE_CAM_NO_01,
(int)IDC_EDIT_GAIN_CAM_NO_00,
(int)IDC_EDIT_GAIN_CAM_NO_01,
(int)IDC_EDIT_EXPOSURE_TIME_CAM_NO_00,
(int)IDC_EDIT_EXPOSURE_TIME_CAM_NO_01,
(int)IDC_EDIT_VIEW_OFFSET_HOR_CAM_NO_00,
(int)IDC_EDIT_VIEW_OFFSET_HOR_CAM_NO_01,
(int)IDC_EDIT_VIEW_WIDTH_CAM_NO,
(int)IDC_EDIT_VIEW_HEIGHT_CAM_NO,
(int)IDC_EDIT_CAMERA_IMAGE_FLIP_00,
(int)IDC_EDIT_CAMERA_IMAGE_FLIP_01 };

CString DISPLAY_SETTING_KEY = "DISPLAY_SETTING";

CString DISPLAY_SETTING_STR = 
"GRID_FONT_SIZE";

int DISPLAY_SETTING_ID[] =
{ (int)IDC_EDIT_GRID_FONT_SIZE };

CString OTHER_SETTING_KEY = "OTHER_SETTING";

CString OTHER_SETTING_STR = 
"ENABLE_MARKER,"
"JPEG_SAVE_QUALITY,"
"FIRST_TAB_DONOT_PROCESS,"
"ENABLE_NG_STOP,"
"ENABLE_MARKING_ALL_TAB";

int OTHER_SETTING_ID[] =
{ (int)IDC_EDIT_ENABLE_MARKER,
(int)IDC_EDIT_JPEG_SAVE_QUALITY,
(int)IDC_EDIT_FIRST_TAB_DONOT_PROCESS,
(int)IDC_EDIT_ENABLE_NG_STOP,
(int)IDC_EDIT_ENABLE_MARKING_ALL_TAB};

CString RESULT_DELETE_KEY = "RESULT_DELETE";

CString RESULT_DELETE_STR =
"RESULT_SAVE_PERIOD,"
"DISK_CAPACITY_ALARM";

int RESULT_DELETE_ID[] =
{ (int)IDC_EDIT_RESULT_SAVE_PERIOD,
(int)IDC_EDIT_DISK_CAPACITY_ALARM };

CString ADDRESS_SETTING_KEY = "ADDRESS_SETTING";

CString ADDRESS_SETTING_STR =
"BIT_IN,"
"BIT_OUT,"
"WORD_IN,"
"WORD_OUT,"
"SLAVE_ID";

int ADDRESS_SETTING_ID[] =
{ (int)IDC_EDIT_BIT_IN,
(int)IDC_EDIT_BIT_OUT,
(int)IDC_EDIT_WORD_IN,
(int)IDC_EDIT_WORD_OUT,
(int)IDC_EDIT_SLAVE_ID };


// CInitSystemSetting 대화 상자

IMPLEMENT_DYNAMIC(CInitSystemSetting, CDialogEx)

CInitSystemSetting::CInitSystemSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_INITSYSTEMSETTING, pParent)
{

}

CInitSystemSetting::~CInitSystemSetting()
{
}

void CInitSystemSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInitSystemSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BUT_EQP_SETTING, &CInitSystemSetting::OnBnClickedButEqpSetting)
	ON_BN_CLICKED(IDC_BUT_CAMERA_SETTING, &CInitSystemSetting::OnBnClickedButCameraSetting)
	ON_BN_CLICKED(IDC_BUT_DISPLAY_SETTING, &CInitSystemSetting::OnBnClickedButDisplaySetting)
	ON_BN_CLICKED(IDC_BUT_RESULT_DELETE, &CInitSystemSetting::OnBnClickedButResultDelete)
	ON_BN_CLICKED(IDC_BUT_OTHER_SETTING, &CInitSystemSetting::OnBnClickedButOtherSetting)
	ON_BN_CLICKED(IDC_BUT_ADDRESS_SETTING, &CInitSystemSetting::OnBnClickedButAddressSetting)
END_MESSAGE_MAP()


// CInitSystemSetting 메시지 처리기

std::vector<CString> StrList;
BOOL CInitSystemSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//SystemSetting.in  파일에 설정 값을 로딩한다.
	//EQP_SETTING 창에 로딩한다.
	//EQP_SETTING System Key 파싱
	//파싱된 system 정보 키를 저장할 변수
	StrList.clear();
	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, EQP_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//EQP_SETTING 파싱된 데이터 넣기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		static TCHAR SettingData[_MAX_PATH];
		memset(SettingData, 0x00, sizeof(SettingData));
		::GetPrivateProfileString((LPCSTR)(LPCTSTR)EQP_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], ""
			, SettingData, _MAX_PATH, GetPrivateProfilePath()
		);
		CWnd* pWnd = GetDlgItem(EQP_SETTING_ID[idx]);
		if(pWnd)
			pWnd->SetWindowTextA(SettingData);
	}

	//CAMERA_SETTING 창에 로딩한다.
	//CAMERA_SETTING System Key 파싱
	//파싱된 system 정보 키을 저장할 변수
	StrList.clear();
	num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, CAMERA_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//CAMERA_SETTING 파싱된 데이터 넣기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		static TCHAR SettingData[_MAX_PATH];
		memset(SettingData, 0x00, sizeof(SettingData));
		::GetPrivateProfileString((LPCSTR)(LPCTSTR)CAMERA_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], ""
			, SettingData, _MAX_PATH, GetPrivateProfilePath()
		);
		CWnd* pWnd = GetDlgItem(CAMERA_SETTING_ID[idx]);
		if (pWnd)
			pWnd->SetWindowTextA(SettingData);
	}

	//DISPLAY_SETTING 창에 로딩한다.
	//DISPLAY_SETTING System Key 파싱
	//파싱된 system 정보 키을 저장할 변수
	StrList.clear();
	num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, DISPLAY_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//DISPLAY_SETTING 파싱된 데이터 넣기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		static TCHAR SettingData[_MAX_PATH];
		memset(SettingData, 0x00, sizeof(SettingData));
		::GetPrivateProfileString((LPCSTR)(LPCTSTR)DISPLAY_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], ""
			, SettingData, _MAX_PATH, GetPrivateProfilePath()
		);
		CWnd* pWnd = GetDlgItem(DISPLAY_SETTING_ID[idx]);
		if (pWnd)
			pWnd->SetWindowTextA(SettingData);
	}


	//OTHER_SETTING 창에 로딩한다.
	//OTHER_SETTING System Key 파싱
	//파싱된 system 정보 키을 저장할 변수
	StrList.clear();
	num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, OTHER_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//OTHER_SETTING 파싱된 데이터 넣기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		static TCHAR SettingData[_MAX_PATH];
		memset(SettingData, 0x00, sizeof(SettingData));
		::GetPrivateProfileString((LPCSTR)(LPCTSTR)OTHER_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], ""
			, SettingData, _MAX_PATH, GetPrivateProfilePath()
		);
		CWnd* pWnd = GetDlgItem(OTHER_SETTING_ID[idx]);
		if (pWnd)
			pWnd->SetWindowTextA(SettingData);
	}

	//RESULT_DELETE 창에 로딩한다.
	//RESULT_DELETE System Key 파싱
	//파싱된 system 정보 키을 저장할 변수
	StrList.clear();
	num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, RESULT_DELETE_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//RESULT_DELETE 파싱된 데이터 넣기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		static TCHAR SettingData[_MAX_PATH];
		memset(SettingData, 0x00, sizeof(SettingData));
		::GetPrivateProfileString((LPCSTR)(LPCTSTR)RESULT_DELETE_KEY, (LPCSTR)(LPCTSTR)StrList[idx], ""
			, SettingData, _MAX_PATH, GetPrivateProfilePath()
		);
		CWnd* pWnd = GetDlgItem(RESULT_DELETE_ID[idx]);
		if (pWnd)
			pWnd->SetWindowTextA(SettingData);
	}

	//ADDRESS_SETTING 창에 로딩한다.
	//ADDRESS_SETTING System Key 파싱
	//파싱된 system 정보 키을 저장할 변수
	StrList.clear();
	num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, ADDRESS_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//ADDRESS_SETTING 파싱된 데이터 넣기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		static TCHAR SettingData[_MAX_PATH];
		memset(SettingData, 0x00, sizeof(SettingData));
		::GetPrivateProfileString((LPCSTR)(LPCTSTR)ADDRESS_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], ""
			, SettingData, _MAX_PATH, GetPrivateProfilePath()
		);
		CWnd* pWnd = GetDlgItem(ADDRESS_SETTING_ID[idx]);
		if (pWnd)
			pWnd->SetWindowTextA(SettingData);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CInitSystemSetting::OnBnClickedButEqpSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//SystemSetting.in  파일에 설정 값을 세팅한다.
	//EQP_SETTING 창에 로딩한다.
	//EQP_SETTING System Key 파싱
	//파싱된 system 정보 키를 저장할 변수
	StrList.clear();
	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, EQP_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//EQP_SETTING 파싱된 데이터 저장하기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		CWnd* pWnd = GetDlgItem(EQP_SETTING_ID[idx]);
		if (pWnd)
		{
			CString settingData;
			pWnd->GetWindowTextA(settingData);
			static TCHAR SettingData[_MAX_PATH];
			memset(SettingData, 0x00, sizeof(SettingData));
			::WritePrivateProfileString((LPCSTR)(LPCTSTR)EQP_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], settingData, GetPrivateProfilePath());
		}
	}
}


void CInitSystemSetting::OnBnClickedButCameraSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//SystemSetting.in  파일에 설정 값을 세팅한다.
	//CAMERA_SETTING 창에 로딩한다.
	//CAMERA_SETTING System Key 파싱
	//파싱된 system 정보 키를 저장할 변수
	StrList.clear();
	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, CAMERA_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//CAMERA_SETTING 파싱된 데이터 저장하기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		CWnd* pWnd = GetDlgItem(CAMERA_SETTING_ID[idx]);
		if (pWnd)
		{
			CString settingData;
			pWnd->GetWindowTextA(settingData);
			static TCHAR SettingData[_MAX_PATH];
			memset(SettingData, 0x00, sizeof(SettingData));
			::WritePrivateProfileString((LPCSTR)(LPCTSTR)CAMERA_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], settingData, GetPrivateProfilePath());
		}
	}
}


void CInitSystemSetting::OnBnClickedButDisplaySetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//SystemSetting.in  파일에 설정 값을 세팅한다.
	//DISPLAY_SETTING 창에 로딩한다.
	//DISPLAY_SETTING System Key 파싱
	//파싱된 system 정보 키를 저장할 변수
	StrList.clear();
	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, DISPLAY_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//DISPLAY_SETTING 파싱된 데이터 저장하기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		CWnd* pWnd = GetDlgItem(DISPLAY_SETTING_ID[idx]);
		if (pWnd)
		{
			CString settingData;
			pWnd->GetWindowTextA(settingData);
			static TCHAR SettingData[_MAX_PATH];
			memset(SettingData, 0x00, sizeof(SettingData));
			::WritePrivateProfileString((LPCSTR)(LPCTSTR)DISPLAY_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], settingData, GetPrivateProfilePath());
		}
	}
}


void CInitSystemSetting::OnBnClickedButResultDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//SystemSetting.in  파일에 설정 값을 세팅한다.
	//OTHER_SETTING 창에 로딩한다.
	//OTHER_SETTING System Key 파싱
	//파싱된 system 정보 키를 저장할 변수
	StrList.clear();
	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, OTHER_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//OTHER_SETTING 파싱된 데이터 저장하기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		CWnd* pWnd = GetDlgItem(OTHER_SETTING_ID[idx]);
		if (pWnd)
		{
			CString settingData;
			pWnd->GetWindowTextA(settingData);
			static TCHAR SettingData[_MAX_PATH];
			memset(SettingData, 0x00, sizeof(SettingData));
			::WritePrivateProfileString((LPCSTR)(LPCTSTR)OTHER_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], settingData, GetPrivateProfilePath());
		}
	}
}


void CInitSystemSetting::OnBnClickedButOtherSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//SystemSetting.in  파일에 설정 값을 세팅한다.
	//RESULT_DELETE 창에 로딩한다.
	//RESULT_DELETE System Key 파싱
	//파싱된 system 정보 키를 저장할 변수
	StrList.clear();
	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, RESULT_DELETE_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//RESULT_DELETE 파싱된 데이터 저장하기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		CWnd* pWnd = GetDlgItem(RESULT_DELETE_ID[idx]);
		if (pWnd)
		{
			CString settingData;
			pWnd->GetWindowTextA(settingData);
			static TCHAR SettingData[_MAX_PATH];
			memset(SettingData, 0x00, sizeof(SettingData));
			::WritePrivateProfileString((LPCSTR)(LPCTSTR)RESULT_DELETE_KEY, (LPCSTR)(LPCTSTR)StrList[idx], settingData, GetPrivateProfilePath());
		}
	}
}


void CInitSystemSetting::OnBnClickedButAddressSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//SystemSetting.in  파일에 설정 값을 세팅한다.
	//ADDRESS_SETTING 창에 로딩한다.
	//ADDRESS_SETTING System Key 파싱
	//파싱된 system 정보 키를 저장할 변수
	StrList.clear();
	CString strSub;
	int num = 0;
	//파싱하여 vector 에 저장한다.
	while (AfxExtractSubString(strSub, ADDRESS_SETTING_STR, num++, ','))
	{
		StrList.push_back(strSub);
	}
	//ADDRESS_SETTING 파싱된 데이터 저장하기
	for (int idx = 0; idx < StrList.size(); idx++)
	{
		CWnd* pWnd = GetDlgItem(ADDRESS_SETTING_ID[idx]);
		if (pWnd)
		{
			CString settingData;
			pWnd->GetWindowTextA(settingData);
			static TCHAR SettingData[_MAX_PATH];
			memset(SettingData, 0x00, sizeof(SettingData));
			::WritePrivateProfileString((LPCSTR)(LPCTSTR)ADDRESS_SETTING_KEY, (LPCSTR)(LPCTSTR)StrList[idx], settingData, GetPrivateProfilePath());
		}
	}
}
