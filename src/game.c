#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"
#include "math.h"
#include "world.h"
#include "gamesim.h"
#include "entities.h"
#include "enemies.h"



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

    // setup world
    WorldRenderingData renderingData = prepareWorldRenderingData();
    GameState state = initEmptyGame();
    player(&state, 0, 0, 0);
    playMusic("ambience", 1);
    //dummy(&state, 20, 0, 0);
    //SwitchResolution(0, 0, true);

    spawnEnemy(&state, (Vector3){20, 0, 0}, 0);
    /*
    spawnEnemy(&state, (Vector3){20, 0, 1}, 0);
    spawnEnemy(&state, (Vector3){20, 0, -1}, 0);
    spawnEnemy(&state, (Vector3){25, 0, 0}, 0);
    spawnEnemy(&state, (Vector3){25, 0, 1}, 0);
    spawnEnemy(&state, (Vector3){25, 0, -1}, 0);
    spawnEnemy(&state, (Vector3){30, 0, 0}, 0);
    spawnEnemy(&state, (Vector3){30, 0, 1}, 0);
    spawnEnemy(&state, (Vector3){30, 0, -1}, 0);
    */

    HideCursor();

	// Main game loop
    while (!WindowShouldClose())
	{

        // temporary fullscreen hack
        if (IsKeyPressed(KEY_P)) {
            SwitchResolution(0, 0, true);
        }
        GameState nextFrame = createNextFrame(&state);
        World renderedWorld = convertToWorld(&state);
        state = nextFrame;

		Begin3DMode();
        
        renderWorld(&renderedWorld, &renderingData);

        End3DMode();
    }

    CloseWindow();

    return 0;
}
