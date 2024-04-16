#pragma once

class CSequenceData;
struct stSeqDataOut;
class CPlcImp
{
public:
	CPlcImp();
	virtual ~CPlcImp();

	virtual int PlcDataReadWritePorc() = 0;

	//Out
	virtual int SigOutEncoderZeroSet(int nMode) = 0;
	virtual int SigOutRecipeChangeAck(int nMode) = 0;
	virtual int SigOutLotEndAck(int nMode) = 0;
	virtual int SigOutLotStartAck(int nMode) = 0;
	virtual int WriteAlarmCode(WORD nAlarmCode) = 0;
	virtual int SigOutReady(int nMode) = 0;
	virtual int SigOutAlivePulse(int nInMode) = 0;
	virtual int SigOutTabZeroReset(int nMode) = 0;
	virtual int SigOutAlarmResetAck(int nMode) = 0;
	virtual int WriteAlarmCodeAndJudge(WORD nAlarmCode, int nID, int nJudge, int nNgCode) = 0;
	virtual void SetInkMarkAcktive(BOOL bUse) = 0;
	virtual BOOL GetInkMarkActive() = 0;
	virtual void SetConnectZone(BOOL bUse) = 0;
	virtual BOOL GetConnectZone() = 0;
	virtual int SigOutAlivePulseReady(int nInMode, BOOL bIsReady) = 0;
	virtual int SigOutAlarmExist(int nMode) = 0;
	virtual void EnableWorkSet(BOOL bMode) = 0;
	virtual int SigOutDiskCapacityAlarm(int nMode) = 0;
	virtual int SigOutDiskCapacityWarning(int nMode) = 0;
	virtual int SignalBitOut(int nIntegration, int nMode, BOOL bLocal = FALSE) = 0;
	

	//In
	virtual int SigInReady() = 0;
	virtual int SigInRun() = 0;
	virtual int SigInLotEnd() = 0;
	virtual int SigInTabZeroReset() = 0;
	virtual int SigInLotStart() = 0;
	virtual int SigInAlarmReset() = 0;
	virtual int SigInAlarmNgAck() = 0;
	virtual int SigInRecipeChange() = 0;
	virtual int SigInInkMarkActive() = 0;
	virtual int SigInConnectZone() = 0;

#ifdef NEW_PLCTYPE
	//통합비전 Cell Key Id를 가져온다.
	virtual int GetCellKey(int num) = 0;

	//Lot End 처리 함수
	virtual void SigOutLotEnd(int TopDefectCnt, int BtmDefectCnt) = 0;

	virtual int WriteBlockData(void* pGlobalData) = 0;

	//In Word 영역의 인덱스의 값을 String 으로 가져온다.
	virtual CString GetInWordData(int idx) = 0;
	//In Word 영역의 인덱스의 값을 String 으로 가져온다.
	virtual CString GetOutWordData(int idx) = 0;
#else
	virtual int WritePLC_Block_device(int address, short* pData, int nNumOfData) = 0;
	virtual int WritePLC_Block_device(int address, int* pData, int nNumOfData) = 0;
	virtual int ReadPLC_Block_device(int address, short* pData, int nNumOfData) = 0;
#endif //NEW_PLCTYPE	

	virtual int ReadBlockAllData(CSequenceData* pSeqData) = 0;
	
	virtual int ReadAllPort_BitIn(BOOL* pSigBitIn) = 0;
	virtual int ReadAllPort_BitOut(BOOL* pSigBitOut) = 0;

};

