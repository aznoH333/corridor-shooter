#ifndef GAME_DIRECTOR
#define GAME_DIRECTOR

#include "gamesim.h"

// the purpose of the game director is to spawn enemies and loot along the level
// think of the game director in l4d2


void initNewGameDirector(GameState* state);
void updateGameDirector(GameState* state);


#endif