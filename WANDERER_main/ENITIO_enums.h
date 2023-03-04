#ifndef ENITIO_enums_h
#define ENITIO_enums_h

typedef enum {
    INVICTA = 0,
    DYNARI = 1,
    EPHILIA = 2,
    AKRONA = 3,
    SOLARIS = 4,
} CLAN_id ;

typedef enum {
    do_nothing = 0,
    attack = 1,
    collect = 2,
    heal = 3,
    revive = 4,
    poisonID = 5,
} action_id ;

typedef enum {
    idle = 0,
    button = 1,
    right = 2,
    up = 3,
    left = 4,
    down = 5,
} joystick_pos ; 

typedef enum {
    nothing = 0,
    bonus6HP = 1,
    bonus1MaxEn = 2,
    bonus1MULTIPLIER = 3,
    fiveminx2EnRegen = 4,
    bomb = 5,
    poison = 6,
} power_up; 

#endif

