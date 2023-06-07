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
class CSpcRefDs
{
public:
	CSpcRefDs(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcRefDs() ;

public:
	static CString MakeRefDsText_1;
	static CString MakeRefDsText_2;

};


#endif // SPCREFDS_H
