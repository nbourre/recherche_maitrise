#pragma once
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>

class CHistogram
{
public:
	char *winCapture;

	CHistogram(void);
	~CHistogram(void);

	IplImage * imgHistogram;
	IplImage * imgSource;

	int SizeHistogram;
	CvHistogram * Histogram;

	void CreateHistogram();
	void CreateHistogram(IplImage *img);

	float GetMaxValue();
	int GetMaxPosition();

	float GetMinValue();
	int GetMinPosition();

	int GlintHistogram(int nbBins);

	int ThreshHistogram(int nbBins);

	void LocateExtremum(double *maxValue, double *minValue,double *hist,int bins1, int nbBins);

	CvPoint RefineEyeLocation(int nbBins);
	
	void SmoothHistogram(int filterSize);
	void RefreshHistoImg();

private:

	float _maxValue;
	int _maxPosition;

	float _minValue;
	int _minPosition;


	
};
