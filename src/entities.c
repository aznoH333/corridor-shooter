#include "entities.h"
#include "entityUtils.h"
#include "math.h"
#include "raylib.h"
#include "utils.h"
#include "particles.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Bullet#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    float velocity;
    float distanceTraveled;
    float maxDistanceTraveled;
    Vector3 direction;
    float lightRadius;
    int internalTimer;

} BulletData;

bool bulletUpdate(Entity* this, GameState* state) {
    BulletData* data = (BulletData*) &this->data;

    Vector3 direction = Vector3Normalize(data->direction);


    Vector3 next = (Vector3) {this->x + direction.x * data->velocity, this->y + direction.y * data->velocity, this->z + direction.z * data->velocity};



    // wall collisions
    Vector3 collisions = checkWorldCollision(next.x, next.y, next.z, this->width, this->height, &state->map);

    if (Vector3Length(collisions) > 0.1f) {
        smokePuff(state, (Vector3){this->x, this->y, this->z});
        playSound("impact_stone", 2, 0.3);
        
        return false;
    }


    // enemy collisions
    Entity* collidedEnemy = getCollidingEntityByType(state, this, ENTITY_ENEMY);

    if (collidedEnemy != NULL) {
        bloodPuff(state, (Vector3){this->x, this->y, this->z});
        playSound("flesh_impact_fast", 0.3, 0.3);

        return false;
    }


    // distance
    data->distanceTraveled += 
        fabs(direction.x) * data->velocity + 
        fabs(direction.y) * data->velocity + 
        fabs(direction.z) * data->velocity;

    if (data->distanceTraveled > data->maxDistanceTraveled) {
        return false;
    }
    this->light.radius = data->lightRadius * (1 - (data->distanceTraveled / data->maxDistanceTraveled));


    // spawn fade particles
    if (data->internalTimer > 0) {
        Vector3 particlePosition = {this->x, this->y, this->z};
    
        for (int i = 1; i < 5; ++i) {
            addEntityPlane(state, 
                particlePosition, 
                this->texture.texture, 
                this->texture.width, 
                this->texture.height, 
                WHITE
            );

            particlePosition.x -= data->direction.x * (i * 0.05f);
            particlePosition.y -= data->direction.y * (i * 0.05f);
            particlePosition.z -= data->direction.z * (i * 0.05f);

        }
    }
    
    
    
    

    // update
    this->x = next.x;
    this->y = next.y;
    this->z = next.z;
    data->internalTimer++;



    

    return true;
}


void bullet(GameState* state, float x, float y, float z, float velocity, Vector3 direction) {
    addEntity(state, (Entity) {
        .texture = simpleTexture("bullet_2", 2, 2),
        .x = x,
        .y = y,
        .z = z,
        .width = 0.1f,
        .height = 0.1f,
        .update = &bulletUpdate,
        .light = emptyLight(),
        .type = ENTITY_BULLET,
        

    }, &(BulletData){
        .velocity = velocity,
        .distanceTraveled = 0,
        .direction = direction,
        .maxDistanceTraveled = 20,
        .lightRadius = 5,
        .internalTimer = 0,
    }, sizeof(BulletData));
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Player#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    Vector3 velocity;
    float speed;

    // gun data
    int gunCooldown;
    int totalGunCooldown;
    
    float minGunSpread;
    int gunSpreadAccumulator;
    float gunSpreadMultiplier;
    float recoilMultiplier;

} PlayerData;

static Vector3 getPlayerInput(void) {
    Vector3 input = (Vector3){
        .x = IsKeyDown(KEY_W) - IsKeyDown(KEY_S),
        .y = 0,
        .z = IsKeyDown(KEY_D) - IsKeyDown(KEY_A)
    };

    if (input.x != 0 || input.z != 0) {
        input = Vector3Normalize(input);
    }

    return input;
}

bool playerUpdate(Entity* this, GameState* state) {
    PlayerData* data = (PlayerData*) &this->data;

    // movement
    {
        float movementAcceleration = 0.1f;
        Vector3 input = getPlayerInput();
        float sprintVelocity = 0.65f + ((float)IsKeyDown(KEY_LEFT_SHIFT) * 0.35f);

        Vector3 next = {
            .x = this->x + data->velocity.x * data->speed,
            .y = 0,
            .z = this->z + data->velocity.z * data->speed
        };

        float velocityLength = Vector2Length((Vector2){data->velocity.x, data->velocity.z});
        if (velocityLength > 0.75f) {
            movementAcceleration = 0.025f;//movementAcceleration / max(totalSpeed - data->speed * 0.5f, 0.1f);
        }


        data->velocity.x = approachNumber(data->velocity.x, input.x * sprintVelocity, movementAcceleration);
        data->velocity.z = approachNumber(data->velocity.z, input.z * sprintVelocity, movementAcceleration);

        Vector3 collisionDirection = checkWorldCollision(next.x, next.y, next.z, this->width, this->height, &state->map);

        // wall collisions
        if (collisionDirection.z != 0) {
            if (fabs(data->velocity.z) > 0.75f) { // add a little bounce when the player collides with a wall above certain speeds
                data->velocity.z *= -0.45f;
            }else {
                data->velocity.z = 0;
            }
        } else {
            this->z = next.z;
        }

        if (collisionDirection.x != 0) {
            if (fabs(data->velocity.x) > 0.75f) {
                data->velocity.x *= -0.45f;
            }else {
                data->velocity.x = 0;
            }
            data->velocity.x = 0;
        } else {
            this->x = next.x;
        }


        float walkTimer = fabs(sin((float)state->internalTimer * 0.15f)) * velocityLength * 0.11f;
        // movement animation
        this->texture.offset.y = walkTimer;

        // footstep noise
        if (walkTimer < 0.01 && velocityLength > 0.1) {
            playSound("footstep", 1, 1);
        }
    }


    // move camera
    {
        float playerCamValue = this->x - state->map.width * 0.66f;
        state->camera.distance = max(state->camera.distance, playerCamValue);
    }

    // shooting
    {
        if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && data->gunCooldown == 0) {
            
            
            // base bullet direction
            Vector3 direction = Vector3Normalize(Vector3Subtract(getMouseHit(state), (Vector3){.x = this->x, .y = this->y, .z = this->z}));

            // calculate spread
            Vector3 spreadVector = Vector3Normalize((Vector3) {
                .x = randomFloat(-1, 1),
                .y = randomFloat(-1, 1),
                .z = randomFloat(-1, 1)
            });
            float spreadMultiplier = data->minGunSpread + (data->gunSpreadAccumulator * data->gunSpreadMultiplier);

            direction = Vector3Add(direction, Vector3Scale(spreadVector, spreadMultiplier));

            // calculate recoil
            Vector3 recoilVector = (Vector3) {
                .x = 0,
                .y = data->gunSpreadAccumulator * data->recoilMultiplier,
                .z = 0
            };

            direction = Vector3Normalize(Vector3Add(direction, recoilVector));


            // fire bullet
            bullet(state, this->x, this->y, this->z, 1.0f, direction);
            data->gunCooldown = data->totalGunCooldown;
            addScreenShake(state, 3);
            playSound("machine_gun_2", 1, 1);
            data->gunSpreadAccumulator++;
            
            muzzleFlash(
                state, 
                Vector3Add((Vector3){this->x, this->y, this->z}, Vector3Scale(direction, 0.2))
            );
            bulletCasing(
                state,
                (Vector3){this->x, this->y, this->z},
                direction
            );
        }



        if (data->gunCooldown == 0 && data->gunSpreadAccumulator > 0) {
            data->gunSpreadAccumulator--;
        }
        data->gunSpreadAccumulator = min(data->gunSpreadAccumulator, 10);



        if (data->gunCooldown > 0) {
            data->gunCooldown--;
        }

        

    }




    return true;
}


void player(GameState* state, float x, float y, float z){
    addEntity(state, (Entity) {
        .texture = simpleTexture("player_alt", 18, 24),
        .x = x,
        .y = y + 0.5f,
        .z = z,
        .width = 1.0f,
        .height = 1.0f,
        .update = &playerUpdate,
        .light = (EntityLight){
            .isLight = true,
            .radius = 20,
            .r = 1,
            .g = 1,
            .b = 1
        },
        .type = ENTITY_PLAYER

    }, &(PlayerData){
        .velocity = (Vector3) {0, 0, 0},
        .speed = 0.1f,

        // gun stuff
        .gunCooldown = 0,
        .totalGunCooldown = 4,
        .minGunSpread = 0.02,
        .gunSpreadAccumulator = 0,
        .gunSpreadMultiplier = 0.002,
        .recoilMultiplier = 0.01,
        
    }, sizeof(PlayerData));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Dummy#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    Vector3 direction;
} DummyData;

bool dummyUpdate(Entity* this, GameState* state){
    DummyData* data = (DummyData*) &this->data;
    

    Vector3 next = (Vector3) {
        .x = this->x + data->direction.x * 0.2,
        .y = this->y + data->direction.y * 0.2,
        .z = this->z + data->direction.z * 0.2
    };

    Vector3 collisions = checkWorldCollision(next.x, next.y, next.z, this->width, this->height, &state->map);


    data->direction.x += -collisions.x;
    data->direction.y += -collisions.y;
    data->direction.z += -collisions.z;



    this->x = next.x;
    this->y = next.y;
    this->z = next.z;


    
    return true;
}

void dummy(GameCamera* state, float x, float y, float z){
    addEntity(state, (Entity){
        .texture = simpleTexture("debug_entities_0001", 32, 32),
        .x = x,
        .y = y + 0.5f,
        .z = z,
        .width = 1.0f,
        .height = 1.0f,
        .update = &dummyUpdate,
        .light = (EntityLight){
            .isLight = true,
            .radius = 5,
            .r = 1,
            .g = 0,
            .b = 0
        },
        .type = ENTITY_UNSET,

    }, &(DummyData){
        .direction = (Vector3) {.x = 1, .y = 1, .z = 1}
    },
        sizeof(DummyData)
    );
}
