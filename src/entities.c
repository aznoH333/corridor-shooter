#include "entities.h"
#include "raylib.h"
#include "utils.h"

typedef struct {
    Vector2 direction;
    float speed;
} PlayerData;

bool playerUpdate(Entity* this, GameState* state) {
    PlayerData* data = (PlayerData*) &this->data;


    // movement
    {
        float movementAcceleration = 0.1f;

        // acceleration
        if (IsKeyDown(KEY_A)) {
            if (data->direction.x > -1) {
                data->direction.x -= movementAcceleration;
            } else {
                data->direction.x = -1;
            }
        }

        if (IsKeyDown(KEY_D)) {
            if (data->direction.x < 1) {
                data->direction.x += movementAcceleration;
            } else {
                data->direction.x = 1;
            }
        }

        if (IsKeyDown(KEY_W)) {
            if (data->direction.y > 1) {
                data->direction.y += movementAcceleration;
            } else {
                data->direction.y = 1;
            }
        }

        if (IsKeyDown(KEY_S)) {
            if (data->direction.y < -1) {
                data->direction.y -= movementAcceleration;
            } else {
                data->direction.y = -1;
            }
        }

        // deceleration
        if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S) && data->direction.y != 0) {
            data->direction.y -= sign(data->direction.y) * movementAcceleration;

            if (fabs(data->direction.y) < movementAcceleration) {
                data->direction.y = 0;
            }
        }

        if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && data->direction.x != 0) {
            data->direction.x -= sign(data->direction.x) * movementAcceleration;

            if (fabs(data->direction.x) < movementAcceleration) {
                data->direction.x = 0;
            }
        }

        


        // wall collisions
        if (fabs(this->z + data->direction.x * data->speed) + this->width * 0.5 > state->map.width * 0.5) {
            data->direction.x = 0;
        }

        if (this->x + data->direction.y + this->width * 0.5 > state->map.length) {
            data->direction.y = 0;
        }

        // update value
        this->x += data->direction.y * data->speed;
        this->z += data->direction.x * data->speed;
    }


    // move camera
    {
        float playerCamValue = this->x - state->map.width * 0.66;
        
        if (playerCamValue > state->camera.distance) {
            state->camera.distance = playerCamValue;
        }
    }



    return true;
}


void player(GameState* state, float x, float y, float z){
    addEntity(state, (Entity) {
        .texture = "player_alt",
        .x = x,
        .y = y + 0.5f,
        .z = z,
        .width = 1.0f,
        .height = 1.0f,
        .textureWidth = 18.0f,
        .textureHeight = 24.0f,
        .update = &playerUpdate,
        .light = (EntityLight){
            .isLight = true,
            .radius = 20,
            .r = 1,
            .g = 1,
            .b = 1
        },
    }, &(PlayerData){
        .direction = (Vector2) {0, 0},
        .speed = 0.05f
    }, sizeof(PlayerData));
}



typedef struct {

} DummyData;

bool dummyUpdate(Entity* this, GameState* state){
    return true;
}

void dummy(GameCamera* state, float x, float y, float z){
    addEntity(state, (Entity){
        .texture = "debug_entities_0001",
        .x = x,
        .y = y + 0.5f,
        .z = z,
        .width = 1.0f,
        .height = 1.0f,
        .textureWidth = 32.0f,
        .textureHeight = 32.0f,
        .update = &dummyUpdate,
        .light = (EntityLight){
            .isLight = true,
            .radius = 5,
            .r = 1,
            .g = 0,
            .b = 0
        },
    }, &(DummyData){},
        sizeof(DummyData)
    );
}
