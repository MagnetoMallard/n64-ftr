//
// Created by mallard on 21/04/2025.
//

#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H
#include "../globals.h"

#define SOUND_CHANNELS 32
#define SONG_COUNT 6

enum MusicState {
    HOLDLOOP,
    KRITTAGIRL,
    FIELDDAY,
    SONGOFSTORMS
};

#include "audio_fx.h"

/**
 * @brief Initializes libdragon audio subsystem and XM playback
 *
 * MUST BE CALLED BEFORE ANY OTHER AUDIO COMMANDS
 */
void audio_playback_init();

/**
 * @brief Wraps mixer_try_play and applies selected master audio effect
 *
 * Call twice a frame
 */
void audio_playback_try_play();

/**
 * @brief Handle input for music playback
 *
 * Ties up: L, R Buttons.
 */
void audio_playback_take_input();

/**
 * @brief Sets the effect to be applied to the master buss of the audio
 *
 * These effects run on the CPU
 * @param fx
 */
void audio_set_master_fx(enum AudioFxPreset fx);

extern xm64player_t xm;
extern char* songs[SONG_COUNT];
extern enum GameSate musicState;
extern uint16_t currentPeak;

#endif //AUDIO_PLAYBACK_H
