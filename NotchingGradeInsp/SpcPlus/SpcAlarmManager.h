// SpcAlarmManager.h : SpcAlarmManager 관리 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCALARMMANAGER_H
#define SPCALARMMANAGER_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CSpcHeader;
class CSpcRefDs;
class CSpcAlarmInData;
class CSpcAlarmManager : public CSpcPlusManager
{
public:
	CSpcAlarmManager() ;
	virtual ~CSpcAlarmManager() ;

	//SPC Plus Header 객체 포인터	
	CSpcHeader* m_SpcHeader;
	//SPC Reference Data Set 객체 포인터	
	CSpcRefDs* m_SpcRefDs;
	///In Data(송신 데이터) 객체 포인터	
	CSpcAlarmInData* m_SpcAlarmInData;
};


#endif // SPCALARMMANAGER_H
