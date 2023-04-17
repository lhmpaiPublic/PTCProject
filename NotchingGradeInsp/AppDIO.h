#pragma once
class CAppDIO
{
public:
	CAppDIO(void);
	~CAppDIO(void);
public:
	enum {
		eIn_TRIGGER			= 0x00000001,
		eIn_BCD_ID_01		= 0x00000002,
		eIn_BCD_ID_02		= 0x00000004,
		eIn_BCD_ID_04		= 0x00000008,
		eIn_BCD_ID_08		= 0x00000010,
		eIn_BCD_ID_16		= 0x00000020,
		eIn_BCD_ID_32		= 0x00000040,
		eIn_BCD_ID_64		= 0x00000080,
		eIn_08				= 0x00000100,
		eIn_09				= 0x00000200,
		eIn_10				= 0x00000400,
		eIn_11				= 0x00000800,
		eIn_12				= 0x00001000,
		eIn_13				= 0x00002000,
		eIn_14				= 0x00004000,
		eIn_15				= 0x00008000,

		eInIn_ID_Mask		= 0x3f,

	};

	enum {
		eOut_TAB_JUDGE_SURFACE	= 0x00000001,  // 0
		eOut_TAB_JUDGE_SIZE		= 0x00000002,
		eOut_BCD_ID_01			= 0x00000004,
		eOut_BCD_ID_02			= 0x00000008,
		eOut_BCD_ID_04			= 0x00000010,
		eOut_BCD_ID_08			= 0x00000020,
		eOut_BCD_ID_16			= 0x00000040,	// 출력
		eOut_BCD_ID_32			= 0x00000080,
		eOut_08					= 0x00000100,	// 8		
		eOut_PULSE = 0x00000200,   // 9
		eOut_MARK_SEL_01 = 0x00000400,   // 10   // 0 1 : mark 출력 Tab이 없을 때 00
		eOut_MARK_SEL_02 = 0x00000800,   // 11   // 1 1 : 3 Tab 출력
		eOut_12					= 0x00001000,
		eOut_13					= 0x00002000,
		eOut_14					= 0x00004000,
		eOut_15					= 0x00008000,

		// 
		eInOut_ID_Mask			= 0xfc,
	};

private:
	static CCriticalSection	m_cs;

public:
	BOOL	IsDioDebugMode();
	int		InputBit(DWORD bitpat, BOOL* bit);
	int		OutputBit(DWORD bitpat, BOOL bit);
	int		InputByte(DWORD bitpat, BYTE* data);

	int		OutputWord(WORD outValue);
	int		InputWord(WORD* outValue);

};

