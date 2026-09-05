#ifndef GUN
#define GUN
#include "raylib.h"


typedef struct {

    // gameplay values
    float minSpread;
    float spreadMultiplier;
    float recoilMultiplier;
    float damage;
    int fireCooldown;
    float bulletVelocity;
    int projectilesPerShot;
    int screenShake;

    // magazine
    int magazineSize;
    int reloadTime;


    // firing sound
    char* firingSound;
    float firingSoundPitch;
    float firingSoundVolume;
    Color bulletColor;

    // bullet casing
    char* bulletCasingTexture;
    char* bulletCasingSound; // plays when the casing bounces of the ground
    float bulletCasingSoundPitch;
    float bulletCasingSoundVolume;
} Gun;






Gun gun(
    int bulletType,       // defines the projectile type (9mm, 9gauge, ect)
    int bulletModifier,   // variant of bullet (normal, incendiary, armor penetrating)
    int receiverType,     // how the gun behaves (gameplay stereotypes like : smg, heavy machine gun, single shot, semi auto, ect)
    int receiverModifier, // just to shuffle the stats around a bit (small, medium, large)
    int magazineType      // used to determine the guns ammo count (small, normal, drum)
);

#endif