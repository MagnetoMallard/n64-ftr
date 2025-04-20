//
// Created by mallard on 20/03/2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <libdragon.h>

#define SOUND_CHANNELS 16
#define SONG_COUNT 3
#define RAD_360 6.28318530718f

enum GameSate {
    MAIN_MENU,
    PAUSED,
    STAGE,
    CREDITS
};

enum MusicState {
    HOLDLOOP,
    KRITTAGIRL,
    FIELDDAY
};

enum GameFonts {
    FONT_FTR
};

extern T3DVec3 dergVector;
extern T3DVec3 dynamoVector;
extern enum GameSate gameState;
extern xm64player_t xm;
extern joypad_inputs_t inputs;
extern joypad_buttons_t btnsUp;
extern joypad_buttons_t btnsPressed;
extern rspq_syncpoint_t syncPoint;
extern char* songs[SONG_COUNT];
extern uint8_t songSelection;
extern rdpq_font_t* ftrFont;
#endif //GLOBALS_H
