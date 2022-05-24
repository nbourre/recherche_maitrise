#include "StdAfx.h"
#include ".\head.h"

CHead::CHead(void)
{
}

CHead::~CHead(void)
{
}

void CHead::searchHead(IplImage *iplInput, int iNeckHeight)
{
    int i;
	int j;
	int iValue;
	int iRight = -1;
	int iLeft = -1;
	int iTop = -1;
	int iBottom = iNeckHeight;

	bool bFound = false;

	// Right side
    for (i = 0; i < iplInput->widthStep; i++)
    {
		for (j = iNeckHeight; j < iplInput->height; j++)
	    {
            iValue = (uchar)iplInput->imageData[j * iplInput->widthStep + i];

			if (iValue > PIXMAX - 10)
			{
                bFound = true;
				iRight = i;
				break;
			}
        }

		if (bFound)
		{
			break;
		}			
    }

	bFound = false;


	// Left Side
	for (i = iplInput->widthStep; i > 0 ; i--)
    {
		for (j = iNeckHeight; j < iplInput->height; j++)
	    {
            iValue = (uchar)iplInput->imageData[j * iplInput->widthStep + i];

			if (iValue > PIXMAX - 10)
			{				
                bFound = true;
				iLeft = i;
				break;
			}
        }

		if (bFound)
		{
			break;
		}			
    }

	bFound = false;

	// Top side
	for (j = iplInput->height - 1; j > iNeckHeight ; j--)
	{	
		for (i = 0; i < iplInput->widthStep; i++)
		{
            iValue = (uchar)iplInput->imageData[j * iplInput->widthStep + i];

			if (iValue >= PIXMAX)
			{
				
                bFound = true;
				iTop = j;
				break;
			}
        }

		if (bFound)
		{
			break;
		}			
    }

	bFound = false;

	rectBox.x = iRight;
	rectBox.y = iNeckHeight;
	rectBox.width = iLeft - iRight;
	rectBox.height = iTop - iNeckHeight;

    _eyes.adjustEyesOffset(cvPoint(rectBox.x, rectBox.y));
}

void CHead::drawBox(IplImage *iplInput)
{
    cvRectangle(iplInput, cvPoint(rectBox.x, rectBox.y), 
	    cvPoint(rectBox.width + rectBox.x,
	    rectBox.height + rectBox.y), CV_RGB(255, 255, 255));

}

void CHead::setHeadROI(IplImage *iplInput)
{
    cvSetImageROI(iplInput, rectBox);
}

void CHead::adjustChildren()
{
    _eyes.adjustEyesOffset(cvPoint(rectBox.x, rectBox.y));
    
}