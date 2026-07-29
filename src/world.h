#ifndef WORLD
#define WORLD



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

typedef struct {
    Plane planes[100];
    int planeCount;

    Billboard billboards[100];
    int billboardCount;
    
    Light lights[10];
    int lightCount;
    
    WorldCamera camera;
} World;


void renderWorld(World* world);

#endif