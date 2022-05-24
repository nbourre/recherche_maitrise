#include "POSITtut.h"

void initializePOSIT( const double &cubeSize )
{	
	modelPoints.push_back(cvPoint3D32f(0.0f, 0.0f, 0.0f));
	modelPoints.push_back(cvPoint3D32f(0.0f, 0.0f, cubeSize));//
	modelPoints.push_back(cvPoint3D32f(0.0f, cubeSize, cubeSize));
	modelPoints.push_back(cvPoint3D32f(0.0f, cubeSize, 0.0f));//
	modelPoints.push_back(cvPoint3D32f(cubeSize, cubeSize, 0.0f));
	modelPoints.push_back(cvPoint3D32f(cubeSize, cubeSize, cubeSize));//
	modelPoints.push_back(cvPoint3D32f(cubeSize, 0.0f, cubeSize));
	posit = cvCreatePOSITObject( &modelPoints[0], (int)modelPoints.size() );
}
	
void initializeIntrinsics()
{
	intrinsics = cvCreateMat( 3, 3, CV_64F );
	cvSetZero( &intrinsics );
	cvmSet( intrinsics , 0, 0, FOCAL_LENGTH );
	cvmSet( intrinsics , 1, 1, FOCAL_LENGTH );
	cvmSet( intrinsics , 0, 2, 320.0);//principal point in the centre of the image
	cvmSet( intrinsics , 1, 2, 240.0);
	cvmSet( intrinsics , 2, 2, 1.0 );
}


void finish()
{
	cvReleasePOSITObject(&posit);
	cvReleaseMat(&intrinsics);
}


void poseEstimationPOSIT()
{
	CvMatr32f rotation_matrix = new float[9];
	CvVect32f translation_vector = new float[3];
	//set posit termination criteria: 100 max iterations, convergence epsilon 1.0e-5
	CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_EPS, 100, 1.0e-5);
	cvPOSIT( posit, &imagePoints[0], FOCAL_LENGTH, criteria, rotation_matrix, translation_vector );	

	createOpenGLMatrixFromPOSIT( posePOSIT, rotation_matrix, translation_vector);
		
	delete [] rotation_matrix;
	delete [] translation_vector;
}


void createOpenGLMatrixFromPOSIT(double ogl[16], const CvMatr32f &rotationMatrix, const CvVect32f &translationVector)
{
	//coordinate system returned is relative to the first 3D input point	
	for (int f=0; f<3; f++)
	{
		for (int c=0; c<3; c++)
		{
			ogl[c*4+f] = rotationMatrix[f*3+c];	//transposed
		}
	}	
	ogl[3] = 0.0;
	ogl[7] = 0.0;	
	ogl[11] = 0.0;
	ogl[12] = translationVector[0];
	ogl[13] = translationVector[1]; 
	ogl[14] = -translationVector[2]; //negative
	ogl[15] = 1.0; //homogeneous	
}

void createOpenGLProjectionMatrix( double projectionMatrix[16],
									const CvMat &intrinsics,
									const double &width, const double &height,
									const double &nearPlane, const double &farPlane )
{
	projectionMatrix[0] = 2.0 * cvmGet( &intrinsics, 0, 0 ) / width;
	projectionMatrix[1] = 0.0;
	projectionMatrix[2] = 0.0;
	projectionMatrix[3] = 0.0;

	projectionMatrix[4] = 0.0;
	projectionMatrix[5] = 2.0 * cvmGet( &intrinsics, 1, 1 ) / height;
	projectionMatrix[6] = 0.0;
	projectionMatrix[7] = 0.0;
	
	projectionMatrix[8] = 2.0 * ( cvmGet( &intrinsics, 0, 2 ) / width) - 1.0;
	projectionMatrix[9] = 2.0 * ( cvmGet( &intrinsics, 1, 2 ) / height ) - 1.0;	
	projectionMatrix[10] = - ( farPlane+nearPlane ) / ( farPlane - nearPlane );
	projectionMatrix[11] = -1.0;

	projectionMatrix[12] = 0.0;
	projectionMatrix[13] = 0.0;
	projectionMatrix[14] = -2.0 * farPlane * nearPlane / ( farPlane - nearPlane );		
	projectionMatrix[15] = 0.0;
}