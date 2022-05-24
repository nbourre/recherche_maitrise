// VDOFXDlg.h : fichier d'en-tête
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "eyes.h"
#include "head.h"
#include "calibration.h"

#include "blobresult.h"


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
    IplImage * iplFrameGray;
	IplImage * iplPrevious;
	IplImage * iplSub;
	IplImage * iplTemp;
    IplImage * iplGray;
	IplImage * iplSample;

    
	CHead _head;
	CEyes _eyes;
    CCalibration _calibration;
	

	CString strSysMsg;

	int NbFrames;
	int iHalfHeight;
    int iNeckHeight;
	int iThreshold;
	int MOVEMENT_THRESHOLD_AFTERINIT;

	long lCounter;
	long lCounterMultiplier;

	bool bShowVideo;	
	bool bInit;	// Flag for initialisation
	bool bEnableThreshold;	
	bool blLoaded;
	bool blSourceCamera;
    	
	bool OpenFile();
	uchar getPixel( IplImage*img, int lin, int col, int channel);
	void drawMovementBox (IplImage *iplView, CHead theHead);
	CvMat * getSubImage(IplImage * iplInput);
	void opening(IplImage *img, int shape);
	void initializeIpl();
	void initCamera();
	bool OpenCamera();


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
	afx_msg void OnBnClickedCheckvalue();
	afx_msg void OnBnClickedBtntest();
	// If checker, verify for camera
	BOOL m_bUseCamera;
};
