// EyeTrackerDlg.h : fichier d'en-tête
//

#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "glint.h"
#include "face.h"
#include "eye.h"
#include "TrackingFunctions.h"
#include "afxcmn.h"

// boîte de dialogue CEyeTrackerDlg
class CEyeTrackerDlg : public CDialog
{
// Construction
public:
	CEyeTrackerDlg(CWnd* pParent = NULL);	// constructeur standard

	// Mes fonctions
	bool OpenFile();


// Données de boîte de dialogue
	enum { IDD = IDD_EYETRACKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Prise en charge DDX/DDV


// Implémentation
protected:
	HICON m_hIcon;

	// Fonctions générées de la table des messages
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedbtnopenfile();
	CString m_strMsg;
	afx_msg void OnBnClickedcheckglint();
	afx_msg void OnBnClickedbtnthreshold();
	CString m_txtNbThresh;
	int m_iNbThresholds;
	afx_msg void OnBnClickedbtnshowimg();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedbtnshowvideo();
	CString m_strVideoStats;

// Mon code
	bool bShowVideo;
	bool bMessageShowed;
	bool bThreshold;
	int NbFrames;

	IplImage * iplTmp;
	afx_msg void OnBnClickedbtnfindcircle();
	afx_msg void OnClose();
	int m_txtLeftAvgX;
	int m_iLeftFrames;
	int m_txtRightAvgX;
	int m_iRightNbFrames;
	int m_txtLeftAvgY;
	int m_txtRightAvgY;
	afx_msg void OnBnClickedButton1();
	CSliderCtrl m_Slider;
	int m_SLIDER_VALUE;
};
