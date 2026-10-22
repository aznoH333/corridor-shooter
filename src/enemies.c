#include "enemies.h"
#include "entityUtils.h"
#include "raymath.h"
#include "utils.h"
#include "particles.h"
#include "fileReader.h"
#include "stdlib.h"
#include "stdbool.h"
#include "entities.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Enemy ai definition#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static EnemyAIValues enemyAis[] = {
    { // debug
        .attackDistMin = 2.5,
        .attackDistMax = 4.0,
        .skittishness = 0.5,
        .attackChance = 0.85,
    }
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Enemy base code#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void moveEnemyInDirection(EnemyData* data, Vector3 direction, float velocity) {
    data->movementDirection = Vector3Normalize(direction);
    data->movementVelocity = velocity;
    data->movementTimer = 1.0f;
}

void moveEnemyTowardPosition(Entity* this, EnemyData* data, Vector3 target) {
    Vector3 dir = Vector3Subtract(
        target,
        (Vector3) {this->x, this->y, this->z}
    );

    float distance = Vector3Length(dir);

    // I have no clue what causes the movement system to land at this equation.
    // But it does.
    float magicSpeedCalculationFormula = min(data->stats.speed, distance * (2*data->deceleration) / (1 + data->deceleration));


    moveEnemyInDirection(data, dir, magicSpeedCalculationFormula);
}

void enemyAiDecision(Entity* this, EnemyData* data, GameState* state) {
    
    // setup variables
    Entity* player = findEntityByType(state, this, ENTITY_PLAYER);

    if (player == NULL) {
        return;
    }

    Vector3 thisPos = (Vector3) { this->x, this->y, this->z };
    Vector3 playerPos = (Vector3) { player->x, this->y, player->z };


    // calculate desired move point
    Vector3 dirToPlayer = Vector3Subtract(
        playerPos,
        thisPos
    );

    float distToPlayer = Vector3Length(dirToPlayer);
    float desiredDist = data->ai.attackDistMin + ((data->ai.attackDistMax - data->ai.attackDistMin) * 0.5);


    // check if should attack
    if (
        distToPlayer < data->ai.attackDistMax && 
        distToPlayer > data->ai.attackDistMin && 
        randomChance(data->ai.attackChance)) 
    { // attack code
    
        // calculate direction
        Vector3 attackDirection = dirToPlayer;


        // set projectile data
        ProjectileInitData pData = (ProjectileInitData){
            .velocity = 0.3,
            .maxDistanceTraveled = 2,
            .lightRadius = 0,
            .damage = 10,
            .texture = simpleTexture("enemy_claw_attack_0001", 23, 22),
            .targetType = ENTITY_PLAYER,
            .trailParticleCount = 2,
            .trailParticleDistance = 0.1f,
            .fadeInTime = 3,
            .fadeOutTime = 10,
            .collideWithTerrain = false,
            .deceleration = 0.01f,
        };
        
        // spawn projectile
        projectile(
            state,
            thisPos,
            attackDirection,
            pData
        );
    
    
    } else { // movement code
        Vector3 desiredMovePoint = Vector3Add(
            playerPos,
            Vector3Scale(Vector3Normalize(dirToPlayer), -desiredDist)
        );

        Vector3 finalMovePoint = desiredMovePoint;
        

        // calculate skittishness
        Vector3 randomDir = Vector3Normalize((Vector3) {randomFloat(-1, 1), 0, randomFloat(-1, 1)});
        Vector3 skittishnessOffset = Vector3Scale(randomDir, data->ai.skittishness);

        finalMovePoint = Vector3Add(finalMovePoint, skittishnessOffset);


        // move to point
        moveEnemyTowardPosition(
            this,
            data, 
            finalMovePoint
        );
    }

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


            addEntityPlane(state, 
                (Vector3) {
                    .x = this->x + (part->z), 
                    .y = this->y + (part->y), 
                    .z = this->z + (part->x)
                }, 
                part->texture, 
                part->textureSizeX,
                part->textureSizeY, 
                part->color,
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
        Vector3 next = Vector3Add((Vector3){.x = this->x, .y = this->y, .z = this->z}, Vector3Scale(data->movementDirection, data->movementVelocity * data->movementTimer));
        
        data->movementTimer = approachNumber(data->movementTimer, 0, data->deceleration);
        //data->movementVelocity = approachNumber(data->movementVelocity, 0, data->deceleration);

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

    

    parts[usedEnemyParts] = (EnemyPartDefinition){
        .name = name,
        .texture = texture,
        .textureSizeX = textureSizeX,
        .textureSizeY = textureSizeY,
        .stats = stats
    };

    usedEnemyParts++;

    printf("Loaded enemy part [path:%s] [name:%s]\n", fullPath, name);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Enemy definitions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ED_NAME_LENGTH 256

typedef struct {
    float width;
    float height;
    EnemyPart parts[MAX_ENEMY_PARTS];
    int usedParts;
    char name[ED_NAME_LENGTH];
    int aiIndex;
    float difficulty;
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
    // name
    char* name =            fileNextAlloc256(&file);
    for (int i = 0; i < ED_NAME_LENGTH; ++i) {
        definition.name[i] = name[i];
    }
    free(name);

    // ai
    definition.aiIndex =    fileNextI(&file);
    definition.difficulty = fileNextF(&file);        // difficulty
    // width / height
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
        float offsetX      = fileNextF(&file) * TEX_SIZE_TO_GAME;
        float offsetY      =-fileNextF(&file) * TEX_SIZE_TO_GAME;
        float offsetZ      =-fileNextF(&file) * TEX_SIZE_TO_GAME * 0.5;
        float rotation     = fileNextF(&file);
        float r            = fileNextF(&file);
        float g            = fileNextF(&file);
        float b            = fileNextF(&file);
        float a            = fileNextF(&file);


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




void spawnEnemyFromDef(GameState* state, Vector3 position, EnemyDefinition* definition) {
    
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

    printf("spawning enemy [id:%s] [health:%f] [speed:%f] [action:%f]\n",definition->name, stats.health, stats.speed, stats.action);


    // copy the parts array
    // c doesn't like when you raw assign arrays.
    EnemyData data = {
        .actionTimer = stats.action,
        .movementDirection = (Vector3) {0},
        .movementVelocity = 0,
        .deceleration = 0.05,
        .movementTimer = 0,
        .health = stats.health,
        .ai = enemyAis[definition->aiIndex],
        .stats = stats,
        .parts = {0},
        .usedParts = definition->usedParts
    };

    for (int i = 0; i < definition->usedParts; ++i) {
        data.parts[i] = definition->parts[i];        
    }


    // spawn entity
    addEntity(state, (Entity){
        .texture = noTexture(),
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


void spawnEnemy(GameState* state, Vector3 position, char* enemyName){
    EnemyDefinition* definition = NULL;
    
    // find definition by name
    for ( int i = 0; i < usedEnemies; ++i ) {
        // compare name
        EnemyDefinition* def = &enemies[i];
        bool found = true;
        
        for ( int j = 0; j < ED_NAME_LENGTH; ++j) {
            if (enemyName[j] != def->name[j]) {
                found = false;
                break;
            }
            else if (enemyName[j] == 0) {
                break; // reached terminator
            }
        }

        if (found) {
            definition = def;
            break;
        }
    }

    if (definition == NULL) {
        printf("failed to find enemy with name %s \n", enemyName);
        return;
    }


    spawnEnemyFromDef(state, position, definition);
}



void spawnEnemyForDiff(GameState* state, Vector3 position, float diff) {
    EnemyDefinition* definition = &enemies[0]; // fallback to small guy if no enemies within diff exist

    EnemyDefinition* availableDefs[8];
    int usedDefs = 0;

    const float ENEMY_DIFF_DIST = 0.7f;
    // find available enemies

    for ( int i = 0; i < usedEnemies; ++i ) {

        EnemyDefinition* def = &enemies[i];

        if (fabs(def->difficulty - diff) < ENEMY_DIFF_DIST) {
            availableDefs[usedDefs] = def;
            usedDefs++;
            
            if (usedDefs >= 8) {
                break;
            }
        }
    }

    // pick random from list
    if (usedDefs > 0) {
        definition = availableDefs[GetRandomValue(0, usedDefs - 1)];
    }
    spawnEnemyFromDef(state, position, definition);
}

