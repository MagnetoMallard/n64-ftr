//
// Created by mallard on 20/04/2025.
//

#ifndef AUDIO_FX_H
#define AUDIO_FX_H

// these functions all apply a single effect to a single set of samples
void audio_fx_lopass_resonant(short *buf, int bufferLength, float cutoff, float resonance);
void audio_fx_lopass_fp(short *buf, int bufferLength, float cutoff);

// Use these different cases to compose effects together
// and set the masterFx param to apply effects to the master mixer
// you can view how the effects are composed in mixer_try_play_custom
// in main.c
// Individual audio_fx should do one operation
enum MasterFxType {
    NONE,
    RESONANT_LP_SWEEP,
    FIXEDPONT_LP_SWEEP,
    RESONANT_LP_DIVE,
    FIXEDPONT_LP_DIVE,
};

extern enum MasterFxType audioMixerMasterFx;
#endif //AUDIO_FX_H
