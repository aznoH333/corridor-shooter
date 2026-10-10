#ifndef ENTITIES
#define ENTITIES 

#include "gamesim.h"


typedef struct {
    float velocity;
    float maxDistanceTraveled;
    float lightRadius;
    float damage;
    EntityTexture texture;
    EntityType targetType;
    int fadeParticleCount;
    float fadeParticleDistance;
} ProjectileInitData;


void player(GameState* state, float x, float y, float z);
void dummy(GameCamera* state, float x, float y, float z);
void projectile(GameState* state, Vector3 position, Vector3 direction, ProjectileInitData data);


#endif