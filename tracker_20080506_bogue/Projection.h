#pragma once

#include "cv.h"
//#include <vector>
#include <math.h>

//using namespace std;

// 2008/04/15
struct peak
{
	int Position;
	double Value;
};

struct dataColumn
{
	int Position;
	double Value;
	bool NearMax;	// Indique si la donnée peak est près d'un max
};

class CProjection
{
public:
	CProjection(void);
	~CProjection(void);

	CProjection(IplImage *img);

	IplImage * CreateProjectionImage();
	CvRect ExtractProjectionRect();
	void CreateHorizontalProjection(IplImage *img);
	void CreateVerticalProjection(IplImage *img);
	void CalculateDiffential();		// Calcule le vecteur de dérivées
	void Smooth(int kernelSize); // Permet de faire un lissage du vecteur
	void Reset();  // Remet à zéro les valeurs

	dataColumn * Values;
	int Count;
	
	double Range; // Plage autour du maximum



	//// Retourne le vecteur de points de croisements // TODO: Comment utiliser les méthodes statics?
	//static vector <CvPoint> GetCrossPeaks(vector <int> VerticalPeaks, vector <int> HorizontalPeaks);


	// Ajout 2008/04/15
	peak MaxPeakSmooth;
	peak MaxPeak;
	peak GetNextPeak();

	CvSize Size;

	
	

private:
//	vector<double> differential;
	bool isInitialized;		// Est initilialisé
	void setSize(int size);	// Dimension de la projection
	bool isHorizontal;		// Type de projection
	CvRect rectProj;

	// Ajout 2008/03/25
	int sum;					// Somme des valeurs de la projection
	double average;				// Moyenne des valeurs de la projection
	double standardDeviation;	// Écart-type de la projection
	void calcStandardDeviation();
	peak findNextPeak();
	
	//vector <peak> peaks;

	// Ajout 2008/04/15
	double buffer;
	void checkBuffer(peak pk);
	void findNextMax();

	//bool * checked;
};


