#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <C:\Users\ilyas\Documents\MATLAB\Stabilization\Simulink\BalloonStab\glut.h>

#define S_FUNCTION_NAME balloon
#define S_FUNCTION_LEVEL 2

#define IN_0_FRAME_BASED    FRAME_NO

#define OUT_0_FRAME_BASED    FRAME_NO

#define SAMPLE_TIME_0        INHERITED_SAMPLE_TIME

#define SFUNWIZ_GENERATE_TLC 1
#define SOURCEFILES "glut32.lib"
#define PANELINDEX           6
#define SFUNWIZ_REVISION     2.0

#include "simstruc.h"

static double h = 0;
static double target = 0;

const int WINDOW_HEIGHT = 700, WINDOW_WIDTH = 1200;

static  int    glInited  = 0;
static  HWND   hGlWindow = NULL;
static  HDC    hDC       = NULL;
static  HGLRC  hRC       = NULL;

#define GL_CLASS_NAME   "gl_class_name"

void CALLBACK resize ( int width, int height )
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

#define Pi 3.1415

void CALLBACK display()
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   glLoadIdentity();
   glFlush();

   int x_ref = WINDOW_WIDTH / 2;
   int y_ref = WINDOW_HEIGHT / 7;
   

   glColor3ub(0, 0, 0); // stripes
   glBegin(GL_LINES);
   glVertex2d(x_ref - 207, y_ref + 25);
   glVertex2d(x_ref - 185, y_ref + 25);
   glEnd();

   glColor3ub(0, 0, 0); // stripes
   glBegin(GL_LINES);
   glVertex2d(x_ref - 196, y_ref + 20);
   glVertex2d(x_ref - 196, y_ref + 30);
   glEnd();
   
   glColor3ub(210, 225, 30); // window
   glBegin(GL_QUADS);
   glVertex2d(x_ref - 185, y_ref + 20);
   glVertex2d(x_ref - 185, y_ref + 30);
   glVertex2d(x_ref - 207, y_ref + 30);
   glVertex2d(x_ref - 207, y_ref + 20);
   glEnd();
   
   glColor3ub(210, 105, 30); // house
   glBegin(GL_QUADS);
   glVertex2d(x_ref - 167, y_ref);
   glVertex2d(x_ref - 167, y_ref + 45);
   glVertex2d(x_ref - 225, y_ref + 45);
   glVertex2d(x_ref - 225, y_ref);
   glEnd();

   
   
   glColor3ub(128, 0, 0); // roof
   glBegin(GL_TRIANGLES);
   glVertex2d(x_ref - 195.5, y_ref + 84);
   glVertex2d(x_ref - 160, y_ref + 45);
   glVertex2d(x_ref - 231, y_ref + 45);
   glEnd();

   glColor3ub(220, 100, 0); // load
   glBegin(GL_QUADS);
   glVertex2d(x_ref + 15, y_ref + h);
   glVertex2d(x_ref + 21, y_ref + 15 + h);
   glVertex2d(x_ref - 21, y_ref + 15 + h);
   glVertex2d(x_ref - 15, y_ref + h);
   glEnd();

   glColor3ub(0, 255, 0); // stripes
   glBegin(GL_LINES);
   glVertex2d(x_ref - 15, y_ref + 15 + h);
   glVertex2d(x_ref - 15, y_ref + 55 + h);
   glEnd();

   glColor3ub(0, 255, 0); // stripes
   glBegin(GL_LINES);
   glVertex2d(x_ref + 15, y_ref + 55 + h);
   glVertex2d(x_ref + 15, y_ref + 15 + h);
   glEnd();

   glColor3ub(0, 255, 0); // stripes
   glBegin(GL_LINES);
   glVertex2d(x_ref + 30, y_ref + 55 + h);
   glVertex2d(x_ref - 30, y_ref + 55 + h);
   glEnd();

   glColor3ub(200, 0, 200); // balloon
   y_ref += 25;
   glBegin(GL_QUADS);
   glVertex2d(x_ref + 30, y_ref + h);
   glVertex2d(x_ref + 30, y_ref + 60 + h);
   glVertex2d(x_ref - 30, y_ref + 60 + h);
   glVertex2d(x_ref - 30, y_ref + h);
   glEnd();

   glColor3ub(0, 100, 10); // ground
   glBegin(GL_QUADS);
   glVertex2d(0, 0);
   glVertex2d(0, WINDOW_HEIGHT / 7);
   glVertex2d(WINDOW_WIDTH, WINDOW_HEIGHT / 7);
   glVertex2d(WINDOW_WIDTH, 0);
   glEnd();

   SwapBuffers ( hDC );
}

static  int getBitDepth ()
{
   HDC          hDC  = GetDC ( NULL );
   HBITMAP      hBmp = CreateCompatibleBitmap ( hDC, 1, 1 );
   BITMAPINFO * bmi  = (LPBITMAPINFO) malloc ( sizeof (BITMAPINFOHEADER) + 4 * sizeof (RGBQUAD) );
   int          res, bitCount;

   bmi -> bmiHeader.biBitCount    = (WORD) GetDeviceCaps ( hDC, BITSPIXEL );
   bmi -> bmiHeader.biCompression = BI_BITFIELDS;
   bmi -> bmiHeader.biSize        = sizeof ( BITMAPINFOHEADER );
   bmi -> bmiHeader.biWidth       = 1;
   bmi -> bmiHeader.biHeight      = 1;
   bmi -> bmiHeader.biClrUsed     = 0;
   bmi -> bmiHeader.biPlanes      = 1;

   res      = GetDIBits ( hDC, hBmp, 0, 1, NULL, bmi, DIB_RGB_COLORS );
   bitCount = bmi->bmiHeader.biBitCount;

   DeleteObject ( hBmp );
   ReleaseDC    ( NULL, hDC );
   DeleteObject ( hBmp );
   free         ( bmi );

   return bitCount;
}

static  int    createOpenGlWindow ( int x, int y, int width, int height, int depthBits )
{
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof (PIXELFORMATDESCRIPTOR),                         // size of this Pixel Format Descriptor
        1,                                                      // version number
        PFD_DRAW_TO_WINDOW |                                    // format must support Window
        PFD_SUPPORT_OPENGL |                                    // format must support OpenGL
        PFD_DOUBLEBUFFER,                                       // must support double buffering
        PFD_TYPE_RGBA,                                          // request an RGBA format
        0,		                       		                 // select color depth
        0, 0, 0, 0, 0, 0,                                       // color bits
        0,                                                      // no alpha buffer
        0,                                                      // shift bit ignored
        0,                                                      // no accumulation cuffer
        0, 0, 0, 0,                                             // accumulation bits ignored
        0,                       		                       // Z-Buffer depth
        8,                                                      // 8-bit stencil buffer
        0,                                                      // no auxiliary buffer
        PFD_MAIN_PLANE,                                         // main drawing layer
        0,                                                      // reserved
        0, 0, 0                                                 // layer masks ignored
    };

    GLuint	pixelFormat;                                        // will hold the selected pixel format

    DWORD windowStyle         = WS_OVERLAPPEDWINDOW;            // set window style
    DWORD windowExtendedStyle = WS_EX_APPWINDOW;                // set window extended style
    RECT  windowRect; 	// define window coordinates
    int   bpp;

    windowRect.left = x; // window position
    windowRect.top = y;
    windowRect.right = x+width;
    windowRect.bottom = y+height;
    pfd.cColorBits = getBitDepth();
    pfd.cDepthBits = depthBits;

    windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;   // window extended style
    windowStyle         = WS_OVERLAPPEDWINDOW;                  // window style
                                                                // adjust window, Account for window worders
    AdjustWindowRectEx ( &windowRect, windowStyle, 0, windowExtendedStyle );

                                                                // create the OpenGL window
    hGlWindow = CreateWindowEx ( windowExtendedStyle,           // extended style
                            GL_CLASS_NAME,                      // class name
                            "Balloon stabilization.",                             // window title
                            windowStyle,                        // window style
                            windowRect.left,                    // window (X,Y) position
                            windowRect.top,
                            windowRect.right - windowRect.left, // window width
                            windowRect.bottom - windowRect.top, // window height
                            HWND_DESKTOP,                       // desktop is window's parent
                            0,                                  // no nenu
                            GetModuleHandle ( NULL ),           // pass the window instance
                            NULL );

    if ( hGlWindow == NULL )                                    // was window creation a success?
        return 0;                                               // if not then return false

    hDC = GetDC ( hGlWindow );                                  // get device context for this window

    if ( hDC == NULL )                                          // did we get a device context?
        return 0;

    bpp = getBitDepth ();                                       // bit depth

    pixelFormat = ChoosePixelFormat ( hDC, &pfd );              // find a compatible pixel format

    if ( pixelFormat == 0 )                                     // did we find a compatible format?
        return 0;                                         	     // failed

    if ( SetPixelFormat ( hDC, pixelFormat, &pfd ) == FALSE )   // try to set pixel format
        return 0;                                         	     // failed

    hRC = wglCreateContext ( hDC );                             // try to get a rendering context

    if ( hRC == NULL )                                          // did we get a rendering context?
        return 0;                                               // failed

                                                                // make the rendering context our current rendering context
    if ( wglMakeCurrent ( hDC, hRC ) == FALSE )
        return 0;                                               // failed

    resize ( width, height );                                   // reshape our GL window

    ShowWindow ( hGlWindow, SW_SHOW );

    glClearColor(0.0f, 0.0f, 0.9f, 1.0f);                       // change background colour

    return 1;                                                   // window creating was successful
                                                                // further initialization will be done in WM_CREATE
}

static  int    destroyOpenGlWindow ()
{
    wglMakeCurrent ( NULL, NULL );

    if ( hRC != NULL )
        wglDeleteContext ( hRC );                               // delete The Rendering Context

    if ( hDC != NULL )
        ReleaseDC ( hGlWindow, hDC );                           // release Our Device Context

    if ( hGlWindow != NULL )
        DestroyWindow ( hGlWindow );                            // destroy the window

    hGlWindow = NULL;                                           // zero the window handle
    hDC       = NULL;                                           // zero the device context
    hRC       = NULL;                                           // zero the rendering context

    return 1;
}

static LRESULT CALLBACK windowProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg )                                              // evaluate window message
    {
        case WM_SIZE:                                           // resizing action has taken place
            resize (LOWORD (lParam), HIWORD (lParam));          // resize window
            if (glInited) display ();

            return 0;
    }

    return DefWindowProc ( hWnd, msg, wParam, lParam );         // pass nnhandled messages to DefWindowProc
}

static void initGL ()
{
    float pos[4] = {1,1,0,1};
    float dir[3] = {-1,-1,1};

    GLfloat mat_specular[] = {1,1,1,1};
                                                                // create window class
    WNDCLASSEX windowClass;

    ZeroMemory ( &windowClass, sizeof (WNDCLASSEX) );           // clear the memory

    windowClass.cbSize          = sizeof (WNDCLASSEX);          // size of the windowClass structure
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;     // redraws the window on any movement / resizing
    windowClass.lpfnWndProc     = (WNDPROC) windowProc;         // windowProc Handles Messages
    windowClass.hInstance       = GetModuleHandle ( NULL );     // set the instance
    windowClass.hbrBackground   = (HBRUSH)12;   // class background brush color
    windowClass.hCursor     = LoadCursor ( NULL, IDC_ARROW );   // load the arrow pointer
    windowClass.lpszClassName   = GL_CLASS_NAME;                // sets the classname
    windowClass.hIcon           = NULL;

    if ( RegisterClassEx ( &windowClass ) == 0 )
        return;

    createOpenGlWindow ( 50, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 32 );

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);

    float amb[4] = {0.9,1,1,1}; // overall colours
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
}


static void mdlInitializeSizes(SimStruct *S) // simulink functions next
{
    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortVectorDimension(S, 0, 1);
    ssSetInputPortRequiredContiguous(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    if (!ssSetNumOutputPorts(S, 0)) return;
    // ssSetOutputPortVectorDimension(S, 0, 3); // num of outputs of s-func

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    ssSetNumSFcnParams(S, 1); // num of inner params

    ssSetNumSampleTimes(S, 1);

    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);

    ssSetNumNonsampledZCs(S, 0);

    ssSetOptions(S, (SS_OPTION_EXCEPTION_FREE_CODE |
                     SS_OPTION_USE_TLC_WITH_ACCELERATOR |
             	     SS_OPTION_WORKS_WITH_CODE_REUSE));

    if (!glInited)
    {
        initGL();
        glInited = 1;
    }
}

static void mdlStart(SimStruct *S)
{
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
    const double  *u  = (const double*)ssGetInputPortSignal(S,0);
 //   double 	  *y  = (double*)ssGetOutputPortRealSignal(S,0); // get output of s-function
     target = mxGetScalar(ssGetSFcnParam(S,0));
     if (target == 0)
         return;

	h = (WINDOW_HEIGHT / 1.7 - WINDOW_HEIGHT / 7) / target * u[0]; // get the system parameters

    display(); // new frame

}

static void mdlTerminate(SimStruct *S)
{
    destroyOpenGlWindow ();
    if (UnregisterClass(GL_CLASS_NAME, GetModuleHandle(NULL))!=0)
        glInited = 0;
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
