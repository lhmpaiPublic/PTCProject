// SpcInData.h : CSpcInData 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCINDATA_H
#define SPCINDATA_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcInData
{
public:
	CSpcInData() ;
	virtual ~CSpcInData() ;

	//JSON 형식의 텍스트 생성
	virtual void makeJSON() {}

};


#endif // SPCINDATA_H
