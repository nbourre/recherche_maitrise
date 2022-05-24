#pragma once

#include <opencv2/opencv.hpp>
#include "Projection.h"

class CHead
{
public:
	CHead(void);
	~CHead(void);

	CProjection projection;
	int x;
	int y;
	int width;
	int height;
	
	// Localise la t�te dans l'image
	// Condition l'image doit �tre un gradient
	void LocateHead(IplImage *img);

	CvRect GetHeadRect()
	{
		return cvRect(x, y, width, height);
	};
};
