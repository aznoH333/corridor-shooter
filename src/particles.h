#ifndef PARTICLES
#define PARTICLES

#include "raylib.h"

void blood(GameState* state, Vector3 position, Vector3 direction, float speed);
void bloodSplash(GameState* state, Vector3 origin, float amount);
void fadeParticle(GameState* state, Vector3 position, char* texture, float textureWidth, float textureHeight, int fadeTime);

#endif