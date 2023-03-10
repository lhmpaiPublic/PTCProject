#pragma once
#include <queue>
#include "GlobalDef.h"
#include "GlobalData.h"

class CAlarmProcess
{
public:
	CAlarmProcess();
	~CAlarmProcess();

	//enum { // Alarm Code
	//	en_Alarm_Continue		= 0x0001,
	//	en_Alarm_Heavy			= 0x0002,
	//	en_Alarm_Continue_2		= 0x0004,
	//	en_Aalrm_Dross_Top		= 0x0008,
	//	en_Alarm_Dross_Btm		= 0x0010,
	//	en_Alarm_FoilExp_Top	= 0x0020,
	//	en_Alarm_FoilExp_Btm	= 0x0040,
	//	en_Alarm_Spatter_Top	= 0x0080,
	//	en_Alarm_Spatter_Btm	= 0x0100,
	//};
	////Wxxx.0 ���� �ҷ� �˶�
	////Wxxx.1 �ɰ� �ҷ� �˶�
	////Wxxx.2 ���� �ҷ� �˶�
	////Wxxx.3 Dross Top - �ҷ� �˶�
	////Wxxx.4 Dross Bot - �ҷ� �˶�
	////Wxxx.5 Foil exposure Top - �ҷ� �˶�
	////Wxxx.6 Foil exposure Bot - �ҷ� �˶�
	////Wxxx.7 Spatter Top - �ҷ� �˶�
	////Wxxx.8 Spatter Bot - �ҷ� �˶�
	
	void ResetAlarmAll();
	int SetAlarm();

private :
	std::queue< WORD > QueueAlarmCode ;

protected :

};

