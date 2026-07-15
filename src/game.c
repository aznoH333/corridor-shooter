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

    // Mesh plane = GenMeshPlane(10.0f, 10.0f, 1, 1);


    // void SetMaterialTexture(Material *material, int mapType, Texture2D texture);
    // void DrawMesh(Mesh mesh, Material material, Matrix transform);  

    // Matrix matrix = {0};

    // MatrixRotateXYZ((Vector3){ DEG2RAD*pitch, DEG2RAD*yaw, DEG2RAD*roll });

    float yaw = 0.0f;
    float pitch = 0.0f;

    while (!WindowShouldClose())
	{
		yaw += 0.01f;
        pitch += 0.02f;

		drawText("empty window", 15, 40, 100, WHITE);
		Begin3DMode();

        // DrawMesh(plane, material, matrix);
        
        plane("debug", 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);


        plane("debug_textures_0002", 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0002", -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0002", 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0002", 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        plane("debug_textures_0002", 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);


        plane("debug_textures_0005", 5.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, PI / 2.0f, PI / 2.0f);
        plane("debug_textures_0005", -5.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, PI / 2.0f, -PI / 2.0f);







        //DrawCubeWires((Vector3){ 0, 0, 0 }, 1.0f, 1.0f, 1.0f, MAROON);

        End3DMode();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
