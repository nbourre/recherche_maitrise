#include "stdafx.h"
#include "CVideo.h"
#include "math.h"

CVideo *vdo = 0;
CvPoint ptMouse;

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
	char *winName = "Original";
	char *win2 = "test";
	char *win3 = "Projection_H";
	char *win4 = "Projection_V";
	char *winDiff = "difference";
	char *winSobel = "Sobel";

	// Création des fenêtres pour l'affichage
	cvvNamedWindow(winName, CV_WINDOW_AUTOSIZE);
	cvvNamedWindow(win2, CV_WINDOW_AUTOSIZE);
	cvvNamedWindow(win3, CV_WINDOW_AUTOSIZE);
	cvvNamedWindow(win4, CV_WINDOW_AUTOSIZE);
	cvvNamedWindow(winDiff, CV_WINDOW_AUTOSIZE);
	//cvvNamedWindow(winSobel, CV_WINDOW_AUTOSIZE);

	CvSize szImg;
	HWND hwnd;
	CvSize filterSize = cvSize(10, 10);

	IplImage * iplT = 0;
	IplImage * iplT_1 = 0;
	IplImage * iplGray = 0;
	IplImage * iplGraySmall = 0;
	IplImage * iplDifference = 0;
	IplImage * iplMovingAverage = 0;
	IplImage * iplTest = 0;
	IplImage * iplThresh = 0;
	IplImage * iplProjectionH = 0;
	IplImage * iplProjectionV = 0;
	IplImage * iplSobel;


	int iFactor = 4;
	double runningAvgWeight = 0.5;
	int avgX = 0;

	int eyeHeightAverage = 0;
	int eyeHeightSum = 0;

	int eyeHeightMovingAverage = 0;
	int eyeHeightMovingSum = 0;

	int * movingAvg;
	int movingAvgSize = 100;
	int movingAvgCounter = 0;
	bool movingAvgStart = false;

	movingAvg = new int[movingAvgSize];

	int loopCounter = 0;

	CvPoint2D32f pt3;
	CvPoint2D32f previousFaceCentre;
	double diffX; // servira à calculer le déplacement entre deux positions du visage
	double diffY;
	int radius;
	int nonZero;
	CvRect rectROI;
	CvRect rectProj;


	// 2008/04/15
	double diff = 0;

	peak pkAvgH;
	pkAvgH.Position = 0;
	pkAvgH.Value = 0;

	// 2008/04/27
	seuilMax = 0.15;


	previousFaceCentre.x = 0;
	previousFaceCentre.y = 0;

	szImg = cvGetSize(iplFrame);
	radius = (int)(szImg.width / (iFactor * 3.25));

	// iplT <-- image redimensionné à T
	// iplT_1 <-- image à T-1
	// iplDifference <-- difference entre 
	iplT = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 8, 1);
	iplT_1 = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 8, 1);
	iplDifference = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 8, 1);
	iplMovingAverage = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 32, 1);
	iplGray = cvCreateImage(cvSize(iplFrame->width, iplFrame->height), 8, 1);
	iplGraySmall = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 8, 1);
	iplTest = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 8, 1);
	iplThresh = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 8, 1);
	iplSobel = cvCreateImage(cvSize(iplFrame->width/iFactor, iplFrame->height/iFactor), 8, 1);

	// Variable propre au projection
	CProjection projH;
	CProjection projV;
	int kernelSize = 5;
	
	CvRect bndRect = cvRect(0,0,0,0);


	bool first = true;

	if (isBusy)
		return;

	isBusy = true;
	while(1)
	{
		loopCounter++;

		if (GetFramePos() >= nbFrames)
			ResetVideo();

		// Capture l'image
		iplFrame = cvQueryFrame(capture);

		// Convertir en 8 bit
		cvCvtColor(iplFrame, iplGray, CV_RGB2GRAY); 
		cvFlip(iplGray);

		// Redimensionne l'image à plus petit pour acquérir de la vitesse
		cvResize(iplGray, iplT);


		if (first)
		{
			// Historique
			iplDifference = cvCloneImage(iplT);
			iplT_1 = cvCloneImage(iplT);
			cvConvertScale(iplT, iplMovingAverage, 1, 0);  // ?
			first = false;
		}
		else
		{
			// Moyenne qui se déplace
			cvRunningAvg(iplT, iplMovingAverage, runningAvgWeight, NULL);
		}

		cvConvertScale(iplMovingAverage, iplT_1, 1, 0); // ?

		// Difference entre l'image 
		cvAbsDiff(iplT, iplT_1, iplDifference);
		

		// Copie
		iplGraySmall = cvCloneImage(iplDifference);

		// Image binaire
		cvThreshold(iplGraySmall, iplGraySmall, 75, 255, CV_THRESH_BINARY);

		// Dilater et eroder pour obtenir les blobs
		cvDilate(iplGraySmall, iplGraySmall, 0, 2);
		cvErode(iplGraySmall, iplGraySmall, 0, 2);


		// Pseudo-méthode pour trouver le centre du visage
		nonZero = cvCountNonZero(iplGraySmall);
		
		if (nonZero > 0 && !first)
		{
			pt3 = getAveragePoint(iplGraySmall);
            
			// Différence de déplacement en X
			diffX = abs((pt3.x - previousFaceCentre.x) / iplGraySmall->width);
			diffY = abs((pt3.y - previousFaceCentre.y) / iplGraySmall->height);

			if ( diffX < 0.1  && diffY < 0.1)
				cvCircle(iplGraySmall, cvPoint((int)pt3.x, (int)pt3.y), radius, cvScalar(255, 255, 255));
			else
				cvCircle(iplGraySmall, cvPoint((int)previousFaceCentre.x, (int)previousFaceCentre.y), 
					radius, cvScalar(255, 255, 255));

			previousFaceCentre = pt3;

		}
		else
		{
			cvCircle(iplGraySmall, cvPoint((int)previousFaceCentre.x, (int)previousFaceCentre.y), 
				radius, cvScalar(255, 255, 255));
		}
		
		// Afficher le rectangle qui fera office de ROI
		cvRectangle(iplGraySmall, 
			cvPoint(previousFaceCentre.x - radius, previousFaceCentre.y - radius),
			cvPoint(previousFaceCentre.x + radius, previousFaceCentre.y + radius),
			cvScalar(255, 255, 255));

		rectROI.height = 2 * radius;
		rectROI.width = 2 * radius;
		rectROI.x = (int)previousFaceCentre.x;
		rectROI.y = (int)previousFaceCentre.y;				
		
		cvSobel(iplDifference, iplDifference, 0, 1, 3);		

		//////////////////////////
		// Afficher la projection horizontale
		//////////////////////////
		projH.CreateHorizontalProjection(iplDifference);
		projH.Smooth(kernelSize);
		iplProjectionH = projH.CreateProjectionImage();
	
		cvvShowImage(win3, iplProjectionH);
		cvReleaseImage(&iplProjectionH);
		
		///////////////////////////////
		// Fin du code de projection
		///////////////////////////////

		/////////////////////////////////////////////////
		// Début du calcul pour la projection verticale
		/////////////////////////////////////////////////

		// Ajoute le maxPeak actuel au vecteur de Peak.
		if (projH.MaxPeak.Position > 0 && vecPeakHor.size() < runningAverageSize)
		{
			addToVector(&vecPeakHor, projH.MaxPeak);
		}
		else if (projH.MaxPeak.Position > 0)
		{
			// Calcul la différence entre le maxPeak actuel et la moyenne
			diff = abs(pkAvgH.Position - projH.MaxPeak.Position) / (double)projH.Size.height;

			if (diff <= seuilMax)
				// Ajouter le peak seulement s'il ne dépasse pas le seuil
				addToVector(&vecPeakHor, projH.MaxPeak);

			// Dessine la ligne du maxPeak actuel
			//cvLine(iplT, cvPoint(0, projH.MaxPeak.Position), cvPoint(iplT->width, projH.MaxPeak.Position), CV_RGB(127, 127, 127));
		}

		// Doit avoir au moins runningAverageSize images pour calculer la moyenne
		if (vecPeakHor.size() >= runningAverageSize)
		{

			pkAvgH = calcAverage(vecPeakHor);

			// Dessine la ligne de la moyenne du maxPeak
			cvLine(iplT, cvPoint(0, pkAvgH.Position), cvPoint(iplT->width, pkAvgH.Position), CV_RGB(255, 255, 255));

			// Calcule le rectangle de la projection horizontale
			rectProj.height = iplDifference->height * seuilMax;
			rectProj.width = iplDifference->width;
			rectProj.x = 0;

			if (diff <= seuilMax)
				rectProj.y = projH.MaxPeak.Position - (rectProj.height / 2);
			else
				rectProj.y = pkAvgH.Position - (rectProj.height / 2);

			cvRectangle(iplT, cvPoint(0, rectProj.y), cvPoint(rectProj.width, rectProj.height + rectProj.y), CV_RGB(0, 0, 0));

			cvSetImageROI(iplDifference, rectProj);

			projV.CreateVerticalProjection(iplDifference);
			iplProjectionV = projV.CreateProjectionImage();

	
			
		}

		/////////////////////////////////////////////////
		// Fin du calcul pour la projection verticale
		/////////////////////////////////////////////////
		
		//////////////////////////
		// Afficher la projection verticale
		//////////////////////////	


		cvResetImageROI(iplDifference);
		cvvShowImage(win4, iplProjectionV);
		cvReleaseImage(&iplProjectionV);

		///////////////////////////////
		// Fin de l'affichage de projection verticale
		///////////////////////////////


		// Début des démonstration visuel
		cvvShowImage(win2, iplGraySmall);
		cvvShowImage(winDiff, iplDifference);
		cvvShowImage(winName, iplT);

		// Empêche le bogue de fermeture de fenêtre
		hwnd = (HWND)cvGetWindowHandle(winName);
		if (hwnd == NULL)
			break;

		if (cvWaitKey(1)==' ' || BreakPlayback)
			cvWaitKey();			

		cvReleaseImage(&iplGraySmall);
		
	}

	cvReleaseImage(&iplSobel);
	cvReleaseImage(&iplThresh);
	cvReleaseImage(&iplTest);
	cvReleaseImage(&iplT_1);
	cvReleaseImage(&iplT);
	cvReleaseImage(&iplGray);
	cvReleaseImage(&iplDifference);
	cvReleaseImage(&iplMovingAverage);

	delete [] movingAvg;

	
	
	isBusy = false;


}



// Retourne la liste de points qui ont été cliquer dans la méthode Lucas-Kanade
CString CVideo::GetPointList()
{
	if (count == NULL || count == 0)
		return NULL;


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
	int i;

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


CvPoint2D32f * CVideo::getPointArray(IplImage *img)
{
	
	CvPoint2D32f *pt;
	int nbPts = 0;
	int cpt = 0;
	uchar data;

	nbPts = cvCountNonZero(img);

	pt = new CvPoint2D32f[nbPts];

	for (int j = 0; j < img->height; j++)
	{
		for (int i = 0; i < img->width; i++)
		{
			data = (uchar)img->imageData[i * img->height + j];

			if (data > 0)
			{
				pt[cpt].x = i;
				pt[cpt++].y = j;
			}
		}

	}

	return pt;
}


// Retourne la position moyenne des points
// Problème problème, ne semble pas retourner la bonne valeur
CvPoint2D32f CVideo::getAveragePoint(IplImage *img)
{
	CvPoint2D32f pt;
	int nbPoints = cvCountNonZero(img);
	long sumX = 0;
	long sumY = 0;

	for (int j = 0; j < img->height; j++)
	{
		for (int i = 0; i < img->widthStep; i++)
		{
			
			if ((uchar)img->imageData[j * img->width + i] > 200)
			{
				sumX += i;
				sumY += j;
			}
		}
	}

	pt.x = sumX / nbPoints;
	pt.y = sumY / nbPoints;

	return pt;
}



void CVideo::addToVector(vector <peak> *vec, peak pk)
{
	int actualSize = vec->size();
	vector <peak>::iterator firstElem;



	if (actualSize >= runningAverageSize)
	{
		firstElem = vec->begin();

		vec->erase(firstElem);
		vec->push_back(pk);
	}
	else
	{
		vec->push_back(pk);
	}
}

peak CVideo::calcAverage(vector <peak> vec)
{
	peak pk;
	int i;
	int nbElements = vec.size();

	pk.Position = 0;
	pk.Value = 0;

	if (nbElements > 0)
	{
		for (i = 0; i < nbElements; i++)
		{
			pk.Position += vec.at(i).Position;
			pk.Value += vec.at(i).Value;
		}

		pk.Position = pk.Position / nbElements;
		pk.Value = pk.Value / nbElements;
	}
	

	return pk;
}
