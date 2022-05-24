// EyeTrackerDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "EyeTracker.h"
#include "EyeTrackerDlg.h"
#include ".\eyetrackerdlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID 1
// Variables globales
CvCapture * capture;
IplImage * iplFrame;
CString strSysMsg;
CTrackingFunctions trckFunctions;

// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

// Implémentation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// boîte de dialogue CEyeTrackerDlg



CEyeTrackerDlg::CEyeTrackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEyeTrackerDlg::IDD, pParent)
	, m_strMsg(_T(""))
	, m_txtNbThresh(_T(""))
	, m_iNbThresholds(0)
	, m_strVideoStats(_T(""))
	, m_txtLeftAvgX(0)
	, m_iLeftFrames(0)
	, m_txtRightAvgX(0)
	, m_iRightNbFrames(0)
	, m_txtLeftAvgY(0)
	, m_txtRightAvgY(0)
	, m_SLIDER_VALUE(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEyeTrackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_txtMsg, m_strMsg);
	DDX_Text(pDX, IDC_txtThresholds, m_iNbThresholds);
	DDV_MinMaxInt(pDX, m_iNbThresholds, 1, 10);
	DDX_Text(pDX, IDC_lblIVideoStatus, m_strVideoStats);
	DDX_Text(pDX, IDC_txtLeftAvg, m_txtLeftAvgX);
	DDX_Text(pDX, IDC_txtLeftNbFrames, m_iLeftFrames);
	DDX_Text(pDX, IDC_txtRightAvg, m_txtRightAvgX);
	DDX_Text(pDX, IDC_txtRightNbFrames, m_iRightNbFrames);
	DDX_Text(pDX, IDC_txtLeftAvgY, m_txtLeftAvgY);
	DDX_Text(pDX, IDC_txtRightAvgY, m_txtRightAvgY);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD, m_Slider);
	DDX_Text(pDX, IDC_TXT_TEST, m_SLIDER_VALUE);
}

BEGIN_MESSAGE_MAP(CEyeTrackerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_btnOpenFile, OnBnClickedbtnopenfile)
	ON_BN_CLICKED(IDC_checkGlint, OnBnClickedcheckglint)
	ON_BN_CLICKED(IDC_btnThreshold, OnBnClickedbtnthreshold)
	ON_BN_CLICKED(IDC_btnShowImg, OnBnClickedbtnshowimg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_btnShowVideo, OnBnClickedbtnshowvideo)
	ON_BN_CLICKED(IDC_btnFindCircle, OnBnClickedbtnfindcircle)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// gestionnaires de messages pour CEyeTrackerDlg

BOOL CEyeTrackerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Ajouter l'élément de menu "À propos de..." au menu Système.

	// IDM_ABOUTBOX doit se trouver dans la plage des commandes système.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Définir l'icône de cette boîte de dialogue. L'infrastructure effectue cela automatiquement
	//  lorsque la fenêtre principale de l'application n'est pas une boîte de dialogue
	SetIcon(m_hIcon, TRUE);			// Définir une grande icône
	SetIcon(m_hIcon, FALSE);		// Définir une petite icône

	// TODO : ajoutez ici une initialisation supplémentaire
	SetTimer(TIMER_ID, 33, NULL);
	//SetTimer(TIMER_ID, 500, NULL);
	bShowVideo = false;
	bMessageShowed = false;
	bThreshold = false;
	
	m_Slider.SetRangeMin(1, false);

	
	return TRUE;  // retourne TRUE, sauf si vous avez défini le focus sur un contrôle
}

void CEyeTrackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Si vous ajoutez un bouton Réduire à votre boîte de dialogue, vous devez utiliser le code ci-dessous
//  pour dessiner l'icône. Pour les applications MFC utilisant le modèle Document/Vue,
//  cela est fait automatiquement par l'infrastructure.

void CEyeTrackerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // contexte de périphérique pour la peinture

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrer l'icône dans le rectangle client
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dessiner l'icône
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Le système appelle cette fonction pour obtenir le curseur à afficher lorsque l'utilisateur fait glisser
//  la fenêtre réduite.
HCURSOR CEyeTrackerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEyeTrackerDlg::OnBnClickedbtnopenfile()
{
	if (!OpenFile())
	{
		m_strMsg = "Error loading file!";
	}

}

// Procédure permettant d'ouvrir une image ou un vidéo.  Le fichier est chargé
// dans la variable globale capture pour un vidéo ou iplFrame pour une image
bool CEyeTrackerDlg::OpenFile()
{
    
	bool bRet = false;
	CString strExtension;

	char * strFilter = {"AVI file (*.avi)|*.avi|JPEG file (*.jpg)|*.jpg|BMP File (*.bmp)|*.bmp|All files (*.*)|*.*||"};

	CFileDialog FileDlg(true, ".avi", NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		bRet = true;

		strExtension = FileDlg.GetFileExt();

		if (strExtension.MakeLower() == "avi")
		{
			if (capture != NULL)
				cvReleaseCapture(&capture);
			capture = cvCaptureFromFile(FileDlg.GetFileName());
			NbFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
			CGlint::LeftCentre = cvPoint(0, 0);
			CGlint::RightCentre = cvPoint(0, 0);
			CGlint::LeftAverage = cvPoint(0, 0);
			CGlint::RightAverage = cvPoint(0, 0);
		}
		else
		{
			
			iplFrame = cvLoadImage(FileDlg.GetFileName(), 0);
		}

		if (capture == NULL || iplFrame == NULL)
		{
			bRet = false;
		}
	}
	else
	{
		bRet = false;
	}
	
	
	return bRet;
}

void CEyeTrackerDlg::OnBnClickedcheckglint()
{
	CGlint glint;
	int decimal, sign;

	glint.FindInImage(iplFrame);

	//m_strMsg = _ecvt(glint.dLastQuanta , 15, &decimal, &sign);
	itoa(glint.iNbWhitePoint, (char *)(LPCTSTR)m_strMsg, 10);


	//dessiner la croix dans l'image originale
	cvCircle(iplFrame, CGlint::LeftCentre, 5, CV_RGB(255, 255, 255), 1, 8, 0);
	cvCircle(iplFrame, CGlint::RightCentre, 5, CV_RGB(255, 255, 255), 1, 8, 0);

	cvNamedWindow("TestOrig", CV_WINDOW_AUTOSIZE);
	cvShowImage("TestOrig",iplFrame);
	
	UpdateData(false);	
}

void CEyeTrackerDlg::OnBnClickedbtnthreshold()
{
	if (iplFrame == NULL)
		return;

	bThreshold = !bThreshold;

	iplTmp = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 
		iplFrame->nChannels);

	if (bThreshold)
        cvNamedWindow("Threshold", CV_WINDOW_AUTOSIZE);
}

void CEyeTrackerDlg::OnBnClickedbtnshowimg()
{
	if (iplFrame != NULL)
	{
		
		cvNamedWindow("CurrentImage", CV_WINDOW_AUTOSIZE);
		cvShowImage("CurrentImage", iplFrame);
	}
	
}

void CEyeTrackerDlg::OnTimer(UINT nIDEvent)
{
	// TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels
	if (bShowVideo)
	{
		if (capture != NULL)
		{
			CGlint glint;

			iplFrame = cvQueryFrame(capture);

			glint.FindInImage(iplFrame);
				
			cvCircle(iplFrame, CGlint::LeftCentre, 5, CV_RGB(255, 255, 255), 1, 8, 0);
			cvCircle(iplFrame, CGlint::RightCentre, 5, CV_RGB(255, 255, 255), 1, 8, 0);

			cvShowImage("Video", iplFrame);

			if (cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) >= NbFrames)
			{
				cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);  // SetPremierFrame
				glint.ResetFrameCount();
			}


			if (bThreshold)
			{
				cvThreshold(iplFrame, iplTmp, 200, 255, CV_THRESH_BINARY);
				cvShowImage("Threshold", iplTmp);
			}

			m_iLeftFrames = CGlint::iNbFrameLeft;
			m_iRightNbFrames = CGlint::iNbFrameRight;

			m_txtLeftAvgX = CGlint::LeftAverage.x;
			m_txtLeftAvgY = CGlint::LeftAverage.y;

			m_txtRightAvgX = CGlint::RightAverage.x;
			m_txtRightAvgY = CGlint::RightAverage.y;

			UpdateData(false);									
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CEyeTrackerDlg::OnBnClickedbtnshowvideo()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	if (capture == NULL)
	{
		AfxMessageBox("No video loaded!");
		return;
	}

	bShowVideo = !bShowVideo;

	if (bShowVideo)
	{
		m_strVideoStats = "Playing!";
	}
	else
	{
		m_strVideoStats = "Stopped!";
	}

	cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
	UpdateData(false);
	
}

void CEyeTrackerDlg::OnBnClickedbtnfindcircle()
{
	IplImage * gray = cvCreateImage(cvGetSize(iplFrame), 8, 1);
	cvZero(gray);

	if (iplFrame->nChannels > 1)
		cvCvtColor( iplFrame, gray, CV_BGR2GRAY );
	else
		cvCopy(iplFrame, gray);
	
	CvMemStorage* storage = cvCreateMemStorage(0);
    cvSmooth( gray, gray, CV_GAUSSIAN, 9, 9 ); // smooth it, otherwise a lot of false circles may be detected
    CvSeq* circles = cvHoughCircles( gray, storage, CV_HOUGH_GRADIENT, 2, gray->height/4, 200, 100 );
    
    for(int i = 0; i < circles->total; i++ )
    {
            float* p = (float*)cvGetSeqElem( circles, i );
            cvCircle( gray, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), CV_RGB(255,255,255), 3, 8, 0 );
    }

	cvNamedWindow( "circles", CV_WINDOW_AUTOSIZE );
    cvShowImage( "circles", gray);
}

void CEyeTrackerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	cvDestroyAllWindows();

	CDialog::OnClose();
}





