#ifndef GAMESIM
#define GAMESIM

#include "world.h"
#include "stdbool.h"

typedef struct Entity Entity;
typedef struct GameState GameState;

#define ENTITY_DATA_SIZE 1024




typedef struct {
    char* texture;
    Color color;
    float width;
    float height;
    Vector3 offset;
    float yaw;
    float pitch;
    float roll;
    bool enableDepthMask;
} EntityTexture;

EntityTexture simpleTexture(char* texture, float width, float height);
EntityTexture rotatedTexture(char* texture, float width, float height, float rotation);
EntityTexture rotatedTextureFull(char* texture, float width, float height, float yaw, float pitch, float roll);


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
    EntityTexture texture;
    float x;
    float y;
    float z;
    float width;
    float height;
    
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
    int screenShake;
} GameCamera;

#define MAX_ENTITIES 1000
typedef struct {
    Entity values[MAX_ENTITIES];
    int count;
} GameEntities;


#define MAX_ADDITIONAL_PLANES 100
typedef struct {
    Plane values[MAX_ADDITIONAL_PLANES];
    int count;
} AdditionalPlanes;


struct GameState {
    GameMap map;
    GameCamera camera;
    GameEntities entities;
    AdditionalPlanes additionalPlanes;
    int internalTimer;
};




World convertToWorld(GameState* state);
GameState createNextFrame(GameState* currentState);
GameState initEmptyGame();
EntityLight emptyLight();
// world manipulation functions
void addEntity(GameState* state, Entity entity, void* data, int dataSize);
void addPlane(GameState* state, Plane plane);
void addEntityPlane(GameState* state, Vector3 position, char* texture, float textureWidth, float textureHeight, Color color);

// camera functions
void addScreenShake(GameState* state, int ammount);


#endif