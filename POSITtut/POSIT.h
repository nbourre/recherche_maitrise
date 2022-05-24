/****************************************
*										*
*			OPENCV POSIT TUTORIAL		*
*		Javier Barandiaran Martirena	*
*			       2006					*
*										*
****************************************/

// POSIT.h: interface for the POSIT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSIT_H__E1A71555_791E_4091_9153_337EF16C8A8D__INCLUDED_)
#define AFX_POSIT_H__E1A71555_791E_4091_9153_337EF16C8A8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <cxcore.h>
#include <cv.h>
#include <GL/glut.h>
#include <vector>

using namespace std;

#define FOCAL_LENGTH 1200.0

class POSIT  
{
public:
	POSIT();
	virtual ~POSIT();

	void initialize( const double &cubeSize,
						const double &aWidth, const double &aHeight,
						const double &nearPlane, const double &farPlane );
	void initializeIntrinsics( const double &aWidth, const double &aHeight );
	void poseEstimation();
	void createOpenGLMatrixFrom( const CvMatr32f &rotationMatrix, const CvVect32f &translationVector);
	void createOpenGLProjectionMatrix(	const double &width, const double &height,
										const double &nearPlane, const double &farPlane );
	void calculatImagePoints();


	
	double width, height; //Image size
	CvMat *intrinsics;
	double posePOSIT[16];
	double poseReal[16];
	double projectionMatrix[16];

	vector<CvPoint3D32f> modelPoints;
	vector<CvPoint2D32f> imagePoints;

	CvPOSITObject* positObject;

};

#endif // !defined(AFX_POSIT_H__E1A71555_791E_4091_9153_337EF16C8A8D__INCLUDED_)
