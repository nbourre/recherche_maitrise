#include "StdAfx.h"
#include ".\eyes.h"

CEyes::CEyes(void)
{
    blLEyeInitialized = false;
    blREyeInitialized = false;
    isAjusted = false;



}

CEyes::CEyes(IplImage * iplSource)
{
    blLEyeInitialized = false;
    blREyeInitialized = false;
    isAjusted = false;

    iplView = cvCreateImage( cvSize( iplSource->roi->width, iplSource->roi->height ), IPL_DEPTH_8U, 3 );

    cvZero(iplView);
    

}

CEyes::~CEyes(void)
{
}

void CEyes::SearchEyes(IplImage *iplInput)
{
    if (!(blLEyeInitialized || blREyeInitialized))
        findBlobs(iplInput);
    else
    {
                
    }
}

void CEyes::findBlobs(IplImage *iplInput)
{	
    // Extract the blobs using a threshold of 245 in the image
	blobs = CBlobResult(iplInput, NULL, iThresh, true);

	// create a file with some of the extracted features
	blobs.PrintBlobs( "c:\\tmp\\blobs.txt" );

	// discard the blobs with less area than 5000 pixels
	// ( the criteria to filter can be any class derived from COperadorBlob ) 
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, iMaximumArea);
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, iMinimumArea);

	// create a file with filtered results
	blobs.PrintBlobs( "c:\\tmp\\filteredBlobs.txt" );

	// build an output image equal to the input but with 3 channels (to draw the coloured blobs)
	
	iplView = cvCreateImage( cvSize( iplInput->roi->width, iplInput->roi->height ), IPL_DEPTH_8U, 3 );
    cvZero(iplView);

	cvSetErrMode(CV_ErrModeSilent);
	

	// plot the selected blobs in a output image
	int i;	

    if (blobs.GetNumBlobs() == 2)
    {
        CBlob blobTmp1;
        CBlob blobTmp2;

        int iX1;
        int iY1;
        int iX2;
        int iY2;
        
        blobs.GetNthBlob(CBlobGetPerimeter(), 0, blobTmp1);
        blobTmp1.FillBlob(iplView, CV_RGB(255, 0, 0));

        blobs.GetNthBlob(CBlobGetPerimeter(), 1, blobTmp2);
        blobTmp2.FillBlob(iplView, CV_RGB(0, 0, 255));

        iX1 = (blobTmp1.maxx - blobTmp1.minx) + blobTmp1.minx;
        iY1 = (blobTmp1.maxy - blobTmp1.miny) + blobTmp1.miny;

        iX2 = (blobTmp2.maxx - blobTmp2.minx) + blobTmp2.minx;
        iY2 = (blobTmp2.maxy - blobTmp2.miny) + blobTmp2.miny;

        blREyeInitialized = true;
        blLEyeInitialized = true;

        if (iX1 > iX2)
        {
            eyeLeft.ptCenter.x = iX1;
            eyeLeft.ptCenter.y = iY1;
            eyeLeft.iRadius = iX1 - blobTmp1.minx;
                    
            eyeRight.ptCenter.x = iX2;
            eyeRight.ptCenter.y = iY2;
            eyeRight.iRadius = iX2 - blobTmp2.minx;
        }
        else
        {
            eyeLeft.ptCenter.x = iX2;
            eyeLeft.ptCenter.y = iY2;
            eyeLeft.iRadius = iX2 - blobTmp2.minx;

            eyeRight.ptCenter.x = iX1;
            eyeRight.ptCenter.y = iY1;
            eyeRight.iRadius = iX1 - blobTmp1.minx;
        }

        eyeLeft.adjustBox();
        eyeRight.adjustBox();
    }
    else
    {
        CBlob blobTmp;

	    for (i = 0; i < blobs.GetNumBlobs(); i++)
	    {
		    blobs.GetNthBlob(CBlobGetPerimeter(), i, blobTmp);
		    blobTmp.FillBlob(iplView, CV_RGB(255, 0, 0));		
	    }
    }

    
}

void CEyes::drawEyes(IplImage *iplInput)
{
    if (!isAjusted)
    {

        if (blLEyeInitialized)
            eyeLeft.drawBox(iplInput);

        if (blREyeInitialized)
            eyeRight.drawBox(iplInput);
    }
    else
    {
        
        if (blLEyeInitialized)
            eyeLeft.drawBoxWithOffset(iplInput);

        if (blREyeInitialized)
            eyeRight.drawBoxWithOffset(iplInput);
    }
}

void CEyes::adjustEyesOffset(CvPoint ptOffset)
{
    eyeLeft.setOffset(ptOffset);
    eyeRight.setOffset(ptOffset);

    eyeRight.hasOffsett = true;
    eyeLeft.hasOffsett = true;
    isAjusted = true;
}

void CEyes::trackEyes(IplImage *iplInput)
{

}
