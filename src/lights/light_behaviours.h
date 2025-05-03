// This header contains update functions for lights

// !! Everything in this file should be static      !!
// !! The functions in this file are all intended   !!
// !! to be invoked via function pointers           !!

#ifndef LIGHTBEHAVIOURS_H

#include "light.h"
#include "../audio/audio_playback.h"
#include <libdragon.h>

typedef struct _lightBehaviour {
    LightUpdateFunction updateFunction;
    char name[32];
} LightBehaviour;

/** @section Light Behaviours
 * fuel for light->lightUpdateFunction
*/
static uint8_t redClr[4] = {0xFF, 0x00, 0x00, 0xFF};
static uint8_t greenClr[4] = {0x00, 0xFF, 0x00, 0xFF};
static uint8_t blueClr[4] = {0x00, 0x00, 0xFF, 0xFF};
static uint8_t purpleClr[4] = {0xFF, 0x00, 0xFF, 0xFF};
static uint8_t* colourWheel[4] = {redClr, greenClr, blueClr, purpleClr};

/**
 * @brief efficient traffic light colour cycle that uses obj time
 * @param light Pointer to a Light strut
 * @param deltaTime Time between frames in seconds. Unused.
 * @param objTime Seconds since stage begin. Used to cycle the lights.
 */
static void light_update_traffic_light(Light *light, float deltaTime, float objTime) {
    int lightIndex = fm_fmodf(objTime, 3);
    memcpy(light->colour, colourWheel[lightIndex], sizeof(uint8_t[4]));
}

/**
 * @brief XM Light Update. pulses every 4 rows on currently playing XM. Uses xmPlayer from globals
 * @param light Pointer to a Light strut
 * @param deltaTime Time between frames in seconds. Unused.
 * @param objTime Seconds since stage begin. Unused
 */
static void light_update_traffic_light_xm(Light *light, float deltaTime, float objTime) {
    static int xmLightIndex = 0;
    static int _lastRow = 0;


    int patIdx, row;
    float secs;
    xm64player_tell(&xm, &patIdx, &row, &secs);

    if ((row + 1) % 0x04 == 0 && row != _lastRow) {
        memcpy(light->colour, colourWheel[xmLightIndex++], sizeof(uint8_t[4]));
        if (xmLightIndex == 4) xmLightIndex = 0;
        _lastRow = row;
    } else {
        light->colour[0] = abs(light->colour[0] - 8);
        light->colour[1] = abs(light->colour[1] - 8);
        light->colour[2] = abs(light->colour[2] - 8);
    }
}
static long strobe[2] = {0x00000000, 0xFFFFFFFF};
bool strobeFlick = true;

static void light_update_xm_tekno_strobe(Light *light, float deltaTime, float objTime) {
    static int _lastRow = 0;
    int row;
    xm64player_tell(&xm, nullptr, &row, nullptr);

    if ((row + 1) % 0x02 == 0 && row != _lastRow) {
        memcpy(light->colour, &strobe[strobeFlick] , sizeof(uint8_t[3]));
        strobeFlick = !strobeFlick;
        _lastRow = row;
    }
}

static void light_update_vol_follow(Light *light, float deltaTime, float objTime) {
    uint8_t val = ((currentPeak & 0xff00)  >> 8) * 0.5;
    light->colour[0] =  val;
    light->colour[1] = val;
    light->colour[2] =  val;
    light->colour[3] = val;
}



#define LIGHTBEHAVIOURS_H

#endif //LIGHTBEHAVIOURS_H
