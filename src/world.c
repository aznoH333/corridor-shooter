#include "world.h"
#include "utils.h"
#include "raylib.h"

// setup data
WorldRenderingData prepareWorldRenderingData() {
    Shader* shader = Get3DShader();
    int usedLightsLoc = GetShaderLocation(*shader, "usedLights");
    int lightsLoc = GetShaderLocation(*shader, "lights[0].position");
   

    return (WorldRenderingData) {
        .lightShader = shader,
        .usedLightsLoc = usedLightsLoc,
        .lightsLoc = lightsLoc
    };
}


// main render function
void renderWorld(World* world, WorldRenderingData* renderingData){

    // apply camera
    setCamera(
        world->camera.x, 
        world->camera.y, 
        world->camera.z, 
        world->camera.rotationHorizontal, 
        world->camera.rotationVertical
    );

    // apply lights
    {
	    SetShaderValue(*renderingData->lightShader,  renderingData->usedLightsLoc, &world->lightCount, SHADER_UNIFORM_INT);


        for (int i = 0; i < world->lightCount;++i) {
            int startingIndex = renderingData->lightsLoc + i * 3;
            
            Light* light = &world->lights[i];
            
            float position[3] = {light->x, light->y, light->z};
            float color[3] = {light->r, light->g, light->b};
            float radius = light->radius;
            
            SetShaderValue(*renderingData->lightShader, startingIndex, &position, SHADER_UNIFORM_VEC3);
            SetShaderValue(*renderingData->lightShader, startingIndex + 1, &radius, SHADER_UNIFORM_FLOAT);
            SetShaderValue(*renderingData->lightShader, startingIndex + 2, &color, SHADER_UNIFORM_VEC3);
        }

    }

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