#ifndef STATE_CONTROL
#define STATE_CONTROL

#include "gamesim.h"


// initializes new main menu screen
void goToMainMenu(GameState* state);

// initialuzes new game (no unlocks)
void createNewCharacter(GameState* state);

// opens gear select screen for the current character
void goToGearSelect(GameState* state);

// opens the map screen
void goToMapScreen(GameState* state);

// prepares a level
void generateNewLevel(GameState* state);

// starts the game (gameplay part)
void goToGame(GameState* state);

// pauses gameplay
void pauseGame(GameState* state);

// unpauses gameplay
void unpauseGame(GameState* state);

// quits the window
void quitGame(GameState* state);

// kills all entities. resets camera and world
void clearGameState(GameState* state);


#endif