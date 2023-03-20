#include "pch.h"
#include "CDebugSet.h"
#include "GlobalData.h"

CDebugSet::CDebugSet()
{
	for (int i = 0; i < en_Debug_Total; i++) {
		switch (i) {
		case	en_Debug_ALL:
			m_strDebugName[i] = _T("ALL");
			break;
		case	en_Debug_Melsec:
			m_strDebugName[i] = _T("MELSEC");
			break;
		case	en_Debug_DIO:
			m_strDebugName[i] = _T("DIO");
			break;
		case	en_Debug_Light:
			m_strDebugName[i] = _T("LIGHT");
			break;
		case	en_Debug_Image:
			m_strDebugName[i] = _T("IMAGE");
			break;
		case	en_Debug_Layout :
			m_strDebugName[i] = _T("LAYOUT");
			break;
		
		}
	}
}

BOOL CDebugSet::GetDebug(int nIndex)
{
	BOOL bDebug = m_bDebug[nIndex];
	if (m_bDebug[en_Debug_ALL] == TRUE) {
		bDebug = TRUE;
	}
	return bDebug ;
}
void CDebugSet::SetDebug(int nIndex, BOOL bDebug)
{
	m_bDebug[nIndex] = bDebug;
}

CString CDebugSet::GetDebugName(int nIndex)
{
	if ((nIndex < 0) || (nIndex > en_Debug_Total)) {
		return _T("");
	}
	return m_strDebugName[nIndex];
}


int CDebugSet::FileCtrl(int nMode)
{

	//CWin32File winFile;
	CString strFileName;
	strFileName.Format(_T("%s\\DebugSetting.ini"), AprData.m_strDataPath);
	CString strSection;
	CString strKey;
	CString strData;
	int i;

	if (nMode == MODE_READ) {
		char buff[256];

		strSection = _T("DEBUG_SET");
		for ( i = 0; i < en_Debug_Total; i++) {
			strKey = m_strDebugName[i];
			::GetPrivateProfileString(strSection, strKey, "0", buff, 256, strFileName);
			m_bDebug[i] = atoi(buff);
		}
	}
	else {
		strSection = _T("DEBUG_SET");
		for ( i = 0; i < en_Debug_Total; i++) {
			strKey = m_strDebugName[i]; 
			strData.Format("%d", m_bDebug[i]);
			::WritePrivateProfileString(strSection, strKey, strData, strFileName);
		}	

	}
	return 0;
}
