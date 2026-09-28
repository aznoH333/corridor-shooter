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
    Color color;

    // stats
    EnemyStats stats;
} EnemyPart;


typedef struct {
    
    // the closest distance to the player the enemy will try to keep (if the enemy is closer it will move back)
    float attackDistMin; 
    
    // the longest distance from which the enemy will attack (the enemy won't attack unless they are atlest this close to the player)
    float attackDistMax;
    
    // a randomized offset from the real travel point that the enemy will travel to
    // if this is 0 then the enemy will travel exactly to where it wants to
    // this makes the enemy movement seem more eratic and less robotic
    float skittishness; 
} EnemyAIValues;




#define MAX_ENEMY_PARTS 32

typedef struct {
    int actionTimer;
    Vector3 movementDirection;
    float movementVelocity;
    float deceleration;
    float movementTimer;
    float health;
    EnemyAIValues ai;
    EnemyStats stats;
    EnemyPart parts[MAX_ENEMY_PARTS];
    int usedParts;
} EnemyData;


void spawnEnemy(GameState* state, Vector3 position, int enemyIndex);
void enemyTakeDamage(Entity* this, EnemyData* data, GameState* state, Vector3 point, float damage);
void initEnemies();


#endif