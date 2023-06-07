// SpcHeader.h : CSpcHeader 클래스
//
//////////////////////////////////////////////////////////////////////

#if !defined SPCHEADER_H
#define SPCHEADER_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpcPlusManager;
class CSpcRefDs;
class CSpcHeader : public CSpcInData
{
public:
	CSpcHeader(CSpcPlusManager* sMgr = NULL) ;
	virtual ~CSpcHeader() ;

private:
	//관리 클래스 객체 포인터
	CSpcPlusManager* manager;

	//Message 고유 번호	
	CString	m_TxnId;
	//In Dataset(송신메시지)	
	CString	m_InDtName;
	//Out Dataset (응답메시지)	
	CString	m_OutDtName;
	//Action ID (Message Name)	
	CString	m_ActId;
	
public:
	static char* MakeHeaderText_1;
	static char* MakeHeaderText_2;

	//Message 고유 번호	
	void setTxnId(CString	TxnId) { m_TxnId = TxnId; }
	//In Dataset(송신메시지)	
	void setInDtName(CString	InDtName) { m_InDtName = InDtName; }
	//Out Dataset (응답메시지)	
	void setOutDtName(CString	OutDtName) { m_OutDtName = OutDtName; }
	//Action ID (Message Name)	
	void setActId(CString	ActId) { m_ActId = ActId; }

	//JSON 형식의 텍스트를 만든다.
	CString makeJSONText_Header();
	CString getJSONText_HeaderTail();
};


#endif // SPCHEADER_H
