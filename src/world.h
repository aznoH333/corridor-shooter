#ifndef WORLD
#define WORLD

#include "raylib.h"

typedef struct {
    char* texture;
    float x;
    float y;
    float z;
    float size;
} Billboard;


typedef struct {
    char* texture;
    float x;
    float y;
    float z;
    float yaw;
    float pitch;
    float roll;
    float width;
    float height;
    Color color;
} Plane;

typedef struct {
    float x;
    float y;
    float z;
    float rotationHorizontal;
    float rotationVertical;
} WorldCamera;

typedef struct {
    float x;
    float y;
    float z;
    float radius;
    float r;
    float g;
    float b;
} Light;


#define MAX_PLANES 100
#define MAX_BILLBOARDS 100
#define MAX_LIGHTS 10

typedef struct {
    Plane planes[MAX_PLANES];
    int planeCount;

    Billboard billboards[MAX_BILLBOARDS];
    int billboardCount;
    
    Light lights[MAX_LIGHTS];
    int lightCount;
    
    WorldCamera camera;
} World;

typedef struct {
    Shader* lightShader;
    int usedLightsLoc;
    int lightsLoc;
} WorldRenderingData;


WorldRenderingData prepareWorldRenderingData();
void renderWorld(World* world, WorldRenderingData* renderingData);
World initializeEmptyWorld();


void pushPlane(World* world, Plane plane);
void pushBillboard(World* world, Billboard billboard);
void pushLight(World* world, Light light);


#endif