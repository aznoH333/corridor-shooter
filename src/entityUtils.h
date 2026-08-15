#ifndef ENTITY_UTILS
#define ENTITY_UTILS

#include "gamesim.h"


Vector3 checkWorldCollision(float x, float y, float z, float width, float height, GameMap* map);
float approachNumber(float value, float target, float step);
BoundingBox getEntityBoundingBox(Entity* entity);
Entity* getCollidingEntityByType(GameState* state, Entity* source, EntityType type);
Entity* findEntityByType(GameState* state, Entity* source, EntityType type);
Vector3 getMouseHit(GameState* state);


#endif
