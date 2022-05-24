// VDOFXDlg.h : fichier d'en-tête
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"


// boîte de dialogue CVDOFXDlg
class CVDOFXDlg : public CDialog
{
// Construction
public:
	CVDOFXDlg(CWnd* pParent = NULL);	// constructeur standard

// Données de boîte de dialogue
	enum { IDD = IDD_VDOFX_DIALOG };


	CvCapture * capture;
	IplImage * iplFrame;
	IplImage * iplPrevious;
	IplImage * iplSub;
	IplImage * iplTemp;

	CString strSysMsg;

	int NbFrames;
	bool bShowVideo;	
	int iThreshold;
	long lCounter;
	long lCounterMultiplier;
	int iHalfHeight;

	bool bInit;	// Flag for initialisation

	bool OpenFile();

    uchar getPixel( IplImage*img, int lin, int col, int channel);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Prise en charge DDX/DDV


// Implémentation
protected:
	HICON m_hIcon;

	// Fonctions générées de la table des messages
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_ValueSlider;
	CEdit m_txtValue;
	int m_iValue;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnopen();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButtonshow();
	BOOL m_bEnableVale;
};
