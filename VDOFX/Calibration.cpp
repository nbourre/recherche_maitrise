#include "StdAfx.h"
#include ".\calibration.h"

CCalibration::CCalibration()
{
    IsInitialized = false;
}

CCalibration::~CCalibration(void)
{
    if (IsInitialized)
    {
        cvReleaseImage(&iplBuffer);
        cvReleaseImage(&iplTemp);
    }
}



void CCalibration::addImage(IplImage *iplT, IplImage *iplT_1)
{
    if (!IsInitialized)
        initCalibration(iplT);

    cvSub(iplT, iplT_1, iplTemp);
    cvThreshold(iplTemp, iplTemp, iMovementThreshold, 255, CV_THRESH_TOZERO);
    cvAdd(iplTemp, iplBuffer, iplBuffer);
}

void CCalibration::initCalibration(IplImage *iplInitializer)
{
    iplBuffer = cvCreateImage(cvSize(iplInitializer->width, iplInitializer->height), 
        iplInitializer->depth, 1);
    iplTemp = cvCreateImage(cvSize(iplInitializer->width, iplInitializer->height), 
        iplInitializer->depth, 1);

    cvZero(iplBuffer);

    IsInitialized = true;

}

IplImage * CCalibration::getBuffer()
{
    return iplBuffer;
}
