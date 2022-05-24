#include "StdAfx.h"
#include ".\trackingfunctions.h"

CTrackingFunctions::CTrackingFunctions(void)
{
}

CTrackingFunctions::~CTrackingFunctions(void)
{
}

CFace CTrackingFunctions::FindFace(IplImage *iplFrame)
{
	CvSize szImg;
	CvSize szROIFace;

    
	CFace face;

	return face;
}

// Fonctions permettant de trouver les deux glints d'une image s'il y a.
// Elle retourne une image avec les deux glints bien identifiés.
//
// Paramètres:
// iplFrame :	l'image source contenant les glints
// Glints:		Un vecteur de deux CGlint.  Un pour l'oeil de gauche et l'autre
//				pour celui de droit.  Glints ne contient rien lorsque dans la
//				fonction.  La fonction change la valeur de celui-ci.
// 
// Exemple:
// cvShow(FindGlints(frame, &LesGlints));
//
// Principe:
// La première étape consiste à trouver tous les points qui sont près du niveau
// de saturation, soit 255.  J'ai pris un seuil de 225.  Ces points on les
// pile dans une Sequence.
// Ensuite, on prend chaque point et on fait un négatif du voisinage.  Si le
// point se retrouve dans une zone noire, candidat pour la pupille, on pile ce
// point sur une nouvelle séquence.
// On ne devrait pas retrouver beaucoup de points qui sont dans cette situation.
// Ce qui implique que ce sont des candidats très intéressants.
// Ensuite, il suffit de mettre en evidence ces points dans une image pour la
// retourner.
IplImage* CTrackingFunctions::FindGlints(IplImage *iplFrame, CGlint *Glints)
{
	IplImage *retImage;

	CvSize szImg;
	CvPoint unPoint;
	CvSeq *ptSeq;

	int i, j;
	int n = 0; //Compteur de point blanc

    IplImage *accumulateur;
	IplImage * acc2; // deuxième accumulateur

	szImg.width = iplFrame->width;
	szImg.height = iplFrame->height;

	accumulateur = cvCreateImage(cvGetSize(iplFrame), 8, 1);
	acc2 = cvCreateImage(cvGetSize(accumulateur), 8, 1);
	retImage = cvCreateImage(cvGetSize(iplFrame), 8, 1);

	CvMemStorage *storage = cvCreateMemStorage();

	ptSeq = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);
	// Seuiller l'image en trois niveaux de gris
	for (j = 0; j < szImg.height; j++)
	{
		for (i = 0; i < szImg.width; i++)
		{
				
            if ((unsigned char)iplFrame->imageData[j*iplFrame->width+i] > 225)
			{
			
				accumulateur->imageData[j*iplFrame->width+i] = 255;

				// On accumule les points blancs dans la liste
				//lst_point[n].x = i;
				//lst_point[n].y = j;
				unPoint.x = i;
				unPoint.y = j;
				cvSeqPush(ptSeq, &unPoint);
				n++;  // incrémente le nombre de points
			}

			else if (unsigned char(iplFrame->imageData[j*iplFrame->width+i])> 200)
			{
				accumulateur->imageData[j*iplFrame->width+i] = (char) 128;
				
			} 
			
			else
			{
				accumulateur->imageData[j*iplFrame->width+i] = (char) 0;
			}

		}
	}

	return accumulateur;
	//// Voisinage négatif du filtre
	//int szNeighbourhood = 5;
	//int NbNeighbours = szNeighbourhood * szNeighbourhood;
	//int middleNeighbourhood = szNeighbourhood / 2;
	//int Neighbours[5][5];
	//int nbPoints = ptSeq->total;
	//int iSum = 0;  // Somme pour faire la moyenne du niveau de gris
	//int iAvg = 0;
	//CvPoint CurrentPoint;

	//CvSeq *seqGlintsPos;  // Séquence de position de glints trouvé;

	//int Threshold = 225;

	//// Parcourir la séquence de points
	//for (int k = 0; k < nbPoints; k++)
	//{
	//	cvSeqPop(ptSeq, &CurrentPoint);

	//	Neighbours[middleNeighbourhood][middleNeighbourhood] = 
	//		255 - (unsigned char)iplFrame->imageData[CurrentPoint.y * iplFrame->width + CurrentPoint.x];

	//	// Remplir le voisinage négatif du filtre
 //       for (j = -middleNeighbourhood; j < middleNeighbourhood; j++)
	//	{
	//		for (i = -middleNeighbourhood; i < middleNeighbourhood; i++)
	//		{
	//			Neighbours[j][i] = 255 - 
	//				(unsigned char)iplFrame->imageData
	//				[(CurrentPoint.y + j) * iplFrame->width + (CurrentPoint.x + i)];

	//			iSum += Neighbours[j][i];
	//		}
	//	}

	//	iAvg = iSum / (NbNeighbours);

	//	if (iAvg >= Threshold)
	//	{
	//		cvSeqPush(seqGlintsPos, &CurrentPoint);
	//	}
	//}

	//return retImage;
}

IplImage* CTrackingFunctions::ThresholdImage(IplImage *iplFrame, int NumberOfThresholds)
{
	IplImage *iplThresh;
	CvSize size = cvSize(iplFrame->width, iplFrame->height);

	iplThresh = cvCreateImage(size, DEPTH, NB_CHANNELS);

	int iThreshSteps = MAX_LEVELS / NumberOfThresholds;

	for (int j = 0; j < iplFrame->height; j++)
	{
		for (int i = 0; i < iplFrame->width; i++)
		{
			for (int n = NumberOfThresholds; n > 0; n--)
			{
				if (((unsigned char)iplFrame->imageData[j * iplFrame->width + i])
					>= (iThreshSteps * (n - 1)))
				{
					try
					{
						iplThresh->imageData[j * iplFrame->width + i] = iThreshSteps * n;
						break;
					}
					catch (char * strErr)
					{
						break;
					}

				}
			}
		}
	}

	return iplThresh;
}













