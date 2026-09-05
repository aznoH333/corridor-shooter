#ifndef ENEMIES
#define ENEMIES

#include "gamesim.h"
#include "raylib.h"

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


void spawnEnemy(GameState* state, Vector3 position, int enemyIndex);
void enemyTakeDamage(Entity* this, EnemyData* data, GameState* state, Vector3 point, float damage);



#endif