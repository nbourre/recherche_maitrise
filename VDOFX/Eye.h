#pragma once

#include "cv.h"
#include "cxcore.h"

class CEye
{
public:
	CEye(void);
	~CEye(void);

	// Box surrounding the eyes
	CvRect rectBox;
    CvRect rectBoxOffset;
	CvPoint ptCenter;
    CvPoint ptOffset;
	int iRadius;
    bool hasOffsett;

    static const int iBoxSide = 25;

    void adjustBox();
    void findEye(IplImage *iplInput);
    void drawBox(IplImage * iplTarget);
    void drawBoxWithOffset(IplImage * iplTarget);
    void setOffset(CvPoint ptNewOffset);
    
	
};
