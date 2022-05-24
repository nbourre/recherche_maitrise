#if !defined IMAGEPROCESSOR
#define IMAGEPROCESSOR

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cv.h"      // include core library interface
#include "highgui.h" // include GUI library interface

class ImageProcessor {

	IplImage* img; // Declare IPL/OpenCV image pointer
public:

	ImageProcessor(CString filename, bool display=true) 
	{
		img = cvvLoadImage( filename ); // load image
		if (display) 
		{
			// create a window
			cvvNamedWindow( "Original Image", 1 );
			// display the image on window
			cvvShowImage( "Original Image", img ); 
		}
	}

	void display()
	{
		cvvNamedWindow("Resulting Image", 1);
		cvvShowImage("Resulting Image", img);
	}

	void execute();


	~ImageProcessor() 
	{
		cvReleaseImage( &img );
	}

};

extern ImageProcessor *proc;
#endif