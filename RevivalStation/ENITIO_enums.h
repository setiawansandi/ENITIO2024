/*  enums_ENITIO.h

    This header file contains enumeration of all IR codes used in ENITIO, and provide
    functions to simplify IR usage in Navitas and games.
*/

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
    revive = 4
} action_id ;

typedef enum {
    idle = 0,
    button = 1,
    right = 2,
    up = 3,
    left = 4,
    down = 5,
} joystick_pos ; 

#endif
