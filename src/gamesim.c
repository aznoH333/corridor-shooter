#include "gamesim.h"
#include "math.h"
#include "utils.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

void pushEntity(GameState* state, Entity* entity);


// when you multiply a number by this constant you convert from texture coordinates (pixels) to game coordinates
const float TEX_SIZE_TO_GAME = 0.03125f;


World convertToWorld(GameState* state) {
    World world = initializeEmptyWorld();
    


    // draw map
    {
        GameMap* map = &state->map;
        for (int i = 0; i < (int)ceil(map->length / map->width); ++i ) {
            // floor
            pushPlane(&world, (Plane) {
                .texture = "ground",
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
                .texture = "ground",
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
                .texture = "wall",
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
                .texture = "wall",
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
            .texture = "wall",
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
            pushPlane(
                &world,
                (Plane) {
                    .texture = entity->texture,
                    .x = entity->x,
                    .y = entity->y + entity->textureOffsetY,
                    .z = entity->z + entity->textureOffsetX,
                    .yaw = 0,
                    .pitch = QUARTER_ROTATION,
                    .roll = QUARTER_ROTATION,
                    // convert from texturesize to gamesize
                    .width = entity->textureWidth * TEX_SIZE_TO_GAME,
                    .height = entity->textureHeight * TEX_SIZE_TO_GAME,
                    .color = entity->color,
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

    
    



    world.camera.y = state->map.width * 0.33;
    world.camera.rotationVertical = -0.2f;
    world.camera.x = state->camera.distance;

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
            .distance = -9999.0f
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