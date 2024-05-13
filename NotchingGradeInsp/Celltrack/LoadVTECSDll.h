#pragma once
#include "./DefineDllFunctionPoint.h"
#include <afxmt.h>

namespace LOADVTECSDll_GROUP
{
	class LoadVTECSDll//:private stDllStatus
	{
	public:
		LoadVTECSDll(void);
		~LoadVTECSDll(void);

	private:

		//dll HANDLE ����
		HMODULE m_hdll;

		//dll ���� �� ��� ������ ������ ����
		CString m_strDllFile, m_strDllFilePath;

		//dll load/unload/call process/attack process ���ͷ���
		CCriticalSection	m_CriticalLoadUnload;

	private:

		//dll �ʱ�ȭ ó�� �Լ�
		pfInitializeDll		m_fInitializeDll;

		//dll �ʱ�ȭ�� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
		pfIsConnected		m_fIsConnected;

		/// <summary>
		/// ASCII Ÿ������̸� true, binary Ÿ�� ����̸� false
		/// </summary>
		pfIsASCIIType		m_fIsASCIIType;

		//dll ����ó�� �Լ�
		pfExitDll			m_fExitDll;

		//����Ʈ ������ ��� �۽� �Լ�
		pfSendData			m_fSendData;

		//���ڿ��� �߰��ϴ� �Լ�
		pfAddLineDLL		m_fAddLine;

		//���Ź��� �޼��� ù��° Queue ��� �޼����� ��ȯ�޴� �Լ�
		fGetReceiveMessageInQueueBuffer	m_fGetReceiveMessageInQueueBuffer;

		//Dll ��� ���� ȭ��
		pfToggleViewHideIPConfigDialog	m_fToggleViewHideIPConfigDialog;

		//Dll���� ��� ���� ���� CallBack �Ǵ� �Լ� ���
		fRegister_CallBackConnectStatusFuntion	m_fRegister_CallBackConnectStatusFuntion;

		//DLL���� ��� ������ �Ǹ� Callback �Ǵ� �Լ� ���
		fRegister_CallBackReceiveFunction		m_fRegister_CallBackReceiveFunction;

	public:

		//dll ���� Unloading ó��
		//����1		:	����
		//��ȯ		:	�����̸� TRUE, �׷��� ������ FALSE
		bool DllSocket_Unitialize(void);

		/// <summary>
		/// dll �ʱ�ȭ-��������
		/// </summary>
		/// <param name="Callback_Connect">��� ���� ���°� ����Ǹ� ȣ���ϴ� call back �Լ� �����ּ�</param>
		/// <param name="Callback_Receive">��� ������ �Ǹ� ȣ���ϴ� call back �Լ� ���� �ּ�</param>
		/// <param name="bUseReceiveQueueBuffer">���� Queue ���� ����̸� true, ������� ������ false</param>
		/// <returns>���� ó���Ǹ� true</returns>
		bool DllSocket_Initialize(callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive = NULL, const bool bUseReceiveQueueBuffer = true);

		/// <summary>
		/// dll �ʱ�ȭ-������ �������� ���������� dll �ʱ�ȭ �� �� ���
		/// </summary>
		/// <param name="strDllFold">dll ������ �ִ� ��� ���ڿ�</param>
		/// <param name="Callback_Connect">��� ���� ���°� ����Ǹ� ȣ���ϴ� call back �Լ� �����ּ�</param>
		/// <param name="Callback_Receive">��� ������ �Ǹ� ȣ���ϴ� call back �Լ� ���� �ּ�</param>
		/// <param name="bUseReceiveQueueBuffer">���� Queue ���� ����̸� true, ������� ������ false</param>
		/// <returns></returns>
		bool DllSocket_Initialize(const CString strDllFold, callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive = NULL, const bool bUseReceiveQueueBuffer = true);

		//dll ������ �Ǹ� TRUE, �׷��� ������ FALSE
		//����1		:	����
		//��ȯ		:	�ʱ�ȭ �Ǿ� ������ TRUE
		bool DllSocket_IsConnected(void);

		//dll���� ���Ź��� Queue ���ۿ��� ù��° �޼����� ȹ���ϴ� �Լ�
		//����1		:	���� �ֱٿ� ���� ���� �޼����� �ް� ������ true, ���� ���ſ� ���� ���� �޼����� �ް� ������ false
		//����1		:	�Լ� ȣ��� ���ÿ� ���� �޼��� Queue ���۸� ����(�⺻ ����: ����)
		//��ȯ		:	���Ź��� �޼����� ���ڿ��� ��ȯ�� ��, NULL�̸� ���Ź��� ���� �����ϴ�.
		char* DllSocket_GetReceiveMessageInQueue(const bool bLastReceiveMsgReturn = false, const bool bDeleteRemainReceiveQueue = false);

		/// <summary>
		/// ASCII Ÿ�� ����̸� true, binary Ÿ��
		/// </summary>
		/// <param name=""></param>
		/// <returns>ASCII Ÿ���̸� true, Binary Ÿ���̸� false</returns>
		bool DllSocket_IsASCIITypeDll(void);

		//�۽��� �޼����� �߰��ϴ� �Լ�
		//����1		:	�� id ���ڿ�
		//����1		:	���ڿ��� ����Ǵ� ���� ��� �� ���� �̸� ����
		//����2		:	�߰��� Data �޼��� ���ڿ�
		//��ȯ		:	�����̸� TRUE
		bool DllSocket_AddToECS(CString& strCellID, CString& strFilePathName, CString& strData);

		/// <summary>
		/// ����Ʈ ���ۿ� ����Ʈ ������ �����Ͽ� �̴������� �۽�
		/// </summary>
		/// <param name="pSendBuffer">�۽��� ����Ʈ ����</param>
		/// <param name="nSendLength">�۽��� ����Ʈ ����</param>
		/// <returns>���������� �۽��� ��� true</returns>
		bool DllSocket_Send(unsigned char* pSendBuffer, const int nSendLength);

		/// <summary>
		/// DLL IP Config â ���̱�/�����
		/// </summary>
		/// <param name="bShow">â ���̱� true, ����� false</param>
		/// <param name="bToggleModeOnIsTrue">â ���̱� ��� ��� ����̸� true, ��۸�忡���� bShow ���� ���� �˴ϴ�.</param>
		/// <returns></returns>
		bool DllSocket_IPConfigToggleShow(bool bShow, bool bToggleModeOnIsTrue);

		//dll ���¸� ��ȯ�Ѵ�.
		//����1		:	����
		//��ȯ		:	�����̸� TRUE
		bool DllSocket_IsIntialized(void);

	private:

		/// <summary>
		/// dll �ʱ�ȭ-������ �������� ���������� dll �ʱ�ȭ �� �� ���
		/// </summary>
		/// <param name="strDllFold"></param>
		/// <param name="Callback_Connect">��� ���� ���°� ����Ǹ� ȣ���ϴ� call back �Լ� �����ּ�</param>
		/// <param name="Callback_Receive">��� ������ �Ǹ� ȣ���ϴ� call back �Լ� ���� �ּ�</param>
		/// <param name="bUseReceiveQueueBuffer">���� Queue ���� ����̸� true, ������� ������ false</param>
		/// <returns></returns>
		bool SubDllSocket_Initialize(const CString strDllFold, callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive = NULL, const bool bUseReceiveQueueBuffer = true);

		//dll ���� Loading ó��
		//����1		:	dll������ �ִ� ���� ��� ���� ������ ���ڿ��� '\\' �߰� �ʿ�
		//��ȯ		:	�����̸� TRUE, �׷��� ������ FALSE
		bool LoadingDll(const CString strToLoadDllPathName);

		//������ �Լ� ���� �Լ�
		//����1		:	����
		//��ȯ		:	�����̸� TRUE, �׷��� ������ FALSE
		bool LinkFuntion(void);

		//������ �Լ��� ��� NULL ó���ϴ� �Լ�
		//����1		:	����
		//��ȯ		:	����
		void LinkFuntionToAllNULL(void);

		/////////////////////////////////////////////////////////////////////// End Class
	};
	/////////////////////////////////////////////////////////////////////////// End Namespace
	
}