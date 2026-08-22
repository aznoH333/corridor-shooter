#include "enemies.h"
#include "entityUtils.h"
#include "raymath.h"
#include "utils.h"
#include "particles.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Enemy base code#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum {
    ENEMY_AI_GRID_APPROACH,
    ENEMY_AI_SHIELD_APPROACH,
    ENEMY_AI_RANGER
} EnemyAI;

typedef struct {
    int actionTimer;
    int actionTimerMax;
    Vector3 movementDirection;
    float movementSpeed;
    float movementVelocity;
    float deceleration;
    float health;
    EnemyAI ai;
} EnemyData;


void moveEnemyInDirection(EnemyData* data, Vector3 direction, float velocity) {
    data->movementDirection = Vector3Normalize(direction);
    data->movementVelocity = velocity;
}

void enemyShootInDirection(EnemyData* data, Vector3 direction) {
    // This is intentionaly empty
}

void enemyAiDecision(Entity* this, EnemyData* data, GameState* state) {
    // this entire function is just ai slop :(
    
    Entity* player = findEntityByType(state, this, ENTITY_PLAYER);

    if (player == NULL) {
        moveEnemyInDirection(data, (Vector3){0, 0, 0}, 0);
        return;
    }

    const float APPROACH_DISTANCE = 3.0f;
    const float RANGER_DISTANCE = 7.0f;
    const float Z_MARGIN = 0.35f;
    const float WALL_MARGIN = 1.0f;
    const float MIN_MOVE_DISTANCE = 0.01f;
    float playerDistance = this->x - player->x;
    float zDifference = player->z - this->z;
    float halfMapWidth = state->map.width * 0.5f;
    float halfEnemyWidth = this->width * 0.5f;
    bool canMoveLeft = this->z - halfEnemyWidth - WALL_MARGIN > -halfMapWidth;
    bool canMoveRight = this->z + halfEnemyWidth + WALL_MARGIN < halfMapWidth;
    float zDirection = zDifference > 0 ? 1.0f : -1.0f;
    Vector3 direction = {0};
    float movementVelocity = data->movementSpeed;
    float maxMoveDistance = data->movementSpeed;
    float desiredMoveDistance = data->movementSpeed;
    bool shouldLimitMoveDistance = false;

    if (data->deceleration > 0) {
        float stepCount = ceilf(data->movementSpeed / data->deceleration);
        maxMoveDistance = stepCount * data->movementSpeed - data->deceleration * stepCount * (stepCount - 1.0f) * 0.5f;
        desiredMoveDistance = maxMoveDistance;
    }

    if (fabsf(zDifference) <= Z_MARGIN) {
        zDirection = canMoveLeft && (!canMoveRight || GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
    }

    if (zDirection < 0 && !canMoveLeft) {
        zDirection = canMoveRight ? 1.0f : 0.0f;
    } else if (zDirection > 0 && !canMoveRight) {
        zDirection = canMoveLeft ? -1.0f : 0.0f;
    }

    switch (data->ai) {
        case ENEMY_AI_GRID_APPROACH:
            if (playerDistance > APPROACH_DISTANCE) {
                if (GetRandomValue(1, 100) <= 80) {
                    direction = (Vector3){-1, 0, 0};
                    desiredMoveDistance = playerDistance - APPROACH_DISTANCE;
                    shouldLimitMoveDistance = true;
                } else {
                    direction = (Vector3){0, 0, zDirection};
                }
            } else {
                if (fabsf(zDifference) > Z_MARGIN) {
                    direction = (Vector3){0, 0, zDirection};
                    desiredMoveDistance = fabsf(zDifference);
                    shouldLimitMoveDistance = true;
                }
            }
            break;

        case ENEMY_AI_SHIELD_APPROACH:
            if (playerDistance > APPROACH_DISTANCE && fabsf(zDifference) <= Z_MARGIN) {
                direction = (Vector3){-1, 0, 0};
                desiredMoveDistance = playerDistance - APPROACH_DISTANCE;
                shouldLimitMoveDistance = true;
            } else {
                if (fabsf(zDifference) > Z_MARGIN) {
                    direction = (Vector3){0, 0, zDirection};
                    desiredMoveDistance = fabsf(zDifference);
                    shouldLimitMoveDistance = true;
                }
            }
            break;

        case ENEMY_AI_RANGER:
            if (playerDistance > RANGER_DISTANCE + Z_MARGIN) {
                direction = (Vector3){-1, 0, 0};
                desiredMoveDistance = playerDistance - RANGER_DISTANCE;
                shouldLimitMoveDistance = true;
            } else if (playerDistance < RANGER_DISTANCE - Z_MARGIN) {
                direction = (Vector3){1, 0, 0};
                desiredMoveDistance = RANGER_DISTANCE - playerDistance;
                shouldLimitMoveDistance = true;
            } else {
                direction = (Vector3){0, 0, zDirection};
            }
            break;
    }

    if (shouldLimitMoveDistance) {
        if (desiredMoveDistance <= MIN_MOVE_DISTANCE) {
            direction = (Vector3){0};
            movementVelocity = 0;
        } else if (desiredMoveDistance < maxMoveDistance && data->deceleration > 0) {
            float lowVelocity = 0;
            float highVelocity = data->movementSpeed;

            for (int i = 0; i < 12; ++i) {
                float testVelocity = (lowVelocity + highVelocity) * 0.5f;
                float stepCount = ceilf(testVelocity / data->deceleration);
                float moveDistance = stepCount * testVelocity - data->deceleration * stepCount * (stepCount - 1.0f) * 0.5f;

                if (moveDistance > desiredMoveDistance) {
                    highVelocity = testVelocity;
                } else {
                    lowVelocity = testVelocity;
                }
            }

            movementVelocity = lowVelocity;
        } else if (data->deceleration <= 0) {
            movementVelocity = min(data->movementSpeed, desiredMoveDistance);
        }
    }

    moveEnemyInDirection(data, direction, movementVelocity);

}

void enemyTakeDamage(Entity* this, EnemyData* data, GameState* state, Vector3 point, float damage) {
    data->health -= damage;

    bloodSplash(
        state,
        (Vector3) {this->x, this->y, this->z},
        damage
    );
}

bool enemyUpdate(Entity* this, GameState* state) {
    EnemyData* data = (EnemyData*) &this->data;


    // taking damage
    {
        Entity* collidedBullet = getCollidingEntityByType(state, this, ENTITY_BULLET);

        if (collidedBullet != NULL) {
            enemyTakeDamage(this, data, state, (Vector3) {collidedBullet->x, collidedBullet->y, collidedBullet->z} , 1);
        }
    }

    // actions
    {
        data->actionTimer--;

        if (data->actionTimer == 0) {
            data->actionTimer = data->actionTimerMax;
            enemyAiDecision(this, data, state);
        }
    }

    // moving
    {
        Vector3 next = Vector3Add((Vector3){.x = this->x, .y = this->y, .z = this->z}, Vector3Scale(data->movementDirection, data->movementVelocity));
        data->movementVelocity = approachNumber(data->movementVelocity, 0, data->deceleration);

        Vector3 collisions = checkWorldCollision(next.x, next.y, next.z, this->width, this->height, &state->map);


        if (collisions.x != 0) {
            next.x = this->x;
        }

        if (collisions.z != 0) {
            next.z = this->z;
        }

        this->x = next.x;
        this->y = next.y;
        this->z = next.z;

    }


    // dying
    if (data->health <= 0) {
        bloodSplash(
            state,
            (Vector3) {this->x, this->y, this->z},
            40
        );

        addScreenShake(state, 8);

        
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Spawning functions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void genericGuy(GameState* state, Vector3 position) {
    addEntity(state, (Entity){
        .texture = "debug_entities_0001",
        .x = position.x,
        .y = position.y + 1,
        .z = position.z,
        .width = 2.0f,
        .height = 2.0f,
        .textureWidth = 64.0f,
        .textureHeight = 64.0f,
        .textureOffsetX = 0,
        .textureOffsetY = 0,
        .update = &enemyUpdate,
        .light = emptyLight,
        .type = ENTITY_ENEMY,
        .color = WHITE,
        .textureRotation = 0,

    }, &(EnemyData){
        .actionTimer = 60,
        .actionTimerMax = 60,
        .movementDirection = (Vector3) {0},
        .movementSpeed = 0.2f,
        .movementVelocity = 0,
        .deceleration = 0.01,
        .health = 10,
        .ai = ENEMY_AI_GRID_APPROACH,
    },
        sizeof(EnemyData)
    );
}


void (*spawnFunctions[])(GameState*, Vector3) = {&genericGuy};


void spawnEnemy(GameState* state, Vector3 position, int enemyIndex){
    void (*spawn)(GameState*, Vector3) = spawnFunctions[enemyIndex];

    spawn(state, position);
}
