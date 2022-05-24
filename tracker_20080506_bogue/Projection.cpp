#include "StdAfx.h"
#include "Projection.h"

CProjection::CProjection(void)
{
	isInitialized = false;
	Range = 0.1;
	MaxPeakSmooth.Position = 0;
	MaxPeakSmooth.Value = 0;
	buffer = 0.1;
}

CProjection::~CProjection(void)
{
	isInitialized = false;
}

CProjection::CProjection(IplImage *img)
{
	isInitialized = false;
	Range = 0.1;
	MaxPeakSmooth.Position = 0;
	MaxPeakSmooth.Value = 0;
	CreateHorizontalProjection(img);
	buffer = 0.1;
}

/// Retourne l'image représentant la projection
IplImage * CProjection::CreateProjectionImage()
{
	IplImage * imgRet = 0;
	
	int i, j;
	double ratio;
	double maxUpperRange, maxLowerRange;
	int width, height;

	if (MaxPeak.Value > 0)
	{
		if (isHorizontal)
		{
			width = rectProj.width;
			height = Count;
			
			imgRet = cvCreateImage(cvSize(width, height), 8, 1);
			cvZero(imgRet);

			ratio = width / MaxPeak.Value;
			maxUpperRange = MaxPeak.Position + Range * height;
			maxLowerRange = MaxPeak.Position - Range * height;	

			for (j = 0; j < height; j++)
			{
				if (j >= maxLowerRange && j <= maxUpperRange)
					cvLine(imgRet, cvPoint(0, j), cvPoint(ratio * Values[j].Value, j), CV_RGB(255, 255, 255));
				else
					cvLine(imgRet, cvPoint(0, j), cvPoint(ratio * Values[j].Value, j), CV_RGB(127, 127, 127));
			}
		}
		else
		{
			width = Count;
			height = rectProj.height;

			imgRet = cvCreateImage(cvSize(width, height), 8, 1);
			cvZero(imgRet);

			ratio = height / MaxPeak.Value;
			maxUpperRange = MaxPeak.Position + Range * width;
			maxLowerRange = MaxPeak.Position - Range * width;	

			for (i = 0; i < width; i++)
			{
				
				if (i >= maxLowerRange && i <= maxUpperRange)
					cvLine(imgRet, cvPoint(i, 0), cvPoint(i, ratio * Values[i].Value), CV_RGB(255, 255, 255));
				else
					cvLine(imgRet, cvPoint(i, 0), cvPoint(i, ratio * Values[i].Value), CV_RGB(127, 127, 127));
			}
		}

	}
	return imgRet;
}

/// Méthode permettant de créer une sommation des niveaux de gris
/// a l'horizontale d'une image
void CProjection::CreateHorizontalProjection(IplImage *img)
{
	int i, j;

	int xStart, yStart;
	int xEnd, yEnd;
	int hSum;
	int nbElements;

	int maxValue = 0;
	int maxPos = 0;
	int totalSum = 0;

	//peaks.clear();
	
	if (img->roi != NULL)
	{
		xStart = img->roi->xOffset;
		yStart = img->roi->yOffset;

		xEnd = img->roi->width + xStart;
		yEnd = img->roi->height + yStart;
		nbElements = img->roi->height - yStart;		
	}
	else
	{
		xStart = 0;
		yStart = 0;
		xEnd = img->width;
		yEnd = img->height;
		nbElements = img->height;
	}



	if (!isInitialized)
	{
		setSize(nbElements);
		isInitialized = true;
	}


	for (j = yStart; j < yEnd; j++)
	{
		hSum = 0;
		for (i = xStart; i < xEnd; i++)
		{
			hSum += (int)img->imageData[j * img->width + i];
		}

		if (hSum > maxValue)
		{
			maxValue = hSum;
			maxPos = j - yStart;
		}
		Values[j - yStart].Value = hSum;
		Values[j - yStart].Position = j;
		Values[j - yStart].NearMax = false;
		totalSum += hSum;
	}

	rectProj.height = (maxPos + Range * img->height) - (maxPos - Range * img->height);
	rectProj.width = xEnd - xStart;
	rectProj.x = xStart;
	rectProj.y = maxPos - Range * img->height;

	MaxPeak.Value = maxValue;
	MaxPeak.Position = maxPos;
	Count = nbElements;
	isHorizontal = true;
	sum = totalSum;
	average = sum / Count;
	//peaks.push_back(MaxPeak);
	checkBuffer(MaxPeak);
	Size.height = yEnd - yStart;
	Size.width = xEnd - xStart;
	
}

/// Méthode permettant de créer une sommation des niveaux de gris
/// a la verticale d'une image
void CProjection::CreateVerticalProjection(IplImage *img)
{
	int i, j;

	int xStart, yStart;
	int xEnd, yEnd;
	int vSum;
	int nbElements;

	int maxValue = 0;
	int maxPos = 0;
	int totalSum = 0;

	//peaks.clear();
	
	if (img->roi != NULL)
	{
		xStart = img->roi->xOffset;
		yStart = img->roi->yOffset;

		xEnd = img->roi->width + xStart;
		yEnd = img->roi->height + yStart;

		nbElements = img->roi->width - xStart;		
	}
	else
	{
		xStart = 0;
		yStart = 0;
		xEnd = img->width;
		yEnd = img->height;

		nbElements = img->width;
	}

	if (!isInitialized)
	{
		setSize(nbElements);
		isInitialized = true;
	}


	for (i = xStart; i < xEnd; i++)
	{
		vSum = 0;
		for (j = yStart; j < yEnd; j++)
		{
			vSum += (int)img->imageData[j * img->width + i];
		}

		if (vSum > maxValue)
		{
			maxValue = vSum;
			maxPos = i - xStart;
		}
		Values[i - xStart].Value = vSum;
		Values[i - xStart].Position = i;
		Values[i - yStart].NearMax = false;
		totalSum += vSum;
	}

	rectProj.height = yEnd - yStart;
	rectProj.width = (maxPos + Range * img->width) - (maxPos - Range * img->width);
	rectProj.y = yStart;
	rectProj.x = maxPos - Range * img->width;

	MaxPeak.Value = maxValue;
	MaxPeak.Position = maxPos;
	Count = nbElements;
	isHorizontal = false;
	sum = totalSum;
	average = sum / Count;

	//peaks.push_back(MaxPeak);
	checkBuffer(MaxPeak);

	Size.height = yEnd - yStart;
	Size.width = xEnd - xStart;


}


// Donne la dimension au vecteur values.
void CProjection::setSize(int size)
{
	Values = new dataColumn[size];
}

// Remet à zéro le vecteur de valeur ainsi que la variable qui
// indique si la projection est horizontale.
void CProjection::Reset()
{
	if (isInitialized)
	{
		delete [] Values;
		isHorizontal = 0;
	}
}

// Fonction retournant le rectangle dans lequel la projection est faite
CvRect CProjection::ExtractProjectionRect()
{
	return rectProj;
}

//// Méthode permettant de calculer la dérivée du vecteur
//void CProjection::CalculateDiffential()
//{
//	int i;
//
//	differential.clear();
//
//	for (i = 2; i < Count - 3; i++)
//	{
//		differential.push_back((-Values[i + 2].Value + 8 * Values[i + 1].Value - 8 * 
//			Values[i - 1].Value + Values[i - 2].Value) / 12);
//	}
//}

// Méthode qui lisse le vecteur Values
void CProjection::Smooth(int kernelSize)
{
	int * kernel;
	int i, j;
	int halfKernel = kernelSize / 2;
	int somme;

	MaxPeakSmooth.Value = 0;
	MaxPeakSmooth.Position = 0;

	for (i = 0; i < Count - 1; i++)
	{
		somme = 0;

		if (i < halfKernel)
		{
			for (j = 0; j < i + halfKernel; j ++)
				somme += Values[j].Value;

			Values[i].Value = somme / (i + halfKernel);
		}
		else if (i >= ((Count - 1) - halfKernel))
		{
			for (j = i; j < Count - 1 ; j ++)
				somme += Values[j].Value;

			Values[i].Value = somme / (Count - i);
		}
		else
		{
			for (j = -halfKernel; j < halfKernel; j++)
				somme += Values[i + j].Value;

			Values[i].Value = somme / kernelSize;
		}

		// 2008/04/15
		// Trouve le maximum après le lissage
		if (Values[i].Value > MaxPeakSmooth.Value)
		{
			MaxPeakSmooth.Value = Values[i].Value;
			MaxPeakSmooth.Position = i;
		}
	}
}

// Calcule l'écart-type de la projection
void CProjection::calcStandardDeviation()
{
	int i;
	double variance;

	variance = 0;

	for (i = 0; i < Count - 1; i++)
	{
		variance += (Values[i].Value - average) * (Values[i].Value - average);
	}

	standardDeviation = sqrt(variance / Count);
}


// Trouve le pic suivant le dernier peak;
peak CProjection::findNextPeak()
{

	int i;
	int maxValue = 0;

	peak nextPeak;
	nextPeak.Position = -1;
	nextPeak.Value = -1;


	for (i = 0; i < Count - 1; i++)
	{
		if (!Values[i].NearMax)
		{
			if (Values[i].Value > maxValue)
			{
				nextPeak.Position = i;
				nextPeak.Value = Values[i].Value;
			}
		}
		else
		{
			i = i;
		}
	}

	if (nextPeak.Position >= 0)
	{
		//peaks.push_back(nextPeak);
		checkBuffer(nextPeak);
	}

	return nextPeak;
	
}







// Méthode qui mets le vecteur check à vrai
void CProjection::checkBuffer(peak pk)
{
	int i;
	double lowerBound;
	double upperBound;

	lowerBound = pk.Position - (buffer * Count);
	upperBound = pk.Position + (buffer * Count);

	if (lowerBound < 0)
		lowerBound = 0;

	if (upperBound > Count)
		upperBound = Count;


	for (i = lowerBound; i < upperBound; i++)
	{
		Values[i].NearMax = true;
	}
	
}

peak CProjection::GetNextPeak()
{
	peak nextPeak;
	nextPeak = findNextPeak();

	return nextPeak;
}


