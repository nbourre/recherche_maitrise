#pragma once

#include "cv.h"
#include "cxcore.h"

class CCalibration
{
public:
    CCalibration();
    ~CCalibration(void);

    

    void addImage(IplImage *iplT, IplImage *iplT_1);
    void initCalibration(IplImage *iplInitializer);
    IplImage * getBuffer();

private:

    IplImage *iplBuffer;
    IplImage *iplTemp;

    static const int iMovementThreshold = 15;
    bool IsInitialized;
    

};
