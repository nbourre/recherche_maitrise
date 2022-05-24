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

#define PIXMAX 255

const char *szMainWindow = "Video";
const char *szSampleWindow = "Sample";
const char *szRightEye = "Right";
const char *szLeftEye = "Left";
const char *szOpening = "Opening";



CVDOFXDlg::CVDOFXDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVDOFXDlg::IDD, pParent)
	, m_iValue(0)
	, m_bEnableVale(FALSE)
	, m_bUseCamera(FALSE)
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
	DDX_Check(pDX, IDC_CHKCAM, m_bUseCamera);
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
	ON_BN_CLICKED(IDC_CHECKVALUE, OnBnClickedCheckvalue)
	ON_BN_CLICKED(IDC_BTNTEST, OnBnClickedBtntest)
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

	UpdateData(true);
	if (m_bUseCamera)
	{
		if (!OpenCamera())
		{
			AfxMessageBox("Could not open camera!");
			blSourceCamera = false;
		}
		else
		{
			initCamera();
			blSourceCamera = true;
		}

	}
	else 
	{
		if (!OpenFile())
		{
			AfxMessageBox("Could not open file!");           		
		}
		else
			blSourceCamera = false;

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

			initializeIpl();

            MOVEMENT_THRESHOLD_AFTERINIT = 100;
			
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

bool CVDOFXDlg::OpenCamera()
{
	bool bRet = false;

	capture = cvCaptureFromCAM(-1);  // Any camera

	if (capture == NULL)
	{
		bRet = false;
	}
	else
	{
		bRet = true;
		NbFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
        initializeIpl();
	}

	return bRet;
}

void CVDOFXDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	cvDestroyAllWindows();
    KillTimer(TIMER_ID);

    if (blLoaded)
    {
        cvReleaseImage(&iplGray);
        cvReleaseImage(&iplPrevious);
        cvReleaseImage(&iplSub);
        //cvReleaseImage(&iplTemp);
    }

	if (capture != NULL)
		cvReleaseCapture(&capture);

	CDialog::OnClose();
}

void CVDOFXDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMER_ID)
	{
		if (bShowVideo)
		{

			if (capture != NULL)
			{
				iplFrame = cvQueryFrame(capture);
				

				cvCvtColor(iplFrame, iplFrameGray, CV_RGB2GRAY);

				if (lCounter < NB_FRAME_INIT)
				{
					if (blSourceCamera)
						cvSmooth(iplFrameGray, iplFrameGray);

                    _calibration.addImage(iplFrameGray, iplPrevious);
                    
				}
				else if (lCounter == NB_FRAME_INIT)
				{
                    iplTemp = _calibration.getBuffer();

					cvCopy(iplFrameGray, iplSample);

					CvPoint pt1;


					IplImage *iplDest;				

					iplDest = cvCreateImage(cvSize(iplTemp->width, iplTemp->height), iplTemp->depth, 1);
					cvZero(iplDest);

					int i;
					int j;
					int iMin;
					int iMax;
					int iValue;


					iMin = 0;
					iMax = iplTemp->width;

					for (j = 0; j < iplTemp->height; j++)
					{                   
						for (i = 0; i < iplTemp->widthStep; i++)
						{
	                    
							iValue = (uchar)iplTemp->imageData[j * iplTemp->widthStep + i];

							if (iValue > MOVEMENT_THRESHOLD_AFTERINIT)
							{
								pt1.x = i;
								pt1.y = j;

								iplDest->imageData[j * iplDest->widthStep + i] = (uchar)PIXMAX;
	                            
							}
						}       
					}
	                
					

					opening(iplDest, CV_SHAPE_RECT);
					
					// Trouvez les coordonnées de la tête
                    _head.searchHead(iplDest, iNeckHeight);
                    _head.drawBox(iplDest);
                    _head.setHeadROI(iplSample);

					// Search for eyes
                    _head._eyes.SearchEyes(iplSample);
                    _head._eyes.drawEyes(iplSample);

					//cvShowImage(szTest, iplSample);

					cvReleaseImage(&iplDest);					
				}
				else
				{					
					if (m_bEnableVale)
					{
						cvThreshold(iplFrameGray, iplFrameGray, m_iValue, 255, CV_THRESH_BINARY);
					}

                    
                    _head.adjustChildren();

                    _head._eyes.eyeLeft.findEye(iplFrameGray);
                    _head._eyes.eyeRight.findEye(iplFrameGray);

                    cvShowImage(szRightEye, iplFrameGray);
                    
                    _head.drawBox(iplFrameGray);//drawMovementBox(iplFrameGray, _head);
                    _head._eyes.drawEyes(iplFrameGray);
					cvShowImage(szMainWindow, iplFrameGray);

					
                }

				cvCopy(iplFrameGray, iplPrevious);


				if (!blSourceCamera)
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
	}


    CDialog::OnTimer(nIDEvent);            
	
    //catch (char *sz)
    //{
    //    bShowVideo = false;
    //    AfxMessageBox(sz);
    //    exit(EXIT_FAILURE);
    //}
}

void CVDOFXDlg::OnBnClickedButtonshow()
{
	if (!bShowVideo)
	{
		
		cvNamedWindow(szMainWindow, CV_WINDOW_AUTOSIZE);
        //cvNamedWindow(szSampleWindow, CV_WINDOW_AUTOSIZE);
		//cvNamedWindow(szOpening, CV_WINDOW_AUTOSIZE);
		cvNamedWindow(szRightEye, CV_WINDOW_AUTOSIZE);
        cvNamedWindow(szLeftEye, CV_WINDOW_AUTOSIZE);
		
        //cvShowImage(szSampleWindow, iplFrame);
        
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
    return ((uchar*)(img->imageData + img->widthStep * lin))[col + channel];
}


void CVDOFXDlg::drawMovementBox(IplImage *iplView, CHead theHead)
{
	cvRectangle(iplView, cvPoint(theHead.rectBox.x, theHead.rectBox.y), 
		cvPoint(theHead.rectBox.width + theHead.rectBox.x,
		theHead.rectBox.height + theHead.rectBox.y), CV_RGB(255, 255, 255));
}

void CVDOFXDlg::OnBnClickedCheckvalue()
{
	// TODO: Add your control notification handler code here
	bEnableThreshold = m_bEnableVale;
	UpdateData(true);
}


void CVDOFXDlg::OnBnClickedBtntest()
{
	// TODO: Add your control notification handler code here
	getSubImage(iplSample);

}

CvMat * CVDOFXDlg::getSubImage(IplImage * iplInput)
{
	if (iplInput->roi == NULL)
	{
		return NULL;
	}
	else
	{
		cvSetErrMode(CV_ErrModeSilent);
		CvRect rect = cvGetImageROI(iplInput);
		CvMat *mt = cvCreateMat(rect.height, rect.width, iplInput->depth);
		CvMat *sMat = cvGetSubRect(iplInput, mt, rect);

		if (cvGetErrStatus() != 0)
		{
			KillTimer(TIMER_ID);
			AfxMessageBox("An error occured in function getSubImage");
			return NULL;
		}
		cvSetErrMode(CV_ErrModeParent);

		return sMat;
	}
}

void CVDOFXDlg::opening(IplImage *img, int shape = CV_SHAPE_RECT)
{
	// Opening to erase noise
	IplConvKernel* element = 0;
	element = cvCreateStructuringElementEx( 5, 5, 2, 2, shape, 0 );
	cvErode(img, img, element, 1);
	cvDilate(img, img, element, 1);                
	cvReleaseStructuringElement(&element);
}

void CVDOFXDlg::initializeIpl()
{
			iplFrame = cvQueryFrame(capture);
			

            // Initialize all the necessary image
			iplPrevious = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);
			iplSub = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);
			iplTemp = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);
            iplFrameGray = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);
            iplGray = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);
			iplSample = cvCreateImage(cvGetSize(iplFrame), iplFrame->depth, 1);

            _calibration.initCalibration(iplFrame);


			cvZero(iplTemp);
			cvCvtColor(iplFrame, iplPrevious, CV_RGB2GRAY);

            blLoaded = true;

                        
			lCounter = 0;
			lCounterMultiplier = 0;
			iHalfHeight = iplFrame->height / 2;
            iNeckHeight = (int)(iHalfHeight * 0.8);
}

void CVDOFXDlg::initCamera()
{
	int i = 0;
	int iInitFrame = 15;

	if (capture != NULL)
	{
		for (i = 0; i <= iInitFrame; i++)
			iplFrame = cvQueryFrame(capture);

		MOVEMENT_THRESHOLD_AFTERINIT = 200;
	}

}