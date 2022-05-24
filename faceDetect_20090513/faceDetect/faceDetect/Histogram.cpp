#include "Histogram.h"
#include <opencv2/legacy/compat.hpp>

CHistogram::CHistogram(void)
{
	SizeHistogram = 256;

	float range_0[] = {0,256};
	float *ranges[] = {range_0};

	imgHistogram = cvCreateImage(cvSize(320, 200), 8, 1);
	Histogram = cvCreateHist(1, &SizeHistogram, CV_HIST_ARRAY, ranges, 1);
}

CHistogram::~CHistogram(void)
{
	cvReleaseHist(&Histogram);
}

void CHistogram::CreateHistogram()
{
	if (imgSource == 0)
	{
		throw "Histogram image not initialized!";
	}

	cvCalcHist(&imgSource, Histogram, 0, NULL);
	cvGetMinMaxHistValue(Histogram, &_minValue, &_maxValue, &_minPosition, &_maxPosition);
	cvScale(Histogram->bins, Histogram->bins, ((double)imgHistogram->height) /_maxValue, 0);

}

void CHistogram::CreateHistogram(IplImage *img)
{
	imgSource = img;

	CreateHistogram();

	RefreshHistoImg();
}

float CHistogram::GetMaxValue()
{
	return _maxValue;
}

int CHistogram::GetMaxPosition()
{
	return _maxPosition;
}

float CHistogram::GetMinValue()
{
	return _minValue;
}

int CHistogram::GetMinPosition()
{
	return _minPosition;
}

// Method that smooth the histogram
void CHistogram::SmoothHistogram(int size)
{
 
    CvHistogram *tmp = 0;
	float sum;
    int bins1 = SizeHistogram;
    int b;
	int i;
	int t = (size - 1)/2;

	int halfSize = (size - 1) / 2;

    
    sum = 0;
    cvCopyHist(Histogram, &tmp);

	for (b = 0; b < bins1; b++)
	{
		if (b <= halfSize)
		{
			for (i = 0; i < b + halfSize; ++i)
			{
				sum += cvQueryHistValue_1D(Histogram, i);
			}

			sum /= (halfSize + b);
		}
		else if (b > (bins1 - 2 - halfSize))
		{
			for (i = 0; i < (bins1 - b + halfSize); ++i)
				sum += cvQueryHistValue_1D(Histogram, b - halfSize + i);

			sum /= (bins1 - b + halfSize);
		}
		else
		{
			for(i = 0; i < size; i++)
			{
				sum += cvQueryHistValue_1D(Histogram, b - halfSize + i);            
			}

			sum /= size;

		}

		cvSetReal1D(tmp->bins, b, sum);
		sum = 0;
	}

    cvCopyHist(tmp, &Histogram);

	cvReleaseHist(&tmp);
}

// Method that determine the presence of glint from the histogram
// This means the left eye is open
int CHistogram::GlintHistogram(int nbBins)
{
	float sum;
    int  bins1 = SizeHistogram;
    int b;
  
    sum = 0;
 
	for (b = bins1-1; b > bins1-nbBins; b--)
	{
				sum += cvQueryHistValue_1D(Histogram, b);
				cvSetReal1D(Histogram->bins, b, 0.0);
	}

	printf("\n sum histo entre 245-255 = %20.15f",sum);

	if(sum > 1)
		return(1);
	else
		return(0);


}

//Function locating the maximum of an histogram 
//
void CHistogram::LocateExtremum(double *maxValue, double *minValue, double *hist,int bins1, int nbBins)
{
	int x, y, i, j;			// Indice de boucle
	int index;
	// Variable tampon utilisé dans le calcul des dérivé dans le but de diminuer le nombre
	//d'operation
	int jj = 0;
	double rayon2 = 0;
	double j2demidim = 0;
	float sigma = 2.0;
	int dimCote = (8*sigma+1);
	const int demiDim = (int)(dimCote)/2;
	double total2 = 0;
	double rsurds = 0;
	double sigmaCarre = (sigma*sigma);
	double deuxSigmaCarre = 2.0*sigmaCarre;
	double K = (1.0/(3.141592*sigmaCarre*sigmaCarre));
	double constante = 0;
	double KDprem;
	
	//Tableau servant a stocker le filtre de derive premiere
	double *mprem;

	KDprem = (-1.0*sqrt((double)2)/(sqrt((double)3.141592*sigma)*deuxSigmaCarre));

	mprem = (double *)malloc(sizeof(double)*dimCote);

	/* Remplissage du filtre de derivee premiere*/
	for(j = -demiDim; j <= demiDim; j++)
	{
		jj        = j*j;
		j2demidim = j+demiDim;
			
		index     = j2demidim;
		rayon2    = jj;
		rsurds    = rayon2/deuxSigmaCarre;
		constante = K*(1-rsurds)*exp(-rsurds);

		constante = KDprem * exp( (-(jj)) / deuxSigmaCarre);
		mprem[index] = constante * j ;
//printf("\n mprem = %20.12f %d",mprem[index],index);		
	}
	/*Fin remplissage du filtre*/
	


	/*Les partie du filtre sans valeur reelle sont mise a zero*/
	for( y=0;y<demiDim;++y)
	{	
		maxValue[y] = minValue[y]=0.0;
	}
	for( y=(bins1-demiDim);y<bins1;++y)
	{	
		maxValue[y] = minValue[y]= 0.0;
	}
	
	
	/*Fin de l'initialisation*/

	// Convolution du filtre de derivee
	for( y=demiDim; y < bins1-demiDim; ++y)
	{	
			//Début calcul dérivée
			total2 = 0;
			for(i=-demiDim;i<=demiDim;i++)
			{
				total2 += (mprem[demiDim + i] * hist[y+i]);
			}
			maxValue[y] = -(total2);
			//printf("\n total = %20.12lf %d",total2,y);
			//fin calcul dérive
	}
	//Fin de la convolution du filtre de dérivée

	//Locate max
	for( y=demiDim; y < bins1-demiDim; ++y)
	{	
//printf("\n min max y y+1??? = %20.12f %20.12f %d",maxValue[y],maxValue[y+1],y);
			if((maxValue[y] > 0.0)&& (maxValue[y+1] > 0.0))
			{
				maxValue[y] = minValue[y] = 0.0;
			}
			else if((maxValue[y] < 0.0)&& (maxValue[y+1] < 0.0))
			{
				maxValue[y] = minValue[y] = 0.0;
			}
			else if((maxValue[y] < 0.0)&& (maxValue[y+1] > 0.0))
			{
				minValue[y] = hist[y];
				maxValue[y] = 0.0;
				//printf("\n min = %20.12f %d",minValue[y],y);
			}
			else
			{
//printf("\n max = %20.12f %d",maxValue[y],y);
				maxValue[y] = hist[y];
				minValue[y] = 0.0;
				//printf("\n max = %20.12f %d",maxValue[y],y);
			}
	}
	free(mprem);
}


// Method that determine the value of the threshold separating the darker region (iris) 
// from the brighter region (pupil, glint, eyelid skin, sclera 
int CHistogram::ThreshHistogram(int nbBins)
{
	float maxT, minT;
	double mean,tot=0.0;
    int  bins1 = SizeHistogram;
    int b;
	int posMaxL, posMaxR;
	int thresh;

	double *maxValue;
	double *minValue;
	double *hist;

	if (imgSource == 0)
	{
		throw "Histogram image not initialized!";
	}

	mean = cvMean(imgSource,0);
	printf("\n mean = %20.12lf",mean);

	maxValue = (double *)malloc(sizeof(double)*bins1);
	minValue = (double *)malloc(sizeof(double)*bins1);
	hist = (double *)malloc(sizeof(double)*bins1);

	for (b = 0; b  < bins1; b++)
	{
		hist[b] = cvQueryHistValue_1D(Histogram, b);
		tot += hist[b];
	}
	mean =0.0;
	for (b = 0; b  < bins1; b++)
	{
		mean += (double)b*hist[b]/tot;
	}

	printf("\n mean sans glint = %20.12lf",mean);

	LocateExtremum(maxValue,minValue,hist,bins1,nbBins);

	// find max left of the mean => first mode darker regions
   
	maxT = 0.0;
	for (b = 0; b < (int)mean; b++)
	{
				if( maxT < maxValue[b])
				{
					maxT = maxValue[b];
					posMaxL = b;
				}
	}

	printf("\n left max = %20.12f %d",maxT,posMaxL);

	maxT = 0.0;
	for (b = bins1-1; b > (int) mean; b--)
	{
				if( maxT < maxValue[b])
				{
					maxT = maxValue[b];
					posMaxR = b;
				}
	}
	printf("\n right max = %20.12f %d",maxT,posMaxR);

	minT = imgSource->height * imgSource->width;
	for (b = posMaxL; b < posMaxR; b++)
	{
				if( (minT > minValue[b]) && (minValue[b] != 0.0))
				{
					minT = minValue[b];
					thresh = b;
				}
	}
	printf("\n  min  = %20.12f %d",minT,thresh);



	free(maxValue);
	free(minValue);
	free(hist);
	
	return(thresh);


}

// Method that determine the true pupil position 
//
CvPoint CHistogram::RefineEyeLocation(int nbBins)
{
	float maxArea, eyeArea;
	CvPoint posGlintMax;
    int  bins1 = SizeHistogram;
	IplImage *img;
	IplImage *imgTemp; // NB 20090723

	if (imgSource == 0)
	{
		throw "Histogram image not initialized!";
	}
	printf("\n dim IMG SOURCE   = %d %d",imgSource->width,imgSource->height);
	img = cvCreateImage(cvSize(imgSource->width, imgSource->height), imgSource->depth, imgSource->nChannels);
	cvCopy(imgSource, img);
	imgTemp = cvCloneImage(img);	
		
	printf("\n nbBins  = %d", bins1 - nbBins);
	
	winCapture = "HIST";

	cvNamedWindow(winCapture, CV_WINDOW_AUTOSIZE);

	//cvShowImage(winCapture, img);
	//cvWaitKey(1);

	cvThreshold(img, img, bins1 - nbBins, 255, CV_THRESH_BINARY);
	//cvShowImage(winCapture, img);
	//cvWaitKey(1);

	cvDilate(img, img,0,1);

	//cvShowImage(winCapture, img);
	//cvWaitKey();

	/*cvFloodFill( img, cvPoint(0,0), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);
	cvFloodFill( img, cvPoint(0,img->height-1), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);

	cvFloodFill( img, cvPoint(img->width-1,0), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);

	cvFloodFill( img, cvPoint(img->width-1,img->height-1), CV_RGB( 0, 0, 0), CV_RGB( 0,0,0 ),
                             CV_RGB( 0, 0, 0 ),0, 4, 0);*/

	// Fusion de l'oeil original et de la position du glint.
	// Ajout NB 20090723
	cvAdd(img, imgSource, imgTemp);
	cvFlip(imgTemp, imgTemp);
	cvShowImage(winCapture, imgTemp);
	//cvWaitKey(1);

	CvMemStorage* stor;
	CvSeq* cont;
	CvPoint* PointArray;
	CvPoint2D32f* PointArray2D32f;
    
    // Create dynamic structure and sequence.
	stor = cvCreateMemStorage(0);
	cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor);
    

           // Find all contours.
    cvFindContours( img, stor, &cont, sizeof(CvContour), 
                    CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    
   
 
     // Search all possible glints.
	 maxArea = 0;
     for(;cont;cont = cont->h_next)
     {  
		int i; // Indicator of cycle.
        int count = cont->total; // This is number point in contour
        CvPoint center;
        CvSize size;
        
        // Number point must be more than or equal to 6 (for cvFitEllipse_32f).        
        if( count < 6 )
             continue;
		eyeArea = fabs(cvContourArea(cont,CV_WHOLE_SEQ));
		if(eyeArea > maxArea)
		{
			int maxX,minX,maxY,minY;

			maxArea = eyeArea;
				
		    printf("\n contour number glint = %d",cont->total);
		    printf("\n Glint Area = %lf",eyeArea);
        
        // Alloc memory for contour point set.    
            PointArray = (CvPoint*)malloc( count*sizeof(CvPoint) );
            PointArray2D32f= (CvPoint2D32f*)malloc( count*sizeof(CvPoint2D32f) );
        
        // Get contour point set.
            cvCvtSeqToArray(cont, PointArray, CV_WHOLE_SEQ);
        
        // Convert CvPoint set to CvBox2D32f set.
			minX = img->width;
			minY = img->height;
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
			posGlintMax.x = minX+(maxX-minX)/2.0;
			posGlintMax.y = minY+(maxY-minY)/2.0;

			free(PointArray);
            free(PointArray2D32f);
			
		}
	 }   
	 cvReleaseMemStorage(&stor);
     printf("\n refine EYE ROI position = %d %d", posGlintMax.x, posGlintMax.y);

	//posGlintMax.x = img->roi->xOffset + posGlintMax.x;
	//posGlintMax.y = img->roi->yOffset + posGlintMax.y;
		
	 printf("\n refine EYE Image position = %d %d", posGlintMax.x, posGlintMax.y);

	 cvReleaseImage(&imgTemp);
	 cvReleaseImage(&img);
	 return(posGlintMax);


}


void CHistogram::RefreshHistoImg()
{	int bin_width;
	int i;

	cvSet (imgHistogram, cvScalarAll(255), 0);

	bin_width = cvRound((double)imgHistogram->width/SizeHistogram);

	for (i = 0; i < SizeHistogram; i++)
		cvRectangle(imgHistogram, cvPoint(i * bin_width, imgHistogram->height),
			cvPoint ((i+1) * bin_width, imgHistogram->height - cvRound (cvGetReal1D(Histogram->bins, i))),
			cvScalarAll(0), -1, 8, 0);
}

