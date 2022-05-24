#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "time.h"
#include "BlobResult.h"

#define PIXMAX 255
#define PIXMIN 0

#define THRESH 225
#define MINNBPIX 45
#define MAXNBPIX 105
#define ROISIZE 15

class CGlint
{
public:
	bool Left;				// Vrai si celui de l'oeil gauche
	CGlint(void);
	~CGlint(void);

	bool FindInImage(IplImage * iplImg);
	void ResetFrameCount();

	int iThreshold;
	double dLastQuanta; // Quanta du temps d'utilisation du CPU
	int iNbWhitePoint;

	static CvPoint LeftCentre;	// Centre du glint gauche
	static CvPoint RightCentre;	// Centre du glint droit

	
	static CvPoint LeftAverage; // Calculer la moyenne de la position de l'oeil gauche
	static CvPoint RightAverage; // Calculer la moyenne de la position de l'oeil droit
	static int iNbFrameRight; // Nb frame qui possèdait un oeil droit
	static int iNbFrameLeft; // Nb frame qui possèdait un oeil gauche

private:
	CvPoint *points;
	CvRect rectROI;
	CvMemStorage* storageGlints;
	CvSeq* seqGlints;

	static CvPoint LeftSum; // Calculer la moyenne de la position de l'oeil gauche
	static CvPoint RightSum; // Calculer la moyenne de la position de l'oeil droit

	bool FindSequences(IplImage *iplImg);	

};

	
