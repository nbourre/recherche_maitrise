#pragma once

#include <cv.h>
#include <highgui.h>
#include "Projection.h"

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

	// Retourne le centre du ROI
	CvPoint GetCentre();

	// Localise les yeux dans l'image
	// Condition : Doit avoir un ROI pour la tête
	void LocateEyes(IplImage *img);
	CvRect GetEyeROI();
};
