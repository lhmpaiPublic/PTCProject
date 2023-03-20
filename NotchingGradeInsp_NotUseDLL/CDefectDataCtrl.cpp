#include "pch.h"
#include "GlobalData.h"
#include "CDefectDataCtrl.h"
#include "CTabRsltInfo.h"
#include "Win32File.h"
#include "CDefectInfo.h"
#include "CAprHash.h"				// 22.05.25 Son Add
// 22.11.30 Ahn Add Start
#include "NotchingGradeInsp.h"
#include "ImageProcessCtrl.h"
#include "QueueCtrl.h"
#include "CCropImgQueueCtrl.h"
// 22.11.30 Ahn Add End
CDefectDataCtrl::CDefectDataCtrl()
{
	m_nLastSaveIndex = 0;
	m_nLastSaveTabIndex = 0;

	::InitializeCriticalSection(&m_csDefData);
	::InitializeCriticalSection(&m_csTabQueue);

	// 22.02.11 Ahn Modify Start
	//m_VecTabRsltList.clear();
	//m_JudgeRsltCtrl.ResetAll();
	RemoveAll();
	// 22.02.11 Ahn Modify End


}
CDefectDataCtrl::~CDefectDataCtrl()
{
	RemoveAll();
	ResetTabQueue();

	::DeleteCriticalSection(&m_csDefData);
	::DeleteCriticalSection(&m_csTabQueue);
}

void CDefectDataCtrl::RemoveAll()
{
	::EnterCriticalSection(&m_csDefData);
	m_NgRsltCtrl.ResetAll();

	// 22.02.11 Ahn Add Start
	m_nOkImageCount = 0 ;
	m_nNgImageCount = 0 ;
	m_nOkCropImgCount = 0 ;
	m_nNGCropImgCount = 0 ;
	// 22.02.11 Ahn Add End

	m_nLastSaveIndex = 0;
	m_nLastSaveTabIndex = 0;
	// 22.06.23 Delete Start
	//while (!m_VecTabRsltList.empty()) {
	//	CTabRsltInfo *pInfo = m_VecTabRsltList.front();
	//	m_VecTabRsltList.erase(m_VecTabRsltList.begin());
	//	delete pInfo;
	//}
	//m_VecTabRsltList.clear();
	// 22.06.23 Delete End

	// 22.11.24 Ahn Add Start
	ClearDefectList();
	// 22.11.24 Ahn Add End

	::LeaveCriticalSection(&m_csDefData);
}

// 22.06.23 Delete Start
//CTabRsltInfo* CDefectDataCtrl::GetTabRsltInfoPtr(int nIndex)
//{
//	if ((int)m_VecTabRsltList.size() <= nIndex) {
//		ASSERT(FALSE);
//		return NULL;
//	}
//	return (m_VecTabRsltList[nIndex]);
//}
//
//int CDefectDataCtrl::GetSize()
//{
//	return (int)m_VecTabRsltList.size();
//}
// 22.06.23 Delete End

int CDefectDataCtrl::DeleteDefect(int nIndex)
{
	//::EnterCriticalSection(&m_csDefData);
	//::LeaveCriticalSection(&m_csDefData);
	return 0;
}
// 22.06.30 Ahn Delete Start
//CString CDefectDataCtrl::GetLotStartDate(CTime ctLotStartTime)
//{
//	CString strLotStTime;
//
//	int nYear = ctLotStartTime.GetYear();
//	int nMonth = ctLotStartTime.GetMonth();
//	int nDay = ctLotStartTime.GetDay();
//	strLotStTime.Format("%04d\\%02d\\%02d", nYear, nMonth, nDay );
//
//
//	return strLotStTime;
//}
//CString CDefectDataCtrl::GetErrorSavePath(CTime ctLotStTime, CString strLotNo)
//{
//	CString strRsltPath;
//	CString strDate;
//	strDate = GetLotStartDate(ctLotStTime);
//	
//	strRsltPath.Format("%s\\%s\\%s", AprData.m_strFeederPath, strDate, strLotNo);
//
//	return strRsltPath;
//}
//
//CString CDefectDataCtrl::GetErrorSavePath(CString strDate, CString strLotNo)
//{
//	CString strRsltPath;
//
//	strRsltPath.Format("%s\\%s\\%s", AprData.m_strFeederPath, strDate, strLotNo);
//
//	return strRsltPath;
//}
// 22.06.30 Ahn Delete Start


int CDefectDataCtrl::SaveErrorData()
{
	// 22.07.14 Ahn Modify Start
	//CStdioFile binFile;
	CFile binFile ;
	// 22.07.14 Ahn Modify End

	// 22.05.25 Ahn Modify Start
	//CString strSavePath = AprData.m_strNowCsvPath;
	CString strSavePath = AprData.m_strNowBinPath;
	// 22.05.25 Ahn Modify End

	CString strFileName;
	int nTabSize = (int)m_queTabRsltInfo.size();
	BOOL bFileOpen = FALSE;

	// LotInfo 저장
	//// Lot ID, Recipe 명, 총 Tab수 NG Tab수 OK Tab수, 
	//{
	//	AprData.SaveLotInfo(CGlobalData::en_Mode_Run);
	//}

	//TabInfo 저장
	if( (AprData.m_strNowBinPath.GetLength() > 0 ) && (AprData.m_NowLotData.m_nTabCount > AprData.m_NowLotData.m_nLastTotalCnt ) )
	{
		AprData.FileCtrl_LotInfo(CGlobalData::en_mode_Countting);
	}	

	for (int nTab = 0; nTab < nTabSize; nTab++) {
		CTabRsltInfo* pTabRsltInfo = PopTabQueue();
		if (pTabRsltInfo == NULL) {
			return 0;
		}

		int nSize = pTabRsltInfo->GetSize();

		// Defect 수 만큼 Loop
		for (int i = 0; i < nSize; i++) {
			// 22.06.09 Ahn Add Start
			CDefectInfo* pDefInfo;
			pDefInfo = pTabRsltInfo->m_vecDefInfo[i];

			// 22.11.21 Ahn Modify Start - JUDGE_GRAY
			//if (pDefInfo->nRank != JUDGE_NG) continue;
			if (pDefInfo->nRank < JUDGE_GRAY) continue;
			// 22.11.21 Ahn Modify End
			// 22.06.09 Ahn Add End

			// 22.11.24 Ahn Add Start
			//AddDefectList(pDefInfo);
			m_DrawDefList.push_back(*pDefInfo);
			// 22.11.24 Ahn Add End

			if (bFileOpen == TRUE) {
				binFile.Close();
				bFileOpen = FALSE;
			}

			strFileName.Format(_T("%s\\%s.bin"), strSavePath, AprData.m_NowLotData.m_strLotNo);

			if (bFileOpen == FALSE) {
				if (binFile.Open(strFileName, CFile::modeWrite | CFile::modeNoTruncate | CFile::typeBinary, NULL) == 0) {
					if (binFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::typeBinary, NULL) == 0) {
						binFile.Close();
					}
					else {
						bFileOpen = TRUE;
					}
				}
				else {
					bFileOpen = TRUE;
				}
			}

			//int nDefSize = (int)pTabRsltInfo->GetDefectCount();
			binFile.SeekToEnd();
			binFile.Write(pDefInfo, sizeof(CDefectInfo));
			m_nLastSaveIndex++;
		}

		// 22.08.10 Ahn Add Start
		if (pTabRsltInfo != NULL) {
			delete pTabRsltInfo;
			pTabRsltInfo = NULL;
		}
		// 22.08.10 Ahn Add End 

		if (bFileOpen == TRUE) {
			binFile.Close();
			bFileOpen = FALSE;
		}
		//m_VecTabRsltList.push_back(pTabRsltInfo);
	}
	return 0;
}

// 22.06.23 Ahn Delete Start
//int CDefectDataCtrl::LoadErrorData(CString strDate, CString strLotNo)
//{
//	m_nLastSaveIndex = 0;
//
//	CFile binFile;
//	CFileStatus fs;
//	CString strSavePath = GetErrorSavePath(strDate, strLotNo);
//	CString strFileName;
//
//	BOOL bFileOpen = FALSE;
//
//	//	int nSize = (int)m_VecTabRsltList.size();
//		//	if (nSize < m_nLastSaveIndex) return 0;
//	RemoveAll();
//
//	int nFileCount = 1;
//	int nFileNo = 0;
//
//	//CTabRsltInfo* pNowTabRsltInfo ;
//	CTabRsltInfo *pTabRsltInfo;
//	//pNowTabRsltInfo = &TabRsltInfo;
//	{
//		strFileName.Format(_T("%s\\TAB_INFO.bin"), strSavePath);
//		if (!binFile.GetStatus(strFileName,fs)) {
//			return -1;
//		}
//
//		if ( binFile.Open(strFileName, CFile::modeRead | CFile::shareDenyNone) != 0) {
//			int bytes;
//			while (1) {
//				pTabRsltInfo = new CTabRsltInfo();
//				CTabRsltBase tempInfo ;
//				bytes = binFile.Read(&tempInfo, sizeof(CTabRsltBase));
//				if (bytes == sizeof(CTabRsltBase)) {
//					// 22.02.11 Ahn Modify Start
//					//pTabRsltInfo->m_nHeadNo = tempInfo.m_nHeadNo;
//					//pTabRsltInfo->m_nTabNo = tempInfo.m_nTabNo;
//					//::_tcsnccpy_s(pTabRsltInfo->m_chImagePath, _countof(pTabRsltInfo->m_chImagePath), tempInfo.m_chImagePath, _TRUNCATE);
//					*(CTabRsltBase*)pTabRsltInfo = tempInfo;
//					// 22.02.11 Ahn Modify End
//
//					m_VecTabRsltList.push_back(pTabRsltInfo);
//					if (pTabRsltInfo->m_nJudge == JUDGE_NG) {
//						m_nNgImageCount++;
//						m_nNGCropImgCount++;
//					}
//					else {
//						if (pTabRsltInfo->m_bImageFlag == TRUE) {
//							m_nOkImageCount++;
//						}
//						if (pTabRsltInfo->m_bCropImgFlag == TRUE) {
//							m_nOkCropImgCount++;	
//						}
//					}
//				}
//				else {
//					if (pTabRsltInfo != NULL) {
//						pTabRsltInfo->RemoveAll();
//						delete pTabRsltInfo;
//						pTabRsltInfo = NULL;
//					}
//					break;
//				}
//			}
//		}
//		binFile.Close();
//	}
//
//	strFileName.Format(_T("%s\\%s.bin"), strSavePath, strLotNo);
//
//	if (binFile.Open(strFileName, CFile::modeRead | CFile::shareDenyNone) != 0) {
//		int bytes;
//		CDefectInfo DefInfo;
//
//		while (1) {
//			bytes = binFile.Read(&DefInfo, sizeof(CDefectInfo));
//
//			if (bytes == sizeof(CDefectInfo)) {
//				CTabRsltInfo* pTempRsltInfo = GetTabRsltInfo(DefInfo.nHeadNo, DefInfo.nTabNo);
//				if (pTempRsltInfo != NULL) {
//					CDefectInfo* pDefData = new CDefectInfo();
//					*pDefData = DefInfo;
//					pTempRsltInfo->AddDefectInfo(pDefData);
//				}
//			}
//			else {
//				break;
//			}
//		}
//	}
//	binFile.Close();
//// 22.05.25 Son Mod End
//
//	return 0;
//}
// 22.06.23 Ahn Delete End

void CDefectDataCtrl::ResetTabQueue()
{
	::EnterCriticalSection(&m_csTabQueue);
	while( !m_queTabRsltInfo.empty() ){
		CTabRsltInfo* pInfo;
		pInfo = m_queTabRsltInfo.front();
		m_queTabRsltInfo.pop();
		delete pInfo;
	}
	::LeaveCriticalSection(&m_csTabQueue);
}

void CDefectDataCtrl::PushBackTabQueue(CTabRsltInfo *pTabRsltInfo)
{
	::EnterCriticalSection(&m_csTabQueue);
	m_queTabRsltInfo.push(pTabRsltInfo);
//	CString strMsg;
//	strMsg.Format("QuAdd [%d]", pTabRsltInfo->GetSize());
//	AprData.SaveDebugLog(strMsg);
	int nTabNo = pTabRsltInfo->m_nTabNo;
	int nJudge = pTabRsltInfo->m_nJudge;
	if (nJudge == JUDGE_NG) {
		m_NgRsltCtrl.SetJudgeNG(nTabNo, nJudge);
	}
	::LeaveCriticalSection(&m_csTabQueue);
}

CTabRsltInfo* CDefectDataCtrl::PopTabQueue()
{
	::EnterCriticalSection(&m_csTabQueue);
	CTabRsltInfo* pTabRsltInfo = NULL ;
	if(!m_queTabRsltInfo.empty()){
		pTabRsltInfo = m_queTabRsltInfo.front();
		m_queTabRsltInfo.pop();
	}	
	::LeaveCriticalSection(&m_csTabQueue);

	return pTabRsltInfo;
}

// 22.06.23 Delete Start
//CTabRsltInfo* CDefectDataCtrl::GetTabRsltInfo(int nHeadNo, int nTabNo)
//{
//	if ( m_VecTabRsltList.empty()) return NULL;
//
//	int nSize = (int)m_VecTabRsltList.size();
//
//	if (nSize < nTabNo) return NULL;
//
//	for (int i = 0; i < nSize; i++){
//		if ((m_VecTabRsltList[i]->m_nTabNo == nTabNo) && (m_VecTabRsltList[i]->m_nHeadNo == nHeadNo)) {
//			return m_VecTabRsltList[i];
//		}
//	}
//
//	return NULL;
//}
// 22.06.23 Delete End


int CDefectDataCtrl::AddJudgeNG(int nTabNo, int nCamNo)
{
	int nRet = m_NgRsltCtrl.SetJudgeNG(nTabNo, nCamNo);
	return nRet;
}

int CDefectDataCtrl::GetNgTabCount()
{
	int nRet = m_NgRsltCtrl.GetNgTabCount() ;
	return nRet;
}

// 22.07.07 Ahn Add Start
int CDefectDataCtrl::LoadDefectInfo(CString strFileName, _VEC_DEFECT_LIST& vecDefList)
{
	int nRet = 0;
	CString strTemp;
	int nIndex = 0;
	CFile binFile;
	int nCount = 0;

	// 22.12.24 Ahn Test Start
	//CString strLogPath = _T("D:\\");
	//CString strLogFile;
	//strLogFile.Format(_T("DefectList.txt"));
	//CString strLogFullPath;
	//CWin32File winFile;
	//strLogFullPath = strLogPath + strLogFile ;
	//CWin32File::DeleteFile(strLogFullPath);
	// 22.12.24 Ahn Test Start

	if (binFile.Open(strFileName, CFile::modeRead | CFile::shareDenyNone) != 0) {
		int bytes;
		CDefectInfo DefInfo;

		CDefectQueueCtrl *pDefQueueCrl = theApp.m_pImgProcCtrl->GetDefectQueuePtr();
		CCropImgQueueCtrl *pCropQueueCtrl = theApp.m_pImgProcCtrl->GetCropImageQueuePtr();

		while (1) {
			bytes = binFile.Read(&DefInfo, sizeof(CDefectInfo));
			if (bytes == sizeof(CDefectInfo)) {
				// 22.11.24 Ahn Modify Start
				//CDefectInfo* pData = new CDefectInfo;
				//*pData = DefInfo;
				//vecDefList.push_back(*pData);
				vecDefList.push_back(DefInfo);
				// 22.11.24 Ahn Modify End
				// 22.11.30 Ahn Add Start
				CDefectInfo* pDefData = new CDefectInfo( DefInfo );
				pDefQueueCrl->PushBack(pDefData);
				CCropImgData CropData ;
				CropData.m_bEnable = TRUE ;
				CString strName;
				strName.Format(_T("%d_%d"), DefInfo.nTabNo+1, DefInfo.nDefNo);
				CropData.m_strDispName = strName ;
				CropData.m_strFileName = DefInfo.szImgFileName;
				pCropQueueCtrl->PushBack(CropData);
				// 22.11.30 Ahn Add End
				nCount++;
					
				// 22.12.24 Ahn Test Start
				//CString strMsg ;
				//strMsg.Format(_T("TabNo [%d], DefNO[%d], %s\r\n"), DefInfo.nTabNo + 1, DefInfo.nDefNo, DefInfo.szImgFileName);
				//winFile.TextSave1Line(strLogPath, strLogFile, strMsg, _T("at"), FALSE);
				// 22.12.24 Ahn Test End
			}
			else {
				break;
			}
		}
	}
	binFile.Close();

	nRet = nCount;
	return nRet;

}
// 22.07.07 Ahn Add End


//// 22.11.24 Ahn Add Start
void CDefectDataCtrl::ClearDefectList()
{
	m_DrawDefList.clear();
}
//
//void CDefectDataCtrl::AddDefectList(CDefectInfo DefData)
//{
//	m_DrawDefList.push_back(DefInfo);
//}
//
//int CDefectDataCtrl::GetDrawDefectCount()
//{
//	int nSize = (int)m_DrawDefList.size();
//
//	return nSize ;
//}
//
//// 22.11.24 Ahn Add End

