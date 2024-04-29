#include "pch.h"
#include <sstream>
//--------------------------------------------------------------
//|
//|생성자
//|
//---------------------------------------------------------------
CStrSuport::CStrSuport(void)
{
}
//--------------------------------------------------------------
//|
//|소멸자
//|
//---------------------------------------------------------------
CStrSuport::~CStrSuport(void)
{
}
//--------------------------------------------------------------
//|
//|숫자를 문자로
//|
//---------------------------------------------------------------
inline void CStrSuport::NtoA(int &n) 
{ 
	n |= 0x30; 
} 

//--------------------------------------------------------------
//|
//|대문자 소문자로
//|
//---------------------------------------------------------------
inline void CStrSuport::UtoL(char &c) 
{ 
	c |= 0x20; 
} 
//--------------------------------------------------------------
//|
//|소문자 대문자로
//|
//---------------------------------------------------------------
inline void CStrSuport::LtoU(char &c) 
{ 
	c &= 0xDF; // ~(0x20) 
} 
//--------------------------------------------------------------
//|
//|string Unicode로 변환
//|
//---------------------------------------------------------------
std::wstring CStrSuport::AnsiToUnicode(const std::string& s)
{
	std::wstring ret(s.length(),' ');
	std::copy(s.begin(), s.end(), ret.begin());
	return ret;
}
//--------------------------------------------------------------
//|
//|wstring strng로 변환
//|
//---------------------------------------------------------------
std::string CStrSuport::wstringTostring(const std::wstring& strSrc)
{
	char* szRet = NULL;
	int len = ::WideCharToMultiByte(CP_ACP, 0, strSrc.c_str(), -1, szRet, 0, NULL, NULL);
	if(0 < len)
	{
		szRet = new char[len];
		::ZeroMemory(szRet, len);
		::WideCharToMultiByte(CP_ACP, 0, strSrc.c_str(), -1, szRet, len, NULL, NULL);
		std::string strRet(szRet);
		delete [] szRet;
		return strRet;
	}
	return std::string(""); 
}
//--------------------------------------------------------------
//|
//|BSTR => CString 로 변환
//|
//---------------------------------------------------------------
CString CStrSuport::bstrToCString(BSTR bstr)   
{   
	char* pbstr;   
	USES_CONVERSION; //convert를 위한 매크로   
	pbstr=OLE2A(bstr); //bstr을 char로 convert   
	return CString(pbstr);   
}
//--------------------------------------------------------------
//|
//|char를 hex로 변환
//|
//---------------------------------------------------------------
CString CStrSuport::Changcharbyhex(CString pass)
{
	CString retvalue="";
	CString temp;
	char *a = pass.GetBuffer(0);
	for( int i = 0; i < pass.GetLength(); i++ )
	{
		temp.Format("%02X", a[i]);
		retvalue += temp;
	}
	return retvalue;
}
//--------------------------------------------------------------
//|
//|BYTE를 hex로 변환
//|
//---------------------------------------------------------------
CString CStrSuport::Changbytetohex(BYTE* byt, int len)
{
	CString temp;
	CString rest="";
	for( int i = 0; i < (int)len; i++ )
	{
		temp.Format("%02X", byt[i]);
		rest += temp;
	}
	if(len==(rest.GetLength()/2))
		return rest;
	return "";
}

CString CStrSuport::ChangbytetohexMelsec(BYTE* byt, int len, int offset, int tablen)
{
	CString temp;
	CString rest = "";
	for (int i = 0; i < (int)len; i++)
	{
		if ((i + 1) % tablen == 1)
		{
			temp.Format("0x%02X : ", (i + offset));
			rest += temp;
		}
		temp.Format("%02X", byt[i]);
		rest += temp;

		if ((i + 1) % tablen == 0)
		{
			rest += "	";
		}
		else
		{
			rest += " ";
		}
	}
	return rest;
}

CString CStrSuport::ChangshorttohexSiemens(short* data, int len, int offset, int tablen)
{
	CString temp;
	CString rest = "";
	for (int i = 0; i < (int)len; i++)
	{
		if ((i+1) % tablen == 1)
		{
			temp.Format("0x%02X : ", (i + offset));
			rest += temp;
		}
		temp.Format("%02X", ((data[i])>>8)&0xff);
		rest += temp;
		temp.Format("%02X", (data[i]) & 0xff);
		rest += temp;

		if ((i + 1) % tablen == 0)
		{
			rest += "	";
		}
		else
		{
			rest += " ";
		}
	}
	return rest;
}

CString CStrSuport::ChanginttohexMelsec(int* data, int len, int offset, int tablen)
{
	CString temp;
	CString rest = "";
	for (int i = 0; i < (int)len; i++)
	{
		if ((i + 1) % tablen == 1)
		{
			temp.Format("0x%02X : ", ((i * sizeof(int)) + offset));
			rest += temp;
		}
		temp.Format("%02X", ((data[i]) >> 24) & 0xff);
		rest += temp;
		temp.Format("%02X", ((data[i]) >> 16) & 0xff);
		rest += temp;
		temp.Format("%02X", ((data[i]) >> 8) & 0xff);
		rest += temp;
		temp.Format("%02X", (data[i]) & 0xff);
		rest += temp;

		if ((i + 1) % tablen == 0)
		{
			rest += "	";
		}
		else
		{
			rest += " ";
		}
	}
	return rest;
}

//--------------------------------------------------------------
//|
//|Hex스트링을값을 문자스트링값으로 변환한다(예 "303041413961"=>"00AA9a"
//|
//---------------------------------------------------------------
CString CStrSuport::HexStringToCharString(CString st)
{
	CString revel="";
	for(int i=0; i < st.GetLength() ; i += 2)
	{
		CString str = "0x";
		char* stop;
		char cha[2];
		str += st.Mid(i,2);
		BYTE ch = (unsigned char)strtol(str, &stop, 16);
		sprintf_s(cha, sizeof(cha), "%c", ch);
		revel += cha;
	}
	return revel;
}
//--------------------------------------------------------------
//|
//|Hex스트링을값을 문자스트링값으로 변환한다(예 "303041413961"=>"00AA9a") bk가 설정한 문자까지 읽는다
//|
//---------------------------------------------------------------
int CStrSuport::HexStringToCharString(CString st,CString& revel,CString bk)
{
	revel="";
	int i;
	for(i=0; i < st.GetLength() ; i += 2)
	{
		CString str = "0x";
		char* stop;
		char cha[2];
		str += st.Mid(i,2);
		if(str==bk)
			break;
		BYTE ch = (unsigned char)strtol(str, &stop, 16);
		sprintf_s(cha, sizeof(cha), "%c", ch);
		revel += cha;
	}
	return i;
}
//--------------------------------------------------------------
//|
//|스크링을 1바이트 문자를 2바이트 HEX로 변환한다
//|
//---------------------------------------------------------------
CString CStrSuport::Chang1Byteby2Hex(CString pass)
{
	CString retvalue="";
	CString temp;
	BYTE *a = (BYTE*)pass.GetBuffer(0);
	for( int i = 0; i < pass.GetLength(); i++ )
	{
		temp.Format("%02X", a[i]);
		retvalue += temp;
	}
	return retvalue;
}
//--------------------------------------------------------------
//|
//|스트링 구분자로 스트링을 분류하여 int로 넘긴다 vector
//|
//---------------------------------------------------------------
void CStrSuport::Tokenize(const tstring& str, std::vector<int>& tokens, const tstring& delimiters)
{
	//맨 첫 글자가 구분자인 경우 무시
	tstring::size_type lastPos = str.find_first_not_of(delimiters,0);
	tstring::size_type pos = str.find_first_of(delimiters, lastPos);
	while(tstring::npos!=pos||tstring::npos!=lastPos)
	{
		tokens.push_back(atoi((str.substr(lastPos, pos-lastPos)).c_str()));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos=str.find_first_of(delimiters, lastPos);
	}
}
void CStrSuport::Tokenize(const tstring& str, std::vector<short>& tokens, const tstring& delimiters)
{
	//맨 첫 글자가 구분자인 경우 무시
	tstring::size_type lastPos = str.find_first_not_of(delimiters,0);
	tstring::size_type pos = str.find_first_of(delimiters, lastPos);
	while(tstring::npos!=pos||tstring::npos!=lastPos)
	{
		tokens.push_back(atoi((str.substr(lastPos, pos-lastPos)).c_str()));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos=str.find_first_of(delimiters, lastPos);
	}
}
//--------------------------------------------------------------
//|
//|스트링 구분자로 스트링을 분류하여 CString로 넘긴다 vector
//|
//---------------------------------------------------------------
void CStrSuport::Tokenize(const tstring& str, std::vector<CString>& tokens, const tstring& delimiters)
{
	//맨 첫 글자가 구분자인 경우 무시
	tstring::size_type lastPos = str.find_first_not_of(delimiters,0);
	tstring::size_type pos = str.find_first_of(delimiters, lastPos);
	while(tstring::npos!=pos||tstring::npos!=lastPos)
	{
		tokens.push_back((str.substr(lastPos, pos-lastPos)).c_str());
		lastPos = str.find_first_not_of(delimiters, pos);
		pos=str.find_first_of(delimiters, lastPos);
	}
}
//--------------------------------------------------------------
//|
//|stringstream으로 토큰 구분
//|
//---------------------------------------------------------------
void CStrSuport::Tokenize(const tstring& str, std::vector<tstring>& tokens)
{
	string buf; // 버퍼 string
	stringstream ss(str); // string을 stream에 넣는다
	while (ss >> buf)
		tokens.push_back(buf);
}
//--------------------------------------------------------------
//|
//|
//|
//---------------------------------------------------------------
BOOL CStrSuport::DeleteVector(vector<CString>& str ,int  index)
{
	str.erase (str.begin()+index);
	return TRUE;
}
//--------------------------------------------------------------
//|
//|문자열 중에서 특정 문자열을 찾아 본다
//|
//---------------------------------------------------------------
BOOL CStrSuport::findstring(const char* str ,const char*  fstr)
{
	vector<char> _chVec(str, str + strlen(str));
	vector<char> _CchVec(fstr, fstr + strlen(fstr));

	vector<char>::iterator ppnt;
	ppnt = search(_chVec.begin(), _chVec.end(), _CchVec.begin(), _CchVec.end());						// Search Algorithm
	if ( ppnt == _chVec.end())
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CStrSuport::findCString(CString str ,CString  fstr)
{
	// Search Algorithm
	if (str.Find(fstr)==-1)
	{
		return FALSE;
	}
	return TRUE;
}
LPCTSTR CStrSuport::FindChars(LPCTSTR p1, LPCTSTR p2)
{
	while (*p1 != NULL)
	{
		LPCTSTR p = p2;
		while (*p != NULL)
		{
			if (*p1 == *p)
				return CharNext(p1);
			p = CharNext(p);
		}
		p1++;
	}
	return NULL;
}

static char hexChar[] = {
	0x30 , 0x31 , 0x32 , 0x33 ,
	0x34 , 0x35 , 0x36 , 0x37 ,
	0x38 , 0x39 , 0x41 , 0x42 ,
	0x43 , 0x44 , 0x45 , 0x46 
};
CString CStrSuport::byteToHexbyteValue(byte* data, int len) 
{
	CString hex="";
	for (int i = 0, h = 0; i < len; i++, h+=2) {
		hex += hexChar[(data[i]&0xF0)>>4];
		hex += hexChar[(data[i]&0x0F)];
	}
	return hex;
}
//--------------------------------------------------------------
//|
//|스트링2바이트의값을 BYTE핵사 값으로 변환한다
//|
//---------------------------------------------------------------
int  CStrSuport::ChangeHexCord(CString data, BYTE* hex, int maxlen)
{
	memset(hex, 0, maxlen);
	int dwtransLen=data.GetLength()/2;
	for(int i = 0 ; i < dwtransLen ; i++ )
	{
		hex[i] = (((BYTE)data.GetAt(i*2)) & 0x0F) << 4;

		if( (BYTE)data.GetAt(i*2) > 0x39 )
			hex[i] += 0x90;

		hex[i] |= ((BYTE)data.GetAt(i*2+1)) & 0x0F;

		if( (BYTE)data.GetAt(i*2+1) > 0x39 )
			hex[i] += 0x09;
	}
	return dwtransLen;
}