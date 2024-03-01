#pragma once

#include "ThumbnailCtrl.h"

// CCropImageViewDlg 대화 상자

class CNotchingGradeInspDoc;
class CNotchingGradeInspView;


class CCropImageViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCropImageViewDlg)

public:
	CCropImageViewDlg(CWnd* pParent = nullptr, CNotchingGradeInspView* pView = nullptr);   // 표준 생성자입니다.
	virtual ~CCropImageViewDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CROP_IMAGE_VIEW };
#endif

public:
	void SetCropViewTimer();
	void KillCropViewTimer();

	void ClearAllCropImage();
private :
	CImageList m_ImgList;
	CThumbnailCtrl m_CropThumbCtrl;
	CStringList m_strImageList;
	//KANG 22.07.08 Add Start
	CExpandViewImage* m_pExpandView;
	//KANG 22.07.08 Add End

protected:
	CWnd* m_pParent;
	CNotchingGradeInspDoc* m_pDoc;
	CNotchingGradeInspView* m_pView;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelectImage(NMHDR* pNMHDR, LRESULT* pResult);	 // 22.07.01 Ahn Add 
	afx_msg void OnChangeDispExpand(NMHDR* pNMHDR, LRESULT* pResult);	 //KANG 22.07.08 Add
	afx_msg void OnChangeDispThumbnail(NMHDR* pNMHDR, LRESULT* pResult); //KANG 22.07.08 Add
	//Crop Image를 그리기 위한 메시지 처리
	afx_msg LRESULT OnCropImageDraw(WPARAM wParam = 0, LPARAM lParam = 0);
};
