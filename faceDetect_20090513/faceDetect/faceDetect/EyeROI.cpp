#include "EyeROI.h"

CEyeROI::CEyeROI(void)
{
	projection.Buffer = 0.12;
	projection.Range = 0.12;
}

CEyeROI::~CEyeROI(void)
{
}

void CEyeROI::LocateEyes(IplImage *img, CvRect recFace)
{
	int zone;

	// Vérifier à créer un ROI pour la zone de la tête
	projection.CreateHorizontalProjection(img);
	projection.Smooth(15);

	zone = (int)(projection.Buffer * img->height);
	
	Rec.y = projection.MaxPeak.Position - zone;

	//printf("\n rec.y = %d  img height = %d ",Rec.y,img->height);
	//if(Rec.y < 0)
	//	Rec.y = 0;
	Rec.height = (projection.MaxPeak.Position + zone) - Rec.y;

	if ((recFace.x | recFace.y | recFace.width | recFace.height) == 0 )
	{
		Rec.x = 0;
		Rec.width = img->width;
	}
	else
	{
		Rec.x = recFace.x;
		Rec.width = recFace.width;
	}



}

CvPoint CEyeROI::GetCentre()
{
	CvPoint ptCentre;

	ptCentre.x = ((Rec.width + Rec.x) + Rec.x) / 2;
	ptCentre.y = ((Rec.y + Rec.height) + Rec.y) / 2;

	return ptCentre;
}

void CEyeROI::LocateEyes(IplImage *img)
{
	int zone;

	// Vérifier à créer un ROI pour la zone de la tête
	projection.CreateHorizontalProjection(img);
	projection.Smooth(15);

	zone = (int)(projection.Buffer * img->height);
	Rec.x = img->roi->xOffset;
	Rec.y = projection.MaxPeak.Position - zone;

	Rec.width = img->roi->width;
	Rec.height = (projection.MaxPeak.Position + zone) - Rec.y;

}

float CEyeROI::ComputeLeftEyeArea(IplImage *imgGray,CvPoint ptLeftEye)
{
	CHistogram hist;
	float leftEyeArea, leftEyeAreaMax;
	float elongX,elongY;
	int iOffsetX = 40; // 20
	int iOffsetY = 30; // 20

	char *winHisto = "HISTOGRAMME";


	char *winBlobLeftEye = "Blob Left Eye";

	cvResetImageROI(imgGray);
	//printf("\n CEyeROI ptLeftEye.y - iOffset + 60 = %d %d",ptLeftEye.y - iOffsetY+60,imgGray->height);
	if((ptLeftEye.y - iOffsetY + 60) > imgGray->height)
		cvSetImageROI(imgGray, cvRect(ptLeftEye.x - iOffsetX, ptLeftEye.y-iOffsetY, 80, imgGray->height-(ptLeftEye.y - iOffsetY)-1)); //, 40,40
	else
		cvSetImageROI(imgGray, cvRect(ptLeftEye.x - iOffsetX, ptLeftEye.y-iOffsetY, 80, 60)); //, 40,40
		
	IplImage * imgTemp = GetROIImage(imgGray);
	/*cvFlip(imgTemp, imgTemp, 0);
	cvNamedWindow(winCapture, CV_WINDOW_AUTOSIZE);
	cvShowImage(winCapture, imgTemp);
	cvWaitKey();
	cvFlip(imgTemp, imgTemp, 0);*/

	hist.imgSource = imgTemp;
	hist.CreateHistogram();
	hist.SmoothHistogram(15);
	hist.RefreshHistoImg();


		//cvNamedWindow(winHisto, CV_WINDOW_AUTOSIZE);
		//cvShowImage(winHisto, hist.imgHistogram);
		//cvWaitKey(1);
		
		
	//printf("\n CEyeROI mean = %20.12lf",cvMean(imgTemp,0));
	if(hist.GlintHistogram(20))
	{
		//printf("\n CEyeROI Left Eye Open");
		CvPoint posGlint = hist.RefineEyeLocation(20);

		posGlint.x = imgGray->roi->xOffset + posGlint.x;
		posGlint.y = imgGray->roi->yOffset + posGlint.y;


		cvResetImageROI(imgGray);
		//printf("\n CEyeROI posGlint.y - iOffset + 60 = %d %d",posGlint.y - iOffsetY+60,imgGray->height);
		if((posGlint.y - iOffsetY + 60) > imgGray->height)
			cvSetImageROI(imgGray, cvRect(posGlint.x - iOffsetX, posGlint.y-iOffsetY, 80, imgGray->height-(posGlint.y - iOffsetY)-1)); //, 40,40
		else
			cvSetImageROI(imgGray, cvRect(posGlint.x - iOffsetX, posGlint.y-iOffsetY, 80, 60)); //, 40,40
		
		cvReleaseImage(&imgTemp);
		IplImage * imgTemp = GetROIImage(imgGray);
		hist.imgSource = imgTemp;

	/*cvFlip(imgTemp, imgTemp, 0);
		cvNamedWindow(winHisto, CV_WINDOW_AUTOSIZE);
		cvShowImage(winHisto, imgTemp);
		cvWaitKey();
		cvFlip(imgTemp, imgTemp, 0);*/



		int thresh = hist.ThreshHistogram(20);
		//printf("\n CEyeROI T = %d",thresh);

		cvThreshold(imgTemp, imgTemp, thresh, 255, CV_THRESH_BINARY);
		if((unsigned char)imgTemp->imageData[0] == 255)
			cvFloodFill( imgTemp, cvPoint(0,0), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);
		if((unsigned char)imgTemp->imageData[(imgTemp->height-1)*imgTemp->widthStep] == 255)
			cvFloodFill( imgTemp, cvPoint(0,imgTemp->height-1), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);
		if((unsigned char)imgTemp->imageData[imgTemp->width-1] == 255)
			cvFloodFill( imgTemp, cvPoint(imgTemp->width-1,0), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);

		if((unsigned char)  imgTemp->imageData[(imgTemp->height-1)*imgTemp->widthStep+imgTemp->widthStep-1] == 255)
			cvFloodFill( imgTemp, cvPoint(imgTemp->width-1,imgTemp->height-1), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);

		cvDilate(imgTemp,imgTemp,0,1);
		cvErode(imgTemp,imgTemp,0,1);

		//cvShowImage(winBlobLeftEye, imgTemp);
		//cvWaitKey(1);

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
		leftEyeAreaMax= 0.0;
        for(;cont;cont = cont->h_next)
        { 
            int i; // Indicator of cycle.
            int count = cont->total; // This is number point in contour
            CvPoint center;
            CvSize size;
        
            // Number point must be more than or equal to 6 (for cvFitEllipse_32f). 
//printf("\n CEyeROI count = %d",count);

			if( count < 20 ) //10
                continue;
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

				elongX = (float)((maxX-minX)/(maxY-minY));
				elongY = (float)((maxY-minY)/(maxX-minX));
	//printf("\n CEyeROI elongX = %20.10lf elongy = %20.10lf",elongX,elongY);

				 if((elongX > 3.0) || 
					 (elongY > 3.0)) //2
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

	//cvShowImage(winBlobLeftEye, imgTemp);
	//cvWaitKey(0);
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
//    cvShowImage(winBlobLeftEye, imgTemp);
//	cvWaitKey();
        
        // Free memory.          
                  free(PointArray);
                  free(PointArray2D32f);
                  free(box);
			   }
			   else
				   continue;
             }
			leftEyeArea = leftEyeAreaMax;
		 //printf("\n CEyeROI Area MAX = %lf",leftEyeArea);
			cvReleaseMemStorage(&stor);
			free(cont);

//cvLine(hist.imgHistogram,cvPoint(thresh,0),cvPoint(thresh,hist.imgHistogram->height),CV_RGB(255,255,255),3,8,0);
//	cvNamedWindow(winHisto, CV_WINDOW_AUTOSIZE);
//		cvShowImage(winHisto, hist.imgHistogram);
//		cvWaitKey();
	}
	else{
		//printf("\n CEyeROI Left Eye Close");
		leftEyeArea = 0.0;
	}

	return leftEyeArea;

}

/// Fonction qui retourne l'image d'un ROI
IplImage * CEyeROI::GetROIImage(IplImage * img)
{
	if (img->roi == NULL)
		return img;

	IplImage * imgOut;

	imgOut = cvCreateImage(cvSize(img->roi->width, img->roi->height), img->depth, img->nChannels);
	cvCopy(img, imgOut);

	return imgOut;
}

CvRect CEyeROI::GetEyeROI()
{
	return Rec;
}