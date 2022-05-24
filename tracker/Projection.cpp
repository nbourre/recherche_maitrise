#include "stdafx.h"
#include "Projection.h"


CProjection::CProjection(void)
{
	isInitialized = false;
	Range = 0.1;
	Buffer = 0.1;
}

CProjection::~CProjection(void)
{
	isInitialized = false;
}

CProjection::CProjection(IplImage *img)
{
	isInitialized = false;
	Range = 0.1;
	CreateHorizontalProjection(img);
	Buffer = 0.1;
}

/// Retourne l'image repr�sentant la projection
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
				if (Values[j].NearMax && Range != 0)
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
				
				//if (i >= maxLowerRange && i <= maxUpperRange)
				if (Values[i].NearMax && Range != 0)
					cvLine(imgRet, cvPoint(i, 0), cvPoint(i, ratio * Values[i].Value), CV_RGB(255, 255, 255));
				else
					cvLine(imgRet, cvPoint(i, 0), cvPoint(i, ratio * Values[i].Value), CV_RGB(127, 127, 127));
			}
		}

	}
	return imgRet;
}

/// M�thode permettant de cr�er une sommation des niveaux de gris
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
			hSum += (uchar)img->imageData[j * img->widthStep + i];
		}

		if (hSum > maxValue)
		{
			maxValue = hSum;
			maxPos = j - yStart;
		}
		Values[j - yStart].Position = j;
		Values[j - yStart].Value = hSum;
		//checked[j - yStart] = false;
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

/// M�thode permettant de cr�er une sommation des niveaux de gris
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
			vSum += (uchar)img->imageData[j * img->widthStep + i];
		}

		if (vSum > maxValue)
		{
			maxValue = vSum;
			maxPos = i - xStart;
		}

		Values[i - xStart].Position = i;
		Values[i - xStart].Value = vSum;
		//checked[i - xStart] = false;
		Values[i - xStart].NearMax = false;
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
	checked = new bool[size];
}

// Remet � z�ro le vecteur de valeur ainsi que la variable qui
// indique si la projection est horizontale.
void CProjection::Reset()
{
	if (isInitialized)
	{
		delete [] Values;
		delete [] checked;
		isHorizontal = 0;
	}
}

// Fonction retournant le rectangle dans lequel la projection est faite
CvRect CProjection::ExtractProjectionRect()
{
	return rectProj;
}

//// M�thode permettant de calculer la d�riv�e du vecteur
//void CProjection::CalculateDiffential()
//{
//	int i;
//
//	differential.clear();
//
//	for (i = 2; i < Count - 3; i++)
//	{
//		differential.push_back((-Values[i + 2] + 8 * Values[i + 1] - 8 * Values[i - 1] + Values[i - 2]) / 12);
//	}
//}

// M�thode qui lisse le vecteur Values
void CProjection::Smooth(int kernelSize)
{
	int * kernel;
	int i, j;
	int halfKernel = kernelSize / 2;
	int somme;

	MaxPeak.Value = 0;
	MaxPeak.Position = 0;

	for (i = 0; i < Count - 1; ++i)
	{
		somme = 0;

		if (i < halfKernel)
		{
			for (j = 0; j < i + halfKernel; ++j)
				somme += Values[j].Value;

			Values[i].Value = somme / (i + halfKernel);
		}
		else if (i >= ((Count - 1) - halfKernel))
		{
			for (j = i; j < Count - 1 ; ++j)
				somme += Values[j].Value;

			Values[i].Value = somme / (Count - i);
		}
		else
		{
			for (j = -halfKernel; j < halfKernel; ++j)
				somme += Values[i + j].Value;

			Values[i].Value = somme / kernelSize;
		}

		// 2008/04/15
		// Trouve le maximum apr�s le lissage
		if (Values[i].Value > MaxPeak.Value)
		{
			MaxPeak.Value = Values[i].Value;
			MaxPeak.Position = i;
		}

		Values[i].NearMax = false;
	}

	checkBuffer(MaxPeak);
}

//// Calcule l'�cart-type de la projection
//void CProjection::calcStandardDeviation()
//{
//	int i;
//	double variance;
//
//	variance = 0;
//
//	for (i = 0; i < Count - 1; i++)
//	{
//		variance += (Values[i] - average) * (Values[i] - average);
//	}
//
//	standardDeviation = sqrt(variance / Count);
//}


// Trouve le pic suivant le dernier peak;
peak CProjection::FindNextPeak()
{

	int i;
	int maxValue = 0;

	peak nextPeak;
	nextPeak.Position = -1;
	nextPeak.Value = -1;


	for (i = 0; i < Count - 1; ++i)
	{
		if (!Values[i].NearMax)
		{
			if (Values[i].Value > maxValue)
			{
				nextPeak.Position = i;
				nextPeak.Value = Values[i].Value;
				maxValue = nextPeak.Value;
			}
		}
		else
		{
			i = i;
		}
	}

	if (nextPeak.Position >= 0)
	{
		checkBuffer(nextPeak);
	}

	return nextPeak;
	
}

peak CProjection::FindNextPeak(int threshDist, double threshold)
{
	int i;
	int maxValue = 0;
	int begin = 0;
	int end = 0;
	int dist = (int)(abs((double)(MaxPeak.Position - threshDist)) * (1 + threshold));

	peak nextPeak;
	nextPeak.Position = -1;
	nextPeak.Value = -1;

	begin = MaxPeak.Position - dist > 0 ? MaxPeak.Position - dist : 0;
	end = MaxPeak.Position + dist < Count ? MaxPeak.Position + dist : Count;

	for (i = begin; i < end - 1; ++i)
	{
		if (!Values[i].NearMax)
		{
			if (Values[i].Value > maxValue)
			{
				nextPeak.Position = i;
				nextPeak.Value = Values[i].Value;
				maxValue = nextPeak.Value;
			}
		}
		else
		{
			i = i;
		}
	}

	if (nextPeak.Position >= 0)
	{
		checkBuffer(nextPeak);
	}

	return nextPeak;
}

// M�thode qui mets le vecteur check � vrai
void CProjection::checkBuffer(peak pk)
{
	int i;
	double lowerBound;
	double upperBound;

	lowerBound = pk.Position - (Buffer * Count);
	upperBound = pk.Position + (Buffer * Count);

	if (lowerBound < 0)
		lowerBound = 0;

	if (upperBound > Count)
		upperBound = Count;


	for (i = lowerBound; i < upperBound; i++)
	{
		//checked[i] = true;
		Values[i].NearMax = true;
	}
}

//peak CProjection::GetNextPeak()
//{
//	findNextPeak();
//
//	return peaks.at(peaks.size() - 1);
//}


