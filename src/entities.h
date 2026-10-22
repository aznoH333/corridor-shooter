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
    int trailParticleCount;
    float trailParticleDistance;
    int fadeInTime;
    int fadeOutTime;
    bool collideWithTerrain;
    float deceleration;
} ProjectileInitData;


void initNewPlayerInstance(GameState* state); // creates a new player. stores him in the state memmory
void player(GameState* state, float x, float y, float z); // places the player from memmory in the world
void dummy(GameCamera* state, float x, float y, float z);
void projectile(GameState* state, Vector3 position, Vector3 direction, ProjectileInitData data);


#endif