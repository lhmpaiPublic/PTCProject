#include "pch.h"
#include "GlobalFunc.h"
#include "Win32File.h"


bool CGlobalFunc::bSynchTime = false;

CGlobalFunc::CGlobalFunc(void) {}
CGlobalFunc::~CGlobalFunc(void) {}

//스트링 특정 char로 파서하는 함수
std::vector<CString> CGlobalFunc::StringParser(CString val, char s = ',')
{
    std::vector<CString> parserVal;
    parserVal.clear();

    CString strSub;
    int num = 0;
    //파싱하여 vector 에 저장한다.
    while (AfxExtractSubString(strSub, val, num++, s))
    {
        parserVal.push_back(strSub);
    }

    return parserVal;
}

//JSON 파일 생성함수
void CGlobalFunc::makeJSONFile(CString FilePath, CString FileName, CString msg)
{
	CWin32File file;
	file.TextSave1Line(FilePath, FileName, msg, "at", FALSE, 999999999);
}

CString CGlobalFunc::ReadIniFile(LPCSTR lpAppName,	LPCSTR lpKeyName, LPCSTR lpFileName, LPCSTR lpDefault)
{
	char value[MAX_PATH];
	::GetPrivateProfileString(lpAppName, lpKeyName, "NA", value, MAX_PATH, lpFileName);
	return value;
}

void CGlobalFunc::WriteIniFile(LPCSTR lpAppName,	LPCSTR lpKeyName,	LPCSTR lpString,	LPCSTR lpFileName)
{
	::WritePrivateProfileString(lpAppName, lpKeyName, lpString, lpFileName);
}

//로컬 시간으로 파일 경로 만들기
CString CGlobalFunc::makeTimeToPath(CString strTime)
{
	CString strPath = "";
	CString strYYYYMM = "";
	CString strHH = "";
	CString strDD = "";

	if (strTime.GetLength() >= 17)
	{
		// 로컬 타임 Str 생성
		strYYYYMM.Format(_T("%d%02d"), strTime.Mid(0,4), strTime.Mid(4,2));
		strHH.Format(_T("%02d"), strTime.Mid(6, 2));
		strDD.Format(_T("%02d"), strTime.Mid(8, 2));

		//경로 생성
		strPath.Format(_T("%s\\%s\\%s\\"), strYYYYMM, strDD, strHH);
	}
	return strPath;
}

//로컬 시간  string로 넘긴다.
CString CGlobalFunc::strLocalTime()
{
	CString strTime;

	while (true)
	{
		if (CGlobalFunc::bSynchTime == false)
		{
			CGlobalFunc::bSynchTime = true;

			SYSTEMTIME	sysTime;
			::GetLocalTime(&sysTime);

			strTime.Format(_T("%04d%02d%02d%02d%02d%02d%03d")
				, sysTime.wYear
				, sysTime.wMonth
				, sysTime.wDay
				, sysTime.wHour
				, sysTime.wMinute
				, sysTime.wSecond
				, sysTime.wMilliseconds
			);

			CGlobalFunc::bSynchTime = false;

			break;
		}
		else
		{
			Sleep(1);
		}
	}

	return strTime;
}

//스래드 종료
void CGlobalFunc::ThreadExit(HANDLE* hThread, DWORD waitTime)
{
	BOOL bLoopFlag = 1;
	UINT ret = 0;
	while (bLoopFlag)
	{
		MSG msg;
		// PumpMessages()
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		switch (ret = MsgWaitForMultipleObjects(1, hThread, FALSE, waitTime, QS_ALLINPUT))
		{
		case WAIT_OBJECT_0:
		case WAIT_FAILED:
			bLoopFlag = 0;
			break;
		case WAIT_TIMEOUT:
			//  TIMEOUT 처리
		case WAIT_OBJECT_0 + 1:
		default:
			// PumpMessages()
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}

//int to CString
CString CGlobalFunc::intToString(int  val)
{
	CString rVal;
	rVal.Format(_T("%d"), val);
	return rVal;
}

//CString to int
int CGlobalFunc::StringToint(CString val) 
{
	int rVal;
	rVal = _ttoi(val);
	return rVal;
}

//float to CString
CString CGlobalFunc::floatToString(double  val, CString format)
{
	CString rVal;
	rVal.Format(format, val);
	return rVal;
}

//CString to float
double CGlobalFunc::StringTofloat(CString val)
{
	double rVal;
	rVal = _ttof(val);
	return rVal;
}


//스래드 메시지 큐 확인용 함수
WORD CGlobalFunc::isPeekMessage(HWND hWnd)
{
	WORD msgval = WM_NULL;
	MSG msg;
	// PumpMessages()
	if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
	{
		msgval = msg.message;
		if (msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msgval;
}

//Tac Time 계산 식
double CGlobalFunc::GetDiffTime(LARGE_INTEGER stTime, double dFrequency)
{
	LARGE_INTEGER edTime;
	QueryPerformanceCounter(&edTime);

	double	dv0, dv1;
	dv0 = (double)stTime.LowPart + ((double)stTime.HighPart * (double)0xffffffff);
	dv1 = (double)edTime.LowPart + ((double)edTime.HighPart * (double)0xffffffff);
	double	dtimev;
	dtimev = (dv1 - dv0) / dFrequency * (double)1000.0;
	return (dtimev);
}