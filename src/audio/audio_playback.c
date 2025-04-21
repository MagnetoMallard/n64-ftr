//
// Created by mallard on 21/04/2025.
//
#include <libdragon.h>
#include "audio_playback.h"

static void music_load(int songIndex);
static void music_init();
static bool mute[SOUND_CHANNELS] = {0};

// externs
xm64player_t xm;
char* songs[SONG_COUNT];
enum AudioFxPreset audioMixerMasterFx = NONE;
enum GameSate musicState = HOLDLOOP;
uint8_t songSelection = 0;

void audio_playback_try_play() {
    if (audio_can_write()) {
        int bufferLength = audio_get_buffer_length();

        short* buf = audio_write_begin();
        mixer_poll(buf, bufferLength);
        audio_fx_preset_apply(buf, bufferLength, audioMixerMasterFx);

        audio_write_end();
    }
}

void audio_playback_init() {
    audio_init(44100, 8);
    mixer_init(SOUND_CHANNELS);
    music_init();
}

void audio_playback_take_input() {
    // trigger on button release
    if (btnsPressed.l) {
        songSelection--;
        music_load(songSelection % SONG_COUNT);
    }

    // trigger on button release
    if (btnsPressed.r) {
        songSelection++;
        music_load(songSelection % SONG_COUNT);
    }
}


static void music_load(int songIndex) {
    if (xm.playing) {
        xm64player_close(&xm);
    }
    xm64player_open(&xm, songs[songIndex]);
    xm64player_play(&xm, 0);
    musicState = songIndex;
}

void music_init() {
    debugf("blah");
    // SETUP SONG LIST
    songs[0] = "rom:/smallhold.xm64";
    songs[1] = "rom:/kritta-girl.xm64";
    songs[2] = "rom:/field-day.xm64";

    music_load(0);

    // Unmute all channels
    memset(mute, 0, sizeof(mute));
}

