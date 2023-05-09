#pragma once

#include <vector>

class CGlobalFunc
{
public:
	CGlobalFunc(void);
	~CGlobalFunc(void);

	//스트링 특정 char로 파서하는 함수
	static std::vector<CString> StringParser(CString val, char s);

};
