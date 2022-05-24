//========================================================================
//    NeHe OpenGL Wizard : nehegl_glut.cpp
//    Wizard Created by: Vic Hollis
//========================================================================
/***********************************************
*                                              *
*         Bruce's GLUT OpenGL Basecode         *
*  Specially made for Nehe's Gamedev Website   *
*            http://nehe.gamedev.net           *
*                April 2003                    *
*                                              *
************************************************
* Project Information:
*
* Author: Bruce "Sinner" Barrera
* E-mail: sinner@opengl.com.br
* Date  : around April 2003 :-)
* Glut Version: 3.7.6
*
* Very special thanks to all people who
* contributed with any online tutors or
* help files. Keep coding!
*
***********************************************/



/****************************************
*										*
*			OPENCV POSIT TUTORIAL		*
*		Javier Barandiaran Martirena	*
*			       2006					*
*										*
****************************************/


#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "POSIT.h"


/* creates a enum type for mouse buttons */
enum {
    BUTTON_LEFT = 0,
    BUTTON_RIGHT,
    BUTTON_LEFT_TRANSLATE,
};

/* set global variables */
int mButton = -1;
int mOldY, mOldX;

/* vectors that makes the rotation and translation of the cube */
float eye[3] = {0.0f, 0.0f, 100.0f};
float rot[3] = {45.0f, 45.0f, 0.0f};


const int ESC = 27;

int wireframe = 0;

/* windows size and position constants */
const int GL_WIN_WIDTH = 450;
const int GL_WIN_HEIGHT = 350;
const int GL_WIN_INITIAL_X = 0;
const int GL_WIN_INITIAL_Y = 0;
const int GL_NEAR = 1.0;
const int GL_FAR = 1000.0;

////POSIT
int winWidth = GL_WIN_WIDTH;
int winHeight = GL_WIN_HEIGHT;
double cubeSize = 10.0f;
POSIT posit;


//------------------------------------------------------------------------
// Window resize function
//------------------------------------------------------------------------
void glutResize(int width, int height)
{
	winWidth = width;
	winHeight = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
////POSIT
	posit.initializeIntrinsics( width,height );
	posit.createOpenGLProjectionMatrix( width, height, 1.0, 1000.0 );
	glLoadMatrixd( posit.projectionMatrix );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//------------------------------------------------------------------------
// Function that handles keyboard inputs
//------------------------------------------------------------------------
void glutKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case ESC:
			exit(0);
		case 'W':
		case 'w':
		    wireframe = !wireframe;
	}
}

//------------------------------------------------------------------------
// If rotation angle is greater of 360 or lesser than -360,
// resets it back to zero.
//------------------------------------------------------------------------
void clamp(float *v)
{
    int i;

    for (i = 0; i < 3; i ++)
        if (v[i] > 360 || v[i] < -360)
            v[i] = 0;
}

//------------------------------------------------------------------------
// Moves the screen based on mouse pressed button
//------------------------------------------------------------------------
void glutMotion(int x, int y)
{
    if (mButton == BUTTON_LEFT)
    {
        /* rotates screen */
        rot[0] -= (mOldY - y);
        rot[1] -= (mOldX - x);
        clamp (rot);
	 }
	else if (mButton == BUTTON_RIGHT)
    {
        /*
           translate the screen, z axis
           gives the idea of zooming in and out 
        */        
        eye[2] -= (mOldY - y) * 0.2f; // here I multiply by a 0.2 factor to 
                                      // slow down the zoom
        clamp (rot);		
    } 
    else if (mButton == BUTTON_LEFT_TRANSLATE)
    {
        eye[0] += (mOldX - x) * 0.1f;
        eye[1] -= (mOldY - y) * 0.1f;
        clamp (rot);
    }
	
////POSIT
	//Update the real pose
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef (-eye[0], -eye[1], -eye[2]);
	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
	glGetDoublev(GL_MODELVIEW_MATRIX, posit.poseReal );
	//Calculate the new project 2d points
	posit.calculatImagePoints();
	//And estimate the new pose
	posit.poseEstimation();


    mOldX = x;
    mOldY = y;	
}

//------------------------------------------------------------------------
// Function that handles mouse input
//------------------------------------------------------------------------
void glutMouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        mOldX = x;
        mOldY = y;
        switch(button)
        {
            case GLUT_LEFT_BUTTON:
                if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
                {
                   mButton = BUTTON_LEFT_TRANSLATE;
                   break;
                } else
                {
                   mButton = BUTTON_LEFT;
                   break;
                }
            case GLUT_RIGHT_BUTTON:
                mButton = BUTTON_RIGHT;
                break;
        }
    } else if (state == GLUT_UP)
      mButton = -1;
}

//------------------------------------------------------------------------
// The glut's menu callback function. Called every time an option is selected
//------------------------------------------------------------------------
void glutMenu(int value)
{
	switch (value)
	{
		case 1:
			glutFullScreen();
			return;

		case 2:
			exit(0);
	}
}

//------------------------------------------------------------------------
// Function that handles special keyboards input (SHIFT, CTRL, ALT)
//------------------------------------------------------------------------
void glutSpecial(int value, int x, int y)
{
    switch (value)
    {
        case GLUT_KEY_F1:
                glutFullScreen();
                return;
    }
}


void renderCube(float size)
{
	glBegin(GL_QUADS);
		// Front Face
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glVertex3f( 0.0f,  0.0f,  0.0f);
		glVertex3f( size,  0.0f,  0.0f);
		glVertex3f( size,  size,  0.0f);
		glVertex3f( 0.0f,  size,  0.0f);
		// Back Face
		glNormal3f( 0.0f, 0.0f,-1.0f);
		glVertex3f( 0.0f,  0.0f, size);
		glVertex3f( 0.0f,  size, size);
		glVertex3f( size,  size, size);
		glVertex3f( size,  0.0f, size);		
		// Top Face
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glVertex3f( 0.0f,  size,  0.0f);
		glVertex3f( size,  size,  0.0f);
		glVertex3f( size,  size, size);
		glVertex3f( 0.0f,  size, size);
		// Bottom Face
		glNormal3f( 0.0f,-1.0f, 0.0f);
		glVertex3f( 0.0f,  0.0f,  0.0f);
		glVertex3f( 0.0f,  0.0f, size);
		glVertex3f( size,  0.0f, size);
		glVertex3f( size,  0.0f,  0.0f);
		// Right face
		glNormal3f( 1.0f, 0.0f, 0.0f);
		glVertex3f( size,  0.0f, 0.0f);
		glVertex3f( size,  0.0f, size);
		glVertex3f( size,  size, size);
		glVertex3f( size,  size, 0.0f);
		// Left Face
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f( 0.0f,  0.0f, 0.0f);
		glVertex3f( 0.0f,  size, 0.0f);
		glVertex3f( 0.0f,  size, size);
		glVertex3f( 0.0f,  0.0f, size);
    glEnd();
}


void renderAxis(float size)
{
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(size, 0.0f, 0.0f);
		glColor3f(0.0,1.0,0.0);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, size, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, size);
	glEnd();
}


void glutDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


////POSIT
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd( posit.projectionMatrix );
	glMatrixMode(GL_MODELVIEW);
	
	//Draw the object with the real pose
	glLoadMatrixd( posit.poseReal );
	glEnable(GL_DEPTH_TEST);
	glLineWidth( 2.0f );	
	renderAxis( cubeSize * 2.0 );	
	glEnable( GL_LIGHTING );
	GLfloat   light_position[]  = { -10.0, 50.0, -10.0, 0.0 };
	glLightfv( GL_LIGHT1, GL_POSITION, light_position );		
	
	glColor3f( 0.7f, 0.7f, 0.7f );
	renderCube( cubeSize );
	
	//Draw the object with the estimated pose
	glDisable( GL_LIGHTING );
	glDisable(GL_DEPTH_TEST);
	glLoadMatrixd( posit.posePOSIT );
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f( 1.0f, 0.0f, 0.0f );
	glLineWidth( 3.0f );
	renderCube( cubeSize );

	//Draw model points with the real pose
	glLoadMatrixd( posit.poseReal );
	glPointSize(6.0f);
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_POINTS );
	for ( int p=0; p<posit.modelPoints.size(); p++ )
	{
		glVertex3f( posit.modelPoints[p].x, posit.modelPoints[p].y, posit.modelPoints[p].z );
	}
	glEnd();
			
	//Draw the calculated 2D points 
	glPointSize(3.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( -winWidth*0.5, winWidth*0.5, -winHeight*0.5, winHeight*0.5, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin( GL_POINTS );
	for ( p=0; p<posit.imagePoints.size(); p++ )
	{
		glVertex2f( posit.imagePoints[p].x, posit.imagePoints[p].y);
	}
	glEnd();
    
    glFlush();
    glutSwapBuffers();    
}



void InitializeOGL()
{
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	GLfloat   light_ambient[]  = { 0.6, 0.6, 0.6, 1.0};
	GLfloat   light_diffuse[]  = { 0.7, 0.7, 0.7, 1.0};
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

    return;
}

//------------------------------------------------------------------------
//   It's the main application function. Note the clean code you can
//   obtain using he GLUT library. No calls to dark windows API
//   functions with many obscure parameters list. =)
//------------------------------------------------------------------------
int main(int argc, char** argv)
{
    /*
        Glut's initialization code. Set window's size and type of display.
        Window size is put half the 800x600 resolution as defined by above
	constants. The windows is positioned at the top leftmost area of
	the screen.
    */
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE );
    glutInitWindowPosition( GL_WIN_INITIAL_X, GL_WIN_INITIAL_Y );
    glutInitWindowSize( GL_WIN_WIDTH, GL_WIN_HEIGHT );
    glutInit( &argc, argv );

    glutCreateWindow("glutbasecode");

    /*
       The function below are called when the respective event
       is triggered. Very simple, isn't it?
    */
    glutReshapeFunc(glutResize);       // called every time  the screen is resized
    glutDisplayFunc(glutDisplay);      // called when window needs to be redisplayed
    glutIdleFunc(glutDisplay);         // called whenever the application is idle
    glutKeyboardFunc(glutKeyboard);    // called when the application receives a input from the keyboard
    glutMouseFunc(glutMouse);          // called when the application receives a input from the mouse
    glutMotionFunc(glutMotion);        // called when the mouse moves over the screen with one of this button pressed
    glutSpecialFunc(glutSpecial);      // called when a special key is pressed like SHIFT

    /*
        Do any lighting, material, alpha, etc initialization or
        configuration here.
    */
    InitializeOGL();

    /*
        Creates a menu attached to the mouses middle button
        the menu here consists in two options, one to toggle
        fullscreen mode and another for exiting the application.
    */
	glutCreateMenu(glutMenu);
	glutAddMenuEntry("Full Screen", 1);
	glutAddMenuEntry("Exit", 2);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	
////POSIT
	/*
		POSIT initialization
	*/
	posit.initialize( cubeSize, GL_WIN_WIDTH, GL_WIN_HEIGHT, GL_NEAR, GL_FAR );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	 /* translate camera to the coordinates provided by eye array */
	glTranslatef (-eye[0], -eye[1], -eye[2]);
    /* rotates the screen by the angles provided by rot array */    
	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
	glGetDoublev( GL_MODELVIEW_MATRIX, posit.poseReal );	
	//Calculate the new project 2d points
	posit.calculatImagePoints();			
	//And estimate the new pose
	posit.poseEstimation();

    /*
       Application's main loop. All the above functions
	 are called whe the respective events are triggered
    */
	glutMainLoop();

	return 0;
}



//---------------------------------------------------------------------------
//    Special Notes: Look the above program and realize how simple it was
//    to write a full working OpenGL application! Most of the
//    ready-to-work codes in internet are big ones with many Windows API
//    calls and so on. Another advantage of this code is that with the right
//    implementation it can be REALLY ported to others OS with ease.
//    Should compile and work with Mesa implementation without problems!
//    I tested it a lot in SuSE Linux 8.2 and of course Windows XP.
//---------------------------------------------------------------------------

