
// NotchingGradeInspDoc.cpp: CNotchingGradeInspDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "NotchingGradeInsp.h"
#endif
#include "CDefectDataCtrl.h"
#include "NotchingGradeInspDoc.h"
#include "ImageProcessCtrl.h"
#include "NotchingGradeInspView.h"
#include "MainFrm.h"
#include "GlobalData.h"
#include "CRecipeCtrl.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNotchingGradeInspDoc

IMPLEMENT_DYNCREATE(CNotchingGradeInspDoc, CDocument)

BEGIN_MESSAGE_MAP(CNotchingGradeInspDoc, CDocument)
	ON_COMMAND(ID_RECIPE_RELOAD, &CNotchingGradeInspDoc::OnRecipeReload)
END_MESSAGE_MAP()


// CNotchingGradeInspDoc 생성/소멸

CNotchingGradeInspDoc::CNotchingGradeInspDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_nViewMode = enInspMode;
	m_nInspState = enInspStop;
//	m_pDefDataCtrl = NULL;
	m_pDefDataCtrl = theApp.m_pImgProcCtrl->GetDefectDataCtrlPtr();
//KANG 22.05.24 Add Start
	m_nHistoryDispMode = enHistoryDisp_Calender;
//KANG 22.05.24 Add End
	// 22.06.09 Ahn Add Start
	m_bReqCounterReset = FALSE;
	// 22.06.09 Ahn Add End
}

CNotchingGradeInspDoc::~CNotchingGradeInspDoc()
{
	//if (m_pDefDataCtrl != NULL) {
	//	delete m_pDefDataCtrl;
	//	m_pDefDataCtrl = NULL;
	//}
}

BOOL CNotchingGradeInspDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	//m_pDefDataCtrl = new CDefectDataCtrl();

	return TRUE;
}




// CNotchingGradeInspDoc serialization

void CNotchingGradeInspDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CNotchingGradeInspDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CNotchingGradeInspDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CNotchingGradeInspDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CNotchingGradeInspDoc 진단

#ifdef _DEBUG
void CNotchingGradeInspDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNotchingGradeInspDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNotchingGradeInspDoc 명령


int CNotchingGradeInspDoc::GetViewMode()
{
	// TODO: 여기에 구현 코드 추가.
	return m_nViewMode;
}


int CNotchingGradeInspDoc::GetInspState()
{
	// TODO: 여기에 구현 코드 추가.
	return m_nInspState;
}

void CNotchingGradeInspDoc::SetViewMode(int nMode)
{
	// TODO: 여기에 구현 코드 추가.
	m_nViewMode = nMode;
}


void CNotchingGradeInspDoc::SetInspState(int nMode)
{
	// TODO: 여기에 구현 코드 추가.
	m_nInspState = nMode;
}


int CNotchingGradeInspDoc::LoadHistory()
{
	CString strDate;
	CString strLotNo;
	strDate = _T("09\\30");
	strLotNo = _T("AAAAAAAAA");
	// 22.06.23 Ahn Delete Start
	//int nRet = m_pDefDataCtrl->LoadErrorData(strDate, strLotNo);
	//return nRet ;
	// 22.06.23 Ahn Delete Start

	return 0;
}



void CNotchingGradeInspDoc::OnRecipeReload()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CRecipeCtrl rcpCtrl;
	AprData.m_pRecipeInfo->m_strRecipeName;

	rcpCtrl.LoadRecipe(AprData.m_pRecipeInfo, AprData.m_pRecipeInfo->m_strRecipeName);
	//AprData.m_pRecipeInfo 
}

// 22.03.25 Ahn Add Start
int CNotchingGradeInspDoc::RecipeChange(int nRecipeNo, CString strRecipeName)
{
	// Check Recipe Name/No
	//AprData.m_NowLotData.m_nRecipeNo;
	//AprData.m_NowLotData.m_strNextRecipeName;

	int nRet = 0;
	CRecipeCtrl rcpCtrl;

	strRecipeName.TrimRight();
	strRecipeName.MakeUpper();
	if (rcpCtrl.IsEnableRecipe(strRecipeName) == TRUE) {
		rcpCtrl.LoadRecipe(AprData.m_pRecipeInfo, strRecipeName);
		AprData.m_NowLotData.m_strRecipeName = strRecipeName;
	}
	else {
		nRet = -1;
		CString strMsg;
		strMsg.Format(_T("Recipe Change Error - RecipeName[ %s ]"), strRecipeName);
		AprData.SaveErrorLog(strMsg);

		// 22.10.13 Ahn Add Start
		AprData.m_ErrStatus.SetError(CErrorStatus::en_RecipeError, strMsg);
		// 22.10.13 Ahn Add End
	}

	return nRet ;
}
// 22.03.25 Ahn Add End

//KANG 22.05.24 Add Start
int CNotchingGradeInspDoc::GetHistoryDispMode()
{
	// TODO: 여기에 구현 코드 추가.
	return m_nHistoryDispMode;
}


void CNotchingGradeInspDoc::SetHistoryDispMode(int nMode)
{
	// TODO: 여기에 구현 코드 추가.
	m_nHistoryDispMode = nMode;
}
//KANG 22.05.24 Add End


// 22.06.09 Ahn Add Start
BOOL CNotchingGradeInspDoc::IsReqCounterReset()
{
	return m_bReqCounterReset;
}

VOID CNotchingGradeInspDoc::SetReqCounterReset(BOOL bFlag)
{
	m_bReqCounterReset = bFlag ;
}
// 22.06.09 Ahn Add End

