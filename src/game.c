#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"
#include "math.h"
#include "world.h"
#include "gamesim.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------



void cube(char* texture, float x, float y, float z, float size) {

    float sideOffset = size / 2.0f;

    plane(texture, x,               y - sideOffset,         z,          size, size, 0.0f, PI, 0.0f);
    plane(texture, x,               y + sideOffset,         z,          size, size, 0.0f, 0.0f, 0.0f);
    plane(texture, x + sideOffset,  y,         z,          size, size, 0.0f, PI / 2.0f, -PI / 2.0f);
    plane(texture, x - sideOffset,  y,         z,          size, size, 0.0f, PI / 2.0f, PI / 2.0f);
    plane(texture, x,               y,         z + sideOffset,   size, size, 0.0f, -PI / 2.0f, -PI);
    plane(texture, x,               y,         z - sideOffset,  size, size, 0.0f, -PI / 2.0f, 0.0f);

}

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
    
    GameState state = initEmptyGame();


	// Main game loop
    while (!WindowShouldClose())
	{

        GameState nextFrame = createNextFrame(&state);
        World renderedWorld = convertToWorld(&nextFrame);
        state = nextFrame;

		Begin3DMode();
        
        renderWorld(&renderedWorld);

        End3DMode();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
