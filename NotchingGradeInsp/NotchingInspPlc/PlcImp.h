#pragma once

class CSequenceData;

class CPlcImp
{

public:
	CPlcImp();
	virtual ~CPlcImp();

	//Out
	virtual int SigOutEncoderZeroSet(int nMode) { return 0; }
	virtual int SigOutRecipeChangeAck(int nMode) { return 0; }
	virtual int SigOutLotEndAck(int nMode) { return 0; }
	virtual int SigOutLotStartAck(int nMode) { return 0; }
	virtual int WriteAlarmCode(WORD nAlarmCode) { return 0; }
	virtual int SigOutReady(int nMode) { return 0; }
	virtual int SigOutAlivePulse(int nInMode) { return 0; }
	virtual int SigOutTabZeroReset(int nMode) { return 0; }
	virtual int SigOutAlarmResetAck(int nMode) { return 0; }
	virtual int WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode) { return 0; }
	virtual void SetInkMarkAcktive(BOOL bUse) {}
	virtual void SetConnectZone(BOOL bUse) {}
	virtual int SigOutAlivePulseReady(int nInMode, BOOL bIsReady) { return 0; }
	virtual int SigOutAlarmExist(int nMode) { return 0; }
	virtual void EnableWorkSet(BOOL bMode) {}
	virtual int SigOutDiskCapacityAlarm(int nMode) { return 0; }
	virtual int SigOutDiskCapacityWarning(int nMode) { return 0; }
	virtual int SignalBitOut(int nIntegration, int nMode, BOOL bLocal = FALSE) { return 0; }
	

	//In
	virtual int SigInReady() { return 0; }
	virtual int SigInRun() { return 0; }
	virtual int SigInLotEnd() { return 0; }
	virtual int SigInTabZeroReset() { return 0; }
	virtual int SigInLotStart() { return 0; }
	virtual int SigInAlarmReset() { return 0; }
	virtual int SigInAlarmNgAck() { return 0; }
	virtual int SigInRecipeChange() { return 0; }
	virtual int SigInInkMarkActive() { return 0; }
	virtual int SigInConnectZone() { return 0; }
	virtual BOOL GetConnectZone() { return TRUE; }
	virtual BOOL GetInkMarkActive() { return TRUE; }

	//임시
	virtual int ReadBlockAllData(CSequenceData* pSeqData) { return 0; }
	virtual int WritePLC_Block_device(int address, short* pData, int nNumOfData) { return 0; }
	virtual int WritePLC_Block_device(int address, int* pData, int nNumOfData) { return 0; }
	virtual int ReadPLC_Block_device(int address, short* pData, int nNumOfData) { return 0; }
	virtual int WriteBlockAllData(int nMode) { return 0; }
	virtual int ReadAllPort_BitIn(BOOL* pSigBitIn) { return 0; }
	virtual int ReadAllPort_BitOut(BOOL* pSigBitOut) { return 0; }

};

