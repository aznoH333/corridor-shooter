#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"
#include "math.h"
#include "world.h"
#include "gamesim.h"
#include "entities.h"
#include "enemies.h"
#include "fileReader.h"
#include "stateControl.h"

int main(void)
{
    // setup window
    SetTraceLogLevel(LOG_WARNING); 
    const int screenWidth = 800;
    const int screenHeight = 400;


    InitTextureWindow(screenWidth, screenHeight, 1920, 1080, "empty project");	
	
    UseShader("./resources/shaders/shaderVert.vs", "./resources/shaders/shaderFrag.fs");
    Use3DShader("./resources/shaders/shader3DVert.vs", "./resources/shaders/shader3DFrag.fs");
    SetTargetFPS(60);


    initEnemies();


    // setup world
    WorldRenderingData renderingData = prepareWorldRenderingData();
    
    GameState* state = initEmptyGame();



    HideCursor();



    
    goToMainMenu(state);


	// Main game loop
    SetExitKey(0);
    while (state->metaState.shouldWindowLive)
	{

        // temporary fullscreen hack
        if (IsKeyPressed(KEY_P)) {
            SwitchResolution(0, 0, true);
        }
        
        GameState* nextFrame = createNextFrame(state);
        World renderedWorld = convertToWorld(state);
        
        free(state);
        state = nextFrame;
        

		Begin3DMode();
        
        renderWorld(&renderedWorld, &renderingData);

        End3DMode();
    }

    CloseWindow();

    return 0;
}
