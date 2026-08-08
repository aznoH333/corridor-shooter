#include "entities.h"
#include "raylib.h"
#include "utils.h"

static Vector2 checkWorldCollision(float x, float y, float z, float width, GameMap* map) {
    (void)y;

    Vector2 collisionDirection = (Vector2){0};
    float halfWidth = width * 0.5f;
    float halfMapWidth = map->width * 0.5f;
    float mapLength = map->length;

    if (x - halfWidth < -10) {
        collisionDirection.x = -1;
    } else if (x + halfWidth > map->length) {
        collisionDirection.x = 1;
    }

    if (z - halfWidth < -halfMapWidth) {
        collisionDirection.y = -1;
    } else if (z + halfWidth > halfMapWidth) {
        collisionDirection.y = 1;
    }

    return collisionDirection;
}

typedef struct {
    Vector2 velocity;
    float speed;
} PlayerData;

static float approachNumber(float value, float target, float amount) {
    if (value < target) {
        return min(value + amount, target);
    }

    if (value > target) {
        return max(value - amount, target);
    }

    return value;
}

static Vector2 getPlayerInput(void) {
    Vector2 input = (Vector2){
        .x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        .y = IsKeyDown(KEY_W) - IsKeyDown(KEY_S),
    };

    if (input.x != 0 || input.y != 0) {
        input = Vector2Normalize(input);
    }

    return input;
}

bool playerUpdate(Entity* this, GameState* state) {
    PlayerData* data = (PlayerData*) &this->data;

    // movement
    {
        float movementAcceleration = 0.1f;
        Vector2 input = getPlayerInput();
        float sprintVelocity = 0.65f + ((float)IsKeyDown(KEY_LEFT_SHIFT) * 0.35f);

        Vector2 next = {
            .x = this->x + data->velocity.y * data->speed,
            .y = this->z + data->velocity.x * data->speed
        };

        float velocityLength = Vector2Length(data->velocity);
        if (velocityLength > 0.75f) {
            movementAcceleration = 0.025f;//movementAcceleration / max(totalSpeed - data->speed * 0.5f, 0.1f);
        }


        data->velocity.x = approachNumber(data->velocity.x, input.x * sprintVelocity, movementAcceleration);
        data->velocity.y = approachNumber(data->velocity.y, input.y * sprintVelocity, movementAcceleration);

        Vector2 collisionDirection = checkWorldCollision(next.x, this->y, next.y, this->width, &state->map);

        // wall collisions
        if (collisionDirection.y != 0) {
            if (fabs(data->velocity.x) > 0.75f) { // add a little bounce when the player collides with a wall above certain speeds
                data->velocity.x *= -0.45f;
            }else {
                data->velocity.x = 0;
            }
        } else {
            this->z = next.y;
        }

        if (collisionDirection.x != 0) {
            if (fabs(data->velocity.y) > 0.75f) {
                data->velocity.y *= -0.45f;
            }else {
                data->velocity.y = 0;
            }
            data->velocity.y = 0;
        } else {
            this->x = next.x;
        }

        // movement animation
        this->textureOffsetY = fabs(sin((float)state->internalTimer * 0.15f)) * velocityLength * 0.11f;
    }


    // move camera
    {
        float playerCamValue = this->x - state->map.width * 0.66f;
        state->camera.distance = max(state->camera.distance, playerCamValue);
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
        .textureOffsetX = 0,
        .textureOffsetY = 0,
        .update = &playerUpdate,
        .light = (EntityLight){
            .isLight = true,
            .radius = 20,
            .r = 1,
            .g = 1,
            .b = 1
        },
    }, &(PlayerData){
        .velocity = (Vector2) {0, 0},
        .speed = 0.1f
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
        .textureOffsetX = 0,
        .textureOffsetY = 0,
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
