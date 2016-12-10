// Starter program for HW 1. 
// Program draws a triangle. Study the program first
// The function generateGeometry can be modified to draw many 2D drawings (e.g. 2D Sierpinski Gasket)
// Generated using randomly selected vertices and bisection

#include "Angel.h"  // Angel.h is homegrown include file, which also includes glew and freeglut
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <time.h>

typedef vec2 point2;

// remember to prototype
void initGPUBuffers();
void shaderSetup( void );
void displayModeP( void );
void displayModeT();
void displayModeE();
void displayModeG();
void displayModeF();
void keyboard( unsigned char key, int x, int y );
void drawPolylineFile(char *filename);
void orthoStuff(float left, float right, float bottom, float top);
void resize(int reshapeWidth, int reshapeHeight);
void maintainRatio();
void mouseModeP(int button, int state, int x, int y);
void mouseModeT(int button, int state, int x, int y);
void mouseModeE(int button, int state, int x, int y);

using namespace std;

// Array for polyline
point2 points[10000];
point2 fernPoints[50000];
point2 drawPoints[10][100];
int drawPointIndex[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int drawLineIndex = 0;
bool firstTime = true;

GLint vertexColorLoc;

int currentFile;
static char filenames[10][30] = {
	"../Polylines/dino.dat",
	"../Polylines/birdhead.dat",
	"../Polylines/dragon.dat",
	"../Polylines/dragon.dat",
	"../Polylines/house.dat",
	"../Polylines/knight.dat",
	"../Polylines/rex.dat",
	"../Polylines/scene.dat",
	"../Polylines/usa.dat",
	"../Polylines/vinci.dat"
};

GLuint program;

static int width = 640;
static int height = 480;

static char currentState = 'p';
static int currentColor = 0; //0 for red, 1 for green, 2 for blue

bool makeNewLine = true;
//bool newPoint = true;

void orthoStuff(float left, float right, float bottom, float top) {
	GLuint ProjLoc = glGetUniformLocation(program, "Proj");
	mat4 ortho = Ortho2D(left, right, bottom, top);
	glUniformMatrix4fv(ProjLoc, 1, GL_FALSE, ortho);
}

void drawPolylineFile(char *filename)
{
	FILE *file;
	char line[100];
	int lineCount, pointCount;
	float x, y, left, right, bottom, top;

	if ((file = fopen(filename, "rt")) == NULL)
	{
		printf("file not found!");
		exit(0);
	}

    while (!feof(file))
    {
        memset(line, 0, 100);
        fscanf(file, "%s", line);
        if (line[0] == '*') {
            break;
        }
        else {
            continue;
        }
    }

    fscanf(file, "%f %f %f %f\n", &left, &top, &right, &bottom);
	orthoStuff(left, right, bottom, top);

	fscanf(file, "%d", &lineCount);
	for (int i = 0; i < lineCount; i++) {
		fscanf(file, "%d", &pointCount);
		for (int n = 0; n < pointCount; n++) {
			fscanf(file, "%f %f", &x, &y);
			points[n] = point2(x, y);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, pointCount);
		glFlush();
	}

	fclose(file);
}


void initGPUBuffers()
{
	// Create a vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
}


void shaderSetup( void )
{
	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );

	vertexColorLoc = glGetUniformLocation(program, "ourColor");
	
    glUseProgram( program );

	glUniform4f(vertexColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        // sets white as color used to clear screen
}

//default polyline chooser
void displayModeP( void )
{
    glClear( GL_COLOR_BUFFER_BIT );                // clear window

	for (int i = 0; i < 10; i++) {
		glViewport((width / 10) * i, height - (width * 0.08) - 10, width / 10, width * 0.08);
		drawPolylineFile(filenames[i]);
	}

	maintainRatio();
	drawPolylineFile(filenames[currentFile]);
	glFlush();
}

//5x5 tiled mode
void displayModeT() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 10; i++) {
		glViewport((width / 10) * i, height - (width * 0.08) - 10, width / 10, width * 0.08);
		drawPolylineFile(filenames[i]);
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			glViewport(i * width / 5, j * (height - (width * 0.08) - 10) / 5, width / 5, (height - (width * 0.08) - 10) / 5);
			drawPolylineFile(filenames[rand() % 10]);
		}
	}

	glFlush();
}

//drawing mode
void displayModeE() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 10; i++) {
		glViewport((width / 10) * i, height - (width * 0.08) - 10, width / 10, width * 0.08);
		drawPolylineFile(filenames[i]);
	}
	
	if (width / (height - (width * 0.08)) > 1.33) {
		orthoStuff(0, width, 0, height - 0.08*width - 10);
	}
	else {
		orthoStuff(0, width, 0, 0.75*width - 10);
	}
	maintainRatio();

	for (int j = 0; j <= drawLineIndex; j++) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(drawPoints[j]), drawPoints[j], GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, drawPointIndex[j]);

		glDrawArrays(GL_LINE_STRIP, 0, drawPointIndex[j]);

		glFlush();
	}

	//makeNewLine = false;
}

//gingerbread man
void displayModeG() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 10; i++) {
		glViewport((width / 10) * i, height - (width * 0.08) - 10, width / 10, width * 0.08);
		drawPolylineFile(filenames[i]);
	}

	orthoStuff(0, 640, 0, 480);
	maintainRatio();

	int nextX, nextY;

	int prevX = 115;
	int prevY = 121;

	int M = 40;
	int L = 3;

	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10000; j++) {
			nextX = ((M*(1 + 2 * L)) - prevY + (abs(prevX - (L*M))));
			nextY = prevX;

			points[j] = point2(prevX, prevY);

			prevX = nextX;
			prevY = nextY;
		}
		prevX += 2;
		prevY += 3;

		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, 10000);
		glFlush();
	}
}

//fern
void displayModeF() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 10; i++) {
		glViewport((width / 10) * i, height - (width * 0.08) - 10, width / 10, width * 0.08);
		drawPolylineFile(filenames[i]);
	}

	orthoStuff(-5, 5, 0, 12);
	maintainRatio();

	double r, x, y;

	time_t t;
	time(&t);
	srand((unsigned int)t);

	point2 temp = {1, 1};

	fernPoints[0][0] = 1;
	fernPoints[0][1] = 1;

	for (int i = 0; i < 50000; i++) {
		r = ((double)rand() / (double)RAND_MAX);

		if ((r >= 0) && (r < 0.83)) {
			fernPoints[i][0] = (0.86* temp[0]) + (0.03 * temp[1]) + 0.0;
			fernPoints[i][1] = (-0.03* temp[0]) + (0.86 * temp[1]) + 1.5;
		} 
		else if ((r >= 0.83) && (r < 0.91)) {
			fernPoints[i][0] = (0.02* temp[0]) + (-0.25 * temp[1]) + 0.0;
			fernPoints[i][1] = (0.21* temp[0]) + (0.23 * temp[1]) + 1.5;
		}
		else if ((r >= 0.91) && (r < 0.99)) {
			fernPoints[i][0] = (-0.15* temp[0]) + (0.27 * temp[1]) + 0.0;
			fernPoints[i][1] = (0.25* temp[0]) + (0.26 * temp[1]) + 0.45;
		}
		else {
			fernPoints[0] = (0.00* temp[0]) + (0.00 * temp[1]) + 0.0;
			fernPoints[1] = (0.00* temp[0]) + (0.17 * temp[1]) + 0.0;
		}
		temp[0] = fernPoints[i][0];
		temp[1] = fernPoints[i][1];

	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(fernPoints), fernPoints, GL_STATIC_DRAW);
	glDrawArrays(GL_POINTS, 0, 50000);
	glFlush();
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
	case 'p':
		currentState = 'p';
		glutDisplayFunc(displayModeP);
		displayModeP();
		glutMouseFunc(mouseModeP);
		break;
	case 't':
		currentState = 't';
		glutDisplayFunc(displayModeT);
		displayModeT();
		glutMouseFunc(mouseModeT);
		break;
	case 'e':
		currentState = 'e';
		glutDisplayFunc(displayModeE);
		glutMouseFunc(mouseModeE);
		displayModeE();
		break;
	case 'g':
		currentState = 'g';
		glutDisplayFunc(displayModeG);
		glutMouseFunc(mouseModeT);
		displayModeG();
		break;
	case 'f':
		currentState = 'f';
		glutDisplayFunc(displayModeF);
		glutMouseFunc(mouseModeT);
		displayModeF();
		break;
	case 'b':
		if (currentState == 'e') {
			//make new line
			makeNewLine = true;
		}
		break;
	case 'c':
		if (currentColor == 0) {
			glUniform4f(vertexColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
			currentColor = 1;
		}
		else if (currentColor == 1)
		{
			glUniform4f(vertexColorLoc, 0.0f, 0.0f, 1.0f, 1.0f);
			currentColor = 2;
		}
		else if (currentColor == 2)
		{
			glUniform4f(vertexColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);
			currentColor = 0;
		}
		switch (currentState)
		{
		case 'p':
			displayModeP();
			break;
		case 't':
			displayModeT();
			break;
		case 'e':
			displayModeE();
			break;
		case 'g':
			displayModeG();
			break;
		case 'f':
			displayModeF();
			break;
		}	
		break;
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
    }
}

void mouseModeP(int button, int state, int x, int y) {
	int i = 0;
	
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN && y <= height * 0.2) {
			i = round(10 * x / width);
			currentFile = i;
			maintainRatio();
			drawPolylineFile(filenames[i]);
		}
		break;
	}
}

void mouseModeT(int button, int state, int x, int y) {}

//click to make points, b to make new line
void mouseModeE(int button, int state, int x, int y) {

	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			if (makeNewLine)
			{
				if (!firstTime) {
					drawLineIndex++;
				}
				
				drawPoints[drawLineIndex][0] = point2(x, height - y);
				drawPointIndex[drawLineIndex]++;
				
				//newPoint = false;
				makeNewLine = false;
				firstTime = false;
			}
			else
			{
				drawPoints[drawLineIndex][drawPointIndex[drawLineIndex]] = point2(x, height - y);
				drawPointIndex[drawLineIndex]++;
			}
		}
	}

}

void resize(int reshapeWidth, int reshapeHeight) {
	width = reshapeWidth;
	height = reshapeHeight;
	glClear(GL_COLOR_BUFFER_BIT);
	maintainRatio();
	//drawPolylineFile(filenames[currentFile]);
}

void maintainRatio() {
	if (currentState == 'e') {
		if (width / (height - (width * 0.08)) > 1.33) {
			//(width - 1.067*(height - 0.08*width)) / 2
			//1.067*(height - 0.08*width)
			glViewport(0, 0, width, height - 0.08*width - 10);
		}
		else {
			glViewport(0, 0, width, 0.75*width - 10);
		}
	}
	else {
		if (width / (height - (width * 0.08)) > 1.33) {
			//(width - 1.067*(height - 0.08*width)) / 2
			//1.067*(height - 0.08*width)
			glViewport((width - 1.067*(height - 0.08*width)) / 2, 0, 1.067*(height - 0.08*width), height - 0.08*width - 10);
		}
		else {
			glViewport(0, (height - 0.825*width) / 2, width, 0.75*width - 10);
		}
	}
}

int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       // intialize GLUT  
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB ); // single framebuffer, colors in RGB
    glutInitWindowSize( width, height );                // Window size: 640 wide X 480 high
	glutInitWindowPosition(100,150);               // Top left corner at (100, 150)
    glutCreateWindow( "PoliBook" );            // Create Window

    glewInit();										// init glew

    initGPUBuffers();							   // Create GPU buffers

    shaderSetup( );                                // Connect this .cpp file to shader file

	//glViewport(0, 0, width, height);
	glPointSize(3);

	//drawPolylineFile("../Polylines/birdhead.dat");
	currentFile = 9;
    glutDisplayFunc( displayModeP );                    // Register display callback function
    glutKeyboardFunc( keyboard );                  // Register keyboard callback function
	glutMouseFunc(mouseModeP);

	// Can add minimalist menus here
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)
	glutReshapeFunc(resize);
	// enter the drawing loop
    glutMainLoop();
    return 0;
}
