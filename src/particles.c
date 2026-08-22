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

        this->light.radius = percentage * 255;
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
    float textureRotation,
    int usedFrames,
    int frameDuration,
    
    // lifetime
    int lifetime,
    bool fadeAway,

    // light
    EntityLight light
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
        .light = light,
        .type = ENTITY_UNSET,
        .color = WHITE,
        .textureRotation = textureRotation,
    }, &data,
        sizeof(ParticleData)
    );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Variation functions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* BLOOD_SPRITES[] = {
    "blood_0001", 
    "blood_0002", 
    "blood_0003",
    "blood_0004"
};
const int BLOOD_SPRITE_COUNT = 4;
void blood(GameState* state, Vector3 position, Vector3 direction, float speed){
    int sprite = GetRandomValue(0, BLOOD_SPRITE_COUNT - 1);
    
    float rotation = randomFloat(0, PI * 2);

    particle(
        state,
        position,
        direction,
        speed,
        0,              // speed decay
        0.1f,          // gravity

        // frames
        (char*[]){BLOOD_SPRITES[sprite]}, 
        16,             // texture width
        16,             // texture height
        rotation,       // texture rotation
        1,              // used frames
        10,             // frame duration 
        
        50,             // lifetime
        false,          // fade away

        emptyLight()    // particle light
    );
}


void bloodSplash(GameState* state, Vector3 origin, float amount){
    int count = ((int)min(pow(amount, 2), 50) + GetRandomValue(0, 5));
    
    
    float speedMin = 0.05;
    float speedMax = 0.15;


    for (int i = 0; i < count; ++i) {
        float horizontalRotation = randomFloat(0, PI * 2);
        float verticalSpeed = randomFloat(-0.3f, 1);

        Vector3 direction = Vector3Normalize((Vector3){cos(horizontalRotation), verticalSpeed, sin(horizontalRotation)});


        // 0.05 -> very small splash
        // 0.1 -> covers width of level
        float speed = randomFloat(speedMin, speedMax);

        blood(
            state,
            origin,
            direction,
            speed
        );
    }
}


void fadeParticle(
    GameState* state, 
    Vector3 position, 
    char* texture, 
    float textureWidth, 
    float textureHeight, 
    float textureRotation,
    int fadeTime, 
    EntityLight light
) {
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
        textureRotation,    // texture rotation
        1,                  // used frames
        10,                 // frame duration 
        
        fadeTime,           // lifetime
        true,               // fade away

        light               // particle light

    );
}



void muzzleFlash(GameState* state, Vector3 position) {
    fadeParticle(
        state, 
        position,
        "muzzle_flash",
        21,
        21,
        randomFloat(0, PI * 2),
        2,
        (EntityLight){
            .isLight = true,
            .radius = 15,
            .r = 0.9,
            .g = 0.7,
            .b = 0
        }
    );
}
