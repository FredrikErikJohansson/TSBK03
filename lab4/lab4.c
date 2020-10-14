// Demo of heavily simplified sprite engine
// by Ingemar Ragnemalm 2009
// used as base for lab 4 in TSBK03.
// OpenGL 3 conversion 2013.

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#include <GL/gl.h>
	#include "MicroGlut.h"
#endif

#include <stdlib.h>
#include "LoadTGA.h"
#include "SpriteLight.h"
#include "GL_utilities.h"
//#include "common/VectorUtils3.h"

// L�gg till egna globaler h�r efter behov.

// This can be made for the closest boid or for all within a distance. 
// This rule may be applied on a much smaller neighborhood than the others, 
// preferably with a falloff so it creates a stronger force the closer the other boid is.
vec3 CalcAvoidance(SpritePtr i, SpritePtr j){
	float maxDistanceSq = 100;
	// For all nearby boids, make a vector pointing away 
	vec3 avoidance = SetVector(i->position.x - j->position.x, i->position.y - j->position.y, 0.0f);


  float distance = Norm(avoidance);

	
	return avoidance;
}
void SpriteBehavior() // Din kod!
{
// L�gg till din labbkod h�r. Det g�r bra att �ndra var som helst i
// koden i �vrigt, men mycket kan samlas h�r. Du kan utg� fr�n den
// globala listroten, gSpriteRoot, f�r att kontrollera alla sprites
// hastigheter och positioner, eller arbeta fr�n egna globaler.
	float kMaxDistance = 100.f;
	SpritePtr sp = gSpriteRoot;
	for (SpritePtr i = sp; i != NULL; i = i->next) {
		float count = 0;
		i->speedDiff = SetVector(0, 0, 0);
		i->averagePosition = SetVector(0, 0, 0);
		i->avoidanceVector = SetVector(0, 0, 0);
		

		for (SpritePtr j = sp; j != NULL; j = j->next) {
			if(j != i) {
				float distance = Norm(VectorSub(i->position, j->position));
				if(distance < kMaxDistance) {
					//Calculate the average difference in speed between the current boid and all other boids. 
					//Add a fraction of this to the speed of the current boid.
					i->speedDiff = VectorAdd(i->speedDiff, VectorSub(j->speed, i->speed)); // Alignment
					
					// Calculate the average position of all boids within a certain distance. 
					// Take this position minus the position of the current boid. 
					// Add a fraction of this vector to the speed of the current boid.
					i->averagePosition = VectorAdd(i->averagePosition, j->position); // Cohesion
					
					// Separation/Avoidance 
					vec3 avoidVec = CalcAvoidance(i, j); // Avoidance
					//i->avoidanceVector = VectorAdd(i->avoidanceVector, avoidVec); // funkar ej

					count++;
				}
			}
		}
		if (count > 0){
			// Divisions
			i->speedDiff = ScalarMult(i->speedDiff, 1.0/count); // Alignment
			i->averagePosition = ScalarMult(i->averagePosition, 1.0/count); // Cohesion
			i->avoidanceVector = ScalarMult(i->avoidanceVector, 1.0/count); // Avoidance
		}
	}
	float kAlignmentWeight = 0.004f;
	float kCohesionWeight = 0.002f;
	float kAvoidanceWeight = 0.07f;

	// Second loop for adding the resulting contributions
	for (SpritePtr i = sp; i != NULL; i = i->next) {
		vec3 alignmentAmount = ScalarMult(i->speedDiff, kAlignmentWeight);
		vec3 cohesionAmount = ScalarMult(i->averagePosition, kCohesionWeight);
		vec3 avoidanceAmount = ScalarMult(i->avoidanceVector, kAvoidanceWeight);
		vec3 total = VectorAdd(VectorAdd(avoidanceAmount, cohesionAmount), alignmentAmount); 
		vec3 newSpeed = VectorAdd(i->speed, total); // Normalize här får de att fastna i kanter
		i->speed = newSpeed;
		i->position = VectorAdd(i->position, i->speed);

	}

   
	
}

// Drawing routine
void Display()
{
	SpritePtr sp;
	
	glClearColor(0, 0, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	DrawBackground();
	
	SpriteBehavior(); // Din kod!
	
// Loop though all sprites. (Several loops in real engine.)
	sp = gSpriteRoot;
	do
	{
		HandleSprite(sp); // Callback in a real engine
		DrawSprite(sp);
		sp = sp->next;
	} while (sp != NULL);
	
	glutSwapBuffers();
}

void Reshape(int h, int v)
{
	glViewport(0, 0, h, v);
	gWidth = h;
	gHeight = v;
}

void Timer(int value)
{
	glutTimerFunc(20, Timer, 0);
	glutPostRedisplay();
}

// Example of user controllable parameter
float someValue = 0.0;

void Key(unsigned char key,
         __attribute__((unused)) int x,
         __attribute__((unused)) int y)
{
  switch (key)
  {
    case '+':
    	someValue += 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case '-':
    	someValue -= 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case 0x1b:
      exit(0);
  }
}

void Init()
{
	TextureData *sheepFace, *blackFace, *dogFace, *foodFace;
	
	LoadTGATextureSimple("bilder/leaves.tga", &backgroundTexID); // Bakgrund
	
	sheepFace = GetFace("bilder/sheep.tga"); // Ett f�r
	blackFace = GetFace("bilder/blackie.tga"); // Ett svart f�r
	dogFace = GetFace("bilder/dog.tga"); // En hund
	foodFace = GetFace("bilder/mat.tga"); // Mat
	
	NewSprite(sheepFace, 100, 200, 1, 1);
	NewSprite(sheepFace, 200, 100, 1.5, -1);
	NewSprite(sheepFace, 250, 200, -1, 1.5);
	NewSprite(sheepFace, 50, 200, -1, 1.5);
	NewSprite(sheepFace, 100, 200, -2, 1.5);
	NewSprite(blackFace, 50, 200, -2, 1.5);

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 2);
	glutCreateWindow("SpriteLight demo / Flocking");
	
	glutDisplayFunc(Display);
	glutTimerFunc(20, Timer, 0); // Should match the screen synch
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	
	InitSpriteLight();
	Init();
	
	glutMainLoop();
	return 0;
}
