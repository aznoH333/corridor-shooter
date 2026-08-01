#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "raymath.h"
#include "math.h"
#include "world.h"
#include "gamesim.h"


typedef struct {
    int dir;
} Foo;

void foo(struct Entity* entity, struct GameState* state) {
    Foo* data = (Foo*)&entity->data;

    if (entity->z - (entity->depth * 0.5f) < -state->map.width * 0.5f) {
        data->dir = 1;
    } else if (entity->z + (entity->depth * 0.5f) > state->map.width * 0.5f) {
        data->dir = -1;
    }

    entity->z += 0.01f * data->dir;
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
    addEntity(&state, (Entity) {
        .texture = "debug_entities_0001",
        .x = 0.0f,
        .y = 0.5f,
        .z = 0.5f,
        .width = 1.0f,
        .height = 1.0f,
        .depth = 1.0f,
        .size = 1.0f,
        .update = &foo,
    }, &(Foo){.dir = -1}, sizeof(Foo));



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
