
// NotchingGradeInspDoc.h: CNotchingGradeInspDoc 클래스의 인터페이스
//


#pragma once
class CDefectDataCtrl;

class CNotchingGradeInspDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CNotchingGradeInspDoc() noexcept;
	DECLARE_DYNCREATE(CNotchingGradeInspDoc)

	// 특성입니다.
protected:
	int m_nViewMode;
	int m_nInspState;
//KANG 22.05.24 Add Start
	int m_nHistoryDispMode;
//KANG 22.05.24 Add End

	CDefectDataCtrl* m_pDefDataCtrl;
//	std::vector< CJuegeResultInfo > m_VecJudgeInfo;

public:
	int GetViewMode();
	int GetInspState();
	void SetViewMode(int nMode);
	void SetInspState(int nMode);
	int RecipeChange(int nRecipeNo, CString strRecipeName);

	int LoadHistory();
	CDefectDataCtrl* GetErrorDataCtrl() { return m_pDefDataCtrl; };

//KANG 22.05.24 Add Start
	int GetHistoryDispMode();
	void SetHistoryDispMode(int nMode);
//KANG 22.05.24 Add End

	// 22.06.09 Ahn Add Start
	BOOL IsReqCounterReset();
	VOID SetReqCounterReset(BOOL bFlag);
	// 22.06.09 Ahn Add End

private :
	BOOL m_bReqCounterReset;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CNotchingGradeInspDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	afx_msg void OnRecipeReload();
};
