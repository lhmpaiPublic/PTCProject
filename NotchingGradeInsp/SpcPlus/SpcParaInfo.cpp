// SpcParaInfo.cpp: CSpcParaInfo 클래스  정의파일
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "SpcPlus.h"
#include "SpcParaInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CSpcParaInfo::MakeParaInfoText =
"					{"
"						\"PARA_INSP_TYPE\": \"%s\","
"						\"PARA_NO\" : \"%s\","
"						\"PARA_CAM_POSITION\": \"%s\","
"						\"PARA_DETECTION_NAME\": \"%s\","
"					},";

//생성자 : 클래스의 초기화 설계
//멤버 객체 생성 및 초기화, 초기화함수 호출등
CSpcParaInfo::CSpcParaInfo(CSpcPlusManager* sMgr)
{
	m_ParaInspType = "DEFECT";
	m_ParaNo = "1";
	m_ParaCamPosition = "TOP";
	m_ParaDetectionName = "PINH";

	if (sMgr)
	{

	}
}

//소멸자
//동적생성된 객체 삭제
//객체가 메모리에서 제거되기전 필요한 정리 수행
CSpcParaInfo::~CSpcParaInfo()
{

}
