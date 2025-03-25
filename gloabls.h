//
// Created by mallard on 20/03/2025.
//

#ifndef GLOABLS_H
#define GLOABLS_H

#include "actor.h"
#include <libdragon.h>

#define SOUND_CHANNELS 16

enum GameSate {
    MAIN_MENU,
    PAUSED,
    STAGE,
    CREDITS
};

enum MusicState {
    HOLDLOOP,
    KRITTAGIRL
};


extern enum GameSate gameState;
extern xm64player_t xm;
extern struct controller_data inputs;

#endif //GLOABLS_H
