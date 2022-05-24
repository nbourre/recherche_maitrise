#pragma once

#include <cxcore.h>
#include <cv.h>
#include <GL/glut.h>
#include <vector>


using namespace std;

#define FOCAL_LENGTH 1200.0

void initializePOSIT( const double &cubeSize );
void poseEstimationPOSIT();
void createOpenGLMatrixFromPOSIT(double ogl[16], const CvMatr32f &rotationMatrix, const CvVect32f &translationVector);
void createOpenGLProjectionMatrix( double projectionMatrix[16],
									const CvMat &intrinsics,
									const double &width, const double &height,
									const double &nearPlane, const double &farPlane );








