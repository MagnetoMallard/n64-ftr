//
// Created by mallard on 20/04/2025.
//

#define  FP_FAC 3
#include <mixer.h>

#include "audio_playback.h"
#include "audio_fx.h"
#include "../../libs/libxm/xm_internal.h"

// Cribbed From
// https://beammyselfintothefuture.wordpress.com/2015/02/16/simple-c-code-for-resonant-lpf-hpf-filters-and-high-low-shelving-eqs/
void audio_fx_lopass_resonant(short *buf, int bufferLength, float cutoff, float resonance) {
    static float bufL = 0;
    static float bufR = 0;
    static float momentumL = 0;
    static float momentumR = 0;


    for (int i = 0; i < bufferLength; i++) {
        if (i % 2) {
            float distanceToGoL = (buf[i]>>FP_FAC) - bufL;
            momentumL += distanceToGoL * cutoff; // Lower / higher number here will lower / raise the cutoff frequency
            bufL += momentumL + distanceToGoL * resonance; // Higher number here (max 1) lessens resonance
            buf[i] = (short)(bufL)<<FP_FAC;
        } else {
            float distanceToGoR = (buf[i]>>FP_FAC)  - bufR;
            momentumR += distanceToGoR * cutoff; // Lower / higher number here will lower / raise the cutoff frequency
            bufR +=  momentumR + distanceToGoR * resonance;// Higher number here (max 1) lessens resonance
            buf[i] = (short)(bufR)<<FP_FAC;
        }
    }
}


// Cribbed from
// https://kiritchatterjee.wordpress.com/2014/11/10/a-simple-digital-low-pass-filter-in-c/
 void audio_fx_lopass_fp(short *buf, int bufferLength, float cutoff) {
    static long long smoothDataFPL = 0;
    static long long smoothDataFPR = 0;
    static long long bufFPL = 0;
    static long long bufFPR = 0;
    short cutoffFP = (short)(cutoff*10);

    for (int i = 0; i < bufferLength ; i++) {
        if (i%2) {
            bufFPL = buf[i] ;
            smoothDataFPL = (smoothDataFPL << cutoffFP)-smoothDataFPL;
            smoothDataFPL += bufFPL;
            smoothDataFPL >>= cutoffFP;
            buf[i] = smoothDataFPL << cutoffFP/FP_FAC;  // Roughly normalise based on cutoffFP fac
        } else {
            bufFPR = buf[i] ; // Shift to fixed point
            smoothDataFPR = (smoothDataFPR << cutoffFP)-smoothDataFPR;
            smoothDataFPR += bufFPR;
            smoothDataFPR >>= cutoffFP;
            buf[i] = smoothDataFPR << cutoffFP/FP_FAC; // Roughly normalise based on cutoff fac
        }
    }
}

void audio_fx_phase(short *buf, int bufferLength, int phaseDelay) {
    for (int i = 0; i < bufferLength; i++) {
        if (i > phaseDelay) {
            buf[i] = (buf[i]>>1) + (buf[(i - phaseDelay)]>>1);
        }
    }
}

void audio_fx_preset_apply(short* buf, int bufferLength, enum AudioFxPreset audioFxPreset) {
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
        case PHASE:
            audio_fx_lopass_resonant(buf, bufferLength << 1, sinSweep, 0.2f);
            audio_fx_phase(buf, bufferLength << 1, (12 * fabs(sinSweep)) + 4);
        break;
        case SLOWDOWN:
            (xm.ctx)->tempo = (xm.ctx)->tempo / 2;
        case NONE:
            default:
    }
}