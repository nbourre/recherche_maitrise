#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "face.h"
#include "eye.h"
#include "glint.h"

class CTrackingFunctions
{
public:
	CFace FindFace(IplImage *iplFrame);
	CFace TrackFace(CvCapture *Sequence);

	IplImage* FindGlints(IplImage *iplFrame, CGlint *Glints);
	IplImage* ThresholdImage(IplImage *iplFrame, int NumberOfThresholds);

	CTrackingFunctions(void);
	~CTrackingFunctions(void);

private:
	const static int MAX_LEVELS = 255;
	const static int DEPTH = 8;
	const static int NB_CHANNELS = 1;
};
