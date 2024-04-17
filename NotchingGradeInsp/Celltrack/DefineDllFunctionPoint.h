#pragma once

#ifndef CONNECT_EVENT_LIST
#define	CONNECT_EVENT_LIST

/// <summary>
/// 통신 연결 상태값 정의
/// </summary>
enum enConnectEvent : signed char
{
	OnError = -1,
	OnInvalid,
	OnStart_CreateSocketMemory,     //소켓 메모리 생성하려고 시작
	OnReady,                        //서버 타입인 경우 client에서 통신 접속을 기다리는 상태
	OnConnected,                    //통신 연결상태
	OnConnectFail,                  //통신 연결 실패 상태
	OnDisConnected,                 //통신 연결 해제 상태
	OnTerminated,                   //통신 연결 제어 이벤트 처리부 해제 상태
	OnCloseComm                     //통신 control close 상태
};

#endif // !ENUM_CONNECT_EVENT_LIST

/// <summary>
/// 통신이 수신되면 수신받은 버퍼 접근주소와 수신받은 바이트 개수를 입력하여 호출하는 콜백함수 선언부
/// <param name="BYTE*">수신받은 버퍼의 접근 주소</param>
/// <param name="int">수신받은 바이트 개수</param>
/// </summary>
typedef UINT(__stdcall* callback_Bufferfunction)(unsigned char*, int);

/// <summary>
/// 통신 연결 상태가 변경되면 호출되는 콜백함수 선언부
/// </summary>
/// <param name="char*">이벤트 발생 내용 문자열</param>
/// <param name="int">연결 상태 int 값</param>
/// <param name="int">오류 코드 값</param>
/// <returns>정상처리 되면 true</returns>
typedef UINT(__stdcall* callback_ConnectionStatusfunction)(char*, int, int);

namespace LOADVTECSDll_GROUP
{
	//함수 포인터 define

	//dll에 송신, 바이트 버퍼와 바이트 개수
	typedef bool(*pfSendData)(byte*, int);

	//dll에 송신하려는 문자열 추가함수
	typedef bool(*pfAddLineDLL)(PCHAR, PCHAR, PCHAR);

	//dll에서 수신받은 메세지를 Queue에서 한개씩 수신받는 함수
	typedef char*(*fGetReceiveMessageInQueueBuffer)(bool,bool);

	//Dll 초기화 처리 함수
	//plc로부터 받은 메세지를 저장하는 Queue 사용유무
	typedef bool(*pfInitializeDll)(PCHAR, bool);

	//dll 통신 연결 상태 반환
	typedef bool(*pfIsConnected)(void);

	//ascii 타입 통신 타입이면 true, binary 타입이면 false
	typedef bool(*pfIsASCIIType)(void);

	//Dll 종료 호출 함수
	typedef bool(*pfExitDll)(void);

	/// <summary>
	/// IP Config Dialog 화면 보이기/숨기기
	/// </summary>
	typedef bool(*pfToggleViewHideIPConfigDialog)(bool, bool);

	//Dll에서 통신 연결 상태 CallBack 되는 함수 등록
	typedef void(*fRegister_CallBackConnectStatusFuntion)(callback_ConnectionStatusfunction);

	//Dll에서 통신 수신 후 callback 되는 함수 등록
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
	char	strID[10]; //문자열 128
	INT32	intCount;    //숫자형
	BYTE	byteData[70]; //바이트형 배열
	UINT32	uintCount[4];  //유니트형 배열
};

#pragma pack()
