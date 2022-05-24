#include "EyeROI.h"

CEyeROI::CEyeROI(void)
{
	projection.Buffer = 0.12;
	projection.Range = 0.12;
}

CEyeROI::~CEyeROI(void)
{
}

void CEyeROI::LocateEyes(IplImage *img, CvRect recFace)
{
	int zone;

	// Vérifier à créer un ROI pour la zone de la tête
	projection.CreateHorizontalProjection(img);
	projection.Smooth(15);

	zone = (int)(projection.Buffer * img->height);
	
	Rec.y = projection.MaxPeak.Position - zone;
	Rec.height = (projection.MaxPeak.Position + zone) - Rec.y;

	if ((recFace.x | recFace.y | recFace.width | recFace.height) == 0 )
	{
		Rec.x = 0;
		Rec.width = img->width;
	}
	else
	{
		Rec.x = recFace.x;
		Rec.width = recFace.width;
	}



}

CvPoint CEyeROI::GetCentre()
{
	CvPoint ptCentre;

	ptCentre.x = ((Rec.width + Rec.x) + Rec.x) / 2;
	ptCentre.y = ((Rec.y + Rec.height) + Rec.y) / 2;

	return ptCentre;
}

void CEyeROI::LocateEyes(IplImage *img)
{
	int zone;

	// Vérifier à créer un ROI pour la zone de la tête
	projection.CreateHorizontalProjection(img);
	projection.Smooth(15);

	zone = (int)(projection.Buffer * img->height);
	Rec.x = img->roi->xOffset;
	Rec.y = projection.MaxPeak.Position - zone;

	Rec.width = img->roi->width;
	Rec.height = (projection.MaxPeak.Position + zone) - Rec.y;

}

CvRect CEyeROI::GetEyeROI()
{
	return Rec;
}