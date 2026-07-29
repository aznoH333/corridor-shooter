#include "world.h"
#include "utils.h"

// main render function
void renderWorld(World* world){

    // apply camera
    setCamera(
        world->camera.x, 
        world->camera.y, 
        world->camera.z, 
        world->camera.rotationHorizontal, 
        world->camera.rotationVertical
    );

    // apply lights
    // TODO this

    // render planes
    for (int i = 0; i < world->planeCount; i++) {
        Plane* planeInstance = &world->planes[i];
        plane(
            planeInstance->texture, 
            planeInstance->x, 
            planeInstance->y, 
            planeInstance->z, 
            planeInstance->width, 
            planeInstance->height, 
            planeInstance->yaw, 
            planeInstance->pitch, 
            planeInstance->roll
        );
    }

    // render billboards
    for (int i = 0; i < world->billboardCount; i++) {
        Billboard* billboardInstance = &world->billboards[i];
        billboard(
            billboardInstance->texture, 
            billboardInstance->x, 
            billboardInstance->y, 
            billboardInstance->z, 
            billboardInstance->size,
            WHITE // TODO apply billboard lighting
        );

    }

    

    


}



World initializeEmptyWorld() {
    return (World) {
        .planes = {0},
        .planeCount = 0,
        .billboards = {0},
        .billboardCount = 0,
        .lights = {0},
        .lightCount = 0,
        .camera = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f,
            .rotationHorizontal = 0.0f,
            .rotationVertical = 0.0f
        }
    };
}

// manipulation functions
void pushPlane(World* world, Plane plane) {
    if (world->planeCount >= MAX_PLANES) {
        return;
    }

    world->planes[world->planeCount] = plane;
    world->planeCount++;
}

void pushBillboard(World* world, Billboard billboard) {
    if (world->billboardCount >= MAX_BILLBOARDS) {
        return;
    }

    world->billboards[world->billboardCount] = billboard;
    world->billboardCount++;
}

void pushLight(World* world, Light light) {
    if (world->lightCount >= MAX_LIGHTS) {
        return;
    }

    world->lights[world->lightCount] = light;
    world->lightCount++;
}