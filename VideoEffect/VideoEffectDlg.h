// VideoEffectDlg.h : fichier d'en-tête
//

#pragma once
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include "afxcmn.h"


// boîte de dialogue CVideoEffectDlg
class CVideoEffectDlg : public CDialog
{
// Construction
public:
	CVideoEffectDlg(CWnd* pParent = NULL);	// constructeur standard

	CvCapture * capture;
	IplImage * iplFrame;
	CString strSysMsg;
	int NbFrames;
	bool bShowVideo;
	int iThreshold;

	bool OpenFile();
	

// Données de boîte de dialogue
	enum { IDD = IDD_VIDEOEFFECT_DIALOG };

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
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButtonShow();
protected:
	CSliderCtrl m_ValueSlider;
};
