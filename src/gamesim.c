#include "gamesim.h"
#include "math.h"
#include "utils.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Entity texture initialization helpers#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EntityTexture simpleTexture(char* texture, float width, float height) {
    return (EntityTexture) {
        .texture = texture,
        .color = WHITE,
        .width = width,
        .height = height,
        .offset = (Vector3) {.x = 0, .y = 0, .z = 0},
        .yaw = 0,
        .pitch = 0,
        .roll = 0
    };
}


EntityTexture rotatedTexture(char* texture, float width, float height, float rotation) {
    EntityTexture t = simpleTexture(texture, width, height);

    t.pitch = rotation;

    return t;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Rendering logic#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void pushEntity(GameState* state, Entity* entity);


// when you multiply a number by this constant you convert from texture coordinates (pixels) to game coordinates
const float TEX_SIZE_TO_GAME = 0.03125f;


const char* groundTexture = "ground_0001";
const char* wallTexture = "wall_0003";


World convertToWorld(GameState* state) {
    World world = initializeEmptyWorld();
    


    // draw map
    {
        GameMap* map = &state->map;
        for (int i = 0; i < (int)ceil(map->length / map->width); ++i ) {
            // floor
            pushPlane(&world, (Plane) {
                .texture = groundTexture,
                .x = i * map->width + map->width * 0.5,
                .y = 0,
                .z = 0,
                .yaw = 0,
                .pitch = 0,
                .roll = 0,
                .width = map->width,
                .height = map->width,
                .color = WHITE,
            });

            // ceiling
            pushPlane(&world, (Plane) {
                .texture = groundTexture,
                .x = i * map->width + map->width * 0.5,
                .y = map->ceilingHeight,
                .z = 0,
                .yaw = 0,
                .pitch = HALF_ROTATION,
                .roll = 0,
                .width = map->width,
                .height = map->width,
                .color = WHITE,
            });

            // walls
            pushPlane(&world, (Plane) {
                .texture = wallTexture,
                .x = i * map->width + map->width * 0.5,
                .y = map->width / 2.0f,
                .z = map->width / 2.0f,
                .yaw = HALF_ROTATION,
                .pitch = -QUARTER_ROTATION,
                .roll = 0,
                .width = map->width,
                .height = map->width,
                .color = WHITE,

            });

            pushPlane(&world, (Plane) {
                .texture = wallTexture,
                .x = i * map->width + map->width * 0.5,
                .y = map->width / 2.0f,
                .z = -map->width / 2.0f,
                .yaw = 0,
                .pitch = QUARTER_ROTATION,
                .roll = 0,
                .width = map->width,
                .height = map->width,
                .color = WHITE,
            });
        }
        
        // draw backwall
        pushPlane(&world, (Plane) {
            .texture = wallTexture,
            .x = map->length,
            .y = map->width / 2.0f,
            .z = 0,
            .yaw = 0,
            .pitch = QUARTER_ROTATION,
            .roll = QUARTER_ROTATION,
            .width = map->width,
            .height = map->width,
            .color = WHITE,
        });
    }
    
    // draw entities
    {
        for (int i = 0; i < state->entities.count; ++i) {
            Entity* entity = &state->entities.values[i];
            EntityTexture texture = entity->texture;
            pushPlane(
                &world,
                (Plane) {
                    .texture = texture.texture,
                    .x = entity->x + texture.offset.x,
                    .y = entity->y + texture.offset.y,
                    .z = entity->z + texture.offset.z,
                    .yaw = texture.yaw,
                    .pitch = QUARTER_ROTATION + texture.pitch,
                    .roll = QUARTER_ROTATION + texture.roll,
                    // convert from texturesize to gamesize
                    .width = texture.width * TEX_SIZE_TO_GAME,
                    .height = texture.height * TEX_SIZE_TO_GAME,
                    .color = texture.color,
                }
            );


            // light
            if (entity->light.isLight) {
                pushLight(
                    &world,
                    (Light) {
                        .x = entity->x,
                        .y = entity->y,
                        .z = entity->z,
                        .radius = entity->light.radius,
                        .r = entity->light.r,
                        .g = entity->light.g,
                        .b = entity->light.b
                    }
                );
            }
        }
    }

    // additional planes
    {
        for (int i = 0; i < state->additionalPlanes.count; ++i) {
            pushPlane(
                &world,
                state->additionalPlanes.values[i]
            );
        }
    }

    
    

    // camera 
    {
    
        // screen shake
        float screenShakeValue = sqrt((float) state->camera.screenShake) * 0.02;
        float screenShakeXOffset = cos(state->internalTimer * 0.5) * screenShakeValue;
        float screenShakeYOffset = sin(state->internalTimer * 0.5) * screenShakeValue;

        // set position
        world.camera.y = state->map.width * 0.33 + screenShakeYOffset;
        world.camera.rotationVertical = -0.2f;
        world.camera.x = state->camera.distance;
        world.camera.z = screenShakeXOffset;
    }

    

    return world;
}


GameState createNextFrame(GameState* currentState) {
    GameState nextFrame = initEmptyGame();
    
    // todo update here
    nextFrame.map = currentState->map;


    

    { // update entities
        for (int i = 0; i < currentState->entities.count; ++i) {
            Entity* entity = &currentState->entities.values[i];
            bool result = entity->update(entity, currentState);

            if (result) {
                pushEntity(&nextFrame, entity);
            }
        }
    }

    { // update camera
        nextFrame.camera = currentState->camera;
        nextFrame.camera.screenShake = max(nextFrame.camera.screenShake * 0.5 , 0);
    }

    { // update time
        nextFrame.internalTimer = currentState->internalTimer + 1;
    }


    return nextFrame;
}



GameState initEmptyGame() {
    return (GameState) {
        .map = (GameMap) {
            .length = 90.0f,
            .width = 7.5f,
            .ceilingHeight = 4.5f
        },

        .camera = (GameCamera) {
            .distance = -9999.0f,
            .screenShake = 0
        },
        .entities = (GameEntities){
            .values = {0},
            .count = 0
        },
        .additionalPlanes = (AdditionalPlanes) {
            .values = {0},
            .count = 0
        },
        .internalTimer = 0
    };
}



void pushEntity(GameState* state, Entity* entity) {
    if (state->entities.count >= MAX_ENTITIES) {
        return;
    }

    state->entities.values[state->entities.count] = *entity;
    state->entities.count++;
}

void addEntity(GameState* state, Entity entity, void* data, int dataSize){
    Entity local = entity;

    if (dataSize > ENTITY_DATA_SIZE) {
        printf("couldn't fit entity data :(\n)");
        return;
    }
    memcpy(entity.data, data, dataSize);
    pushEntity(state, &entity);

}


void addPlane(GameState* state, Plane plane) {
    if (state->additionalPlanes.count >= MAX_ADDITIONAL_PLANES) {
        return;
    }

    state->additionalPlanes.values[state->additionalPlanes.count] = plane;
    state->additionalPlanes.count++;
}


void addEntityPlane(
    GameState* state, 
    Vector3 position, 
    char* texture, 
    float textureWidth, 
    float textureHeight, 
    Color color) {
        addPlane(
            state,
            (Plane) {
                .texture = texture,
                .x = position.x,
                .y = position.y,
                .z = position.z,
                .yaw = 0,
                .pitch = QUARTER_ROTATION,
                .roll = QUARTER_ROTATION,
                // convert from texturesize to gamesize
                .width = textureWidth * TEX_SIZE_TO_GAME,
                .height = textureHeight * TEX_SIZE_TO_GAME,
                .color = color,
            }
        );
}


EntityLight emptyLight(){
    return (EntityLight) {
        .isLight = false,
        .radius = 0,
        .r = 0,
        .g = 0,
        .b = 0
    };
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Camera manipulation#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void addScreenShake(GameState* state, int ammount) {
    state->camera.screenShake = min(state->camera.screenShake + ammount, 30);
}
