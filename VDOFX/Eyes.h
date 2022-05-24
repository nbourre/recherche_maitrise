#pragma once

#include "eye.h"
#include "cv.h"
#include "cxcore.h"
#include "blobresult.h"

class CEyes
{
public:
	CEyes(void);
    CEyes(IplImage * iplSource);
	~CEyes(void);

	CEye eyeLeft;
	CEye eyeRight;

    bool blREyeInitialized;
    bool blLEyeInitialized;
    bool isAjusted;

	IplImage * iplView;
    
	// Search for eyes in the ROI image.  If eyes are found, the values will be
	// in the eyeLeft and eyeRight properties.
	void SearchEyes(IplImage *iplInput);

    void drawEyes(IplImage *iplInput);

    void adjustEyesOffset(CvPoint ptOffset);

    void trackEyes(IplImage *iplInput);

private:
	// Look for blobs that could match eyes
	CBlobResult blobs;
	const static int iMinimumArea = 20;
	const static int iMaximumArea = 35;
    const static int iThresh = 245;
	void findBlobs(IplImage *iplInput);
};
