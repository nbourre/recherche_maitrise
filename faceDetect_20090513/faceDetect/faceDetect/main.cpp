#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>  // Lib pour la conversion des strings
#include <direct.h>
#include <math.h>

#include "projection.h"
#include "head.h"
#include "EyeROI.h"
#include "Eyes.h"
#include "Histogram.h"
#include "../../../cvBlobsLib/BlobResult.h"

using namespace std;

#define MAXPIX 255
#define THRESHEYEDIST 40.0
#define MAX_PATH 255

const int projectionSmooth = 15;
const int runningAverageSize = 20;

// Paramètre
// D:\_université\_recherche\P20_NF.avi
// D:\_université\_recherche\P18_FAT.avi
// D:\_université\_recherche\P18_FLD.avi
// meunier_loop_crop.avi

string csvFile;


// PROTOTYPE
CvRect locateFace(IplImage *img);
IplImage * GetROIImage(IplImage *img);
IplImage * GetSubPixByRect(IplImage *img);
string getExtension(string szFilename);
void addToVector(vector <CvPoint> *vec, CvPoint value);
CvPoint calcAverage(vector <CvPoint> vec);
string createCSV(int line, CvPoint LeftEye, int leftEyeArea);
string GetCurrentPath();

int main(int argc, char ** argv)
{
	char* winSobel = "Sobel head";
	char* winTemp = "Temp";
	char* winGray = "Gris";
	char* winGradientX = "Gradient X";
	char* winEyesGradient = "Eyes gradient";
	char* winOriginal = "Originale";
	char* winProjection = "Projection";
	char* winEyesProjection = "Eyes projection";
	char *winCapture = "Capture";
	char *winBlob = "Blob";
	char *winBlobLeftEye = "Blob Left Eye";

	CvCapture * capture = 0;
	IplImage * imgOriginal = 0;
	IplImage * imgGradientX = 0;
	IplImage * imgGradientY = 0;
	IplImage * imgGradientXY = 0;
	IplImage * imgGray = 0;
	IplImage * imgProj = 0;
	IplImage * imgProjHorizontal = 0;
	IplImage * imgSmooth = 0;
	IplImage * imgGradientX_S = 0;
	IplImage * imgGradientY_S = 0;
	IplImage * imgTemp = 0;
	IplImage * imgTempProj = 0;

	CHead head;
	CEyeROI eyeROI;
	CEyes eyes;
	
	CProjection projMain;
	CProjection projY;

	CvSize sz;

	//clock_t start, end;

	//start = clock();

	CvRect rectEyeZone;

	CProjection projEyes;
	IplImage * imgEyes;
	IplImage * imgProjEyes;

	long nbFrames = 0;
	long frameCount = 0;
	long loopCounter = 0;

	vector <CvPoint> vecEyeZonePos;

	CvPoint ptLeftEye;
	float leftEyeArea;
	float leftEyeAreaMax;
	CvPoint ptEyeZoneAvg;
	CvPoint ptLeftEyePrevious;
	CvRect rectEyeZonePrevious;
	float leftEyeAreaPrevious;
	double eyeZoneThreshold = 100;
	bool isInitialized = false;
	bool isLeftEyeLocated = false;
	int radius = 10;
	bool capData = false;
	

	cout<<"Usage : facedetect filename.[jpg|avi]\r\n";
	cout<<"[q] : Quit\r\n";
	cout<<"[space] : Pause\r\n";
	cout<<"[d] : toggle data capture\r\n";

	
	if (argc > 1)
	{
 		string szFile(_strlwr(argv[1]));
		string szExtension(getExtension(szFile));
		
		if (szExtension == "jpg")
		{
			imgOriginal = cvLoadImage(szFile.c_str());
			nbFrames = 0;
		}
		else if (szExtension == "avi")
		{
			capture = cvCaptureFromFile(szFile.c_str());
			imgOriginal = cvQueryFrame(capture);
			nbFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
		}
	}
	else
	{
		printf("Usage : faceDetect picture");
		printf("Press a key to continue...");
		cvWaitKey();
		return 0;
	}
	
	// Construction des objets
	sz = cvSize(imgOriginal->width, imgOriginal->height);
	imgGray = cvCreateImage(sz, 8, 1);
	imgGradientX = cvCreateImage(sz, 8, 1);
	imgGradientY = cvCreateImage(sz, 8, 1);
	imgGradientXY = cvCreateImage(sz, 8, 1);
	imgSmooth = cvCreateImage(sz, 8, 1);
	imgGradientX_S = cvCreateImage(sz, IPL_DEPTH_16S, 1);
	imgGradientY_S = cvCreateImage(sz, IPL_DEPTH_16S, 1);
	imgTemp = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	imgTempProj = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	// Création des fenêtres
	cvNamedWindow(winProjection, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(winSobel, CV_WINDOW_AUTOSIZE);
	//cvNamedWindow(winGradientX, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(winEyesProjection, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(winOriginal);
	cvNamedWindow(winBlob);
	//cvNamedWindow(winBlobLeftEye);

	// Touche appuyée
	int key = 0;

	// Initialisation du contenu du CSV
	csvFile = createCSV(0, cvPoint(0, 0), 0);

	CHistogram hist;
	CBlobResult blobs;
	CBlob blob;


	double cursorPosition = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
	
// Nombre de frame a sauter avant de commencer l'analyse
	int offsetFrames = 0;		// Nombre de frames skippés
	int nbFramesSautes = 0;		// Compteur
	nbFrames = 899;			// Nombre d'images à capturer
	capData = 1;				// 1 si on veut capturer des données

	//cursorPosition = jumpToFrame;

	//cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursorPosition);

	for (frameCount = 0;(key !='q') && (frameCount < nbFrames); frameCount++)
	{

		printf("\n frameCount = %d ",frameCount);


		/*if (frameCount > nbFrames - 2)
		{
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);
			frameCount = 0;
		}*/

		if (nbFrames == 0)
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, offsetFrames);

		if (nbFrames > 1)
		{
			imgOriginal = cvQueryFrame(capture);
			cursorPosition = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
			if(offsetFrames)
			{				
				//nbFramesSautes++;

				//printf("frame sautes : %i\toffset : %i", nbFramesSautes, offsetFrames);
				//if(nbFramesSautes < offsetFrames)
				//{
				//	frameCount--;
				//	continue;
				//}
			}
		}
			
		////////////////////////////////////////////////////
		// Pré-traitement

		// Convertion de la couleur vers le niveau de gris
		cvCvtColor(imgOriginal, imgGray, CV_RGB2GRAY);
		
		// Lissage de l'image originale
		cvSmooth(imgGray, imgSmooth, CV_BLUR, 9, 9);	//////  MEMORY LEAK ICI!!

		// Application du Sobel X horizontal
		// Modification de l'image destination pour Sobel en 16_S
		// 2008/08/27 NB : Suite au recommendation de FM
		cvSobel (imgSmooth, imgGradientX_S, 1, 0, 3);
		cvAbs(imgGradientX_S, imgGradientX_S);
		cvConvertScale(imgGradientX_S, imgGradientX, 1, 0);

		// Calculer le gradient en Y vertical

		cvSobel (imgSmooth, imgGradientY_S, 0, 1, 3);
		cvAbs(imgGradientY_S, imgGradientY_S);
		cvConvertScale(imgGradientY_S, imgGradientY, 1, 0);

		//cvOr(imgGradientX, imgGradientY, imgGradientXY);

		cvAdd(imgGradientX, imgGradientY, imgGradientXY);

		//cvThreshold(imgGradientX, imgGradientX, 50, 255, CV_THRESH_TOZERO);


		// Fin pré=traitement
		////////////////////////////////////////////////////


		////////////////////////////////////////////////////
		// Analyse des blobs pour les yeux
		////////////////////////////////////////////////////

		// Trouve les blobs avec un seuil de plus de 100
		//blobs = CBlobResult(imgGradientX, NULL, 50, true);
		blobs = CBlobResult(imgGradientXY, NULL, 50, true);
		blobs.Filter(blobs, B_EXCLUDE, CBlobGetPerimeter(), B_GREATER, 200);
		blobs.Filter(blobs, B_EXCLUDE, CBlobGetPerimeter(), B_LESS, 20);
		blobs.Filter(blobs, B_EXCLUDE, CBlobGetElongation(), B_GREATER, 5);

		int nbBlobs = blobs.GetNumBlobs();		
		cvZero (imgTemp);

		for (int z = 0; z < nbBlobs; z++)
		{
			blob = blobs.GetBlob(z);
			blob.FillBlob(imgTemp, CV_RGB(255, 255, 255));
		}

		cvDilate(imgTemp,imgTemp,0,1);
		cvErode(imgTemp,imgTemp,0,1);

		//cvFlip (imgTemp);

		cvShowImage(winBlob, imgTemp);
		cvWaitKey(1);

		blobs.ClearBlobs();


		//cvShowImage(winSobel, imgGradientX);

		//cvOr(imgGradientXY, imgTemp, imgGradientXY);
		cvFlip (imgGradientXY);
		cvShowImage(winSobel, imgGradientXY);
		cvWaitKey(1);

		// Fin Analyse des blobs
		////////////////////////////////////////////////////
		

		//////////////////////////////////
		// Début de la détection de la tete


		//head.LocateHead(imgGradientX);
		//imgProj = head.projection.CreateProjectionImage();
		//cvShowImage(winProjection, imgProj);

		// Fin de la détection de la tete
		//////////////////////////////////


		//////////////////////////////////
		// Début détection zone des yeux

		//cvSetImageROI(imgGradientX, cvRect(head.x, head.y, head.width, head.height));
		//eyeROI.LocateEyes(imgGradientX);
		
		//eyeROI.LocateEyes(imgGradientX, cvRect(0, 0, 0, 0));

		eyeROI.LocateEyes(imgTemp, cvRect(0, 0, 0, 0));


		imgTempProj = eyeROI.projection.CreateProjectionImage();
		cvShowImage(winProjection, imgTempProj);

		cvReleaseImage(&imgTempProj);

		///////////////////////////////////////////
		// Filtre et seuil la position de la ROI
		if (!isInitialized)
		{
			// Calcul la moyenne de la position de la zone des yeux;
			addToVector(&vecEyeZonePos, eyeROI.GetCentre());
			ptEyeZoneAvg = calcAverage(vecEyeZonePos);

			if (loopCounter >= runningAverageSize)
			{
				isInitialized = true;
			}
		}
		else
		{
			//double diffEyeZone = abs(eyeROI.GetCentre().y - ptEyeZoneAvg.y) / (double)imgGray->height;
			double diffEyeZone = fabs(double (eyeROI.GetCentre().y - ptEyeZoneAvg.y));
		

			if (diffEyeZone > eyeZoneThreshold)
			{
				printf("Out of bound EYES ZONE => %25.20lf",diffEyeZone );
				eyeROI.Rec = rectEyeZonePrevious;
				ptLeftEye.x = ptLeftEyePrevious.x;
				ptLeftEye.y = ptLeftEyePrevious.y;
				leftEyeArea = leftEyeAreaPrevious;

				leftEyeArea = eyeROI.ComputeLeftEyeArea(imgGray, ptLeftEye);

				printf("\nOut of bound  EYE ZONE LEFT EYE AREA = %20.10f",leftEyeArea );

				leftEyeAreaPrevious = leftEyeArea;
				cvResetImageROI(imgGray);
				
		

		////////////////////////////
		// Écriture dans le CSV
		////////////////////////////

		//if (loopCounter < nbFrames)
				if (capData)
				{
					csvFile.append(createCSV(frameCount + 1, ptLeftEye,(int)leftEyeArea));
				}

				if (key == 'd')
				{
					capData = !capData;
				}
				loopCounter++;

				cvShowImage(winOriginal, imgOriginal);
				cvWaitKey(1);
				continue;
			}

			addToVector(&vecEyeZonePos, eyeROI.GetCentre());

			ptEyeZoneAvg = calcAverage(vecEyeZonePos);
		}

		//ptEyeZoneAvg = calcAverage(vecEyeZonePos);

		

		rectEyeZone = eyeROI.GetEyeROI();
		//imgProjHorizontal = eyeROI.projection.CreateProjectionImage();

		// Dessiner la région des yeux sur l'image originale
			
		// fin détection zone des yeux
		//////////////////////////////////



		//////////////////////////////////
		// Début détection des yeux
		

		//cvSetImageROI(imgSmooth, rectEyeZone);
		//imgEyes = GetROIImage(imgSmooth);
		//cvResetImageROI(imgSmooth);

		cvSetImageROI(imgTemp, rectEyeZone);
		imgEyes = GetROIImage(imgTemp);
		cvResetImageROI(imgTemp);


		//cvResetImageROI(imgGradientX);

		//CEyes eyes(imgEyes);
		eyes.imgEyes = imgEyes;
		eyes.LocateEyes();

		// Création de l'image visuelle de la projection
		imgProjEyes = eyes.projEyes.CreateProjectionImage();
		
		cvReleaseImage(&imgEyes);


		cvShowImage(winOriginal, imgOriginal);
		cvShowImage(winEyesProjection, imgProjEyes);
		cvWaitKey(1);
		cvReleaseImage(&imgProjEyes);


		// fin détection zone des yeux
		//////////////////////////////////
		
		//end = clock();
		//double dif;

		//dif = difftime(end, start) / CLOCKS_PER_SEC;

		//printf("Temp d'execution : %.4f secondes", dif);

		////////////////////////////
		// Dessin

		// Dessiner l'oeil gauche
		//CvPoint ptLeftEye;
		ptLeftEye.x = eyes.LeftEye.x + eyeROI.Rec.x;
		ptLeftEye.y = eyes.LeftEye.y + eyeROI.Rec.y;

		if(!isLeftEyeLocated)
			isLeftEyeLocated = TRUE;
		else
		{
			// Si l'oeil de gauche dépasse le seuil de tolérance
			if((fabs(double(ptLeftEye.x - ptLeftEyePrevious.x)) > THRESHEYEDIST) ||
			(fabs(double(ptLeftEye.y - ptLeftEyePrevious.y)) > THRESHEYEDIST))
			{
				printf("Out of bound LEFT EYE => DX =  %d DY = %d",ptLeftEye.x - ptLeftEyePrevious.x,ptLeftEye.y - ptLeftEyePrevious.y );
				ptLeftEye.x = ptLeftEyePrevious.x;
				ptLeftEye.y = ptLeftEyePrevious.y;
				leftEyeArea = leftEyeAreaPrevious;

				leftEyeArea = eyeROI.ComputeLeftEyeArea(imgGray, ptLeftEye);

				printf("\nOut of bound  LEFT EYE LEFT EYE AREA = %20.10f",leftEyeArea );

				leftEyeAreaPrevious = leftEyeArea;

				cvResetImageROI(imgGray);

			////////////////////////////	
			// Écriture dans le CSV
			////////////////////////////

			//if (loopCounter < nbFrames)
					if (capData)
					{
						csvFile.append(createCSV(frameCount + 1, ptLeftEye,(int)leftEyeArea));
					}

					if (key == 'd')
					{
						capData = !capData;
					}
					loopCounter++;

				/*cvShowImage(winOriginal, imgOriginal);
				cvShowImage(winEyesProjection, imgProjEyes);
				cvWaitKey();
				cvReleaseImage(&imgProjEyes);*/
				continue;
			}
		}
		cvCircle(imgOriginal, ptLeftEye, radius, CV_RGB(255, 0, 0), 2);

		// Dessiner l'oeil droit
		/* CvPoint ptRightEye;
		ptRightEye.x = eyes.RightEye.x + eyeROI.Rec.x;
		ptRightEye.y = eyes.RightEye.y + eyeROI.Rec.y;

		cvCircle(imgOriginal, ptRightEye, radius, CV_RGB(255, 0, 0), 2);*/
			
		// Capture d'écran
		//if (loopCounter == 50)
		//{

		leftEyeArea = eyeROI.ComputeLeftEyeArea(imgGray, ptLeftEye);

		printf("\n IN bound LEFT EYE => LEFT EYE AREA = %20.10f",leftEyeArea );

		leftEyeAreaPrevious = leftEyeArea;

		cvResetImageROI(imgGray);






		//key = cvWaitKey();
//		cvReleaseImage(&imgTemp);
//	cvResetImageROI(imgGray);
	
		//}			

		//////////////////
		// point milieu
		//cvCircle(imgOriginal, cvPoint((ptLeftEye.x + ptRightEye.x)/2, (ptLeftEye.y + ptRightEye.y)/2), 
		//	radius, CV_RGB(0, 255, 255));

		/////////////////////////////
		// Garder le T-1

		rectEyeZonePrevious = eyeROI.Rec;

		ptLeftEyePrevious.x = ptLeftEye.x;
		ptLeftEyePrevious.y = ptLeftEye.y;
		leftEyeAreaPrevious = leftEyeArea;

		////////////////////////////
		// Écriture dans le CSV
		////////////////////////////

		//if (loopCounter < nbFrames)
		if (capData)
		{
			csvFile.append(createCSV(frameCount + 1, ptLeftEye,(int)leftEyeArea));
		}

		if (key == 'd')
		{
			capData = !capData;
		}

		////////////////////////////
		// Affichage des images


		//cvShowImage(winGradientX, imgGradientX);
		//cvFlip(imgGradientX, imgGradientX, 0);

		//cvShowImage(winSobel, imgGradientX);
		
		
		//cvShowImage(winProjection, imgProjHorizontal);
		//cvShowImage(winEyesProjection, imgProjEyes);	
		cvShowImage(winOriginal, imgOriginal);

		if (loopCounter == nbFrames)
			key = cvWaitKey();		
		else
			key = cvWaitKey(1);

 		if (key == ' ')
			key = cvWaitKey();
		else if (key == '-')  // 
		{
			if (cursorPosition >= 15)
				cursorPosition -= 15;
			else
				0;
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursorPosition);
		}
		else if (key == '+')
		{
			if (cursorPosition < nbFrames - 20)
				cursorPosition += 15;
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, cursorPosition);
		}

	
		cvReleaseImage(&imgProj);
		//cvReleaseImage(&imgProjHorizontal);
		//cvReleaseImage(&imgProjEyes);

		++loopCounter;
	}

	// Écriture dans le fichier
	time_t curr = time(0);
	struct tm * timeinfo;
	char buff[80];
	timeinfo = localtime (&curr);
	strftime(buff, 80, "%Y%m%d_%H%M%S", timeinfo);

	string szTime(buff);
	string csvFilename = GetCurrentPath() + "\\" + szTime + ".csv";
	ofstream fichier(csvFilename.c_str());
	fichier << csvFile;
	fichier.close();
	
	cvReleaseImage(&imgTemp);

	cvReleaseImage(&imgSmooth);
	cvReleaseImage(&imgGray);
	cvReleaseImage(&imgGradientX);
	cvReleaseCapture(&capture);
	
	return 0;
}

/// Fonction qui retourne l'image d'un ROI
IplImage * GetROIImage(IplImage * img)
{
	if (img->roi == NULL)
		return img;

	IplImage * imgOut;

	imgOut = cvCreateImage(cvSize(img->roi->width, img->roi->height), img->depth, img->nChannels);
	cvCopy(img, imgOut);

	return imgOut;
}

string getExtension(string szFilename)
{
	string retExtension;
	size_t dot;

	dot = szFilename.rfind(".");
	retExtension = szFilename.substr(dot + 1);

	return retExtension;

}

void addToVector(vector <CvPoint> *vec, CvPoint value)
{
	int actualSize = vec->size();
	vector <CvPoint>::iterator firstElem;

	if (actualSize >= runningAverageSize)
	{
		firstElem = vec->begin();

		vec->erase(firstElem);
		vec->push_back(value);
	}
	else
	{
		vec->push_back(value);
	}
}

CvPoint calcAverage(vector <CvPoint> vec)
{
	CvPoint avg;
	int i;
	int nbElements = vec.size();

	avg.x = 0;
	avg.y = 0;

	if (nbElements > 0)
	{
		for (i = 0; i < nbElements; i++)
		{
			avg.x += vec.at(i).x;
			avg.y += vec.at(i).y;
		}

		avg.x = avg.x / nbElements;
		avg.y = avg.y / nbElements;
	}
	

	return avg;
}

/// Permet de générer une ligne CSV
string createCSV(int line, CvPoint LeftEye,int LeftEyeArea)
{
	string szCSV;
	ostringstream out;

	if (line == 0)
	{
		szCSV = "Frame;leftX;leftY;Area\r";
	}
	else
	{
		out << line;
		out << ";";
		out << LeftEye.x;
		out << ";";
		out << LeftEye.y;
		out << ";";
		out << LeftEyeArea;
		out << "\r";
		szCSV += out.str();
	}
	
	
	return szCSV;
}


string GetCurrentPath()
{
	char buff[MAX_PATH] = "";
	char *ptr; 

	string szOut;

	ptr = getcwd(buff, MAX_PATH);
	szOut = buff;
	return szOut;	
}

/// Ancien code de Francois.

	/*	int iOffsetX = 40; // 20
		int iOffsetY = 30; // 20

		cvResetImageROI(imgGray);
		printf("\n ptLeftEye.y - iOffset + 60 = %d %d",ptLeftEye.y - iOffsetY+60,imgGray->height);
		if((ptLeftEye.y - iOffsetY + 60) > imgGray->height)
			cvSetImageROI(imgGray, cvRect(ptLeftEye.x - iOffsetX, ptLeftEye.y-iOffsetY, 80, imgGray->height-(ptLeftEye.y - iOffsetY)-1)); //, 40,40
		else
			cvSetImageROI(imgGray, cvRect(ptLeftEye.x - iOffsetX, ptLeftEye.y-iOffsetY, 80, 60)); //, 40,40
		
		IplImage * imgTemp = GetROIImage(imgGray);
		cvFlip(imgTemp, imgTemp, 0);
		cvNamedWindow(winCapture, CV_WINDOW_AUTOSIZE);
		cvShowImage(winCapture, imgTemp);
		cvWaitKey();
		cvFlip(imgTemp, imgTemp, 0);

		hist.imgSource = imgTemp;
		hist.CreateHistogram();
		hist.SmoothHistogram(15);
		hist.RefreshHistoImg();
		
		printf("\n mean = %20.12lf",cvMean(imgTemp,0));
		if(hist.GlintHistogram(20))
		{
			printf("\n Left Eye Open");
			CvPoint posGlint = hist.RefineEyeLocation(10);

			posGlint.x = imgGray->roi->xOffset + posGlint.x;
			posGlint.y = imgGray->roi->yOffset + posGlint.y;


			cvResetImageROI(imgGray);
			printf("\n posGlint.y - iOffset + 60 = %d %d",posGlint.y - iOffsetY+60,imgGray->height);
			if((posGlint.y - iOffsetY + 60) > imgGray->height)
				cvSetImageROI(imgGray, cvRect(posGlint.x - iOffsetX, posGlint.y-iOffsetY, 80, imgGray->height-(posGlint.y - iOffsetY)-1)); //, 40,40
			else
				cvSetImageROI(imgGray, cvRect(posGlint.x - iOffsetX, posGlint.y-iOffsetY, 80, 60)); //, 40,40
		
			cvReleaseImage(&imgTemp);
			IplImage * imgTemp = GetROIImage(imgGray);
			hist.imgSource = imgTemp;

			cvFlip(imgTemp, imgTemp, 0);
			cvNamedWindow(winCapture, CV_WINDOW_AUTOSIZE);
			cvShowImage(winCapture, imgTemp);
			cvWaitKey();
			cvFlip(imgTemp, imgTemp, 0);



			int thresh = hist.ThreshHistogram(20);
			printf("\n T = %d",thresh);

			cvThreshold(imgTemp, imgTemp, thresh, 255, CV_THRESH_BINARY);

            cvShowImage(winBlobLeftEye, imgTemp);
			cvWaitKey();

			//cvDilate(imgTemp,imgTemp,0,1);
			//cvErode(imgTemp,imgTemp,0,1);
//printf("\n imgTemp 0 0 = %d",(unsigned char)imgTemp->imageData[0]);
			cvFloodFill( imgTemp, cvPoint(0,0), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);

		
//printf("\n imgTemp height 0 = %d",(unsigned char)imgTemp->imageData[(imgTemp->height-1)*imgTemp->widthStep]);
			if((unsigned char)imgTemp->imageData[(imgTemp->height-1)*imgTemp->widthStep] == 255)
				cvFloodFill( imgTemp, cvPoint(0,imgTemp->height-1), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);

//printf("\n imgTemp 0 width  = %d",(unsigned char)imgTemp->imageData[imgTemp->width-1]);
			if((unsigned char)imgTemp->imageData[imgTemp->width-1] == 255)
				cvFloodFill( imgTemp, cvPoint(imgTemp->widthStep-1,0), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);
//printf("\n imgTemp height width  = %d",(unsigned char)imgTemp->imageData[(imgTemp->height-1)*imgTemp->widthStep+imgTemp->widthStep-1]);
			if((unsigned char)  imgTemp->imageData[(imgTemp->height-1)*imgTemp->widthStep+imgTemp->widthStep-1] == 255)
				cvFloodFill( imgTemp, cvPoint(imgTemp->width-1,imgTemp->height-1), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);

			cvDilate(imgTemp,imgTemp,0,1);
			cvErode(imgTemp,imgTemp,0,1);

			cvShowImage(winBlobLeftEye, imgTemp);
			cvWaitKey();

	        CvMemStorage* stor;
		    CvSeq* cont;
		    CvBox2D32f* box;
		    CvPoint* PointArray;
		    CvPoint2D32f* PointArray2D32f;
    
           // Create dynamic structure and sequence.
		    stor = cvCreateMemStorage(0);
		    cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor);
    

           // Find all contours.
            cvFindContours( imgTemp, stor, &cont, sizeof(CvContour), 
                    CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    
   
 
          // This cycle draw all contours and approximate it by ellipses.
			leftEyeAreaMax = 0;
            for(;cont;cont = cont->h_next)
            {  
		

               int i; // Indicator of cycle.
               int count = cont->total; // This is number point in contour
               CvPoint center;
               CvSize size;
        
            // Number point must be more than or equal to 6 (for cvFitEllipse_32f).        
               if( count < 20 )
                   continue;

//printf("\n count = %d",count);
			leftEyeArea = fabs(cvContourArea(cont,CV_WHOLE_SEQ));
		    if(leftEyeArea > leftEyeAreaMax)
			{
				int maxX,minX,maxY,minY;
				
		    //printf("\n contour number = %d",cont->total);
		    //printf("\n Area = %lf",leftEyeArea);
        
        // Alloc memory for contour point set.    
               PointArray = (CvPoint*)malloc( count*sizeof(CvPoint) );
               PointArray2D32f= (CvPoint2D32f*)malloc( count*sizeof(CvPoint2D32f) );
        
        // Alloc memory for ellipse data.
                box = (CvBox2D32f*)malloc(sizeof(CvBox2D32f));
        
        // Get contour point set.
                cvCvtSeqToArray(cont, PointArray, CV_WHOLE_SEQ);
        
        // Convert CvPoint set to CvBox2D32f set.
				minX = imgTemp->width;
				minY = imgTemp->height;
				maxX = maxY = 0;
                for(i=0; i<count; i++)
                {
                   PointArray2D32f[i].x = (float)PointArray[i].x;
                   PointArray2D32f[i].y = (float)PointArray[i].y;
				   if(PointArray[i].x < minX)
					   minX = PointArray[i].x;
				   else if(PointArray[i].x > maxX)
					   maxX = PointArray[i].x;
				   if(PointArray[i].y < minY)
					   minY = PointArray[i].y;
				   else if(PointArray[i].y > maxY)
					   maxY = PointArray[i].y;
                 }

				 if(((float)((maxX-minX)/(maxY-minY))) > 3.0 || 
					 ((float)((maxY-minY)/(maxX-minX))) > 3.0)
				 {
					 free(PointArray);
                     free(PointArray2D32f);
                     free(box);
					 continue;
				 }
        
				leftEyeAreaMax = leftEyeArea;
        // Fits ellipse to current contour.
                 cvFitEllipse(PointArray2D32f, count, box);
      
				 cvZero(imgTemp);
        // Draw current contour.
                 cvDrawContours(imgTemp,cont,CV_RGB(255,255,255),CV_RGB(255,255,255),0,1,8,cvPoint(0,0));

	cvShowImage(winBlobLeftEye, imgTemp);
	cvWaitKey();
        // Convert ellipse data from float to integer representation.
                 center.x = cvRound(box->center.x);
                 center.y = cvRound(box->center.y);
                 size.width = cvRound(box->size.width*0.5);
                 size.height = cvRound(box->size.height*0.5);
                 box->angle = -box->angle;
        
        // Draw ellipse.
                 cvEllipse(imgTemp, center, size,
                  box->angle, 0, 360,
                  CV_RGB(128,128,128), 3, CV_AA, 0);
    cvShowImage(winBlobLeftEye, imgTemp);
	cvWaitKey();
        
        // Free memory.          
                  free(PointArray);
                  free(PointArray2D32f);
                  free(box);
			   }
			   else
				   continue;
             }
			leftEyeArea = leftEyeAreaMax;
		    printf("\n Area MAX = %lf",leftEyeArea);
			cvReleaseMemStorage(&stor);
			free(cont); 

cvLine(hist.imgHistogram,cvPoint(thresh,0),cvPoint(thresh,hist.imgHistogram->height),CV_RGB(255,255,255),3,8,0);



		}
		else{
			printf("\n Left Eye Close");
			leftEyeArea = 0.0;
		}

	
		cvNamedWindow(winTemp, CV_WINDOW_AUTOSIZE);
		cvShowImage(winTemp, hist.imgHistogram);
*/