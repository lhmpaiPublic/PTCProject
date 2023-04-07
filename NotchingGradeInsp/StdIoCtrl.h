#pragma once

class CStdIoCtrl
{
protected:
	CStdIoCtrl();

public:
	typedef enum {
		eIO_Ccl = 0,
		eIO_Api,
		eIO_Fbi,
		eIO_MXCtrl,
		eIO_AXL,
		eIO_Max,
	}eIOTYPE;

	static int	GetLimitPort(eIOTYPE type);
	static int	GetLimitSeqNo(eIOTYPE type);
	static CString GetIoName(int mode)
	{
		CString str = "NoDefine";
		switch (mode)
		{
		case eIO_Ccl: str = "CC-Link"; break;
		case eIO_Api: str = "ContecIo"; break;
		case eIO_Fbi: str = "InterfaceIo"; break;
		case eIO_MXCtrl: str = "MX Control"; break;
		case eIO_AXL: str = "AJIN-DIO"; break;
		default: ASSERT(FALSE); ; break;
		}
		return (str);
	}
	static CStdIoCtrl* GetInstance(eIOTYPE type, int PortMax, int SeqNo, int RegMin, int RegMax);

public:
	virtual ~CStdIoCtrl();
	virtual int Open(BOOL debug = FALSE) = NULL;
	virtual int Close() = NULL;
	virtual int Inport(WORD port, BYTE* value) = NULL;
	virtual int OutPort(WORD port, BYTE value) = NULL;
	virtual int ReadOutport(WORD port, BYTE* value) = NULL;
	virtual int InportWord( WORD* value) { return 0; };
	virtual int OutPortWord( WORD value) { return 0; };
	virtual int ReadOutportWord(WORD* value) { return 0; };

	virtual int InportDevice(char cDevice, WORD port, BYTE* value) { ASSERT(0); return(0); };
	virtual int OutportDevice(char cDevice, WORD port, BYTE value) { ASSERT(0); return(0); };
	virtual int InportDeviceWord(char cDevice, WORD port, WORD* value) { ASSERT(0); return(0); };
	virtual int ReadReg(WORD dev, WORD* data);
	virtual int WriteReg(WORD dev, WORD data);
	virtual int WriteReg_Block(WORD dev, WORD* data, int nNumOfData);
	virtual eIOTYPE GetIoType(void) { return(m_Type); }

	BOOL	IsOpened();
	BOOL	IsDebugMode();
	void	SetInportMap(WORD port, BYTE value);

private:
	static eIOTYPE m_Type;

protected:
	void	ErrorProc(int nRet, char* cap);
	BOOL	IsPortEorror(WORD port);
	BOOL	IsRegistEorror(WORD Regist);
	BOOL	m_bOpened;
	BOOL	m_bDebugMode;

	enum {
		enMaxMapPort = 256,
		enMaxMapReg = 256,
	};
	BYTE	m_OutMap[enMaxMapPort];			// 디버그용
	BYTE	m_InMap[enMaxMapPort];			// 디버그용
	DWORD	m_RegMap[enMaxMapReg];			// 디버그용
	static int		m_nPortMax;				// 사용 포트 수
	static int		m_nRegistMin;			// 최소 레지스터
	static int		m_nRegistMax;			// 최대 레지스터

};

