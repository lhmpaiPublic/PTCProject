// SpcHeader.cpp: CSpcHeader 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcHeader.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcHeader::MakeHeaderText_1 =
"{"
"	\"TXN_ID\": \"%s\","
"	\"inDTName\" : \"%s\","
"	\"outDTName\" : \"%s\","
"	\"actID\" : \"%s\",";
//+%s RefDs
CString CSpcHeader::MakeHeaderText_2 =
"}";
//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcHeader::CSpcHeader(CSpcPlusManager* sMgr)
{
	m_TxnId = "";
	m_InDtName = "IN_DATA";
	m_OutDtName = "";
	m_ActId = "INSPECTION";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcHeader::~CSpcHeader()
{

}
