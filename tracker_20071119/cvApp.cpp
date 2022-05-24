#include "stdafx.h"
#include "cvApp.h"

ImageProcessor *proc = 0;

void process (void *img)
{
	IplImage *image = reinterpret_cast<IplImage *>(img);
	cvErode(image, image, 0, 2);
}

void ImageProcessor::execute()
{
	process(img);
}
