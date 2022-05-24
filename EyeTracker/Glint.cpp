#include "StdAfx.h"
#include ".\glint.h"


CvPoint CGlint::LeftCentre = cvPoint(0, 0);
CvPoint CGlint::RightCentre = cvPoint(0, 0);

CvPoint CGlint::LeftAverage = cvPoint(0, 0);
CvPoint CGlint::RightAverage = cvPoint(0, 0);

CvPoint CGlint::LeftSum = cvPoint(0, 0);
CvPoint CGlint::RightSum = cvPoint(0, 0);

int CGlint::iNbFrameLeft = 0;
int CGlint::iNbFrameRight = 0;

CGlint::CGlint(void)
{
	iThreshold = THRESH;  // Par défaut
	iNbWhitePoint = 0;
	dLastQuanta = 0.;

	rectROI.height = 2 * ROISIZE + 1;
	rectROI.width = 2 * ROISIZE + 1;

	storageGlints = cvCreateMemStorage(0);
	seqGlints = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storageGlints);
}

CGlint::~CGlint(void)
{
	cvReleaseMemStorage(&storageGlints);
}

double Distance(CvPoint pt1, CvPoint pt2);

/* Sort 2d points in left-to-right top-to-bottom order */
static int cmp_func( const void* _a, const void* _b, void* userdata )
{
    CvPoint* a = (CvPoint*)_a;
    CvPoint* b = (CvPoint*)_b;
    int y_diff = a->y - b->y;
    int x_diff = a->x - b->x;
    return x_diff ? x_diff : y_diff;
}

bool CGlint::FindInImage(IplImage * iplImg)
{
	bool bRet = false;

	IplImage * iplThresh = cvCreateImage(cvGetSize(iplImg), 8, 1);
	IplImage * iplGray = cvCreateImage(cvGetSize(iplImg), 8, 1);

	// Fenêtre crée autour des points blancs
	IplImage * iplRect = cvCreateImage(cvSize(2 * ROISIZE + 1, 2 * ROISIZE + 1),
										8, 1);
	IplImage * iplNegRect = cvCreateImage(cvSize(2 * ROISIZE + 1, 2 * ROISIZE + 1),
										8, 1);


	IplImage * iplNeg = cvCreateImage(cvGetSize(iplImg), 8, 1);

	int iNonZero = 0;
	double dRatio = 0.0;
	int iTotalArea = (2 * ROISIZE + 1) * (2 * ROISIZE + 1);
	int iDistMin = 100;
	CvPoint ptMiddle;
	CvPoint ptRectCenter;

	ptRectCenter.x = ROISIZE;
	ptRectCenter.y = ROISIZE;

	// Création des images binaires
	if (iplImg->nChannels > 1)
	{
		cvCvtColor(iplImg, iplGray, CV_RGB2GRAY);
		cvFlip(iplGray, iplGray, 0);
	}
	else
	{
		cvCopy(iplImg, iplGray);
	}

	// Binarisation de l'image
	cvThreshold(iplGray, iplThresh, iThreshold, PIXMAX, CV_THRESH_BINARY);

	// Montre l'image binaire
	//cvNamedWindow("Threshold", CV_WINDOW_AUTOSIZE);
	//cvShowImage("Threshold", iplThresh);

	cvNamedWindow("Rect", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("Previous", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("Thresh", CV_WINDOW_AUTOSIZE);


	for (int j = 0; j < iplThresh->height; j++)
	{
		for (int i = 0; i < iplThresh->width; i++)
		{
			// Trouver les points blancs dans l'image binaire
			if ((unsigned char)iplThresh->imageData[j * iplThresh->width + i] > THRESH)
			{
				// Va chercher la fenêtre autour du pixel blanc
				cvGetRectSubPix(iplGray, iplRect, cvPoint2D32f(i, j));

				// Montre la fenêtre
				cvShowImage("Rect", iplRect);

				// Négatif de la fenêtre
				cvNot(iplRect, iplNegRect);

				// Seuillage de la fenêtre
				cvThreshold(iplNegRect, iplRect, THRESH, PIXMAX, CV_THRESH_BINARY);

				// Compte les nombres de pixel blanc
				iNonZero = cvCountNonZero(iplRect);

				dRatio = (double)iNonZero / (double)iTotalArea;

				// Cette partie sert à trouver le milieu de l'oeil en faisant
				// une moyenne des positions des pixels blanc retrouvé.
				if (iNonZero >= MINNBPIX)
				{
					if (iNonZero >= MAXNBPIX)
					{
						int iTmpNonZero = iTotalArea;
						int iTmpThresh = THRESH;

						// Modification du seuillage pour réduire le bruit dans
						// la pupille
						while (iTmpNonZero >= MAXNBPIX)
						{
							cvThreshold(iplNegRect, iplRect, iTmpThresh++, PIXMAX, CV_THRESH_BINARY);
							iTmpNonZero = cvCountNonZero(iplRect);

						}
                        
						if (iTmpNonZero > 0)
							iNonZero = iTmpNonZero;
					}

					int iSumX = 0;
					int iSumY = 0;
					CvPoint ptCentre;
					CvPoint ptLeft;
					CvPoint ptRight;
					CvPoint ptTop;
					CvPoint ptBottom;

					int iMinX = iTotalArea;
					int iMinY = iTotalArea;
					int iMaxX = 0;
					int iMaxY = 0;

					for (int n = 0; n < iplRect->height; n++)
					{
						for (int m = 0; m < iplRect->widthStep; m++)
						{
							if ((unsigned char)iplRect->imageData[n * iplRect->widthStep + m] >= THRESH)
							{
								iSumX += m;
								iSumY += n;
								

								if (m < iMinX)
									iMinX = m;
								
								if (n < iMinY)
									iMinY = n;
								
								if (m > iMaxX)
									iMaxX = m;

								if (n > iMaxY)
									iMaxY = n;


							}
						}
					}

  					ptCentre.x = (double)iSumX / (double)iNonZero;
					ptCentre.y = (double)iSumY / (double)iNonZero;

					ptLeft.x = iMinX;
					ptLeft.y = ptCentre.y;
					ptRight.x = iMaxX;
					ptRight.y = ptCentre.y;
					ptTop.x = ptCentre.x;
					ptTop.y = iMinY;
					ptBottom.x = ptCentre.x;
					ptBottom.y = iMaxY;

					int radius = ptCentre.x - ptLeft.x;

					//cvCircle(iplNegRect, ptCentre, radius, CV_RGB(0,0,0), 1, 8, 0);
					//cvLine(iplRect, ptTop, ptBottom, CV_RGB(0, 0, 0), 1, 8, 0);
					//cvLine(iplRect, ptLeft, ptRight, CV_RGB(0, 0, 0), 1, 8, 0);

					// Remettre les points dans l'image
					CvPoint ptCentreOeil = cvPoint(i, j);

					ptCentreOeil.x = ptCentreOeil.x - ROISIZE + ptCentre.x;
					ptCentreOeil.y = ptCentreOeil.y - ROISIZE + ptCentre.y;

					ptCentreOeil.y = iplImg->height - ptCentreOeil.y;



					cvSeqPush(seqGlints, &ptCentreOeil);
					//cvSeqPush(seqGlints, &ptCentre);
					// À vérifier cvMinEnclosingCircle
				}
						
				//cvShowImage("Previous", iplNegRect);
				//cvShowImage("Thresh", iplRect);

				//cvWaitKey(0);
			}
		}
	}

	// Analyse des points cumulés
	// Pour l'instant, on prend le plus à gauche et le plus à droite seulement
	if (seqGlints->total > 0)
	{
		cvSeqSort(seqGlints, cmp_func, 0);

		// Au moins deux glints
		if (seqGlints->total > 1)
		{
			CvPoint ptGlint1;
			CvPoint ptGlint2;

			ptGlint1 = *(CvPoint*)cvGetSeqElem(seqGlints, 0);  // Premier
			ptGlint2= *(CvPoint*)cvGetSeqElem(seqGlints, seqGlints->total - 1); // Dernier

			CGlint::LeftCentre = ptGlint1;
			CGlint::RightCentre = ptGlint2;

			
			CGlint::LeftSum.x = CGlint::LeftSum.x + CGlint::LeftCentre.x;
			CGlint::LeftSum.y = CGlint::LeftSum.y + CGlint::LeftCentre.y;

			CGlint::RightSum.x = CGlint::RightSum.x + CGlint::RightCentre.x;
			CGlint::RightSum.y = CGlint::RightSum.y + CGlint::RightCentre.y;

			CGlint::iNbFrameLeft++;
			CGlint::iNbFrameRight++;

			bRet = true;
		}
		// Un seul point
		else
		{
			CvPoint ptGlint = *(CvPoint*)cvGetSeqElem(seqGlints, 0);

			if ((iplImg->width / 2) > ptGlint.x)
			{
				CGlint::LeftCentre = ptGlint;
				CGlint::iNbFrameLeft++;

				CGlint::LeftSum.x = CGlint::LeftSum.x + CGlint::LeftCentre.x;
				CGlint::LeftSum.y = CGlint::LeftSum.y + CGlint::LeftCentre.y;
			}
			else
			{
				CGlint::RightCentre = ptGlint;
				CGlint::iNbFrameRight++;

				CGlint::RightSum.x = CGlint::RightSum.x + CGlint::RightCentre.x;
				CGlint::RightSum.y = CGlint::RightSum.y + CGlint::RightCentre.y;
			}
			
			bRet = true;
		}

		if (iNbFrameLeft > 0)
		{
			CGlint::LeftAverage.x = LeftSum.x / iNbFrameLeft;
			CGlint::LeftAverage.y = LeftSum.y / iNbFrameLeft;
		}
        
		if (iNbFrameRight > 0)
		{
			CGlint::RightAverage.x = RightSum.x / iNbFrameRight;
			CGlint::RightAverage.y = RightSum.y / iNbFrameRight;
		}
	}

	cvReleaseImage(&iplThresh);
	cvReleaseImage(&iplGray);
	cvReleaseImage(&iplNeg);
	cvReleaseImage(&iplNegRect);
	cvReleaseImage(&iplRect);

	

	return bRet;
}

double Distance(CvPoint pt1, CvPoint pt2)
{
	return sqrt((double)((pt1.x - pt2.x)^2 + (pt1.y - pt2.y)^2));
}

void CGlint::ResetFrameCount()
{
	CGlint::iNbFrameLeft = 0;
	CGlint::iNbFrameRight = 0;

	CGlint::LeftAverage = cvPoint(0, 0);
	CGlint::RightAverage = cvPoint(0, 0);

	CGlint::RightSum = cvPoint(0, 0);
	CGlint::LeftSum = cvPoint(0, 0);
}
