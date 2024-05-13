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

		//dll HANDLE 변수
		HMODULE m_hdll;

		//dll 파일 및 경로 정보를 저장한 변수
		CString m_strDllFile, m_strDllFilePath;

		//dll load/unload/call process/attack process 인터럽터
		CCriticalSection	m_CriticalLoadUnload;

	private:

		//dll 초기화 처리 함수
		pfInitializeDll		m_fInitializeDll;

		//dll 초기화가 되어 있는지 확인하는 함수
		pfIsConnected		m_fIsConnected;

		/// <summary>
		/// ASCII 타입통신이면 true, binary 타입 통신이면 false
		/// </summary>
		pfIsASCIIType		m_fIsASCIIType;

		//dll 종료처리 함수
		pfExitDll			m_fExitDll;

		//바이트 단위로 통신 송신 함수
		pfSendData			m_fSendData;

		//문자열을 추가하는 함수
		pfAddLineDLL		m_fAddLine;

		//수신받은 메세지 첫번째 Queue 요소 메세지를 반환받는 함수
		fGetReceiveMessageInQueueBuffer	m_fGetReceiveMessageInQueueBuffer;

		//Dll 통신 설정 화면
		pfToggleViewHideIPConfigDialog	m_fToggleViewHideIPConfigDialog;

		//Dll에서 통신 연결 상태 CallBack 되는 함수 등록
		fRegister_CallBackConnectStatusFuntion	m_fRegister_CallBackConnectStatusFuntion;

		//DLL에서 통신 수신이 되면 Callback 되는 함수 등록
		fRegister_CallBackReceiveFunction		m_fRegister_CallBackReceiveFunction;

	public:

		//dll 파일 Unloading 처리
		//인자1		:	없음
		//반환		:	정상이면 TRUE, 그렇지 않으면 FALSE
		bool DllSocket_Unitialize(void);

		/// <summary>
		/// dll 초기화-실행파일
		/// </summary>
		/// <param name="Callback_Connect">통신 연결 상태가 변경되면 호출하는 call back 함수 접근주소</param>
		/// <param name="Callback_Receive">통신 수신이 되면 호출하는 call back 함수 접근 주소</param>
		/// <param name="bUseReceiveQueueBuffer">수신 Queue 버퍼 사용이면 true, 사용하지 않으면 false</param>
		/// <returns>정상 처리되면 true</returns>
		bool DllSocket_Initialize(callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive = NULL, const bool bUseReceiveQueueBuffer = true);

		/// <summary>
		/// dll 초기화-동일한 폴더에서 실행파일이 dll 초기화 할 때 사용
		/// </summary>
		/// <param name="strDllFold">dll 파일이 있는 경로 문자열</param>
		/// <param name="Callback_Connect">통신 연결 상태가 변경되면 호출하는 call back 함수 접근주소</param>
		/// <param name="Callback_Receive">통신 수신이 되면 호출하는 call back 함수 접근 주소</param>
		/// <param name="bUseReceiveQueueBuffer">수신 Queue 버퍼 사용이면 true, 사용하지 않으면 false</param>
		/// <returns></returns>
		bool DllSocket_Initialize(const CString strDllFold, callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive = NULL, const bool bUseReceiveQueueBuffer = true);

		//dll 연결이 되면 TRUE, 그렇지 않으면 FALSE
		//인자1		:	없음
		//반환		:	초기화 되어 있으면 TRUE
		bool DllSocket_IsConnected(void);

		//dll에서 수신받은 Queue 버퍼에서 첫번째 메세지를 획득하는 함수
		//인자1		:	가장 최근에 받은 수신 메세지를 받고 싶으면 true, 가장 과거에 받은 수신 메세지를 받고 싶으면 false
		//인자1		:	함수 호출과 동시에 수신 메세지 Queue 버퍼를 제거(기본 제거: 권장)
		//반환		:	수신받은 메세지를 문자열로 변환한 것, NULL이면 수신받은 것이 없습니다.
		char* DllSocket_GetReceiveMessageInQueue(const bool bLastReceiveMsgReturn = false, const bool bDeleteRemainReceiveQueue = false);

		/// <summary>
		/// ASCII 타입 통신이면 true, binary 타입
		/// </summary>
		/// <param name=""></param>
		/// <returns>ASCII 타입이면 true, Binary 타입이면 false</returns>
		bool DllSocket_IsASCIITypeDll(void);

		//송신할 메세지를 추가하는 함수
		//인자1		:	셀 id 문자열
		//인자1		:	문자열이 저장되는 파일 경로 및 파일 이름 포함
		//인자2		:	추가할 Data 메세지 문자열
		//반환		:	정상이면 TRUE
		bool DllSocket_AddToECS(CString& strCellID, CString& strFilePathName, CString& strData);

		/// <summary>
		/// 바이트 버퍼와 바이트 개수를 대입하여 이더넷으로 송신
		/// </summary>
		/// <param name="pSendBuffer">송신할 바이트 버퍼</param>
		/// <param name="nSendLength">송신할 바이트 개수</param>
		/// <returns>정상적으로 송신할 경우 true</returns>
		bool DllSocket_Send(unsigned char* pSendBuffer, const int nSendLength);

		/// <summary>
		/// DLL IP Config 창 보이기/숨기기
		/// </summary>
		/// <param name="bShow">창 보이기 true, 숨기기 false</param>
		/// <param name="bToggleModeOnIsTrue">창 보이기 토글 모드 사용이면 true, 토글모드에서는 bShow 값은 무시 됩니다.</param>
		/// <returns></returns>
		bool DllSocket_IPConfigToggleShow(bool bShow, bool bToggleModeOnIsTrue);

		//dll 상태를 반환한다.
		//인자1		:	없음
		//반환		:	정상이면 TRUE
		bool DllSocket_IsIntialized(void);

	private:

		/// <summary>
		/// dll 초기화-동일한 폴더에서 실행파일이 dll 초기화 할 때 사용
		/// </summary>
		/// <param name="strDllFold"></param>
		/// <param name="Callback_Connect">통신 연결 상태가 변경되면 호출하는 call back 함수 접근주소</param>
		/// <param name="Callback_Receive">통신 수신이 되면 호출하는 call back 함수 접근 주소</param>
		/// <param name="bUseReceiveQueueBuffer">수신 Queue 버퍼 사용이면 true, 사용하지 않으면 false</param>
		/// <returns></returns>
		bool SubDllSocket_Initialize(const CString strDllFold, callback_ConnectionStatusfunction Callback_Connect, callback_Bufferfunction Callback_Receive = NULL, const bool bUseReceiveQueueBuffer = true);

		//dll 파일 Loading 처리
		//인자1		:	dll파일이 있는 파일 경로 지정 마지막 문자열에 '\\' 추가 필요
		//반환		:	정상이면 TRUE, 그렇지 않으면 FALSE
		bool LoadingDll(const CString strToLoadDllPathName);

		//포인터 함수 연결 함수
		//인자1		:	없음
		//반환		:	정상이면 TRUE, 그렇지 않으면 FALSE
		bool LinkFuntion(void);

		//포인터 함수를 모두 NULL 처리하는 함수
		//인자1		:	없음
		//반환		:	없음
		void LinkFuntionToAllNULL(void);

		/////////////////////////////////////////////////////////////////////// End Class
	};
	/////////////////////////////////////////////////////////////////////////// End Namespace
	
}