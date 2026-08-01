#ifndef GAMESIM
#define GAMESIM

#include "world.h"
#include "stdbool.h"

typedef struct Entity Entity;
typedef struct GameState GameState;

#define ENTITY_DATA_SIZE 1024

struct Entity{
    // generic data
    char* texture;
    float x;
    float y;
    float z;
    float width;
    float height;
    float size;

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
};




World convertToWorld(GameState* state);
GameState createNextFrame(GameState* currentState);
GameState initEmptyGame();
void addEntity(GameState* state, Entity entity, void* data, int dataSize);




#endif