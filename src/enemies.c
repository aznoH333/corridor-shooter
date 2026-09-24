#include "enemies.h"
#include "entityUtils.h"
#include "raymath.h"
#include "utils.h"
#include "particles.h"
#include "fileReader.h"
#include "stdlib.h"
#include "stdbool.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Enemy base code#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
    float movementVelocity = data->stats.speed;
    float maxMoveDistance = data->stats.speed;
    float desiredMoveDistance = data->stats.speed;
    bool shouldLimitMoveDistance = false;

    if (data->deceleration > 0) {
        float stepCount = ceilf(data->stats.speed / data->deceleration);
        maxMoveDistance = stepCount * data->stats.speed - data->deceleration * stepCount * (stepCount - 1.0f) * 0.5f;
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
            float highVelocity = data->stats.speed;

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
            movementVelocity = min(data->stats.speed, desiredMoveDistance);
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

    { // drawing
        for (int i = 0; i < data->usedParts; ++i) {
            EnemyPart* part = &data->parts[i];

            Vector3 augh = (Vector3) {
                    .x = this->x,// - (part->z * TEX_SIZE_TO_GAME), 
                    .y = 2 - (part->y * TEX_SIZE_TO_GAME),//this->y, + i, 
                    .z = this->z + (part->x * TEX_SIZE_TO_GAME)
                };
            printf("what %f\n", augh.y);

            addEntityPlane(state, 
                augh, 
                part->texture, 
                part->textureSizeX,
                part->textureSizeY, 
                WHITE,
                0,
                QUARTER_ROTATION + part->rotation,
                QUARTER_ROTATION
            );

        }
    }


    { // actions
        data->actionTimer--;

        if (data->actionTimer == 0) {
            data->actionTimer = data->stats.action;
            enemyAiDecision(this, data, state);
        }
    }

    { // moving
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
        goreExplosion(
            state,
            (Vector3) {this->x, this->y, this->z},
            40
        );

        addScreenShake(state, 8);
        playSound("gore", 1, 0.3);

        
        return false;
    }

    return true;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Enemy partsn#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    char* name;
    char* texture;
    float textureSizeX;
    float textureSizeY;
    EnemyStats stats;
} EnemyPartDefinition;

static EnemyPartDefinition parts[32] = {0};
static int usedEnemyParts = 0;
void loadPart(char* path, int index, struct dirent* dir) {
    // first two entries are . and .. (thanks whover designed that)    
    if (index < 2 || index > 32) {return;}

    char fullPath[128] = {0};// make path
    sprintf(&fullPath, "%s/%s", path, dir->d_name);

    LoadedFile file = readFile(fullPath);
    EnemyStats stats = {0};


    // read data from file
                         fileSkip(&file);         // version number
    char* name =         fileNextAlloc256(&file); // part name
    char* texture =      fileNextAlloc256(&file); // texture name
    float textureSizeX = fileNextF(&file);        // texture size x
    float textureSizeY = fileNextF(&file);        // texture size y
    stats.health =       fileNextF(&file);        // part health
    stats.healthMult =   fileNextF(&file);        // part health mult
    stats.speed =        fileNextF(&file);        // part speed
    stats.speedMult =    fileNextF(&file);        // part speed mult
    stats.action =       fileNextF(&file);        // part action timer
    stats.actionMult =   fileNextF(&file);        // part action mult

    

    parts[index - 2] = (EnemyPartDefinition){
        .name = name,
        .texture = texture,
        .textureSizeX = textureSizeX,
        .textureSizeY = textureSizeY,
        .stats = stats
    };

    usedEnemyParts = index - 2;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Enemy definitions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    float width;
    float height;

    EnemyPart parts[MAX_ENEMY_PARTS];
    int usedParts;
} EnemyDefinition;

#define MAX_ENEMY_DEFINITIONS 20
static EnemyDefinition enemies[MAX_ENEMY_DEFINITIONS] = {0};
static int usedEnemies = 0;

void loadEnemy(char* path, int index, struct dirent* dir) {
    // init file
    if (index < 2 || index > MAX_ENEMY_DEFINITIONS) {return;}

    char fullPath[128] = {0};// make path
    sprintf(&fullPath, "%s/%s", path, dir->d_name);


    LoadedFile file = readFile(fullPath);
    EnemyDefinition definition = {0};
    
    // read data from file
                            fileSkip(&file);         // version number
    definition.width =      fileNextF(&file);        // enemy width
    definition.height =     fileNextF(&file);        // enemy height



    // iterate parts
    int usedParts = 0;
    while(fileHasNext(&file)) {
        // find referenced part by name
        char* partName = fileNext(&file);
        int partIndex = -1;
        for ( int i = 0; i < usedEnemyParts; ++i ) {
            EnemyPartDefinition* part = &parts[i];
            bool found = true;
            for ( int j = 0; j < 256; j++ ) {
                if (part->name[j] != partName[j]) {
                    found = false;
                    break;
                }
            }
            // stop search
            if (found) {
                partIndex = i;
                break;
            }
        }
        // couldn't find part
        if (partIndex == -1 ) {
            printf("couldn't find part with name %s \n", partName);
            // skip this entry
            for (int i = 0; i < 8; ++i) {
                fileSkip(&file);
            }
            
            continue;
        }

        // read part
        float offsetX      = fileNextF(&file);
        float offsetY      = fileNextF(&file);
        float offsetZ      = fileNextF(&file);
        float rotation     = fileNextF(&file);
        char r             = fileNextI(&file);
        char g             = fileNextI(&file);
        char b             = fileNextI(&file);
        char a             = fileNextI(&file);

        // build final part
        EnemyPartDefinition* part = &parts[partIndex];
        definition.parts[usedParts++] = (EnemyPart) {
            .texture = part->texture,
            .x = offsetX,
            .y = offsetY,
            .z = offsetZ,
            .rotation = rotation,
            .textureSizeX = part->textureSizeX,
            .textureSizeY = part->textureSizeY,
            .color = (Color) {
                .r = r,
                .g = g,
                .b = b,
                .a = a
            },
            .stats = part->stats
        };

    }

    definition.usedParts = usedParts;
    printf("loaded enemy definition for %s [parts:%d] \n", dir->d_name, definition.usedParts);

    enemies[usedEnemies++] = definition;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Loading and initialization#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initEnemies() {
    // load parts
    doForEachFileInFolder("./resources/parts", &loadPart); 
    doForEachFileInFolder("./resources/enemies", &loadEnemy); 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Spawning functions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


EnemyStats combineStats(EnemyStats first, EnemyStats second) {
    return (EnemyStats) {
        .health = (first.health + second.health) * second.healthMult,
        .healthMult = 0,

        .speed = (first.speed + second.speed) * second.speedMult,
        .speedMult = 0,

        .action = (first.action + second.action) * second.actionMult,
        .actionMult = 0
    };

}



void spawnEnemy(GameState* state, Vector3 position, int enemyIndex){
    
    EnemyDefinition* definition = &enemies[enemyIndex];

    // combine stats

    EnemyStats stats = { // init empty
        .health = 0,
        .healthMult = 0,
        
        .speed = 0,
        .speedMult = 0,
        
        .action = 0,
        .actionMult = 0,
    };


    for (int i = 0; i < definition->usedParts; ++i) {
        EnemyPart* part = &definition->parts[i];

        stats = combineStats(stats, part->stats);
    }

    printf("spawning enemy [id:%d] [health:%f] [speed:%f] [action:%f]\n",enemyIndex, stats.health, stats.speed, stats.action);


    // copy the parts array
    // c doesn't like when you raw assign arrays.
    EnemyData data = {
        .actionTimer = stats.action,
        .movementDirection = (Vector3) {0},
        .movementVelocity = 0,
        .deceleration = 0.01,
        .health = stats.health,
        .ai = ENEMY_AI_GRID_APPROACH,
        .stats = stats,
        .parts = {0},
        .usedParts = definition->usedParts
    };

    for (int i = 0; i < definition->usedParts; ++i) {
        data.parts[i] = definition->parts[i];        
    }


    // spawn entity
    addEntity(state, (Entity){
        .texture = simpleTexture("picus", 32, 32),//noTexture(),
        .x = position.x,
        .y = position.y + definition->height / 2,
        .z = position.z,
        .width = definition->width,
        .height = definition->height,
        .update = &enemyUpdate,
        .light = emptyLight(),
        .type = ENTITY_ENEMY,
    }, &data,
        sizeof(EnemyData)
    );
}
