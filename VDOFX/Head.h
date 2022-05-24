#pragma once

#include "cv.h"
#include "cxcore.h"
#include "eyes.h"

class CHead
{
public:
	CHead(void);
	~CHead(void);

    static const int PIXMAX = 255;

	double dTilt;
	double dYaw;
	double dRoll;

	CvPoint ptCenter;

    CvRect rectBox;  // Enclosing box

    CEyes _eyes;

    void searchHead(IplImage *iplInput, int iNeckHeight);
    void drawBox(IplImage *iplInput);
    //void drawAll(IplImage *iplInput);
    void setHeadROI(IplImage *iplInput);
    void adjustChildren();
};
