#ifndef PARTICLES
#define PARTICLES

#include "raylib.h"

void blood(GameState* state, Vector3 position, Vector3 direction, float speed);
void bloodSplash(GameState* state, Vector3 origin, float amount);
void fadeParticle(GameState* state, Vector3 position, char* texture, float textureWidth, float textureHeight, float textureRotation, int fadeTime, EntityLight light);
void muzzleFlash(GameState* state, Vector3 position);
void smokePuff(GameState* state, Vector3 position);
void bloodPuff(GameState* state, Vector3 position);
void bulletCasing(GameState* state, Vector3 position, Vector3 baseDirection);

#endif