#include "gamesim.h"
#include "math.h"
#include "utils.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "ui.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#UI state and meta game state functions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define UI_STATES_COUNT 3
static UIState uiStates[UI_STATES_COUNT] = {
    {
        .name = GAME,
        .allowEntityUpdates = true,
        .drawMouseCursor = false,
    },
    {
        .name = GAME_PAUSED,
        .allowEntityUpdates = false,
        .drawMouseCursor = true,
    },
    {
        .name = MAIN_MENU,
        .allowEntityUpdates = true,
        .drawMouseCursor = true,
    }
};


void switchGameState(GameState* state, UIStateName newState) {
    for (int i = 0; i < UI_STATES_COUNT; ++i) {
        if (uiStates[i].name == newState) {
            state->metaState.state = uiStates[i];
        }
    }
}

void updatePlayerReference(GameState* state, Entity* player) {
    state->metaState.playerEntity = player;
}

Entity* getPlayerReference(GameState* state) {
    return state->metaState.playerEntity;
}



void pauseGame() {

}

// meta upadte handling
static char ammoCounter[8] = "xxx/xxx";
static char stimCounter[3] = "xx";




void updateMetaState(GameState* state) { // used to draw ui elements
    // draw mouse
    if (state->metaState.state.drawMouseCursor) {
        Vector2 mousePos = getMousePosition();
        
        
        sprMain("icons_0001", mousePos.x, mousePos.y, 4, 4, false, false, 0, WHITE, 7);

    }
    
    
    // do update
    switch (state->metaState.state.name) {

        case MAIN_MENU: {
            drawText("Main menu placeholder", 850, 600, 60, WHITE);

            if (boolButton(860, 700, 200, 70, "play")) {
                switchGameState(state, GAME);
            }
            
            if (boolButton(860, 820, 200, 70, "exit")) {
                CloseWindow(); // quit game
            }



            break;
        }

        case GAME_PAUSED: {
            drawText("Paused", 850, 600, 60, WHITE);

            if (boolButton(860, 700, 200, 70, "resume")) {
                switchGameState(state, GAME);
            }
            
            if (boolButton(860, 820, 200, 70, "quit")) {
                switchGameState(state, MAIN_MENU);
            }

            break;
        }

        case GAME: { 
            // draw player hud
            UIValues* values = &state->metaState.uiValues;

            // draw ammo
            insertNumberInString(&ammoCounter, 0, 2, values->playerAmmo); // worlds shittiest sprintf
            insertNumberInString(&ammoCounter, 4, 6, values->playerAmmoMax); 
            
            drawText(ammoCounter, 1560, 900, 50, WHITE);
            bar(1360, 950, 400, values->playerAmmo, values->playerAmmoMax);
            sprMain("icons_0003", 1800, 965, 4, 4, false, false, 0, WHITE, 0);
            

            // draw health
            sprMain("icons_0001", 130, 965, 4, 4, false, false, 0, WHITE, 0);
            bar(160, 950, 400, values->playerHealth, values->playerHealthMax);


            // draw stims
            sprMain("icons_0004", 140, 912, 5, 5, false, false, 0, WHITE, 0);
            insertNumberInString(stimCounter, 1, 1, values->playerStimCount);
            drawText(stimCounter, 190, 890, 50, WHITE);
            


            // reload bar
            if (values->playerReloadPercentage < 1) {
                bar(810, 950, 300, values->playerReloadPercentage, 1.0);
            }



            // pausing the game
            if (IsKeyPressed(KEY_O)) {
                switchGameState(state, GAME_PAUSED);
            }
            
            break;
        }


    }

}



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
        .pitch = QUARTER_ROTATION,
        .roll = QUARTER_ROTATION,
        .enableDepthMask = true,
        .enabled = true,
    };
}



EntityTexture noTexture() {
    return (EntityTexture) {
        .enabled = false
    };
}

EntityTexture simpleColoredTexture(char* texture, float width, float height, Color color) {
    EntityTexture t = simpleTexture(texture, width, height);

    t.color = color;

    return t;
}



EntityTexture rotatedTexture(char* texture, float width, float height, float rotation) {
    EntityTexture t = simpleTexture(texture, width, height);

    t.pitch = rotation;

    return t;
}

EntityTexture rotatedTextureFull(char* texture, float width, float height, float yaw, float pitch, float roll) {
    EntityTexture t = simpleTexture(texture, width, height);

    t.yaw = yaw;
    t.pitch = pitch;
    t.roll = roll;

    return t;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Rendering logic#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pushEntity(GameState* state, Entity* entity);




const char* groundTexture = "metal_tiled_3";
const char* ceilingTexture = "concrete_tiled_10";
const char* wallTexture = "brick_test_2_0003";


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
                .enableDepthMask = true,
            });

            // ceiling
            pushPlane(&world, (Plane) {
                .texture = ceilingTexture,
                .x = i * map->width + map->width * 0.5,
                .y = map->ceilingHeight,
                .z = 0,
                .yaw = 0,
                .pitch = HALF_ROTATION,
                .roll = 0,
                .width = map->width,
                .height = map->width,
                .color = WHITE,
                .enableDepthMask = true,
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
                .enableDepthMask = true,
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
                .enableDepthMask = true,
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
            .enableDepthMask = true,
        });
    }
    
    // draw entities
    {
        for (int i = 0; i < state->entities.count; ++i) {
            Entity* entity = &state->entities.values[i];
            EntityTexture texture = entity->texture;

            if (texture.enabled == false) {
                continue;
            }


            float yaw = texture.yaw;
            float pitch = texture.pitch;
            float roll = texture.roll;


            pushPlane(
                &world,
                (Plane) {
                    .texture = texture.texture,
                    .x = entity->x + texture.offset.x,
                    .y = entity->y + texture.offset.y,
                    .z = entity->z + texture.offset.z,
                    .yaw = yaw,
                    .pitch = pitch,
                    .roll = roll,
                    // convert from texturesize to gamesize
                    .width = texture.width * TEX_SIZE_TO_GAME,
                    .height = texture.height * TEX_SIZE_TO_GAME,
                    .color = texture.color,
                    .enableDepthMask = texture.enableDepthMask,
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#World updating#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameState* createNextFrame(GameState* currentState) {
    GameState* nextFrame = initEmptyGame();
    

    // todo update here
    nextFrame->map = currentState->map;

    { // update entities
        
        if (currentState->metaState.state.allowEntityUpdates) {
            bool updateResults[MAX_ENTITIES] = {0};
        
        
            for (int i = 0; i < currentState->entities.count; ++i) {
                Entity* entity = &currentState->entities.values[i];
                
                // we have to cull entities in a later loop otherwise weird update order related bugs happen when an entity attempts to modify an older entity
                bool result = entity->update(entity, currentState);


                updateResults[i] = result;
                
            }

            for (int i = 0; i < currentState->entities.count; ++i) {
                if (updateResults[i]) {
                    Entity* entity = &currentState->entities.values[i];
                    pushEntity(nextFrame, entity);
                }
            }
        } else {
            // skip update checking logic if entities cant update
            for (int i = 0; i < currentState->entities.count; ++i) {    
                Entity* entity = &currentState->entities.values[i];
                pushEntity(nextFrame, entity);
            }
            
        }
        
        
    }

    { // update camera
        nextFrame->camera = currentState->camera;
        nextFrame->camera.screenShake = max(nextFrame->camera.screenShake * 0.5 , 0);
    }

    { // update time
        nextFrame->internalTimer = currentState->internalTimer + 1;
    }

    { // update ui
        nextFrame->metaState = currentState->metaState;
        updateMetaState(nextFrame);

    }


    return nextFrame;
}



GameState* initEmptyGame() {
    // init
    GameState* output = malloc(sizeof(GameState));
    
    output->map = (GameMap) {
        .length = 90.0f,
        .width = 7.5f,
        .ceilingHeight = 4.5f
    };

    output->camera = (GameCamera) {
        .distance = -9999.0f,
        .screenShake = 0
    };
    output->entities = (GameEntities){
        .values = {0},
        .count = 0
    };
    output->additionalPlanes = (AdditionalPlanes) {
        .values = {0},
        .count = 0
    };
    output->metaState = (GameMetaState) {
        .state = uiStates[0], // game state
        .playerEntity = NULL,
        .uiValues = {0},
    };
    output->internalTimer = 0;
    
    // return
    return output;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#World manipulation#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    Color color,
    float yaw,
    float pitch,
    float roll
    
) {
        addPlane(
            state,
            (Plane) {
                .texture = texture,
                .x = position.x,
                .y = position.y,
                .z = position.z,
                .yaw = yaw,
                .pitch = pitch,
                .roll = roll,
                // convert from texturesize to gamesize
                .width = textureWidth * TEX_SIZE_TO_GAME,
                .height = textureHeight * TEX_SIZE_TO_GAME,
                .color = color,
                .enableDepthMask = true
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
