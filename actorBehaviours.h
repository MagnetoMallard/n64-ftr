// This header contains update functions for actors and lights
// A mix of draw functions and update functions.
//
// !! Everything in this file should be static      !!
// !! The functions in this file are all intended   !!
// !! to be invoked via function pointers           !!

#ifndef ACTOR_BEHAVIOURS_H
#define ACTOR_BEHAVIOURS_H

#include "globals.h"
#include "lights.h"

/**
 * @brief Update function for the derg. rotates the derg.
 */static inline void dragon_update(Actor *self, float objTime) {
    self->rot[1] = fm_fmodf(objTime, RAD_360);
}

/**
 * @brief Dynamo's custom part draw function for the eyes
 * !! NOT WORKING YET!!
 */
static inline void dynamo_part_draw(T3DModelIter *it, T3DModelState *state, float objTime) {
    char strCheck[12]  = "EYES";
    if (it->material && it->material->name == strCheck ) {
        int objTimeInt = objTime;
        if (objTimeInt % 10) {
            // sets up tex manuallly
            rdpq_set_lookup_address(1, dynamoEyeTex[1]->data);
        } else {
            t3d_model_draw_material(it->object->material, state);
        }

        rdpq_set_lookup_address(1, dynamoEyeTex[1]->data);

    }
    t3d_model_draw_material(it->object->material, state);
    rspq_block_run(it->object->userBlock);
    it->object->isVisible = false;
}

/** @section Light Behaviours
 * fuel for light->lightUpdateFunction
*/
static uint8_t light_col1[4] = {0xFF, 0x00, 0x00, 0xFF};
static uint8_t light_col2[4] = {0x00, 0xFF, 0x00, 0xFF};
static uint8_t light_col3[4] = {0x00, 0x00, 0xFF, 0xFF};
static uint8_t* colourWheel[3] = {light_col1, light_col2, light_col3};

/**
 * @brief efficient traffic light colour cycle that uses obj time
 * @param light Pointer to a Light strut
 * @param deltaTime Time between frames. Unused.
 * @param objTime An arbitrary counter from boot time we use throughout the engine.
 *
 * Used to cycle the lights
 */
static void light_update_traffic_light(Light *light, float deltaTime, float objTime) {
    int lightIndex = fm_fmodf(objTime, 3);
    memcpy(light->colour, colourWheel[lightIndex], sizeof(uint8_t[4]));
}

/**
 * @brief XM Light Update. pulses every 4 rows on currently playing XM. Uses xmPlayer from globals
 * @param light Pointer to a Light strut
 * @param deltaTime Time between frames. Unused.
 * @param objTime An arbitrary counter from boot time we use throughout the engine.
 */
static void light_update_traffic_light_xm(Light *light, float deltaTime, float objTime) {
    static int xmLightIndex = 0;
    static int _lastRow = 0;

    int patIdx, row;
    float secs;
    xm64player_tell(&xm, &patIdx, &row, &secs);

    if (row % 0x04 == 0 && row != _lastRow) {
        memcpy(light->colour, colourWheel[xmLightIndex++], sizeof(uint8_t[4]));
        if (xmLightIndex == 3) xmLightIndex = 0;
        _lastRow = row;
    }
}

#endif // ACTOR_BEHAVIOURS_H