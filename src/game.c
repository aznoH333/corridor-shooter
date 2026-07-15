#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"
#include "math.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	SetTraceLogLevel(LOG_WARNING); 
    const int screenWidth = 800;
    const int screenHeight = 400;

	InitTextureWindow(screenWidth, screenHeight, 1920, 1080, "empty project");	
	UseShader("./resources/shaders/shaderVert.vs", "./resources/shaders/shaderFrag.fs");
    SetTargetFPS(60);
	// Main game loop


    float yaw = 0.0f;
    float pitch = 0.0f;

    while (!WindowShouldClose())
	{
		yaw += 0.01f;
        pitch += 0.02f;

		drawText("empty window", 15, 40, 100, WHITE);
		Begin3DMode();

        
        plane("debug_textures_0002", 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);


        plane("debug_textures_0002", 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0002", -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0002", 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0002", 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug", 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, yaw, pitch, 0.0f);


        plane("debug_textures_0005", 5.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, PI / 2.0f, PI / 2.0f);
        plane("debug_textures_0005", -5.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, PI / 2.0f, -PI / 2.0f);



        End3DMode();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
