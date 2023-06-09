#pragma once

#include <vector>

class CGlobalFunc
{
public:
	CGlobalFunc(void);
	~CGlobalFunc(void);

	//스트링 특정 char로 파서하는 함수
	static std::vector<CString> StringParser(CString val, char s);

	//JSON 파일 생성함수
	static void makeJSONFile(CString FilePath, CString FileName, CString msg);

	//Ini 파일을 읽기
	static CString ReadIniFile(LPCSTR lpAppName, LPCSTR lpKeyName,  LPCSTR lpFileName, LPCSTR lpDefault = "NA");

	//Ini 파일에 쓰기
	static void WriteIniFile(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName);

	//로컬 시간으로 파일 경로 만들기
	static CString makeTimeToPath(CString strTime);

	//로컬 시간  string로 넘긴다.
	static CString strLocalTime();

	//스래드 종료
	static void ThreadExit(HANDLE* hThread, DWORD waitTime);

};
