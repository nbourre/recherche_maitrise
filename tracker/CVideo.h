#if !defined CVIDEO
#define CVIDEO

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cv.h"      // include core library interface
#include "highgui.h" // include GUI library interface
#include "Projection.h"
#include <vector>

using namespace std;


class CVideo
{
	CvCapture *capture;
	IplImage *iplFrame;
	int nbFrames;

public:

	CVideo (CString filename, bool display = true) 
	{
		capture = cvCaptureFromFile(filename);

		if (capture != NULL)
		{
			nbFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
			blLoaded = true;
			BreakPlayback = false;
			SliderValue = 0;
			iplFrame = cvQueryFrame(capture);

			if (display)
			{				
				cvvNamedWindow("First Frame", CV_WINDOW_AUTOSIZE);
				cvvShowImage("First Frame", iplFrame);

			}
		}

		LKActive = false;
		isBusy = false;
	}

	//Constructeur à partir de la caméra 
	CVideo (int cam) 
	{
		capture = cvCaptureFromCAM(cam);

		if (capture != NULL)
		{
			nbFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
			blLoaded = true;
			BreakPlayback = false;
			SliderValue = 0;
			iplFrame = cvQueryFrame(capture);
		}

		LKActive = false;
		isBusy = false;
	}

	CVideo ()
	{
		if (capture != NULL)
		{
			cvReleaseCapture(&capture);
		}

		 blLoaded = false;
	}

	IplImage * QueryFrame();	// Permet d'extraire une image
	int SliderValue;
	int GetFramePos();			// Retourne la position de l'image
	bool BreakPlayback;

	double GetFPS();
	void ResetVideo();
	void SetFramePosition(int Position);
	void Play();
	void PlayProcessed();
	//void LucasKanade();
	CvPoint2D32f *getPointArray(IplImage * img);
	CvPoint2D32f getAveragePoint(IplImage *img);  // Retourne la position moyenne des points
	bool GetStatus();
	bool GetLKStatus();

	// Ajout 2008/03/25
	//void DrawCrossPoint(IplImage *img, vector <CvPoint> crossPoints);

	CString GetPointList();	// Utiliser avec Lucas-Kanade
	CvPoint GetMousePos();
	
	~CVideo()
	{
		if (blLoaded)
			cvReleaseCapture(&capture);

		if (iplFrame != NULL)
			cvReleaseImage(&iplFrame);
	}

private:
	bool blLoaded;

	// Pour Lucas-Kanade
	CvPoint2D32f* points[2];
	CvPoint2D32f* swap_points; 
	CvPoint2D32f* ptTime[10];
	char* status;
	int count;
	int need_to_init;
	int night_mode;
	int flags;
	bool LKActive;
	bool isBusy;

	CvRect* rects[2];
	int count_rect;

	void savePoints(int nbPoints);
	


	static const int MAX_COUNT = 500;
	static const int T = 10;
	static const int SPLIT_T = 255 / T;

	// 2008/04/15
	vector <peak> vecPeakHor;
	static const int runningAverageSize = 20;

	// Calcule la moyenne
	peak calcAverage(vector <peak> vec);
	void addToVector(vector <peak> *vec, peak pk);

	// 2008/05/21
	vector <int> vecDistance;
	double calcAverage(vector <int> vec);
	void addToVector(vector <int> *vec, int value);

};



extern CVideo *vdo;
#endif