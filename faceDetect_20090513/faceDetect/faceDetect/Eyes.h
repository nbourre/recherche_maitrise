#pragma once
#include <cv.h>
#include <highgui.h>
#include "Projection.h"

class CEyes
{
public:
	CEyes();
	CEyes(IplImage *imgEyesZone);
	~CEyes(void);

	int x;
	int y;
	IplImage * imgEyes;

	IplImage * imgLeftEye;
	IplImage * imgRightEye;

	CProjection projEyes;
	CProjection projLeftEye;
	CProjection projRightEye;

	CvPoint LeftEye;
	CvPoint RightEye;

	// Méthode qui regroupe d'autres sous-méthode
	// pour faire la localisation
	void LocateEyes();

private:

	// Méthode retournant l'image du ROI;
	IplImage *getROIImage(IplImage * img);

	// Méthode permettant de faire le prétraitement
	void preprocess();

	// Méthode permettant de faire la recherche d'un oeil
	// dans le ROI donné.
	// isLeft : Vrai si oeil gauche
	void locateEye(CvRect roi, bool isLeft);
	

	int eyeWidth;
	
};
