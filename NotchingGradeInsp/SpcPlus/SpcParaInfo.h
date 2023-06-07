// SpcParaInfo.h : CSpcParaInfo 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCPARAINFO_H
#define SPCPARAINFO_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcParaInfo : public CSpcInData
{
public:
	CSpcParaInfo(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcParaInfo() ;

private:
	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;

	//검사유형
	//DIMENSION/DEFECT/ALIGN	
	CString	m_ParaInspType;
	//PARA 기입 순서 (KEY Value) 
	//1, 2, 3 …. N 	
	CString	m_ParaNo;
	//카메라 위치정보
	//TOP/BTM
	//TOP/BTM, 해당사항이 없는 경우 "NA" 기입	
	CString	m_ParaCamPosition;
	//측정항목명칭 or ROI 명칭  
	//LG에서 Vision검사기 외관불량항목 표준화Rev1.7.ppt 발송내용참고, 코드명기입	
	CString	m_ParaDetectionName;
	//정의된 Para Key : Value N개	
	CString	m_ParaData;

public:
	static char* MakeParaInfoText_1;
	static char* MakeParaInfoText_2;

	//검사유형
	//DIMENSION/DEFECT/ALIGN	
	void setParaInspType(CString	ParaInspType) { m_ParaInspType = ParaInspType; }
	//PARA 기입 순서 (KEY Value) 
	//1, 2, 3 …. N 	
	void setParaNo(CString	ParaNo) { m_ParaNo = ParaNo; }
	//카메라 위치정보
	//TOP/BTM
	//TOP/BTM, 해당사항이 없는 경우 "NA" 기입	
	void setParaCamPosition(CString	ParaCamPosition) { m_ParaCamPosition = ParaCamPosition; }
	//측정항목명칭 or ROI 명칭  
	//LG에서 Vision검사기 외관불량항목 표준화Rev1.7.ppt 발송내용참고, 코드명기입	
	void setParaDetectionName(CString	ParaDetectionName) { m_ParaDetectionName = ParaDetectionName; }
	//정의된 Para Key : Value N개	
	void appendParaData(CString	ParaKey, CString	ParaData);

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_ParaInfo();
};


#endif // SPCPARAINFO_H
