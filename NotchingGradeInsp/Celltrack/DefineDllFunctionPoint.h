#pragma once

#ifndef CONNECT_EVENT_LIST
#define	CONNECT_EVENT_LIST

/// <summary>
/// ��� ���� ���°� ����
/// </summary>
enum enConnectEvent : signed char
{
	OnError = -1,
	OnInvalid,
	OnStart_CreateSocketMemory,     //���� �޸� �����Ϸ��� ����
	OnReady,                        //���� Ÿ���� ��� client���� ��� ������ ��ٸ��� ����
	OnConnected,                    //��� �������
	OnConnectFail,                  //��� ���� ���� ����
	OnDisConnected,                 //��� ���� ���� ����
	OnTerminated,                   //��� ���� ���� �̺�Ʈ ó���� ���� ����
	OnCloseComm                     //��� control close ����
};

#endif // !ENUM_CONNECT_EVENT_LIST

/// <summary>
/// ����� ���ŵǸ� ���Ź��� ���� �����ּҿ� ���Ź��� ����Ʈ ������ �Է��Ͽ� ȣ���ϴ� �ݹ��Լ� �����
/// <param name="BYTE*">���Ź��� ������ ���� �ּ�</param>
/// <param name="int">���Ź��� ����Ʈ ����</param>
/// </summary>
typedef UINT(__stdcall* callback_Bufferfunction)(unsigned char*, int);

/// <summary>
/// ��� ���� ���°� ����Ǹ� ȣ��Ǵ� �ݹ��Լ� �����
/// </summary>
/// <param name="char*">�̺�Ʈ �߻� ���� ���ڿ�</param>
/// <param name="int">���� ���� int ��</param>
/// <param name="int">���� �ڵ� ��</param>
/// <returns>����ó�� �Ǹ� true</returns>
typedef UINT(__stdcall* callback_ConnectionStatusfunction)(char*, int, int);

namespace LOADVTECSDll_GROUP
{
	//�Լ� ������ define

	//dll�� �۽�, ����Ʈ ���ۿ� ����Ʈ ����
	typedef bool(*pfSendData)(byte*, int);

	//dll�� �۽��Ϸ��� ���ڿ� �߰��Լ�
	typedef bool(*pfAddLineDLL)(PCHAR, PCHAR, PCHAR);

	//dll���� ���Ź��� �޼����� Queue���� �Ѱ��� ���Ź޴� �Լ�
	typedef char*(*fGetReceiveMessageInQueueBuffer)(bool,bool);

	//Dll �ʱ�ȭ ó�� �Լ�
	//plc�κ��� ���� �޼����� �����ϴ� Queue �������
	typedef bool(*pfInitializeDll)(PCHAR, bool);

	//dll ��� ���� ���� ��ȯ
	typedef bool(*pfIsConnected)(void);

	//ascii Ÿ�� ��� Ÿ���̸� true, binary Ÿ���̸� false
	typedef bool(*pfIsASCIIType)(void);

	//Dll ���� ȣ�� �Լ�
	typedef bool(*pfExitDll)(void);

	/// <summary>
	/// IP Config Dialog ȭ�� ���̱�/�����
	/// </summary>
	typedef bool(*pfToggleViewHideIPConfigDialog)(bool, bool);

	//Dll���� ��� ���� ���� CallBack �Ǵ� �Լ� ���
	typedef void(*fRegister_CallBackConnectStatusFuntion)(callback_ConnectionStatusfunction);

	//Dll���� ��� ���� �� callback �Ǵ� �Լ� ���
	typedef void(*fRegister_CallBackReceiveFunction)(callback_Bufferfunction);

}

#pragma pack(1)

struct stDllStatus
{
	bool bIsConnected;
	bool bInitialzed;

	stDllStatus()
	{
		bIsConnected = false;
		bInitialzed = false;
	}

	BOOL operator!=(const stDllStatus& Input)
	{
		return 0 != memcmp(this, &Input, sizeof(*this));
	}

	stDllStatus& operator=(const stDllStatus& Input)
	{
		memcpy(this, &Input, sizeof(stDllStatus));
		return *this;
	}
};

struct typeTest
{
	char	strID[10]; //���ڿ� 128
	INT32	intCount;    //������
	BYTE	byteData[70]; //����Ʈ�� �迭
	UINT32	uintCount[4];  //����Ʈ�� �迭
};

#pragma pack()
