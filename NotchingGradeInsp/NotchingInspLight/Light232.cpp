// Light232.cpp : 구현 파일입니다.
//

//#include "../stdafx.h"
#include "pch.h" // 추가
#include "Light232.h"


// CLight232

IMPLEMENT_DYNAMIC(CLight232, CCmdTarget)


CLight232::CLight232(CString port,CString baudrate,CString parity,CString databit,CString stopbit)
{
                  m_sComPort = port;
                  m_sBaudRate = baudrate;
                  m_sParity = parity;
                  m_sDataBit = databit;
                  m_sStopBit = stopbit;
                  m_bFlowChk = 1;
                  m_bIsOpenned = FALSE;
                  m_nLength = 0;
                  memset(m_sInBuf,0,MAXBUF*2);
                  m_pEvent = new CEvent(FALSE,TRUE);
}

CLight232::~CLight232()
{
	 if (m_bIsOpenned)
          Close();
      delete m_pEvent;
}


BEGIN_MESSAGE_MAP(CLight232, CCmdTarget)
END_MESSAGE_MAP()



// CLight232 메시지 처리기입니다.
void CLight232::ResetSerial()
{
                  DCB                              dcb;
                  DWORD      DErr;
                  COMMTIMEOUTS           CommTimeOuts;
 
                  if (!m_bIsOpenned)
                                   return;

				  // 통신포트의 모든 에러를 리셋 
                  ClearCommError(m_hComDev,&DErr,NULL);
				  // 모든 Rx/Tx 동작을 제한하고 또한 Buffer의 내용을 버림
                  SetupComm(m_hComDev,InBufSize,OutBufSize);
				  // 모든 Rx/Tx 동작을 제한하고 또한 Buffer의 내용을 버림
				  PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
 
                  // set up for overlapped I/O (MSDN 참조)
				  // 통신 선로상에서 한바이트가 전송되고 또한 바이트가 전송되기까지의 시간
				  CommTimeOuts.ReadIntervalTimeout = MAXDWORD ;
				  
				  // Read doperation 에서 TimeOut을 사용하지 않음
                  CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
                  CommTimeOuts.ReadTotalTimeoutConstant = 0 ;
 
                  // CBR_9600 is approximately 1byte/ms. For our purposes, allow
                  // double the expected time per character for a fudge factor.
                  CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
                  CommTimeOuts.WriteTotalTimeoutConstant = 1000;
				  // 통신포트의TimeOut을설정
                  SetCommTimeouts(m_hComDev, &CommTimeOuts);                
 
 
                  memset(&dcb,0,sizeof(DCB));
                  dcb.DCBlength = sizeof(DCB);
				  // 통신포트의 DCB를 얻음
 
                  GetCommState(m_hComDev, &dcb);
				  // DCB를 설정(DCB: 시리얼통신의 제어 파라메터, MSDN 참조)
 
                  dcb.fBinary = TRUE;
                  dcb.fParity = TRUE;
 
                  if (m_sBaudRate == "300")
                                   dcb.BaudRate = CBR_300;
                  else if (m_sBaudRate == "600")
                                   dcb.BaudRate = CBR_600;
                  else if (m_sBaudRate == "1200")
                                   dcb.BaudRate = CBR_1200;
                  else if (m_sBaudRate == "2400")
                                   dcb.BaudRate = CBR_2400;
                  else if (m_sBaudRate == "4800")
                                   dcb.BaudRate = CBR_4800;
                  else if (m_sBaudRate == "9600")
                                   dcb.BaudRate = CBR_9600;
                  else if (m_sBaudRate == "14400")
                                   dcb.BaudRate = CBR_14400;
                  else if (m_sBaudRate == "19200")
                                   dcb.BaudRate = CBR_19200;
                  else if (m_sBaudRate == "28800")
                                   dcb.BaudRate = CBR_38400;
                  else if (m_sBaudRate == "33600")
                                   dcb.BaudRate = CBR_38400;
                  else if (m_sBaudRate == "38400")
                                   dcb.BaudRate = CBR_38400;
                  else if (m_sBaudRate == "56000")
                                   dcb.BaudRate = CBR_56000;
                  else if (m_sBaudRate == "57600")
                                   dcb.BaudRate = CBR_57600;
                  else if (m_sBaudRate == "115200")
                                   dcb.BaudRate = CBR_115200;
                  else if (m_sBaudRate == "128000")
                                   dcb.BaudRate = CBR_128000;
                  else if (m_sBaudRate == "256000")
                                   dcb.BaudRate = CBR_256000;
                  else if (m_sBaudRate == "PCI_9600")
                                   dcb.BaudRate = 1075;
                  else if (m_sBaudRate == "PCI_19200")
                                   dcb.BaudRate = 2212;
                  else if (m_sBaudRate == "PCI_38400")
                                   dcb.BaudRate = 4300;
                  else if (m_sBaudRate == "PCI_57600")
                                   dcb.BaudRate = 6450;
                  else if (m_sBaudRate == "PCI_500K")
                                   dcb.BaudRate = 56000;
 
 
                  if (m_sParity == "None")
                                   dcb.Parity = NOPARITY;
                  else if (m_sParity == "Even")
                                   dcb.Parity = EVENPARITY;
                  else if (m_sParity == "Odd")
                                   dcb.Parity = ODDPARITY;
 
                  if (m_sDataBit == "7 Bit")
                                   dcb.ByteSize = 7;
                  else if (m_sDataBit == "8 Bit")
                                   dcb.ByteSize = 8;                             
 
                  if (m_sStopBit == "1 Bit")
                                   dcb.StopBits = ONESTOPBIT;
                  else if (m_sStopBit == "1.5 Bit")
                                   dcb.StopBits = ONE5STOPBITS;
                  else if (m_sStopBit == "2 Bit")
                                   dcb.StopBits = TWOSTOPBITS;
 
                  dcb.fRtsControl = RTS_CONTROL_ENABLE;
                  dcb.fDtrControl = DTR_CONTROL_ENABLE;
                  dcb.fOutxDsrFlow = FALSE;
 
                  if (m_bFlowChk) {
                                   dcb.fOutX = FALSE;
                                   dcb.fInX = FALSE;
                                   dcb.XonLim = 2048;
                                   dcb.XoffLim = 1024;
                  }
                  else {
                                   dcb.fOutxCtsFlow = TRUE;
                                   dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
                  }
 
				   // 설정된 DCB로 통신포트의 제어 파라메터를 설정
                  SetCommState(m_hComDev, &dcb);
				  // Input Buffer에 데이터가 들어왔을 때 이벤트가 발생하도록 설정
                  SetCommMask(m_hComDev,EV_RXCHAR);
 
}
 
void CLight232::Close()
{
                  if (!m_bIsOpenned)
                                   return;
 
                  m_bIsOpenned = FALSE;
                  SetCommMask(m_hComDev, 0);
                  EscapeCommFunction(m_hComDev, CLRDTR);
                  PurgeComm(m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
                  Sleep(500);
 
}
 
UINT CommThread(LPVOID lpData)
{
                  extern short	 g_nRemoteStatus;
                  DWORD			 ErrorFlags;
                  COMSTAT		 ComStat;
                  DWORD			 EvtMask ;
                  char           buf[MAXBUF];
                  DWORD			 Length;
                  int            size;
                  int            insize = 0;

				  

				   // 통신클래스의 객체포인터를 얻음
				  CLight232* Comm = (CLight232*)lpData;
 
				  // 통신포트가 열려 있다면
                  while (Comm->m_bIsOpenned)
                  {
								   EvtMask = 0;
                                   Length = 0;
                                   insize = 0;
                                   memset(buf,'\0',MAXBUF);
								   //이벤트를 기다림
                                   WaitCommEvent(Comm->m_hComDev,&EvtMask, NULL);
                                   ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat);

								    // EV_RXCHAR에서 이벤트가 발생하면
                                   if ((EvtMask & EV_RXCHAR) && ComStat.cbInQue)
                                   {
									   Sleep(100); // 180518 감지되고 나서 100 ms 만큼 기다렸다 읽는다. 안해주면 나눠서 옴 -_-;

													 if (ComStat.cbInQue > MAXBUF)
                                                                       size = MAXBUF;
                                                     else
                                                                    //   size = ComStat.cbInQue;
																		size = MAXBUF;  // 180518 ComStat.cbInQue 만큼 읽으면 나눠짐. MAX 값 ㄱㄱ
                                                     do
                                                     {
                                                                       ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat);
	   
																	   // overlapped I/O를 통해 데이터를 읽음
                                                                       if (!ReadFile(Comm->m_hComDev,buf+insize,size,&Length,&(Comm->m_OLR)))
                                                                       {
                                                                                        // 에러
                                                                                        TRACE("Error in ReadFile\n");
                                                                                        if (GetLastError() == ERROR_IO_PENDING)
                                                                                        {
                                                                                                          if (WaitForSingleObject(Comm->m_OLR.hEvent, 1000) != WAIT_OBJECT_0) 
                                                                                                                            Length = 0;
                                                                                                          else
                                                                                                                            GetOverlappedResult(Comm->m_hComDev,&(Comm->m_OLR),&Length,FALSE);
                                                                                        }
                                                                                        else
                                                                                                          Length = 0;
                                                                       }
                                                                       insize += Length;
                                                     } while ((Length!=0) && (insize<size));
                                                     ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat);
 
                                                     if (Comm->m_nLength + insize > MAXBUF*2)
                                                                       insize = (Comm->m_nLength + insize) - MAXBUF*2;

													  // 이벤트 발생을 잠시 중단하고 input buffer에 데이터를 복사 
                                                     Comm->m_pEvent->ResetEvent();
                                                     memcpy(Comm->m_sInBuf+Comm->m_nLength,buf,insize);
                                                     Comm->m_nLength += insize;
													  // 복사가 끝나면 다시 이벤트를 활성화 시키고
                                                     Comm->m_pEvent->SetEvent();
                                                     LPARAM temp=(LPARAM)Comm;
													 // 데이터가 들어왔다는 메시지를 발생
                                                     SendMessage(Comm->m_hWnd,WM_MYRECEIVE,Comm->m_nLength,temp);
                                   }
                  }
                  PurgeComm(Comm->m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
                  LPARAM temp=(LPARAM)Comm;
				  // 쓰레드가 종료될 때 종료 메시지를 보냄
                  SendMessage(Comm->m_hWnd,WM_MYCLOSE,0,temp);
                  return 0;
}
 
void CLight232::HandleClose()
{
                  CloseHandle(m_hComDev);
                  CloseHandle(m_OLR.hEvent);
                  CloseHandle(m_OLW.hEvent);
 
}
 
BOOL CLight232::Create(HWND hWnd)
{
                  m_hWnd = hWnd;
 
                  m_hComDev = CreateFile(m_sComPort, GENERIC_READ | GENERIC_WRITE,
                                   0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                   NULL);
 
                  if (m_hComDev!=INVALID_HANDLE_VALUE)
                                   m_bIsOpenned = TRUE;
                  else
                                   return FALSE;
 
                  ResetSerial();
 
                  m_OLW.Offset = 0;
                  m_OLW.OffsetHigh = 0;
                  m_OLR.Offset = 0;
                  m_OLR.OffsetHigh = 0;
 
                  m_OLR.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                  if (m_OLR.hEvent == NULL) {
                                   CloseHandle(m_OLR.hEvent);
                                   return FALSE;
                  }
                  m_OLW.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                  if (m_OLW.hEvent == NULL) {
                                   CloseHandle(m_OLW.hEvent);
                                   return FALSE;
                  }
                  AfxBeginThread(CommThread,(LPVOID)this);
                  EscapeCommFunction(m_hComDev, SETDTR);
                  return TRUE;
 
}
 
BOOL CLight232::Send(LPCTSTR outbuf, int len)
{
                  BOOL        bRet=TRUE;
                  DWORD       ErrorFlags;
                  COMSTAT     ComStat;
 
                  DWORD       BytesWritten;
                  DWORD       BytesSent=0;

				  //--유니코드 멀티 문자로 바꿔 주는 코드--// 
				  char *sTime = NULL;
				  int nLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)outbuf, -1, sTime, 0, NULL, NULL);
				  sTime = new char[nLen+1];
				  WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)outbuf, -1, sTime, 128, NULL, NULL);
				  //--유니코드 멀티 문자로 바꿔 주는 코드--// by - DG Gwak

				  // 통신 포트의 모든 에러를 리셋
                  ClearCommError(m_hComDev, &ErrorFlags, &ComStat);

				  // overlapped I/O를 통하여 outbuf의 내용을 len길이 만큼 전송
  //                if (!WriteFile(m_hComDev, outbuf, len, &BytesWritten, &m_OLW))  //메세지 전송인데 길이만큼 안감 
				  if (!WriteFile(m_hComDev, sTime, len, &BytesWritten, &m_OLW))  //메세지 전송인데 길이만큼 안감 // 180518
                  {
                                   if (GetLastError() == ERROR_IO_PENDING)
                                   {
                                                     if (WaitForSingleObject(m_OLW.hEvent,1000) != WAIT_OBJECT_0)
													 {bRet=FALSE;}
                                                     else
													 {GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE);}
                                   }
                                   else /* I/O error */
								   bRet=FALSE;/* ignore error */
                  }
 
				  // 다시 한번 통신포트의 모든 에러를 리셋

                  ClearCommError(m_hComDev, &ErrorFlags, &ComStat);

				  //--char 포인터 사용으로 문자열 해제--//
				  delete [] sTime;
				  //--char 포인터 사용으로 문자열 해제--// by - DG Gwak

                  return bRet;
 
}

BOOL CLight232::Send(LPSTR outbuf, int len)
{

    BOOL        bRet = TRUE;
    DWORD       ErrorFlags;
    COMSTAT     ComStat;

    DWORD       BytesWritten;
    DWORD       BytesSent = 0;

    // 통신 포트의 모든 에러를 리셋
    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);

    // overlapped I/O를 통하여 outbuf의 내용을 len길이 만큼 전송
//                if (!WriteFile(m_hComDev, outbuf, len, &BytesWritten, &m_OLW))  //메세지 전송인데 길이만큼 안감 
    if (!WriteFile(m_hComDev, outbuf, len, &BytesWritten, &m_OLW))  //메세지 전송인데 길이만큼 안감 // 180518
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            if (WaitForSingleObject(m_OLW.hEvent, 1000) != WAIT_OBJECT_0)
            {
                bRet = FALSE;
            }
            else
            {
                GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE);
            }
        }
        else /* I/O error */
            bRet = FALSE;/* ignore error */
    }

    // 다시 한번 통신포트의 모든 에러를 리셋

    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);

    return bRet;

    return 0;
}
 
int CLight232::Receive(LPSTR inbuf, int len)
{
                  CSingleLock lockObj((CSyncObject*) m_pEvent,FALSE);
                  // argument value is not valid

                  if (len == 0)
                                   return -1;
                  else if  (len > MAXBUF)
                                   return -1;
 
                  if (m_nLength == 0)
                  {
                                   inbuf[0] = '\0';
                                   return 0;
                  }
				    // 정상적이라면 본루틴으로 들어와 실제 들어온 데이터의 길이 만큼 Input Buffer에서 데이터를 읽음
                  else if (m_nLength <= len)
                  {
                                   lockObj.Lock();
                                   memcpy(inbuf,m_sInBuf,m_nLength);
                                   memset(m_sInBuf,0,MAXBUF*2);
                                   int tmp = m_nLength;
                                   m_nLength = 0;
                                   lockObj.Unlock();
                                   return tmp;
                  }
                  else
                  {
                                   lockObj.Lock();
                                   memcpy(inbuf,m_sInBuf,len);
                                   memmove(m_sInBuf,m_sInBuf+len,MAXBUF*2-len);
                                   m_nLength -= len;
                                   lockObj.Unlock();
                                   return len;
                  }
}
 
void CLight232::Clear()
{
                  PurgeComm(m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
                  memset(m_sInBuf,0,MAXBUF*2);
                  m_nLength = 0;
 
}
