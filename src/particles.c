#include "gamesim.h"
#include "particles.h"
#include "entityUtils.h"
#include "math.h"
#include "raymath.h"
#include "utils.h"
#include "vec3Utils.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ParticleSplatter#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    float chance;
    char* texture;
    float width;
    float height;
} ParticleSplatter;


ParticleSplatter noSplatter() {
    return (ParticleSplatter){
        .chance = 0,
        .texture = 0,
        .width = 0,
        .height = 0
    };
}




typedef struct {
    int lifeTime;
} ParticleSplatterData;


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

    ParticleSplatter splatter;
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
    EntityLight light,
    ParticleSplatter splatter
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
        .internalTimer = 0,
        .splatter = splatter,
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

        emptyLight(),   // particle light
        noSplatter()    // splatter
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

        light,              // particle light
        noSplatter()        // splatter
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


void smokePuff(GameState* state, Vector3 position){
    fadeParticle(
        state, 
        position,
        "smoke_puff_0001",
        7,
        7,
        randomFloat(0, PI * 2),
        30,
        emptyLight()
    );
}


void bloodPuff(GameState* state, Vector3 position){
    fadeParticle(
        state, 
        position,
        "smoke_puff_0002",
        7,
        7,
        randomFloat(0, PI * 2),
        20,
        emptyLight()
    );
}


void bulletCasing(GameState* state, Vector3 position, Vector3 baseDirection){
    
    
    
    Vector3 casingDirection = Vector3Normalize(Vector3Add((Vector3){.x = 0, .y = 0.5, .z = 0}, baseDirection)); 

    casingDirection = Vector3Rotate(baseDirection, -PI / 2, PI / 4, 0);

    Vector3 directionRandomness = Vector3Normalize((Vector3){
        .x = randomFloat(-1, 1),
        .y = randomFloat(-1, 1),
        .z = randomFloat(-1, 1)
    });

    casingDirection = Vector3Add(casingDirection, Vector3Scale(directionRandomness, 0.1));



    float rotation = randomFloat(0, PI * 2);
    
    particle(
        state,              // gamestate
        position,           // position
        casingDirection,    // direction
        0.1,                // speed
        0,                  // speed decay
        0.14,                // gravity

        (char*[]){"casing"},// frames
        3,                  // texture width
        3,                  // texture height
        rotation,           // texture rotation
        1,                  // used frames
        10,                 // frame duration 
        
        30,                 // lifetime
        false,              // fade away
        emptyLight(),       // particle light
        noSplatter()
    );
}
