//
// Created by mallard on 20/03/2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <libdragon.h>
#include <t3d/t3d.h>

#define RAD_360 6.28318530718f

enum GameState {
    MAIN_MENU,
    PAUSED,
    STAGE,
    CREDITS
};

enum GameFonts {
    FONT_FTR
};

extern T3DVec3 dergVector;
extern T3DVec3 dynamoVector;
extern enum GameState gameState;
extern joypad_inputs_t inputs;
extern joypad_buttons_t btnsUp;
extern joypad_buttons_t btnsPressed;
extern rspq_syncpoint_t syncPoint;
extern rdpq_font_t *ftrFont;
extern rdpq_font_t *V5PRC___;
extern rdpq_font_t *ftrFontSkinny;
extern rdpq_font_t *ftrFontBig;

#endif //GLOBALS_H
