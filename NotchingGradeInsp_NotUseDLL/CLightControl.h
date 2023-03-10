#pragma once



/*
	ALT Light 
	PORT : 1000
	IP : 192.168.10.10
	MASK : 255.255.255.0
	GATEWAY : 192.168.10.1

	

	CMD 표 
	0x12 : Request / Response
	0x13 개별 Segment Bright Write Requset / Response 
*/
//#define MAX_LIGHT_CH 2 
//#define MAX_LIGHT_UNIT 2

class CLightSocket;

class CLightControl
{
public :

	enum {
		UDP_PORT = 1000,
		TCP_PORT = 1000,
	};
	enum {
		en_CMD_CH_MODE			= 0x12,
		en_CMD_SEG_BRI			= 0x13,
		en_CMD_ALL_SEG_WRRITE	= 0x14,
		en_CMD_ALL_SEG_READ		= 0x15, // All Segment Read Request/Response 같은 CMD사용
		en_CMD_CH_WRITE			= 0x16,
		en_CMD_ALL_CH_WRITE		= 0x17,
		en_CMD_ALL_CH_READ		= 0x18, // 
		en_CMD_ADJUST_WRITE		= 0x19,
		en_CMD_ALL_ADJUST_WRITE	= 0x1A,
		en_CMD_ALL_ADJUST_READ	= 0x1B,
		en_CMD_DATA_SAVE		= 0x1C,
		en_CMD_DATA_LOAD		= 0x1D,

	};

	enum {
		MAX_CHANEL = 8,
	};

	enum {
		en_CH_MODE_Length				= 3, // Request/Response Command의 길이 cmd, channel, check-sum 
		en_SEG_BRI_Length				= 4, // Set Bright One Chennel CMD의 길이 cmd, chennel No, BrightLv, checksum
		en_ALL_SEG_BRI_W_Length			= 10,
		en_ALL_SEG_BRI_R_Length			= 2,
		en_ALL_SEG_BRI_R_RESP_Length	= 10,
		en_CH_WRITE_Length				= 4, 
		en_ALL_CH_WRITE_Length			= 10,
		en_CH_READ_REQ_Length			= 2,
		en_ALL_CH_READ_RESP_Length		= 10,
		en_SEG_ADJUST_WRITE_Length		= 4,
		en_ALL_ADJUST_WRITE_Length		= 10,
		en_ALL_AJJUST_READ_Length		= 2,
		en_ALL_ADJUST_READ_RESP_Length	= 10,
		en_DATA_SAVE_Length				= 2,
		en_DATA_LOAD_Length				= 2,
	};

	CLightControl(void);
	~CLightControl();
	int Open(void);
	int Close(void);

	int SetLevel(int ch, int nLevel);
	int SetLevel(int unit, int ch, int nLevel);
	int SetLevelAll(int unit, int *pLevel, int nLength = 8);
	int GetLevelAll(int unit, int* pLevel, int nLength = 8);

	int CheckLevel(int ch, int nLevel);
	int SetLampON(int ch, BOOL bON);
	BOOL IsLampError(int ch);
	BOOL IsLampErrorAll(void);

//	int SendNRecvPacket(int unit, char* pSendBuff, char* pRecvBuff, int nRecvLen, long lTimeOver);
//	int SendNRecvPacket(CLightSocket* pSock, char* pSendBuff, char* pRecvBuff, int nRecvLen, long lTimeOver);
	int SendNRecvPacket(int unit, char* pSendBuff, char* pRecvBuff, int nCmdLen, int nRecvLen,long lTimeOver);
	int SendNRecvPacket(CLightSocket* pSock, char* pSendBuff, char* pRecvBuff, int nSendLen, int nRecvLen, long lTimeOver);

	// 22.06.71 Ahn Add Start
	BOOL IsOpened();
	// 22.06.71 Ahn Add End
	int Pump(void);
	// 22.07.07 Ahn Add Start
	int GetMaxUnit() { return m_nMaxLight; };
	// 22.07.07 Ahn Add End

private :
	int SetLevel_8Bit(int ch, BYTE level);
	int SetLevel_8Bit(int unit, int ch, BYTE level);

private:
	int GetConnect(int no);
	int Reconnect(int no);

private:
	int SetUnit(int no, const CString& ip, int mode);
	int DeleteUnit(void);

private:
//	CLightSocket* m_pLightSock;
	CLightSocket* m_pClient[MAX_LIGHT_CHANEL] ;
	CLightSocket* GetClient( int ch ) ;


	BOOL m_bOpened;

	int m_nMaxLight;
	int m_pLevel[MAX_CHANEL] ;
	int m_pFineLevel[MAX_CHANEL] ;
	


};

