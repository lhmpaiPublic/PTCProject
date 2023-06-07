// SpcSpecInfo.cpp: CSpcSpecInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcSpecInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcSpecInfo::MakeSpecInfoText =
"					{"
"						\"PARA_INSP_TYPE\": \"%s\","
"						\"PARA_CAM_POSITION\": \"%s\","
"						\"PARA_DETECTION_NAME\": \"%s\","
"					},";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcSpecInfo::CSpcSpecInfo(CSpcPlusManager* sMgr)
{
	m_SpecInspType = "DEFECT";
	m_SpecCamPosition = "TOP";
	m_SpecDetectonName = "PINH";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcSpecInfo::~CSpcSpecInfo()
{

}
