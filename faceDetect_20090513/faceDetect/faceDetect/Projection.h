#pragma once

#include <opencv2/opencv.hpp>
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
	bool NearMax;
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
	//void CalculateDiffential();		// Calcule le vecteur de dérivées
	void Smooth(int kernelSize); // Permet de faire un lissage du vecteur
	void Reset();  // Remet à zéro les valeurs

	//int * Values;
	dataColumn * Values;
	int Count;
	
	double Range; // Plage autour du maximum

	// Ajout 2008/03/25
	//vector <peak> GetPeaks();	// retourne le vecteur de pics;

	//// Retourne le vecteur de points de croisements // TODO: Comment utiliser les méthodes statics?
	//static vector <CvPoint> GetCrossPeaks(vector <int> VerticalPeaks, vector <int> HorizontalPeaks);


	// Ajout 2008/04/15
	peak MaxPeak;
	
	// Retourne le prochain max après le max primaire
	peak FindNextPeak();

	// Retourne le prochain max après le max primaire
	// threshDist --> Seuil moyen de la distance entre deux peaks
	// threshhold --> Seuil de distance maximum à parcourir par rapport à la distance moyenne
	peak FindNextPeak(int threshDist, double threshold);

	CvSize Size;
	double Buffer;				// Zone tampon autour d'un peak
	

private:
	//vector<double> differential;
	bool isInitialized;		// Est initilialisé
	void setSize(int size);	// Dimension de la projection
	bool isHorizontal;		// Type de projection
	CvRect rectProj;

	// Ajout 2008/03/25
	int sum;					// Somme des valeurs de la projection
	double average;				// Moyenne des valeurs de la projection
	//double standardDeviation;	// Écart-type de la projection
	//void calcStandardDeviation();
	//void findNextPeak();
	
	//vector <peak> peaks;

	// Ajout 2008/04/15
	
	void checkBuffer(peak pk);	// Procédure mettant à vrai les colonnes qui sont près d'un max
	

	bool * checked;
};


