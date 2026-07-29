#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"
#include "math.h"
#include "world.h"

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
    World world = initializeEmptyWorld();

    pushPlane(&world, (Plane) {
        .texture = "debug_textures_0002",
        .x = 0,
        .y = 0,
        .z = 0,
        .yaw = 0,
        .pitch = 0,
        .roll = 0,
        .width = 20.0f,
        .height = 20.0f
    });

    pushBillboard(&world, (Billboard){
        .texture = "debug_entities_0001",
        .x = 0,
        .y = 0.5f,
        .z = 0,
        .size = 1.0f
    });


    world.camera.x = -5.0f;
    world.camera.y = 1.0f;


	// Main game loop
    while (!WindowShouldClose())
	{

		Begin3DMode();
        
        renderWorld(&world);

        End3DMode();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
