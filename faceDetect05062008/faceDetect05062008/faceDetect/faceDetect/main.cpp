#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <cvaux.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "projection.h"


#define MAXPIX 255

const int thresholdImg = 200;
const int projectionSmooth = 15;

CvRect locateFace(IplImage *img);
IplImage * GetROIImage(IplImage *img);
IplImage * GetSubPixByRect(IplImage *img);


int main(int argc, char ** argv)
{
   	char* winSobel = "Sobel head";
	char* winTemp = "Temp";
	char* winGray = "Gris";
	char* winGradientX = "Gradient X";
	char* winGradientXY = "Gradient XY";
	char* winEyesGradient = "Eyes gradient";
	char* winOriginal = "Originale";
	char* winProjection = "Projection";
	char* winEyesProjection = "Eyes projection";
	char* winLeftEyeGradientX = "Left Eye Gradient X";
	char* winLeftEyeGradientXY = "Left Eye Gradient XY";

	char* winGradientContourXY = "Gradient Contour XY";
	char* winGradientXY_Canny = "Gradient XY Canny";
	char* winGradientXY_Flood = "Gradient XY Flood";
	char* winGradientXY_Invert = "Gradient XY Invert";

	IplImage * imgOriginal = 0;
	IplImage * imgGradientX = 0;
	IplImage * imgGradientXY = 0;
	IplImage * imgGradientX_S = 0;
	IplImage * imgGradientY_S = 0;
	IplImage * imgGradientXY_U = 0;
	IplImage * imgGradientXY_U_Canny = 0;
	IplImage * imgGradientXY_U_Flood = 0;
	IplImage * imgGradientXY_U_Invert = 0;
	IplImage * imgGradientContourXY_U = 0;
	IplImage * imgGray = 0;
	IplImage * imgProj = 0;
	IplImage * imgProjHorizontal = 0;
	IplImage * imgSmooth = 0;
	IplImage * imgThreshold = 0;

	CProjection projMain;
	CProjection projY;

	CvSize sz;

	clock_t start, end;

	start = clock();

	CvRect rectEyeZone;
	
	if (argc > 1)
		imgOriginal = cvLoadImage(argv[1]);
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
	imgGradientXY = cvCreateImage(sz, IPL_DEPTH_16S, 1);
	imgGradientX_S = cvCreateImage(sz, IPL_DEPTH_16S, 1);
	imgGradientY_S = cvCreateImage(sz, IPL_DEPTH_16S, 1);
	imgGradientXY_U = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	imgGradientXY_U_Canny = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	imgGradientXY_U_Flood = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	imgGradientXY_U_Invert = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	imgGradientContourXY_U = cvCreateImage(sz, IPL_DEPTH_8U, 3);
	imgSmooth = (IplImage*)cvClone(imgGray);
	imgThreshold = (IplImage*)cvClone(imgGray);
	

	// Convertion de la couleur vers le niveau de gris
	cvCvtColor(imgOriginal, imgGray, CV_RGB2GRAY);
	
	// Lissage de l'image originale
	cvSmooth(imgGray, imgSmooth, CV_GAUSSIAN, 9, 9);

	// Application du Sobel X
	//cvSobel(imgSmooth, imgGradientX, 1, 0, 3); // gradient + conserve

	cvSobel(imgSmooth, imgGradientX, 2, 0, 5); // df^2/dx^2 + conserve


	// Application du Sobel XY
	cvSobel(imgSmooth, imgGradientX_S, 1, 0, 3); // gradient +/- conserve
	//cvSobel(imgSmooth, imgGradientY_S, 0, 1, 3);
	cvAbs(imgGradientX_S,imgGradientX_S);
	//cvAbs(imgGradientY_S,imgGradientY_S);
	//cvAdd(imgGradientX_S,imgGradientY_S,imgGradientXY,0);
	//cvConvertScaleAbs( imgGradientX_S , imgGradientXY_U, 1, 0);

	cvConvertScaleAbs(imgGradientX_S, imgGradientXY_U, 1, 0);

	cvThreshold (imgGradientXY_U, imgGradientXY_U, 127, 255, CV_THRESH_TOZERO);

	cvNamedWindow(winTemp, CV_WINDOW_AUTOSIZE);
	cvShowImage(winTemp, imgGradientXY_U);

	cvWaitKey();




		cvCanny(imgSmooth,imgGradientXY_U,50,100,3);
		//cvDilate(imgGradientXY_U, imgGradientXY_U,0,1);
		//cvErode(imgGradientXY_U, imgGradientXY_U,0,3);

		cvCopy(imgGradientXY_U,imgGradientXY_U_Canny);

		cvFloodFill( imgGradientXY_U, cvPoint(0,0), CV_RGB( 255, 255, 255 ), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);
		cvCopy(imgGradientXY_U,imgGradientXY_U_Flood);
		cvNot(imgGradientXY_U,imgGradientXY_U);

		cvCopy(imgGradientXY_U,imgGradientXY_U_Invert);

		CvMemStorage* stor;
		CvSeq* cont;
		CvBox2D32f* box;
		CvPoint* PointArray;
		CvPoint2D32f* PointArray2D32f;
    
    // Create dynamic structure and sequence.
		stor = cvCreateMemStorage(0);
		cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor);
    

    // Find all contours.
    cvFindContours( imgGradientXY_U, stor, &cont, sizeof(CvContour), 
                    CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    
    // Clear images. IPL use.
    cvZero(imgGradientContourXY_U);
   
 
    // This cycle draw all contours and approximate it by ellipses.
    for(;cont;cont = cont->h_next)
    {  
		

        int i; // Indicator of cycle.
        int count = cont->total; // This is number point in contour
        CvPoint center;
        CvSize size;
        
        // Number point must be more than or equal to 6 (for cvFitEllipse_32f).        
        if( count < 6 )
            continue;

		printf("\n contour number = %d",cont->total);
		printf("\n Area = %lf",fabs(cvContourArea(cont,CV_WHOLE_SEQ)));
        
        // Alloc memory for contour point set.    
        PointArray = (CvPoint*)malloc( count*sizeof(CvPoint) );
        PointArray2D32f= (CvPoint2D32f*)malloc( count*sizeof(CvPoint2D32f) );
        
        // Alloc memory for ellipse data.
        box = (CvBox2D32f*)malloc(sizeof(CvBox2D32f));
        
        // Get contour point set.
        cvCvtSeqToArray(cont, PointArray, CV_WHOLE_SEQ);
        
        // Convert CvPoint set to CvBox2D32f set.
        for(i=0; i<count; i++)
        {
            PointArray2D32f[i].x = (float)PointArray[i].x;
            PointArray2D32f[i].y = (float)PointArray[i].y;
        }
        
        // Fits ellipse to current contour.
        cvFitEllipse(PointArray2D32f, count, box);
        
        // Draw current contour.
        cvDrawContours(imgGradientContourXY_U,cont,CV_RGB(255,255,255),CV_RGB(255,255,255),0,1,8,cvPoint(0,0));
        
        // Convert ellipse data from float to integer representation.
        center.x = cvRound(box->center.x);
        center.y = cvRound(box->center.y);
        size.width = cvRound(box->size.width*0.5);
        size.height = cvRound(box->size.height*0.5);
        box->angle = -box->angle;
        
        // Draw ellipse.
        cvEllipse(imgGradientContourXY_U, center, size,
                  box->angle, 0, 360,
                  CV_RGB(255,255,0), 3, CV_AA, 0);
        
        // Free memory.          
        free(PointArray);
        free(PointArray2D32f);
        free(box);
    }




	//////////////////////////////////
	// Début de la détection de la tete
	//////////////////////////////////
	cvThreshold(imgGradientX, imgThreshold, thresholdImg, 255, CV_THRESH_BINARY);

	//projMain.Range = 0;  // Élimine le range blanc

	// Règle des tampons autour des pics
	projMain.Buffer = 0.15;
	projMain.Range = 0.15;
	

	// Création de la projection verticale
	projMain.CreateVerticalProjection(imgGradientX);

	// Lissage de la projection
	projMain.Smooth(projectionSmooth);

	// Déclaration des pics
	peak pkLeft;
	peak pkRight;

	pkLeft = projMain.MaxPeak;

	// Trouve le deuxième pic
	pkRight = projMain.FindNextPeak();

	// Triage des pics
	if (pkLeft.Position > pkRight.Position)
	{
		peak tmp = pkLeft;
		pkLeft = pkRight;
		pkRight = tmp;
	}

	//// Tracer du rectangle entre les pics
	//cvRectangle(imgOriginal, cvPoint(pkLeft.Position, 0), cvPoint(pkRight.Position, imgOriginal->height), CV_RGB(MAXPIX,MAXPIX, MAXPIX));
	
	imgProj = projMain.CreateProjectionImage();

	//////////////////////////////////
	// Fin de la détection de la tete
	//////////////////////////////////



	//////////////////////////////////
	// Début détection zone des yeux
	//////////////////////////////////
	// Creation de la projection à partir du gradient en Y
	projY.Buffer = 0.10;
	projY.Range = 0.10;

	projY.CreateHorizontalProjection(imgGradientX);
	projY.Smooth(projectionSmooth);

	imgProjHorizontal = projY.CreateProjectionImage();

	int upperBound;
	int lowerBound;

	upperBound = (int)(projY.MaxPeak.Position - (projY.Buffer * imgGradientX->height));
	lowerBound = (int)(projY.MaxPeak.Position + (projY.Buffer * imgGradientX->height));

	//cvRectangle(imgOriginal, cvPoint(0, upperBound), cvPoint(imgOriginal->width, lowerBound), CV_RGB(255, 255, 255));

	// Bounding box
	rectEyeZone.x = pkLeft.Position;
	rectEyeZone.y = upperBound;
	rectEyeZone.height = lowerBound - upperBound;
	rectEyeZone.width = pkRight.Position - pkLeft.Position;

	cvRectangle(imgOriginal, cvPoint(rectEyeZone.x, rectEyeZone.y), 
		cvPoint(rectEyeZone.width + rectEyeZone.x, rectEyeZone.height + rectEyeZone.y), CV_RGB(255, 255, 255));
	
	//////////////////////////////////
	// fin détection zone des yeux
	//////////////////////////////////

	//////////////////////////////////
	// Début détection des yeux
	//////////////////////////////////
	
	CProjection projEyes;
	IplImage * imgEyes;
	IplImage * imgLeftEye;
	IplImage * imgProjEyes;
	IplImage * imgEyesGrad;
	IplImage * imgLeftEyeGradXY;
	IplImage * imgLeftEyeGradXY_U;

	cvSetImageROI(imgSmooth, rectEyeZone);
	imgEyes = GetROIImage(imgSmooth);
	cvResetImageROI(imgSmooth);

	imgEyesGrad = cvCreateImage(cvGetSize(imgEyes), imgEyes->depth, imgEyes->nChannels);

	cvSobel(imgEyes, imgEyesGrad, 0, 2, 5);

	projEyes.CreateVerticalProjection(imgEyesGrad);
	projEyes.Smooth(projectionSmooth);

	peak leftEye;
	peak rightEye;

	leftEye = projEyes.MaxPeak;
	rightEye = projEyes.FindNextPeak();

	if (leftEye.Position > rightEye.Position)
	{
		peak tmp;
		tmp = leftEye;
		leftEye = rightEye;
		rightEye = tmp;
	}

	int width;
	CvRect roi;
	IplImage * imgTemp;
	CProjection projTemp;

	width = (int)(projEyes.Buffer * imgEyes->width) * 2;

	/////////////////
	// Oeil gauche
	/////////////////
	roi.height = imgEyes->height;
	roi.width = width;
	roi.x = rectEyeZone.x + (leftEye.Position - (width / 2));  // ATTENTION!! La position est par rapport à l'image originale
	roi.y = rectEyeZone.y;

	cvSetImageROI(imgGradientX, roi);
	imgTemp = GetROIImage(imgGradientX);
	cvResetImageROI(imgGradientX);

	cvSetImageROI(imgGradientXY_U, roi);
	imgLeftEyeGradXY_U = GetROIImage(imgGradientXY_U);
	cvResetImageROI(imgGradientXY_U);

	cvNamedWindow(winLeftEyeGradientXY, CV_WINDOW_AUTOSIZE);
	cvShowImage(winLeftEyeGradientXY, imgLeftEyeGradXY_U);



	cvNamedWindow(winLeftEyeGradientX, CV_WINDOW_AUTOSIZE);
	cvShowImage(winLeftEyeGradientX, imgTemp);

	

	projTemp.CreateHorizontalProjection(imgTemp);
	projTemp.Smooth(projectionSmooth);

	CvPoint ptLeftEye;
	ptLeftEye.x = leftEye.Position + rectEyeZone.x;
	ptLeftEye.y = projTemp.MaxPeak.Position + rectEyeZone.y;

	cvCircle(imgOriginal, ptLeftEye, 10, CV_RGB(255, 0, 0));

	/////////////////
	//Oeil droit
	/////////////////

	roi.x = rectEyeZone.x + (rightEye.Position - (width / 2));  // ATTENTION!! La position est par rapport à l'image originale

	cvSetImageROI(imgGradientX, roi);
	imgTemp = GetROIImage(imgGradientX);
	cvResetImageROI(imgGradientX);

	projTemp.CreateHorizontalProjection(imgTemp);
	projTemp.Smooth(projectionSmooth);

	CvPoint ptRightEye;
	ptRightEye.x = rightEye.Position + rectEyeZone.x;
	ptRightEye.y = projTemp.MaxPeak.Position + rectEyeZone.y;

	cvCircle(imgOriginal, ptRightEye, 10, CV_RGB(255, 0, 0));


		
	// Création de l'image visuelle de la projection
	imgProjEyes = projEyes.CreateProjectionImage();

	//////////////////////////////////
	// fin détection zone des yeux
	//////////////////////////////////
	
	end = clock();
	double dif;

	dif = difftime(end, start) / CLOCKS_PER_SEC;

	printf("Temp d'execution : %.4f secondes", dif);

	////////////////////////////
	// Affichage des images
	////////////////////////////

	cvNamedWindow(winGradientX, CV_WINDOW_AUTOSIZE);
	cvShowImage(winGradientX, imgGradientX);

	cvNamedWindow(winGradientXY, CV_WINDOW_AUTOSIZE);
	cvShowImage(winGradientXY, imgGradientXY_U);

	cvNamedWindow(winGradientXY_Canny, CV_WINDOW_AUTOSIZE);
	cvShowImage(winGradientXY_Canny, imgGradientXY_U_Canny);

	cvNamedWindow(winGradientXY_Flood, CV_WINDOW_AUTOSIZE);
	cvShowImage(winGradientXY_Flood, imgGradientXY_U_Flood);

	cvNamedWindow(winGradientXY_Invert, CV_WINDOW_AUTOSIZE);
	cvShowImage(winGradientXY_Invert, imgGradientXY_U_Invert);

	cvNamedWindow(winGradientContourXY, CV_WINDOW_AUTOSIZE);
	cvShowImage(winGradientContourXY, imgGradientContourXY_U);


	cvNamedWindow(winSobel, CV_WINDOW_AUTOSIZE);
	cvShowImage(winSobel, imgProj);

	cvNamedWindow(winProjection, CV_WINDOW_AUTOSIZE);
	cvShowImage(winProjection, imgProjHorizontal);

	cvNamedWindow(winEyesGradient, CV_WINDOW_AUTOSIZE);
	cvShowImage(winEyesGradient, imgEyesGrad);	

	cvNamedWindow(winEyesProjection, CV_WINDOW_AUTOSIZE);
	cvShowImage(winEyesProjection, imgProjEyes);	

	cvNamedWindow(winOriginal);
	cvShowImage(winOriginal, imgOriginal);

	

	cvWaitKey();

	cvReleaseImage(&imgTemp);
	cvReleaseImage(&imgProjEyes);
	cvReleaseImage(&imgEyes);
	cvReleaseImage(&imgThreshold);
	cvReleaseImage(&imgSmooth);
	cvReleaseImage(&imgProj);
	cvReleaseImage(&imgProjHorizontal);
	cvReleaseImage(&imgGray);
	cvReleaseImage(&imgGradientX);
	cvReleaseImage(&imgGradientXY);
	cvReleaseImage(&imgGradientX_S);
	cvReleaseImage(&imgGradientY_S);
	cvReleaseImage(&imgGradientXY_U);
	cvReleaseImage(&imgGradientXY_U_Canny);
	cvReleaseImage(&imgGradientXY_U_Flood);
	cvReleaseImage(&imgGradientXY_U_Invert);
	cvReleaseImage(&imgOriginal);
	//cvReleaseImage(&imgLeftEye);
	//cvReleaseImage(&imgLeftEyeGradXY);
	cvReleaseImage(&imgLeftEyeGradXY_U);
	
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
