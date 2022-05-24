// VideoEffectDlg.cpp : fichier d'impl�mentation
//

#include "stdafx.h"
#include "VideoEffect.h"
#include "VideoEffectDlg.h"
#include ".\videoeffectdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID 1


// bo�te de dialogue CAboutDlg utilis�e pour la bo�te de dialogue '� propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Donn�es de bo�te de dialogue
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

// Impl�mentation
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


// bo�te de dialogue CVideoEffectDlg



CVideoEffectDlg::CVideoEffectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoEffectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoEffectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CVideoEffectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SHOW, OnBnClickedButtonShow)
END_MESSAGE_MAP()


// gestionnaires de messages pour CVideoEffectDlg

BOOL CVideoEffectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Ajouter l'�l�ment de menu "� propos de..." au menu Syst�me.

	// IDM_ABOUTBOX doit se trouver dans la plage des commandes syst�me.
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

	// D�finir l'ic�ne de cette bo�te de dialogue. L'infrastructure effectue cela automatiquement
	//  lorsque la fen�tre principale de l'application n'est pas une bo�te de dialogue
	SetIcon(m_hIcon, TRUE);			// D�finir une grande ic�ne
	SetIcon(m_hIcon, FALSE);		// D�finir une petite ic�ne

	// TODO : ajoutez ici une initialisation suppl�mentaire

	SetTimer(TIMER_ID, 33, NULL);
	bShowVideo = false;
	
	return TRUE;  // retourne TRUE, sauf si vous avez d�fini le focus sur un contr�le
}

void CVideoEffectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// Si vous ajoutez un bouton R�duire � votre bo�te de dialogue, vous devez utiliser le code ci-dessous
//  pour dessiner l'ic�ne. Pour les applications MFC utilisant le mod�le Document/Vue,
//  cela est fait automatiquement par l'infrastructure.

void CVideoEffectDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // contexte de p�riph�rique pour la peinture

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrer l'ic�ne dans le rectangle client
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dessiner l'ic�ne
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		
		CDialog::OnPaint();
	}
}

// Le syst�me appelle cette fonction pour obtenir le curseur � afficher lorsque l'utilisateur fait glisser
//  la fen�tre r�duite.
HCURSOR CVideoEffectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVideoEffectDlg::OnBnClickedButtonOpen()
{
	// TODO: Add your control notification handler code here
	if (!OpenFile())
	{
		AfxMessageBox("Could not open file!");
	}
}

bool CVideoEffectDlg::OpenFile()
{
	bool bRet = false;
	CString strExtension;

	char * strFilter = {"AVI file (*.avi)|*.avi|JPEG file (*.jpg)|*.jpg|BMP File (*.bmp)|*.bmp|All files (*.*)|*.*||"};

	CFileDialog FileDlg(true, ".avi", NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		bRet = true;
		bShowVideo = false;
		strExtension = FileDlg.GetFileExt();

		if (strExtension.MakeLower() == "avi")
		{
			capture = cvCaptureFromFile(FileDlg.GetFileName());
			NbFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
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

void CVideoEffectDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	cvDestroyAllWindows();

	CDialog::OnClose();
}

void CVideoEffectDlg::OnTimer(UINT nIDEvent)
{
	if (bShowVideo)
	{
		if (capture != NULL)
		{
			iplFrame = cvQueryFrame(capture);

			cvShowImage("Video", iplFrame);

			if (cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) >= NbFrames)
			{
				cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);  // SetPremierFrame
			}
		}

	}


	CDialog::OnTimer(nIDEvent);
}

void CVideoEffectDlg::OnBnClickedButtonShow()
{
	if (!bShowVideo)
	{
		
		cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
		//cvCreateTrackbar("Slider", "Video", &iThreshold, 255, CVideoEffectDlg::Threshold); 

	}
	else
	{
		cvDestroyWindow("Video");
	}
	
	bShowVideo = !bShowVideo;
	UpdateData(false);
}




