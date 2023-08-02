// CDebugImageAcqDlg.cpp: 구현 파일
//

#include "pch.h"
#include "NotchingGradeInsp.h"
#include "NotchingGradeInspView.h"
#include "CDebugImageAcqDlg.h"
#include "afxdialogex.h"
#include "ImageProcessCtrl.h"
#include "Win32File.h"
#include "BitmapStd.h"
#include "QueueCtrl.h"
#include "FrameInfo.h"
#include "CImageProcess.h"
#include "GlobalData.h"
#include "CRecipeCtrl.h"
#include "CThreadQueueCtrl.h"
#include "CImageProcSimDlg.h"

// CDebugImageAcqDlg 대화 상자
#define T_ID_THREAD_CHECK 105 

IMPLEMENT_DYNAMIC(CDebugImageAcqDlg, CDialogEx)

CDebugImageAcqDlg::CDebugImageAcqDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEBUG_IMG_ACQ_DLG, pParent)
	, m_nImgAcqCount(0)
	, m_strFilePath(_T(""))
	, m_strLotID(_T(""))
	, m_bChkProcessAll(FALSE)
{
	m_pView = (CNotchingGradeInspView*)pParent;
	m_pRecipeCtrl = NULL;
	m_pThread = NULL; 
}

CDebugImageAcqDlg::~CDebugImageAcqDlg()
{
	if (m_pRecipeCtrl != NULL) {
		delete m_pRecipeCtrl;
	}

	if (m_pThread != NULL) {
		Kill();
	}
}

void CDebugImageAcqDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_IMAGE_COUNT, m_nImgAcqCount);
	DDX_Text(pDX, IDC_ED_SEL_DIR, m_strFilePath);
	DDX_Control(pDX, IDC_COM_RECIPE_SELECT, m_cmbRecipeSel);
	DDX_Text(pDX, IDC_ED_LOT_ID, m_strLotID);
	DDX_Check(pDX, IDC_CHK_INFINIT, m_bChkProcessAll);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CDebugImageAcqDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SELECT_FOLDER, &CDebugImageAcqDlg::OnBnClickedBtnSelectFolder)
	ON_BN_CLICKED(IDC_BTN_IMAGE_ACQ_START, &CDebugImageAcqDlg::OnBnClickedBtnImageAcqStart)
	ON_BN_CLICKED(IDC_BTN_INITIALIZE, &CDebugImageAcqDlg::OnBnClickedBtnInitialize)
	ON_CBN_SELCHANGE(IDC_COM_RECIPE_SELECT, &CDebugImageAcqDlg::OnCbnSelchangeComRecipeSelect)
	ON_BN_CLICKED(IDC_BTN_IMAGE_ACQ_STOP, &CDebugImageAcqDlg::OnBnClickedBtnImageAcqStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ONE_BY_ONE, &CDebugImageAcqDlg::OnBnClickedBtnOneByOne)
END_MESSAGE_MAP()


// CDebugImageAcqDlg 메시지 처리기


void CDebugImageAcqDlg::OnBnClickedBtnSelectFolder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szFilter[256] = _T("Image Files (*.bmp;*.JPG;*.JPEG)|*.bmp;*.JPG;*.JPEG|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("bmp"), m_strFilePath, OFN_READONLY, szFilter, this);
	if (dlg.DoModal() == IDOK) {
		CString strSelectFile;
		strSelectFile = dlg.GetPathName();
		int nPos = strSelectFile.ReverseFind('\\');

		m_strFilePath = strSelectFile.Left(nPos );
		UpdateData(FALSE);
		//LoadImageFile(m_strSrcFileName);
	}

}


void CDebugImageAcqDlg::OnBnClickedBtnImageAcqStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCamNo = 0;
	UpdateData(TRUE);

	if (m_ThreadPrm.bRunFlag == TRUE ) {
		MessageBox(_T("이미 실행 중입니다."));
		return;
	}

	CString strSavePath;
	CString strYYYYMM;
	CString strHH;
	CString strDD;
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	// 22.03.03 Ahn Modify Start
	//strTime.Format(_T("%02d\\%02d"), sysTime.wMonth, sysTime.wDay);
	strYYYYMM.Format(_T("%d%02d"), sysTime.wYear, sysTime.wMonth);
	strHH.Format(_T("%02d"), sysTime.wHour);
	strDD.Format(_T("%02d"), sysTime.wDay);
	// 22.03.03 Ahn Modify End

	// 22.08.10 Ahn Modify Start
	AprData.m_strNowDatePath.Format(_T("%s\\%s\\%s"), strYYYYMM, strDD, strHH);
	strSavePath.Format(_T("%s\\NG\\%s\\%s"), AprData.m_strImagePath, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);
	AprData.m_strNowNgPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	strSavePath.Format(_T("%s\\CROP"), AprData.m_strNowNgPath);
	CWin32File::CreateDirectory(strSavePath);
	strSavePath.Format(_T("%s\\Overlay"), AprData.m_strNowNgPath);
	CWin32File::CreateDirectory(strSavePath);
	strSavePath.Format(_T("%s\\OK\\%s\\%s"), AprData.m_strImagePath, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);
	AprData.m_strNowOkPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	strSavePath.Format(_T("%s\\%s\\%s"), AprData.m_strFeederPath, strYYYYMM, strDD);
	AprData.m_strNowCsvPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	strSavePath.Format(_T("%s\\BINARY\\%s\\%s"), AprData.m_strResultPath, AprData.m_strNowDatePath, AprData.m_NowLotData.m_strLotNo);
	AprData.m_strNowBinPath = strSavePath;
	CWin32File::CreateDirectory(strSavePath);
	// 22.08.10 Ahn Modify End

	m_ThreadPrm.bRunFlag = TRUE;

	//m_ThreadPrm.bProcAllMode = m_bChkProcessAll;
	m_ProgressCtrl.SetPos(0);
	if (m_bChkProcessAll == FALSE) {
		m_ThreadPrm.nTargetCount = m_nImgAcqCount;
	}
	else {
		int nMaxSize = 0;
		
		nMaxSize = (int)m_ThreadPrm.m_pvecTabList->size();
		m_ThreadPrm.nTargetCount = nMaxSize;
	}

	if ((m_bChkProcessAll == TRUE) && (m_nImgAcqCount ==1)) {
		m_ThreadPrm.bOneByOne = TRUE;
	}
	else {
		m_ThreadPrm.bOneByOne = FALSE;
	}
	m_ProgressCtrl.SetRange(0, m_ThreadPrm.nTargetCount);

	m_ThreadPrm.nProcCount = 0;

	Begin();
}

void CDebugImageAcqDlg::OnBnClickedBtnImageAcqStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ThreadPrm.bRunFlag = FALSE;
	Kill();
}

int CDebugImageAcqDlg::ClearTabList()
{
//	int nSize = (int)m_vecTabList.size();
//
//	VEC_TAB_LIST::iterator iterF, iterL;
////	for (iter = m_vecTabList.begin(); iter != m_vecTabList.end(); ) {
////		m_vecTabList.erase(iter++);
////	}
//	if (nSize > 0) {
//		iterF = m_vecTabList.begin();
//		iterL = m_vecTabList.end();
//		m_vecTabList.erase(iterF, iterL);
//	}
	//m_ThreadPrm.m_pvecTabList = NULL;
	//Sleep(1000);
	//m_vecTabList.clear();
	return 0;
}


void CDebugImageAcqDlg::OnBnClickedBtnInitialize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_ThreadPrm.bRunFlag == TRUE) {
		MessageBox(_T("실행중입니다. 정지 후 다시 시도하세요."));
		return;
	}

	m_nTotalCount = 0 ;
	m_nNowCount = 0 ;

	m_vecTabList.clear();
	//ClearTabList();

	VEC_UNIT_LIST vecUnitList[MAX_CAMERA_NO];
	vecUnitList[0].clear();
	vecUnitList[1].clear();

	UpdateData(TRUE);
	BOOL bExist = CWin32File::ExistFile(m_strFilePath);

	if (bExist==TRUE) {
		CStringList strList;
		CWin32File::GetFileList(m_strFilePath, strList);
		m_ThreadPrm.strImagePath = m_strFilePath;

		int nSize = (int) strList.GetCount();
		POSITION pos = strList.GetHeadPosition();
		CString strFileName;


		for (; pos != NULL; ) {
			CDebugAcqUnit unit;
			strFileName = strList.GetNext(pos);

			// 23.02.16 Ahn Test Start
			//CString strLotID = CImageProcSimDlg::GetTokenFromString(strFileName, '_', 3);
			//CString strTabNo = CImageProcSimDlg::GetTokenFromString(strFileName, '_', 4);
			//CString strPos = CImageProcSimDlg::GetTokenFromString(strFileName, '_', 5);
			CString strLotID = CImageProcSimDlg::GetTokenFromString(strFileName, '_', 2);
			CString strTabNo = CImageProcSimDlg::GetTokenFromString(strFileName, '_', 5);
			CString strPos = CImageProcSimDlg::GetTokenFromString(strFileName, '_', 6);
			// 23.02.16 Ahn Test Start
			int nTabNo = atoi(strTabNo);

			if (strPos.Compare("TAB") == 0) {
				unit.nPosition = 0;
			}
			else {
				unit.nPosition = 1;
			}

			unit.strFileName = strFileName;
			unit.nTabNo = nTabNo;
			vecUnitList[unit.nPosition].push_back(unit);
		}
	}

	// Sort 
	SortAcqUnit(vecUnitList[0]);
	SortAcqUnit(vecUnitList[1]);
	// MergeList();
	MergeAcqUnitList(m_vecTabList, vecUnitList[0], vecUnitList[1]);

	m_ThreadPrm.m_pvecTabList = &m_vecTabList;
	m_strLotID = AprData.m_NowLotData.m_strLotNo;

	vecUnitList[0].clear();
	vecUnitList[1].clear();

	UpdateData(FALSE);
}
int CDebugImageAcqDlg::MergeAcqUnitList(VEC_TAB_LIST& vecTabList, VEC_UNIT_LIST& vecTopList, VEC_UNIT_LIST& vecBtmList)
{
	BOOL bEmpty = FALSE;

	//vecTabList.clear();
	ClearTabList();

	VEC_UNIT_LIST::iterator iterTop;
	VEC_UNIT_LIST::iterator iterBtm;
	iterTop = vecTopList.begin();
	iterBtm = vecBtmList.begin();

	while ((iterTop != vecTopList.end()) || (iterBtm != vecBtmList.end() )) {
		CDebugAcqTab AcqTab;
		if (iterTop == vecTopList.end()) {
			// BTM 
			AcqTab.AcqUnit[CAM_POS_TOP].bExist = FALSE;

			AcqTab.AcqUnit[CAM_POS_BOTTOM].bExist = TRUE;
			AcqTab.AcqUnit[CAM_POS_BOTTOM].nPosition = CAM_POS_BOTTOM;
			AcqTab.AcqUnit[CAM_POS_BOTTOM].nTabNo = iterBtm->nTabNo;
			AcqTab.AcqUnit[CAM_POS_BOTTOM].strFileName = iterBtm->strFileName;
			vecTabList.push_back(AcqTab);
			iterBtm++;
		}else if (iterBtm == vecBtmList.end()) {
			AcqTab.AcqUnit[CAM_POS_BOTTOM].bExist = FALSE;

			AcqTab.AcqUnit[CAM_POS_TOP].bExist = TRUE;
			AcqTab.AcqUnit[CAM_POS_TOP].nPosition = CAM_POS_TOP ;
			AcqTab.AcqUnit[CAM_POS_TOP].nTabNo = iterTop->nTabNo;
			AcqTab.AcqUnit[CAM_POS_TOP].strFileName = iterTop->strFileName;
			vecTabList.push_back(AcqTab);
			iterTop++;
		}
		else {
			//if (iterTop->nTabNo == iterBtm->nTabNo) 
			{
				AcqTab.AcqUnit[CAM_POS_TOP].bExist = AcqTab.AcqUnit[CAM_POS_BOTTOM].bExist = TRUE;
				AcqTab.AcqUnit[CAM_POS_TOP].nTabNo = AcqTab.AcqUnit[CAM_POS_BOTTOM].nTabNo = iterTop->nTabNo;
				AcqTab.AcqUnit[CAM_POS_TOP].nPosition = CAM_POS_TOP;
				AcqTab.AcqUnit[CAM_POS_BOTTOM].nPosition = CAM_POS_BOTTOM;
				AcqTab.AcqUnit[CAM_POS_TOP].strFileName = iterTop->strFileName;
				AcqTab.AcqUnit[CAM_POS_BOTTOM].strFileName = iterBtm->strFileName;
				vecTabList.push_back(AcqTab);
				iterTop++;
				iterBtm++;
			}
		}
	}
	return 0;
}

int CDebugImageAcqDlg::SortAcqUnit(VEC_UNIT_LIST& vecUnitList)
{
	int nSize = (int)vecUnitList.size();

	CDebugAcqUnit tempUnit;
	CDebugAcqUnit* pTar, * pSrc;
	for ( int i = 0; i < nSize-1; i++) {
		pSrc = &vecUnitList[i];
		for (int j = i + 1; j < nSize; j++) {
			pTar = &vecUnitList[j];
			if (pSrc->nTabNo > pTar->nTabNo) {
				tempUnit = *pSrc;
				*pSrc = *pTar;
				*pTar = tempUnit;
			}
		}
	}
	return 0;
}


BOOL CDebugImageAcqDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (m_pRecipeCtrl == NULL) {
		m_pRecipeCtrl = new CRecipeCtrl();
	}

	m_pRecipeInfo = m_pRecipeCtrl->GetRecipeInfoPtr();

	CStringList strList;
	m_pRecipeCtrl->SearchRecipeFile(strList);
	int i = 0;
	int nCount = (int)strList.GetSize();
	m_cmbRecipeSel.Clear();
	POSITION pos;
	pos = strList.GetHeadPosition();
	CString strRecipeName;
	int nSelNo = -1;
	for (i = 0; i < nCount; i++) {
		strRecipeName = strList.GetNext(pos);
		strRecipeName.Replace(_T(".ini"), _T(""));
		m_cmbRecipeSel.AddString(strRecipeName);
		if (strRecipeName.Compare(m_pRecipeInfo->m_strRecipeName) == 0) {
			nSelNo = i;
		}
	}

	m_ProgressCtrl.SetRange(0, 100);
	m_ProgressCtrl.SetPos(0);
	m_ThreadPrm.pThis = this;

	SetTimer(T_ID_THREAD_CHECK, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDebugImageAcqDlg::SetProgressPos(int nPos)
{
	m_ProgressCtrl.SetPos(nPos);
	UpdateData(FALSE);
}

void CDebugImageAcqDlg::OnCbnSelchangeComRecipeSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	ASSERT(m_pRecipeCtrl);
	int nSelNo = m_cmbRecipeSel.GetCurSel();
	CString strSelRecipeName = _T("");
	UpdateData(TRUE);
	//	m_cmbRecipeName.GetWindowTextA(strSelRecipeName);
	m_cmbRecipeSel.GetLBText(nSelNo, strSelRecipeName);

	CString strRecipePath;
	strRecipePath.Format(_T("%s\\Recipe"), AprData.m_strDataPath);

	if (m_pRecipeInfo != nullptr) {
		//m_pRecipeCtrl->FileCtrl(strSelRecipeName, MODE_READ, m_pRecipeInfo);
		m_pRecipeCtrl->LoadRecipe(m_pRecipeInfo, strSelRecipeName);
		*AprData.m_pRecipeInfo = *m_pRecipeInfo;

		m_ThreadPrm.pRecipeInfo = AprData.m_pRecipeInfo;
	}
}


// Thread

void CDebugImageAcqDlg::Begin()
{
	m_ThreadPrm.bKill = FALSE;

	//	m_DisphWnd = NULL;
	if (m_pThread == NULL) {
		m_pThread = AfxBeginThread((AFX_THREADPROC)CtrlThreadDebugAcqProc,
			(LPVOID)&m_ThreadPrm,
			THREAD_PRIORITY_HIGHEST,
			0,
			CREATE_SUSPENDED,
			NULL);

		if (m_pThread != NULL) {
			m_pThread->m_bAutoDelete = FALSE;
			m_pThread->ResumeThread();
		}
	}

}
void CDebugImageAcqDlg::Kill(void)
{
	DWORD	dwCode;
	LONG	ret;

	if (m_pThread != NULL) {
		// 스래드 종료 코드를 가져온다.
		ret = ::GetExitCodeThread(m_pThread->m_hThread, &dwCode);
		if (ret && dwCode == STILL_ACTIVE) {
			// 종료코드가 아직 Atctive 이면 종료 세팅하고 종료 대기
			m_ThreadPrm.bKill = TRUE;
			WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		}
		delete m_pThread;
		m_pThread = NULL;
		m_ThreadPrm.bRunFlag = FALSE;

	}

}

int CDebugImageAcqDlg::FileNameFromTabNo(CString strFileName)
{
	int nRet = -1;
	CString strFind = _T("TabNo[");
	int nFindStrSize = strFind.GetLength();
	int nStartPos = strFileName.Find(_T("TabNo[")) + nFindStrSize ;
	int nStrLen = strFileName.GetLength();

	int nEndPos;
	int nCount = 0;
	for( int i = nStartPos; i < nStrLen ; i++, nCount++ ){
		if (strFileName.GetAt(i) == ']') {
			nEndPos = i;
			break;
		}
	}

	CString strNo = strFileName.Mid(nStartPos, nCount);
	nRet = atoi(strNo.GetBuffer());

	return nRet;
}

//
//UINT CDebugImageAcqDlg::CtrlThreadDebugAcqProc(LPVOID pParam)
//{
//	_D_ACQ_THREAD_PARAM* pThreadPrm = (_D_ACQ_THREAD_PARAM*)pParam;
//	//CQueueCtrl* pQueueCtrl = theApp.m_pImgProcCtrl->GetQueuePtr();
//
//	CThreadQueueCtrl* pThreadQue[MAX_CAMERA_NO];
//	pThreadQue[CAM_POS_TOP] = theApp.m_pImgProcCtrl->GetThreadQueuePtr(CAM_POS_TOP);
//	pThreadQue[CAM_POS_BOTTOM] = theApp.m_pImgProcCtrl->GetThreadQueuePtr(CAM_POS_BOTTOM);
//
//	VEC_TAB_LIST::iterator iter;
//	iter = pThreadPrm->m_pvecTabList->begin() ; 
//
//	// 22.09.08 Ahn Add Start
//	BYTE* pImgBackPtr[MAX_CAMERA_NO]; 
//	pImgBackPtr[CAM_POS_TOP] = nullptr ;
//	pImgBackPtr[CAM_POS_BOTTOM] = nullptr ;
//	int nBackLength = 0 ;
//	int nBackWidth = 0 ;
//	int nFrameSize = 9700 ;
//	// 22.09.08 Ahn Add End
//
//	while (1) {
//		if (pThreadPrm == NULL) {
//			break;
//		}
//
//		if (pThreadPrm->bKill == TRUE) {
//			break;
//		}
//
//		if (pThreadPrm->m_pvecTabList == NULL) {
//		//	pThreadPrm->bKill = TRUE;
//		//	break;;
//			continue ;
//		}
//
//		if (pThreadPrm->m_pvecTabList->size() <= 0 ) {
//		//	pThreadPrm->bKill = TRUE;
//		//	break;
//			continue;
//		}
//
//		if (iter == pThreadPrm->m_pvecTabList->end()) {
//			pThreadPrm->bKill = TRUE;
//			break;
//		}
//
//		if( pThreadPrm->m_pvecTabList->empty() == TRUE ){
//			pThreadPrm->bRunFlag = FALSE;
//			continue;
//		}
//
//		if (pThreadPrm->bRunFlag == FALSE) {
//			continue;
//		}
//
//		if ((pThreadPrm->bOneByOne == 1) && (pThreadPrm->bWaitOne == TRUE)) {
//			continue;
//		}
//	
//		CString strNameTop;
//		CString strNameBtm;
//		for (int nCamNo = 0; nCamNo  < MAX_CAMERA_NO; nCamNo++) {
//		//	if (*pPosPtr[nCamNo] == NULL)continue;
//			if (iter->AcqUnit[nCamNo].bExist == FALSE) continue;
//
//			CDebugAcqUnit* pUnit = &(iter->AcqUnit[nCamNo] );
//
//
//			strNameTop = pUnit->strFileName;
//	
//			CString strFullPath;
//			strFullPath.Format(_T("%s\\%s"), pThreadPrm->strImagePath, strNameTop);
//	
//			if (CWin32File::ExistFile(strFullPath) <= 0) {
//				CString strMsg;
//				strMsg.Format(_T("파일이 존재하지 않습니다. %s"), strFullPath);
//				AfxMessageBox(strMsg);
//				pThreadPrm->bRunFlag = FALSE;
//				continue;
//			}
//			int nTabNo = FileNameFromTabNo(strNameTop);
//
//			CBitmapStd bmp;
//			bmp.ReadBitmap(strFullPath);
//			CSize size = bmp.GetImgSize();
//			BYTE* pImgPtr = new BYTE[ size.cx * size.cy  + 1 ];
//			BYTE* pOrgPtr = bmp.GetImgPtr();
//			memcpy(pImgPtr, pOrgPtr, sizeof(BYTE) * size.cx * size.cy);
//
//			int nLeft = 0;
//			int nRight = 0;
//			int nLevel = 0;
//
//			int nInspMode = CFrameInfo::en_TopFrame;
//			if (nCamNo == CAM_POS_TOP) {
//				CImageProcess::FindTabLevel(pImgPtr, size.cx, size.cy, &nLevel, pThreadPrm->pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_TOP], CImageProcess::en_FindLeft);
//				CImageProcess::GetTabHeadPos(pImgPtr, size.cx, size.cy, pThreadPrm->pRecipeInfo, nLeft, nRight, nLevel);
//				CFrameInfo::en_TopFrame;
//			} else {
//				CImageProcess::FindTabLevel(pImgPtr, size.cx, size.cy, &nLevel, pThreadPrm->pRecipeInfo->TabCond, AprData.m_pRecipeInfo->TabCond.nEdgeFindMode[CAM_POS_BOTTOM], CImageProcess::en_FindRight);
//				nInspMode = CFrameInfo::en_BottomFrame;
//			}
//
//			CFrameInfo* pFrmInfo;
//			pFrmInfo = new CFrameInfo;
//			pFrmInfo->m_nHeadNo = nCamNo;
//			pFrmInfo->m_nWidth = size.cx;
//			pFrmInfo->m_nHeight = size.cy;
//			pFrmInfo->SetImgPtr(pImgPtr);
//			pFrmInfo->m_nTabLeft = nLeft;
//			pFrmInfo->m_nTabRight = nRight;
//			pFrmInfo->m_nTabLevel = nLevel;
//			pFrmInfo->m_nInspMode = nInspMode;
//			pFrmInfo->nTabNo = nTabNo ;
//
//			pThreadQue[nCamNo]->push(pFrmInfo);
//		}
//
//		iter++;
//
//		pThreadPrm->nProcCount++;
//
//		if (pThreadPrm->bWaitOne == FALSE) {
//			pThreadPrm->bWaitOne = TRUE ;
//		}
//
//		Sleep(AprData.m_nSleep);
//	}
//
//	pThreadPrm->bKill = FALSE;
//
//	pThreadPrm->m_pvecTabList->clear() ;
//	AfxEndThread(0);
//
//	return 0;
//}

// 22.09.08 Ahn Add Start

UINT CDebugImageAcqDlg::CtrlThreadDebugAcqProc(LPVOID pParam)
{
	_D_ACQ_THREAD_PARAM* pThreadPrm = (_D_ACQ_THREAD_PARAM*)pParam;
	//CQueueCtrl* pQueueCtrl = theApp.m_pImgProcCtrl->GetQueuePtr();

	CQueueCtrl* pThreadQue[MAX_CAMERA_NO];

	pThreadQue[CAM_POS_TOP] = theApp.m_pImgProcCtrl->GetQueueFrmPtr(CAM_POS_TOP);
	pThreadQue[CAM_POS_BOTTOM] = theApp.m_pImgProcCtrl->GetQueueFrmPtr(CAM_POS_BOTTOM);

	VEC_TAB_LIST::iterator iter;
	iter = pThreadPrm->m_pvecTabList->begin();

	// 22.09.08 Ahn Add Start
	int nBackLength = 0;
	int nBackWidth = 0;
	int nFrameLength = AprData.m_System.m_nCamViewHeight ;
	int nFrameWidth = AprData.m_System.m_nCamViewWidth;
	int nFrameSize = nFrameLength * nFrameWidth;

	BYTE* pImgBackPtr[MAX_CAMERA_NO];
	pImgBackPtr[CAM_POS_TOP] = new BYTE[nFrameLength * nFrameWidth + 1];
	memset(pImgBackPtr[CAM_POS_TOP], 0x00, sizeof(BYTE) * nFrameLength * nFrameWidth + 1);
	pImgBackPtr[CAM_POS_BOTTOM] = new BYTE[nFrameLength * nFrameWidth + 1];
	memset(pImgBackPtr[CAM_POS_BOTTOM], 0x00, sizeof(BYTE) * nFrameLength * nFrameWidth + 1);

	CTabInfo ResvTabInfo;
	ResvTabInfo.nImageLength = 0 ;
	ResvTabInfo.pImgPtr = pImgBackPtr[CAM_POS_TOP];
	ResvTabInfo.pImgBtmPtr = pImgBackPtr[CAM_POS_BOTTOM];
	// 22.09.08 Ahn Add End

	// 22.11.18 Ahn Add Start
	int nFrameCount = 0;
	// 22.11.18 Ahn Add End

	while (1) {
		if (pThreadPrm == NULL) {
			break;
		}

		if (pThreadPrm->bKill == TRUE) {
			break;
		}

		if (pThreadPrm->m_pvecTabList == NULL) {
			//	pThreadPrm->bKill = TRUE;
			//	break;;
			continue;
		}

		if (pThreadPrm->m_pvecTabList->size() <= 0) {
			//	pThreadPrm->bKill = TRUE;
			//	break;
			continue;
		}

		if (iter == pThreadPrm->m_pvecTabList->end()) {
			pThreadPrm->bKill = TRUE;
			break;
		}

		if (pThreadPrm->m_pvecTabList->empty() == TRUE) {
			pThreadPrm->bRunFlag = FALSE;
			continue;
		}

		if (pThreadPrm->bRunFlag == FALSE) {
			continue;
		}

		if ((pThreadPrm->bOneByOne == 1) && (pThreadPrm->bWaitOne == TRUE)) {
			continue;
		}

		CString strNameTop;
		CString strNameBtm;
		if (iter->AcqUnit[CAM_POS_TOP].bExist != FALSE)	{
			CDebugAcqUnit* pUnitTop = &(iter->AcqUnit[CAM_POS_TOP]);
			CDebugAcqUnit* pUnitBtm = &(iter->AcqUnit[CAM_POS_BOTTOM]);
			strNameTop = pUnitTop->strFileName;
			strNameBtm = pUnitBtm->strFileName;

			CString strFullPathTop;
			CString strFullPathBtm;
			strFullPathTop.Format(_T("%s\\%s"), pThreadPrm->strImagePath, strNameTop);
			strFullPathBtm.Format(_T("%s\\%s"), pThreadPrm->strImagePath, strNameBtm);

			if (CWin32File::ExistFile(strFullPathTop) <= 0) {
				CString strMsg;
				strMsg.Format(_T("파일이 존재하지 않습니다. %s"), strFullPathTop);
				AfxMessageBox(strMsg);
				pThreadPrm->bRunFlag = FALSE;
				continue;
			}
			if (CWin32File::ExistFile(strFullPathBtm) <= 0) {
				CString strMsg;
				strMsg.Format(_T("파일이 존재하지 않습니다. %s"), strFullPathBtm);
				AfxMessageBox(strMsg);
				pThreadPrm->bRunFlag = FALSE;
				continue;
			}

			int nTabNo = FileNameFromTabNo(strNameTop);

			CBitmapStd bmpTop;
			CBitmapStd bmpBtm;
			bmpTop.ReadBitmap(strFullPathTop);
			bmpBtm.ReadBitmap(strFullPathBtm);
			CSize size = bmpTop.GetImgSize();
			
			if ( (ResvTabInfo.nImageLength + size.cy) >= nFrameLength) {
				BYTE* pImgPtrTop = new BYTE[nFrameSize + 1];
				memset(pImgPtrTop, 0x00, sizeof(BYTE) * nFrameSize + 1);
				BYTE* pImgPtrBtm = new BYTE[nFrameSize + 1];
				memset(pImgPtrBtm, 0x00, sizeof(BYTE) * nFrameSize + 1);

				int nCopyLength = ResvTabInfo.nImageLength;
				if (nCopyLength > nFrameLength) {
					nCopyLength = nFrameLength ;
				}

				BYTE* pOrgPtrTop = bmpTop.GetImgPtr();
				BYTE* pOrgPtrBtm = bmpBtm.GetImgPtr();
				if (nCopyLength > 0) {
					memcpy(pImgPtrTop, ResvTabInfo.pImgPtr, sizeof(BYTE) * size.cx * nCopyLength);
					memcpy(pImgPtrBtm, ResvTabInfo.pImgBtmPtr, sizeof(BYTE) * size.cx * nCopyLength);

					int nCopyLen = nFrameLength - nCopyLength;
					memcpy(pImgPtrTop + (size.cx * nCopyLength), pOrgPtrTop, sizeof(BYTE) * size.cx * nCopyLen);
					memcpy(pImgPtrBtm + (size.cx * nCopyLength), pOrgPtrBtm, sizeof(BYTE) * size.cx * nCopyLen);

					int nRemainLength = size.cy - nCopyLen ;
					memcpy(ResvTabInfo.pImgPtr	 , pOrgPtrTop + (size.cx * nCopyLen) , sizeof(BYTE) * size.cx * nRemainLength);
					memcpy(ResvTabInfo.pImgBtmPtr, pOrgPtrBtm + (size.cx * nCopyLen) , sizeof(BYTE) * size.cx * nRemainLength);
					ResvTabInfo.nImageLength = nRemainLength ;
				}

				int nHeight = size.cy;
				int nWidth = size.cx;

				CFrameInfo* pFrmInfoTop = new CFrameInfo;
				CFrameInfo* pFrmInfoBtm = new CFrameInfo;

				// 22.11.18 Ahn Add Start
				pFrmInfoTop->m_nFrameCount = nFrameCount ;
				pFrmInfoBtm->m_nFrameCount = nFrameCount++ ;
				// 22.11.18 Ahn Add End

				pFrmInfoTop->m_nHeadNo = CAM_POS_TOP;
				pFrmInfoTop->m_nHeight = nFrameLength;
				pFrmInfoTop->m_nWidth = nWidth;
				pFrmInfoTop->SetImgPtr(pImgPtrTop);

				pFrmInfoBtm->m_nHeadNo = CAM_POS_BOTTOM;
				pFrmInfoBtm->m_nHeight = nFrameLength;
				pFrmInfoBtm->m_nWidth = nWidth;
				pFrmInfoBtm->SetImgPtr(pImgPtrBtm);

				pThreadQue[CAM_POS_TOP]->PushBack(pFrmInfoTop);
				pThreadQue[CAM_POS_BOTTOM]->PushBack(pFrmInfoBtm);
			}
			else {
				int nResvLength = ResvTabInfo.nImageLength;
				BYTE* pRevTop = (ResvTabInfo.pImgPtr + (size.cx * nResvLength));
				BYTE* pRevBtm = (ResvTabInfo.pImgBtmPtr + (size.cx * nResvLength));
				int nCopyLength = size.cy; 
				memcpy(pRevTop, bmpTop.GetImgPtr(), sizeof(BYTE)* size.cx * nCopyLength);
				memcpy(pRevBtm, bmpBtm.GetImgPtr(), sizeof(BYTE)* size.cx * nCopyLength );
				ResvTabInfo.nImageLength += nCopyLength;
			}
		}

		iter++;

		pThreadPrm->nProcCount++;

		if (pThreadPrm->bWaitOne == FALSE) {
			pThreadPrm->bWaitOne = TRUE;
		}

		Sleep(AprData.m_nSleep);
	}

	delete[] pImgBackPtr[CAM_POS_TOP];
	delete[] pImgBackPtr[CAM_POS_BOTTOM];

	pThreadPrm->bKill = FALSE;

	pThreadPrm->m_pvecTabList->clear();
	AfxEndThread(0);

	return 0;
}
// 22.09.08 Ahn Add End
void CDebugImageAcqDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == T_ID_THREAD_CHECK) {
		KillTimer(T_ID_THREAD_CHECK);

		if (m_ThreadPrm.bRunFlag == TRUE) {
			int nPos = m_ProgressCtrl.GetPos();
			if (nPos < m_ThreadPrm.nProcCount) {
				m_ProgressCtrl.SetPos(m_ThreadPrm.nProcCount);
			}
		}
		else if(( m_ThreadPrm.bKill == FALSE) && (m_pThread != NULL) ){
			Kill();
		}

		SetTimer(T_ID_THREAD_CHECK, 100, NULL);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDebugImageAcqDlg::Destroy()
{
	if (m_ThreadPrm.bRunFlag == TRUE) {
		Kill();
	}
}


void CDebugImageAcqDlg::OnBnClickedBtnOneByOne()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ThreadPrm.bWaitOne = FALSE;


}
