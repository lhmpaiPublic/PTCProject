#pragma once

#include <vector>
#include "excel.h"
#include "ExcelConst.h"
#include "KExcel.h"


//KANG 22.05.24 Add Start
class CDefectSearchList
{
public:
	int nNo;
	SYSTEMTIME tLotStartTime;
	CString strLotID;
	CString strModel;
	CString strLane;
	int nCell;
	int nTopBottom;
	int nType;
	int nRank;
	double dXSize;
	double dYSize;
	BOOL bMarking;
	double dDefRadius;
	double dDefTheta;
	CString strImageFileName;
};
//KANG 22.05.24 Add End

class CExcelResultFileCtrl
{
public :
	CExcelResultFileCtrl();
	~CExcelResultFileCtrl();

private :
	CString m_strFileName;
//	KExcel m_Excel;

public :
	int m_nSaveCount ; 

	enum {
		en_colLotID = 0,
		en_colTotalCount,
		en_colCellNo,
		en_colInspTime,
		en_colPosition,
		en_colJudge,
		en_colSurface,
		en_colExposure,
		en_colDefR,
		en_colDefTheta,
		en_colDefSizeX,
		en_colDefSizeY,
		en_colDefImage,
		en_colInkMarking,
		en_colReason,
		en_MaxCols,
	};

	//typedef std::vector< CResultData > VEC_RESULT_DATA ; 
	typedef std::vector<CDefectSearchList*> VEC_DEF_SEARCH_LIST;
	VEC_DEF_SEARCH_LIST* pVecDefSearchList;

	typedef struct threadPrm{
		CString m_strPath ;
		int		m_nTotalCount;
		int		m_nSaveCount;
		BOOL	m_bKill;
	} _EX_THREAD_PRM;

	static BOOL IsImageFileAvailable(CString strImgFile); // 22.06.29 Ahn Modify
	static int InitExcelFile(KExcel* pExcel);
	static int SaveTitleLine(KExcel* pExcel);
	static int SaveExcelOneLine(KExcel* pExcel, int nRow, int nTotalCellCnt, CDefectSearchList defInfo);
	CString GetFileName() { return m_strFileName; };
	void SetFileName(CString strFileName) {
		m_strFileName = strFileName;
	};
	// 22.06.14 Ahn Add Start
	static int MakeDefectListSheet(KExcel* pExcel, VEC_DEF_SEARCH_LIST* pList, CExcelResultFileCtrl* pThis);
	static int MakeChartSheet(KExcel* pExcel, VEC_DEF_SEARCH_LIST* pList, CExcelResultFileCtrl* pThis);
	// 22.06.14 Ahn Add End
	//int SaveExeltFile(KExcel* pExcel, CString strSaveFile, VEC_DEF_SEARCH_LIST& VecDefSearchList);

	// 
	//VEC_DEF_SEARCH_LIST *m_pVecResult;

	int MakeDummyData();
// 22.05.27 Ahn Add Start
protected:
	BOOL	m_bComplet;
	BOOL	m_bKill;
	int		m_nThreadId;
	CWinThread* m_pThread;				//!< ƒXƒŒƒbƒh(CWinThread *)x1

public :
	void Begin();
	void Kill();
	BOOL IsComplet() { return m_bComplet; };
	void ResetComplet() { m_bComplet = FALSE ; };
protected:
	static UINT CtrlExcelFileSaveThread(LPVOID pParam);
	// 22.05.27 Ahn Add End
};

