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
	////Wxxx.0 연속 불량 알람
	////Wxxx.1 심각 불량 알람
	////Wxxx.2 연속 불량 알람
	////Wxxx.3 Dross Top - 불량 알람
	////Wxxx.4 Dross Bot - 불량 알람
	////Wxxx.5 Foil exposure Top - 불량 알람
	////Wxxx.6 Foil exposure Bot - 불량 알람
	////Wxxx.7 Spatter Top - 불량 알람
	////Wxxx.8 Spatter Bot - 불량 알람
	
	void ResetAlarmAll();
	int SetAlarm();

private :
	std::queue< WORD > QueueAlarmCode ;

protected :

};

