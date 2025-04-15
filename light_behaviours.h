// This header contains update functions for lights

// !! Everything in this file should be static      !!
// !! The functions in this file are all intended   !!
// !! to be invoked via function pointers           !!

#ifndef LIGHTBEHAVIOURS_H

#include "lights.h"
#include "globals.h"

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
        light->colour[3] -= 32;
    }


    // int16_t pointSample[2];
    // mixer_poll(pointSample, 2);
    //
    // light->colour[2] = abs(pointSample[0]) & 0xFF ; // get an 8 bit val

}

#define LIGHTBEHAVIOURS_H

#endif //LIGHTBEHAVIOURS_H
