// ISpcIf.h : Spc Plus Interface
//
//////////////////////////////////////////////////////////////////////

#if !defined ISPCIF_H
#define ISPCIF_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

Interface ISpcIf
{
	virtual ~ISpcIf() {} ;
	virtual void init() = 0;
};


#endif // ISPCIF_H
