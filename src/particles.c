#include "gamesim.h"
#include "particles.h"
#include "entityUtils.h"
#include "math.h"
#include "raymath.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Base particle#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    Vector3 direction;
    // movement 
    float speed;
    float speedDecay;
    float gravity;
    
    // animation
    char* frames[8]; 
    int usedFrames;
    int frameDuration;
    
    // lifetime
    int lifetime;
    bool fadeAway;

    int internalTimer;
}ParticleData;

bool particleUpdate(Entity* this, GameState* state) {
    ParticleData* data = (ParticleData*) &this->data;

    // timer
    data->internalTimer++;

    // animation
    {
        int animationIndex = ((int)floor((double)data->internalTimer / data->frameDuration)) % data->usedFrames;


        this->texture = data->frames[animationIndex]; // results in a blank texture
    }

    // movement
    {
        Vector3 next = Vector3Add((Vector3) {this->x, this->y, this->z}, Vector3Scale(data->direction, data->speed));



        this->x = next.x;
        this->y = next.y;
        this->z = next.z;

        // gravity
        data->direction.y -= data->gravity;
        data->direction = Vector3Normalize(data->direction);
    }

    // fade
    if (data->fadeAway){
        float percentage = 1 - ((float)data->internalTimer / data-> lifetime);

        this->color.a = percentage * 255;
    }



    return data->internalTimer < data->lifetime;
}

void particle(
    GameState* state, 
    Vector3 position, 
    Vector3 direction,
    // movement 
    float speed, 
    float speedDecay,
    float gravity, 
    
    // animation
    char* frames[8], 
    int usedFrames,
    int frameDuration,
    
    // lifetime
    int lifetime,
    bool fadeAway
) {
    
    ParticleData data = {
        .direction = Vector3Normalize(direction),
        .speed = speed,
        .speedDecay = speedDecay,
        .gravity = gravity,
        .frames = {0},
        .usedFrames = usedFrames,
        .frameDuration = frameDuration,
        .lifetime = lifetime,
        .fadeAway = fadeAway,
        .internalTimer = 0
    };

    // copy frames
    for (int i = 0; i < usedFrames; ++i) {
        data.frames[i] = frames[i];
    }
    
    
    addEntity(state, (Entity){
        .texture = frames[0],
        .x = position.x,
        .y = position.y,
        .z = position.z,
        .width = 1.0f,
        .height = 1.0f,
        .textureWidth = 32.0f,
        .textureHeight = 32.0f,
        .textureOffsetX = 0,
        .textureOffsetY = 0,
        .update = &particleUpdate,
        .light = emptyLight(),
        .type = ENTITY_UNSET,
        .color = WHITE,
    }, &data,
        sizeof(ParticleData)
    );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Variation functions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void blood(GameState* state, Vector3 position, Vector3 direction, float speed){
    particle(
        state,
        position,
        direction,
        speed,
        0.1f,           // speed decay
        0.025f,         // gravity

        // frames
        (char*[]){"debug_textures_0002", "debug_textures_0005"}, 
        2,              // used frames
        10,             // frame duration 
        
        10,             // lifetime
        false           // fade away
    );
}
