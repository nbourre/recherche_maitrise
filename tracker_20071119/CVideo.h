#if !defined CVIDEO
#define CVIDEO

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cv.h"      // include core library interface
#include "highgui.h" // include GUI library interface


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
			
			if (display)
			{
				iplFrame = cvQueryFrame(capture);
				cvvNamedWindow("First Frame", CV_WINDOW_AUTOSIZE);
				cvvShowImage("First Frame", iplFrame);

			}
		}

		LKActive = false;
		isBusy = false;
	}

	CVideo ()
	{
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
	void LucasKanade();
	bool GetStatus();
	bool GetLKStatus();

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

	void savePoints(int nbPoints);
	

	static const int MAX_COUNT = 500;
	static const int T = 10;
	static const int SPLIT_T = 255 / T;
	
	

};



extern CVideo *vdo;
#endif