// CheckerBoard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

/*  Create checkerboard texture  */
#define checkImageWidth 128
#define checkImageHeight 72
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;

bool bFullScreen = false;

void makeCheckImage(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

void buildProjection( double *frustum, double alpha, double beta, double skew, double u0, double v0, int img_width, int img_height, double near_clip, double far_clip )
{
     
    // These parameters define the final viewport that is rendered into by
    // the camera.
    double L = 0;
    double R = img_width;
    double B = 0;
    double T = img_height;
     
    // near and far clipping planes, these only matter for the mapping from
    // world-space z-coordinate into the depth coordinate for OpenGL
    double N = near_clip;
    double F = far_clip;
     
    // set the viewport parameters
    //viewport[0] = L;
    //viewport[1] = B;
    //viewport[2] = R-L;
    //viewport[3] = T-B;
     
    // construct an orthographic matrix which maps from projected
    // coordinates to normalized device coordinates in the range
    // [-1, 1].  OpenGL then maps coordinates in NDC to the current
    // viewport
	Mat ortho = Mat::zeros(4, 4, CV_64FC1);
	ortho.at<double>(0,0) =  2.0/(R-L); ortho.at<double>(0,3) = -(R+L)/(R-L);
    ortho.at<double>(1,1) =  2.0/(T-B); ortho.at<double>(1,3) = -(T+B)/(T-B);
    ortho.at<double>(2,2) = -2.0/(F-N); ortho.at<double>(2,3) = -(F+N)/(F-N);
    ortho.at<double>(3,3) =  1.0;
     cout << ortho << endl;
    // construct a projection matrix, this is identical to the
    // projection matrix computed for the intrinsicx, except an
    // additional row is inserted to map the z-coordinate to
    // OpenGL.
	Mat tproj = Mat::zeros(4, 4, CV_64FC1);
    tproj.at<double>(0,0) = alpha; tproj.at<double>(0,1) = skew; tproj.at<double>(0,2) = -u0;
	tproj.at<double>(1,1) = beta; tproj.at<double>(1,2) = -v0;
	tproj.at<double>(2,2) = N+F; tproj.at<double>(2,3) = N*F;
	tproj.at<double>(3,2) = -1.0;
	cout << tproj << endl;
    // resulting OpenGL frustum is the product of the orthographic
    // mapping to normalized device coordinates and the augmented
    // camera intrinsic matrix
	Mat frustumMat = ortho*tproj;
	cout << frustumMat << endl;
	frustumMat = frustumMat.t();
	memcpy_s(frustum, 16 * 64, frustumMat.data, 16 * 64);
}

void init(void)
{    
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                   GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                   GL_NEAREST);
   
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
                checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                checkImage);
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //set camera
   //glLoadIdentity();
   //gluLookAt(0, 0, 2.0, 0, 0, 0, 0, 1.0, 0);

   glEnable(GL_TEXTURE_2D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glBindTexture(GL_TEXTURE_2D, texName);
   glBegin(GL_QUADS);
   GLfloat w = checkImageWidth / 10 * 0.1;
   GLfloat h = checkImageHeight / 10 * 0.1;
   /*glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
   glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
   glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
   glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);*/

   glTexCoord2f(0.0, 0.0); glVertex3f(-w, -h, 0.0);
   glTexCoord2f(0.0, 1.0); glVertex3f(-w, h, 0.0);
   glTexCoord2f(1.0, 1.0); glVertex3f(w, h, 0.0);
   glTexCoord2f(1.0, 0.0); glVertex3f(w, -h, 0.0);
   /*glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
   glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
   glTexCoord2f(1.0, 1.0); glVertex3f(2.41421, 1.0, -1.41421);
   glTexCoord2f(1.0, 0.0); glVertex3f(2.41421, -1.0, -1.41421);*/
   glEnd();
   glFlush();
   glutSwapBuffers();
   glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h)
{
	//double *frustum = new double[16];
    //buildProjection(frustum, 100.0, 100.0, 0.0, w/2, h/2, w, h, 0.001, 1000.0);
	GLdouble Near = 0.001;
	GLdouble Far = 1000.0;
	double focal = 1000.0;
	Mat perspMat = Mat::zeros(4, 4, CV_64FC1);
	perspMat.at<double>(0, 0) = focal;
	perspMat.at<double>(0, 2) = -w/2;
	perspMat.at<double>(1, 1) = focal;
	perspMat.at<double>(1, 2) = -h/2;
	perspMat.at<double>(2, 2) = Near + Far;
	perspMat.at<double>(2, 3) = Near * Far;
	perspMat.at<double>(3, 2) = -1.0;
	perspMat = perspMat.t();
	double *persp = (double *)perspMat.data;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0, w, 0, h, Near, Far);
	glMultMatrixd(persp);
    //glLoadMatrixd(frustum);
    //gluPerspective(45.0, (GLfloat) w/(GLfloat) h, 0.001, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -2.0);
    //delete []frustum;
}

void savePPM(char *ppmFileName)
{
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    char *pixels = new char[width * height * 3];

    glReadPixels(0,0, width, height, GL_RGB, GL_UNSIGNED_BYTE ,pixels);

	FILE* ppmFile = new FILE;
	fopen_s(&ppmFile, ppmFileName, "wb");

    fprintf(ppmFile, "P6\n");
    fprintf(ppmFile, "%d %d\n", width, height);
    fprintf(ppmFile, "255\n");
    fwrite(pixels, 1, width*height*3, ppmFile);

    fclose(ppmFile);
    //free(pixels);
	delete []pixels;
}

Mat screenShot()
{
	int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    uchar *pixels = new uchar[width * height * 3];

    glReadPixels(0,0, width, height, GL_RGB, GL_UNSIGNED_BYTE ,pixels);

	Mat image(height, width, CV_8UC3, pixels);
	flip(image, image, 0);
	cvtColor(image, image, CV_RGB2GRAY);
	//imshow("", image);
	//waitKey(0);
	return image;
}

bool findCorners(Mat* image, int nRow, int nCol)
{
	vector<Point2f> corners;
	bool bFind = findChessboardCorners(*image, Size(nRow, nCol), corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE );

	if(bFind)
	{
		cout << "found checkerboard" << endl;
		Mat result = (*image).clone();
		//drawChessboardCorners(result, Size(nRow, nCol), corners, bFind);
		//imshow("result", result);
		//waitKey(-1);
	}
	else
		cout << "no checkerboard" << endl;

	return bFind;
}

void randomRT(int count)
{
	double angle = 0;
	double transX, transY, transZ;
	int n = 0;
	int i = 0;
	while(1)
	{
		if (n >= count)
		{
			cout << i << " iterations" << endl;
			break;
		}

		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		transX = static_cast <double> (rand()) / static_cast <double> (RAND_MAX) * 0.2 - 0.1;
		transY = static_cast <double> (rand()) / static_cast <double> (RAND_MAX) * 0.1 - 0.05;
		transZ = -static_cast <double> (rand()) / static_cast <double> (RAND_MAX) * 4.0 - 2.0;
		glTranslated(0, 0, transZ);
		//glTranslated(0, 0, 2);
		//glLoadIdentity();
		angle = static_cast <double> (rand()) / static_cast <double> (RAND_MAX) * 180.0 - 90.0;
		glRotated(angle, 1, 0, 0);
		angle = static_cast <double> (rand()) / static_cast <double> (RAND_MAX) * 180.0 - 90.0;
		glRotated(angle, 0, 1, 0);
		angle = static_cast <double> (rand()) / static_cast <double> (RAND_MAX) * 180.0 - 90.0;
		glRotated(angle, 0, 0, 1);

		i++;
		//glFlush();
		glutSwapBuffers();
		Sleep(100);

		Mat snap = screenShot();
		//imshow("", snap);
		//waitKey(1000);
		if (findCorners(&snap, 8, 15))
			n++;
	}
}

void specialKeyboard(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		  glMatrixMode(GL_MODELVIEW);
		  glRotated(-10, 0, 1, 0);
		  glutSwapBuffers();
		  break;
	case GLUT_KEY_RIGHT:
		glMatrixMode(GL_MODELVIEW);
		glRotated(10, 0, 1, 0);
		glutSwapBuffers();
		break;
	case GLUT_KEY_UP:
		glMatrixMode(GL_MODELVIEW);
		glRotated(10, 1, 0, 0);
		glutSwapBuffers();
		break;
	case GLUT_KEY_DOWN:
		glMatrixMode(GL_MODELVIEW);
		glRotated(-10, 1, 0, 0);
		glutSwapBuffers();
		break;
      default:
         break;
	}
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
	  case '+':
		  /*glMatrixMode(GL_TEXTURE);
		  glRotated(10, 0, 0, 1);*/
		  glMatrixMode(GL_MODELVIEW);
		  glRotated(10, 0, 0, 1);
		  glFlush();
		  glutSwapBuffers();
		  break;
	  case '-':
		  /*glMatrixMode(GL_TEXTURE);
		  glRotated(-10, 0, 0, 1);*/
		  glMatrixMode(GL_MODELVIEW);
		  glRotated(-10, 0, 0, 1);
		  glFlush();
		  glutSwapBuffers();
		  break;
	  case 'm':
		  float modelViewMatrix[16];
		  glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
		  for (int i = 0; i < 4; i++)
		  {
			  for (int j = 0; j < 4; j ++)
			  {
				std::cout << modelViewMatrix[i + j * 4] << " ";
				if (j == 3)
					std::cout << std::endl;
			  }
		  }
		  std::cout << std::endl;
		  break;
	  case 'p':
		  float projectionMatrix[16];
		  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		  for (int i = 0; i < 4; i++)
		  {
			  for (int j = 0; j < 4; j ++)
			  {
				std::cout << projectionMatrix[i + j * 4] << " ";
				if (j == 3)
					std::cout << std::endl;
			  }
		  }
		  std::cout << std::endl;
		  break;
	  case 's':
		  {
		  //savePPM("screenshot.ppm");
			  Mat snap = screenShot();
			  findCorners(&snap, 8, 15);
			  //imwrite("screeshot.jpg", snap);
			  break;
		  }
	  case 'f':
		  if(bFullScreen)
		  {
			  glutPositionWindow(640,480);
			  reshape(640, 480);
			  bFullScreen = false;
			  cout << "exit full screen" << endl;;
		  }
		  else
		  {
			  glutFullScreen();
			  bFullScreen = true;
			  cout << "enter full screen" << endl;
		  }
		  break;
	  case 'a':
		  randomRT(10);
		  break;
	  case 'r':
		  glMatrixMode(GL_MODELVIEW);
		  glLoadIdentity();
		  glTranslated(0, 0, -2.0);
		  break;
      default:
         break;
   }
}

int _tmain(int argc, char* argv[])
{	
	int winWidth = 1920;
	int widHeight = 1080;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(winWidth, widHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Checker Board");
	//glutFullScreen();
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMainLoop();
	return 0;
}

