//
// Created by mallard on 20/04/2025.
//

// Cribbed From
// https://beammyselfintothefuture.wordpress.com/2015/02/16/simple-c-code-for-resonant-lpf-hpf-filters-and-high-low-shelving-eqs/
void audio_fx_lopass_resonant(short *buf, int bufferLength, float cutoff, float resonance) {
    static float bufL = 0;
    static float bufR = 0;
    static float momentumL = 0;
    static float momentumR = 0;


    for (int i = 0; i < bufferLength; i++) {
        if (i % 2) {
            float distanceToGoL = (buf[i]>>1) - bufL;
            momentumL += distanceToGoL * cutoff; // Lower / higher number here will lower / raise the cutoff frequency
            bufL += momentumL + distanceToGoL * resonance; // Higher number here (max 1) lessens resonance
            buf[i] = (short)(bufL)<<1;
        } else {
            float distanceToGoR = (buf[i]>>1)  - bufR;
            momentumR += distanceToGoR * cutoff; // Lower / higher number here will lower / raise the cutoff frequency
            bufR +=  momentumR + distanceToGoR * resonance;// Higher number here (max 1) lessens resonance
            buf[i] = (short)(bufR)<<1;
        }
    }
}


#define  FP_FAC 3
// Cribbed from
// https://kiritchatterjee.wordpress.com/2014/11/10/a-simple-digital-low-pass-filter-in-c/
 void audio_fx_lopass_fp(short *buf, int bufferLength, float cutoff) {
    static long long smoothDataFPL = 0;
    static long long smoothDataFPR = 0;
    static long long bufFPL = 0;
    static long long bufFPR = 0;
    short cutoffFP = (short)(cutoff*10);

    for (int i = 0; i <=bufferLength ; i++) {
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

