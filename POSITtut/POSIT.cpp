// POSIT.cpp: implementation of the POSIT class.
//
//////////////////////////////////////////////////////////////////////

#include "POSIT.h"
#include <iostream>



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

POSIT::POSIT()
{

}

POSIT::~POSIT()
{
	cvReleasePOSITObject(&positObject);
	cvReleaseMat(&intrinsics);
}



void POSIT::initialize( const double &cubeSize,
							const double &aWidth, const double &aHeight,
							const double &nearPlane, const double &farPlane )
{	
	width = aWidth;
	height = aHeight;

	modelPoints.push_back(cvPoint3D32f(0.0f, 0.0f, 0.0f));
	modelPoints.push_back(cvPoint3D32f(0.0f, 0.0f, cubeSize));
	modelPoints.push_back(cvPoint3D32f(0.0f, cubeSize, cubeSize));
	modelPoints.push_back(cvPoint3D32f(0.0f, cubeSize, 0.0f));
	modelPoints.push_back(cvPoint3D32f(cubeSize, 0.0f, 0.0f));
	modelPoints.push_back(cvPoint3D32f(cubeSize, cubeSize, 0.0f));
	modelPoints.push_back(cvPoint3D32f(cubeSize, cubeSize, cubeSize));
	modelPoints.push_back(cvPoint3D32f(cubeSize, 0.0f, cubeSize));

	intrinsics = cvCreateMat( 3, 3, CV_64F );
	initializeIntrinsics( width, height);
	createOpenGLProjectionMatrix( width, height, nearPlane, farPlane );

	positObject = cvCreatePOSITObject( &modelPoints[0], (int)modelPoints.size() );
}
	

void POSIT::initializeIntrinsics( const double &aWidth, const double &aHeight )
{	
	width = aWidth;
	height = aHeight;
	cvSetZero( intrinsics );
	cvmSet( intrinsics , 0, 0, FOCAL_LENGTH );
	cvmSet( intrinsics , 1, 1, FOCAL_LENGTH );
	cvmSet( intrinsics , 0, 2, width * 0.5 );//principal point in the centre of the image
	cvmSet( intrinsics , 1, 2, height * 0.5 );
	cvmSet( intrinsics , 2, 2, 1.0 );
}


void POSIT::poseEstimation()
{
	CvMatr32f rotation_matrix = new float[9];
	CvVect32f translation_vector = new float[3];
	//set posit termination criteria: 100 max iterations, convergence epsilon 1.0e-5
	CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_EPS, 100, 1.0e-5);
	cvPOSIT( positObject, &imagePoints[0], FOCAL_LENGTH, criteria, rotation_matrix, translation_vector );	

	createOpenGLMatrixFrom( rotation_matrix, translation_vector);
	
	//Show the results
	#ifdef _DEBUG		
		cout << "\n\n-......- POSE ESTIMATED -......-\n";
		cout << "\n-.- MODEL POINTS -.-\n";
		for (int p=0; p<modelPoints.size(); p++ )
		{
			cout << modelPoints[p].x << ", " << modelPoints[p].y << ", " << modelPoints[p].z << "\n";
		}

		cout << "\n-.- IMAGE POINTS -.-\n";
		for ( p=0; p<modelPoints.size(); p++ )
		{
			cout << imagePoints[p].x << ", " << imagePoints[p].y << " \n";
		}

		cout << "\n-.- REAL POSE\n";
		for ( p=0; p<4; p++ )
		{		
			cout << poseReal[p] << " | " << poseReal[p+4] << " | " << poseReal[p+8] << " | " << poseReal[p+12] << "\n";
		}

		cout << "\n-.- ESTIMATED POSE\n";
		for ( p=0; p<4; p++ )
		{		
			cout << posePOSIT[p] << " | " << posePOSIT[p+4] << " | " << posePOSIT[p+8] << " | " << posePOSIT[p+12] << "\n";
		}		
	#endif

	delete [] rotation_matrix;
	delete [] translation_vector;
}


void POSIT::createOpenGLMatrixFrom(const CvMatr32f &rotationMatrix, const CvVect32f &translationVector)
{
	//coordinate system returned is relative to the first 3D input point	
	for (int f=0; f<3; f++)
	{
		for (int c=0; c<3; c++)
		{
			posePOSIT[c*4+f] = rotationMatrix[f*3+c];	//transposed
		}
	}	
	posePOSIT[3] = 0.0;
	posePOSIT[7] = 0.0;	
	posePOSIT[11] = 0.0;
	posePOSIT[12] = translationVector[0];
	posePOSIT[13] = translationVector[1]; 
	posePOSIT[14] = -translationVector[2]; //negative
	posePOSIT[15] = 1.0; //homogeneous	
}


void POSIT::createOpenGLProjectionMatrix( const double &width, const double &height,
									const double &nearPlane, const double &farPlane )
{
	projectionMatrix[0] = 2.0 * cvmGet( intrinsics, 0, 0 ) / width;
	projectionMatrix[1] = 0.0;
	projectionMatrix[2] = 0.0;
	projectionMatrix[3] = 0.0;

	projectionMatrix[4] = 0.0;
	projectionMatrix[5] = 2.0 * cvmGet( intrinsics, 1, 1 ) / height;
	projectionMatrix[6] = 0.0;
	projectionMatrix[7] = 0.0;
	
	projectionMatrix[8] = 2.0 * ( cvmGet( intrinsics, 0, 2 ) / width) - 1.0;
	projectionMatrix[9] = 2.0 * ( cvmGet( intrinsics, 1, 2 ) / height ) - 1.0;	
	projectionMatrix[10] = - ( farPlane+nearPlane ) / ( farPlane - nearPlane );
	projectionMatrix[11] = -1.0;

	projectionMatrix[12] = 0.0;
	projectionMatrix[13] = 0.0;
	projectionMatrix[14] = -2.0 * farPlane * nearPlane / ( farPlane - nearPlane );		
	projectionMatrix[15] = 0.0;
}


void POSIT::calculatImagePoints()
{
	/*The origin of the coordinates system is in the centre of the image
	*/
	imagePoints.clear();
	for ( int p=0; p<modelPoints.size(); p++ )
	{
		CvPoint3D32f point3D;
		//Transform the 3D points with the real pose
		point3D.x = poseReal[0] * modelPoints[p].x 
				+ poseReal[4]*modelPoints[p].y
				+ poseReal[8]*modelPoints[p].z;
		point3D.x = point3D.x + poseReal[12];
		
		point3D.y = poseReal[1] * modelPoints[p].x 
				+ poseReal[5]*modelPoints[p].y
				+ poseReal[9]*modelPoints[p].z;
		point3D.y = point3D.y + poseReal[13];

		point3D.z = poseReal[2] * modelPoints[p].x 
				+ poseReal[6]*modelPoints[p].y
				+ poseReal[10]*modelPoints[p].z;
		point3D.z = point3D.z + poseReal[14];

		CvPoint2D32f point2D;
		//Project the transformed 3D points
		point2D.x = cvmGet( intrinsics, 0, 0 ) * point3D.x / (-point3D.z); 
		point2D.y = cvmGet( intrinsics, 1, 1 ) * point3D.y / (-point3D.z);	

		imagePoints.push_back( point2D );
	}
}