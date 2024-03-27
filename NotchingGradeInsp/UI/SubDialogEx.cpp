#include "pch.h"
#include "SubDialogEx.h"

IMPLEMENT_DYNAMIC(CSubDialogEx, CDialogEx)

CSubDialogEx::CSubDialogEx()
{

}

CSubDialogEx::CSubDialogEx(UINT nIDTemplate, CWnd *pParent /*= NULL*/)
	: CDialogEx(nIDTemplate, pParent)
{

}

CSubDialogEx::CSubDialogEx(LPCTSTR lpszTemplateName, CWnd *pParent /*= NULL*/)
	: CDialogEx(lpszTemplateName, pParent)
{

}

CSubDialogEx::~CSubDialogEx()
{

}


BEGIN_MESSAGE_MAP(CSubDialogEx, CDialogEx)

END_MESSAGE_MAP()
