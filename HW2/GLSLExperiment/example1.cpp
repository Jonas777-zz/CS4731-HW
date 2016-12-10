// Draws colored cube  

#include "Angel.h"


//----------------------------------------------------------------------------
int width = 0;
int height = 0;

// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
//void quad( int a, int b, int c, int d );
//void colorcube(void);
//void drawCube(void);
void updateMinMax(float x, float y, float z);
void drawPly();
void idle();
mat4 ShearX(float h);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef struct {
	int v1;
	int v2;
	int v3;
} tri3;

enum idleMode {xPos, xNeg, yPos, yNeg, zPos, zNeg, rot, pulse, none};
idleMode currentIdle = none;

// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

//point4 points[NumVertices];
color4 colors[NumVertices];

//index for which ply to display
static int fileNum = 11;

//array of filenames
static char fileNames[43][25] = {
	"airplane.ply",
	"ant.ply",
	"apple.ply",
	"balance.ply",
	"beethoven.ply",
	"big_atc.ply",
	"big_dodge.ply",
	"big_porsche.ply",
	"big_spider.ply",
	"canstick.ply",
	"chopper.ply",
	"cow.ply",
	"dolphins.ply",
	"egret.ply",
	"f16.ply",
	"footbones.ply",
	"fracttree.ply",
	"galleon.ply",
	"hammerhead.ply",
	"helix.ply",
	"hind.ply",
	"kerolamp.ply",
	"ketchup.ply",
	"mug.ply",
	"part.ply",
	"pickup_big.ply",
	"pump.ply",
	"pumpa_tb.ply",
	"sandal.ply",
	"saratoga.ply",
	"scissors.ply",
	"shark.ply",
	"steeringweel.ply",
	"stratocaster.ply",
	"street_lamp.ply",
	"teapot.ply",
	"tennis_shoe.ply",
	"tommygun.ply",
	"trashcan.ply",
	"turbine.ply",
	"urn2.ply",
	"walkman.ply",
	"weathervane.ply"
};

//# of verts and polys from plys
int numVerts;
int numPolys;

//points and polys from plys
point4 points[15000];
tri3 polys[25000];

point4 pointBuf[100000];
color4 colorBuf[100000] = {color4(0.0, 0.0, 1.0, 1.0)};

//min and max variables for bounding box
float xmin, xmax, ymin, ymax, zmin, zmax;

//translation offsets
float deltaX = 0;
float deltaY = 0;
float deltaZ = 0;

//rotation stuff
bool rotating = false;
float deltaRot = 0;

//normals
bool drawNorms = false;
point4 polyNorms[25000];
point4 vecNorms[50000];

//bool doPulse = false;
float currentPulse = 0;
float pulseInc = 0.1f;
float tweaker = 0.001f;

//shearing
float shearX = 0;

//// Vertices of a unit cube centered at origin, sides aligned with axes
//point4 vertices[8] = {
//    point4( -0.5, -0.5,  0.5, 1.0 ),
//    point4( -0.5,  0.5,  0.5, 1.0 ),
//    point4(  0.5,  0.5,  0.5, 1.0 ),
//    point4(  0.5, -0.5,  0.5, 1.0 ),
//    point4( -0.5, -0.5, -0.5, 1.0 ),
//    point4( -0.5,  0.5, -0.5, 1.0 ),
//    point4(  0.5,  0.5, -0.5, 1.0 ),
//    point4(  0.5, -0.5, -0.5, 1.0 )
//};
//// RGBA olors
//color4 vertex_colors[8] = {
//    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
//    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
//    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
//    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
//    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
//    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
//    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
//    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
//};
// quad generates two triangles for each face and assigns colors
//    to the vertices
//int Index = 0;
//void quad( int a, int b, int c, int d )
//{
//    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
//    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
//    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
//    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
//    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
//    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
//}

void readPly(int index) {

	FILE *file;

	char line[100];
	
	int lineVerts;
	int v1, v2, v3;
	float x, y, z;

	xmax = ymax = zmax = -99999;
	xmin = ymin = zmin = 99999;

	file = fopen(fileNames[fileNum], "rt");

	while (!feof(file)) {
		memset(line, 0, 100);

		fscanf(file, "%s", line);

		if (strcmp(line, "vertex") == 0) {
			fscanf(file, "%d", &numVerts);
		} else if (strcmp(line, "face") == 0) {
			fscanf(file, "%d", &numPolys);
		} else if (strcmp(line, "end_header") == 0) {
			break;
		} else {
			continue;
		}
	}

	for (int i = 0; i < numVerts; i++) {
		fscanf(file, "%f %f %f", &x, &y, &z);
		points[i] = point4(x, y, z, 1);

		if (i == 0) {
			xmin = xmax = x;
			ymin = ymax = y;
			zmin = zmax = z;
		}
		updateMinMax(x, y, z);
	}

	for (int j = 0; j < numPolys; j++) {
		fscanf(file, "%d %d %d %d", &lineVerts, &v1, &v2, &v3);

		polys[j].v1 = v1;
		polys[j].v2 = v2;
		polys[j].v3 = v3;
	}

	fclose(file);
}

void updateMinMax(float x, float y, float z) {
	if (xmin > x)
		xmin = x;
	if (xmax < x)
		xmax = x;
	if (ymin > y)
		ymin = y;
	if (ymax < y)
		ymax = y;
	if (zmin > z)
		zmin = z;
	if (zmax < z)
		zmax = z;
}

// generate 12 triangles: 36 vertices and 36 colors
//void colorcube()
//{
//    quad( 1, 0, 3, 2 );
//    quad( 2, 3, 7, 6 );
//    quad( 3, 0, 4, 7 );
//    quad( 6, 5, 1, 2 );
//    quad( 4, 5, 6, 7 );
//    quad( 5, 4, 0, 1 );
//}

void generateGeometry( void )
{	
    //colorcube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    //glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  //NULL, GL_STATIC_DRAW );
    //glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    //glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );


	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );
     // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//void drawCube(void)
//{
//	// change to GL_FILL
//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
//	// draw functions should enable then disable the features 
//	// that are specifit the themselves
//	// the depth is disabled after the draw 
//	// in case you need to draw overlays
//	glEnable( GL_DEPTH_TEST );
//    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
//	glDisable( GL_DEPTH_TEST ); 
//}

void drawPly() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//vars for Newell method
	float x1, x2, y1, y2, z1, z2, len;
	int n = 0;

	for (int i = 0; i < numPolys * 3; i = i + 3) {
		pointBuf[i] = points[polys[i / 3].v1];
		pointBuf[i + 1] = points[polys[i / 3].v2];
		pointBuf[i + 2] = points[polys[i / 3].v3];

		x1 = pointBuf[i].x - pointBuf[i + 1].x;
		x2 = pointBuf[i].x - pointBuf[i + 2].x;
		y1 = pointBuf[i].y - pointBuf[i + 1].y;
		y2 = pointBuf[i].y - pointBuf[i + 2].y;
		z1 = pointBuf[i].z - pointBuf[i + 1].z;
		z2 = pointBuf[i].z - pointBuf[i + 2].z;

		len = sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2) + pow(zmax - zmin, 2)) / 50;

		polyNorms[i / 3] = point4(((y1*z2) - (z1*y2)) / len, ((z1*x2) - (x1*z2)) / len, ((x1*y2) - (y1*x2)) / len, 1);
		
		vecNorms[n] = point4((pointBuf[i].x + pointBuf[i + 1].x + pointBuf[i + 2].x) / 3, (pointBuf[i].y + pointBuf[i + 1].y + pointBuf[i + 2].y) / 3, (pointBuf[i].z + pointBuf[i + 1].z + pointBuf[i + 2].z) / 3, 1);
		n++;
		vecNorms[n] = point4(vecNorms[n-1].x + polyNorms[i / 3].x, vecNorms[n-1].y + polyNorms[i / 3].y, vecNorms[n-1].z + polyNorms[i / 3].z, 1);
		n++;

		//pulse stuff
		if (currentIdle == pulse) {

			float adjust1 = sqrt(pow(polyNorms[i / 3].x, 2) + pow(polyNorms[i / 3].y, 2) + pow(polyNorms[i / 3].z, 2)) / sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2) + pow(zmax - zmin, 2));
			float adjust2 = sqrt(pow(polyNorms[(i + 1) / 3].x, 2) + pow(polyNorms[(i + 1) / 3].y, 2) + pow(polyNorms[(i + 1) / 3].z, 2)) / sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2) + pow(zmax - zmin, 2));
			float adjust3 = sqrt(pow(polyNorms[(i + 2) / 3].x, 2) + pow(polyNorms[(i + 2) / 3].y, 2) + pow(polyNorms[(i + 2) / 3].z, 2)) / sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2) + pow(zmax - zmin, 2));

			pointBuf[i].x += tweaker * currentPulse * polyNorms[i / 3].x / adjust1;
			pointBuf[i + 1].x += tweaker * currentPulse * polyNorms[(i + 1) / 3].x / adjust2;
			pointBuf[i + 2].x += tweaker * currentPulse * polyNorms[(i + 2) / 3].x / adjust3;

			pointBuf[i].y += tweaker * currentPulse * polyNorms[i / 3].y / adjust1;
			pointBuf[i + 1].y += tweaker * currentPulse * polyNorms[(i + 1) / 3].y / adjust2;
			pointBuf[i + 2].y += tweaker * currentPulse * polyNorms[(i + 2) / 3].y / adjust3;

			pointBuf[i].z += tweaker * currentPulse * polyNorms[i / 3].z / adjust1;
			pointBuf[i + 1].z += tweaker * currentPulse * polyNorms[(i + 1) / 3].z / adjust2;
			pointBuf[i + 2].z += tweaker * currentPulse * polyNorms[(i + 2) / 3].z / adjust3;
		}
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf) + sizeof(colorBuf), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf), pointBuf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf), sizeof(colorBuf), colorBuf);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys * 3);
	glDisable(GL_DEPTH_TEST);

	if (drawNorms) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(vecNorms), vecNorms, GL_STATIC_DRAW);

		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINES, 0, numPolys * 2);
		glDisable(GL_DEPTH_TEST);
	}

	glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
	readPly(fileNum);
	// SOME RANDOM TIPS
	//========================================================================
	// remember to enable depth buffering when drawing in 3d

	// avoid using glTranslatex, glRotatex, push and pop
	// pass your own view matrix to the shader directly
	// refer to the latest OpenGL documentation for implementation details

    // Do not set the near and far plane too far appart!
	// depth buffers do not have unlimited resolution
	// surfaces will start to fight as they come nearer to each other
	// if the planes are too far appart (quantization errors :(   )

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)60.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 10000.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(-(xmax+xmin)/2 + deltaX, -(ymax+ymin)/2 + deltaY, -sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2) + pow(zmax - zmin, 2)) + deltaZ) * Angel::RotateY(0.0f) * Angel::RotateX(deltaRot) * ShearX(shearX);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	drawPly();

	//drawCube();
    //glFlush(); // force output to graphics hardware

	// use this call to double buffer
	//glutSwapBuffers();
	// you can implement your own buffers with textures
}

//create transform matrix for shearing
mat4 ShearX(float h) {
	mat4 c;
	c[0][1] = h;
	return c;
}

//----------------------------------------------------------------------------

//idle function. does all translating, rotating, and pulsing
void idle() {

	switch (currentIdle) {
	case xPos:
		deltaX += (xmax - xmin) / 100;
		display();
		break;
	case xNeg:
		deltaX -= (xmax - xmin) / 100;
		display();
		break;
	case yPos:
		deltaY += (ymax - ymin) / 100;
		display();
		break;
	case yNeg:
		deltaY -= (ymax - ymin) / 100;
		display();
		break;
	case zPos:
		deltaZ += (zmax - zmin) / 100;
		display();
		break;
	case zNeg:
		deltaZ -= (zmax - zmin) / 100;
		display();
		break;
	case rot:
		if (deltaRot == 360) {
			deltaRot = 0;
		}
		else {
			deltaRot += 1;
		}
		display();
		break;
	case pulse:
		if (currentPulse >= 10.0f) {
			pulseInc = -0.1f;
		}
		else if (currentPulse <= 0.0f) {
			pulseInc = 0.1f;
		}
		currentPulse += pulseInc;
		display();
		break;
	case none:
		break;
	}

}

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
	case 'W':
		//fileNum = 0;
		shearX = 0;
		currentIdle = none;
		deltaX = deltaY = deltaZ = deltaRot = 0;
		display();
		break;
	case 'N':
		if (fileNum == 42) {
			fileNum = 0;
		}
		else {
			fileNum++;
		}
		shearX = 0;
		deltaX = deltaY = deltaZ = deltaRot = 0;
		currentIdle = none;
		display();
		break;
	case 'P':
		if (fileNum == 0) {
			fileNum = 42;
		}
		else {
			fileNum--;
		}
		shearX = 0;
		deltaX = deltaY = deltaZ = deltaRot = 0;
		currentIdle = none;
		display();
		break;
	case 'X':
		if (currentIdle == xPos) {
			currentIdle = none;
		}
		else {
			currentIdle = xPos;
		}
		break;
	case 'x':
		if (currentIdle == xNeg) {
			currentIdle = none;
		}
		else {
			currentIdle = xNeg;
		}
		break;
	case 'Y':
		if (currentIdle == yPos) {
			currentIdle = none;
		}
		else {
			currentIdle = yPos;
		}
		break;
	case 'y':
		if (currentIdle == yNeg) {
			currentIdle = none;
		}
		else {
			currentIdle = yNeg;
		}
		break;
	case 'Z':
		if (currentIdle == zPos)
			currentIdle = none;
		else
			currentIdle = zPos;
		break;
	case 'z':
		if (currentIdle == zNeg)
			currentIdle = none;
		else
			currentIdle = zNeg;
		break;
	case 'R':
		if (currentIdle == rot)
			currentIdle = none;
		else
			currentIdle = rot;
		break;
	case 'm':
		if (drawNorms) {
			drawNorms = false;
		}
		else {
			drawNorms = true;
		}
		display();
		break;
	case 'B':
		if (currentIdle == pulse) {
			currentIdle = none;
		}
		else {
			currentPulse = 0;
			pulseInc = 0.1f;
			currentIdle = pulse;
		}
		display();
		break;
	case 'h':
		shearX += 1;
		display();
		break;
	case 'H':
		shearX -= 1;
		display();
		break;
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "Color Cube" );

	// init glew
    glewInit();

    generateGeometry();

	//readPly(fileNum);

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutIdleFunc(idle);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
