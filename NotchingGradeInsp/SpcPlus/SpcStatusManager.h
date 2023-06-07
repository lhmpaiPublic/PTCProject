// SpcStatusManager.h : CSpcStatusManager 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCSTATUSMANAGER_H
#define SPCSTATUSMANAGER_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcHeader;
class CSpcRefDs;
class CSpcStatusInData;
class CSpcStatusManager : public CSpcPlusManager
{
public:
	CSpcStatusManager() ;
	virtual ~CSpcStatusManager() ;

	//SPC Plus Header 객체 포인터	
	CSpcHeader* m_SpcHeader;
	//SPC Reference Data Set 객체 포인터	
	CSpcRefDs* m_SpcRefDs;
	//In Data(송신 데이터) 객체 포인터	
	CSpcStatusInData* m_SpcStatusInData;
};


#endif // SPCSTATUSMANAGER_H
