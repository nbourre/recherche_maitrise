#pragma once
#include <cv.h>
#include <highgui.h>

class CHistogram
{
public:
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

	void SmoothHistogram(int filterSize);
	void RefreshHistoImg();

private:

	float _maxValue;
	int _maxPosition;

	float _minValue;
	int _minPosition;


	
};
