// SystemSettingDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "SystemSettingDlg.h"
#include "afxdialogex.h"
#include "SetValue.h"
#include "GlobalData.h"
#include "SystemSetting.h"
#include "NotchingGradeInspView.h"
#include "NotchingGradeInsp.h"
#include "ImageProcessCtrl.h"
#include "SpcInfo.h"

static CString UiText1[][3] =
{
	{_T("파라메터명"), _T("Param Name"), _T("参数名")},
	{_T("카메라 1"), _T("Camera 1"), _T("相机1")},
	{_T("카메라 2"), _T("Camera 2"), _T("相机2")},
};

static CString UiText2[][3] =
{
	{_T("분해능X"), _T("X Resolution"), _T("X分解能")},
	{_T("라인 레이트"), _T("Line Rate"), _T("线扫倍率")},
	{_T("노광시간"), _T("ExposureTime"), _T("曝光时间")},
	{_T("게인"), _T("Gain"), _T("Gain")},
	{_T("뷰 폭"), _T("Width"), _T("视野宽度")},
	{_T("뷰 높이"), _T("Height"), _T("视野高度")},
	{_T("뷰 옵셋"), _T("View Offset"), _T("视野补偿")},
	{_T("좌우반전"), _T("Flip Mode"), _T("左右翻转")},
};

static CString UiText3[][3] =
{
	{_T("검사 장비 설정"), _T("Equipment type Setting"), _T("检查模式")},
	{_T("양극 검사"), _T("Cathode inspection"), _T("阳极模式")},
	{_T("음극 검사"), _T("Anode inspection"), _T("阴极模式")},
	{_T("카메라 파라메터"), _T("Camera parameter"), _T("相机参数设定")},
	{_T("공통 설정"), _T("Common parameter"), _T("通用设置")},
	{_T("분해능 Y"), _T("Resolution Y"), _T("Y分解能")},
	{_T("외부동기(해제시 내부동기)"), _T("Enable external trigger"), _T("外部动机(解除勾选时内部动机)")},
	{_T("표시 언어 설정"), _T("Display Language"), _T("显示语言")},
	{_T("기타 설정"), _T("Other"), _T("其他设置")},
	{_T("마커사용"), _T("Use marker"), _T("使用喷码")},
	{_T("모든 탭 마킹출력"), _T("Marking all tab"), _T("强制所有TAB喷码")},
	{_T("NG Stop(시스템 설정 우선)"), _T("Use NG Stop"), _T("NG停机(系统设置优先)")},
	{_T("JPEG 저장 품질"), _T("JPEG save quality"), _T("图片保存品质")},
	{_T("결과 보존 기간(일)"), _T("Result save period(day)"), _T("数据保存时长（天）")},
	{_T("저장"), _T("Save"), _T("保存")},
	{_T("취소"), _T("Cancel"), _T("取消")},
	{_T("이미지저장형식"), _T("Out Image Format"), _T("输出图像格式")},
};

enum UiText3Name
{
	ST_MACHINE_TYPE,
	RAD_ANODE_MODE,
	RAD_CATHODE_MODE,
	ST_CAM_PARAM,
	ST_BASE_SETTING,
	ST_RESOLUTION_Y,
	CHK_EXTERNAL_TRIGGER,
	ST_LANG_SETTING,
	ST_OTHER_SETTING,
	CHK_ENABLE_MARKER,
	CHK_MARKING_ALL,
	CHK_ENABLE_NG_STOP,
	ST_JPEG_QUALITY,
	ST_DELETE_PERIOD,
	BTN_OK,
	BTN_CANCEL,
	ST_RESOLUTION_Y2,
	MAX_COUNT
};

static int UiText3NameText[] =
{
	IDC_ST_MACHINE_TYPE,
	IDC_RAD_ANODE_MODE,
	IDC_RAD_CATHODE_MODE,
	IDC_ST_CAM_PARAM,
	IDC_ST_BASE_SETTING,
	IDC_ST_RESOLUTION_Y,
	IDC_CHK_EXTERNAL_TRIGGER,
	IDC_ST_LANG_SETTING,
	IDC_ST_OTHER_SETTING,
	IDC_CHK_ENABLE_MARKER,
	IDC_CHK_MARKING_ALL,
	IDC_CHK_ENABLE_NG_STOP,
	IDC_ST_JPEG_QUALITY,
	IDC_ST_DELETE_PERIOD,
	IDC_BTN_OK,
	IDC_BTN_CANCEL,
	IDC_ST_RESOLUTION_Y2
};

static CString UiText4[][3] =
{
	{_T("시스템 설정을 적용 및 저장 하시겠습니까?"), _T("Do you want to apply and save system settings?"), _T("是否要保存系统设定")}
};

// CSystemSettingDlg 대화 상자

IMPLEMENT_DYNAMIC(CSystemSettingDlg, CDialogEx)

CSystemSettingDlg::CSystemSettingDlg(CWnd* pParent /*=nullptr*/, BOOL bMode /*=TRUE*/)
	: CDialogEx(IDD_DLG_SYSTEM_SETTING, pParent)
	, m_nRadMachineMode(0)
	, m_nJpgSaveQulaity(0)
	, m_nEdOverflowCountMax(5)
	, m_nEdMissTabIdMax(3)
	, m_bChkFirstTabDoNotProc(FALSE)
	, m_bChkEnableNgStop(FALSE)
	, m_EdMachineID(_T(""))
	// 22.08.31 Ahn Delete Start
	//, m_nGraphSizeFoilExp(0)
	//, m_nGraphSizeSurface(0)
	// 22.08.31 Ahn Delete End
	, m_nEdSavePeriod(0)
	, m_nDiskCapacityAlarm(0)
	, m_nLanguage(0)
	// 23.02.17 Son Add Start
	, m_EdBitIn(_T(""))
	, m_EdBitOut(_T(""))
	, m_EdWordIn(_T(""))
	, m_EdWordOut(_T(""))
	// 23.02.17 Son Add End
	// 23.02.27 Son Add Start
	, m_RadLogo(0)
	// 23.02.27 Son Add End
	// 23.02.28 Son Add Start
	, m_RadPlcMode(0)
	, m_EdIPAddress(_T(""))
	, m_nEdPort(0)
	// 23.02.28 Son Add End
	// 23.03.03 Son Add Start
	, m_EdBitIn_16(_T(""))
	, m_EdBitOut_16(_T(""))
	, m_EdWordIn_16(_T(""))
	, m_EdWordOut_16(_T(""))
	// 23.03.03 Son Add End
{
	m_SysSetting = AprData.m_System;

	m_bChkTriggerMode = FALSE;
	m_pParent = (CNotchingGradeInspView*)pParent;

	m_bStartMode = bMode; // 22.08.05 Ahn Add 
	//SPC+ 출력 여부 플래그
	m_bChkDisableSpcPlus = FALSE;
}

CSystemSettingDlg::~CSystemSettingDlg()
{

}

void CSystemSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RAD_ANODE_MODE, m_nRadMachineMode);
	DDX_Control(pDX, IDC_GRID_CTRL_CAMERA_PARAM, m_GridCameraParam);
	DDX_Control(pDX, IDC_COM_IMAGEOUTFORMAT, m_ImageFormat);
	DDX_Text(pDX, IDC_ED_EDGE_DET_MEAN_WIDTH, m_nEdMeanWidth);
	DDX_Text(pDX, IDC_ED_RESOLUTION_VER, m_dEdResolY);
	DDX_Control(pDX, IDC_ED_DUMMY, m_EdDummy);
	DDX_Check(pDX, IDC_CHK_TRIGGER_MODE, m_bChkTriggerMode);
	DDX_Text(pDX, IDC_ED_JPEG_SAVE_QUALITY, m_nJpgSaveQulaity);
	DDX_Text(pDX, IDC_ED_OVERFLOW_MAX, m_nEdOverflowCountMax);
	DDX_Check(pDX, IDC_CHK_FIRST_TAB_DONOT_PROC, m_bChkFirstTabDoNotProc);
	DDX_Check(pDX, IDC_CHK_ENABLE_MARKER, m_bChkEnableMarker);
	DDX_Check(pDX, IDC_CHK_ENABLE_NG_STOP, m_bChkEnableNgStop);
	DDX_Check(pDX, IDC_CHK_MARKING_ALL, m_bChkMarkingAllTab);
	DDX_Text(pDX, IDC_ED_MACHINE_ID, m_EdMachineID);
	DDX_Text(pDX, IDC_ED_RESULT_SAVE_PERIOD, m_nEdSavePeriod);
	DDX_Text(pDX, IDC_ED_DISK_CAPACITY_ALARM, m_nDiskCapacityAlarm);
	DDX_Radio(pDX, IDC_RAD_KOREAN, m_nLanguage);
	DDX_Text(pDX, IDC_ED_BIT_IN_ADDRESS, m_EdBitIn);
	DDX_Text(pDX, IDC_ED_BIT_OUT_ADDRESS, m_EdBitOut);
	DDX_Text(pDX, IDC_ED_WORD_IN_ADDRESS, m_EdWordIn);
	DDX_Text(pDX, IDC_ED_WORD_OUT_ADDRESS, m_EdWordOut);
	DDX_Radio(pDX, IDC_RAD_LOGO1, (int&)m_RadLogo);
	DDX_Radio(pDX, IDC_RAD_PLC_MODE1, (int&)m_RadPlcMode);
	DDX_Text(pDX, IDC_ED_IP, m_EdIPAddress);
	DDX_Text(pDX, IDC_ED_PORT, m_nEdPort);
	DDX_Text(pDX, IDC_ED_BIT_IN_ADDRESS_16, m_EdBitIn_16);
	DDX_Text(pDX, IDC_ED_BIT_OUT_ADDRESS_16, m_EdBitOut_16);
	DDX_Text(pDX, IDC_ED_WORD_IN_ADDRESS_16, m_EdWordIn_16);
	DDX_Text(pDX, IDC_ED_WORD_OUT_ADDRESS_16, m_EdWordOut_16);
	DDX_Check(pDX, IDC_CHK_ENABLE_NON_NG_SAVE, m_bChkEnableNonNgSave);
	DDX_Check(pDX, IDC_CHK_ENABLE_NON_NG_STOP, m_bChkEnableNonNgStop);
	DDX_Text(pDX, IDC_ED_MISS_TAB_ID_MAX, m_nEdMissTabIdMax);
	DDX_Check(pDX, IDC_CHK_DISABLE_SPCPLUS, m_bChkDisableSpcPlus);
}


BEGIN_MESSAGE_MAP(CSystemSettingDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_OK, &CSystemSettingDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CSystemSettingDlg::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_RAD_ANODE_MODE, &CSystemSettingDlg::OnBnClickedRadPositiveMode)
	ON_BN_CLICKED(IDC_RAD_CATHODE_MODE, &CSystemSettingDlg::OnBnClickedRadPositiveMode)
	ON_NOTIFY(NM_CLICK, IDC_GRID_CTRL_CAMERA_PARAM, &CSystemSettingDlg::OnClickGridCtrlCamParam)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_CTRL_CAMERA_PARAM, &CSystemSettingDlg::OnDBClickGridCtrlCamParam) // 22.07.25 Ahn Add
	ON_EN_SETFOCUS(IDC_ED_EDGE_DET_MEAN_WIDTH, &CSystemSettingDlg::OnEnSetfocusEdEdgeDetMeanWidth)
	ON_EN_SETFOCUS(IDC_ED_RESOLUTION_VER, &CSystemSettingDlg::OnEnSetfocusEdResolutionVer)
	ON_BN_CLICKED(IDC_CHK_EXTERNAL_TRIGGER, &CSystemSettingDlg::OnBnClickedChkExternalTrigger)
	ON_BN_CLICKED(IDC_BTN_CAM_PARAM_SET_ALL, &CSystemSettingDlg::OnBnClickedBtnCamParamSetAll)
	ON_BN_CLICKED(IDC_CHK_ENABLE_MARKER, &CSystemSettingDlg::OnBnClickedChkEnableMarker)
	ON_EN_SETFOCUS(IDC_ED_JPEG_SAVE_QUALITY, &CSystemSettingDlg::OnEnSetfocusEdJpegSaveQuality)
	ON_BN_CLICKED(IDC_CHK_FIRST_TAB_DONOT_PROC, &CSystemSettingDlg::OnBnClickedChkFirstTabDonotProc)
	ON_BN_CLICKED(IDC_CHK_MARKING_ALL, &CSystemSettingDlg::OnEnClickChkMarkingAllTab)
	ON_BN_CLICKED(IDC_CHK_ENABLE_NG_STOP, &CSystemSettingDlg::OnBnClickedChkEnableNgStop)
	ON_EN_SETFOCUS(IDC_ED_MACHINE_ID, &CSystemSettingDlg::OnEnSetfocusEdMachineId)
	ON_WM_DESTROY()
	ON_EN_SETFOCUS(IDC_ED_RESULT_SAVE_PERIOD, &CSystemSettingDlg::OnEnSetfocusEdResultSavePeriod)
	ON_EN_SETFOCUS(IDC_ED_DISK_CAPACITY_ALARM, &CSystemSettingDlg::OnEnSetfocusEdDiskCapacityAlarm)
	ON_BN_CLICKED(IDC_RAD_KOREAN, &CSystemSettingDlg::OnBnClickedRadKorean)
	ON_BN_CLICKED(IDC_RAD_ENGLISH, &CSystemSettingDlg::OnBnClickedRadEnglish)
	// 23.02.17 Son Add Start
	ON_EN_SETFOCUS(IDC_ED_BIT_IN_ADDRESS, &CSystemSettingDlg::OnEnSetfocusEdBitIn)
	ON_EN_SETFOCUS(IDC_ED_BIT_OUT_ADDRESS, &CSystemSettingDlg::OnEnSetfocusEdBitOut)
	ON_EN_SETFOCUS(IDC_ED_WORD_IN_ADDRESS, &CSystemSettingDlg::OnEnSetfocusEdWordIn)
	ON_EN_SETFOCUS(IDC_ED_WORD_OUT_ADDRESS, &CSystemSettingDlg::OnEnSetfocusEdWordOut)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RAD_LOGO1, IDC_RAD_LOGO3, &CSystemSettingDlg::LogoCtrl)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RAD_PLC_MODE1, IDC_RAD_PLC_MODE2, &CSystemSettingDlg::PLCCtrl)
	ON_EN_SETFOCUS(IDC_ED_IP, &CSystemSettingDlg::OnEnSetfocusEdIP)
	ON_EN_SETFOCUS(IDC_ED_PORT, &CSystemSettingDlg::OnEnSetfocusEdPort)
	ON_CBN_SELCHANGE(IDC_COM_IMAGEOUTFORMAT, &CSystemSettingDlg::OnCbnSelchangeComImageoutformat)
	ON_BN_CLICKED(IDC_CHK_ENABLE_NON_NG_SAVE, &CSystemSettingDlg::OnBnClickedChkEnableNonNgSave)
	ON_BN_CLICKED(IDC_CHK_ENABLE_NON_NG_STOP, &CSystemSettingDlg::OnBnClickedChkEnableNonNgStop)
	ON_EN_SETFOCUS(IDC_ED_OVERFLOW_MAX, &CSystemSettingDlg::OnSetfocusEdOverflowMax)
//	ON_WM_SETFOCUS()
ON_EN_SETFOCUS(IDC_ED_MISS_TAB_ID_MAX, &CSystemSettingDlg::OnSetfocusEdMissTabIdMax)
ON_BN_CLICKED(IDC_CHK_DISABLE_SPCPLUS, &CSystemSettingDlg::OnBnClickedChkDisableSpcplus)
END_MESSAGE_MAP()





int CSystemSettingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CSystemSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

}

void CSystemSettingDlg::OnBnClickedBtnOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	if (MessageBox(UiText4[0][__Lang], 0, MB_YESNO) == IDYES) {
		m_SysSetting.FileCtrl(MODE_WRITE);
		AprData.m_System = m_SysSetting;
	}

	CDialogEx::OnOK();
}


void CSystemSettingDlg::OnBnClickedBtnCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


BOOL CSystemSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_EdDummy.SetFocus();

	//if (m_pSysSetting == NULL)
	//{
	//	m_pSysSetting = new CSystemSetting;
	//}
	//m_SysSetting.FileCtrl(MODE_READ);

	// 22.09.01 Ahn Add Start
	
	// 22.09.01 Ahn Add End

	DataControl(MODE_READ);

	MakeGridCtrl();

	DisplayLanguage();  // 22.09.02 Ahn Add

	CWnd* pWnd = NULL;
	pWnd = (CWnd*)GetDlgItem(IDC_ST_DEBUG_SETTING);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_DEBUG_SIGOUT);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_DEBUG_SIGIN);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_DEBUG_SIGIN_RUN);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHK_DEBUG_SIGOUT_ALIVEPULSE);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}

	if (m_bStartMode == TRUE) {
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_CAM_PARAM_SET_ALL);
		if (pWnd != nullptr) {
			pWnd->EnableWindow(FALSE);
		}		
	}
	
	// 23.02.28 Son Add Start
	BOOL bShow = SW_HIDE;
	if (m_RadPlcMode == 1) {
		bShow = SW_SHOW;
	}
	pWnd = (CWnd*)GetDlgItem(IDC_ST_IP);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_PORT);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_IP);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_PORT);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}
	// 23.02.28 Son Add End

		//이미지 포맷 세팅 창
	m_ImageFormat.Clear();
	m_ImageFormat.AddString("JPG");
	m_ImageFormat.AddString("BMP");
	m_ImageFormat.AddString("PNG");
	
	CString imgFormat = AprData.getGSt()->GetOutImageFormat();

	if (imgFormat.Compare(".jpg") == 0)
	{
		m_ImageFormat.SetCurSel(0);
	}
	else if (imgFormat.Compare(".bmp") == 0)
	{
		m_ImageFormat.SetCurSel(1);
	}
	else
	{
		m_ImageFormat.SetCurSel(2);
	}

	//SPC+ 사용 설정 초기값 세팅
	m_bChkDisableSpcPlus = m_SysSetting.m_bDisableSpcPlus;
	((CButton*)GetDlgItem(IDC_CHK_DISABLE_SPCPLUS))->SetCheck(m_bChkDisableSpcPlus);
	//SPC+ 출력 여부 플래그 세팅 - SPC+ SpcInfo = 정보파일에 
	SPCINFO->setSPCStartFlag(!m_bChkDisableSpcPlus);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


int CSystemSettingDlg::MakeGridCtrl()
{

	CGridCtrl* pGridCtrl;
	pGridCtrl = &m_GridCameraParam;
	int nRowHeight = 30;
	pGridCtrl->SetRowHeight(0, nRowHeight);

	if (m_pFontGrid == NULL) {
		CFont* pFont = NULL;
		LOGFONT lf;
		pFont = pGridCtrl->GetFont();
		pFont->GetLogFont(&lf);
		CString strFontFace = _T("MS PGOTHIC");
		lf.lfHeight = (long)( AprData.m_System.m_nGridFontSize );
		lf.lfWeight = FW_NORMAL;
		::_tcsnccpy_s(lf.lfFaceName, _countof(lf.lfFaceName), strFontFace.GetBuffer(), _TRUNCATE);
		m_pFontGrid = new CFont();
		m_pFontGrid->CreateFontIndirect(&lf);
		pGridCtrl->SetFont(m_pFontGrid, TRUE);
	}

	int m_nFixCols = 1;
	int m_nFixRows = 1;
	int m_nCols = MAX_CAMERA_NO + 1 ;
	int m_nRows = en_CamPramRows;
	int nRow = 0;
	int nCol = 0;

	//pGridCtrl->SetAutoSizeStyle();

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

	for (nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++) {
		CString strText;
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = 0;
		Item.col = nCol;
		//col 타이틀 테스트 세팅
		Item.strText = UiText1[nCol][__Lang];

		//폰트 픽셀 넓이 저정
		CDC* dc = GetDC();
		pGridCtrl->setGrideFontWidth(nCol, dc, Item.strText, 2);
		//cd 메모리 해제
		ReleaseDC(dc);

		pGridCtrl->SetItem(&Item);
	}

	//// 22.09.01 Ahn Add End
	for (int nRow = 1; nRow < pGridCtrl->GetRowCount(); nRow++)
	{
		for (int nCol = 0; nCol < pGridCtrl->GetColumnCount(); nCol++)
		{
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			if (nRow < pGridCtrl->GetFixedRowCount())
				strText.Format(_T("Column %d"), nCol);
			//Row 타이틀 세팅
			else if (nCol < pGridCtrl->GetFixedColumnCount())
				strText.Format(_T("%s"), UiText2[nRow - 1][__Lang]);
			else
				continue;
				//strText.Format(_T("%d"), nRow * nCol);
			Item.strText = strText;

			//폰트 픽셀 넓이 저정
			CDC* dc = GetDC();
			pGridCtrl->setGrideFontWidth(nCol, dc, strText, 2);
			//cd 메모리 해제
			ReleaseDC(dc);

			pGridCtrl->SetItem(&Item);
			pGridCtrl->SetRowHeight(nRow, nRowHeight);
		}
	}
	//크기조정
	pGridCtrl->setGridFullColumnWidthVec();

	pGridCtrl->ShowWindow(SW_SHOW);
	UpdateGrid();
	return 0;
}
int CSystemSettingDlg::UpdateGrid()
{
	int nRows;

	CGridCtrl* pGrid = &m_GridCameraParam;

	nRows = en_CamPramRows;

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

	int nCameraNo;
	int nParamNo;
	for (int nRow = 1; nRow < nRows; nRow++) {
		nParamNo = nRow - 1;
		for (int nCol = 1; nCol < pGrid->GetColumnCount(); nCol++)
		{
			nCameraNo = nCol - 1;
			CString strText;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = nRow;
			Item.col = nCol;

			switch (nParamNo) {
			//case	en_CamSerialNo:
			//	strText.Format(_T("%s"), m_SysSetting.m_strSerialNo[nCameraNo]);
			//	break;
			case	en_CamResolutionHor:	
				strText.Format(_T("%lf"), m_SysSetting.m_dResolX[nCameraNo]);
				break; 
			case	en_CamAquisitionRate :
				strText.Format(_T("%.5lf"), m_SysSetting.m_dCamLineRate[nCameraNo]);
				break;
			case	en_CamExposureTime:
				strText.Format(_T("%.5lf"), m_SysSetting.m_dExposureTime[nCameraNo]);
				break;
			case	en_CamGain:
				strText.Format(_T("%.1lf"), m_SysSetting.m_dCamGain[nCameraNo]);
				break;
			case	en_CamViewWidth:
				strText.Format(_T("%d"), m_SysSetting.m_nCamViewWidth);
				if (nCameraNo > 0) {
					Item.crFgClr = RGB(100, 100, 100);
				}
				break;
			case	en_CamViewHeight:
				strText.Format(_T("%d"), m_SysSetting.m_nCamViewHeight);
				if (nCameraNo > 0) {
					Item.crFgClr = RGB(100, 100, 100);
				}
				break;
			case	en_CamViewOffset:
				strText.Format(_T("%d"), m_SysSetting.m_nCamViewOffset[nCameraNo]);
				break;

			// 22.07.05 Ahn Add Start
			case	en_CamImgFlipMode :
				strText.Format(_T("% s"), ( (m_SysSetting.m_nCamImageFlip[nCameraNo] == 0) ? _T("Off") : _T("Flip") ) ) ;
				break;
			// 22.07.05 Ahn Add End
			}

			Item.strText = strText;
			pGrid->SetItem(&Item);
			//			pGrid->RedrawCell(Item.row, Item.col);
		}
	}
	pGrid->Refresh();
	return 0;
}

void CSystemSettingDlg::OnBnClickedRadPositiveMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE); 
	DataControl(MODE_WRITE);

}

double CSystemSettingDlg::CalcAquisitionRate(double dSpeed, double dResolutionY)
{
	double dLineRate = dSpeed / dResolutionY * 1000.0;
	return dLineRate;
}


double CSystemSettingDlg::CalcExposureTime(double dAcqRate, double dSpeed)
{
	double dExposureTime = ( (dSpeed * 1000.0 ) / dAcqRate) - 0.83333333;
	return dExposureTime;
}


BOOL CSystemSettingDlg::CheckExposureTime(double dAcqRate, double* dExpTime)
{
	ASSERT(dExpTime);

	double dExposureTime = CalcExposureTime(dAcqRate, 1000.0 );

	if (*dExpTime > dExposureTime) {
		return FALSE;
	}

	return TRUE;
}

BOOL CSystemSettingDlg::CheckViewWidthAndOffset(int nWidth, int nOffset)
{
	BOOL bRet = FALSE;

	//this->m_SysSetting.

	return bRet;
}

BOOL CSystemSettingDlg::CheckPrmWidth(int nWidth)
{
	BOOL bRet = TRUE;

	for( int i = 0 ; i < MAX_CAMERA_NO ; i++ ){
		int nAllWidth = ( nWidth + m_SysSetting.m_nCamViewOffset[i] );

		if (nAllWidth > MAX_FRAME_SIZE) {
			bRet = FALSE;
			break ;
		}
	}
	return bRet;
}

BOOL CSystemSettingDlg::CheckPrmOffset(int nOffset)
{
	BOOL bRet = TRUE;

	int nAllWidth = ( m_SysSetting.m_nCamViewWidth + nOffset );

	if (nAllWidth > MAX_FRAME_SIZE) {
		bRet = FALSE;
	}

	return bRet;
}

// 22.07.25 Ahn Add Start
void CSystemSettingDlg::OnDBClickGridCtrlCamParam(NMHDR* pNMHDR, LRESULT* pResult)
{
	CGridCtrl* pGridCtrl = &m_GridCameraParam;

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
		CString strValue = _T("");
		szTemp = pGridCtrl->GetTextMatrix(iRow, iCol);
		CSetValue	setvalue(this);
		CString	guidemsg = _T("");
		int nTemp = 0;

		int nCameraNo = iCol - 1;
		int nParamNo = (iRow - 1);

		BOOL bUpdateGrid = FALSE;
		switch (nParamNo) {
		case	en_CamImgFlipMode:
			if (m_SysSetting.m_nCamImageFlip[nCameraNo] == 0) {
				m_SysSetting.m_nCamImageFlip[nCameraNo] = 1;
			}
			else {
				m_SysSetting.m_nCamImageFlip[nCameraNo] = 0;
			}
			bUpdateGrid = TRUE;
			break;
		default :
			break;
		}

		if (bUpdateGrid == TRUE) {
			UpdateGrid();
		}

		pGridCtrl->Refresh();
	}
}
// 22.07.25 Ahn Add End

void CSystemSettingDlg::OnClickGridCtrlCamParam(NMHDR* pNMHDR, LRESULT* pResult)
{
	CGridCtrl* pGridCtrl = &m_GridCameraParam;

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
		CString strValue = _T("");
		szTemp = pGridCtrl->GetTextMatrix(iRow, iCol);
		CSetValue	setvalue(this);
		CString	guidemsg = _T("");
		int nTemp = 0;

		int nCameraNo = iCol - 1;
		int nParamNo = (iRow - 1);

		BOOL bUpdateGrid = FALSE;

		BOOL bCheckExpTime = FALSE ;
		switch (nParamNo) {
		//case	en_CamSerialNo:
		//	guidemsg = _T("카메라 No.%d의 시리얼 번호를 입력해 주세요.");
		//	setvalue.SetValue(guidemsg, strValue, 20, 5);
		//	pGridCtrl->SetTextMatrix(iRow, iCol, strValue);
		//	m_SysSetting.m_strSerialNo[nCameraNo] = strValue;
		//	break;
		case	en_CamResolutionHor:
			guidemsg = _LANG( _T("카메라 X 축 분해능을 입력해 주세요.(단위 um)"), _T("Please enter the camera x resolution.[um]") );
			dTmpValue = atof(szTemp.GetBuffer());
			setvalue.SetValue(guidemsg, dTmpValue, 200.0, 1.0);
			szTemp.Format(_T("%lf"), dTmpValue);
			pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
			m_SysSetting.m_dResolX[nCameraNo] = dTmpValue;
			break;
		case	en_CamAquisitionRate :
			guidemsg = _LANG( _T("카메라 Line Rate를 입력해 주세요."), _T("Please enter the camera line rate.") );
			dTmpValue = atof(szTemp.GetBuffer());
			setvalue.SetValue(guidemsg, dTmpValue, 1000000.0, 300.0);
			szTemp.Format(_T("%lf"), dTmpValue);
			pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
			m_SysSetting.m_dCamLineRate[nCameraNo] = dTmpValue;
			bCheckExpTime = TRUE;
			break;
		case	en_CamExposureTime:
			guidemsg = _LANG( _T("카메라 노광시간(Exposure Time)을 입력해 주세요.(단위 usec)"), _T("Please enter the camera expoure time.[usec]") );
			dTmpValue = atof(szTemp.GetBuffer());
			setvalue.SetValue(guidemsg, dTmpValue, 1449.99, 2.4556);
			szTemp.Format(_T("%lf"), dTmpValue);
			pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
			m_SysSetting.m_dExposureTime[nCameraNo] = dTmpValue;
			break;
		case	en_CamGain:
			guidemsg = _LANG( _T("카메라 게인값을 입력해 주세요."), _T("Please enter the camera gain value.") );
			dTmpValue = atof(szTemp.GetBuffer());
			setvalue.SetValue(guidemsg, dTmpValue, 9.9, 1.0);
			szTemp.Format(_T("%lf"), dTmpValue);
			pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
			m_SysSetting.m_dCamGain[nCameraNo] = dTmpValue;
			break;
		case	en_CamViewWidth:
			if (nCameraNo == 0) {
				guidemsg = _LANG( _T("카메라 프레임의 사이즈를 입력해 주세요. (단위 Pixel)"), _T("Please enter thre camera frame width.[pixel]") );
				nTemp = atoi(szTemp.GetBuffer());
				setvalue.SetValue(guidemsg, nTemp, MAX_FRAME_SIZE, MIN_FRAME_SIZE); // 22.03.25 Ahn Modify
				if (CheckPrmWidth(nTemp) == TRUE) {
					szTemp.Format(_T("%d"), nTemp);
					pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
					m_SysSetting.m_nCamViewWidth = nTemp;
					bUpdateGrid = TRUE;
				} else {
					CString strMsg;
					strMsg.Format(_LANG( _T("입력한 카메라 뷰폭 값은 범위를 초과합니다.")
										,_T("The entered camera view width value is out of range.") ) );
					MessageBox(strMsg);
				}
			}
			break;
		case	en_CamViewHeight:
			if (nCameraNo == 0) {
				guidemsg = _LANG( _T("카메라 프레임의 높이를 입력해 주세요. (단위 Pixel)"), _T("please enter the camera frame height.[pixel]") );
				nTemp = atoi(szTemp.GetBuffer());
				setvalue.SetValue(guidemsg, nTemp, 30000, 0);
				szTemp.Format(_T("%d"), nTemp);
				pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
				m_SysSetting.m_nCamViewHeight = nTemp;
				bUpdateGrid = TRUE;
			}
			break;
		case	en_CamViewOffset :
			guidemsg = _LANG(_T("카메라 프레임의 오프셋(왼쪽기준) 입력해 주세요. (단위 Pixel)"), _T("Please enter the camera frame view offset."));
			nTemp = atoi(szTemp.GetBuffer());
			setvalue.SetValue(guidemsg, nTemp, 14000, 0);
			if (CheckPrmOffset(nTemp) == TRUE) {
				szTemp.Format(_T("%d"), nTemp);
				pGridCtrl->SetTextMatrix(iRow, iCol, szTemp);
				m_SysSetting.m_nCamViewOffset[nCameraNo] = nTemp;
				bUpdateGrid = TRUE;
			} else {
				CString strMsg;
				strMsg.Format(_LANG(_T("입력한 카메라 오프셋 값은 카메라 범위를 초과합니다."), _T("Please enter the camera frame offset value is out of range.") ) );
				MessageBox(strMsg);
			}
			break;
		}

		if (bCheckExpTime == TRUE) {
			double dExpTime = m_SysSetting.m_dExposureTime[nCameraNo] ;
			if (CheckExposureTime(dTmpValue, &dExpTime) == FALSE) {
				m_SysSetting.m_dExposureTime[nCameraNo] = dExpTime;
				bUpdateGrid = TRUE;
			}
		}

		if (bUpdateGrid == TRUE) {
			UpdateGrid();
		}

		pGridCtrl->Refresh();
	}
}


void CSystemSettingDlg::OnEnSetfocusEdEdgeDetMeanWidth()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;
	int nValue = m_nEdMeanWidth ;
	int nMax = 200 ;
	int nMin = 10 ;
	setValue.SetValue(strMsg, nValue, nMax, nMin);
	m_nEdMeanWidth = nValue ;
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}


void CSystemSettingDlg::OnEnSetfocusEdResolutionVer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();
	CSetValue setValue(this);
	CString strMsg;
	double dMax = 200.0;
	double dMin = 1.0;
	setValue.SetValue(strMsg, m_dEdResolY, dMax, dMin) ;
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}


int CSystemSettingDlg::DataControl(int nMode)
{

	if (nMode == MODE_WRITE) {		
		m_SysSetting.m_nMachineMode = m_nRadMachineMode;
		m_SysSetting.m_bTriggerMode = m_bChkTriggerMode;
		m_SysSetting.m_nEdgeSearchMean = m_nEdMeanWidth;
		m_SysSetting.m_dResolY = m_dEdResolY;
		m_SysSetting.m_bChkEnableMarker = m_bChkEnableMarker;
		m_SysSetting.m_nJpegSaveQuality = m_nJpgSaveQulaity;
		m_SysSetting.m_nOverflowCountMax = m_nEdOverflowCountMax;
		m_SysSetting.m_nMissTabIdMax = m_nEdMissTabIdMax;
		m_SysSetting.m_bMarkingAllTab = m_bChkMarkingAllTab ;
		m_SysSetting.m_bEnableNgStop = m_bChkEnableNgStop;
		m_SysSetting.m_strMachineID = m_EdMachineID;
		m_SysSetting.m_nResultSavePeriod = m_nEdSavePeriod;
		m_SysSetting.m_nDiskCapacity = m_nDiskCapacityAlarm;

		switch (m_nLanguage) {
		case	0 : 
			m_SysSetting.m_strLangCode = LANGUAGE_CODE_KOR;
			break;
		case	1 :
			m_SysSetting.m_strLangCode = LANGUAGE_CODE_ENG;
			break;
		}
		m_SysSetting.m_nBitIn = (int)strtol(m_EdBitIn, NULL, 10);
		m_SysSetting.m_nBitOut = (int)strtol(m_EdBitOut, NULL, 10);
		m_SysSetting.m_nWordIn = (int)strtol(m_EdWordIn, NULL, 10);
		m_SysSetting.m_nWordOut = (int)strtol(m_EdWordOut, NULL, 10);
		m_SysSetting.m_nLogo = m_RadLogo;
		m_SysSetting.m_nPlcMode = m_RadPlcMode;
		m_SysSetting.m_strPLCIPAddress = m_EdIPAddress;
		m_SysSetting.m_nPLCPort = m_nEdPort;
		CString strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nBitIn);
		m_EdBitIn_16 = strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nBitOut);
		m_EdBitOut_16 = strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nWordIn);
		m_EdWordIn_16 = strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nWordOut);
		m_EdWordOut_16 = strTmp;
		// 23.03.03 Son Add End

		m_SysSetting.m_bNonNgSave = m_bChkEnableNonNgSave;
		m_SysSetting.m_bNonNgStop = m_bChkEnableNonNgStop;

		//SPC Plus 실행여부 전역 변수에 세팅한다.
		m_SysSetting.m_bDisableSpcPlus = m_bChkDisableSpcPlus;


		UpdateData(FALSE);
	}
	else {
		UpdateData(TRUE);
		m_nRadMachineMode = m_SysSetting.m_nMachineMode ;
		m_bChkTriggerMode = m_SysSetting.m_bTriggerMode;		
		m_nEdMeanWidth = m_SysSetting.m_nEdgeSearchMean ;
		m_dEdResolY = m_SysSetting.m_dResolY ;
		m_SysSetting.m_dResolY1000P = m_dEdResolY;
		m_bChkEnableMarker = m_SysSetting.m_bChkEnableMarker ;
		m_nJpgSaveQulaity = m_SysSetting.m_nJpegSaveQuality ;
		m_nEdOverflowCountMax = m_SysSetting.m_nOverflowCountMax;
		m_nEdMissTabIdMax = m_SysSetting.m_nMissTabIdMax;
		m_bChkMarkingAllTab = m_SysSetting.m_bMarkingAllTab ;
		m_bChkEnableNgStop = m_SysSetting.m_bEnableNgStop ;
		m_EdMachineID = m_SysSetting.m_strMachineID ;
		m_nEdSavePeriod = m_SysSetting.m_nResultSavePeriod ;
		m_nDiskCapacityAlarm = m_SysSetting.m_nDiskCapacity ;
		if (m_SysSetting.m_strLangCode.Compare(LANGUAGE_CODE_KOR) == 0) {
			m_nLanguage = 0 ;
		}
		else if (m_SysSetting.m_strLangCode.Compare(LANGUAGE_CODE_ENG) == 0) {
			m_nLanguage = 1 ;
		}
		CString strTmp;
		strTmp.Format(_T("%d"), m_SysSetting.m_nBitIn);
		m_EdBitIn = strTmp;
		strTmp.Format(_T("%d"), m_SysSetting.m_nBitOut);
		m_EdBitOut = strTmp;
		strTmp.Format(_T("%d"), m_SysSetting.m_nWordIn);
		m_EdWordIn = strTmp;
		strTmp.Format(_T("%d"), m_SysSetting.m_nWordOut);
		m_EdWordOut = strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nBitIn);
		m_EdBitIn_16 = strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nBitOut);
		m_EdBitOut_16 = strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nWordIn);
		m_EdWordIn_16 = strTmp;
		strTmp.Format(_T("%X"), m_SysSetting.m_nWordOut);
		m_EdWordOut_16 = strTmp;
		m_RadLogo = m_SysSetting.m_nLogo;
		m_RadPlcMode = m_SysSetting.m_nPlcMode;
		m_EdIPAddress = m_SysSetting.m_strPLCIPAddress;
		m_nEdPort = m_SysSetting.m_nPLCPort;
		m_bChkEnableNonNgSave = m_SysSetting.m_bNonNgSave;
		m_bChkEnableNonNgStop = m_SysSetting.m_bNonNgStop;

		//SPC Plus 실행여부
		m_bChkDisableSpcPlus = m_SysSetting.m_bDisableSpcPlus;


		UpdateData(FALSE);
	}

	return 0;
}

void CSystemSettingDlg::OnBnClickedChkExternalTrigger()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	DataControl(MODE_WRITE);
}


void CSystemSettingDlg::OnBnClickedBtnCamParamSetAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 검사 중인지 Check하고 검사중엔 사용인 경우 Error 후 Return 

	if (m_pParent->IsRun() == TRUE) {
	//	MessageBox( _LANG( _T("검사중 카메라 파라메터 쓰기는 실행 할 수 없습니다."), _T("") ) ) ;
		return;
	}
	int nLocalRet = 0;

	// 22.05.09 Ahn Add Start - Camera Reset 
	//theApp.m_pImgProcCtrl->ResetCamera();
	// 22.05.09 Ahn Add End

	// 22.01.12 Ahn Modify Start
	BOOL bTriggerMode = m_SysSetting.m_bTriggerMode;
	theApp.m_pImgProcCtrl->SetTriggerMode(bTriggerMode);

	if (bTriggerMode == FALSE) { 
		// External Trigger Mode에서는 Error발생 하므로 값 쓰지 말것.
		double dExpTime = m_SysSetting.m_dExposureTime[0];
		nLocalRet = theApp.m_pImgProcCtrl->SetExposureTime(dExpTime);
	}
	// 22.01.12 Ahn Modify End

	// 22.05.09 Ahn Delete Start -  Grab 이후 실행하면 Error 발생됨
	//int nWidth = m_SysSetting.m_nCamViewWidth;
	//int nHeight = m_SysSetting.m_nCamViewHeight;
	//nLocalRet = theApp.m_pImgProcCtrl->SetFrameHeight(nHeight);
	//nLocalRet = theApp.m_pImgProcCtrl->SetFrameWidth(nWidth);
	// 22.05.09 Ahn Delete End

	for (int i = 0; i < MAX_CAMERA_NO;i++) {
		double dGain = m_SysSetting.m_dCamGain[i];
		theApp.m_pImgProcCtrl->SetGain(i, dGain);
		
		// 22.05.09 Ahn Delete Start - Grab 이후 실행하면 Error 발생됨
		int nOffset = m_SysSetting.m_nCamViewOffset[i];
		theApp.m_pImgProcCtrl->SetFrameOffset(i, nOffset);
		// 22.05.09 Ahn Delete End
	}
}

// 22.08.31 Ahn Delete Start
//void CSystemSettingDlg::OnEnSetfocusEdGridFontSize()
//{
//	m_EdDummy.SetFocus();
//
//	CSetValue setValue(this);
//	CString strMsg;
//	int nValue = m_nGridFondSize;
//	int nMax = 40;
//	int nMin = 4;
//	setValue.SetValue(strMsg, m_nGridFondSize, nMax, nMin);
//	DataControl(MODE_WRITE);
//
//	UpdateData(FALSE);
//}
// 22.08.31 Ahn Delete End

void CSystemSettingDlg::OnBnClickedChkEnableMarker()
{
	UpdateData(TRUE);
	DataControl(MODE_WRITE);
}

// 22.01.11 Ahn Add Start
void CSystemSettingDlg::OnEnClickChkMarkingAllTab()
{
	UpdateData(TRUE);
	DataControl(MODE_WRITE);
}
// 22.01.11 Ahn Add End

void CSystemSettingDlg::OnEnSetfocusEdJpegSaveQuality()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 100;
	int nMin = 10;
	setValue.SetValue(strMsg, m_nJpgSaveQulaity, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
	
}


void CSystemSettingDlg::OnBnClickedChkFirstTabDonotProc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	DataControl(MODE_WRITE);
}


void CSystemSettingDlg::OnBnClickedChkEnableNgStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	DataControl(MODE_WRITE);
}



void CSystemSettingDlg::OnEnSetfocusEdMachineId()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 32 ;
	int nMin = 10 ;
	setValue.SetValue(strMsg, m_EdMachineID, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}


void CSystemSettingDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//if (m_pSysSetting != NULL) {
	//	delete m_pSysSetting;
	//	m_pSysSetting = NULL;
	//}

	if (m_pFontGrid != NULL) {
		delete m_pFontGrid;
		m_pFontGrid = NULL;
	}
}

// 22.08.31 Ahn Delete Start
//void CSystemSettingDlg::OnEnSetfocusEdGraphSizeFoilexp()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_EdDummy.SetFocus();
//
//	CSetValue setValue(this);
//	CString strMsg;
//
//	int nMax = 500;
//	int nMin = 100;
//	setValue.SetValue(strMsg, m_nGraphSizeFoilExp, nMax, nMin);
//	DataControl(MODE_WRITE);
//
//	UpdateData(FALSE);
//}
//
//void CSystemSettingDlg::OnEnSetfocusEdGraphSizeSurface()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_EdDummy.SetFocus();
//
//	CSetValue setValue(this);
//	CString strMsg;
//
//	int nMax = 500;
//	int nMin = 100;
//	setValue.SetValue(strMsg, m_nGraphSizeSurface, nMax, nMin);
//	DataControl(MODE_WRITE);
//
//	UpdateData(FALSE);
//}
// 22.08.31 Ahn Delete End

// 22.07.04 Ahn Add Start
void CSystemSettingDlg::OnEnSetfocusEdResultSavePeriod()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 365;
	int nMin = 0;
	setValue.SetValue(strMsg, m_nEdSavePeriod, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}

void CSystemSettingDlg::OnEnSetfocusEdDiskCapacityAlarm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 2000;
	int nMin = 1;
	setValue.SetValue(strMsg, m_nDiskCapacityAlarm, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}
// 22.07.04 Ahn Add End


void CSystemSettingDlg::OnBnClickedRadKorean()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	DataControl(MODE_WRITE);
}


void CSystemSettingDlg::OnBnClickedRadEnglish()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	DataControl(MODE_WRITE);
}

// 22.09.01 Ahn Add Start
void CSystemSettingDlg::DisplayLanguage()
{
	CWnd* pWnd;
	CString strDispName;
	for (int idx = 0; idx < MAX_COUNT; idx++)
	{
		pWnd = NULL;
		pWnd = GetDlgItem(UiText3NameText[idx]);
		if (pWnd != nullptr)
		{
			strDispName = UiText3[idx][__Lang];
			pWnd->SetWindowTextA(strDispName);
		}
	}
}
// 22.09.01 Ahn Add End


// 23.02.17 Son Add Start
void CSystemSettingDlg::OnEnSetfocusEdBitIn()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 5;
	int nMin = 4;
	setValue.SetValue(strMsg, m_EdBitIn, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}
void CSystemSettingDlg::OnEnSetfocusEdBitOut()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 5;
	int nMin = 4;
	setValue.SetValue(strMsg, m_EdBitOut, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}
void CSystemSettingDlg::OnEnSetfocusEdWordIn()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 5;
	int nMin = 4;
	setValue.SetValue(strMsg, m_EdWordIn, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}
void CSystemSettingDlg::OnEnSetfocusEdWordOut()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 5;
	int nMin = 4;
	setValue.SetValue(strMsg, m_EdWordOut, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}
// 23.02.17 Son Add End

// 23.02.27 Son Add Start
void CSystemSettingDlg::LogoCtrl(UINT id)
{
	UpdateData(TRUE);
	switch (id)
	{
	case IDC_RAD_LOGO1:
		m_RadLogo = 0;
		break;
	case IDC_RAD_LOGO2:
		m_RadLogo = 1;
		break;
	case IDC_RAD_LOGO3:
		m_RadLogo = 2;
		break;
	default:
		break;
	}
	DataControl(MODE_WRITE);
}
// 23.02.27 Son Add End
// 23.02.28 Son Add Start
void CSystemSettingDlg::PLCCtrl(UINT id)
{
	UpdateData(TRUE);
	BOOL bShow = SW_HIDE;
	switch (id)
	{
	case IDC_RAD_PLC_MODE1:
		m_RadPlcMode = 0;
		break;
	case IDC_RAD_PLC_MODE2:
		m_RadPlcMode = 1;
		bShow = SW_SHOW;
		break;
	default:
		break;
	}
	CWnd* pWnd = NULL;
	pWnd = (CWnd*)GetDlgItem(IDC_ST_IP);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ST_PORT);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_IP);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}

	pWnd = (CWnd*)GetDlgItem(IDC_ED_PORT);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(bShow);
	}
	UpdateData(FALSE);
	DataControl(MODE_WRITE);
}

void CSystemSettingDlg::OnEnSetfocusEdIP()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 32;
	int nMin = 10;
	setValue.SetValue(strMsg, m_EdIPAddress, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}

void CSystemSettingDlg::OnEnSetfocusEdPort()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 99999;
	int nMin = 1000;
	setValue.SetValue(strMsg, m_nEdPort, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}
// 23.02.28 Son Add End



void CSystemSettingDlg::OnCbnSelchangeComImageoutformat()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	switch (m_ImageFormat.GetCurSel())
	{
	case 1: AprData.getGSt()->SetOutImageFormat(".bmp");
		break;
	case 2: AprData.getGSt()->SetOutImageFormat(".png");
		break;
	case 0:
	default: AprData.getGSt()->SetOutImageFormat(".jpg");
		break;
	}

}


void CSystemSettingDlg::OnBnClickedChkEnableNonNgSave()
{
	UpdateData(TRUE);

	DataControl(MODE_WRITE);

}


void CSystemSettingDlg::OnBnClickedChkEnableNonNgStop()
{
	UpdateData(TRUE);

	DataControl(MODE_WRITE);

}


void CSystemSettingDlg::OnSetfocusEdOverflowMax()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 100;
	int nMin = 2;
	setValue.SetValue(strMsg, m_nEdOverflowCountMax, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}



void CSystemSettingDlg::OnSetfocusEdMissTabIdMax()
{
	m_EdDummy.SetFocus();

	CSetValue setValue(this);
	CString strMsg;

	int nMax = 100;
	int nMin = 0;
	setValue.SetValue(strMsg, m_nEdMissTabIdMax, nMax, nMin);
	DataControl(MODE_WRITE);

	UpdateData(FALSE);
}

void CSystemSettingDlg::OnBnClickedChkDisableSpcplus()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef SPCPLUS_CREATE
//SPC+ 사용 해더파일
	//if (((CButton*)GetDlgItem(IDC_CHK_DISABLE_SPCPLUS))->GetCheck() == TRUE)
	//{
	//	SPCINFO->setSPCStartFlag(FALSE);
	//}
	//else
	//{
	//	SPCINFO->setSPCStartFlag(TRUE);
	//}

	UpdateData(TRUE);
	//SPC+ 출력여부 체크 박스 체크 이벤트 시 SpcInfo - 정보 클래스 변경한다.
	SPCINFO->setSPCStartFlag(!m_bChkDisableSpcPlus);

	DataControl(MODE_WRITE);
#endif //SPCPLUS_CREATE
}