#pragma once


// CCameraWaveDlg 대화 상자
//class CGraphCtrl;
#include "GraphCtrl.h"

#define T_ID_CAMERA_WAVE 105

class CCameraWaveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraWaveDlg)

public:
	CCameraWaveDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCameraWaveDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CAM_WAVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


private :
	CGraphCtrl m_WaveGraph;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	void SetWaveData(BYTE* pWaveData, int nSize);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedChkWaveRun();
	afx_msg void OnBnClickedChkTrgMode();
	BOOL m_bChkTrgMode;
	BOOL m_bChkWaveRun;
	int m_nRadSelCamNo;
	afx_msg void OnBnClickedRadCam01();
};
