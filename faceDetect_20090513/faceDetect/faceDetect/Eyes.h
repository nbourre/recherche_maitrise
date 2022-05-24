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

	// M�thode qui regroupe d'autres sous-m�thode
	// pour faire la localisation
	void LocateEyes();

private:

	// M�thode retournant l'image du ROI;
	IplImage *getROIImage(IplImage * img);

	// M�thode permettant de faire le pr�traitement
	void preprocess();

	// M�thode permettant de faire la recherche d'un oeil
	// dans le ROI donn�.
	// isLeft : Vrai si oeil gauche
	void locateEye(CvRect roi, bool isLeft);
	

	int eyeWidth;
	
};
