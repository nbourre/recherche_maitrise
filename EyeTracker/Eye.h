#pragma once

#include "cv.h"
#include "cxcore.h"

class CEye
{
public:
	CvPoint2D32f Center;	// Centre de la pupille
	double PupilDiameter;	// Diamètre de la pupille
	double GlazeDirection;	// Direction du regard dans un l'un des cadran
							// visuelles (9 cadran)
	double OrientationX;	// Orientation sur l'axe des X
	double OrientationY;	// Orientation sur l'axe des y
	double OrientationZ;	// Orientation sur l'axe des z

	CEye(void);
	~CEye(void);
};
