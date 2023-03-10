#pragma once


// CDebugImageAcqDlg 대화 상자
class CDebugAcqUnit {
public :
	BOOL bExist;
	int nTabNo;
	int nPosition; // 0 : TOP, 1 : BOTTOM 
	CString strFileName;
};
class CDebugAcqTab {
public :
	CDebugAcqUnit AcqUnit[MAX_CAMERA_NO];
};

class CNotchingGradeInspView;
class CRecipeCtrl;
class CDebugImageAcqDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDebugImageAcqDlg)

public:
	CDebugImageAcqDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDebugImageAcqDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEBUG_IMG_ACQ_DLG };
#endif

	CNotchingGradeInspView* m_pView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSelectFolder();
	afx_msg void OnBnClickedBtnImageAcqStart();
	int m_nImgAcqCount;
	afx_msg void OnBnClickedBtnInitialize();

private :
	int m_nTotalCount;
	int m_nNowCount;
//	CStringList m_strListHead[MAX_CAMERA_NO];
//	POSITION m_posList[MAX_CAMERA_NO];

	CRecipeCtrl *m_pRecipeCtrl;
	CRecipeInfo* m_pRecipeInfo;

	typedef std::vector< CDebugAcqTab > VEC_TAB_LIST;
	VEC_TAB_LIST m_vecTabList;
	typedef std::vector< CDebugAcqUnit > VEC_UNIT_LIST;

	int SortAcqUnit(VEC_UNIT_LIST &vecUnitList);
	int MergeAcqUnitList(VEC_TAB_LIST& vecTabList, VEC_UNIT_LIST &vecTopList, VEC_UNIT_LIST &vecBtmList);
	int ClearTabList();
	int GetFrameImage(BYTE* pImgPtr, int nLength, BYTE* pBackPtr, int nBackLength);

public:
	CString m_strFilePath;
	CComboBox m_cmbRecipeSel;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComRecipeSelect();
	CString m_strLotID;

	void Destroy();

// Thread 관련 추가.
protected:

	static UINT CtrlThreadDebugAcqProc(LPVOID pParam);
	static int FileNameFromTabNo(CString strFileName);
	BOOL	m_bKill;
	int		m_nThreadId;
	CWinThread* m_pThread;				//!< 긚깒긞긤(CWinThread *)x1

	void Begin();
	void Kill(void);
public:
	afx_msg void OnBnClickedBtnImageAcqStop();
	BOOL m_bChkProcessAll;
	CProgressCtrl m_ProgressCtrl;
	void SetProgressPos(int nPos);

	//typedef struct {
	//	int nTabNo;
	//	BOOL m_bAvailableTop;
	//	BOOL m_bAvailableBtm;
	//	CString strFileNameTop;
	//	CString strFileNameBtm;
	//} _TAB_FOR_DEBUG ;

	//typedef std::vector< _TAB_FOR_DEBUG > VEC_TAB_DEBUG;

	typedef struct {
		CString strLotID;
		CString strImagePath;
		BOOL	bKill;
		BOOL	bRunFlag;
		int		nTargetCount;
		int		nProcCount;
		BOOL	bOneByOne;
		BOOL	bWaitOne;
		CRecipeInfo* pRecipeInfo;
	//	CStringList strList[MAX_CAMERA_NO];
		VEC_TAB_LIST* m_pvecTabList;
	//	POSITION pos[MAX_CAMERA_NO];
		CDebugImageAcqDlg* pThis;
	} _D_ACQ_THREAD_PARAM;

	_D_ACQ_THREAD_PARAM m_ThreadPrm;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnOneByOne();
};
