#include "pch.h"
#include "GlobalFunc.h"
#include "Win32File.h"

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