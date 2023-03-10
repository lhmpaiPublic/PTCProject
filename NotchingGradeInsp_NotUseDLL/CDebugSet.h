#pragma once
class CDebugSet
{

public :
	enum {
		en_Debug_ALL = 0,
		en_Debug_Melsec,
		en_Debug_DIO,
		en_Debug_Light,
		en_Debug_Image,
		en_Debug_Layout, 
		en_Debug_Total,
	};

	CDebugSet();

	BOOL GetDebug(int nIndex);
	void SetDebug(int nIndex, BOOL bDebug);
	CString GetDebugName(int nIndex);


	int FileCtrl(int nMode);

private:
	BOOL m_bDebug[en_Debug_Total];
	CString m_strDebugName[en_Debug_Total] ;

};

