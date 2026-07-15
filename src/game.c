#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"

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

    Mesh plane = GenMeshPlane(10.0f, 10.0f, 1, 1);

    Material material = LoadMaterialDefault();
    SetMaterialTexture(&material, MATERIAL_MAP_DIFFUSE, LoadTexture("./resources/textures/debug.png"));

    // void SetMaterialTexture(Material *material, int mapType, Texture2D texture);
    // void DrawMesh(Mesh mesh, Material material, Matrix transform);  

    Matrix matrix = {0};

    // MatrixRotateXYZ((Vector3){ DEG2RAD*pitch, DEG2RAD*yaw, DEG2RAD*roll });

    float yaw = 0.0f;
    float pitch = 0.0f;

    while (!WindowShouldClose())
	{
		yaw += 1.0f;
        pitch += 2.0f;
        matrix = MatrixRotateXYZ((Vector3){ DEG2RAD*pitch, DEG2RAD*yaw, 0.0f });

		drawText("empty window", 15, 40, 1, WHITE);
		Begin3DMode();

        DrawMesh(plane, material, matrix);
        
        //DrawCube((Vector3){ 0, 0, 0 }, 1.0f, 1.0f, 1.0f, RED);
        //DrawCubeWires((Vector3){ 0, 0, 0 }, 1.0f, 1.0f, 1.0f, MAROON);

        End3DMode();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
