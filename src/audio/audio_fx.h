//
// Created by mallard on 20/04/2025.
//

#ifndef AUDIO_FX_H
#define AUDIO_FX_H

// these functions all apply a single effect to a single set of samples

/**
 * @brief Resonant lowpass filter using floating point maths with a wonky cutoff param.
 *
 * This filter is not all that strong per octave as the resonance characteristic
 * Always makes a big lift
 *
 * It makes for a good effect, but it wont "dive" atm.
 *
 * Cribbed from
 * https://beammyselfintothefuture.wordpress.com/2015/02/16/simple-c-code-for-resonant-lpf-hpf-filters-and-high-low-shelving-eqs/
 *
 * @param buf pointer to first element of an array of 16 bit samples.
 * @param bufferLength length of that array. Needs to be at least 2 for this to work.
 * @param cutoff technically should be a value between 0 and 1.0, but right now this behaves a bit strangely.
 *
 *  Values above 2.0 or so will cause crashes
 * @param resonance strictly between 0.0 and 1.0
 *
 */
void audio_fx_lopass_resonant(short* buf, int bufferLength, float cutoff, float resonance);

/**
 * @brief Speedy efficient lowpass filter using fixed point maths.
 *
 * Sweeps surprisingly well considering its narrow input range.
 *
 * @param buf pointer to first element of an array of 16 bit samples.
 * @param bufferLength length of that array. Needs to be at least 2 for this to work.
 * @param cutoff value between 0.0 and 1.0
 */
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


// Timers and such for Preset system
static float filterTimer = 0;

/**
 *
 * @brief a simple preset system we're going to use for now to apply fx to any buffer.
 *
 * We're using this for now to compose the individual effects into easy to use presets
 * having them as an enumerated type means that the desired preset can be easily
 * paramterised and stored.
 *
 * There's probably a more flexible way we can do this with function pointers
 * and such like, or making effect primative structs
 *
 * But this is simlpe and works fine. We may be fancier later.
 *
 * @param buf pointer to first element of an array of 16 bit samples.
 * @param bufferLength length of that array. Needs to be at least 2 for this to work.
 * @param audioFxPreset choose from a list of presets to apply!
 */
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
