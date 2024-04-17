#include "Pch.h"
#include "LoadVTECSDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	DLL_PATH_END_CHAR		_T('\\')

namespace LOADVTECSDll_GROUP
{
	LoadVTECSDll::LoadVTECSDll(void)
	{
		//dll 모듈 초기화
		m_hdll = NULL;

		m_strDllFilePath.Empty();
#ifdef _WIN64
		m_strDllFile = _T("SPCTransUtilx64.dll");
#else
		m_strDllFile = _T("SPCTransUtilx86.dll");
#endif // WIN32

		//함수 포인터 초기화
		LinkFuntionToAllNULL();	
	}

	LoadVTECSDll::~LoadVTECSDll(void)
	{
		DllSocket_Unitialize();
	}

	bool LoadVTECSDll::LoadingDll(const CString strToLoadDllPathName)
	{
		try
		{
			if (NULL == m_hdll)
			{
				const int nLength = strToLoadDllPathName.GetLength();
				if (nLength <= 0)
				{
					m_strDllFilePath = m_strDllFile;
				}
				else
				{
					TCHAR* p = (TCHAR*)strToLoadDllPathName.GetString();
					if (DLL_PATH_END_CHAR == p[nLength - 1])
					{
						m_strDllFilePath.Format(_T("%s%s"), strToLoadDllPathName, m_strDllFile);
					}
					else
					{
						m_strDllFilePath.Format(_T("%s\\%s"), strToLoadDllPathName, m_strDllFile);
					}
				}
				m_hdll = GetModuleHandle(m_strDllFilePath);
				if (NULL == m_hdll)
				{
					m_hdll = LoadLibrary(m_strDllFilePath);
				}
			}
		}
		catch (CException* exc)
		{
			CString str;
			exc->GetErrorMessage(str.GetBuffer(MAX_PATH), MAX_PATH);
			exc->Delete();
			str.ReleaseBuffer();

			TRACE(_T("Error in LoadingDll[%s]\n"), str);
			m_hdll = NULL;
			exc->Delete();
			m_CriticalLoadUnload.Unlock();
			return false;
		}
		return NULL != m_hdll;
	}

	bool LoadVTECSDll::DllSocket_Unitialize(void)
	{
		m_CriticalLoadUnload.Lock();

		bool bExitOK = true;
		try
		{
			if (NULL == m_hdll && !m_strDllFilePath.IsEmpty())
			{
				m_hdll = GetModuleHandle(m_strDllFilePath);
			}
			if (NULL != m_hdll)
			{
				if (NULL != m_fExitDll)
				{
					bExitOK = m_fExitDll();
				}
				
				if (FreeLibrary(m_hdll))
				{
					m_hdll = NULL;
				}
				
				LinkFuntionToAllNULL();
			}
		}
		catch (CException* exc)
		{
			TRACE(_T("Error in UnloadingDll\n"));
			exc->Delete();
			bExitOK = false;
			m_hdll = NULL;
		}
		bExitOK &= (NULL == m_hdll);
		m_CriticalLoadUnload.Unlock();
		return bExitOK;
	}

	bool LoadVTECSDll::LinkFuntion(void)
	{
		if (NULL != m_hdll)
		{
			if (NULL == m_fInitializeDll)
			{
				m_fInitializeDll = (pfInitializeDll)GetProcAddress(m_hdll, "InitializeDll");
			}

			if (NULL == m_fIsConnected)
			{
				m_fIsConnected = (pfIsConnected)GetProcAddress(m_hdll, "IsConnected");
			}

			if (NULL == m_fIsASCIIType)
			{
				m_fIsASCIIType = (pfIsASCIIType)GetProcAddress(m_hdll, "IsASCIIType");
			}

			if (NULL == m_fExitDll)
			{
				m_fExitDll = (pfExitDll)GetProcAddress(m_hdll, "Exitdll");
			}

			if (NULL == m_fSendData)
			{
				m_fSendData = (pfSendData)GetProcAddress(m_hdll, "SendData");
			}

			if (NULL == m_fAddLine)
			{
				m_fAddLine = (pfAddLineDLL)GetProcAddress(m_hdll, "AddLine");
			}

			if (NULL == m_fGetReceiveMessageInQueueBuffer)
			{
				m_fGetReceiveMessageInQueueBuffer = (fGetReceiveMessageInQueueBuffer)GetProcAddress(m_hdll, "GetReceiveMessageInQueueBuffer");
			}

			if (NULL == m_fToggleViewHideIPConfigDialog)
			{
				m_fToggleViewHideIPConfigDialog = (pfToggleViewHideIPConfigDialog)GetProcAddress(m_hdll, "ToggleViewHideIPConfigDialog");
			}

			if (NULL == m_fRegister_CallBackConnectStatusFuntion)
			{
				m_fRegister_CallBackConnectStatusFuntion = (fRegister_CallBackConnectStatusFuntion)GetProcAddress(m_hdll, "RegCallBackFun_ConnectStatus");
			}

			if (NULL == m_fRegister_CallBackReceiveFunction)
			{
				m_fRegister_CallBackReceiveFunction = (fRegister_CallBackReceiveFunction)GetProcAddress(m_hdll, "RegCallBackFun_ReceiveBuffer");
			}
		}
		else
		{
			LinkFuntionToAllNULL();
		}
		return  	NULL != m_fAddLine && NULL != m_fInitializeDll && NULL != m_fIsConnected && NULL != m_fIsASCIIType &&
			NULL != m_fToggleViewHideIPConfigDialog && NULL != m_fSendData && NULL != m_fGetReceiveMessageInQueueBuffer &&
			NULL != m_fExitDll && NULL != m_fRegister_CallBackConnectStatusFuntion && NULL != m_fRegister_CallBackReceiveFunction;
	}

	bool LoadVTECSDll::DllSocket_Initialize(callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive, const bool bUseReceiveQueueBuffer)
	{
		m_CriticalLoadUnload.Lock();
		CString strPath;
		GetCurrentDirectory(MAX_PATH, strPath.GetBuffer(MAX_PATH));
		strPath.ReleaseBuffer();
		strPath += _T('\\');
		const bool bReturn = SubDllSocket_Initialize(strPath, Callback_Connect, Callback_Receive, bUseReceiveQueueBuffer);
		m_CriticalLoadUnload.Unlock();
		return bReturn;
	}

	bool LoadVTECSDll::DllSocket_Initialize(const CString strDllFold, callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive, const bool bUseReceiveQueueBuffer)
	{
		m_CriticalLoadUnload.Lock();
		const bool bReturn = SubDllSocket_Initialize(strDllFold, Callback_Connect, Callback_Receive, bUseReceiveQueueBuffer);
		m_CriticalLoadUnload.Unlock();
		return bReturn;
	}

	bool LoadVTECSDll::SubDllSocket_Initialize(const CString strDllFold, callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive, const bool bUseReceiveQueueBuffer)
	{
		if (LoadingDll(strDllFold) &&  LinkFuntion())
		{
#ifdef _UNICODE
			CStringA strDll;
			strDll = strDllFold;
#else
			CString strDll = strDllFold;
#endif // _UNICODE

			//초기화
			if (m_fInitializeDll((char*)strDll.GetString(), bUseReceiveQueueBuffer))
			{
				if (NULL != m_fRegister_CallBackConnectStatusFuntion && NULL != Callback_Connect)
				{
					m_fRegister_CallBackConnectStatusFuntion(Callback_Connect);
				}

				if (NULL != m_fRegister_CallBackReceiveFunction && NULL != Callback_Receive)
				{
					m_fRegister_CallBackReceiveFunction(Callback_Receive);
				}
				return true;
			}
		}
		return false;
	}

	bool LoadVTECSDll::DllSocket_IsConnected(void)
	{
		m_CriticalLoadUnload.Lock();

		if (NULL != m_fIsConnected)
		{
			const bool bReturn = m_fIsConnected();
			m_CriticalLoadUnload.Unlock();
			return bReturn;
		}

		m_CriticalLoadUnload.Unlock();
		return false;
	}

	char* LoadVTECSDll::DllSocket_GetReceiveMessageInQueue(const bool bLastReceiveMsgReturn, const bool bDeleteRemainReceiveQueue)
	{
		m_CriticalLoadUnload.Lock();

		if (NULL != m_fGetReceiveMessageInQueueBuffer)
		{
			char* pReturn = m_fGetReceiveMessageInQueueBuffer(bLastReceiveMsgReturn, bDeleteRemainReceiveQueue);
			m_CriticalLoadUnload.Unlock();
			return pReturn;
		}
		m_CriticalLoadUnload.Unlock();
		return NULL;
	}

	bool LoadVTECSDll::DllSocket_IsASCIITypeDll(void)
	{
		m_CriticalLoadUnload.Lock();

		if (NULL != m_fIsASCIIType)
		{
			const bool bReturn = m_fIsASCIIType();
			m_CriticalLoadUnload.Unlock();
			return bReturn;
		}
		m_CriticalLoadUnload.Unlock();
		return false;
	}

	bool LoadVTECSDll::DllSocket_AddToECS(CString& strCellID, CString& strFilePathName, CString& strData)
	{
		m_CriticalLoadUnload.Lock();

		if (NULL != m_fAddLine && !strFilePathName.IsEmpty() && !strData.IsEmpty())
		{
#ifdef _UNICODE
			CStringA strA, strB, strC;
			strA = strFilePathName;
			strB = strData;
			strC = strCellID;
			const bool bReturn = m_fAddLine((PCHAR)strC.GetString(), (PCHAR)strA.GetString(), (PCHAR)strB.GetString());
#else
			const bool bReturn = m_fAddLine((PCHAR)strCellID.GetString(), (PCHAR)strFilePathName.GetString(), (PCHAR)strData.GetString());
#endif // _UNICODE
			
			m_CriticalLoadUnload.Unlock();
			return bReturn;
		}

		m_CriticalLoadUnload.Unlock();
		return false;
	}

	bool LoadVTECSDll::DllSocket_Send(unsigned char* pSendBuffer, const int nSendLength)
	{
		m_CriticalLoadUnload.Lock();

		if (NULL != m_fSendData && NULL!= pSendBuffer && nSendLength>0)
		{
			const bool bReturn = m_fSendData(pSendBuffer, nSendLength);
			m_CriticalLoadUnload.Unlock();
			return bReturn;
		}

		m_CriticalLoadUnload.Unlock();
		return false;
	}

	bool LoadVTECSDll::DllSocket_IPConfigToggleShow(bool bShow, bool bToggleModeOnIsTrue)
	{
		m_CriticalLoadUnload.Lock();

		if (NULL != m_fToggleViewHideIPConfigDialog)
		{
			const bool bReturn = m_fToggleViewHideIPConfigDialog(bShow, bToggleModeOnIsTrue);
			m_CriticalLoadUnload.Unlock();
			return bReturn;
		}

		m_CriticalLoadUnload.Unlock();
		return false;
	}

	bool LoadVTECSDll::DllSocket_IsIntialized(void)
	{
		m_CriticalLoadUnload.Lock();

		if 	(NULL != m_fAddLine && NULL != m_fInitializeDll && NULL != m_fIsConnected && NULL != m_fIsASCIIType &&
			NULL != m_fToggleViewHideIPConfigDialog && NULL != m_fSendData && NULL != m_fGetReceiveMessageInQueueBuffer &&
			NULL != m_fExitDll && NULL != m_fRegister_CallBackConnectStatusFuntion && NULL != m_fRegister_CallBackReceiveFunction)
		{
			m_CriticalLoadUnload.Unlock();
			return true;
		}

		m_CriticalLoadUnload.Unlock();

		return false;
	}

	void LoadVTECSDll::LinkFuntionToAllNULL(void)
	{
		m_fInitializeDll = NULL;

		m_fExitDll = NULL;

		m_fIsConnected = NULL;

		m_fIsASCIIType = NULL;

		m_fSendData = NULL;

		m_fAddLine = NULL;

		m_fGetReceiveMessageInQueueBuffer = NULL;

		m_fToggleViewHideIPConfigDialog = NULL;

		m_fRegister_CallBackConnectStatusFuntion = NULL;

		m_fRegister_CallBackReceiveFunction = NULL;
	}

	/////////////////////////////////////////////////////////////////////////// End Namespace
}