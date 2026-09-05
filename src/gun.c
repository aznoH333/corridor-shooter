#include "gun.h"
#include "stdbool.h"
#include "string.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Element definitions#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    
    bool changeColor;
    Color projectileColor;

    bool changeCasingSprite;
    char* bulletCasingSprite;

    bool changeCasingSound;
    char* bulletCasingSound;

    bool changeFiringSound;
    char* firingSound;

} BulletVisuals;


BulletVisuals noVisualChange() {
    return (BulletVisuals) {
        .changeColor = false,
        .projectileColor = WHITE,
        
        .changeCasingSprite = false,
        .bulletCasingSprite = NULL,
        
        .changeCasingSound = false,
        .bulletCasingSound = NULL,
        
        .changeFiringSound = false,
        .firingSound = NULL
    };
}


typedef struct { 
    // each attribute has 2 values
    // the first one is added to the baseline (usually 0 or 1)
    // the second is used as a multiplier for the first
    // the multipliers are applied sequentialy

    // spread base
    float spreadBase;
    float spreadBaseMultiplier;

    // spread
    float spread;
    float spreadMultiplier;

    // recoil
    float recoil;
    float recoilMultiplier;

    // damage
    float damage;
    float damageMultiplier;

    // projectiles
    float projectiles;
    float projectilesMultiplier;

    // fire rate
    float firerate;
    float firerateMultiplier;

    // ammo
    float ammo;
    float ammoMultiplier;

    // reload time
    float reload;
    float reloadMultiplier;

    // bullet velocity
    float velocity;
    float velocityMultiplier;

    BulletVisuals bulletVisuals;

    // sounds
    float shootPitchMultiplier;
    float shootVolumeMultiplier;

    float casingPitchMultiplier;
    float casingVolumeMultiplier;

    // screen shake
    float screenShake;
    float screenShakeMultiplier;

} GunElement;




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Bullet types#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GunElement bulletTypes[] = {
    // 9mm round
    {
        .spreadBase = 0.01,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.002,
        .spreadMultiplier = 1.0,

        .recoil = 0.01,
        .recoilMultiplier = 1.0,

        .damage = 1.0,
        .damageMultiplier = 1.0,

        .projectiles = 1.0,
        .projectilesMultiplier = 1.0,

        .firerate = 10,
        .firerateMultiplier = 1.0,

        .ammo = 10,
        .ammoMultiplier = 1,

        .reload = 30,
        .reloadMultiplier = 1,

        .velocity = 1.7,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = true,
            .projectileColor = {.r = 255, .g = 183, .b = 74, .a = 255},
            
            .changeCasingSprite = true,
            .bulletCasingSprite = "casing",
            
            .changeCasingSound = true,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = true,
            .firingSound = "machine_gun_2"
        },

        .shootPitchMultiplier = 1,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 0.25,

        .screenShake = 3,
        .screenShakeMultiplier = 1.0,
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Bullet modifiers#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GunElement bulletModifiers[] = {
    // default round
    {
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 1.0,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 1.0,

        .ammo = 0,
        .ammoMultiplier = 1,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = false,
            .projectileColor = {.r = 255, .g = 183, .b = 74, .a = 255},
            
            .changeCasingSprite = false,
            .bulletCasingSprite = "casing",
            
            .changeCasingSound = false,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = false,
            .firingSound = "machine_gun_2"
        },

        .shootPitchMultiplier = 1,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 1.0,
    }
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Receiver types#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GunElement receiverTypes[] = {
    // default round
    {
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 1.0,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 1.0,

        .ammo = 0,
        .ammoMultiplier = 1,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = false,
            .projectileColor = {.r = 255, .g = 183, .b = 74, .a = 255},
            
            .changeCasingSprite = false,
            .bulletCasingSprite = "casing",
            
            .changeCasingSound = false,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = false,
            .firingSound = "machine_gun_2"
        },

        .shootPitchMultiplier = 1,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 1.0,
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Receiver modifiers#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GunElement receiverModifiers[] = {
    // default round
    {
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 1.0,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 1.0,

        .ammo = 0,
        .ammoMultiplier = 1,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = false,
            .projectileColor = {.r = 255, .g = 183, .b = 74, .a = 255},
            
            .changeCasingSprite = false,
            .bulletCasingSprite = "casing",
            
            .changeCasingSound = false,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = false,
            .firingSound = "machine_gun_2"
        },

        .shootPitchMultiplier = 1,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 1.0,
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Magazine types#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GunElement magazineTypes[] = {
    // default round
    {
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 1.0,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 1.0,

        .ammo = 0,
        .ammoMultiplier = 1,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = false,
            .projectileColor = {.r = 255, .g = 183, .b = 74, .a = 255},
            
            .changeCasingSprite = false,
            .bulletCasingSprite = "casing",
            
            .changeCasingSound = false,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = false,
            .firingSound = "machine_gun_2"
        },

        .shootPitchMultiplier = 1,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 1.0,
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Gun assembly#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Gun applyGunElement(Gun gun, GunElement element) {
    
    
    char* firingSound = gun.firingSound;
    if (element.bulletVisuals.changeFiringSound) {
        firingSound = element.bulletVisuals.firingSound;
    }

    Color bulletColor = gun.bulletColor;
    if (element.bulletVisuals.changeColor) {
        bulletColor = element.bulletVisuals.projectileColor;
    }

    char* casing = gun.bulletCasingTexture;
    if (element.bulletVisuals.changeCasingSprite) {
        casing = element.bulletVisuals.bulletCasingSprite;
    }

    char* casingSound = gun.bulletCasingSound;
    if (element.bulletVisuals.changeCasingSound) {
        casingSound = element.bulletVisuals.bulletCasingSound;
    }
    
    
    return (Gun) {
        .minSpread = (gun.minSpread + element.spreadBase) * element.spreadBaseMultiplier,
        .spreadMultiplier = (gun.spreadMultiplier + element.spread) * element.spreadMultiplier,
        .recoilMultiplier = (gun.recoilMultiplier + element.recoil) * element.recoilMultiplier,
        .fireCooldown = (gun.fireCooldown + element.firerate) * element.firerateMultiplier,
        .bulletVelocity = (gun.bulletVelocity + element.velocity) * element.velocityMultiplier,
        .magazineSize = (gun.magazineSize + element.ammo) * element.ammoMultiplier,
        .reloadTime = (gun.reloadTime + element.reload) * element.reloadMultiplier,
        .projectilesPerShot = (gun.projectilesPerShot + element.projectiles) * element.projectilesMultiplier,
        .screenShake = (gun.screenShake + element.screenShake) * element.screenShakeMultiplier,
        .damage = (gun.damage + element.damage) * element.damageMultiplier,


        // firing sound
        .firingSound = firingSound,
        .firingSoundPitch = gun.firingSoundPitch * element.shootPitchMultiplier,
        .firingSoundVolume = gun.firingSoundVolume * element.shootVolumeMultiplier,
        .bulletColor = bulletColor,

        // bullet casing
        .bulletCasingTexture = casing,
        .bulletCasingSound = casingSound, // plays when the casing bounces of the ground
        .bulletCasingSoundPitch = gun.bulletCasingSoundPitch * element.casingPitchMultiplier,
        .bulletCasingSoundVolume = gun.bulletCasingSoundVolume * element.casingVolumeMultiplier
    };
}




Gun gun(
    int bulletType,       // defines the projectile type (9mm, 9gauge, ect)
    int bulletModifier,   // variant of bullet (normal, incendiary, armor penetrating)
    int receiverType,     // how the gun behaves (gameplay stereotypes like : smg, heavy machine gun, single shot, semi auto, ect)
    int receiverModifier, // just to shuffle the stats around a bit (small, medium, large)
    int magazineType      // used to determine the guns ammo count (small, normal, drum)
) {
    // init empty gun
    
    Gun gun = { 
        .minSpread = 0,
        .spreadMultiplier = 0,
        .recoilMultiplier = 0,
        .fireCooldown = 0,
        .bulletVelocity = 0,
        .magazineSize = 0,
        .reloadTime = 0,
        .projectilesPerShot = 0,
        .screenShake = 0,
        .damage = 0,


        // firing sound
        .firingSound = "machine_gun_2",
        .firingSoundPitch = 1,
        .firingSoundVolume = 1,
        .bulletColor = WHITE,

        // bullet casing
        .bulletCasingTexture = "casing",
        .bulletCasingSound = "shell_bounce", // plays when the casing bounces of the ground
        .bulletCasingSoundPitch = 1,
        .bulletCasingSoundVolume = 1
    };

    // apply modifiers
    gun = applyGunElement(gun, bulletTypes[bulletType]);
    gun = applyGunElement(gun, bulletModifiers[bulletModifier]);
    gun = applyGunElement(gun, receiverTypes[receiverType]);
    gun = applyGunElement(gun, receiverModifiers[receiverModifier]);
    gun = applyGunElement(gun, magazineTypes[magazineType]);

    
    
    return gun;
}
// TODO : gun generation here