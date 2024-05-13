#pragma once

class CSubDialogEx : public CDialogEx
{
	DECLARE_DYNAMIC(CSubDialogEx)

public:
	//생성자
	CSubDialogEx();
	CSubDialogEx(UINT nIDTemplate, CWnd *pParent = NULL);
	CSubDialogEx(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL);
	// 소멸자
	virtual ~CSubDialogEx();

protected:

	DECLARE_MESSAGE_MAP()

};
