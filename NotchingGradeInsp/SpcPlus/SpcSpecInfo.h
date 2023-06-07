// SpcSpecInfo.h : CSpcSpecInfo 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCSPECINFO_H
#define SPCSPECINFO_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcSpecInfo
{
public:
	CSpcSpecInfo(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcSpecInfo() ;

private:
	//장비 Spec	
	CString	m_SpecInspType;
	//카메라 위치정보
	//TOP/BTM	
	CString	m_SpecCamPosition;
	//측정항목명칭 or ROI 명칭  	
	CString	m_SpecDetectonName;
	//정의된 Spec Key : Value N개	
	std::vector<CString>	m_SpecData;

public:
	static CString MakeSpecInfoText;

	//장비 Spec	
	void setSpecInspType(CString	SpecInspType) { m_SpecInspType = SpecInspType; }
	//카메라 위치정보
	//TOP/BTM	
	void setSpecCamPosition(CString	SpecCamPosition) { m_SpecCamPosition = SpecCamPosition; }
	//측정항목명칭 or ROI 명칭  	
	void setSpecDetectonName(CString	SpecDetectonName) { m_SpecDetectonName = SpecDetectonName; }
	//정의된 Spec Key : Value N개	
	void setSpecData(CString	SpecData) { m_SpecData.push_back(SpecData); }

};


#endif // SPCSPECINFO_H
