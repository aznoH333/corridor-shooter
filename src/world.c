#include "world.h"
#include "utils.h"

// main render function
void renderWorld(World* world){

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

    // apply lights
    // TODO this

    // apply camera
    setCamera(
        world->camera.x, 
        world->camera.y, 
        world->camera.z, 
        world->camera.rotationHorizontal, 
        world->camera.rotationVertical
    );


}

