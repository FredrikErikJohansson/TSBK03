// Revised 2019 with a bit better variable names.

// This worked 2015: 
// Linux: gcc lab0.c ../common/*.c ../common/Linux/MicroGlut.c -lGL -o lab0 -I../common -I../common/Linux
// Mac: gcc lab0.c ../common/*.c ../common/Mac/MicroGlut.m -o lab0 -framework OpenGL -framework Cocoa -I../common/Mac -I../common

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	//uses framework Cocoa
#else
	#include <GL/gl.h>
	#include "MicroGlut.h"
#endif
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

//constants
const int initWidth=512, initHeight=512;

// Model-to-world matrix
// Modify this matrix.
// See below for how it is applied to your model.
mat4 bunny_model = {{ 1.0, 0.0, 0.0, -0.5,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, -0.8,
					0.0, 0.0, 0.0, 1.0}};
vec3 bunny_color = {1,0.6,0.2};
mat4 teddy_model = {{ 1.0, 0.0, 0.0, 0.5,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.2,
					0.0, 0.0, 0.0, 1.0}};
vec3 teddy_color = {0.6,0.2,1};
// World-to-view matrix. Usually set by lookAt() or similar.
mat4 view;
// Projection matrix, set by a call to perspective().
mat4 projection;

vec3 light_pos = {1,1,1};
vec3 light_color = {1,1,1};

// Globals
// * Model(s)
Model *bunny;
Model *teddy;
// * Reference(s) to shader program(s)
GLuint program;
// * Texture(s)
GLuint texture;

unsigned int angle = 45;

void init(void)
{
	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.3,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	printError("GL inits"); // This is merely a vague indication of where something might be wrong

	projection = perspective(90, 1.0, 0.1, 1000);
	view = lookAt(0,0,1.5, 0,0,0, 0,1,0);

	// Load and compile shader
	program = loadShaders("lab0.vert", "lab0.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
	bunny = LoadModelPlus("objects/stanford-bunny.obj");
	teddy = LoadModelPlus("objects/teddy.obj");
	printError("load models");

	// Load textures
	LoadTGATextureSimple("textures/maskros512.tga",&texture);
	printError("load textures");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	bunny_model.m[0] = cos(angle+t*0.01);
	bunny_model.m[2] = sin(angle+t*0.01);
	bunny_model.m[8] = -sin(angle+t*0.01);
	bunny_model.m[10] = cos(angle+t*0.01);

	//activate the program, and set its variables
	glUseProgram(program);
	//the last argument has to be the same as the texture-unit that is to be used

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, bunny_model.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view.m);
	glUniform3f(glGetUniformLocation(program, "light_pos"), light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(glGetUniformLocation(program, "light_color"), light_color.x, light_color.y, light_color.z);
	glUniform3f(glGetUniformLocation(program, "object_color"), bunny_color.x, bunny_color.y, bunny_color.z);
	DrawModel(bunny, program, "pos", "norm", NULL);
	
	glUniform1i(glGetUniformLocation(program,"example_texture"),0);
	glActiveTexture(GL_TEXTURE0); //which texture-unit is active
	glBindTexture(GL_TEXTURE_2D, texture); //load the texture to active texture-unit
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, teddy_model.m);
	glUniform3f(glGetUniformLocation(program, "object_color"), teddy_color.x, teddy_color.y, teddy_color.z);
	DrawModel(teddy, program, "pos", "norm", "tex_coord");

	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("Lab 0 - OpenGL 3.2+ Introduction");
	glutDisplayFunc(display); 
	glutRepeatingTimer(20);
	init ();
	glutMainLoop();
	exit(0);
}

