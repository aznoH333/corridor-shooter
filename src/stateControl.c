#include "stateControl.h"
#include "entities.h"
#include "enemies.h"


// initializes new main menu screen
void goToMainMenu(GameState* state) {

    clearGameState(state);
    switchGameState(state, MAIN_MENU);


}

// initialuzes new game (no unlocks)
void createNewCharacter(GameState* state) {
    initNewPlayerInstance(state);
}

// opens gear select screen for the current character
void goToGearSelect(GameState* state) {

}

// opens the map screen
void goToMapScreen(GameState* state) {

}

// prepares a level
void generateNewLevel(GameState* state) {
    clearGameState(state);
    
    
    // temporary generation
    player(state, 0, 0, 0);

    spawnEnemy(state, (Vector3){50, 0, 0}, 0);
    spawnEnemy(state, (Vector3){50, 0, 2}, 1);
    spawnEnemy(state, (Vector3){50, 0, -2}, 2);
    spawnEnemy(state, (Vector3){40, 0, -2}, 3);
    spawnEnemy(state, (Vector3){40, 0, 0}, 4);
    spawnEnemy(state, (Vector3){40, 0, 2}, 5);
}

// starts the game (gameplay part)
void goToGame(GameState* state) {
    switchGameState(state, GAME);
    //playMusic("ambience", 1);
}

// pauses gameplay
void pauseGame(GameState* state) {
    if (state->metaState.state.name == GAME) {
        switchGameState(state, GAME_PAUSED);
    }
}

// unpauses gameplay
void unpauseGame(GameState* state) {
    if (state->metaState.state.name == GAME_PAUSED) {
        switchGameState(state, GAME);
    }
}

// quits the window
void quitGame(GameState* state) {
    state->metaState.shouldWindowLive = false; // TODO do more stuff here later
}



// kills all entities. resets camera and world
void clearGameState(GameState* state) {
    state->entities.count = 0;
    state->camera.distance = -999; // lol
}