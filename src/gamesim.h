#ifndef GAMESIM
#define GAMESIM

#include "world.h"
#include "stdbool.h"

typedef struct Entity Entity;
typedef struct GameState GameState;

#define ENTITY_DATA_SIZE 1024

typedef struct {
    bool isLight;
    float radius;
    float r;
    float g;
    float b;
} EntityLight;

typedef enum {
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_BULLET,
    ENTITY_UNSET,
} EntityType;

struct Entity{
    // generic data
    char* texture;
    float x;
    float y;
    float z;
    float width;
    float height;
    float textureWidth;
    float textureHeight;
    float textureOffsetX;
    float textureOffsetY;
    Color color;
    EntityLight light;
    EntityType type;

    // entity specific
    char data[ENTITY_DATA_SIZE];
    bool(*update)(Entity*, GameState*);
};



typedef struct {
    float length;
    float width;
    float ceilingHeight;
} GameMap;

typedef struct {
    float distance;
} GameCamera;

#define MAX_ENTITIES 100
typedef struct {
    Entity values[MAX_ENTITIES];
    int count;
} GameEntities;


struct GameState {
    GameMap map;
    GameCamera camera;
    GameEntities entities;
    int internalTimer;
};




World convertToWorld(GameState* state);
GameState createNextFrame(GameState* currentState);
GameState initEmptyGame();
EntityLight emptyLight();
void addEntity(GameState* state, Entity entity, void* data, int dataSize);




#endif