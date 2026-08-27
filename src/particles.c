#include "gamesim.h"
#include "particles.h"
#include "entityUtils.h"
#include "math.h"
#include "raymath.h"
#include "utils.h"
#include "vec3Utils.h"
#include "enemies.h"


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
    int lifeTime;
    int fadeAfter;
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
    int fadeAfter;
} ParticleSplatterData;


bool particleSplatterUpdate(Entity* this, GameState* state) {
    
    
    ParticleSplatterData* data = (ParticleSplatterData*) &this->data;

    data->lifeTime--;

    // set color
    float color = min(1, (float)data->lifeTime / data->fadeAfter);

    this->texture.color.a = color * 160;


    return data->lifeTime > 0;
}

void particleSplatter(
    GameState* state,
    Vector3 position,
    ParticleSplatter definition,
    float yaw,
    float pitch,
    float roll
) {
    
    
    EntityTexture texture = rotatedTextureFull(definition.texture, definition.width, definition.height, yaw, pitch, roll);
    texture.enableDepthMask = false;

    addEntity(state, (Entity){
        .texture = texture,
        .x = position.x,
        .y = position.y,
        .z = position.z,
        .width = 0.0f,
        .height = 0.0f,
        .update = &particleSplatterUpdate,
        .light = emptyLight(),
        .type = ENTITY_UNSET,
    }, 
    &(ParticleSplatterData){
        .lifeTime = definition.lifeTime,
        .fadeAfter = definition.fadeAfter,
    },
        sizeof(ParticleSplatterData)
    );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Particle bounce#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    bool enabled;
    float bounciness;
    char* bounceSound;
    bool useBounceSound;
    float minBounceForce;
    bool isFrozen;
    float rotationForce;
    float soundVolume;
    float soundPitch;
} ParticleBounce;


ParticleBounce noBounce() {
    return (ParticleBounce) {
        .enabled = false,
        .bounciness = 0,
        .bounceSound = "",
        .useBounceSound = false,
        .minBounceForce = 0,
        .isFrozen = false,
        .rotationForce = 0,
        .soundVolume = 0,
        .soundPitch = 0
    };
}


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
    float baseLightRadius;

    int internalTimer;

    ParticleSplatter splatter;
    ParticleBounce bounce;
}ParticleData;

bool particleUpdate(Entity* this, GameState* state) {
    ParticleData* data = (ParticleData*) &this->data;

    

    // animation
    {
        int animationIndex = ((int)floor((double)data->internalTimer / data->frameDuration)) % data->usedFrames;


        this->texture.texture = data->frames[animationIndex]; // results in a blank texture
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

        this->texture.pitch += data->bounce.rotationForce;
    }

    // fade
    if (data->fadeAway){
        float percentage = 1 - ((float)data->internalTimer / data-> lifetime);

        this->texture.color.a = percentage * 255;

        this->light.radius = percentage * data->baseLightRadius;
    }


    // world interaction
    if ((data->splatter.chance > 0 || data->bounce.enabled) && !data->bounce.isFrozen){
        Vector3 collision = checkWorldCollision(this->x, this->y, this->z, this->width, this->height, &state->map);
        
        float collided = Vector3Length(collision) > 0.1;

        if (collided) {
            
            
            if (randomChance(data->splatter.chance)) {
                Vector3 worldAllignedPosition = getClosestWorldPosition(&state->map, (Vector3) {this->x, this->y, this->z});
                
                // add a tiny offset to avoid z fighting
                
                float zFightOffset = randomFloat(0.01, 0.05);
                
                worldAllignedPosition.x -= collision.x * zFightOffset;
                worldAllignedPosition.y -= collision.y * zFightOffset;
                worldAllignedPosition.z -= collision.z * zFightOffset;


                float yaw = 0;//PI / 2 * collision.y;
                float roll = 0; // PI / 2 * collision.z;
                float pitch = 0;



                if (collision.z != 0) {
                    pitch = QUARTER_ROTATION * - collision.z;
                    yaw = randomFloat(0, FULL_ROTATION);
                }

                else if (collision.y != 0) {
                    yaw = randomFloat(0, FULL_ROTATION);// HALF_ROTATION * collision.y;
                }

                else if (collision.x != 0){
                    roll = QUARTER_ROTATION * collision.x;
                    pitch = randomFloat(0, FULL_ROTATION);

                }
                


                particleSplatter(
                    state, 
                    (Vector3) worldAllignedPosition, 
                    data->splatter,
                    yaw,
                    pitch,
                    roll 
                );
            }


            if (data->bounce.enabled) {
                // bounce


                data->direction.y *= 1 - (fabs(collision.y) * 2);
                data->direction.x *= 1 - (fabs(collision.x) * 2);
                data->direction.z *= 1 - (fabs(collision.z) * 2);
                data->bounce.rotationForce *= 1 - (fabs(collision.z) * 2);


                data->speed *= data->bounce.bounciness;
                

                if (data->bounce.useBounceSound) {
                    playSound(data->bounce.bounceSound, data->bounce.soundPitch, data->bounce.soundVolume);
                }

                // freeze
                if (data->speed < data->bounce.minBounceForce) {
                    data->bounce.isFrozen = true;
                    data->speed = 0;
                    data->splatter.chance = 0;
                    this->y = this->height * 0.5;
                    data->bounce.rotationForce = 0;
                }

            }

            return data->bounce.enabled;
        }




    }


    // timer
    data->internalTimer++;

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
    ParticleSplatter splatter,
    ParticleBounce bounce,
    float width,
    float height,
    bool enableDepthMask
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
        .baseLightRadius = light.radius,
        .bounce = bounce
    };

    // copy frames
    for (int i = 0; i < usedFrames; ++i) {
        data.frames[i] = frames[i];
    }
    
    EntityTexture texture = rotatedTexture(frames[0], textureWidth, textureHeight, textureRotation);
    texture.enableDepthMask = enableDepthMask;
    addEntity(state, (Entity){
        .texture = texture,
        .x = position.x,
        .y = position.y,
        .z = position.z,
        .width = width,
        .height = height,
        .update = &particleUpdate,
        .light = light,
        .type = ENTITY_UNSET,
        
    }, &data,
        sizeof(ParticleData)
    );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Blod and gore#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* BLOOD_SPRITES[] = {
    "blood_0001", 
    "blood_0002", 
    "blood_0003",
    "blood_0004"
};

const char* BLOOD_SPLATTER_TEXTURES[] = {
    "blood_splatter_0001", 
    "blood_splatter_0002", 
    "blood_splatter_0003",
    "blood_splatter_0004"
};

const int BLOOD_SPRITE_COUNT = 4;
const int BLOOD_SPLATTER_TEXTURE_COUNT = 4;
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
        (ParticleSplatter) { // splatter
            .chance = 0.5,
            .texture = BLOOD_SPLATTER_TEXTURES[GetRandomValue(0, BLOOD_SPLATTER_TEXTURE_COUNT - 1)],
            .width = 24,
            .height = 24,
            .lifeTime = GetRandomValue(6000, 8000),
            .fadeAfter = GetRandomValue(5000, 5500),
        },
        noBounce(),     // bounce
        0.25,           // width
        0.25,           // height
        true            // enable depth mask
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


const char* GORE_TEXTURES[] = {
    "gore_0001",
    "gore_0002",
    "gore_0003",
    "gore_0004"
};
const int GORE_TEXTURE_COUNT = 4;
void gore(GameState* state, Vector3 position, Vector3 direction, float speed) {
    int sprite = GetRandomValue(0, GORE_TEXTURE_COUNT - 1);
    
    float rotation = randomFloat(0, PI * 2);

    particle(
        state,
        position,
        direction,
        speed,
        0,              // speed decay
        0.15f,           // gravity

        // frames
        (char*[]){GORE_TEXTURES[sprite]}, 
        16,             // texture width
        16,             // texture height
        rotation,       // texture rotation
        1,              // used frames
        10,             // frame duration 
        
        600,             // lifetime
        true,            // fade away

        emptyLight(),   // particle light
        (ParticleSplatter) { // splatter
            .chance = 0.75f,
            .texture = BLOOD_SPLATTER_TEXTURES[GetRandomValue(0, BLOOD_SPLATTER_TEXTURE_COUNT - 1)],
            .width = 24,
            .height = 24,
            .lifeTime = GetRandomValue(6000, 8000),
            .fadeAfter = GetRandomValue(5000, 5500),
        },
        (ParticleBounce) {  // bounce
            .enabled = true,
            .bounciness = 0.25,
            .bounceSound = "flesh_impact_fast",
            .useBounceSound = true,
            .minBounceForce = 0.02,
            .isFrozen = false,
            .rotationForce = randomFloat(0.02, 0.08) * direction.z,
            .soundVolume = 0.25,
            .soundPitch = 1.2
        },
        0.25,           // width
        0.25,           // height
        false           // enable depth mask
    );
}


void goreExplosion(GameState* state, Vector3 position, float amount) {
    bloodSplash(state, position, amount);

    int count = ((int)min(amount, 10) + GetRandomValue(0, 2));
    
    
    float speedMin = 0.05;
    float speedMax = 0.15;


    for (int i = 0; i < count; ++i) {
        float horizontalRotation = randomFloat(0, PI * 2);
        float verticalSpeed = randomFloat(-0.2f, 3);

        Vector3 direction = Vector3Normalize((Vector3){cos(horizontalRotation), verticalSpeed, sin(horizontalRotation)});


        // 0.05 -> very small splash
        // 0.1 -> covers width of level
        float speed = randomFloat(speedMin, speedMax);

        gore(
            state,
            position,
            direction,
            speed
        );
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Fade particle#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        noSplatter(),       // splatter
        noBounce(),         // bounce
        0.25,               // width
        0.25,               // height
        true                // enable depth mask
    );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Gun effects#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            .radius = 50,
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
        0.1,              // speed
        0,                  // speed decay
        0.28,               // gravity

        (char*[]){"casing"},// frames
        3,                  // texture width
        3,                  // texture height
        rotation,           // texture rotation
        1,                  // used frames
        10,                 // frame duration 
        
        600,                // lifetime
        true,               // fade away
        emptyLight(),       // particle light
        noSplatter(),
        (ParticleBounce) {  // bounce
            .enabled = true,
            .bounciness = 0.5,
            .bounceSound = "shell_bounce",
            .useBounceSound = true,
            .minBounceForce = 0.02,
            .isFrozen = false,
            .rotationForce = 0.05,
            .soundVolume = 0.25,
            .soundPitch = 1
        },
        0.1,                // width
        0.1,                // height
        false               // enable depth mask
    );
}
