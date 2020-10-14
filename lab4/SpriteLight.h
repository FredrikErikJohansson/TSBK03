// SpriteLight - Heavily simplified sprite engine
// by Ingemar Ragnemalm 2009

// What does a mogwai say when it sees a can of soda?
// Eeek! Sprite light! Sprite light!

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
#else
	#include <GL/gl.h>
	#include "MicroGlut.h"
#endif

#include "LoadTGA.h"
#include "VectorUtils3.h"

typedef struct FPoint
{
	GLfloat h, v;
} FPoint;

typedef struct SpriteRec
{
	vec3 position;
	TextureData *face;
	vec3 speed;
	GLfloat rotation;
	struct SpriteRec *next;

	//Custom
	vec3 speedDiff;
	vec3 averagePosition;
	vec3 avoidanceVector;
	
	// Add custom sprite data here as needed
} SpriteRec, *SpritePtr;

// Globals: The sprite list, background texture and viewport dimensions (virtual or real pixels)
extern SpritePtr gSpriteRoot;
extern GLuint backgroundTexID;
extern long gWidth, gHeight;

// Functions
TextureData *GetFace(char *fileName);
struct SpriteRec *NewSprite(TextureData *f, GLfloat h, GLfloat v, GLfloat hs, GLfloat vs);
void HandleSprite(SpritePtr sp);
void DrawSprite(SpritePtr sp);
void DrawBackground();

void InitSpriteLight();
