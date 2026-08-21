#include "gamesim.h"
#include "particles.h"
#include "entityUtils.h"
#include "math.h"
#include "raymath.h"
#include "utils.h"

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

        // speed decay
        data->speed = max(data->speed - data->speedDecay, 0);
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
    float textureWidth,
    float textureHeight,
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
        .textureWidth = textureWidth,
        .textureHeight = textureHeight,
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

const char* BLOOD_SPRITES[] = {"debug_textures_0002", "debug_textures_0005", "debug_entities_0001"};
const int BLOOD_SPRITE_COUNT = 3;
void blood(GameState* state, Vector3 position, Vector3 direction, float speed){
    int sprite = GetRandomValue(0, BLOOD_SPRITE_COUNT - 1);
    
    particle(
        state,
        position,
        direction,
        speed,
        0,              // speed decay
        0.05f,          // gravity

        // frames
        (char*[]){BLOOD_SPRITES[sprite]}, 
        32,             // texture width
        32,             // texture height
        1,              // used frames
        10,             // frame duration 
        
        50,             // lifetime
        false           // fade away
    );
}


void bloodSplash(GameState* state, Vector3 origin, float amount){
    int count = (int)min(pow(amount, 2), 30) + GetRandomValue(0, 5);
    float speedMin = amount * 0.005;
    float speedMax = (amount + 30) * 0.005;


    for (int i = 0; i < count; ++i) {
        float horizontalRotation = randomFloat(0, PI * 2);
        float verticalSpeed = randomFloat(-0.3f, 1);

        Vector3 direction = Vector3Normalize((Vector3){cos(horizontalRotation), verticalSpeed, sin(horizontalRotation)});

        float speed = randomFloat(speedMin, speedMax);

        blood(
            state,
            origin,
            direction,
            speed
        );
    }
}


void fadeParticle(GameState* state, Vector3 position, char* texture, float textureWidth, float textureHeight, int fadeTime) {
    particle(
        state,              // gamestate
        position,           // position
        (Vector3){0,0,0},   // direction
        0,                  // speed
        0,                  // speed decay
        0,                  // gravity

        (char*[]){texture}, // frames
        textureWidth,       // texture width
        textureHeight,      // texture height
        1,                  // used frames
        10,                 // frame duration 
        
        fadeTime,           // lifetime
        true                // fade away
    );
}
