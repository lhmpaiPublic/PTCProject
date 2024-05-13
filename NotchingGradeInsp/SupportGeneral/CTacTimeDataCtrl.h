#pragma once

#include<vector>

class CTactTimeData {
//	SYSTEMTIME stStartTime;		// 
//	SYSTEMTIME stEndTime;		// 
public :
	int	nCellNo;				// Cell No
	double dTactTime;			// second
};

class CTacTimeDataCtrl
{
public :
	CTacTimeDataCtrl();
	~CTacTimeDataCtrl();
	typedef std::vector< CTactTimeData > _VEC_TACT_DATA;

private:
	_VEC_TACT_DATA m_vecTactTime;
	int m_nMaxListSize;
	enum {
		en_MinTacListSize = 100, 
		en_MaxTactListSize = 100000,
	};
public :
	void SetMaxListSize(int nMaxSize);
	_VEC_TACT_DATA* GetTactTimeDataPtr() { return &m_vecTactTime; };
	int AddNewTactData(CTactTimeData Data);
	BOOL IsEmpty();
	void ResetQueue();
	int GetSize();

protected :
	CRITICAL_SECTION m_csQueue;

};

