#include "gameDirector.h"
#include "math.h"
#include "enemies.h"
#include "stdio.h"
#include "utils.h"


void initNewGameDirector(GameState* state) {
    // init new distances
    float levelLength = state->map.length;


    // how many interaction points does the game director get?
    int gdPointCount = min(ceil(levelLength / 16.0f), GD_MAX_POINTS);

    // figure out point placement
    for (int i = 0; i < gdPointCount; ++i) {
        state->gameDirector.gdPoints[i] = i * 16.0f + (sinf(i) * 2.0f);
        printf("generating gd point at %f \n", i * 16.0f + (sinf(i) * 2.0f));
    }

    // set vals
    state->gameDirector.pointsUsed = gdPointCount;
    state->gameDirector.reached = 0;

}


#define GD_SPAWN_DIST 20.0

void spawnEnemyGroup(GameState* state, float spawnX, float groupSize, float groupDifficulty) {
    

    int gSize = max(floor(groupSize), 1);
    int enemiesToSpawn = GetRandomValue(gSize, gSize + 2);


    for (int i = 0; i < enemiesToSpawn; ++i) {
        // pick enemy to spawn
        Vector3 position = (Vector3){
            .x = spawnX,
            .y = 0,
            .z = 0
        };
        
        spawnEnemyForDiff(state, position, groupDifficulty);
    }



}




void updateGameDirector(GameState* state) {

    float dist = state->camera.distance;

    if (state->gameDirector.reached >= state->gameDirector.pointsUsed || dist + GD_SPAWN_DIST > state->map.length) {
        
        return;
    }

    
    float nextPoint = state->gameDirector.gdPoints[state->gameDirector.reached];


    if (dist > nextPoint) {
        state->gameDirector.reached++;


        // do action
        // temporary placeholder
        
        spawnEnemyGroup(state, nextPoint + GD_SPAWN_DIST, randomFloat(2, 4), randomFloat(1, 3));

        printf("reached point %f, debug [reached:%d] \n", nextPoint, state->gameDirector.reached);

    }

}
