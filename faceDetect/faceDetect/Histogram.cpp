#include "Histogram.h"

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

