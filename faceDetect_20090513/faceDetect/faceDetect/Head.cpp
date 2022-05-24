#include "Head.h"

CHead::CHead(void)
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;

	projection.Buffer = 0.15;
	projection.Range = 0.15;
}

CHead::~CHead(void)
{
}

void CHead::LocateHead(IplImage *img)
{
	// Création de la projection verticale
	projection.CreateVerticalProjection(img);

	// Lissage de la projection
	projection.Smooth(15);

	peak pkLeft;
	peak pkRight;

	pkLeft = projection.MaxPeak;
	pkRight = projection.FindNextPeak();

	// Triage des peaks
	if (pkLeft.Position > pkRight.Position)
	{
		peak tmp = pkLeft;
		pkLeft = pkRight;
		pkRight = tmp;
	}

	x = pkLeft.Position;
	width = pkRight.Position - x;
	
	y = 0;
	height = img->height;
}