// SpcRefDs.h : CSpcRefDs 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCREFDS_H
#define SPCREFDS_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcInData;
class CSpcRefDs : public CSpcInData
{
public:
	CSpcRefDs(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcRefDs() ;

	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;
public:
	static CString MakeRefDsText_1;
	static CString MakeRefDsText_2;

	//JSON 형식의 텍스트를 만든다.
	CString getJSONText_RefDsFront();
	CString getJSONText_RefDsTail();

};


#endif // SPCREFDS_H
