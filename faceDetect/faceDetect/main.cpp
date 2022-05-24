#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <cvaux.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>  // Lib pour la conversion des strings
#include <direct.h>
#include <BlobResult.h>

#include "projection.h"
#include "head.h"
#include "EyeROI.h"
#include "Eyes.h"
#include "Histogram.h"

using namespace std;

#define MAXPIX 255

const int projectionSmooth = 15;
const int runningAverageSize = 20;
const int THRESHOLD_STD = 50;
const int MAX_LEVEL = 255;

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
string createCSV(int line, CvPoint LeftEye, CvPoint RightEye);
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
	char *winXor = "XOR";

	CvCapture * capture = 0;
	IplImage * imgOriginal = 0;
	IplImage * imgGradientX = 0;
	IplImage * imgGradientX_S = 0;
	IplImage * imgGradientY = 0;
	IplImage * imgGradientY_S = 0;
	IplImage * imgGray = 0;
	IplImage * imgProj = 0;
	IplImage * imgProjHorizontal = 0;
	IplImage * imgSmooth = 0;	
	IplImage * imgCrossSobel = 0;
	IplImage * imgTempProj = 0;
	IplImage * imgThresh = 0;
	IplImage * imgTemplate = 0; // 20081016 NB
	IplImage * imgCorrelation = 0; // 20081016 NB
	IplImage * imgTemp_8U = 0; // 20081023

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
	//IplImage * imgProjEyes;

	long nbFrames = 0;
	long frameCount = 0;
	long loopCounter = 0;

	vector <CvPoint> vecEyeZonePos;
	CvPoint ptEyeZoneAvg;
	CvRect rectEyeZonePrevious;
	double eyeZoneThreshold = 0.10;
	bool isInitialized = false;
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
	imgGradientX_S = cvCreateImage(sz, IPL_DEPTH_16S, 1);
	imgGradientY = cvCreateImage(sz, 8, 1);
	imgGradientY_S = cvCreateImage(sz, IPL_DEPTH_16S, 1);

	imgSmooth = cvCreateImage(sz, 8, 1);
	
	imgCrossSobel = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	imgTemp_8U = cvCreateImage(sz, IPL_DEPTH_8U, 1); // 20081023 NB
	imgTempProj = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	imgThresh = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	// 20081016 -- Section où l'on initialise les données de corrélation pour un test.
	//imgTemplate = cvLoadImage("c:\\temp\\template.jpg", 0);
	//imgCorrelation = cvCreateImage(cvSize(imgThresh->width - imgTemplate->width + 1, imgThresh->height - imgTemplate->height + 1) 
	//	, IPL_DEPTH_32F, 1);
	
	// Création des fenêtres
	cvNamedWindow(winProjection, CV_WINDOW_AUTOSIZE);
	//cvNamedWindow(winSobel, CV_WINDOW_AUTOSIZE);
	//cvNamedWindow(winGradientX, CV_WINDOW_AUTOSIZE);
	//cvNamedWindow(winEyesProjection, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(winOriginal);
	//cvNamedWindow(winXor, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(winBlob);

	// Touche appuyée
	int key = 0;

	// Initialisation du contenu du CSV
	csvFile = createCSV(0, cvPoint(0, 0), cvPoint(0, 0));

	CHistogram hist;
	CBlobResult blobs;
	CBlob blob;

	double cursorPosition = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
	
	for (frameCount = 0;key !='q'; frameCount++)
	{
		if (frameCount > nbFrames - 2)
		{
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);
			frameCount = 0;
		}
		if (nbFrames > 1)
		{
			imgOriginal = cvQueryFrame(capture);
			cursorPosition = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);
		}

		////////////////////////////////////////////////////
		// Pré-traitement

		// Convertion de la couleur vers le niveau de gris
		cvCvtColor(imgOriginal, imgGray, CV_RGB2GRAY);
		
		// Lissage de l'image originale
		cvSmooth(imgGray, imgSmooth, CV_BLUR, 9, 9);	//////  MEMORY LEAK ICI!!

		// Application du Sobel X
		// Modification de l'image destination pour Sobel en 16_S
		// 2008/08/27 NB : Suite au recommendation de FM
		cvSobel (imgSmooth, imgGradientX_S, 1, 0, 3);
		cvAbs(imgGradientX_S, imgGradientX_S);
		cvConvertScale(imgGradientX_S, imgGradientX, 1, 0);

		// Application du Sobel Y
		// Modification de l'image destination pour Sobel en 16_S
		// 2008/10/16 NB
		cvSobel (imgSmooth, imgGradientY_S, 0, 1, 3);
		cvAbs(imgGradientY_S, imgGradientY_S);
		cvConvertScale(imgGradientY_S, imgGradientY, 1, 0);

		cvThreshold(imgGradientX, imgCrossSobel, THRESHOLD_STD,  MAX_LEVEL, CV_THRESH_BINARY);
		cvThreshold(imgGradientY, imgThresh, THRESHOLD_STD, MAX_LEVEL, CV_THRESH_BINARY);
		cvXor(imgCrossSobel, imgThresh, imgCrossSobel);

		//cvMatchTemplate(imgCrossSobel, imgTemplate, imgCorrelation, CV_TM_CCORR_NORMED);

		
		//cvDilate(imgCrossSobel, imgCrossSobel);
		//cvErode(imgCrossSobel, imgCrossSobel);

		//cvFlip (imgCrossSobel, imgCrossSobel);
		//cvShowImage(winXor, imgCrossSobel);
		//cvShowImage(winBlob, imgCorrelation);

		// Fin pré=traitement
		////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		// Analyse des blobs pour les yeux
		////////////////////////////////////////////////////

//		// Trouve les blobs avec un seuil de plus de 100
//		blobs = CBlobResult(imgCrossSobel, NULL, 100, false);
//		blobs.Filter(blobs, B_EXCLUDE, CBlobGetPerimeter(), B_GREATER, 500);
//		blobs.Filter(blobs, B_EXCLUDE, CBlobGetPerimeter(), B_LESS, 10);
//
//		int nbBlobs = blobs.GetNumBlobs();
//		cvZero (imgTemp_8U);
//
//		for (int z = 0; z < nbBlobs; z++)
//		{
//			blob = blobs.GetBlob(z);
//			blob.FillBlob(imgTemp_8U, CV_RGB(255, 255, 255));
//		}
//
////		cvFlip (imgTemp);
//
//		cvShowImage(winBlob, imgTemp_8U);
//
//		blobs.ClearBlobs();
//
//		cvOr(imgGradientX, imgTemp_8U, imgGradientX);



				
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
		eyeROI.LocateEyes(imgGradientX, cvRect(0, 0, 0, 0));

		imgTempProj = eyeROI.projection.CreateProjectionImage();
		cvShowImage(winProjection, imgTempProj);

		cvReleaseImage(&imgTempProj);

		///////////////////////////////////////////
		// Filtre et seuil la position de la ROI
		if (!isInitialized)
		{
			// Calcul la moyenne de la position de la zone des yeux;
			addToVector(&vecEyeZonePos, eyeROI.GetCentre());

			if (loopCounter >= runningAverageSize)
			{
				isInitialized = true;
			}
		}
		else
		{
			double diffEyeZone = abs(eyeROI.GetCentre().y - ptEyeZoneAvg.y) / (double)imgGray->height;

			if (diffEyeZone > eyeZoneThreshold)
			{
				eyeROI.Rec = rectEyeZonePrevious;
			}

			addToVector(&vecEyeZonePos, eyeROI.GetCentre());
		}

		ptEyeZoneAvg = calcAverage(vecEyeZonePos);

		

		rectEyeZone = eyeROI.GetEyeROI();
		//imgProjHorizontal = eyeROI.projection.CreateProjectionImage();

		// Dessiner la région des yeux sur l'image originale
			
		// fin détection zone des yeux
		//////////////////////////////////



		//////////////////////////////////
		// Début détection des yeux
		

		cvSetImageROI(imgSmooth, rectEyeZone);
		imgEyes = GetROIImage(imgSmooth);
		cvResetImageROI(imgSmooth);

		cvResetImageROI(imgGradientX);

		//CEyes eyes(imgEyes);
		eyes.imgEyes = imgEyes;
		eyes.LocateEyes();

		// Création de l'image visuelle de la projection
		//imgProjEyes = eyes.projEyes.CreateProjectionImage();
		
		cvReleaseImage(&imgEyes);



		// fin détection zone des yeux
		//////////////////////////////////
		
		//end = clock();
		//double dif;

		//dif = difftime(end, start) / CLOCKS_PER_SEC;

		//printf("Temp d'execution : %.4f secondes", dif);

		////////////////////////////
		// Dessin

		// Dessiner l'oeil gauche
		CvPoint ptLeftEye;
		ptLeftEye.x = eyes.LeftEye.x + eyeROI.Rec.x;
		ptLeftEye.y = eyes.LeftEye.y + eyeROI.Rec.y;

		cvCircle(imgOriginal, ptLeftEye, radius, CV_RGB(255, 0, 0), 2);

		// Dessiner l'oeil droit
		CvPoint ptRightEye;
		ptRightEye.x = eyes.RightEye.x + eyeROI.Rec.x;
		ptRightEye.y = eyes.RightEye.y + eyeROI.Rec.y;

		cvCircle(imgOriginal, ptRightEye, radius, CV_RGB(255, 0, 0), 2);
			
		// Capture d'écran
		//if (loopCounter == 50)
		//{

		int iOffset = 20;
		cvSetImageROI(imgGray, cvRect(ptLeftEye.x - iOffset, ptLeftEye.y - iOffset, 40, 40));
		
		IplImage * imgTemp = GetROIImage(imgGray);
		//cvFlip(imgTemp, imgTemp, 0);
		cvNamedWindow(winCapture, CV_WINDOW_AUTOSIZE);
		cvShowImage(winCapture, imgTemp);
		
		hist.imgSource = imgTemp;
		hist.CreateHistogram();
		hist.SmoothHistogram(15);
		hist.RefreshHistoImg();

		cvNamedWindow(winTemp, CV_WINDOW_AUTOSIZE);
		cvShowImage(winTemp, hist.imgHistogram);

		//key = cvWaitKey();
		cvReleaseImage(&imgTemp);

		cvResetImageROI(imgGray);
	
		//}			

		//////////////////
		// point milieu
		//cvCircle(imgOriginal, cvPoint((ptLeftEye.x + ptRightEye.x)/2, (ptLeftEye.y + ptRightEye.y)/2), 
		//	radius, CV_RGB(0, 255, 255));

		/////////////////////////////
		// Garder le T-1

		rectEyeZonePrevious = eyeROI.Rec;


		////////////////////////////
		// Écriture dans le CSV
		////////////////////////////

		//if (loopCounter < nbFrames)
		if (capData)
		{
			csvFile.append(createCSV(frameCount + 1, ptLeftEye, ptRightEye));
		}

		if (key == 'd')
		{
			capData = !capData;
		}

		////////////////////////////
		// Affichage des images


		//cvFlip(imgGradientY, imgGradientY, 0);
		//cvShowImage(winGradientX, imgGradientY);
		//cvFlip(imgGradientX, imgGradientX, 0);
		//cvShowImage(winSobel, imgGradientX);
		
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
	
	cvReleaseImage(&imgCrossSobel);
	cvReleaseImage(&imgThresh);

	cvReleaseImage(&imgSmooth);
	cvReleaseImage(&imgGray);
	cvReleaseImage(&imgGradientY_S);
	cvReleaseImage(&imgGradientY);
	cvReleaseImage(&imgGradientX_S);
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

string createCSV(int line, CvPoint LeftEye, CvPoint RightEye)
{
	string szCSV;
	ostringstream out;

	if (line == 0)
	{
		szCSV = "Frame;leftX;leftY;rightX;rightY\r";
	}
	else
	{
		out << line;
		out << ";";
		out << LeftEye.x;
		out << ";";
		out << LeftEye.y;
		out << ";";
		out << RightEye.x;
		out << ";";
		out << RightEye.y;
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