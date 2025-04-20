//
// Created by mallard on 20/04/2025.
//

#ifndef AUDIO_FX_H
#define AUDIO_FX_H

// these functions all apply a single effect to a single set of samples
void audio_fx_lopass_resonant(short* buf, int bufferLength, float cutoff, float resonance);

void audio_fx_lopass_fp(short* buf, int bufferLength, float cutoff);

// Use these different cases to compose effects together
// and set the masterFx param to apply effects to the master mixer
// you can view how the effects are composed in mixer_try_play_custom
// in main.c
// Individual audio_fx should do one operation
enum AudioFxPreset {
    NONE,
    RESONANT_LP_SWEEP,
    FIXEDPONT_LP_SWEEP,
    RESONANT_LP_DIVE,
    FIXEDPONT_LP_DIVE,
};

static float filterTimer = 0;

static void audio_fx_preset_apply(short* buf, int bufferLength, enum AudioFxPreset audioFxPreset) {
    float sinSweep = (fm_cosf(filterTimer += 0.05f) + 1.0f) * 0.4f;

    switch (audioFxPreset) {
        case RESONANT_LP_SWEEP:
            audio_fx_lopass_resonant(buf, bufferLength << 1, sinSweep, 0.2f);
            break;
        case FIXEDPONT_LP_SWEEP:
            audio_fx_lopass_fp(buf, bufferLength << 1, sinSweep);
            break;
        case RESONANT_LP_DIVE:
            audio_fx_lopass_resonant(buf, bufferLength << 1, 0.001f, 0.5f);
            break;
        case FIXEDPONT_LP_DIVE:
            audio_fx_lopass_fp(buf, bufferLength << 1, 0.2f);
            break;
        case NONE:
        default:
    }
}
#endif //AUDIO_FX_H
