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
TextureData *sheepFace, *blackFace, *dogFace, *foodFace;

// This can be made for the closest boid or for all within a distance. 
// This rule may be applied on a much smaller neighborhood than the others, 
// preferably with a falloff so it creates a stronger force the closer the other boid is.
vec3 CalcAvoidance(float maxDistanceSq, vec3 j){
	//float maxDistanceSq = 100;
	// For all nearby boids, make a vector pointing away 
	vec3 sub = ScalarMult(j, -1);
	vec3 avoidance = SetVector(sub.x, sub.y, 0.0f);
 	float distance = Norm(avoidance);
	//vec3 avoid_normalized = ScalarMult(avoidance, distance);
	
	return ScalarMult(avoidance, (1 - distance/maxDistanceSq));
}
void SpriteBehavior() // Din kod!
{
// L�gg till din labbkod h�r. Det g�r bra att �ndra var som helst i
// koden i �vrigt, men mycket kan samlas h�r. Du kan utg� fr�n den
// globala listroten, gSpriteRoot, f�r att kontrollera alla sprites
// hastigheter och positioner, eller arbeta fr�n egna globaler.
	float kMaxDistance = 200.f;
	SpritePtr i;
	SpritePtr j;
	for (i = gSpriteRoot; i != NULL; i = i->next) {
		float count = 0;
		i->speedDiff = SetVector(0, 0, 0);
		i->averagePosition = SetVector(0, 0, 0);
		i->avoidanceVector = SetVector(0, 0, 0);
		

		for (j = gSpriteRoot; j != NULL; j = j->next) {
			if(j != i) {
				float distance = Norm(VectorSub(i->position, j->position));

				if(j->face == dogFace) {
					float fleeing_distance = 300;
					if(distance < fleeing_distance) 
						i->avoidanceVector = ScalarMult(VectorAdd(i->avoidanceVector, CalcAvoidance(fleeing_distance, VectorSub(j->position, i->position))), 20);
					continue;
				}			

				if(distance < kMaxDistance) {
					//Calculate the average difference in speed between the current boid and all other boids. 
					//Add a fraction of this to the speed of the current boid.
					if(j->face != dogFace)
						i->speedDiff = VectorAdd(i->speedDiff, VectorSub(j->speed, i->speed)); // Alignment
					
					// Calculate the average position of all boids within a certain distance. 
					// Take this position minus the position of the current boid. 
					// Add a fraction of this vector to the speed of the current boid.
					if(i->face != dogFace)
						i->averagePosition = VectorAdd(i->averagePosition, VectorSub(j->position, i->position)); // Cohesion
					else if(distance < 200)
						i->averagePosition = VectorAdd(i->averagePosition, VectorSub(j->position, i->position)); // Cohesion
					
					// Separation/Avoidance
					float maxDistanceSq = 50;
					if(distance < maxDistanceSq)  
						i->avoidanceVector = VectorAdd(i->avoidanceVector, CalcAvoidance(maxDistanceSq, VectorSub(j->position, i->position)));

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

	// Second loop for adding the resulting contributions
	for (i = gSpriteRoot; i != NULL; i = i->next) {
		float kAlignmentWeight = 0.09f; // Hur mycket boidsen ska följa samma riktning
		float kCohesionWeight = 0.008f; // Hur mycket boidsen ska röra sig mot masscentrum
		float kAvoidanceWeight = 0.05f; // Hur mycket boidsen ska undvika varandra/en annorlunda boid
		float speed_increase = 1.0f;
		if(i->face == blackFace) {
			int max = 2;
			int min = 1;
			int speed = rand()%(max-min + 1) + min;
			 // Leader
			kAlignmentWeight = 0;
			kCohesionWeight = 0.003f;

			speed_increase = speed;
		}

		if(i->face == dogFace) {
			kAlignmentWeight = 0;
			kCohesionWeight = 0.002f;
			kAvoidanceWeight = 0;
			speed_increase = 4;
		}

		vec3 alignmentAmount = ScalarMult(i->speedDiff, kAlignmentWeight);
		vec3 cohesionAmount = ScalarMult(i->averagePosition, kCohesionWeight);
		vec3 avoidanceAmount = ScalarMult(i->avoidanceVector, kAvoidanceWeight);
		//vec3 total = VectorAdd(VectorAdd(avoidanceAmount, cohesionAmount), alignmentAmount); 
		i->speed = VectorAdd(i->speed, alignmentAmount);
		i->speed = VectorAdd(i->speed, cohesionAmount);
		i->speed = VectorAdd(i->speed, avoidanceAmount);
		i->speed = ScalarMult(i->speed, speed_increase/Norm(i->speed)); // Normalize här får de att fastna i kanter
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
	LoadTGATextureSimple("bilder/leaves.tga", &backgroundTexID); // Bakgrund
	
	sheepFace = GetFace("bilder/sheep.tga"); // Ett f�r
	blackFace = GetFace("bilder/blackie.tga"); // Ett svart f�r
	dogFace = GetFace("bilder/dog.tga"); // En hund
	foodFace = GetFace("bilder/mat.tga"); // Mat
	
	NewSprite(sheepFace, 100, 200, 1, 1);
	NewSprite(sheepFace, 200, 100, 1, 1);
	NewSprite(sheepFace, 250, 200, 1, 1);
	NewSprite(sheepFace, 50, 200, 1, -1);
	NewSprite(sheepFace, 100, 200, 1, -1);
	NewSprite(blackFace, 60, 200, 1, -1);
	NewSprite(blackFace, 20, 200, 1, -1);
	NewSprite(sheepFace, 400, 400, -1, 1);
	NewSprite(sheepFace, 420, 420, -1, 1);
	NewSprite(sheepFace, 430, 430, -1, 1);
	NewSprite(sheepFace, 440, 440, -1, -1);
	NewSprite(sheepFace, 450, 450, -1, -1);

	NewSprite(dogFace, 0, 0, 2, 2);

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
