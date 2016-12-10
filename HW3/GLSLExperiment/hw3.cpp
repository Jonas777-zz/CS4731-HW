
#include "Angel.h"

int width = 0;
int height = 0;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef Angel::vec3 point3;
typedef struct {
	int v1;
	int v2;
	int v3;
} tri3;

void display();
void keyboard(unsigned char key, int x, int y);
void idle();
void  readPly(int index);
//void drawPly();
void updateMinMax(float x, float y, float z, int index);
void shaderSetup();
void pushMatrix();
void popMatrix();
float* floatify(mat4 m);
void lines();
void bounds(int i);

GLuint program;

using namespace std;

float xmin[6];
float xmax[6];
float ymin[6];
float ymax[6];
float zmin[6];
float zmax[6];

//static int fileNum = 0;
static char fileNames[6][25] = {
	"cow.ply",
	"helix.ply",
	"tommygun.ply",
	"pickup_big.ply",
	"sandal.ply",
	"street_lamp.ply"
};

float lvlRot[3] = { 0 };

bool doWiggle = false;
float wiggle = 0;
float wiggleInc = 1;

//# of verts and polys from plys
int numVerts[6];
int numPolys[6];

//points and polys from plys
point4 points[6][15000];
tri3 polys[6][25000];

point3 polyNorms[6][25000];
point3 vecNorms[6][50000];

//point4 pointBuf[100000];
color4 colorBuf[6][100000];

point4 pointBuf[6][100000];

point4 lineBuf[3][50];

bool drawBounds = false;
point4 boundBuf[6][16];

//point4 cowBuf[100000];
//point4 helixBuf[100000];
//point4 pickupBuf[100000];
//point4 sandalBuf[100000];
//point4 lampBug[100000];
//point4 gunBuf[100000];

color4 red = color4(1.0, 0.0, 0.0, 1.0);
color4 blue = color4(0.0, 0.0, 1.0, 1.0);
color4 green = color4(0.0, 1.0, 0.0, 1.0);
color4 purp = color4(1.0, 0.0, 1.0, 1.0);
color4 turq = color4(0.0, 1.0, 1.0, 1.0);
color4 yel = color4(1.0, 1.0, 0.0, 1.0);

mat4 matrixStack[20] = {Angel::identity()};
mat4 ctm;
int stackIndex = 0;

float shiny = 100.0;
point4 lightPosition;

color4 ambientProduct, diffuseProduct, specularProduct;
color4 lightAmbient, lightDiffuse, lightSpecular;
color4 matAmbient, matDiffuse, matSpecular;

void pushMatrix() {
	matrixStack[stackIndex] = ctm;
	stackIndex++;
	matrixStack[stackIndex] = ctm;
}

void popMatrix() {
	matrixStack[stackIndex] = Angel::identity();
	stackIndex--;
	ctm = matrixStack[stackIndex];
}

void shaderSetup(void)
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));
	
	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(pointBuf[0])));

	lightPosition = point4(1.0, 2.0, 3.0, 0.0);

	lightAmbient = color4(0.2, 0.2, 0.2, 1.0);
	lightDiffuse = color4(1.0, 1.0, 1.0, 1.0);
	lightSpecular = color4(1.0, 1.0, 1.0, 1.0);

	matAmbient = color4(0.2, 0.2, 0.2, 1.0);
	matDiffuse = color4(1.0, 0.8, 0.0, 1.0);
	matSpecular = color4(1.0, 1.0, 1.0, 1.0);

	ambientProduct = lightAmbient * matAmbient;
	diffuseProduct = lightDiffuse * matDiffuse;
	specularProduct = lightSpecular * matSpecular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambientProduct);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specularProduct);

	glUniform4fv(glGetUniformLocation(program, "lightPos"), 1, lightPosition);

	glUniform1f(glGetUniformLocation(program, "shininess"), shiny);

	// sets the default color to clear screen
	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

void lines() {
	//top level connector: cow to other stuff
	lineBuf[0][0] = point4((xmax[3] + xmin[3]) / 2, ymax[3], (zmax[3] + zmin[3]) / 2, 1.0);
	lineBuf[0][1] = point4((xmax[3] + xmin[3]) / 2, ymax[3] + 3, (zmax[3] + zmin[3]) / 2, 1.0);
	lineBuf[0][2] = point4((xmax[1] + xmin[1]) / 2, ymax[1] + 3, (zmax[1] + zmin[1]) / 2, 1.0);
	lineBuf[0][3] = point4((xmax[1] + xmin[1]) / 2, ymax[1], (zmax[1] + zmin[1]) / 2, 1.0);
}

void bounds(int i) {
	boundBuf[i][0] = point4(xmin[i], ymax[i], zmax[i], 1);
	boundBuf[i][1] = point4(xmax[i], ymax[i], zmax[i], 1);
	boundBuf[i][2] = point4(xmax[i], ymax[i], zmin[i], 1);
	boundBuf[i][3] = point4(xmin[i], ymax[i], zmin[i], 1);

	boundBuf[i][4] = point4(xmin[i], ymax[i], zmax[i], 1);
	boundBuf[i][5] = point4(xmin[i], ymin[i], zmax[i], 1);

	boundBuf[i][6] = point4(xmax[i], ymin[i], zmax[i], 1);
	boundBuf[i][7] = point4(xmax[i], ymax[i], zmax[i], 1);
	boundBuf[i][8] = point4(xmax[i], ymin[i], zmax[i], 1);

	boundBuf[i][9] = point4(xmax[i], ymin[i], zmin[i], 1);
	boundBuf[i][10] = point4(xmax[i], ymax[i], zmin[i], 1);
	boundBuf[i][11] = point4(xmax[i], ymin[i], zmin[i], 1);
	
	boundBuf[i][12] = point4(xmin[i], ymin[i], zmin[i], 1);
	boundBuf[i][13] = point4(xmin[i], ymax[i], zmin[i], 1);
	boundBuf[i][14] = point4(xmin[i], ymin[i], zmin[i], 1);
	boundBuf[i][15] = point4(xmin[i], ymin[i], zmax[i], 1);
}

void readPly(int index) {

	FILE *file;

	char line[100];

	int lineVerts;
	int v1, v2, v3;
	float x, y, z;
	float x1, x2, y1, y2, z1, z2;
	int n = 0;

	xmax[index] = -99999;
	ymax[index] = -99999;
	zmax[index] = -99999;
	xmin[index] = 99999;
	ymin[index] = 99999;
	zmin[index] = 99999;

	file = fopen(fileNames[index], "rt");

	while (!feof(file)) {
		memset(line, 0, 100);

		fscanf(file, "%s", line);

		if (strcmp(line, "vertex") == 0) {
			fscanf(file, "%d", &numVerts[index]);
		}
		else if (strcmp(line, "face") == 0) {
			fscanf(file, "%d", &numPolys[index]);
		}
		else if (strcmp(line, "end_header") == 0) {
			break;
		}
		else {
			continue;
		}
	}

	for (int i = 0; i < numVerts[index]; i++) {
		fscanf(file, "%f %f %f", &x, &y, &z);
		points[index][i] = point4(x, y, z, 1);

		if (i == 0) {
			xmin[index] = x;
			xmax[index] = x;
			ymin[index] = y;
			ymax[index] = y;
			zmin[index] = z;
			zmax[index] = z;
		}
		updateMinMax(x, y, z, index);
	}

	for (int j = 0; j < numPolys[index]; j++) {
		fscanf(file, "%d %d %d %d", &lineVerts, &v1, &v2, &v3);

		polys[index][j].v1 = v1;
		polys[index][j].v2 = v2;
		polys[index][j].v3 = v3;
	}

	for (int i = 0; i < numPolys[index] * 3; i = i + 3) {
		pointBuf[index][i] = points[index][polys[index][i / 3].v1];
		pointBuf[index][i + 1] = points[index][polys[index][i / 3].v2];
		pointBuf[index][i + 2] = points[index][polys[index][i / 3].v3];

		x1 = pointBuf[index][i].x - pointBuf[index][i + 1].x;
		x2 = pointBuf[index][i].x - pointBuf[index][i + 2].x;
		y1 = pointBuf[index][i].y - pointBuf[index][i + 1].y;
		y2 = pointBuf[index][i].y - pointBuf[index][i + 2].y;
		z1 = pointBuf[index][i].z - pointBuf[index][i + 1].z;
		z2 = pointBuf[index][i].z - pointBuf[index][i + 2].z;
		
		polyNorms[index][i / 3] = point3(((y1*z2) - (z1*y2)), ((z1*x2) - (x1*z2)), ((x1*y2) - (y1*x2)));
		
		vecNorms[index][n] = point3((pointBuf[index][i].x + pointBuf[index][i + 1].x + pointBuf[index][i + 2].x) / 3, (pointBuf[index][i].y + pointBuf[index][i + 1].y + pointBuf[index][i + 2].y) / 3, (pointBuf[index][i].z + pointBuf[index][i + 1].z + pointBuf[index][i + 2].z) / 3);
		n++;
		vecNorms[index][n] = point3(vecNorms[index][n-1].x + polyNorms[index][i / 3].x, vecNorms[index][n-1].y + polyNorms[index][i / 3].y, vecNorms[index][n-1].z + polyNorms[index][i / 3].z);
		n++;

		switch (index) {
		case 0:
			colorBuf[0][i] = purp;
			colorBuf[0][i + 1] = purp;
			colorBuf[0][i + 2] = purp;
			break;
		case 1:
			colorBuf[1][i] = red;
			colorBuf[1][i + 1] = red;
			colorBuf[1][i + 2] = red;
			break;
		case 2:
			colorBuf[2][i] = blue;
			colorBuf[2][i + 1] = blue;
			colorBuf[2][i + 2] = blue;
			break;
		case 3:
			colorBuf[3][i] = green;
			colorBuf[3][i + 1] = green;
			colorBuf[3][i + 2] = green;
			break;
		case 4:
			colorBuf[4][i] = turq;
			colorBuf[4][i + 1] = turq;
			colorBuf[4][i + 2] = turq;
			break;
		case 5:
			colorBuf[5][i] = yel;
			colorBuf[5][i + 1] = yel;
			colorBuf[5][i + 2] = yel;
			break;
		default:
			colorBuf[index][i] = purp;
			colorBuf[index][i + 1] = purp;
			colorBuf[index][i + 2] = purp;
			break;
		}
	}

	fclose(file);
}

void updateMinMax(float x, float y, float z, int index) {
	if (xmin[index] > x)
		xmin[index] = x;
	if (xmax[index] < x)
		xmax[index] = x;
	if (ymin[index] > y)
		ymin[index] = y;
	if (ymax[index] < y)
		ymax[index] = y;
	if (zmin[index] > z)
		zmin[index] = z;
	if (zmax[index] < z)
		zmax[index] = z;
}

void display(void)
{
	//readPly(fileNum);
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)60.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 10000.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0]; viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0]; viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0]; viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0]; viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2]; viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2]; viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2]; viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2]; viewMatrixf[15] = perspectiveMat[3][3];

	ctm = Angel::identity();
	ctm = ctm * Angel::Translate(-(xmax[0] + xmin[0]) / 2 , -(ymax[0] + ymin[0]) / 2 , -sqrt(pow(xmax[0] - xmin[0], 2) + pow(ymax[0] - ymin[0], 2) + pow(zmax[0] - zmin[0], 2))) * Angel::Translate(0.0, 3.5, 0.0) * Angel::RotateY(lvlRot[0]) * Angel::Scale(0.4f);

	float modelMatrixf[16];
	modelMatrixf[0] = ctm[0][0]; modelMatrixf[4] = ctm[0][1];
	modelMatrixf[1] = ctm[1][0]; modelMatrixf[5] = ctm[1][1];
	modelMatrixf[2] = ctm[2][0]; modelMatrixf[6] = ctm[2][1];
	modelMatrixf[3] = ctm[3][0]; modelMatrixf[7] = ctm[3][1];
	modelMatrixf[8] = ctm[0][2]; modelMatrixf[12] = ctm[0][3];
	modelMatrixf[9] = ctm[1][2]; modelMatrixf[13] = ctm[1][3];
	modelMatrixf[10] = ctm[2][2]; modelMatrixf[14] = ctm[2][3];
	modelMatrixf[11] = ctm[3][2]; modelMatrixf[15] = ctm[3][3];

	// set up projection matrices
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewMatrixf);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	pushMatrix();

	diffuseProduct = lightDiffuse * colorBuf[0][0];
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);
	
	//draw cow
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf[0]) + sizeof(vecNorms[0]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf[0]), pointBuf[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf[0]), sizeof(vecNorms[0]), vecNorms[0]);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys[0] * 3);
	glDisable(GL_DEPTH_TEST);

	/*point4 vecNormsD[60000];
	for (int i = 0; i < 60000; i++) {
		vecNormsD[i] = point4(vecNorms[0][i], 1.0);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(vecNormsD[0]), vecNormsD[0], GL_STATIC_DRAW);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINES, 0, numPolys[0] * 2);
	glDisable(GL_DEPTH_TEST);*/

	////draw lines
	//glBufferData(GL_ARRAY_BUFFER, sizeof(lineBuf[0]), lineBuf[0], GL_STATIC_DRAW);
	//glEnable(GL_DEPTH_TEST);
	//glDrawArrays(GL_LINE_STRIP, 0, 4);
	//glDisable(GL_DEPTH_TEST);

	//draw bounds
	if (drawBounds) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(boundBuf[0]), boundBuf[0], GL_STATIC_DRAW);
		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINE_STRIP, 0, 16);
		glDisable(GL_DEPTH_TEST);
	}

	pushMatrix();

	//transform for helix
	ctm = ctm * Angel::Translate(7.0, -4.0 + wiggle, 0.0) * Angel::RotateY(lvlRot[1]) * Angel::Scale(0.05f);
	modelMatrixf[0] = ctm[0][0]; modelMatrixf[4] = ctm[0][1];
	modelMatrixf[1] = ctm[1][0]; modelMatrixf[5] = ctm[1][1];
	modelMatrixf[2] = ctm[2][0]; modelMatrixf[6] = ctm[2][1];
	modelMatrixf[3] = ctm[3][0]; modelMatrixf[7] = ctm[3][1];
	modelMatrixf[8] = ctm[0][2]; modelMatrixf[12] = ctm[0][3];
	modelMatrixf[9] = ctm[1][2]; modelMatrixf[13] = ctm[1][3];
	modelMatrixf[10] = ctm[2][2]; modelMatrixf[14] = ctm[2][3];
	modelMatrixf[11] = ctm[3][2]; modelMatrixf[15] = ctm[3][3];
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	diffuseProduct = lightDiffuse * colorBuf[1][0];
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);

	//draw helix
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf[1]) + sizeof(vecNorms[1]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf[1]), pointBuf[1]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf[1]), sizeof(vecNorms[1]), vecNorms[1]);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys[1] * 3);
	glDisable(GL_DEPTH_TEST);

	//draw bounds
	if (drawBounds) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(boundBuf[1]), boundBuf[1], GL_STATIC_DRAW);
		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINE_STRIP, 0, 16);
		glDisable(GL_DEPTH_TEST);
	}

	pushMatrix();

	//transform for tommygun
	ctm = ctm * Angel::Translate(0.0, -100.0 + wiggle, 0.0) * Angel::RotateY(lvlRot[2]) * Angel::Scale(30.0f);
	modelMatrixf[0] = ctm[0][0]; modelMatrixf[4] = ctm[0][1];
	modelMatrixf[1] = ctm[1][0]; modelMatrixf[5] = ctm[1][1];
	modelMatrixf[2] = ctm[2][0]; modelMatrixf[6] = ctm[2][1];
	modelMatrixf[3] = ctm[3][0]; modelMatrixf[7] = ctm[3][1];
	modelMatrixf[8] = ctm[0][2]; modelMatrixf[12] = ctm[0][3];
	modelMatrixf[9] = ctm[1][2]; modelMatrixf[13] = ctm[1][3];
	modelMatrixf[10] = ctm[2][2]; modelMatrixf[14] = ctm[2][3];
	modelMatrixf[11] = ctm[3][2]; modelMatrixf[15] = ctm[3][3];
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	diffuseProduct = lightDiffuse * colorBuf[2][0];
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);

	//draw tommygun
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf[2]) + sizeof(vecNorms[2]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf[2]), pointBuf[2]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf[2]), sizeof(vecNorms[2]), vecNorms[2]);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys[2] * 3);
	glDisable(GL_DEPTH_TEST);

	//draw bounds
	if (drawBounds) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(boundBuf[2]), boundBuf[2], GL_STATIC_DRAW);
		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINE_STRIP, 0, 16);
		glDisable(GL_DEPTH_TEST);
	}

	popMatrix();
	popMatrix();
	pushMatrix();

	//transform for pickup
	ctm = ctm * Angel::Translate(-7.0, -2.5 - wiggle, 0.0) * Angel::RotateY(lvlRot[1]) * Angel::Scale(0.7f);
	modelMatrixf[0] = ctm[0][0]; modelMatrixf[4] = ctm[0][1];
	modelMatrixf[1] = ctm[1][0]; modelMatrixf[5] = ctm[1][1];
	modelMatrixf[2] = ctm[2][0]; modelMatrixf[6] = ctm[2][1];
	modelMatrixf[3] = ctm[3][0]; modelMatrixf[7] = ctm[3][1];
	modelMatrixf[8] = ctm[0][2]; modelMatrixf[12] = ctm[0][3];
	modelMatrixf[9] = ctm[1][2]; modelMatrixf[13] = ctm[1][3];
	modelMatrixf[10] = ctm[2][2]; modelMatrixf[14] = ctm[2][3];
	modelMatrixf[11] = ctm[3][2]; modelMatrixf[15] = ctm[3][3];
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	diffuseProduct = lightDiffuse * colorBuf[3][0];
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);

	//draw pickup
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf[3]) + sizeof(vecNorms[3]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf[3]), pointBuf[3]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf[3]), sizeof(vecNorms[3]), vecNorms[3]);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys[3] * 3);
	glDisable(GL_DEPTH_TEST);

	//draw bounds
	if (drawBounds) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(boundBuf[3]), boundBuf[3], GL_STATIC_DRAW);
		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINE_STRIP, 0, 16);
		glDisable(GL_DEPTH_TEST);
	}

	pushMatrix();

	//transform for sandal
	ctm = ctm * Angel::Translate(5.0, -10.0 + wiggle, 0.0) * Angel::RotateY(lvlRot[2]) * Angel::Scale(0.8f);
	modelMatrixf[0] = ctm[0][0]; modelMatrixf[4] = ctm[0][1];
	modelMatrixf[1] = ctm[1][0]; modelMatrixf[5] = ctm[1][1];
	modelMatrixf[2] = ctm[2][0]; modelMatrixf[6] = ctm[2][1];
	modelMatrixf[3] = ctm[3][0]; modelMatrixf[7] = ctm[3][1];
	modelMatrixf[8] = ctm[0][2]; modelMatrixf[12] = ctm[0][3];
	modelMatrixf[9] = ctm[1][2]; modelMatrixf[13] = ctm[1][3];
	modelMatrixf[10] = ctm[2][2]; modelMatrixf[14] = ctm[2][3];
	modelMatrixf[11] = ctm[3][2]; modelMatrixf[15] = ctm[3][3];
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	diffuseProduct = lightDiffuse * colorBuf[4][0];
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);

	//draw sandal
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf[4]) + sizeof(vecNorms[4]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf[4]), pointBuf[4]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf[4]), sizeof(vecNorms[4]), vecNorms[4]);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys[4] * 3);
	glDisable(GL_DEPTH_TEST);

	//draw bounds
	if (drawBounds) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(boundBuf[4]), boundBuf[4], GL_STATIC_DRAW);
		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINE_STRIP, 0, 16);
		glDisable(GL_DEPTH_TEST);
	}

	popMatrix();
	pushMatrix();

	//transform for street lamp
	ctm = ctm * Angel::Translate(-10.0, -10.0 + wiggle, 0.0) * Angel::RotateY(lvlRot[2]) * Angel::Scale(0.8f);
	modelMatrixf[0] = ctm[0][0]; modelMatrixf[4] = ctm[0][1];
	modelMatrixf[1] = ctm[1][0]; modelMatrixf[5] = ctm[1][1];
	modelMatrixf[2] = ctm[2][0]; modelMatrixf[6] = ctm[2][1];
	modelMatrixf[3] = ctm[3][0]; modelMatrixf[7] = ctm[3][1];
	modelMatrixf[8] = ctm[0][2]; modelMatrixf[12] = ctm[0][3];
	modelMatrixf[9] = ctm[1][2]; modelMatrixf[13] = ctm[1][3];
	modelMatrixf[10] = ctm[2][2]; modelMatrixf[14] = ctm[2][3];
	modelMatrixf[11] = ctm[3][2]; modelMatrixf[15] = ctm[3][3];
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	diffuseProduct = lightDiffuse * colorBuf[5][0];
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuseProduct);

	//draw street lamp
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf[5]) + sizeof(vecNorms[5]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf[5]), pointBuf[5]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf[5]), sizeof(vecNorms[5]), vecNorms[5]);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys[5] * 3);
	glDisable(GL_DEPTH_TEST);

	//draw bounds
	if (drawBounds) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(boundBuf[5]), boundBuf[5], GL_STATIC_DRAW);
		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINE_STRIP, 0, 16);
		glDisable(GL_DEPTH_TEST);
	}

	glFlush();
	glutSwapBuffers();
}

float* floatify(mat4 m) {
	float fmat[16];
	fmat[0] = m[0][0]; fmat[4] = m[0][1];
	fmat[1] = m[1][0]; fmat[5] = m[1][1];
	fmat[2] = m[2][0]; fmat[6] = m[2][1];
	fmat[3] = m[3][0]; fmat[7] = m[3][1];

	fmat[8] = m[0][2]; fmat[12] = m[0][3];
	fmat[9] = m[1][2]; fmat[13] = m[1][3];
	fmat[10] = m[2][2]; fmat[14] = m[2][3];
	fmat[11] = m[3][2]; fmat[15] = m[3][3];
	return fmat;
}

//void drawPly() {
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	for (int i = 0; i < numPolys * 3; i = i + 3) {
//		pointBuf[i] = points[polys[i / 3].v1];
//		pointBuf[i + 1] = points[polys[i / 3].v2];
//		pointBuf[i + 2] = points[polys[i / 3].v3];
//
//		colorBuf[i] = blue;
//		colorBuf[i + 1] = blue;
//		colorBuf[i + 2] = blue;
//	}
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf) + sizeof(colorBuf), NULL, GL_STATIC_DRAW);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf), pointBuf);
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf), sizeof(colorBuf), colorBuf);
//
//	glEnable(GL_DEPTH_TEST);
//	glDrawArrays(GL_TRIANGLES, 0, numPolys * 3);
//	glDisable(GL_DEPTH_TEST);
//
//	glFlush();
//	glutSwapBuffers();
//}

void idle() {
	lvlRot[0] += 0.4f;
	lvlRot[1] -= 1.0f;
	lvlRot[2] += 3.0f;

	if (doWiggle) {
		if (wiggle >= 2.0f) {
			wiggleInc = -0.03f;
		}
		else if (wiggle <= -2.0f) {
			wiggleInc = 0.03f;
		}
		wiggle += wiggleInc;
	}

	display();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case 'e':
		drawBounds = !drawBounds;
		break;
	case 's':
		doWiggle = !doWiggle;
		break;
	}
}

int main(int argc, char **argv)
{
	// init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	width = 1920;
	height = 1080;

	readPly(0);
	readPly(1);
	readPly(2);
	readPly(3);
	readPly(4);
	readPly(5);

	lines();

	bounds(0);
	bounds(1);
	bounds(2);
	bounds(3);
	bounds(4);
	bounds(5);

	lvlRot[0] = 0;
	lvlRot[1] = 0;
	lvlRot[2] = 0;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
	glutCreateWindow("Kinetic Sculpture");

	// init glew
	glewInit();

	shaderSetup();

	// assign handlers
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
	glutMainLoop();
	return 0;
}