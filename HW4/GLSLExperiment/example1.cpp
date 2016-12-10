// Draws colored cube  

#include "Angel.h"
#include "bmpread.h"
#include <complex>
typedef std::complex<float> Complex;

//----------------------------------------------------------------------------
int width = 0;
int height = 0;

// remember to prototype
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
//void quad( int a, int b, int c, int d );
//void colorcube(void);
//void drawCube(void);
void updateMinMax(float x, float y, float z);
void drawPly();
void idle();
void makeWalls();

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef struct {
	int v1;
	int v2;
	int v3;
} tri3;

// handle to program
GLuint program;

bmpread_t bitmap1;
bmpread_t bitmap2;

GLuint cubeTex;
bmpread_t cubePosX, cubeNegX, cubePosY, cubeNegY, cubePosZ, cubeNegZ;

GLuint textures[2];
bool drawTexture = true;

float currentTexMode;

using namespace std;

//# of verts and polys from plys
int numVerts;
int numPolys;

//points and polys from plys
point4 points[15000];
tri3 polys[25000];

point4 pointBuf[100000];
color4 colorBuf[100000];

point4 wallBuf[3][6];
color4 wallColor[6];
vec3 wallNorms[3][6];
vec2 texCoords[3][6];

//lighting
point4 light_position;
color4 light_ambient;
color4 light_diffuse;
color4 light_specular;

float light[3];
mat4 shadow_m;
bool drawShadows = false;

bool drawReflection = false;
bool drawRefraction = false;

//min and max variables for bounding box
float xmin, xmax, ymin, ymax, zmin, zmax;

float deltaRot = 0;

//normals
vec4 polyNorms[100000];
vec4 vecNorms[100000];
vec3 normals[100000];

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

void makeWalls() {
	//bottom
	wallBuf[0][0] = point4(0.0, 0.0, 0.0, 1.0);
	wallBuf[0][1] = point4(0.0, 0.0, 1.0, 1.0);
	wallBuf[0][2] = point4(1.0, 0.0, 1.0, 1.0);
	wallBuf[0][3] = point4(1.0, 0.0, 1.0, 1.0);
	wallBuf[0][4] = point4(1.0, 0.0, 0.0, 1.0);
	wallBuf[0][5] = point4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < 6; i++) {
		wallNorms[0][i] = vec3(0.0, 1.0, 0.0);
	}

	texCoords[0][0] = vec2(0.0, 0.0);
	texCoords[0][1] = vec2(0.0, 2.0);
	texCoords[0][2] = vec2(2.0, 2.0);
	texCoords[0][3] = vec2(2.0, 2.0);
	texCoords[0][4] = vec2(2.0, 0.0);
	texCoords[0][5] = vec2(0.0, 0.0);

	//right
	wallBuf[1][0] = point4(0.0, 0.0, 0.0, 1.0);
	wallBuf[1][1] = point4(1.0, 0.0, 0.0, 1.0);
	wallBuf[1][2] = point4(1.0, 1.0, 0.0, 1.0);
	wallBuf[1][3] = point4(1.0, 1.0, 0.0, 1.0);
	wallBuf[1][4] = point4(0.0, 1.0, 0.0, 1.0);
	wallBuf[1][5] = point4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < 6; i++) {
		wallNorms[1][i] = vec3(0.0, 0.0, 1.0);
	}

	texCoords[1][0] = vec2(0.0, 0.0);
	texCoords[1][1] = vec2(2.0, 0.0);
	texCoords[1][2] = vec2(2.0, 2.0);
	texCoords[1][3] = vec2(2.0, 2.0);
	texCoords[1][4] = vec2(0.0, 2.0);
	texCoords[1][5] = vec2(0.0, 0.0);

	//left
	wallBuf[2][0] = point4(0.0, 0.0, 0.0, 1.0);
	wallBuf[2][1] = point4(0.0, 1.0, 0.0, 1.0);
	wallBuf[2][2] = point4(0.0, 1.0, 1.0, 1.0);
	wallBuf[2][3] = point4(0.0, 1.0, 1.0, 1.0);
	wallBuf[2][4] = point4(0.0, 0.0, 1.0, 1.0);
	wallBuf[2][5] = point4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < 6; i++) {
		wallNorms[2][i] = vec3(1.0, 0.0, 0.0);
	}

	texCoords[2][0] = vec2(0.0, 0.0);
	texCoords[2][1] = vec2(0.0, 2.0);
	texCoords[2][2] = vec2(2.0, 2.0);
	texCoords[2][3] = vec2(2.0, 2.0);
	texCoords[2][4] = vec2(2.0, 0.0);
	texCoords[2][5] = vec2(0.0, 0.0);

	for (int i = 0; i < 6; i++) {
		wallColor[i] = color4(1.0, 1.0, 1.0, 1.0);
	}
}

void readPly() {

	FILE *file;

	char line[100];
	
	int lineVerts;
	int v1, v2, v3;
	float x, y, z;

	xmax = ymax = zmax = -99999;
	xmin = ymin = zmin = 99999;

	file = fopen("cow.ply", "rt");

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

void shaderSetup( void )
{	
	if (!bmpread("grass.bmp", 0, &bitmap1))
	{
		fprintf(stderr, "error loading grass");
		exit(1);
	}

	if (!bmpread("stones.bmp", 0, &bitmap2))
	{
		fprintf(stderr, "error loading stones");
		exit(1);
	}

	if (!bmpread("nvposx.bmp", 0, &cubePosX))
	{
		fprintf(stderr, "error loading cubePosX");
		exit(1);
	}

	if (!bmpread("nvnegx.bmp", 0, &cubeNegX))
	{
		fprintf(stderr, "error loading cubeNegX");
		exit(1);
	}

	if (!bmpread("nvposy.bmp", 0, &cubePosY))
	{
		fprintf(stderr, "error loading cubePosY");
		exit(1);
	}

	if (!bmpread("nvnegy.bmp", 0, &cubeNegY))
	{
		fprintf(stderr, "error loading cubeNegY");
		exit(1);
	}

	if (!bmpread("nvposz.bmp", 0, &cubePosZ))
	{
		fprintf(stderr, "error loading cubePosZ");
		exit(1);
	}

	if (!bmpread("nvnegz.bmp", 0, &cubeNegZ))
	{
		fprintf(stderr, "error loading cubeNegZ");
		exit(1);
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap1.width, bitmap1.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap1.rgb_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	bmpread_free(&bitmap1);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap2.width, bitmap2.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap2.rgb_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	bmpread_free(&bitmap2);

	glEnable(GL_TEXTURE_CUBE_MAP);
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &cubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, cubePosX.width, cubePosX.height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubePosX.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, cubeNegX.width, cubeNegX.height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeNegX.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, cubePosY.width, cubePosY.height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubePosY.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, cubeNegY.width, cubeNegY.height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeNegY.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, cubePosZ.width, cubePosZ.height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubePosZ.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, cubeNegZ.width, cubeNegZ.height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeNegZ.rgb_data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	bmpread_free(&cubePosX);
	bmpread_free(&cubeNegX);
	bmpread_free(&cubePosY);
	bmpread_free(&cubeNegY);
	bmpread_free(&cubePosZ);
	bmpread_free(&cubeNegZ);

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );

     // set up vertex arrays
	GLuint offset = 0;
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(offset) );
	offset += sizeof(wallBuf[0]);

	GLuint vNormals = glGetAttribLocation(program, "vNormals");
	glEnableVertexAttribArray(vNormals);
	glVertexAttribPointer(vNormals, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(wallNorms[0]);

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));

	//cube map
	GLuint texMapLocation = glGetUniformLocation(program, "texMap");
	glUniform1i(texMapLocation, 1);

	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	//init lighting
	light_position = point4(1.0, 1.0, 1.0, 0.0);
	light_ambient = color4(0.2, 0.2, 0.2, 1.0);
	light_diffuse = color4(1.0, 1.0, 1.0, 1.0);
	light_specular = color4(1.0, 1.0, 1.0, 1.0);

	light[0] = light_position.x;
	light[1] = light_position.y;
	light[2] = light_position.z;

	shadow_m = Angel::identity();
	shadow_m[3][1] = -1.0 / light[1];
	shadow_m[3][3] = 0.0;

	color4 material_ambient(1.0, 1.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float material_shininess = 5.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);

	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

void drawPly() {

	if (drawReflection) {
		glUniform1f(glGetUniformLocation(program, "doTexture"), 2.0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	}

	if (drawRefraction) {
		glUniform1f(glGetUniformLocation(program, "doTexture"), 3.0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	}

	//vars for Newell method
	float x1, x2, y1, y2, z1, z2, len;
	int n = 0;

	for (int i = 0; i < numPolys * 3; i = i + 3) {
		pointBuf[i] = points[polys[i / 3].v1];
		pointBuf[i + 1] = points[polys[i / 3].v2];
		pointBuf[i + 2] = points[polys[i / 3].v3];

		colorBuf[i] = color4(1.0, 0.5, 0.3, 1.0);
		colorBuf[i + 1] = color4(1.0, 0.5, 0.3, 1.0);
		colorBuf[i + 2] = color4(1.0, 0.5, 0.3, 1.0);

		vec3 tempNormal = normalize(cross(pointBuf[i + 1] - pointBuf[i], pointBuf[i + 2] - pointBuf[i + 1]));

		normals[i] = tempNormal;
		normals[i + 1] = tempNormal;
		normals[i + 2] = tempNormal;

		x1 = pointBuf[i].x - pointBuf[i + 1].x;
		x2 = pointBuf[i].x - pointBuf[i + 2].x;
		y1 = pointBuf[i].y - pointBuf[i + 1].y;
		y2 = pointBuf[i].y - pointBuf[i + 2].y;
		z1 = pointBuf[i].z - pointBuf[i + 1].z;
		z2 = pointBuf[i].z - pointBuf[i + 2].z;

		polyNorms[i] = vec4(((y1*z2) - (z1*y2)), ((z1*x2) - (x1*z2)), ((x1*y2) - (y1*x2)), 1.0);
		polyNorms[i + 1] = vec4(((y1*z2) - (z1*y2)), ((z1*x2) - (x1*z2)), ((x1*y2) - (y1*x2)), 1.0);
		polyNorms[i + 2] = vec4(((y1*z2) - (z1*y2)), ((z1*x2) - (x1*z2)), ((x1*y2) - (y1*x2)), 1.0);
	}

	GLuint offset = 0;
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(pointBuf);

	GLuint vNormals = glGetAttribLocation(program, "vNormals");
	glEnableVertexAttribArray(vNormals);
	glVertexAttribPointer(vNormals, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(normals);

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glDisableVertexAttribArray(vTexCoord);

	color4 ambient_product = light_ambient * color4(0.0, 0.0, 1.0, 1.0);
	color4 diffuse_product = light_diffuse * color4(1.0, 0.0, 1.0, 1.0);

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf), pointBuf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf), sizeof(normals), normals);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolys * 3);
	glDisable(GL_DEPTH_TEST);

	offset = 0;
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(wallBuf[0]);

	vNormals = glGetAttribLocation(program, "vNormals");
	glEnableVertexAttribArray(vNormals);
	glVertexAttribPointer(vNormals, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(wallNorms[0]);

	vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));

	if (drawReflection || drawRefraction) {
		glUniform1f(glGetUniformLocation(program, "doTexture"), currentTexMode);
	}
	
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
	readPly();
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	modelMat = modelMat * Angel::Translate(-(xmax+xmin)/2, -(ymax+ymin)/2, -sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2) + pow(zmax - zmin, 2))) * Angel::Translate(0.0f, 0.5f, -2.0f);
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

	modelMat = modelMat * Angel::Translate(0.0f, -0.8f, -2.0f) * Angel::RotateY(-45.0) * Angel::Scale(5.0f);
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];

	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	color4 ambient_product = light_ambient * color4(1.0, 1.0, 1.0, 1.0);
	color4 diffuse_product = light_diffuse * color4(1.0, 1.0, 1.0, 1.0);

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	if (drawTexture) {
		glUniform1f(glGetUniformLocation(program, "doTexture"), 1.0);
		currentTexMode = 1.0;
	}
	else {
		glUniform1f(glGetUniformLocation(program, "doTexture"), 0.0);
		currentTexMode = 0.0;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(wallBuf[0]) + sizeof(wallNorms[0]) + sizeof(texCoords[0]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(wallBuf[0]), wallBuf[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(wallBuf[0]), sizeof(wallNorms[0]), wallNorms[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(wallBuf[0]) + sizeof(wallNorms[0]), sizeof(texCoords[0]), texCoords[0]);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_DEPTH_TEST);
	
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallBuf[1]) + sizeof(wallNorms[1]) + sizeof(texCoords[1]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(wallBuf[1]), wallBuf[1]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(wallBuf[1]), sizeof(wallNorms[1]), wallNorms[1]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(wallBuf[1]) + sizeof(wallNorms[1]), sizeof(texCoords[1]), texCoords[1]);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_DEPTH_TEST);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallBuf[2]) + sizeof(wallNorms[2]) + sizeof(texCoords[2]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(wallBuf[2]), wallBuf[2]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(wallBuf[2]), sizeof(wallNorms[2]), wallNorms[2]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(wallBuf[2]) + sizeof(wallNorms[2]), sizeof(texCoords[2]), texCoords[2]);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_DEPTH_TEST);

	modelMat = modelMat * Angel::Translate(0.5f, 0.25f, 0.5f) * Angel::RotateY(deltaRot) * Angel::RotateY(45.0f) * Angel::Scale(0.1f);
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];

	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

	drawPly();

	if (drawShadows) {
		modelMat = modelMat * Angel::Translate(0.0f, -2.5f, 0.0f) * Angel::Translate(light[0], light[1], light[2]) * shadow_m * Angel::Translate(-light[0], -light[1], -light[2]) * Angel::Scale(0.3f);
		modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
		modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
		modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
		modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];

		modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
		modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
		modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
		modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];
		glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);

		ambient_product = light_ambient * color4(0.0, 0.0, 0.0, 1.0);
		diffuse_product = light_diffuse * color4(0.0, 0.0, 0.0, 1.0);

		glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
		glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

		GLuint offset = 0;
		GLuint vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		offset += sizeof(pointBuf);

		GLuint vNormals = glGetAttribLocation(program, "vNormals");
		glEnableVertexAttribArray(vNormals);
		glVertexAttribPointer(vNormals, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		offset += sizeof(polyNorms);

		GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
		glDisableVertexAttribArray(vTexCoord);

		glBufferData(GL_ARRAY_BUFFER, sizeof(pointBuf) + sizeof(polyNorms), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointBuf), pointBuf);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointBuf), sizeof(polyNorms), polyNorms);

		glEnable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, numPolys * 3);
		glDisable(GL_DEPTH_TEST);

		offset = 0;
		vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		offset += sizeof(wallBuf[0]);

		vNormals = glGetAttribLocation(program, "vNormals");
		glEnableVertexAttribArray(vNormals);
		glVertexAttribPointer(vNormals, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		offset += sizeof(wallNorms[0]);

		vTexCoord = glGetAttribLocation(program, "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
	}
	
	glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

//idle function. does all translating, rotating, and pulsing
void idle() {
	if (deltaRot == 360) {
		deltaRot = 0;
	}
	else {
		deltaRot += 1;
	}
	display();
}

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
	case 'A':
		drawShadows = !drawShadows;
		break;
	case 'B':
		drawTexture = !drawTexture;
		break;
	case 'C':
		drawReflection = !drawReflection;
		drawRefraction = false;
		break;
	case 'D':
		drawRefraction = !drawRefraction;
		drawReflection = false;
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

	makeWalls();

    shaderSetup();

	//readPly(fileNum);

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
