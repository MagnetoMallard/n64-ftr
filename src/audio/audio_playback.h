//
// Created by mallard on 21/04/2025.
//

#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H
#include "../globals.h"

#define SOUND_CHANNELS 16
#define SONG_COUNT 3

enum MusicState {
    HOLDLOOP,
    KRITTAGIRL,
    FIELDDAY
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

extern xm64player_t xm;
extern char* songs[SONG_COUNT];
extern enum AudioFxPreset audioMixerMasterFx ;
extern enum GameSate musicState;
extern uint8_t songSelection;

#endif //AUDIO_PLAYBACK_H
