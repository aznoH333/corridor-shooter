#ifndef ENEMIES
#define ENEMIES

#include "gamesim.h"
#include "raylib.h"


// a base struct that holds some stats about the enemy
// this is used by the enemy itself and enemy parts
// the final stats are calculated from all the enemy parts
typedef struct {
    // how much does the enemy have
    float health;
    float healthMult;
    
    // how fast does the enemy move
    float speed;
    float speedMult;
    
    // how often the enemy decides to act (coodlown -> lower is faster)
    float action;
    float actionMult;
} EnemyStats;


typedef struct {
    // visual
    char* texture;
    float x;
    float y;
    float z;
    float rotation;
    float textureSizeX;
    float textureSizeY;

    // stats
    EnemyStats stats;
} EnemyPart;


typedef enum {
    ENEMY_AI_GRID_APPROACH,
    ENEMY_AI_SHIELD_APPROACH,
    ENEMY_AI_RANGER
} EnemyAI;


#define MAX_ENEMY_PARTS 8

typedef struct {
    int actionTimer;
    Vector3 movementDirection;
    float movementVelocity;
    float deceleration;
    float health;
    EnemyAI ai;
    EnemyStats stats;
    EnemyPart parts[MAX_ENEMY_PARTS];
    int usedParts;
} EnemyData;


void spawnEnemy(GameState* state, Vector3 position, int enemyIndex);
void enemyTakeDamage(Entity* this, EnemyData* data, GameState* state, Vector3 point, float damage);
void initEnemies();


#endif