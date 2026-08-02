#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"
#include "math.h"
#include "world.h"
#include "gamesim.h"
#include "entities.h"




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



	// Main game loop
    while (!WindowShouldClose())
	{

        GameState nextFrame = createNextFrame(&state);
        World renderedWorld = convertToWorld(&nextFrame);
        state = nextFrame;

		Begin3DMode();
        
        renderWorld(&renderedWorld, &renderingData);

        End3DMode();
    }

    CloseWindow();

    return 0;
}
