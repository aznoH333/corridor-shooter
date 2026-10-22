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


void updateGameDirector(GameState* state) {

    float dist = state->camera.distance;
    const float spawnDist = 20.0;

    if (state->gameDirector.reached >= state->gameDirector.pointsUsed || dist + spawnDist > state->map.length) {
        
        return;
    }

    
    float nextPoint = state->gameDirector.gdPoints[state->gameDirector.reached];


    if (dist > nextPoint) {
        state->gameDirector.reached++;


        // do action
        // temporary placeholder
        spawnEnemy(state, (Vector3){nextPoint + spawnDist, 0, 0}, 0);

        printf("reached point %f, debug [reached:%d] \n", nextPoint, state->gameDirector.reached);

    }

}
