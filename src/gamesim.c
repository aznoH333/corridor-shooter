#include "gamesim.h"
#include "math.h"
#include "utils.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

void pushEntity(GameState* state, Entity* entity);



World convertToWorld(GameState* state) {
    World world = initializeEmptyWorld();
    


    // draw map
    {
        GameMap* map = &state->map;
        for (int i = 0; i < (int)ceil(map->length / map->width); ++i ) {
            // floor
            pushPlane(&world, (Plane) {
                .texture = "debug_textures_0002",
                .x = i * map->width + map->width * 0.5,
                .y = 0,
                .z = 0,
                .yaw = 0,
                .pitch = 0,
                .roll = 0,
                .width = map->width,
                .height = map->width
            });

            // ceiling
            pushPlane(&world, (Plane) {
                .texture = "debug_textures_0002",
                .x = i * map->width + map->width * 0.5,
                .y = map->ceilingHeight,
                .z = 0,
                .yaw = 0,
                .pitch = HALF_ROTATION,
                .roll = 0,
                .width = map->width,
                .height = map->width
            });

            // walls
            pushPlane(&world, (Plane) {
                .texture = "debug_textures_0005",
                .x = i * map->width + map->width * 0.5,
                .y = map->width / 2.0f,
                .z = map->width / 2.0f,
                .yaw = HALF_ROTATION,
                .pitch = -QUARTER_ROTATION,
                .roll = 0,
                .width = map->width,
                .height = map->width
            });

            pushPlane(&world, (Plane) {
                .texture = "debug_textures_0005",
                .x = i * map->width + map->width * 0.5,
                .y = map->width / 2.0f,
                .z = -map->width / 2.0f,
                .yaw = 0,
                .pitch = QUARTER_ROTATION,
                .roll = 0,
                .width = map->width,
                .height = map->width
            });
        }
        
        // draw backwall
        pushPlane(&world, (Plane) {
            .texture = "debug_textures_0005",
            .x = map->length,
            .y = map->width / 2.0f,
            .z = 0,
            .yaw = 0,
            .pitch = QUARTER_ROTATION,
            .roll = QUARTER_ROTATION,
            .width = map->width,
            .height = map->width
        });
    }
    
    // draw entities
    {
        for (int i = 0; i < state->entities.count; ++i) {
            Entity* entity = &state->entities.values[i];

            pushBillboard(&world, (Billboard){
                .texture = entity->texture,
                .x = entity->x,
                .y = entity->y,
                .z = entity->z,
                .size = entity->size
            });
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


    return nextFrame;
}



GameState initEmptyGame() {
    return (GameState) {
        .map = (GameMap) {
            .length = 30.0f,
            .width = 5.0f,
            .ceilingHeight = 5.0f
        },

        .camera = (GameCamera) {
            .distance = 0.0f
        },
        .entities = (GameEntities){
            .values = {0},
            .count = 0
        }
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

