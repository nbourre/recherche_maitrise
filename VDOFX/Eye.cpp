#include "StdAfx.h"
#include ".\eye.h"

CEye::CEye(void)
{
    rectBox.height = iBoxSide;
    rectBox.width = iBoxSide;
}

CEye::~CEye(void)
{
}

void CEye::adjustBox()
{
    rectBox.x = ptCenter.x - (iBoxSide / 2);
    rectBox.y = ptCenter.y - (iBoxSide / 2);
}

void CEye::setOffset(CvPoint ptNewOffset)
{
    ptOffset.x = ptNewOffset.x;
    ptOffset.y = ptNewOffset.y;

    rectBoxOffset.height = rectBox.height;
    rectBoxOffset.width = rectBox.width;
    rectBoxOffset.x = rectBox.x + ptOffset.x;
    rectBoxOffset.y = rectBox.y + ptOffset.y;
}

void CEye::drawBox(IplImage * iplTarget)
{
    if (ptCenter.x < iplTarget->width && ptCenter.y < iplTarget->height)
    {
        CvPoint ptTemp;
        CvPoint ptTemp2;

        ptTemp.x = rectBox.x;
        ptTemp.y = rectBox.y;

        ptTemp2.x = ptTemp.x + iBoxSide;
        ptTemp2.y = ptTemp.y + iBoxSide;
        
        cvRectangle( iplTarget, ptTemp, ptTemp2, CV_RGB(255, 255, 255), 1, 8, 0);
    }

}

void CEye::drawBoxWithOffset(IplImage * iplTarget)
{
    if ((ptCenter.x + ptOffset.x) < iplTarget->width && (ptCenter.y + ptOffset.y) < iplTarget->height)
    {
        CvPoint ptTemp;
        CvPoint ptTemp2;

        ptTemp.x = rectBox.x + ptOffset.x;
        ptTemp.y = rectBox.y + ptOffset.y;

        ptTemp2.x = ptTemp.x + iBoxSide;
        ptTemp2.y = ptTemp.y + iBoxSide;
        
        cvRectangle( iplTarget, ptTemp, ptTemp2, CV_RGB(255, 255, 255), 1, 8, 0);
    }
}

/// Find the eye within an image with a ROI
void CEye::findEye(IplImage *iplInput)
{
    cvSetImageROI( iplInput, rectBoxOffset);

    cvThreshold (iplInput, iplInput, 245, 255, CV_THRESH_BINARY);

    int i;
    int j;
    
    // Limite du ROI
    int rightLimit = rectBoxOffset.x + iBoxSide;
    int bottomLimit = rectBoxOffset.y + iBoxSide;

    // Limite de la pupile
    int leftSide = 0;
    int rightSide = 0;
    int top = 0;
    int bottom = 0;
    int iValue;

    CvPoint realCenter;

    realCenter.x = rectBoxOffset.x + ptCenter.x;
    realCenter.y = rectBoxOffset.y + ptCenter.y;    

    // Trouver la limite supérieur de la pupille
    for (j = rectBoxOffset.y + 1; j < bottomLimit - 1; j++)
    {
        for (i = rectBoxOffset.x + 1; i < rightLimit - 1; i++)
        {
            iValue = (uchar)iplInput->imageData[j * iplInput->widthStep + i];

            if (iValue > 245)
            {
                top = j;
                break;
            }
        }

        if (top > 0)
            break;
    }

    // Trouver la limite inférieure
    for (j = bottomLimit - 1; j > top + 1; j--)
    {
        for (i = rectBoxOffset.x + 1; i < rightLimit - 1; i++)
        {
            iValue = (uchar)iplInput->imageData[j * iplInput->widthStep + i];

            if (iValue > 245)
            {
                bottom = j;
                break;
            }
        }

        if (bottom > 0)
            break;
    }



    //ptCenter.x = (leftSide + rightSide) / 2;
    //rectBox.y = (top + bottom) / 2;
    rectBoxOffset.y = (top + bottom) / 2;


    cvResetImageROI( iplInput);
}