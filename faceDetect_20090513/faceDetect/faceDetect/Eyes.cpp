#include "Eyes.h"

CEyes::CEyes()
{
	imgLeftEye = 0;
	imgRightEye = 0;
}

CEyes::CEyes(IplImage *imgEyesZone)
{
	imgEyes = imgEyesZone;
	imgLeftEye = 0;
	imgRightEye = 0;
}

CEyes::~CEyes(void)
{
	if (imgLeftEye != 0)
		cvReleaseImage(&imgLeftEye);
	if (imgRightEye != 0)
		cvReleaseImage(&imgRightEye);
}

//Fonction permettant d'extraire le ROI d'une image
IplImage *CEyes::getROIImage(IplImage *img)
{
	if (img->roi == NULL)
		return img;

	IplImage * imgOut;

	imgOut = cvCreateImage(cvSize(img->roi->width, img->roi->height), img->depth, img->nChannels);
	cvCopy(img, imgOut);

	return imgOut;
}

// Procédure permettant le prétraitement de l'image
void CEyes::preprocess()
{
	IplImage* imgEyesGrad;

	imgEyesGrad = cvCreateImage(cvGetSize(imgEyes), imgEyes->depth, imgEyes->nChannels);

	cvSobel(imgEyes, imgEyesGrad, 0, 2, 5);

	projEyes.CreateVerticalProjection(imgEyesGrad);
	projEyes.Smooth(15);

	LeftEye.x = projEyes.MaxPeak.Position;
	RightEye.x = projEyes.FindNextPeak().Position;

	if (LeftEye.x > RightEye.x)
	{
		CvPoint tmp;
		tmp = LeftEye;
		LeftEye = RightEye;
		RightEye = tmp;
	}

	eyeWidth = (int)(projEyes.Buffer * imgEyes->width) * 2;

	cvReleaseImage(&imgEyesGrad);

}

void CEyes::locateEye(CvRect roi, bool isLeft)
{
	IplImage *imgTemp;
	CProjection projTemp;

	cvSetImageROI(imgEyes, roi);
	imgTemp = getROIImage(imgEyes);
	cvResetImageROI(imgEyes);

	cvSobel(imgTemp, imgTemp, 2, 0, 5);
	
	projTemp.CreateHorizontalProjection(imgTemp);
	projTemp.Smooth(15);

	if (isLeft)
	{
		
		LeftEye.y = projTemp.MaxPeak.Position;

		if (imgLeftEye != 0)
			cvReleaseImage(&imgLeftEye);
		imgLeftEye = imgTemp;
		projLeftEye = projTemp;
	}
	else
	{
		RightEye.y = projTemp.MaxPeak.Position;

		if (imgRightEye != 0)
			cvReleaseImage(&imgRightEye);
		imgRightEye = imgTemp;
		projRightEye = projTemp;
	}
}

void CEyes::LocateEyes()
{
	CvRect roi;

	preprocess();

	// Oeil gauche
	roi.height = imgEyes->height;
	roi.width = eyeWidth;
	roi.x = LeftEye.x - (eyeWidth / 2);
	roi.y = 0;

	locateEye(roi, true);

	// Oeil droit
	roi.x = RightEye.x - (eyeWidth / 2);

	locateEye(roi, false);
}