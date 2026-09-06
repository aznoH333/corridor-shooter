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

    // weight
    float weight;
    float weightMultiplier;

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
    { // 9mm round
        .spreadBase = 0.01,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.002,
        .spreadMultiplier = 1.0,

        .recoil = 0.01,
        .recoilMultiplier = 1.0,

        .weight = 10,
        .weightMultiplier = 1,

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
            .bulletCasingSprite = "casing_0001",
            
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
    
    { // 9 gauge shotgun shell
        .spreadBase = 0.04,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.002,
        .spreadMultiplier = 1.0,

        .recoil = 0.05,
        .recoilMultiplier = 1.0,

        .weight = 20,
        .weightMultiplier = 1,

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
            .bulletCasingSprite = "casing_0002",
            
            .changeCasingSound = true,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = true,
            .firingSound = "shotgun"
        },

        .shootPitchMultiplier = 1,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 0.25,

        .screenShake = 40,
        .screenShakeMultiplier = 1.0,
    },

    { // 9 gauge slug shell
        .spreadBase = 0.02,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.01,
        .spreadMultiplier = 1.0,
        
        .weight = 20,
        .weightMultiplier = 1,

        .recoil = 0.05,
        .recoilMultiplier = 1.0,

        .damage = 4,
        .damageMultiplier = 1.0,

        .projectiles = 1.0,
        .projectilesMultiplier = 1.0,

        .firerate = 30,
        .firerateMultiplier = 1.0,

        .ammo = 5,
        .ammoMultiplier = 1,

        .reload = 30,
        .reloadMultiplier = 1,

        .velocity = 1.7,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = true,
            .projectileColor = {.r = 140, .g = 140, .b = 140, .a = 255},
            
            .changeCasingSprite = true,
            .bulletCasingSprite = "casing_0003",
            
            .changeCasingSound = true,
            .bulletCasingSound = "shell_bounce",
            
            .changeFiringSound = true,
            .firingSound = "shotgun"
        },

        .shootPitchMultiplier = 1,
        .shootVolumeMultiplier = 1,

        .casingPitchMultiplier = 1,
        .casingVolumeMultiplier = 0.25,

        .screenShake = 40,
        .screenShakeMultiplier = 1.0,
    },
    
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Bullet modifiers#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GunElement bulletModifiers[] = {
    { // default round
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,
        
        .weight = 0,
        .weightMultiplier = 1,

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
    
    { // piercing round
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.2,
        
        .weight = 0,
        .weightMultiplier = 1,

        .damage = 0.0,
        .damageMultiplier = 1.2,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 1.2,

        .ammo = 0,
        .ammoMultiplier = 0.8,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0,
        .velocityMultiplier = 1.0,

        .bulletVisuals = {
            .changeColor = true,
            .projectileColor = {.r = 255, .g = 255, .b = 255, .a = 255},
            
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
    
    { // triplicate round
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.3333,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .weight = 0,
        .weightMultiplier = 1,

        .damage = 0.0,
        .damageMultiplier = 0.4,

        .projectiles = 0.0,
        .projectilesMultiplier = 3.0,

        .firerate = 0,
        .firerateMultiplier = 1.0,

        .ammo = 0,
        .ammoMultiplier = 1,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0,
        .velocityMultiplier = 0.75,

        .bulletVisuals = {
            .changeColor = true,
            .projectileColor = {.r = 255, .g = 255, .b = 85, .a = 180},
            
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
    { // incendiary round // TODO incendiary effect
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .weight = 0,
        .weightMultiplier = 1,

        .damage = 0.0,
        .damageMultiplier = 0.75,

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
            .changeColor = true,
            .projectileColor = {.r = 170, .g = 54, .b = 0, .a = 255},
            
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

    
    { // stun round // TODO incendiary effect
        .spreadBase = 0.0,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 1.0,

        .recoil = 0.0,
        .recoilMultiplier = 1.0,

        .weight = 0,
        .weightMultiplier = 1,

        .damage = 0.0,
        .damageMultiplier = 0.75,

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
            .changeColor = true,
            .projectileColor = {.r = 85, .g = 85, .b = 255, .a = 255},
            
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

        .weight = 20,
        .weightMultiplier = 1,

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

        .weight = 15,
        .weightMultiplier = 1,

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

        .weight = 35,
        .weightMultiplier = 1,

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

        .weight = 50,
        .weightMultiplier = 1,

        .damage = 0.0,
        .damageMultiplier = 1.0,

        .projectiles = 0.0,
        .projectilesMultiplier = 1,

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
    // rifle
    {
        .spreadBase = -0.002,
        .spreadBaseMultiplier = 1.0,

        .spread = 0.0,
        .spreadMultiplier = 0.8,

        .recoil = 0.001,
        .recoilMultiplier = 1.0,

        .weight = 25,
        .weightMultiplier = 1,

        .damage = 3.0,
        .damageMultiplier = 1.2,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 1.1,

        .ammo = 0,
        .ammoMultiplier = 0.85,

        .reload = 0,
        .reloadMultiplier = 1,

        .velocity = 0,
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

        .casingPitchMultiplier = 0.9,
        .casingVolumeMultiplier = 1,

        .screenShake = 3,
        .screenShakeMultiplier = 1.0,
    },

    // sniper rifle
    {
        .spreadBase = -0.002,
        .spreadBaseMultiplier = 0.8,

        .spread = 0.0,
        .spreadMultiplier = 0.7,

        .recoil = 0.01,
        .recoilMultiplier = 1.1,

        .weight = 30,
        .weightMultiplier = 1,

        .damage = 0.0,
        .damageMultiplier = 5,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

        .firerate = 0,
        .firerateMultiplier = 4,

        .ammo = -2,
        .ammoMultiplier = 0.3,

        .reload = 0,
        .reloadMultiplier = 1.2,

        .velocity = 0,
        .velocityMultiplier = 1.6,

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

        .shootPitchMultiplier = 0.5,
        .shootVolumeMultiplier = 1.2,

        .casingPitchMultiplier = 0.5,
        .casingVolumeMultiplier = 2,

        .screenShake = 10,
        .screenShakeMultiplier = 1.0,
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

        .weight = 0,
        .weightMultiplier = 1,

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

        .weight = 0,
        .weightMultiplier = 0.8,

        .damage = 0.0,
        .damageMultiplier = 0.7,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

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

        .weight = 0,
        .weightMultiplier = 0.7,

        .damage = 0.0,
        .damageMultiplier = 0.5,

        .projectiles = 0.0,
        .projectilesMultiplier = 1.0,

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

        .weight = 0,
        .weightMultiplier = 1.3,

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

        .weight = 10,
        .weightMultiplier = 1,

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
        .minSpread = fmax((gun.minSpread + element.spreadBase) * element.spreadBaseMultiplier, 0),
        .spreadMultiplier = fmax((gun.spreadMultiplier + element.spread) * element.spreadMultiplier, 0),
        .recoilMultiplier = fmax((gun.recoilMultiplier + element.recoil) * element.recoilMultiplier, 0),
        
        
        .gunWeight = (gun.gunWeight + element.weight) * element.weightMultiplier,
        
        
        
        .fireCooldown = fmax((gun.fireCooldown + element.firerate) * element.firerateMultiplier, 0.1),
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
        .bulletCasingSoundVolume = 1,

        // aiming
        .gunWeight = 0,
    };

    // apply modifiers
    gun = applyGunElement(gun, bulletTypes[bulletType]);
    gun = applyGunElement(gun, bulletModifiers[bulletModifier]);
    gun = applyGunElement(gun, receiverTypes[receiverType]);
    gun = applyGunElement(gun, receiverModifiers[receiverModifier]);
    gun = applyGunElement(gun, magazineTypes[magazineType]);

    printf("final weight %f \n", gun.gunWeight);
    
    return gun;
}
// TODO : gun generation here