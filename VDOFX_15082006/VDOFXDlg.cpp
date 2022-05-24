// VDOFXDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "VDOFX.h"
#include "VDOFXDlg.h"
#include ".\vdofxdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// boîte de dialogue CVDOFXDlg
#define TIMER_ID 1
#define NB_FRAME_INIT 200
#define COUNTER_TURN 100000
#define MOVEMENT_THRESHOLD 15


const char *szMainWindow = "Video";
const char *szSampleWindow = "Sample";



CVDOFXDlg::CVDOFXDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVDOFXDlg::IDD, pParent)
	, m_iValue(0)
	, m_bEnableVale(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVDOFXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDERVALUE, m_ValueSlider);
	DDX_Control(pDX, IDC_EDITVALUE, m_txtValue);
	DDX_Text(pDX, IDC_EDITVALUE, m_iValue);
	DDX_Check(pDX, IDC_CHECKVALUE, m_bEnableVale);
}

BEGIN_MESSAGE_MAP(CVDOFXDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTNOPEN, OnBnClickedBtnopen)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTONSHOW, OnBnClickedButtonshow)
END_MESSAGE_MAP()


// gestionnaires de messages pour CVDOFXDlg

BOOL CVDOFXDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Définir l'icône de cette boîte de dialogue. L'infrastructure effectue cela automatiquement
	//  lorsque la fenêtre principale de l'application n'est pas une boîte de dialogue
	SetIcon(m_hIcon, TRUE);			// Définir une grande icône
	SetIcon(m_hIcon, FALSE);		// Définir une petite icône

	// TODO : ajoutez ici une initialisation supplémentaire
	m_ValueSlider.SetRange(0, 255);
	m_ValueSlider.SetPos(0);
	
	SetTimer(TIMER_ID, 33, NULL);
	bShowVideo = false;

	lCounter = 0;
	lCounterMultiplier = 0;




	return TRUE;  // retourne TRUE, sauf si vous avez défini le focus sur un contrôle
}

// Si vous ajoutez un bouton Réduire à votre boîte de dialogue, vous devez utiliser le code ci-dessous
//  pour dessiner l'icône. Pour les applications MFC utilisant le modèle Document/Vue,
//  cela est fait automatiquement par l'infrastructure.

void CVDOFXDlg::OnPaint() 
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
HCURSOR CVDOFXDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVDOFXDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	m_iValue = m_ValueSlider.GetPos();
	iThreshold = m_iValue;
	UpdateData(false);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVDOFXDlg::OnBnClickedBtnopen()
{
	// TODO: Add your control notification handler code here
	if (!OpenFile())
	{
		AfxMessageBox("Could not open file!");
	}
}

bool CVDOFXDlg::OpenFile()
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
			//capture = cvCaptureFromCAM(-1);
			NbFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
			
			iplFrame = cvQueryFrame(capture);
            //cvSetImageCOI(iplFrame, 1);
			iplPrevious = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, iplFrame->nChannels);
			iplSub = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, iplFrame->nChannels);
			iplTemp = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, iplFrame->nChannels);

   //         iplPrevious = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);
			//iplSub = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);
			//iplTemp = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);

			cvZero(iplTemp);
			cvCopy(iplFrame, iplPrevious);

                        
			lCounter = 0;
			lCounterMultiplier = 0;
			iHalfHeight = iplFrame->height / 2;
			
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

void CVDOFXDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	cvDestroyAllWindows();

	if (capture != NULL)
		cvReleaseCapture(&capture);

	CDialog::OnClose();
}

void CVDOFXDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (bShowVideo)
	{
		if (capture != NULL)
		{
			iplFrame = cvQueryFrame(capture);


			cvSub(iplFrame, iplPrevious, iplSub);
    		
			if (lCounter < NB_FRAME_INIT)
			{
				cvThreshold(iplSub, iplSub, MOVEMENT_THRESHOLD, 255, CV_THRESH_TOZERO);
				cvAdd(iplSub, iplTemp, iplTemp);

                
				cvShowImage(szMainWindow, iplTemp);
			}
            else if (lCounter == NB_FRAME_INIT)
            {
                
                CvPoint pt1;
                IplImage *iplGray;
                IplImage *iplDest;
                IplConvKernel* element = 0;

                iplGray = cvCreateImage(cvSize(iplTemp->width, iplTemp->height), iplTemp->depth, 1);
                iplDest = cvCreateImage(cvSize(iplTemp->width, iplTemp->height), iplTemp->depth, 1);
                cvZero(iplDest);

                cvCvtColor(iplTemp, iplGray, CV_RGB2GRAY);
                                                                
                int i;
                int j;
                int iMin;
                int iMax;
                int iValue;


                iMin = 0;
                iMax = iplTemp->width;

                pt1.x = 0;
                pt1.y = 0;

                cvCircle(iplGray, pt1, 5, CV_RGB(0, 0, 255), 3, 8, 0);

                pt1.x = iplGray->width;
                pt1.y = iplGray->height;

                cvCircle(iplGray, pt1, 5, CV_RGB(0, 255, 0), 3, 8, 0);

                
                                
                for (j = 0; j < iplGray->height; j++)
                {                   
                    for (i = 0; i < iplGray->widthStep; i++)
                    {
                    
                        //iValue = (int)getPixel(iplTemp, j, i, 1);
                        iValue = (int)(uchar)iplGray->imageData[j * iplGray->widthStep + i];

                        if (iValue > 20)
                        {
                            pt1.x = i;
                            pt1.y = j;

                            cvCircle(iplDest, pt1, 1, CV_RGB(255, 255, 255), 1, 8, 0);
                        }
                    }       
                }

                element = cvCreateStructuringElementEx( 3, 3, 1, 1, CV_SHAPE_RECT, 0 );
                cvErode(iplDest, iplDest, element, 1);
                cvDilate(iplDest, iplDest, element, 1);
                
                cvReleaseStructuringElement(&element);
                
                cvShowImage(szSampleWindow, iplDest);

                cvReleaseImage(&iplGray);
                cvReleaseImage(&iplDest);

                    
            }
			else
			{               
                cvSub(iplFrame, iplTemp, iplSub);
                cvShowImage(szMainWindow, iplSub);
			}

			cvCopy(iplFrame, iplPrevious);

			//cvShowImage(szMainWindow, iplSub);
			//cvShowImage(szMainWindow, iplTemp);
			//cvShowImage(szMainWindow, iplFrame);
    		

			if (cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES) >= NbFrames)
			{
				cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);  // SetPremierFrame
			}

			lCounter++;

			if (lCounter == COUNTER_TURN)
			{
				lCounterMultiplier++;
				lCounter = 0;
			}
            }

		}

	

    try
    {
        CDialog::OnTimer(nIDEvent);            
    }	
    catch (char *sz)
    {
        bShowVideo = false;
        AfxMessageBox(sz);
        exit(EXIT_FAILURE);
    }
}

void CVDOFXDlg::OnBnClickedButtonshow()
{
	if (!bShowVideo)
	{
		
		cvNamedWindow(szMainWindow, CV_WINDOW_AUTOSIZE);
        cvNamedWindow(szSampleWindow, CV_WINDOW_AUTOSIZE);
		//cvCreateTrackbar("Slider", szMainWindow, &iThreshold, 255, CVideoEffectDlg::Threshold); 

        cvShowImage(szSampleWindow, iplFrame);

	}
	else
	{
		cvDestroyWindow(szMainWindow);
	}
	
	bShowVideo = !bShowVideo;
	UpdateData(false);
}

uchar CVDOFXDlg::getPixel( IplImage*img, int lin, int col, int channel ) 
{
    int offset;
    int x;
    int y;

   


    return ((uchar*)(img->imageData + img->widthStep * lin))[col + channel];
}
