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
    float x = 0.0f;
    float y = 2.0f;
    float z = 0.0f;

    float CUBE_SIZE = 100.0f;
    float SIDE_OFFSET = CUBE_SIZE / 2.0f;

    float HALF_PI = (PI / 2.0f) - 0.05f;


    while (!WindowShouldClose())
	{

        if (IsKeyDown(KEY_A)) {
            yaw -= 0.02f;
        }

        if (IsKeyDown(KEY_D)) {
            yaw += 0.02f;
        }

        if (IsKeyDown(KEY_S)) {
            pitch -= 0.02f;
        }

        if (IsKeyDown(KEY_W)) {
            pitch += 0.02f;
        }


        if (IsKeyDown(KEY_UP)) {
            x += cos(yaw) * 0.1f;
            z += sin(yaw) * 0.1f;
        }

        if (IsKeyDown(KEY_DOWN)) {
            x -= cos(yaw) * 0.1f;
            z -= sin(yaw) * 0.1f;
        }

        if (IsKeyDown(KEY_LEFT)) {
            x -= sin(-yaw) * 0.1f;
            z -= cos(-yaw) * 0.1f;
        }

        if (IsKeyDown(KEY_RIGHT)) {
            x += sin(-yaw) * 0.1f;
            z += cos(-yaw) * 0.1f;
        }



        pitch = Clamp(pitch, -HALF_PI, HALF_PI);

		drawText("empty window", 15, 40, 100, WHITE);
		Begin3DMode();

        
        plane("debug_textures_0002", 0.0f,  0.0f,        0.0f, CUBE_SIZE, CUBE_SIZE, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0005", 0.0f,  CUBE_SIZE,   0.0f, CUBE_SIZE, CUBE_SIZE, 0.0f, PI, 0.0f);



        plane("debug", 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, yaw, pitch, 0.0f);
        billboard("debug_entities_0001", 0.0f, 0.5f, 0.0f, 1.0f);


        plane("debug_textures_0005", SIDE_OFFSET,   SIDE_OFFSET, 0.0f,          CUBE_SIZE, CUBE_SIZE, 0.0f, PI / 2.0f, PI / 2.0f);
        plane("debug_textures_0005", -SIDE_OFFSET,  SIDE_OFFSET, 0.0f,          CUBE_SIZE, CUBE_SIZE, 0.0f, PI / 2.0f, -PI / 2.0f);

        plane("debug_textures_0005", 0.0f,          SIDE_OFFSET, SIDE_OFFSET,   CUBE_SIZE, CUBE_SIZE, 0.0f, PI / 2.0f, -PI);
        plane("debug_textures_0005", 0.0f,          SIDE_OFFSET, -SIDE_OFFSET,  CUBE_SIZE, CUBE_SIZE, 0.0f, PI / 2.0f, 0.0f);



        setCamera(x, y, z, yaw, pitch);


        End3DMode();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
