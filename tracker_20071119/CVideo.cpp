#include "stdafx.h"
#include "CVideo.h"

CVideo *vdo = 0;
IplImage *image = 0;
CvPoint pt;
int add_remove_pt;
void on_mouse( int event, int x, int y, int flags, void* param );
CvPoint ptMouse;
//CvPoint *pt;
//int addNbPt;

// Retourne la prochaine image dans le vidéo
IplImage * CVideo::QueryFrame()
{
	return cvQueryFrame(capture);
}

// Retourne la position du prochain frame
int CVideo::GetFramePos()
{
	return (int)cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
}

double CVideo::GetFPS()
{
	return cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
}

// Remet à zéro le pointeur de frame
void CVideo::ResetVideo()
{
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);
}

// Permet de positionner le pointeur sur le frame demandé
void CVideo::SetFramePosition(int Position)
{
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, Position);
}

// Joue le vidéo en accélerer
void CVideo::Play()
{
	cvvNamedWindow("Orignal", CV_WINDOW_AUTOSIZE);

	while(1)
	{
		iplFrame = cvQueryFrame(capture);
		cvvShowImage("Orignal", iplFrame);
		if (cvWaitKey(1)==' ')
			break;
	}

	if (iplFrame != NULL || iplFrame != 0)
	{
		cvReleaseImage(&iplFrame);
	}
}

void CVideo::PlayProcessed()
{
	cvvNamedWindow("Orignal", CV_WINDOW_AUTOSIZE);

	IplImage * iplThresh = cvCreateImage(cvGetSize(iplFrame), 8, 1);

	if (isBusy)
		return;

	isBusy = true;
	while(1)
	{
		if (GetFramePos() >= nbFrames)
			ResetVideo();
		iplFrame = cvQueryFrame(capture);

		
		cvThreshold (iplFrame, iplFrame, SliderValue, 255, CV_THRESH_BINARY);
		//cvCanny (iplFrame, iplThresh, 50, 200, 3);
		cvvShowImage("Orignal", iplFrame);
		if (cvWaitKey(1)==' ' || BreakPlayback)
			break;
	}
	isBusy = false;

	cvReleaseImage(&iplThresh);
}

void CVideo::LucasKanade()
{

	if (isBusy)
		return;

	char *winName = "Lucas-Kanade";
	IplImage *grey = 0, *prev_grey = 0, *pyramid = 0, *prev_pyramid = 0, *swap_temp;

	cvvNamedWindow(winName, CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback( winName, on_mouse, 0 );

	
	int iPressedChar;
	int i;
	int j;
	int k;
	int win_size = 10;

	// Lucas-Kanade initialisation
	points[0] = 0;
	points[1] = 0;
	status = 0;
	count = 0;
	need_to_init = 0;
	night_mode = 0;
	flags = 0;
	add_remove_pt = 0;
	//addNbPt = 0;

	// Indique que l'objet est oqp.
	LKActive = true;
	isBusy = true;

	for (;;)
	{
		iplFrame = 0;
		if (GetFramePos() >= nbFrames)
			ResetVideo();
		iplFrame = cvQueryFrame(capture);

		if (!iplFrame)
			break;

		if (!image)
		{
			//Allouer tous les tampons
			image = cvCreateImage(cvGetSize(iplFrame), 8, 3);
			image->origin = iplFrame->origin;

			grey = cvCreateImage(cvGetSize(iplFrame), 8, 1);
			prev_grey = cvCreateImage(cvGetSize(iplFrame), 8, 1);

			pyramid = cvCreateImage(cvGetSize(iplFrame), 8, 1);
			prev_pyramid = cvCreateImage(cvGetSize(iplFrame), 8, 1);

			points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
			points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));

			for (i = 0; i < T; i++)
			{
				ptTime[i] = (CvPoint2D32f*)cvAlloc(MAX_COUNT * sizeof(ptTime[0][0]));
			}

			status = (char*)cvAlloc(MAX_COUNT);
			flags = 0;

			//pt = (CvPoint*) cvAlloc(MAX_COUNT * sizeof(pt[0]));
		}

		

		cvCopy( iplFrame, image, 0 );
        cvCvtColor( image, grey, CV_BGR2GRAY );

		if (night_mode)
			cvZero (image);

		if( need_to_init )
        {
            /* automatic initialization */
            IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
            IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );
            double quality = 0.01;
            double min_distance = 10;

            count = MAX_COUNT;
            cvGoodFeaturesToTrack( grey, eig, temp, points[1], &count,
                                   quality, min_distance, 0, 3, 0, 0.04 );
            cvFindCornerSubPix( grey, points[1], count,
                cvSize(win_size, win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
            cvReleaseImage( &eig );
            cvReleaseImage( &temp );

            add_remove_pt = 0;
        }
        else if( count > 0 )
        {
			// Augmentation du nombre d'itération à 30 au lieu de 20 dans les critères.
            cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
                points[0], points[1], count, cvSize(win_size,win_size), 3, status, 0,
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,30,0.03), flags );
            flags |= CV_LKFLOW_PYR_A_READY;

			// Sauvegarde les points dans un vecteur de points temporelle
			savePoints(count);

			// Dessine les points
			for (i = 0; i < T; i++)
			{
				for (j = 0; j < count; j++)
					cvCircle(image, cvPointFrom32f(ptTime[i][j]), 2, CV_RGB(255, i * SPLIT_T, i * SPLIT_T), -1, 8, 0);
			}
			
            

            for( i = k = 0; i < count; i++ )
            {

				if( add_remove_pt )
				{
					
					double dx = pt.x - points[1][i].x;
					double dy = pt.y - points[1][i].y;

					if( dx*dx + dy*dy <= 25 )
					{
						add_remove_pt = 0;
						continue;
					}
				}
	            
				if( !status[i] )
					continue;
	            
				points[1][k++] = points[1][i];
				cvCircle( image, cvPointFrom32f(points[1][i]), 3, CV_RGB(0,255,0), -1, 8,0);			
			
            }

			count = k;


        }

		if( add_remove_pt && count < MAX_COUNT )
        {
            points[1][count++] = cvPointTo32f(pt);
            cvFindCornerSubPix( grey, points[1] + count - 1, 1,
                cvSize(win_size,win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
            add_remove_pt = 0;
        }

        CV_SWAP( prev_grey, grey, swap_temp );
        CV_SWAP( prev_pyramid, pyramid, swap_temp );
        CV_SWAP( points[0], points[1], swap_points );
        need_to_init = 0;

		cvvShowImage(winName, image);

		// Vérifie si une touche a été appuyée
		iPressedChar = cvWaitKey(10);
		if ((char)iPressedChar == 27)
		{
			break;
		}

		switch( (char) iPressedChar)
        {
        case 'r':
            need_to_init = 1;
            break;
        case 'c':
            count = 0;
            break;
        case 'n':
            night_mode ^= 1;
            break;
		case ' ':
			cvWaitKey();
			break;
        default:
            ;
        }
	}

	LKActive = false;
	isBusy = false;

	cvvDestroyWindow(winName);
}

void on_mouse( int event, int x, int y, int flags, void* param )
{
    if( !image )
        return;

    if( image->origin )
        y = image->height - y;

    if( event == CV_EVENT_LBUTTONDOWN )
    {
        pt = cvPoint(x, y);
        add_remove_pt = 1;
    }
	else if (event == CV_EVENT_MOUSEMOVE)
	{
		ptMouse = cvPoint(x, y);
	}
}

// Retourne la liste de points qui ont été cliquer dans la méthode Lucas-Kanade
CString CVideo::GetPointList()
{
	if (count == NULL || count == 0)
		return NULL;


	int iNbPoints;
	CString szRet = "";
	CString szPoint = "";
	CString szX = "";
	CString szY = "";

	for (int i = 0; i < count; i++)
	{
		szX = "";
		szY = "";

		//Conversion du float en CString
		szX.Format("%3.f", points[1][i].x);
		szY.Format("%3.f", points[1][i].y);
		
		szPoint.Format ("Points %d --> x=%s, y=%s\r\n", i, szX, szY);
		szRet = szRet + szPoint;
	}

	return szRet;
}

// Retourne vrai si LK est en fonction
bool CVideo::GetLKStatus()
{
	if (LKActive != NULL)
		return LKActive;
	else
		return NULL;
}

// Retourne vrai si un affichage vidéo est en fonction
bool CVideo::GetStatus()
{
	if (isBusy != NULL)
		return isBusy;
	else
		return NULL;
}

// Retourne la position de la souris
CvPoint CVideo::GetMousePos()
{
	return ptMouse;
}

// Copier les valeurs dans le vecteur de temps passé
void CVideo::savePoints(int nbPoints)
{

	// Copie T vers T + 1
	for (int i = T - 2; i >= 0; i--)
	{
		for (int j = 0; j < nbPoints; j++)
		{
			ptTime[i + 1][j] = ptTime[i][j];
		}
	}


	for (i = 0; i < nbPoints; i++)
	{
		ptTime[0][i] = points[1][i];
	}

}