#include "gun.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

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
    },

    // 9 gauge shotgun shell
    {
        .spreadBase = 0.04,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.002,
        .spreadMultiplier = 1.0,

        .recoil = 0.05,
        .recoilMultiplier = 1.0,

        .damage = 0.6,
        .damageMultiplier = 1.0,

        .projectiles = 10.0,
        .projectilesMultiplier = 1.0,

        .firerate = 30,
        .firerateMultiplier = 1.0,

        .ammo = 5,
        .ammoMultiplier = 1,

        .reload = 30,
        .reloadMultiplier = 1,

        .velocity = 1.6,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = true,
            .projectileColor = {.r = 170, .g = 85, .b = 0, .a = 255},
            
            .changeCasingSprite = true,
            .bulletCasingSprite = "casing",
            
            .changeCasingSound = true,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = true,
            .firingSound = "shotgun"
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
    // default receiver
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
    },
    // submachine gun
    {
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.3,

        .spread = 0.0,
        .spreadMultiplier = 1.3,

        .recoil = 0.0,
        .recoilMultiplier = 0.75,

        .damage = 0.0,
        .damageMultiplier = 0.75,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = -3,
        .firerateMultiplier = 1.0,

        .ammo = 30,
        .ammoMultiplier = 1,

        .reload = -4,
        .reloadMultiplier = 1,

        .velocity = -0.2,
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

        .shootPitchMultiplier = 1.2,
        .shootVolumeMultiplier = 0.9,

        .casingPitchMultiplier = 1.1,
        .casingVolumeMultiplier = 0.9,

        .screenShake = 0,
        .screenShakeMultiplier = 1.0,
    },
    // machine gun
    {
        .spreadBase = 0.04,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.01,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 1.0,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 0.5,

        .ammo = 10,
        .ammoMultiplier = 2,

        .reload = 15,
        .reloadMultiplier = 1.5,

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

        .casingPitchMultiplier = 1.2,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 1.0,
    },
    // minigun
    {
        .spreadBase = 0.04,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.001,
        .spreadMultiplier = 1.0,

        .recoil = 0.01,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 0.5,

        .projectiles = 0.0,
        .projectilesMultiplier = 0.3,

        .firerate = 3,
        .firerateMultiplier = 0.25,

        .ammo = 20,
        .ammoMultiplier = 3,

        .reload = 15,
        .reloadMultiplier = 2,

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
        .shootVolumeMultiplier = 0.75,

        .casingPitchMultiplier = 1.3,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 0.5,
    },
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Receiver modifiers#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GunElement receiverModifiers[] = {
    // default receiver type
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
    },

    // short receiver type
    {
        .spreadBase = 0.01,
        .spreadBaseMultiplier = 1.1,

        .spread = 0.0001,
        .spreadMultiplier = 1.0,

        .recoil = 0.01,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 0.7,

        .projectiles = 0.0,
        .projectilesMultiplier = 0.8,

        .firerate = 0,
        .firerateMultiplier = 0.75,

        .ammo = 10,
        .ammoMultiplier = 1.1,

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

        .shootPitchMultiplier = 1.2,
        .shootVolumeMultiplier = 0.9,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 0.75,
    },

    // super short receiver
    {
        .spreadBase = 0.02,
        .spreadBaseMultiplier = 1.1,

        .spread = 0.0001,
        .spreadMultiplier = 1.4,

        .recoil = 0.01,
        .recoilMultiplier = 1.2,

        .damage = 0.0,
        .damageMultiplier = 0.5,

        .projectiles = 0.0,
        .projectilesMultiplier = 0.6,

        .firerate = 0,
        .firerateMultiplier = 0.5,

        .ammo = 20,
        .ammoMultiplier = 1.1,

        .reload = 0,
        .reloadMultiplier = 0.75,

        .velocity = 0,
        .velocityMultiplier = 0.95,

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

        .shootPitchMultiplier = 1.4,
        .shootVolumeMultiplier = 0.7,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 1,

        .screenShake = 0,
        .screenShakeMultiplier = 0.5,
    },
    // heavy receiver type
    {
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .damage = 0.0,
        .damageMultiplier = 1.25,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.3,

        .firerate = 0,
        .firerateMultiplier = 1.4,

        .ammo = 0,
        .ammoMultiplier = 0.9,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0.1,
        .velocityMultiplier = 1.2,

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

        .shootPitchMultiplier = 0.9,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 0.9,

        .screenShake = 0,
        .screenShakeMultiplier = 1.3,
    },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Magazine types#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GunElement magazineTypes[] = {
    // default magazine
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
    


    // firing speed underflow
    // if a weapon should fire more than once pre frame 

    
    return (Gun) {
        .minSpread = fmax((gun.minSpread + element.spreadBase) * element.spreadBaseMultiplier, 0),
        .spreadMultiplier = fmax((gun.spreadMultiplier + element.spread) * element.spreadMultiplier, 0),
        .recoilMultiplier = fmax((gun.recoilMultiplier + element.recoil) * element.recoilMultiplier, 0),
        .fireCooldown = fmax((gun.fireCooldown + element.firerate) * element.firerateMultiplier, 1),
        .bulletVelocity = fmax((gun.bulletVelocity + element.velocity) * element.velocityMultiplier, 0.1),
        .magazineSize = fmax((gun.magazineSize + element.ammo) * element.ammoMultiplier, 1),
        .reloadTime = fmax((gun.reloadTime + element.reload) * element.reloadMultiplier, 1),
        .projectilesPerShot = fmax((gun.projectilesPerShot + element.projectiles) * element.projectilesMultiplier, 1),
        .screenShake = fmax((gun.screenShake + element.screenShake) * element.screenShakeMultiplier, 0),
        .damage = fmax((gun.damage + element.damage) * element.damageMultiplier, 0.1),


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