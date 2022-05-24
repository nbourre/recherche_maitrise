#pragma once

#include "cv.h"
#include "cxcore.h"

class CFace
{
public:
	double orientation;
	CvBox2D32f Region;

	CFace(void);
	~CFace(void);
};
