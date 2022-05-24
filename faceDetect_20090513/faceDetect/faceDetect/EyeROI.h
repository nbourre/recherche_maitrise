#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include "Projection.h"
#include "Histogram.h"

class CEyeROI
{
public:
	CEyeROI(void);
	~CEyeROI(void);

	CProjection projection;

	CvRect Rec;

	

	// Localise les yeux dans l'image
	// Condition : Aucune
	void LocateEyes(IplImage *img, CvRect recFace);

	float ComputeLeftEyeArea(IplImage *imgGray,CvPoint ptLeftEye);

	IplImage * GetROIImage(IplImage * img);

	// Retourne le centre du ROI
	CvPoint GetCentre();

	// Localise les yeux dans l'image
	// Condition : Doit avoir un ROI pour la tête
	void LocateEyes(IplImage *img);
	CvRect GetEyeROI();
};
