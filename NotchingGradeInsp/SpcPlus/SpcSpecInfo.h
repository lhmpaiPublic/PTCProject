// SpcSpecInfo.h : CSpcSpecInfo 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCSPECINFO_H
#define SPCSPECINFO_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcSpecInfo : public CSpcInData
{
public:
	CSpcSpecInfo(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcSpecInfo() ;

private:
	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;

	//장비 Spec	
	CString	m_SpecInspType;
	//카메라 위치정보
	//TOP/BTM	
	CString	m_SpecCamPosition;
	//측정항목명칭 or ROI 명칭  	
	CString	m_SpecDetectonName;
	//정의된 Spec Key : Value N개	
	CString	m_SpecData;

public:
	static char* MakeSpecInfoText_1;
	static char* MakeSpecInfoText_2;

	//장비 Spec	
	void setSpecInspType(CString	SpecInspType) { m_SpecInspType = SpecInspType; }
	//카메라 위치정보
	//TOP/BTM	
	void setSpecCamPosition(CString	SpecCamPosition) { m_SpecCamPosition = SpecCamPosition; }
	//측정항목명칭 or ROI 명칭  	
	void setSpecDetectonName(CString	SpecDetectonName) { m_SpecDetectonName = SpecDetectonName; }
	//정의된 Spec Key : Value N개	
	void appendSpec(CString	SpecKey, CString	SpecData);

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_SpecInfo();

};


#endif // SPCSPECINFO_H
